
/**
@brief    BREP ライブリラリ
@file     Brep.cpp
@version  1.2.0

@sa "AN INTRODUCTION TO SOLID MODELING", M.MANTYLA, COMPUTER SCIENCE PRESS (1988), ISBN 0-88175-108-1
@sa BREPLIBRARY http://breplibrary.sourceforge.net/
*/


#include "Brep.h"


using namespace jbxl;


//////////////////////////////////////////////////////////////////////////////////
// BREP_SOLID 
//

/**
BREP_SOLID::BREP_SOLID()

空の Solidを作る
*/
BREP_SOLID::BREP_SOLID()
{
    freed    = false;
    
    facetno  = 0;
    vertexno = 0;
    vertexid = 0;
    vcount   = 0;       // 不定または未定
    //
    counter  = NULL;
    octree   = new OctreeNode(NULL, this);
    rbound.set(HUGE_VALF, -HUGE_VALF, HUGE_VALF, -HUGE_VALF, HUGE_VALF, -HUGE_VALF);
}



/**
BREP_SOLID::~BREP_SOLID()

ディストラクタ
*/
BREP_SOLID::~BREP_SOLID()
{
    if (!freed) FreeData();
}



/**
void BREP_SOLID::FreeData()

Solid が持つ Shell以下のオブジェクトとVertex の Octreeを全て削除
*/
void BREP_SOLID::FreeData()
{
    if (freed) return;

    BREP_SHELL* pbsh;
    BREP_SHELL_LIST::iterator ishell = shells.begin();

    CVCounter* cnt = NULL;
    if (counter!=NULL) {
        cnt = counter->GetUsableCounter();
        if (cnt!=NULL) cnt->SetMax(100);
    }

    int shn = (int)shells.size();
    while (ishell!=shells.end()) {
        if (cnt!=NULL) cnt->MakeChildCounter(100/shn);
        pbsh = *ishell;
        ishell = shells.erase(ishell);  // Shellの数は多くないはずなので，counter処理のために   
        delete pbsh;                    // eraseの空処理を Shellに行わせる
        if (cnt!=NULL) cnt->DeleteChildCounter();
    }

    contours.clear();
    wings.clear();
    shortage_wings.clear();
    surplus_contours.clear();

    delete octree;
    //
    freed = true;
}



/**
void  BREP_SOLID::CloseData()
*/
void  BREP_SOLID::CloseData()
{
    BREP_SHELL_LIST::iterator ishell;
    for (ishell=shells.begin(); ishell!=shells.end(); ishell++) (*ishell)->CloseData();
    for (ishell=shells.begin(); ishell!=shells.end(); ishell++) rbound.fusion((*ishell)->rbound);

    octree->ComputeVerticesNormal();
}



/** 
void  BREP_SOLID::ConnectShell(BREP_SHELL* shell)

Solidに指定したShellを結合する．
*/
void  BREP_SOLID::ConnectShell(BREP_SHELL* shell)
{
    if (shell!=NULL) {
        shell->solid = this;
        shells.push_back(shell);
    }
}



/**
void  BREP_SOLID::DisconnectShell(BREP_SHELL* shell)

Solidから指定したShellを削除する．
*/
void  BREP_SOLID::DisconnectShell(BREP_SHELL* shell)
{
    if (shell!=NULL) {
        BREP_SHELL_LIST::iterator ishell;
        ishell = std::find(shells.begin(), shells.end(), shell);
        if (ishell!=shells.end()) shells.erase(ishell);
        shell->solid = NULL;
    }
}




//////////////////////////////////////////////////////////////////////////////////
// BREP_SHELL 
//

BREP_SHELL::BREP_SHELL(BREP_SOLID* pr_solid)
{
    solid = pr_solid;
    rbound.set(HUGE_VALF, -HUGE_VALF, HUGE_VALF, -HUGE_VALF, HUGE_VALF, -HUGE_VALF);

    if (solid!=NULL) solid->ConnectShell(this);
}



