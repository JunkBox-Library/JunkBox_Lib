#ifndef  __JBXL_JBXL_ERROR_HEADER_
#define  __JBXL_JBXL_ERROR_HEADER_

/** 
@brief   JunkBox_Lib 状態ヘッダ
@file    jbxl_state.h
@version 1.0.1
@author  Fumi.Iseki (C)
@date    2022 06/23
*/

#include <stdio.h>


////////////////////////////////////////////////////////////////////////////////////////

void    jbxl_fprint_state(FILE* fp, int no);
void    jbxl_fprint_state_jp(FILE* fp, int no);
#define jbxl_print_state(no)   jbxl_fprint_state(stderr, (no))


void    jbxl_init_state_info(void);
void    jbxl_add_all_states(void);
void    jbxl_add_state(int id, const char* smb, const char* str);




////////////////////////////////////////////////////////////////////////////////////////
// STATE
//       エラー状態は 負数にする

#define  JBXL_STATE_ANCHOR           999999999  ///< アンカーノード

#define  JBXL_NORMAL                 1          ///< 正常
#define  JBXL_NONE                   0          ///< 情報無し
#define  JBXL_ERROR                 -1          ///< エラー
#define  JBXL_NODATA                -2          ///< 有効なデータが無い
#define  JBXL_NULL_ERROR            -3          ///< 参照先のアドレスが NULL
#define  JBXL_CANCEL                -4          ///< 処理がキャンセルされた
#define  JBXL_INCOMPLETE            -5          ///< データが不完全
#define  JBXL_DATA_REMAINS          -6          ///< データが残っている

#define  JBXL_MALLOC_ERROR          -10         ///< メモリ確保エラー
#define  JBXL_ARGS_ERROR            -11         ///< 不正な引数（NULLなど）

#define  JBXL_FILE_OPEN_ERROR       -20         ///< ファイルオープン エラー
#define  JBXL_FILE_READ_ERROR       -21         ///< ファイル読み込みエラー
#define  JBXL_FILE_WRITE_ERROR      -22         ///< ファイル書き込みエラー
#define  JBXL_FILE_CLOSE_ERROR      -23         ///< ファイルのクローズエラー
#define  JBXL_FILE_EXIST_ERROR      -24         ///< ファイルが存在しない（シンボリックリンクもダメ）
#define  JBXL_FILE_MAKE_ERROR       -25         ///< ファイル作成エラー
#define  JBXL_FILE_DESTOPEN_ERROR   -26         ///< ディスティネーションファイルのオープン失敗

#define  JBXL_TOOLS_ERROR           -100        ///< Tools ライブラリのエラー
#define  JBXL_TOOLS_BUF_ERROR       -101        ///< バッファ（データ格納）部の領域がない
#define  JBXL_TOOLS_BUFSZ_ERROR     -102        ///< バッファ（データ格納）部の大きさが足りない

#define  JBXL_BUFFER_ERROR          -200        ///< Buffer ライブラリのエラー
#define  JBXL_BUFFER_MAKE_ERROR     -201        ///< Buffer の作成に失敗
#define  JBXL_BUFFER_CMP_ERROR      -211        ///< 比較文字数が vldsz より大きい

#define  JBXL_NET_ERROR             -300        ///< Network ライブラリのエラー
#define  JBXL_NET_SOCKET_ERROR      -301        ///< ソケットの作成に失敗
#define  JBXL_NET_OPTION_ERROR      -302        ///< オプションの設定に失敗
#define  JBXL_NET_BIND_ERROR        -303        ///< バインドに失敗
#define  JBXL_NET_INFO_ERROR        -304        ///< ホスト情報の取得に失敗
#define  JBXL_NET_CONNECT_ERROR     -305        ///< 接続に失敗
#define  JBXL_NET_LISTEN_ERROR      -306        ///< リッスンに失敗
#define  JBXL_NET_RECV_ERROR        -311        ///< データの受信エラー
#define  JBXL_NET_SEND_ERROR        -312        ///< データの送信エラー
#define  JBXL_NET_BUF_ERROR         -313        ///< 受信バッファにデータは存在するはずだが，原因不明の理由により獲得に失敗した
#define  JBXL_NET_BUFSZ_ERROR       -314        ///< 受信バッファの長さが足りない．はみ出したデータは捨てられた

#define  JBXL_NET_SEND_TIMEOUT      -391        ///< 送信タイムアウト
#define  JBXL_NET_RECV_TIMEOUT      -392        ///< 受信タイムアウト
#define  JBXL_NET_TRANS_TIMEOUT     -393        ///< 転送タイムアウト

