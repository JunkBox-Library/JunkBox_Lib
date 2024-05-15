#ifndef  __JBXL_TINY_XML_H_
#define  __JBXL_TINY_XML_H_

/** 
@brief   Tiny XML ライブラリヘッダ 
@file    txml.h
@version 1.0
@author  Fumi.Iseki (C)
@date    2009 2/10

@par タイトル
Tiny XML txml.h v1.0 @n
サブセット版　整形式XML 簡易パーサ ライブラリヘッダ

@see tXML

@attention
全てのパターンのパース可能性は保障しない@n
複雑な XMLは（恐らく）パースできない．@n
<?xml ... ?> 以外の <? ..... ?> と <! .....  > は処理しない（読み込みは行う）@n
エンティティは処理しない．@n
*/

#include "tools.h"
#include "ttree.h"
#include "jbxl_state.h"


/** @typedef tXML
typedef  tTree  tXML;   ///<

@b tXML  (@b tList, @b tTree, @b _tList と同じもの)
@see tTree 

1. ノードデータ tXML：
@code
    int     ldat.id     ノードの種別．XML_ANCHOR_NODE, XML_NAME_NODE, XML_CONTENT_NODE, ..........
    int     ldat.lv     種別がノード値である子ノードの個数
    int     ldat.sz     未使用
    Buffer  ldat.key    ノード名またはノード値
    Buffer  ldat.val    <! ... > の未処理データと <!-- ... -->のコメント情報を格納 
    void*   ldat.ptr    未使用 
    tXML*   ldat.lst    属性リストへのポインタ．ldat.lst->ldat.keyに属性名，ldat.lst->ldat.valに属性値("'付き)を格納

    tXML*   next        子ノード（長女）へのポインタ
    tXML*   prev        親ノードへのポインタ
    tXML*   altp        その他へのポインタ（汎用） 
    tXML*   yngr        子ノード（末っ子）へのポインタ        // back から変更

    tXML*   esis        前の姉妹（姉）ノードへのポインタ
    tXML*   ysis        次の姉妹（妹）ノードへのポインタ

    int     depth       深さ                    
    int     num         子ノードの数（全ての子ノードの数）    
    int     ctrl        制御用
    int     state       ノードの状態  JBXL_XML_PARSED, JBXL_XML_NODE_OPENED, JBXL_XML_NODE_CLOSES, JBXL_XML_PARSE_ERR, ....
@endcode

2. パースしたXML情報を格納するツリー構造
@code
    <?xml version="1.0" ?><A><B><C>ABC</C><D />EFG</B><E /><!-- comment --></A> 
    
      NULL (XML_ANCHOR_NODE) -> xml (XML_DOC_NODE)
                             -> A (XML_NAME_NODE) -> B (XML_NAME_NODE) -> C (XML_NAME_NODE) -> ABC (XML_CONTENT_NODE)
                                                                       -> D (XML_NAME_NODE)
                                                                       -> EFG (XML_CONTENT_NODE)
                                                  -> E (XML_NAME_NODE)
                                                  -> @COMMENT_NODE (XML_COMMENT_NODE)     // ただしコメントの処理は暫定的
@endcode

3. ノード（ノード）種別 (ldat.id)
@code
    XML_ANCHOR_NODE            アンカーノード
    XML_NAME_NODE              ネームノード
    XML_CONTENT_NODE           内容（コンテント）ノード
    XML_DOC_NODE               <? xml... ?> ノード
    XML_COMMENT_NODE           コメントノード
    XML_DATA_NODE              <! ... > ノード
    XML_PROCESS_NODE           XML_DOC_NODE 以外の <? ... ?>ノード
@endcode

4. 状態 (state)
@code
    JBXL_XML_DEFAULT_STATE     ノードは，正常にパースされたかどうかの判定はまだ行われていない．
    JBXL_XML_PARSED            アンカーノードにのみ設定される．このXMLツリーは正常にパースされたことを示す．
    JBXL_XML_NODE_STARTED      このノードはオープン状態にある． <tagname>
    JBXL_XML_NODE_ENDED        このノードは正常にクローズした． </tagname>
    JBXL_XML_NODE_EMPTY        このノードは空要素ノードである <  />, <!--  -->, <!   >, <?   ?> の型

    JBXL_XML_NOT_CLOSED        最終的にノードが閉じなかったが，解釈できるところまでは解釈した．
    JBXL_XML_MULTI_ROOT        アンカーノードにのみ設定される．このXMLツリーは 2つ以上の独立したツリーを持つ

    JBXL_XML_ERROR             tXML ライブラリのエラー
    JBXL_XML_PARSE_ERROR       解釈エラー
    JBXL_XML_SEQUENCE_ERROR    アンカーを越えてツリーを作ろうとした．
    JBXL_XML_MEMORY_ERROR      メモリの確保エラー
@endcode
*/
typedef  tTree  tXML;   ///<


