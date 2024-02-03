/**
@brief    OBJ用 ツール
@file     OBJTool.cpp
@author   Fumi.Iseki (C)
*/

#include  "OBJDataTool.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////
// 

OBJData::~OBJData(void)
{
}


void  OBJData::init(int n)
{
    obj_name = init_Buffer();
    num_obj  = n;
    phantom_out = false;

    next     = NULL;
    geo_node = NULL;
    mtl_node = NULL;
    affine_trans = NULL;
}


void  OBJData::free(void)
{
    free_Buffer(&obj_name);

    delAffineTrans();
    affine_trans = NULL;

    delete(geo_node);
    delete(mtl_node);
    geo_node = NULL;
    mtl_node = NULL;

    delete_next();
}


void  OBJData::delete_next(void)
{
    if (next==NULL) return;

    OBJData* _next = next;
    while (_next!=NULL) {
        OBJData* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    next = NULL;
}


void  OBJData::addObject(MeshObjectData* meshdata, bool collider)
{
    if (meshdata==NULL) return;

    OBJData* ptr_obj = this;
    while (ptr_obj->next!=NULL) ptr_obj = ptr_obj->next;
    // 
    ptr_obj->next = new OBJData(-1);
    num_obj++;

    if (meshdata->affine_trans!=NULL) { // Grass の場合は NULL
        ptr_obj->next->setAffineTrans(*meshdata->affine_trans);
    }
    ptr_obj->next->obj_name = dup_Buffer(meshdata->data_name);
    //ptr_obj->next->collider = collider;

    MeshFacetNode* facet = meshdata->facet;
    OBJFacetGeoNode** _geo_node = &(ptr_obj->next->geo_node);
    OBJFacetMtlNode** _mtl_node = &(ptr_obj->next->mtl_node);
    while (facet!=NULL) {
        if (facet->num_vertex != facet->num_texcrd) {
            PRINT_MESG("OBJData::addObject: Error: missmatch vertex and uvmap number! (%d != %d)\n", facet->num_vertex, facet->num_texcrd);
            facet = facet->next;
            continue;
        }

        *_geo_node = new OBJFacetGeoNode();
        *_mtl_node = new OBJFacetMtlNode();

        (*_geo_node)->num_index = facet->num_index;
        (*_geo_node)->data_index = (int*)malloc(sizeof(int)*(*_geo_node)->num_index);
        for (int i=0; i<(*_geo_node)->num_index; i++) {
            (*_geo_node)->data_index[i] = facet->data_index[i];
        }

        (*_geo_node)->collider = collider;
        (*_geo_node)->num_vertex = facet->num_vertex;
        (*_geo_node)->vv = (Vector<double>*)malloc(sizeof(Vector<double>)*(*_geo_node)->num_vertex);
        (*_geo_node)->vn = (Vector<double>*)malloc(sizeof(Vector<double>)*(*_geo_node)->num_vertex);
        (*_geo_node)->vt = (UVMap<double>*) malloc(sizeof(UVMap<double>) *(*_geo_node)->num_vertex);
        for (int i=0; i<(*_geo_node)->num_vertex; i++) {
            (*_geo_node)->vv[i] = facet->vertex_value[i];
            (*_geo_node)->vn[i] = facet->normal_value[i];
            (*_geo_node)->vt[i] = facet->texcrd_value[i];
        }

        if (facet->material_id.buf[0]=='#') facet->material_id.buf[0] = '_';
        (*_geo_node)->material = dup_Buffer(facet->material_id);
        (*_mtl_node)->material = dup_Buffer(facet->material_id);
        (*_mtl_node)->material_param = facet->material_param;
        (*_mtl_node)->same_material  = facet->same_material;
        (*_mtl_node)->map_kd = make_Buffer_bystr(facet->material_param.getTextureName());

        _geo_node = &((*_geo_node)->next);
        _mtl_node = &((*_mtl_node)->next);
        facet = facet->next;
    }
}


void  OBJData::execAffineTrans(void)
{
    OBJData* obj = next;
    while (obj!=NULL) {
        if (obj->affine_trans!=NULL) {
            OBJFacetGeoNode* facet = obj->geo_node;
            while(facet!=NULL) {
                for (int i=0; i<facet->num_vertex; i++) {
                    facet->vv[i] = obj->affine_trans->execTrans (facet->vv[i]);
                    facet->vn[i] = obj->affine_trans->execRotate(facet->vn[i]);
                }
                facet = facet->next;
            }
        }
        obj = obj->next;
    }
}


void  OBJData::outputFile(const char* fname, const char* path)
{
    char* packname = pack_head_tail_char(get_file_name(fname), ' ');
    Buffer file_name = make_Buffer_bystr(packname);
    ::free(packname);

    rewrite_sBuffer_bystr(&file_name, ":", "_");
    rewrite_sBuffer_bystr(&file_name, "*", "_");
    rewrite_sBuffer_bystr(&file_name, "?", "_");
    rewrite_sBuffer_bystr(&file_name, "\"", "_");
    rewrite_sBuffer_bystr(&file_name, "<", "_");
    rewrite_sBuffer_bystr(&file_name, ">", "_");
    if (file_name.buf[0]=='.') file_name.buf[0] = '_';
    //
    Buffer out_path;
    if (path==NULL) out_path = make_Buffer_bystr("./");
    else            out_path = make_Buffer_bystr(path);
    cat_Buffer(&file_name, &out_path);
    change_file_extension_Buffer(&out_path, ".obj");
    //
    FILE* fp = fopen((char*)out_path.buf, "wb");
    if (fp!=NULL) {
        output_obj(fp);
        fclose(fp);
    }
    free_Buffer(&file_name);
    free_Buffer(&out_path);

    return;
}


void  OBJData::output_obj(FILE* fp)
{
    if (fp==NULL) return;

    fprintf(fp, "# %s\n", OBJDATATOOL_STR_OBJFL);
    fprintf(fp, "# %s\n", OBJDATATOOL_STR_TOOL);
    fprintf(fp, "# %s\n", OBJDATATOOL_STR_AUTHOR);
    fprintf(fp, "# %s\n", OBJDATATOOL_STR_VER);
    fprintf(fp, "# \n");

    int p_num = 1;
    OBJData* obj = next;
    while (obj!=NULL) {
        fprintf(fp, "# SHELL\n");
        OBJFacetGeoNode* facet = obj->geo_node;
        while(facet!=NULL) {
            fprintf(fp, "# FACET\n");
            fprintf(fp, "mtllib %s\n", facet->material.buf);    // ファイル名

            for (int i=0; i<facet->num_vertex; i++) {
                fprintf(fp, "v %lf %lf %lf\n", facet->vv[i].x, facet->vv[i].z, -facet->vv[i].y);
            }
            for (int i=0; i<facet->num_vertex; i++) {
                fprintf(fp, "vt %lf %lf\n", facet->vt[i].u, facet->vt[i].v);
            }
            for (int i=0; i<facet->num_vertex; i++) {
                fprintf(fp, "vn %lf %lf %lf\n", facet->vn[i].x, facet->vn[i].z, -facet->vn[i].y);
            }
            //
            fprintf(fp, "usemtl %s\n", facet->material.buf);    // マテリアル名
            for (int i=0; i<facet->num_index/3; i++) {
                fprintf(fp, "f %d/%d/%d", facet->data_index[i*3  ]+p_num, facet->data_index[i*3  ]+p_num, facet->data_index[i*3  ]+p_num);
                fprintf(fp, " %d/%d/%d ", facet->data_index[i*3+1]+p_num, facet->data_index[i*3+1]+p_num, facet->data_index[i*3+1]+p_num);
                fprintf(fp, "%d/%d/%d\n", facet->data_index[i*3+2]+p_num, facet->data_index[i*3+2]+p_num, facet->data_index[i*3+2]+p_num);
            }
            p_num += facet->num_vertex;

            facet = facet->next;
        }
        obj = obj->next;
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
OBJFacetGeoNode::~OBJFacetGeoNode(void)
{
    free();
}


void  OBJFacetGeoNode::init(void)
{
    material = init_Buffer();
    collider = true;
    num_index  = 0;
    num_vertex = 0;

    data_index = NULL;
    vv = vn = NULL;
    vt = NULL;
    uvmap_trans = NULL;
    next = NULL;
}


void OBJFacetGeoNode::free(void)
{
    free_Buffer(&material);
    num_index  = 0;
    num_vertex = 0;

    if (data_index!=NULL) ::free(data_index);
    data_index = NULL;

    if (vv!=NULL) ::free(vv);
    if (vn!=NULL) ::free(vn);
    if (vt!=NULL) ::free(vt);
    vv = vn = NULL;
    vt = NULL;

    freeAffineTrans(uvmap_trans);
    uvmap_trans = NULL;

    delete_next();
}


void  OBJFacetGeoNode::delete_next(void)
{
    if (next==NULL) return;

    OBJFacetGeoNode* _next = next;
    while (_next!=NULL) {
        OBJFacetGeoNode* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    next = NULL;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
OBJFacetMtlNode::~OBJFacetMtlNode(void)
{
    free();
}


void  OBJFacetMtlNode::init(void)
{
    material = init_Buffer();
    map_kd   = init_Buffer();
    memset(&material_param, 0, sizeof(material_param));
    next = NULL;
}


void  OBJFacetMtlNode::free(void)
{
    free_Buffer(&material);
    free_Buffer(&map_kd);
    delete_next();
}


void  OBJFacetMtlNode::delete_next(void)
{
    if (next==NULL) return;

    OBJFacetMtlNode* _next = next;
    while (_next!=NULL) {
        OBJFacetMtlNode* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    next = NULL;
}

