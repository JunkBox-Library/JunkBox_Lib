
#ifndef  __JBXL_CIPHER_H_
#define  __JBXL_CIPHER_H_


/** 
@brief   暗号用ライブラリヘッダ
@file    cipher.h
@author  Fumi.Iseki (C)
*/


#include "buffer.h"


#ifdef HAVE_UNISTD_H
    #ifndef _XOPEN_SOURCE
        #define _XOPEN_SOURCE
    #endif
    #include <unistd.h>
#endif

#ifdef HAVE_CRYPT_H
    #include <crypt.h>
#endif


#ifndef _XOPEN_SOURCE
    #define _XOPEN_SOURCE
#endif


void   setkey_byBase64(Buffer);
void   setkey_byBuffer(Buffer);

Buffer encrypt_Base64(Buffer, int);
Buffer encrypt_Buffer(Buffer, int);

Buffer to_bin64(Buffer);
Buffer from_bin64(Buffer);
void   dump_bin64(char*, Buffer);


#endif

