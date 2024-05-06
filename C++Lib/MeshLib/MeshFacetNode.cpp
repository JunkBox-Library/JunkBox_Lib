/**
@brief    Mesh Facet Node用 ツール
@file     MeshFacetNode.cpp
@author   Fumi.Iseki (C)
*/

#include  "MeshFacetNode.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////
// MeshFacetNode クラス
//

void  MeshFacetNode::init(void)
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
    weight_value = NULL;

    next         = NULL;
    prev         = NULL;
}


/**
@brief ノードにマテリアルパラメータを設定し，他のノードに同じマテリアルが存在するかチャックする．
*/
void  MeshFacetNode::setMaterialParam(MaterialParam mparam)
{
    material_param.free();
    material_param.dup(mparam);
    material_param.enable = true;

    MeshFacetNode* node = prev;
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


void  MeshFacetNode::setMaterialID(const char* str)
{
    free_Buffer(&material_id);
    
    if (str!=NULL) {
        if (!strncmp(str, JBXL_MATERIAL_PREFIX, strlen(JBXL_MATERIAL_PREFIX))) {
            material_id = make_Buffer_str(str);
        }
        else {
            char* texture_name = material_param.getName();
            if (texture_name==NULL) return;

            material_id = make_Buffer_str(JBXL_MATERIAL_PREFIX);
            Buffer texture_id = make_Buffer_bystr(texture_name);
            del_file_extension_Buffer(&texture_id);
            cat_Buffer(&texture_id, &material_id);
            free_Buffer(&texture_id);
            //
            cat_s2Buffer("_", &material_id);
            cat_s2Buffer(str, &material_id);
        }
    }
    //
    else {
        char* texture_name = material_param.getName();
        if (texture_name==NULL) return;

        material_id = make_Buffer_str(JBXL_MATERIAL_PREFIX);
        Buffer texture_id = make_Buffer_bystr(texture_name);
        del_file_extension_Buffer(&texture_id);
        cat_Buffer(&texture_id, &material_id);
        free_Buffer(&texture_id);
    }
    return;
}


void  MeshFacetNode::set(int vertex, int polygon, int vcount)
{
    num_vertex  = vertex;
    num_texcrd  = num_vertex;
    num_polygon = polygon;
    num_index   = num_polygon*vcount;

    return;
}


void  MeshFacetNode::free(void)
{
    delMaterialParam();
    free_Buffer(&material_id);

    free_value();
}


void  MeshFacetNode::free_value(void)
{
    freeNull(data_index); 
    freeNull(vertex_value); 
    freeNull(normal_value); 
    freeNull(texcrd_value); 
    freeArrayParams<double>(weight_value, num_vertex); 

    data_index   = NULL;
    vertex_value = NULL;
    normal_value = NULL;
    texcrd_value = NULL; 
    weight_value = NULL; 
}


void  MeshFacetNode::clear(void)
{
    this->free();
    init();
}


/**
bool  MeshFacetNode::getm(int vertex, int polygon, int vcount)

必要なメモリを確保する．
失敗，成功に係らず，以前のメモリは開放される．

@retval true  メモリの確保に成功．
@retval false メモリの確保に失敗．

@param vertex   頂点の数
@param polygon  ポリゴンの数
@param vcount   １ポリゴン当たりの頂点数（固定）
*/
bool  MeshFacetNode::getm(int vertex, int polygon, int vcount)
{
    free_value();

    if (vertex >0) num_vertex  = vertex;
    if (polygon>0) num_polygon = polygon;
    if (vcount >0) num_index   = num_polygon*vcount;

    if (num_vertex<=0 || num_polygon<=0) return false;
    num_texcrd = num_vertex;

    data_index   = (int*)malloc(sizeof(int)*num_index);
    vertex_value = (Vector<double>*)malloc(sizeof(Vector<double>)*num_vertex);
    normal_value = (Vector<double>*)malloc(sizeof(Vector<double>)*num_vertex);
    texcrd_value = (UVMap<double>*) malloc(sizeof(UVMap<double>) *num_texcrd);
    weight_value = (ArrayParam<double>*)malloc(sizeof(ArrayParam<double>)*num_vertex);     // option

    if (data_index==NULL || vertex_value==NULL || normal_value==NULL || texcrd_value==NULL) {
        this->free();
        return false;
    }

    memset(data_index,   0, sizeof(int)*num_index);
    memset(vertex_value, 0, sizeof(Vector<double>)*num_vertex);
    memset(normal_value, 0, sizeof(Vector<double>)*num_vertex);
    memset(texcrd_value, 0, sizeof(UVMap<double>) *num_texcrd);
    memset(weight_value, 0, sizeof(ArrayParam<double>)*num_vertex);

    return true;
}


/**
bool  MeshFacetNode::computeVertexDirect(ContourBaseData* facetdata)

インデックス化された頂点データを直接 MeshObjectのデータとしてインポートする．

@param  facetdata ContourBaseDataへのポインタ．
@return インポートに成功したかどうか．
*/
bool  MeshFacetNode::computeVertexDirect(ContourBaseData* facetdata)
{
    if (facetdata==NULL) return false;
    if (facetdata->index==NULL || facetdata->vertex==NULL || facetdata->normal==NULL) return false;

    // num_data -> num_vertex, num_index/vcount -> num_polygon, num_index -> num_index
    set(facetdata->num_data, facetdata->num_index/facetdata->vcount, facetdata->vcount);
    if (!getm()) return false;

    for (int i=0; i<facetdata->num_index; i++) {
        data_index[i]   = facetdata->index[i];
    }
    for (int i=0; i<num_vertex; i++) {
        vertex_value[i] = facetdata->vertex[i];
        normal_value[i] = facetdata->normal[i];
        weight_value[i].dup(facetdata->weight[i]);
    }
    if (facetdata->texcrd!=NULL) {
        for (int i=0; i<num_texcrd; i++) {
            texcrd_value[i] = facetdata->texcrd[i];
        }
    }

    return true;
}


/**
bool  MeshFacetNode::computeVertexByBREP(ContourBaseData* facetdata)

BREPを使用して，頂点データを処理する．頂点データは再インデックス化される@n
データがインデックス化されていない場合，重複頂点を削除するのでデータサイズが小さくなる．@n
法線ベクトルが計算されていない場合（facetdata->normal がNULLの場合），法線ベクトルを計算する．@n
頂点数が多い場合は，処理に時間が掛かる．@n

@param  facetdata ContourBaseDataへのポインタ．
@return インポートに成功したかどうか．
*/
bool  MeshFacetNode::computeVertexByBREP(ContourBaseData* facetdata)
{
    if (facetdata==NULL) return false;
    if (facetdata->index==NULL || facetdata->vertex==NULL || facetdata->normal==NULL) return false;

    BREP_SOLID* brep = new BREP_SOLID();
    if (brep==NULL) return false;
    // 重複登録を許可しない．データチェックはしない．
    CreateTriSolidFromVector(brep, facetdata->num_data, facetdata->vertex, facetdata->normal, facetdata->texcrd, facetdata->weight, false, false); 

    long int  vnum;
    BREP_VERTEX** vertex_data = GetOctreeVertices(brep->octree, &vnum);
    if (vertex_data==NULL) {
        freeBrepSolid(brep);
        return false;
    }
    int vcount = facetdata->vcount;

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
        weight_value[i].dup(vertex_data[i]->weight);
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


/**
bool  MeshFacetNode::computeVertexDirect(Vector<double>* impvtx, Vector<double>* impnrm, UVMap<double>* impmap, ArrayParam<double>* impwgt, int impnum, int vcount)

整列化（インデックス化ではない）された頂点データを直接 MeshObjectのデータとしてインポートする．@n
元のデータの再現性が良い．処理時間が早い．@n
法線ベクトルが必須．データサイズは大きくなる．@n

@param impvtx インポートする頂点の座標データ（必須）
@param impnrm インポートする法線ベクトルデータ（必須）
@param impmap インポートする頂点のUVマップデータ（オプション）
@param impwgt インポートする頂点の重みデータ（オプション）
@param impnum インポートするデータの数
@param vcount ポリゴンの頂点数．通常は 3
@return インポートに成功したかどうか．
*/
bool  MeshFacetNode::computeVertexDirect(Vector<double>* impvtx, Vector<double>* impnrm, UVMap<double>* impmap, ArrayParam<double>* impwgt, int impnum, int vcount)
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
    if (impwgt!=NULL) {
        for (int i=0; i<num_vertex; i++) {
            weight_value[i].dup(impwgt[i]);
        }
    }

    return true;
}


/**
bool  MeshFacetNode::computeVertexByBREP(Vector<double>* impvtx, Vector<double>* impnrm, UVMap<double>* impmap, ArrayParam<double>* impwgt, int impnum, int vcount)

BREPを使用して，頂点データを処理する．頂点データは再インデックス化される@n
データがインデックス化されていない場合，重複頂点を削除するのでデータサイズが小さくなる．@n
法線ベクトルが計算されていない場合（ipnrmがNULLの場合），法線ベクトルを計算する．@n
頂点数が多い場合は，処理に時間が掛かる．@n

@param impvtx インポートする頂点の座標データ．（必須）
@param impnrm インポートする法線ベクトルデータ．NULLの場合，再計算が行われる．（オプション）
@param impmap インポートする頂点のUVマップデータ．（オプション）
@param impmap インポートする頂点の重みデータ．（オプション）
@param impnum インポートするデータの数
@param vcount ポリゴンの頂点数．通常は 3
@return インポートに成功したかどうか．
*/
bool  MeshFacetNode::computeVertexByBREP(Vector<double>* impvtx, Vector<double>* impnrm, UVMap<double>* impmap, ArrayParam<double>* impwgt, int impnum, int vcount)
{
    if (impvtx==NULL) return false;

    BREP_SOLID* brep = new BREP_SOLID();
    if (brep==NULL) return false;
    CreateTriSolidFromVector(brep, impnum, impvtx, impnrm, impmap, impwgt, false, false);   // 重複登録を許可しない．データチェックはしない．

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
        weight_value[i].dup(vertex_data[i]->weight);
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


void  MeshFacetNode::execAffineTransUVMap(UVMap<double>* uvmap, int uvnum)
{
    if (uvmap==NULL) uvmap = texcrd_value;
    if (uvnum==-1)   uvnum = num_texcrd;

    material_param.texture.execTrans(uvmap, uvnum);

    return;
}


/**
UVMap<double>*  MeshFacetNode::generatePlanarUVMap(Vector<double> scale, UVMap<double>* uvmap)

頂点データから，各 nodeの Planar UVマップを生成する@n
頂点データ(vertex_value, normal_value) がインポート済みでなければならない．@n

@param  scale オブジェクトのサイズ
@param  uvmap 生成したオブジェクトを格納する UVMapへのポインタ(サイズは num_texcrd). NULLの場合は領域を新たに確保する．
@return UVMap へのポインタ．サイズは num_texcrd. 失敗した場合は NULL

@sa libopenmetaverse OpenMetaverse.Rendering.Meshmerizer:MeshmerizerR.cs
*/
UVMap<double>*  MeshFacetNode::generatePlanarUVMap(Vector<double> scale, UVMap<double>* uvmap)
{
    if (num_texcrd!=num_vertex) return NULL;

    if (uvmap==NULL) {
        size_t len = num_texcrd*sizeof(UVMap<double>);
        uvmap = (UVMap<double>*)malloc(len);
        if (uvmap==NULL) return NULL;
        memset(uvmap, 0, len);
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

void  jbxl::freeMeshObjectList(MeshFacetNode*& node)
{
    if (node==NULL) return;

    MeshFacetNode* next = node->next;
    if (next!=NULL) freeMeshObjectList(next);

    freeMeshFacetNode(node);

    return;
}


//
MeshFacetNode*  jbxl::DelMeshFacetNode(MeshFacetNode* node)
{
    if (node==NULL) return NULL;

    if (node->prev!=NULL) node->prev->next = node->next;
    if (node->next!=NULL) node->next->prev = node->prev;
            
    MeshFacetNode* next = node->next;
    freeMeshFacetNode(node);

    return next;
}


MeshFacetNode*  jbxl::AddMeshFacetNode(MeshFacetNode* list, MeshFacetNode* node)
{
    if (list==NULL) return node;
    if (node==NULL) return list;

    node->prev = list;
    node->next = list->next;

    if (list->next!=NULL) list->next->prev = node;
    list->next = node;

    return node;
}

