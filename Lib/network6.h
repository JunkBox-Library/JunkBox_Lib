
#ifndef  __JBXL_NETWORK_H_
#define  __JBXL_NETWORK_H_


/** 
@brief   ネットワーク用ライブラリヘッダ
@file    network.h
@author  Fumi.Iseki (C)
*/


#include "tools.h"
#include "jbxl_state.h"


#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef WIN32
    //#include <winsock2.h>    // included at tools.h
    #include <ws2tcpip.h>
    #include <fcntl.h>

    #ifndef socklen_t        
        #define socklen_t  int
    #endif
    extern WSADATA WsaData;

    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #pragma comment( lib, "ws2_32.lib" )

#else
    // Unix/Linux
    #include <sys/socket.h>

    #ifdef HAVE_UNISTD_H
        #ifndef _XOPEN_SOURCE
            #define _XOPEN_SOURCE
        #endif
        #include <unistd.h>
    #endif

    #ifdef HAVE_SYS_SOCKIO_H
        #include <sys/sockio.h>
    #endif

    #ifdef HAVE_NETINET_IN_H
        #include <netinet/in.h>
    #endif
    #ifdef HAVE_ARPA_INET_H
        #include <arpa/inet.h>
    #endif

    #include <netdb.h>

    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <net/if.h>
#endif


//#define  NET_MAX_IF_NUM       10      ///< 取り扱うネットワークインターフェイスの最大数


/////////////////////////////////////////////////////////////////////////////////////////////
// for WinSock

int   init_network(void);
void  cleanup_network(void);


/////////////////////////////////////////////////////////////////////////////////////////////
// Berkeley Socket

int   _udp_server_socket(int port, struct addrinfo** info, int family);                 ///< call socket(), bind()
int   _udp_server_socket_setopt(int, int, const void*, int, struct addrinfo**, int f);  ///< call socket(), setsockopt(), bind()
int   _udp_client_socket(char* host, int, struct addrinfo**, int family);               ///< call socket()
int   _udp_bind(int sock, int port, int family);                                        ///< call bind()
int   _udp_bind_setopt(int sock, int port, int opt, const void*, int len, int family);  ///< call setsockopt(), bind()

/*
int  udp_server_socket(int port, struct addrinfo** info);        	///< udp のサーバソケットを作成する．IPv6 -> IPv4
int  udp_server_socket_ipv4(int port, struct addrinfo** info);      ///< IPv4 用udp のサーバソケットを作成する．
int  udp_server_socket_ipv6(int port, struct addrinfo** info);      ///< IPv6 用udp のサーバソケットを作成する．
*/
#define  udp_server_socket(p, a)                        _udp_server_socket((p), (a),  AF_UNSPEC)
#define  udp_server_socket_ipv4(p)                      _udp_server_socket((p), NULL, AF_INET)
#define  udp_server_socket_ipv6(p, a)                   _udp_server_socket((p), (a),  AF_INET6)

#define  udp_server_socket_setopt(p, o, v, l, a)        _udp_server_socket_setopt((p), (o), (v), (l), (a),  AF_UNSPEC)
#define  udp_server_socket_setopt_ipv4(p, o, v, l)      _udp_server_socket_setopt((p), (o), (v), (l), NULL, AF_INET)
#define  udp_server_socket_setopt_ipv6(p, o, v, l, a)   _udp_server_socket_setopt((p), (o), (v), (l), (a),  AF_INET6)

#define  udp_client_socket(h, p, a)                     _udp_client_socket((h), (p), (a), AF_UNSPEC)
#define  udp_client_socket_ipv4(h, p, a)                _udp_client_socket((h), (p), (a), AF_INET)
#define  udp_client_socket_ipv6(h, p, a)                _udp_client_socket((h), (p), (a), AF_INET6)

#define  udp_bind(s, p)                                 _udp_bind((s), (p), AF_UNSPEC)
#define  udp_bind_ipv4(s, p)                            _udp_bind((s), (p), AF_INET)
#define  udp_bind_ipv6(s, p)                            _udp_bind((s), (p), AF_INET6)

