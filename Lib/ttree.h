#ifndef  __JBXL_TINY_TREE_H_
#define  __JBXL_TINY_TREE_H_

/** 
@brief   Tiny Tree Graph 構造ライブラリヘッダ
@file    ttree.h
@version
@author  Fumi.Iseki (C)
@date    2008 2/1
@see     tList (_tList), tTree
*/

#include "buffer.h"
#include "tlist.h"


/** @typedef  tTree;

@code
tTree 構造体
    tList_data  ldat        データ

    tList*      next        子ノードへのポインタ（子供のうちの誰か）
    tList*      prev        親ノードへのポインタ
    tList*      altp        他のノードへのポインタ

    tList*      yngr        子（末っ子）ノードへのポインタ  // back から変更
    tList*      esis        前の姉妹ノードへのポインタ
    tList*      ysis        次の姉妹ノードへのポインタ

    int         depth       深さ                    
    int         num         子ノードの数 
    int         ctrl        制御用         
    int         state       ノードの状態

tList_data 構造体
    int         id;         ノードID
    int         lv;         ノード値（整数）
    Buffer      key;        ノードのキー
    Buffer      val;        ノード値（文字列）
    void*       ptr;        汎用．構造体などへのポインタ．（ptr->X を freeできないので，リストのようなポインタを設定してはいけない）
    int         sz;         *ptr のサイズ
    struct _tList*   lst;   リストデータへのポインタ
@endcode
 */
typedef  tList        tTree;


/// ldat.ctrl リスト制御用
#define    TREE_ANCHOR_NODE  JBXL_STATE_ANCHOR        ///< アンカーノード
#define    TREE_NOCTRL_NODE                  0        ///< 何の制御（制限）も受けていないノード．デフォルト．
#define    TREE_NOCMP_NODE                 100        ///< 比較対照から外すノード．通常は無条件で一致させる．
#define    TREE_NOCMP_COPY_NODE            101        ///< 比較対照から外し，最後にコピー処理を行うノード．通常は無条件で一致させる．
#define    TREE_COPY_NODE                  102        ///< 後でコピー処理を行うノード．copy_tTree_byctrl()など．
#define    TREE_NOSIS_NODE                 103        ///< このノードの姉妹ノードは処理しない．一部の関数のみ有効．
#define    TREE_ALREADY_FOUND_NODE         110        ///< 検索などにおいて既に見つけたノード．見つけたことを確定したノード．
#define    TREE_ALREADY_FOUND_NODE_TEMP    111        ///< 一時的に比較対照から外す場合にノード．作業中に設定．


////////////////////////////////////////////////////////////////////////////////////////////////////
// Tiny Tree Graph Node

tTree*  new_tTree_node(void);                           ///< ツリー用の空ノードを動的に生成．
tTree*  new_tTree_anchor_node(void);                    ///< ツリー用の ANCHORノードを動的に生成．
tTree   make_tTree_node(tList_data data);               ///< ツリー用ノードを静的に作成．
tTree*  del_tTree_anchor_node(tTree* node);             ///< ANCHORノードを削除して，TOP（長女）へのポインターを返す．
#define del_tTree_anchor(t) del_tTree_anchor_node((t))  ///< ANCHORノードを削除して，TOP（長女）へのポインターを返す．
#define new_tTree()         new_tTree_node()            ///< new_tTree_node()
#define new_tTree_anchor()  new_tTree_anchor_node()     ///< new_tTree_anchor_node()

tTree*  add_tTree_node         (tTree* pp, tTree* pt);          ///< ツリー ppへノード nodeを末っ子として追加．
tTree*  add_tTree_node_bydata  (tTree* pp, tList_data ldat);    ///< データから Treeノードをつくり出し,それを ppの子ノード（末っ子）として追加．
tTree*  add_tTree_node_bystr   (tTree* pp, int id, int lv, const char*  key, const char*  val, void* ptr, int sz);  ///< ノードを末っ子としてリストに追加．
tTree*  add_tTree_node_byBuffer(tTree* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz);          ///< ノードを末っ子としてリストに追加．

