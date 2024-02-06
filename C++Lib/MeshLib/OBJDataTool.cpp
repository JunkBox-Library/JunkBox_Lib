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
    this->free();
}


void  OBJData::init(int n)
{
    this->obj_name = init_Buffer();
    this->num_obj  = n;
    this->phantom_out = true;

    this->next     = NULL;
    this->geo_node = NULL;
    this->mtl_node = NULL;
    this->affine_trans = NULL;
}


void  OBJData::free(void)
{
    free_Buffer(&(this->obj_name));

    delAffineTrans();
    this->affine_trans = NULL;

    delete(this->geo_node);
    delete(this->mtl_node);
    this->geo_node = NULL;
    this->mtl_node = NULL;

    this->delete_next();
}


void  OBJData::delete_next(void)
{
    if (this->next==NULL) return;

    OBJData* _next = this->next;
    while (_next!=NULL) {
        OBJData* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    this->next = NULL;
}


void  OBJData::addObject(MeshObjectData* meshdata, bool collider)
{
    if (meshdata==NULL) return;

    OBJData* ptr_obj = this;
    while (ptr_obj->next!=NULL) ptr_obj = ptr_obj->next;
    // 
    ptr_obj->next = new OBJData(-1);
    this->num_obj++;

    if (meshdata->affine_trans!=NULL) { // Grass の場合は NULL
        ptr_obj->next->setAffineTrans(*meshdata->affine_trans);
    }
    ptr_obj->next->obj_name = dup_Buffer(meshdata->data_name);

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

        // Material
        (*_mtl_node)->material = dup_Buffer(facet->material_id);
        (*_mtl_node)->same_material = facet->same_material;
        if (!(*_mtl_node)->same_material) {
            (*_mtl_node)->material_param.dup(facet->material_param);
            (*_mtl_node)->setup_params();
        }

        _geo_node = &((*_geo_node)->next);
        _mtl_node = &((*_mtl_node)->next);
        facet = facet->next;
    }
}


