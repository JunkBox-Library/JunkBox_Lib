/**
@brief   Buffer型サポートプログラム 
@file    buffer.c
@version 1.3
@author  Fumi.Iseki (C)
@date    2012 7/23
@see     Buffer
*/

#include "buffer.h"
#include "jbxl_state.h"


/**
Buffer*  new_Buffer(int sz)

空のBuffer型変数を生成する．

@param  sz Buffer型変数のバッファの大きさ．0以下の場合は，バッファ部は作成しない．

@return つくり出した Buffer型変数へのポインタ．作成に失敗した場合はNULLが返る．
*/
Buffer*  new_Buffer(int sz)
{
    Buffer*  buf;

    buf = (Buffer*)malloc(sizeof(Buffer));
    if (buf==NULL) return NULL;

    memset(buf, 0, sizeof(Buffer));
    buf->vldsz = -1;
    buf->state = JBXL_NORMAL;

    if (sz>0) *buf = make_Buffer(sz + 1);
    return buf;
}


/**
Buffer  init_Buffer()

初期化したBuffer型変数を返す．

@attention a = init_Buffer() とした場合，aに元々値が入っていても，その値は freeされないので注意．
@return 初期化されたBuffer型変数．
*/
Buffer  init_Buffer()
{
    Buffer  buf;

    memset(&buf, 0, sizeof(Buffer));
    buf.vldsz = -1;
    buf.state = JBXL_NORMAL;

    return buf;
}


/**
Buffer  make_Buffer(int sz)

Buffer型変数のバッファ部をつくり出す．

sz は Buffer型変数のバッファの大きさ．@n
バッファ部は 0x00で初期化される．bufsz=sz, vldsz=0 となる．@n
バッファ部の作成に失敗した場合,バッファのサイズ(bufsz)は 0,バッファへのポインタは NULLとなる．

@param  sz Buffer型変数のバッファの大きさ．
@return 作り出した Buffer型変数．
*/
Buffer  make_Buffer(int sz)
{
    Buffer  buf;

    memset(&buf, 0, sizeof(Buffer));
    buf.state = JBXL_NORMAL;
    if (sz<0) return buf;

    buf.bufsz = sz;
    buf.buf = (unsigned char*)malloc(buf.bufsz+1);

    if (buf.buf==NULL) {
        buf.bufsz = 0;
        buf.vldsz = -1;
        buf.state = JBXL_ERROR;
    }
    else {
        memset(buf.buf, 0, buf.bufsz+1);
    }

    return buf;
}


/**
Buffer  make_Buffer_bychar(unsigned char cc)

文字から，Buffer型変数のバッファ部をつくり出す．

make_Buffer_bychar('@\0') は Buffer変数の初期化にも使用される．

@param  cc  Buffer型変数のバッファに格納する文字
@return つくり出した Buffer型変数．バッファ部の作成に失敗した場合,
@return バッファのサイズ(bufsz)は 0,バッファへのポインタは NULLとなる．

@see make_Buffer_bystr()
*/
Buffer  make_Buffer_bychar(unsigned char cc)
{
    Buffer  buf;
    
    buf = make_Buffer(LADDR);
    if (buf.buf==NULL) return buf;

    buf.buf[0] = cc;
    buf.vldsz = 1;
    return buf;
}


/**
void  free_Buffer(Buffer* buf)

Buffer型変数のバッファ部を解放する．

@param  buf  開放するバッファを持った Bufferデータへのポインタ．
*/
void  free_Buffer(Buffer* buf)
{
    if (buf!=NULL) {
        if (buf->buf!=NULL) free(buf->buf);
        buf->buf = NULL;
        memset(buf, 0, sizeof(Buffer));
        buf->vldsz = -1;
        buf->state = JBXL_NORMAL;
    }
}


/**
void   del_Buffer(Buffer** buf)

new_Buffer()で作り出した Buffer型変数を削除する．

@param  buf  削除する Bufferデータのポインタへのポインタ．
@see new_Buffer()
*/
void   del_Buffer(Buffer** buf)
{
    if (buf!=NULL && *buf!=NULL) {
        if ((*buf)->buf!=NULL) free((*buf)->buf);
        (*buf)->buf = NULL;
        free(*buf);
        *buf = NULL;
    }
}


/**
Buffer  set_Buffer(void* dat, int len)

Buffer型変数のバッファ部を新しく作り, そのバッファに bufをコピーする．@n
lenはコピーするデータ長．もし lenが 0未満の場合は, bufは文字列として扱われる．@n
buf が文字列の場合は make_Buffer_bystr() も使用可能．

@param  dat  コピーするバッファへのポインタ．
@param  len  バッファ長(バイト単位)．
@return 新しく作られた Buffer型データ．
*/
Buffer  set_Buffer(void* dat, int len)
{
    Buffer  buf;

    buf = init_Buffer();
    if (dat==NULL) return buf;

    if (len<0) len = (int)strlen((char*)dat);
    buf = make_Buffer(len);
    if (buf.buf==NULL) return buf;

    memcpy(buf.buf, dat, len);
    buf.vldsz = len;
    return  buf;
}


/**
Buffer  make_Buffer_randomstr(int n)

ランダムな n文字から，Buffer型変数のバッファ部をつくり出す．
*/
Buffer  make_Buffer_randomstr(int n)
{
    char* str = random_str(n);

    Buffer buf = set_Buffer(str, -1);
    free(str);

    return buf;
}


/**
Buffer  dup_Buffer(Buffer buf)

Buffer型変数のコピーをつくる．

@param  buf  コピーする変数．
@return 新しく作られた Buffer型データ．
*/
Buffer  dup_Buffer(Buffer buf)
{
    Buffer  str = buf;

    if (buf.bufsz>=0 && buf.buf!=NULL) {
        str.buf = (unsigned char*)malloc(buf.bufsz+1);
        memcpy(str.buf, buf.buf, buf.bufsz);
        str.buf[buf.bufsz] = '\0';
    }
    //
    return  str;
}


/**
Buffer  rept_Buffer(unsigned char cc, int n)

文字 ccを n回繰り返したBuffer型データを返す．

@param  cc  繰り返す文字．
@param  n   繰り返す回数．
@return 新しく作られた Buffer型データ．
*/
Buffer  rept_Buffer(unsigned char cc, int n)
{
    int  i;
    Buffer  str;
    
    if (n<=0) n = 1;
    str = make_Buffer(n);
    for (i=0; i<n; i++) str.buf[i] = cc;
    str.vldsz = n;
    return  str;
}


/**
void  expand_Buffer(Buffer* buf, int len)

buf のデータ部を lenに拡張した新しい Buffer を返す．@n
bug のデータ部は 解放される．free_Buffer()
*/
void  expand_Buffer(Buffer* buf, int len)
{
    if (buf->bufsz>len) return;

    Buffer expand = make_Buffer(len);
    copy_Buffer(buf, &expand);
    free_Buffer(buf);

    *buf = expand;
}


