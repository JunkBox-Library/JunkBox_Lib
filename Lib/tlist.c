/** 
@brief   Tiny List 構造ライブラリ
@file    tlist.c
@version 
@author  Fumi.Iseki (C)
@date    2008 2/1
@see     _tList
*/

#include  "tlist.h"
#include  "jbxl_state.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
// List Data
//     静的に生成される． 
//

/**
tList_data  init_tList_data(void)

空のノードデータを静的に作成．データを初期化するのに使用する．

@return 作られたノードデータ．
*/
tList_data  init_tList_data(void)
{
    tList_data pp;

    memset(&pp, 0, sizeof(tList_data));
    return pp;
}


/**
tList_data  make_tList_data(int id, int lv, Buffer key, Buffer val, void* ptr, int sz)

データを指定してノードデータを作成@n
sz>0 で ptr==NULL の場合は ptrの領域を確保し，0クリアする．

@param  id   ノードのID
@param  lv   ノードのデータ
@param  key  ノードのキー   (Buffer)  複製
@param  val  ノードのデータ (Buffer)  複製
@param  ptr  汎用データへのポインタ   複製
@param  sz   *ptr のサイズ

@return 作られたノードデータ．
*/
tList_data  make_tList_data(int id, int lv, Buffer key, Buffer val, void* ptr, int sz)
{
    tList_data pp;

    memset(&pp, 0, sizeof(tList_data));

    pp.id  = id;
    pp.lv  = lv;
    pp.sz  = sz;
    pp.key = dup_Buffer(key);    
    pp.val = dup_Buffer(val);

    if (sz>0) {
        pp.ptr = (void*)malloc(sz);
        if (pp.ptr!=NULL) {
            if (ptr!=NULL) memcpy(pp.ptr, ptr, sz);
            else           memset(pp.ptr, 0,   sz);
        }
    }
    
    return pp;
}


/**
tList_data  make_tList_data_bystr(int id, int lv, const char* key, const char* val, void* ptr, int sz)

文字列データを指定してノードデータを作成@n
sz>0 で ptr==NULL の場合は ptrの領域を確保し，0クリアする．

@param  id   ノードのID
@param  lv   ノードのデータ
@param  key  ノードのキー            複製
@param  val  ノードのデータ          複製
@param  ptr  汎用データへのポインタ  複製
@param  sz   *ptr のサイズ

@return 作られたノードデータ．
*/
tList_data  make_tList_data_bystr(int id, int lv, const char* key, const char* val, void* ptr, int sz)
{
    tList_data pp;
    
    memset(&pp, 0, sizeof(tList_data));

    pp.id  = id;
    pp.lv  = lv;
    pp.sz  = sz;
    pp.key = make_Buffer_bystr(key);    // key==NULLなら init_Buffer()
    pp.val = make_Buffer_bystr(val);

    if (sz>0) {
        pp.ptr = (void*)malloc(sz);
        if (pp.ptr!=NULL) {
            if (ptr!=NULL) memcpy(pp.ptr, ptr, sz);
            else           memset(pp.ptr, 0,   sz);
        }
    }

    return pp;
}


/**
void  free_tList_data(tList_data* ldat)

ノードデータのバッファ部をクリアする．データ自身は削除しない．

@param  ldat  クリアするノードデータ
*/
void  free_tList_data(tList_data* ldat)
{
    if (ldat==NULL) return;

    ldat->id = 0;
    ldat->lv = 0;
    ldat->sz = 0;

    free_Buffer(&(ldat->key));    
    free_Buffer(&(ldat->val));
    if (ldat->ptr!=NULL) free(ldat->ptr);
    del_all_tList(&(ldat->lst));

    ldat->key = init_Buffer();
    ldat->val = init_Buffer();
    ldat->ptr = NULL;
    ldat->lst = NULL;

    return;
}


/**
void  del_tList_data(tList_data** ldat)

ノードデータを削除する．

@param  ldat  クリアするノードデータ
*/
void  del_tList_data(tList_data** ldat)
{
    if (ldat==NULL || *ldat==NULL) return;

    free_tList_data(*ldat);
    *ldat = NULL;

    return;
}


