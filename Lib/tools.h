#ifndef  __JBXL_TOOLS_HEADER_
#define  __JBXL_TOOLS_HEADER_

/** 
@brief   汎用ツールヘッダ
@file    tools.h
@author  Fumi.Iseki (C)
@date    2020 10/27
*/

#include "common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <ctype.h>


#ifdef WIN32
    #include <winsock2.h>        // for ntohl, ntohs, htonl, htons
    #ifndef HAVE_TIME_H
        #define  HAVE_TIME_H
    #endif
#else
    #include <unistd.h>    
    #include <sys/sysinfo.h>
#endif


#ifdef HAVE_ARPA_INET_H
    #include <arpa/inet.h>
#endif

#ifdef TIME_WITH_SYS_TIME
    #include <time.h>
    #include <sys/time.h>
#else
    #ifdef HAVE_TIME_H 
        #include <time.h>
    #endif 
    #ifdef HAVE_SYS_TIME_H 
        #include <sys/time.h>
    #endif 
#endif

#ifdef HAVE_SYS_PARAM_H 
    #include <sys/param.h>
#endif

#ifdef HAVE_SYSLOG_H
    #include <syslog.h>
#endif

#ifdef HAVE_WAIT_H 
    #include <wait.h>
#endif
#ifdef HAVE_SYS_WAIT_H 
    #include <sys/wait.h>
#endif

#ifdef HAVE_SYS_ERRNO_H 
    #include <sys/errno.h>
#else
    #include <errno.h>
#endif
#include <signal.h>


#ifndef HUGE_VALF
#ifdef HUGE_VAL
#define HUGE_VALF ((double)HUGE_VAL)
#endif
#endif


/*
マクロ
*/
#define  CHAR_CR            0x0d        ///< 改行 
#define  CHAR_LF            0x0a        ///< ラインフィード 
#define  CHAR_TAB           0x09        ///< タブ 

#define  CODE_US            0
#define  CODE_EN            0
#define  CODE_EUC           1           //  
#define  CODE_SJIS          2           //  
#define  CODE_UTF8          3           //  

#define UNKNOWN_ENDIAN      0

#ifndef LITTLE_ENDIAN
  #define  LITTLE_ENDIAN    1234
  #define  BIG_ENDIAN       4321
  #define  PDP_ENDIAN       3412
#endif

#ifdef WIN32
  #define  CHAR_DELI_DIR    '\\'
  #define  STR_DELI_DIR     "\\"
  #define  STR_CRNT_DIR     ".\\"
  #define  STR_UPST_DIR     "..\\"
#else
  #define  CHAR_DELI_DIR    '/'
  #define  STR_DELI_DIR     "/"
  #define  STR_CRNT_DIR     "./"
  #define  STR_UPST_DIR     "../"
#endif


typedef union _union_val {
    long long int            llint;
    unsigned long long int   ullint;
    long int                 lint;
    unsigned long int        ulint;
    int                      nint;
    unsigned int             uint;
    short int                sint;
    unsigned short int       usint;
    char                     nchar;
    unsigned char            uchar;
    double                   dreal;
    float                    real;
    sDWord                   sdword;
    uDWord                   udword;
    sWord                    sword;
    uWord                    uword;
    sByte                    sbyte;
    uByte                    ubyte;
} unionVal;
    

typedef union _union_ptr {
    long long int*           llintp;
    unsigned long long int*  ullintp;
    long int*                lintp;
    unsigned long int*       ulintp;
    int*                     nintp;
    unsigned int*            uintp;
    short int*               sintp;
    unsigned short int*      usintp;
    char*                    ncharp;
    unsigned char*           ucharp;
    double*                  drealp;
    float*                   realp;
    sDWord*                  sdwordp;
    uDWord*                  udwordp;
    sWord*                   swordp;
    uWord*                   uwordp;
    sByte*                   sbytep;
    uByte*                   ubytep;
} unionPtr;
    