#define  udp_bind_setopt(s, p, o, v, l)                 _udp_bind_setopt((s), (p), (o), (v), (l), AF_UNSPEC)
#define  udp_bind_setopt_ipv4(s, p, o, v, l)            _udp_bind_setopt((s), (p), (o), (v), (l), AF_INET)
#define  udp_bind_setopt_ipv6(s, p, o, v, l)            _udp_bind_setopt((s), (p), (o), (v), (l), AF_INET6)

//
int   _tcp_server_socket(int port, int family);                                         ///< call socket(), bind(), listen()
int   _tcp_server_socket_setopt(int port, int opt, const void*, int len, int famliy);   ///< call socket(), setsockopt(), bind(), listen()
int   _tcp_server_bind(int port, int family);                                           ///< call socket(), bind()
int   _tcp_server_bind_setopt(int port, int opt, const void*, int len, int family);     ///< call socket(), setsockopt(), bind()

#define  tcp_server_socket(p)                       _tcp_server_socket((p), AF_UNSPEC)
#define  tcp_server_socket_ipv4(p)                  _tcp_server_socket((p), AF_INET)
#define  tcp_server_socket_ipv6(p)                  _tcp_server_socket((p), AF_INET6)

#define  tcp_server_socket_setop(p, o, v, l)        _tcp_server_socket_setopt((p), (o), (v), (l), AF_UNSPEC)
#define  tcp_server_socket_setop_ipv4(p, o, v, l)   _tcp_server_socket_setopt((p), (o), (v), (l), AF_INET)
#define  tcp_server_socket_setop_ipv6(p, o, v, l)   _tcp_server_socket_setopt((p), (o), (v), (l), AF_INET6)

#define  tcp_server_bind(p)                         _tcp_server_bind((p), AF_UNSPEC)
#define  tcp_server_bind_ipv4(p)                    _tcp_server_bind((p), AF_INET)
#define  tcp_server_bind_ipv6(p)                    _tcp_server_bind((p), AF_INET6)

#define  tcp_server_bind_setopt(p, o, v, l)         _tcp_server_bind_setopt((p), (o), (v), (l), AF_UNSPEC)
#define  tcp_server_bind_setopt_ipv4(p, o, v, l)    _tcp_server_bind_setopt((p), (o), (v), (l), AF_INET)
#define  tcp_server_bind_setopt_ipv6(p, o, v, l)    _tcp_server_bind_setopt((p), (o), (v), (l), AF_INET6)

int   _tcp_client_socket(char* hname, int port, int family);                            ///< call socket(), connect()
int   _tcp_client_bind_socket(char* host, int sport, int cport, int family);            ///< call socket(), bind(), connect()
int   _tcp_bind(int sock, int port, int family);                                        ///< call bind()
int   _tcp_bind_setopt(int sock, int port, int opt, const void*, int len, int family);  ///< call setsockopt(), bind()
int   _tcp_connect(int sock, char* host, int port, int family);                         ///< call connect()

#define  tcp_client_socket(h, p)                    _tcp_client_socket((h), (p), AF_UNSPEC)
#define  tcp_client_socket_ipv4(h, p)               _tcp_client_socket((h), (p), AF_INET)
#define  tcp_client_socket_ipv6(h, p)               _tcp_client_socket((h), (p), AF_INET6)

#define  tcp_client_bind_socket(h, s, c)            _tcp_client_bind_socket((h), (c), (s), AF_UNSPEC)
#define  tcp_client_bind_socket_ipv4(h, s, c)       _tcp_client_bind_socket((h), (c), (s), AF_INET)
#define  tcp_client_bind_socket_ipv6(h, s, c)       _tcp_client_bind_socket((h), (c), (s), AF_INET6)

#define  tcp_bind(s, p)                             _tcp_bind((s), (p), AD_UNSPEC)
#define  tcp_bind_ipv4(s, p)                        _tcp_bind((s), (p), AD_INET)
#define  tcp_bind_ipv6(s, p)                        _tcp_bind((s), (p), AD_INET6)

#define  tcp_bind_setopt(s, p, o, v, l)             _tcp_bind_setopt((s), (p), (o), (v), (l), AF_UNSPEC)
#define  tcp_bind_setopt_ipv4(s, p, o, v, l)        _tcp_bind_setopt((s), (p), (o), (v), (l), AF_INET)
#define  tcp_bind_setopt_ipv6(s, p, o, v, l)        _tcp_bind_setopt((s), (p), (o), (v), (l), AF_INET6)

