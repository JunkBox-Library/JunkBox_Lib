#ifndef  __JBXL_DH_TOOL_H_
#define  __JBXL_DH_TOOL_H_

#include "xtools.h"
#include "asn1_tool.h"

#ifndef HAVE_OPENSSL_SSL_H
#ifndef DISABLE_SSL
#define DISABLE_SSL
#endif
#endif

#ifdef  DISABLE_SSL
#undef  ENABLE_SSL
#endif


#ifdef  ENABLE_SSL


/**
@brief   Diffie-Hellman用 ライブラリヘッダ
@file    dh_tool.h
@author  Fumi.Iseki (C)

@par コンパイルオプション
--I/usr/local/ssl/include -L/usr/local/ssl/lib -lcrypto

@attention
このプログラムは OpenSSL を使用しています．@n
This product includes software developed by the OpenSSL Project
for use in the OpenSSL Toolkit. (http://www.openssl.org/)
*/ 

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/core_names.h>
#endif


#ifdef  WIN32
#pragma  comment(lib, "openssl.lib")
#endif

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    EVP_PKEY* JBXL_DH_new(OSSL_PARAM* params, int ks);

    #define  JBXL_DH            EVP_PKEY
    #define  JBXL_DH_free(p)    EVP_PKEY_free((p))
    #define  JBXL_DH_size(p)    EVP_PKEY_get_size((p));
#else 
    #define  JBXL_DH            DH
    #define  JBXL_DH_free(p)    DH_free((p))
    #define  JBXL_DH_size(p)    DH_size((p));
#endif


// Diffie-Hellman

int      save_DHspki_with_private(Buffer  pki, FILE* fp, JBXL_DH* dhkey);
Buffer   read_DHspki_with_private(FILE* fp, JBXL_DH** p_dhkey);

Buffer   get_DHspki_ff(char* fn, int sz, JBXL_DH** p_dhkey);
#define  get_DHspki_file(p, s, d)    get_DHspki_ff((p), (s), (d))

Buffer   gen_DHspki(int ks, JBXL_DH** p_dhkey);
Buffer   gen_DHspki_fs(Buffer pki, JBXL_DH** p_dhkey);

Buffer   get_DHsharedkey   (Buffer pki,  JBXL_DH* dhkey);
Buffer   get_DHsharedkey_fY(Buffer ykey, JBXL_DH* dhkey);

Buffer   get_DHYkey(Buffer param);
Buffer   get_DHPkey(Buffer param);
Buffer   get_DHGkey(Buffer param);
Buffer   get_DHalgorism(Buffer param);
Buffer   get_DHprivatekey(JBXL_DH* dhkey);

Buffer   join_DHpubkey(Buffer param, Buffer key);


#endif        //  DISABLE_SSL

#endif        // __JBXL_SSL_TOOL_H_