BREP_SHELL::~BREP_SHELL()
{
    CVCounter* counter = NULL;
    if (solid!=NULL) {
        if (solid->counter!=NULL) {
            counter = solid->counter->GetUsableCounter();
            if (counter!=NULL) counter->SetMax(100);
        }
        solid->DisconnectShell(this);
    }

    // Facets の破棄
    BREP_FACET*  pbfc;
    BREP_FACET_LIST::iterator ifacet = facets.begin();

    int cnt = 0;
    int itv = Max(1, (int)facets.size()/100);
    while (ifacet!=facets.end()) {
        pbfc = *ifacet;
        ifacet = facets.erase(ifacet);
        pbfc->shell = NULL;
        delete(pbfc);
        cnt++;
        
        // カウンタ処理
        if (counter!=NULL && cnt%itv==0) counter->StepIt(); 
    }
}



void  BREP_SHELL::CloseData()
{
    BREP_FACET_LIST::iterator ifacet;
    for (ifacet=facets.begin(); ifacet!=facets.end(); ifacet++) (*ifacet)->CloseData();
    for (ifacet=facets.begin(); ifacet!=facets.end(); ifacet++) rbound.fusion((*ifacet)->rbound);
}



//
void BREP_SHELL::ConnectFacet(BREP_FACET* facet)
{
    if (facet!=NULL) {
        facet->shell = this;
        facets.push_back(facet);
    }
}



//
void BREP_SHELL::DisconnectFacet(BREP_FACET* facet)
{
    if (facet!=NULL) {
        BREP_FACET_LIST::iterator ifacet;
        ifacet = std::find(facets.begin(), facets.end(), facet);
        if (ifacet!=facets.end()) facets.erase(ifacet);

        facet->shell = NULL;
    }
}




//////////////////////////////////////////////////////////////////////////////////
// BREP_FACET 
//

BREP_FACET::BREP_FACET(BREP_SHELL* pr_shell)
{
    shell = pr_shell;
    if (shell!=NULL) shell->ConnectFacet(this);
    
    deletable = false; 
    notdelete = false;
    tolerance = Facet_Tolerance;
    
    rbound.set(HUGE_VALF, -HUGE_VALF, HUGE_VALF, -HUGE_VALF, HUGE_VALF, -HUGE_VALF);
}



BREP_FACET::~BREP_FACET()
{
    if (shell!=NULL) shell->DisconnectFacet(this);

    BREP_CONTOUR* bpcn;
    BREP_CONTOUR_LIST::iterator icon = outer_contours.begin();
    while (icon!=outer_contours.end()) {
        bpcn = *icon;
        icon = outer_contours.erase(icon);
        bpcn->facet = NULL;
        delete (bpcn);
    }
}



void  BREP_FACET::CloseData()
{
    BREP_CONTOUR_LIST::iterator icon;
    for (icon=outer_contours.begin(); icon!=outer_contours.end(); icon++) (*icon)->CloseData();

    ComputePlaneEquation();
}



//
void BREP_FACET::ConnectContour(BREP_CONTOUR* contour)
{
    if (contour!=NULL) {
        contour->facet = this;
        outer_contours.push_back(contour);
    }
}



void BREP_FACET::DisconnectContour(BREP_CONTOUR* contour)
{
    if (contour!=NULL) {
        BREP_CONTOUR_LIST::iterator icon;
        icon = std::find(outer_contours.begin(), outer_contours.end(), contour);
        if (icon!=outer_contours.end()) outer_contours.erase(icon);

        contour->facet = NULL;
    }
}



