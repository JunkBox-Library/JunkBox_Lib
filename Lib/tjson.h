#ifndef  __JBXL_TINY_JSON_H_
#define  __JBXL_TINY_JSON_H_

/** 
@brief   Tiny JSON ライブラリヘッダ 
@file    tjson.h
@version 1.2.0
@author  Fumi.Iseki (C)
@date    2021 8/19

@par タイトル
Tiny JSON tjson.h v1.0 @n
サブセット版 JSON 簡易Parser ライブラリヘッダ

@see tJson.cpp

@attention
全てのパターンのパース可能性は保障しない．@n
*/

#include "tools.h"
#include "ttree.h"
#include "jbxl_state.h"


// ノード種別    (ldat.id)
#define  JSON_ANCHOR_NODE    JBXL_STATE_ANCHOR      ///< アンカーノード
#define  JSON_TEMP_NODE             -1              ///< 一時的なノード．削除対象．
#define  JSON_BRACKET_NODE           1              ///< '{' を格納したノード．key は持たない．
#define  JSON_DATA_NODE              2              ///< key:val の形の通常のノード
#define  JSON_ARRAY_NODE             3              ///< 配列ノード．処理された場合，JSON_ARRAY_VALUE_NODE を子ノードとして持つ．
#define  JSON_ARRAY_VALUE_NODE       4              ///< 配列の要素データのノード．

// 属性値の形式  (ldat.lv)
#define  JSON_VALUE_UNRESOLV        -1              ///< 属性値の種類は未確定．
#define  JSON_VALUE_NULL             0              ///< 属性値なし．
#define  JSON_VALUE_INT              1              ///< 属性値：整数  未サポート
#define  JSON_VALUE_REAL             2              ///< 属性値：実数　未サポート
#define  JSON_VALUE_BOOL             3              ///< 属性値：論理値　未サポート
#define  JSON_VALUE_STR              4              ///< 属性値：文字列   
#define  JSON_VALUE_OBJ              5              ///< 属性値：オブジェクト
#define  JSON_VALUE_ARRAY            6              ///< 属性値：配列

#define  JSON_ONELINE_FORMAT         0              ///< 改行なしの一行にする．
#define  JSON_CRLF_FORMAT            1              ///< ノードの終わりを CR(0x0d), LF(0x0a)で改行する．
#define  JSON_INDENT_FORMAT          2              ///< 先頭にインデント(TAB)をつけ，ノードごとに改行する．


/////////////////////////////////////////////////////////////////////////////////////////////
// JSON Functions

#define new_json_node()         new_tTree_node()            ///< JSONデータのノードを生成   new_tTree_node()
#define new_json_anchor_node()  new_tTree_anchor_node()     ///< JSONデータの ANCHORノードを生成   new_tTree_anchor_node()
#define new_json()              new_tTree_node()            ///< JSONデータのノードを生成   new_tTree_node()
#define new_json_anchor()       new_tTree_anchor_node()     ///< JSONデータの ANCHORノードを生成   new_tTree_anchor_node()
#define new_json_anchor_node()  new_tTree_anchor_node()     ///< JSONデータの ANCHORノードを生成   new_tTree_anchor_node()
#define del_json_anchor(t)      del_tTree_anchor_node((t))  ///< JSONデータの ANCHORノードを削除   del_tTree_anchor_node()
#define del_json_anchor_node(t) del_tTree_anchor_node((t))  ///< JSONデータの ANCHORノードを削除   del_tTree_anchor_node()

#define del_json(j)             del_tTree((j))              ///< JSONデータの削除           del_tTree()
#define del_all_json(j)         del_all_tTree((j))          ///< 全JSONデータの削除         del_all_tTree()
#define del_json_node(j)        del_tTree_node((j))         ///< JSONデータのノード削除     del_tTree_node()
#define free_json(j)            free_tTree_node((j))        ///< JSONデータのノードデータを解放     free_tTree_node() 
#define free_json_node(j)       free_tTree_node((j))        ///< JSONデータのノードデータを解放     free_tTree_node() 