// ノード種別    (ldat.id)
#define  XML_ANCHOR_NODE    JBXL_STATE_ANCHOR   ///< アンカーノード
#define  XML_NAME_NODE              1           ///< ネームノード
#define  XML_CONTENT_NODE           2           ///< 内容（コンテント）ノード
#define  XML_DOC_NODE               3           ///< <? xml... ?> ノード
#define  XML_COMMENT_NODE           4           ///< コメントノード
#define  XML_DATA_NODE              5           ///< <! ... > ノード
#define  XML_PROCESS_NODE           6           ///< XML_DOC_NODE 以外の <? ... ?>ノード

// XML Format
#define  XML_ONELINE_FORMAT         0           ///< 改行なしの一行にする．
#define  XML_CRLF_FORMAT            1           ///< ノードの終わりを CR(0x0d), LF(0x0a)で改行する．
#define  XML_TAB_FORMAT             2           ///< 先頭にインデント(TAB)をつけ，ノードごとに改行する．
#define  XML_SPACE_FORMAT           3           ///< 先頭に空白2つ "  " をつけ，ノードごとに改行する．
#define  XML_SPACE2_FORMAT          3           ///< 先頭に空白2つ "  " をつけ，ノードごとに改行する．
#define  XML_SPACE4_FORMAT          4           ///< 先頭に空白4つ "    " をつけ，ノードごとに改行する．

// 代用ノード名 (ldat.key.buf)
#define  XML_COMMENT_NODE_KEY       "@COMMENT_NODE"
#define  XML_DATA_NODE_KEY          "@DATA_NODE"



/////////////////////////////////////////////////////////////////////////////////////////////
// XML Functions

#define new_xml_node()          new_tTree_node()            ///< new_tTree_node()
#define new_xml()               new_tTree_node()            ///< new_tTree_node()
#define new_xml_anchor_node()   new_tTree_anchor_node()     ///< new_tTree_anchor_node()
#define del_xml_anchor(t)       del_tTree_anchor_node((t))  ///< del_tTree_anchor_node()
#define del_xml_anchor_nodet()  del_tTree_anchor_node((t))  ///< del_tTree_anchor_node()


// Paser
tXML*   xml_parse(char* pp);                        ///< 文字列のXMLデータを解釈して，tXMLのツリーを生成する．
tXML*   xml_parse_file(const char* pp);             ///< ファイルから読み込んでパースする．
tXML*   xml_parse_seq(tXML* xml,  char* pp);        ///< 部分的な XMLデータを解釈して，tXMLのツリー構造に格納する．断片的にXMLデータを入力する場合に使用する．
tXML*   xml_main_parse(tXML* xml, char* pp, int skip);  ///< 部分的な XMLデータを解釈して，tXMLのツリー構造に格納する．パーサの内部的なメイン関数．

int     xml_parse_start_node(char* pp, char** node_name, char** node_attr, int* node_end);  ///< XMLの開始ノードを処理する．
int     xml_parse_end_node(char* pp, char** node_name);                                     ///< XMLの終了ノードを処理する．
int     xml_parse_content(char* pp, char** content);                                        ///< XMLのコンテントを処理する．
int     xml_parse_comment_node(char* pp, char** comment);                                   ///< XMLのコメントノードを処理する．
int     xml_parse_data_node(char* pp, char** data);                                         ///< XMLのデータノード（その他エレメントノードなど）を処理する．
int     xml_parse_processing_node(char* pp, char** node_name, char** node_attr);            ///< XMLのプロセッシングノードを処理する
tList*  xml_parse_attr(char* pp);                                                           ///< XMLのノード属性を解釈して，リスト(tList)にする．