/**
void  BREP_FACET::ComputePlaneEquation()

@note
平面の方程式　N・P＋d = 0   
(N:法線ベクトル，P:平面の点の位置ベクトル，d:定数，原点から平面への距離x-1）
*/
void  BREP_FACET::ComputePlaneEquation()
{
    normal = outer_contours.front()->normal;

    double emax =  0.0;
    double dmin =  HUGE_VALF;
    double dmax = -HUGE_VALF;

    BREP_CONTOUR_LIST::iterator icon;
    for (icon=outer_contours.begin(); icon!=outer_contours.end(); icon++) {
        (*icon)->CloseFacet(this, emax, dmin, dmax);
    }

    d = (dmin + dmax)/2.0;
    tolerance = Max((dmax - dmin)/2.0, Facet_Tolerance);
    tolerance = Max(emax, tolerance);
    
    double extent = tolerance + emax;
    rbound.xmin -= extent;
    rbound.ymin -= extent;
    rbound.zmin -= extent;
    rbound.xmax += extent;
    rbound.ymax += extent;
    rbound.zmax += extent;
}




//////////////////////////////////////////////////////////////////////////////////
// BREP_CONTOUR

BREP_CONTOUR::BREP_CONTOUR(BREP_FACET* pr_facet)
{
    facet = pr_facet;
    if (facet!=NULL) facet->ConnectContour(this);
    
    wing      = NULL;
    dup_edge  = 0;
    collision = false;
    hasCollisionVector = false;

    rbound.set(HUGE_VALF, -HUGE_VALF, HUGE_VALF, -HUGE_VALF, HUGE_VALF, -HUGE_VALF);
}



//
BREP_CONTOUR::~BREP_CONTOUR()
{
    if (facet!=NULL) facet->DisconnectContour(this);
    DestroyWings();
}



void  BREP_CONTOUR::CloseData()
{
    ComputeNormal();
    if (!hasCollisionVector) ComputeDirectRS();
}



/** 
void BREP_CONTOUR::ConnectWing(BREP_WING* new_wing)

新しい Wingを Contourのリストに追加する．
*/
void BREP_CONTOUR::ConnectWing(BREP_WING* new_wing)
{
    if (new_wing==NULL) return;

    new_wing->contour = this;

    if (wing==NULL) {   // 最初の Wing
        new_wing->next = new_wing->prev = new_wing;
        wing = new_wing;
    }
    else {              // ２番目以降の Wing
        new_wing->next = wing;
        new_wing->prev = wing->prev;
        new_wing->prev->next = new_wing->next->prev = new_wing;
    }
}



//
void  BREP_CONTOUR::DisconnectWing(BREP_WING* dis_wing)
{
    if (wing==dis_wing) {
        if (dis_wing->next==dis_wing) {
            wing = NULL;
        }
        else {
            wing = dis_wing->next;
        }
    }

    if (dis_wing->next) dis_wing->next->prev = dis_wing->prev;
    if (dis_wing->prev) dis_wing->prev->next = dis_wing->next;

    dis_wing->contour = NULL;
    dis_wing->next = dis_wing->prev = NULL;
}



/**
BREP_WING*  BREP_CONTOUR::CreateWing(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)

vertex1をスタートVertexとした，Contour,Vertexに関連付けられらたWingを作る（必要なら新しいEdgeも）．@n
作成時，指定するVertexは順序付けられていなければエラーとなる．
*/
BREP_WING*  BREP_CONTOUR::CreateWing(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)
{
    // 新しく作成するWingのスタートVertexは，一つ前のWingのエンドVertexと同一でなければならない．
    // つまり Wingは順序よく作成しなければならない．
    if (wing!=NULL && GetWingOtherSide(wing->prev)->vertex!=vertex1) {
        DEBUG_MODE PRINT_MESG("CreateWing: Irregular order of Wings!!\n");
        return NULL;
    }

    // vertex1をスタートVertexとしたWingを作る．すでに空のWingがある場合にはそれを返す．
    // vertex1==vertex2 の場合は NULLを返す．
    BREP_WING* wing = CreateWingWithoutContour(vertex1, vertex2);
    if (wing==NULL) return NULL;

    // WingをContourに登録
    ConnectWing(wing); 
    if (GetWingOtherSide(wing)->contour!=NULL) {    // 対応するEdgeは２つの参照されたWingを持つ完全なEdge
        wing->edge->complete = true;
    }

    // WingをスタートVertexのリングに登録
    (wing->vertex)->wing_list.push_back(wing); 

    return wing;
}



