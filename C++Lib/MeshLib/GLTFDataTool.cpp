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
    this->alt_name      = init_Buffer();
    this->phantom_out   = true;
    this->no_offset     = false;

    this->forUnity      = true;
    this->forUE         = false;
    this->engine        = JBXL_3D_ENGINE_UE;

    this->shellNode       = NULL;

    this->affineTrans   = NULL;
    this->skeleton.init();

    this->bin_buffer    = init_Buffer();
    this->bin_offset    = 0;

    this->node_num      = 0;
    this->view_num      = 0;
    this->mesh_num      = 0;
    this->material_num  = 0;
    this->texture_num   = 0;

    this->json_data     = NULL;

    this->scenes        = NULL;
    this->scenes_name   = NULL;
    this->scenes_nodes  = NULL;
    this->nodes         = NULL;
    this->meshes        = NULL;
    this->buffers       = NULL;
    this->buffviews     = NULL;
    this->accessors     = NULL;
    this->materials     = NULL;
    this->textures      = NULL;
    this->images        = NULL;

    initGLTF();
}


void  GLTFData::free(void)
{
    free_Buffer(&(this->gltf_name));
    free_Buffer(&(this->alt_name));
    free_Buffer(&(this->bin_buffer));

    del_all_json(&(this->json_data));
    this->skeleton.free();

    if (this->shellNode!=NULL) {
        this->shellNode->delete_next();
        this->shellNode = NULL;
    }

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

    this->materials    = json_append_array_key(this->json_data, "materials");
    this->textures     = json_append_array_key(this->json_data, "textures");
    this->images       = json_append_array_key(this->json_data, "images");

    this->buffers      = json_append_array_key(this->json_data, "buffers");
    this->buffviews    = json_append_array_key(this->json_data, "bufferViews");
    this->accessors    = json_append_array_key(this->json_data, "accessors");
    json_insert_parse(this->buffers, "{\"uri\":, \"byteLength\":}");
    json_insert_parse(this->json_data, JBXL_GLTF_SAMPLER);
}


////////////////////////////////////////////////////////////////////////////////////////////

void  GLTFData::addShell(MeshObjectData* meshdata, bool collider, SkinJointData* joints)
{
    if (meshdata==NULL) return;
    if (this->node_num==0 && this->gltf_name.buf==NULL) this->gltf_name = dup_Buffer(meshdata->data_name);
    this->alt_name = dup_Buffer(meshdata->alt_name);

    MeshFacetNode* facet = meshdata->facet;
    int shell_indexes  = 0;
    int shell_vertexes = 0;
    while (facet!=NULL) {
        shell_indexes  += facet->num_index;
        shell_vertexes += facet->num_vertex;
        facet = facet->next;
    }

    facet = meshdata->facet;
    AffineTrans<double>* affine = meshdata->affineTrans;

    addScenesNodes(facet, affine);
    addMeshes(facet);
    addMaterials(facet);
    addTextures(facet, affine);

    //addBufferViewsAoS(facet);
    //addAccessorsAoS(facet);

    addBufferViewsSoA(facet);
    addAccessorsSoA(facet);

    makeShellGeometory(facet, shell_indexes, shell_vertexes);

    if (collider) {
    }

    if (joints!=NULL) {
    }

    return;
}


void  GLTFData::addScenesNodes(MeshFacetNode* facet, AffineTrans<double>* affine)
{
    if (facet==NULL) return;

    // scenes
    if (this->node_num==0) {
        json_set_str_val(this->scenes_name, (char*)this->gltf_name.buf);
    }
    json_append_array_int_val(this->scenes_nodes, this->node_num);

    char buf[LBUF];

    // nodes
    memset(buf, 0, LBUF);
    snprintf(buf, LBUF-1, JBXL_GLTF_MESH, (char*)this->alt_name.buf, this->node_num);
    tJson* mesh = json_insert_parse(this->nodes, buf);
    tJson* matrix = json_append_array_key(mesh, "matrix");

    if (affine!=NULL) {
        affine->computeMatrix(true);
        for (int j=1; j<=4; j++) {
            for (int i=1; i<=4; i++) {
                float element = (float)(affine->matrix.element(i,j));
                json_append_array_real_val(matrix, element);
            }
        }
    }
    this->node_num++;

    return;
}