// バイト型 リングバッファ
typedef  struct _ringBuffer {
    unsigned char* buf;         ///< バッファ
    int   bufsz;                ///< バッファ部のサイズ
    int   spoint;               ///< バッファデータの開始点
    int   epoint;               ///< バッファデータの終点+1 (データの追加点)
    int   datasz;               ///< 保存されているデータの全体の長さ
    int   npoint;               ///< 次のメッセージのスタートポイント（メッセージデータ用）
    int   datano;               ///< 格納されているデータの数（メッセージデータ用）
    int   state;                ///< リングバッファの状態
} ringBuffer;


#define  mstream   ringBuffer   ///< バイト型 メッセージストリーム


// 矩形
typedef  struct _rectangle {
    int   x;                    ///< X 座標
    int   y;                    ///< Y 座標
    int   xsize;                ///< 幅
    int   ysize;                ///< 高さ
} rectAngle;


//
// 大域変数
//
extern int   DebugMode;         ///< ON の場合 ライブラリ全体がデバックモードであることを示す．デフォルトは OFF．
extern int   UnitTestMode;      ///< ユニット単体でのテストを行う場合に使用する．デフォルトは OFF.

extern int   KanjiCode;         ///< 処理用漢字コード．CODE_US, CODE_SJIS, CODE_EDUがある．デフォルトは CODE_US．
extern int   HostEndian;        ///< ホストの Endian

extern unsigned char  LocalIPNum[4];     ///< 127.0.0.1 のバイナリ
extern unsigned char  LocalIPNum6[16];   ///< ::1 のバイナリ

#define  LOCAL_IPv4     "127.0.0.1"
#define  LOCAL_IPv6     "::1"
#define  LOCAL_IPADDR   LOCAL_IPv4
#define  LOCAL_IPADDR6  LOCAL_IPv6


// not C++
#ifndef CPLUSPLUS
    int  isNull(void* p);
    #define freeNull(p)     {if(!isNull((void*)p)) free(p); (p)=NULL;}
    #define freenull(p)     {if(!isNull((void*)p)) free(p); (p)=NULL;}
#endif

#define free_null(p)   freeNull((p))

#define _tochar(a)   (char*)(a)



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 文字（列）操作
char*   get_line(char* buf, int n);                              ///< 文字型データbufのバッファ内の n行目を取り出す．改行コードは削除される．要 free()
char*   get_seq_data(char* buf, int* ptr);                       ///< 文字型データbufのバッファ内の 行データをシーケンシャルに取り出す．
char*   awk (char* str, char cc, int n);                         ///< ccを区切り記号として, strのバッファ内の n番目の項目を返す．要 free()
char*   cawk(char* str, char cc, int n);                         ///< 連続するccを区切り記号として, strのバッファ内の n番目の項目を返す．要 free()

char*   replace_str(char* buf, int len, const char* frm, const char* tos);    ///< 文字列 buf中の frmを tosに書き換えたものを返す．free() してはいけない．    
char*   cut_str(char* buf, int ls, int le);                      ///< buf[ls]〜buf[le] を切り出す．要 free()
char*   dup_str(char* buf);                                      ///< 文字列を複製する．要 free()
#define dup_string(s) dup_str((s))                               ///< dup_str()

char*   skip_chars(char* pp, const char* check);                 ///< check[]中の何れかの文字までポインタをスキップさせる．ただし クォーテーション内は完全スキップ
char*   skip_char(char* pp, char cc);                            ///< cc の文字までポインタをスキップさせる．ただし クォーテーション内は完全スキップ
char*   skip_char_pair(char* pp, char pair, char end);           ///< pair と end で閉じるまでポインタをスキップさせる．ただし クォーテーション内は完全スキップ
char*   skip_string_end(char* pp);                               ///< 次の文字列を一つスキップする．最期のクォーテーションの位置を返す．