/**
void BREP_CONTOUR::DestroyWings()

Contourに関連付けられた Wingをすべて破棄する．
*/
void BREP_CONTOUR::DestroyWings()
{
    BREP_WING* first = wing;
    if (first==NULL) return;

    BREP_WING* prev;
    for (BREP_WING* swing=first->prev; swing!=first; swing=prev) {
        prev = swing->prev;
        DestroyWing(swing);
    }
    DestroyWing(first);
}



//
void  BREP_CONTOUR::CloseFacet(BREP_FACET* facet, double& emax, double& dmin, double& dmax)
{
    if (wing==NULL) return;

    BREP_WING* next = wing;
    do {
        BREP_WING* swing = next;
        //
        double d = -(facet->normal * swing->vertex->point);
        if (d < dmin) dmin = d;
        if (d > dmax) dmax = d;
        if (swing->vertex->tolerance>emax) emax = swing->vertex->tolerance;
        facet->rbound.fusion(swing->vertex->point);
        //
        next = swing->next;
    } while (next!=wing);
}



/**
void  BREP_CONTOUR::ComputeNormal()

Newells method により面の法線ベクトルを計算する．
*/
void  BREP_CONTOUR::ComputeNormal()
{
    normal = Vector<double>(0.0, 0.0, 0.0);

    Vector<double> next = wing->vertex->point;
    for (BREP_WING* swing=wing; swing; swing=(swing->next==wing ? NULL:swing->next)) {
        Vector<double> cur = next;
        next = swing->next->vertex->point;
        normal.x += (cur.y - next.y) * (cur.z + next.z);
        normal.y += (cur.z - next.z) * (cur.x + next.x);
        normal.z += (cur.x - next.x) * (cur.y + next.y);
    }
    normal.normalize();
}



/**
void  BREP_CONTOUR::ComputeDirectRS()

衝突判定用ベクトルの計算
*/
void  BREP_CONTOUR::ComputeDirectRS()
{
    TVector<double> point = Vertex2TVector(wing->next->vertex);

    directR  = point - Vertex2TVector(wing->vertex);
    directS  = Vertex2TVector(wing->next->next->vertex) - point;
    directRS = directR^directS;
    directR.norm();
    directS.norm();
    directRS.norm();

    hasCollisionVector = true;
}



BREP_CONTOUR* CreateContour(BREP_FACET* facet)
{
    BREP_CONTOUR* contour = new BREP_CONTOUR(facet);
    return contour;
}




//////////////////////////////////////////////////////////////////////////////////
// BREP_WING
// 

BREP_WING::BREP_WING(BREP_VERTEX* vx)
{
    vertex  = vx;
    prev    = NULL;
    next    = NULL;
    edge    = NULL;
    contour = NULL;
}




//////////////////////////////////////////////////////////////////////////////////
// BREP_EDGE
//

BREP_EDGE::BREP_EDGE(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)
{
    wing1 = new BREP_WING(vertex1);
    wing2 = new BREP_WING(vertex2);
    wing1->edge = wing2->edge = this;

    center    = (vertex1->point + vertex2->point)/2.;
    edge_list = NULL;
    complete  = false;
    tolerance = Edge_Tolerance;
}