/**
tList_data  dup_tList_data(tList_data ldat)

ノードデータの複製を作成する．@n
ldat.ptr にデータがあるなら，ldat.sz には正確にldat.ptrのサイズが設定されている必要がある．

@param  ldat  複製するノードデータ
@return 複製したノードデータへのポインタ．
*/
tList_data  dup_tList_data(tList_data ldat)
{
    tList_data dup;    

    memcpy(&dup, &ldat, sizeof(tList_data));
    dup.key = dup_Buffer(ldat.key);
    dup.val = dup_Buffer(ldat.val);
    if (ldat.ptr!=NULL && ldat.sz>0) {
        dup.ptr = (void*)malloc(ldat.sz);
        if (dup.ptr!=NULL) memcpy(dup.ptr, ldat.ptr, ldat.sz);
    }
    dup.lst = dup_tList(ldat.lst);
    
    return dup;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// Tiny List
//

/**
tList*  new_tList_node(void)

リスト用の空ノードを動的に生成する．

@return 生成されたノードへのポインタ．
*/
tList*  new_tList_node(void)
{
    tList* pp;

    pp = (tList*)malloc(sizeof(tList));
    if (pp==NULL) return NULL;
    memset(pp, 0, sizeof(tList));
    pp->ldat  = init_tList_data();
    pp->state = JBXL_NORMAL;

    return pp;
}


/**
tList  make_tList_node(tList_data ldat)

リスト用ノードを静的に作成する．@n
ldat はそのまま新しい変数内で使用される．（単純に代入される）

この関数で生成したノードを del_*_tList()関数で削除してはいけない．

@param  ldat  ノードデータ
@return 作られたノード．
*/
tList  make_tList_node(tList_data ldat)
{
    tList pp;

    memset(&pp, 0, sizeof(tList));
    pp.ldat  = ldat;    
    pp.state = JBXL_NORMAL;

    return pp;
}


/** 
tList*  free_tList_node(tList* node) 

リスト用ノードのバッファ部(データ)の解放．

@param  node  解放するノードへのポインタ．
@return 削除したノードの直後のノードへのポインタ．
*/
tList*  free_tList_node(tList* node) 
{    
    if (node==NULL) return NULL;

    free_tList_data(&(node->ldat));

    tList* pp = NULL;
    if (node->prev!=NULL) node->prev->next = node->next;
    if (node->next!=NULL) {
        node->next->prev = node->prev;
        pp = node->next;
    }

    return pp;
}


/**    
tList*  del_tList_node(tList** node)

リスト用のノードを削除．
リストポインタ nodeが指すノードを削除し,リスト構造を詰める．

@param[in]  *node  削除ノードへのポインタ．
@param[out] *node  NULL
@return 削除したノードの直後のノードへのポインタ．
*/
tList*  del_tList_node(tList** node)
{
    if (node==NULL || *node==NULL) return NULL;

    tList* pp = free_tList_node(*node);
    free(*node);
    *node = NULL;

    return pp;
}    


/**
tList*  dup_tList_node(tList* node)

ノードデータを複製する (new)．ノードのポインタは複製しない．
*/             
tList*  dup_tList_node(tList* node)
{
    tList* pp;
    int  sz;

    if (node==NULL) return NULL;

    sz = sizeof(tList);
    pp = (tList*)malloc(sz);
    if (pp==NULL) return NULL;

    memcpy(pp, node, sz);
    pp->ldat = dup_tList_data(node->ldat);
    pp->next = NULL;
    pp->prev = NULL;
    pp->altp = NULL;
    pp->yngr = NULL;
    pp->esis = NULL;
    pp->ysis = NULL;

    return pp;
}


/**
tList*  move_tList_node(tList* pp, tList* node)

nodeを現在のリストから切り離し，ppへ移動する．@n
nodeを削除しないで del_tList_node(), add_tList_node() を実行するようなもの．

@param  pp    移動先で親となるノードへのポインタ．
@param  node  移動するノードへのポインタ．
@return 移動したノードノードへのポインタ．
*/
tList*  move_tList_node(tList* pp, tList* node)
{
    if (pp==NULL || node==NULL) return NULL;
    
    if (node->prev!=NULL) node->prev->next = node->next;
    if (node->next!=NULL) node->next->prev = node->prev;

    node->prev = pp;
    node->next = pp->next;
    if (pp->next!=NULL) pp->next->prev = node;
    pp->next   = node;
    
    return node;
}


/**
tList*  swap_tList_node(tList* pp1, tList* pp2)

pp1ノード と pp2ノード のノードデータを入れ替える．
*/
tList*  swap_tList_node(tList* pp1, tList* pp2)
{
    if (pp1==NULL || pp2==NULL) return NULL;

    tList_data swp = pp1->ldat;
    pp1->ldat = pp2->ldat;
    pp2->ldat = swp;

    return pp1;
/*
    if (*pp1==NULL || *pp2==NULL) return NULL;

    tList* p1 = dup_tList_node(*pp1);
    tList* p2 = dup_tList_node(*pp2);

    insert_tList(*pp1, p2);
    insert_tList(*pp2, p1);

    del_tList_node(pp1);
    del_tList_node(pp2);

    *pp1 = p2;
    *pp2 = p1;

    return p2;
*/
}


tList*  new_tList_anchor_node(void)
{
    tList* pp;

    pp = (tList*)malloc(sizeof(tList));
    if (pp==NULL) return NULL;
    memset(pp, 0, sizeof(tList));
    pp->ldat    = init_tList_data();
    pp->ldat.id = TLIST_ANCHOR_NODE;
    pp->depth   = -1;
    pp->state   = JBXL_STATE_ANCHOR;    // TLIST_ANCHOR_NODE と同じ

    return pp;
}


tList*  del_tList_anchor_node(tList* node)
{
    tList* pp = node;

    if (node!=NULL && node->ldat.id==TLIST_ANCHOR_NODE) {
        pp = node->next;
        free_tList_node(node);
        free(node);
    }

    return pp;
}



//////////////////////////////////////////////////////////////////

/**
tList*  add_tList_node_bydata(tList* pp, tList_data ldat) 

データ(ldat)からリスト用ノードを生成し(new),それを指定したリストの後ろに追加．@n

リストポインタ ppが指すノードの後につくり出した ノードを挿入する．
ldat は指定されたものがそのまま使用される．

@param  pp    追加する場所の手前のノードへのポインタ．
@param  ldat  追加するノードデータ．このデータがそのまま使用される．
           
@return 追加したノードへのポインタ．
*/
tList*  add_tList_node_bydata(tList* pp, tList_data ldat) 
{
    tList* pt;

    pt = new_tList_node();
    pt->ldat = ldat;

    if (pp==NULL) {
        return pt;
    }
    //
    if (pp->next==NULL) {
        pt->next = NULL;
        pt->prev = pp;
        pp->next = pt;
        return pt;
    }

    pt->next = pp->next;
    pt->prev = pp;
    pp->next = pt;
    pt->next->prev = pt;

    return pt;
}


/**
tList*  add_tList_node_bystr(tList* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz) 

文字列データからリスト用ノードを生成し(new),それをリストに追加．

リストポインタ ppが指すノードの後につくり出した ノードを挿入する．
sz>0 で ptr==NULL の場合は ptrの領域を確保し，0クリアする．

@param  pp   追加する場所の手前のノードへのポインタ．
@param  id   追加するデータ．
@param  lv   追加するデータ．
@param  key  追加するデータ．        複製
@param  val  追加するデータ．        複製
@param  ptr  汎用データへのポインタ  複製
@param  sz   *ptr のサイズ
           
@return 追加したノードへのポインタ．
*/
tList*  add_tList_node_bystr(tList* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz) 
{
    tList* pt;
    tList_data ldat;

    ldat = make_tList_data_bystr(id, lv, key, val, ptr, sz);
    pt   = add_tList_node_bydata(pp, ldat);

    return pt;
}


/**
tList*  add_tList_node_byBuffer(tList* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz) 

Buffer データからリスト用ノードを生成し(new),それをリストに追加．

リストポインタ ppが指すノードの後につくり出した ノードを挿入する．
sz>0 で ptr==NULL の場合は ptrの領域を確保し，0クリアする．

@param  pp    追加する場所の手前のノードへのポインタ．
@param  id    追加するデータ．
@param  lv    追加するデータ．
@param  key   追加するデータ．        複製
@param  val   追加するデータ．        複製
@param  *ptr  汎用データへのポインタ  複製
@param  sz    *ptr のサイズ
           
@return 追加したノードへのポインタ．
*/
tList*  add_tList_node_byBuffer(tList* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz) 
{
    tList* pt;
    tList_data ldat;

    ldat = make_tList_data(id, lv, key, val, ptr, sz);
    pt   = add_tList_node_bydata(pp, ldat);

    return pt;
}


/**
void  set_tList_node_bydata(tList* node, tList_data dat)

リストのノードに値を設定する．@n
ldat は指定されたものがそのまま使用される．

@param  node  設定するノードへのポインタ．
@param  dat   設定するノードデータ．このデータがそのまま使用される．
*/
void  set_tList_node_bydata(tList* node, tList_data dat)
{
    if (node==NULL) return;

    free_tList_data(&(node->ldat));
    node->ldat = dat;
}


/**
void  set_tList_node_bystr(tList* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz)

文字列データから リストのノードに値を設定する．それぞれのデータは複製されて設定される．@n
key, valが NULLの場合は，その値はノードは設定されない．

@param  pp   ノード値を設定するノードへのポインタ
@param  id   ノードのID
@param  lv   ノードのデータ
@param  key  ノードのキー            複製
@param  val  ノードのデータ          複製
@param  ptr  汎用データへのポインタ  複製
@param  sz   *ptr のサイズ
*/
void  set_tList_node_bystr(tList* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz)
{
    if (pp==NULL) return;

    pp->ldat.id = id;
    pp->ldat.lv = lv;
    pp->ldat.sz = sz;

    if (key!=NULL) {
        free_Buffer(&(pp->ldat.key));
        pp->ldat.key = make_Buffer_bystr(key);
    }
    if (val!=NULL) {
        free_Buffer(&(pp->ldat.val));
        pp->ldat.val = make_Buffer_bystr(val);
    }

    if (sz>0 && ptr!=NULL) {
        if (pp->ldat.ptr!=NULL) free(pp->ldat.ptr);
        pp->ldat.ptr = (void*)malloc(sz);
        if (pp->ldat.ptr!=NULL) memcpy(pp->ldat.ptr, ptr, sz);
    }
}


/**
void  set_tList_node_byBuffer(tList* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz)

リストのノードに値を設定する．それぞれのデータは複製されて設定される．@n
key.buf, val.bufが NULLの場合は，その値はノードには設定されない．

@param  pp   ノード値を設定するノードへのポインタ
@param  id   ノードのID
@param  lv   ノードのデータ
@param  key  ノードのキー   (Buffer) 複製
@param  val  ノードのデータ (Buffer) 複製
@param  ptr  汎用データへのポインタ  複製
@param  sz   *ptr のサイズ
*/
void  set_tList_node_byBuffer(tList* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz)
{
    if (pp==NULL) return;

    pp->ldat.id = id;
    pp->ldat.lv = lv;

    if (pp->ldat.key.buf!=NULL) {
        free_Buffer(&(pp->ldat.key));
        pp->ldat.key = dup_Buffer(key);
    }

    if (pp->ldat.val.buf!=NULL) {
        free_Buffer(&(pp->ldat.val));
        pp->ldat.val = dup_Buffer(val);
    }

    if (sz>0 && ptr!=NULL) {
        if (pp->ldat.ptr!=NULL) free(pp->ldat.ptr);
        pp->ldat.ptr = (void*)malloc(sz);
        if (pp->ldat.ptr!=NULL) memcpy(pp->ldat.ptr, ptr, sz);
    }
}


/**
tList*  update_tList_node(tList* pp, tList* pt)

ppを pt->keyで検索し，該当ノードがあればノードをコピーする．無ければ最後にノードを追加する(new)．

@param  pp  検索を開始するノードへのポインタ．
@param  pt  設定するノードへのポインタ．検索キーは pt->key
@return 設定または追加したノードへのポインタ
*/
tList*  update_tList_node(tList* pp, tList* pt)
{
    tList* pm;
    tList_data ldat;

    if (pt==NULL) return pp;
    
    ldat = pp->ldat;
    pm = update_tList_node_byBuffer(pp, ldat.id, ldat.lv, ldat.key, ldat.val, ldat.ptr, ldat.sz);
    
    return pm;
}


/**
tList*  update_tList_node_bydata(tList* pp, char* srch, tList_data ldat)

ppを srchで検索し，該当ノードがあればノードを設定する．無ければ最後にノードを追加する(new)．@n
ldat は指定されたものがそのまま使用される．

@param  pp    検索を開始するノードへのポインタ．
@param  srch  検索キー
@param  ldat  追加するノードデータ．このデータがそのまま使用される．

@return 設定または追加したノードへのポインタ
*/
tList*  update_tList_node_bydata(tList* pp, char* srch, tList_data ldat)
{
    tList* pm = NULL;

    if (pp==NULL || srch==NULL) return NULL;

    pm = strncmp_tList(pp, srch, 0, 1);
    if (pm!=NULL) set_tList_node_bydata(pm, ldat);
    else {
        pm = find_tList_end(pp);
        pm = add_tList_node_bydata(pm, ldat);
    }
    
    return pm;
}


/**
tList*  update_tList_node_bystr(tList* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz)

ppを keyで検索し，該当ノードがあればノードを設定する．無ければ最後にノードを追加する(new)．@n
それぞれのデータは複製される．

@param  pp   検索を開始するノードへのポインタ．
@param  id   ノードのID
@param  lv   ノードのデータ
@param  key  ノードのキー．検索キー      複製
@param  val  ノードのデータ (Buffer)  複製
@param  ptr  汎用データへのポインタ      複製
@param  sz   *ptr のサイズ

@return 設定または追加したノードへのポインタ
*/
tList*  update_tList_node_bystr(tList* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz)
{
    tList* pm = NULL;

    if (pp==NULL || key==NULL) return NULL;

    pm = strncmp_tList(pp, key, 0, 1);
    if (pm!=NULL) {
        set_tList_node_bystr(pm, id, lv, NULL, val, ptr, sz);
    }
    else {
        pm = find_tList_end(pp);
        pm = add_tList_node_bystr(pm, id, lv, key, val, ptr, sz);
    }
    
    return pm;
}


/**
tList*  update_tList_node_byBuffer(tList* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz)

ppを key.bufで検索し，該当ノードがあればノードを設定する．無ければ最後にノードを追加する(new)．@n
それぞれのデータは複製される．

@param  pp   検索を開始するノードへのポインタ．
@param  id   ノードのID
@param  lv   ノードのデータ
@param  key  ノードのキー   (Buffer)  複製
@param  val  ノードのデータ (Buffer)  複製
@param  ptr  汎用データへのポインタ      複製
@param  sz   *ptr のサイズ

@return 設定または追加したノードへのポインタ
*/
tList*  update_tList_node_byBuffer(tList* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz)
{
    tList* pm = NULL;

    if (pp==NULL || key.buf==NULL) return NULL;

    pm = strncmp_tList(pp, (char*)key.buf, 0, 1);
    if (pm!=NULL) set_tList_node_byBuffer(pm, id, lv, key, val, ptr, sz);
    else {
        pm = find_tList_end(pp);
        pm = add_tList_node_byBuffer(pm, id, lv, key, val, ptr, sz);
    }
    
    return pm;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// tiny List
//

/** 
tList*  del_tList(tList** pp) 

指定したリストノード以降のリストを削除．

@param[in]  *pp  削除するノードへのポインタ．
@param[out] *pp  NULL

@return 削除したノード(s)の親ノードへのポインタ．
*/
tList*  del_tList(tList** pp) 
{    
    tList* pt;
    tList* pm;
    tList* pw;

    if (pp==NULL || *pp==NULL) return NULL;

    pt = (*pp)->prev;
    if (pt!=NULL) pt->next = NULL;

    pm = *pp;
    while (pm!=NULL) {
        pw = pm;
        pm = pm->next;
        free_tList_data(&(pw->ldat));
        free(pw);
    }
    *pp = NULL;

    return pt;
}


/**
void  del_all_tList(tList** pp)

リストの全ノードの削除．ポインタ ppのノードを含むリスト全体を削除する．@n
但し altp のリンク先は削除しない．
pp はりスト中であれば，どこを指していても良い．

@param[in]  *pp  削除を開始するノードへのポインタ．
@param[out] *pp  NULL
*/
void  del_all_tList(tList** pp)
{
    tList* pm;
    tList* pv;

    if (pp==NULL || *pp==NULL) return;

    pm = *pp;
    pv = (*pp)->prev;

    do {
        pm = del_tList_node(&pm);
    } while (pm!=NULL);

    pm = pv;
    while (pm!=NULL) {
        pv = pm->prev;
        del_tList_node(&pm);
        pm = pv;
    }

    *pp = NULL;
}


/**     
int  del_tList_key(tList* pl, const char* key, int no)
        
plからサーチして，no番目の keyのノードを削除する．
        
@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）．大文字，小文字を区別しない．    
@param  no   一致した物の中で何番目の物を削除するか指定する．1から数える．0の場合は全て削除する．
        
@return 削除したノードの数．
*/      
int  del_tList_key(tList** pl, const char* key, int no)
{       
    int dl = 0;
    int nn = 0;
    if (no<0) no = 0;

    tList* pp = *pl;

    while (pp!=NULL) {
        if (ex_strncasecmp((char*)(pp->ldat).key.buf, key, 0)) {
            nn++;
            if (no==0 || no==nn) {
                if (pp->prev==NULL) *pl = pp->next;
                pp = del_tList_node(&pp);    
                dl++;
                if (no!=0) break;
            }
            else {
                pp = pp->next;
            }
        }
        else {
            pp = pp->next;
        }
    }

    return dl;
}


/**
tList*  dup_tList(tList* pp)

リストを複製する．

@param  pp  複製元のリストへのポインタ．
@return 複製されたリストへのポインタ．
*/             
tList*  dup_tList(tList* pp)
{
    tList* pt;
    tList* pl;
    tList* tt;

    if (pp==NULL) return NULL;

    pt = pl = dup_tList_node(pp);
    pp = pp->next;

    while(pp!=NULL) {
        tt = dup_tList_node(pp);
        pl = insert_tList(pl, tt);
        pp = pp->next;
    }

    return pt;
}


/**
tList*  add_tList_end(tList* pp, tList* pt)

リストppの最後に リストptを追加する．

@param  pp  先頭のリストへのポインタ
@param  pt  追加するリストへのポインタ
@return リストの先頭へのポインタ

@code 
pp->1->2->3, pt->4->5 => pp->1->2->3->pt->4->5 
@endcode
*/
tList*  add_tList_end(tList* pp, tList* pt)
{
    if (pt==NULL) return pp;
    if (pp==NULL) return pt;

    tList* pe = find_tList_end(pp);
    pe->next = pt;
    pt->prev = pe;

    return pp;
}


/**
tList*  insert_tList(tList* pp, tList* pt)

ノードppの直ぐ後ろに ptを挿入する．
    
@param  pp  先頭のリストへのポインタ
@param  pt  挿入するリストへのポインタ
@return 挿入した先頭のリストへのアドレス．

@code
pp->1->2->3, pt->4->5 => pp->pt->4->5->1->2->3
@endcode
*/
tList*  insert_tList(tList* pp, tList* pt)
{
    tList* pe;
    
    if (pt==NULL) return pp;
    if (pp==NULL) return pt;

    pe = find_tList_end(pt);
    if (pp->next!=NULL) pp->next->prev = pe;
    pe->next = pp->next;
    pp->next = pt;
    pt->prev = pp;
    
    return pt;
}


/**
void  print_tList(File* fp, tList* pp)

リストの表示．ポインタ pp以降の全てのノードを fpに表示する．

@param  fp  出力するファイルへのポインタ．NULLの場合は stderr
@param  pp  表示を開始するノードへのポインタ．
*/
void  print_tList(FILE* fp, tList* pp)
{
    if (fp==NULL) fp = stderr;

    if (pp!=NULL) {
        while(pp!=NULL) {
            tList_data ld = pp->ldat;
            fprintf(fp, "[%d] [%d] [%s] [%s]\n", ld.id, ld.lv, ld.key.buf, ld.val.buf);
            //if (pp->next!=NULL) print_tList(pp->next);
            pp = pp->next;
        }
    }
    else {
        fprintf(fp, "(List is NULL)\n");
    }
    return;
}


/**
void  dump_tList(FILE* fp, tList* pp)

val.buf を 16進ダンプする．他のデータは通常通り表示する．
*/
void  dump_tList(FILE* fp, tList* pp)
{
    if (fp==NULL) fp = stderr;

    if (pp!=NULL) {
        while(pp!=NULL) {
            tList_data ld = pp->ldat;
            fprintf(fp, "[%d] [%d] [%s] [%d]\n", ld.id, ld.lv, ld.key.buf, ld.val.vldsz);
            fdump(fp, (unsigned char*)ld.val.buf, ld.val.vldsz);
            pp = pp->next;
        }
    }
    else {
        fprintf(fp, "(List is NULL)\n");
    }
    return;
}


/**
int  count_tList(tList* pp)

リストの ppノード以降のノードの数を数える．

@param  pp  数え始めるノードへのポインタ．
@return ノードの数．
*/
int  count_tList(tList* pp)
{
    int cnt = 0;

    while (pp!=NULL) {
        cnt++;
        pp = pp->next;
    }    
    return cnt;
}



///////////////////////////////////////////////////////////////////////////////////////////

/**
tList*  find_tList_top(tList* pl)

リストの最初のノードを探す．

@param  pl  サーチを開始するノードへのポインタ．
@return リストの最初のノードへのポインタ．
*/
tList*  find_tList_top(tList* pl)
{
    if (pl==NULL) return NULL;

    while (pl->prev!=NULL) pl = pl->prev;
    return pl;
}


/**
tList*  find_tList_end(tList* pl)

リストの最後のノードを探す．

@attention
リスト構造の最後のノードを探す関数なので，tTree 構造に適用しても正しいノードを見つけることは出来ない．@n
tTreeに対しては find_tTree_end()を使うこと．

@param  pl  サーチを開始するノードへのポインタ．
@return リストの最後のノードへのポインタ．
*/
tList*  find_tList_end(tList* pl)
{
    if (pl==NULL) return NULL;

    while (pl->next!=NULL) pl = pl->next;
    return pl;
}



///////////////////////////////////////////////////////////////////////////////////////////
// String Compare
//

/**
tList*  strncmp_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるノードのサーチ．

ポインタ pl以降のノードで,キー部の文字列が keyと前方一致（部分的も可）するノードの内，
no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strncmp_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (len<=-3) return NULL;
    if (no<=0) no = 1;

    while (pl!=NULL) {
        if (ex_strncmp((char*)pl->ldat.key.buf, key, len)) {
            nn++;
            if (no==nn) return pl;
        }
        pl = pl->next;
    }
    return NULL;
}


/**
tList*  strncasecmp_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるノードのサーチ．大文字小文字を無視する．

ポインタ pl以降のノードで,キー部の文字列が keyと前方一致（部分的も可）するノードの内，
no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strncasecmp_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (len<=-3) return NULL;
    if (no<=0) no = 1;

    while (pl!=NULL) {
        if (ex_strncasecmp((char*)(pl->ldat).key.buf, key, len)) {
            nn++;
            if (no==nn) return pl;
        }
        pl = pl->next;
    }
    return NULL;
}


/**
tList*  strnrvscmp_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるノードのサーチ．文字の後方から比べる．

ポインタ pl以降のノードで,キー部の文字列が keyと後方一致（部分的も可）するノードの内，
no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strnrvscmp_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (len<=-3) return NULL;
    if (no<=0) no = 1;

    while (pl!=NULL) {
        if (ex_strnrvscmp((char*)(pl->ldat).key.buf, key, len)) {
            nn++;
            if (no==nn) return pl;
        }
        pl = pl->next;
    }
    return NULL;
}


/**
tList*  strncaservscmp_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるノードのサーチ．文字の後方から比べる．大文字小文字を無視．

ポインタ pl以降のノードで,キー部の文字列が keyと後方一致（部分的も可）するノードの内，
no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strncaservscmp_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (len<=-3) return NULL;
    if (no<=0) no = 1;

    while (pl!=NULL) {
        if (ex_strncaservscmp((char*)(pl->ldat).key.buf, key, len)) {
            nn++;
            if (no==nn) return pl;
        }
        pl = pl->next;
    }
    return NULL;
}


/**
tList*  strstr_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるノードの部分文字列サーチ．

ポインタ pl以降のノードで,キー部の文字列または keyの文字列が相手の文字列に含まれるノードの内，
no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー．
@param  len  0以上: key が plのキーに含まれるかどうか検査する．
@param  len  負数:  plのキーが Keyに含まれるかどうか検査する．
@param  no   一致した物の中で何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strstr_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (no<=0) no = 1;

    while (pl!=NULL) {
        if (len>=0) {
            if (strstr((char*)(pl->ldat).key.buf, key)!=NULL) {
                nn++;
                if (no==nn) return pl;
            }
        }
        else if (len<0) {
            if (strstr(key, (char*)(pl->ldat).key.buf)!=NULL) {
                nn++;
                if (no==nn) return pl;
            }
        }

        pl = pl->next;
    }
    return NULL;
}


/**
tList*  strstrcase_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるノードの部分文字列サーチ．大文字小文字を無視．

ポインタ pl以降のノードで,キー部の文字列または keyの文字列が相手の文字列に含まれるノードの内，
no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー．
@param  len  0以上: key が plのキーに含まれるかどうか検査する．
@param  len  負数:  plのキーが Keyに含まれるかどうか検査する．
@param  no   一致した物の中で何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strstrcase_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (no<=0) no = 1;

    while (pl!=NULL) {
        if (len>=0) {
            if (strstrcase((char*)(pl->ldat).key.buf, key)!=NULL) {
                nn++;
                if (no==nn) return pl;
            }
        }
        else if (len<0) {
            if (strstrcase(key, (char*)(pl->ldat).key.buf)!=NULL) {
                nn++;
                if (no==nn) return pl;
            }
        }

        pl = pl->next;
    }
    return NULL;
}



///////////////////////////////////////////////////////////////////////////////////////////
// String Compare Back List
//

/**
tList*  strncmp_back_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるリストの後方からのノードのサーチ．@n
ポインタ pl以降のノードで,キー部の文字列が keyと前方一致（部分的も可）するノードの内，後ろから数えて no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で後ろから数えて何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strncmp_back_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (len<=-3) return NULL;
    if (no<=0) no = 1;

    pl = find_tList_end(pl);
    
    while (pl!=NULL) {
        if (ex_strncmp((char*)(pl->ldat).key.buf, key, len)) {
            nn++;
            if (no==nn) return pl;
        }
        pl = pl->prev;
    }
    return NULL;
}


/**
tList*  strncasecmp_back_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるリストの後方からのノードのサーチ．大文字小文字を無視する．@n
ポインタ pl以降のノードで,キー部の文字列が keyと前方一致（部分的も可）するノードの内，後ろから数えて no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で後ろから数えて何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strncasecmp_back_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (len<=-3) return NULL;
    if (no<=0) no = 1;

    pl = find_tList_end(pl);

    while (pl!=NULL) {
        if (ex_strncasecmp((char*)(pl->ldat).key.buf, key, len)) {
            nn++;
            if (no==nn) return pl;
        }
        pl = pl->prev;
    }
    return NULL;
}


/**
tList*  strnrvscmp_back_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるリストの後方からのノードのサーチ．文字も後方から比べる．@n
ポインタ pl以降のノードで,キー部の文字列が keyと後方一致（部分的も可）するノードの内，後ろから数えて no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で後ろから数えて何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strnrvscmp_back_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (len<=-3) return NULL;
    if (no<=0) no = 1;

    pl = find_tList_end(pl);

    while (pl!=NULL) {
        if (ex_strnrvscmp((char*)(pl->ldat).key.buf, key, len)) {
            nn++;
            if (no==nn) return pl;
        }
        pl = pl->prev;
    }
    return NULL;
}


/**
tList*  strncaservscmp_back_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるリストの後方からのノードのサーチ．文字も後方から比べる．大文字小文字を無視．@n
ポインタ pl以降のノードで,キー部の文字列が keyと後方一致（部分的も可）するノードの内，後ろから数えて no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー（文字列）
@param  len  1以上: 一致させる長さ．
@param  len  @b TLIST_MATCH_COMPLETE   (0): 完全一致．
@param  len  @b TLIST_MATCH_TLISTKEY  (-1): pl->key.buf の長さに合わせる．
@param  len  @b TLIST_MATCH_STRINGKEY (-2): key の長さに合わせる．
@param  no   一致した物の中で後ろから数えて何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strncaservscmp_back_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (len<=-3) return NULL;
    if (no<=0) no = 1;

    pl = find_tList_end(pl);

    while (pl!=NULL) {
        if (ex_strncaservscmp((char*)(pl->ldat).key.buf, key, len)) {
            nn++;
            if (no==nn) return pl;
        }
        pl = pl->prev;
    }
    return NULL;
}


/**
tList*  strstr_back_tList(tList* pl, const char* key, int len, int no)

char* 型変数によるリストの後方からのノードの部分文字列サーチ．

ポインタ pl以降のノードで,キー部の文字列または keyの文字列が相手の文字列に含まれる．@n
ノードの内，後ろから数えて no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー．
@param  len  0以上: key が plのキーに含まれるかどうか検査する．
@param  len  負数:  plのキーが Keyに含まれるかどうか検査する．
@param  no   一致した物の中で後ろから数えて何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strstr_back_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (no<=0) no = 1;

    pl = find_tList_end(pl);

    while (pl!=NULL) {
        if (len>=0) {
            if (strstr((char*)(pl->ldat).key.buf, key)!=NULL) {
                nn++;
                if (no==nn) return pl;
            }
        }
        else if (len<0) {
            if (strstr(key, (char*)(pl->ldat).key.buf)!=NULL) {
                nn++;
                if (no==nn) return pl;
            }
        }

        pl = pl->prev;
    }
    return NULL;
}


/**
tList*  strstrcase_back_tList(tList* pl, const char* key, int len, int no)

char* 型変数による後方からのノードの部分文字列サーチ．大文字小文字を無視．

ポインタ pl以降のノードで,キー部の文字列または keyの文字列が相手の文字列に含まれる．@n
ノードの内，後ろから数えて no番目にあるのを捜し出す．

@param  pl   サーチを開始するノードへのポインタ．
@param  key  サーチキー．
@param  len  0以上: key が plのキーに含まれるかどうか検査する．
@param  len  負数:  plのキーが Keyに含まれるかどうか検査する．
@param  no   一致した物の中で後ろから数えて何番目の物を返すか指定する．1から数える．

@return 一致したノードへのポインタ
@retval NULL 一致したものが無い
*/
tList*  strstrcase_back_tList(tList* pl, const char* key, int len, int no)
{
    int nn = 0;

    if (pl==NULL || key==NULL) return NULL;
    if (no<=0) no = 1;

    pl = find_tList_end(pl);

    while (pl!=NULL) {
        if (len>=0) {
            if (strstrcase((char*)(pl->ldat).key.buf, key)!=NULL) {
                nn++;
                if (no==nn) return pl;
            }
        }
        else if (len<0) {
            if (strstrcase(key, (char*)(pl->ldat).key.buf)!=NULL) {
                nn++;
                if (no==nn) return pl;
            }
        }

        pl = pl->prev;
    }
    return NULL;
}



///////////////////////////////////////////////////////////////////////////////////////////

/**
tList*  search_id_tList(tList* pl, int id, int no) 

リストの中から no番目の idノード(ldat.id)を探し出し，tListのポインタを返す．@n
key はケースインセンシティブ．

@param  pl   検索を行うリストへのポインタ．
@param  id   ノードのid値．
@param  no   何番目のノードを取り出すか指定する．1から数える．

@return tList へのポインタ
*/
tList*  search_id_tList(tList* pl, int id, int no) 
{
    int nn = 0;

    if (pl==NULL) return NULL;
    if (no<=0) no = 1;

    while (pl!=NULL) {
        if (pl->ldat.id == id) {
            nn++;
            if (no==nn) return pl;
        }
        pl = pl->next;
    }
    return NULL;
}


/**
tList*  search_key_tList(tList* pl, const char* key, int no) 

リストの中から no番目の keyノード(ldat.key)を探し出し，tListへのポインタを返す．@n
key はケースインセンシティブ．

@param  pl    検索を行うリストへのポインタ．
@param  key   ノードキー．大文字，小文字を区別しない．
@param  no    何番目のノードを取り出すか指定する．1から数える．

@return tList へのポインタ
*/
tList*  search_key_tList(tList* pl, const char* key, int no) 
{
    tList* pp;
    if (pl==NULL || key==NULL) return NULL;
    if (no<=0) no = 1;
    
    pp = strncasecmp_tList(pl, key, 0, no);        // 完全一致
    
    return pp;
}


/**
Buffer  buffer_key_tList(tList* list, const char* key, int no) 

リストの中から no番目の keyノード(ldat.key)を探し出し，ldat.valのコピーを返す．@n
key はケースインセンシティブ．

@param  list  検索を行うリストへのポインタ．
@param  key   ヘッダ種別．大文字，小文字を区別しない．
@param  no    何番目のノードを取り出すか指定する．1から数える．

@return ldat.val データのコピー．
*/
Buffer  buffer_key_tList(tList* list, const char* key, int no) 
{
    tList* pp;
    Buffer buf;

    buf = init_Buffer();
    if (list==NULL || key==NULL) return buf;
    if (no<=0) no = 1;
    
    pp = strncasecmp_tList(list, key, 0, no);        // 完全一致
    if (pp!=NULL) {
        buf = dup_Buffer(pp->ldat.val);
    }
    
    return buf;
}


/**
Buffer  buffer_key_value_tList(tList* list, const char* key, char* data, int no) 

リストの中から no番目の keyノードを探し出し，ノード値が data で始まる場合，そのノード値を返す．@n
key, dataはケースインセンシティブ．

@param  list  検索を行うリストへのポインタ．
@param  key   ヘッダ種別． 大文字，小文字を区別しない．
@param  data  チェックするノード値の最初の文字．
@param  no    何番目のノードを取り出すか指定する．1から数える．

@return ノード値の格納された Buffer型変数．
*/
Buffer  buffer_key_value_tList(tList* list, const char* key, char* data, int no) 
{
    tList* pp;
    Buffer buf;
    char*  str;
    int    len;

    buf = init_Buffer();
    if (list==NULL || key==NULL) return buf;
    if (no<=0) no = 1;

    if (data==NULL) {
        buf = buffer_key_tList(list, key, no);
        return buf;
    }

    buf = init_Buffer();
    len = (int)strlen(data); 
    
    pp = strncasecmp_tList(list, key, 0, no);
    if (pp!=NULL) {
        str = (char*)pp->ldat.val.buf;
        if (str!=NULL && !strncasecmp(str, data, len)) {
            buf = make_Buffer_bystr(str);
            return buf;
        }
    }

    return buf;
}


/*
int  set_value_tList(tList* list, const char* key, int no, const char* value, int add_mode)

リスト(lt)中の no番目の keyノードの値に valueを設定する．

no が 0以下の場合は，全ての keyノードの値に対して設定が行われる．@n
keyノードが存在せず，かつ mode==ON の場合は，リストの最後に追加される．

@param  list    処理対象のリスト
@param  key     設定を行うノードのキー部．大文字，小文字を区別しない．
@param  value   設定される文字列．
@param  no      keyが一致する何個目のノードに対して設定を行うか．1から数える．0以下の場合はkeyが一致するすべてのノードに対して設定を行う．
@param  add_mod この値がON かつ指定したノードが無い場合，ノードをリストの最後に追加する．

@retval 設定されたノードの数 指定されたノードが存在しない場合は（追加された場合も）0
@retval 負数    エラー
*/
int  set_value_tList(tList* list, const char* key, int no, const char* value, int add_mode)
{
    int  cn = 0;
    tList* pm;

    if (list==NULL || key==NULL || value==NULL) return JBXL_ARGS_ERROR;

    if (no>0) {
        pm = strncasecmp_tList(list, key, 0, no);
        if (pm!=NULL) {
            int rep = set_value_tList_node(pm, value);
            if (rep) cn = 1;
        }
    }
    else {        // no<=0
        int nn = 1;
        cn = 0;
        pm = strncasecmp_tList(list, key, 0, nn);
        while (pm!=NULL) {
            int rep = set_value_tList_node(pm, value);
            if (rep) cn++;
            pm = strncasecmp_tList(list, key, 0, ++nn);
        }
    }

    // Not Found
    if (add_mode==ON && cn==0) {
        add_tList_node_str(list, key, value);
    }

    return cn;
}


/**
int  set_value_tList_node(tList* lp, const char* value)

lp->ldat.val に文字列を設定する（置き換える）．
*/
int  set_value_tList_node(tList* lp, const char* value)
{
    if (lp==NULL || value==NULL) return FALSE;
    
    Buffer buf = make_Buffer_bystr(value);
    free_Buffer(&lp->ldat.val);
    lp->ldat.val = buf;

    return TRUE;
}


/**
int  replace_value_tList(tList* list, const char* key, int no, const char* srcval, char* value)

リスト(lt)中の no番目の keyノードの値の srcvalの部分を value に置き換える．

no が 0以下の場合は，全ての keyノードの値に対して置き換えが行われる．

@param  list   処理対象のリスト
@param  key    置き換えを行うノードのキー部．大文字，小文字を区別しない．
@param  srcval 置き換え対象の文字列．NULLなら指定した項目の文字列全体．
@param  value  置き換えを行う文字列．
@param  no     何個目のノードを置き換えるか．1から数える．0以下の場合はkeyが一致する全てのノードを置き換える

@retval 設定されたノードの数 指定されたノードが存在しない場合は（追加された場合も）0
@retval 負数   エラー
*/
int  replace_value_tList(tList* list, const char* key, int no, const char* srcval, char* value)
{
    int  cn = 0;
    tList* pm;

    if (list==NULL || key==NULL || value==NULL) return JBXL_ARGS_ERROR;
    if (srcval==NULL) {
        return set_value_tList(list, key, no, value, OFF);
    }

    if (no>0) {
        pm = strncasecmp_tList(list, key, 0, no);
        if (pm!=NULL) {
            int rep = replace_value_tList_node(pm, srcval, value);
            if (rep) cn = 1;
        }
    }
    else {      // no<=0
        int nn = 1;
        cn = 0;
        pm = strncasecmp_tList(list, key, 0, nn);
        while (pm!=NULL) {
            int rep = replace_value_tList_node(pm, srcval, value);
            if (rep) cn++;
            pm = strncasecmp_tList(list, key, 0, ++nn);
        }
    }
    
    return cn;
}


/**
int  replace_value_tList_node(tList* lp, const char* srcval, const char* value)

lt->ldat.val のsrcval部分を  value に置き換える．
*/
int  replace_value_tList_node(tList* lp, const char* srcval, const char* value)
{
    if (lp==NULL || value==NULL) return FALSE;
    if (srcval==NULL) {
        return set_value_tList_node(lp, value);
    }

    Buffer buf = replace_sBuffer(lp->ldat.val, srcval, value);
    free_Buffer(&lp->ldat.val);
    lp->ldat.val = buf;
    
    return TRUE;
}
    

/**
tList*  awk_tList(char* str, char cc)

文字列を区切り文字で区切って，各項目をリストのキー部に入れて返す．@n
項目が存在しない場合でもリストは返す（キー部の bufが NULL）．

@param  str  処理する文字列
@param  cc   項目の区切り文字
@return 項目をキー部に入れたリスト．項目が存在しない場合でもリストは返す．
@see cawk_tList
*/
tList*  awk_tList(char* str, char cc)
{
    int  nn = 1;
    char*  item;
    tList* lp = NULL;

    if (str==NULL) return NULL;

    lp = add_tList_node_bystr(NULL, 1, 0, NULL, NULL, NULL, 0);

    // first item
    item = awk(str, cc, nn);
    if (item==NULL) return lp;
    lp->ldat.key = make_Buffer_bystr(item);

    //
    item = awk(str, cc, ++nn);
    while (item!=NULL) {
        lp = add_tList_node_bystr(lp, nn, 0, item, NULL, NULL, 0);
        free(item);
        item = awk(str, cc, ++nn);
    }

    if (lp!=NULL) lp = find_tList_top(lp);
    return lp;
}


/**
tList*  cawk_tList(char* str, char cc)

文字列を区切り文字で区切って，各項目をリストのキー部に入れて返す．@n
複数の区切り文字も1個と見なす．@n
項目が存在しない場合でもリストは返す（キー部の bufが NULL）．

@param  str  処理する文字列
@param  cc   項目の区切り文字
@return 項目をキー部に入れたリスト．項目が存在しない場合でもリストは返す．
*/
tList*  cawk_tList(char* str, char cc)
{
    int  nn = 1;
    char*  item;
    tList* lp = NULL;

    if (str==NULL) return NULL;

    lp = add_tList_node_bystr(NULL, 1, 0, NULL, NULL, NULL, 0);

    // first item
    item = cawk(str, cc, nn);
    if (item==NULL) return lp;
    lp->ldat.key = make_Buffer_bystr(item);

    //
    item = cawk(str, cc, ++nn);
    while (item!=NULL) {
        lp = add_tList_node_bystr(lp, nn, 0, item, NULL, NULL, 0);
        free(item);
        item = cawk(str, cc, ++nn);
    }

    if (lp!=NULL) lp = find_tList_top(lp);
    return lp;
}


/**
tList*  awk_Buffer_tList(Buffer buf, char cc)

Buffer 中の文字列を区切り文字で区切って，各項目をリストのキー部に入れて返す．@n
項目が存在しない場合でもリストは返す（キー部の bufが NULL）．

@param  buf  処理する Buffer変数
@param  cc   項目の区切り文字
@return 項目をキー部に入れたリスト．項目が存在しない場合でもリストは返す．
@see cawk_Buffer_tList
*/
tList*  awk_Buffer_tList(Buffer buf, char cc)
{
    int  nn = 1;
    Buffer item;
    tList* lp = NULL;

    if (buf.buf==NULL) return NULL;

    item = awk_Buffer(buf, cc, nn);
    while (item.buf!=NULL) {
        lp = add_tList_node_bystr(lp, nn, 0, (char*)item.buf, NULL, NULL, 0);
        free_Buffer(&item);
        item = awk_Buffer(buf, cc, ++nn);
    }

    if (lp!=NULL) lp = find_tList_top(lp);
    return lp;
}


/**
tList*  cawk_Buffer_tList(Buffer buf, char cc)

Buffer 中の文字列を区切り文字で区切って，各項目をリストのキー部に入れて返す．@n
複数の区切り文字も1個と見なす．@n
項目が存在しない場合でもリストは返す（キー部の bufが NULL）．

@param  buf  処理する Buffer変数
@param  cc   項目の区切り文字
@return 項目をキー部に入れたリスト．項目が存在しない場合でもリストは返す．
*/
tList*  cawk_Buffer_tList(Buffer buf, char cc)
{
    int  nn = 1;
    Buffer item;
    tList* lp = NULL;

    if (buf.buf==NULL) return NULL;

    item = cawk_Buffer(buf, cc, nn);
    while (item.buf!=NULL) {
        lp = add_tList_node_bystr(lp, nn, 0, (char*)item.buf, NULL, NULL, 0);
        free_Buffer(&item);
        item = cawk_Buffer(buf, cc, ++nn);
    }

    if (lp!=NULL) lp = find_tList_top(lp);
    return lp;

}


/**
char*  get_str_join_tList(tList* lp, const char* deli)

リストの一連のキー部を deliを区切りにして結合して，その文字列を返す．@n
要 free()
*/
char*  get_str_join_tList(tList* lp, const char* deli)
{
    Buffer buf = get_Buffer_join_tList(lp, deli);
    return (char*)buf.buf;
}


/**
Buffer  get_Buffer_join_tList(tList* lp, const char* deli)

リストの一連のキー部を deliを区切りにして結合する．
*/
Buffer  get_Buffer_join_tList(tList* lp, const char* deli)
{
    Buffer buf;
    
    buf = init_Buffer();
    if (lp==NULL) return buf;

    buf = make_Buffer(LBUF);

    if (lp!=NULL && lp->ldat.key.buf!=NULL) {
        cat_s2Buffer((char*)lp->ldat.key.buf, &buf);
        lp = lp->next;

        while (lp!=NULL && lp->ldat.key.buf!=NULL) {
            if (deli!=NULL) cat_s2Buffer(deli, &buf);
            cat_s2Buffer((char*)lp->ldat.key.buf, &buf);
            lp = lp->next;
        }
    }

    return buf;
}



//////////////////////////////////////////////////////////////////////////
// for Configuration File
//

/**
char*  get_str_param_tList(tList* lt, const char* key, const char* dflt)

リストから keyに対応するデータ部を 文字列として読み出す．キーがない場合は，デフォルト値 dflt を返す．@n
read_index_tList_file() と組み合わせて使用する．@n
要 free()

@param  lt    ファイルから read_index_tList_file() を使用して読み込んだリスト．
@param  key   検索キー
@param  dflt  デフォルト値
@return key に対応するデータ部．key がリストに存在しない場合は，dflt
*/
char*  get_str_param_tList(tList* lt, const char* key, const char* dflt)
{
    Buffer buf;

    buf = buffer_key_tList(lt, key, 1);
    if (buf.buf==NULL) buf = make_Buffer_bystr(dflt);
        
    return (char*)buf.buf;
}


/**
int  get_int_param_tList(tList* lt, const char* key, int dflt)

リストから keyに対応するデータ部を 整数として読み出す．キーがない場合は，デフォルト値 dflt を返す．@n
read_index_tList_file() と組み合わせて使用する．

@param  lt    ファイルから read_index_tList_file() を使用して読み込んだリスト．
@param  key   検索キー
@param  dflt  デフォルト値
@return key に対応するデータ部．key がリストに存在しない場合は，dflt
*/
int  get_int_param_tList(tList* lt, const char* key, int dflt)
{
    Buffer buf;

    buf = buffer_key_tList(lt, key, 1);
    if (buf.buf!=NULL) {
        int ret = atoi((char*)buf.buf);
        free_Buffer(&buf);
        return ret;
    }
    return dflt;
}


/**
double  get_double_param_tList(tList* lt, const char* key, double dflt)

リストから keyに対応するデータ部を double型実数として読み出す．キーがない場合は，デフォルト値 dflt を返す．@n
read_index_tList_file() と組み合わせて使用する．

@param  lt    ファイルから read_index_tList_file() を使用して読み込んだリスト．
@param  key   検索キー
@param  dflt  デフォルト値
@return key に対応するデータ部．key がリストに存在しない場合は，dflt
*/
double  get_double_param_tList(tList* lt, const char* key, double dflt)
{
    Buffer buf;

    buf = buffer_key_tList(lt, key, 1);
    if (buf.buf!=NULL) {
        double ret = atof((char*)buf.buf);
        free_Buffer(&buf);
        return ret;
    }
    return dflt;
}


/**
float  get_float_param_tList(tList* lt, const char* key, float dflt)

リストから keyに対応するデータ部を floate型実数として読み出す．キーがない場合は，デフォルト値 dflt を返す．@n
read_index_tList_file() と組み合わせて使用する．

@param  lt    ファイルから read_index_tList_file() を使用して読み込んだリスト．
@param  key   検索キー
@param  dflt  デフォルト値
@return key に対応するデータ部．key がリストに存在しない場合は，dflt
*/
float  get_float_param_tList(tList* lt, const char* key, float dflt)
{
    Buffer buf;

    buf = buffer_key_tList(lt, key, 1);
    if (buf.buf!=NULL) {
        float ret = (float)atof((char*)buf.buf);
        free_Buffer(&buf);
        return ret;
    }
    return dflt;
}


/**
int  get_bool_param_tList(tList* lt, const char* key, int dflt)

リストから keyに対応するデータ部を 論理値（実際は整数）として読み出す．キーがない場合は，デフォルト値 dflt を返す．@n
read_index_tList_file() と組み合わせて使用する．

@param  lt    ファイルから read_index_tList_file() を使用して読み込んだリスト．
@param  key   検索キー
@param  dflt  デフォルト値
@return key に対応するデータ部．key がリストに存在しない場合は，dflt
*/
int  get_bool_param_tList(tList* lt, const char* key, int dflt)
{
    int  ret = dflt;
    char* val = NULL;

    if (dflt) val = get_str_param_tList(lt, key, "true");
    else      val = get_str_param_tList(lt, key, "false");

    if (val!=NULL) {
        if (!strcasecmp("true", val)) ret = 1;       // TRUE
        else ret = 0;                                // FALSE
        free(val);
    }

    return ret;
}



/////////////////////////////////////////////////////////////////////////////
// Tiny List File I/O
//

/**
tList*  read_tList_file(const char* fname, int mode) 

ファイルから一行ずつ読み込んでリストのキー部に格納．空行はリストに加えない．

@param  fname  ファイル名． 
@param  mode   0: ファイルをそのまま読み込む．
@param  mode   1: 先頭が # の行はリストに加えない．また前後の空白を削除し，その他の連続する空白も1つの空白に変換する．@n
                  さらにタブは一個の空白として処理される．
@param  mode   2: 1の場合に加えて，途中の # 以降も無視する

@return 格納したリストへのポインタ．
*/
tList*  read_tList_file(const char* fname, int mode) 
{
    tList* lp = NULL;
    FILE* fp;

    fp = fopen(fname, "rb");
    if (fp!=NULL) {
        lp = read_tList_fp(fp, mode);
        fclose(fp);
    }

    return lp;
}


/**
tList*  read_tList_fp(FILE* fp, int mode) 

ファイルポインタが示すファイルから一行ずつ読み込んでリストのキー部に格納．@n
空行はリストに加えない．

@param  fp    ファイルポインタ． 
@param  mode  0: ファイルをそのまま読み込む．
@param  mode  1: 先頭が # の行はリストに加えない．また前後の空白を削除し，その他の連続する空白も1つの空白に変換する．@n
                 また，タブは一個の空白として処理される．
@param  mode  2: 1の場合に加えて，途中の # 以降も無視する

@return 格納したリストへのポインタ．

@bug ディレクトリを読ませると，戻ってこない．
*/
tList*  read_tList_fp(FILE* fp, int mode) 
{
    char    val[LBUF+1];
    char*   str;
    tList*  lp = NULL;
    tList*  lt = NULL;

    if (fp==NULL) return NULL;

    str = fgets(val, LBUF, fp);     // str is unused
    while (!feof(fp)) {
        if (mode>0) {
            if (mode>1) {
                int i;
                for (i=0; i<(int)strlen(val); i++) {
                    if (val[i]=='#') {
                        val[i] = '\0';
                        break;
                    }
                    if (i>=LBUF) break;
                }
            }
            str = pack_char(val, ' ');
        }
        else {
            str = (char*)malloc(LBUF+1);
            if (str!=NULL) {
                memset(str, 0, LBUF+1);
                strncpy(val, str, LBUF);
            }
        }

        if (str!=NULL) {
            if (strlen(str)>0) {    // 空行のチェック
                if (mode==0 || str[0]!='#') {
                    lt = add_tList_node_str(lt, str, NULL);
                    if (lp==NULL) lp = lt;
                }
            }
            free(str);
        }
        str = fgets(val, LBUF, fp);     // str is unused
    }
    
    return lp;
}


/**
tList*  read_index_tList_file(const char* fname, char deli) 

ファイルから一行ずつ読み込んで，deliを区切り文字にしてリストのキー部とデータ部に格納．@n
データ部に区切り記号が存在しても良い@n
キー部とデータ部の前後の空白，TAB, CR, LF は削除する．@n
空行はリストに加えない．#で始まる行はリストに加えない

@param  fname  ファイル名． 
@param  deli   区切り文字

@return 格納したリストへのポインタ．
*/
tList*  read_index_tList_file(const char* fname, char deli) 
{
    tList* lp = NULL;
    FILE* fp;

    fp = fopen(fname, "rb");
    if (fp!=NULL) {
        lp = read_index_tList_fp(fp, deli);
        fclose(fp);
    }
    return lp;
}


/**
tList*  read_index_tList_fp(FILE* fp, char deli)

ファイルポインタが示すファイルから一行ずつ読み込んで，deliを区切り文字にしてリストのキー部とデータ部に格納．@n
データ部に区切り記号が存在しても良い@n
キー部とデータ部の前後の空白，TAB, CR, LF は削除する．@n
空行はリストに加えない．#で始まる行はリストに加えない

@param  fp    ファイルポインタ． 
@param  deli  区切り文字

@return 格納したリストへのポインタ．
*/
tList*  read_index_tList_fp(FILE* fp, char deli)
{
    Buffer key, val;
    Buffer fst, snd;
    tList* pl;
    tList* pp;
    tList* lt = NULL;

    pp = pl = read_tList_fp(fp, 1);
    while (pp!=NULL) {
        fst = awk_Buffer(pp->ldat.key, deli, 1);
        //snd = awk_Buffer(pp->ldat.key, deli, 2);
        snd = make_Buffer_bystr((char*)&(pp->ldat.key.buf[fst.vldsz]));
        key = pack_Buffer(fst, ' ');
        val = pack_Buffer(snd, ' ');
        if (lt==NULL) lt = add_tList_node_byBuffer(NULL, 0, 0, key, val, NULL, 0);
        else               add_tList_node_byBuffer(lt,   0, 0, key, val, NULL, 0);
    
        free_Buffer(&key);
        free_Buffer(&val);
        free_Buffer(&fst);
        free_Buffer(&snd);
        
        pp = pp->next;
    }
    del_all_tList(&pl);

    return lt;
}


/**
tList*  read_Buffer_tList_file(const char* fname) 

ファイルから Buffer型変数を 2つずつ読み込んで，リストのキー部とバッファ部に格納し返す．

@param  fname  ファイル名． 
@return 格納したリストへのポインタ．
*/
tList*  read_Buffer_tList_file(const char* fname) 
{
    tList* lp = NULL;
    FILE* fp;

    fp = fopen(fname, "rb");
    if (fp!=NULL) {
        lp = read_Buffer_tList_fp(fp);
        fclose(fp);
    }
    return lp;
}


/**
tList*  read_Buffer_tList_fp(FILE* fp) 

ファイルポインタから Buffer型変数を 2つずつ読み込んで，リストのキー部とバッファ部に格納し返す．

@param  fp  ファイルポインタ． 
@return 格納したリストへのポインタ．
*/
tList*  read_Buffer_tList_fp(FILE* fp) 
{
    int  cc;
    tList*  lp = NULL;
    tList*  lt = NULL;
    Buffer key, val;

    if (fp==NULL) return NULL;

    cc = read_Buffer2_fp(&key, &val, fp);
    while (!feof(fp) && cc) {
        lt = add_tList_node_Buffer(lt, key, val);
        if (lp==NULL) lp = lt;
        free_Buffer(&key);    
        free_Buffer(&val);    
        cc = read_Buffer2_fp(&key, &val, fp);
    }
    
    free_Buffer(&key);    
    free_Buffer(&val);    
    return lp;
}


/**
int  save_Buffer_tList_file(const char* fname, tList* lp) 

リストのキー部とバッファ部の Buffer型変数をファイルへ書き込む．@n
ファイルが既にある場合は，追加書き込みされる．

@param  fname  ファイル名． 
@param  lp     書き込みを行うリストデータへのポインタ．

@retval TRUE   書き込み成功．
@retval FALSE  書き込み失敗
*/
int  save_Buffer_tList_file(const char* fname, tList* lp) 
{
    int  ret=FALSE;
    FILE* fp;

    fp = fopen(fname, "ab");
    if (fp!=NULL) {
        ret = save_Buffer_tList_fp(fp, lp);
        fclose(fp);
    }
    return ret;
}


/**
int  save_Buffer_tList_fp(FILE* fp, tList* lp) 

リストのキー部とバッファ部の Buffer型変数をファイルポンタが指すファイルへ書き込む．@n
ファイルが既にある場合は，追加書き込みされる．

@param  fp     ファイルポインタ． 
@param  lp     書き込みを行うリストデータへのポインタ．

@retval TRUE   書き込み成功．
@retval FALSE  書き込み失敗
*/
int  save_Buffer_tList_fp(FILE* fp, tList* lp) 
{
    int cc=TRUE;

    if (fp==NULL) return FALSE;

    while (lp!=NULL && cc) {
        cc = save_Buffer2_fp(lp->ldat.key, lp->ldat.val, fp);    
        lp = lp->next;
    }

    if (!cc) return FALSE;
    return TRUE;
}

