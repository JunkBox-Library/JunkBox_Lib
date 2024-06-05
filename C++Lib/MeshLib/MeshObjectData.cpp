/**
@brief    Mesh Data用 ツール
@file     MeshObjectData.cpp
@author   Fumi.Iseki (C)
*/

#include  "MeshObjectData.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////
// MeshObjectData クラス
//

void  MeshObjectData::init(const char* name)
{
    data_name    = make_Buffer_str(name);
    alt_name     = make_Buffer_str(name);
    canonical_filename_Buffer(&data_name);
    canonical_filename_Buffer(&alt_name);

    ttl_index    = 0;
    ttl_vertex   = 0;
    ttl_texcrd   = 0;
    num_node     = 0;
    num_vcount   = 3;

    facet        = NULL;
    facet_end    = NULL;
    affineTrans  = NULL;

    num_import   = 0;
    impvtx_value = NULL;
    impnrm_value = NULL;
    impmap_value = NULL;
    impwgt_value = NULL;
}


void  MeshObjectData::free(void)
{
    free_Buffer(&data_name);
    free_Buffer(&alt_name);
    free_value();

    delAffineTrans();

    freeMeshFacetList(facet);
    facet = facet_end = NULL;
}


void  MeshObjectData::free_value(void)
{
    freeNull(impvtx_value);
    freeNull(impnrm_value);
    freeNull(impmap_value);
    freeArrayParams<int>(impwgt_value, num_import);

    impvtx_value = NULL;
    impnrm_value = NULL;
    impmap_value = NULL;
    impwgt_value = NULL;
}


void  MeshObjectData::clear(void)
{
    this->free();
    init();
}


/**
bool  MeshObjectData::addData(ContourBaseData* contours, MaterialParam* param)

インデックス化された ContourBaseDataを importTriData()を介さずに，直接 Nodeデータに書き込む．@n
CONTOUR(ポリゴン)を選択的に処理することはできない．予め CONTOURに分解しておくか，CONTOURが1つのみの場合に使用する．
この後 MeshFacetNode::computeVertexDirect() を使用して頂点データの計算を行う．

@param  contours
@param  param
@retval           true: 処理の成功．false: 処理の失敗．
*/
bool  MeshObjectData::addData(ContourBaseData* contours, MaterialParam* param)
{
    DEBUG_MODE PRINT_MESG("MeshObjectData::addData() for ContourBaseData: start.\n");
    char* name = NULL;
    if (param!=NULL) name = param->getParamString();

    bool ret = addNode(contours, name, param);
    if (ret && param!=NULL) facet_end->setMaterialParam(*param);    // Materialデータを追加

    DEBUG_MODE PRINT_MESG("MeshObjectData::addData() for ContourBaseData: end.\n");
    return ret;
}


/**
bool  MeshObjectData::addData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, ArrayParam<int>* wgt, int vnum, MaterialParam* param, bool useBrep)

指定した頂点ベクトルのデータを追加し，MeshObjectのデータ（通常はCONTOUR すなわちポリゴン単位）を作成する．@n
vct, nrm, map は3個づつ組になって三角ポリゴンを表す．従って vnumは必ず3の倍数になるはず．@n
その後 MeshFacetNode::computeVertexDirect() または MeshFacetNode::computeVertexByBREP() を使用して頂点データの計算を行う．

@param vct      追加対象の頂点座標データへのポインタ
@param nrm      追加対象の頂点の法線ベクトルのデータへのポインタ
@param map      追加対象のテクスチャ座標のデータへのポインタ
@param wgt      頂点の重みデータへのポインタ（オプション）
@param vnum     データ数
@param param    マテリアル用パラメータへのポインタ
@param useBrep  BREPを使用して頂点を配置する．
@retval         true: 処理の成功．false: 処理の失敗．
*/
bool  MeshObjectData::addData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, ArrayParam<int>* wgt, int vnum, MaterialParam* param, bool useBrep)
{
    DEBUG_MODE PRINT_MESG("MeshObjectData::addData() for Vector<>: start.\n");
    bool ret = importTriData(vct, nrm, map, wgt, vnum);
    if (ret) {
        char* name = NULL;
        if (param!=NULL) name = param->getParamString();
        ret = addNode(name, param, useBrep);
    }
    if (ret && param!=NULL) facet_end->setMaterialParam(*param);    // Materialデータを追加

    DEBUG_MODE PRINT_MESG("MeshObjectData::addData() for Vector<>: end.\n");
    return ret;
}