BREP_EDGE::~BREP_EDGE()
{
    // BREP_EDGE: この Edge はまだ Contourで使われている!!
    if (wing1->contour || wing2->contour) {
        DEBUG_MODE PRINT_MESG("~BREP_EDGE: This Edge is still used!!\n");
    }

    delete wing1;
    delete wing2;

    if (edge_list!=NULL) {
        BREP_EDGE_LIST::iterator iedge = std::find(edge_list->begin(), edge_list->end(), this);
        if (iedge!=edge_list->end()) {
            edge_list->erase(iedge);
        }
        if (edge_list->size()==1) {
            (*(edge_list->begin()))->edge_list = NULL;
            delete(edge_list);
        }
    }
}



void  BREP_EDGE::CloseData()
{
    // M.Segal, SIGGRAPH '90 p105
    double e = (wing1->vertex)->tolerance;
    e = Max(e, (wing2->vertex)->tolerance);
    tolerance = Max(e, tolerance);
}




//////////////////////////////////////////////////////////////////////////////////
// BREP_VERTEX
//

BREP_VERTEX::BREP_VERTEX()
{
    // Vector類は宣言された時に，既に初期化されている．

    index           = -1;
    calc_normal     = true;
    tolerance       = Abs_Vertex_Tolerance;

    forbidden_list  = NULL;
    distance2       = HUGE_VALF;
}



BREP_VERTEX::~BREP_VERTEX()
{
    // Wingリストがまだ空でない!!
    if (!wing_list.empty()) {
        DEBUG_MODE PRINT_MESG("~BREP_VERTEX:  List of Wing is not empty!!\n");
    }

    if (forbidden_list!=NULL) {
        forbidden_list->clear();
        delete(forbidden_list);
    }
}



//
void BREP_VERTEX::DisconnectWing(BREP_WING* wing)
{
    BREP_WING_LIST::iterator iwing;
    //
    iwing = std::find(wing_list.begin(), wing_list.end(), wing);
    if (iwing==wing_list.end()) {
        // この Wingは Vertexと接続していません!!
        DEBUG_MODE PRINT_MESG("DisconnectWing: This Wing does not connect with Vertex!!\n");
        return;
    }

    wing_list.erase(iwing);
}



/**
void  BREP_VERTEX::ComputeNormal()

Vertexの法線ベクトルを計算する．@n
Vertexの周りの Contourの法線ベクトルの加算平均を計算する
*/
void  BREP_VERTEX::ComputeNormal()
{
    normal = Vector<double>(0.0, 0.0, 0.0);

    BREP_WING_LIST::iterator iwing;
    for (iwing=wing_list.begin(); iwing!=wing_list.end(); iwing++) {
        BREP_WING* wing = *iwing;
        normal = normal + (wing->contour->facet)->normal;
    }
    normal.normalize();
}



void  BREP_VERTEX::ComputeTolerance()
{
    double max = Xabs(point.x);
    max = Max(max, Xabs(point.y));
    max = Max(max, Xabs(point.z));

    tolerance = Vertex_Tolerance * max;                  // 相対
    tolerance = Max(tolerance, Abs_Vertex_Tolerance);    // 絶対
}



void  BREP_VERTEX::CloseData() 
{
    ComputeTolerance();
}




//////////////////////////////////////////////////////////////////////////////////
// OctreeNode
//

OctreeNode::OctreeNode(BREP_VERTEX* new_vertex, BREP_SOLID* sld) 
{
    solid  = sld;
    vertex = new_vertex;

    if (solid!=NULL && vertex!=NULL) {
        vertex->index = solid->vertexid;
        solid->vertexid++;
    }

    for (int i=0; i<8; i++) child[i] = NULL;
}



OctreeNode::~OctreeNode() 
{
    for (int i=0; i<8; i++) {
        if (child[i]!=NULL) delete child[i];
    }

    if (vertex!=NULL) {
        delete vertex;
        if (solid!=NULL) solid->vertexno--;
    }
}



