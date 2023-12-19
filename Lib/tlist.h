#ifndef  __JBXL_TINY_LIST_H_
#define  __JBXL_TINY_LIST_H_

/** 
@brief   Tiny List 構造ライブラリヘッダ
@file    tlist.h
@version
@author  Fumi.Iseki (C)
@date    2008 2/1
@see     tList (_tList)
*/

#include "buffer.h"


struct  _tList_data;
struct  _tList;


struct  _tList_data {
    int                 id;      ///< ノードID
    int                 lv;      ///< ノード値（整数）
    Buffer              key;     ///< ノードのキー
    Buffer              val;     ///< ノード値（文字列）
    void*               ptr;     ///< 汎用（構造体など）
    int                 sz;      ///< *ptrのサイズ
    struct _tList*      lst;     ///< リストデータへのポインタ
};


/** @struct  tList_data
@code
tList_data 構造体
    int                 id;      ノードID
    int                 lv;      ノード値（整数）
    Buffer              key;     ノードのキー
    Buffer              val;     ノード値（文字列）
    void*               ptr;     汎用．構造体などへのポインタ．（ptr->X を freeできないのでリストのようなポインタを設定してはいけない）
    int                 sz;      *ptr のサイズ
    struct _tList*      lst;     リストデータへのポインタ
@endcode
*/
typedef  struct _tList_data tList_data;


struct  _tList {
    struct _tList_data  ldat;    ///< データ

    struct _tList*      next;    ///< 子ノードへのポインタ（通常は yngr）
    struct _tList*      prev;    ///< 親ノードへのポインタ
    struct _tList*      altp;    ///< 他のノードへのポインタ
    struct _tList*      yngr;    ///< 子（末っ子）ノードへのポインタ  for tTree

    struct _tList*      esis;    ///< 前の姉妹ノードへのポインタ      for tTree
    struct _tList*      ysis;    ///< 次の姉妹ノードへのポインタ      for tTree

    int                 depth;   ///< 深さ                            for tTree
    int                 num;     ///< 子ノードの数                    for tTree
    int                 ctrl;    ///< 制御用
    int                 state;   ///< ノードの状態
};


/** @struct tList
@code
tList 構造体
    tList_data  ldat    データ

    tList*      next    子ノードへのポインタ
    tList*      prev    親ノードへのポインタ
    tList*      altp    他のノードへのポインタ
    tList*      yngr    子（末っ子）ノードへのポインタ  for tTree

    tList*      esis    前の姉妹ノードへのポインタ      for tTree
    tList*      ysis    次の姉妹ノードへのポインタ      for tTree

    int         depth   深さ                            for tTree
    int         num     子ノードの数                    for tTree
    int         ctrl    制御用                          for tTree
    int         state   ノードの状態
@endcode
*/
typedef  struct _tList  tList;


// Anchor Node
#define  TLIST_ANCHOR_NODE      JBXL_STATE_ANCHOR   ///< アンカーノード


// for strncmp_tList()
#define  TLIST_MATCH_COMPLETE    0          ///< 完全一致
#define  TLIST_MATCH_TLISTKEY   -1          ///< リストのキーの長さに合わせる
#define  TLIST_MATCH_STRINGKEY  -2          ///< 比較する文字列の長さに合わせる
#define  TLIST_MATCH_STRKEY     -2          ///< 比較する文字列の長さに合わせる


////////////////////////////////////////////////////////////////////////////////////////////////////
// List Data

tList_data  init_tList_data(void);      ///< 空のノードデータを静的に作成．データを初期化するのに使用する．

