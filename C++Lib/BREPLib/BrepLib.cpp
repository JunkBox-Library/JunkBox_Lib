
/**
@brief   BREPライブラリ
@file    BrepLib.cpp
@author  Fumi.Iseki (C)
*/


#include "BrepLib.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////
// BrepSolidList
// 

BrepSolidList::~BrepSolidList(void)
{ 
    DEBUG_INFO("DESTRUCTOR: BrepSolidList");
}



void  BrepSolidList::init(void)
{
    solid_list.clear();
}



void  BrepSolidList::free(void)
{
    BREP_SOLID_LIST::iterator isolid;
    for (isolid=solid_list.begin(); isolid!=solid_list.end(); isolid++) freeBrepSolid(*isolid);

    solid_list.clear();
}



void  BrepSolidList::clear(void)
{
    BREP_SOLID_LIST::iterator isolid;
    for (isolid=solid_list.begin(); isolid!=solid_list.end(); isolid++) (*isolid)->FreeData();

    solid_list.clear();
}




////////////////////////////////////////////////////////////////////////////////////////
//

/*
データにソリッドを追加する．
*/
int  BrepSolidList::addSolid(MeshObjectData* mesh)
{
    if (mesh==NULL) return 0;

    Vector<double> vertex[3];
    Vector<double> normal[3];

    BREP_SOLID* solid = new BREP_SOLID();
    BREP_SHELL* shell = new BREP_SHELL(solid);

    MeshObjectNode* node = mesh->nodelist;

    while (node!=NULL) {
        //
        for (int num=0; num<node->num_index-2; num+=3) {
            BREP_FACET* facet = new BREP_FACET(shell);
            //
            int idx0 = node->data_index[num];
            int idx1 = node->data_index[num+1];
            int idx2 = node->data_index[num+2];
        
            vertex[0] = node->vertex_value[idx0];
            vertex[1] = node->vertex_value[idx1];
            vertex[2] = node->vertex_value[idx2];
            normal[0] = node->normal_value[idx0];
            normal[1] = node->normal_value[idx1];
            normal[2] = node->normal_value[idx2];

            BREP_CONTOUR* contour = CreateContourByVector(facet, vertex, normal, NULL, false);
            if (contour!=NULL) {
                solid->contours.push_back(contour);
            }
            else {
                deleteNull(facet);
            }
        }
        node = node->next;
    }

    int fno = CloseTriSolid(solid, false, NULL);
    solid->facetno = fno;

    //
    if (mesh->affine_trans!=NULL) {
        long int  vnum;
        BREP_VERTEX** vertex_data = GetOctreeVertices(solid->octree, &vnum);

        for (long int i=0; i<vnum; i++) {
            vertex_data[i]->point  = mesh->affine_trans->execTrans (vertex_data[i]->point);
            vertex_data[i]->normal = mesh->affine_trans->execRotate(vertex_data[i]->normal);
        }
        ::free(vertex_data);
    }

    solid_list.push_back(solid);

    return fno;
}



void  BrepSolidList::outputFile(const char* fname, const char* path, bool binfile)
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
    change_file_extension_Buffer(&out_path, ".stl");
    //
    if (binfile) {
        WriteSTLFileB((char*)out_path.buf, solid_list);
    }
    else {
        WriteSTLFileA((char*)out_path.buf, solid_list);
    }

    free_Buffer(&file_name);
    free_Buffer(&out_path);

    return;
}



BREP_SOLID*  BrepSolidList::getMerge(CVCounter* counter)
{
    BREP_SOLID* solid  = new BREP_SOLID();
    BREP_SHELL* shell  = new BREP_SHELL(solid);

    // Counter
    if (counter!=NULL) {
        int maxnum = 0;
        BREP_SOLID_LIST::iterator isolid;
        for (isolid=solid_list.begin(); isolid!=solid_list.end(); isolid++){
            BREP_CONTOUR_LIST::iterator icon;
            for (icon=(*isolid)->contours.begin(); icon!=(*isolid)->contours.end(); icon++){
                maxnum++;
            }
        }
        counter->GetUsableCounter();
        counter->Start(maxnum);
    }

    BREP_SOLID_LIST::iterator isolid;
    for (isolid=solid_list.begin(); isolid!=solid_list.end(); isolid++){
        //
        BREP_CONTOUR_LIST::iterator icon;
        for (icon=(*isolid)->contours.begin(); icon!=(*isolid)->contours.end(); icon++){
            Vector<double> vect[3];
            Vector<double> norm[3];
            //
            BREP_WING* wing = (*icon)->wing;
            for (int i=0; i<3; i++) {
                vect[i] = wing->vertex->point;
                norm[i] = wing->vertex->normal;
                wing = wing->next;
            }
            //
            BREP_FACET* facet = new BREP_FACET(shell);
            BREP_CONTOUR* contour = CreateContourByVector(facet, vect, norm, NULL, false);
            if (contour!=NULL) solid->contours.push_back(contour);
            else  deleteNull(facet);
            //
            if (counter!=NULL) counter->StepIt();
        }
    }

    int fno = CloseTriSolid(solid, false, NULL);
    solid->facetno = fno;

    return solid;
}