#define  tcp_connect(s, h, p)                       _tcp_connect((s), (h), (p), AF_UNSPEC)
#define  tcp_connect_ipv4(s, h, p)                  _tcp_connect((s), (h), (p), AF_INET)
#define  tcp_connect_ipv6(s, h, p)                  _tcp_connect((s), (h), (p), AF_INET6)

//
int   accept_intr(int, struct sockaddr*, socklen_t*);                           ///< call accept()
int   socket_close(int sock);                                                   ///< call shutdown(), close()
int   set_nonblock_socket(int sock);
int   set_block_socket(int sock);


/////////////////////////////////////////////////////////////////////////////////////////////
// Valid Sockets

int   get_valid_udp_socket(int min, int max, unsigned short* port);
int   get_valid_tcp_server_socket(int min, int max, unsigned short* port);
int   get_valid_tcp_client_socket(int min, int max, char* hostname, unsigned short sport, unsigned short* cport);


/////////////////////////////////////////////////////////////////////////////////////////////
// Communication

int   udp_recv(int, char*, int, struct addrinfo*);
int   udp_send(int, char*, int, struct addrinfo*);
int   tcp_recv(int, char*, int);
int   tcp_send(int, char*, int);

int   udp_recv_wait(int sock, char* data, int size, struct addrinfo* sv_addr, int tm);
int   tcp_recv_wait(int sock, char* data, int sz, int tm);

int   tcp_send_mesgln(int, char*); 
int   tcp_recv_mstream(int sock, char* mesg, int sz, mstream* sb, int tm);

int   recv_wait(int sock, int tm);
int   recv_wait_twin(int sock1, int sock2, int tm);
int   send_wait(int sock, int tm);

#define  udp_send_mesg(s, m, a)         udp_send((s), (m), 0, (a))
#define  tcp_send_mesg(s, m)            tcp_send((s), (m), 0)


/////////////////////////////////////////////////////////////////////////////////////////////
// IP address

struct sockaddr*  make_sockaddr_bynum(unsigned char* addr, int port, int family);    ///< IPv4/IPv6 対応
struct sockaddr*  make_sockaddr_bystr(const char* addr, int port);                   ///< IPv4/IPv6 対応

#define  make_sockaddr_bynum_ipv4(a, p) make_sockaddr_bynum((a), (p), AF_INET)
#define  make_sockaddr_bynum_ipv6(a, p) make_sockaddr_bynum((a), (p), AF_INET6)

char*  _get_hostname(const char* host, int family);                 ///< IPv4/IPv6 アドレス（文字列）, FQDN  → ホスト名（文字列）

#define  get_hostname(h)                _get_hostname((h), AF_UNSPEC) 
#define  get_hostname_ipv4(h)           _get_hostname((h), AF_INET) 
#define  get_hostname_ipv6(h)           _get_hostname((h), AF_INET6) 

char*  get_hostname_bynum(unsigned char* bin, int family);          ///< IPv4/IPv6 アドレス（バイナリ）→ ホスト名（文字列）
char*  get_hostname_bystr(const char* str);                         ///< IPv4/IPv6 アドレス（文字列）  → ホスト名（文字列）

#define  get_hostname_bynum_ipv4(n)     get_hostname_bynum((n), AF_INET)
#define  get_hostname_bynum_ipv6(n)     get_hostname_bynum((n), AF_INET6)

char*  get_ipaddr_byname(const char* host, int family);             ///< ホスト名 → IPv4/IPv6 アドレス（文字列）
unsigned char*  get_ipaddr_byname_num(const char* h, int family);   ///< ホスト名 → IPv4/IPv6 アドレス（バイナリ）

#define  get_ipaddr_byname_ipv4(h)      get_ipaddr_byname((h), AF_INET)
#define  get_ipaddr_byname_ipv6(h)      get_ipaddr_byname((h), AF_INET6)
#define  get_ipaddr_byname_num_ipv4(h)  get_ipaddr_byname_num((h), AF_INET)
#define  get_ipaddr_byname_num_ipv6(h)  get_ipaddr_byname_num((h), AF_INET6)