/**
void  clear_Buffer(Buffer* str)

Buffer型変数 のバッファ部を 0クリアする．

@param  str  クリアする Buffer型データへのポインタ．
*/
void  clear_Buffer(Buffer* str)
{
    if (str->buf!=NULL) memset(str->buf, 0, str->bufsz+1);
    str->vldsz = 0;
    str->state = JBXL_NORMAL;
}


/**
int  copy_Buffer(Buffer* src, Buffer* dst)

Buffer型変数 srcから dstへバッファをコピーする．

もし, dstに srcのバッファをコピーするだけのスペースが無い場合は, 新たにバッファ領域を確保してコピーする．

@param  src    コピー元へのポインタ．
@param  dst    コピー先へのポインタ．

@retval 0以上 dstのバッファにコピーされた文字数．
@retval JBXL_NULL_EEROR    srcまたは dstが NULL 
@retval JBXL_MALLOC_ERROR  メモリの確保に失敗した．何も実行されなかった．

@attention
注(重要): 関数内で使用する場合,参照型以外の引数に対してこの関数を用いてはいけない．
つまり以下のような使い方は間違い．
@code
    func(Buffer buf){ 
        ...............
        copy_Buffer(&src, &buf);
        ...............
    }
@endcode
これは下記のように変更する．
@code
    func(Buffer* buf){ 
        ...............
        copy_Buffer(&src, buf);
        ...............
    }
@endcode
理由：関数内でバッファ部の大きさが変わる可能性があるが，
呼び出した側ではそのことを知ることができない．
*/
int  copy_Buffer(Buffer* src, Buffer* dst)
{
    int sz, dz;

    if (src==NULL || dst==NULL) return JBXL_NULL_ERROR;
    if (src->buf==NULL) return JBXL_NULL_ERROR;

    sz = src->vldsz;
    if (sz<0)  sz = (int)strlen((const char*)src->buf);
    if (sz==0) return 0;

    if (dst->bufsz < sz) {
        unsigned char* buf;
        //dz  = (int)((src->bufsz+1)*BUFFER_FACT);
        dz  = (int)((sz+1)*BUFFER_FACT);
        buf = (unsigned char*)malloc(dz+1);
        if (buf==NULL) return JBXL_MALLOC_ERROR;
        memset(buf, 0, dz+1);

        free(dst->buf);
        dst->bufsz = dz;
        dst->buf   = buf;
    }

    //memset(dst->buf, 0, dst->bufsz+1);        too late
    memcpy(dst->buf, src->buf, sz);
    dst->buf[sz] = '\0';
    dst->vldsz = sz;

    return  sz;
}


/**
int  cat_Buffer(Buffer* src, Buffer* dst)

Buffer変数 srcから dstへバッファを catする．

もし, dstに srcの バッファを catするだけのスペースが無い場合は
新たにバッファ領域を確保して catする．

@param  src    cat元へのポインタ．
@param  dst    cat先へのポインタ．

@retval 0以上 dstのバッファにコピーされた文字数．
@retval JBXL_NULL_EEROR    srcまたは dstが NULL 
@retval JBXL_MALLOC_ERROR  メモリの確保に失敗した．何も実行されなかった．

@attention
注(重要): 関数内で使用する場合,参照型以外の引数に対してこの関数を用いてはいけない．
つまり以下のような使い方は間違い．
@code
    func(Buffer buf){ 
        ...............
        cat_Buffer(&src, &buf);
        ...............
    }
@endcode
これは下記のように変更する．
@code
    func(Buffer* buf){ 
        ...............
        cat_Buffer(&src, buf);
        ...............
    }
@endcode
理由：関数内でバッファ部の大きさが変わる可能性があるが，
呼び出した側ではそのことを知ることができない．
*/
int  cat_Buffer(Buffer* src, Buffer* dst)
{
    int sz, dz;

    if (src==NULL || dst==NULL) return JBXL_NULL_ERROR;
    if (src->buf==NULL) return JBXL_NULL_ERROR;

    sz = src->vldsz;
    if (sz<0) sz = (int)strlen((const char*)src->buf);
    if (sz==0) return 0;

    dz = dst->vldsz;
    if (dz<0) {
        if (dst->buf!=NULL) dz = (int)strlen((const char*)dst->buf);
        else                dz = 0;
    }

    if (dst->bufsz < sz+dz) {
        Buffer buf;
        buf.state = dst->state;
        buf.bufsz = (int)((dst->bufsz+sz+1)*BUFFER_FACT);
        buf.vldsz = dz;
        buf.buf   = (unsigned char*)malloc(buf.bufsz+1);
        if (buf.buf==NULL) return JBXL_MALLOC_ERROR;
   
        memcpy(buf.buf, dst->buf, dz);
        buf.buf[dz] = '\0';
        free_Buffer(dst);
        *dst = buf;
/*
        Buffer buf;                 // dstの待避 
        buf.bufsz = dst->bufsz;
        buf.vldsz = dz;
        buf.buf   = (unsigned char*)malloc(buf.bufsz+1);
        if (buf.buf==NULL) return JBXL_MALLOC_ERROR; 

        buf.buf[buf.bufsz] = '\0';
        memcpy(buf.buf, dst->buf, buf.bufsz);
        free_Buffer(dst);

        //dst->bufsz = (int)((buf.bufsz+src->bufsz+1)*BUFFER_FACT);
        dst->bufsz = (int)((buf.bufsz+sz+1)*BUFFER_FACT);
        dst->buf   = (unsigned char*)malloc(dst->bufsz+1);
        if (dst->buf==NULL) {
               *dst = buf;
               return JBXL_MALLOC_ERROR;
        }
        memset(dst->buf, 0, dst->bufsz+1);
        memcpy(dst->buf, buf.buf, buf.vldsz);
        free_Buffer(&buf);
*/
    }

    //memset(dst->buf+dz, 0, dst->bufsz-dz+1);  too late
    memcpy(dst->buf+dz, src->buf, sz);
    dst->buf[dz+sz] = '\0';
    dst->vldsz = sz + dz;

    return  dst->vldsz;
}


/**
int  ins_Buffer(Buffer* src, Buffer* dst)

Buffer変数 dst の前に src を挿入する．

@param  src    挿入元のポインタ．
@param  dst    挿入先へのポインタ．

@retval 0以上 dstのバッファにコピーされた文字数．
@retval JBXL_NULL_EEROR    srcまたは dstが NULL 
@retval JBXL_MALLOC_ERROR  メモリの確保に失敗した．何も実行されなかった．
*/
int  ins_Buffer(Buffer* src, Buffer* dst)
{
    Buffer tmp = make_Buffer(src->bufsz + dst->bufsz);

    int ret = copy_Buffer(src, &tmp);
    if (ret<0) {
        free_Buffer(&tmp);
        return ret;
    }

    ret = cat_Buffer(dst, &tmp);
    if (ret<0) {
        free_Buffer(&tmp);
        return ret;
    }

    free_Buffer(dst);
    *dst = tmp;

    return  dst->vldsz;
}