/**
bool  MeshObjectData::addData(TriPolygonData* tridata, int tnum, int pnum, MaterialParam* param, bool useBrep)

TriPolygonData (三角ポリゴンデータ) を単位としてデータを追加し，MeshObjectのデータを作成する．@n
pnum を指定すると，指定されたポリゴンデータのみが追加される．これにより面ごとのデータ構造を形成することができる．@n

@param tridata  追加対象の三角ポリゴンデータへのポインタ
@param tnum     三角ポリゴンデータの数
@param pnum     追加するデータのポリゴン番号（選択的に追加する場合に指定する）．-1以下なら全てのポリゴンデータを追加する．
@param param    マテリアル用パラメータへのポインタ
@param useBrep  BREPを使用して頂点を配置する．
@retval         true: 処理の成功．false: 処理の失敗．
*/
bool  MeshObjectData::addData(TriPolygonData* tridata, int tnum, int pnum, MaterialParam* param, bool useBrep)
{
    DEBUG_MODE PRINT_MESG("MeshObjectData::addData() for TriPolygonData: start.\n");
    bool ret = importTriData(tridata, tnum, pnum);
    if (ret) {
        char* name = NULL;
        if (param!=NULL) name = param->getParamString();
        ret = addNode(name, param, useBrep);
    }
    //
    if (ret) {
        if (pnum>=0)     facet_end->setFacetNo(pnum);
        if (param!=NULL) facet_end->setMaterialParam(*param);       // Materialデータを追加
    }

    DEBUG_MODE PRINT_MESG("MeshObjectData::addData() for TriPolygonData: end.\n");
    return ret;
}


/**
bool  MeshObjectData::importTriData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, ArrayParam<int>* wgt, int vnum)

指定した頂点ベクトルのデータを取り込む．@n
vct, nrm, map は3個づつ組になって三角ポリゴンを表す．従って vnumは必ず3の倍数になるはず．

@param vct      頂点座標データへのポインタ
@param nrm      頂点の法線ベクトルのデータへのポインタ
@param map      テクスチャ座標のデータへのポインタ
@param wgt      頂点の重みデータへのポインタ（オプション）
@param vnum     データ数
@retval         true: 処理の成功．false: 処理の失敗．
*/
bool  MeshObjectData::importTriData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, ArrayParam<int>* wgt, int vnum)
{
    DEBUG_MODE PRINT_MESG("MeshObjectData::importTriData: for Vector<>: start.\n");
    if (vct==NULL) return false;
    //
    free_value();

    int lsize = sizeof(Vector<double>)*vnum;

    // Vertex Position
    if (impvtx_value!=NULL) freeNull(impvtx_value);
    impvtx_value = (Vector<double>*)malloc(lsize);
    if (impvtx_value!=NULL) {
        memcpy(impvtx_value, vct, lsize);
    }
    else return false;

    // Normal Vector
    if (nrm!=NULL) {
        if (impnrm_value!=NULL) freeNull(impnrm_value);
        impnrm_value = (Vector<double>*)malloc(lsize);
        if (impnrm_value!=NULL) {
            memcpy(impnrm_value, nrm, lsize);
        }
        else {
            freeNull(impvtx_value);
            return false;
        }
    }

    // UV Map
    if (map!=NULL) {
        int msize = sizeof(UVMap<double>)*vnum;
        if (impmap_value!=NULL) freeNull(impmap_value);
        impmap_value = (UVMap<double>*)malloc(msize);
        if (impmap_value!=NULL) {
            memcpy(impmap_value, map, msize);
        }
        else {
            freeNull(impvtx_value);
            freeNull(impnrm_value);
            return false;
        }
    }

    // Vertex Weight (option)
    if (wgt!=NULL) {
        if (impwgt_value!=NULL) freeArrayParams<int>(impwgt_value, num_import);
        int wsize = sizeof(ArrayParam<int>)*vnum;
        impwgt_value = (ArrayParam<int>*)malloc(wsize);
        if (impwgt_value!=NULL) {
            memset(impwgt_value, 0, wsize);
            for (int i=0; i<vnum; i++) {
                impwgt_value[i].dup(wgt[i]);
            }
        }
    }

    //
    num_vcount = 3;         // Contour（ポリゴン）を形成する頂点数
    num_import = vnum;      // 総頂点数

    DEBUG_MODE PRINT_MESG("MeshObjectData::importTriData: for Vector<>: end.\n");
    return true;
}