char*  get_myipaddr(int family);                                    ///< 自分の [IPv4/IPv6 ネットワークアドレス]/[ネットマスク]（文字列）
unsigned char*  get_myipaddr_num(int family);                       ///< 自分の [IPv4/IPv6 アドレス],[ネットマスク]（バイナリ）

#define  get_myipaddr_ipv4()            get_myipaddr(AF_INET)
#define  get_myipaddr_ipv6()            get_myipaddr(AF_INET6)
#define  get_myipaddr_num_ipv4()        get_myipaddr_num(AF_INET)
#define  get_myipaddr_num_ipv6()        get_myipaddr_num(AF_INET6)

char*  _get_localip_bydest(const char* host, int family);           ///< 自分の IPv4/IPv6 アドレス（文字列）

#define  get_localip_bydest(h)          _get_localip_bydest((h), AF_UNSPEC)
#define  get_localip_bydest_ipv4(h)     _get_localip_bydest((h), AF_INET)
#define  get_localip_bydest_ipv6(h)     _get_localip_bydest((h), AF_INET6)

#define  get_localip()                  _get_localip_bydest("www.google.co.jp", AF_UNSPEC)
#define  get_localip_ipv4()             _get_localip_bydest("www.google.co.jp", AF_INET)
#define  get_localip_ipv6()             _get_localip_bydest("www.google.co.jp", AF_INET6)

unsigned char*  get_mynetaddr_num(int family);                      ///< 自分の IPv4/IPv6 ネットワークアドレス（バイナリ）
char*  get_mynetaddr(int family);                                   ///< 自分の IPv4/IPv6 ネットワークアドレス（文字列）

#define  get_mynetaddr_num_ipv4()       get_mynetaddr_num(AF_INET)
#define  get_mynetaddr_num_ipv6()       get_mynetaddr_num(AF_INET6)
#define  get_mynetaddr_ipv4()           get_mynetaddr(AF_INET)
#define  get_mynetaddr_ipv6()           get_mynetaddr(AF_INET6)


/////////////////////////////////////////////////////////////////////////////////////////////
// Tools

unsigned char*  to_address_num(char*, int md, int mk, int family);	///< IPv4/IPv6 アドレス（文字列）→ [IPv4/IPv6 アドレス],[ネットマスク]（数字8/32Byte）
char*  to_address_char(unsigned char* addr, int mask, int family);  ///< [IPv4/IPv6 アドレス],[ネットマスク]（数字8/32Byte）→ IPv4/IPv6 アドレス（文字列）

#define  to_address_num8_ipv4(a, m)     to_address_num((a), (m), 1, AF_INET)    ///< IPv4 アドレス（文字列）→ [IPv4 アドレス],[ネットマスク]（数字8byte）
#define  to_address_num4_ipv4(a, m)     to_address_num((a), (m), 0, AF_INET)    ///< IPv4 アドレス（文字列）→ [IPv4 アドレス]（数字4byte）
#define  to_address_char8_ipv4(a)       to_address_char((a), 1, AF_INET)        ///< [IPv4 アドレス],[ネットマスク]（数字8byte）→ IPv4 アドレス（文字列）
#define  to_address_char4_ipv4(a)       to_address_char((a), 0, AF_INET)        ///< [IPv4 アドレス]（数字4byte）→ IPv4 アドレス（文字列）

#define  to_address_num32_ipv6(a, m)    to_address_num((a), (m), 1, AF_INET6)   ///< IPv6 アドレス（文字列）→ [IPv6 アドレス],[ネットマスク]（数字32byte）
#define  to_address_num16_ipv6(a, m)    to_address_num((a), (m), 0, AF_INET6)   ///< IPv6 アドレス（文字列）→ [IPv6 アドレス]（数字16byte）
#define  to_address_char32_ipv6(a)      to_address_char((a), 1, AF_INET6)       ///< [IPv6 アドレス],[ネットマスク]（数字32byte）→ IPv6 アドレス（文字列）
#define  to_address_char16_ipv6(a)      to_address_char((a), 0, AF_INET6)       ///< [IPv6 アドレス]（数字16byte）→ IPv6 アドレス（文字列）

