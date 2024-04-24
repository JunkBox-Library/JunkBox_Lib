#ifndef  __JBXL_CPP_MESHOBJECTDATA_H_
#define  __JBXL_CPP_MESHOBJECTDATA_H_

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


class  MeshFacetNode;
class  MeshObjectData;


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

    Vector<double>* vertex_value;   ///< 頂点データの並び．要素数は num_vertex
    Vector<double>* normal_value;   ///< 法線ベクトルデータの並び．要素数は num_vertex
    UVMap<double>*  texcrd_value;   ///< テクスチャマップの並び．要素数は num_texcrd

    MeshFacetNode* next;
    MeshFacetNode* prev;

public:
    MeshFacetNode(void) { init();}
    virtual ~MeshFacetNode(void) {}

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
    bool    computeVertexDirect(Vector<double>* vtx, Vector<double>* nml, UVMap<double>* map, int num, int vcount=3);
    bool    computeVertexByBREP(Vector<double>* vtx, Vector<double>* nml, UVMap<double>* map, int num, int vcount=3);
};


inline void  freeMeshFacetNode(MeshFacetNode*& node) { if(node!=NULL) { node->free(); delete node; node=NULL;} }
void  freeMeshObjectList(MeshFacetNode*& node);

MeshFacetNode*  DelMeshFacetNode(MeshFacetNode* node);
MeshFacetNode*  AddMeshFacetNode(MeshFacetNode* list, MeshFacetNode* node);



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MeshObject Data: for Object
//

/**
class  MeshObjectData

*/
class  MeshObjectData
{
public:
    Buffer  data_name;                  ///< データ名

    int     ttl_index;                  ///< インデックスの総数（実質的データ数）
    int     ttl_vertex;                 ///< 頂点データの総数．
    int     ttl_texcrd;                 ///< テクスチャ画像の座標総数．通常は ttl_vertexと同じ値．
    int     num_node;                   ///< テクスチャー単位の面の数（Node の数）
    int     num_vcount;                 ///< 1ポリゴン あたりの頂点数．現在は 3のみサポート

    MeshFacetNode* facet;               ///< FACETデータのリストへのポインタ
    MeshFacetNode* facet_end;           ///< FACETデータのリストの最後のデータへのポインタ

    AffineTrans<double>* affineTrans;   ///< アフィン変換．ここで使用するのは，shift, rotate, scale(size) のみ

private: // 入力データの作業用
    int     num_import;                 ///< 入力データの数

    Vector<double>* impvtx_value;       ///< 入力された頂点データ．3個で 1ポリゴンを表現．法線方向は右手順．
    Vector<double>* impnrm_value;       ///< 入力された頂点の法線ベクトル．impvtx_value と対応．
    UVMap<double>*  impmap_value;       ///< 入力されたテクスチャ座標データ．impvtx_value と対応．

public:
    MeshObjectData(const char* name=NULL) { init(name);}
    virtual ~MeshObjectData(void)   {}

    void    init(const char* name=NULL);
    void    free(void);
    void    free_value(void);
    void    clear(void);

    //
    void    setName(const char* str) { free_Buffer(&data_name); data_name=make_Buffer_str(str);}
    char*   getName(void) { return _tochar(data_name.buf);}
    
    void    setAffineTrans(AffineTrans<double> a) { delAffineTrans(); affineTrans = new AffineTrans<double>(); affineTrans->dup(a);}
    void    delAffineTrans(void) { freeAffineTrans(affineTrans);}

public:
    bool    addData(ContourBaseData* facetdata, MaterialParam* param);
    bool    addData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, int vnum, MaterialParam* param, bool useBrep);
    bool    addData(TriPolygonData* tridata, int tnum, int fnum, MaterialParam* param, bool useBrep);  ///< 処理するFACETを選択できる

    void    joinData(MeshObjectData*& data);    ///< data は削除される．
    void    setMaterialParam(MaterialParam param, int num=-1);
    //
    bool    importTriData(TriPolygonData* tridata, int tnum, int fnum=-1);
    bool    importTriData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, int vnum);
    bool    addNode(ContourBaseData* facetdata, const char* name, MaterialParam* param);
    bool    addNode(const char* name, MaterialParam* param, bool useBrep);
};


inline void  freeMeshObjectData(MeshObjectData*& data) { if(data!=NULL) { data->free(); delete data; data=NULL;} }


}       // namespace

#endif

