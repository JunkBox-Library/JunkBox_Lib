﻿/**
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

    this->num_facets     = 0;
    this->facet_index    = NULL;
    this->facet_vertex   = NULL;

    //this->material       = init_Buffer();

    this->vi             = NULL;
    this->vv             = NULL;
    this->vn             = NULL;
    this->vu             = NULL;
    this->vj             = NULL;
    this->vw             = NULL;
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

    if (this->vi!=NULL) ::free(this->vi);
    this->vi = NULL;
    if (this->vv!=NULL) ::free(this->vv);
    if (this->vn!=NULL) ::free(this->vn);
    if (this->vu!=NULL) ::free(this->vu);
    this->vv = NULL;
    this->vn = NULL;
    this->vu = NULL;

    if (this->vj!=NULL) ::free(this->vj);
    if (this->vw!=NULL) ::free(this->vw);
    this->vj = NULL;
    this->vw = NULL;

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
    this->bin_mode          = JBXL_GLTF_BIN_AOS;
    //this->bin_mode          = JBXL_GLTF_BIN_SOA;
    this->bin_seq           = false;
    //this->bin_seq           = true;

    this->gltf_name         = init_Buffer();
    this->alt_name          = init_Buffer();
    this->phantom_out       = true;
    this->no_offset         = false;
    this->glb_out           = false;
    this->center            = Vector<double>(0.0, 0.0, 0.0);

    this->has_joints        = false;
    this->joints_list       = NULL;

    this->forUnity          = true;
    this->forUE             = false;
    this->engine            = JBXL_3D_ENGINE_UNITY;

    this->image_list        = new_tList_anchor_node();
    this->material_list     = new_tList_anchor_node();

    this->affineTrans       = NULL;
    this->affineSkeleton.init();

    this->bin_buffer        = init_Buffer();
    this->matrix_buffer     = init_Buffer();
    this->bin_offset        = 0;
    this->matrix_offset     = 0;

    this->shell_no          = 0;
    this->node_no           = 0;
    this->mesh_no           = 0;
    this->mesh_prim_no      = 0;
    this->skin_no           = 0;
    this->view_no           = 0;
    this->accessor_no       = 0;
    this->material_no       = 0;
    this->image_no          = 0;

    this->num_joints         = 0;
    this->joint_offset      = 0;

    this->json_data         = NULL;

    this->scenes            = NULL;
    this->scenes_name       = NULL;
    this->scenes_nodes      = NULL;
    this->nodes             = NULL;
    this->meshes            = NULL;
    this->skins             = NULL;
    this->buffers           = NULL;
    this->buffviews         = NULL;
    this->accessors         = NULL;
    this->materials         = NULL;
    this->textures          = NULL;
    this->images            = NULL;

    this->nodes_children    = NULL;

    this->shellNode         = NULL;

    initGLTF();
}


void  GLTFData::free(void)
{
    free_Buffer(&(this->gltf_name));
    free_Buffer(&(this->alt_name));
    free_Buffer(&(this->matrix_buffer));
    free_Buffer(&(this->bin_buffer));

    del_tList(&(this->image_list));
    del_tList(&(this->material_list));
    del_json(&(this->json_data));

    if (this->shellNode!=NULL) {
        this->shellNode->delete_next();
        this->shellNode = NULL;
    }

    if (this->joints_list!=NULL) del_tList(&this->joints_list);
    this->joints_list = NULL;       // unnecessary. just to be sure

    this->affineSkeleton.free();
    this->delAffineTrans();
    this->affineTrans = NULL;       // unnecessary. just to be sure
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
    this->skins        = json_append_array_key(this->json_data, "skins");
    this->meshes       = json_append_array_key(this->json_data, "meshes");

    this->materials    = json_append_array_key(this->json_data, "materials");
    this->textures     = json_append_array_key(this->json_data, "textures");
    this->images       = json_append_array_key(this->json_data, "images");

    this->buffers      = json_append_array_key(this->json_data, "buffers");
    this->buffviews    = json_append_array_key(this->json_data, "bufferViews");
    this->accessors    = json_append_array_key(this->json_data, "accessors");
}


///////////////////////////////////////////////////////////////////////////////////
// Affine Transfer
//

/**
AffineTrans<double>  GLTFData::getAffineTrans4Engine(AffineTrans<double> affine)

使用するエンジンに合わせて，FACET毎の Affine変換のパラメータを変更する．
*/
/*
AffineTrans<double>  GLTFData::getAffineTrans4Engine(AffineTrans<double> affine)
{
    AffineTrans<double> trans;
    for (int i=1; i<=4; i++) trans.matrix.element(i, i) = 1.0;
    //
    if (this->engine==JBXL_3D_ENGINE_UE) {
        for (int i=1; i<=4; i++) trans.matrix.element(i, i) = 100.0;
    }
    else {
        trans.matrix.element(2, 2) =  0.0;
        trans.matrix.element(3, 3) =  0.0;
        trans.matrix.element(3, 2) = -1.0;    // y -> -z
        trans.matrix.element(2, 3) =  1.0;    // z -> y
    }
    //
    trans.affineMatrixFllow(affine);     // engineTrans = engineTrans * (*affine)
    return trans;
}
*/


AffineTrans<double>  GLTFData::getAffineBaseTrans4Engine(void)
{
    AffineTrans<double> trans;
    for (int i=1; i<=4; i++) trans.matrix.element(i, i) = 1.0;
    //
    if (this->engine==JBXL_3D_ENGINE_UE) {
        for (int i=1; i<=4; i++) trans.matrix.element(i, i) = 100.0;
    }
    else {
        trans.matrix.element(2, 2) =  0.0;
        trans.matrix.element(3, 3) =  0.0;
        trans.matrix.element(3, 2) = -1.0;    // y -> -z
        trans.matrix.element(2, 3) =  1.0;    // z -> y
    }
    //
    //trans.affineMatrixFllow(affine);     // engineTrans = engineTrans * (*affine)
    return trans;
}


/**
 UV Map を処理する
*/
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



//////////////////////////////////////////////////////////////////////////////////////////

/**
void  GLTFData::addShell(MeshObjectData* shelldata, bool collider, SkinJointData* skin_joint, tList* joints_connection)

この関数は SOLID毎に複数回呼ばれ，SOLIDに SHELLを追加する．

@param  shelldata    SHELLデータ．複数の FACETを含む．
@param  collider     コライダーのサポート
@param  skin_joint   ジョイントデータ．デフォルトは NULL
@param  joints_connection  ジョイントの連結情報．デフォルトは NULL
*/
void  GLTFData::addShell(MeshObjectData* shelldata, bool collider, SkinJointData* skin_joint, tList* joints_connection)
{
    if (shelldata==NULL) return;
    if (this->shell_no==0 && this->gltf_name.buf==NULL) {
        if (shelldata->data_name.buf!=NULL) this->gltf_name = dup_Buffer(shelldata->data_name);
        else                                this->gltf_name = make_Buffer_bystr("Object");
    }
    if (shelldata->alt_name.buf!=NULL) {
        this->alt_name = dup_Buffer(shelldata->alt_name);
    }

    if (skin_joint!=NULL && joints_connection!=NULL) {
        if (this->joints_list==NULL && !this->has_joints) {
            this->joints_list = joints_connection;
            this->has_joints = true;
            this->num_joints = skin_joint->num_joints;
        }
        else {
            if (joints_connection!=NULL) del_tList(&joints_connection);
            joints_connection = NULL;
        }
    }

    MeshFacetNode* facet = shelldata->facet;
    int shell_indexes  = 0;
    int shell_vertexes = 0;
    while (facet!=NULL) {
        shell_indexes  += facet->num_index;
        shell_vertexes += facet->num_vertex;
        facet = facet->next;
    }

    AffineTrans<double>* affine = shelldata->affineTrans;
    if (no_offset && affine!=NULL) {
        if (this->shell_no==0) {
            this->center = affine->shift;
        }
        affine->shift = affine->shift - this->center;
        affine->computeMatrix();
    }
    facet = shelldata->facet;

    //
    this->addScenes();
    if (this->node_no==0 && this->has_joints) {
        this->addSkeletonNodes(skin_joint, affine);
    }
    this->addNodes(affine);

    this->addTextures(facet);
    this->addMaterials(facet);

    this->addMeshes(facet);
    this->execAffineUVMap(facet, affine);

    // for bin data
    if (this->bin_mode==JBXL_GLTF_BIN_AOS) {
        this->addBufferViewsAoS(facet);
        this->addAccessorsAoS(facet);
    }
    else {
        this->addBufferViewsSoA(facet);
        this->addAccessorsSoA(facet);
    }

    if (this->has_joints) {
        //this->addSkins(this->joint_offset);
        this->addBufferViewsIBM();
        this->addAccessorsIBM(); 
        //this->createInverseBindMatrix(skin_joint);
        del_json_node(&this->skins);
    }
    else {
        del_json_node(&this->skins);
    }

    // BINデータ作成
    if (this->bin_seq) {
        // その都度 BINデータを作成していく
        if (this->bin_mode==JBXL_GLTF_BIN_AOS) {
            this->createBinDataSeqAoS(facet, shell_indexes, shell_vertexes);
        }
        else {
            this->createBinDataSeqSoA(facet, shell_indexes, shell_vertexes);
        }
    }
    else {
        // データを一旦 GLTFShellNodeに保存．最後に closeSolid() で一気に BINデータを作成
        this->createShellGeoData(facet, shell_indexes, shell_vertexes, skin_joint);
    }

    //
    this->phantom_out = true;
    if (collider) {
        this->phantom_out = false;
    }
    //
    free_Buffer(&this->alt_name);
    this->shell_no++;
    return;
}


