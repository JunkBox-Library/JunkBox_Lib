
/** 
@brief   Tiny Tree Graph 構造ライブラリ
@file    ttree.c
@version 
@author  Fumi.Iseki (C)
@date    2008 2/1
*/


#include  "ttree.h"
#include  "jbxl_state.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////
// ノード操作
//

/**
tTree*  new_tTree_node(void)

ツリー用の空ノードを動的に生成．
@return 生成されたノードへのポインタ．
*/
tTree*  new_tTree_node(void)
{
    tTree* pp;

    pp = (tList*)malloc(sizeof(tTree));
    if (pp==NULL) return NULL;
    memset(pp, 0, sizeof(tTree));
    pp->ldat  = init_tList_data();
    pp->state = JBXL_NORMAL;

    return pp;
}



tTree*  new_tTree_anchor_node(void)
{
    tTree* pp;

    pp = (tList*)malloc(sizeof(tTree));
    if (pp==NULL) return NULL;
    memset(pp, 0, sizeof(tTree));
    pp->ldat    = init_tList_data();
    pp->ldat.id = TREE_ANCHOR_NODE;
    pp->depth   = -1;
    pp->state   = JBXL_STATE_ANCHOR;    // TREE_ANCHOR_NODE と同じ

    return pp;
}



tTree*  del_tTree_anchor_node(tTree* node)
{
    tTree* pp = node;

    if (node!=NULL && node->ldat.id==TREE_ANCHOR_NODE) {
        pp = node->next;
        free_tTree_node(node);
        free(node);
    }

    return pp;    
}



/**
tTree  make_tTree_node(tList_data data)

ツリー用ノードを静的に作成．
@param  data ノードデータ
@return 作られたノード．
*/
tTree  make_tTree_node(tList_data data)
{
    tTree pp;

    memset(&pp, 0, sizeof(tTree));
    pp.ldat  = data;
    pp.state = JBXL_NORMAL;

    return pp;
}



/**
tTree*  add_tTree_node(tTree* pp, tTree* node) 

ツリー ppへノード nodeを追加．ポインタ ppが指すノードの子（末っ子）ノードとして node（そのもの）を追加する．@n
node が子ノードを持つ場合は，それも追加される．@n
node が姉妹ノードを持っていてもそれらは無視する（処理しない）．@n

@param  pp    追加するノードの親となるノードへのポインタ．
@param  node  追加するノードへのポインタ．node->next 以下がツリーでも良い．
@return 追加したノードへのポインタ．失敗した場合は NULL
*/
tTree*  add_tTree_node(tTree* pp, tTree* node) 
{
    if (node==NULL) return NULL;
    if (pp==NULL) return node;

    node->prev = pp;
    node->ysis = NULL;
    node->esis = pp->yngr;

    if (pp->yngr!=NULL) pp->yngr->ysis = node;
    if (pp->next==NULL) pp->next = node;
    pp->yngr = node;

    node->depth = pp->depth + 1;
    pp->num++;
    
    if (node->next!=NULL) {
        node->next->depth = node->depth + 1;
        adjust_tTree_depth(node->next);
    }

    return node;
}



/**
tTree*  add_tTree_node_bydata(tTree* pp, tList_data ldat) 

データから Treeノードをつくり出し,それを ppの子ノード（末っ子）として追加．@n
ldat は指定されたものがそのまま使用される．

@param  pp    追加するノードの親となるノードへのポインタ．
@param  ldat  追加するノードデータ．このデータがそのまま使用される．
@return 追加したノードへのポインタ．
*/
tTree*  add_tTree_node_bydata(tTree* pp, tList_data ldat) 
{
    tTree* pt;

    pt = new_tTree_node();
    pt->ldat  = ldat;
    pt->depth = 0;

    if (pp==NULL) return pt;

    pt->prev = pp;
    pt->ysis = NULL;
    pt->esis = pp->yngr;

    if (pp->yngr!=NULL) pp->yngr->ysis = pt;
    if (pp->next==NULL) pp->next = pt;
    pp->yngr = pt;

    pt->depth = pp->depth + 1;
    pp->num++;
    
    return pt;
}



/**
tTree*  add_tTree_node_byBuffer(tTree* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz) 

データからノードをつくり出し,それを末っ子としてリストに追加．@n
リストポインタ ppが指すノードの後につくり出した ノードを挿入する．

@param  pp   追加する場所の手前のノードへのポインタ．
@param  id   追加するデータ．
@param  lv   追加するデータ．
@param  key  追加するデータ．        複製
@param  val  追加するデータ．        複製
@param  ptr  汎用データへのポインタ  複製
@param  sz   *ptr のサイズ
@return 追加したノードへのポインタ．
*/
tTree*  add_tTree_node_byBuffer(tTree* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz) 
{
    tTree* pt;
    tList_data ldat;

    ldat = make_tList_data(id, lv, key, val, ptr, sz);
    pt   = add_tTree_node_bydata(pp, ldat);

    return pt;
}



/**
tTree*  add_tTree_node_bystr(tTree* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz) 

データからノードをつくり出し,それを末っ子としてリストに追加．@n
リストポインタ ppが指すノードの後につくり出した ノードを挿入する．

@param  pp   追加する場所の手前のノードへのポインタ．
@param  id   追加するデータ．
@param  lv   追加するデータ．
@param  key  追加するデータ．        複製
@param  val  追加するデータ．        複製
@param  ptr  汎用データへのポインタ  複製
@param  sz   *ptr のサイズ
@return 追加したノードへのポインタ．
*/
tTree*  add_tTree_node_bystr(tTree* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz) 
{
    tTree* pt;
    tList_data ldat;

    ldat = make_tList_data_bystr(id, lv, key, val, ptr, sz);
    pt   = add_tTree_node_bydata(pp, ldat);

    return pt;
}