/** @typedef tJson
typedef  tTree  tJson;

@b tJson  (@b tList, @b tTree, @b _tList と同じもの)
@see tTree 

1. ノードデータ tJson：
@code
    int     ldat.id     オブジェクトの種別． JSON_ANCHOR_NODE, JSON_BRACKET_NODE, JSON_DATA_NODE, JSON_ARRAY_NODE
    int     ldat.lv     属性値の形式． JSON_VALUE_NULL, JSON_VALUE_UNRESOLV, JSON_VALUE_INT, JSON_VALUE_STR など
    int     ldat.sz     配列処理の回数．
    Buffer  ldat.key    属性の名前
    Buffer  ldat.val    属性の値
    void*   ldat.ptr    未使用 
    tJson*  ldat.lst    未使用

    tJson*  next        子ノード（長女）へのポインタ
    tJson*  prev        親ノードへのポインタ
    tJson*  altp        汎用
    tJson*  yngr        子ノード（末っ子）へのポインタ    

    tJson*  esis        前の姉妹（姉）ノードへのポインタ
    tJson*  ysis        次の姉妹（妹）ノードへのポインタ

    int     depth       深さ                    
    int     num         子ノードの数（全ての子ノードの数）    
    int     ctrl        制御用
    int     state       ノードの状態  JBXL_JSON_DEFAULT_STATE, JBXL_JSON_PARSED, JBXL_JSON_PARSE_TERM, JBXL_JSON_MULTI_ROOT, ....
@endcode

2. パースしたJSON情報を格納するツリー構造
@code
    {"A":{"B":{"C":ABC, "DEFG"}, "E":"X"}} 
    
    (JSON_ANCHOR_NODE)
        -> { (JSON_BRACKET_NODE)
            -> "A":{ (JSON_DATA_NODE, JSON_VALUE_OBJ)
                -> "B":{ (JSON_DATA_NODE, JSON_VALUE_OBJ) 
                    -> "C":ABC (JSON_DATA_NODE, JSON_VALUE_UNRESOLV)
                    -> "DEFG" (JSON_DATA_NODE, JSON_VALUE_NULL)
                -> "E":"X" (JSON_DATA_NODE, JSON_VALUE_STR)
@endcode

3, ノード（ノード）種別 (ldat.id)
@code
    JSON_ANCHOR_NODE            アンカーノード
    JSON_TEMP_NODE              一時的なノード．削除対象．
    JSON_BRACKET_NODE           '{' を格納したノード．key は持たない． 
    JSON_DATA_NODE              key:val の形の通常のノード
    JSON_ARRAY_NODE             配列ノード．処理された場合，JSON_ARRAY_VALUE_NODE を子ノードとして持つ．
    JSON_ARRAY_VALUE_NODE       配列の要素データのノード．
@endcode

4. 属性値(val)の形式（ldat.lv）
@code
    JSON_VALUE_UNRESOLV
    JSON_VALUE_NULL  
    JSON_VALUE_INT  
    JSON_VALUE_REAL 
    JSON_VALUE_BOOL
    JSON_VALUE_STR
    JSON_VALUE_OBJ  
    JSON_VALUE_ARRAY 
@endcode

5. 配列
  配列の要素ノードのノード種別（ldat.id）は JSON_ARRAY_VALUE_NODE, ldat.key.buf は "ARRAY_VALUE" となる．@n
  属性値は 現在 JSON_VALUE_INT, JSON_VALUE_REAL, JSON_VALUE_STR, JSON_VALUE_NULL（ldat.lv）のみサポート．

6. 状態 (state)
@code
    JBXL_JSON_DEFAULT_STATE     ノードは，正常にパースされたかどうかの判定はまだ行われていない．
    JBXL_JSON_PARSED            アンカーノードにのみ設定される．このJSONツリーは正常にパースされたことを示す．
    JBXL_JSON_MULTI_ROOT        アンカーノードにのみ設定される．このJSONツリーは 2つ以上の独立したツリーを持つ
    JBXL_JSON_NODE_OPENED       このノードはオープン状態にある． {
    JBXL_JSON_NODE_CLOSED       このノードは正常にクローズした． }
    JBXL_JSON_NODE_EMPTY        JSONの空のノード
    JBXL_JSON_NOT_CLOSED        最終的にノードが閉じなかったが，解釈できるところまでは解釈した．
    JBXL_JSON_PARSE_TERM        JSON のパースが途中で終了した．入力データが不完全．
    JBXL_JSON_IN_STR            SONデータの属性値処理で文字列を処理中．
    JBXL_JSON_IN_ARRAY          JSONデータの属性値処理で配列を処理中．

    JBXL_JSON_ERROR             tJson ライブラリのエラー
    JBXL_JSON_PARSE_ERROR       解釈エラー
    JBXL_JSON_SEQUENCE_ERROR    アンカーを越えてツリーを作ろうとした．
    JBXL_JSON_MEMORY_ERROR      メモリの確保エラー
@endcode
@sa jbxl_state.h
*/
typedef  tTree  tJson;