/** 
OctreeNode* OctreeNode::AddWithUnique(BREP_VERTEX* new_vertex) 

Octreeに Vertex new_vertexそのものを登録する．登録した OctreeNodeを返す．
既に同じ位置のVertexが登録済みの場合は，その OctreeNodeを返す．@n
*/
OctreeNode* OctreeNode::AddWithUnique(BREP_VERTEX* new_vertex) 
{
    OctreeNode* o = NULL;
    OctreeNode* p = this;
    int cmp = -1;

    if (new_vertex==NULL) return NULL;

    if (p->vertex==NULL) {      // 一番最初の Vertex
        p->vertex = new_vertex;
        if (solid!=NULL) {
            p->vertex->index = solid->vertexid;
            solid->vertexid++;
        }
        return p;
    }

    while (p!=NULL) {
        cmp = CompareVertex(p->vertex, new_vertex);
        if (cmp==8) return p;   // 既に同じ位置のVertexが登録済み
        o = p;
        p = p->child[cmp];
    }

    if (cmp>=0 && o!=NULL) {
        p = new OctreeNode(new_vertex, solid);
        o->child[cmp] = p;
    }
    return p;
}



/**
OctreeNode* OctreeNode::AddWithDuplicates(BREP_VERTEX* new_vertex) 

Octreeに Vertex new_vertex そのものを登録する．重複登録を許す．@n
登録した OctreeNodeを返す．
*/
OctreeNode* OctreeNode::AddWithDuplicates(BREP_VERTEX* new_vertex) 
{
    OctreeNode* o = NULL;
    OctreeNode* p = this;
    int cmp = -1;

    if (new_vertex==NULL) return this;

    if (p->vertex==NULL) {  // 一番最初の Vertex
        p->vertex = new_vertex;
        if (solid!=NULL) {
            p->vertex->index = solid->vertexid;
            solid->vertexid++;
        }
        return p;
    }

    while (p!=NULL) {
        cmp = CompareVertex(p->vertex, new_vertex);
        o = p;
        p = p->child[cmp%8];
    }

    if (cmp>=0 && o!=NULL) {
        p = new OctreeNode(new_vertex, solid);
        o->child[cmp%8] = p;
    }
    return p;
}



/**
OctreeNode* OctreeNode::FindSubtree(BREP_VERTEX* element)

Vertex element と同じ位置にある Vertexを含むノードを返す．
*/
OctreeNode* OctreeNode::FindSubtree(BREP_VERTEX* element)
{
    OctreeNode* p = this;
    while(p!=NULL){
        int cmp = CompareVertex(p->vertex, element);
        if (cmp==8) return p;
        if (cmp<0)  return NULL; 
        p = p->child[cmp];
    }
    return NULL;
}



//
void OctreeNode::ComputeVerticesNormal(void)
{
    for (int i=0; i<8; i++) {
        if (child[i]!=NULL) {
            child[i]->ComputeVerticesNormal();
        }
    }
    if (vertex!=NULL && vertex->calc_normal) vertex->ComputeNormal();
}




//////////////////////////////////////////////////////////////////////////////////
// Other Functions
//

/**
void jbxl::ConnectWingToVertex(BREP_WING* wing)

WingのスタートVertexのリングに，この Wingを登録する．@n
Wingが実在する（使用されている）ためには，このリングに登録されてることと，Contourから（へ）
リンクされることである．
*/
DllExport  void jbxl::ConnectWingToVertex(BREP_WING* wing)
{
    if (wing!=NULL) {
        (wing->vertex)->wing_list.push_back(wing);
    }
}



