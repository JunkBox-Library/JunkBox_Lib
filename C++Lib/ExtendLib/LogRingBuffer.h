
#ifndef  __JBXL_CPP_LOG_RING_BUFFER_H__
#define  __JBXL_CPP_LOG_RING_BUFFER_H__



/**
@brief    Log用 Ring Buffer ツール ヘッダ
@file     LogRingBuffer.h
@author   Fumi.Iseki (C)
*/



#include "xtools.h"


//
namespace jbxl {


// kindData
#define  LOG_RB_UNKNOWN_DATA        0
#define  LOG_RB_TEXT_DATA           1
#define  LOG_RB_TEXT_HALF_DATA      2
#define  LOG_RB_BINARY_DATA         3
#define  LOG_RB_BINHEX_DATA         4

// inputSrc
#define  LOG_RB_MESG                0
#define  LOG_RB_MESSAGE             0
#define  LOG_RB_INFO                1
#define  LOG_RB_INFORMATION         1
#define  LOG_RB_DEBUG               2
#define  LOG_RB_WARN                3
#define  LOG_RB_WARNING             3
#define  LOG_RB_ERR                 4
#define  LOG_RB_ERROR               4



class CLogRingBuffer
{
public:
    CLogRingBuffer(int size) { init(size);}
    virtual ~CLogRingBuffer(void);

public:
    int     init(int size);
    void    clear(void);
    void    putRingBuffer(Buffer buf, int input=LOG_RB_MESG, int kind=LOG_RB_UNKNOWN_DATA);
    void    rewriteBinHexRingBuffer(int n, int input);
    void    putRingFormat(int input, char* fmt, ...);
    void    putRingFormat(int input, char* fmt, va_list args);

    Buffer  getRingBuffer(void);        // バッファの先頭を読み込む．データポインタを移動させる．
    Buffer  getRingBuffer(int pos);     // バッファの位置を指定して読み込む．データポインタの移動無し．

    int     getMaxBufSize(void)     { return maxBufSize;}
    int     getMaxLineX(void)       { return maxLineX;}
    int     getMaxLineY(void)       { return maxLineY;}
    int     getTotalSize(void)      { return tlDataSize;}
    int     getLastPosition(void)   { return wPos;}

    int     getLengthX(int n)       { if(n<0) n+=maxBufSize; return (int)strlen((const char*)pBuf[n%maxBufSize].buf);}
    int     getKindData(int n)      { if(n<0) n+=maxBufSize; return kindData[n%maxBufSize];}

public:
    Buffer* pBuf;

protected:
    int     maxBufSize;     // Buffer の数．ラインの数．
    int     tlDataSize;

    int     maxLineX;       // コンテキストのXサイズ
    int     maxLineY;       // コンテキストのYサイズ
    int     indentSize;     // 

    int     rPos;
    int     wPos;

    int*    kindData;       //
    int*    inputSrc;       //
};



}       // namespace


#endif

