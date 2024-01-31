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
    free_Buffer(&obj_name);

    delAffineTrans();
    affine_trans = NULL;

    delete(geo_node);
    delete(mtl_node);
    geo_node = NULL;
    mtl_node = NULL;

    delete_next();
}


void  OBJData::init(int n)
{
    obj_name = init_Buffer();
    collider = true;
    num_obj  = n;

    next = NULL;
    geo_node = NULL;
    mtl_node = NULL;
    affine_trans = NULL;
}


void  OBJData::delete_next(void)
{
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

    ptr_obj->next->setAffineTrans(*meshdata->affine_trans);
    ptr_obj->next->obj_name = dup_Buffer(meshdata->data_name);
    ptr_obj->next->collider = collider;

    MeshFacetNode* facet = meshdata->facet;
    OBJFacetGeoNode** _geo_node = &(ptr_obj->next->geo_node);
    OBJFacetMtlNode** _mtl_node = &(ptr_obj->next->mtl_node);
    while (facet!=NULL) {
        *_geo_node = new OBJFacetGeoNode();
        *_mtl_node = new OBJFacetMtlNode();

        int num_vertex = facet->num_vertex;
        int num_texcrd = facet->num_texcrd;
        (*_geo_node)->vv = (Vector<double>*)malloc(sizeof(Vector<double>)*num_vertex);
        (*_geo_node)->vn = (Vector<double>*)malloc(sizeof(Vector<double>)*num_vertex);
        (*_geo_node)->vt = (UVMap<double>*) malloc(sizeof(UVMap<double>) *num_texcrd);
        for (int i=0; i<num_vertex; i++) {
            (*_geo_node)->vv[i] = facet->vertex_value[i];
            (*_geo_node)->vn[i] = facet->normal_value[i];
        }
        for (int i=0; i<num_texcrd; i++) {
            (*_geo_node)->vt[i] = facet->texcrd_value[i];
        }

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


///////////////////////////////////////////////////////////////////////////////////////////////////
//
void  OBJFacetGeoNode::init(void)
{
    material = init_Buffer();

    vv = vn = NULL;
    vt = NULL;
    uvmap_trans = NULL;
    next = NULL;
}


OBJFacetGeoNode::~OBJFacetGeoNode(void)
{
    free_Buffer(&material);

    ::free(vv);
    ::free(vn);
    ::free(vt);
    vv = vn = NULL;
    vt = NULL;

    freeAffineTrans(uvmap_trans);
    uvmap_trans = NULL;

    delete_next();
}


void  OBJFacetGeoNode::delete_next(void)
{
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
void  OBJFacetMtlNode::init(void)
{
    material = init_Buffer();
    map_kd   = init_Buffer();

    memset(&material_param, 0, sizeof(material_param));

    next = NULL;
}


OBJFacetMtlNode::~OBJFacetMtlNode(void)
{
    free_Buffer(&material);
    free_Buffer(&map_kd);

    delete_next();
}


void  OBJFacetMtlNode::delete_next(void)
{
    OBJFacetMtlNode* _next = next;

    while (_next!=NULL) {
        OBJFacetMtlNode* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    next = NULL;
}






