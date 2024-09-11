/**
@brief    FBX用 ツール
@file     FBXDataTool.cpp
@author   Fumi.Iseki (C)
*/

#include  "FBXDataTool.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////
//

FBXData::~FBXData(void)
{
    DEBUG_MODE PRINT_MESG("FBXData::DESTRUCTOR:\n");
    this->free();
}


void  FBXData::init(void)
{
    this->fbx_name    = init_Buffer();
    this->phantom_out = true;
    this->no_offset   = false;

    this->has_joints  = false;
    this->joints_list = NULL;

    this->forUnity    = true;
    this->forUE       = false;

    this->engine      = JBXL_3D_ENGINE_UE;
    this->affineTrans = NULL;
    this->skeleton.init();
}


void  FBXData::free(void)
{
    free_Buffer(&(this->fbx_name));
    this->skeleton.free();
    this->delAffineTrans();
    this->affineTrans = NULL;
    
    if (this->joints_list!=NULL) del_tList(&this->joints_list);
    this->joints_list = NULL;
}


void  FBXData::setEngine(int e)
{
    this->setUnity(false);
    this->setUE(false);
    //
    this->engine = e;
    if (e==JBXL_3D_ENGINE_UNITY)   this->setUnity(true);
    else if (e==JBXL_3D_ENGINE_UE) this->setUE(true);

    return;
}


//void  FBXData::outputFile(const char* fname, const char* out_dirn, const char* ptm_dirn, const char* tex_dirn, const char* bin_dirn)
void  FBXData::outputFile(const char* fname, const char* out_dirn, const char* tex_dirn, const char* bin_dirn)
{
    UNUSED(out_dirn);
    UNUSED(tex_dirn);
    UNUSED(bin_dirn);

    char* packname = pack_head_tail_char(get_file_name(fname), ' ');
    Buffer file_name = make_Buffer_bystr(packname);
    ::free(packname);
    //
    canonical_filename_Buffer(&file_name, TRUE);
    if (file_name.buf[0]=='.') file_name.buf[0] = '_';

    free_Buffer(&file_name);
    return;
}


//////////////////////////////////////////////////////////////////////////////////////////

void  FBXData::addShell(MeshObjectData* meshdata, bool collider, SkinJointData* joints_data, tList* joints_template)
{
    //PRINT_MESG("FBXData::addShell: start\n");

    if (meshdata==NULL) return;

    if (joints_data!=NULL && joints_template!=NULL) {
        if (this->joints_list==NULL && !this->has_joints) {
            this->joints_list = joints_template;
            this->has_joints = true;
        }
        else {
            if (joints_template!=NULL) del_tList(&joints_template);
            joints_template = NULL;
        }
    }

    MeshFacetNode* facet = meshdata->facet;
    while (facet!=NULL) {
        if (facet->num_vertex != facet->num_texcrd) {
            PRINT_MESG("FBXData::addShell: Error: missmatch vertex and uvmap number! (%d != %d)\n", facet->num_vertex, facet->num_texcrd);
            facet = facet->next;
            continue;
        }

        // UV Map and PLANAR Texture
        if (facet->material_param.mapping == MATERIAL_MAPPING_PLANAR) {
            Vector<double> scale(1.0, 1.0, 1.0);
            if (meshdata->affineTrans!=NULL) scale = meshdata->affineTrans->getScale();
            facet->generatePlanarUVMap(scale, facet->texcrd_value);
        }
        facet->execAffineTransUVMap(facet->texcrd_value, facet->num_vertex);

        //facet->num_index;
        //facet->num_vertex;

        //int*            index = facet->data_index;
        //Vector<double>* vectr = facet->vertex_value;
        //Vector<double>* norml = facet->normal_value;
        //UVMap<double>*  uvmap = facet->texcrd_value;

        facet = facet->next;
    }

    //
    this->phantom_out = true;
    if (collider) {
        this->phantom_out = false;
    }

    //
    if (this->has_joints && this->joints_list!=NULL) {
        
    }

    return;
}


/**
Vector<double>  FBXData::execAffineTrans(void)

FBXデータの 原点縮退変換を行う．
no_offset が trueの場合，データの中心を原点に戻し，実際の位置をオフセットで返す．
*/
Vector<double>  FBXData::execAffineTrans(void)
{
    Vector<double> center(0.0, 0.0, 0.0);

    if (this->no_offset) center = affineTrans->getShift();

    return center;
}


void  FBXData::output_fbx(FILE* fp)
{
    if (fp==NULL) return;
    
    return;
}