char*   pack_char_len(char* pp, char cc, int len);               ///< 文字列の一部の先頭のcc(複数)，終わりのcc(複数)，CR, LF を削除．要 free()
char*   pack_head_tail_char(char* pp, char cc);                  ///< 文字の先頭のcc(複数)，TAB, CR, LF．終わりのcc(複数)，TAB, CR, LF を削除．要 free()
char*   change_esc(char* pp);                                    ///< 文字列中の CR, LF を @\r, @\n に変換する．要 free()
char*   chomp(char* str);                                        ///< 最初の改行コード以降を無効にする．
char*   double_bs(char* fn);                                     ///< 文字列中の @\ を @\@\に置き換えた文字列を返す．要 free()
char*   numbering_name(const char* fmt, int n);                  ///< フォーマット fmtに従って，nを含む文字列を作り出す．要 free()
#define pack_char(s, c)   pack_char_len((s), (c), -1)            ///< pack_char_len()

int     is_number(unsigned char*);                               ///< 数字かどうか判定する（簡易版）．整数(1) と小数点付き数字(2) のみ．

// thread unsafe
char*   itostr(int n);                                           ///< int を文字に変換する．free() は不要    
char*   ltostr(long int n);                                      ///< long int を文字に変換する．free() は不要    
char*   ultostr(unsigned long int n);                            ///< unsigned long int を文字に変換する．free() は不要    
char*   lltostr(long long int n);                                ///< long long int を文字に変換する．free() は不要    
char*   ulltostr(unsigned long long int n);                      ///< unsigned long long int を文字に変換する．free() は不要    
char*   ftostr(float n);                                         ///< float  を文字に変換する．free() は不要    
char*   dtostr(double n);                                        ///< double を文字に変換する．free() は不要    

// thread safe
char*   itostr_ts(int n);                                        ///< int を文字に変換する．要 free()   
char*   ltostr_ts(long int n);                                   ///< long int を文字に変換する．要 free()   
char*   ultostr_ts(unsigned long int n);                         ///< unsigned long int を文字に変換する．要 free()   
char*   lltostr_ts(long long int n);                             ///< long long int を文字に変換する．要 free()
char*   ulltostr_ts(unsigned long long int n);                   ///< unsigned long long int を文字に変換する．要 free() 
char*   ftostr_ts(float n);                                      ///< float  を文字に変換する．要 free()
char*   dtostr_ts(double n);                                     ///< double を文字に変換する．要 free()

int     count_lines(const char* buf);                            ///< 文字列データの行数を数える．行頭が '@\0'の場合も1行と数える．
int     hexstr2i(const char* str);                               ///< 16進の文字列を整数に変換する．
    
unsigned long long int ntohull(unsigned long long int s);        ///< Network形式からHost形式へ，64bit unsigned long long int データの変換 
#define htonull(s)    ntohull((s))                               ///< Host形式からNetwork形式へ，64bit unsigned long long int データの変換 ntohull()

void    swap_byte(void* p, int s, int b);                        ///< sの長さのpのバイト順序をbバイト毎に逆順にする．
short   swaps(unsigned short p);                                 ///< 16bit の上下8bitを入れ替える．
int     swapl(unsigned int   p);                                 ///< 32bit pを8bitづつ逆順にする
void    reverse_str(uByte* p, int s);                            ///< バイト(octet)列を逆順にする．
void    upper_string(char* str);
#define int_swap(p, s)     {int swap=(p); (p)=(s); (s)=swap;}    ///< データを入れ替える

// 比較
int     bincmp(unsigned char* b1, unsigned char* b2, int n);     ///< バイナリデータを n Byte比べる．一致するなら 0
int     strnrvscmp(const char* s1, const char* s2, int n);       ///< 文字列 s1と s2を後ろから n文字比較する．一致するなら 0
int     strncaservscmp(const char* s1, const char* s2, int n);   ///< 文字列 s1と s2を後ろから n文字比較する．大文字小文字は区別しない．一致するなら 0
char*   strstrcase(const char* buf, const char* nd);             ///< 文字列 bufの中に文字列 ndがあるかどうかをチェックする．大文字小文字は区別しない．