/**
BREP_WING* jbxl::CreateWingWithoutContour(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)

vertex1をスタートVertexとしたWingを（必要ならEdgeも）作成して返す．

Wingが作成された時点では，WingはContourには関連付けられていない． 
*/
DllExport BREP_WING* jbxl::CreateWingWithoutContour(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)
{
    BREP_WING* wing = NULL;

    // vertex1 - vertex2 間にエッジが既に登録されているかどうかチェックする．
    BREP_EDGE* edge = FindEdge(vertex1, vertex2);
    if (edge==NULL) { // Edgeは存在しない．
        edge = CreateEdge(vertex1, vertex2);
        if (edge==NULL) return NULL;    // vertex1==vertex2 なら NULL
        wing = edge->wing1;
    }
    else{ // Edgeは既に存在する．
        if      (edge->wing1->vertex==vertex1) wing = edge->wing1;
        else if (edge->wing2->vertex==vertex1) wing = edge->wing2;
        
        // Wingが既に Contourで使われている場合，他の Wingを探す．
        if (edge->edge_list!=NULL && wing!=NULL && wing->contour!=NULL) {
            BREP_EDGE_LIST::iterator iedge=edge->edge_list->begin();
            while (iedge!=edge->edge_list->end() && wing->contour!=NULL) {
                if      ((*iedge)->wing1->vertex==vertex1) wing = (*iedge)->wing1;
                else if ((*iedge)->wing2->vertex==vertex1) wing = (*iedge)->wing2;
                iedge++;
            }
        }

        // このWingは既に Contourで使われているか？
        if (wing!=NULL && wing->contour!=NULL){
            // 新しくEdgeを作り直す
            BREP_EDGE* new_edge = CreateEdge(vertex1, vertex2);
            if (new_edge==NULL) return NULL;

            if (edge->edge_list==NULL) {
                edge->edge_list = new BREP_EDGE_LIST();
                edge->edge_list->push_back(edge);
            }
            edge->edge_list->push_back(new_edge);
            new_edge->edge_list = edge->edge_list;
            wing = new_edge->wing1;
        }
    }

    return wing;
}



/**
void jbxl::DestroyWing(BREP_WING* wing)
*/
DllExport void jbxl::DestroyWing(BREP_WING* wing)
{
    wing->vertex->DisconnectWing(wing);
    wing->contour->DisconnectWing(wing);

    if (!wing->edge->wing1->contour && !wing->edge->wing2->contour) {
        delete wing->edge;
    }
    else {
        wing->edge->complete = false;
    }
}



/**
BREP_EDGE* jbxl::FindEdge(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)

vertex1と vertex2が既にエッジで結ばれているかどうかをチェックする．

もし結ばれていれば，最初に見つけた Edgeを返す．
@code
    vertex1--Wing1--Edge--Wing2--vertex2  
@endcode
*/
DllExport BREP_EDGE* jbxl::FindEdge(BREP_VERTEX* vertex1, BREP_VERTEX* vertex2)
{
    BREP_WING_LIST list;
    BREP_WING_LIST::iterator iwing;

    list = vertex2->wing_list;
    for (iwing=list.begin(); iwing!=list.end(); iwing++){
        BREP_WING* wing = *iwing;
        if (GetWingOtherSide(wing)->vertex==vertex1) return wing->edge;
    }

    list = vertex1->wing_list;
    for (iwing=list.begin(); iwing!=list.end(); iwing++) {
        BREP_WING* wing = *iwing;
        if (GetWingOtherSide(wing)->vertex==vertex2) return wing->edge;
    }

    return NULL;
}



/**
要free
*/
DllExport  BREP_VERTEX** jbxl::GetOctreeVertices(OctreeNode* octree, long int* vertexno)
{
    BREP_VERTEX** vindex = NULL;

    long int num = OctreeGetter(octree, NULL, 0);

    if (num>0) {
        vindex = (BREP_VERTEX**)malloc(num*sizeof(BREP_VERTEX*));
    }
    if (vindex!=NULL) {
        OctreeGetter(octree, vindex, 0);
    }
    if (vertexno!=NULL) *vertexno = num;

    return vindex;
}



