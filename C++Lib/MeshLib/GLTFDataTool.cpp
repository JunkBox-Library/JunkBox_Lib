/**
@brief    GLTF用 ツール
@file     GLTFDataTool.cpp
@author   Fumi.Iseki (C)
*/

#include  "GLTFDataTool.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////
// Class GLTFShellNode
//

void  GLTFShellNode::init(void)
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
    //this->uvmap_trans    = NULL;
    this->next           = NULL;

    return;
}


void  GLTFShellNode::free(void)
{
    if (this->facet_index !=NULL) ::free(this->facet_index);
    if (this->facet_vertex!=NULL) ::free(this->facet_vertex);
    this->facet_index  =NULL;
    this->facet_vertex =NULL;

    if (this->data_index!=NULL) ::free(this->data_index);
    this->data_index = NULL;
    if (this->vv!=NULL) ::free(this->vv);
    if (this->vn!=NULL) ::free(this->vn);
    if (this->vt!=NULL) ::free(this->vt);
    this->vv = NULL;
    this->vn = NULL;
    this->vt = NULL;

    //freeAffineTrans(this->uvmap_trans);
    //this->uvmap_trans = NULL;

    this->delete_next();
}


void  GLTFShellNode::delete_next(void)
{
    if (next==NULL) return;

    GLTFShellNode* _next = this->next;
    while (_next!=NULL) {
        GLTFShellNode* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    this->next = NULL;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Class GLTFData
//

GLTFData::~GLTFData(void)
{
    DEBUG_MODE PRINT_MESG("GLTFData::DESTRUCTOR:\n");
    this->free();
}


void  GLTFData::init(void)
{
    this->bin_mode      = JBXL_GLTF_BIN_AOS;
    this->bin_seq       = false;

    this->gltf_name     = init_Buffer();
    this->alt_name      = init_Buffer();
    this->phantom_out   = true;
    this->no_offset     = false;

    this->forUnity      = true;
    this->forUE         = false;
    this->engine        = JBXL_3D_ENGINE_UNITY;

    this->image_list    = new_tList_anchor_node();
    this->material_list = new_tList_anchor_node();

    this->affineTrans   = NULL;
    this->skeleton.init();

    this->bin_buffer    = init_Buffer();
    this->bin_offset    = 0;

    this->shell_no      = 0;
    this->node_no       = 0;
    this->image_no      = 0;
    this->material_no   = 0;
    this->mesh_no       = 0;
    this->view_no       = 0;

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

    this->shellNode     = NULL;

    initGLTF();
}


void  GLTFData::free(void)
{
    free_Buffer(&(this->gltf_name));
    free_Buffer(&(this->alt_name));
    free_Buffer(&(this->bin_buffer));

    del_tList(&(this->image_list));
    del_tList(&(this->material_list));
    del_json(&(this->json_data));
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
}



//////////////////////////////////////////////////////////////////////////////////////////

/**
void  GLTFData::addShell(MeshObjectData* shelldata, bool collider, SkinJointData* joints)

この関数は SOLID毎に複数回呼ばれ，SOLIDに SHELLを追加する．

@param  shelldata  SHELLデータ．複数の FACETを含む．
@param  collider   コライダーのサポート
@param  joints     ジョイントデータ．デフォルトは NULL
*/
void  GLTFData::addShell(MeshObjectData* shelldata, bool collider, SkinJointData* joints)
{
    if (shelldata==NULL) return;
    if (this->shell_no==0 && this->gltf_name.buf==NULL) {
        if (shelldata->data_name.buf!=NULL) this->gltf_name = dup_Buffer(shelldata->data_name);
        else                                this->gltf_name = make_Buffer_bystr("Object");
    }
    if (shelldata->alt_name.buf!=NULL) {
        this->alt_name = dup_Buffer(shelldata->alt_name);
    }

    MeshFacetNode* facet = shelldata->facet;
    int shell_indexes  = 0;
    int shell_vertexes = 0;
    while (facet!=NULL) {
        shell_indexes  += facet->num_index;
        shell_vertexes += facet->num_vertex;
        facet = facet->next;
    }

    facet = shelldata->facet;
    AffineTrans<double>* affine = shelldata->affineTrans;

    //
    this->addScenesNodes(facet, affine);
    this->addTextures(facet);
    this->addMaterials(facet);
    this->addMeshes(facet);

    // for bin data
    if (this->bin_mode==JBXL_GLTF_BIN_AOS) {
        this->addBufferViewsAoS(facet);
        this->addAccessorsAoS(facet);
    }
    else {
        this->addBufferViewsSoA(facet);
        this->addAccessorsSoA(facet);
    }
    this->execAffineUVMap(facet, affine);

    if (this->bin_seq) {
        if (this->bin_mode==JBXL_GLTF_BIN_AOS) {
            this->createBinDataSeqAoS(facet, shell_indexes, shell_vertexes);
        }
        else {
            this->createBinDataSeqSoA(facet, shell_indexes, shell_vertexes);
        }
    }
    else {
        this->createShellGeoData(facet, shell_indexes, shell_vertexes);
    }

    //
    this->phantom_out = true;
    if (collider) {
        this->phantom_out = false;
    }

    //
    if (joints!=NULL) {
    }

    //
    free_Buffer(&this->alt_name);
    this->shell_no++;
    return;
}


void  GLTFData::addScenesNodes(MeshFacetNode* facet, AffineTrans<double>* affine)
{
    if (facet==NULL) return;
    char buf[LBUF];

    // scenes
    if (this->node_no==0) {
        json_set_str_val(this->scenes_name, (char*)this->gltf_name.buf);
    }
    json_append_array_int_val(this->scenes_nodes, this->node_no);

    // nodes
    memset(buf, 0, LBUF);
    Buffer node_name = init_Buffer();
    if (this->alt_name.buf!=NULL) {
        node_name = dup_Buffer(this->alt_name);
    }
    else {
        node_name = make_Buffer_bystr("Node_#");
        cat_i2Buffer(this->node_no, &node_name);
    }
    snprintf(buf, LBUF-1, JBXL_GLTF_MESH, (char*)node_name.buf, this->node_no);
    tJson* mesh = json_insert_parse(this->nodes, buf);
    free_Buffer(&node_name);

    // affine translarion
    tJson* matrix = json_append_array_key(mesh, "matrix");
    if (affine!=NULL) {
        AffineTrans<double> trans = this->getAffineTrans4Engine(*affine);
        for (int j=1; j<=4; j++) {
            for (int i=1; i<=4; i++) {
                float element = (float)trans.matrix.element(i, j);
                json_append_array_real_val(matrix, element);
            }
        }
        trans.free();
    }
    this->node_no++;

    return;
}


void  GLTFData::addTextures(MeshFacetNode* facet)
{
    if (facet==NULL) return;
    char buf[LBUF];

    while (facet!=NULL) {
        if (!facet->same_material) {
            char* image_name = (char*)facet->material_param.texture.getName();
            tList* lt = search_key_tList(this->image_list, image_name, 1);
            //
            if (lt==NULL) {
                tList* ltend = find_tList_end(this->image_list);
                add_tList_node_bystr(ltend, this->image_no, 0, image_name, NULL, NULL, 0);
                // images
                memset(buf, 0, LBUF);
                snprintf(buf, LBUF-1, JBXL_GLTF_IMAGE, image_name);
                json_insert_parse(this->images, buf);

                // textures
                memset(buf, 0, LBUF);
                //snprintf(buf, LBUF-1, JBXL_GLTF_TEXTURE, this->image, 0);
                snprintf(buf, LBUF-1, JBXL_GLTF_TEXTURE, this->image_no);
                json_insert_parse(this->textures, buf);

                this->image_no++;
            }
        }
        facet = facet->next;
    }
    return;
}


void  GLTFData::addMaterials(MeshFacetNode* facet)
{
    if (facet==NULL) return;
    char buf[LBUF];

    while (facet!=NULL) {
        if (!facet->same_material) {
            char* material_name = (char*)facet->material_id.buf; 
            if (material_name[0]=='#') material_name++;
            //
            tList* lt = search_key_tList(this->material_list, material_name, 1);
            if (lt==NULL) {
                tList* ltend = find_tList_end(this->material_list);
                add_tList_node_bystr(ltend, this->material_no, 0, material_name, NULL, NULL, 0);
                // 
                char* image_name = (char*)facet->material_param.texture.getName();
                int img_no = 0;
                lt = search_key_tList(this->image_list, image_name, 1);
                if (lt!=NULL) {
                    img_no = lt->ldat.id;
                }
                else {
                    PRINT_MESG("GLTFData::addMaterials: ERROR: lost image file (%s). this is coding miss!\n", image_name);
                }
                //
                memset(buf, 0, LBUF);
                snprintf(buf, LBUF-1, JBXL_GLTF_MATERIAL, material_name, 1.0, 1.0, 1.0, 1.0, img_no);
                json_insert_parse(this->materials, buf);
                this->material_no++;
            }
        }
        facet = facet->next;
    }
    return;
}


void  GLTFData::addMeshes(MeshFacetNode* facet)
{
    if (facet==NULL) return;
    char buf[LBUF];
    int access_no = 0;

    tJson* primitives = json_append_array_key(this->meshes, "primitives");
    while (facet!=NULL) {
        char* material_name = (char*)facet->material_id.buf; 
        if (material_name[0]=='#') material_name++;
        //
        int mtl_no = 0;
        tList* lt = search_key_tList(this->material_list, material_name, 1);
        if (lt!=NULL) {
            mtl_no = lt->ldat.id;
        }
        else {
            PRINT_MESG("GLTFData::addMeshes: ERROR: lost material (%s). this is coding miss!\n", material_name);
        }
        //
        memset(buf, 0, LBUF);
        access_no = this->mesh_no*4;
        snprintf(buf, LBUF-1, JBXL_GLTF_MESH_PRIM, access_no, access_no+1, access_no+2, access_no+3, mtl_no);
        json_insert_parse(primitives, buf);

        this->mesh_no++;
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
        gltfFacetMinMax mm = getFacetMinMax(facet);

        // accessor of indexies
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR_S, this->view_no, 0LU, 5125, facet->num_index, "SCALAR", mm.index_max, mm.index_min);   // 5125: unsigned int
        json_insert_parse(this->accessors, buf);
        this->view_no++;

        // accessors of vertex/normal/uvmap
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR_V3, this->view_no,               0LU, 5126, facet->num_vertex, "VEC3", 
                                                     mm.vertex_x_max, mm.vertex_y_max, mm.vertex_z_max, mm.vertex_x_min, mm.vertex_y_min, mm.vertex_z_min);
        json_insert_parse(this->accessors, buf);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR_V3, this->view_no, sizeof(float)*3LU, 5126, facet->num_vertex, "VEC3",
                                                     mm.normal_x_max, mm.normal_y_max, mm.normal_z_max, mm.normal_x_min, mm.normal_y_min, mm.normal_z_min);
        json_insert_parse(this->accessors, buf);
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR_V2, this->view_no, sizeof(float)*6LU, 5126, facet->num_vertex, "VEC2",
                                                     mm.texcrd_u_max, mm.texcrd_u_max, mm.texcrd_v_min, mm.texcrd_v_min);
        json_insert_parse(this->accessors, buf);
        this->view_no++;

        facet = facet->next;
    }
    return;
}