// 拡張 len => 1 以上 一致させる長さ． 0 完全一致． -1 dat の長さに合わせる． -2 key の長さに合わせる
int     ex_strncmp(const char* s1, const char* s2, int n);       ///< 文字列 s1とs2を拡張比較する．一致するなら TRUE
int     ex_strncasecmp(const char* s1, const char* s2, int n);   ///< 文字列 s1とs2を拡張比較する．大文字小文字を区別しない．一致するなら TRUE
int     ex_strnrvscmp(const char* s1, const char* s2, int n);    ///< 文字列 s1とs2を後ろから拡張比較する．一致するなら TRUE
int     ex_strncaservscmp(const char* s1, const char* s2, int n);///< 文字列 s1とs2を後ろから拡張比較する．一致するなら TRUE
#define ex_strcmp(a, b)    ex_strncasecmp((a), (b), -1)          ///< 文字列 aの長さに合わせて比較する．大文字小文字を区別しない．一致するなら TRUE

// エンディアン  (マクロ isBigEndianは ntohs() を使用したもの）
int     is_little_endian(void);                                  ///< エンディアンの動的チェック
int     is_big_endian(void);                                     ///< エンディアンの動的チェック
void    check_endian(void);                                      ///< システムのエンディアンを判別して，大域変数 HostEndian に設定する

double           double_from_little_endian(void* ptr);           ///< エンディアンによるデータ変換
float            float_from_little_endian(void* ptr);
int              int_from_little_endian(void* ptr);
unsigned int     uint_from_little_endian(void* ptr);
short            short_from_little_endian(void* ptr);
unsigned short   ushort_from_little_endian(void* ptr);
double           double_from_big_endian(void* ptr);
float            float_from_big_endian(void* ptr);
int              int_from_big_endian(void* ptr);
unsigned int     uint_from_big_endian(void* ptr);
short            short_from_big_endian(void* ptr);
unsigned short   ushort_from_big_endian(void* ptr);

// 時間
//char*   get_localtime(char c1, char c2, char c3, char c4);              ///< 現時刻の文字列を返す．free() してはいけない．
//char*   get_localtime_ts(char c1, char c2, char c3, char c4);           ///< 現時刻の文字列を返す．必ず free() すること!．
char*   get_local_timestamp(time_t date, const char* format);           ///< 要 free()
char*   get_gmt_timestamp(time_t date, const char* format);             ///< 要 free()

// 入出力
void    print_escape(const char* format, char* mesg);                   ///< エスケープ文字を含むメッセージの出力（stderr）
void    fdump(FILE* fp, unsigned char* mesg, int n);                    ///< 16進ダンプを吐き出す
void    print_16x(FILE* fp, unsigned char* mesg, int n);                ///< 16進ダンプを吐き出す（１行）

// 漢字コード
void    kanji_convert(unsigned char* mesg);                             ///< 大域変数 KnjiCodeに従って漢字コードを変換する．
void    kanji_convert_euc2sjis(unsigned char* mesg);                    ///< EUCをSJISに変換する．
void    kanji_convert_sjis2euc(unsigned char* mesg);                    ///< SJISをEUCに変換する．
void    euc2sjis(unsigned char *c1, unsigned char *c2);                 ///< EUC  -> SJIS
void    sjis2euc(unsigned char *c1, unsigned char *c2);                 ///< SJIS -> EUC

// ファイル
int     file_from_to(const char* s, const char* d, const char* mode);   ///< modeに従って,ファイル sを dにコピーする．
int     fp_from_to(FILE* s, FILE* d, long int sz);                      ///< ファイル sを dにコピーする．
char*   get_file_name(const char* str);                                 ///< フルパスからファイル名へのポインタを取り出す．free() してはいけない．
char*   get_file_extension(const char* str);                            ///< フルパスからファイル名の拡張子へのポインタを取り出す．free() してはいけない．
char*   del_file_name(const char* str);                                 ///< フルパスを書き換えてファイル名部分を削除する．free() してはいけない．
char*   del_file_extension(const char* str);                            ///< フルパスを書き換えて拡張子を削除する．free() してはいけない．