/**
int  copy_b2Buffer(void* src, Buffer* dst, int len)

任意のバイナリデータsrcを Buffer型変数dstへ lenバイト copyする．

もし, dstに srcのバッファを copyするだけのスペースが無い場合は新たにバッファ領域を確保して copyする．@n
lenが 0未満の場合は, srcは文字列として扱われる．

@param  src    copy元の任意データへのポインタ．
@param  dst    copy先のBuffer型変数へのポインタ．
@param  len    copyするバイト数．

@retval 0以上  dstのバッファにコピーされた文字数．
@retval JBXL_NULL_EEROR    srcまたは dstが NULL 
@retval JBXL_MALLOC_ERROR  メモリの確保に失敗した．何も実行されなかった．

@attention
注(重要): 関数内で使用する場合,参照型以外の引数に対してこの関数を用いてはいけない．
つまり以下のような使い方は間違い．
@code
    func(Buffer buf){ 
        ...............
        copy_b2Buffer(&src, &buf, n);
        ...............
    }
@endcode
これは下記のように変更する．
@code
    func(Buffer* buf){ 
        ...............
        copy_b2Buffer(&src, buf, n);
        ...............
    }
@endcode
理由：関数内でバッファ部の大きさが変わる可能性があるが，
func を呼び出した側ではそのことを知ることができない．
*/
int  copy_b2Buffer(void* src, Buffer* dst, int len)
{
    int sz, dz;

    if (src==NULL || dst==NULL) return JBXL_NULL_ERROR;
    if (len<0) sz = (int)strlen((const char*)src);
    else       sz = len;
    if (sz<=0) return 0;

    if (dst->bufsz < sz) {
        unsigned char* buf;
        dz  = (int)((sz+1)*BUFFER_FACT);
        buf = (unsigned char*)malloc(dz+1);
        if (buf==NULL) return JBXL_MALLOC_ERROR;
        memset(buf, 0, dz+1);

        if (dst->buf!=NULL) free(dst->buf);
        dst->bufsz = dz;
        dst->buf   = buf;
    }

    memcpy(dst->buf, src, sz);
    dst->buf[sz] = '\0';
    dst->vldsz = sz;
    dst->state = JBXL_NORMAL;

    return  sz;
}


/**
int  cat_b2Buffer(void* src, Buffer* dst, int len)

任意のバイナリデータsrcを Buffer型変数dstへ lenバイト catする．

もし, dstに srcのバッファを catするだけのスペースが無い場合は新たにバッファ領域を確保して catする．@n
lenが 0未満の場合は, srcは文字列として扱われる．

@param  src    cat元の任意データへのポインタ．
@param  dst    cat先のBuffer型変数へのポインタ．
@param  len    catするバイト数．

@retval 0以上 dstのバッファにコピーされた文字数．
@retval JBXL_NULL_EEROR    srcまたは dstが NULL 
@retval JBXL_MALLOC_ERROR  メモリの確保に失敗した．何も実行されなかった．

@attention
注(重要): 関数内で使用する場合,参照型以外の引数に対してこの関数を用いてはいけない．
つまり以下のような使い方は間違い．
@code
    func(Buffer buf){ 
        ...............
        cat_b2Buffer(&src, &buf, n);
        ...............
    }
@endcode
これは下記のように変更する．
@code
    func(Buffer* buf){ 
        ...............
        cat_b2Buffer(&src, buf, n);
        ...............
    }
@endcode
理由：関数内でバッファ部の大きさが変わる可能性があるが，
func を呼び出した側ではそのことを知ることができない．
*/
int  cat_b2Buffer(void* src, Buffer* dst, int len)
{
    int sz, dz;

    if (src==NULL || dst==NULL) return JBXL_NULL_ERROR;
    if (len<0) sz = (int)strlen((const char*)src);
    else       sz = len;
    if (sz<=0) return 0;
   
    dz = dst->vldsz;
    if (dz<0) {
        if (dst->buf!=NULL) dz = (int)strlen((const char*)dst->buf);
        else                dz = 0;
    }

    if (dst->bufsz < sz+dz) {
        Buffer buf;
        buf.state = dst->state;
        buf.bufsz = (int)((dst->bufsz+sz+1)*BUFFER_FACT);
        buf.vldsz = dz;
        buf.buf   = (unsigned char*)malloc(buf.bufsz+1);
        if (buf.buf==NULL) return JBXL_MALLOC_ERROR;
    
        memcpy(buf.buf, dst->buf, dz);
        buf.buf[dz] = '\0';
        free_Buffer(dst);
        *dst = buf;    
    }

    memcpy(dst->buf+dz, src, sz);
    dst->buf[dz+sz] = '\0';
    dst->vldsz = sz + dz;

    return  dst->vldsz;
}


/**
int  ins_b2Buffer(void* src, Buffer* dst, int len)

任意のバイナリデータsrcを Buffer型変数dstの前に lenバイト挿入する．@n
lenが 0未満の場合は, srcは文字列として扱われる．

@param  src    挿入元の任意データへのポインタ．
@param  dst    挿入先のBuffer型変数へのポインタ．
@param  len    挿入するバイト数．

@retval 0以上 dstのバッファにコピーされた文字数．
@retval JBXL_NULL_EEROR    srcまたは dstが NULL 
@retval JBXL_MALLOC_ERROR  メモリの確保に失敗した．何も実行されなかった．
*/
int  ins_b2Buffer(void* src, Buffer* dst, int len)
{
    Buffer tmp = make_Buffer(dst->bufsz+len);

    int ret = copy_b2Buffer(src, &tmp, len);
    if (ret<0) {
        free_Buffer(&tmp);
        return ret;
    }

    ret = cat_Buffer(dst, &tmp);
    if (ret<0) {
        free_Buffer(&tmp);
        return ret;
    }

    free_Buffer(dst);
    *dst = tmp;

    return  dst->vldsz;
}


/**
int  copy_i2Buffer(int src, Buffer* dst)

整数 srcを文字列に変換して，dstへ copyする．
*/
int  copy_i2Buffer(int src, Buffer* dst)
{
    char num[LEN_INT];

    snprintf(num, LEN_INT-1, "%d", src);
    return copy_b2Buffer((void*)num, dst, (int)strlen(num));
}


/**
int  cat_i2Buffer(int src, Buffer* dst)

整数 srcを文字列に変換して，dstへ catする．
*/
int  cat_i2Buffer(int src, Buffer* dst)
{
    char num[LEN_INT];

    snprintf(num, LEN_INT-1, "%d", src);
    return cat_b2Buffer((void*)num, dst, (int)strlen(num));
}