void  GLTFData::addScenes(void)
{
    if (this->node_no==0) {
        json_set_str_val(this->scenes_name, (char*)this->gltf_name.buf);
        // skeleton structure
        if (this->has_joints) {
            json_append_array_int_val(this->scenes_nodes, 0);
            this->joint_offset = 2;
        }
    }

    if (!this->has_joints) {
        json_append_array_int_val(this->scenes_nodes, this->node_no);
    }

    return;
}


void  GLTFData::addNodes(AffineTrans<double>* affine)
{
    char buf[LBUF];

    // nodes
    Buffer node_name = init_Buffer();
    if (this->alt_name.buf!=NULL) {
        node_name = dup_Buffer(this->alt_name);
    }
    else {
        node_name = make_Buffer_bystr("Node_#");
        cat_i2Buffer(this->node_no, &node_name);
    }
    memset(buf, 0, LBUF);
    snprintf(buf, LBUF-1, JBXL_GLTF_NODES_MESH, (char*)node_name.buf, this->shell_no);
    tJson* mesh = json_insert_parse(this->nodes, buf);
    free_Buffer(&node_name);

    if (this->has_joints) {
        // skins
        //memset(buf, 0, LBUF);
        //snprintf(buf, LBUF-1, JBXL_GLTF_NODES_SKIN, this->skin_no);
        //json_insert_parse(mesh, buf);
    }
    //else {
        // affine translarion
        tJson* matrix = json_append_array_key(mesh, "matrix");
        if (affine!=NULL) {
            AffineTrans<double> trans = this->getAffineBaseTrans4Engine();
            trans.affineMatrixFllow(*affine);

            for (int j=1; j<=4; j++) {
                for (int i=1; i<=4; i++) {
                    float element = (float)trans.matrix.element(i, j);
                    json_append_array_real_val(matrix, element);
                }
            }
            trans.free();
        }
    //}

    json_append_array_int_val(this->nodes_children, this->node_no);
    this->node_no++;

    return;
}


void  GLTFData::addSkeletonNodes(SkinJointData* skin_joint, AffineTrans<double>* affine)
{
    char buf[LBUF];
    memset(buf, 0, LBUF);

    // Root node of model
    tJson* nodes_root = json_insert_parse(this->nodes, JBXL_GLTF_NODES_ROOT);
    this->nodes_children = json_append_array_key(nodes_root, "children");
    json_append_array_int_val(this->nodes_children, this->joint_offset-1);
    this->node_no++;

/*
    tJson* root_matrix = json_append_array_key(nodes_root, "matrix");
    if (affine!=NULL) {
        AffineTrans<double> trans = this->getAffineBaseTrans4Engine();
        trans.affineMatrixFllow(*affine);
        for (int j=1; j<=4; j++) {
            for (int i=1; i<=4; i++) {
                float element = (float)trans.matrix.element(i, j);
                json_append_array_real_val(root_matrix, element);
            }
        }
        trans.free();
    }
*/


    // Root node of skeletons
    tJson* skeleton_root = json_insert_parse(this->nodes, JBXL_GLTF_NODES_AVATAR);
    tJson* skeleton_children = json_append_array_key(skeleton_root, "children");
    json_append_array_int_val(skeleton_children, this->joint_offset);
    this->node_no++;





    // skeleton の位置合わせ
    Vector<double> pelvis = Vector<double>(0.0, 0.0, 1.067);                // ほぼ定数だが，一応データから....
    pelvis.x = skin_joint->alt_inverse_bind[0].matrix.element(1, 4);        // 0 means mPelvis
    pelvis.y = skin_joint->alt_inverse_bind[0].matrix.element(2, 4);
    pelvis.z = skin_joint->alt_inverse_bind[0].matrix.element(3, 4);

    AffineTrans<double> joint_space = skin_joint->inverse_bind[0] * skin_joint->bind_shape;     // 0 means mPelvis
    AffineTrans<double> joint_trans = joint_space.getInvAffine();
    Vector<double> shift = joint_trans.execRotateScale(pelvis);
    joint_trans.shift = joint_trans.shift - shift;
    this->affineSkeleton = (*affine)*joint_trans;      // Joint -> Real の Affine変換
    joint_space.free();
    joint_trans.free();

    if (this->no_offset) this->affineSkeleton.shift = this->affineSkeleton.shift - affine->shift;
    this->affineSkeleton.computeMatrix();

    AffineTrans<double> trans = this->getAffineBaseTrans4Engine();
    trans.affineMatrixFllow(this->affineSkeleton);

    tJson* skeleton_matrix  = json_append_array_key(skeleton_root, "matrix");
    for (int j=1; j<=4; j++) {
        for (int i=1; i<=4; i++) {
            json_append_array_real_val(skeleton_matrix, trans.matrix.element(i,j));
        }
    }
    trans.free();


    //
    tList* jl = this->joints_list;
    if (jl!=NULL) jl = jl->next;     // top is Json ACKHOR
    tList* jt = jl;
    
    int jnt = 0;
    while (jl!=NULL) {
        // order check!
        if (strncmp((char*)jl->ldat.val.buf+1, skin_joint->joint_names.get_value(jnt), strlen(skin_joint->joint_names.get_value(jnt)))) {
            PRINT_MESG("GLTFData::addSkeletonNodes: ERROR: Joint is not in the correct order. %s != \"%s\"\n", (char*)jl->ldat.val.buf, skin_joint->joint_names.get_value(jnt));
        }
        //
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_NODES_SKLTN, (char*)jl->ldat.val.buf);
        tJson* skltn = json_insert_parse(this->nodes, buf);
        tJson* children = json_append_array_key(skltn, "children");
        //
        int count = 0;
        tList* jp = jt; // top 
        while (jp!=NULL) {
            if (jp->ldat.lv==jl->ldat.id) {
                json_append_array_int_val(children, jp->ldat.id + this->joint_offset);
                count++;
            }
            jp = jp->next;
        }
        if (count==0) {
            del_json_node(&children);
        }

        skin_joint->alt_inverse_bind[jnt].computeComponents();
        tJson* shift = json_append_array_key(skltn, "translation");
        json_append_array_real_val(shift, skin_joint->alt_inverse_bind[jnt].shift.x);
        json_append_array_real_val(shift, skin_joint->alt_inverse_bind[jnt].shift.y);
        json_append_array_real_val(shift, skin_joint->alt_inverse_bind[jnt].shift.z);

/*
        if (jnt==0) { 
           tJson* rotate = json_append_array_key(skltn, "rotation");
            json_append_array_real_val(rotate, trans.rotate.x);
            json_append_array_real_val(rotate, trans.rotate.y);
            json_append_array_real_val(rotate, trans.rotate.z);
            json_append_array_real_val(rotate, trans.rotate.s);

            tJson* scale = json_append_array_key(skltn, "scale");
            json_append_array_real_val(scale, trans.scale.x);
            json_append_array_real_val(scale, trans.scale.y);
            json_append_array_real_val(scale, trans.scale.z);
        }
        tJson* matrix = json_append_array_key(skltn, "matrix");
        for (int j=1; j<=4; j++) {
            for (int i=1; i<=4; i++) {
                double element = skin_joint->alt_inverse_bind[jnt].matrix.element(i, j);
                json_append_array_real_val(matrix, element);
            }
        }
        if (jl->ldat.ptr!=NULL) {
            tJson* transl = json_append_array_key(skltn, "translation");
            Vector<float>* vec = (Vector<float>*)(jl->ldat.ptr);
            json_append_array_real_val(transl, vec->x);
            json_append_array_real_val(transl, vec->y);
            json_append_array_real_val(transl, vec->z);
        }
*/
        this->node_no++;
        jl = jl->next;
        jnt++;
    }
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
                snprintf(buf, LBUF-1, JBXL_GLTF_IMAGES, image_name);
                json_insert_parse(this->images, buf);

                // textures
                memset(buf, 0, LBUF);
                snprintf(buf, LBUF-1, JBXL_GLTF_TEXTURES, this->image_no);
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
                //snprintf(buf, LBUF-1, JBXL_GLTF_MTRLS, material_name, 1.0, 1.0, 1.0, 1.0, img_no);
                snprintf(buf, LBUF-1, JBXL_GLTF_MTLS_NAME_PBR, material_name, img_no);
                json_insert_parse(this->materials, buf);
                tJson* pbr = search_key_json(this->materials, "pbrMetallicRoughness", FALSE, this->material_no + 1);
                if (pbr!=NULL) {
                    this->addMaterialParameters(pbr, facet);
                }
                this->material_no++;
            }
        }
        facet = facet->next;
    }
    return;
}