/**
TriPolygonData (三角ポリゴンデータ) を単位としてデータを取り込む．@n
pnum を指定すると，指定されたポリゴンデータのみが追加される．@n

@param tridata  三角ポリゴンデータへのポインタ
@param tnum     三角ポリゴンデータの数
@param pnum     追加するデータの三角ポリゴンの番号（選択的に追加する番号）．-1以下なら全てのポリゴンデータを追加する．
@retval         true: 処理の成功．false: 処理の失敗．
*/
bool  MeshObjectData::importTriData(TriPolygonData* tridata, int tnum, int pnum)
{
    DEBUG_MODE PRINT_MESG("MeshObjectData::importTriData: for TriPolygonData: start.\n");
    if (tridata==NULL) return false;

    free_value();

    int num = 0;
    if (pnum>=0) {
        for (int i=0; i<tnum; i++) {
            if (tridata[i].polygonNum==pnum) num++;
        }
        if (num==0) return false;
    }
    else num = tnum;

    int vnum  = num*3;
    int lsize = sizeof(Vector<double>)*vnum;

    // Vertex Position
    if (impvtx_value!=NULL) freeNull(impvtx_value);
    impvtx_value = (Vector<double>*)malloc(lsize);
    if (impvtx_value!=NULL) {
        for (int i=0, n=0; i<tnum; i++) {
            if (tridata[i].polygonNum==pnum || pnum<0) {
                impvtx_value[n*3]   = tridata[i].vertex[0];
                impvtx_value[n*3+1] = tridata[i].vertex[1];
                impvtx_value[n*3+2] = tridata[i].vertex[2];
                n++;
            }
        }
    }
    else return false;

    // Normal Vector
    if (impnrm_value!=NULL) freeNull(impnrm_value);
    if (tridata[0].has_normal) {
        impnrm_value = (Vector<double>*)malloc(lsize);
        if (impnrm_value!=NULL) {
            for (int i=0, n=0; i<tnum; i++) {
                if (tridata[i].polygonNum==pnum || pnum<0) {
                    impnrm_value[n*3]   = tridata[i].normal[0];
                    impnrm_value[n*3+1] = tridata[i].normal[1];
                    impnrm_value[n*3+2] = tridata[i].normal[2];
                    n++;
                }
            }
        }
        else {
            freeNull(impvtx_value);
            return false;
        }
    }

    // UV Map
    if (impmap_value!=NULL) freeNull(impmap_value);
    if (tridata[0].has_texcrd) {
        int msize = sizeof(UVMap<double>)*vnum;
        impmap_value = (UVMap<double>*)malloc(msize);
        if (impmap_value!=NULL) {
            for (int i=0, n=0; i<tnum; i++) {
                if (tridata[i].polygonNum==pnum || pnum<0) {
                    impmap_value[n*3]   = tridata[i].texcrd[0];
                    impmap_value[n*3+1] = tridata[i].texcrd[1];
                    impmap_value[n*3+2] = tridata[i].texcrd[2];
                    n++;
                }
            }
        }
        else {
            freeNull(impvtx_value);
            freeNull(impnrm_value);
            return false;
        }
    }

    // Vertex Weight (option)
    if (impwgt_value!=NULL) freeArrayParams<int>(impwgt_value, num_import);
    if (tridata[0].has_weight) {
        int wsize = sizeof(ArrayParam<int>)*vnum;
        impwgt_value = (ArrayParam<int>*)malloc(wsize);
        if (impwgt_value!=NULL) {
            memset(impwgt_value, 0, vnum);
            for (int i=0, n=0; i<tnum; i++) {
                if (tridata[i].polygonNum==pnum || pnum<0) {
                    impwgt_value[n*3]  .dup(tridata[i].weight[0], false);
                    impwgt_value[n*3+1].dup(tridata[i].weight[1], false);
                    impwgt_value[n*3+2].dup(tridata[i].weight[2], false);
                    n++;
                }
            }
        }
    }

    //
    num_vcount = 3;         // Contour（ポリゴン）を形成する頂点数
    num_import = vnum;      // 総頂点数

    DEBUG_MODE PRINT_MESG("MeshObjectData::importTriData: for TriPolygonData: end.\n");
    return true;
}