/**
tTree*  insert_tTree_node(tTree* pp, tTree* node) 

ツリー ppへノード nodeを追加．ポインタ ppが指すノードの子（長子）ノードとして node（そのもの）を追加する．@n
node が子ノードを持つ場合は，それも追加される．@n
node が姉妹ノードを持っていてもそれらは無視する（処理しない）．@n

@param  pp    追加するノードの親となるノードへのポインタ．
@param  node  追加するノードへのポインタ．node->next 以下がツリーでも良い．
@return 追加したノードへのポインタ．失敗した場合は NULL
*/
tTree*  insert_tTree_node(tTree* pp, tTree* node) 
{
    if (node==NULL) return NULL;
    if (pp==NULL) return node;

    if (pp->next!=NULL) pp->next->esis = node;
    node->ysis = pp->next;
    node->esis = NULL;

    pp->next = node;
    node->prev = pp;

    node->depth = pp->depth + 1;
    pp->num++;
    
    if (node->next!=NULL) {
        node->next->depth = node->depth + 1;
        adjust_tTree_depth(node->next);
    }

    return node;
}



/**
tTree*  insert_tTree_node_bydata(tTree* pp, tList_data ldat) 

データから tTreeノードをつくり出し,それを ppの子ノード（長子）として追加．@n
ldat は指定されたものがそのまま使用される．

@param  pp    追加するノードの親となるノードへのポインタ．
@param  ldat  追加するノードデータ．このデータがそのまま使用される．
@return 追加したノードへのポインタ．
*/
tTree*  insert_tTree_node_bydata(tTree* pp, tList_data ldat) 
{
    tTree* pt;

    pt = new_tTree_node();
    pt->ldat  = ldat;
    pt->depth = 0;

    if (pp==NULL) return pt;
    //
    if (pp->next!=NULL) pp->next->esis = pt;
    pt->ysis = pp->next;
    pt->esis = NULL;

    pp->next = pt;
    pt->prev = pp;

    pt->depth = pp->depth + 1;
    pp->num++;
    
    return pt;
}



/**
tTree*  insert_tTree_node_byBuffer(tTree* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz) 

データからノードをつくり出し,それを長子としてリストに追加．@n
リストポインタ ppが指すノードの後につくり出した ノードを挿入する．

@param  pp   追加する場所の手前のノードへのポインタ．
@param  id   追加するデータ．
@param  lv   追加するデータ．
@param  key  追加するデータ．        複製
@param  val  追加するデータ．        複製
@param  ptr  汎用データへのポインタ  複製
@param  sz   *ptr のサイズ
@return 追加したノードへのポインタ．
*/
tTree*  insert_tTree_node_byBuffer(tTree* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz) 
{
    tTree* pt;
    tList_data ldat;

    ldat = make_tList_data(id, lv, key, val, ptr, sz);
    pt   = insert_tTree_node_bydata(pp, ldat);

    return pt;
}


/**
tTree*  insert_tTree_node_bystr(tTree* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz) 

データからノードをつくり出し,それを長子としてリストに追加．@n
リストポインタ ppが指すノードの後につくり出した ノードを挿入する．

@param  pp   追加する場所の手前のノードへのポインタ．
@param  id   追加するデータ．
@param  lv   追加するデータ．
@param  key  追加するデータ．        複製
@param  val  追加するデータ．        複製
@param  ptr  汎用データへのポインタ  複製
@param  sz   *ptr のサイズ
@return 追加したノードへのポインタ．
*/
tTree*  insert_tTree_node_bystr(tTree* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz) 
{
    tTree* pt;
    tList_data ldat;

    ldat = make_tList_data_bystr(id, lv, key, val, ptr, sz);
    pt   = insert_tTree_node_bydata(pp, ldat);

    return pt;
}



/**    
tTree*  free_tTree_node(tTree* node) 

ツリーノードの削除．削除されたノードが子ノードを持つ場合は，その子ノードを格上げする（木構造を詰める）@n

@return 削除したノードの親ノードへのポインタ．
*/
tTree*  free_tTree_node(tTree* node) 
{
    if (node==NULL) return NULL;

    if (node->next!=NULL) {    // 子ノードを持つ場合
        tTree* ss;
        
        if (node->ldat.id!=TREE_ANCHOR_NODE) {
            node->next->depth--;
            adjust_tTree_depth(node->next);
        }

        ss = node->next;
        ss->prev = node->prev;
        while (ss->ysis!=NULL) {
            ss = ss->ysis;
            ss->prev = node->prev;
        } 

        ss->ysis = node->ysis;
        node->next->esis = node->esis;
        if (node->ysis!=NULL) node->ysis->esis = ss;
        if (node->esis!=NULL) node->esis->ysis = node->next;

        if (node->prev!=NULL) {
            if (node->prev->next==node) node->prev->next = node->next;
            if (node->prev->yngr==node) node->prev->yngr = ss;
        }
    }

    else {                    // 子ノードを持たない場合
        if (node->prev!=NULL) {
            if (node->prev->next==node) node->prev->next = node->ysis;
            if (node->prev->yngr==node) node->prev->yngr = node->esis;
        }
        if (node->ysis!=NULL) node->ysis->esis = node->esis;
        if (node->esis!=NULL) node->esis->ysis = node->ysis;
    }

    tTree* pp = node->prev;
    free_tList_data(&(node->ldat));
    if (node->prev!=NULL) node->prev->num += node->num - 1;

    return pp;
}



/**    
tTree*  del_tTree_node(tTree** node) 

ツリーノードの削除．削除されたノードが子ノードを持つ場合は，その子ノードを格上げする（木構造を詰める）@n
node は動的に確保された変数でなければならない．

@param  *node  削除するノードへのポインタ．
@return 削除したノードの親ノードへのポインタ．
*/
tTree*  del_tTree_node(tTree** node) 
{
    if (node==NULL || *node==NULL) return NULL;

    tTree* pp = free_tTree_node(*node);
    free(*node);
    *node = NULL;

    return pp;
}