/**
void  GLTFData::addMaterialParameters(tJson* pbr, MeshFacetNode* facet)

glTF-2.0.html
    5.19. Material
*/
void  GLTFData::addMaterialParameters(tJson* pbr, MeshFacetNode* facet)
{
    char buf[LBUF];

    MaterialParam param  = facet->material_param;
    TextureParam texture = param.texture;

    char kind_obj   = param.getKind();
    bool hasAlpha   = texture.hasAlphaChannel();
    int  alpha_mode = texture.getAlphaMode();

    float red       = (float)texture.getColor(0);
    float green     = (float)texture.getColor(1);
    float blue      = (float)texture.getColor(2);
    float transp    = (float)texture.getColor(3);
    memset(buf, 0, LBUF);
    snprintf(buf, LBUF-1, JBXL_GLTF_MTLS_BCOLORF, red, green, blue, transp);
    json_insert_parse(pbr, buf);

    if (kind_obj!='E') {        // !Earth
        float shininess = (float)param.getShininess();
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_MTLS_ROUGHF, 1.0f - shininess);
        json_insert_parse(pbr, buf);
    }

    /*
    float glossiness = (float)param.getGlossiness();
    memset(buf, 0, LBUF);
    snprintf(buf, LBUF-1, JBXL_GLTF_MTLS_ROUGHF, 1.0f - glossiness);
    json_insert_parse(pbr, buf);
    */
    //
    //bright(0 or 1), light(?)

    if (kind_obj=='T' || kind_obj=='G') {       // Tree and Grass
        json_insert_parse(pbr->prev, "{\"alphaMode\":\"BLEND\"}");
    }
    else if (alpha_mode==MATERIAL_ALPHA_NONE) {
        json_insert_parse(pbr->prev, "{\"alphaMode\":\"OPAQUE\"}");
    }
    else if (alpha_mode==MATERIAL_ALPHA_BLENDING && hasAlpha) {
        json_insert_parse(pbr->prev, "{\"alphaMode\":\"BLEND\"}");
    }
    else if (alpha_mode==MATERIAL_ALPHA_MASKING && hasAlpha) {
        json_insert_parse(pbr->prev, "{\"alphaMode\":\"MASK\"}");
        float cutoff = (float)texture.getAlphaCutoff();
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_MTLS_CUTOFF, cutoff);
        json_insert_parse(pbr->prev, buf);
    }
    else if (alpha_mode==MATERIAL_ALPHA_EMISSIVE) {
        //
    }
    else {
        json_insert_parse(pbr->prev, "{\"alphaMode\":\"OPAQUE\"}");
    }

    float glow = (float)param.getGlow();
    if (glow>0.0) {
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_MTLS_EMISSIVE, glow*red, glow*green, glow*blue);
        json_insert_parse(pbr->prev, buf);
    }

    //param.printParam(stderr);
    return;
}


void  GLTFData::addMeshes(MeshFacetNode* facet)
{
    if (facet==NULL) return;
    char buf[LBUF];
    int accessor_no = 0;

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
        if (this->has_joints) {
            accessor_no = this->mesh_prim_no;
            snprintf(buf, LBUF-1, JBXL_GLTF_MESHES_PRIM_J, accessor_no, accessor_no+1, accessor_no+2, accessor_no+3, accessor_no+4, accessor_no+5, mtl_no);
            this->mesh_prim_no += 6;
        }
        else {
            accessor_no = this->mesh_prim_no;
            snprintf(buf, LBUF-1, JBXL_GLTF_MESHES_PRIM, accessor_no, accessor_no+1, accessor_no+2, accessor_no+3, mtl_no);
            this->mesh_prim_no += 4;
        }
        json_insert_parse(primitives, buf);

        this->mesh_no++;
        facet = facet->next;
    }
    return;
}


void  GLTFData::addSkins(int joint_offset)
{
    if (!this->has_joints) return;
    char buf[LBUF];
    
    memset(buf, 0, LBUF);
    snprintf(buf, LBUF-1, JBXL_GLTF_SKINS, this->accessor_no, joint_offset);
    tJson* skn = json_insert_parse(skins, buf);
    tJson* jnt = json_append_array_key(skn, "joints");
    for (int j=0; j<this->num_joints; j++) {
        json_append_array_int_val(jnt, j + joint_offset);
        //json_append_array_int_val(jnt, j);
    }
    //json_append_array_int_val(jnt, joint_offset);
    this->skin_no++;
    return;
}


void  GLTFData::closeSolid(void)
{
    //
    if (this->bin_buffer.buf==NULL && !this->bin_seq) {
        if (this->bin_mode==JBXL_GLTF_BIN_AOS) createBinDataAoS();
        else                                   createBinDataSoA();
    }

    return;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
// I: Index    unsigned int x 1
// V: Vertex   float        x 3
// N: Normal   float        x 3
// U: UVMap    float        x 2
// J: Joint    unsigned int x 4
// W: Weight   float        x 4
// M: Matrix   float        x 16
// n: Index Number
// m: Vertex Number


///////////////////////////////////////////////////////////////////////
// AoS

/**
AoS
    I(1)xn, {V(3), N(3), U(3), J(4), W(4)}xm ....... facet
    I(1)xn, {V(3), N(3), U(3), J(4), W(4)}xm ....... facet
    ......................................................
    I(1)xn, {V(3), N(3), U(3), J(4), W(4)}xm ....... facet

*/
void  GLTFData::addBufferViewsAoS(MeshFacetNode* facet)
{
    if (facet==NULL) return;

    char buf[LBUF];
    unsigned int float_size  = (unsigned int)sizeof(float);
    unsigned int uint_size   = (unsigned int)sizeof(unsigned int);
    unsigned int shortu_size = (unsigned int)sizeof(short unsigned int);

    unsigned int v_stride = float_size*8U;                                                      // V(3), N(3), U(2)
    unsigned int j_stride = v_stride + shortu_size*4U + float_size*4U;                          // V(3), N(3), u(2) + J(4), W(4)

    while (facet!=NULL) {
        // bufferview of indexies
        unsigned int i_length = (unsigned int)facet->num_index*uint_size;                       // I(1) x n
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS_ELEMENT, 0, this->bin_offset, i_length);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += i_length;

        if (this->has_joints){
            // bufferview of vertex/normal/uvmap/weighted joints/weight
            unsigned int j_length = (unsigned int)facet->num_vertex*j_stride;                   // (V, N, U, J, W) x n
            memset(buf, 0, LBUF);
            snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS, 0, this->bin_offset, j_length, j_stride);
            json_insert_parse(this->buffviews, buf);
            this->bin_offset += j_length;
        }
        else {
            // bufferview of vertex/normal/uvmap
            unsigned int v_length = (unsigned int)facet->num_vertex*v_stride;                   // (V, N, U) x n
            memset(buf, 0, LBUF);
            snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS, 0, this->bin_offset, v_length, v_stride);
            json_insert_parse(this->buffviews, buf);
            this->bin_offset += v_length;
        }
        facet = facet->next;
    }
    return;
}


