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
    this->gltf_name     = init_Buffer();
    this->phantom_out   = true;
    this->no_offset     = false;

    this->forUnity      = true;
    this->forUE         = false;

    this->engine        = JBXL_3D_ENGINE_UE;
    this->affineTrans   = NULL;
    this->skeleton.init();

    this->bin_buffer    = init_Buffer();
    this->bin_offset    = 0;

    this->node_num      = 0;
    this->view_num      = 0;
    this->access_num    = 0;

    this->json_data     = NULL;

    this->scenes        = NULL;
    this->scenes_name   = NULL;
    this->scenes_nodes  = NULL;
    this->nodes         = NULL;
    this->meshes        = NULL;
    this->buffers       = NULL;
    this->buffviews     = NULL;
    this->accessors     = NULL;

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
    //tJson* materials = json_append_array_bykey(this->json_data, "\"materials\"");
    //tJson* textures  = json_append_array_bykey(this->json_data, "\"textures\"");
    //tJson* images    = json_append_array_bykey(this->json_data, "\"images\"");
    //tJson* samplers  = json_append_array_bykey(this->json_data, "\"samplers\"");
    //this->scene      = json_append_array_key(this->json_data, "scene");

    json_insert_parse(this->json_data, "{\"scene\":0}");
    this->scenes       = json_append_array_key(this->json_data, "scenes");
    this->scenes_name  = json_insert_parse(this->scenes, "{\"name\":}");
    this->scenes_nodes = json_append_array_key(this->scenes->next, "nodes");
    this->nodes        = json_append_array_key(this->json_data, "nodes");
    this->meshes       = json_append_array_key(this->json_data, "meshes");
    this->buffers      = json_append_array_key(this->json_data, "buffers");
    this->buffviews    = json_append_array_key(this->json_data, "bufferViews");
    this->accessors    = json_append_array_key(this->json_data, "accessors");
    json_insert_parse(this->buffers, "{\"uri\":, \"byteLength\":}");
}


//void  GLTFData::outputFile(const char* fname, const char* out_path, const char* tex_dirn)
void  GLTFData::outputFile(const char* fname, const char* out_path)
{
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

    // output json/binary
    cat_Buffer(&file_name, &gltf_path);
    change_file_extension_Buffer(&gltf_path, ".gltf");
    Buffer bin_path = dup_Buffer(gltf_path);
    change_file_extension_Buffer(&bin_path, ".bin");
    this->output_gltf((char*)gltf_path.buf, (char*)bin_path.buf);

    free_Buffer(&gltf_path);
    free_Buffer(&bin_path);
    free_Buffer(&file_name);
    //free_Buffer(&rel_tex);
    //
    return;
}



////////////////////////////////////////////////////////////////////////////////////////////