/**
bool  MeshObjectData::addNode(ContourBaseData* facetdata, const char* name, MaterialParam* param)

*/
bool  MeshObjectData::addNode(ContourBaseData* facetdata, const char* name, MaterialParam* param)
{
    DEBUG_MODE PRINT_MESG("MeshObjectData::addNode(): for ContourBaseData: start.\n");
    bool ret = false;

    MeshFacetNode* node = new MeshFacetNode();
    if (node==NULL) return ret;
    //
    if (param!=NULL) node->setMaterialParam(*param);
    node->setMaterialID(name);
    
    ret = node->computeVertexDirect(facetdata);

    if (ret) {
        if (facet==NULL) facet = facet_end = node;
        else             facet_end = AddMeshFacetNode(facet_end, node);
        num_node++;
        ttl_index  += node->num_index;
        ttl_vertex += node->num_vertex;
        ttl_texcrd += node->num_texcrd;
    }

    DEBUG_MODE PRINT_MESG("MeshObjectData::addNode(): for ContourBaseData: end.\n");
    return ret;
}


/**
bool  MeshObjectData::addNode(const char* name, MaterialParam* param, bool useBrep)

@param name     ノードの名前
@param useBrep  BREPを使用して頂点を配置する．
*/
bool  MeshObjectData::addNode(const char* name, MaterialParam* param, bool useBrep)
{
    DEBUG_MODE PRINT_MESG("MeshObjectData::addNode(): for TriPolygonData or Vector<>: start.\n");

    bool ret = false;
    if (impvtx_value==NULL) return ret;

    MeshFacetNode* node = new MeshFacetNode();
    if (node==NULL) return ret;
    //
    if (param!=NULL) node->setMaterialParam(*param);
    node->setMaterialID(name);

    if (useBrep) {
        ret = node->computeVertexByBREP(impvtx_value, impnrm_value, impmap_value, impwgt_value, num_import, num_vcount);
    }
    else {
        ret = node->computeVertexDirect(impvtx_value, impnrm_value, impmap_value, impwgt_value, num_import, num_vcount);
    }
    if (ret) {
        if (facet==NULL) facet = facet_end = node;
        else             facet_end = AddMeshFacetNode(facet_end, node);
        num_node++;
        ttl_index  += node->num_index;
        ttl_vertex += node->num_vertex;
        ttl_texcrd += node->num_texcrd;
    }
    //
    freeNull(impvtx_value);
    freeNull(impnrm_value);
    freeNull(impmap_value);
    freeArrayParams<int>(impwgt_value, node->num_vertex);
    impwgt_value = NULL;

    DEBUG_MODE PRINT_MESG("MeshObjectData::addNode(): for TriPolygonData or Vector<>: end.\n");
    return ret;
}


/**
void  MeshObjectData::setMaterialParam(MaterialParam param, int num)

@param param  マテリアルパラメータ
@param num    0以上の場合は指定したノードに，-1の場合は先頭から順にノードにパラメータを設定する
*/
void  MeshObjectData::setMaterialParam(MaterialParam param, int num)
{
    MeshFacetNode* node = facet;

    if (num>=0) {
        while (node!=NULL) {
            if (node->facet_no==num) {
                node->setMaterialParam(param);
                node->setMaterialID(param.getParamString());
                return;
            }
            node = node->next;
        }
    }
    else {
        while (node!=NULL) {
            if (!node->material_param.enable) {
                node->setMaterialParam(param);
                node->setMaterialID(param.getParamString());
                return;
            }
            node = node->next;
        }
    }
    return;
}


/**
現在の形状データに，dataを面の一部(Node)として結合させる．アフィン変換のパラメータの違うものは結合できない．

@param data 結合するMeshObjectデータ．結合後データは削除される．データのアフィン変換は無視する．
*/
void  MeshObjectData::joinData(MeshObjectData*& data)
{
    if (data==NULL) return;

    ttl_index  += data->ttl_index;
    ttl_vertex += data->ttl_vertex;
    ttl_texcrd += data->ttl_texcrd;
    num_node   += data->num_node;

    if (facet_end==NULL) {   //  最初のデータ
        setAffineTrans(*data->affineTrans);
        facet = data->facet;
        facet_end = data->facet_end;
    }
    else if (data->facet!=NULL) {
        facet_end->next = data->facet;
        data->facet->prev = facet_end;
        facet_end = data->facet_end;
    }

    data->facet = NULL;
    freeMeshObjectData(data);

    return;
}