/**
AoS
    I(1)xn, {V(3), N(3), U(3), J(4), W(4)}xm ....... facet
    I(1)xn, {V(3), N(3), U(3), J(4), W(4)}xm ....... facet
    ......................................................
    I(1)xn, {V(3), N(3), U(3), J(4), W(4)}xm ....... facet

*/
void  GLTFData::addAccessorsAoS(MeshFacetNode* facet)
{
    if (facet==NULL) return;
    char buf[LBUF];

    unsigned int float_size  = (unsigned int)sizeof(float);
    unsigned int shortu_size = (unsigned int)sizeof(short unsigned int);

    while (facet!=NULL) {
        gltfFacetMinMax mm = getFacetMinMax(facet);

        // accessor of indexies
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS, this->view_no, 0U, 5125, facet->num_index, "SCALAR");            // 5125: unsigned int
        json_insert_parse(this->accessors, buf);
        this->view_no++;
        this->accessor_no++;

        // accessors of vertex/normal/uvmap
        unsigned int offset = 0;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS_V3, this->view_no, 0U, 5126, facet->num_vertex, "VEC3",
                 mm.vertex_x_max, mm.vertex_y_max, mm.vertex_z_max, mm.vertex_x_min, mm.vertex_y_min, mm.vertex_z_min);
        json_insert_parse(this->accessors, buf);
        offset += float_size*3U;
        this->accessor_no++;
        //
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS, this->view_no, offset, 5126, facet->num_vertex, "VEC3");         // 5126: float
        json_insert_parse(this->accessors, buf);
        offset += float_size*3U;
        this->accessor_no++;
        //
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS, this->view_no, offset, 5126, facet->num_vertex, "VEC2");
        json_insert_parse(this->accessors, buf);
        offset += float_size*2U;
        this->accessor_no++;
        //
        if (this->has_joints) {
            // weighted joints
            memset(buf, 0, LBUF);
            snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS, this->view_no, offset, 5123, facet->num_vertex, "VEC4");     // 5123: short unsigned int
            json_insert_parse(this->accessors, buf);
            offset += shortu_size*4U;
            this->accessor_no++;
            // weight value
            memset(buf, 0, LBUF);
            snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS, this->view_no, offset, 5126, facet->num_vertex, "VEC4");
            json_insert_parse(this->accessors, buf);
            offset += float_size*4U;
            this->accessor_no++;
        }
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

    unsigned int float_size  = (unsigned int)sizeof(float);
    unsigned int uint_size   = (unsigned int)sizeof(unsigned int);
    unsigned int shortu_size = (unsigned int)sizeof(short unsigned int);

    unsigned int j_length   = shortu_size*4U;
    unsigned int w_length   = float_size*4U;

    unsigned int temp_len = (unsigned int)shell_indexes*uint_size + (unsigned int)shell_vertexes*float_size*8U;     // I, V, N, U of the Shell
    if (this->has_joints) {
        temp_len += (unsigned int)shell_vertexes*(j_length + w_length);      // + J, W of the Shell
    }
    unsigned char* temp_buffer = (unsigned char*)malloc(temp_len);
    if (temp_buffer==NULL) {
        PRINT_MESG("GLTFData::createBinDataSeqAoS: ERROR: temp_buffer. no more memory.\n");
        return;
    }
    //
    if (this->bin_buffer.buf==NULL) {
        this->bin_buffer = make_Buffer(temp_len);
        if (this->bin_buffer.bufsz<=0) {
            PRINT_MESG("GLTFData::createBinDataSeqAoS: ERROR: bin_buffer. no more memory.\n");
            ::free(temp_buffer);
            return;
        }
    }
    //
    unsigned int offset   = 0U;

    while (facet!=NULL) {
        // binary of indexies
        unsigned int i_length = (unsigned int)facet->num_index*uint_size;
        memcpy((void*)(temp_buffer + offset), (void*)facet->data_index, i_length);
        offset += i_length;

        // binary of vertex/normal/uvmap
        float temp;
        for (int i=0; i<facet->num_vertex; i++) {       // x n
            temp = (float)facet->vertex_value[i].x;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->vertex_value[i].y;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->vertex_value[i].z;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->normal_value[i].x;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->normal_value[i].y;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->normal_value[i].z;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->texcrd_value[i].u;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = 1.0f - (float)facet->texcrd_value[i].v;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;

            if (this->has_joints) {
                unsigned int total = 0;
                unsigned int jnum  = (unsigned int)facet->weight_value[i].get_size();
                int wcount = 0;
                for (unsigned int j=0; j<jnum; j++) {
                    unsigned int w = (unsigned int)facet->weight_value[i].get_value(j);
                    if (w!=0) wcount++;
                    total += w;
                }
                if (wcount>4) PRINT_MESG("GLTFData::createBinDataSeqAoS: WARNING: more than 4 weighted joints (%d)\n", wcount);

                short unsigned int weight_index[4];
                float              weight_value[4];
                memset(weight_index, 0, j_length);
                memset(weight_value, 0, w_length);

                if (total!=0) {
                    unsigned int jcnt = 0;
                    for (unsigned int j=0; j<jnum; j++) {
                        unsigned int w = (unsigned int)facet->weight_value[i].get_value(j);
                        if (w!=0) {
                            weight_index[jcnt] = (short unsigned int)(j);
                            weight_value[jcnt] = (float)w/(float)total;
                            jcnt++;
                            if (jcnt>=4) break;
                        }
                    }
                }
                memcpy((void*)(temp_buffer + offset), (void*)weight_index, j_length);
                offset += j_length;
                memcpy((void*)(temp_buffer + offset), (void*)weight_value, w_length);
                offset += w_length;
            }
        }
        facet = facet->next;
    }
    //
    cat_b2Buffer(temp_buffer, &(this->bin_buffer), temp_len);
    ::free(temp_buffer);
    return;
}



///////////////////////////////////////////////////////////////////////
// SoA

/**
SoA
    Ixn, Vxm, Nxm, Uxm, Jxm ,Wxm ...... facet
    Ixn, Vxm, Nxm, Uxm, Jxm ,Wxm ...... facet
    .........................................
    Ixn, Vxm, Nxm, Uxm, Jxm ,Wxm ...... facet

*/
void  GLTFData::addBufferViewsSoA(MeshFacetNode* facet)
{
    if (facet==NULL) return;
    char buf[LBUF];

    unsigned int length = 0;
    unsigned int float_size  = (unsigned int)sizeof(float);
    unsigned int uint_size   = (unsigned int)sizeof(unsigned int);
    unsigned int shortu_size = (unsigned int)sizeof(short unsigned int);

    while (facet!=NULL) {
        // bufferview of indexies
        length = (unsigned int)facet->num_index*uint_size;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS_ELEMENT, 0, this->bin_offset, length);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        // bufferview of vertex
        length = (unsigned int)facet->num_vertex*float_size*3U;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS, 0, this->bin_offset, length, float_size*3U);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        // bufferview of normal
        length = (unsigned int)facet->num_vertex*float_size*3U;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS, 0, this->bin_offset, length, float_size*3U);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        // bufferview of uvmap
        length = (unsigned int)facet->num_vertex*float_size*2U;
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS, 0, this->bin_offset, length, float_size*2U);
        json_insert_parse(this->buffviews, buf);
        this->bin_offset += length;

        if (this->has_joints) {
            // weighted joint number 
            length = (unsigned int)facet->num_vertex*shortu_size*4U;
            memset(buf, 0, LBUF);
            snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS, 0, this->bin_offset, length, shortu_size*4U);
            json_insert_parse(this->buffviews, buf);
            this->bin_offset += length;

            // weight value
            length = (unsigned int)facet->num_vertex*float_size*4U;
            memset(buf, 0, LBUF);
            snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS, 0, this->bin_offset, length, float_size*4U);
            json_insert_parse(this->buffviews, buf);
            this->bin_offset += length;
        }
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
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS, this->view_no, 0U, 5125, facet->num_index, "SCALAR");        // 5125: unsigned int
        json_insert_parse(this->accessors, buf);
        this->view_no++;
        this->accessor_no++;

        // accessors of vertex/normal/uvmap
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS_V3, this->view_no, 0U, 5126, facet->num_vertex, "VEC3",
                 mm.vertex_x_max, mm.vertex_y_max, mm.vertex_z_max, mm.vertex_x_min, mm.vertex_y_min, mm.vertex_z_min);
        json_insert_parse(this->accessors, buf);
        this->view_no++;
        this->accessor_no++;
        //
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS, this->view_no, 0U, 5126, facet->num_vertex, "VEC3");         // 5126: floaat
        json_insert_parse(this->accessors, buf);
        this->view_no++;
        this->accessor_no++;
        //
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS, this->view_no, 0U, 5126, facet->num_vertex, "VEC2");
        json_insert_parse(this->accessors, buf);
        this->view_no++;
        this->accessor_no++;
        
        if (this->has_joints) {
            // weighted joints
            memset(buf, 0, LBUF);
            snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS,  this->view_no, 0U, 5123, facet->num_vertex, "VEC4");    // 5123: short unsigned int
            json_insert_parse(this->accessors, buf);
            this->view_no++;
            this->accessor_no++;
            // weight value
            memset(buf, 0, LBUF);
            snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS,  this->view_no, 0U, 5126, facet->num_vertex, "VEC4");
            json_insert_parse(this->accessors, buf);
            this->view_no++;
            this->accessor_no++;
        }

        facet = facet->next;
    }
    return;
}