/**
int  ins_i2Buffer(int src, Buffer* dst)

整数 srcを文字列に変換して，dstの前に 挿入する．
*/
int  ins_i2Buffer(int src, Buffer* dst)
{
    char num[LEN_INT];

    snprintf(num, LEN_INT-1, "%d", src);
    return ins_b2Buffer((void*)num, dst, (int)strlen(num));
}


/**
int  cmp_Buffer(Buffer src, Buffer dst, int n)

バッファ部の比較．

Buffer変数 srcと dstのバッファ部分を比較する．@n
n<=0 なら 完全一致, n>0 なら 先頭から nバイト比較する．

@param src  比較元へのポインタ．
@param dst  比較先へのポインタ．
@param n    比較文字数．

@retval 0  srcと dstは一致している．
@retval 1  srcは dstと一致していない．
@retval JBXL_NULL_ERROR        バッファが NULL
@retval JBXL_BUFFER_CMP_ERROR  n が vldsz より大きい

@attention 文字列データのみ適用．コーディングがベタベタ．
*/
int  cmp_Buffer(Buffer src, Buffer dst, int n)
{
    int i;

    if (src.buf==NULL || dst.buf==NULL) return JBXL_NULL_ERROR;
    if (n>src.vldsz   || n>dst.vldsz)   return JBXL_BUFFER_CMP_ERROR;
    
    if (n<=0) {
        if (src.vldsz!=dst.vldsz) return 1;
        else n = src.vldsz;
    }
    
    for (i=0; i<n; i++) {
        if (src.buf[i]!=dst.buf[i]) return 1;
    }
    return 0;
}


/**
Buffer  encode_base64_Buffer(Buffer buf)

バイナリデータ buf.bufの buf.vldszバイトを Base64にエンコード する．

buf.vldszが -1以下の場合は buf.vldszは buf.bufの最初の 0x00のまでの長さ(strlen()+1)となる．@n
つまり buf.buf は文字列と 見なされる．

入力バッファ部が 3byte(8bit*3)の場合, 出力バッファ部は 4byte(6bit*4)となる．@n
入力バッファ部が 6bit境界でない場合, 6bit境界まで 0が付加されたと見される．@n 
出力バッファ部が 4byte境界でない場合, 4byte境界まで '='を付加して出力する．@n
また,出力バッファ部での 60byte毎の改行は行わない．@n
一般に n byte 入力された場合, Base64の出力の文字数は (n+2)/3*4 byte となる．

@param  buf  Base64にエンコードするデータ．
@return Base64にエンコードされた文字列(Buffer型)

@par 例
@code
    'A',0x00,0x01 を文字列(sz=1)とみなして符号化すると "QQ==" となる．
    'A',0x00,0x01 の 3byteを符号化(sz=3)すると "QQAB" となる．
@endcode
*/
Buffer  encode_base64_Buffer(Buffer buf)
{
    unsigned char* bas;
    int     sz;
    Buffer ecd = init_Buffer();
     
    //
    if (buf.buf==NULL) return ecd;
    if (buf.vldsz<0) sz = (int)strlen((const char*)buf.buf);
    else             sz = buf.vldsz;
    if (sz<=0) return ecd;

    bas = encode_base64(buf.buf, sz);
    if (bas==NULL) return ecd;
    
    sz  = ((sz + 2)/3)*4 + 1;
    ecd = make_Buffer(sz);
    if (ecd.buf==NULL) {
        free(bas);
        return ecd;
    }

    copy_s2Buffer((char*)bas, &ecd);

    free(bas);
    return ecd;
}


/**
Buffer  decode_base64_Buffer(Buffer str)

strのバッファを Base64からデコードする．

変換は str.bufの先頭から順次行われる．A-Za-z0-9+/ 以外は無視する(例えば改行コード)．@n
入力バッファ部が 4byte(6bit*4)の場合, 出力バッファ部は 3byte (8bit*3)となる．@n
入力バッファ部のバイト数が 4の倍数でない場合(不正なデータ), 足りない入力バッファ部には '='が挿入されているものとみなす．

@param  str  Base64からデコードする文字列．
@return Base64からデコードされたデータ(Buffer型)

@par 例
@code
    "QQ" をデコードすると 'A',0x00 となる．
@endcode
*/
Buffer  decode_base64_Buffer(Buffer str)
{
    unsigned char* bas;
    int    sz;
    Buffer dcd = init_Buffer();

    if (str.buf==NULL) return dcd;

    bas = decode_base64(str.buf, &sz);
    if (bas==NULL) return dcd;

    dcd = make_Buffer(sz);
    if (dcd.buf==NULL) {
        free(bas);
        return dcd;
    }

    copy_b2Buffer(bas, &dcd, sz);
    dcd.vldsz = sz;

    free(bas);
    return dcd;
}


/**
Buffer  encode_base64_filename_Buffer(Buffer buf, unsigned char cc)

バイナリデータ bufを Base64で encodeしてファイル名を作る．ただし '/' は cc として扱う．
*/
Buffer  encode_base64_filename_Buffer(Buffer buf, unsigned char cc)
{
    if (cc == 0x00) cc = '-';

    Buffer enc = encode_base64_Buffer(buf);

    int i;
    for (i=0; i<enc.vldsz; i++) {
       if (enc.buf[i]=='/') enc.buf[i] = cc;
    }

    return enc;
}


/**
Buffer  decode_base64_filename_Buffer(Buffer str, unsigned char cc)

bufを Base64で decodeしてファイル名からバイナリデータを取り出す．ただし cc は '/' として扱う．
*/
Buffer  decode_base64_filename_Buffer(Buffer str, unsigned char cc)
{
    if (cc == 0x00) cc = '-';

    int i;
    for (i=0; i<str.vldsz; i++) {
        if (str.buf[i]==cc) str.buf[i] = '/';
    }

    Buffer dec = decode_base64_Buffer(str);

    return dec;
}


/**
Buffer  get_line_Buffer(Buffer str, int n)

複数行の文字列バッファから任意の行を取り出す．

strのバッファ内の n行目を取り出す．改行コードは削除される．@n
n は 1から数える．取り出した行を Buffer型変数のバッファに格納（コピー）して返す．

@param  str  操作対象の文字列(含む改行)．
@param  n    行の指定．1から数える．
@return 指定された n行目の文字列(Buffer型)．改行は含まない．
*/
Buffer  get_line_Buffer(Buffer str, int n)
{
    int i, j, pos, cnt;
    unsigned char* buf;

    Buffer ret = init_Buffer();
    if (str.buf==NULL) return ret;

    buf = str.buf;
    for(i=0,j=0; j<n-1; j++) {
        while (buf[i]!=CHAR_LF && buf[i]!=CHAR_CR && buf[i]!='\0') i++;
        while (buf[i]==CHAR_LF || buf[i]==CHAR_CR) i++;
    }
    if (buf[i]=='\0') return ret;

    pos = i;
    while (buf[i]!=CHAR_LF && buf[i]!=CHAR_CR && buf[i]!='\0') i++;
    cnt = i - pos;

    if (cnt>=0) {
        ret = make_Buffer(cnt+1);
        if (ret.buf==NULL) return str;

        for(i=0; i<cnt; i++) ret.buf[i] = buf[pos+i];
        ret.buf[cnt] = '\0';
    }

    ret.vldsz = (int)strlen((const char*)ret.buf);

    return  ret;
}