/**    
tTree*  move_tTree_node(tTree* pp, tTree* node) 

nodeを現在のツリーから切り離し，ppへ移動する．

元のツリーに於いて，nodeが子ノードを持つ場合は，その子ノードを格上げする（木構造を詰める）@n
移動に於いては，node が姉妹ノードを持っていてもそれらは無視する．@n
nodeを削除しないで del_tTree_node(), add_tTree_node() を実行するようなもの．

@param  pp    移動先で親となるノードへのポインタ．
@param  node  移動するノードへのポインタ．node->next 以下がツリーでも良い．
@return 移動したノードノードへのポインタ．
*/
tTree*  move_tTree_node(tTree* pp, tTree* node) 
{
    if (node==NULL || pp==NULL) return NULL;

    // ノードの切り離し
    if (node->next!=NULL) {    //     子ノードを持つ場合
        tTree* ss;
        node->next->depth--;
        adjust_tTree_depth(node->next);

        ss = node->next;
        ss->prev = node->prev;
        while (ss->ysis!=NULL) {
            ss = ss->ysis;
            ss->prev = node->prev;
        } 

        ss->ysis = node->ysis;
        node->next->esis = node->esis;
        if (node->ysis!=NULL) node->ysis->esis = ss;
        if (node->esis!=NULL) node->esis->ysis = node->next;

        if (node->prev!=NULL) {
            if (node->prev->next==node) node->prev->next = node->next;
            if (node->prev->yngr==node) node->prev->yngr = ss;
        }
    }
    else {                    // 子ノードを持たない場合
        if (node->prev!=NULL) {
            if (node->prev->next==node) node->prev->next = node->ysis;
            if (node->prev->yngr==node) node->prev->yngr = node->esis;
        }
        if (node->ysis!=NULL) node->ysis->esis = node->esis;
        if (node->esis!=NULL) node->esis->ysis = node->ysis;
    }
    if (node->prev!=NULL) node->prev->num += node->num - 1;

    // ノードの再結合（移動）
    node->prev = pp;
    node->ysis = NULL;
    node->esis = pp->yngr;

    if (pp->yngr!=NULL) pp->yngr->ysis = node;
    if (pp->next==NULL) pp->next = node;
    pp->yngr = node;

    node->depth = pp->depth + 1;
    pp->num++;
    
    if (node->next!=NULL) {
        node->next->depth = node->depth + 1;
        adjust_tTree_depth(node->next);
    }

    return node;
}



