/**
@brief    三角Contour(Facet)用ライブラリ 
@file     TriBrep.cpp
@author   Fumi.Iseki (C)

@attention
このファイルの関数は Contour(=Facet)が三角形であることを前提とする．
*/

#include "TriBrep.h"


using namespace jbxl;


/**
void jbxl:: CreateContoursList(BREP_SOLID* solid)

削除マークの付いたContour(Facet)を削除し，全Contour(Facet)の Listを作る．@n
ただし，読み込み（初期）データは削除しない．
*/
DllExport void  jbxl::CreateContoursList(BREP_SOLID* solid)
{
    solid->contours.clear();

    BREP_SHELL_LIST shells = solid->shells;
    BREP_SHELL_LIST::iterator ishell;
    for (ishell=shells.begin(); ishell!=shells.end(); ishell++){
        BREP_FACET* pbfc;
        BREP_FACET_LIST::iterator ifacet;
        for (ifacet =(*ishell)->facets.begin(); ifacet!=(*ishell)->facets.end(); ifacet++){
            if ((*ifacet)->deletable && !(*ifacet)->notdelete) {
                pbfc = *ifacet;
                ifacet = (*ishell)->facets.erase(ifacet);
                ifacet--;
                pbfc->shell = NULL;
                delete(pbfc);
            }
            else {
                BREP_CONTOUR_LIST::iterator icon;
                for (icon=(*ifacet)->outer_contours.begin(); icon!=(*ifacet)->outer_contours.end(); icon++) {
                    (*ifacet)->deletable = false;
                    solid->contours.push_back(*icon);
                }
            }
        }
    }
    solid->facetno = (int)solid->contours.size();
}


/**
void  jbxl::CreateWingsList(BREP_SOLID* solid)

全 Wingの Listを作る．(Contourに結び付けられていないものも含む) @n
CreateContoursList()に依存する．
*/
DllExport void  jbxl::CreateWingsList(BREP_SOLID* solid)
{
    solid->wings.clear();

    BREP_CONTOUR_LIST::iterator icon;
    for (icon=solid->contours.begin(); icon!=solid->contours.end(); icon++) {
        BREP_WING* first = (*icon)->wing;
        if (first!=NULL) {
            BREP_WING* next = first;
            do {
                BREP_WING* wing = next;
                next = wing->next;
                solid->wings.push_back(wing);
                if (!wing->edge->complete) solid->wings.push_back(GetWingOtherSide(wing));
            } while(next!=first);
        }
    }
}


/**
void  jbxl::CreateSurplusContoursList(BREP_SOLID* solid)

多重Edgeを持つ Contourの Listを作る．@n
CreateContoursList()に依存する．
*/
DllExport void  jbxl::CreateSurplusContoursList(BREP_SOLID* solid)
{
//  DEBUG_MODE PRINT_MESG("Start CreateSurplusContoursList\n");
    solid->surplus_contours.clear();

    BREP_CONTOUR_LIST::iterator icon;
    for (icon=solid->contours.begin(); icon!=solid->contours.end(); icon++){
        if (DupEdgeNumber(*icon)>0) {
            solid->surplus_contours.push_back(*icon);
        }
    }
//  DEBUG_MODE PRINT_MESG("END CreateSurplusContoursList\n");
}