#define  JBXL_LIST_ERROR            -400        ///< tList ライブラリのエラー

#define  JBXL_TREE_ERROR            -500        ///< tTree ライブラリのエラー

#define  JBXL_XTOOLS_ERROR          -600        ///< xTools ライブラリーのエラー
#define  JBXL_XTOOLS_RUNLEN_ERROR   -601        ///< Run Length データが 0x00 で終わっている
#define  JBXL_XTOOLS_RUNLEN2_ERROR  -602        ///< Run Length データに 0x00, 0x00 が存在する（連長圧縮のデータではないのでは？）

#define  JBXL_XML_ERROR             -700        ///< tXML ライブラリのエラー
#define  JBXL_XML_PARSE_ERROR       -701        ///< XML の解釈エラー
#define  JBXL_XML_SEQUENCE_ERROR    -702        ///< XMLノードのリンクエラー
#define  JBXL_XML_MEMORY_ERROR      -703        ///< XML のメモリエラー

#define  JBXL_XML_PARSED             700        ///< XMLパース済み
#define  JBXL_XML_MULTI_ROOT         701        ///< XMLは複数のルート（TOP）を持っている．（パース済み）
#define  JBXL_XML_NODE_OPENED        702        ///< XMLノードは開いている
#define  JBXL_XML_NODE_CLOSED        703        ///< XMLノードは閉じている
#define  JBXL_XML_NODE_EMPTY         704        ///< XMLの空のノード
#define  JBXL_XML_NOT_CLOSED         710        ///< XMLデータが閉じていない
#define  JBXL_XML_DEFAULT_STATE      799        ///< XMLデータの初期状態

#define  JBXL_JSON_ERROR            -800        ///< tJSON ライブラリのエラー
#define  JBXL_JSON_PARSE_ERROR      -801        ///< JSON の解釈エラー
#define  JBXL_JSON_SEQUENCE_ERROR   -802        ///< JSONノードのリンクエラー
#define  JBXL_JSON_MEMORY_ERROR     -803        ///< JSON のメモリエラー

#define  JBXL_JSON_PARSED            800        ///< JSONパース済み
#define  JBXL_JSON_MULTI_ROOT        801        ///< JSONは複数のルート（TOP）を持っている．（パース済み）
#define  JBXL_JSON_NODE_OPENED       802        ///< JSONノードは開いている
#define  JBXL_JSON_NODE_CLOSED       803        ///< JSONノードは閉じている
#define  JBXL_JSON_NODE_EMPTY        804        ///< JSONの空のノード
#define  JBXL_JSON_NOT_CLOSED        810        ///< JSONデータが閉じていない．原因不明．パースアルゴリズムのミス？
#define  JBXL_JSON_PARSE_TERM        811        ///< JSON のパースが途中で終了した．入力データが不完全．
#define  JBXL_JSON_IN_STR            820        ///< JSONデータの属性値処理で文字列を処理中．
#define  JBXL_JSON_IN_ARRAY          821        ///< JSONデータの属性値処理で配列を処理中．
#define  JBXL_JSON_DEFAULT_STATE     899        ///< JSONデータの初期状態


////////////////////////////////////////////////////////////////////////////////////////
// xLib

#define  JBXL_SSL_ERROR             -1100       ///< SSL ライブラリのエラー
#define  JBXL_SSL_RECV_ERROR        -1101       ///< SSL 受信エラー
#define  JBXL_SSL_SEND_ERROR        -1102       ///< SSL 送信エラー

#define  JBXL_SPKI_ERROR            -1110       ///< SPKI エラー
#define  JBXL_DH_ERROR              -1120       ///< DH エラー
#define  JBXL_DH_KEY_ERROR          -1121       ///< DH鍵のエラー

#define  JBXL_BDB_OPEN_ERROR        -1201       ///< BDB でデータファイルのオープンに失敗
#define  JBXL_BDB_KEY_ERROR         -1202       ///< BDB でキーの取得に失敗
#define  JBXL_BDB_GET_ERROR         -1203       ///< BDB でデータの取得に失敗
#define  JBXL_BDB_PUT_ERROR         -1204       ///< BDB でデータの登録に失敗
#define  JBXL_BDB_DATASZ_ERROR      -1205       ///< BDB でデータのサイズが不明
#define  JBXL_BDB_MEMORY_ERROR      -1206       ///< BDB でメモリの確保に失敗