void    close_xml(tXML* pp);                        ///< パースエラーを起こしたツリーを強制的に正常化する．
int     isnot_xml_name(unsigned char* pp);          ///< XML名として不適切かどうかを大まかに判断する．


#define find_xml_top(p)        find_tList_top(p)    ///< find_tList_top()
#define find_xml_end(p)        find_tTree_end(p)    ///< find_tTree_end()

// 逆パース
Buffer  xml_inverse_parse(tXML* pp, int mode);                              ///< ppに格納された XMLデータを元の書式に戻して Bufferに格納する．xml_parse() の逆．
void    xml_open_node_Buffer (tXML* pp, Buffer* buf, int mode, int indent); ///< ツリー中のXMLのオープンノードのデータを元の書式に戻して Bufferに格納する．
void    xml_close_node_Buffer(tXML* pp, Buffer* buf, int mode, int indent); ///< ツリー中のXMLのクローズノードのデータを元の書式に戻して Bufferに格納する．
void    xml_attr_to_Buffer(tList* pp, Buffer* buf);                         ///< リストに保存されたノードの属性をテキストへ戻す．
void    _xml_to_Buffer(tXML* pp, Buffer* buf, int mode, int indent);        ///< xml_inverse_parse()用の補助関数．

// Operation
tXML*   init_xml_doc(void);                                                 ///< XML のドキュメントヘッダを作成する．
char*   get_first_xml_nodename(tXML* xml);                                  ///< XMLツリーの最初のノードの名前を返す．
tXML*   add_xml_node(tXML* xml, const char* name);                          ///< XMLツリーのxml の直下（姉妹の一番下：末っ子）にノード（属性は指定しない）を挿入する．
tXML*   insert_xml_node(tXML* xml, const char* name);                       ///< XMLツリーのxml の直下（姉妹の一番上：長子）にノード（属性は指定しない）を挿入する．

tXML*   add_xml_attr(tXML* xml, const char* attr);                          ///< xml に属性（attr）を追加する．
tXML*   add_xml_attr_str   (tXML* xml, const char* name, const char* value);///< xml に属性名 name, 文字列の属性値 value を持つノードを追加する．
tXML*   add_xml_attr_int   (tXML* xml, const char* name, int   value);      ///< xml に属性名 name, 整数の属性値 value を持つノードを追加する．
tXML*   add_xml_attr_float (tXML* xml, const char* name, float value);      ///< xml に属性名 name, float型実数の属性値 value を持つノードを追加する．
tXML*   add_xml_attr_double(tXML* xml, const char* name, double value);     ///< xml に属性名 name, double型実数の属性値 value を持つノードを追加する．

tXML*   add_xml_content_node(tXML* xml, const char* value);                 ///< XMLツリーのxml の直下にコンテントを挿入する．
tXML*   append_xml_content_node(tXML* xml, const char* value);              ///< XMLツリーのxml の直下のコンテントノードにコンテンツを追加する．
tXML*   set_xml_content_node(tXML* xml, const char* value);                 ///< XMLツリーのxml の直下のコンテントノードの値を置き換える．
int     add_xml_content_area(tXML* xml, int len);                           ///< xml に空のコンテントノードを追加する．

#define join_xml(a, b)              add_tTree_node((a), (b))                ///< add_tTree_node()

Buffer  make_xml_attr_bystr   (const char* name, const char* value);        ///< 属性名 name, 文字列の属性値 value を持つ Buffer型変数を作成する．
Buffer  make_xml_attr_byint   (const char* name, int   value);              ///< 属性名 name, 整数の属性値 value を持つ Buffer型変数を作成する．
Buffer  make_xml_attr_byfloat (const char* name, float value);              ///< 属性名 name, float型実数の属性値 value を持つ Buffer型変数を作成する．
Buffer  make_xml_attr_bydouble(const char* name, double value);             ///< 属性名 name, double型実数の属性値 value を持つ Buffer型変数を作成する．