long int jbxl::OctreeGetter(OctreeNode* p, BREP_VERTEX** vtx, long int counter)
{
    if (p->vertex!=NULL) {
        if (vtx!=NULL && p->vertex->index>=0) vtx[p->vertex->index] = p->vertex;
        counter++;
    }

    for (int i=0; i<8; i++) {
        if (p->child[i]!=NULL) {
            counter = OctreeGetter(p->child[i], vtx, counter);
        }
    }

    return counter;
}



/**
BREP_VERTEX*  jbxl::AddVertex2Octree(BREP_VERTEX* vert, OctreeNode* octree, bool dupli)

Vertex vert を octreeに登録する．

dupli がfalseの場合:@n
既に同じ位置に Vertexが登録されている場合（UVマップも比べる）は，vertは消去されて，既に登録されている Vertexが返される．@n
この関数を使用後は vertを使用してはいけない（deleteされている可能性があるので）@n

dupli が trueの場合:@n
既に同じ位置に Vertexが登録されていても，重複して登録する．@n

@param vert 登録する頂点データ
@param octree 頂点データを格納するオクトツリー
@param dupli true: 頂点の重複登録を許可する．false: 重複登録を許可しない．
@return オクトツリー中の頂点データ

@par 使用例
@code
    vert = AddVertex2Octree(vert, octree, false);
@endcode
*/
DllExport BREP_VERTEX*  jbxl::AddVertex2Octree(BREP_VERTEX* vert, OctreeNode* octree, bool dupli)
{
    OctreeNode* node;

    if (dupli) node = octree->AddWithDuplicates(vert); 
    else       node = octree->AddWithUnique(vert);
    if (node==NULL) return NULL;

    if (node->vertex==vert) {
        octree->solid->vertexno++;      // Vertexを新規登録したのでカウントする．
    }
    return node->vertex;
}



/**
*/
DllExport BREP_EDGE* jbxl::CreateEdge(BREP_VERTEX* v1, BREP_VERTEX* v2)
{
    if (v1==v2) return NULL;

    BREP_EDGE* edge = new BREP_EDGE(v1, v2);
    edge->CloseData();

    return edge;
}



/**
int jbxl::CompareVertex(BREP_VERTEX* v1, BREP_VERTEX* v2)

v2 に対する v1 の位置を検査する．

@retval 0〜7 Vertex の位置
@retval 8    同じVertex
*/
DllExport int jbxl::CompareVertex(BREP_VERTEX* v1, BREP_VERTEX* v2)
{
    double  tolerance = v1->tolerance + v2->tolerance;
    double  dist2 = (v1->point.x - v2->point.x)*(v1->point.x - v2->point.x) +
                    (v1->point.y - v2->point.y)*(v1->point.y - v2->point.y) +
                    (v1->point.z - v2->point.z)*(v1->point.z - v2->point.z);
    if (dist2<=tolerance*tolerance && v1->uvmap==v2->uvmap) return 8;   // 同じ位置．同じテクスチャマップ．

    int code = 0;
    if (v1->point.x > v2->point.x) code += 1;
    if (v1->point.y > v2->point.y) code += 2;
    if (v1->point.z > v2->point.z) code += 4;
    return code;   // x1 > x2 || y1 > y2 || z1 > z2
}



/**
BREP_WING* jbxl::GetWingOtherSide(BREP_WING* wing)

Edgeに関連付けられたもう一方の Wing を返す．
*/
DllExport BREP_WING* jbxl::GetWingOtherSide(BREP_WING* wing)
{ 
    if (wing==wing->edge->wing1) return wing->edge->wing2;
    //
    return wing->edge->wing1;
}



DllExport TVector<double>  jbxl::Vertex2TVector(BREP_VERTEX* v)
{
    TVector<double>  tv;

    tv.x   = v->point.x;
    tv.y   = v->point.y;
    tv.z   = v->point.z;
    tv.n   = sqrt(tv.x*tv.x + tv.y*tv.y + tv.z*tv.z);
    tv.t   = v->tolerance;
    
    return  tv;
}

