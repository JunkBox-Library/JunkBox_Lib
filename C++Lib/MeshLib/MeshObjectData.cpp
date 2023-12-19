/**
@brief    Mesh Data用 ツール
@file     MeshObjectData.cpp
@author   Fumi.Iseki (C)
*/


#include  "MeshObjectData.h"


using namespace jbxl;



///////////////////////////////////////////////////////////////////////////////////////////////////
// MeshObjectNode クラス
//

void  MeshObjectNode::init(void)
{
    same_material= false;
    material_id  = init_Buffer();
    facet_no     = -1;
    material_param.init();

    num_vertex   = 0;
    num_texcrd   = 0;
    num_polygon  = 0;
    num_index    = 0;

    data_index   = NULL;
    vertex_value = NULL;
    normal_value = NULL;
    texcrd_value = NULL;

    next         = NULL;
    prev         = NULL;
}



/**
@brief ノードにマテリアルパラメータを設定し，他のノードに同じマテリアルが存在するかチャックする．
*/
void  MeshObjectNode::setMaterialParam(MaterialParam mparam)
{
    material_param.free();
    material_param.dup(mparam);
    material_param.enable = true;

    MeshObjectNode* node = prev;
    while (node!=NULL) {
        if (isSameMaterial(material_param, node->material_param)) {
            setMaterialID(_tochar(node->material_id.buf));
            same_material = true;
            return;
        }
        node = node->prev;
    }

    node = next;
    while (node!=NULL) {
        if (isSameMaterial(material_param, node->material_param)) {
            setMaterialID(_tochar(node->material_id.buf));
            same_material = true;
            return;
        }
        node = node->next;
    }

    return;
}



void  MeshObjectNode::setMaterialID(const char* str)
{
    free_Buffer(&material_id);
    
    if (str!=NULL) {
        if (str[0]=='#') {
            material_id = make_Buffer_str(str);
        }
        else {
            Buffer randomstr = make_Buffer_randomstr(MOBJN_MTERIALID_RAND_LEN);
            material_id = make_Buffer_str("#MATERIAL_");
            cat_Buffer(&randomstr, &material_id);
            free_Buffer(&randomstr);
            cat_s2Buffer("_", &material_id);
            cat_s2Buffer(str, &material_id);
        }
    }
    //
    else {
        Buffer randomstr = make_Buffer_randomstr(MOBJN_MTERIALID_RAND_LEN);
        material_id = make_Buffer_str("#MATERIAL_");
        cat_Buffer(&randomstr, &material_id);
        free_Buffer(&randomstr);
    }

    return;
}



void  MeshObjectNode::set(int vertex, int polygon, int vcount)
{
    num_vertex  = vertex;
    num_texcrd  = num_vertex;
    num_polygon = polygon;
    num_index   = num_polygon*vcount;

    return;
}



void  MeshObjectNode::free(void)
{
    delMaterialParam();
    free_Buffer(&material_id);

    free_value();
}



void  MeshObjectNode::free_value(void)
{
    freeNull(data_index); 
    freeNull(vertex_value); 
    freeNull(normal_value); 
    freeNull(texcrd_value); 
}



void  MeshObjectNode::clear(void)
{
    this->free();
    init();
}



/**
bool  MeshObjectNode::getm(int vertex, int polygon, int vcount)

必要なメモリを確保する．
失敗，成功に係らず，以前のメモリは開放される．

@retval true  メモリの確保に成功．
@retval false メモリの確保に失敗．

@param vertex   頂点の数
@param polygon  ポリゴンの数
@param vcount   １ポリゴン当たりの頂点数（固定）
*/
bool  MeshObjectNode::getm(int vertex, int polygon, int vcount)
{
    free_value();

    if (vertex >0) num_vertex  = vertex;
    if (polygon>0) num_polygon = polygon;
    if (vcount >0) num_index   = num_polygon*vcount;

    if (num_vertex<=0 || num_polygon<=0) return false;
    num_texcrd = num_vertex;

    vertex_value = (Vector<double>*)malloc(num_vertex*sizeof(Vector<double>));
    normal_value = (Vector<double>*)malloc(num_vertex*sizeof(Vector<double>));
    texcrd_value = (UVMap<double>*) malloc(num_texcrd*sizeof(UVMap<double>));
    data_index   = (int*)malloc(num_index*sizeof(int));

    if (data_index==NULL || vertex_value==NULL || normal_value==NULL || texcrd_value==NULL) {
        this->free();
        return false;
    }

    return true;
}