#define make_xml_attr_str(n, v)     make_xml_attr_bystr   ((n), (v))        ///< make_xml_attr_bystr()
#define make_xml_attr_int(n, v)     make_xml_attr_byint   ((n), (v))        ///< make_xml_attr_byint()
#define make_xml_attr_float(n, v)   make_xml_attr_byfloat ((n), (v))        ///< make_xml_attr_byfloat()
#define make_xml_attr_double(n, v)  make_xml_attr_bydouble((n), (v))        ///< make_xml_attr_bydouble()

char*   xml_get_node_content(tXML* node);                                   ///< コンテントへのポインタを返す．free() してはいけない．
char*   xml_get_node_attr(tXML* node, const char* attr);                    ///< 属性値を持つノードのポインタから，指定された属性値へのポインタを返す．free() してはいけない．

tXML*   dup_merge_xml(tXML* pp, tXML* tp);                                  ///< XMLツリー ppの直下にXMLツリー tpを複製する．

#define free_xml(p)                 free_tTree_node((p))                    ///< free_tTree_node()
#define del_xml(p)                  del_tTree((p))                          ///< 指定したノード以下のXMLツリー（ppの姉妹は含まない）を削除する．
#define del_xml_node(p)             del_tTree_node((p))                     ///< 指定したノードを削除する．
#define del_all_xml(p)              del_all_tTree((p))                      ///< XMLツリーの全ノードの削除．ポインタ ppのノードを含むXMLツリー全体を削除する．
#define del_sisters_xml(p)          del_sisters_tTree((p))                  ///< 指定したノード以下のXMLツリー（ppの姉妹を含む）を削除する．
#define del_sisters_children_xml(p) del_sisters_children_tTree((p))         ///< 指定したノードの姉妹XMLツリー，子XMLツリーを削除する．指定したXMLノードも削除する．

void    print_xml(FILE* fp, tXML* pp, int mode);                            ///< XMLの表示（出力）．
void    print_xml_tree(FILE* fp, tXML* pp, const char* space);              ///< XMLツリーをそのままツリー表示する．デバッグ用．
void    print_xml_node(FILE* fp, tXML* pp);                                 ///< MLのノード情報を表示する．デバッグ用．
void    print_sister_xml_tree(FILE* fp, tXML* pp, const char* space);       ///< XMLツリーの表示．ppの姉妹ノードも出力する．

#define print_tXML(f, x)            print_tTree((f), (x))
#define print_tXML_tree(f, x, s)    print_tTree_tree((f), (x), (s))


///////////////////////////////////////////////////////////////////////////////////////////
// Search and Get/Set    操作対象は１番最初に一致したもの

tXML*   get_xml_node(tXML* pp, tXML* pt);                    ///< 同じパターンの枝を探し，ptに最初に一致した枝の，ptの最後のノードに対応したノードへのポインタを返す．
int     set_xml_node(tXML* pp, tXML* pt, const char* val);   ///< 同じパターンの枝を探し，ptに最初に一致した枝の，ptの最後のノード対応したノードにノード名をコピーする．
int     set_xml_end_node(tXML* pp, tXML* pt);                ///< 同じパターンの枝を探し，ptに最初に一致した枝の，ptの最後のノードに対応したノードへのポインタを返す．

tXML*   get_xml_attr_node(tXML* pp, const char* key, const char* val); ///< 属性 key=val のノードを探し，最初に見つけたノードを返す．