int   is_same_sockaddr(struct sockaddr* addr1, struct sockaddr* addr2);
int   is_same_network(char* addr1, char* addr2, char* mask);
int   is_same_network_num(unsigned char* addr1, unsigned char* addr2, unsigned char* mask, int family);

#define  is_same_network_num_ipv4(a1, a2, m)  is_same_network_num((a1), (a2), (m), AF_INET) 
#define  is_same_network_num_ipv6(a1, a2, m)  is_same_network_num((a1), (a2), (m), AF_INET6) 

void  udp_hole_punching(int sock, struct addrinfo* addr, int num);  ///


/////////////////////////////////////////////////////////////////////////////////////////////
// for IPv4 only 

int   udp_client_socket_sockaddr_in(char*, int, struct sockaddr_in*);   ///< use udp_client_socket()

int   udp_recv_sockaddr_in(int, char*, int, struct sockaddr_in*);       ///< use udp_recv()
int   udp_send_sockaddr_in(int, char*, int, struct sockaddr_in*);       ///< use udp_send()
int   udp_recv_wait_sockaddr_in(int sock, char* data, int size, struct sockaddr_in* sv_addr, int tm);   ///< use udp_recv_wait()

#define  udp_send_mesg_sockaddr_in(s, m, a)  udp_send_sockaddr_in((s), (m), 0, (a))

struct sockaddr_in  get_sockaddr_in(char* hostname, unsigned short cport);
struct sockaddr_in  get_sockaddr_in_bynum(char* ipnum, unsigned short cport);
struct sockaddr_in  get_local_sockaddr_in(unsigned short cport);

//int    is_same_sockaddr_ipv4(struct sockaddr_in addr1, struct sockaddr_in addr2);
#define  is_same_sockaddr_in(a, b)  is_same_sockaddr((struct sockaddr*)&(a), (struct sockaddr*)&(b))    ///< use is_same_sockaddr()

unsigned char*  get_ipaddr_num_ipv4(struct in_addr);        ///< 構造体 → IPv4 アドレス（バイナリ） use get_ipaddr_byname_num()
char*  get_ipaddr_ipv4(struct in_addr);                     ///< 構造体 → IPv4 アドレス（文字列）   use get_ipaddr_byname()

void   udp_hole_punching_sockaddr_in(int sock, struct sockaddr_in addr, int nm);    ///< use udp_hole_punching()


#endif // __JBXL_NETWORK_H_


/**
//////////////////////////////////////////////////////////////////////
- Network 構造体
@code
    struct in_addr {
        u_int32_t s_addr;           // 32bit
    };

    struct sockaddr_in {
        u_char  sin_len;            // 8bit
        u_char  sin_family;         // 8bit
        u_short sin_port;           // 16bit          --+
        struct  in_addr sin_addr;   // 32bit          --+-- struct sockaddr
        char    sin_zero[8];        // 8Byte padding  --+
    };

    struct sockaddr {
        __uint8_t     sa_len;       // 8bit
        sa_family_t   sa_family;    // 8bit
        char          sa_data[14];  // 14Byte   // sin_port+sin_addr+sin_zero
    };


    // 28Byte
    struct sockaddr_in6 {
        __uint8_t   sin6_len;       // 8bit
        sa_family_t sin6_family;    // 8bit
        in_port_t   sin6_port;      // 16bit
        __uint32_t  sin6_flowinfo;  // 32bit
        struct in6_addr sin6_addr;  // 128bit
        __uint32_t  sin6_scope_id;  // 32bit
    };


    struct addrinfo {
        int              ai_flags;
        int              ai_family;
        int              ai_socktype;
        int              ai_protocol;
        socklen_t        ai_addrlen;
        struct sockaddr *ai_addr;           // sockaddr == sockaddr_in6
        char            *ai_canonname;
        struct addrinfo *ai_next; 
    }; 


    struct sockaddr* addr = (struct sockaddr*)&[struct sockaddr_in];
    struct sockaddr* addr = [struct addrinfo].ai_addr;

    IPアドレス  [struct sockadd_in].sin_addr,  [strunt addrinfo].ai_addr->sin_addr
    ポート番号  [struct sockadd_in].sin_port,  [struct addrinfo].ai_addr->sin_port
@endcode
*/

