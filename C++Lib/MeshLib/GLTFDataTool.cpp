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


void  GLTFData::init(int n)
{
    initGLTF();

    this->gltf_name   = init_Buffer();
    this->num_gltf    = n;
    this->phantom_out = true;
    this->no_offset   = false;

    this->forUnity    = true;
    this->forUE       = false;

    this->engine      = JBXL_3D_ENGINE_UE;
    this->next        = NULL;
    this->affineTrans = NULL;
    this->skeleton.init();

    this->index_buffer  = init_Buffer();
    this->vertex_buffer = init_Buffer();
    this->normal_buffer = init_Buffer();
    this->texcrd_buffer = init_Buffer();


    this->json_data     = NULL;
    this->buffers       = NULL;
    this->buffviews     = NULL;
    this->accessors     = NULL;
}


void  GLTFData::free(void)
{
    free_Buffer(&(this->gltf_name));
    del_all_json(&(this->json_data));
    this->skeleton.free();

    this->delAffineTrans();
    this->affineTrans = NULL;

    this->delete_next();
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


void  GLTFData::delete_next(void)
{
    if (this->next==NULL) return;

    GLTFData* _next = this->next;
    while (_next!=NULL) {
        GLTFData* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    this->next = NULL;
}


void  GLTFData::initGLTF(void)
{
    PRINT_MESG("GLTFData::initGLTF: start\n");

    char asset[] = "{asset:{\"copyright\": , \"generator\": , \"version\": }";
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

    this->buffers   = json_append_array_bykey(this->json_data, "\"buffers\"");
    this->buffviews = json_append_array_bykey(this->json_data, "\"bufferViews\"");
    this->accessors = json_append_array_bykey(this->json_data, "\"accessors\"");
}


void  GLTFData::outputFile(const char* fname, const char* out_path, const char* tex_dirn)
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
    Buffer rel_tex;     //  Texture 相対パス
    if (tex_dirn==NULL) rel_tex = make_Buffer_bystr("");
    else                rel_tex = make_Buffer_bystr(tex_dirn);

    cat_Buffer(&file_name, &gltf_path);
    change_file_extension_Buffer(&gltf_path, ".gltf");

    fp = fopen((char*)gltf_path.buf, "wb");
    if (fp!=NULL) {
        this->output_gltf(fp);
        fclose(fp);
    }
    //
    free_Buffer(&gltf_path);
    free_Buffer(&rel_tex);
    //
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////

void  GLTFData::addObject(MeshObjectData* meshdata, bool collider, SkinJointData* joints)
{
    PRINT_MESG("GLTFData::addObject: start\n");
    char  buf[LBUF];

    int data_len    = 0;
    int data_offset = 0;

    Buffer vertex_base64 = init_Buffer();
    MeshFacetNode* facet = meshdata->facet;
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
        data_len = facet->num_index*(int)sizeof(int);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ELEMENT_ARRAY_BUFFER, 0, facet->num_index*(int)sizeof(int), data_offset);
        json_append_nodes_bystr(this->buffviews, buf);
        data_offset += data_len;

        //
        data_len = facet->num_vertex*(int)(sizeof(float)*8);    // 8 = 3 + 3 + 2 (vertex + normal + uvmap)
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ARRAY_BUFFER, 0, data_len, data_offset, (int)sizeof(float)*8);
        json_append_nodes_bystr(this->buffviews, buf);
        data_offset += data_len;
        











        //facet->num_index;
        //facet->num_vertex;

        int*  data_index = (int*)malloc(sizeof(int)*facet->num_index);
        if (data_index!=NULL) {
            for (int i=0; i<facet->num_index; i++) {
                data_index[i] = facet->data_index[i];
            }
        }
        else {
            return;
        }

        int dlen = (int)sizeof(float)*facet->num_vertex;
        float* vertex = (float*)malloc(dlen*3);
        float* normal = (float*)malloc(dlen*3);
        float* texcrd = (float*)malloc(dlen*2);
        if (vertex!=NULL && normal!=NULL && texcrd!=NULL) {
            for (int i=0; i<facet->num_vertex; i++) {
                vertex[i*3]   = (float)facet->vertex_value[i].x;
                vertex[i*3+1] = (float)facet->vertex_value[i].y;
                vertex[i*3+2] = (float)facet->vertex_value[i].z;
                //normal[i] = (float)facet->normal_value[i];
                //texcrd[i] = (float)facet->texcrd_value[i];
            }
        }
        else {
            freeNull(vertex);
            freeNull(normal);
            freeNull(texcrd);
            freeNull(data_index);
            return;
        }

        /*
        Buffer temp = encode_base64_Buffer_bin((unsigned char*)vertex, dlen*3, TRUE);
        cat_Buffer(&temp, &vertex_base64);
        free_Buffer(&temp);
        */
        
        //int*            index = facet->data_index;
        //Vector<double>* vectr = facet->vertex_value;
        //Vector<double>* norml = facet->normal_value;
        //UVMap<double>*  uvmap = facet->texcrd_value;

        freeNull(vertex);
        freeNull(normal);
        freeNull(texcrd);
        freeNull(data_index);
        facet = facet->next;
    }

print_message("========>> %s\n", (char*)vertex_base64.buf);


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

    GLTFData* gltf = this->next;  // Top はアンカー
    if (gltf!=NULL && this->no_offset) center = gltf->affineTrans->shift;

    return center;
}


void  GLTFData::output_gltf(FILE* fp)
{
    print_tTree(stderr, this->json_data);
    print_json(stderr, this->json_data, JSON_INDENT_FORMAT);

    if (fp==NULL) return;

    return;
}