/**
void  GLTFData::createBinDataSeqSoA(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)

GLTFの Geometoryデータを SoA形式で 逐次作成し，this->bin_bufferに追加していく．
最終的に作成された this->bin_buffer はそのまま出力できる． 
this->shellNode は使用しない．

SoA
    Ixn, Vxm, Nxm, Uxm, Jxm ,Wxm ...... facet
    Ixn, Vxm, Nxm, Uxm, Jxm ,Wxm ...... facet
    .........................................
    Ixn, Vxm, Nxm, Uxm, Jxm ,Wxm ...... facet

*/
void  GLTFData::createBinDataSeqSoA(MeshFacetNode* facet, int shell_indexes, int shell_vertexes)
{
    if (facet==NULL) return;

    unsigned int float_size  = (unsigned int)sizeof(float);
    unsigned int uint_size   = (unsigned int)sizeof(unsigned int);
    unsigned int shortu_size = (unsigned int)sizeof(short unsigned int);

    unsigned int j_length = shortu_size*4U;
    unsigned int w_length = float_size*4U;

    unsigned int temp_len = (unsigned int)shell_indexes*uint_size + (unsigned int)shell_vertexes*float_size*8U;     // I, V, N, U of the Shell
    if (this->has_joints) {
        temp_len += (unsigned int)shell_vertexes*(j_length + w_length);      // + J, W of the Shell
    }
    unsigned char* temp_buffer = (unsigned char*)malloc(temp_len);
    if (temp_buffer==NULL) {
        PRINT_MESG("GLTFData::createBinDataSeqSoA: ERROR: temp_buffer. no more memory.\n");
        return;
    }
    //
    if (this->bin_buffer.buf==NULL) {
        this->bin_buffer = make_Buffer(temp_len);
        if (this->bin_buffer.bufsz<=0) {
            PRINT_MESG("GLTFData::createBinDataSeqSoA: ERROR: bin_buffer. no more memory.\n");
            ::free(temp_buffer);
            return;
        }
    }
    //
    unsigned int offset   = 0U;

    while (facet!=NULL) {
        // binary of indexies
        unsigned int i_length = (unsigned int)facet->num_index*sizeof(unsigned int);
        memcpy((void*)(temp_buffer + offset), (void*)facet->data_index, i_length);
        offset += i_length;

        // binary of vertex/normal/uvmap
        float temp;
        for (int i=0; i<facet->num_vertex; i++) {       // x n
            temp = (float)facet->vertex_value[i].x;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->vertex_value[i].y;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->vertex_value[i].z;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
        }

        for (int i=0; i<facet->num_vertex; i++) {
            temp = (float)facet->normal_value[i].x;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->normal_value[i].y;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = (float)facet->normal_value[i].z;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
        }

        for (int i=0; i<facet->num_texcrd; i++) {
            temp = (float)facet->texcrd_value[i].u;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
            temp = 1.0f - (float)facet->texcrd_value[i].v;
            memcpy((void*)(temp_buffer + offset), (void*)&temp, float_size);
            offset += float_size;
        }

        if (this->has_joints) {
            // weighted joints
            for (int i=0; i<facet->num_vertex; i++) {
                unsigned int total = 0;
                unsigned int jnum  = (unsigned int)facet->weight_value[i].get_size();
                int wcount = 0;
                for (unsigned int j=0; j<jnum; j++) {
                    unsigned int w =  (unsigned int)facet->weight_value[i].get_value(j);
                    if (w!=0) wcount++;
                    total += w;
                }
                if (wcount>4) PRINT_MESG("GLTFData::createBinDataSeqSoA: WARNING: more than 4 weighted joints (%d)\n", wcount);

                short unsigned int weight_index[4];
                memset(weight_index, 0, j_length);

                if (total!=0) {
                    unsigned int jcnt = 0;
                    for (unsigned int j=0; j<jnum; j++) {
                        unsigned int w = (unsigned int)facet->weight_value[i].get_value(j);
                        if (w!=0) {
                            weight_index[jcnt] = (short unsigned int)(j);
                            jcnt++;
                            if (jcnt>=4) break;
                        }
                    }
                }
                memcpy((void*)(temp_buffer + offset), (void*)weight_index, j_length);
                offset += j_length;
            }
            // weight value
            for (int i=0; i<facet->num_vertex; i++) {
                unsigned int total = 0;
                unsigned int jnum  = (unsigned int)facet->weight_value[i].get_size();
                for (unsigned int j=0; j<jnum; j++) {
                    total += (unsigned int)facet->weight_value[i].get_value(j);
                }
                float weight_value[4];
                memset(weight_value, 0, w_length);

                if (total!=0) {
                    unsigned int jcnt = 0;
                    for (unsigned int j=0; j<jnum; j++) {
                        unsigned int w = (unsigned int)facet->weight_value[i].get_value(j);
                        if (w!=0) {
                            weight_value[jcnt] = (float)w/(float)total;
                            jcnt++;
                            if (jcnt>=4) break;
                        }
                    }
                }
                memcpy((void*)(temp_buffer + offset), (void*)weight_value, w_length);
                offset += w_length;
            }
        }

        facet = facet->next;
    }
    //
    cat_b2Buffer(temp_buffer, &(this->bin_buffer), temp_len);
    ::free(temp_buffer);

    return;
}



/////////////////////////////////////////////////////////////////////////////////////
// Create all bin data at once