void  GLTFData::addAccessorsSoA(MeshFacetNode* facet)
{
    if (facet==NULL) return;
    char buf[LBUF];

    while (facet!=NULL) {
        gltfFacetMinMax mm = getFacetMinMax(facet);

        // accessor of indexies
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR_S, this->view_no, 0LU, 5125, facet->num_index, "SCALAR", mm.index_max, mm.index_min);   // 5125: unsigned int
        json_insert_parse(this->accessors, buf);
        this->view_no++;

        // accessors of vertex/normal/uvmap
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR_V3, this->view_no, 0LU, 5126, facet->num_vertex, "VEC3",
                                                     mm.vertex_x_max, mm.vertex_y_max, mm.vertex_z_max, mm.vertex_x_min, mm.vertex_y_min, mm.vertex_z_min);
        json_insert_parse(this->accessors, buf);
        this->view_no++;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR_V3, this->view_no, 0LU, 5126, facet->num_vertex, "VEC3",
                                                     mm.normal_x_max, mm.normal_y_max, mm.normal_z_max, mm.normal_x_min, mm.normal_y_min, mm.normal_z_min);
        json_insert_parse(this->accessors, buf);
        this->view_no++;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSOR_V2, this->view_no, 0LU, 5126, facet->num_vertex, "VEC2",
                                                     mm.texcrd_u_max, mm.texcrd_u_max, mm.texcrd_v_min, mm.texcrd_v_min);
        json_insert_parse(this->accessors, buf);
        this->view_no++;

        facet = facet->next;
    }
    return;
}