/////////////////////////////////////////////////////////////////////////////////////////////
// Parser

tJson*   json_parse(const char* str, int num);                      ///< 文字列のJSONデータを解釈して，tJsonのツリーを生成する．ANCHOR付き．
tJson*   json_parse_prop (tJson* json, const char* str, int num);   ///< JSON Main Parser．json が NULL の場合は ANCHOR付き．
tJson*   json_parse_seq  (tJson* json, const char* str, int num);   ///< 断片化した JSONデータを読み込んで処理する．
tJson*   json_array_parse(tJson* json, const char* str, int num);   ///< JSONデータの 配列ノードの値（配列データ）を処理する．

tJson*   _json_array_parse(tJson* json, int num);                                           ///< json_parse_prop() の補助関数．配列処理用．
tJson*   _json_parse_term (tJson* json, const char* st, const char* ed, const char* com);   ///< json_parse_prop() の補助関数．断片的な入力データ用．


/////////////////////////////////////////////////////////////////////////////////////////////
// 逆 Parser

Buffer   json_inverse_parse(tJson* pp, int mode);                                       ///< tJsonデータをmodeに従って元の書式に戻して Bufferに格納する．
Buffer   json_inverse_parse_opt(tJson* pp, const char* crlf, const char* space);        ///< tJsonデータを指定に従って元の書式に戻して Bufferに格納する．
void     print_json(FILE* fp, tJson* json, int mode);                                   ///< tJsonデータをmodeに従って fp に出力する.
void     print_json_opt(FILE* fp, tJson* json, const char* crlf, const char* space);    ///< tJsonデータを指定に従って fp に出力する.

void     _json_to_Buffer(tJson* pp, Buffer* buf, const char* crlf, const char* space);  ///< tJsonデータを元の書式に戻して Bufferに格納する．補助関数． 

#define  print_tJson(f, j)          print_tTree((f), (j))                               ///< file, json
#define  print_tJson_tree(f, j, s)  print_tTree_tree((f), (j), (s))                     ///< file, json, space


/////////////////////////////////////////////////////////////////////////////////////////////
// Tools

tJson*   json_parse_file(const char* fn, int num);                                      ///< JSONデータをファイルから読み込んで，パースする．

void     json_set_str_val(tJson* json, const char* val);                                ///< json ノード "key":val に文字列の属性値(value)を設定する．
void     json_set_int_val(tJson* json, int val);                                        ///< json ノード "key":val に整数の属性値(value)を設定する．
void     json_set_real_val(tJson* json, float val);                                     ///< json ノード "key":val に実数の属性値(value)を設定する．

void     json_copy_val (tJson* f_json, tJson* t_json);                                  ///< f_json から t_json へ属性値(value)をコピーする．
void     json_copy_data(tJson* f_json, tJson* t_json);                                  ///< f_json から t_json へ属性名(key)と属性値(value)をコピーする．

tJson*   json_insert_child(tJson* parent, tJson* child);                                ///< parent に dict または array の child を繋げる．
tJson*   json_insert_parse(tJson* parent, const char* str);                             ///< str をパースして繋げる．str は { または [ で始まる必要がある．

tJson*   json_append_obj_key(tJson* json, const char* key);                             ///< 属性値(value)なしのリストデータ "key":{} を追加する．
tJson*   json_append_array_key(tJson* json, const char* key);                           ///< 値(value)なしの配列 "key":[] を追加する．

void     json_append_obj_str_val (tJson* json, const char* key, const char* val);       ///< {} の要素として "key":val（val は文字列）を追加する．
void     json_append_obj_int_val (tJson* json, const char* key, int val);               ///< {} の要素として "key":val（val は整数）を追加する．
void     json_append_obj_real_val(tJson* json, const char* key, float val);             ///< {} の要素として "key":val（va lは実数）を追加する．