#define add_tTree_node_int(p, k, v)         add_tTree_node_bystr((p), (k), (v), NULL, NULL, NULL, 0)            ///< add_tTree_node_bystr()
#define add_tTree_node_str(p, k, v)         add_tTree_node_bystr((p), 0, 0, (char*)(k), (char*)(v), NULL, 0)    ///< add_tTree_node_bystr()
#define add_tTree_node_Buffer(p, k, v)      add_tTree_node_byBuffer((p), 0, 0, (k), (v), NULL, 0)               ///< add_tTree_node_byBuffer()

tTree*  insert_tTree_node         (tTree* pp, tTree* pt);       ///< ツリー ppへノード nodeを長子として追加．
tTree*  insert_tTree_node_bydata  (tTree* pp, tList_data ldat); ///< ノードをつくり出し,それを ppの子ノード（長子）として追加．
tTree*  insert_tTree_node_bystr   (tTree* pp, int id, int lv, const char*  key, const char*  val, void* ptr, int sz); ///< ノードを長子としてリストに追加．
tTree*  insert_tTree_node_byBuffer(tTree* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz);       ///< ノードを長子としてリストに追加．

#define insert_tTree_node_int(p, k, v)      insert_tTree_node_bystr((p), (k), (v), NULL, NULL, NULL, 0)         ///< insert_tTree_node_bystr()
#define insert_tTree_node_str(p, k, v)      insert_tTree_node_bystr((p), 0, 0, (char*)(k), (char*)(v), NULL, 0) ///< insert_tTree_node_bystr()
#define insert_tTree_node_Buffer(p, k, v)   insert_tTree_node_byBuffer((p), 0, 0, (k), (v), NULL, 0)            ///< insert_tTree_node_byBuffer()

tTree*  move_tTree_node(tTree* node, tTree* pp);                                        ///< nodeを現在のツリーから切り離し，ppへ移動する．
int     replace_all_tTree_node(tTree* pp, char* key, char* src, char* dst, int len);    ///< 対象の全てのノードのノード値を dst に書き換える．
tTree*  del_tTree_node(tTree** node); ///< ツリーノードの削除．削除されたノードが子ノードを持つ場合は，その子ノードを格上げする（木構造を詰める）
tTree*  free_tTree_node(tTree* node); ///< ツリーノードの解放．解放されたノードが子ノードを持つ場合は，その子ノードを格上げする（木構造を詰める）

#define set_tTree_node_bydata(p, k)                  set_tList_node_bydata((p), (k))                            ///< set_tList_node_bydata()
#define set_tTree_node_bystr(p, i, l, k, v, d, s)    set_tList_node_bystr((p), (i), (l), (k), (v), (d), (s))    ///< set_tList_node_bystr()   
#define set_tTree_node_byBuffer(p, i, l, k, v, d, s) set_tList_node_byBuffer((p), (i), (l), (k), (v), (d), (s)) ///< set_tList_node_byBuffer()

#define set_tTree_node_int(p, k, d)          set_tList_node_bystr((p), (k), (v), NULL, NULL, NULL, 0)           ///< set_tList_node_bystr()
#define set_tTree_node_str(p, k, d)          set_tList_node_bystr((p), 0, 0, (char*)(k), (char*)(v), NULL, 0)   ///< set_tList_node_bystr()
#define set_tTree_node_Buffer(p, k, d)       set_tList_node_byBuffer((p), 0, 0, (k), (v), NULL, 0)              ///< set_tList_node_byBuffer(

#define dup_tTree_node(p)                    dup_tList_node(p)                                                  ///< dup_tList_node() 


////////////////////////////////////////////////////////////////////////////////////////////////////
// Tiny Tree Graph

tTree*  del_tTree(tTree** pp);                  ///< 指定したノード以下のツリーを削除する．
void    del_all_tTree(tTree** pp);              ///< ツリーの全ノードの削除．ポインタ ppのノードを含むツリー全体を削除する．
tTree*  del_children_tTree(tTree** pp);         ///< 指定したノードの子ツリーを削除する．指定したノードは削除しない．
tTree*  del_sisters_tTree(tTree** pp);          ///< 指定したノード以下のXMLツリー（ppの姉妹を含む）を削除する．
tTree*  del_sisters_children_tTree(tTree** pp); ///< 指定したノードの姉妹ツリー，子ツリーを削除する．

