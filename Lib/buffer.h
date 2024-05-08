#ifndef  __JBXL_BUFFER_H_
#define  __JBXL_BUFFER_H_

/** 
@brief   Buffer型サポートヘッダ
@file    buffer.h
@version 1.3
@author  Fumi.Iseki (C)
@date    2012 7/23
@see     Buffer
*/

#include "tools.h"


/** @struct Buffer

typedef sruct _Buffer Buffer;

Buffer型は文字列操作時の配列破壊を防止する．@n
buffer.c のBuffer型対応関数を使う限り, プログラマ(ライブラリ利用者)は配列破壊を考慮する必要は無い．@n
通常, vldszはプログラマ(ライブラリ利用者)が保証しなければならない．

したがってBuffer型変数を返す関数では常に vldszが設定されているか注意すべきである．@n
ただし,データが文字列の場合は -1以下にしておいてもよい．@n
一方 bufszはバッファの長さを現す変数であり，プログラマが書き換えてはいけない．@n

Buffer型変数はバッファ領域を動的に確保するので,使用が終ったら必ず del_Buffer(または free_Buffer)でバッファ領域を開放しなければならない．

@attention
buf[bufsz]はシステムが使用するので,プログラマは buf[bufsz]は無いものとして扱うこと．

@see del_Buffer(), free_Buffer()
*/
typedef struct _Buffer {
    int   bufsz;           //!< 確保してあるバッファの大きさ - 1．
    int   vldsz;           //!< データの長さ．バイナリデータの場合も使用可能．文字列の場合は 0x00 を含まない．
    int   state;           //!< 変数の状態を表す．正常は JBXL_NORMAL
    unsigned char* buf;    //!< バッファの先頭へのポインタ．str[bufsz]は必ず 0x00となる．
} Buffer;


#define  BUFFER_FACT   (1.3)        ///< Buffer 変数の領域確保のための領域確保倍率


/** @def make_Buffer_bystr

文字列から，Buffer型変数のバッファ部をつくり出す．@n
make_Buffer_bystr("") は Buffer変数の初期化にも使用される．
バイナリデータから Buffer変数を作り出すには，set_Buffer()を用いる．@n
バッファ部の作成に失敗した場合, バッファのサイズ(bufsz)は 0, バッファへのポインタは NULLとなる．

@param  str Buffer型変数のバッファに格納する文字列
@return 作り出した Buffer型変数．
@see    set_Buffer()
*/
#define make_Buffer_bystr(str)      set_Buffer((void*)(str), -1)        ///< set_Buffer()
#define make_Buffer_bybin(str, len) set_Buffer((void*)(str), (len))     ///< set_Buffer()
#define make_Buffer_byrandomstr(sz) make_Buffer_randomstr(sz)           ///< make_Buffer_randomstr()

#define make_Buffer_str(str)        set_Buffer((void*)(str), -1)        ///< set_Buffer()
#define make_Buffer_bin(str, len)   set_Buffer((void*)(str), (len))     ///< set_Buffer()
#define make_Buffer_randomstr(sz)   make_Buffer_randomstr(sz)           ///< make_Buffer_randomstr()

// new_Buffer() -> new_Buffer(0)
Buffer* new_Buffer(int sz);                                 ///< 空のBuffer型変数を生成する．
Buffer  init_Buffer(void);                                  ///< 初期化したBuffer型変数を返す．
Buffer  make_Buffer(int sz);                                ///< Buffer型変数のバッファ部をつくり出す．
Buffer  make_Buffer_bychar(unsigned char cc);               ///< 文字から，Buffer型変数のバッファ部をつくり出す．
Buffer  make_Buffer_randomstr(int sz);                      ///< ランダムな n文字から，Buffer型変数のバッファ部をつくり出す．
Buffer  set_Buffer(void* buf, int sz);                      ///< Buffer型変数のバッファ部を新しく作り, そのバッファに bufをコピーする．
Buffer  dup_Buffer(Buffer buf);                             ///< Buffer型変数のコピーをつくる．
Buffer  rept_Buffer(unsigned char cc, int n);               ///< 文字 ccを n回繰り返したBuffer型データを返す．

void    expand_Buffer(Buffer* buf, int len);                ///< buf のデータ部を lenに拡張した新しい Buffer を返す．

void    del_Buffer(Buffer** buf);                           ///< new_Buffer()で作り出した Buffer型変数を削除する．
void    free_Buffer(Buffer* buf);                           ///< Buffer型変数のバッファ部を解放する
void    clear_Buffer(Buffer* buf);                          ///< Buffer型変数 のバッファ部を 0クリアする．

