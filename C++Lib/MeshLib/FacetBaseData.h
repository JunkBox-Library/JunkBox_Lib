#ifndef  __JBXL_FACET_BASE_H_
#define  __JBXL_FACET_BASE_H_

/**

3D Facet用基本データ

*/

#include <vector>
#include <algorithm>

#include "Vector.h"
#include "Rotation.h"
#include "buffer.h"


namespace  jbxl {


class   FacetTriIndex;
class   FacetTriData;

class   FacetBaseData;
class   TriPolyData;


typedef std::vector<Vector<double> > FACET_VECTOR_ARRAY;
typedef std::vector<Vector<float> >  FACET_VECTOR_ARRAY32;
typedef std::vector<FacetTriIndex>   FACET_TRIINDX_ARRAY;
typedef std::vector<FacetTriData>    FACET_TRIDATA_ARRAY;



///////////////////////////////////////////////////////////////////////////////////////////////
// Index of Triangle Polygon Data
//

class  FacetTriIndex
{
public:
    int  v1,  v2,  v3;  // Vertex Index
    int  n1,  n2,  n3;  // Normal Index
    int uv1, uv2, uv3;  // UVmap  Index

public:
    FacetTriIndex(int w1=0,int w2=0,int w3=0,int m1=0,int m2=0,int m3=0) { init(w1,w2,w3,m1,m2,m3);}
    virtual ~FacetTriIndex(void) {}

    void init(int w1=0,int w2=0,int w3=0,int m1=0,int m2=0,int m3=0,int u1=0,int u2=0,int u3=0) { set(w1,w2,w3,m1,m2,m3,u1,u2,u3);}
    void set (int w1=0,int w2=0,int w3=0,int m1=0,int m2=0,int m3=0,int u1=0,int u2=0,int u3=0);
    void mlt_set(int d1=0,int d2=0,int d3=0);

public:
    Vector<double> SurfaceNormal(FACET_VECTOR_ARRAY* coords) { 
        Vector<double> normal = NewellMethod<double>((*coords)[v1],(*coords)[v2],(*coords)[v3]);
        return normal.normalize();
    }

    //
    Vector<float> SurfaceNormal(FACET_VECTOR_ARRAY32* coords) {
        Vector<float> normal = NewellMethod<float>((*coords)[v1], (*coords)[v2], (*coords)[v3]);
        return normal.normalize();
    }
};



///////////////////////////////////////////////////////////////////////////////////////////////
// Triangle Polygon Data
//

class  FacetTriData
{
public:
    int facetNum;

    Vector<double>  v1,  v2,  v3;
    Vector<double>  n1,  n2,  n3;
    UVMap<double>   uv1, uv2, uv3;

public:
    FacetTriData(int n=0) { init(); facetNum = n;}
    virtual ~FacetTriData(void) {}

    void  init(void);

public:
    void  execScale(double x, double y, double z);
    void  execShift(double x, double y, double z) { Vector<double> m(x, y, z); v1 = v1 + m; v2 = v2 + m; v3 = v3 + m;}
    void  execRotate(Quaternion<double> q);
    void  ComputeTriNormal() { Vector<double> nv = NewellMethod(v1, v2, v3); nv.normalize(); n1 = n2 = n3 = nv;}
};



//////////////////////////////////////////////////////////////////////////////////////
// Facet Base Data (インデックス化されたデータ)
//

class  FacetBaseData
{
public:
    int     num_index;              ///< インデックスの数．(index の要素数）
    int     num_data;               ///< データ数．（vertex, normal, texcrd の要素数）
    int     vcount;                 ///< ポリゴンの頂点数．通常は3 

    int*    index;                  ///< インデックスデータ
    Vector<double>* vertex;         ///< 頂点データ
    Vector<double>* normal;         ///< 法線ベクトル
    UVMap<double>*  texcrd;         ///< テクスチャマップ

public:
    FacetBaseData(int idx=0, int num=0) { init(idx, num);}
    virtual ~FacetBaseData(void) {} 

    void  init(int idx=0, int num=0);
    void  free(void);
    bool  getm(void);
    void  dup(FacetBaseData a);

public:
    void  execScale(Vector<double> scale);
    void  execShift(Vector<double> shift);
    void  execRotate(Quaternion<double> quat);
};


inline void  freeFacetBaseData(FacetBaseData*& facet) { if(facet!=NULL){ facet->free(); delete facet; facet = NULL;}}



//////////////////////////////////////////////////////////////////////////////////////
//  Triangle Polygon Data
//

class  TriPolyData
{
public:
    int             facetNum;       ///< 面番号
    bool            has_normal;     ///< 配列データの場合，一番最初のデータが値を持っていれば十分である．
    bool            has_texcrd;     ///< 配列データの場合，一番最初のデータが値を持っていれば十分である．

    Vector<double>  vertex[3];
    Vector<double>  normal[3];
    UVMap<double>   texcrd[3];

public:
    TriPolyData(void) { init();}
    virtual ~TriPolyData(void) {}   

    void  init(void);
    void  free(void) { init();}
    void  dup(TriPolyData a);

public:
    void  execScale(Vector<double> scale);
    void  execShift(Vector<double> shift);
    void  execRotate(Quaternion<double> quat);
};


TriPolyData* dupTriPolyData(TriPolyData* data, int num);
TriPolyData* joinTriPolyData(TriPolyData*& first, int num_f, TriPolyData*& next, int num_n);

inline void  freeTriPolyData(TriPolyData*& tridata) { if(tridata!=NULL){ tridata->free(); delete tridata; tridata = NULL;}}
void  freeTriPolyData(TriPolyData*& tridata, int n);



}       // namespace

#endif