void  GLTFData::addMeshes(MeshFacetNode* facet)
{
    if (facet==NULL) return;
    char buf[LBUF];

    tJson* primitives = json_append_array_key(this->meshes, "primitives");
    while (facet!=NULL) {
        // meshes->primitives
        memset(buf, 0, LBUF);
        int access_num = this->mesh_num*4;
        snprintf(buf, LBUF-1, JBXL_GLTF_MESH_PRIMITIVE, access_num, access_num+1, access_num+2, access_num+3, this->mesh_num);
        json_insert_parse(primitives, buf);

        this->mesh_num++;
        facet = facet->next;
    }

    return;
}


void  GLTFData::addMaterials(MeshFacetNode* facet)
{
    if (facet==NULL) return;
    char buf[LBUF];

    while (facet!=NULL) {
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_MATERIAL, (char*)facet->material_id.buf, 1.0, 1.0, 1.0, 1.0, this->material_num);
        json_insert_parse(this->materials, buf);

        this->material_num++;
        facet = facet->next;
    }

    return;
}


void  GLTFData::addTextures(MeshFacetNode* facet, AffineTrans<double>* affine)
{
    if (facet==NULL) return;
    char buf[LBUF];

    while (facet!=NULL) {
        // UV Map and PLANAR Texture
/*
        size_t len = facet->num_texcrd*sizeof(UVMap<double>);
        UVMap<double>* uvmap = (UVMap<double>*)malloc(len);
        if (uvmap!=NULL) {
            memcpy(uvmap, facet->texcrd_value, len);
            //
            if (facet->material_param.mapping==MATERIAL_MAPPING_PLANAR) {
                Vector<double> scale(1.0, 1.0, 1.0);
                if (affine!=NULL) scale = affine->scale;
                facet->generatePlanarUVMap(scale, uvmap);
            }
            facet->execAffineTransUVMap(uvmap, facet->num_texcrd);
        }
*/
        if (facet->material_param.mapping==MATERIAL_MAPPING_PLANAR) {
           Vector<double> scale(1.0, 1.0, 1.0);
            if (affine!=NULL) scale = affine->scale;
            facet->generatePlanarUVMap(scale, facet->texcrd_value);
            //facet->generatePlanarUVMap(scale, uvmap);
        }
        facet->execAffineTransUVMap(facet->texcrd_value, facet->num_texcrd);

        memset(buf, 0, LBUF);
        //snprintf(buf, LBUF-1, JBXL_GLTF_TEXTURE, this->texture_num, 0);
        snprintf(buf, LBUF-1, JBXL_GLTF_TEXTURE, this->texture_num);
        json_insert_parse(this->textures, buf);

        // image of texture
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_IMAGE, (char*)facet->material_param.texture.getName());
        json_insert_parse(this->images, buf);

        this->texture_num++;
        facet = facet->next;
    }
    return;
}


void  GLTFData::addBufferViewsAoS(MeshFacetNode* facet)
{
    if (facet==NULL) return;

    char buf[LBUF];
    long unsigned int length = 0;

    while (facet!=NULL) {
        // bufferview of indexies
        length = (long unsigned int)facet->num_index*sizeof(int);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ELEMENT_VIEW, 0, this->bin_offset, length);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        // bufferview of vertex/normal/uvmap
        length = (long unsigned int)facet->num_vertex*sizeof(float)*8LU;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEW, 0, this->bin_offset, length, (int)sizeof(float)*8);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        facet = facet->next;
    }
    return;
}


