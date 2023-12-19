

/**
@brief    Log用 Ring Buffer ツール
@file     LogRingBuffer.cpp
@author   Fumi.Iseki (C)
*/


#include "LogRingBuffer.h"


using namespace jbxl;


CLogRingBuffer::~CLogRingBuffer(void)
{
    //DEBUG_INFO("DESTRUCTOR: CLogRingBuffer");

    if (pBuf!=NULL) del_Buffer_dim(&pBuf);

    //DEBUG_INFO("DESTRUCTOR: CLogRingBuffer");
}



int  CLogRingBuffer::init(int size)
{
    bool  err_flag = false;

    maxBufSize = tlDataSize = 0;
    maxLineX   = maxLineY   = 0;
    rPos = wPos = 0;
    indentSize = 13;    // 5 + 8  "1234 SERVER: "

    pBuf     = (Buffer*)malloc(sizeof(Buffer)*size);
    kindData = (int*)malloc(sizeof(int)*size);
    inputSrc = (int*)malloc(sizeof(int)*size);

    if (pBuf!=NULL && inputSrc!=NULL && kindData!=NULL) {
        memset(pBuf,      0, sizeof(Buffer)*size);
        memset(kindData, 0, sizeof(int)*size);
        memset(inputSrc, 0, sizeof(int)*size);
        maxBufSize = size;

        for (int i=0; i<size; i++) {
            pBuf[i] = make_Buffer(LBUF);
            if (pBuf[i].buf==NULL) {
                err_flag = true;
                break;
            }
            pBuf[i].state = size;
        }
    }
    else {
        err_flag = true;
    }

    if (err_flag) {
        if (pBuf!=NULL) del_Buffer_dim(&pBuf);
        if (kindData!=NULL) free(kindData);
        if (inputSrc!=NULL) free(inputSrc);
        kindData = inputSrc = NULL;
        return 1;
    }

    return 0;
}



void  CLogRingBuffer::clear()
{
    for (int i=0; i<maxBufSize; i++) {
        pBuf[i].buf[0] = '\0';
        pBuf[i].vldsz  = 0;
    }

    tlDataSize = 0;
    maxLineX = maxLineY = 0;
    rPos = wPos = 0;

    return;
}



/**
void  CLogRingBuffer::putRingBuffer(Buffer buf, int input, int kind)

バッファの最後にデータを追加する．

@param buf   追加するデータ．
@param input データのタグ LOG_RB_MESG, LOG_RB_INFO, LOG_RB_DEBUG, LOG_RB_WARN, LOG_RB_ERR
@param kind  データの種類 LOG_RB_UNKNOWN_DATA, LOG_RB_TEXT_DATA, LOG_RB_TEXT_HALF_DATA, LOG_RB_BINARY_DATA, LOG_RB_BINHEX_DATA
*/

void  CLogRingBuffer::putRingBuffer(Buffer buf, int input, int kind)
{
    int  i,  nxt = 0;
    bool recalcX = false;
    char num[10];

    if (buf.buf==NULL || buf.vldsz<=0) return;
    Buffer* dim = decompline_Buffer_dim(buf, ON);
    if (dim==NULL || dim->state<=0) return;
    tlDataSize += buf.vldsz;    

#ifdef DEBUG
    int total = 0;
    for (i=0; i<dim->state; i++) {
        total += dim[i].vldsz;
    }
    if (buf.vldsz!=total) {
        DEBUG_ERR("CLogRingBuffer::putRingBuffer(): ERROR: mismatch total data size!! %d %d", buf.vldsz, total);
    }
#endif
    
    // 前の行の続き
    int pos = wPos - 1;
    if (pos<0) pos += maxBufSize;
    if (kindData[pos]==LOG_RB_TEXT_HALF_DATA && isText_Buffer(dim[0]) 
            && inputSrc[pos]==input && input!=LOG_RB_INFO && kind==LOG_RB_UNKNOWN_DATA) {
        cat_Buffer(&dim[0], &pBuf[pos]);
        kindData[pos] = LOG_RB_TEXT_DATA;
        maxLineX = Max(maxLineX, getLengthX(pos));
        nxt = 1;    // 処理済
    }
    
    // 新しい行．
    pos = wPos - 1; 
    for (i=nxt; i<dim->state; i++) {
        pos++;      // == wPos
        if (pos>=maxBufSize) pos -= maxBufSize;
        if (!recalcX && maxLineY==maxBufSize && maxLineX==getLengthX(pos)) recalcX = true;

        snprintf(num, 10, "%04d| ", pos);
        copy_s2Buffer(num, &pBuf[pos]);
        if      (input==LOG_RB_ERROR)   cat_s2Buffer("ERROR: ", &pBuf[pos]);
        else if (input==LOG_RB_WARNING) cat_s2Buffer("WARN : ", &pBuf[pos]);
        else if (input==LOG_RB_DEBUG)   cat_s2Buffer("DEBUG: ", &pBuf[pos]);
        else if (input==LOG_RB_INFO)    cat_s2Buffer("INFO : ", &pBuf[pos]);
        //else                          cat_s2Buffer("        ", &pBuf[pos]);

        cat_Buffer(&dim[i], &pBuf[pos]);
        if (kind==LOG_RB_UNKNOWN_DATA) kindData[pos] = LOG_RB_TEXT_DATA;
        else                           kindData[pos] = kind;
        inputSrc[pos] = input;
        maxLineX = Max(maxLineX, getLengthX(pos));
    }

    // 最後のデータの再確認
    if (nxt<dim->state && kind==LOG_RB_UNKNOWN_DATA) {
        if (isText_Buffer(dim[dim->state-1])) {
            unsigned char end = dim[dim->state-1].buf[dim[dim->state-1].vldsz-1]; 
            if (end!=CHAR_CR && end!=CHAR_LF) kindData[pos] = LOG_RB_TEXT_HALF_DATA;
        }
        else {
            kindData[pos] = LOG_RB_BINARY_DATA;
        }
    }

    pos++;
    if (pos>=maxBufSize) pos -= maxBufSize;
    wPos = pos;

    // 次のn行を目印行にする．
    Buffer spbuf = rept_Buffer(' ', 80);
    cat_s2Buffer("\n", &spbuf);
    for (i=0; i<2; i++) {
        copy_Buffer(&spbuf, &pBuf[pos]);
        inputSrc[pos] = LOG_RB_INFO;
        kindData[pos] = LOG_RB_TEXT_DATA;
        pos++;
        if (pos>=maxBufSize) pos -= maxBufSize;
    }
    free_Buffer(&spbuf);

    // コンテキストのサイズを計算
    if (maxLineY<maxBufSize) {
        if (wPos+1<maxLineY) maxLineY = maxBufSize;
        else  maxLineY = wPos + 1;
    }
    //DEBUG_WARN("MaxY = %d", maxLineY);
    
    if (recalcX) {
        maxLineX = 0;
        for (i=0; i<maxBufSize; i++) {
            maxLineX = Max(maxLineX, getLengthX(i));
        }
    }

    del_Buffer_dim(&dim);
    return;
}



