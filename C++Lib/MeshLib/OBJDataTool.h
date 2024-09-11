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


#define  OBJDATATOOL_STR_OBJFL      "OBJ File"
#define  OBJDATATOOL_STR_MTLFL      "MTL File"
#define  OBJDATATOOL_STR_TOOL       "Created by CBJDataTool in the JunkBox_Lib++ (https://github.com/JunkBox-Library)"
#define  OBJDATATOOL_STR_AUTHOR     "JBXL OBJ Data Tool Library (C) 2024 by Fumi.Iseki"
#define  OBJDATATOOL_STR_VER        "version 1.0.0, 1 Feb. 2024"

#define  OBJDATATOOL_MAX_FACET      500     ///< Max Facets number per File

class  OBJData;
class  OBJFacetGeoNode;
class  OBJFacetMtlNode;


//////////////////////////////////////////////////////////////////////////////////////////
//

/**
先頭のデータは SHELLデータへのアンカー．

num_obj はアンカーのみ有効な値を持つ．アンカーでない場合は num_obj == -1

*/
class  OBJData
{
public:
    OBJData(int n=0) { this->init(n);}  // n: OBJデータ（SHELLデータ）の総数．デフォルト（n=0）ではアンカーを作る
    virtual ~OBJData(void) { this->free();}

public:
    Buffer  obj_name;
    int     num_obj;                    // nextに続くOBJ（SHELL）データの総数．        

    bool    phantom_out;
    bool    no_offset;
    int     engine;

    bool    forUnity;
    bool    forUE;

    AffineTrans<double>* affineTrans;

    OBJData* next;
    OBJFacetGeoNode* geo_node;
    OBJFacetMtlNode* mtl_node;

public:
    void    init(int n); 
    void    free(void); 
    void    delete_next(void);

    void    setUnity(bool b) { this->forUnity = b;}
    void    setUE(bool b)    { this->forUE    = b;}
    void    setEngine(int);

    void    setAffineTrans (AffineTrans<double> a) { delAffineTrans(); affineTrans = new AffineTrans<double>(); affineTrans->dup(a);}
    void    delAffineTrans (void) { freeAffineTrans(this->affineTrans);}

    Vector<double> execAffineTrans(void);

public:
    void    addShell(MeshObjectData* shelldata, bool collider);
    void    closeSolid(void) {}

    //void    outputFile(const char* fn, const char* out_path, const char* ptm_dirn, const char* tex_dirn, const char* mtl_dirn);
    //void    output_mtl(char* fn, char* out_dirn, char* ptm_dirn, char* tex_dirn, char* bin_dirn);
    //void    output_obj(char* fn, char* out_dirn, char* ptm_dirn, char* tex_dirn, char* bin_dirn);
    void    outputFile(const char* fn, const char* out_path, const char* tex_dirn, const char* mtl_dirn);
    void    output_mtl(char* fn, char* out_dirn, char* tex_dirn, char* bin_dirn);
    void    output_obj(char* fn, char* out_dirn, char* tex_dirn, char* bin_dirn);
    //void    output_mtl(const char* mtl_path, const char* tex_dirn);
    //void    output_obj(const char* obj_path, const char* mtl_path);
};


inline void  freeOBJData(OBJData* obj) { if(obj!=NULL) { obj->free(); delete obj; obj=NULL;} }



//////////////////////////////////////////////////////////////////////////////////////////
//
class  OBJFacetGeoNode
{
public:
    OBJFacetGeoNode() { this->init();}
    virtual ~OBJFacetGeoNode(void) { this->free();}

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
    virtual ~OBJFacetMtlNode(void) { this->free();}

public:
    Buffer  material;
    bool    same_material;
    MaterialParam material_param;

    Buffer  map_kd;
    Buffer  map_ks;
    Buffer  map_bump;

    Vector<double> kd;
    Vector<double> ka;
    Vector<double> ks;
    //Vector<double> ke;
    //Vector<double> ns;

    double  dd;
    double  ni;
    int     illum;

    OBJFacetMtlNode* next;

public:
    void    init(void);
    void    free(void); 
    void    delete_next(void);
    void    setup_params(void);
};


}

#endif
