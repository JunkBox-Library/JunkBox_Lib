#ifndef  __JBXL_HTTPS_TOOL_H_
#define  __JBXL_HTTPS_TOOL_H_

/** 
@brief   HTTPS ツールライブラリ ヘッダ
@file    https_tool.h
@author  Fumi.Iseki (C)
*/

#include "ssl_tool.h"
#include "http_tool.h"


#ifdef DISABLE_SSL
#undef ENABLE_SSL
#endif 

#ifdef ENABLE_SSL

int  recv_https_header(int sofd, SSL* ssl, tList** pl, int* len, int tm, FILE* fp, int* state);
int  send_https_header(int sofd, SSL* ssl, tList*  pl, int mode);

int  recv_https_content(int sofd, SSL* ssl, Buffer* buf, int len, int tm, FILE* fp, int* state);
int  recv_https_chunked(int sofd, SSL* ssl, Buffer* buf, int tm,  FILE* fp, int* state);
int  recv_https_closed (int sofd, SSL* ssl, Buffer* buf, int tm,  FILE* fp);

int  recv_https_chunked_remain(int sofd, SSL* ssl, Buffer* buf, int chnksz, int tm);

int  recv_https_Buffer(int sofd, SSL* ssl, tList** pl, Buffer* buf, int timeout, int* hdonly, int* state, int nochunk);
int  send_https_Buffer(int sofd, SSL* ssl, tList*  pl, Buffer* buf);

int  recv_https_file(int sofd, SSL* ssl, tList** pl, const char* fname, const char* wdir, int timeout, int* hdonly, int* state);
int  send_https_file(int sofd, SSL* ssl, tList*  pl, const char* fname);

int  save_https_xml(int cofd, SSL* ssl, tList** pl, tXML** xml, char** recvfn, const char* wdir, int timeout, int* state);

#endif  // ENABLE_SSL

#endif  // __JBXL_HTTPS_TOOL_H_