/**
void  CLogRingBuffer::rewriteBinHexRingBuffer(int n, int input)

バッファ位置 nにあるデータを 16進表示で書き直す．
主に，位置 nにあるデータがバイナリの場合に使用する．
*/
void  CLogRingBuffer::rewriteBinHexRingBuffer(int n, int input)
{
    while (n<0) n += maxBufSize;
    if (n>=maxBufSize) n = n%maxBufSize;

    Buffer dup = pBuf[n];
    dup.buf    = dup.buf   + indentSize;
    dup.vldsz  = dup.vldsz - indentSize;

    Buffer buf = dump_Buffer(dup);
    wPos = n;
    putRingBuffer(buf, input, LOG_RB_BINHEX_DATA);

    free_Buffer(&buf);
}



/**
void  CLogRingBuffer::putRingFormat(int input, char* fmt, ...)

可変数変数用の書き込み用関数．バッファの最後に生成された文字列を追加する．
*/
void  CLogRingBuffer::putRingFormat(int input, char* fmt, ...)
{
    char*  nfmt;
    int    len;
    Buffer buf;

    va_list  args;
    va_start(args, fmt);

    len  = (int)strlen(fmt);    
    nfmt = (char*)malloc(len+1);
    if (nfmt==NULL) return;

    strncpy(nfmt, fmt, len);
    nfmt[len] = '\0';

    buf = make_Buffer(LBUF);
    vsnprintf((char*)buf.buf, LBUF-1, nfmt, args);
    recalc_strlen_Buffer(&buf);
    free(nfmt);

    va_end(args);

    putRingBuffer(buf, input, LOG_RB_TEXT_DATA);
    free_Buffer(&buf);
    
    return;
}



/**
void  CLogRingBuffer::putRingFormat(int input, char* fmt, va_list args)

可変数変数用の書き込み用関数．バッファの最後に生成された文字列を追加する．
上位の関数から呼び出して使用する．
*/
void  CLogRingBuffer::putRingFormat(int input, char* fmt, va_list args)
{
    char*  nfmt;
    int    len;
    Buffer buf;

    len  = (int)strlen(fmt);    
    nfmt = (char*)malloc(len+1);
    if (nfmt==NULL) return;

    strncpy(nfmt, fmt, len);
    nfmt[len] = '\0';

    buf = make_Buffer(LBUF);
    vsnprintf((char*)buf.buf, LBUF-1, nfmt, args);
    recalc_strlen_Buffer(&buf);
    free(nfmt);

    putRingBuffer(buf, input, LOG_RB_TEXT_DATA);
    free_Buffer(&buf);
    
    return;
}



/**
Buffer  CLogRingBuffer::getRingBuffer(void)
    
バッファの先頭のデータを読み込む．データポインタを移動させる．

@return バッファの先頭のデータ
*/
Buffer  CLogRingBuffer::getRingBuffer(void)
{
    Buffer buf = make_Buffer(LBUF);

    copy_Buffer(&pBuf[rPos], &buf);
    buf.state = inputSrc[rPos];

    rPos++;
    if (rPos>=maxBufSize) rPos -= maxBufSize;

    return buf;
}



/**
Buffer  CLogRingBuffer::getRingBuffer(int pos)

バッファの位置を指定して読み込む．データポインタの移動無し．

@return 指定した位置のデータ
*/
Buffer  CLogRingBuffer::getRingBuffer(int pos)
{
    Buffer buf = init_Buffer();
    buf.state  = -1;

    if (pos<0) return buf;
    if (pos>=maxBufSize) pos = pos%maxBufSize;

    buf = make_Buffer(LBUF);
    copy_Buffer(&pBuf[pos], &buf);
    buf.state = inputSrc[pos];

    return buf;
}