//char*   make_file_path(const char* str);                              ///< 使用停止，get_file_path を使え．/// フルパスからパスを生成する．strは変化しない．要 free()．
char*   get_file_path(const char* str);                                 ///< ファイル名を含むパスからパスのみを生成する．strは変化しない．要 free()．
char*   cut_file_extension(const char* str);                            ///< 拡張子を削除したフパス名を生成する．strは変化しない．要 free()．

unsigned long   file_size(const char* fn);                              ///< ファイルの大きさを返す．
int             file_exist(const char* fn);                             ///< ファイルの存在を検査する．
FILE*           file_chmod_open(const char* fn, const char* fm, mode_t mode); ///< ファイルの許可属性をmode へ変更した後，ファイルを fmモードでオープン
char*           temp_filename(const char*, int);                        ///< /dev/urandom を利用して作業用ファイルのランダムなファイル名を得る．

unsigned char*  read_file (const char* fname, unsigned long int* size);                     ///< ファイルからデータを読み込む
long int        write_file(const char* fname, unsigned char* buf, unsigned long int size);  ///< ファイルにデータを書き込む

// 乱数
void    init_rand(void);                                                ///< /dev/urandom からデータで乱数の系列を初期化する
char*   randstr(int n);                                                 ///< ランダムに A-Za-z0-9 までの文字を n文字生成する．
unsigned char* randbit(int n);                                          ///< ランダムに n bitのバイナリを生成する．
#define random_str(n)   temp_filename(NULL, (n))                        ///< ランダムな文字列を生成．要 free(). see temp_filename()

// Base64
unsigned char*  encode_base64(unsigned char* str, int  sz);             ///< バイナリデータ bufを base64にエンコードする．要 free()
unsigned char*  decode_base64(unsigned char* str, int* sz);             ///< bufを base64からデコードする．要 free()
unsigned char*  encode_base64_filename(unsigned char* str, int  sz, unsigned char cc); ///< バイナリデータ bufを Base64で encodeしてファイル名を作る．ただし '/' は cc として扱う．要 free()
unsigned char*  decode_base64_filename(unsigned char* str, int* sz, unsigned char cc); ///< bufを Base64で decodeしてバイナリデータを取り出す．ただし cc は '/' として扱う．要 free()
unsigned char*  encode_urlenc(unsigned char* str, int  sz);             ///< バイナリデータ bufを URLエンコードする．要 free()
unsigned char*  decode_urlenc(unsigned char* str, int* sz);             ///< buf を URLエンコードからデコードする．要 free()
unsigned char*  encode_quoted_printable(unsigned char* str, int  sz);   ///< バイナリデータ bufを quoted printable にエンコードする．要 free()
unsigned char*  decode_quoted_printable(unsigned char* str, int* sz);   ///< buf を quoted printableからデコードする．要 free()
unsigned char*  encode_hex(unsigned char cc);                           ///< キャラクタを16進コードの文字列に変換する．要 free()
unsigned char   decode_hex(unsigned char c1, unsigned char c2);         ///< 16進コードをキャラクタに変換する．O-9,A-F以外が指定された場合は0とみなす． 内容を書き換える．

// バイト型リングバッファー
ringBuffer*     new_ringBuffer(int sz);                                 ///< リングバッファを生成する．
ringBuffer      init_ringBuffer(void);                                  ///< リングバッファを初期化する．
ringBuffer      make_ringBuffer(int sz);                                ///< バッファ部が存在するリングバッファを作り出す．
void            del_ringBuffer  (ringBuffer** pp);                      ///< リングバッファそのものを削除する．new_ringBuffer() と対．
void            free_ringBuffer (ringBuffer* pp);                       ///< リングバッファを解放する．make_ringBuffer() と対．
void            clear_ringBuffer(ringBuffer* pp);                       ///< リングバッファのデータを削除する．
int             put_ringBuffer(ringBuffer* rb, unsigned char* pp, int sz);  ///< リングバッファにデータを格納する．
unsigned char*  get_ringBuffer(ringBuffer* rb, int sz);                 ///< リングバッファからデータを得る．要 free()
int             seek_ringBuffer(ringBuffer* rb, int sz);                ///< リングバッファのデータポインタを seek する．
unsigned char*  ref_ringBuffer(ringBuffer* ring, int pos);              ///< spoint 地点から pos番目のデータへの参照．posは 0から数える．spoint は変化しない．