/**
void  GLTFData::createShellGeoData(MeshFacetNode* facet, int shell_indexes, int shell_vertexes, SkinJointData* skin_joint)

SHELL毎に呼び出され，SHELL中の全FACETのジオメトリ情報を this->shellNode に格納する．

@param  facet           SHELL中の FACETの先頭データ．
@param  shell_indexes   SHELL中の indexデータの総数．
@param  shell_vetexes   SHELL中の vertexデータの総数．
@param  skin_joint      SHELLの Joint情報へのポインタ．
*/
void  GLTFData::createShellGeoData(MeshFacetNode* facet, int shell_indexes, int shell_vertexes, SkinJointData* skin_joint)
{
    unsigned int float_size  = (unsigned int)sizeof(float);
    unsigned int uint_size   = (unsigned int)sizeof(unsigned int);
    unsigned int shortu_size = (unsigned int)sizeof(short unsigned int);

    unsigned int j_length = shortu_size*4U;
    unsigned int w_length = float_size*4U;

    GLTFShellNode* shell_node  = new GLTFShellNode();
    shell_node->shell_indexes  = shell_indexes;
    shell_node->shell_vertexes = shell_vertexes;

    shell_node->vi = (unsigned int*) malloc(uint_size*shell_indexes);
    shell_node->vv = (Vector<float>*)malloc(sizeof(Vector<float>)*shell_vertexes);
    shell_node->vn = (Vector<float>*)malloc(sizeof(Vector<float>)*shell_vertexes);
    shell_node->vu = (UVMap<float>* )malloc(sizeof(UVMap<float> )*shell_vertexes);
    if (this->has_joints) {
        shell_node->vj = (Vector4<short unsigned int>*)malloc(sizeof(Vector4<short unsigned int>)*shell_vertexes);
        shell_node->vw = (Vector4<float>*)malloc(sizeof(Vector4<float>)*shell_vertexes);
        shell_node->vm = (float*)malloc(float_size*16U*this->num_joints);
    }
    //
    if (shell_node->vi==NULL || shell_node->vv==NULL || shell_node->vn==NULL || shell_node->vu==NULL) {
        delete(shell_node);
        return;
    }
    if (this->has_joints) {
        if (shell_node->vj==NULL || shell_node->vw==NULL || shell_node->vm==NULL) {
            delete(shell_node);
            return;
        }
    }

    //
    MeshFacetNode* temp_facet = facet;
    while (temp_facet!=NULL) {
        shell_node->num_facets++;        // この SHELLにある FACETの数を数える
        temp_facet = temp_facet->next;
    }
    shell_node->facet_index  = (unsigned int*)malloc(shell_node->num_facets*uint_size);
    shell_node->facet_vertex = (unsigned int*)malloc(shell_node->num_facets*uint_size);

    unsigned int index_offset  = 0;
    unsigned int vertex_offset = 0;
    unsigned int facet_no      = 0;

    while (facet!=NULL) {
        // save index and vertex number
        shell_node->facet_index [facet_no] = facet->num_index;
        shell_node->facet_vertex[facet_no] = facet->num_vertex;
        facet_no++;

        // indexies
        for (int i=0; i<facet->num_index; i++) {
            shell_node->vi[index_offset + i] = facet->data_index[i];
        }
        index_offset += facet->num_index;

        // vertex/normal/uvmap
        for (int i=0; i<facet->num_vertex; i++) {
            shell_node->vv[vertex_offset + i]   = facet->vertex_value[i];
            shell_node->vn[vertex_offset + i]   = facet->normal_value[i];
            shell_node->vu[vertex_offset + i].u = facet->texcrd_value[i].u;
            shell_node->vu[vertex_offset + i].v = 1.0f - facet->texcrd_value[i].v;
        }

        if (this->has_joints) {
            for (int i=0; i<facet->num_vertex; i++) {
                unsigned int total = 0;
                unsigned int jnum  = (unsigned int)facet->weight_value[i].get_size();
                int wcount = 0;
                for (unsigned int j=0; j<jnum; j++) {
                    unsigned int w = (unsigned int)facet->weight_value[i].get_value(j);
                    if (w>0) wcount++;
                    total += w;
                }
                if (wcount>4) PRINT_MESG("GLTFData::createShellGeoData: WARNING: more than 4 weighted joints (%d)\n", wcount);
                //
                short unsigned int weight_index[4];
                float              weight_value[4];
                memset(weight_index, 0, j_length);
                memset(weight_value, 0, w_length);
                if (total!=0) {
                    unsigned int jcnt = 0;
                    for (unsigned int j=0; j<jnum; j++) {
                        unsigned int w = (unsigned int)facet->weight_value[i].get_value(j);
                        if (w!=0) {
                            weight_index[jcnt] = (short unsigned int)(j);
                            weight_value[jcnt] = (float)w/(float)total;
                            jcnt++;
                            if (jcnt>=4) break;
                        }
                    }
                }
                for (int j=0; j<4; j++) {
                    shell_node->vj[vertex_offset + i].element(j+1) = weight_index[j];
                    shell_node->vw[vertex_offset + i].element(j+1) = weight_value[j];
                }
            } 
        }
        vertex_offset += facet->num_vertex;

        facet = facet->next;
    }
    
    // Inverse Bind Matrix
    for (int k=0; k<this->num_joints; k++) {
        int kz = k*16; 
        for (int j=1; j<=4; j++) {
            int jz = (j-1)*4;
            for (int i=1; i<=4; i++) {
                shell_node->vm[kz + jz + i - 1] = (float)skin_joint->inverse_bind[k].matrix.element(i, j);
            }
        }
    } 

    // shell_node をリストの最後に繋げる
    GLTFShellNode* prv = NULL;
    GLTFShellNode* ptr = this->shellNode;
    while(ptr!=NULL) {
        prv = ptr;
        ptr = ptr->next;
    }
    if (prv==NULL) this->shellNode = shell_node;
    else           prv->next       = shell_node;

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
    unsigned int float_size  = (unsigned int)sizeof(float);
    unsigned int uint_size   = (unsigned int)sizeof(unsigned int);
    unsigned int shortu_size = (unsigned int)sizeof(short unsigned int);

    unsigned int j_length = shortu_size*4U;
    unsigned int w_length = float_size*4U;

    unsigned int solid_indexes  = 0;
    unsigned int solid_vertexes = 0;
    GLTFShellNode*  shell_node = this->shellNode;
    while (shell_node!=NULL) {
        solid_indexes  += shell_node->shell_indexes;
        solid_vertexes += shell_node->shell_vertexes;
        shell_node = shell_node->next;
    }

    unsigned int buffer_len = (unsigned int)solid_indexes*uint_size + (unsigned int)solid_vertexes*float_size*8U;        // I, V, N, U of the Shell
    if (this->has_joints) {
        buffer_len += (unsigned int)solid_vertexes*(j_length + w_length) + (unsigned int)this->num_joints*float_size*16U; //+ J, W, M of the Shell
    }
    this->bin_buffer = make_Buffer(buffer_len);
    if (this->bin_buffer.buf==NULL) {
        PRINT_MESG("GLTFData::createBinDataAoS: ERROR: No more memory.\n");
        return;
    }

    shell_node = this->shellNode;
    while (shell_node!=NULL) {             // 全SHELL
        unsigned int i_offset = 0;
        unsigned int v_offset = 0;
        for (unsigned int f=0; f<shell_node->num_facets; f++) {      // SHELL中の FACET
            // binary of indexies
            unsigned int i_length = shell_node->facet_index[f] * uint_size;
            cat_b2Buffer(shell_node->vi + i_offset, &(this->bin_buffer), i_length);
            i_offset += shell_node->facet_index[f];

            for (unsigned int i=0; i<shell_node->facet_vertex[f]; i++) {
                // vertex
                cat_b2Buffer(&shell_node->vv[v_offset + i].x, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vv[v_offset + i].y, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vv[v_offset + i].z, &(this->bin_buffer), float_size);
                // normal
                cat_b2Buffer(&shell_node->vn[v_offset + i].x, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vn[v_offset + i].y, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vn[v_offset + i].z, &(this->bin_buffer), float_size);
                // uvmap
                cat_b2Buffer(&shell_node->vu[v_offset + i].u, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vu[v_offset + i].v, &(this->bin_buffer), float_size);

                if (this->has_joints) {
                    // weighted joint
                    for (int j=1; j<=4; j++) {
                        cat_b2Buffer(&shell_node->vj[v_offset + i].element(j), &(this->bin_buffer), shortu_size);
                    }
                    // weight value
                    for (int j=1; j<=4; j++) {
                        cat_b2Buffer(&shell_node->vw[v_offset + i].element(j), &(this->bin_buffer), float_size);
                    }
                }
            }
            v_offset += shell_node->facet_vertex[f];
        }

        // Inverse Bind Matrix
/*
        for (int k=0; k<this->num_joints; k++) {
            int kz = k*16; 
            for (int j=1; j<=4; j++) {
                int jz = (j-1)*4;
                for (int i=1; i<=4; i++) {
                    cat_b2Buffer(&shell_node->vm[kz + jz + i - 1], &(this->bin_buffer), float_size);
                }
            }
        } 
*/
        for (int i=0; i<this->num_joints*16; i++) {
            cat_b2Buffer(&shell_node->vm[i], &(this->bin_buffer), float_size);
        } 

        shell_node = shell_node->next;
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
    unsigned int float_size  = (unsigned int)sizeof(float);
    unsigned int uint_size   = (unsigned int)sizeof(unsigned int);
    unsigned int shortu_size = (unsigned int)sizeof(short unsigned int);

    unsigned int j_length = shortu_size*4U;
    unsigned int w_length = float_size*4U;

    unsigned int solid_indexes  = 0;
    unsigned int solid_vertexes = 0;
    GLTFShellNode*  shell_node = this->shellNode;
    while (shell_node!=NULL) {
        solid_indexes  += shell_node->shell_indexes;
        solid_vertexes += shell_node->shell_vertexes;
        shell_node = shell_node->next;
    }

    unsigned int buffer_len = (unsigned int)solid_indexes*uint_size + (unsigned int)solid_vertexes*float_size*8U;           // I, V, N, U of the Shell
    if (this->has_joints) {
        buffer_len += (unsigned int)solid_vertexes*(j_length + w_length) + (unsigned int)this->num_joints*float_size*16U;   //+ J, W, M of the Shell
    }
    this->bin_buffer = make_Buffer(buffer_len);
    if (this->bin_buffer.buf==NULL) {
        PRINT_MESG("GLTFData::createBinDataSoA: ERROR: No more memory.\n");
        return;
    }

    shell_node = this->shellNode;
    while (shell_node!=NULL) {             // 全SHELL
        unsigned int i_offset = 0;
        unsigned int v_offset = 0;
        for (unsigned int f=0; f<shell_node->num_facets; f++) {      // SHELL中の FACET
            // binary of indexies
            unsigned int i_length = shell_node->facet_index[f] * uint_size;
            cat_b2Buffer(shell_node->vi + i_offset, &(this->bin_buffer), i_length);
            i_offset += shell_node->facet_index[f];

            // vertex
            for (unsigned int i=0; i<shell_node->facet_vertex[f]; i++) {
                cat_b2Buffer(&shell_node->vv[v_offset + i].x, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vv[v_offset + i].y, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vv[v_offset + i].z, &(this->bin_buffer), float_size);
            }
            // normal
            for (unsigned int i=0; i<shell_node->facet_vertex[f]; i++) {
                cat_b2Buffer(&shell_node->vn[v_offset + i].x, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vn[v_offset + i].y, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vn[v_offset + i].z, &(this->bin_buffer), float_size);
            }
            // uvmap
            for (unsigned int i=0; i<shell_node->facet_vertex[f]; i++) {
                cat_b2Buffer(&shell_node->vu[v_offset + i].u, &(this->bin_buffer), float_size);
                cat_b2Buffer(&shell_node->vu[v_offset + i].v, &(this->bin_buffer), float_size);
            }

            if (this->has_joints) {
                // weighted joints
                for (unsigned int i=0; i<shell_node->facet_vertex[f]; i++) {
                    for (int j=1; j<=4; j++) {
                        cat_b2Buffer(&shell_node->vj[v_offset + i].element(j), &(this->bin_buffer), shortu_size);
                    }
                }
                // weight value
                for (unsigned int i=0; i<shell_node->facet_vertex[f]; i++) {
                    for (int j=1; j<=4; j++) {
                        cat_b2Buffer(&shell_node->vw[v_offset + i].element(j), &(this->bin_buffer), float_size);
                    }
                }
            }
            v_offset += shell_node->facet_vertex[f];
        }
        shell_node = shell_node->next;
    }
    return;
}


///////////////////////////////////////////////////////////////////////
// Inverse Bind Matrix

void  GLTFData::addBufferViewsIBM(void)
{
    char buf[LBUF];
    unsigned int float_size = (unsigned int)sizeof(float);

    // 4x4 matrix
    unsigned int length = float_size*16U*this->num_joints;
    memset(buf, 0, LBUF);
    snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS_DATA, 0, this->bin_offset, length);
    json_insert_parse(this->buffviews, buf);
    this->bin_offset += length;
}