void  GLTFData::addObject(MeshObjectData* meshdata, bool collider, SkinJointData* joints)
{
    if (meshdata==NULL) return;
    if (this->node_num==0 && this->gltf_name.buf==NULL) gltf_name = dup_Buffer(meshdata->data_name);

    int total_index  = 0;
    int total_vertex = 0;

    // get working memory
    MeshFacetNode* facet = meshdata->facet;
    while (facet!=NULL) {
        total_index  += facet->num_index;
        total_vertex += facet->num_vertex;
        facet = facet->next;
    }
    long unsigned int temp_len = (long unsigned int)total_index*sizeof(int) + (long unsigned int)total_vertex*sizeof(float)*8LU;
    unsigned char* temp_data = (unsigned char*)malloc(temp_len);
    if (temp_data==NULL) {
        PRINT_MESG("GLTFData::addObject: Error: No more memory.\n");
        return;
    }
    if (this->bin_buffer.buf==NULL) {
        this->bin_buffer = make_Buffer(temp_len);
        if (this->bin_buffer.bufsz<=0) {
            PRINT_MESG("GLTFData::addObject: Error: No more memory.\n");
            ::free(temp_data);
            return;
        }
    }

    //
    char buf[LBUF];
    long unsigned int length = 0;
    long unsigned int offset = 0;

    // scenes
    if (this->node_num==0) json_set_str_val(this->scenes_name, (char*)this->gltf_name.buf);
    json_append_array_int_val(this->scenes_nodes, this->node_num);

    // nodes
    memset(buf, 0, LBUF);
    snprintf(buf, LBUF-1, JBXL_GLTF_MESH, (char*)meshdata->alt_name.buf, this->node_num);
    tJson* mesh = json_insert_parse(this->nodes, buf);
    tJson* matrix = json_append_array_key(mesh, "matrix");

    if (meshdata->affineTrans!=NULL) {
        this->affineTrans = newAffineTrans<double>(*meshdata->affineTrans);
        this->affineTrans->computeMatrix(true);
        for (int j=1; j<=4; j++) {
            for (int i=1; i<=4; i++) {
                float element = (float)(this->affineTrans->matrix.element(i,j));
                json_append_array_real_val(matrix, element);
            }
        }
    }
    this->node_num++;

    // FACETS
    tJson* primitives = json_append_array_key(this->meshes, "primitives");
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

        // meshes->primitives
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_MESH_PRIMITIVE, this->access_num, this->access_num+1, this->access_num+2, this->access_num+3);
        json_insert_parse(primitives, buf);

        //
        // bufferview of indexies
        length = (long unsigned int)facet->num_index*sizeof(int);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ELEMENT_BUFFER, 0, this->bin_offset, length);
        json_insert_parse(this->buffviews, buf);

        // accessor of indexies
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, this->view_num, 0LU, 5125, facet->num_index, "SCALAR");  // 5125: unsigned int
        json_insert_parse(this->accessors, buf);
        this->view_num++;
        this->access_num++;

        // binary of indexies
        memcpy((void*)(temp_data + offset), (void*)facet->data_index, length);
        offset += length;
        this->bin_offset += length;

        //
        // bufferview of vertex/normal/uvmap
        length = (long unsigned int)facet->num_vertex*sizeof(float)*8LU;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_BUFFER, 0, this->bin_offset, length, (int)sizeof(float)*8);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;
        
        // accessors of vertex/normal/uvmap
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, this->view_num,               0LU, 5126, facet->num_vertex, "VEC3");    // 5126: float
        json_insert_parse(this->accessors, buf);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, this->view_num, sizeof(float)*3LU, 5126, facet->num_vertex, "VEC3");    // 5126: float
        json_insert_parse(this->accessors, buf);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, this->view_num, sizeof(float)*6LU, 5126, facet->num_vertex, "VEC2");    // 5126: float
        json_insert_parse(this->accessors, buf);
        this->view_num++;
        this->access_num += 3;

        // binary of vertex/normal/uvmap
        float temp;
        length = sizeof(float);
        for (int i=0; i<facet->num_vertex; i++) {
            temp = (float)facet->vertex_value[i].x;
            memcpy((void*)(temp_data + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->vertex_value[i].y;
            memcpy((void*)(temp_data + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->vertex_value[i].z;
            memcpy((void*)(temp_data + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->normal_value[i].x;
            memcpy((void*)(temp_data + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->normal_value[i].y;
            memcpy((void*)(temp_data + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->normal_value[i].z;
            memcpy((void*)(temp_data + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->texcrd_value[i].u;
            memcpy((void*)(temp_data + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->texcrd_value[i].v;
            memcpy((void*)(temp_data + offset), (void*)&temp, length);
            offset += length;
        }

        facet = facet->next;
    }

    cat_b2Buffer(temp_data, &(this->bin_buffer), temp_len);
    ::free(temp_data);

    //
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


void  GLTFData::output_gltf(char* json_file, char* bin_file)
{

    char* bin_fname = get_file_name(bin_file);

    tJson* uri = search_key_json(this->buffers, "uri", FALSE, 1);
    if (uri!=NULL) json_set_str_val(uri, bin_fname);
    tJson* len = search_key_json(this->buffers, "byteLength", FALSE, 1);
    if (uri!=NULL) json_set_int_val(len, this->bin_buffer.vldsz);

    FILE* fp = fopen(json_file, "w");
    if (fp==NULL) return;
    print_json(fp, this->json_data, JSON_INDENT_FORMAT);
    fclose(fp);

    fp = fopen(bin_file, "wb");
    if (fp==NULL) return;
    fwrite((void*)(this->bin_buffer.buf), this->bin_buffer.vldsz, 1, fp);
    fclose(fp);

    return;
}