tList_data  make_tList_data      (int id, int lv, Buffer key, Buffer val, void* ptr, int sz);               ///< データを指定してノードデータを作成
tList_data  make_tList_data_bystr(int id, int lv, const char*  key, const char*  val, void* ptr, int sz);   ///< 文字列データを指定してノードデータを作成
#define     make_tList_data_byBuffer(i, l, k, v, d, s)  make_tList_data((i), (l), (k), (v), (d), (s))       ///< make_tList_data()
#define     make_tList_data_int(k, v)       make_tList_data_bystr((k), (v), NULL, NULL, NULL, 0)            ///< make_tList_data_bystr()
#define     make_tList_data_str(k, v)       make_tList_data_bystr(0, 0, (char*)(k), (char*)(v), NULL, 0)    ///< make_tList_data_bystr()
#define     make_tList_data_Buffer(k, v)    make_tList_data(0, 0, (k), (v), NULL, 0)                        ///< make_tList_data()

void        free_tList_data(tList_data* ldat);      ///< ノードデータのバッファ部をクリアする．データ自身は削除しない．
void        del_tList_data(tList_data** ldat);      ///< ノードデータを削除する．
tList_data  dup_tList_data(tList_data ldat);        ///< ノードデータの複製を作成する


////////////////////////////////////////////////////////////////////////////////////////////////////
// Tiny List Node

tList*  new_tList_node(void);                       ///< リスト用の空ノードを動的に生成する．
tList   make_tList_node(tList_data data);           ///< リスト用ノードを静的に作成する．
tList*  del_tList_node(tList** node);               ///< リスト用のノードを削除．
tList*  free_tList_node(tList* node);               ///< リスト用ノードのバッファ部(データ)の解放．
tList*  dup_tList_node (tList* node);               ///< ノードデータを複製する (new)．ノードのポインタは複製しない．
tList*  move_tList_node(tList* pp, tList* node);    ///< nodeを現在のリストから切り離し，ppへ移動する．
tList*  swap_tList_node(tList* pp1, tList* pp2);    ///< pp1ノード と pp2ノードを入れ替える．

tList*  new_tList_anchor_node(void);                ///< リスト用の ANCHORノードを動的に生成．
tList*  del_tList_anchor_node(tList* node);         ///< ANCHORノードを削除して，TOPのポインターを返す．
#define new_tList_anchor()       add_tList_node_anchor()    ///< new_tList_anchor_node()
#define del_tList_anchor(t)      del_tList_anchor_node((t)) ///< del_tList_abchor_node()


// 追加は new_tList_node() を使用
tList*  add_tList_node_bydata  (tList* pp, tList_data ldat);        ///< データ(ldat)からリスト用ノードを生成し(new),それを指定したリストの後ろに追加．
tList*  add_tList_node_bystr   (tList* pp, int id, int lv, const char* key, const char* val, void* ptr, int sz);    ///< 文字列データからリスト用ノードを生成し(new),それをリストに追加．
tList*  add_tList_node_byBuffer(tList* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz);              ///< Buffer データからリスト用ノードを生成し(new),それをリストに追加．
#define add_tList_node_anchor()            add_tList_node_bystr(NULL, TLIST_ANCHOR_NODE, 0, NULL, NULL, NULL, 0)    ///< add_tList_node_bystr()
#define add_tList_node_int(p, k, v)        add_tList_node_bystr((p), (k), (v), NULL, NULL, NULL, 0)                 ///< add_tList_node_bystr()
#define add_tList_node_str(p, k, v)        add_tList_node_bystr((p), 0, 0, (char*)(k), (char*)(v), NULL, 0)         ///< add_tList_node_bystr()
#define add_tList_node_null(p)             add_tList_node_bystr((p), 0, 0, NULL, NULL, NULL, 0)                     ///< add_tList_node_bystr()
#define add_tList_node_Buffer(p, k, v)     add_tList_node_byBuffer((p), 0, 0, (k), (v), NULL, 0)                    ///< add_tList_node_byBuffer()