void  GLTFData::addAccessorsIBM(void)
{
    char buf[LBUF];
    unsigned int float_size = (unsigned int)sizeof(float);
    unsigned int offset = 0;
    unsigned int length = float_size*16U;

    //for (int i=0; i<this->num_joints; i++) {
        // 4x4 matrix
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLTF_ACCESSORS, this->view_no, offset, 5126, this->num_joints, "MAT4");
        json_insert_parse(this->accessors, buf);
        offset += length;
        this->accessor_no++;
        this->mesh_prim_no++;
    //}
    this->view_no++;
}


void  GLTFData::createInverseBindMatrix(SkinJointData* skin_joint)
{
    unsigned int float_size = (unsigned int)sizeof(float);


    // Matrix
    for (int k=0; k<this->num_joints; k++) {
        for (int j=1; j<=4; j++) {
            for (int i=1; i<=4; i++) {
                float element = skin_joint->bind_shape.matrix.element(i, j);
                cat_b2Buffer(&element, &this->bin_buffer, float_size);
            }
        }
    }
    return;
}



//////////////////////////////////////////////////////////////////////////////////////////
// Output
//

void  GLTFData::outputFile(const char* fname, const char* out_dirn, const char* ptm_dirn, const char* tex_dirn, const char* bin_dirn)
{
    //PRINT_MESG("GLTFData::outputFile: start\n");

    char* packname = pack_head_tail_char(get_file_name(fname), ' ');
    Buffer file_name = make_Buffer_bystr(packname);
    ::free(packname);
    //
    canonical_filename_Buffer(&file_name, TRUE);
    if (file_name.buf[0]=='.') file_name.buf[0] = '_';
    
    //
    if (this->glb_out) {
        this->output_glb ((char*)file_name.buf, (char*)out_dirn, (char*)ptm_dirn, (char*)tex_dirn, (char*)bin_dirn);
    }
    else {
        this->output_gltf((char*)file_name.buf, (char*)out_dirn, (char*)ptm_dirn, (char*)tex_dirn, (char*)bin_dirn);
    }

    free_Buffer(&file_name);
    return;
}


void  GLTFData::output_gltf(char* fn, char* out_dirn, char* ptm_dirn, char* tex_dirn, char* bin_dirn)
{
    Buffer out_path = make_Buffer_bystr(out_dirn);
    if (this->phantom_out) cat_s2Buffer(ptm_dirn, &out_path);
    cat_s2Buffer(fn, &out_path);
    change_file_extension_Buffer(&out_path, ".gltf");

    Buffer bin_path = make_Buffer_bystr(out_dirn);
    if (this->phantom_out) cat_s2Buffer(ptm_dirn, &bin_path);
    cat_s2Buffer(bin_dirn, &bin_path);
    cat_s2Buffer(fn, &bin_path);
    change_file_extension_Buffer(&bin_path, ".bin");

    Buffer tex_path = make_Buffer_bystr(tex_dirn);
    Buffer rel_path = make_Buffer_bystr(bin_dirn);
    cat_s2Buffer(fn, &rel_path);
    change_file_extension_Buffer(&rel_path, ".bin");
#ifdef WIN32
    replace_char(rel_path.buf, rel_path.vldsz, '\\', '/');
    replace_char(tex_path.buf, tex_path.vldsz, '\\', '/');
#endif
    convertJson_TexturePath((char*)tex_path.buf);

    // update buffers
    char buf[LBUF];
    memset(buf, 0, LBUF);
    snprintf(buf, LBUF-1, JBXL_GLTF_BUFFERS_BIN, (char*)rel_path.buf, this->bin_buffer.vldsz);
    json_insert_parse(this->buffers, buf);

    if (this->has_joints && this->matrix_buffer.vldsz>0) {
        Buffer base64 = encode_base64_Buffer(this->matrix_buffer);
        unsigned int len = base64.vldsz + LBUF;
        char* big_buf = (char*)malloc(len);
        memset(big_buf, 0, len);
        snprintf(big_buf, len-1, JBXL_GLTF_BUFFERS_B64, (char*)base64.buf, this->matrix_buffer.vldsz);
        json_insert_parse(this->buffers, big_buf);
        free_Buffer(&base64);
        ::free(big_buf);
    }

    // output json data
    FILE* fp = fopen((char*)out_path.buf, "w");
    if (fp!=NULL) {
        print_json(fp, this->json_data, JSON_INDENT_FORMAT);
        fclose(fp);
    }

    // output binary data
    fp = fopen((char*)bin_path.buf, "wb");
    if (fp!=NULL) {
        fwrite((void*)(this->bin_buffer.buf), this->bin_buffer.vldsz, 1, fp);
        fclose(fp);
    }

    free_Buffer(&tex_path);
    free_Buffer(&rel_path);
    free_Buffer(&out_path);
    free_Buffer(&bin_path);
    return;
}