void     json_append_array_str_val (tJson* json, const char* val);                      ///< 配列 [] の要素として 文字列 val を追加する．
void     json_append_array_int_val (tJson* json, int val);                              ///< 配列 [] の要素として 整数 val を追加する．
void     json_append_array_real_val(tJson* json, float val);                            ///< 配列 [] の要素として 実数 val を追加する．
//
tJson*   join_json(tJson* parent, tJson** child);                                       ///< parent の子として child そのものを 直接繋げる．
#define  dup_merge_json(p, c)               dup_merge_tTree((p), (c))                   ///< p の子として c の複製を繋げる．


/////////////////////////////////////////////////////////////////////////////////////////////
// Search

#define  find_top_json(p, n)                search_top_bracket_json((p), (n))           ///< search_top_bracket_json()
#define  find_sister_json(p, n)             search_sister_json((p), (n))                ///< search_sister_json()
#define  find_key_json(p, k)                search_key_json((p), (k), FALSE, 1)         ///< search_key_json()
#define  find_key_child_json(p, k)          search_key_child_json( (p), (k), FALSE)     ///< search_key_child_json()
#define  find_key_sister_json(p, k)         search_key_sister_json((p), (k), FALSE)     ///< search_key_sister_json()
#define  find_key_json_obj(p, k)            search_key_json_obj((p), (k))               ///< search_key_json_obj()
#define  find_double_key_json(p, k1, k2)    search_double_key_json((p), (k1), (k2), FALSE)  ///< search_doublekey_json()

tJson*   search_top_bracket_json(tJson* pp, int nn);                                    ///< ツリーが複数のルート(TOP)を持つ場合（JBXL_JSON_MULTI_ROOT），指定されたTOPへのポインタを返す．
tJson*   search_sister_json(tJson* pp, int nn);                                         ///< nn個先の sister ノードを返す．正数の場合は younger sister. 負数の場合は elder sister．

tJson*   search_key_json(tJson* pp, const char* key, int needval, int nn);              ///< 名前（属性名）が key である nn番目のノードへのポインタを返す
tJson*   search_key_child_json(tJson* pp, const char* key, int needval);                ///< 子の姉妹ノードで名前（属性名）が key である nn番目のノードへのポインタを返す．      
tJson*   search_key_sister_json(tJson* pp, const char* key, int needval);               ///< 姉妹ノードで名前（属性名）が key である nn番目のノードへのポインタを返す．      
tJson*   search_key_json_obj(tJson* pp, const char* key, int nn);                       ///< 名前（属性名）が key である nn番目のオブジェクトノード（JSON_VALUE_OBJ）へのポインタを返す．ex.) "key":{}
tJson*   search_double_key_json(tJson* pp, const char* key1, const char* key2, int needval);  ///< 属性名が key1 -> key2 の親子関係を持つ，key2ノードのポインタを返す．

tJson*   _search_key_json(tJson* pp, const char* key, int need, int* nn);               ///< search_key_json() の補助関数
tJson*   _search_key_json_obj(tJson* pp, const char* key, int* nn);                     ///< search_key_json_obj() の補助関数
int      _json_check_node_bykey(tJson* pp, const char* key, int needval, int nn);       ///< search 系関数の補助関数

Buffer   get_json_val(tJson* json);
Buffer   get_key_json_val(tJson* pp, const char* key, int nn);                          ///< 名前（属性名）が key である nn番目のノードの属性値を返す．
Buffer   get_key_sister_json_val(tJson* pp, const char* key);                           ///< 姉妹ノードで名前（属性名）が key である nn番目のノードの属性値を返す．
Buffer   get_double_key_json_val(tJson* pp, const char* key1, const char* key2);        ///< key1 -> key2 の親子関係を持つ，key2ノードの属性値を返す．

Buffer   get_Buffer_from_json(tJson* json);
char*    get_string_from_json(tJson* json);                                             ///< 要 free()

//
tList*   search_all_node_strval_json(tJson* json, const char* name, const char* val);   ///< 指定した条件に合う全てのノードへのポインタを，リストに格納して返す．
tList*   _search_all_node_strval_json(tList* list, tJson* pp, const char* name, const char* val);   ///< search_all_node_strval_json() の補助関数   
// ex.) search_all_node_strval_json(json, "ARRAY_VALUE", "jupyterhub-j20000xx");


#endif  // __JBXL_TINY_JSON_H_

