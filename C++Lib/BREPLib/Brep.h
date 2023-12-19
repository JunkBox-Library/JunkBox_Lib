#ifndef __JBXL_BREP_H_
#define __JBXL_BREP_H_


/**
@brief    BREP Library Header
@file     Brep.h
@version  1.2.0

@sa "AN INTRODUCTION TO SOLID MODELING", M.MANTYLA, COMPUTER SCIENCE PRESS (1988), ISBN 0-88175-108-1
@sa Modified BREPLIBRARY http://breplibrary.sourceforge.net/ 
@sa The license of this library is subject to the license of BREPLIBRARY.
*/


#ifdef WIN32
#pragma warning(disable:4251)
#endif


#include "ClassBox.h"

#include <list>
#include <algorithm>

#include "Vector.h"
#include "TVector.h"



namespace jbxl {


class  DllExport BREP_SOLID;
class  DllExport BREP_SHELL;
class  DllExport BREP_FACET;
class  DllExport BREP_CONTOUR;
class  DllExport BREP_EDGE;
class  DllExport BREP_WING;
class  DllExport BREP_VERTEX;
class  DllExport OctreeNode;

typedef std::list<BREP_SOLID*>   BREP_SOLID_LIST;
typedef std::list<BREP_SHELL*>   BREP_SHELL_LIST;
typedef std::list<BREP_FACET*>   BREP_FACET_LIST;
typedef std::list<BREP_CONTOUR*> BREP_CONTOUR_LIST;
typedef std::list<BREP_EDGE*>    BREP_EDGE_LIST;
typedef std::list<BREP_WING*>    BREP_WING_LIST;
typedef std::list<BREP_VERTEX*>  BREP_VERTEX_LIST;

DllExport BREP_WING* CreateWingWithoutContour(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2);
DllExport BREP_WING* GetWingOtherSide(BREP_WING* wing);

DllExport BREP_EDGE* FindEdge(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2);
DllExport BREP_EDGE* CreateEdge(BREP_VERTEX* v1, BREP_VERTEX* v2);

DllExport BREP_VERTEX*  AddVertex2Octree(BREP_VERTEX* vertex, OctreeNode* octree, bool dupli=false);
DllExport BREP_VERTEX** GetOctreeVertices(OctreeNode* octree, long int* vertexno);      // 要 free
DllExport long int OctreeGetter(OctreeNode* p, BREP_VERTEX** vtx, long int counter);

DllExport void ConnectWingToVertex(BREP_WING* wing);
DllExport void DestroyWing(BREP_WING* wing);
DllExport int  CompareVertex(BREP_VERTEX* v1, BREP_VERTEX* v2);

DllExport TVector<double>  Vertex2TVector(BREP_VERTEX* v);



//////////////////////////////////////////////////////////////////////////////////
// BREP_SOLID
//

class  DllExport BREP_SOLID 
{
public:
    unsigned int    facetno;            ///< 面の数
    unsigned int    vertexno;           ///< 頂点の数
    unsigned int    vertexid;           ///< 頂点のカウンタ．Vertex のIDを決める際に使用
    int             vcount;             ///< １面あたりの頂点数

    BREP_SHELL_LIST shells;
    OctreeNode* octree;
    RBound<double>  rbound;

    BREP_CONTOUR_LIST contours;         ///< Contours リスト
    BREP_WING_LIST    wings;            ///< Wings リスト

    BREP_CONTOUR_LIST surplus_contours; ///< 過剰 Contours リスト
    BREP_WING_LIST    shortage_wings;   ///< 不足 Wings リスト

    std::string name;
    CVCounter*  counter;                ///< 仮想計量カウンタ

    bool        freed;                  ///< 開放済み確認フラグ

public:
    BREP_SOLID();
    virtual ~BREP_SOLID();

    void  FreeData(void);
    void  CloseData(void);
    void  ConnectShell(BREP_SHELL* shell);
    void  DisconnectShell(BREP_SHELL* shell);
};


inline void  freeBrepSolid(BREP_SOLID*& solid) { if(solid!=NULL){ solid->FreeData(); delete solid; solid=NULL;} }



//////////////////////////////////////////////////////////////////////////////////
// BREP_SHELL
//

class DllExport BREP_SHELL
{
public:
    BREP_SOLID*     solid;
    BREP_FACET_LIST facets;
    RBound<double>  rbound;

public:
    BREP_SHELL(BREP_SOLID* pr_solid);
    virtual ~BREP_SHELL();

    void CloseData();
    void ConnectFacet(BREP_FACET* facet);
    void DisconnectFacet(BREP_FACET* facet);
};



//////////////////////////////////////////////////////////////////////////////////
// BREP_FACET
//

class DllExport BREP_FACET
{
public:
    BREP_SHELL*     shell;
    BREP_CONTOUR_LIST outer_contours;

    Vector<double>  normal;
    RBound<double>  rbound;

    double          d;                  ///< 定数，原点から平面への距離*(-1)
    double          tolerance;