void  GLTFData::addBufferViewsSoA(MeshFacetNode* facet)
{
    if (facet==NULL) return;

    char buf[LBUF];
    long unsigned int length = 0;

    while (facet!=NULL) {
        // bufferview of indexies
        length = (long unsigned int)facet->num_index*sizeof(int);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ELEMENT_VIEW, 0, this->bin_offset, length);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        // bufferview of vertex
        length = (long unsigned int)facet->num_vertex*sizeof(float)*3LU;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEW, 0, this->bin_offset, length, (int)sizeof(float)*3);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        // bufferview of normal
        length = (long unsigned int)facet->num_vertex*sizeof(float)*3LU;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEW, 0, this->bin_offset, length, (int)sizeof(float)*3);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        // bufferview of uvmap
        length = (long unsigned int)facet->num_vertex*sizeof(float)*2LU;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEW, 0, this->bin_offset, length, (int)sizeof(float)*2);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        facet = facet->next;
    }
    return;
}


void  GLTFData::addAccessorsAoS(MeshFacetNode* facet)
{
    if (facet==NULL) return;

    char buf[LBUF];

    while (facet!=NULL) {
        // accessor of indexies
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, this->view_num, 0LU, 5125, facet->num_index, "SCALAR");   // 5125: unsigned int
        json_insert_parse(this->accessors, buf);
        this->view_num++;

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

        facet = facet->next;
    }
    return;
}


void  GLTFData::addAccessorsSoA(MeshFacetNode* facet)
{
    if (facet==NULL) return;

    char buf[LBUF];

    while (facet!=NULL) {
        // accessor of indexies
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, this->view_num, 0LU, 5125, facet->num_index, "SCALAR");   // 5125: unsigned int
        json_insert_parse(this->accessors, buf);
        this->view_num++;

        // accessors of vertex/normal/uvmap
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, this->view_num, 0LU, 5126, facet->num_vertex, "VEC3");    // 5126: float
        json_insert_parse(this->accessors, buf);
        this->view_num++;

        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, this->view_num, 0LU, 5126, facet->num_vertex, "VEC3");    // 5126: float
        json_insert_parse(this->accessors, buf);
        this->view_num++;

        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR, this->view_num, 0LU, 5126, facet->num_vertex, "VEC2");    // 5126: float
        json_insert_parse(this->accessors, buf);
        this->view_num++;

        facet = facet->next;
    }
    return;
}