/**
インデックス化された頂点データを直接 MeshObjectのデータとしてインポートする．
*/
bool  MeshObjectNode::computeVertexDirect(FacetBaseData* facetdata)
{
    if (facetdata==NULL) return false;
    if (facetdata->index==NULL || facetdata->vertex==NULL || facetdata->normal==NULL) return false;

    set(facetdata->num_data, facetdata->num_index/facetdata->vcount, facetdata->vcount);
    if (!getm()) return false;

    for (int i=0; i<facetdata->num_index; i++) {
        data_index[i]   = facetdata->index[i];
    }
    for (int i=0; i<num_vertex; i++) {
        normal_value[i] = facetdata->normal[i];
        vertex_value[i] = facetdata->vertex[i];
    }

    if (facetdata->texcrd!=NULL) {
        for (int i=0; i<num_texcrd; i++) {
            texcrd_value[i] = facetdata->texcrd[i];
        }
    }

    return true;
}



/**
整列化（インデックス化ではない）された頂点データを直接 MeshObjectのデータとしてインポートする．

元のデータの再現性が良い．処理時間が早い．@n
法線ベクトルが必須．データサイズは大きくなる．@n

@param impvtx インポートする頂点の座標データ（必須）
@param impnrm インポートする法線ベクトルデータ（必須）
@param impmap インポートする頂点のUVマップデータ（オプション）
@param impnum インポートするデータの数
@param vcount ポリゴンの頂点数．通常は 3
@return インポートに成功したかどうか．
*/
bool  MeshObjectNode::computeVertexDirect(Vector<double>* impvtx, Vector<double>* impnrm, UVMap<double>* impmap, int impnum, int vcount)
{
    if (impvtx==NULL || impnrm==NULL) return false;

    set(impnum, impnum/vcount, vcount);
    if (!getm()) return false;

    for (int i=0; i<num_vertex; i++) {
        vertex_value[i] = impvtx[i];
        normal_value[i] = impnrm[i];
        data_index[i]   = i;
    }

    if (impmap!=NULL) {
        for (int i=0; i<num_vertex; i++) {
            texcrd_value[i] = impmap[i];
        }
    }

    return true;
}



/**
bool  MeshObjectNode::computeVertexByBREP(Vector<double>* impvtx, Vector<double>* impnrm, UVMap<double>* impmap, int impnum, int vcount)

BREPを使用して，頂点データを処理する．頂点データは再インデックス化される@n

データがインデックス化されていない場合，重複頂点を削除するのでデータサイズが小さくなる．@n
法線ベクトルが計算されていない場合（ipnrmがNULLの場合），法線ベクトルを計算する．@n
頂点数が多い場合は，処理に時間が掛かる．@n

@param impvtx インポートする頂点の座標データ．（必須）
@param impnrm インポートする法線ベクトルデータ．NULLの場合，再計算が行われる．（オプション）
@param impmap インポートする頂点のUVマップデータ．（オプション）
@param impnum インポートするデータの数
@param vcount ポリゴンの頂点数．通常は 3
@return インポートに成功したかどうか．
*/
bool  MeshObjectNode::computeVertexByBREP(Vector<double>* impvtx, Vector<double>* impnrm, UVMap<double>* impmap, int impnum, int vcount)
{
    if (impvtx==NULL) return false;

    BREP_SOLID* brep = new BREP_SOLID();
    if (brep==NULL) return false;
    CreateTriSolidFromVector(brep, impnum, impvtx, impnrm, impmap, false, false);   // 重複登録を許可しない．データチェックはしない．

    long int  vnum;
    BREP_VERTEX** vertex_data = GetOctreeVertices(brep->octree, &vnum);
    if (vertex_data==NULL) {
        freeBrepSolid(brep);
        return false;
    }

    // メモリの確保
    set((int)vnum, brep->facetno, vcount);
    if (!getm()) {
        ::free(vertex_data);
        freeBrepSolid(brep);
        return false;
    }

    // Vertex & Normal & Texcoord
    for (int i=0; i<num_vertex; i++) {
        vertex_value[i] = vertex_data[i]->point;
        normal_value[i] = vertex_data[i]->normal;
        texcrd_value[i] = vertex_data[i]->uvmap;
    }

    // Index
    int polyn = 0;
    BREP_CONTOUR_LIST::iterator icon;
    for (icon=brep->contours.begin(); icon!=brep->contours.end(); icon++){
        BREP_WING* wing = (*icon)->wing;
        for (int i=0; i<vcount; i++) {
            BREP_VERTEX* vertex = wing->vertex;
            if (vertex!=NULL) {
                data_index[polyn*vcount+i] = vertex->index;
            }
            wing = wing->next;
        }
        polyn++;
    }

    ::free(vertex_data);
    freeBrepSolid(brep);

    return true;
}