// メッセージストリーム
int             put_mstream(mstream* sb, unsigned char* pp);            ///< メッセージ（文字列）ストリーム sb へメッセージ（の一部）を格納する
unsigned char*  get_mstream(mstream* sb);                               ///< メッセージ（文字列）ストリーム sb から次のメッセージを取り出す．改行コードは削除される．
unsigned char*  fgets_mstream(unsigned char* mesg, mstream* sb);        ///< メッセージストリームからメッセージを一行だけ取り出す．

#define  init_mstream()       init_ringBuffer()         ///< init_ringBuffer()
#define  new_mstream(s)       new_ringBuffer((s))       ///< new_ringBuffer()
#define  make_mstream(s)      make_ringBuffer((s))      ///< make_ringBuffer()
#define  free_mstream(s)      free_ringBuffer((s))      ///< free_ringBuffer()
#define  del_mstream(s)       del_ringBuffer((s))       ///< del_ringBuffer()
#define  clear_mstream(s)     clear_ringBuffer((s))     ///< clear_ringBuffer()

// UUID
unsigned char*  uuid2guid(unsigned char* p);            ///< uuid を guid に変換する．要 free()
unsigned char*  guid2uuid(unsigned char* p);            ///< guid を uuid に変換する．要 free()


#ifndef WIN32
// プロセス
unsigned long int get_used_memory(void);                ///< 使用中のメモリサイズを得る．
unsigned long int get_free_memory(void);                ///< 未使用のメモリサイズを得る．

void    set_sigterm_child(void (*handler)(int));        ///< child プロセス終了時の処理を設定．
void    sigterm_child(int signal);                      ///< child プロセス終了時の処理

void    ignore_sigterm_child(void);                     ///< 以後 child プロセスの終了を無視する．

void    set_sigsegv_handler(void (*handler)(int));      ///< セグメンテーションエラー時のハンドラを設定
void    trap_segmentation_falt(int signal);             ///< セグメンテーションエラー時にデフォルトで呼び出される関数．

#define set_sigseg_handler(h)  set_sigsegv_handler((h)) ///< set_sigsegv_handler()

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// マクロ関数

#define copy_file(src, dst)  file_from_to((src), (dst), "wb")    ///< srcから dstへファイルをコピーする．file_from_to()
#define cat_file(src, dst)   file_from_to((src), (dst), "ab")    ///< srcから dstへファイルを catする．  file_from_to()

#define  isBigEndian    (is_big_endian())
#define  isLittleEndian (is_little_endian())

#define  ntoh_st(p, s) {if(!isBigEndian) swap_byte((void*)(p),sizeof(*(p)),(s));}    ///< network形式から host形式へ．構造体pに対して sバイトづつ変換．
#define  hton_st(p, s) {if(!isBigEndian) swap_byte((void*)(p),sizeof(*(p)),(s));}    ///< host形式から network形式へ．構造体pに対して sバイトづつ変換．
#define  ntoh_ar(p, s) {if(!isBigEndian) swap_byte((void*)(p),(s),sizeof(*(p)));}    ///< network形式から host形式へ．長さsバイトの配列pに対して変換
#define  hton_ar(p, s) {if(!isBigEndian) swap_byte((void*)(p),(s),sizeof(*(p)));}    ///< host形式から network形式へ．長さsバイトの配列pに対して変換．

#define  ntoh_data(p, s, c) {if(!isBigEndian) swap_byte((void*)(p),(s),(c));}        ///< network形式からhost形式へ.  cバイトづつ変換する．
#define  hton_data(p, s, c) {if(!isBigEndian) swap_byte((void*)(p),(s),(c));}        ///< host形式から network形式へ. cバイトづつ変換する．


/////////////////////////////////////////////////////////////////////////////////////
// ログファイル

#ifdef WIN32
    #define  JBXL_LOG_FILE   "\\jbxl.log"           ///< デフォルトのログファイル
    #define  JBXL_ERR_FILE   "\\jbxl.err"           ///< デフォルトのエラーファイル
