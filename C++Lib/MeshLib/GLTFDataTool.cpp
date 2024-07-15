/**
@brief    GLTF用 ツール
@file     GLTFDataTool.cpp
@author   Fumi.Iseki (C)
*/

#include  "GLTFDataTool.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////
//

GLTFData::~GLTFData(void)
{
    DEBUG_MODE PRINT_MESG("GLTFData::DESTRUCTOR:\n");
    this->free();
}


void  GLTFData::init(void)
{
    this->gltf_name   = init_Buffer();
    this->phantom_out = true;
    this->no_offset   = false;

    this->forUnity    = true;
    this->forUE       = false;

    this->engine      = JBXL_3D_ENGINE_UE;
    this->affineTrans = NULL;
    this->skeleton.init();

    this->bin_buffer  = init_Buffer();

    this->json_data   = NULL;
    this->buffers     = NULL;
    this->buffviews   = NULL;
    this->accessors   = NULL;

    initGLTF();
}


void  GLTFData::free(void)
{
    free_Buffer(&(this->gltf_name));
    free_Buffer(&(this->bin_buffer));

    del_all_json(&(this->json_data));
    this->skeleton.free();

    this->delAffineTrans();
    this->affineTrans = NULL;
}


void  GLTFData::setEngine(int e)
{
    this->setUnity(false);
    this->setUE(false);
    //
    this->engine = e;
    if (e==JBXL_3D_ENGINE_UNITY)   this->setUnity(true);
    else if (e==JBXL_3D_ENGINE_UE) this->setUE(true);

    return;
}


void  GLTFData::initGLTF(void)
{
    PRINT_MESG("GLTFData::initGLTF: start\n");

    char asset[] = "{\"asset\": {\"copyright\", \"generator\", \"version\"}}";
    this->json_data = json_parse(asset, 99);

    tJson* temp;
    temp = search_double_key_json(this->json_data, "asset", "copyright", FALSE);
    if (temp!=NULL) json_set_str_val(temp, JBXL_GLTF_COPYRIGHT);
    temp = search_double_key_json(this->json_data, "asset", "generator", FALSE);
    if (temp!=NULL) json_set_str_val(temp, JBXL_GLTF_GENERATOR);
    temp = search_double_key_json(this->json_data, "asset", "version", FALSE);
    if (temp!=NULL) json_set_str_val(temp, JBXL_GLTF_VERSION);

    //tJson* exused    = json_append_array_bykey(this->json_data, "\"extensionsUsed\"");
    //tJson* scene     = json_append_array_bykey(this->json_data, "\"scene\"");
    //tJson* scenes    = json_append_array_bykey(this->json_data, "\"scenes\"");
    //tJson* nodes     = json_append_array_bykey(this->json_data, "\"nodes\"");
    //tJson* materials = json_append_array_bykey(this->json_data, "\"materials\"");
    //tJson* meshes    = json_append_array_bykey(this->json_data, "\"meshes\"");
    //tJson* textures  = json_append_array_bykey(this->json_data, "\"textures\"");
    //tJson* images    = json_append_array_bykey(this->json_data, "\"images\"");
    //tJson* samplers  = json_append_array_bykey(this->json_data, "\"samplers\"");

    this->buffers   = json_append_array_key(this->json_data, "buffers");
    this->buffviews = json_append_array_key(this->json_data, "bufferViews");
    this->accessors = json_append_array_key(this->json_data, "accessors");
}