void  MeshObjectNode::execAffineTrans(UVMap<double>* uvmap, int uvnum)
{
    if (uvmap==NULL) uvmap = texcrd_value;
    if (uvnum==-1)   uvnum = num_texcrd;

    material_param.execTrans(uvmap, uvnum); 

    return;
}



/**
頂点データから，各 nodeの Planar UVマップを生成する
頂点データ(vertex_value, normal_value) がインポート済みでなければならない．

@param  scale オブジェクトのサイズ
@param  uvmap 生成したオブジェクトを格納する UVMapへのポインタ(サイズは num_texcrd). NULLの場合は領域を新たに確保する．
@return UVMap へのポインタ．サイズは num_texcrd. 失敗した場合は NULL

@sa libopenmetaverse OpenMetaverse.Rendering.Meshmerizer:MeshmerizerR.cs
*/
UVMap<double>*  MeshObjectNode::generatePlanarUVMap(Vector<double> scale, UVMap<double>* uvmap)
{
    if (num_texcrd!=num_vertex) return NULL;

    if (uvmap==NULL) {
        size_t len = num_texcrd*sizeof(UVMap<double>);
        uvmap = (UVMap<double>*)malloc(len);
    }

    for (int i=0; i<num_texcrd; i++) {
        //
        Vector<double> binormal;

        if (normal_value[i].x>=0.5 || normal_value[i].x<=-0.5) {
            binormal.set(0.0, 1.0, 0.0);
            if (normal_value[i].x<0.0) binormal = - binormal;
        }
        else {
            binormal.set(1.0, 0.0, 0.0);
            if (normal_value[i].y>0.0) binormal = - binormal;
        }

        Vector<double> tangent = binormal^normal_value[i];
        Vector<double> pos(vertex_value[i].x*scale.x, vertex_value[i].y*scale.y, vertex_value[i].z*scale.z);

        uvmap[i].u = 0.5 + (binormal*pos)*2.0;
        uvmap[i].v = 0.5 - (tangent *pos)*2.0;
    }

    return uvmap;
}




///////////////////////////////////////////////////////////////////

void  jbxl::freeMeshObjectList(MeshObjectNode*& node)
{
    if (node==NULL) return;

    MeshObjectNode* next = node->next;
    if (next!=NULL) freeMeshObjectList(next);

    freeMeshObjectNode(node);

    return;
}



//
MeshObjectNode*  jbxl::DelMeshObjectNode(MeshObjectNode* node)
{
    if (node==NULL) return NULL;

    if (node->prev!=NULL) node->prev->next = node->next;
    if (node->next!=NULL) node->next->prev = node->prev;
            
    MeshObjectNode* next = node->next;
    freeMeshObjectNode(node);

    return next;
}



MeshObjectNode*  jbxl::AddMeshObjectNode(MeshObjectNode* list, MeshObjectNode* node)
{
    if (list==NULL) return node;
    if (node==NULL) return list;

    node->prev = list;
    node->next = list->next;

    if (list->next!=NULL) list->next->prev = node;
    list->next = node;

    return node;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// MeshObjectData クラス
//

void  MeshObjectData::init(const char* name)
{
    data_name    = make_Buffer_str(name);

    ttl_index    = 0;
    ttl_vertex   = 0;
    ttl_texcrd   = 0;
    num_node     = 0;
    num_vcount   = 3;

    nodelist     = NULL;
    endplist     = NULL;
    affine_trans = NULL;

    num_import   = 0;
    impvtx_value = NULL;
    impnrm_value = NULL;
    impmap_value = NULL;
}



void  MeshObjectData::free(void)
{
    free_Buffer(&data_name);
    free_value();

    delAffineTrans();

    freeMeshObjectList(nodelist);
    nodelist = endplist = NULL;
}



void  MeshObjectData::free_value(void)
{
    freeNull(impvtx_value);
    freeNull(impnrm_value);
    freeNull(impmap_value);
}



void  MeshObjectData::clear(void)
{
    this->free();
    init();
}



/**
bool  MeshObjectData::addData(FacetBaseData* facetdata, MaterialParam* param)

インデックス化された FacetBaseDataを importTriData()を介さずに，直接 Nodeデータに書き込む．@n
FACETを選択的に処理することはできない．予め FACETに分解しておくか，FACETが1つのみの場合に使用する．

この後 MeshObjectNode::computeVertexDirect() を使用して頂点データの計算を行う．
*/
bool  MeshObjectData::addData(FacetBaseData* facetdata, MaterialParam* param)
{
    char* name = NULL;
    if (param!=NULL) name = param->getAdditionalName();

    bool ret = addNode(facetdata, name);
    if (ret && param!=NULL) endplist->setMaterialParam(*param);

    return ret;
}



/**
bool  MeshObjectData::addData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, int vnum, MaterialParam* param, bool useBrep)

指定した頂点ベクトルのデータを追加し，MeshObjectのデータ（通常はFACET単位）を作成する．@n
vct, nrm, map は3個づつ組になって三角ポリゴンを表す．従って vnumは必ず3の倍数になるはず．@n
FACETを選択的に処理することはできない．予め FACETに分解しておくか，FACETが1つのみの場合に使用する．

その後 MeshObjectNode::computeVertexDirect() または MeshObjectNode::computeVertexByBREP() を使用して頂点データの計算を行う．

@param vct      追加対象の頂点座標データへのポインタ
@param nrm      追加対象の頂点の法線ベクトルのデータへのポインタ
@param map      追加対象のテクスチャ座標のデータへのポインタ
@param vnum     データ数
@param param    マテリアル用パラメータへのポインタ
@param useBrep  BREPを使用して頂点を配置する．速度は遅くなるが，頂点数（データ量）は減る．
*/
bool  MeshObjectData::addData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, int vnum, MaterialParam* param, bool useBrep)
{
    bool ret = importTriData(vct, nrm, map, vnum);
    if (ret) {
        char* name = NULL;
        if (param!=NULL) name = param->getAdditionalName();
        ret = addNode(name, useBrep);
    }

    if (ret && param!=NULL) endplist->setMaterialParam(*param);

    return ret;
}