/**
void  GLTFData::createBinDataSeqAoS(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)

GLTFの Geometoryデータを AoS形式で 逐次作成し，this->bin_bufferに追加していく．
最終的に作成された this->bin_buffer はそのまま出力できる． 
this->shellNode は使用しない．
*/
void  GLTFData::createBinDataSeqAoS(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)
{
    if (facet==NULL) return;

    long unsigned int temp_len = (long unsigned int)shell_indexes*sizeof(int) + (long unsigned int)shell_vertexes*sizeof(float)*8LU;
    unsigned char* temp_buffer = (unsigned char*)malloc(temp_len);
    if (temp_buffer==NULL) {
        PRINT_MESG("GLTFData::createBinDataSeqAoS: ERROR: No more memory.\n");
        return;
    }
    if (this->bin_buffer.buf==NULL) {
        this->bin_buffer = make_Buffer(temp_len);
        if (this->bin_buffer.bufsz<=0) {
            PRINT_MESG("GLTFData::createBinDataSeqAoS: ERROR: No more memory.\n");
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


/**
void  GLTFData::createBinDataSeqSoA(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)

GLTFの Geometoryデータを SoA形式で 逐次作成し，this->bin_bufferに追加していく．
最終的に作成された this->bin_buffer はそのまま出力できる． 
this->shellNode は使用しない．
*/
void  GLTFData::createBinDataSeqSoA(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)
{
    if (facet==NULL) return;

    long unsigned int temp_len = (long unsigned int)shell_indexes*sizeof(int) + (long unsigned int)shell_vertexes*sizeof(float)*8LU;
    unsigned char* temp_buffer = (unsigned char*)malloc(temp_len);
    if (temp_buffer==NULL) {
        PRINT_MESG("GLTFData::createBinDataSeqSoA: ERROR: No more memory.\n");
        return;
    }
    if (this->bin_buffer.buf==NULL) {
        this->bin_buffer = make_Buffer(temp_len);
        if (this->bin_buffer.bufsz<=0) {
            PRINT_MESG("GLTFData::createBinDataSeqSoA: ERROR: No more memory.\n");
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


void  GLTFData::execAffineUVMap(MeshFacetNode* facet, AffineTrans<double>* affine)
{
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
        }
        facet->execAffineTransUVMap(facet->texcrd_value, facet->num_texcrd);

        facet = facet->next;
    }
    return;
}


/////////////////////////////////////////////////////////////////////////////////////
// Create all bin data at once

/**
void  GLTFData::createShellGeoData(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)

SHELL毎に呼び出され，SHELL中の全FACETのジオメトリ情報を this->shellNode に格納する．

@param  facet           SHELL中の FACETの先頭データ．
@param  shell_indexes   SHELL中の indexデータの総数．
@param  shell_vetexes   SHELL中の vertexデータの総数．
@param  affine          SHELLの Affine変換へのポインタ．
*/
void  GLTFData::createShellGeoData(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)
{
    if (facet==NULL) return;

    GLTFShellNode* _shell_node = new GLTFShellNode();
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
    //
    MeshFacetNode* temp_facet = facet;
    while (temp_facet!=NULL) {
        _shell_node->num_facet++;        // この SHELLにある FACETの数を数える
        temp_facet = temp_facet->next;
    }
    _shell_node->facet_index  = (int*)malloc(_shell_node->num_facet*sizeof(int));
    _shell_node->facet_vertex = (int*)malloc(_shell_node->num_facet*sizeof(int));

    long unsigned int index_offset  = 0;
    long unsigned int vertex_offset = 0;
    long unsigned int facet_no      = 0;

    while (facet!=NULL) {
        // save index and vertex number
        _shell_node->facet_index [facet_no] = facet->num_index;
        _shell_node->facet_vertex[facet_no] = facet->num_vertex;
        facet_no++;

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
    GLTFShellNode* prv = NULL;
    GLTFShellNode* ptr = this->shellNode;
    while(ptr!=NULL) {
        prv = ptr;
        ptr = ptr->next;
    }
    if (prv==NULL) this->shellNode = _shell_node;
    else           prv->next       = _shell_node;

    return;
}


/**
void  GLTFData::createBinDataAoS(void)

GLTFの this->shell_node に格納された Geometoryデータを AoS形式で this->bin_bufferに格納する．
this->shell_node 中の Geometory データは，予め createShellGeoData() で計算しておく必要がある．
this->bin_buffer はそのまま出力できる． 
*/
void  GLTFData::createBinDataAoS(void)
{
    if (this->shellNode==NULL) return;

    long unsigned int solid_indexes  = 0;
    long unsigned int solid_vertexes = 0;

    GLTFShellNode*  _shell_node = this->shellNode;
    while (_shell_node!=NULL) {
        solid_indexes  += _shell_node->shell_indexes;
        solid_vertexes += _shell_node->shell_vertexes;
        _shell_node = _shell_node->next;
    }

    long unsigned int buffer_len = solid_indexes*sizeof(int) + solid_vertexes*sizeof(float)*8LU;
    this->bin_buffer = make_Buffer(buffer_len);
    if (this->bin_buffer.buf==NULL) {
        PRINT_MESG("GLTFData::createBinDataAoS: ERROR: No more memory.\n");
        return;
    }

    long unsigned int i_length = 0;
    long unsigned int v_length = sizeof(float);
    float temp = 0.0f;

    _shell_node = this->shellNode;
    while (_shell_node!=NULL) {             // 全SHELL
        long unsigned int i_offset = 0;
        long unsigned int v_offset = 0;
        for (int f=0; f<_shell_node->num_facet; f++) {      // SHELL中の FACET
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


/**
void  GLTFData::createBinDataSoA(void)

GLTFの this->shell_node に格納された Geometoryデータを SoA形式で this->bin_bufferに格納する．
this->shell_node 中の Geometory データは，予め createShellGeoData() で計算しておく必要がある．
this->bin_buffer はそのまま出力できる． 
*/
void  GLTFData::createBinDataSoA(void)
{
    if (this->shellNode==NULL) return;

    long unsigned int solid_indexes  = 0;
    long unsigned int solid_vertexes = 0;

    GLTFShellNode*  _shell_node = this->shellNode;
    while (_shell_node!=NULL) {
        solid_indexes  += _shell_node->shell_indexes;
        solid_vertexes += _shell_node->shell_vertexes;
        _shell_node = _shell_node->next;
    }

    long unsigned int buffer_len = solid_indexes*sizeof(int) + solid_vertexes*sizeof(float)*8LU;
    this->bin_buffer = make_Buffer(buffer_len);
    if (this->bin_buffer.buf==NULL) {
        PRINT_MESG("GLTFData::createBinDataSoA: ERROR: No more memory.\n");
        return;
    }

    long unsigned int i_length = 0;
    long unsigned int v_length = sizeof(float);
    float temp = 0.0f;

    _shell_node = this->shellNode;
    while (_shell_node!=NULL) {             // 全SHELL
        long unsigned int i_offset = 0;
        long unsigned int v_offset = 0;
        for (int f=0; f<_shell_node->num_facet; f++) {      // SHELL中の FACET
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



//////////////////////////////////////////////////////////////////////////////////////////
// Output
//

void  GLTFData::outputFile(const char* fname, const char* out_path, const char* tex_dirn, const char* bin_dirn)
{
    char* packname = pack_head_tail_char(get_file_name(fname), ' ');
    Buffer file_name = make_Buffer_bystr(packname);
    ::free(packname);

    canonical_filename_Buffer(&file_name);
    if (file_name.buf[0]=='.') file_name.buf[0] = '_';
    //
    Buffer gltf_path;   // 出力パス
    if (out_path==NULL) gltf_path = make_Buffer_bystr("./");
    else                gltf_path = make_Buffer_bystr(out_path);
    Buffer bin_path = make_Buffer_bystr(out_path);

    Buffer rel_bin;     // bin 相対パス
    if (bin_dirn==NULL) rel_bin = make_Buffer_bystr("");
    else                rel_bin = make_Buffer_bystr(bin_dirn);

    changeTexturePath((char*)tex_dirn);
    //
    if (this->bin_buffer.buf==NULL && !this->bin_seq) {
        if (this->bin_mode==JBXL_GLTF_BIN_AOS) createBinDataAoS();
        else                                   createBinDataSoA();
    }

    // output json/binary
    cat_Buffer(&file_name, &gltf_path);
    change_file_extension_Buffer(&gltf_path, ".gltf");

    cat_Buffer(&rel_bin, &bin_path);
    cat_Buffer(&file_name, &bin_path);
    change_file_extension_Buffer(&bin_path, ".bin");

    this->output_gltf((char*)gltf_path.buf, (char*)bin_path.buf);

    free_Buffer(&gltf_path);
    free_Buffer(&bin_path);
    free_Buffer(&file_name);
    free_Buffer(&rel_bin);
    //
    return;
}


void  GLTFData::output_gltf(char* json_file, char* bin_file)
{
    char* out_path = get_file_path(json_file);
    int pos = strlen(out_path);
    char* bin_fname = &bin_file[pos];
    ::free(out_path);

    tJson* uri = search_key_json(this->buffers, "uri", FALSE, 1);
    if (uri!=NULL) json_set_str_val(uri, bin_fname);
    tJson* blen = search_key_json(this->buffers, "byteLength", FALSE, 1);
    if (uri!=NULL) json_set_int_val(blen, this->bin_buffer.vldsz);

    // json
    FILE* fp = fopen(json_file, "w");
    if (fp==NULL) return;
    print_json(fp, this->json_data, JSON_INDENT_FORMAT);
    fclose(fp);

    // binary data
    fp = fopen(bin_file, "wb");
    if (fp==NULL) return;
    fwrite((void*)(this->bin_buffer.buf), this->bin_buffer.vldsz, 1, fp);
    fclose(fp);

    return;
}


// "xyz.png"  ->  "(tex_dirn)xyz.png"
void  GLTFData::changeTexturePath(char* tex_dirn)
{
    if (tex_dirn==NULL) return;

    tJson* jsn = this->images;
    if (jsn!=NULL) jsn = jsn->next;
    if (jsn!=NULL) {
        while (jsn->esis!=NULL) jsn = jsn->esis;
    }
    //
    while (jsn!=NULL) {     // here is {
        tJson* uri = search_key_json(jsn, "uri", FALSE, 1);
        if (uri!=NULL) {
            Buffer* tex = new_Buffer(strlen(tex_dirn) + uri->ldat.val.vldsz + 5);   // ../ + \0 + 1(予備)
            cat_s2Buffer("\"", tex); 
            if (this->phantom_out) cat_s2Buffer("../", tex); 
            cat_s2Buffer(tex_dirn, tex);
            cat_s2Buffer(&(uri->ldat.val.buf[1]), tex); 
            free_Buffer(&uri->ldat.val);
            uri->ldat.val = *tex;
        }
        jsn = jsn->ysis;
    }
    return;
}



//////////////////////////////////////////////////////////////////////////////////////////

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


/**
AffineTrans<double>  GLTFData::getAffineTrans4Engine(AffineTrans<double> affine)

使用するエンジンに合わせて，FACET毎の Affine変換のパラメータを変更する．
*/
AffineTrans<double>  GLTFData::getAffineTrans4Engine(AffineTrans<double> affine)
{
    AffineTrans<double> trans;
    for (int i=1; i<=4; i++) trans.matrix.element(i, i) = 1.0;
    //
    if (this->engine==JBXL_3D_ENGINE_UNITY) {
        trans.matrix.element(2, 2) =  0.0;
        trans.matrix.element(3, 3) =  0.0;
        trans.matrix.element(3, 2) = -1.0;    // y -> -z
        trans.matrix.element(2, 3) =  1.0;    // z -> y
    }
    else if (this->engine==JBXL_3D_ENGINE_UE) {
        for (int i=1; i<=4; i++) trans.matrix.element(i, i) = 100.0;
    }
    //
    trans.affineMatrixFllow(affine);     // engineTrans = engineTrans * (*affine)

    return trans;
}


gltfFacetMinMax  GLTFData::getFacetMinMax(MeshFacetNode* facet)
{
    gltfFacetMinMax min_max;

    min_max.index_max = facet->data_index[0];
    min_max.index_min = facet->data_index[0];
    min_max.vertex_x_max = (float)facet->vertex_value[0].x;
    min_max.vertex_x_min = (float)facet->vertex_value[0].x;
    min_max.vertex_y_max = (float)facet->vertex_value[0].y;
    min_max.vertex_y_min = (float)facet->vertex_value[0].y;
    min_max.vertex_z_max = (float)facet->vertex_value[0].z;
    min_max.vertex_z_min = (float)facet->vertex_value[0].z;
    min_max.normal_x_max = (float)facet->normal_value[0].x;
    min_max.normal_x_min = (float)facet->normal_value[0].x;
    min_max.normal_y_max = (float)facet->normal_value[0].y;
    min_max.normal_y_min = (float)facet->normal_value[0].y;
    min_max.normal_z_max = (float)facet->normal_value[0].z;
    min_max.normal_z_min = (float)facet->normal_value[0].z;
    min_max.texcrd_u_max = (float)facet->texcrd_value[0].u;
    min_max.texcrd_u_min = (float)facet->texcrd_value[0].u;
    min_max.texcrd_v_max = 1.0f - (float)facet->texcrd_value[0].v;
    min_max.texcrd_v_min = 1.0f - (float)facet->texcrd_value[0].v;

    int   i_temp;
    float f_temp;

    for (int i=1; i<facet->num_index; i++) {
        i_temp = facet->data_index[i];
        if (min_max.index_max < i_temp) min_max.index_max = i_temp;
        if (min_max.index_min > i_temp) min_max.index_min = i_temp;
    }
    for (int i=1; i<facet->num_vertex; i++) {
        f_temp = (float)facet->vertex_value[i].x;
        if (min_max.vertex_x_max < f_temp) min_max.vertex_x_max = f_temp;
        if (min_max.vertex_x_min > f_temp) min_max.vertex_x_min = f_temp;
        f_temp = (float)facet->vertex_value[i].y;
        if (min_max.vertex_y_max < f_temp) min_max.vertex_y_max = f_temp;
        if (min_max.vertex_y_min > f_temp) min_max.vertex_y_min = f_temp;
        f_temp = (float)facet->vertex_value[i].z;
        if (min_max.vertex_z_max < f_temp) min_max.vertex_z_max = f_temp;
        if (min_max.vertex_z_min > f_temp) min_max.vertex_z_min = f_temp;

        f_temp = (float)facet->normal_value[i].x;
        if (min_max.normal_x_max < f_temp) min_max.normal_x_max = f_temp;
        if (min_max.normal_x_min > f_temp) min_max.normal_x_min = f_temp;
        f_temp = (float)facet->normal_value[i].y;
        if (min_max.normal_y_max < f_temp) min_max.normal_y_max = f_temp;
        if (min_max.normal_y_min > f_temp) min_max.normal_y_min = f_temp;
        f_temp = (float)facet->normal_value[i].z;
        if (min_max.normal_z_max < f_temp) min_max.normal_z_max = f_temp;
        if (min_max.normal_z_min > f_temp) min_max.normal_z_min = f_temp;

        f_temp = (float)facet->texcrd_value[i].u;
        if (min_max.texcrd_u_max < f_temp) min_max.texcrd_u_max = f_temp;
        if (min_max.texcrd_u_min > f_temp) min_max.texcrd_u_min = f_temp;
        f_temp = 1.0f - (float)facet->texcrd_value[i].v;
        if (min_max.texcrd_v_max < f_temp) min_max.texcrd_v_max = f_temp;
        if (min_max.texcrd_v_min > f_temp) min_max.texcrd_v_min = f_temp;
    }
    return min_max;
}