/**
Buffer  get_seq_data_Buffer(Buffer str, int* ptr)

複数行の文字列バッファ内の行データをシーケンシャルに取り出す．

改行コードは削除される．連続して呼び出す場合，一番最初は *ptrに 0を設定し，以後は *ptrの内容は変えない．@n
取り出した行を Buffer型変数のバッファに格納して返す．@n
バッファを越えて行を読もうとした時は NULL を返す． バッファの終わりは valdszで知る．

@param[in]      str  操作対象文の Buffer型データ(含む改行)
@param[in,out]  ptr  in: 読み込みを開始するデータの位置（バイト）．@n
                     out: 次のデータが始まる位置（バイト）． 
@return              指定されたポインタ以下の一行分の文字列．改行は含まない．
*/
Buffer  get_seq_data_Buffer(Buffer str, int* ptr)
{
    int   i, pos, cnt;
    unsigned char* buf;

    Buffer ret = init_Buffer();

    if (str.buf==NULL) return ret;
    if (str.vldsz<=0) str.vldsz = (int)strlen((const char*)str.buf);
    if (*ptr >= str.vldsz) return ret;

    buf = str.buf;
    pos = *ptr;
    while (buf[*ptr]!=CHAR_LF && buf[*ptr]!=CHAR_CR && buf[*ptr]!='\0') (*ptr)++;
    cnt = *ptr - pos;

    if (cnt>=0) {
        ret = make_Buffer(cnt+1);
        if (ret.buf==NULL) return ret;

        for(i=0; i<cnt; i++) ret.buf[i] = buf[pos+i];
        ret.buf[cnt] = '\0';
    }

    ret.vldsz = (int)strlen((const char*)ret.buf);

    // 空行
    while (buf[*ptr]==CHAR_LF || buf[*ptr]==CHAR_CR) (*ptr)++;

    return  ret;
}


/**
Buffer  awk_Buffer(Buffer str, char cc, int n)

Buffer文字列に対する awk．

ccを区切り記号として, strのバッファ内の n番目の項目を返す．

@param  str  操作対象文字列．
@param  cc   区切り文字．
@param  n    項目の指定．1から数える．

@return 指定された n番目の項目の文字列(Buffer型)．
@see    cawk_Buffer(), awk(), cawk()
*/
Buffer  awk_Buffer(Buffer str, char cc, int n)
{
    int i, j, pos, cnt;
    unsigned char* buf = str.buf;
    Buffer item = init_Buffer();

    if (buf==NULL) return item;
    if (n<=0) n = 1;

    for(i=0,j=0; j<n-1; j++) {
        while (buf[i]!='\0' && buf[i]!=cc) i++;
        if (buf[i]==cc) i++;
    }
    if (buf[i]=='\0') return item;

    pos = i;
    while (buf[i]!='\0' && buf[i]!=cc) i++;
    cnt = i - pos;

    item = make_Buffer(cnt);
    if (item.buf==NULL) return item;

    for (i=0; i<cnt; i++) item.buf[i] = buf[pos+i];
    item.vldsz = (int)strlen((const char*)item.buf);

    return item;
}


/**
Buffer  cawk_Buffer(Buffer str, char cc, int n)

Buffer文字列に対する(変形の)awk．

ccを区切り記号として, strのバッファ内の n番目の項目を返す．@n
n は 1から数える．連続する cc(区切り)は一つの区切りとみなす．

@param  str  操作対象文字列．
@param  cc   区切り文字．
@param  n    項目の指定．1から数える．

@return 指定された n番目の項目の文字列(Buffer型)．
@see    awk_Buffer(), awk(), cawk()
*/
Buffer  cawk_Buffer(Buffer str, char cc, int n)
{
    int i, j, pos, cnt;
    unsigned char* buf = str.buf;
    Buffer item = init_Buffer();

    if (buf==NULL) return item;
    if (n<=0) n = 1;

    i = 0;
    for(j=0; j<n-1; j++) {
        while (buf[i]!='\0' && buf[i]!=cc) i++;
        while (buf[i]!='\0' && buf[i]==cc) i++;
    }
    if (buf[i]=='\0') return item;

    pos = i;
    while (buf[i]!='\0' && buf[i]!=cc) i++;
    cnt = i - pos;

    item = make_Buffer(cnt);
    if (item.buf==NULL) return item;

    for (i=0; i<cnt; i++) item.buf[i] = buf[pos+i];
    item.vldsz = (int)strlen((const char*)item.buf);

    return item;
}


/**
Buffer  pack_Buffer(Buffer buf, char cc)

文字列の先頭のcc(複数)，終わりのcc(複数)，TAB, CR, LF を削除@n
文字列の先頭，終わり部分でない場所にある ccは削除されない．

@param  buf  操作対象の Buffer型変数
@param  cc   削除する文字
@return 変換後の文字列を含んだ Buffer型変数．
*/
Buffer  pack_Buffer(Buffer buf, char cc)
{
    Buffer res = init_Buffer();

    if (buf.buf==NULL) return res;

    res.buf = (unsigned char*)pack_char((char*)buf.buf, cc);
    res.vldsz = (int)strlen((char*)res.buf);
    res.bufsz = res.vldsz;

    return res;
}


/**
void kanji_convert_Buffer(Buffer* str)

大域変数 @b KanjiCode (tools.h) に従って漢字コードを変換する．@n
@param[in,out] str  変換する文字列を持つBuffer型データ．内容が書き換えられる．

@attention 現在は KanjiCodeが SJISの場合のみ EUCを SJISに変換する．
@see       KanjiCode
*/
void kanji_convert_Buffer(Buffer* str)
{
    int i; 
    unsigned char *c1, *c2;

    if (KanjiCode==CODE_SJIS) {
        i = 0;
        while(str->buf[i]!='\0'){
            c1 = &(str->buf[i]);
            c2 = c1 + 1;
            if(*c1>=0xa1 && *c1<=0xfe){
                euc2sjis(c1, c2);    
                i = i + 2;
            } 
            else  i++;
        }
    }              
    /*
    else if (KanjiCode==CODE_EUC) {
        i = 0;
        while(str->buf[i]!='\0'){
            c1 = (unsigned char) str->buf[i];
            c2 = (unsigned char) str->buf[i+1];
            if((c1>=0x81 && c1<=0x9f) || (c1>=0xe0 && c1<=0xff)) {
                sjis2euc(c1, c2);    
                i = i + 2;
            } 
            else  i++;
        }
    } 
    */             
}


