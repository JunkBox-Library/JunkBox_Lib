#ifndef  __JBXL_PROTOCOL_H_
#define  __JBXL_PROTOCOL_H_

/**
@brief   プロトコル解析ライブラリ ヘッダ
@file    protocol.h
@author  Fumi.Iseki (C)
@date    2009 2/5


Example)
@code
    tList* lp  = get_protocol_header_list(buf, ':', TRUE, TRUE);
    Buffer cnt = restore_protocol_contents(lp);
    .........
    .........
    .........
    set_protocol_contents(lp, cnt);
    Buffer snd = restore_protocol_header(lp, ": ", ON, NULL);

    free_Buffer(&cnt);
    del_tList(&lp);
@endcode
*/


#include "xtools.h"


// Header tList のキー
#define HDLIST_FIRST_LINE_KEY       "FIRST_LINE"
#define HDLIST_CONTENTS_KEY         "CONTENTS_DATA"
#define HDLIST_CONTINUE             "HEADER_CONTINUE"
#define HDLIST_END_KEY              "HEADER_END"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Protocol Header

tList*  get_protocol_header_list(Buffer buf, char deli, int fstline, int rcntnt);
Buffer  restore_protocol_header (tList* pp, char* deli, int mode, int* hdsz);

Buffer  restore_protocol_contents(tList* pp);
void    set_protocol_contents(tList* pp, Buffer cnt);

tList*  get_protocol_header_list_seq(tList* list, Buffer buf, char deli, int fstline, int rcntnt);
tList*  get_protocol_header_list_file(char* fname, char deli, int fstline, int rcntnt);

// ヘッダデータの作成
#define create_protocol_header(p, k, v)                     add_tList_node_bystr((p), 0, 0, (char*)(k), (char*)(v), NULL, 0)
#define add_protocol_header(p, k, v)                        add_tList_node_bystr((p), 0, 0, (char*)(k), (char*)(v), NULL, 0)
#define create_protocol_header_Buffer(p, k, v)              add_tList_node_byBuffer((p), 0, 0, (k), (v), NULL, 0)
#define add_protocol_header_Buffer(p, k, v)                 add_tList_node_byBuffer((p), 0, 0, (k), (v), NULL, 0)
#define delete_protocol_header(p, k, n)                     del_tList_key((p), (k), (n))

// 検索    ldat.valのコピーを返す
Buffer  search_protocol_header(tList* list, char* key, int no);
Buffer  search_protocol_header_item (tList* list, char* key, int no, char deli, int nm);
Buffer  search_protocol_header_value(tList* list, char* key, char* data, int no);
Buffer  search_protocol_header_partvalue(tList* list, char* key, char* data, int no);

#define get_protocol_header(l, k, n)                        search_protocol_header((l), (k), (n))
#define get_protocol_header_item(l, k, n, d, m)             search_protocol_header_item((l), (k), (n), (d), (m))
#define get_protocol_header_value(l, k, d, n)               search_protocol_header_value((l), (k), (d), (n)) 
#define get_protocol_header_partvalue(l, k, d, n)           search_protocol_header_partvalue((l), (k), (d), (n))

// 設定，置き換え
//int     set_protocol_header(tList* list, char* key, int no, char* value, int mode);
int     set_protocol_header(tList* list, char* key, char* value, int no, int mode);

#define set_protocol_record_item(l, d, m, v)                set_item_tList_node((l), (d), (m), (v))
#define replace_protocol_record_item(l, d, m, s, v)         replace_item_tList_node((l), (d), (m), (s), (v))


/**
int     replace_protocol_header(tList* list, char* key, int no, char* srcval, char* value)

リスト(lt)中の no番目の keyノードの値の srcvalの部分を value に置き換える．
no が 0以下の場合は，全ての keyノードの値に対して置き換えが行われる．@n
replace_valute_tList() と同じ関数 (in Lib/tlist.c)
@param  list    処理対象のリスト
@param  key     置き換えを行うノードのキー部
@param  no      何個目の項目を置き換えるか．1から数える．0以下の場合はkeyが一致する全ての項目を置き換える
@param  srcval  置き換え対象の文字列．
@param  value   置き換えを行う文字列．

@retval 0以上   変更されたノードの数．指定されたノードが存在しないは（追加された場合も）0
@retval 負数    エラー．
*/
#define replace_protocol_header(list, key, no, srcval, value)   replace_value_tList((list), (key), (no), (srcval), (value))


/**
int     replace_protocol_header_item(tList* list, char* key, int no, char deli, int nm, char* srcval, char* value)

key をキーにした no番目のノードの値の中で，deliを区切りにした nm番目の項目（文字列）の srcval部分を valueで置き換える．
@param  list    検索対象のヘッダ方向を格納したリスト
@param  key     ヘッダ種別．
@param  no      同じヘッダ種別が複数ある場合，何番目のノードかを指定する．1から数える．
                noが 0以下の場合は keyが一致する全てのノードに対して操作を行う．
@param  deli    ノード値（文字列）の区切り文字．
@param  nm      deli を区切り文字として何番目の項目か？ 1から数える．
@param  srcval  置き換え対象の文字列．NULLなら指定した項目の文字列全体
@param  value   置き換える文字列．
@return 置き換えを行ったノード数．
*/
#define replace_protocol_header_item(list, key, no, deli, nm, srcval, value)  replace_item_tList((list), (key), (no), (deli), (nm), (srcval), (value))


/**
int     set_protocol_header_item(tList* list, char* key, int no, char deli, int nm, char* value)

key をキーにした no番目のノードの値の中で，deliを区切りにした nm番目の項目（文字列）にvalue を設定する．
@param  list   検索対象のヘッダ方向を格納したリスト
@param  key    ヘッダ種別．
@param  no     同じヘッダ種別が複数ある場合，何番目のノードかを指定する．1から数える．
               noが 0以下の場合は keyが一致する全てのノードに対して操作を行う．
@param  deli   ノード値（文字列）の区切り文字．
@param  nm     deli を区切り文字として何番目の項目か？ 1から数える．
@param  value  設定する文字列．
@return 設定を行ったノード数．
*/
#define set_protocol_header_item(list, key, no, deli, nm, value)        set_item_tList((list), (key), (no), (deli), (nm), (value))


//
int     search_crlfcrlf(char* mesg);
int     is_header_continue(tList* lp);

void    print_protocol_header(tList* lp, int content);

tList*  find_protocol_end(tList* lp);
#endif

