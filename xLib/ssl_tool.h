
#ifndef  __JBXL_SSL_TOOL_H_
#define  __JBXL_SSL_TOOL_H_


#include "xtools.h"
#include "asn1_tool.h"

//#include "dh_tool.h"


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
@brief   SSL暗号用ライブラリヘッダ
@file    ssl_tool.h
@author  Fumi.Iseki (C)

@par コンパイルオプション
--I/usr/local/ssl/include -L/usr/local/ssl/lib -lcrypto

@attention
このプログラムは OpenSSL を使用しています．@n
This product includes software developed by the OpenSSL Project
for use in the OpenSSL Toolkit. (http://www.openssl.org/)
*/ 

#define OPENSSL_NO_KRB5 

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>


#ifdef  WIN32
#pragma  comment(lib, "openssl.lib")
#endif


// Key Exchange Algorism
#define SSL_DH               1
#define SSL_RSA              2


//
#define  SSL_ENC_BLCKSZ      128        // ブロック暗号化のブロックサイズ

// CRYPT Algorism
#define SSL_AES128CBC        1
#define SSL_3DES3CBC         10


#define SSL_IV_SIZE          16


//
int      gen_CRYPT_SharedKey(int keyex, Buffer spki, Buffer* shdkey, void* ptr);


// データ（含むバイナリ）送受信用(IPv4)（暗号＋復号）
int      udp_send_crypt_Buffer_sockaddr_in(int sock, Buffer* data, struct sockaddr_in* sv, Buffer* key, EVP_CIPHER* cipher);
int      udp_recv_crypt_Buffer_sockaddr_in(int sock, Buffer* data, struct sockaddr_in* sv, Buffer* key, EVP_CIPHER* cipher);

int      udp_send_crypt_Buffer(int sock, Buffer* data, struct addrinfo* sv, Buffer* key, EVP_CIPHER* cipher);
int      udp_recv_crypt_Buffer(int sock, Buffer* data, struct addrinfo* sv, Buffer* key, EVP_CIPHER* cipher);
int      tcp_send_crypt_Buffer(int sock, Buffer* data, Buffer* key, EVP_CIPHER* cipher);
int      tcp_recv_crypt_Buffer(int sock, Buffer* data, Buffer* key, EVP_CIPHER* cipher);

Buffer   get_plain_Buffer(Buffer data, Buffer* key, EVP_CIPHER* cipher);
Buffer   get_crypt_Buffer(Buffer data, Buffer* key, EVP_CIPHER* cipher);



// メッセージ（テキスト）送信用（暗号＋Base64）
int      tcp_send_crypt_mesg   (int sock, char*   mesg, Buffer* key, EVP_CIPHER* cipher);
int      tcp_send_crypt_sBuffer(int sock, Buffer* mesg, Buffer* key, EVP_CIPHER* cipher);

// メッセージ（テキスト）変換用（暗号化＋Base64）
// get_plain_ は tcp_send_crypt_, udp_send_crypt_ のデータ受信にも使用可．
Buffer   get_plain_message(char*  mesg, Buffer* key, EVP_CIPHER* cipher);
Buffer   get_plain_sBuffer(Buffer mesg, Buffer* key, EVP_CIPHER* cipher);
Buffer   get_crypt_message(char*  mesg, Buffer* key, EVP_CIPHER* cipher);
Buffer   get_crypt_sBuffer(Buffer mesg, Buffer* key, EVP_CIPHER* cipher);
#define  get_plain_sBuffer_str(m, k, c)   get_plain_message((m), (k), (c));
#define  get_crypt_sBuffer_str(m, k, c)   get_crypt_message((m), (k), (c));


// Client's Side Check
int      check_server_spki(Buffer ip, Buffer spki, char* fn);
int      save_spki_with_ipaddr(Buffer ipa, Buffer pki, FILE* fp);
Buffer   read_spki_with_ipaddr(Buffer ipa, FILE* fp);


// EVP
EVP_CIPHER* init_EVPAPI_Buffer(int type);
Buffer   decode_EVPAPI_Buffer(Buffer buf, Buffer shkey, EVP_CIPHER* cipher);
Buffer   encode_EVPAPI_Buffer(Buffer buf, Buffer shkey, EVP_CIPHER* cipher);
void     free_EVP_CIPHER(EVP_CIPHER** p_cipher);


// SSL/TLS
SSL_CTX* ssl_client_setup(char* ca);
SSL*     ssl_client_socket(int sock, SSL_CTX* ssl_ctx,  int mode);
#define  ssl_client_connect(s, c, m)  ssl_client_socket((s), (c), (m))

SSL_CTX* ssl_server_setup(char* crt_fn, char* key_fn, char* chn_fn);
SSL*     ssl_server_socket(int sock, SSL_CTX* ssl_ctx);

int      ssl_read_fullchain_cert_file(SSL_CTX* ssl_ctx, char* file);
int      ssl_add_chain_file(SSL_CTX* ssl_ctx, char* file);

void     ssl_init(void);
void     ssl_close(SSL* ssl);
tList*   ssl_get_cert_info(SSL* ssl);

int      ssl_recv(SSL* ssl, char* rmsg, int size);
int      ssl_send(SSL* ssl, char* smsg, int size);
int      ssl_recv_wait(int sock, SSL* ssl, char* mesg, int sz, int tm);
int      ssl_send_mesgln(SSL* ssl, char* mesg);
int      ssl_recv_mstream(int sock, SSL* ssl, char* mesg, int sz, mstream* sb, int tm);

int      ssl_tcp_recv(int sock, SSL* ssl, char* rmsg, int size);
int      ssl_tcp_send(int sock, SSL* ssl, char* smsg, int size);
int      ssl_tcp_recv_wait(int sock, SSL* ssl, char* mesg, int sz, int tm);
int      ssl_tcp_send_mesgln(int sock, SSL* ssl, char* mesg);
int      ssl_tcp_recv_mstream(int sock, SSL* ssl, char* mesg, int sz, mstream* sb, int tm);


/**    
int  ssl_tcp_send_mesg(int sock, SSL* ssl, char* mesg)
      
SSL経由でメッセージ(文字列)を送信する．
@param  sock   ソケット記述子．sslがNULLの場合，このソケットし使用する．
@param  ssl    SSL用ストリーム
@param  mesg   送信用メッセージ
@retval  0以上 送信バイト数（改行を含む）．
@retval -1     失敗した(send()の戻り値)．
*/
#define ssl_tcp_send_mesg(sock, ssl, mesg)  ssl_tcp_send((sock), (ssl), (mesg), 0)


/**    
int  ssl_send_mesg(SSL* ssl, char* mesg)
      
SSL経由でメッセージ(文字列)を送信する．
@param  ssl    SSL用ストリーム
@param  mesg   送信用メッセージ
@retval  0以上 送信バイト数．
@retval -1     失敗した(send()の戻り値)．
*/
#define ssl_send_mesg(ssl, mesg)  ssl_send((ssl), (mesg), 0)



// SSL/TLS with Buffer
int      ssl_recv_Buffer(SSL* ssl, Buffer* str);
int      ssl_send_Buffer(SSL* ssl, Buffer* str);
int      ssl_recv_Buffer_wait(int sock, SSL* ssl, Buffer* str, int tm);
int      ssl_send_sBuffer(SSL* ssl, Buffer* str);
int      ssl_send_sBufferln(SSL* ssl, Buffer* str);
int      ssl_recv_mstream_Buffer(int sock, SSL* ssl, Buffer* mesg, mstream* sb, int tm);
int      ssl_recv_lines_Buffer(int sock, SSL* ssl, Buffer* mesg, int tm);

int      ssl_tcp_recv_Buffer(int sock, SSL* ssl, Buffer* str);
int      ssl_tcp_send_Buffer(int sock, SSL* ssl, Buffer* str);
int      ssl_tcp_recv_Buffer_wait(int sock, SSL* ssl, Buffer* str, int tm);
int      ssl_tcp_send_sBuffer(int sock, SSL* ssl, Buffer* str);
int      ssl_tcp_send_sBufferln(int sock, SSL* ssl, Buffer* str);
int      ssl_tcp_recv_mstream_Buffer(int sock, SSL* ssl, Buffer* mesg, mstream* sb, int tm);
int      ssl_tcp_recv_lines_Buffer(int sock, SSL* ssl, Buffer* mesg, int tm);

int      ssl_tcp_recv_Buffer_tosize(int sock, SSL* ssl, Buffer* str, Buffer* mod, int size);
int      ssl_tcp_recv_Buffer_tosize_wait(int sock, SSL* ssl, Buffer* str, Buffer* mod, int size, int tm);


#endif        //  DISABLE_SSL

#endif        // __JBXL_SSL_TOOL_H_

