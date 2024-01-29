#ifndef  __JBXL_CPP_RINGBUFFER_H_
#define  __JBXL_CPP_RINGBUFFER_H_

/**
@brief    汎用リングバッファライブラリ ヘッダ
@file     RingBuffer.h
@author   Fumi.Iseki (C)
*/

#include "tools++.h"


namespace jbxl {


class CRingBuffer
{
public:
    CRingBuffer(void) { init_data();}
    CRingBuffer(int rsz, int dsz) { init(rsz, dsz);}
    virtual ~CRingBuffer(void)  { free();}

protected:
    void**  buf;

    int     bufsz;
    int     datasz;
    int     spoint;
    int     epoint;
    int     datano;

public:
    int     state;
    BOOL    enable;

protected:
    void    init_data(void);

public:
    BOOL    init(int ring_size, int data_size);
    void    free(void);
    void    clear(void);

    void*   get(void);
    void*   get(int pos);
    void    put(void* ptr);
    void    put(void* ptr, int pos);
};


}        // namespace


#endif