/*
void  GLTFData::makeBinDataAoS(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)
{
    if (facet==NULL) return;

    long unsigned int temp_len = (long unsigned int)shell_indexes*sizeof(int) + (long unsigned int)shell_vertexes*sizeof(float)*8LU;
    unsigned char* temp_buffer = (unsigned char*)malloc(temp_len);
    if (temp_buffer==NULL) {
        PRINT_MESG("GLTFData::makeBinDataAoS: ERROR: No more memory.\n");
        return;
    }
    if (this->bin_buffer.buf==NULL) {
        this->bin_buffer = make_Buffer(temp_len);
        if (this->bin_buffer.bufsz<=0) {
            PRINT_MESG("GLTFData::makeBinDataAoS: ERROR: No more memory.\n");
            ::free(temp_buffer);
            return;
        }
    }

    //
    long unsigned int length = 0;
    long unsigned int offset = 0;

    while (facet!=NULL) {
        // binary of indexies
        length = (long unsigned int)facet->num_index*sizeof(int);
        memcpy((void*)(temp_buffer + offset), (void*)facet->data_index, length);
        offset += length;

        // binary of vertex/normal/uvmap
        float temp;
        length = sizeof(float);

        for (int i=0; i<facet->num_vertex; i++) {
            temp = (float)facet->vertex_value[i].x;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->vertex_value[i].y;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->vertex_value[i].z;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->normal_value[i].x;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->normal_value[i].y;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->normal_value[i].z;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->texcrd_value[i].u;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = 1.0f - (float)facet->texcrd_value[i].v;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
        }

        facet = facet->next;
    }
    
    cat_b2Buffer(temp_buffer, &(this->bin_buffer), temp_len);
    ::free(temp_buffer);

    return;
}


void  GLTFData::makeBinDataSoA(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)
{
    if (facet==NULL) return;

    long unsigned int temp_len = (long unsigned int)shell_indexes*sizeof(int) + (long unsigned int)shell_vertexes*sizeof(float)*8LU;
    unsigned char* temp_buffer = (unsigned char*)malloc(temp_len);
    if (temp_buffer==NULL) {
        PRINT_MESG("GLTFData::makeBinDataSoA: ERROR: No more memory.\n");
        return;
    }
    if (this->bin_buffer.buf==NULL) {
        this->bin_buffer = make_Buffer(temp_len);
        if (this->bin_buffer.bufsz<=0) {
            PRINT_MESG("GLTFData::makeBinDataSoA: ERROR: No more memory.\n");
            ::free(temp_buffer);
            return;
        }
    }

    //
    long unsigned int length = 0;
    long unsigned int offset = 0;

    while (facet!=NULL) {
        // binary of indexies
        length = (long unsigned int)facet->num_index*sizeof(int);
        memcpy((void*)(temp_buffer + offset), (void*)facet->data_index, length);
        offset += length;

        // binary of vertex/normal/uvmap
        float temp;
        length = sizeof(float);

        for (int i=0; i<facet->num_vertex; i++) {
            temp = (float)facet->vertex_value[i].x;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->vertex_value[i].y;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->vertex_value[i].z;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
        }

        for (int i=0; i<facet->num_vertex; i++) {
            temp = (float)facet->normal_value[i].x;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->normal_value[i].y;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = (float)facet->normal_value[i].z;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
        }

        for (int i=0; i<facet->num_texcrd; i++) {
            temp = (float)facet->texcrd_value[i].u;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
            temp = 1.0f - (float)facet->texcrd_value[i].v;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, length);
            offset += length;
        }

        facet = facet->next;
    }
    
    cat_b2Buffer(temp_buffer, &(this->bin_buffer), temp_len);
    ::free(temp_buffer);

    return;
}
*/

void  GLTFData::makeShellGeometory(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)
{
    if (facet==NULL) return;

    GLTFShellGeoNode* _shell_node = new GLTFShellGeoNode();
    _shell_node->shell_indexes  = shell_indexes;
    _shell_node->shell_vertexes = shell_vertexes;
    _shell_node->data_index     = (int*)malloc(sizeof(int)*shell_indexes);
    _shell_node->vv             = (Vector<double>*)malloc(sizeof(Vector<double>)*shell_vertexes);
    _shell_node->vn             = (Vector<double>*)malloc(sizeof(Vector<double>)*shell_vertexes);
    _shell_node->vt             = (UVMap<double>* )malloc(sizeof(UVMap<double> )*shell_vertexes);
    //
    if (_shell_node->vv==NULL || _shell_node->vn==NULL || _shell_node->vt==NULL) {
        delete(_shell_node);
        return;
    }

    MeshFacetNode* temp_facet = facet;
    while (temp_facet!=NULL) {
        _shell_node->num_facet++;        // この SHELLにある FACETの数を数える
        temp_facet = temp_facet->next;
    }
    _shell_node->facet_index  = (int*)malloc(_shell_node->num_facet*sizeof(int));
    _shell_node->facet_vertex = (int*)malloc(_shell_node->num_facet*sizeof(int));

    int index_offset  = 0;
    int vertex_offset = 0;
    int facet_num     = 0;

    while (facet!=NULL) {
        _shell_node->facet_index [facet_num] = facet->num_index;
        _shell_node->facet_vertex[facet_num] = facet->num_vertex;
        facet_num++;

        // indexies
        for (int i=0; i<facet->num_index; i++) {
            _shell_node->data_index[index_offset + i] = facet->data_index[i];
        }
        index_offset += facet->num_index;

        // vertex/normal/uvmap
        for (int i=0; i<facet->num_vertex; i++) {
            _shell_node->vv[vertex_offset + i] = facet->vertex_value[i];
            _shell_node->vn[vertex_offset + i] = facet->normal_value[i];
            _shell_node->vt[vertex_offset + i] = facet->texcrd_value[i];
        }
        vertex_offset += facet->num_vertex;

        facet = facet->next;
    }
    
    // _shell_node をリストの最後に繋げる
    GLTFShellGeoNode* prv = NULL;
    GLTFShellGeoNode* ptr = this->shellNode;
    while(ptr!=NULL) {
        prv = ptr;
        ptr = ptr->next;
    }
    if (prv==NULL) this->shellNode = _shell_node;
    else           prv->next       = _shell_node;

    return;
}


