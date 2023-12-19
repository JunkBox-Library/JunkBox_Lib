

/**
@brief    汎用リングバッファライブラリ
@file     RingBuffer.cpp
@author   Fumi.Iseki (C)
*/



#include  "RingBuffer.h"


using namespace jbxl;




void  CRingBuffer::init_data(void)
{
    buf     = NULL;
    bufsz   = 0;
    datasz  = 0;
    spoint  = 0;
    epoint  = 0;
    datano  = 0;
    state   = 0;
    enable  = FALSE;
}



BOOL  CRingBuffer::init(int ring_size, int data_size)
{
    init_data();

    buf = (void**)malloc(ring_size*sizeof(void*));
    if (buf==NULL) return FALSE;
    memset(buf, 0, ring_size*sizeof(void*));
    bufsz  = ring_size;
    datasz = data_size;

    BOOL ret = TRUE;
    for (int i=0; i<bufsz; i++) {
        buf[i] = (void*)malloc(datasz);
        if (buf[i]==NULL) {
            this->free();
            ret = FALSE;
            state = -1;
            break;
        }
        memset(buf[i], 0, datasz);
    }

    if (ret) enable = TRUE;

    return ret;
}



void  CRingBuffer::free(void)
{
    if (!isNull(buf)) {
        for (int i=0; i<bufsz; i++) {
            if (!isNull(buf[i])) {
                ::free(buf[i]);
            }
        }
        ::free(buf);
    }    
    init_data();

    return;
}



void  CRingBuffer::clear(void)
{
    for (int i=0; i<bufsz; i++) {
        memset(buf[i], 0, datasz);
    }
    spoint  = 0;
    epoint  = 0;
    datano  = 0;
    state   = 0;

    return;
}



void* CRingBuffer::get(void)
{
    void* ptr = NULL;

    if (datano>0) {
        ptr = buf[spoint];
        spoint++;
        if (spoint==bufsz) spoint = 0;
        datano--;
    }

    return ptr;
}



void* CRingBuffer::get(int pos)
{
    pos = epoint + pos;
    if (pos<0) pos = bufsz + pos % bufsz;
    else       pos = pos % bufsz;

    return buf[pos];
}



void  CRingBuffer::put(void* ptr)
{
    memcpy(buf[epoint], ptr, datasz);
    epoint++;
    if (epoint==bufsz) epoint = 0;
    if (datano<bufsz)  datano++;

    return;
}



void  CRingBuffer::put(void* ptr, int pos)
{
    pos = epoint + pos;
    if (pos<0) pos = bufsz + pos % bufsz;
    else       pos = pos % bufsz;

    memcpy(buf[pos], ptr, datasz);

    return;
}