    bool            deletable;          ///< 削除できる
    bool            notdelete;          ///< 削除してはいけない

public:
    BREP_FACET(BREP_SHELL* pr_shell);
    virtual ~BREP_FACET();

    void CloseData();
    void ConnectContour(BREP_CONTOUR* contour);
    void DisconnectContour(BREP_CONTOUR* contour);

    void ComputePlaneEquation();
};



//////////////////////////////////////////////////////////////////////////////////
// BREP_CONTOUR
//

class DllExport BREP_CONTOUR
{
public:
    BREP_FACET*     facet;
    BREP_WING*      wing;

    RBound<double>  rbound;
    Vector<double>  normal;

    /// △QRS  三角Contour衝突検出用
    TVector<double> directR;
    TVector<double> directS;
    TVector<double> directRS;

    int             dup_edge;               ///< 多重エッジの数．
    bool            collision;
    bool            hasCollisionVector;

public:
    BREP_CONTOUR(BREP_FACET* pr_facet);
    virtual ~BREP_CONTOUR();

    void CloseData();
    void ConnectWing(BREP_WING* wing);
    void DisconnectWing(BREP_WING* wing);
    void DestroyWings();

    void CloseFacet(BREP_FACET* facet, double& emax, double& dmin, double& dmax);
    void ComputeNormal();
    void ComputeDirectRS();

    BREP_WING* CreateWing(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2);
};



//////////////////////////////////////////////////////////////////////////////////
// BREP_WING
//

class DllExport BREP_WING
{
public:
    BREP_VERTEX*    vertex;     ///< Start of Vertex
    BREP_WING*      prev;
    BREP_WING*      next;

    BREP_EDGE*      edge;
    BREP_CONTOUR*   contour;

public:
    BREP_WING(BREP_VERTEX* v);
    virtual ~BREP_WING() {}

    void CloseData() {}
};



//////////////////////////////////////////////////////////////////////////////////
// BREP_EDGE
//

class DllExport BREP_EDGE
{
public: 
    BREP_WING*      wing1;
    BREP_WING*      wing2;

    Vector<double>  center;
    BREP_EDGE_LIST* edge_list;  ///< 多重エッジのリストへのポインタ

    bool            complete;   ///< 完全なエッジ．２つの Wingは共に使用されている．
    double          tolerance;

public:
    BREP_EDGE(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2);
    virtual ~BREP_EDGE();

    void CloseData();
};



//////////////////////////////////////////////////////////////////////////////////
// BREP_VERTEX
//

class DllExport BREP_VERTEX
{
public:
    BREP_WING_LIST  wing_list;

    Vector<double>  point;                  ///< 頂点の座標．
    Vector<double>  normal;                 ///< 法線ベクトル．周りの Contour の法線ベクトルの平均．
    UVMap<double>   uvmap;                  ///< 曲面のUV座標

    // optional
    long int        index;                  ///< シーケンシャルに増加する一意的な番号．Octree に格納されるときに設定される．
    bool            calc_normal;            ///< normal を計算するか? しない場合は入力値を使用する．

    double          distance2;
    double          tolerance;

    BREP_VERTEX_LIST*  forbidden_list;      ///< お互いに Edgeを張ることを禁止された Vertexの List

public:
    BREP_VERTEX();
    virtual ~BREP_VERTEX();

    void CloseData();
    void DisconnectWing(BREP_WING* wing);

    void ComputeNormal();
    void ComputeTolerance();
};



//////////////////////////////////////////////////////////////////////////////////
// OctreeNode
//

class DllExport OctreeNode
{
public:
    BREP_SOLID*  solid;

private:
    BREP_VERTEX* vertex;        ///< ルートノードの場合は NULL
    OctreeNode*  child[8];

public:
    /// OctreeNodeのコンストラクタは，BREP_VERTEXに対して，新たに領域を作らずにポインタをコピーすることに注意!!
    OctreeNode(BREP_VERTEX* new_vertex, BREP_SOLID* sld);
    ~OctreeNode();
 
    ///  Octreeに Vertex new_vertexそのものを登録する．登録した OctreeNodeを返す．
    ///  既に同じ位置のVertexが登録済み（UVマップも比べる）の場合は，その OctreeNodeを返す．
    OctreeNode* AddWithUnique(BREP_VERTEX* new_vertex);

    ///  Octreeに Vertex new_vertex そのものを登録する．重複登録を許す．
    ///  登録した OctreeNode を返す．
    OctreeNode* AddWithDuplicates(BREP_VERTEX* new_vertex);

    ///  Vertex element と同じ位置にある Vertexを含むノードを返す．
    OctreeNode* FindSubtree(BREP_VERTEX* element);
    
    // 
    void ComputeVerticesNormal(void);

    //
    friend  BREP_VERTEX** GetOctreeVertices(OctreeNode* octree, long int* vertexno);
    friend  long int OctreeGetter(OctreeNode* p, BREP_VERTEX** vtx, long int counter);

    friend  BREP_VERTEX* AddVertex2Octree(BREP_VERTEX* vertex, OctreeNode* octree, bool dupli);
};


}       // namespace


#endif