int     copy_Buffer(Buffer* src, Buffer* dst);              ///< Buffer型変数 srcから dstへバッファをコピーする．
int     cat_Buffer (Buffer* src, Buffer* dst);              ///< Buffer変数 srcから dstへバッファを catする．
int     ins_Buffer (Buffer* src, Buffer* dst);              ///< Buffer変数 dst の前に srcを挿入する．

int     copy_b2Buffer(void* src, Buffer* dst, int sz);      ///< 任意のバイナリデータsrcを Buffer型変数dstへ lenバイト copyする
int     cat_b2Buffer (void* src, Buffer* dst, int sz);      ///< 任意のバイナリデータsrcを Buffer型変数dstへ lenバイト catする．
int     ins_b2Buffer (void* src, Buffer* dst, int sz);      ///< 任意のバイナリデータsrcを Buffer型変数dstの前に lenバイト挿入する

int     copy_i2Buffer(int src, Buffer* dst);                ///< 整数 srcを文字列に変換して，dstへ copyする．
int     cat_i2Buffer (int src, Buffer* dst);                ///< 整数 srcを文字列に変換して，dstへ catする．
int     ins_i2Buffer (int src, Buffer* dst);                ///< 整数 srcを文字列に変換して，dstの前に 挿入する．


/** @def copy_s2Buffer

char*変数 srcから Buffer型変数dstへ文字列を copyする．@n
もし, dstに srcをコピーするだけのスペースが無い場合は, 新たにバッファ領域を確保してコピーする．

@param  src コピー元へのポインタ．
@param  dst コピー先へのポインタ．
@retval 0     srcまたは dstが NULL
@retval -1    メモリの確保に失敗した．何も実行されなかった．
@retval 1以上 dstのバッファにコピーされた文字数．
*/
#define copy_s2Buffer(src, dst)    copy_b2Buffer((void*)(src), (dst), -1)    ///< copy_b2Buffer()


/** @def cat_s2Buffer

char*変数 srcから Buffer型変数dstへ文字列を catする．@n
もし, dstに srcの文字列を catするだけのスペースが無い場合は新たにバッファ領域を確保して catする．

@param src cat元へのポインタ．
@param dst cat先へのポインタ．
@retval  0    srcまたは dstが NULL
@retval -1    メモリの確保に失敗した．何も実行されなかった．
@retval 1以上 dstのバッファにコピーされた文字数．
*/
#define cat_s2Buffer(src, dst)     cat_b2Buffer( (void*)(src), (dst), -1)    ///< cat_b2Buffer()
#define ins_s2Buffer(src, dst)     ins_b2Buffer( (void*)(src), (dst), -1)    ///< cat_b2Buffer()


// ツール
void    kanji_convert_Buffer(Buffer* mesg);                             ///< 大域変数 @b KanjiCode (tools.h) に従って漢字コードを変換する．@n
Buffer  encode_base64_Buffer(Buffer buf);                               ///< バイナリデータ buf.bufの buf.vldszバイトを Base64にエンコード する
Buffer  decode_base64_Buffer(Buffer buf);                               ///< strのバッファを Base64からデコードする
Buffer  encode_base64_filename_Buffer(Buffer buf, unsigned char cc);    ///< バイナリデータ bufを Base64で encodeしてファイル名を作る．ただし '/' は cc として扱う．
Buffer  decode_base64_filename_Buffer(Buffer buf, unsigned char cc);    ///< bufを Base64で decodeしてバイナリデータを取り出す．ただし cc は '/' として扱う． 

Buffer  get_line_Buffer(Buffer buf, int n);                             ///< 複数行の文字列バッファから任意の行を取り出す．
Buffer  get_seq_data_Buffer(Buffer str, int* ptr);                      ///< 複数行の文字列バッファ内の行データをシーケンシャルに取り出す．
Buffer  awk_Buffer (Buffer buf, char cc, int n);                        ///< Buffer文字列に対する awk．
Buffer  cawk_Buffer(Buffer buf, char cc, int n);                        ///< Buffer文字列に対する(変形の)awk．
Buffer  pack_Buffer(Buffer buf, char cc);                               ///< 文字列の先頭のcc(複数)，終わりのcc(複数)，TAB, CR, LF を削除

int     cmp_Buffer(Buffer src, Buffer dst, int n);                      ///< バッファ部の比較．
void    chomp_Buffer(Buffer* buf);                                      ///< 最初の改行コード以降を無視する．
int     isText_Buffer(Buffer buf);                                      ///< Buffer型変数 bufのバッファ部がテキストかどうか検査する．
Buffer  dump_Buffer(Buffer buf);                                        ///< Buffer型変数のデバッグ用 16進を Buffer型変数に出力する．
int     recalc_strlen_Buffer(Buffer* buf);                              ///< Buffer型変数のデータ部を文字列と見なして，その長さを返す．

