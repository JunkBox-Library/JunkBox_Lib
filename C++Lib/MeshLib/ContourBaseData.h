#ifndef  __JBXL_CONTOUR_BASE_H_
#define  __JBXL_CONTOUR_BASE_H_

/**

3D Contour用基本データ

*/

#include <vector>
#include <algorithm>

#include "xLib/llsd_tool.h"
#include "Vector.h"
#include "Rotation.h"
#include "buffer.h"


namespace  jbxl {


class   ContourTriIndex;
class   ContourTriData;

class   ContourBaseData;
class   TriPolygonData;


typedef std::vector<Vector<double> >   CONTOUR_VECTOR_ARRAY;
typedef std::vector<Vector<float> >    CONTOUR_VECTOR_ARRAY32;
typedef std::vector<ContourTriIndex>   CONTOUR_TRIINDX_ARRAY;
typedef std::vector<ContourTriData>    CONTOUR_TRIDATA_ARRAY;



///////////////////////////////////////////////////////////////////////////////////////////////
// Index of Triangle Polygon Data
//    ContourTriData を設定するためのインデックス
//

class  ContourTriIndex
{
public:
    int  v1,  v2,  v3;  // Vertex/Weight Index
    int  n1,  n2,  n3;  // Normal Index
    int uv1, uv2, uv3;  // UVmap  Index

public:
    ContourTriIndex(int w1=0,int w2=0,int w3=0,int m1=0,int m2=0,int m3=0) { init(w1,w2,w3,m1,m2,m3);}
    virtual ~ContourTriIndex(void) {}

    void init(int w1=0,int w2=0,int w3=0,int m1=0,int m2=0,int m3=0,int u1=0,int u2=0,int u3=0) { set(w1,w2,w3,m1,m2,m3,u1,u2,u3);}
    void set (int w1=0,int w2=0,int w3=0,int m1=0,int m2=0,int m3=0,int u1=0,int u2=0,int u3=0);
    void mlt_set(int d1=0,int d2=0,int d3=0);

public:
    Vector<double> SurfaceNormal(CONTOUR_VECTOR_ARRAY* coords) { 
        Vector<double> normal = NewellMethod<double>((*coords)[v1],(*coords)[v2],(*coords)[v3]);
        return normal.normalize();
    }

    Vector<float> SurfaceNormal(CONTOUR_VECTOR_ARRAY32* coords) {
        Vector<float> normal = NewellMethod<float>((*coords)[v1], (*coords)[v2], (*coords)[v3]);
        return normal.normalize();
    }
};


///////////////////////////////////////////////////////////////////////////////////////////////
// Triangle Contour Data
//    ContourTriIndex をインデックスとした 3角ポリゴンデータ
//    ex.) tri_data.v1 = coords[tri_indx.v1];
//    注)  OpenSimulator の Prim, Sculpt Prim の解析に使用する．

class  ContourTriData
{
public:
    int contourNum;

    Vector<double>  v1,  v2,  v3;
    Vector<double>  n1,  n2,  n3;
    UVMap<double>   uv1, uv2, uv3;
    llsd_weight     w1,  w2,  w3;

public:
    ContourTriData(int n=0) { init(); contourNum = n;}
    virtual ~ContourTriData(void) {}

    void  init(void);

public:
    void  execScale(double x, double y, double z);
    void  execShift(double x, double y, double z) { Vector<double> m(x, y, z); v1 = v1 + m; v2 = v2 + m; v3 = v3 + m;}
    void  execRotate(Quaternion<double> q);
    void  ComputeTriNormal() { Vector<double> nv = NewellMethod(v1, v2, v3); nv.normalize(); n1 = n2 = n3 = nv;}
};


//////////////////////////////////////////////////////////////////////////////////////
// Contour Base Data (インデックス化されたデータ)
//      ContourTriIndex +  ContourTriData
//

class  ContourBaseData
{
public:
    int     num_index;              ///< インデックスの数．(index の要素数）
    int     num_data;               ///< データ数．（vertex, normal, texcrd, weight の要素数）
    int     vcount;                 ///< ポリゴンの頂点数．通常は3 

    int*            index;          ///< インデックスデータ
    Vector<double>* vertex;         ///< 頂点データ       vertex[index[0]], vertex[index[1]], vertex[index[2]], ... の順に並ぶ
    Vector<double>* normal;         ///< 法線ベクトル     normal[index[0]], normal[index[1]], normal[index[2]], ... の順に並ぶ
    UVMap<double>*  texcrd;         ///< テクスチャマップ texcrd[index[0]], texcrd[index[1]], texcrd[index[2]], ... の順に並ぶ
    llsd_weight*    weight;         ///< Skin の Weight   weight[index[0]], weight[index[1]], weight[index[2]], ... の順に並ぶ

public:
    ContourBaseData(int idx=0, int num=0) { init(idx, num);}
    virtual ~ContourBaseData(void) {} 

    void  init(int idx=0, int num=0);
    void  free(void);
    bool  getm(void);
    void  dup(ContourBaseData a);

public:
    void  execScale(Vector<double> scale);
    void  execShift(Vector<double> shift);
    void  execRotate(Quaternion<double> quat);
};


inline void  freeContourBaseData(ContourBaseData*& contour) { if(contour!=NULL){ contour->free(); delete contour; contour = NULL;}}


//////////////////////////////////////////////////////////////////////////////////////
//  Triangle Polygon Data
//      ContourTriData の別表現
//

class  TriPolygonData
{
public:
    int             polygonNum;     ///< ポリゴン番号
    bool            has_normal;     ///< 配列データの場合，一番最初のデータが値を持っていれば十分である．
    bool            has_texcrd;     ///< 配列データの場合，一番最初のデータが値を持っていれば十分である．
    bool            has_weight;     ///< 配列データの場合，一番最初のデータが値を持っていれば十分である．

    Vector<double>  vertex[3];
    Vector<double>  normal[3];
    UVMap<double>   texcrd[3];
    llsd_weight     weight[3];

public:
    TriPolygonData(void) { init();}
    virtual ~TriPolygonData(void) {}   

    void  init(void);
    void  free(void) { init();}
    void  dup(TriPolygonData a);

public:
    void  execScale(Vector<double> scale);
    void  execShift(Vector<double> shift);
    void  execRotate(Quaternion<double> quat);
    void  ComputeTriNormal() { Vector<double> nv = NewellMethod(vertex[0], vertex[1], vertex[2]); nv.normalize(); normal[0] = normal[1] = normal[2] = nv;}
};


TriPolygonData*  dupTriPolygonData(TriPolygonData* data, int num);
TriPolygonData*  joinTriPolygonData(TriPolygonData*& first, int num_f, TriPolygonData*& next, int num_n);

inline void   freeTriPolygonData(TriPolygonData*& tridata) { if(tridata!=NULL){ tridata->free(); delete tridata; tridata = NULL;}}
void   freeTriPolygonData(TriPolygonData*& tridata, int n);



}       // namespace

#endif