void    adjust_tTree_depth(tTree* pp);          ///< 指定したノード ppを基準にして，木の深さを測り直す
void    print_tTree(FILE* fp, tTree* pp);       ///< ツリーの表示．ポインタ ノードのキー部のバッファをfpに表示する．
void    print_tTree_tree(FILE* fp, tTree* pp, const char* sp);   ///< ツリーの表示．ポインタ ノードのキー部のバッファをfpに表示する．

tTree*  add_tTree(tTree* pp, tTree* pt);        ///< ツリー tpへ ツリー ttを追加．
tTree*  div_tTree(tTree* pp);                   ///< ツリー tp から ツリー ttを分離する．
tTree*  dup_merge_tTree(tTree* pp, tTree* tp);  ///< ツリー ppの直下にツリー tpを複製する．
void    merge_tTree(tTree* tp, tTree* tt);      ///< ツリー tp にツリー tt を結合する．結合後，tt の内容は壊れる(tpとノードを交換した形になる)．
void    exchange_tTree(tTree* tl, tTree* tt);   ///< ツリー tlと ツリー ttを交換する．
int     count_tTree(tTree* pp);                 ///< ツリーの ppノード以降のノードの数を数える．

/**
tList*  find_tTree_top(tList* p)
ツリーのトップ（ルート）を見つける
@param p 検索するツリーの一部へのポインタ
*/
#define find_tTree_top(p)   find_tList_top((p)) ///< find_tList_top()
tTree*  find_tTree_end(tTree* pp);              ///< ツリーの最終ノードを見つける．

// 検索/置換
tTree*  strncmp_tTree    (tTree* pp, const char*  key, int len, int no);///< ツリーノードのキー値のサーチ
tTree*  strncasecmp_tTree(tTree* pp, const char*  key, int len, int no);///< ツリーノードのキー値のサーチ．大文字小文字を無視する．

int     find_match_tTree  (tTree* pp, tTree* pt);                       ///< ツリー pp内で ツリー ptと同じパターンの枝を探す．
tList*  find_match_tTree_endlist(tTree* pp, tTree* pt);                 ///< pp内で ptと同じパターンの枝を全て探して，その枝の最後のノードへの情報を返す．
tList*  find_match_tTree_endlist_rcsv(tTree* pp, tTree* pt, tTree* te); ///< find_match_tTree_endlist() の補助関数

int     check_match_tTree (tTree* tp, tTree* tr);   ///< tpが trと同じパターン(キー値)を持っているかどうかを検査する．
tTree*  cmp_sisters_tTree (tTree* tp, tTree* tr);   ///< tpの姉妹ノードが trの姉妹ノードと同じキー値を持っているかどうかを検査する．

int     replace_tTree_node(tTree* pp, tTree* pt);   ///< 同じパターンの枝を検索し，ptのノードの属性で置き換える．
void    copy_tTree_byctrl (tTree* pt);              ///< 同じパターンの枝を検索し，ptのノードの属性をコピーする．
void    clear_tTree_ctrl  (tTree* pp);              ///< ppツリーの ctrlをクリアする．

Buffer  get_value_tTree  (tTree* pp, tTree* pt);    ///< 同じパターンの枝を検索し，一致した枝があれば，その枝の最後のノードの値を返す．
#define set_value_tTree(p, t)    replace_tTree_node((p), (t))   ///< replace_tTree_node()

// 補助的関数
tTree*  next_strncmp_vertical_tTree    (tTree* pp, const char* key, int len, int no, int* nn);  ///< tTree 検索用補助関数．vertical は縦方向探索
tTree*  next_strncasecmp_vertical_tTree(tTree* pp, const char* key, int len, int no, int* nn);  ///< tTree 検索用補助関数．vertical は縦方向探索

tTree*  next_strncmp_horizon_tTree    (tTree* pp, const char* key, int len, int no, int* nn);   ///< tTree 検索用補助関数．horizon は擬似的な横方向探索
tTree*  next_strncasecmp_horizon_tTree(tTree* pp, const char* key, int len, int no, int* nn);   ///< tTree 検索用補助関数．horizon は擬似的な横方向探索


#endif  // __JBXL_TINY_TREE_H_