/**
void  replace_all_tTree_node(tTree* pp, char* key, char* src, char* dst, int len)

ツリー pp内で keyをキー，srcをノード値として持つ全てのノードのノード値を dst に書き換える．

@param  tp   置換対象のツリー
@param  key  サーチキー
@param  src  置換対象のノード値
@param  dst  置換後のノード値
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．

@return 置き換えたノードの数
*/
int  replace_all_tTree_node(tTree* tp, char* key, char* src, char* dst, int len)
{
    int nn = 0;

    do { 
        if (ex_strncmp((char*)(tp->ldat).key.buf, (char*)key, len)) {
            if (ex_strncmp((char*)(tp->ldat.val.buf), (char*)src, len)) {
                free_Buffer(&(tp->ldat.val));
                tp->ldat.val = make_Buffer_bystr(dst);
                nn++;
            }
        }

        if (tp->next!=NULL) nn += replace_all_tTree_node(tp->next, key, src, dst, len);

        tp = tp->ysis;
    } while(tp!=NULL);

    return nn;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ツリー操作
//

/**
tTree*  del_tTree(tTree** pp) 

指定したノード以下のツリーを削除する．

@param[in]  *pp  削除するツリーの先頭ノード
@param[out] *pp  NULL
@return 削除したツリーの親ノードへのポインタ．
*/
tTree*  del_tTree(tTree** pp) 
{
    tTree* pt;

    if (pp==NULL || *pp==NULL) return NULL;

    // 子ノードの削除
    if ((*pp)->next!=NULL) del_sisters_children_tTree(&((*pp)->next));

    // 自分自身の削除
    pt = (*pp)->prev;
    if (pt!=NULL) {
        if (pt->next==*pp) pt->next = (*pp)->ysis;
        if (pt->yngr==*pp) pt->yngr = (*pp)->esis;
        pt->num--;
    }
    if ((*pp)->ysis!=NULL) (*pp)->ysis->esis = (*pp)->esis;
    if ((*pp)->esis!=NULL) (*pp)->esis->ysis = (*pp)->ysis;

    free_tList_data(&((*pp)->ldat));
    free(*pp);
    *pp = NULL;

    return pt;
}



/**
tTree*  del_children_tTree(tTree** pp) 

指定したノードの子ツリーを削除する．指定したノードは削除しない．

@param  *pp  削除する子ツリーの親ノード
@return 削除したツリーの親ノードへのポインタ．*pp がそのまま返る．
*/
tTree*  del_children_tTree(tTree** pp) 
{    
    if (pp==NULL || *pp==NULL) return NULL;

    if ((*pp)->next!=NULL) del_sisters_children_tTree(&((*pp)->next));

    (*pp)->num  = 0;
    (*pp)->next = NULL;
    (*pp)->yngr = NULL;

    return *pp;
}


/**
tTree*  del_sisters_tTree(tTree** pp)

指定したノード以下のXMLツリー（ppの姉妹を含む）を削除する．

@param[in,out]  *pp  削除するツリーの先頭ノード．削除後は NULLになる．
@return 削除したXMLツリーの親ノードへのポインタ．
*/
tTree*  del_sisters_tTree(tTree** pp)
{
    tTree* pt;

    if (pp==NULL || *pp==NULL) return NULL;

    pt = (*pp)->prev;
    if (pt!=NULL) {
        pt->next = NULL;
        pt->yngr = NULL;
        pt->num  = 0;
    }

    del_sisters_children_tTree(pp);

    return pt;
}



/**
tTree*  del_sisters_children_tTree(tTree** pp) 

指定したノードの姉妹ツリー，子ツリーを削除する．@n
指定したノードも削除する．

@param  *pp  削除するツリーの起点ノード
@return 削除したツリー郡の親ノードへのポインタ．

@attention 再帰処理用．親ノードに対する処理は行わないので，別途呼び出し側で行うこと．
*/
tTree*  del_sisters_children_tTree(tTree** pp) 
{
    tTree* pm;
    tTree* pt;

    if (pp==NULL || *pp==NULL) return NULL;
    pt = (*pp)->prev;

    pm = *pp;
    while (pm->esis!=NULL) pm = pm->esis;
    while (pm!=NULL) {
        tTree* pw = pm;
        if (pm->next!=NULL) del_sisters_children_tTree(&(pm->next)); 
        pm = pm->ysis;
        free_tList_data(&(pw->ldat));
        free(pw);
    }
    *pp = NULL;

    pt->next = NULL;
    pt->yngr = NULL;

    return pt;
}



/**
void  del_all_tTree(tTree** pp)

ツリーの全ノードの削除．ポインタ ppのノードを含むツリー全体を削除する．@n
*pp はツリー中であれば，どこを指していても良い．

@param  *pp  削除を開始するノードへのポインタ．ツリー中であれば，どこを指していても良い．
*/
void  del_all_tTree(tTree** pp)
{
    tTree* pm;

    if (pp==NULL || *pp==NULL) return;

    pm = *pp;
    while (pm->prev!=NULL) pm = pm->prev;
    del_tTree(&pm);

    *pp = NULL;
    return;
}



/**
tTree*  add_tTree(tTree* tp, tTree* tt)

ツリー tpへ ツリー ttを追加．@n
add_tTree_node() との相違は，add_tTree()が先頭(tt)の姉妹ノードもツリー tpに追加する点にある．

@param  tp  追加するツリーの親となるノードへのポインタ．
@param  tt  追加するツリーへのポインタ．
           
@return 追加したツリーのノードへのポインタ．失敗した場合は NULL
*/
tTree*  add_tTree(tTree* tp, tTree* tt)
{
    int    nn;
    tTree* tm;
    tTree* tw;

    if (tt==NULL) return NULL;
    if (tp==NULL) return tt;

    while(tt->esis!=NULL) tt = tt->esis;
    tt->esis = tp->yngr;
    if (tp->yngr!=NULL) tp->yngr->ysis = tt;
    if (tp->next==NULL) tp->next = tt;

    nn = 0;
    tm = tw = tt;
    while (tm!=NULL) {
        nn++;
        tm->prev  = tp;
        tm->depth = tp->depth + 1;

        if (tm->next!=NULL) {
            tm->next->depth = tm->depth + 1;
            adjust_tTree_depth(tm->next);
        }
        tw = tm;
        tm = tm->ysis;
    }

    tp->yngr = tw;
    tp->num += nn;

    return tt;
}



/**
tTree*  div_tTree(tTree* tp, tTree* tt)

ツリー tp から ツリー ttを分離する．

@param  tt  ツリーへの分離ポイント．
@return 分離したツリーの元親ノードへのポインタ．失敗した場合は NULL
*/
tTree*  div_tTree(tTree* tt)
{
    if (tt==NULL) return NULL;
    if (tt->prev==NULL) return tt;

    if (tt->prev->next==tt) tt->prev->next = tt->ysis;
    if (tt->prev->yngr==tt) tt->prev->yngr = tt->esis;

    if (tt->ysis!=NULL) tt->ysis->esis = tt->esis;
    if (tt->esis!=NULL) tt->esis->ysis = tt->ysis;

    tt->depth = 0;
    if (tt->next!=NULL) {
        tt->next->depth = 1;
        adjust_tTree_depth(tt->next);
    }

    tt->prev->num--;
    tt->prev = NULL;

    return tt;
}



/**
tTree*  dup_merge_tTree(tTree* pp, tTree* tp)

ツリー ppの直下に（Yunger Sisterとして）ツリー tpを複製する．@n
pp がNULLの場合は，ツリーの深さは調整されない

@param  pp  複製されたツリーのトップとなるノード
@param  tp  複製するツリー

@return 複製されたツリーへのポインタ．pp がNULLでない場合は pp 
@return pp がNULLの場合は，新しく複製したツリーのトップ．
*/             
tTree*  dup_merge_tTree(tTree* pp, tTree* tp)
{
    if (tp==NULL) return pp;

    if (pp!=NULL) {
        if (tp->ctrl!=TREE_NOSIS_NODE) while(tp->esis!=NULL) tp = tp->esis;
        while(tp!=NULL) {
            tTree* pt = dup_tList_node(tp);
            pt->next = pt->prev = pt->yngr = pt->ysis = pt->esis = NULL;
            add_tTree(pp, pt);
            if (tp->next!=NULL) dup_merge_tTree(pt, tp->next);
            if (tp->ctrl!=TREE_NOSIS_NODE) tp = tp->ysis;
            else                           tp = NULL;
        }        
    }
    else {
        pp = new_tTree_anchor_node();
        dup_merge_tTree(pp, tp);
        pp = del_tTree_anchor_node(pp); 
    }

    return pp;
}



/**
void  merge_tTree(tTree* tp, tTree* tt)

ツリー tp にツリー tt を結合する．結合後，tt の内容は壊れる(tpとノードを交換した形になる)．

tt が tpの一部と同じ構造(キー値)を持つ場合，末端ノードは ttのノードで置き換える．tp に存在しない枝は追加される．@n
ツリーの深さは tpを深さを元に再計算される．
          
@param[in,out] tp  in: tt の結合ポイント．out: 結合後のツリー．
@param[in,out] tt  in: 結合するツリー．out: 内容は破壊される．要注意

@par 例
@code
tp                           tr
A --> B --> E                A --> B --> X
  --> C --> M --> X            --> C --> M
              --> Y            --> D
        --> N
@endcode
以上の場合，merge_tTree(tp, tr) を実行すると以下のようになる．
@code
tp
A --> B --> E 
        --> X (tr)
  --> C --> M (tr)
        --> N 
  --> D (tr)

tt
A --> B 
  --> C --> M --> X (tp)
              --> Y (tp)
@endcode
*/
void  merge_tTree(tTree* tp, tTree* tt)
{
    tTree* tl;
    tTree* nt;
    tTree* nl;
    int    depth;

    if (tp==NULL || tt==NULL) return;

    depth = tp->depth;
    tl = tp;
    while (tt!=NULL) {
        if ((tt->ldat).key.buf==NULL) return;
        if (tl!=NULL && (tl->ldat).key.buf==NULL) return;
        while (tl!=NULL && strcmp((char*)((tl->ldat).key.buf), (char*)((tt->ldat).key.buf))) tl = tl->ysis;

        nt = tt;
        nl = tl;
        tt = tt->ysis;

        if (tl==NULL) {    
            div_tTree(nt);
            add_tTree(tp->prev, nt);
            tl = nt;
            return;
        }
        else if (nl->next!=NULL && nt->next!=NULL) {
            merge_tTree(nl->next, nt->next);
            tl = tl->ysis;
        }
        else {
            tl = tl->ysis;
            exchange_tTree(nl, nt);
        }
    }

    tp->depth = depth;
    adjust_tTree_depth(tp);

    return;
}



/**
void  exchange_tTree(tTree* tl, tTree* tt)

ツリー tlと ツリー ttを交換する．

@param  tl  交換対象のツリー
@param  tt  交換対象のツリー
*/
void  exchange_tTree(tTree* tl, tTree* tt)
{
    int    dt = tt->depth;
    tTree* pt = tt->prev;
    tTree* yt = tt->ysis;
    tTree* et = tt->esis;

    if (tl->esis!=NULL) tl->esis->ysis = tt;
    if (tl->ysis!=NULL) tl->ysis->esis = tt;
    if (tl->prev!=NULL) {
        if (tl->prev->next==tl) tl->prev->next = tt;
        if (tl->prev->yngr==tl) tl->prev->yngr = tt;
    }

    tt->ysis  = tl->ysis;
    tt->esis  = tl->esis;
    tt->prev  = tl->prev;
    tt->depth = tl->depth;
    if (tt->next!=NULL) {
        tt->next->depth = tt->depth + 1;
        adjust_tTree_depth(tt->next);
    }

    if (et!=NULL) et->ysis = tl;
    if (yt!=NULL) yt->esis = tl;
    if (pt!=NULL) {
        if (pt->next==tt) pt->next = tl;
        if (pt->yngr==tt) pt->yngr = tl;
    }

    tl->ysis  = yt;
    tl->esis  = et;
    tl->prev  = pt;
    tl->depth = dt;
    if (tl->next!=NULL) {
        tl->next->depth = dt + 1;
        adjust_tTree_depth(tl->next);
    }
}



/**
void  adjust_tTree_depth(tTree* pp)

指定したノード ppを基準にして，木の深さを測り直す

@param  pp  基準となるノードへのポインタ
*/
void  adjust_tTree_depth(tTree* pp)
{
    int depth;
    tTree* pt;
    
    if (pp==NULL) return;

    depth = pp->depth;
    pt = pp;
    while (pt->ysis!=NULL) {
        pt = pt->ysis;
        pt->depth = depth;
        if (pt->next!=NULL) {
            pt->next->depth = depth + 1;
            adjust_tTree_depth(pt->next);
        }
    }

    pt = pp;
    while (pt->esis!=NULL) {
        pt = pt->esis;
        pt->depth = depth;
        if (pt->next!=NULL) {
            pt->next->depth = depth + 1;
            adjust_tTree_depth(pt->next);
        }
    }

    if (pp->next!=NULL) {
        pp->next->depth = depth + 1;
        adjust_tTree_depth(pp->next);
    }

    return;
}



/**
void  print_tTree_tree(FILE* fp, tTree* pp, const char* space)

ツリーの表示．ポインタ pp以降の全てのノードのキー部のバッファを fpに表示する．
pp->ctrl が TREE_NOSIS_NODE の場合は，ppの姉妹ノードは出力しない．

@param  fp     出力するファイルへのポインタ．NULLの場合は stderr
@param  pp     表示を開始するノードへのポインタ．
@param  space  出力の書式を揃えるための空白．
*/
void  print_tTree_tree(FILE* fp, tTree* pp, const char* space)
{
    if (fp==NULL) fp = stderr;

    if (pp!=NULL) {
        if (pp->ctrl!=TREE_NOSIS_NODE) while(pp->esis!=NULL) pp = pp->esis;
        do { 
            int i;
            tList_data ld = pp->ldat;

            if (pp->depth>=1) {
                for(i=1; i<pp->depth; i++) fprintf(fp, "%s", space);
                fprintf(fp, " -> ");
            }
            fprintf(fp, "%d: %d %d %s %s\n", pp->depth, ld.id, ld.lv, ld.key.buf, ld.val.buf);
            if (pp->next!=NULL) print_tTree_tree(fp, pp->next, space);

            if (pp->ctrl!=TREE_NOSIS_NODE) pp = pp->ysis;
            else                           pp = NULL;
        } while(pp!=NULL);
    }
    else {
        fprintf(fp, "(Tree is NULL)\n");
    }
    fflush(fp);

    return;
}



/**
void  print_tTree(FILE* fp, tTree* pp)

ツリーの表示．ポインタ pp以降の全てのノードのキー部のバッファを fpに表示する．
pp->ctrl が TREE_NOSIS_NODE の場合は，ppの姉妹ノードは出力しない．

@param  fp     出力するファイルへのポインタ．NULLの場合は stderr
@param  pp     表示を開始するノードへのポインタ．
*/
void  print_tTree(FILE* fp, tTree* pp)
{
    if (fp==NULL) fp = stderr;

    if (pp!=NULL) {
        if (pp->ctrl!=TREE_NOSIS_NODE) while(pp->esis!=NULL) pp = pp->esis;
        do { 
            tList_data ld = pp->ldat;
            //
            fprintf(fp, "[%d]: [%d] [%d] [%s] [%s]\n", pp->depth, ld.id, ld.lv, ld.key.buf, ld.val.buf);
            if (pp->next!=NULL) print_tTree(fp, pp->next);

            if (pp->ctrl!=TREE_NOSIS_NODE) pp = pp->ysis;
            else                           pp = NULL;
        } while(pp!=NULL);
    }
    else {
        fprintf(fp, "(Tree is NULL)\n");
    }
    fflush(fp);

    return;
}



/**
tTree*  find_tTree_end(tTree* pp)

ツリーの最終ノードを見つける．
*/
tTree*  find_tTree_end(tTree* pp)
{
    if (pp==NULL) return NULL;

    while(pp->prev!=NULL) pp = pp->prev;    // Top を探す
    while(pp->yngr!=NULL) pp = pp->yngr;

    return pp;
}



/**
int  count_tTree(tTree* pp)

ツリーの ppノード以降のノードの数を数える．

@param  pp  数え始めるノードへのポインタ．姉妹ノードも数える．
@return ノードの数．
*/
int  count_tTree(tTree* pp)
{
    int cnt = 0;

    if (pp==NULL) return 0;
    while(pp->esis!=NULL) pp = pp->esis;

    do { 
        cnt++;
        if (pp->next!=NULL) cnt += count_tTree(pp->next);
        pp = pp->ysis;
    } while(pp!=NULL);

    return cnt;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pattern Matching
//

/**
tTree*  strncmp_tTree(tTree* pp, const char* key, int len, int no)

ツリーノードのキー値のサーチ．@n
ポインタ pp以降のノードで,キー部の文字列が keyと前方一致（部分的も可）するノードの内 no番目にあるのを捜し出す．

@param  pp   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）．
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tTree*  strncmp_tTree(tTree* pp, const char* key, int len, int no)
{
    tTree* pt = NULL;
    int nn = 0;

    if (pp==NULL) return NULL;
    if (len<=-3)  return NULL;
    if (no<=0) no = 1;

    if (ex_strncmp((char*)(pp->ldat).key.buf, key, len)) {
        nn++;
        if (no==nn) return pp;
    }
    if (pp->next!=NULL) pt = next_strncmp_vertical_tTree(pp->next, key, len, no, &nn);

    return pt;
}



/**
tTree*  strncasecmp_tTree(tTree* pp, const char* key, int len, int no)

ツリーノードのキー値のサーチ．大文字小文字を無視する．@n
ポインタ pp以降のノードで,キー部の文字列が keyと前方一致（部分的も可）するノードの内 no番目にあるのを捜し出す．

@param  pp   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）．大文字，小文字を区別しない．
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tTree*  strncasecmp_tTree(tTree* pp, const char* key, int len, int no)
{
    tTree* pt = NULL;
    int nn = 0;

    if (pp==NULL) return NULL;
    if (len<=-3)  return NULL;
    if (no<=0) no = 1;

    if (ex_strncasecmp((char*)(pp->ldat).key.buf, key, len)) {
        nn++;
        if (no==nn) return pp;
    }
    if (pp->next!=NULL) pt = next_strncasecmp_vertical_tTree(pp->next, key, len, no, &nn);

    return pt;
}



/**
tTree*  cmp_sisters_tTree(tTree* tp, tTree* tr)

ノードtpの姉妹ノードが ノードtrの姉妹ノードと同じパターン（キー値を比較）を持っているかどうかを検査する．

ただし，tr->ctrl が TREE_NOCMP_NODE または TREE_NOCMP_COPY_NODE のノードは比べない(常に一致とする)．@n
また tp->ctrl が TREE_ALREADY_FOUND_NODE の場合は，常に一致しない．@n
もし同じノードパターンがある場合，trの各ノードの altpには，対応する tpの各ノードへのポインタが格納される．

@param  tp   比べる姉妹ノードの長女ノード
@param  tr   探す姉妹ノードパターンの長女ノード

@return tp中で trと同じパターンが始まるノードへのポインタ．trの各ノードの altpには，対応する tpの各ノードへのポインタが格納される．
@retval NULL tpに同じ姉妹パターンは無い．この場合，trのaltpの値は不定となる．

@par 例
以下の場合，cmp_sisters_tTree(tp, tr) は (3)へのポインタを返す．また trの Aノード
の altp には (3) へのポインタが，trの Xノードのaltpには(4)へのポインタが格納される．
最初に見つかったパターンのみ評価される．
@code
tp                tr
--> A (1)         --> A         A, B, X は キー値(ldat.key.buf)
--> B (2)         --> X
--> A (3)
--> X (4)
--> A (5)
--> X (6)
@endcode
*/
tTree*  cmp_sisters_tTree(tTree* tp, tTree* tr)
{
    tTree* ta;
    tTree* tb = NULL;
    tTree* ts;

    ts = tp;
    while (ts!=NULL){
        ta = ts;
        tb = tr;
        while (ta!=NULL && tb!=NULL) {
            if (ta->ctrl==TREE_ALREADY_FOUND_NODE) break;
            if (tb->ctrl!=TREE_NOCMP_NODE && tb->ctrl!=TREE_NOCMP_COPY_NODE) {
                if ((ta->ldat).key.buf!=NULL && (tb->ldat).key.buf!=NULL) {
                    if (strcmp((char*)((ta->ldat).key.buf), (char*)((tb->ldat).key.buf))) break;
                }
                else break;
            }
            tb->altp = ta;
            ta = ta->ysis;
            tb = tb->ysis;
        }

        if (tb==NULL) return ts;

        ts = ts->ysis;
    }
    if (tb!=NULL) return NULL;

    return ts;
}



/**
int  check_match_tTree(tTree* tp, tTree* tr)

ツリー tpが ツリー trと同じパターン(キー値)を持っているかどうかを検査する．@n
tp のトップと tr のトップはキー値が一致している必要がある．一致していなければ，同じパターンは無しとする．@n
ただし，tr->ctrl が @b TREE_NOCMP_NODE または @b TREE_NOCMP_COPY_NODE のノードは比べない(常に一致とする)．

一度見つけた tpの枝の最後のノードに対しては ctrlを @b TREE_ALREADY_FOUND_NODE を設定するので，続けてチェックする
場合などは ctrl をクリアする必要がある．

もし同じツリーパターンがある場合，trの各ノードの altpには，一番最初に見つかった対応する tpの各ノードへのポインタが格納される．

@param  tp    検索対象のツリー
@param  tr    検索パターンのツリー

@retval TRUE  tp中に trと同じいツリーパターンが存在する．trの各ノードの altpには，一番最初に見つかった対応する tpの各ノードへのポインタが格納される．
@retval FALSE tpに同じツリーパターンは無い．この場合，trのaltpの値は不定となる．
*/
int  check_match_tTree(tTree* tp, tTree* tr)
{
    int  ret;
    tTree* te;
    tTree* ts;

    tTree* tt;
    tTree* ta;
    tTree* tb;

    if (tp==NULL || tr==NULL) return FALSE;

    te = find_tList_end(tr);

    ts = tp;
    while (ts!=NULL) {
        tt = cmp_sisters_tTree(ts, tr);      // その階層でキー値が全て一致しているか確認
        if (tt==NULL) return FALSE;          // 一致していなければ，FALSE
        
        ta  = tt;                            // 比べられるツリー
        tb  = tr;                            // 比べるパターン
        ret = TRUE;
        while (tb!=NULL && ret) {
            if (tb->next==NULL) ret = TRUE;
            // ->ta, ->tb->tx: FALSE
            else if (tb->next!=NULL && ta->next==NULL) ret = FALSE;
            // ->ta->xa, ->tb->xb: xaとxbをチェック
            else ret = check_match_tTree(ta->next, tb->next);

            ta = ta->ysis;
            tb = tb->ysis;
        }

        if (ret) {
            if (tr==te) tt->ctrl = TREE_ALREADY_FOUND_NODE;
            return TRUE;
        }

        ts = tt->ysis;
    }

    return FALSE;
}



/**
int find_match_tTree(tTree* pp, tTree* pt)

ツリー pp内で ツリー ptと同じパターンの枝を探す．@n 
同じパターンの探索では キー値のみを比較し，ノード値は比較しない．@n
ただし，pt->ctrl が @b TREE_NOCMP_NODE または @b TREE_NOCMP_COPY_NODE のノードは比べない(常に一致とする)．
    
もし同じツリーパターンがある場合，trの各ノードの altpには，一番最初に見つかった対応する ppの各ノードへ
のポインタが格納される．

check_match_tTree() との違い．
- check_match_tTree() では比べる枝の開始ノードはppに固定される．
- find_match_tTree() は pp内を移動しながら検索できる．

@param  pp    検索対象のツリー
@param  pt    検索パターンのツリー

@retval TRUE  pp中に pt同じいツリーパターンが存在する．ptの各ノードの altpには，一番最初に見つかった対応する ppの各ノードへのポインタが格納される．
@retval FALSE ppに同じツリーパターンは無い．この場合，ptのaltpの値は不定となる．
*/
int  find_match_tTree(tTree* pp, tTree* pt)
{
    int   ret;
    tTree* pm;

    pm = pp;
    while(pp!=NULL) {
        ret = check_match_tTree(pp, pt);
        if (ret) return TRUE;

        if (pp->next!=NULL) {
            ret = find_match_tTree(pp->next, pt);
            if (ret) {
                clear_tTree_ctrl(pm);
                return TRUE;
            }
        }
        pp = pp->ysis;
    }

    return FALSE;
}



/**
void  clear_tTree_ctrl(tTree* pp)

ppツリーの ctrlをクリアする．
*/
void  clear_tTree_ctrl(tTree* pp)
{
    while (pp->esis!=NULL) pp = pp->esis;

    while (pp!=NULL) {
        pp->ctrl = TREE_NOCTRL_NODE;
        if (pp->next!=NULL) clear_tTree_ctrl(pp->next);
        pp = pp->ysis;
    }
}



/**
tList*  find_match_tTree_endlist(tTree* pp, tTree* pt)

ツリー pp内で ツリー ptと同じパターンの枝を全て探して，その枝の最後のノードへの情報をリストにして返す．@n
該当ノードへのポインタは 返された各リストのaltp が保持している． 

比較では キー値のみを比較し，ノード値は比較しない．
また，pt->ctrl が @b TREE_NOCMP_NODE または @b TREE_NOCMP_COPY_NODE のノードは比べない(常に一致とする)．
    
@param  pp  検索対象のツリー
@param  pt  検索パターンのツリー

@return 該当ノードへのポインタを保持するリスト．
*/
tList*  find_match_tTree_endlist(tTree* pp, tTree* pt)
{
    tTree* te;
    tList* lp;

    te = find_tTree_end(pt);
    while(pp->esis!=NULL) pp = pp->esis;

    lp = find_match_tTree_endlist_rcsv(pp, pt, te);
    if (lp!=NULL) clear_tTree_ctrl(pp);
    
    return lp;
}



/**
tList*  find_match_tTree_endlist_rcsv(tTree* pp, tTree* pt, tTree* te)

find_match_tTree_endlist() の補助関数
*/
tList*  find_match_tTree_endlist_rcsv(tTree* pp, tTree* pt, tTree* te)
{
    tList* lt = NULL;
    tList* lp = NULL;

    while(pp!=NULL) {
        int ret = check_match_tTree(pp, pt);
        if (ret && te->altp!=NULL) {
            tList* lm = new_tList_node();
            lm->altp = te->altp;
            lt = insert_tList(lt, lm);
            if (lp==NULL) lp = lt;
            te->altp = NULL;
        }
            
        if (pp->next!=NULL) {
            tList* lm = find_match_tTree_endlist_rcsv(pp->next, pt, te);
            if (lm!=NULL) {
                lt = insert_tList(lt, lm);
                if (lp==NULL) lp = lt;
                clear_tTree_ctrl(pp->next);
            }
        }
    
        if (!ret) pp = pp->ysis;    // 見つかった場合はもう一度．見つからなかった場合へ次へ．
    }
    
    return lp;
}



/**
void  replace_tTree_node(tTree* pp, tTree* pt)

ツリー pp内で ツリー ptと同じパターン（キー値を比較）の枝を検索し，ppに一致したパターンの枝があれば，@n
その枝の各ノードに対して，対応するそれぞれの（pt->ctrl が TREE_COPY_NODE または TREE_NOCMP_COPY_NODE である）@n
ptのノードの属性で置き換える．@n
パターンの一致（検索）では ldat.key（キー値）が比較される．

置き換える属性は ldat.id, ldat.lv, ldat.sz, ldat.key, ldat.val, ldat.ptr, ldat.lst @n
置き換えを行うのは pt->ctrl が @b TREE_COPY_NODE または @b TREE_NOCMP_COPY_NODE の場合のみである．(重要) @n
ldat.val, ldat.ptr, ldat.lst については，ptで値が設定されていなければ，置き換えを行わない．
    
@param  pp    置き換え対象のツリー
@param  pt    置き換えるツリー

@retval TRUE  置換する枝を見つけた．正常に置換されたかどうかは不明．
@retval FALSE 置換する枝を見つけられなかった．
*/
int replace_tTree_node(tTree* pp, tTree* pt)
{
    int ret;

    if (pp==NULL || pt==NULL) return FALSE;
    while(pp->esis!=NULL) pp = pp->esis;
    
    ret = find_match_tTree(pp, pt);
    if (ret) {
        copy_tTree_byctrl(pt);
        adjust_tTree_depth(pp);
    }

    return ret;
}



/**
void  copy_tTree_byctrl(tTree* pt)

replace_tTree_node()の補助関数．

ツリー ptにおいて，pt->ctrl が TREE_COPY_NODE または TREE_NOCMP_COPY_NODE の場合，
pt->altp のノードへ ptの属性をコピーする．@n
pt->ldat.sz には正確に pt->ldat.ptrのサイズが設定されている必要がある．
*/
void  copy_tTree_byctrl(tTree* pt)
{
    while(pt!=NULL) {
        if (pt->altp!=NULL) {
            if (pt->ctrl==TREE_COPY_NODE || pt->ctrl==TREE_NOCMP_COPY_NODE) {
                pt->altp->ldat.id = pt->ldat.id;        
                pt->altp->ldat.lv = pt->ldat.lv;
                pt->altp->ldat.sz = pt->ldat.sz;

                if (pt->ldat.key.buf!=NULL) {
                    free_Buffer(&(pt->altp->ldat.key));
                    pt->altp->ldat.key = dup_Buffer(pt->ldat.key);
                }
                if (pt->ldat.val.buf!=NULL) {
                    free_Buffer(&(pt->altp->ldat.val));
                    pt->altp->ldat.val = dup_Buffer(pt->ldat.val);
                }

                if (pt->ldat.ptr!=NULL && pt->ldat.sz>0) {
                    if (pt->altp->ldat.ptr!=NULL) free(pt->altp->ldat.ptr);
                    pt->altp->ldat.ptr = (void*)malloc(pt->ldat.sz);
                    if (pt->altp->ldat.ptr!=NULL) memcpy(pt->altp->ldat.ptr, pt->ldat.ptr, pt->ldat.sz);
                }

                if (pt->ldat.lst!=NULL) {
                    del_all_tList(&(pt->altp->ldat.lst));
                    pt->altp->ldat.lst = dup_tList(pt->ldat.lst);
                }
            }
        }

        if (pt->next!=NULL) copy_tTree_byctrl(pt->next);
        pt = pt->ysis;
    }

    return;
}



/**
Buffer  get_value_tTree(tTree* pp, tTree* pt)

ツリー pp内で ツリー ptと同じパターンの枝を検索し，ppに一致したパターンの
枝があれば，その枝の最後のノードの値を返す．

@param  pp  検索対象のツリー
@param  pt  検索するパターン
@return ptの最後のノードに対応する ppのノードのノード値

@par 例
以下の場合，Y(*) のノード値が返る．
@code
tp                           tr
A --> B --> E                C --> M --> Y
  --> C --> M --> X               
              --> Y(*)         
        --> N
@endcode
*/
Buffer  get_value_tTree(tTree* pp, tTree* pt)
{
    int fnd;
    Buffer val;

    val = init_Buffer();
    if (pp==NULL || pt==NULL) return val;

    while(pp->esis!=NULL) pp = pp->esis;
    
    fnd = find_match_tTree(pp, pt);
    if (fnd) {
        tTree* tt = find_tTree_end(pt);
        if (tt->altp!=NULL) {
            val = dup_Buffer(tt->altp->ldat.val);
        }
    }

    return val;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//

/**
tTree*  next_strncmp_vertical_tTree(tTree* pp, const char* key, int len, int no, int* nn)

tTree 検索用補助関数．vertical は縦方向探索
*/
tTree*  next_strncmp_vertical_tTree(tTree* pp, const char* key, int len, int no, int* nn)
{
    do { 
        if (ex_strncmp((char*)(pp->ldat).key.buf, key, len)) {
            (*nn)++;
            if (no==*nn) return pp;
        }
        if (pp->next!=NULL) {
            tTree* tt = next_strncmp_vertical_tTree(pp->next, key, len, no, nn);
            if (tt!=NULL) return tt;
        }
        pp = pp->ysis;
    } while(pp!=NULL);

    return NULL;
}



/**
tTree*  next_strncmp_horizon_tTree (tTree* pp, const char* key, int len, int no, int* nn)

tTree 検索用補助関数．horizon は擬似的な横方向探索（完全な横方向探索ではない）
*/
tTree*  next_strncmp_horizon_tTree(tTree* pp, const char* key, int len, int no, int* nn)
{
    do { 
        if (ex_strncmp((char*)(pp->ldat).key.buf, key, len)) {
            (*nn)++;
            if (no==*nn) return pp;
        }
        if (pp->ysis!=NULL) {
            tTree* tt = next_strncmp_horizon_tTree(pp->ysis, key, len, no, nn);
            if (tt!=NULL) return tt;
        }
        pp = pp->next;
    } while(pp!=NULL);

    return NULL;
}



/**
tTree*  next_strncasecmp_vertical_tTree(tTree* pp, const char* key, int len, int no, int* nn)

tTree 検索用補助関数．vertical は縦方向探索
*/
tTree*  next_strncasecmp_vertical_tTree(tTree* pp, const char* key, int len, int no, int* nn)
{
    do { 
        if (ex_strncasecmp((char*)(pp->ldat).key.buf, key, len)) {
            (*nn)++;
            if (no==*nn) return pp;
        }
        if (pp->next!=NULL) {
            tTree* tt = next_strncasecmp_vertical_tTree(pp->next, key, len, no, nn);
            if (tt!=NULL) return tt;
        }
        pp = pp->ysis;
    } while(pp!=NULL);

    return NULL;
}



/**
tTree*  next_strncasecmp_horizon_tTree (tTree* pp, const char* key, int len, int no, int* nn)

tTree 検索用補助関数．horizon は擬似的な横方向探索（完全な横方向探索ではない）
*/
tTree*  next_strncasecmp_horizon_tTree(tTree* pp, const char* key, int len, int no, int* nn)
{
    do { 
        if (ex_strncasecmp((char*)(pp->ldat).key.buf, key, len)) {
            (*nn)++;
            if (no==*nn) return pp;
        }
        if (pp->ysis!=NULL) {
            tTree* tt = next_strncasecmp_horizon_tTree(pp->ysis, key, len, no, nn);
            if (tt!=NULL) return tt;
        }
        pp = pp->next;
    } while(pp!=NULL);

    return NULL;
}