void  GLTFData::output_glb(char* fn, char* out_dirn, char* ptm_dirn, char* tex_dirn, char* bin_dirn)
{
    UNUSED(bin_dirn);

    Buffer out_path = make_Buffer_bystr(out_dirn);
    if (this->phantom_out) cat_s2Buffer(ptm_dirn, &out_path);
    cat_s2Buffer(fn, &out_path);
    change_file_extension_Buffer(&out_path, ".glb");

    Buffer tex_path = make_Buffer_bystr(out_dirn);
    cat_s2Buffer(tex_dirn, &tex_path);

    glbFileHeader header;
    memcpy((void*)&(header.magic), (void*)JBXL_GLB_HEADER, 4);
    header.version = 2;
    header.length  = 0;

    glbTextureInfo* texture_info = getGLBTextureInfo((char*)tex_path.buf);
    uDWord tex_size = convertJson_gltf2glb(texture_info);
    /**/
    DEBUG_MODE {
        glbTextureInfo* info = texture_info;
        while (info!=NULL) {
            PRINT_MESG("GLTFData::output_glb: Texture File = %s, Length = %d, Pad = %d\n",  (char*)info->fname->buf, info->length, info->pad);
            info = info->next;
        }
    }/**/

    // JSON Data
    glbDataChunk json_chunk;
    Buffer json_out   = json_inverse_parse(this->json_data, JSON_ONELINE_FORMAT);
    uDWord len        = json_out.vldsz;         // + sizeof(uDWord)*2;
    uDWord pad        = (4 - len%4)%4;          // len -> pad: 0->0, 1->3, 2->2, 3->1
    json_chunk.length = len + pad;
    json_chunk.pad    = pad;
    json_chunk.data   = (uByte*)json_out.buf;
    memcpy((void*)&(json_chunk.type), (void*)JBXL_GLB_TYPE_JSON, 4);

    // BIN Data
    glbDataChunk bin_chunk;
    len = this->bin_buffer.vldsz + tex_size;    // + sizeof(uDWord)*2;
    pad = (4 - len%4)%4;
    bin_chunk.length = len + pad;
    bin_chunk.pad    = pad;
    bin_chunk.data   = (uByte*)bin_buffer.buf;
    memcpy((void*)&(bin_chunk.type), (void*)JBXL_GLB_TYPE_BIN, 4);
    // 
    header.length  = sizeof(glbFileHeader) + json_chunk.length + bin_chunk.length + sizeof(uDWord)*4;

    // output
    FILE* fp = fopen((char*)out_path.buf, "wb");
    if (fp!=NULL) {
        fwrite((void*)(&header), sizeof(glbFileHeader), 1, fp);
        // Json
        fwrite((void*)(&json_chunk), sizeof(uDWord)*2, 1, fp);
        fwrite((void*)(json_chunk.data), json_out.vldsz, 1, fp);
        for (uDWord i=0; i<json_chunk.pad; i++) fwrite((void*)" ", 1, 1, fp);
        // Bin
        fwrite((void*)(&bin_chunk), sizeof(uDWord)*2, 1, fp);
        fwrite((void*)(bin_chunk.data), bin_buffer.vldsz, 1, fp);
        for (uDWord i=0; i<bin_chunk.pad; i++) fwrite((void*)"\0", 1, 1, fp);
        // Texrure
        glbTextureInfo* tex_info = texture_info;
        while (tex_info!=NULL) {
            FILE* tp = fopen((char*)tex_info->fname->buf, "rb");
            if (tp!=NULL) {
                uByte* tex_buf = (uByte*)malloc(tex_info->length);
                if (tex_buf!=NULL) {
                    fread(tex_buf, 1, tex_info->length, tp);
                    fwrite((void*)tex_buf, tex_info->length, 1, fp);
                    for (uDWord i=0; i<tex_info->pad; i++) fwrite((void*)"\0", 1, 1, fp);
                    ::free(tex_buf);
                }
                fclose(tp);
            }
            tex_info = tex_info->next;
        }
    }
    fclose(fp);

    free_Buffer(&out_path);
    free_Buffer(&tex_path);
    free_Buffer(&json_out);
    freeGLBTextureInfo(texture_info);
    return;
}


// "xyz.png"  ->  "(tex_dirn)xyz.png"
void  GLTFData::convertJson_TexturePath(char* tex_dirn)
{
    if (tex_dirn==NULL) return;

    tJson* json = this->images;
    if (json!=NULL) json = json->next;
    if (json!=NULL) {
        while (json->esis!=NULL) json = json->esis;
    }
    //
    while (json!=NULL) {     // here is {
        tJson* uri = search_key_json(json, "uri", FALSE, 1);
        if (uri!=NULL) {
            Buffer* tex = new_Buffer((int)strlen(tex_dirn) + uri->ldat.val.vldsz + 5);   // ../ + \0 + 1(予備)
            cat_s2Buffer("\"", tex); 
            if (this->phantom_out) cat_s2Buffer("../", tex); 
            cat_s2Buffer(tex_dirn, tex);
            cat_s2Buffer(&(uri->ldat.val.buf[1]), tex); 
            free_Buffer(&uri->ldat.val);
            uri->ldat.val = *tex;
        }
        json = json->ysis;
    }
    return;
}



//////////////////////////////////////////////////////////////////////////////////////////

// Affine 

gltfFacetMinMax  GLTFData::getFacetMinMax(MeshFacetNode* facet)
{
    gltfFacetMinMax min_max;

    //min_max.index_max = facet->data_index[0];
    //min_max.index_min = facet->data_index[0];
    min_max.vertex_x_max = (float)facet->vertex_value[0].x;
    min_max.vertex_x_min = (float)facet->vertex_value[0].x;
    min_max.vertex_y_max = (float)facet->vertex_value[0].y;
    min_max.vertex_y_min = (float)facet->vertex_value[0].y;
    min_max.vertex_z_max = (float)facet->vertex_value[0].z;
    min_max.vertex_z_min = (float)facet->vertex_value[0].z;
/*
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

    for (int i=1; i<facet->num_index; i++) {
        i_temp = facet->data_index[i];
        if (min_max.index_max < i_temp) min_max.index_max = i_temp;
        if (min_max.index_min > i_temp) min_max.index_min = i_temp;
    }
*/

    float  f_temp;
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
/*
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
*/
    }
    return min_max;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// GLB
//

/**
uDWord  GLTFData::convertJson_gltf2glb(glbTextureInfo* tex_info)

glTFの Jsonデータを glbの Jsonデータに変換する．

@param   tex_info  テクスチャ情報を格納したデータへのポインタ
@retval  テクスチャデータの合計サイズ．PADを含む．uDWord（unsigned 4Byte int）
*/
uDWord  GLTFData::convertJson_gltf2glb(glbTextureInfo* tex_info)
{
    char buf[LBUF];

    uDWord tex_size = 0;
    while (tex_info!=NULL) {
        // bufferViews
        memset(buf, 0, LBUF);
        uDWord length = tex_info->length + tex_info->pad;
        snprintf(buf, LBUF-1, JBXL_GLTF_VIEWS_DATA, 0, this->bin_offset, length);
        json_insert_parse(this->buffviews, buf);
        // images
        memset(buf, 0, LBUF);
        snprintf(buf, LBUF-1, JBXL_GLB_ACCESSORS_PNG_IMAGE, this->view_no);
        json_insert_parse(this->images, buf);
        del_json(&(tex_info->json->prev));
        //
        this->bin_offset += length;
        this->view_no++;
        this->accessor_no++;
        tex_size = tex_size + length;
        tex_info = tex_info->next;
    }

    // buffers
    uDWord bin_len = this->bin_buffer.vldsz + tex_size;
    uDWord bin_pad = (4 - bin_len%4)%4;

    tJson* buffer_uri = search_key_json(this->buffers, "uri", FALSE, 1);
    tJson* buffer_len = buffer_uri->ysis;
    del_json(&buffer_uri);
    json_set_int_val(buffer_len, bin_len + bin_pad);

    return tex_size;
}


glbTextureInfo*  GLTFData::getGLBTextureInfo(const char* tex_dirn)
{
    // Search Texture Top
    tJson* tjsn = this->images;
    if (tjsn!=NULL) tjsn = tjsn->next;
    if (tjsn!=NULL) {
        while (tjsn->esis!=NULL) tjsn = tjsn->esis;
    }

    // Texture Data
    glbTextureInfo*  p_tex_info = NULL;
    glbTextureInfo** n_info = &p_tex_info;
    //
    while (tjsn!=NULL) {     // here is {
        tJson* texture_uri = search_key_json(tjsn, "uri", FALSE, 1);
        if (texture_uri!=NULL) {
            *n_info = (glbTextureInfo*)malloc(sizeof(glbTextureInfo));
            (*n_info)->length = 0;
            (*n_info)->pad    = 0;
            (*n_info)->fname  = new_Buffer((int)strlen(tex_dirn) + texture_uri->ldat.val.vldsz + 2);     // \0 + 1(予備)
            (*n_info)->json   = texture_uri;
            (*n_info)->next   = NULL;
            //
            cat_s2Buffer(tex_dirn, (*n_info)->fname);
            if (texture_uri->ldat.val.buf[0]=='"') {                                                // 最初の " を避ける
                cat_s2Buffer(&(texture_uri->ldat.val.buf[1]), (*n_info)->fname);
            }
            if ((*n_info)->fname->buf[(*n_info)->fname->vldsz-1]=='"') {                            // 最期の " を避ける
                (*n_info)->fname->buf[(*n_info)->fname->vldsz-1] = '\0';
                (*n_info)->fname->vldsz--;
            }
            //
            uDWord len = file_size((char*)(*n_info)->fname->buf);
            (*n_info)->length = len;
            (*n_info)->pad    = (4 - len%4)%4;
            //
            n_info = &((*n_info)->next);
        }
        tjsn = tjsn->ysis;
    }
    
    /*
    glbTextureInfo* info = p_tex_info;
    while (info!=NULL) {
        print_message("======> %s %d %d\n",  (char*)info->fname->buf, info->length, info->pad);
        info = info->next;
    }
    */
    return p_tex_info;
}


void  GLTFData::freeGLBTextureInfo(glbTextureInfo* texture_info)
{
    glbTextureInfo* tinfo = texture_info;
    while (tinfo!=NULL) {
        glbTextureInfo* next = tinfo->next;
        free_Buffer(tinfo->fname);
        ::free(tinfo);
        tinfo = next;
    }
    return;
}