// 設定
void    set_tList_node_bydata  (tList* pp, tList_data ldat);        ///< リストのノードに値を設定する．
void    set_tList_node_bystr   (tList* pp, int id, int lv, const char*  key, const char*  val, void* ptr, int sz);  ///< 文字列データからリストのノードに値を設定する．それぞれのデータは複製されて設定される．
void    set_tList_node_byBuffer(tList* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz);              ///< リストのノードに値を設定する．それぞれのデータは複製されて設定される．
#define set_tList_node_int(p, k, v)        set_tList_node_bystr((p), (k), (v), NULL, NULL, NULL, 0)                 ///< set_tList_node_bystr()
#define set_tList_node_str(p, k, v)        set_tList_node_bystr((p), 0, 0, (char*)(k), (char*)(v), NULL, 0)         ///< set_tList_node_bystr()
#define set_tList_node_Buffer(p, k, v)     set_tList_node_byBuffer((p), 0, 0, (k), (v), NULL, 0)                    ///< set_tList_node_byBuffer()

// 検索して更新．無ければ追加．（追加は new_tList_node()を使用）
tList*  update_tList_node       (tList* pp, tList* pt);           ///< pt->keyで検索し，ノードがあればノードをコピーする．無ければ最後にノードを追加する(new)．
tList*  update_tList_node_bydata(tList* pp, char* key, tList_data ldat);  ///< keyで検索し，ノードがあればノードをコピーする．無ければ最後にノードを追加する(new)．
tList*  update_tList_node_bystr(tList* pp, int id, int lv, const char*  key, const char*  val, void* ptr, int sz);  ///< keyで検索し，ノードがあればノードをコピーする．無ければ最後に追加する(new)
tList*  update_tList_node_byBuffer(tList* pp, int id, int lv, Buffer key, Buffer val, void* ptr, int sz);       ///< key.bufで検索し，ノードがあれば設定する．無ければ最後に追加する(new)．
#define update_tList_node_int(p, k, v)     update_tList_node_bystr((p), (k), (v), NULL, NULL, NULL, 0)          ///< update_tList_node_bystr()
#define update_tList_node_str(p, k, v)     update_tList_node_bystr((p), 0, 0, (char*)(k), (char*)(v), NULL, 0)  ///< update_tList_node_bystr()
#define update_tList_node_Buffer(p, k, v)  update_tList_node_byBuffer((p), 0, 0, (k), (v), NULL, 0)             ///< update_tList_node_byBuffer()


////////////////////////////////////////////////////////////////////////////////////////////////////
// Tiny List

tList*  del_tList(tList** pp);                                          ///< 指定したリストノード以降のリストを削除．
void    del_all_tList(tList** pp);                                      ///< リストの全ノードの削除．ポインタ ppのノードを含むリスト全体を削除する．
int     del_tList_key(tList** pp, const char* key, int no);             ///< plからサーチして，no番目の keyのノードを削除する．

tList*  add_tList_end(tList* pp, tList* pt);                            ///< リストppの最後に リストptを追加する．
tList*  insert_tList(tList* pp, tList* pt);                             ///< ノードppの直ぐ後ろに ptを挿入する．
tList*  dup_tList(tList* pp);                                           ///< リストを複製する．
void    print_tList(FILE* fp, tList* pp);                               ///< リストの表示．ポインタ pp以降の全てのノードを fpに表示する．
void    dump_tList(FILE* fp, tList* pp);                                ///< val.buf を 16進ダンプする．他のデータは通常通り表示する．
int     count_tList(tList* pp);                                         ///< リストの ppノード以降のノードの数を数える．

#define add_tList(p, t)                 insert_tList((p), (t))          ///< insert_tList()
#define add_tList_node(p, t)            insert_tList((p), (t))          ///< insert_tList()
#define insert_tList_node(p, t)         insert_tList((p), (t))          ///< insert_tList()
#define new_tList()   	                new_tList_node()                ///< new_tList_node()

tList*  find_tList_top(tList* pp);                                      ///< リストの最初のノードを探す．
tList*  find_tList_end(tList* pp);                                      ///< リストの最後のノードを探す．