#define  JBXL_LDAP_BASE_ERROR       -1301       ///< LDAP のBASE名が不明
#define  JBXL_LDAP_USER_ERROR       -1302       ///< ユーザ認証失敗(ユーザが存在しない)
#define  JBXL_LDAP_PASSWD_ERROR     -1303       ///< ユーザ認証失敗(ユーザは存在するが，パスワードが一致しない)
#define  JBXL_LDAP_NO_USER_ERROR    -1304       ///< ユーザ名が空白か NULL
#define  JBXL_LDAP_NO_ENTRY_ERROR   -1306       ///< エントリ情報がない
#define  JBXL_LDAP_NO_ATTR_ERROR    -1307       ///< エントリの属性がない
#define  JBXL_LDAP_NO_VAL_ERROR     -1308       ///< エントリの属性値がない

#define  JBXL_BVH_HIERARCHY_ERROR   -1401       ///< HIERARCHYデータの書き込み失敗
#define  JBXL_BVH_MOTION_ERROR      -1402       ///< MOTIONデータの書き込み失敗

#define  JBXL_ISNET_CMD_ERROR       -1501       ///< サーバから 'OK', 'ERR' 以外のものを受信した
#define  JBXL_ISNET_USER_ERROR      -1501       ///< ユーザ認証失敗(ユーザが存在しない)
#define  JBXL_ISNET_PASSWD_ERROR    -1502       ///< ユーザ認証失敗(ユーザは存在するが，パスワードが一致しない)
#define  JBXL_ISNET_SERVER_ERROR    -1503       ///< 認証ホストの検証エラー
#define  JBXL_ISNET_PUBKEYALG_ERROR -1504       ///< 知らない公開鍵暗号アルゴリズム
#define  JBXL_ISNET_CMNKEYALG_ERROR -1505       ///< 知らない共通鍵暗号アルゴリズム
#define  JBXL_ISNET_NULLANS_ERROR   -1506       ///< サーバからの返答が NULL
#define  JBXL_ISNET_CONNECT_ERROR   -1507       ///< 認証サーバとの接続エラー
#define  JBXL_ISNET_START_ERROR     -1508       ///< 認証サーバとの通信開始エラー（相手は認証サーバでない？）
#define  JBXL_ISNET_AGREE_ERROR     -1509       ///< 暗号合意エラー（サーバ認証を含む）
#define  JBXL_ISNET_CHALLENGE_ERROR -1510       ///< チャレンジキー合意エラー
#define  JBXL_ISNET_SPKI_ERROR      -1520       ///< SPKIのエラー（ISNET内）
#define  JBXL_ISNET_DHKEY_ERROR     -1530       ///< DH鍵エラー（ISNET内）

#define  JBXL_ASN1_ERROR            -1600       ///< tASN1 ライブラリのエラー


////////////////////////////////////////////////////////////////////////////////////////
// gLib

#define  JBXL_GRAPH_ERROR           -2000       ///< GRAPH ライブラリーのエラー
#define  JBXL_GRAPH_CANCEL          -2001       ///< 処理がキャンセルされた  
#define  JBXL_GRAPH_HEADER_ERROR    -2002       ///< 画像ヘッダーのエラー
#define  JBXL_GRAPH_MEMORY_ERROR    -2003       ///< メモリエラー
#define  JBXL_GRAPH_NODATA_ERROR    -2004       ///< データが無い
#define  JBXL_GRAPH_NOFILE_ERROR    -2005       ///< ファイルが存在しない
#define  JBXL_GRAPH_OPFILE_ERROR    -2006       ///< ファイルのオープンエラー
#define  JBXL_GRAPH_RDFILE_ERROR    -2007       ///< ファイルの読み込みエラー
#define  JBXL_GRAPH_WRFILE_ERROR    -2008       ///< ファイルの書き込みエラー
#define  JBXL_GRAPH_FILESZ_ERROR    -2009       ///< ファイルサイズのエラー

#define  JBXL_GRAPH_IVDARG_ERROR    -2020       ///< 無効な引数
#define  JBXL_GRAPH_IVDMODE_ERROR   -2021       ///< 無効なモード
#define  JBXL_GRAPH_IVDPARAM_ERROR  -2022       ///< 無効なパラメータ
#define  JBXL_GRAPH_IVDDATA_ERROR   -2023       ///< 無効なデータ

#define  JBXL_GRAPH_THROUGH_ERROR   -2888       ///< エラー処理をスルーする


#endif      // __JBXL_JBXL_ERROR_HEADER_
