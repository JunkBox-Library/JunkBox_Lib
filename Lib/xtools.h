#ifndef  __JBXL_EXTEND_TOOLS_H_
#define  __JBXL_EXTEND_TOOLS_H_
 
/** 
@brief   汎用拡張ツールヘッダ 
@file    xtools.h
@version 1.1
@author  Fumi.Iseki (C)
@date    2009 2/13
*/

#include "tlist.h"
#include "network.h"

#ifndef WIN32
    #include <dirent.h>
#endif


// File Save & Load Mode
#define  JBXL_FIO_ANY          0x0000
#define  JBXL_FIO_IPADDRESS    0x0001
#define  JBXL_FIO_PRIV_KEY     0x000a
#define  JBXL_FIO_PUB_KEY      0x000b
#define  JBXL_FIO_SPKI         0x000c

#define  JBXL_FIO_ORIGINAL     0x0000      // 本来のデータ形式．曖昧!!
#define  JBXL_FIO_BIN          0x0000
#define  JBXL_FIO_DER          0x0000
#define  JBXL_FIO_BASE64       0x0010
#define  JBXL_FIO_STRING       0x0030
#define  JBXL_FIO_AES          0x00a0
#define  JBXL_FIO_DES          0x00b0
#define  JBXL_FIO_3DES         0x00c0


////////////////////////////////////////////////////////////////////////////////////////////
// Network

// for IPv4 互換用
int     udp_recv_Buffer_sockaddr_in(int sock, Buffer* buf, struct sockaddr_in* sv);     ///< recvform()をラッピングした関数．UDPデータを受信する．IPv4専用．
int     udp_send_Buffer_sockaddr_in(int sock, Buffer* buf, struct sockaddr_in* sv);     ///< sendto() をラッピングした関数．UDP経由でデータを送る．IPv4専用．
int     udp_send_sBuffer_sockaddr_in(int sock, Buffer* buf, struct sockaddr_in* sv);    ///< DP経由で文字列データを送る．IPv4専用
int     udp_recv_Buffer_wait_sockaddr_in(int sock, Buffer* buf, struct sockaddr_in* sv_addr, int tm);   ///< UDP経由でデータを受信する．IPv4専用．

struct  sockaddr_in  get_sockaddr_in_Buffer(Buffer buf);                                ///< FQDN:port または IPaddress:port の形式の Buffer変数から ソケット情報を得る．IPv4専用

// for IPv4/IPv6 
int     udp_recv_Buffer(int sock, Buffer* buf, struct addrinfo* sv);                    ///< recvform()をラッピングした関数．UDPデータを受信する．
int     udp_send_Buffer(int sock, Buffer* buf, struct addrinfo* sv);                    ///< sendto() をラッピングした関数．UDP経由でデータを送る．
int     tcp_recv_Buffer(int sock, Buffer* buf);                                         ///< TCP経由でデータを受信する．バッファリングなし．
int     tcp_send_Buffer(int sock, Buffer* buf);                                         ///< TCP経由でデータを送信する．

int     udp_recv_Buffer_wait(int sock, Buffer* buf, struct addrinfo* sv_addr, int tm);  ///< UDP経由でデータを受信する．待ち時間（タイムアウト）を指定できる．
int     tcp_recv_Buffer_wait(int sock, Buffer* buf, int tm);                            ///< TCP経由でデータを受信する．待ち時間（タイムアウト）を指定できる．

int     tcp_recv_Buffer_tosize     (int sock, Buffer* b, Buffer* m, int sz);            ///< TCP経由でデータを size バイトまで受信する．バッファリングなし．
int     tcp_recv_Buffer_tosize_wait(int sock, Buffer* b, Buffer* m, int sz, int tm);    ///< TCP経由でデータを size バイトまで受信する．待ち時間を指定できる．

int     udp_send_sBuffer   (int sock, Buffer* buf, struct addrinfo* sv);                ///< UDP経由で文字列データを送る．
int     tcp_send_sBuffer   (int sock, Buffer* buf);                                     ///< TCP経由で文字列データを送信する．
int     tcp_send_sBufferln (int sock, Buffer* buf);                                     ///< TCPメッセージ(文字列)に改行(0x0d, 0x0a)を付け加えて送信する．  

int     tcp_recv_mstream_Buffer(int sock, Buffer* buf, mstream* sb, int tm);            ///< TCP経由でメッセージを受信する．バッファリングあり．
int     tcp_recv_lines_Buffer(int sock, Buffer* buf, int tm);                           ///< TCP経由でメッセージを複数行受信する．簡易バッファリングあり．

// URL
Buffer  comp_hostport(char* ipaddr, unsigned short port);                               ///< ホスト名とポート番号から，"ホスト名:ポート番号" の文字列を生成する．
int     decomp_hostport(Buffer buf, Buffer* ipaddr, unsigned short* port);              ///< "ホスト名:ポート番号" 形式の文字列から，ホスト名とポート番号を分離する．

Buffer  comp_url(char* protocol, char* host, unsigned short port, char* dir);           ///< "プロトコル://ホスト名:ポート番号/ディレクトリ名" の文字列を生成する．
int     decomp_url(Buffer url, Buffer* srvurl, Buffer* protocol, Buffer* srvfqdn, unsigned short* sport, Buffer* srvdir);   ///< URLを分解する．

Buffer  make_form_urlenc(const char* key, const char* val);                             ///< key=val (valはURL Encodeされる)の文字を作成する．
void    add_form_urlenc(Buffer* buf, const char* key, const char* val);                 ///< 既存の文字に &key=val (valはURL Encodeされる) を追加する．

#define make_hostport(h, p)     comp_hostport((h), (p))                                 ///< comp_hostport()
#define make_url(t, h, p, d)    comp_url((t), (h), (p), (d))                            ///< comp_url()