// 検索  リストへのポインタを返す 
//       len: 1以上:一致させる長さ，0:完全一致，-1:pl->keyの長さに合わせる，-2:keyの長さに合わせる
tList*  strncmp_tList   (tList* pl, const char*  key, int len, int no);             ///< char* 型変数によるノードのサーチ．
tList*  strnrvscmp_tList(tList* pl, const char*  key, int len, int no);             ///< char* 型変数によるノードのサーチ．文字の後方から比べる
tList*  strstr_tList    (tList* pl, const char*  key, int len, int no);             ///< char* 型変数によるノードの部分文字列サーチ．

tList*  strncasecmp_tList   (tList* pl, const char*  key, int len, int no);         ///< char* 型変数によるノードのサーチ．大文字小文字を無視する
tList*  strncaservscmp_tList(tList* pl, const char*  key, int len, int no);         ///< char* 型変数によるノードのサーチ．文字の後方から比べる．大文字小文字を無視．
tList*  strstrcase_tList    (tList* pl, const char*  key, int len, int no);         ///< char* 型変数によるノードの部分文字列サーチ．大文字小文字を無視．

tList*  strncmp_back_tList   (tList* pl, const char*  key, int len, int no);        ///< char* 型変数によるリストの後方からのノードのサーチ．
tList*  strnrvscmp_back_tList(tList* pl, const char*  key, int len, int no);        ///< char* 型変数によるリストの後方からのノードのサーチ．文字も後方から比べる．
tList*  strstr_back_tList    (tList* pl, const char*  key, int len, int no);        ///< char* 型変数によるリストの後方からのノードの部分文字列サーチ．

tList*  strncasecmp_back_tList   (tList* pl, const char*  key, int len, int no);    ///< char* 型変数によるリストの後方からのノードのサーチ．大文字小文字を無視．
tList*  strncaservscmp_back_tList(tList* pl, const char*  key, int len, int no);    ///< char* 型変数によるリストの後方からのノードのサーチ．文字も後方から比べる．大文字小文字を無視．
tList*  strstrcase_back_tList    (tList* pl, const char*  key, int len, int no);    ///< char* 型変数による後方からのノードの部分文字列サーチ．大文字小文字を無視．

// 検索
tList*  search_id_tList (tList* lt, int id, int no);                                ///< リストの中から no番目の idノード(ldat.id)を探し出し，tListのポインタを返す．
tList*  search_key_tList(tList* lt, const char* key, int no);                       ///< リストの中から no番目の keyノード(ldat.key)を探し出し，tListへのポインタを返す．大文字小文字を無視．

// 検索　ldat.val のコピーを返す 
Buffer  buffer_key_tList(tList* lt, const char* key, int no);                       ///< リストの中から no番目の keyノード(ldat.key)を探し出し，ldat.valのコピーを返す．
Buffer  buffer_key_value_tList(tList* lt, const char* key, char* data, int no);     ///< リストの中から no番目の keyノードを探し出し，ノード値が data で始まる場合，そのノード値を返す．

// Tools
tList*  awk_tList (char* str, char cc);                                 ///< 文字列を区切り文字で区切って，各項目をリストのキー部に入れて返す．
tList*  cawk_tList(char* str, char cc);                                 ///< 文字列を区切り文字で区切って，各項目をリストのキー部に入れて返す．連続文字列対応．
tList*  awk_Buffer_tList (Buffer buf, char cc);                         ///< Buffer 中の文字列を区切り文字で区切って，各項目をリストのキー部に入れて返す．
tList*  cawk_Buffer_tList(Buffer buf, char cc);                         ///< Buffer 中の文字列を区切り文字で区切って，各項目をリストのキー部に入れて返す．連続文字列対応．
char*   get_str_join_tList(tList* lp, const char* deli);                ///< リストの一連のキー部を deliを区切りにして結合して，その文字列を返す． 要 free
Buffer  get_Buffer_join_tList(tList* lp, const char* deli);             ///< リストの一連のキー部を deliを区切りにして結合して返す．