/**
Buffer  randstr_Buffer(int n)

ランダムに A-Za-z0-9+/ までの文字を n文字生成する．

@param  n  出力する文字数．
@return ランダムに生成された n文字の文字列(Buffer型)．
*/
Buffer  randstr_Buffer(int n)
{
    char   base[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    Buffer pass;
    int    i, sz;

    pass = make_Buffer(n);
    if (pass.buf==NULL) return pass;

    sz = (int)strlen(base);
    for (i=0; i<n; i++) pass.buf[i] = base[rand()%sz];
    pass.vldsz = n;

    return pass;
}


/**
Buffer  randbit_Buffer(int n) 

ランダムに n bitのバイナリを生成する．

@param  n  出力する bit数．
@return ランダムに生成された n bitのデータ(Buffer型)．
*/
Buffer  randbit_Buffer(int n) 
{
    int   i, sz;
    Buffer str;
    
    sz = (n+7)/8;
    str = make_Buffer(sz);
    if (str.buf==NULL) return str;
    str.vldsz = sz;

    for (i=0; i<n; i++) setBit(str.buf, i, rand()%2);
    return str;     
}


#define  DUMP_BUFFER_LINE_SIZE     85     // 16*5("%02x "+"%c ") + 5("   "+"\r\n");
#define  DUMP_BUFFER_DELI_START    48     // 16*3("%02x ") 
#define  DUMP_BUFFER_CHAR_START    51     // 16*3("%02x ") + 3("   ") 

/**
Buffer  dump_Buffer(Buffer buf)

Buffer型変数のデバッグ用 16進を Buffer型変数に出力する．

buf.bufの buf.vldszバイトを16進表示する．@n
buf.vldsz<0 の場合は 文字列とみなす．

@param  buf  変換する Buffer型データ．
@return ダンプデータが格納された Buffer型データ．
*/
Buffer  dump_Buffer(Buffer buf)
{
    int    sz;
    char   wrkbuf[10];
    Buffer str = init_Buffer();

    if (buf.buf==NULL) return str;
    if (buf.vldsz<0) sz = (int)strlen((const char*)buf.buf) + 1;
    else             sz = buf.vldsz;

    int lineno = (sz + 15)/16;
    str = make_Buffer(lineno*DUMP_BUFFER_LINE_SIZE+1);

    int l;
    for (l=0; l<lineno; l++) {
        int sp = l*DUMP_BUFFER_LINE_SIZE;
        int i  = 0;
        while (i<16) {
            sprintf(wrkbuf, "%02x ", buf.buf[l*16+i]);
            memcpy(str.buf+sp+i*3, wrkbuf, 3);

            if (buf.buf[l*16+i]>=0x20 && buf.buf[l*16+i]<=0x7e) {
                sprintf(wrkbuf, "%c ", buf.buf[l*16+i]);
                memcpy(str.buf+sp+DUMP_BUFFER_CHAR_START+i*2, wrkbuf, 2);
            }
            else {
                memcpy(str.buf+sp+DUMP_BUFFER_CHAR_START+i*2, ". ", 2);
            }

            if (l*16+i>=sz) {
                memcpy(str.buf+sp+i*3, "   ", 3);
                memcpy(str.buf+sp+DUMP_BUFFER_CHAR_START+i*2, "  ", 2);
            }

            i++;
        }
        memcpy(str.buf+sp+DUMP_BUFFER_DELI_START, "   ", 3);
        str.buf[sp+DUMP_BUFFER_LINE_SIZE-2] = '\r';
        str.buf[sp+DUMP_BUFFER_LINE_SIZE-1] = '\n';
    }
    
    str.vldsz = lineno*DUMP_BUFFER_LINE_SIZE;
    return str;
}


/**
int  recalc_strlen_Buffer(Buffer* buf)

Buffer型変数のデータ部を文字列と見なして，その長さを返す．@n
vldsz は再計算される．

@param[in]  buf  操作対象文字列 
@param[out] buf  @b vldsz 文字列の長さ．再計算される．
@return  文字列の長さ．
*/
int  recalc_strlen_Buffer(Buffer* buf)
{
    int len = (int)strlen((char*)buf->buf);
    buf->vldsz = len;
    return len;
}


/**
void    add_indent_Buffer(Buffer* buf, char* cc, int num);

buf にインデント(cc x num個) を付加する．

@param  buf  インデントを付加するバッファ．
@param  cc   インデントの文字．' ' や '\t'
param   len  インデントの文字数．
*/
void    add_indent_Buffer(Buffer* buf, char cc, int len)
{
    char* indent = (char*)malloc(len + 1);
    if (indent!=NULL) {
        memset(indent, cc, len);
        indent[len] = '\0';
        cat_s2Buffer(indent, buf);
        free(indent);
    }
}


/**
void  chomp_Buffer(Buffer* str)

最初の改行コード以降を無視する．@n
最初の改行コードに場所に，'@\0'が代入される．

@param[in,out] str  操作対象の文字列(Buffer型)．
*/
void  chomp_Buffer(Buffer* str)
{
    int i, len;
    
    if (str->buf==NULL) return;

    len = (int)strlen((const char*)str->buf);
    for (i=0; i<len; i++) {
        if (str->buf[i]==CHAR_LF || str->buf[i]==CHAR_CR) {
            str->buf[i] = '\0';
            str->vldsz = (int)strlen((const char*)str->buf);
            return;
        }
    }
}


/**
int  isText_Buffer(Buffer buf)

Buffer型変数 bufのバッファ部がテキストかどうか検査する．

@param  buf   検査対象の Buffer型変数．
@retval TRUE  バッファ部はテキストである．
@retval FALSE バッファ部はテキストではない．
*/
int  isText_Buffer(Buffer buf)
{
    for (int i=0; i<buf.vldsz; i++) {
        if (buf.buf[i]<0x20 && buf.buf[i]!=0x0a && buf.buf[i]!=0x0d 
                            && buf.buf[i]!=0x00 && buf.buf[i]!=0x09) return FALSE;
        if (buf.buf[i]==0x7f) return FALSE; // DEL
    }

    //if (buf.vldsz==(int)strlen((const char*)buf.buf)) return TRUE;
    return FALSE;
}



////////////////////////////////////////////////////////////////////////////////////
// File I/O
// 

/**
int  fgets_Buffer(Buffer* str, FILE* fp)

拡張fgets．文字列の読み込みに使用する．改行コードは削除する．@n
strのバッファは予め十分な大きさを確保しておかなければならない． 

@param  str  文字列を読みこむための Buffer型データバッファ．
@param  fp   文字列を読みこむファイルへのポインタ．
@return 読み込んだ文字列の長さ（改行コードを含む）．
*/
int  fgets_Buffer(Buffer* str, FILE* fp)
{
    char* p;
    int   n, m;
    UNUSED(p);

    memset(str->buf, 0, str->bufsz);
    p = fgets((char*)str->buf, str->bufsz, fp);
    m = (int)strlen((const char*)str->buf);

    n = 0;
    while(str->buf[n]!=CHAR_LF && str->buf[n]!=CHAR_CR && str->buf[n]!='\0') n++;
    str->buf[n] = '\0';
    str->vldsz  = n;

//    if (KanjiCode!=US) kanji_convert_Buffer(str);

    return m;
}


/**
int  read_lines_Buffer(Buffer* str, FILE* fp)

複数文字列行の読み込み．文字列の読み込みに使用する．@n
strのバッファは予め十分な大きさを確保しておかなければならない． 

@param  str  文字列を読みこむための Buffer型データバッファ．
@param  fp   文字列を読みこむファイルへのポインタ．
@return 読み込んだ全データの長さ．
*/
int  read_lines_Buffer(Buffer* str, FILE* fp)
{
    Buffer buf;
    char* pp;
    UNUSED(pp);

    buf = make_Buffer(str->bufsz);
    if (buf.buf==NULL) return JBXL_BUFFER_MAKE_ERROR;

    pp = fgets((char*)buf.buf, buf.bufsz, fp);
    buf.vldsz = (int)strlen((const char*)buf.buf);
    copy_Buffer(&buf, str);
    clear_Buffer(&buf);

    while (!feof(fp)) {
        pp = fgets((char*)buf.buf, buf.bufsz, fp);
        buf.vldsz = (int)strlen((const char*)buf.buf);
        cat_Buffer(&buf, str);
        clear_Buffer(&buf);
    }

    free_Buffer(&buf);
    return str->vldsz;
}


/**
Buffer  read_Buffer_file(const char* fn)

ファイル fn の内容を Buffer型変数に読み込む．

@param  fn  ファイル名
@return 読み込んだファイルの内容を保持する Buffer型変数
*/
Buffer  read_Buffer_file(const char* fn)
{
    int     sz;
    Buffer  buf;
    FILE*   fp;

    buf = init_Buffer();

    sz = file_size(fn);
    if (sz<=0) return buf;

    fp = fopen(fn, "rb");
    if (fp==NULL) return buf;

    buf = read_Buffer_data(fp, sz);
    fclose(fp);
    
    return buf;
}


/**
Buffer  read_Buffer_data(FILE* fp, int sz)

ファイルポインタ fp から szバイトをBuffer型変数に読み込む．

@param  fp  ファイルポインタ
@param  sz  読み込みサイズ
@return 読み込んだファイルの内容を保持する Buffer型変数
*/
Buffer  read_Buffer_data(FILE* fp, int sz)
{
    int     cc;
    Buffer  buf;

    buf = init_Buffer();
    if (sz<0) return buf;

    buf = make_Buffer(sz);
    if (buf.buf==NULL) return buf;
    buf.vldsz = sz;

    cc = (int)fread(buf.buf, buf.vldsz, 1, fp);
    if (cc!=1) {
        free_Buffer(&buf);
        return buf;
    }
    
    return buf;
}


/**
int   save_Buffer_file(Buffer buf, char* fn)

ファイル fn へ Buffer型変数の buf部を書き込む．

@param  buf   書き込む Buffer型変数．
@param  fn    ファイル名

@retval TRUE  書き込み成功．
@retval FALSE 書き込みに失敗．ファイルの内容は保証されない．
*/
int   save_Buffer_file(Buffer buf, char* fn)
{
    int   cc;
    FILE* fp;

    fp = fopen(fn, "wb");
    if (fp==NULL) return FALSE;

    cc = (int)fwrite(buf.buf, buf.vldsz, 1, fp);
    if (cc!=1) return FALSE;
    fclose(fp);
    
    return TRUE;
}



////////////////////////////////////////////////////////////////////////////////////
//  文字列操作
// 

/**
Buffer  erase_bBuffer(Buffer buf, char* bin, int len)

buf.buf 中にある bin のデータ（各1Byte，順不同）を削除する．

@param  buf  操作する Buffer変数
@param  bin  削除するデータを格納したメモリへのポインタ
@param  len  binデータの長さ（-1以下の場合は strlen(bin)）

@return 変換した文字列を格納した Buffer変数．要free
*/
Buffer  erase_bBuffer(Buffer buf, char* bin, int len)
{
    int     i, j, n;
    char cc;

    Buffer res = dup_Buffer(buf);
    if (bin==NULL) return res;
    Buffer wrk = make_Buffer(buf.vldsz + 1);

    if (len<0) len = (int)strlen(bin);
    for (i=0; i<len; i++) {
        cc = bin[i];
        n  = 0;
        for (j=0; j<res.vldsz; j++) {
            if (cc!=res.buf[j]) wrk.buf[n++] = res.buf[j];
        }
        wrk.vldsz = n;

        copy_Buffer(&wrk, &res);
        clear_Buffer(&wrk);
    }
                
    free_Buffer(&wrk);

    return res;
}


/**
Buffer  replace_sBuffer_bystr(Buffer buf, const char* frm, const char* tos)

buf.buf中の文字列 frmを tosで置き換えた 新しい Bufferを返す．@n
frm は buf.buf とメモリ領域が重なってはいけない．@n
文字列型の Bufferにのみ対応．

@param  buf  操作する Buffer変数
@param  frm  変換する文字列
@param  tos  変換後の文字列

@return 変換した文字列を格納した Buffer変数．state==JBXL_ERROR の場合は失敗．
*/
Buffer  replace_sBuffer_bystr(Buffer buf, const char* frm, const char* tos)
{
    int    i, j, flen, tlen;
    Buffer res;

    res = init_Buffer();
    if (buf.buf==NULL || frm==NULL || tos==NULL) return res;

    flen = (int)strlen(frm);
    tlen = (int)strlen(tos);
    if (buf.vldsz<=0) buf.vldsz = (int)strlen((const char*)buf.buf);

    int difsz = tlen - flen;
    if (difsz<0) difsz = 0;
    else         difsz = difsz*100;    // 100個まで
    res = make_Buffer((int)((buf.vldsz+difsz+1)*BUFFER_FACT));

    i = j = 0;
    while (i<buf.vldsz && j<res.bufsz) {
        if (!strncmp((const char*)(buf.buf+i), frm, flen)){
            res.vldsz = (int)strlen((const char*)res.buf);
            cat_s2Buffer(tos, &res);
            i += flen;
            j += tlen;
        }
        else {
            res.buf[j++] = buf.buf[i++];
        }
    }

    if (j==res.bufsz) res.state = JBXL_ERROR;
    res.vldsz = (int)strlen((const char*)res.buf);

    return res;
}


/**
int  replace_sBuffer_file(char* fn, Buffer frm, Buffer tos)

テキストファイル中の frm の文字列を tos に書き換える

@param  fn    操作するファイル名
@param  frm   変換する文字列
@param  tos   変換後の文字列

@retval TRUE　書き込み成功．
@retval FALSE 書き込みに失敗．ファイルの内容は保証されない．
*/
int  replace_sBuffer_file(char* fn, Buffer frm, Buffer tos)
{
    int  ret;
    Buffer buf, res;

    buf = read_Buffer_file(fn);
    if (buf.buf==NULL) return FALSE;

    res = replace_sBuffer(buf, (char*)frm.buf, (char*)tos.buf);
    free_Buffer(&buf);
    if (res.buf==NULL) return FALSE;

    ret = save_Buffer_file(res, fn);
    free_Buffer(&buf);

    return ret;
}


/**
void  rewrite_sBuffer_bystr(Buffer* buf, const char* frm, const char* tos)

buf.buf中の文字列 frmを tosで置き換える．@n
buf.buf自体を書き換えること以外は replace_sBuffer_bystr() と同じ．

@param  buf  操作する Buffer変数
@param  frm  変換する文字列
@param  tos  変換後の文字列
*/
void  rewrite_sBuffer_bystr(Buffer* buf, const char* frm, const char* tos)
{
    if (buf==NULL || frm==NULL || tos==NULL) return;

    Buffer tmp = replace_sBuffer_bystr(*buf, frm, tos);
    if (tmp.state==JBXL_NORMAL) {
        free_Buffer(buf);
        *buf = tmp;
    }
    return;
}


/**
void  rewrite_Buffer_bychar(Buffer* buf, const char frm, const char toc)

buf.buf中の文字 frmを tosで置き換える．@n

@param  buf  操作する Buffer変数
@param  frm  変換する文字
@param  toc  変換後の文字
*/
void  rewrite_Buffer_bychar(Buffer* buf, const char frm, const char toc)
{
    if (buf==NULL) return;

    for (int i=0; i<buf->vldsz; i++) {
        if (buf->buf[i]==frm) buf->buf[i] = toc;
    }
    return;
}



////////////////////////////////////////////////////////////////////////////////////
//  ヘッダ（書式）付特殊ファイルI/O
// 

/**
int   save_Buffer2_format_fp(Buffer key, Buffer buf, FILE* fp)

ファイル fp へ 2つのBuffer型変数 keyと bufを書き込む．@n
データ検索用ファイルを作成するときに使用する．

@param  key   書き込むキー部のBuffer型変数．
@param  buf   書き込むデータ部のBuffer型変数．
@param  fp    ファイルディスクリプタ

@retval TRUE　書き込み成功．
@retval FALSE 書き込みに失敗．ファイルの内容は保証されない．
*/
int   save_Buffer2_format_fp(Buffer key, Buffer buf, FILE* fp)
{
    int   cc;
    
    cc = save_Buffer_format_fp(key, fp);
    if (!cc) return FALSE;

    cc = save_Buffer_format_fp(buf, fp);
    if (!cc) return FALSE;
    
    return TRUE;
}


/**
int   save_Buffer_format_fp(Buffer buf, FILE* fp)

ファイル fp へ Buffer型変数の buf部を書き込む．@n
buf部に先立ち，buf部の大きさが intで書き込まれる．

@param  buf   書き込む Buffer型変数．
@param  fp    ファイルディスクリプタ

@retval TRUE　書き込み成功．
@retval FALSE 書き込みに失敗．ファイルの内容は保証されない．
*/
int   save_Buffer_format_fp(Buffer buf, FILE* fp)
{
    int   cc, sz;

    sz = htonl(buf.vldsz);
    cc = (int)fwrite(&sz, sizeof(sz), 1, fp);
    if (cc!=1) return FALSE;

    cc = (int)fwrite(buf.buf, buf.vldsz, 1, fp);
    if (cc!=1) return FALSE;
    
    return TRUE;
}


/**
Buffer  read_Buffer_format_fp(FILE* fp)

ファイル fp から Buffer型変数の buf部を読み込む．@n
ただしファイルの先頭にファイルサイズを示す4Byteの値が必要

@param  fp  ファイルディスクリプタ
@return 読み込んだ buf部を持つ Buffer型変数．
*/
Buffer  read_Buffer_format_fp(FILE* fp)
{
    int     cc, sz;
    Buffer  buf;

    buf = init_Buffer();
    cc = (int)fread(&sz, sizeof(sz), 1, fp);
    if (cc!=1) return buf;
    sz = ntohl(sz);

    buf = make_Buffer(sz);
    if (buf.buf==NULL) return buf;
    buf.vldsz = sz;
    cc = (int)fread(buf.buf, buf.vldsz, 1, fp);
    if (cc!=1) {
        free_Buffer(&buf);
        return buf;
    }
    
    return buf;
}


/**
int  read_Buffer2_format_fp(Buffer* key, Buffer* buf, FILE* fp)

ファイル fp から 2つのBuffer型変数の keyと bufを読み込む．@n
データ検索用ファイルからの読み込み時に使用する．

@param  key   読み込むキー部のBuffer型変数へのポインタ．
@param  buf   読み込むデータ部のBuffer型変数へのポインタ．
@param  fp    ファイルディスクリプタ

@retval TRUE  読み込み成功．
@retval FALSE 読み込みに失敗．
*/
int  read_Buffer2_format_fp(Buffer* key, Buffer* buf, FILE* fp)
{
    if (key==NULL || buf==NULL) return FALSE;

    *key = init_Buffer();
    *buf = init_Buffer();
    
    *key = read_Buffer_format_fp(fp);
    if (key->buf==NULL) return FALSE;

    *buf = read_Buffer_format_fp(fp);
    if (buf->buf==NULL) {
        free_Buffer(key);
        return FALSE;
    }

    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// etc. etc.
//

//
// get_char_ringbuffer() -> pop_char_ringStack()
// put_char_ringbuffer() -> push_char_ringStack()
//


/**
char  pop_char_ringStack(Buffer* buf)

簡易 char型 ringStack POP
*/
char  pop_char_ringStack(Buffer* buf)
{
    buf->vldsz--;
    if (buf->vldsz<0) buf->vldsz = buf->bufsz - 1;

    return  (char)buf->buf[buf->vldsz];
}


/**
void  push_char_ringStack(Buffer* buf, char cc)

簡易 char型 ringStack PUSH
*/
void  push_char_ringStack(Buffer* buf, char cc)
{
    buf->buf[buf->vldsz] = (unsigned char)cc;
    buf->vldsz++;
    if (buf->vldsz==buf->bufsz) buf->vldsz = 0;

    return;
}