void  OBJData::execAffineTrans(void)
{
    OBJData* obj = this->next;
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


void  OBJData::outputFile(const char* fname, const char* out_path, const char* mtl_dirn)
{
    FILE* fp = NULL;
    char* packname = pack_head_tail_char(get_file_name(fname), ' ');
    Buffer file_name = make_Buffer_bystr(packname);
    ::free(packname);

    canonical_filename_Buffer(&file_name);
    if (file_name.buf[0]=='.') file_name.buf[0] = '_';
    //
    Buffer obj_path;
    if (out_path==NULL) obj_path = make_Buffer_bystr("./");
    else                obj_path = make_Buffer_bystr(out_path);
    //
    Buffer rel_path;    //  相対パス
    if (mtl_dirn==NULL) rel_path = make_Buffer_bystr("");
    else                rel_path = make_Buffer_bystr(mtl_dirn);
    //
    cat_Buffer(&file_name, &rel_path);
    change_file_extension_Buffer(&rel_path, ".mtl");

    Buffer mtl_path = dup_Buffer(obj_path);
    cat_Buffer(&rel_path, &mtl_path);

    cat_Buffer(&file_name, &obj_path);
    change_file_extension_Buffer(&obj_path, ".obj");

    // MTL
    fp = fopen((char*)mtl_path.buf, "wb");
    if (fp!=NULL) {
        this->output_mtl(fp);
        fclose(fp);
    }

    // OBJECT
    fp = fopen((char*)obj_path.buf, "wb");
    if (fp!=NULL) {
        this->output_obj(fp, (char*)rel_path.buf);
        fclose(fp);
    }
    //
    free_Buffer(&obj_path);
    free_Buffer(&mtl_path);
    free_Buffer(&rel_path);
    //
    return;
}


void  OBJData::output_mtl(FILE* fp)
{
    if (fp==NULL) return;

    fprintf(fp, "# %s\n", OBJDATATOOL_STR_MTLFL);
    fprintf(fp, "# %s\n", OBJDATATOOL_STR_TOOL);
    fprintf(fp, "# %s\n", OBJDATATOOL_STR_AUTHOR);
    fprintf(fp, "# %s\n", OBJDATATOOL_STR_VER);

    OBJData* obj = this->next;
    while (obj!=NULL) {
        OBJFacetMtlNode* node = obj->mtl_node;
        while(node!=NULL) {
            if (!node->same_material) {
                fprintf(fp, "#\n");
                fprintf(fp, "newmtl %s\n", node->material.buf);         // マテリアル名

                fprintf(fp, "Kd %lf %lf %lf\n", node->kd.x, node->kd.y, node->kd.z);
                fprintf(fp, "d %lf\n", node->dd);
                //
                fprintf(fp, "map_Kd %s\n", node->map_kd.buf);           // Texture ファイル名
            }
            node = node->next;
        }
        obj = obj->next;
    }
}


void  OBJData::output_obj(FILE* fp, const char* mtl_path)
{
    if (fp==NULL) return;

    fprintf(fp, "# %s\n", OBJDATATOOL_STR_OBJFL);
    fprintf(fp, "# %s\n", OBJDATATOOL_STR_TOOL);
    fprintf(fp, "# %s\n", OBJDATATOOL_STR_AUTHOR);
    fprintf(fp, "# %s\n", OBJDATATOOL_STR_VER);

    int p_num = 1;
    OBJData* obj = this->next;
    while (obj!=NULL) {
        fprintf(fp, "# \n# SHELL\n");
        OBJFacetGeoNode* facet = obj->geo_node;
        while(facet!=NULL) {
            fprintf(fp, "# FACET\n");
            fprintf(fp, "mtllib %s\n", mtl_path);       // ファイル名

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
            //
            facet = facet->next;
        }
        obj = obj->next;
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
OBJFacetGeoNode::~OBJFacetGeoNode(void)
{
    this->free();
}


void  OBJFacetGeoNode::init(void)
{
    this->material = init_Buffer();
    this->collider = true;
    this->num_index  = 0;
    this->num_vertex = 0;

    this->data_index = NULL;
    this->vv = this->vn = NULL;
    this->vt = NULL;
    this->uvmap_trans = NULL;
    this->next = NULL;
}


void OBJFacetGeoNode::free(void)
{
    free_Buffer(&(this->material));
    this->num_index  = 0;
    this->num_vertex = 0;

    if (this->data_index!=NULL) ::free(this->data_index);
    this->data_index = NULL;

    if (this->vv!=NULL) ::free(this->vv);
    if (this->vn!=NULL) ::free(this->vn);
    if (this->vt!=NULL) ::free(this->vt);
    this->vv = this->vn = NULL;
    this->vt = NULL;

    freeAffineTrans(this->uvmap_trans);
    this->uvmap_trans = NULL;

    this->delete_next();
}


void  OBJFacetGeoNode::delete_next(void)
{
    if (next==NULL) return;

    OBJFacetGeoNode* _next = this->next;
    while (_next!=NULL) {
        OBJFacetGeoNode* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    this->next = NULL;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
OBJFacetMtlNode::~OBJFacetMtlNode(void)
{
    this->free();
}


void  OBJFacetMtlNode::init(void)
{
    this->material = init_Buffer();
    this->map_kd   = init_Buffer();
    memset(&(this->material_param), 0, sizeof(this->material_param));
    this->next = NULL;
}


void  OBJFacetMtlNode::free(void)
{
    free_Buffer(&(this->material));
    free_Buffer(&(this->map_kd));
    delete_next();
}


void  OBJFacetMtlNode::delete_next(void)
{
    if (this->next==NULL) return;

    OBJFacetMtlNode* _next = this->next;
    while (_next!=NULL) {
        OBJFacetMtlNode* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    this->next = NULL;
}


void  OBJFacetMtlNode::setup_params(void)
{
    this->map_kd = make_Buffer_str(this->material_param.getTextureName());
    
    this->kd = this->material_param.getColor();
    this->dd = this->material_param.getAlpha();

    return;
}