// 値変更
int     set_value_tList(tList* lt, const char* key, int no, const char* value, int mode);           ///< リスト(lt)中の no番目の keyノードの値に valueを設定する．
int     replace_value_tList(tList* lt, const char* key, int no, const char* srcval, char* value);   ///< リスト(lt)中の no番目の keyノードの値の srcvalの部分を value に置き換える．

int     set_value_tList_node(tList* lt, const char* value);                                         ///< lt->ldat.val に文字列を設定する（置き換える）．
int     replace_value_tList_node(tList* lt, const char* srcval, const char* value);                 ///< lt->ldat.val のsrcval部分を  value に置き換える．

// for Configuration File
char*   get_str_param_tList(tList* lt, const char* key, const char* dflt);  ///< keyに対応する値を 文字列として読み出す．キーがない場合は，デフォルト値 dflt を返す．要 free()．
int     get_int_param_tList(tList* lt, const char* key, int dflt);          ///< keyに対応する値を 整数として読み出す．キーがない場合は，デフォルト値 dflt を返す．．
double  get_double_param_tList(tList* lt, const char* key, double dflt);    ///< keyに対応する値を double型実数として読み出す．キーがない場合は，デフォルト値 dflt を返す．
float   get_float_param_tList (tList* lt, const char* key, float dflt);     ///< keyに対応する値を float型実数として読み出す．キーがない場合は，デフォルト値 dflt を返す．
int     get_bool_param_tList  (tList* lt, const char* key, int dflt);       ///< keyに対応する値を 論理値（実際は整数）として読み出す．キーがない場合は，デフォルト値 dflt を返す．

#define get_strparam_tList(l, k, d)    get_str_param_tList((l), (k), (d))   ///< get_str_param_tList()
#define get_intparam_tList(l, k, d)    get_int_param_tList((l), (k), (d))   ///< get_intparam_tList()
#define get_doubleparam_tList(l, k, d) get_double_param_tList((l), (k), (d))///< get_doubleparam_tList()
#define get_boolparam_tList(l, k, d)   get_bool_param_tList((l), (k), (d))  ///< get_boolparam_tList()


////////////////////////////////////////////////////////////////////////////////////////////////////
// File I/O with Tiny List

tList*  read_tList_fp(FILE* fp, int mode);                  ///< ファイルポインタが示すファイルから一行ずつ読み込んでリストのキー部に格納．
tList*  read_tList_file(const char* fn, int mode);          ///< ファイルから一行ずつ読み込んでリストのキー部に格納．空行はリストに加えない．
tList*  read_index_tList_fp(FILE* fp, char deli);           ///< ファイルポインタが示すファイルから一行ずつ読み込んで，deliを区切り文字にしてリストのキー部とデータ部に格納．
tList*  read_index_tList_file(const char* fn, char deli);   ///< ファイルから一行ずつ読み込んで，deliを区切り文字にしてリストのキー部とデータ部に格納．

tList*  read_Buffer_tList_fp(FILE* fp);                     ///< ファイルポインタから Buffer型変数を 2つずつ読み込んで，リストのキー部とバッファ部に格納し返す．
tList*  read_Buffer_tList_file(const char* fn);             ///< ファイルから Buffer型変数を 2つずつ読み込んで，リストのキー部とバッファ部に格納し返す．
int     save_Buffer_tList_fp(FILE* fp, tList* lt);          ///< リストのキー部とバッファ部の Buffer型変数をファイルポンタが指すファイルへ書き込む．
int     save_Buffer_tList_file(const char* fn, tList* lt);  ///< リストのキー部とバッファ部の Buffer型変数をファイルへ書き込む．


#endif  // __JBXL_TINY_LIST_H_