tXML*   get_xml_content(tXML* pp, tXML* pt);                 ///< 同じパターンの枝を探し，ptに最初に一致した枝の，ptの最後のノードに対応したノードのコンテントへのポインタを altp に入れて返す．
int     set_xml_content(tXML* pp, tXML* pt, const char* val);///< 同じパターンの枝を探し，ptに最初に一致した枝の，ptの最後ノードに対応したのノードのコンテントを contentで置き換える．
tList*  get_xml_attr(tXML* pp, tXML* pt);                    ///< 同じパターンの枝を探し，ptに最初に一致した枝の，ptの最後のノードに対応したノードのノード属性値へのリストを altp に入れて返す．
int     set_xml_attr(tXML* pp, tXML* pt, tList* at);         ///< 同じパターンの枝を探し，ptに最初に一致した枝の，ptの最後のノードに対応したノードのノードの属性としてatの値をコピーする．

tXML*   get_xml_node_bystr(tXML* pp, const char* str);                      ///< get_xml_node(tXML* pp, tXML* pt) の _bystr バージョン
int     set_xml_node_bystr(tXML* pp, const char* str, const char* val);     ///< set_xml_node(tXML* pp, tXML* pt, const char* val) の _bystr バージョン
int     set_xml_end_node_bystr(tXML* pp, const char* str);                  ///< set_xml_end_node(tXML* pp, tXML* pt) の _bystr バージョン

tXML*   get_xml_content_bystr(tXML* pp, const char* str);                   ///< get_xml_content(tXML* pp, tXML* pt) の _bystr バージョン
int     set_xml_content_bystr(tXML* pp, const char* str, const char* val);  ///< set_xml_content(tXML* pp, tXML* pt, char* val) の _bystr バージョン
tList*  get_xml_attr_bystr(tXML* pp, const char* str);                      ///< get_xml_attr(tXML* pp, tXML* pt, tList* at) の _bystr バージョン．
int     set_xml_attr_bystr(tXML* pp, const char* pt, tList* at);            ///< set_xml_attr(tXML* pp, tXML* pt, tList* at) の _bystr バージョン．

int     get_xml_int_content(tXML* pp, tXML* pt);                            ///< get_xml_content() を使用し，XML コンテンツの内容を int型で返す．
int     get_xml_int_content_bystr(tXML* pp, const char* str);               ///< get_xml_int_content(tXML* pp, tXML* pt) の _bystr バージョン
float   get_xml_float_content(tXML* pp, tXML* pt);                          ///< get_xml_content() を使用し，XML コンテンツの内容を float型で返す．
float   get_xml_float_content_bystr(tXML* pp, const char* str);             ///< get_xml_float_content(tXML* pp, tXML* pt) の _bystr バージョン．
double  get_xml_double_content(tXML* pp, tXML* pt);                         ///< get_xml_content() を使用し，XML コンテンツの内容を double型で返す．
double  get_xml_double_content_bystr(tXML* pp, const char* str);            ///< get_xml_double_content(tXML* pp, tXML* pt) の _bystr バージョン．
char*   get_xml_char_content(tXML* pp, tXML* pt);                           ///< get_xml_content() を使用し，XML コンテンツの内容を char*型で返す．free() してはいけない．
char*   get_xml_char_content_bystr(tXML* pp, const char* str);              ///< get_xml_char_content(tXML* pp, tXML* pt) の _bystr バージョン．free() してはいけない．

char*   get_xml_char_attr(tXML* pp, tXML* pt, const char* attr);                ///< get_xml_node() で検索したノードから，属性値 attrの値を char*型で取り出す．free() してはいけない．
char*   get_xml_char_attr_bystr(tXML* pp, const char* str, const char* attr);   ///< get_xml_char_attr() の _bystr バージョン．free() してはいけない．
int     get_xml_int_attr(tXML* pp, tXML* pt, const char* attr);                 ///< get_xml_node() で検索したノードから，属性値 attrの値を int型で取り出す．
int     get_xml_int_attr_bystr(tXML* pp, const char* str, const char* attr);    ///< get_xml_int_attr() の _bystr バージョン．
double  get_xml_double_attr(tXML* pp, tXML* pt, const char* attr);              ///< get_xml_node() で検索したノードから，属性値 attrの値を double型で取り出す．
double  get_xml_double_attr_bystr(tXML* pp, const char* str, const char* attr); ///< get_xml_double_attr() の _bystr バージョン．