/**
TriPolyData (三角ポリゴンデータ) を単位としてデータを追加し，MeshObjectのデータを作成する．@n
num を指定すると，指定されたFACET（面）のデータのみが追加される．これにより面ごとのデータ構造を形成することができる．@n

@param tridata  追加対象の三角ポリゴンデータへのポインタ
@param tnum     三角ポリゴンデータの数
@param fnum     追加するデータのFACETの番号（選択的に追加する場合に指定する）．-1以下なら全てのFACETデータを追加する．
@param param    マテリアル用パラメータへのポインタ
@param useBrep  BREPを使用して頂点を配置する．速度は遅くなるが，頂点数（データ量）は減る．
*/
bool  MeshObjectData::addData(TriPolyData* tridata, int tnum, int fnum, MaterialParam* param, bool useBrep)
{
    bool ret = importTriData(tridata, tnum, fnum);
    if (ret) {
        char* name = NULL;
        if (param!=NULL) name = param->getAdditionalName();
        ret = addNode(name, useBrep);
    }
    //
    if (ret) {
        if (fnum>=0)     endplist->setFacetNo(fnum);
        if (param!=NULL) endplist->setMaterialParam(*param);
    }

    return ret;
}



/**
指定した頂点ベクトルのデータを取り込む．@n
vct, nrm, map は3個づつ組になって三角ポリゴンを表す．従って vnumは必ず3の倍数になるはず．

@param vct      頂点座標データへのポインタ
@param nrm      頂点の法線ベクトルのデータへのポインタ
@param map      テクスチャ座標のデータへのポインタ
@param vnum     データ数
*/
bool  MeshObjectData::importTriData(Vector<double>* vct, Vector<double>* nrm, UVMap<double>* map, int vnum)
{
    if (vct==NULL) return false;
    //
    free_value();

    int lsize = sizeof(Vector<double>)*vnum;
    impvtx_value = (Vector<double>*)malloc(lsize);
    if (impvtx_value!=NULL) memcpy(impvtx_value, vct, lsize);
    else return false;

    if (nrm!=NULL) {
        impnrm_value = (Vector<double>*)malloc(lsize);
        if (impnrm_value!=NULL) {
            memcpy(impnrm_value, nrm, lsize);
        }
        else {
            freeNull(impvtx_value);
            return false;
        }
    }

    if (map!=NULL) {
        int msize = sizeof(UVMap<double>)*vnum;
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

    num_vcount = 3;
    num_import = vnum;

    return true;
}



/**
TriPolyData (三角ポリゴンデータ) を単位としてデータを取り込む．@n
num を指定すると，指定されたFACET（面）のデータのみが追加される．これにより面ごとのデータ構造を形成することができる．

@param tridata  三角ポリゴンデータへのポインタ
@param tnum     三角ポリゴンデータの数
@param fnum     追加するデータのFACETの番号（選択的に追加する番号）．-1以下なら全てのFACETデータを追加する．
*/
bool  MeshObjectData::importTriData(TriPolyData* tridata, int tnum, int fnum)
{
    if (tridata==NULL) return false;

    free_value();

    int pnum = 0;
    if (fnum>=0) {
        for (int i=0; i<tnum; i++) {
            if (tridata[i].facetNum==fnum) pnum++;
        }
        if (pnum==0) return false;
    }
    else pnum = tnum;

    int vnum  = pnum*3;
    int lsize = sizeof(Vector<double>)*vnum;

    impvtx_value = (Vector<double>*)malloc(lsize);
    if (impvtx_value!=NULL) {
        for (int i=0, n=0; i<tnum; i++) {
            if (tridata[i].facetNum==fnum || fnum<0) {
                impvtx_value[n*3]   = tridata[i].vertex[0];
                impvtx_value[n*3+1] = tridata[i].vertex[1];
                impvtx_value[n*3+2] = tridata[i].vertex[2];
                n++;
            }
        }
    }
    else return false;

    impnrm_value = NULL;
    if (tridata[0].has_normal) {
        impnrm_value = (Vector<double>*)malloc(lsize);
        if (impnrm_value!=NULL) {
            for (int i=0, n=0; i<tnum; i++) {
                if (tridata[i].facetNum==fnum || fnum<0) {
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

    impmap_value = NULL;
    if (tridata[0].has_texcrd) {
        int msize = sizeof(UVMap<double>)*vnum;
        impmap_value = (UVMap<double>*)malloc(msize);
        if (impmap_value!=NULL) {
            for (int i=0, n=0; i<tnum; i++) {
                if (tridata[i].facetNum==fnum || fnum<0) {
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

    num_vcount = 3;
    num_import = vnum;

    return true;
}



bool  MeshObjectData::addNode(FacetBaseData* facetdata, const char* name)
{
    bool ret = false;

    MeshObjectNode* node = new MeshObjectNode();
    if (node==NULL) return ret;
    node->setMaterialID(name);
    
    ret = node->computeVertexDirect(facetdata);

    if (ret) {
        if (nodelist==NULL) nodelist = endplist = node;
        else                endplist = AddMeshObjectNode(endplist, node);
        num_node++;
        ttl_index  += node->num_index;
        ttl_vertex += node->num_vertex;
        ttl_texcrd += node->num_texcrd;
    }

    return ret;
}



/**

@param name    ノードの名前
@param useBrep BREPを使用して頂点を配置する．速度は遅くなるが，頂点数（データ量）は減る．
*/
bool  MeshObjectData::addNode(const char* name, bool useBrep)
{
    bool ret = false;
    if (impvtx_value==NULL) return ret;

    MeshObjectNode* node = new MeshObjectNode();
    if (node==NULL) return ret;
    node->setMaterialID(name);

    if (useBrep) {
        ret = node->computeVertexByBREP(impvtx_value, impnrm_value, impmap_value, num_import, num_vcount);
    }
    else {
        ret = node->computeVertexDirect(impvtx_value, impnrm_value, impmap_value, num_import, num_vcount);
    }

    if (ret) {
        if (nodelist==NULL) nodelist = endplist = node;
        else                endplist = AddMeshObjectNode(endplist, node);
        num_node++;
        ttl_index  += node->num_index;
        ttl_vertex += node->num_vertex;
        ttl_texcrd += node->num_texcrd;
    }
    //
    freeNull(impvtx_value);
    freeNull(impnrm_value);
    freeNull(impmap_value);

    return ret;
}



/**
void  MeshObjectData::setMaterialParam(MaterialParam param, int num)

@param param  マテリアルパラメータ
@param num    0以上の場合は指定したノードに，-1の場合は先頭から順にノードにパラメータを設定する
*/
void  MeshObjectData::setMaterialParam(MaterialParam param, int num)
{
    MeshObjectNode* node = nodelist;

    if (num>=0) {
        while (node!=NULL) {
            if (node->facet_no==num) {
                node->setMaterialParam(param);
                return;
            }
            node = node->next;
        }
    }
    else {
        while (node!=NULL) {
            if (!node->material_param.enable) {
                node->setMaterialParam(param);
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

    if (endplist==NULL) {   //  最初のデータ
        setAffineTrans(*data->affine_trans);
        nodelist = data->nodelist;
        endplist = data->endplist;
    }
    else if (data->nodelist!=NULL) {
        endplist->next = data->nodelist;
        data->nodelist->prev = endplist;
        endplist = data->endplist;
    }

    data->nodelist = NULL;
    freeMeshObjectData(data);

    return;
}