/**
void  jbxl::CreateShortageWingsList(BREP_SOLID* solid)

不完全な Wingの Listを作る．WingsListを利用する．@n
CreateContoursList(), CreateWingsList()に依存する．
*/
DllExport void  jbxl::CreateShortageWingsList(BREP_SOLID* solid)
{
    solid->shortage_wings.clear();

    BREP_WING_LIST::iterator iwing;
    for (iwing=solid->wings.begin(); iwing!=solid->wings.end(); iwing++){
        if ((*iwing)->contour==NULL) solid->shortage_wings.push_back(*iwing);
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
void  jbxl::DeleteSurplusContours(BREP_SOLID* solid)

多重Edgeを持つ Contour(Facet)の削除 @n
Contours List と Wings Listの内容は保障される．

@par 要求データ
- 全てのContour(Facet)の List (Contours List)．
    - 外側の関数で行う．CreateContoursList()を使用しても良い．
- 多重エッジの数が１以上のContour(Facet)が格納された作業List（SurplusContours List）．
    - 外側の関数で行う．CreateSuplusContoursList()を使用しても良い

@par アルゴリズム
-# 作業Listの中にある３辺が多重EdgeのFacetに削除マークをつける（高速化のため） 
    - 該当Facet(Contour)を削除し，そのFacet(Contour)を裏返して登録する （ReverseContours）
    - 多重Edgeが一つでもある場合は登録を中止
-# ２辺が多重Edgeの Facetに削除マークをつける．１と同様の処理を行う．
-# １辺が多重Edgeの Facetに削除マークをつける．１と同様の処理を行う．
-# この段階で，作業Listの中には Facetは存在しないはず．まだ存在する場合はエラー．
*/
DllExport void  jbxl::DeleteSurplusContours(BREP_SOLID* solid)
{
    if (solid->surplus_contours.empty()) return;
//  DEBUG_MODE PRINT_MESG("Start DeleteSurplusContours\n");
    
    CVCounter* counter = NULL;
    if (solid->counter!=NULL) counter = solid->counter->GetUsableCounter();
    if (counter!=NULL) counter->SetMax((int)solid->surplus_contours.size()*3);
    
    BREP_CONTOUR_LIST::iterator icon;
    // ３つの多重Edgeを持つ Facetの処理
    for (icon=solid->surplus_contours.begin(); icon!=solid->surplus_contours.end(); icon++){
        if ((*icon)->dup_edge==3) (*icon)->facet->deletable = true;
        if (counter!=NULL) counter->StepIt();
    }
    ReverseContours(solid);         // ３辺が多重Edgeの Facetを裏返す
    CreateShortageWingsList(solid);
    JoinShortageWings(solid);       // 一つにまとまるEdgeは一つにまとめる．

    CreateSurplusContoursList(solid);
    if (solid->surplus_contours.empty()) {
//      DEBUG_MODE PRINT_MESG("End   DeleteSurplusContours\n");
        return;
    }

    // ２つの多重Edgeを持つ Facetの処理
    for (icon=solid->surplus_contours.begin(); icon!=solid->surplus_contours.end(); icon++){
        if ((*icon)->dup_edge==2) (*icon)->facet->deletable = true;
        if (counter!=NULL) counter->StepIt();
    }
    ReverseContours(solid);         // ２辺が多重Edgeの Facetを裏返す
    CreateShortageWingsList(solid);
    JoinShortageWings(solid);   

    CreateSurplusContoursList(solid);
    if (solid->surplus_contours.empty()) {
//      DEBUG_MODE PRINT_MESG("End   DeleteSurplusContours\n");
        return;
    }

    // １つの多重Edgeを持つ Facetの処理
    for (icon=solid->surplus_contours.begin(); icon!=solid->surplus_contours.end(); icon++){
        if ((*icon)->dup_edge==1) (*icon)->facet->deletable = true;
        if (counter!=NULL) counter->StepIt();
    }
    ReverseContours(solid);         // １辺が多重Edgeの Facetを裏返す
    CreateShortageWingsList(solid);
    JoinShortageWings(solid);   
        
    CreateSurplusContoursList(solid);
    if (!solid->surplus_contours.empty()) {
//      DEBUG_MODE PRINT_MESG("EraseSurplusContour: 多重Edgeを持つ Facetデータが残っている？\n");
    }
//  DEBUG_MODE PRINT_MESG("End   DeleteSurplusContours\n");
    return;
}


/**
void  jbxl::DeleteStraightEdges(BREP_SOLID* solid)

複数のVertexが直線に並んだ場合，両端のVertex間に作られた Edge(Wing)に関連する Contour(Facet)を削除．@n
Contours List と Wings Listの内容は保障される．

@par 要求データ
- 全てのWingの List（Wings List）．
    - 外側の関数で行う．CreateWingsList()を使用しても良い．
- Contourに関連付けられていない（使用されていない）Wingが格納された作業List（ShortageWings List）．
    - 外側の関数で行う．CreateShortageWingsList() を使用しても良い．
*/
DllExport void  jbxl::DeleteStraightEdges(BREP_SOLID* solid)
{
    if (solid->shortage_wings.empty()) return;
//  DEBUG_MODE PRINT_MESG("Start DeleteStraightEdges\n");

    CVCounter* counter = NULL;
    if (solid->counter!=NULL) counter = solid->counter->GetUsableCounter();
    if (counter!=NULL) counter->SetMax((int)solid->shortage_wings.size());

    BREP_WING_LIST::iterator wing1;
    for (wing1=solid->shortage_wings.begin(); wing1!=solid->shortage_wings.end(); wing1++){
        BREP_VERTEX* vert[3];
        vert[0] = (*wing1)->vertex;
        BREP_WING_LIST::iterator wing2;
        for (wing2=solid->shortage_wings.begin(); wing2!=solid->shortage_wings.end(); wing2++){
            if (IsConnectEdges(*wing1, *wing2)) {
                vert[1] = (*wing2)->vertex;
                vert[2] = GetWingOtherSide(*wing2)->vertex;
                IsForbiddenEdge(vert);  // 直線に並んだ Vertexを禁止する．
            }
        }
        if (counter!=NULL) counter->StepIt();
    }
    CreateContoursList(solid);          // 該当Contour(Facet)を削除し，全てのContour(Facet)の Listを作り直す．
    CreateWingsList(solid);             // WingsListを作り直す．
    
//  DEBUG_MODE PRINT_MESG("End   DeleteStraightEdges\n");
    return;
}


/**
void  jbxl::DeleteShortageWings(BREP_SOLID* solid) 

不完全 Wing(Edge)を持つFacet(Contour)を削除 @n
Contours List と Wings Listの内容は保障される．

@par 要求データ
- ShortageWings List．CreateShortageWingsList()で作成しても良い
*/
DllExport void  jbxl::DeleteShortageWings(BREP_SOLID* solid) 
{
    if (solid->shortage_wings.empty()) return;
//  DEBUG_MODE PRINT_MESG("Start DeleteShortageWings\n");

    CVCounter* counter = NULL;
    if (solid->counter!=NULL) counter = solid->counter->GetUsableCounter();
    if (counter!=NULL) counter->SetMax((int)solid->shortage_wings.size());

    BREP_WING_LIST::iterator iwing;
    for (iwing=solid->shortage_wings.begin(); iwing!=solid->shortage_wings.end(); iwing++){
        if ((*iwing)->contour!=NULL) (*iwing)->contour->facet->deletable = true;
        BREP_WING* othwing = GetWingOtherSide(*iwing);
        if (othwing->contour!=NULL)  othwing->contour->facet->deletable = true;
        if (counter!=NULL) counter->StepIt();
    }

    CreateContoursList(solid);
    CreateWingsList(solid);
//  DEBUG_MODE PRINT_MESG("End   DeleteShortageWings\n");
    return;
}


#define  JBXL_BREP_MAX_LOOPCNT  20


/**
void  jbxl::FillShortageWings(BREP_SOLID* solid, int method, bool mode)

足りないContourを補充する（全ての不足Contourが無くなるか，補充不可能になるまで）．@n
Contours List, Wings List, SohrtageWins Listの内容は保障される．

@par 要求データ
- ShortageWings List．CreateShortageWingsList()で作成しても良い

@param  solid   ソリッドデータへのポインタ
@param  method  @b 1: 三番目のVertexを探すアルゴリズムは Next.
@param  method  @b 2: 三番目のVertexを探すアルゴリズムは Near.
@param  mode    @b true : 多重Edgeを認める．
@param  mode    @b false: 多重Edgeを認めない．
*/
DllExport void  jbxl::FillShortageWings(BREP_SOLID* solid, int method, bool mode)
{
    if (solid->shortage_wings.empty()) return;
//  DEBUG_MODE PRINT_MESG("Start FillShortageWings\n");

    int wmax = (int)solid->shortage_wings.size();
    CVCounter* counter = NULL;
    if (solid->counter!=NULL) counter = solid->counter->GetUsableCounter();
    if (counter!=NULL) counter->SetMax(wmax);

    int lpc = 0;
    int pno = 1;
    while (solid->shortage_wings.size()>1 && pno!=0 && lpc<JBXL_BREP_MAX_LOOPCNT) {
        if      (method==1) pno = FillShortageWings_Next(solid, mode);
        else if (method==2) pno = FillShortageWings_Near(solid, mode);
        else {
//          DEBUG_MODE PRINT_MESG("FillShortageWings: 知らない手法が指定された %d\n", method);
//          DEBUG_MODE PRINT_MESG("End   FillShortageWings\n");
            return;
        }
        CreateShortageWingsList(solid);
        //DEBUG_MODE PRINT_MESG("FillShortageWings: 不完全Wingデータは %d/%d 個  %d\n", solid->shortage_wings.size(), wmax, pno);
        lpc++;
    }

//  DEBUG_MODE PRINT_MESG("End   FillShortageWings\n");
    return;
}


/**
int  jbxl::FillShortageWings_Next(BREP_SOLID* solid, bool mode)

足りないContourを補充する（一周期のみ）．Nextアルゴリズム

Edgeに繋がっているEdgeを探してそのVertexから三角形(Contour)を作る．@n
Contours List と Wings Listの内容は保障される．

@par 要求データ
- ShortageWings List．CreateShortageWingsList()で作成しても良い

@param  solid  ソリッドデータへのポインタ
@param  mode   @b true : 多重Edgeを認める．
@param  mode   @b false: 多重Edgeを認めない．
*/
DllExport int  jbxl::FillShortageWings_Next(BREP_SOLID* solid, bool mode)
{
    int patchno = 0;

    BREP_WING_LIST::iterator wing1;
    for (wing1=solid->shortage_wings.begin(); wing1!=solid->shortage_wings.end(); wing1++){
        if (!IsIncludeCompleteEdge(*wing1)) {
            BREP_VERTEX* vert[3];

            vert[0] = (*wing1)->vertex;
            BREP_WING_LIST::iterator wing2;
            for (wing2=solid->shortage_wings.begin(); wing2!=solid->shortage_wings.end(); wing2++){
                if (!IsIncludeCompleteEdge(*wing2) && IsConnectEdges(*wing1, *wing2)) {
                    vert[1] = (*wing2)->vertex;
                    vert[2] = GetWingOtherSide(*wing2)->vertex; // vert[]: Contourを貼る頂点
                    BREP_SHELL* shell = GetWingOtherSide(*wing1)->contour->facet->shell;
                    if (PatchupContour(shell, vert, mode)) patchno++;
                }
            }
        }
    }
    CreateContoursList(solid);
    CreateWingsList(solid);

    return patchno;
}


#define  JBXL_BREP_MAX_QUEUESIZE  1


/**
DllExport int  jbxl::FillShortageWings_Near(BREP_SOLID* solid, bool mode)

足りないContourを補充する（一周期のみ）．Nearアルゴリズム

Edgeの中心から近いVertexを探して三角形(Contour)を作る．@n
Contours List と Wings Listの内容は保障される．

@par 要求データ
- ShortageWings List．CreateShortageWingsList()で作成しても良い

@param  solid  ソリッドデータへのポインタ
@param  mode   @b true : 多重Edgeを認める．
@param  mode   @b false: 多重Edgeを認めない．
*/
DllExport int  jbxl::FillShortageWings_Near(BREP_SOLID* solid, bool mode)
{
    int patchno = 0;
    BREP_VERTEX_LIST  vertex_list;

    BREP_WING_LIST::iterator wing1;
    for (wing1=solid->shortage_wings.begin(); wing1!=solid->shortage_wings.end(); wing1++){
        if (!IsIncludeCompleteEdge(*wing1)) {
            BREP_VERTEX* vert[3];
            vert[0] = (*wing1)->vertex;
            vert[1] = GetWingOtherSide(*wing1)->vertex;
            vert[2] = NULL;
            vertex_list.clear();

            BREP_WING_LIST::iterator wing2;
            for (wing2=solid->shortage_wings.begin(); wing2!=solid->shortage_wings.end(); wing2++){
                if (wing1!=wing2) {
                    BREP_VERTEX* vrtx = (*wing2)->vertex;
                    if (vrtx!=vert[0] && vrtx!=vert[1]) {
                    /*  int cnt = 0;
                        BREP_VERTEX* v = vert[1];
                        do {
                            v = FindConnectEdgeVertex(v);
                            cnt++;
                        } while (v!=vert[0] && v!=vert[1] && v!=vrtx && v!=NULL && cnt<100);
                    */  
                    //  if (v!=vrtx) {
                            Vector<double> vect = (*wing1)->edge->center - vrtx->point;  
                            vrtx->distance2 = vect.norm2();
                            SetMinVertex(&vertex_list, vrtx);
                    //  }
                    }
                }
            }

            int cnt=0;
            BREP_SHELL* shell = GetWingOtherSide(*wing1)->contour->facet->shell;
            BREP_VERTEX_LIST::iterator ivert;
            for (ivert=vertex_list.begin(); ivert!=vertex_list.end(); ivert++){
                vert[2] = *ivert;
                if (PatchupContour(shell, vert, mode)) {
                    patchno++;
                    break;
                }
                cnt++;
                if (cnt>=JBXL_BREP_MAX_QUEUESIZE) break;
            }

        }
    }
    CreateContoursList(solid);
    CreateWingsList(solid);

    return patchno;
}


/**
void  jbxl::SetMinVertex(BREP_VERTEX_LIST* list, BREP_VERTEX* vrtx)

FillShortageWings_Near()用
*/
DllExport void  jbxl::SetMinVertex(BREP_VERTEX_LIST* list, BREP_VERTEX* vrtx)
{
    BREP_VERTEX_LIST::iterator ivert;
    for (ivert=list->begin(); ivert!=list->end(); ivert++){
        if (vrtx==(*ivert)) return; // 既に登録済み
        if (vrtx->distance2 < (*ivert)->distance2) {
            list->insert(ivert, vrtx);
            return;
        }
    }
    if (ivert==list->end()) list->push_back(vrtx);
    return;
}


/**
BREP_VERTEX*  jbxl::FindConnectEdgeVertex(BREP_VERTEX* vert)

vertexと Edgeで繋がった vertexの内，最初に見つけたものを返す．
*/
DllExport BREP_VERTEX*  jbxl::FindConnectEdgeVertex(BREP_VERTEX* vert)
{
    BREP_WING_LIST::iterator iwing;
    for (iwing=vert->wing_list.begin(); iwing!=vert->wing_list.end(); iwing++){
        if ((*iwing)->contour==NULL) {
            return GetWingOtherSide(*iwing)->vertex;
        }
    }
    return NULL;
}


/**
bool  jbxl::PatchupContour(BREP_SHELL* shell, BREP_VERTEX** vert, bool mode)

Shellに対して，vert[0]〜vert[2]の三角形Contourを補充する．

@param          shell  シェルデータへのポインタ
@param[in,out]  vert   頂点情報．補填された三角形の頂点情報が追加される．
@param          mode   @b true:  多重Edgeを認める．
@param          mode   @b false: 多重Edgeを認めない．
*/
DllExport bool  jbxl::PatchupContour(BREP_SHELL* shell, BREP_VERTEX** vert, bool mode)
{
    if (vert[0]->forbidden_list!=NULL) {
        BREP_VERTEX_LIST::iterator vertex; 
        vertex = std::find(vert[0]->forbidden_list->begin(), vert[0]->forbidden_list->end(), vert[2]);
        if (vertex!=vert[0]->forbidden_list->end()) return false;   // 禁止Listの中に見つけた． 
    }

    BREP_SOLID* solid  = shell->solid;
    CVCounter* counter = NULL;
    if (solid->counter!=NULL) counter = solid->counter->GetUsableCounter();

    BREP_FACET* facet  = new BREP_FACET(shell);
    BREP_CONTOUR* contour = CreateContourByVertex(facet, vert);     
    if (contour!=NULL) {
        if (DupEdgeNumber(contour)==0 || mode) {
            BREP_CONTOUR* collision;
            facet->CloseData();
            if (!IsCollisionContours(solid, contour, &collision)) {
                solid->contours.push_back(contour);
//              PrintFacetAsciiSTL(contour);
                if (counter!=NULL) counter->StepIt();
                return true;
            }
            else FastDeleteFacet(facet);
        }
        else FastDeleteFacet(facet);
    }
    else FastDeleteFacet(facet); 

    return  false;
}


/**
void  jbxl::ReverseContours(BREP_SOLID* solid)

削除マークの付いたFacet(Contour)を削除し，同じ頂点を持つContourを裏返して登録する.
もし，登録した Contourに多重 Edgeがある場合は登録を中止する．@n
ただし，読み込み（初期）データは削除しない．

Contours List と Wings Listの内容は保障される．
*/
DllExport void  jbxl::ReverseContours(BREP_SOLID* solid)
{
    // 全ての Facetに対して
    BREP_SHELL_LIST shells = solid->shells;
    BREP_SHELL_LIST::iterator ishell;
    for (ishell=shells.begin(); ishell!=shells.end(); ishell++){
        BREP_FACET_LIST::iterator ifacet;
        for (ifacet =(*ishell)->facets.begin(); ifacet!=(*ishell)->facets.end(); ifacet++){
            // 削除マークが付いているなら
            if ((*ifacet)->deletable && !(*ifacet)->notdelete) {
                // 逆周りで頂点を Vector vに格納
                BREP_VERTEX*  v[3];
                BREP_FACET*   pbfc;
                BREP_CONTOUR* pbcn;
                BREP_CONTOUR_LIST::iterator icon;
                for (icon=(*ifacet)->outer_contours.begin(); icon!=(*ifacet)->outer_contours.end(); icon++) {
                    BREP_WING* wing = (*icon)->wing;
                    for (int i=0; i<3; i++) {
                        v[i] = wing->vertex;
                        wing = wing->prev;
                    }
                }

                // 削除マークが付いている Facetを削除
                pbfc   = *ifacet;
                ifacet = (*ishell)->facets.erase(ifacet);
                ifacet--;
                pbfc->shell = NULL;
                delete(pbfc);

                // 裏返しの Contourを登録
                pbfc = new BREP_FACET(*ishell);
                pbcn = CreateContourByVertex(pbfc, v); 
                if (pbcn!=NULL) {
                    if (DupEdgeNumber(pbcn)==0) {
                        pbfc->CloseData();
                    }
                    else {
                        FastDeleteFacet(pbfc);
                    }
                }
                else {
                    FastDeleteFacet(pbfc);
                }
            }
        }
    }

    CreateContoursList(solid);  
    CreateWingsList(solid);
}


/**
void  jbxl::JoinShortageWings(BREP_SOLID* solid)

不完全かつ多重Edgeになっているが，実は一つのEdgeにまとめることができるものを一つにまとめる．@n
Contours List と Wings Listの内容は保障される．

@par 要求データ
- ShortageWings List．CreateShortageWingsList()で作成しても良い
*/
DllExport void  jbxl::JoinShortageWings(BREP_SOLID* solid)
{
    if (solid->shortage_wings.empty()) return;
//  DEBUG_MODE PRINT_MESG("Start JoinShortageWings\n");

    BREP_WING_LIST::iterator iwing;
    for (iwing=solid->shortage_wings.begin(); iwing!=solid->shortage_wings.end(); iwing++){
        BREP_WING* wingA = (*iwing);

        if (GetWingOtherSide(wingA)->contour==NULL) {       // 両方NULL
            delete (wingA->edge);
            continue;
        }
        
        if (wingA->edge->edge_list!=NULL) {
            BREP_EDGE_LIST* edge_list = wingA->edge->edge_list;
            BREP_EDGE_LIST::iterator iedge;
            for (iedge=edge_list->begin(); iedge!=edge_list->end(); iedge++){
                if (wingA==(*iedge)->wing1 || wingA==(*iedge)->wing2) continue;
                if ((*iedge)->wing1->contour!=NULL && (*iedge)->wing2->contour!=NULL) continue;

                BREP_WING* wingB;
                if      ((*iedge)->wing1->contour!=NULL) wingB = (*iedge)->wing1;
                else if ((*iedge)->wing2->contour!=NULL) wingB = (*iedge)->wing2;
                else continue;  // 両方NULLをこちらが先に見つけた．→ 上のdeleteコードに任せる．

                if (wingA->vertex==wingB->vertex) {
                    if      (wingA->edge->wing1==wingA) wingA->edge->wing1 = wingB;
                    else if (wingA->edge->wing2==wingA) wingA->edge->wing2 = wingB;
                    if      (wingB->edge->wing1==wingB) wingB->edge->wing1 = wingA;
                    else if (wingB->edge->wing2==wingB) wingB->edge->wing2 = wingA;

                    BREP_EDGE* sedge = wingB->edge;
                    wingB->edge = wingA->edge;
                    wingA->edge = sedge;
                    wingB->edge->complete = true;
                    wingA->edge->complete = false;
                    break;
                }
            }
        }
    }
    CreateContoursList(solid);
    CreateWingsList(solid);

//  DEBUG_MODE PRINT_MESG("End   JoinShortageWings\n");
    return;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
int  jbxl::DupEdgeNumber(BREP_CONTOUR* contour)

Contourの多重Edgeの数を数える．
*/
DllExport int  jbxl::DupEdgeNumber(BREP_CONTOUR* contour)
{
    contour->dup_edge = 0;

    BREP_WING* first = contour->wing;
    if (first!=NULL) {
        BREP_WING* next = first;
        do {
            BREP_WING* wing = next;
            next = wing->next;
            if (wing->edge->edge_list!=NULL) contour->dup_edge++;
        } while(next!=first);
    }
    return  contour->dup_edge;
}


/**
BREP_CONTOUR*  jbxl::CreateContourByVector(BREP_FACET* facet, Vector<double>* vect, Vector<double>* nrml, UVMap<double>* uvmp, Vertex_Weight* wght, bool dupli)

Facet と Vector[] から Contour を作る．

vect[0]〜vect[2] は３点の位置ベクトル（順序付けされている）
nrml[0]〜nrml[2] は３点の法線ベクトル（NULL なら再計算される）
uvmp[0]〜uvmp[2] は３点の曲面座標データ．

@param  facet FACETデータへのポインタ
@param  vect  3個の頂点データ vect[3] へのポインタ
@param  nrml  頂点の法線ベクトルデータへのポインタ．
@param  uvmp  頂点の曲面座標データへのポインタ．
@param  weght 頂点の重みデータへのポインタ．
@param  dupli true: 頂点の重複登録を許可する．false: 重複登録を許可しない．
*/
DllExport BREP_CONTOUR*  jbxl::CreateContourByVector(BREP_FACET* facet, Vector<double>* vect, Vector<double>* nrml, UVMap<double>* uvmp, Vertex_Weight* wght, bool dupli)
{
    BREP_SOLID*   solid;
    BREP_CONTOUR* contour;
    BREP_VERTEX*  vertex[3];

    solid = facet->shell->solid;

    // Vertex の生成
    for (int i=0; i<3; i++) {
        vertex[i] = new BREP_VERTEX();
        vertex[i]->point = vect[i];
        if (nrml!=NULL) {
            vertex[i]->calc_normal = false;
            vertex[i]->normal = nrml[i];
        }
        if (uvmp!=NULL) {
            vertex[i]->uvmap  = uvmp[i];
        }
        if (wght!=NULL) {
            vertex[i]->weight = wght[i];
        }
        vertex[i]->CloseData();
    }

    // 同じ点がある．または３点が直線上にある．
    if (vertex[0]==vertex[1] || vertex[1]==vertex[2] || vertex[0]==vertex[2]) return NULL;
    if (IsForbiddenEdge(vertex)) return NULL;

    // Vertexをソリッドの Octreeに登録
    BREP_VERTEX* vert;
    for (int i=0; i<3; i++) {
        vert = AddVertex2Octree(vertex[i], solid->octree, dupli);
        if (vert!=vertex[i]) {
            delete vertex[i];
            vertex[i] = vert;
        }
    }

    contour = CreateContourByVertex(facet, vertex); 

    return  contour;
}


/**
BREP_CONTOUR*  jbxl::CreateContourByVertex(BREP_FACET* facet, BREP_VERTEX** vertex)

Facet と Vertex[3] から Vertex を作る．
vertex[0]〜vertex[2] は 3点の Vertex（順序付けされている）
*/
DllExport BREP_CONTOUR*  jbxl::CreateContourByVertex(BREP_FACET* facet, BREP_VERTEX** vertex)
{
    // 同じ点がある．または３点が直線上にある．
    if (vertex[0]==vertex[1] || vertex[1]==vertex[2] || vertex[0]==vertex[2]) return NULL;
    if (IsForbiddenEdge(vertex)) return NULL;

    BREP_CONTOUR* contour = new BREP_CONTOUR(facet);

    // Wingを作る．
    BREP_WING*      wing = contour->CreateWing(vertex[0], vertex[1]);
    if (wing!=NULL) wing = contour->CreateWing(vertex[1], vertex[2]);
    if (wing!=NULL) wing = contour->CreateWing(vertex[2], vertex[0]);
    if (wing!=NULL) {
        contour->CloseData();  // 面の法線ベクトルを計算．
    }
    else {
        delete(contour);
        contour = NULL;
    }

    return  contour;
}


/**
bool  jbxl::IsIncludeCompleteEdge(BREP_WING* wing)

Wingに関連したEdge及びその多重Edgeは完全か？ @n
そこにあるEdgeの内，最低一つは閉じているか？
*/
DllExport bool  jbxl::IsIncludeCompleteEdge(BREP_WING* wing)
{
    BREP_EDGE* edge = wing->edge;
    if (edge->complete) return true;
    if (edge->edge_list!=NULL) {
        BREP_EDGE_LIST::iterator iedge;
        for (iedge=edge->edge_list->begin(); iedge!=edge->edge_list->end(); iedge++){
            if ((*iedge)->complete) return true;
        }
    }
    return false;
}


/**
void  jbxl::FastDeleteFacet(BREP_FACET* facet)

直前に生成した Facetを高速に削除する．
 
直前に生成したものでない Facetを指定した場合は誤作動する．@n
Facetの数が多い場合，delete(Facet) または ~BREP_FACET()を実行すると一々find()を呼び出すので時間が掛かる．
*/
DllExport void  jbxl::FastDeleteFacet(BREP_FACET* facet)
{
    if (facet==NULL) return;
    if (facet->shell!=NULL) {
        facet->shell->facets.pop_back();
        facet->shell = NULL;
    }

    // Destroy its contours
    BREP_CONTOUR*  bpcn;
    BREP_CONTOUR_LIST::iterator contour = facet->outer_contours.begin();

    while (contour!=facet->outer_contours.end()) {
        bpcn = *contour;
        contour = facet->outer_contours.erase(contour);
        bpcn->facet = NULL;
        delete (bpcn);  
    }
    free(facet);
    
    return;
}


/**
void  jbxl::SetDeletableContoursByEdge(BREP_EDGE* edge)

Edgeに関連付けられた Contourに削除可能のマークをつける．削除は行わない．
*/
DllExport void  jbxl::SetDeletableContoursByEdge(BREP_EDGE* edge)
{
    if (edge->edge_list!=NULL) {
        BREP_EDGE_LIST::iterator iedge;
        for (iedge=edge->edge_list->begin(); iedge!=edge->edge_list->end(); iedge++){
            if ((*iedge)->wing1->contour!=NULL) {
                (*iedge)->wing1->contour->facet->deletable = true;
            }
            if ((*iedge)->wing2->contour!=NULL) {
                (*iedge)->wing2->contour->facet->deletable = true;
            }
        }
    }
    else {
        if (edge->wing1->contour!=NULL) {
            edge->wing1->contour->facet->deletable = true;
        }
        if (edge->wing2->contour!=NULL) {
            edge->wing2->contour->facet->deletable = true;
        }
    }
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
//

/*
int  jbxl::IsAtLine(BREP_VERTEX** v) 

３点 *v[0], *v[1], *v[2] が一直線上にあるかどうかを調べる.

@attention
toleranceの計算は再考が必要か？
*/
DllExport int  jbxl::IsAtLine(BREP_VERTEX** v) 
{
    TVector<double> vect  = Vertex2TVector(v[0]);
    TVector<double> vect1 = Vertex2TVector(v[1]);
    TVector<double> vect2 = Vertex2TVector(v[2]) - vect;
    TVector<double> vect3 = Vertex2TVector(v[2]) - vect1;
    vect1 = vect1 - vect;

    int mode = 0;
    double tt;
    double th = ProportionVector(vect2, vect1, tt);
    tt = Max(tt, Zero_Eps);
    if (th>1.-tt) mode = 1;         // 0->1->2
    else {
        th = ProportionVector(vect1, vect2, tt);
        tt = Max(tt, Zero_Eps);
        if (th>1.-tt) mode = 2;     // 0->2->1
        else {
            th = ProportionVector(vect3, vect2, tt);
            tt = Max(tt, Zero_Eps);
            if (th>1.-tt) mode = 3; // 1->0->2
        }
    }
    return  mode;
}


/**
bool  jbxl::IsForbiddenEdge(BREP_VERTEX** vert)

Vertex vert[0]〜vert[2] が一直線に並んでいるか調べる．@n
一直線の場合は，該当Edgeを禁止する．また，既にその Edgeがある場合は関連 Contourを削除する．
*/
DllExport bool  jbxl::IsForbiddenEdge(BREP_VERTEX** vert)
{
    BREP_EDGE* edge = NULL;
    int mode = IsAtLine(vert);
    if (mode==1) {      // 0->1->2
        if (vert[0]->forbidden_list==NULL) vert[0]->forbidden_list = new BREP_VERTEX_LIST();
        if (vert[2]->forbidden_list==NULL) vert[2]->forbidden_list = new BREP_VERTEX_LIST();
        vert[0]->forbidden_list->push_back(vert[2]);
        vert[2]->forbidden_list->push_back(vert[0]);
        edge = FindEdge(vert[0], vert[2]);
    }
    else if (mode==2) { // 0->2->1
        if (vert[0]->forbidden_list==NULL) vert[0]->forbidden_list = new BREP_VERTEX_LIST();
        if (vert[1]->forbidden_list==NULL) vert[1]->forbidden_list = new BREP_VERTEX_LIST();
        vert[0]->forbidden_list->push_back(vert[1]);
        vert[1]->forbidden_list->push_back(vert[0]);
        edge = FindEdge(vert[0], vert[1]);
    }
    else if (mode==3) { // 2->0->1
        if (vert[1]->forbidden_list==NULL) vert[1]->forbidden_list = new BREP_VERTEX_LIST();
        if (vert[2]->forbidden_list==NULL) vert[2]->forbidden_list = new BREP_VERTEX_LIST();
        vert[1]->forbidden_list->push_back(vert[2]);
        vert[2]->forbidden_list->push_back(vert[1]);
        edge = FindEdge(vert[1], vert[2]);
    }
    if (edge!=NULL) SetDeletableContoursByEdge(edge);

    if (mode==0) return false;
    return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
bool jbxl::IsCollisionContours(BREP_SOLID* solid, BREP_CONTOUR* contour, BREP_CONTOUR** collision)

Contour が他の Contourと衝突しているかどうかをチェックする．

@note
処理スピードは遅い
*/
DllExport bool jbxl::IsCollisionContours(BREP_SOLID* solid, BREP_CONTOUR* contour, BREP_CONTOUR** collision)
{
    *collision = NULL;

    BREP_CONTOUR_LIST::iterator icon;
    for (icon=solid->contours.begin(); icon!=solid->contours.end(); icon++){
        if (!disJunctBounds(contour->facet->rbound, (*icon)->facet->rbound)) {
        if (!(*icon)->facet->deletable || (*icon)->facet->notdelete) {
            int svrtx = CommonVertex(*icon, contour);
            if (svrtx==3) {
                *collision = *icon;
                return true;
            }
            
            int lineno;
            if (SamePlaneContour(*icon, contour, lineno)) {
                // ３角形の内部点
                if (!contour->hasCollisionVector) contour->ComputeDirectRS();
                if (IsInTriangle(*icon, contour)) {
                    *collision = *icon;
                    return true;
                }
                if (!(*icon)->hasCollisionVector) (*icon)->ComputeDirectRS();
                if (IsInTriangle(contour, *icon)) {
                    *collision = *icon;
                    return true;
                }
                
                // 2D衝突検査
                if (CollisionTriContour2D(*icon, contour)) {
                    *collision = *icon;
                    return true;
                }
            }
            else {
                if (lineno==1) continue;
                // 3D衝突検査
                if (!contour->hasCollisionVector) contour->ComputeDirectRS();
                if (CollisionTriContour3D(*icon, contour)) {
                    *collision = *icon;
                    return true;
                }
                if (!(*icon)->hasCollisionVector) (*icon)->ComputeDirectRS();
                if (CollisionTriContour3D(contour, *icon)) {
                    *collision = *icon;
                    return true;
                }
            }
        }
        }
    }
    return false;
}


/**
bool  jbxl::CollisionTriContour3D(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2)

contour1と contour2が衝突しているかどうかをチェックする．@n
contour2->directRS, directR, directS が予め計算されていなければならない．→例えば ComputeDirectRS() を使用 
*/
DllExport bool  jbxl::CollisionTriContour3D(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2)
{
    double  tc, uc, vc, tm, um, ut, vt, tt, tmt, umt;
    BREP_WING* wing  = contour1->wing;
    TVector<double> directR  = contour2->directR;
    TVector<double> directS  = contour2->directS;
    TVector<double> directRS = contour2->directRS;
    TVector<double> directT;
    TVector<double> directB;
    TVector<double> directQB;
    TVector<double> directN;

    // 各辺による衝突検出
    for (int i=0; i<3; i++) {
        TVector<double> point   = Vertex2TVector(wing->vertex);
        TVector<double> directQ = Vertex2TVector(contour2->wing->vertex) - point;

        for (int j=0; j<2; j++) {       
            directN = Vertex2TVector(wing->next->vertex);
            if (j==0) directB = directN - point;
            else      directB = directB + (Vertex2TVector(wing->next->next->vertex) - directN)*0.5;
            directQB = directQ^directB;

            um  = directRS*directB;
            tm  = directQB*directS;
            umt = TVectorMultiTolerance(directRS, directB);
            tmt = TVectorMultiTolerance(directQB, directS);

            if (Xabs(um)>Max(umt, Zero_Eps) && Xabs(tm)>Max(tmt, Zero_Eps)) {
                uc =  tm;
                tc = -directQB*directR;
                vc =  directRS*directQ;
                ut = tmt;
                tt = TVectorMultiTolerance(directQB, directR);
                vt = TVectorMultiTolerance(directRS, directQ);

                ut = Max(Collision_Tolerance, (um*ut+umt*uc)/(um*um));
                tt = Max(Collision_Tolerance, (tm*tt+tmt*tc)/(tm*tm));
                vt = Max(Collision_Tolerance, (um*vt+umt*vc)/(um*um));
                uc  = uc/um;
                tc  = tc/tm;
                vc  = vc/um;
                directT = directR + tc*directS;

                if (uc>ut && 1.-uc>ut && tc>tt && 1.-tc>tt && vc>vt && 1.-vc>vt &&
                    uc*directT.n>directT.t && (1.-uc)*directT.n>directT.t &&
                    tc*directS.n>directS.t && (1.-tc)*directS.n>directS.t && 
                    vc*directB.n>directB.t && (1.-vc)*directB.n>directB.t) { 
                //  DEBUG_MODE PRINT_MESG("3D衝突 %d !!!!  %e  %e  %e\n", j+1, uc, tc, vc);
                //  PrintFacetAsciiSTL(contour1);
                //  PrintFacetAsciiSTL(contour2);
                    return true;
                }
            }
        }
        wing = wing->next;
    }

    return false;
}


DllExport bool  jbxl::CollisionTriContour2D(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2)
{
    double  uc, vc, tm, um, ut, vt, tmt, umt;
    BREP_WING* wing = contour1->wing;
    TVector<double> directR  = contour2->directR;
    TVector<double> directS  = contour2->directS;
    TVector<double> directRS = contour2->directRS;
    
    wing = contour1->wing;
    for (int i=0; i<3; i++) {
        TVector<double> point    = Vertex2TVector(wing->vertex);
        TVector<double> directB  = Vertex2TVector(wing->next->vertex) - point;
        TVector<double> directQ  = Vertex2TVector(contour2->wing->vertex) - point;
        TVector<double> directQB = directQ^directB;
        directB.norm();

        tm  = directQB*directS;
        um  = directRS*directB;
        tmt = TVectorMultiTolerance(directQB, directS);
        umt = TVectorMultiTolerance(directRS, directB);

        if (Xabs(tm)<=Max(tmt, Zero_Eps) || Xabs(um)<=Max(umt, Zero_Eps)) {
            TVector<double> directBR = directB^directR;
            TVector<double> directQR = directQ^directR;
            TVector<double> directQB = directQ^directB;
            uc = ProportionVector(directQB, directBR, ut);
            vc = ProportionVector(directQR, directBR, vt);
            ut = Max(ut, Collision_Tolerance);
            vt = Max(vt, Collision_Tolerance);
            if (uc>ut && 1.-uc>ut && vc>vt && 1.-vc>vt && 
                uc*directR.n>directR.t && (1.-uc)*directR.n>directR.t &&
                vc*directB.n>directB.t && (1.-vc)*directB.n>directB.t) { 
    //              DEBUG_MODE PRINT_MESG("同一平面上1  %e  %e    %e  %e\n", uc, vc, ut, vt);
    //              PrintFacetAsciiSTL(contour1);
    //              PrintFacetAsciiSTL(contour2);
                    return true;
            }

            directBR = directB^directS;
            directQR = (directQ+directR)^directS;
            directQB = (directQ+directR)^directB;
            uc = ProportionVector(directQB, directBR, ut);
            vc = ProportionVector(directQR, directBR, vt);
            ut = Max(ut, Collision_Tolerance);
            vt = Max(vt, Collision_Tolerance);
            if (uc>ut && 1.-uc>ut && vc>vt && 1.-vc>vt &&  
                uc*directS.n>directS.t && (1.-uc)*directS.n>directS.t &&
                vc*directB.n>directB.t && (1.-vc)*directB.n>directB.t) { 
//              DEBUG_MODE PRINT_MESG("同一平面上2  %e  %e    %e  %e\n", uc, vc, ut, vt);
//              PrintFacetAsciiSTL(contour1);
//              PrintFacetAsciiSTL(contour2);
                return true;
            }

            TVector<double> directT = directR + directS;
            directBR = directB^directT;
            directQR = directQ^directT;
            directQB = directQ^directB;
            uc = ProportionVector(directQB, directBR, ut);
            vc = ProportionVector(directQR, directBR, vt);
            ut = Max(ut, Collision_Tolerance);
            vt = Max(vt, Collision_Tolerance);
            if (uc>ut && 1.-uc>ut && vc>vt && 1.-vc>vt &&
                uc*directT.n>directT.t && (1.-uc)*directT.n>directT.t &&
                vc*directB.n>directB.t && (1.-vc)*directB.n>directB.t) { 
//              DEBUG_MODE PRINT_MESG("同一平面上3  %e  %e    %e  %e\n", uc, vc, ut, vt);
//              PrintFacetAsciiSTL(contour1);
//              PrintFacetAsciiSTL(contour2);
                return true;
            }
        }

        wing = wing->next;
    }

    return false;
}


DllExport bool  jbxl::IsInTriangle(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2)
{
    double  tc, uc, tt, ut;
    TVector<double>  directS  = contour2->directS;
    TVector<double>  directRS = contour2->directRS;
    TVector<double>  directQ, directT, directN;
    BREP_WING* wing = contour1->wing;

    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            directN = Vertex2TVector(wing->vertex);
            if (j==0) directQ = Vertex2TVector(contour2->wing->vertex) - directN;
            else      directQ = directQ - (Vertex2TVector(wing->next->vertex) - directN)*0.5;

            TVector<double>  directSQ = directS^directQ;
            TVector<double>  directQR = directQ^contour2->directR;
            tc = ProportionVector(directQR, directSQ, tt);
            uc = ProportionVector(directSQ, directRS, ut);
            directT = contour2->directR + tc*directS;
            ut = Max(ut, Collision_Tolerance);
            tt = Max(tt, Collision_Tolerance);
            if (tc>tt && 1.-tc>tt && uc>ut && 1.-uc>ut &&
                tc*directS.n>directS.t && (1.-tc)*directS.n>directS.t &&
                uc*directT.n>directT.t && (1.-uc)*directT.n>directT.t) {
//              DEBUG_MODE PRINT_MESG("三角形の内部 %d : %e  %e    %e  %e\n", j+1, tc, uc, tt, ut);
                //PrintFacetAsciiSTL(contour1);
                //PrintFacetAsciiSTL(contour2);
                return true;
            }
        }
    }
    return false;
}


DllExport bool  jbxl::SamePlaneContour(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2, int& lineno)
{
    double  um, umt, tm, tmt;
    BREP_WING* wing = contour1->wing;
    TVector<double>  directB, directQ;

    lineno = 0;
    for (int i=0; i<3; i++) {
        directB = Vertex2TVector(wing->next->vertex)     - Vertex2TVector(wing->vertex);
        directQ = Vertex2TVector(contour2->wing->vertex) - Vertex2TVector(wing->vertex);
        um  = contour2->directRS*directB;
        tm  = contour2->directRS*directQ;
        umt = TVectorMultiTolerance(contour2->directRS, directB);
        tmt = TVectorMultiTolerance(contour2->directRS, directQ);
        if (Xabs(um)<Max(umt, Zero_Eps) && Xabs(tm)<Max(tmt, Zero_Eps)) {
            lineno++;
        }
    }

    if (lineno>=2) return true;
    return  false;
}


DllExport int  jbxl::CommonVertex(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2)
{
    BREP_VERTEX_LIST  vertex_list;

    int cnt = 0;
    BREP_WING* wing1 = contour1->wing;
    for (int i=0; i<3; i++) {
        bool common = false;
        BREP_WING* wing2 = contour2->wing;
        for (int j=0; j<3; j++) {
            if (wing1->vertex==wing2->vertex) {
                cnt++;
                common = true;
                break;
            }
            wing2 = wing2->next;
        }
        if (!common) vertex_list.push_back(wing1->vertex);
        wing1 = wing1->next;
    }
    return cnt;
}


/**
int  jbxl::CreateTriSolidFromSTL(BREP_SOLID* solid, STLData* stldata, int fno, bool check)

STLDataから BREP_SOLIDを生成する．BREP_SOLIDに使用された有効なファセットの数を返す．@n
STLData中の法線ベクトルは無視して再計算する．@n
カウンタ使用可能．

@param  solid   ソリッドデータへのポインタ
@param  stldata STLデータへのポインタ
@param  fno     Facet(STLデータ)の数
@param  check   データの不正検査を行うか？

@retval -1  ソリッドがNULL，stldataがNULL またはソリッドの Octreeが NULL
@retval -3  操作がキャンセルされた．
*/
DllExport int  jbxl::CreateTriSolidFromSTL(BREP_SOLID* solid, STLData* stldata, int fno, bool check)
{
    BREP_SHELL*   shell;
    BREP_FACET*   facet;
    BREP_CONTOUR* contour;
    CVCounter* counter = NULL;
    Vector<double> v[4];

    if (solid==NULL || stldata==NULL) return -1;
    if (solid->octree==NULL) return -1; 

    solid->contours.clear();

    // カウンタ
    int intvl = 1;
    if (solid->counter!=NULL) {
        counter = solid->counter->GetUsableCounter();
        if (counter!=NULL) {
            counter->SetMax(100);
            counter->ResetRate(80, 100);
            intvl = Max(1, fno/100);
        }
    }

    shell = new BREP_SHELL(solid);
    for (int i=0; i<fno; i++) {
        // 読み込んだ STLデータを作業用Vector変数に格納
        v[0].x = stldata[i].vect[3]; v[0].y = stldata[i].vect[4];  v[0].z = stldata[i].vect[5];
        v[1].x = stldata[i].vect[6]; v[1].y = stldata[i].vect[7];  v[1].z = stldata[i].vect[8]; 
        v[2].x = stldata[i].vect[9]; v[2].y = stldata[i].vect[10]; v[2].z = stldata[i].vect[11];
        // ここでは法線ベクトルは 再計算させる．

        facet = new BREP_FACET(shell); 
        contour = CreateContourByVector(facet, v, NULL, NULL, NULL, false);
        if (contour!=NULL) {
/*
            if (check) {    
                // 衝突判定
                BREP_CONTOUR* collision;
                facet->CloseData();
                if (!IsCollisionContours(solid, contour, &collision)) {
                    solid->contours.push_back(contour);             // IsCollisionContours()用
                }
                else {
                    collision->facet->deletable = true;             // 衝突相手も消す．
                    FastDeleteFacet(facet);
                }
            }
            else  solid->contours.push_back(contour);
*/
            solid->contours.push_back(contour);
        }
        else  {
            deleteNull(facet);          // 中身は空
        }

        if (counter!=NULL && i%intvl==0) {
            counter->StepIt();
            if (counter->isCanceled()) {
                deleteNull(facet);
                return -3;
            }
        }
    }
    if (counter!=NULL) counter->PutFill();

    // 
    fno = CloseTriSolid(solid, check, counter);

    return fno;
}


/**
DllExport int  jbxl::CreateTriSolidFromVector(BREP_SOLID* solid, int vno, Vector<double>* vect, Vector<double>* nrml, UVMap<double>* uvmp, Vertex_Weight* wght, bool dupli, bool check)

Vector<double>から BREP_SOLIDを生成する．BREP_SOLIDに使用された有効なファセットの数を返す．@n
カウンタ使用可能．

@param  solid ソリッドデータへのポインタ
@param  vno   頂点データの数
@param  vect  頂点の座標データへのポインタ．
@param  nrml  頂点の法線ベクトルデータへのポインタ．
@param  uvmp  頂点の曲面座標データへのポインタ．
@param  wgt   頂点の重みデータへのポインタ．
@param  dupli true: 頂点の重複登録を許可する．false: 重複登録を許可しない．
@param  check データの不正検査を行うか？

@retval -1  ソリッドがNULL，stldataがNULL またはソリッドの Octreeが NULL
@retval -3  操作がキャンセルされた．
*/
DllExport int  jbxl::CreateTriSolidFromVector(BREP_SOLID* solid, int vno, Vector<double>* vect, Vector<double>* nrml, UVMap<double>* uvmp, Vertex_Weight* wght, bool dupli, bool check)
{
    BREP_SHELL*   shell;
    BREP_FACET*   facet;
    BREP_CONTOUR* contour;
    CVCounter* counter = NULL;
    int fno = vno/3;

    if (solid==NULL || vect==NULL) return -1;
    if (solid->octree==NULL) return -1; 

    solid->contours.clear();

    // カウンタ
    int intvl = 1;
    if (solid->counter!=NULL) {
        counter = solid->counter->GetUsableCounter();
        if (counter!=NULL) {
            counter->SetMax(100);
            counter->ResetRate(80, 100);
            intvl = Max(1, fno/100);
        }
    }

    Vector<double>* normal = NULL;
    UVMap<double>*  uvmap  = NULL;
    Vertex_Weight*  weight = NULL;
    shell = new BREP_SHELL(solid);
    //
    for (int i=0; i<fno; i++) {
        facet = new BREP_FACET(shell); 
        if (nrml!=NULL) normal = nrml + i*3;
        if (uvmp!=NULL) uvmap  = uvmp + i*3;
        if (wght!=NULL) weight = wght + i*3;
        contour = CreateContourByVector(facet, vect+i*3, normal, uvmap, weight, dupli);

        if (contour!=NULL) {
/*
            if (check) {    
                // 衝突判定．
                BREP_CONTOUR* collision;
                facet->CloseData();
                if (!IsCollisionContours(solid, contour, &collision)) {
                    solid->contours.push_back(contour);             // IsCollisionContours()用
                }
                else {
                    collision->facet->deletable = true;             // 衝突相手も消す．
                    FastDeleteFacet(facet);
                }
            }
            else  solid->contours.push_back(contour);
*/
            solid->contours.push_back(contour);
        }
        else  {
            deleteNull(facet);          // 中身は空
        }

        if (counter!=NULL && i%intvl==0) {
            counter->StepIt();
            if (counter->isCanceled()) {
                deleteNull(facet);
                return -3;
            }
        }
    }
    if (counter!=NULL) counter->PutFill();

    //
    fno = CloseTriSolid(solid, check, counter);
    
    return fno;
}


/**
void  jbxl::AddVector2TriSolid(BREP_SOLID* solid, BREP_SHELL* shell, Vector<double>* vect, Vector<double>* nrml, UVMap<double>* uvmp, Vertex_Weight* wgt, bool dupli)

vect[3]を BREP_SOLIDに１個ずつシーケンシャルに追加する．
データの追加が終わったら，必ず CloseSolid() を呼ぶこと．@n

カウンタは無し．外部で利用すること．

@param  solid ソリッドデータへのポインタ
@param  shell シェルデータへのポインタ
@param  vect  3個の頂点データ vect[3] へのポインタ
@param  nrml  頂点の法線ベクトルデータへのポインタ．
@param  uvmp  頂点の曲面座標データへのポインタ．
@param  dupli true: 頂点の重複登録を許可する．false: 重複登録を許可しない．
*/
DllExport void  jbxl::AddVector2TriSolid(BREP_SOLID* solid, BREP_SHELL* shell, Vector<double>* vect, Vector<double>* nrml, UVMap<double>* uvmp, Vertex_Weight* wgt, bool dupli)
{
    BREP_FACET*   facet;
    BREP_CONTOUR* contour;

    if (solid==NULL || shell==NULL || vect==NULL) return;
    if (solid->octree==NULL) return; 

    facet = new BREP_FACET(shell); 
    contour = CreateContourByVector(facet, vect, nrml, uvmp, wgt, dupli);
    if (contour!=NULL) {
/*
        if (check) {    
            // 衝突判定
            BREP_CONTOUR* collision;
            facet->CloseData();
            if (!IsCollisionContours(solid, contour, &collision)) {
                solid->contours.push_back(contour);             // IsCollisionContours()用
            }
            else {
                collision->facet->deletable = true;             // 衝突相手も消す．
                FastDeleteFacet(facet);
            }
        }
        else  solid->contours.push_back(contour);
*/
        solid->contours.push_back(contour);
    }
    else {
        deleteNull(facet);
    }

    return;
}


/**
int  jbxl::CloseTriSolid(BREP_SOLID* solid, bool check, CVCounter* counter)

AddVector2TriSolid() の最後に呼んで，データをクローズする．
BREP_SOLIDに使用された有効なファセットの数を返す．

@param  solid    ソリッドデータへのポインタ
@param  check    データのエラーチェックを行うか? 行う場合，不正データは削除される．
@param  counter  仮想カウンタ．
@retval -1  ソリッドがNULL
*/
DllExport int  jbxl::CloseTriSolid(BREP_SOLID* solid, bool check, CVCounter* counter)
{
    if (solid==NULL) return -1;

    // deletableになっているContourがあるかもしれないので，CreateContoursList()で作り直し．     
    CreateContoursList(solid);
    CreateWingsList(solid);

    if (check) {
        // 多重Edgeの削除
        CreateSurplusContoursList(solid);   
        if (counter!=NULL) counter->MakeChildCounter(10);
        DeleteSurplusContours(solid);
        if (counter!=NULL) counter->DeleteChildCounter();
        // 直線に並んだ Edgeの削除
        CreateShortageWingsList(solid);
        if (counter!=NULL) counter->MakeChildCounter(10);
        DeleteStraightEdges(solid);         
        if (counter!=NULL) counter->DeleteChildCounter();
    }

    CreateSurplusContoursList(solid);   // 作り直し 
    CreateShortageWingsList(solid); 
    solid->vcount = 3;                  // for Triangle Facet
    solid->CloseData();                 // Close Octree
    //
    return  (int)solid->contours.size();
}


DllExport  bool  jbxl::IsConnectEdges(BREP_WING* wing1, BREP_WING* wing2)
{
    if (GetWingOtherSide(wing1)->vertex==wing2->vertex) return true;
    return false;
}