int     replace_xml_content(tXML*pp, tXML* pt, const char* src, const char* dst);   ///< get_xml_content() を使用し，XMLキーの src部分を dstで書き換える．
int     replace_xml_content_bystr(tXML*pp, const char* str, const char* src, const char* dst);   ///< replace_xml_content(tXML*pp, tXML* pt, char* src, char* dst) の _bystr バージョン．

#define get_xml_node_str(p, s)              get_xml_node_bystr((p), (s))                    ///< get_xml_node_bystr()
#define set_xml_node_str(p, s, v)           set_xml_node_bystr((p), (s), (v))               ///< set_xml_node_bystr()
#define set_xml_end_node_str(p, s)          set_xml_end_node_bystr((p), (s))                ///< set_xml_end_node_bystr()
#define get_xml_content_str(p, s)           get_xml_content_bystr((p), (s))                 ///< get_xml_content_bystr()
#define set_xml_content_str(p, s, v)        set_xml_content_bystr((p), (s), (v))            ///< set_xml_content_bystr()
#define get_xml_attr_str(p, s)              get_xml_attr_bystr((p), (s))                    ///< get_xml_attr_bystr()
#define set_xml_attr_str(p, t, a)           set_xml_attr_bystr((p), (t), (a))               ///< set_xml_attr_bystr()
#define replace_xml_content_str(p, t, r, d) replace_xml_content_bystr((p), (t), (r), (d))   ///< replace_xml_content_bystr()

#define get_xml_int_content_str(p, s)       get_xml_int_content_bystr((p), (s))             ///< get_xml_int_content_bystr()
#define get_xml_double_content_str(p, s)    get_xml_double_content_bystr((p), (s))          ///< get_xml_double_content_bystr()
#define get_xml_char_content_str(p, s)      get_xml_char_content_bystr((p), (s))            ///< get_xml_char_content_bystr()
#define get_xml_char_attr_str(p, s, v)      get_xml_char_sttr_bystr((p), (s), (v))          ///< get_xml_char_sttr_bystr()

#define get_xml_str_content(p, s)           get_xml_char_content((p), (s))                  ///< get_xml_char_content()
#define get_xml_str_content_str(p, s)       get_xml_char_content_bystr((p), (s))            ///< get_xml_char_content_bystr()
#define get_xml_str_content_bystr(p, s)     get_xml_char_content_bystr((p), (s))            ///< get_xml_char_content_bystr()
#define get_xml_str_attr(p, s)              get_xml_char_attr((p), (s))                     ///< get_xml_char_attr()
#define get_xml_str_attr_str(p, s, v)       get_xml_char_attr_bystr((p), (s), (v))          ///< get_xml_char_attr_bystr()
#define get_xml_str_attr_bystr(p, s, v)     get_xml_char_attr_bystr((p), (s), (v))          ///< get_xml_char_attr_bystr()
#define get_xml_int_attr_str(p, s, v)       get_xml_int_attr_bystr((p), (s), (v))           ///< get_xml_int_attr_bystr()
#define get_xml_double_attr_str(p, s, v)    get_xml_double_attr_bystr((p), (s), (v))        ///< get_xml_double_attr_bystr()

// Operation for Multi Node
tList*  get_xml_node_list(tXML* pp, tXML* pt);    ///< XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに一致した枝の，ptの最後のノードに対応するノードへのポインタをリストに格納して返す．
tList*  get_xml_content_list(tXML* pp, tXML* pt); ///< XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに一致した枝の，ptの最後のノードに対応するノードのコンテントへのポインタをリストに格納して返す．
int     set_xml_content_list(tXML* pp, tXML* pt, const char* content);              ///< get_xml_content_list() で検出したコンテントを, content で置き換える．

tList*  get_xml_node_list_bystr(tXML* pp, const char* str);                         ///< get_xml_node_list() の _bystr バージョン．
tList*  get_xml_content_list_bystr(tXML* pp, const char* str);                      ///< get_xml_conetnt_list_bystr() の _bystr バージョン．
int     set_xml_content_list_bystr(tXML* pp, const char* str, const char* content); ///< set_xml_content_list_bystr() の _bystr バージョン．

