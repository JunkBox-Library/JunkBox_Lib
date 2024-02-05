#ifndef  __JBXL_CPP_OBJ_TOOL_H_
#define  __JBXL_CPP_OBJ_TOOL_H_

/**
  Wavefront OBJ ファイル用ツール

*/

#include  "tools++.h"
#include  "txml.h"

#include  "TriBrep.h"
#include  "Rotation.h"
#include  "ContourBaseData.h"
#include  "MaterialParam.h"
#include  "MeshObjectData.h"


namespace jbxl {


#define  OBJDATATOOL_STR_OBJFL  "OBJ File"
#define  OBJDATATOOL_STR_MTLFL  "MTL File"
#define  OBJDATATOOL_STR_TOOL   "Created by CBJDataTool in the JunkBox_Lib++ (https://github.com/JunkBox-Library)"
#define  OBJDATATOOL_STR_AUTHOR "JBXL OBJ Data Tool Library (C) 2024 by Fumi.Iseki"
#define  OBJDATATOOL_STR_VER    "version 1.0.0, 1 Feb. 2024"


class  OBJData;
class  OBJFacetGeoNode;
class  OBJFacetMtlNode;


//////////////////////////////////////////////////////////////////////////////////////////
//
/**
先頭のデータはアンカー．
アンカーでない場合は num_obj == -1
*/
class  OBJData
{
public:
    OBJData(int n=0) { this->init(n);}    // デフォルト（n=0）ではアンカーを作る
    virtual ~OBJData(void);

public:
    Buffer  obj_name;
    bool    phantom_out;
    int     num_obj;

    OBJData* next;
    OBJFacetGeoNode* geo_node;
    OBJFacetMtlNode* mtl_node;

    AffineTrans<double>* affine_trans;

public:
    void    init(int n); 
    void    free(void); 
    void    delete_next(void);

    void    setAffineTrans (AffineTrans<double> a) { delAffineTrans(); affine_trans = new AffineTrans<double>(); affine_trans->dup(a);}
    void    delAffineTrans (void) { freeAffineTrans(this->affine_trans);}
    void    execAffineTrans(void);

    void    addObject(MeshObjectData* meshdata, bool collider);

    void    outputFile(const char* fn, const char* path, const char* texture_path);
    void    output_mtl(FILE* fp);
    void    output_obj(FILE* fp, const char* fname);
};


inline void  freeOBJData(OBJData* obj) { if(obj!=NULL) { obj->free(); delete obj; obj=NULL;} }



//////////////////////////////////////////////////////////////////////////////////////////
//
class  OBJFacetGeoNode
{
public:
    OBJFacetGeoNode() { this->init();}
    virtual ~OBJFacetGeoNode(void);

public:
    Buffer  material;
    int     num_index;
    int     num_vertex;
    bool    collider;

    int*    data_index;
    Vector<double>* vv;
    Vector<double>* vn;
    UVMap<double>*  vt;

    AffineTrans<double>* uvmap_trans;

    OBJFacetGeoNode* next;

public:
    void    init(void);
    void    free(void); 
    void    delete_next(void);
};


//////////////////////////////////////////////////////////////////////////////////////////
//
class  OBJFacetMtlNode
{
public:
    OBJFacetMtlNode() { this->init();}
    virtual ~OBJFacetMtlNode(void);

public:
    Buffer  material;
    bool    same_material;
    MaterialParam material_param;

    Vector<double> kd;
    Vector<double> ka;
    Vector<double> ks;
    Vector<double> ke;
    Vector<double> ni;
    Vector<double> ns;

    double  dd;
    int     illum;
    Buffer  map_kd;

    OBJFacetMtlNode* next;

public:
    void    init(void);
    void    free(void); 
    void    delete_next(void);
    void    setup_params(void);
};


}

#endif
