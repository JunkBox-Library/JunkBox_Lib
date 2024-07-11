#ifndef  __JBXL_CPP_MESHOBJECTDATA_H_
#define  __JBXL_CPP_MESHOBJECTDATA_H_

#include  "MeshFacetNode.h"


namespace jbxl {


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MeshObject Data: for Object
//    3Dオブジェクトの最終的なデータ形式
//        ContourTriData + ContourTriIndex -> MeshFacetNode -> MeshObjectData  (for Prim or Sculpt Prim)
//        TriPolygonData + ContourTriIndex -> MeshFacetNode -> MeshObjectData
//        ContourBaseData -> MeshFacetNode -> MeshObjectData

/**
class  MeshObjectData

*/
class  MeshObjectData
{
public:
    Buffer  data_name;                  ///< データ名
    Buffer  alt_name;                   ///< 別名

    int     ttl_index;                  ///< インデックスの総数（実質的データ数）
    int     ttl_vertex;                 ///< 頂点データの総数．
    int     ttl_texcrd;                 ///< テクスチャ画像の座標総数．通常は ttl_vertexと同じ値．
    int     num_node;                   ///< テクスチャー単位の面の数（Node の数）
    int     num_vcount;                 ///< 1ポリゴン あたりの頂点数．現在は 3のみサポート

    MeshFacetNode* facet;               ///< FACETデータ（１面のポリゴンデータ）のリストへのポインタ
    MeshFacetNode* facet_end;           ///< FACETデータのリストの最後のデータへのポインタ

    AffineTrans<double>* affineTrans;   ///< アフィン変換．ここで使用するのは，shift, rotate, scale(size) のみ

private: // 入力データの作業用
    int     num_import;                 ///< 入力データの数

    Vector<double>*  impvtx_value;      ///< 入力された頂点データ．3個で 1ポリゴンを表現．法線方向は右手順．
    Vector<double>*  impnrm_value;      ///< 入力された頂点の法線ベクトル．impvtx_value と対応．
    UVMap<double>*   impmap_value;      ///< 入力されたテクスチャ座標データ．impvtx_value と対応．
    ArrayParam<int>* impwgt_value;      ///< 入力された頂点の重み．Jointを持つデータに使用される．

public:
    MeshObjectData(const char* name=NULL) { init(name);}
    virtual ~MeshObjectData(void)   { free();}

    void    init(const char* name=NULL);
    void    free(void);
    void    free_value(void);
    void    clear(void);

    //
    void    setName(const char* str) { free_Buffer(&data_name); data_name=make_Buffer_str(str); canonical_filename_Buffer(&data_name);}
    char*   getName(void) { return _tochar(data_name.buf);}
    void    setAltName(const char* str) { free_Buffer(&alt_name); alt_name=make_Buffer_str(str); canonical_filename_Buffer(&alt_name);}
    char*   getAltName(void) { return _tochar(alt_name.buf);}
    
    void    setAffineTrans(AffineTrans<double> a) { delAffineTrans(); affineTrans = new AffineTrans<double>(); affineTrans->dup(a);}
    void    delAffineTrans(void) { freeAffineTrans(affineTrans);}

public:
    //      addData(){addNode();}
    bool    addData(ContourBaseData* facetdata, MaterialParam* param);
    bool    addNode(ContourBaseData* facetdata, const char* name, MaterialParam* param);
    
    //      addData(){importTriData(); addNode();}
    bool    addData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, ArrayParam<int>* wgt, int vnum, MaterialParam* param, bool useBrep);
    bool    addData(TriPolygonData* tridata, int tnum, int fnum, MaterialParam* param, bool useBrep);  ///< 処理するFACETを選択できる
    bool    importTriData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, ArrayParam<int>* wgt, int vnum);
    bool    importTriData(TriPolygonData* tridata, int tnum, int fnum=-1);
    bool    addNode(const char* name, MaterialParam* param, bool useBrep);
    //
    void    joinData(MeshObjectData*& data);    ///< data は削除される．
    void    setMaterialParam(MaterialParam param, int num=-1);
};


inline void  freeMeshObjectData(MeshObjectData*& data) { if(data!=NULL) { data->free(); delete data; data=NULL;} }


}       // namespace

#endif