#define get_xml_node_list_str(p, s)          get_xml_node_list_bystr((p), (s))          ///< get_xml_node_list_bystr()
#define get_xml_content_list_str(p, s)       get_xml_content_list_bystr((p), (s))       ///< get_xml_content_list_bystr()
#define set_xml_content_list_str(p, s, c)    set_xml_content_list_bystr((p), (s), (c))  ///< set_xml_content_list_bystr()


///////////////////////////////////////////////////////////////////////////////////////////
// Search XML Node (tTree用の関数とほぼ同じ)．ツリーを返す．

int     find_match_xml(tXML* pp, tXML* pt);                         ///< ツリー pp内で ツリー ptと同じパターンの枝を探す．姉妹ツリーも検索するので注意．
tXML*   find_match_xml_endlist(tXML* pp, tXML* pt);                 ///< ツリー pp内で ツリー ptと同じパターンの枝を全て探して，その枝のptの最後のノードに対応するノードの情報をリストにして返す．
tXML*   find_match_xml_end_node(tXML* pp, tXML* pt);                ///< XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptの最後のノードに対応する pp内のノードへのポインタを返す．
tList*  _find_match_xml_endlist_rcsv(tXML* pp, tXML* pt, tXML* te); ///< find_match_xml_endlist() の補助関数

int     check_match_xml(tXML* tp, tXML* tr);                        ///< XMLツリー tpが XMLツリー trと同じかどうかを検査する．
tXML*   cmp_sisters_xml(tXML* tp, tXML* tr);                        ///< XMLノード tpの姉妹ノードが trの姉妹ノードと同じ XMLノードまたはコンテントであるかを比較する．


/////////////////////////////////////////////////////////////////////////////////////////////
// Simple Node Functions.

int     get_node_integer(tXML* tp, const char* name, int no);   ///< tp中のXMLデータから no番目のノード '@<node_name>node_content(整数)@</node_name>' の node_content(整数)を得る．
Buffer  get_node_content(tXML* tp, const char* name, int no);   ///< tp中のXMLデータから no番目のノード '@<name>node_content@</name>' のnode_contentのコピーを得る．

int     return_exist_node(tXML* tp, const char* name, int no, Buffer* value);   ///< bufの中に ノード '@<name>content@</name>' が存在するかどうかチェックする．

int     replace_all_node_integer (tXML* tp, const char* name, int src, int dst);                      ///< XMLツリー pp内で ノード名が nameである全てのノードのコンテント（整数）を，srcから dstに書き換える．
int     replace_all_node_contents(tXML* tp, const char* name, const char* src, const char* dst);      ///< XMLツリー pp内で ノード名が nameである全てのノードのコンテントを，srcから dstに書き換える．
int     _replace_all_node_contents_rcsv(tXML* tp, const char* name, const char* src, const char* dst);///< replace_all_node_content() の補助関数

int     replace_all_node_byid(tXML* tp, const char* src, const char* dst, int id);          ///< XMLツリー pp内で ノードの種別が idである全てのノードの内容を srcから dstに書き換える．
int     _replace_all_node_byid_rcsv(tXML* tp, const char* src, const char* dst, int id);    ///< replace_all_node_byid() の補助関数


/////////////////////////////////////////////////////////////////////////////////////////////
// XML-RPC

Buffer  xml_rpc_request_pack(const char* method, tXML* xml);                ///< XML-RPC用のデータを生成する．
tXML*   xml_rpc_add_member(tXML* xml, char* name, char* value, char* kind); ///< XML-RPC の @<member> データを作って，繋げていく．
tXML*   xml_rpc_end_member(tXML* xml);                                      ///< @<struct>@<member> データを閉じて，@<value> データを作り出す．
tXML*   xml_rpc_add_array(tXML* xml, tXML* arry);                           ///< @<value>@<struct> データから 配列データ @<data>...@</data> を作って繋げていく．
tXML*   xml_rpc_end_array(tXML* xml);                                       ///< 配列データを閉じて，送信用データを生成する．


#endif  // __JBXL_TINY_XML_H_