void  GLTFData::makeBinDataAoS(void)
{
    if (this->shellNode==NULL) return;

    int solid_indexes  = 0;
    int solid_vertexes = 0;

    GLTFShellGeoNode*  _shell_node = this->shellNode;
    while (_shell_node!=NULL) {
        solid_indexes  += _shell_node->shell_indexes;
        solid_vertexes += _shell_node->shell_vertexes;
        _shell_node = _shell_node->next;
    }

    int buffer_len = (int)(solid_indexes*sizeof(int) + solid_vertexes*sizeof(float)*8);
    this->bin_buffer = make_Buffer(buffer_len);
    if (this->bin_buffer.buf==NULL) return;

    int   i_length = 0;
    int   v_length = sizeof(float);
    float temp = 0.0f;

    _shell_node = this->shellNode;
    while (_shell_node!=NULL) {
        int i_offset = 0;
        int v_offset = 0;
        for (int f=0; f<_shell_node->num_facet; f++) {
            // binary of indexies
            i_length = _shell_node->facet_index[f] * sizeof(int);
            cat_b2Buffer(_shell_node->data_index + i_offset, &(this->bin_buffer), i_length);
            i_offset += _shell_node->facet_index[f];

            for (int i=0; i<_shell_node->facet_vertex[f]; i++) {
                temp = (float)_shell_node->vv[v_offset + i].x;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = (float)_shell_node->vv[v_offset + i].y;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = (float)_shell_node->vv[v_offset + i].z;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                //
                temp = (float)_shell_node->vn[v_offset + i].x;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = (float)_shell_node->vn[v_offset + i].y;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = (float)_shell_node->vn[v_offset + i].z;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                //
                temp = (float)_shell_node->vt[v_offset + i].u;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = 1.0f - (float)_shell_node->vt[v_offset + i].v;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
            }
            v_offset += _shell_node->facet_vertex[f];
        }
        _shell_node = _shell_node->next;
    }
    return;
}