//void  GLTFData::outputFile(const char* fname, const char* out_path, const char* tex_dirn)
void  GLTFData::outputFile(const char* fname, const char* out_path)
{
    PRINT_MESG("GLTFData::outputFile: start\n");

    FILE* fp = NULL;
    char* packname = pack_head_tail_char(get_file_name(fname), ' ');
    Buffer file_name = make_Buffer_bystr(packname);
    ::free(packname);

    canonical_filename_Buffer(&file_name);
    if (file_name.buf[0]=='.') file_name.buf[0] = '_';
    //
    Buffer gltf_path;    // 出力パス
    if (out_path==NULL) gltf_path = make_Buffer_bystr("./");
    else                gltf_path = make_Buffer_bystr(out_path);
    //
    //Buffer rel_tex;     //  Texture 相対パス
    //if (tex_dirn==NULL) rel_tex = make_Buffer_bystr("");
    //else                rel_tex = make_Buffer_bystr(tex_dirn);

    // output json
    cat_Buffer(&file_name, &gltf_path);
    change_file_extension_Buffer(&gltf_path, ".gltf");
    fp = fopen((char*)gltf_path.buf, "w");
    if (fp!=NULL) {
        this->output_gltf(fp);
        fclose(fp);
    }

    // output binary
    change_file_extension_Buffer(&gltf_path, ".bin");
    fp = fopen((char*)gltf_path.buf, "wb");
    if (fp!=NULL) {
        this->output_bin(fp);
        fclose(fp);
    }
    //
    free_Buffer(&file_name);
    free_Buffer(&gltf_path);
    //free_Buffer(&rel_tex);
    //
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////

void  GLTFData::addObject(MeshObjectData* meshdata, bool collider, SkinJointData* joints)
{
    PRINT_MESG("GLTFData::addObject: start.\n");
    int total_index  = 0;
    int total_vertex = 0;

    MeshFacetNode* facet = meshdata->facet;
    while (facet!=NULL) {
        total_index  += facet->num_index;
        total_vertex += facet->num_vertex;
        facet = facet->next;
    }
    long unsigned int bin_len = (long unsigned int)total_index*sizeof(int) + (long unsigned int)total_vertex*sizeof(float)*8LU;
    unsigned char* bin_dat = (unsigned char*)malloc(bin_len);
    if (bin_dat==NULL) {
        PRINT_MESG("GLTFData::addObject: Error: No more memory.\n");
        return;
    }
    if (this->bin_buffer.buf==NULL) {
        this->bin_buffer = make_Buffer(bin_len);
        if (this->bin_buffer.bufsz<=0) {
            PRINT_MESG("GLTFData::addObject: Error: No more memory.\n");
            ::free(bin_dat);
            return;
        }
    }

    //
    char buf[LBUF];
    int  view_num = 0;
    long unsigned int data_len    = 0;
    long unsigned int data_offset = 0;

    facet = meshdata->facet;
    while (facet!=NULL) {
        if (facet->num_vertex != facet->num_texcrd) {
            PRINT_MESG("GLTFData::addObject: Error: missmatch vertex and uvmap number! (%d != %d)\n", facet->num_vertex, facet->num_texcrd);
            facet = facet->next;
            continue;
        }
        // UV Map and PLANAR Texture
        if (facet->material_param.mapping == MATERIAL_MAPPING_PLANAR) {
            Vector<double> scale(1.0, 1.0, 1.0);
            if (meshdata->affineTrans!=NULL) scale = meshdata->affineTrans->scale;
            facet->generatePlanarUVMap(scale, facet->texcrd_value);
        }
        facet->execAffineTransUVMap(facet->texcrd_value, facet->num_vertex);

        //
        // bufferview of indexies
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ELEMENT_ARRAY_BUFFER, 0, data_offset, data_len);
        json_insert_parse(this->buffviews, buf);

        // accessor of indexies
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, view_num, 0LU, 5123, facet->num_index, "SCALSR");  // 5123: unsigned short
        json_insert_parse(this->accessors, buf);
        view_num++;

        // binary of indexies
        data_len = (long unsigned int)facet->num_index*sizeof(int);
        memcpy((void*)(bin_dat + data_offset), (void*)facet->data_index, data_len);
        data_offset += data_len;

        //
        // bufferview of vertex/normal/uvmap
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ARRAY_BUFFER, 0, data_offset, data_len, (int)sizeof(float)*8);
        json_insert_parse(this->buffviews, buf);
        
        // accessors of vertex/normal/uvmap
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, view_num,               0LU, 5126, facet->num_vertex, "VEC3");    // 5126: float
        json_insert_parse(this->accessors, buf);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, view_num, sizeof(float)*3LU, 5126, facet->num_vertex, "VEC3");    // 5126: float
        json_insert_parse(this->accessors, buf);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, view_num, sizeof(float)*6LU, 5126, facet->num_vertex, "VEC2");    // 5126: float
        json_insert_parse(this->accessors, buf);
        view_num++;

        // binary of vertex/normal/uvmap
        float temp;
        data_len = sizeof(float);
        for (int i=0; i<facet->num_vertex; i++) {
            temp = (float)facet->vertex_value[i].x;
            memcpy((void*)(bin_dat + data_offset), (void*)&temp, data_len);
            data_offset += data_len;
            temp = (float)facet->vertex_value[i].y;
            memcpy((void*)(bin_dat + data_offset), (void*)&temp, data_len);
            data_offset += data_len;
            temp = (float)facet->vertex_value[i].z;
            memcpy((void*)(bin_dat + data_offset), (void*)&temp, data_len);
            data_offset += data_len;
            temp = (float)facet->normal_value[i].x;
            memcpy((void*)(bin_dat + data_offset), (void*)&temp, data_len);
            data_offset += data_len;
            temp = (float)facet->normal_value[i].y;
            memcpy((void*)(bin_dat + data_offset), (void*)&temp, data_len);
            data_offset += data_len;
            temp = (float)facet->normal_value[i].z;
            memcpy((void*)(bin_dat + data_offset), (void*)&temp, data_len);
            data_offset += data_len;
            temp = (float)facet->texcrd_value[i].u;
            memcpy((void*)(bin_dat + data_offset), (void*)&temp, data_len);
            data_offset += data_len;
            temp = (float)facet->texcrd_value[i].v;
            memcpy((void*)(bin_dat + data_offset), (void*)&temp, data_len);
            data_offset += data_len;
        }

        facet = facet->next;
    }

//print_json(stderr, this->json_data, 2);

    cat_b2Buffer(bin_dat, &(this->bin_buffer), bin_len);
print_message("===> %d  %d %d\n", data_offset, bin_len, bin_buffer.vldsz);
    ::free(bin_dat);


    if (meshdata==NULL) return;

    if (collider) {
    }

    if (joints!=NULL) {
    }

    return;
}


/**
Vector<double>  GLTFData::execAffineTrans(void)

GLTFデータの Affine変換を行う．
no_offset が trueの場合，データの中心を原点に戻し，実際の位置をオフセットで返す．
*/
Vector<double>  GLTFData::execAffineTrans(void)
{
    Vector<double> center(0.0, 0.0, 0.0);

    if (this->no_offset) center = affineTrans->shift;

    return center;
}


void  GLTFData::output_gltf(FILE* fp)
{
    print_tTree(stderr, this->json_data);
    print_json(stderr, this->json_data, JSON_INDENT_FORMAT);

    if (fp==NULL) return;

    return;
}