////////////////////////////////////////////////////////////////////////////////////////////
// File I/O
int     save_tagged_Buffer(Buffer buf, FILE* fp, unsigned int mode, int prfm);  ///< Bufferを指定された形式に従ってタグ付きでファイルに保存する．
Buffer  read_tagged_Buffer(FILE* fp, unsigned int* mode);                       ///< save_tagged_Buffer() で保存したファイルから，Buffer をタグに従って読み込む．
Buffer  fgets_mstream_Buffer(Buffer buf, mstream* sb);                          ///< メッセージ buf.buf はメッセージストリームに一旦バッファリングされ，この関数により一行ずつ読み出される．

////////////////////////////////////////////////////////////////////////////////////////////
// Run Length
int     get_runlength_byte(unsigned char* buf, int len, int pos);   ///< 圧縮されていない状態で pos(Byte)の位置が，0の連長圧縮されているデータではどの位置に来るかを計算する
Buffer  decode_runlength  (unsigned char* buf, int len, int sz);    ///< 0の連長圧縮された bufから szバイトの通常のデータを取り出す．
Buffer  encode_runlength  (unsigned char* buf, int len);            ///< bufの szバイトを 0で連長圧縮する

#define get_runlength_byte_Buffer(b, s)     get_runlength_byte((b).buf, (b).vldsz, (s))     ///< get_runlength_byte()
#define decode_runlength_Buffer(b, s)       decode_runlength((b).buf, (b).vldsz, (s))       ///< decode_runlength()
#define encode_runlength_Buffer(b)          encode_runlength((b).buf, (b).vldsz)            ///< encode_runlength()

////////////////////////////////////////////////////////////////////////////////////////////
// Ring Buffer

int      put_Buffer_ringBuffer(ringBuffer* rb, Buffer* buf);        ///< リングバッファ rb へ Buffer データを vldszバイト格納する．
Buffer*  get_Buffer_ringBuffer(ringBuffer* rb, int sz);             ///< リングバッファ rb から szバイトのデータを取り出し，Buffer型データに格納する．要 del_Buffer()

////////////////////////////////////////////////////////////////////////////////////////////
// 配列
Buffer* get_Buffer_dim_tList(tList* lp);                ///< リストを分解してキー部(key)の配列を造る
Buffer* get_Buffer_dim_tList_value(tList* lp);          ///< リストを分解してバリュー部(val)の配列を造る

Buffer* awk_Buffer_dim (Buffer  buf, char cc);          ///< 文字列を awkで分解して配列を作る
Buffer* cawk_Buffer_dim(Buffer  buf, char cc);          ///< 文字列を cawkで分解して配列を作る
Buffer  join_Buffer_dim(Buffer* dim, const char* deli); ///< 配列の要素を dliを間に入れて繋げる

Buffer* decompline_Buffer_dim(Buffer buf, int mode);    ///< データを行単位に分解する．改行は @\r@\n

void    del_Buffer_dim(Buffer** buf);                   ///< 配列を削除する．

////////////////////////////////////////////////////////////////////////////////////////////
// tList and Buffer
tList*  get_tList_line_Buffer(Buffer buf, int n);           ///< buf から n行目を取り出して，' ' で分解してリストに格納して返す．
tList*  get_tList_seq_data_Buffer(Buffer buf, int* ptr);    ///< buf から行データをシーケンシャルに取り出して，それぞれを ' ' で分解してリストに格納して返す．

////////////////////////////////////////////////////////////////////////////////////////////
// Extend tList
int     set_item_tList(tList* list, char* key, int no, char deli, int nm, char* value);     ///< key をキーにした no番目のノードに対して set_item_tList_node() を行う．
int     replace_item_tList(tList* list, char* key, int no, char deli, int nm, char* srcval, char* value); ///< key をキーにした no番目のノードに対して replace_item_tList_node() を行う．
int     set_item_tList_node(tList* list, char deli, int nm, char* value);                   ///< deliを区切りにした nm番目の項目（文字列）に value を設定する
int     replace_item_tList_node(tList* list, char deli, int nm, char* srcval, char* value); ///< deliを区切りにした nm番目の項目（文字列）の srcval部分を valueで置き換える．

////////////////////////////////////////////////////////////////////////////////////////////
// File, Directory
tList*  get_dir_files(const char* dirn);                                ///< 指定されたディレクトリにあるファイル名の一覧を取得．
tList*  get_dir_files_rcsv(const char* dirn);                           ///< 指定されたディレクトリにあるファイル名の一覧を再帰的に取得．

void    del_file_extension_Buffer(Buffer* path);                        ///< ファイルの拡張子を削除する．
void    change_file_extension_Buffer(Buffer* path, const char* ext);    ///< ファイルの拡張子を extにする．ファイルに拡張子が無い場合は extを付加する

Buffer  relative_path_Buffer(Buffer path_a, Buffer path_b);             ///< path_a から path_b への相対パスを返す．
void    canonical_filename_Buffer(Buffer* fname);                       ///< fname の問題になりそうな ASCII文字を '_' に変換する．

////////////////////////////////////////////////////////////////////////////////////////////
// Resource (file) List
tList*  _add_key_val_list(tList* pp, tList* list, const char* key, const char* val, int mode);
tList*  add_resource_list(const char* path, int keylen, tList* list, tList* extn, int mode);  ///< ディレクトリ pathを検索して，リソースリストにファイルを追加し，リストの先頭を返す．
char*   get_resource_path(const char* name, tList* lp);                 ///< リソースの名前から，ファイルのパスを得る．戻り値は free() してはいけない．


#endif  // __JBXL_EXTEND_TOOLS_H_

