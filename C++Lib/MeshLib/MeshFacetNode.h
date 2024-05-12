#ifndef  __JBXL_CPP_MESHFACETNODE_H_
#define  __JBXL_CPP_MESHFACETNODE_H_

#include  "tools++.h"
#include  "txml.h"

#include  "TriBrep.h"
#include  "Rotation.h"
#include  "AffineTrans.h"
#include  "ContourBaseData.h"
#include  "MaterialParam.h"


namespace jbxl {


//#define   JBXL_MATERIALID_RAND_LEN  8
#define   JBXL_MATERIAL_PREFIX      "#MATERIAL_"


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MeshFacetNode: for Facet Data 
//

/**
@brief MeshObject の Polygonデータ（1面）を格納するクラス．リスト構造を取る．

data_index[i*3], data_index[i*3+1], data_index[i*3+2] が示す vertex_value, normal_value, texcrd_value で一つの面（3角ポリゴン）を形成する．
例えば，vertex_value[data_index[i*3]], vertex_value[data_index[i*3+1]], vertex_value[data_index[i*3+2]] で3角ポリゴン座標を表す．

MeshObjectNode -> MeshFacetNode
*/
class  MeshFacetNode
{
public:
    Buffer  material_id;            ///< マテリアルを識別するID．JBXL_MATERIAL_PREFIX で始まる．
    bool    same_material;          ///< 他の Node が既に同じマテリアルを使用している．
    int     facet_no;               ///< 面（Polygon）番号

    MaterialParam material_param;   ///< マテリアルパラメータ

    int     num_index;              ///< 頂点の延べ数．num_polygon*MeshObjectData::num_vcount (num_polygon*3)（data_index の要素数）
    int     num_polygon;            ///< ポリゴンの数
    int     num_vertex;             ///< 頂点のデータ数．（vertex_value, normal_value の要素数）
    int     num_texcrd;             ///< テクスチャ画像の座標数．通常は num_vertex に等しい．（texcrd_value の要素数）

    int*    data_index;             ///< インデックスデータ．要素数は num_index

    Vector<double>*  vertex_value;  ///< 頂点データの並び．要素数は num_vertex
    Vector<double>*  normal_value;  ///< 法線ベクトルデータの並び．要素数は num_vertex
    UVMap<double>*   texcrd_value;  ///< テクスチャマップの並び．要素数は num_texcrd
    ArrayParam<int>* weight_value;  ///< 頂点の重み．Jointを持つデータに使用される．要素数は num_vertex

    MeshFacetNode* next;
    MeshFacetNode* prev;

public:
    MeshFacetNode(void) { init();}
    virtual ~MeshFacetNode(void) { free();}

    void    init(void);
    void    free(void);
    void    free_value(void);
    void    clear(void);

    void    set (int vertex,   int polygon,   int vcount=3);
    bool    getm(int vertex=0, int polygon=0, int vcount=0);    ///< メモリの確保

    //
    void    setFacetNo(int no) { if (no>=0) facet_no = no;}
    void    setMaterialParam(MaterialParam param);
    void    delMaterialParam(void) { material_param.clear();}

    void    setMaterialID(const char* str);
    void*   getMaterialID(void) { return _tochar(material_id.buf);}

    void    execAffineTransUVMap(UVMap<double>* uvmap=NULL, int num=-1);
    UVMap<double>*  generatePlanarUVMap(Vector<double> scale, UVMap<double>* uvmap=NULL);

public:
    bool    computeVertexDirect(ContourBaseData* facetdata);
    bool    computeVertexByBREP(ContourBaseData* facetdata);
    bool    computeVertexDirect(Vector<double>* vtx, Vector<double>* nml, UVMap<double>* map, ArrayParam<int>* wgt, int num, int vcount=3);
    bool    computeVertexByBREP(Vector<double>* vtx, Vector<double>* nml, UVMap<double>* map, ArrayParam<int>* wgt, int num, int vcount=3);
};


inline void  freeMeshFacetNode(MeshFacetNode*& node) { if(node!=NULL) { node->free(); delete node; node=NULL;} }
void  freeMeshObjectList(MeshFacetNode*& node);

MeshFacetNode*  DelMeshFacetNode(MeshFacetNode* node);
MeshFacetNode*  AddMeshFacetNode(MeshFacetNode* list, MeshFacetNode* node);


}       // namespace

#endif