void    add_indent_Buffer(Buffer* buf, char cc, int num);               ///< インデント(cc x num) を付加する．

#define strcmp_Buffer(a, b)            cmp_Buffer((a), (b), 0)          ///< cmp_Buffer()
#define strncmp_Buffer(a, b, n)        cmp_Buffer((a), (b), (n))        ///< cmp_Buffer()
#define bincmp_Buffer(a, b)            cmp_Buffer((a), (b), 0)          ///< cmp_Buffer()
#define binncmp_Buffer(a, b, n)        cmp_Buffer((a), (b), (n))        ///< cmp_Buffer()

Buffer  randstr_Buffer(int n);         //!< 英数字を n個生成
Buffer  randbit_Buffer(int n);         //!< ビットを n個生成 

// ファイルI/O
int     fgets_Buffer(Buffer* str, FILE* fp);            ///< 拡張fgets．文字列の読み込みに使用する．改行コードは削除する
int     read_lines_Buffer(Buffer* str, FILE* fp);       ///< 複数文字列行の読み込み．文字列の読み込みに使用する

Buffer  read_Buffer_file(const char* fn);               ///< ファイル fn の内容を Buffer型変数に読み込む．
Buffer  read_Buffer_data(FILE* fp, int sz);             ///< ファイルポインタ fp から szバイトをBuffer型変数に読み込む．
int     save_Buffer_file(Buffer buf, char* fn);         ///< ファイル fn へ Buffer型変数の buf部を書き込む

// 文字削除
Buffer  erase_bBuffer(Buffer buf, char* bin, int len);  ///< buf.buf 中にある bin のデータ（各1Byte，順不同）を削除する．
#define erase_Buffer(b, f)              erase_bBuffer((b), (char*)((f).buf), (f).vldsz) ///< erase_bBuffer()
#define erase_sBuffer(b, f)             erase_bBuffer((b), (char*)(f), -1)              ///< erase_bBuffer()

// 文字列置換
Buffer  replace_sBuffer_bystr(Buffer buf, const char* frm, const char* tos);        ///< buf.buf中の文字列 frmを tosで置き換えた 新しい Bufferを返す
int     replace_sBuffer_file(char* fn, Buffer frm, Buffer tos);                     ///< テキストファイル中の frm の文字列を tos に書き換える
#define replace_sBuffer_str(buf, f, t)  replace_sBuffer_bystr((buf), (f), (t))      ///< replace_sBuffer()
#define replace_sBuffer(buf, f, t)      replace_sBuffer_bystr((buf), (f), (t))      ///< replace_sBuffer()

// buf.buf自体を書き換える．
void    rewrite_sBuffer_bystr(Buffer* buf, const char* frm, const char* tos);       ///< buf.buf自体を書き換えること以外は replace_sBuffer_bystr() と同じ
#define rewrite_sBuffer_str(b, f, t)  rewrite_sBuffer_bystr((b), (f), (t))          ///< rewrite_sBuffer_bystr()
#define rewrite_sBuffer(b, f, t)  rewrite_sBuffer_bystr((b), (f), (t))              ///< rewrite_sBuffer_bystr()
#define rewrite_sBuffer_file(f, fr, to) replace_sBuffer_file((f), (fr), (to))       ///< replace_sBuffer_file()

void    rewrite_Buffer_bychar(Buffer* buf, const char frm, const char toc);         ///< buf.buf中の文字 frmを tocで置き換える

// ヘッダ(書式)付き特殊ファイルI/O
int     save_Buffer_fp(Buffer, FILE*);                              ///< ファイル fp へ Buffer型変数の buf部を書き込む
Buffer  read_Buffer_fp(FILE*);                                      ///< ファイル fp から Buffer型変数の buf部を読み込む
int     save_Buffer2_fp(Buffer  key, Buffer  buf, FILE* fp);        ///< ファイル fp へ 2つのBuffer型変数 keyと bufを書き込む
int     read_Buffer2_fp(Buffer* key, Buffer* buf, FILE* fp);        ///< ファイル fp から 2つのBuffer型変数の keyと bufを読み込む．

// Buffer型を利用した 簡易char型リングスタック
char    pop_char_ringStack(Buffer* buf);                            ///< 簡易 char型 ringStack POP
void    push_char_ringStack(Buffer* buf, char cc);                  ///< 簡易 char型 ringStack PUSH


#endif

