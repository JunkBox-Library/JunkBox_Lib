#ifndef  __JBXL_CPP_OBJ_TOOL_H_
#define  __JBXL_CPP_OBJ_TOOL_H_

/**
  3D OBJ ファイル用ツール

*/

#include  "tools++.h"
#include  "txml.h"

#include  "TriBrep.h"
#include  "Rotation.h"
#include  "ContourBaseData.h"
#include  "MaterialParam.h"
#include  "MeshObjectData.h"


namespace jbxl {


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
    OBJData(int n=0) { init(n);}    // デフォルト（n=0）ではアンカーを作る
    virtual ~OBJData(void);

public:
    Buffer obj_name;
    bool   collider;

    int num_obj;

    OBJData* next;
    OBJFacetGeoNode* geo_node;
    OBJFacetMtlNode* mtl_node;

    AffineTrans<double>* affine_trans;

public:
    void    init(int n); 
    void    delete_next(void);

    void    setAffineTrans(AffineTrans<double> a) { delAffineTrans(); affine_trans = new AffineTrans<double>(); affine_trans->dup(a);}
    void    delAffineTrans(void) { freeAffineTrans(affine_trans);}
    void    addObject(MeshObjectData* meshdata, bool collider);
};


//////////////////////////////////////////////////////////////////////////////////////////
//
class  OBJFacetGeoNode
{
public:
    OBJFacetGeoNode() { init();}
    virtual ~OBJFacetGeoNode(void);

public:
    Buffer  material;

    Vector<double>* vv;
    Vector<double>* vn;
    UVMap<double>*  vt;

    AffineTrans<double>* uvmap_trans;

    OBJFacetGeoNode* next;
//    OBJFacetGeoNode* prev;

public:
    void    init(void);
    void    delete_next(void);
};


//////////////////////////////////////////////////////////////////////////////////////////
//
class  OBJFacetMtlNode
{
public:
    OBJFacetMtlNode() { init();}
    virtual ~OBJFacetMtlNode(void);

public:
    Buffer  material;

    bool    same_material;
    MaterialParam material_param;

    Vector<double> ka;
    Vector<double> kd;
    Vector<double> ks;
    Vector<double> ke;
    Vector<double> ni;
    Vector<double> ns;

    double  dd;
    int     illum;
    Buffer  map_kd;

    OBJFacetMtlNode* next;
//    OBJFacetMtlNode* prev;

public:
    void    init(void);
    void    delete_next(void);
};


}

#endif