void  GLTFData::makeBinDataSoA(void)
{
    if (this->shellNode==NULL) return;

    int solid_indexes  = 0;
    int solid_vertexes = 0;

    GLTFShellGeoNode*  _shell_node = this->shellNode;
    while (_shell_node!=NULL) {
        solid_indexes  += _shell_node->shell_indexes;
        solid_vertexes += _shell_node->shell_vertexes;
        _shell_node = _shell_node->next;
    }

    int buffer_len = (int)(solid_indexes*sizeof(int) + solid_vertexes*sizeof(float)*8);
    this->bin_buffer = make_Buffer(buffer_len);
    if (this->bin_buffer.buf==NULL) return;

    int   i_length = 0;
    int   v_length = sizeof(float);
    float temp = 0.0f;

    _shell_node = this->shellNode;
    while (_shell_node!=NULL) {
        int i_offset = 0;
        int v_offset = 0;
        for (int f=0; f<_shell_node->num_facet; f++) {
            // binary of indexies
            i_length = _shell_node->facet_index[f] * sizeof(int);
            cat_b2Buffer(_shell_node->data_index + i_offset, &(this->bin_buffer), i_length);
            i_offset += _shell_node->facet_index[f];

            for (int i=0; i<_shell_node->facet_vertex[f]; i++) {
                temp = (float)_shell_node->vv[v_offset + i].x;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = (float)_shell_node->vv[v_offset + i].y;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = (float)_shell_node->vv[v_offset + i].z;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
            }
            //
            for (int i=0; i<_shell_node->facet_vertex[f]; i++) {
                temp = (float)_shell_node->vn[v_offset + i].x;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = (float)_shell_node->vn[v_offset + i].y;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = (float)_shell_node->vn[v_offset + i].z;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
            }
            //
            for (int i=0; i<_shell_node->facet_vertex[f]; i++) {
                temp = (float)_shell_node->vt[v_offset + i].u;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
                temp = 1.0f - (float)_shell_node->vt[v_offset + i].v;
                cat_b2Buffer(&temp, &(this->bin_buffer), v_length);
            }
            v_offset += _shell_node->facet_vertex[f];
        }
        _shell_node = _shell_node->next;
    }
    return;
}



///////////////////////////////////////////////////////////////////////////////////////////////////

void  GLTFData::outputFile(const char* fname, const char* out_path, const char* tex_dirn, const char* bin_dirn)
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

    //makeBinDataAoS();
    makeBinDataSoA();

    // output json/binary
    cat_Buffer(&file_name, &gltf_path);
    change_file_extension_Buffer(&gltf_path, ".gltf");
    Buffer bin_path = dup_Buffer(gltf_path);
    change_file_extension_Buffer(&bin_path, ".bin");

    if (tex_dirn!=NULL) {
    }
    if (bin_dirn!=NULL) {
    }

    this->output_gltf((char*)gltf_path.buf, (char*)bin_path.buf);

    free_Buffer(&gltf_path);
    free_Buffer(&bin_path);
    free_Buffer(&file_name);
    //free_Buffer(&rel_tex);
    //
    return;
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


/**
Vector<double>  GLTFData::execDegeneracy(void)

GLTFデータの 原点縮退変換を行う．
no_offset が trueの場合，データの中心を原点に戻し，実際の位置をオフセットで返す．
*/
Vector<double>  GLTFData::execDegeneracy(void)
{
    Vector<double> center(0.0, 0.0, 0.0);

    if (this->no_offset) center = affineTrans->shift;

    return center;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
void  GLTFShellGeoNode::init(void)
{
    this->shell_indexes  = 0;
    this->shell_vertexes = 0;
    this->collider       = true;

    this->num_facet      = 0;
    this->facet_index    = NULL;
    this->facet_vertex   = NULL;

    //this->material   = init_Buffer();

    this->data_index     = NULL;
    this->vv             = NULL;
    this->vn             = NULL;
    this->vt             = NULL;
    this->uvmap_trans    = NULL;
    this->next           = NULL;

    return;
}


void  GLTFShellGeoNode::free(void)
{
    if (this->facet_index !=NULL) ::free(this->facet_index);
    if (this->facet_vertex!=NULL) ::free(this->facet_vertex);
    this->facet_index   =NULL;
    this->facet_vertex =NULL;

    if (this->data_index!=NULL) ::free(this->data_index);
    this->data_index = NULL;
    if (this->vv!=NULL) ::free(this->vv);
    if (this->vn!=NULL) ::free(this->vn);
    if (this->vt!=NULL) ::free(this->vt);
    this->vv = NULL;
    this->vn = NULL;
    this->vt = NULL;

    freeAffineTrans(this->uvmap_trans);
    this->uvmap_trans = NULL;

    this->delete_next();
}


void  GLTFShellGeoNode::delete_next(void)
{
    if (next==NULL) return;

    GLTFShellGeoNode* _next = this->next;
    while (_next!=NULL) {
        GLTFShellGeoNode* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    this->next = NULL;
}