#else
    #define  JBXL_LOG_FILE   "/var/log/jbxl.log"    ///< デフォルトのログファイル
    #define  JBXL_ERR_FILE   "/var/log/jbxl.err"    ///< デフォルトのエラーファイル
#endif


extern  FILE*  FP_LogFile;
extern  FILE*  FP_ErrFile;

void    open_logfile (void);                ///< デフォルトのログファイルをオープンする
void    close_logfile(void);                ///< デフォルトのログファイルをクローズする
void    print_logfile(const char*, ...);    ///< デフォルトのログファイルにデータを書き込む

void    open_errfile (void);                ///< デフォルトのエラーファイルをオープンする
void    close_errfile(void);                ///< デフォルトのエラーファイルをクローズする
void    print_errfile(const char*, ...);    ///< デフォルトのエラーファイルにデータを書き込む


/////////////////////////////////////////////////////////////////////////////////////
// メッセージ関数

void     print_message(const char*, ...);                   ///< バッファリングなしのメッセージ出力（stderr）
void    fprint_message(FILE*, const char*, ...);            ///< バッファリングなしのメッセージ出力


// 環境依存用の出力関数
#ifdef WIN32
    #define  PRINT_MESG(...)    {}                          ///< 環境依存用の出力関数．MS Windows用は未実装
    #define  PRINT_ESC(...)     {}                          ///< 環境依存用の出力関数．MS Windows用は未実装
    #define  PRINT_INFO(...)    {}                          ///< 環境依存用の出力関数．MS Windows用は未実装
    #define  PRINT_WARN(...)    {}                          ///< 環境依存用の出力関数．MS Windows用は未実装
    #define  PRINT_ERROR(...)   {}                          ///< 環境依存用の出力関数．MS Windows用は未実装
#else 
    #define  PRINT_MESG         print_message               ///< 環境依存用の出力関数．print_message()
    #define  PRINT_ESC          print_escape                ///< 環境依存用の出力関数．print_escape()
    #define  PRINT_INFO         print_message               ///< 環境依存用の出力関数．print_message()
    #define  PRINT_WARN         print_message               ///< 環境依存用の出力関数．print_message()
    #define  PRINT_ERROR        print_message               ///< 環境依存用の出力関数．print_message()
#endif



/////////////////////////////////////////////////////////////////////////////////////
// for Debug
//

#ifdef _DEBUG
    #ifndef EBUG
        #define EBUG
    #endif
#endif

#ifdef EBUG
    #ifndef _DEBUG
        #define _DEBUG
    #endif
#endif


//
#define  DEBUG_MODE     if(DebugMode==ON)
#define  NO_DEBUG_MODE  if(DebugMode==OFF)


/////////////////////////////////////////////////////////////////////////////////
//　コンパイル時設定デバッグ用
//        不要になった場合（EBUGを指定しない場合），コードは削除される

#ifdef  EBUG     // -DEBUG
    #define  DEBUG_INFO         PRINT_INFO      ///< デバッグ用出力関数．コンパイル時に削除可能
    #define  DEBUG_WARN         PRINT_WARN      ///< デバッグ用出力関数．コンパイル時に削除可能
    #define  DEBUG_ERROR        PRINT_ERROR     ///< デバッグ用出力関数．コンパイル時に削除可能
    #define  DEBUG_MESG         PRINT_MESG      ///< デバッグ用出力関数．コンパイル時に削除可能
#else  
    #define  DEBUG_INFO(...)    {}              ///< デバッグ用出力関数．コンパイル時に削除可能
    #define  DEBUG_WARN(...)    {}              ///< デバッグ用出力関数．コンパイル時に削除可能
    #define  DEBUG_ERROR(...)   {}              ///< デバッグ用出力関数．コンパイル時に削除可能
    #define  DEBUG_MESG(...)    {}              ///< デバッグ用出力関数．コンパイル時に削除可能
#endif


#endif  // __JBXL_TOOLS_HEADER_

