
#ifndef  __JBXL_HTTP_TOOL_H_
#define  __JBXL_HTTP_TOOL_H_


/** 
@brief   HTTP ツールライブラリ ヘッダ
@file    http_tool.h
@author  Fumi.Iseki (C)

@attention
opensslライブラリが無くても使用できるように https_tool とは分離
*/


#include "txml.h"
#include "protocol.h"


#ifndef DISABLE_ZLIB 
    #include "gz_tool.h"
#endif


#define  HTTP_CONNECTION_CLOSE        0
#define  HTTP_CONNECTION_KEEP         1
#define  HTTP_HEADER_NOT_HTTP        -1990
#define  HTTP_HEADER_CLOSED_SESSION  -1997
#define  HTTP_HEADER_CHUNKED         -1998
#define  HTTP_HEADER_UNKNOWN_LEN     -1999

//
#define  HTTP_ERROR_METHOD           -1     ///< HTTP で無い場合は負数にする．
#define  HTTP_UNKNOWN_METHOD          0     ///< 0 固定．これ以下はエラーとする． 

// HTTP
#define  HTTP_GET_METHOD              1
#define  HTTP_PUT_METHOD              2
#define  HTTP_POST_METHOD             3
#define  HTTP_DELETE_METHOD           4
#define  HTTP_OPTIONS_METHOD          5
#define  HTTP_HEAD_METHOD             6
#define  HTTP_TRACE_METHOD            7
#define  HTTP_CONNECT_METHOD          8
#define  HTTP_PATCH_METHOD            9
#define  HTTP_LINK_METHOD            10
#define  HTTP_UNLINK_METHOD          11 

// WebDAV
#define  HTTP_PROPFIND_METHOD        20
#define  HTTP_PROPPATCH_METHOD       21
#define  HTTP_MKCOL_METHOD           22
#define  HTTP_COPY_METHOD            23
#define  HTTP_MOVE_METHOD            24
#define  HTTP_LOCK_METHOD            25
#define  HTTP_UNLOCK_METHOD          26

#define  HTTP_RESPONSE_METHOD        99

#define  HTTP_TIMEOUT                60  ///< 秒
#define  HTTP_RESPONSE_TIMEOUT       180 ///< 秒
#define  HTTP_PROXY_TIMEOUT          60  ///< 秒


int    recv_http_header(int sock, tList** pl, int* len, int tm, FILE* fp, int* state);
int    send_http_header(int sock, tList*  pl, int mode);

int    recv_http_content(int sofd, Buffer* buf, int len, int tm, FILE* fp, int* state);
int    recv_http_chunked(int sofd, Buffer* buf, int tm,  FILE* fp, int* state);
int    recv_http_closed (int sofd, Buffer* buf, int tm,  FILE* fp);

int    recv_http_chunked_remain(int sofd, Buffer* buf, int chnksz, int tm);

int    recv_http_Buffer(int sock, tList** pl, Buffer* buf, int timeout, int* hdonly, int* state, int nochunk);
int    send_http_Buffer(int sock, tList*  pl, Buffer* buf);

int    recv_http_file(int sock, tList** pl, const char* fn, const char* wdir, int timeout, int* hdonly, int* state);
int    send_http_file(int sock, tList*  pl, const char* fn);

int    save_http_xml(int sock, tList** pl, tXML** xml, char** recvfn, const char* wdir, int timeout, int* state);


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tools
int    get_http_method(char* data);

int    is_http_header_field(tList* pl, char* field, char* value, int n);
int    get_http_header_method(tList* pl);
void   set_http_host_header(tList* pl, char* hname, unsigned short hport);
void   dump_http_header(FILE* fp, tList* pp);

int    get_http_status_num(tList* pl);
float  get_http_version_num(tList* pl);

int    get_chunked_size(char* ptr, int* hdsz, int* tlsz);

char*  get_http_header_date(time_t tm);             // need free()
Buffer rebuild_http_Buffer(tList* pl, Buffer* buf); // need free()


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// HTTP Proxy
void   simple_web_proxy(int bofd, char* myip, int tmout);
int    www2browser_relay(int bofd, int wofd, int btm, int wtm, int keep);
Buffer http_proxy_header_analyze(tList* pl, Buffer* server, unsigned short* sport, int* timeout, int* keep);


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Alternative Tools
int    tcp_relay(int sofd, int cofd, int tm);


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server Side (Junk Code)
int    send_http_res_file(int sock, char* fn, int mode);
void   send_http_passwd_req(int sock);


#endif  // __JBXL_HTTP_TOOL_H_

