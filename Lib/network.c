//
/** 
@brief   ネットワーク用ライブラリ for IPv4/IPv6
@file    network.c
@author  Fumi.Iseki (C)
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include "network.h"
#include <time.h>


//////////////////////////////////////////////////////////////////////////////////
// for WinSock
//

#ifdef WIN32
WSADATA WsaData;
#endif


int  init_network(void)
{
    int ret = 0;

#ifdef WIN32
    // for 10093 エラー
    ret = WSAStartup(MAKEWORD(2,0), &WsaData);
    if (ret!=0) WSACleanup();
#endif

    return ret;
}


void  cleanup_network(void)
{
#ifdef WIN32
    WSACleanup();
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////
// Berkeley Socket
//

/**
int  _udp_server_socket(int port, struct addrinfo** sv_addr, int family) 

IPv4/IPv6 の UDPサーバソケットを作り,接続待ち状態になる．

この関数内で呼び出されるネットワーク関数: socket(), bind()

@param  port    ポート番号
@param  sv_addr addrinfo 構造体へのポインタへのポインタが返る．要 freeaddrinfo(*sv_addr)
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_SOCKET_ERROR  ソケットが作成できなかった．
@retval JBXL_NET_BIND_ERROR    バインドに失敗．
@retval JBXL_NET_INFO_ERROR    ホスト情報の取得に失敗．
*/
int  _udp_server_socket(int port, struct addrinfo** sv_addr, int family) 
{
    int    sofd, err, nullflg = OFF;
    struct addrinfo  hints;
    struct addrinfo* address;
    int    nonblock = FALSE;

    if (sv_addr==NULL) {
        nullflg = ON;
        sv_addr = &address;
    }
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;
    //
    if (port<0) {
        nonblock = TRUE;
        port = -port;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE; // 任意のIPアドレスの接続を許す設定．サーバ用．
    hints.ai_socktype = SOCK_DGRAM;  

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, sv_addr);
    freeNull(str);
    if (err!=0) {
        *sv_addr = NULL;
        return JBXL_NET_INFO_ERROR;
    }

    sofd = (int)socket((*sv_addr)->ai_family, (*sv_addr)->ai_socktype, (*sv_addr)->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(*sv_addr);
        *sv_addr = NULL;
        return JBXL_NET_SOCKET_ERROR;
    }
    if (nonblock) sofd = set_nonblock_socket(sofd);
 
    // TIME_WAIT しない設定．
    int lg = 1;
    #ifdef WIN32
        err = setsockopt(sofd, SOL_SOCKET, SO_REUSEADDR, (const char*)&lg, sizeof(lg));
    #else
        err = setsockopt(sofd, SOL_SOCKET, SO_REUSEADDR, &lg, sizeof(lg));
    #endif
    if (err<0) {
        freeaddrinfo(*sv_addr);
        socket_close(sofd);
        return JBXL_NET_OPTION_ERROR;
    } 

    err = bind(sofd, (*sv_addr)->ai_addr, (int)(*sv_addr)->ai_addrlen);
    if (err<0) {
        freeaddrinfo(*sv_addr);
        *sv_addr = NULL;
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    if (nullflg) freeaddrinfo(*sv_addr);
    return sofd;
}


/**
int  _udp_server_socket_setopt(int port, int opt, const void* optval, int optlen, struct addrinfo** sv_addr, int family) 

IPv4/IPv6 の UDPサーバソケットを作り,接続待ち状態になる．オプションの指定（setsockopt）が可能．

この関数内で呼び出されるネットワーク関数: socket(), setsockopt(), bind()

@param  port    ポート番号
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ
@param  sv_addr addrinfo 構造体へのポインタへのポインタが返る．要 freeaddrinfo(*sv_addr)
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_SOCKET_ERROR   ソケットが作成できなかった．
@retval JBXL_NET_OPTION_ERROR   オプションの設定に失敗．
@retval JBXL_NET_BIND_ERROR     バインドに失敗．
@retval JBXL_NET_INFO_ERROR     ホスト情報の取得に失敗．
*/
int  _udp_server_socket_setopt(int port, int opt, const void* optval, int optlen, struct addrinfo** sv_addr, int family) 
{
    int    sofd, err, nullflg = OFF;
    struct addrinfo  hints;
    struct addrinfo* address;
    int    nonblock = FALSE;

    if (sv_addr==NULL) {
        nullflg = ON;
        sv_addr = &address;
    }
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;
    //
    if (port<0) {
        nonblock = TRUE;
        port = -port;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;  

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, sv_addr);
    freeNull(str);
    if (err!=0) return JBXL_NET_INFO_ERROR;

    sofd = (int)socket((*sv_addr)->ai_family, (*sv_addr)->ai_socktype, (*sv_addr)->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(*sv_addr);
        *sv_addr = NULL;
        return JBXL_NET_SOCKET_ERROR;
    }
    if (nonblock) sofd = set_nonblock_socket(sofd);
 
    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            freeaddrinfo(*sv_addr);
            *sv_addr = NULL;
            socket_close(sofd);
            return JBXL_NET_OPTION_ERROR;
        } 
    }
 
    err = bind(sofd, (*sv_addr)->ai_addr, (int)(*sv_addr)->ai_addrlen);
    if (err<0) {
        freeaddrinfo(*sv_addr);
        *sv_addr = NULL;
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    if (nullflg) freeaddrinfo(*sv_addr);
    return sofd;
}


/**
int  _udp_client_socket(char* hostname, int port, struct addrinfo** sv_addr, int family)

UDPのクライアントソケットを作る．
正常終了した場合, *sv_addrにサーバの情報が格納される．

この関数内で呼び出されるネットワーク関数: socket(), getaddrinfo()

@param  hostname サーバ名
@param  port     サーバポート番号
@param  sv_addr  addrinfo 構造体へのポインタへのポインタが返る．要 freeaddrinfo(*sv_addr)
@param  family   プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                 AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_SOCKET_ERROR  ソケットが作成できなかった．
@retval JBXL_NET_INFO_ERROR    ホスト情報の取得に失敗．
*/
int  _udp_client_socket(char* hostname, int port, struct addrinfo** sv_addr, int family)
{
    int    sofd, err;
    struct addrinfo  hints;
 
    if (sv_addr==NULL) return JBXL_NET_INFO_ERROR;
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE; // 任意のIPアドレスの接続を許す設定．本来はサーバ用．多分不必要（無視される）．
    hints.ai_socktype = SOCK_DGRAM;  
 
    char* str = itostr_ts(port);
    err = getaddrinfo(hostname, str, &hints, sv_addr);
    freeNull(str);
    if (err!=0) {
        *sv_addr = NULL;
        return JBXL_NET_INFO_ERROR;
    }

    sofd = (int)socket((*sv_addr)->ai_family, (*sv_addr)->ai_socktype, (*sv_addr)->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(*sv_addr);
        *sv_addr = NULL;
        return JBXL_NET_SOCKET_ERROR;
    }

    return sofd;
}


/**
int  _udp_bind(int sofd, int port, int family) 

UDPソケットにポートをバインドする．

@param  sofd    バインドするソケットの記述子．
@param  port    ポート番号
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_BIND_ERROR   バインドに失敗．
@retval JBXL_NET_INFO_ERROR   ホスト情報の取得に失敗．
*/
int  _udp_bind(int sofd, int port, int family) 
{
    int    err;
    struct addrinfo  hints;
    struct addrinfo* address;
 
    if (sofd<=0) return sofd;
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;  

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, &address);
    freeNull(str);
    if (err!=0) return JBXL_NET_INFO_ERROR;
 
    err = bind(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/**
int  _udp_bind_setopt(int sofd, int port, int opt, const void* optval, int optlen, int family) 

IPv4/IPv6 のUDPソケットにポートをバインドする．オプションの指定（setsockopt）が可能．

@param  sofd    バインドするソケットの記述子．
@param  port    ポート番号
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_OPTION_ERROR  オプションの設定に失敗．
@retval JBXL_NET_BIND_ERROR    バインドに失敗．
@retval JBXL_NET_INFO_ERROR    ホスト情報の取得に失敗．
 */
int  _udp_bind_setopt(int sofd, int port, int opt, const void* optval, int optlen, int family) 
{
    int    err;
    struct addrinfo  hints;
    struct addrinfo* address;
 
    if (sofd<=0) return sofd;
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            socket_close(sofd);
            return JBXL_NET_OPTION_ERROR;
        } 
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;  

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, &address);
    freeNull(str);
    if (err!=0) {
        socket_close(sofd);
        return JBXL_NET_INFO_ERROR;
    }
 
    err = bind(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/**
int  _tcp_server_socket(int port, int family)

IPv4/IPv6 のTCPのサーバソケットを作り，リッスンにより接続要求を受けられる状態にする．@n
この関数に続いて accept()関数を呼ぶ事により,実際の接続待ち状態となる．

この関数内で呼び出されるネットワーク関数は socket(), bind(), listen()

@param  port    ポート番号．マイナスの場合は ノンブロックとなる．
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_SOCKET_ERROR  ソケットが作成できなかった．
@retval JBXL_NET_BIND_ERROR    バインドに失敗．
@retval JBXL_NET_INFO_ERROR    ホスト情報の取得に失敗．
@retval JBXL_NET_LISTEN_ERROR  リッスンに失敗．
*/
int  _tcp_server_socket(int port, int family) 
{
    int    sofd, err;
    struct addrinfo  hints;
    struct addrinfo* address;
    int    nonblock = FALSE;
 
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;  

    if (port<0) {
        nonblock = TRUE;
        port = -port;
    }

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, &address);
    freeNull(str);
    if (err!=0) return JBXL_NET_INFO_ERROR;

    sofd = (int)socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(address);
        return JBXL_NET_SOCKET_ERROR;
    }
    if (nonblock) sofd = set_nonblock_socket(sofd);

    // TIME_WAIT しない設定．
    int lg = 1;
    #ifdef WIN32
        err = setsockopt(sofd, SOL_SOCKET, SO_REUSEADDR, (const char*)&lg, sizeof(lg));
    #else
        err = setsockopt(sofd, SOL_SOCKET, SO_REUSEADDR, &lg, sizeof(lg));
    #endif
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_OPTION_ERROR;
    }

    err = bind(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    err = listen(sofd, 10);
    if (err==-1) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_LISTEN_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/**
int  _tcp_server_socket_setopt(int port, int opt, const void* optval, int optlen, int family) 

IPv4/IPv6 のTCPのサーバソケットを作り，リッスンにより接続要求を受けられる状態にする．@n
この関数に続いて accept()関数を呼ぶ事により,実際の接続待ち状態となる．
オプションの指定（setsockopt）が可能．

この関数内で呼び出されるネットワーク関数は socket(), setsockopt(), bind(), listen()

@param  port    ポート番号．マイナスの場合は ノンブロックとなる．
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_SOCKET_ERROR  ソケットが作成できなかった．
@retval JBXL_NET_OPTION_ERROR  オプションの設定に失敗．
@retval JBXL_NET_BIND_ERROR    バインドに失敗．
@retval JBXL_NET_INFO_ERROR    ホスト情報の取得に失敗．
@retval JBXL_NET_LISTEN_ERROR  リッスンに失敗．
*/
int  _tcp_server_socket_setopt(int port, int opt, const void* optval, int optlen, int family) 
{
    int    sofd, err;
    struct addrinfo  hints;
    struct addrinfo* address;
    int    nonblock = FALSE;
 
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;  

    if (port<0) {
        nonblock = TRUE;
        port = -port;
    }

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, &address);
    freeNull(str);
    if (err!=0) return JBXL_NET_INFO_ERROR;

    sofd = (int)socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(address);
        return JBXL_NET_SOCKET_ERROR;
    }
    if (nonblock) sofd = set_nonblock_socket(sofd);

    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            freeaddrinfo(address);
            socket_close(sofd);
            return JBXL_NET_OPTION_ERROR;
        }
    }

    err = bind(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    err = listen(sofd, 10);
    if (err==-1) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_LISTEN_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/**
int  _tcp_server_bind(int port, int family) 

IPv4/IPv6 のTCPのサーバのソケット記述子を作り，バインドする．
tcp_server_socket() との違いは listen()関数を呼び出さない点にある．

この関数内で呼び出されるネットワーク関数: socket(), bind()

@param  port    ポート番号．
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_SOCKET_ERROR  ソケットが作成できなかった．
@retval JBXL_NET_BIND_ERROR    バインドに失敗．
@retval JBXL_NET_INFO_ERROR    ホスト情報の取得に失敗．
*/
int  _tcp_server_bind(int port, int family) 
{
    int    sofd, err;
    struct addrinfo  hints;
    struct addrinfo* address;
    int    nonblock = FALSE;

    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;  

    if (port<0) {
        nonblock = TRUE;
        port = -port;
    }

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, &address);
    freeNull(str);
    if (err!=0) return JBXL_NET_INFO_ERROR;

    sofd = (int)socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(address);
        return JBXL_NET_SOCKET_ERROR;
    }
    if (nonblock) sofd = set_nonblock_socket(sofd);

    // TIME_WAIT しない設定．
    int lg = 1;
    #ifdef WIN32
        err = setsockopt(sofd, SOL_SOCKET, SO_REUSEADDR, (const char*)&lg, sizeof(lg));
    #else
        err = setsockopt(sofd, SOL_SOCKET, SO_REUSEADDR, &lg, sizeof(lg));
    #endif
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_OPTION_ERROR;
    }

    err = bind(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/**
int  _tcp_server_bind_setopt(int port, int opt, const void* optval, int optlen, int family) 

IPv4/IPv6 のTCPのサーバのソケット記述子を作り，バインドする．オプションの指定（setsockopt）が可能．
tcp_server_socket() との違いは listen()関数を呼び出さない点にある．

この関数内で呼び出されるネットワーク関数: socket(), setsockopt(), bind()

@param  port    ポート番号
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_SOCKET_ERROR  ソケットが作成できなかった．
@retval JBXL_NET_OPTION_ERROR  オプションの設定に失敗．
@retval JBXL_NET_BIND_ERROR    バインドに失敗．
@retval JBXL_NET_INFO_ERROR    ホスト情報の取得に失敗．
 */
int  _tcp_server_bind_setopt(int port, int opt, const void* optval, int optlen, int family) 
{
    int    sofd, err;
    struct addrinfo  hints;
    struct addrinfo* address;
    int    nonblock = FALSE;
 
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;  

    if (port<0) {
        nonblock = TRUE;
        port = -port;
    }

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, &address);
    freeNull(str);
    if (err!=0) return JBXL_NET_INFO_ERROR;

    sofd = (int)socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(address);
        return JBXL_NET_SOCKET_ERROR;
    }
    if (nonblock) sofd = set_nonblock_socket(sofd);

    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            freeaddrinfo(address);
            socket_close(sofd);
            return JBXL_NET_OPTION_ERROR;
        }
    }

    err = bind(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/**
int  _tcp_client_socket(char* hostname, int port, int family) 

IPv4/IPv6 のTCPのクライアントソケットを作り，サーバに接続する．

この関数内で呼び出されるネットワーク関数: socket(), getaddrinfo(), connect()

@param  hostname サーバ名
@param  port     サーバポート番号
@param  family   プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                 AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_SOCKET_ERROR   ソケットが作成できなかった．
@retval JBXL_NET_INFO_ERROR     ホスト情報の取得に失敗．
@retval JBXL_NET_CONNECT_ERROR  接続に失敗．
 */
int  _tcp_client_socket(char* hostname, int port, int family) 
{
    int    sofd, err;
    struct addrinfo  hints;
    struct addrinfo* address;
 
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;  

    char* str = itostr_ts(port);
    err = getaddrinfo(hostname, str, &hints, &address);
    freeNull(str);
    if (err!=0) return JBXL_NET_INFO_ERROR;

    sofd = (int)socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(address);
        return JBXL_NET_SOCKET_ERROR;
    }

    err = connect(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_CONNECT_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/**
int  _tcp_client_bind_socket(char* hostname, int sport, int cport, int family)  

IPv4/IPv6 のTCPのクライアントソケットを作り，サーバに接続する．

この関数内で呼び出されるネットワーク関数: socket(), bind(), getaddrinfo(), connect()

@param  hostname サーバ名
@param  sport    サーバポート番号
@param  cport    自分のポート番号
@param  family   プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                 AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_SOCKET_ERROR   ソケットが作成できなかった．
@retval JBXL_NET_BIND_ERROR     バインドに失敗．
@retval JBXL_NET_INFO_ERROR     ホスト情報の取得に失敗．
@retval JBXL_NET_CONNECT_ERROR  接続に失敗．
 */
int  _tcp_client_bind_socket(char* hostname, int sport, int cport, int family)  
{
    int    sofd, err;
    struct addrinfo  hints;
    struct addrinfo* address;
 
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;  

    char* str = itostr_ts(cport);
    err = getaddrinfo(NULL, str, &hints, &address);
    freeNull(str);
    if (err!=0) return JBXL_NET_INFO_ERROR;

    sofd = (int)socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(address);
        return JBXL_NET_SOCKET_ERROR;
    }

    err = bind(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }
    freeaddrinfo(address);

    //
    str = itostr_ts(sport);
    err = getaddrinfo(hostname, str, &hints, &address);
    freeNull(str);
    if (err!=0) {
        socket_close(sofd);
        return JBXL_NET_INFO_ERROR;
    }

    err = connect(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_CONNECT_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/**
int  _tcp_bind(int sofd, int port, int family)  

IPv4/IPv6 のTCPソケットにポートをバインドする．

@param  sofd    バインドするソケットの記述子．
@param  port    ポート番号
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_BIND_ERROR   バインドに失敗．
@retval JBXL_NET_INFO_ERROR   ホスト情報の取得に失敗．
*/
int  _tcp_bind(int sofd, int port, int family)  
{
    int    err;
    struct addrinfo  hints;
    struct addrinfo* address;
 
    if (sofd<=0) return sofd;
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;  

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, &address);
    freeNull(str);
    if (err!=0) {
        socket_close(sofd);
        return JBXL_NET_INFO_ERROR;
    }

    err = bind(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/**
int  _tcp_bind_setopt(int sofd, int port, int opt, const void* optval, int optlen, int family)

IPv4/IPv6 のTCPソケットにポートをバインドする．オプションの指定（setsockopt）が可能．

@param  sofd    バインドするソケットの記述子．
@param  port    ポート番号
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ
@param  family  プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_OPTION_ERROR   オプションの設定に失敗．
@retval JBXL_NET_BIND_ERROR     バインドに失敗．
@retval JBXL_NET_INFO_ERROR     ホスト情報の取得に失敗．
*/
int  _tcp_bind_setopt(int sofd, int port, int opt, const void* optval, int optlen, int family)
{
    int    err;
    struct addrinfo  hints;
    struct addrinfo* address;
 
    if (sofd<=0) return sofd;
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            socket_close(sofd);
            return JBXL_NET_OPTION_ERROR;
        } 
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;  

    char* str = itostr_ts(port);
    err = getaddrinfo(NULL, str, &hints, &address);
    freeNull(str);
    if (err!=0) {
        socket_close(sofd);
        return JBXL_NET_INFO_ERROR;
    }

    err = bind(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_BIND_ERROR;
    }

    freeaddrinfo(address);
    return sofd;
}


/*
int  _tcp_connect(int sofd, char* hostname, int port, int family) 

IPv4/IPv6 のTCPのクライアントソケットを通して，サーバに接続する．

この関数内で呼び出される関数: getaddrinfo(), connect()

@param  sofd     ソケット記述子
@param  hostname サーバ名
@param  port     サーバポート番号
@param  family   プロトコルファミリー（AF_INET/AF_INET6/AF_UNSPEC)． 
                 AF_UNSPEC の場合は先ずIPv6で接続を試み，不可ならIPv4で接続する．

@retval 0以上  作成されたソケット記述子．
@retval JBXL_NET_INFO_ERROR      ホスト情報の取得に失敗．
@retval JBXL_NET_CONNECT_ERROR   接続に失敗．
 */
int  _tcp_connect(int sofd, char* hostname, int port, int family) 
{
    int    err;
    struct addrinfo  hints;
    struct addrinfo* address;
 
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;  

    char* str = itostr_ts(port);
    err = getaddrinfo(hostname, str, &hints, &address);
    freeNull(str);
    if (err!=0) {
        socket_close(sofd);
        return JBXL_NET_INFO_ERROR;
    }

    err = connect(sofd, address->ai_addr, (int)address->ai_addrlen);
    if (err<0) {
        freeaddrinfo(address);
        socket_close(sofd);
        return JBXL_NET_CONNECT_ERROR;
    }

    freeaddrinfo(address);
    return 0;
}


/**
int  accept_intr(int sock, struct sockaddr* cl_addr, socklen_t* cdlen)

accept()でブロック中に割り込みが掛かって，accept()が失敗で終了しても
再施行できるようにするためのラッパー関数 (for Solaris)

@param  sock    ソケット
@param  cl_addr ソケットの情報が入る sockaddr 構造体へのポインタ
@param  cdlen   sockaddr 構造体（*cl_addr）のサイズ

@retval  0以上  accept() で作成されたソケット記述子．
@retval -1      エラー．
 */
int accept_intr(int sock, struct sockaddr* cl_addr, socklen_t* cdlen)
{
    int nsofd = 0;

    do {
        nsofd = (int)accept(sock, cl_addr, cdlen);
    } while (nsofd==-1 && errno==EINTR);

    //if (nsofd<0) Error("accept_intr");

    return nsofd;
}


/**
int  socket_close(int sofd)

未送信のデータを破棄して，ソケットを完全に閉じる．

@attention fork() した場合，親プロセスで socket_close()すると，子プロセスのソケットにも影響を及ぼす．

@param  sofd   クローズしたいソケット記述子を指定．

@retval  1以上 最後に受信したバイト数
@retval  0     正常切断．
@retval -1     エラー
*/
int  socket_close(int sofd)
{
    int  err = -1;

    if (sofd>0) {
        #ifdef WIN32
            err = shutdown(sofd, 2);
            closesocket(sofd);
        #else
            err = shutdown(sofd, SHUT_RDWR);
            close(sofd);
        #endif
    }
    return err;
}


/**
int   set_nonblock_socket(int sock)

*/
int   set_nonblock_socket(int sock)
{
#ifdef WIN32
    u_long val = 1;
    ioctlsocket(sock, FIONBIO, &val);
#else
    int val = fcntl(sock, F_GETFL, 0);
    if (val>=0) fcntl(sock, F_SETFL, val | O_NONBLOCK);
#endif
    return sock;
}


/**
int   set_block_socket(int sock)

*/
int   set_block_socket(int sock)
{
#ifdef WIN32
    u_long val = 0;
    ioctlsocket(sock, FIONBIO, &val);
#else
    int val = fcntl(sock, F_GETFL, 0);
    if (val>=0) fcntl(sock, F_SETFL, val & ~O_NONBLOCK);
#endif
    return sock;
}



////////////////////////////////////////////////////////////////////////////////////////
// Valid Sockets
//

int  get_valid_udp_socket(int min, int max, unsigned short* port)
{
    int  i, sock, range;
    struct addrinfo* address;

    range = max - min + 1;
    *port = rand()%range + min;

    i = 1;
    sock = udp_server_socket((int)*port, &address);
    while(sock<=0 && i<range) {
        (*port)++;
        if (*port>max) *port = ((int)*port)%max + min - 1;
        sock = udp_server_socket((int)*port, &address);
        i++;
    }

    if (sock<=0) *port = 0;

    freeaddrinfo(address);
    return sock;
}


int  get_valid_tcp_server_socket(int min, int max, unsigned short* port)
{
    int  i, sock, range;

    range = max - min + 1;
    *port = rand()%range + min;

    i = 1;
    sock = tcp_server_socket((int)*port);
    while(sock<=0 && i<range) {
        (*port)++;
        if (*port>max) *port = ((int)*port)%max + min - 1;
        sock = tcp_server_socket((int)*port);
        i++;
    }

    if (sock<=0) *port = 0;

    return sock;
}


int  get_valid_tcp_client_socket(int min, int max, char* hname, unsigned short sport, unsigned short* cport)
{
    int  i, sock, range;

    range  = max - min + 1;
    *cport = rand()%range + min;

    i = 1;
    sock = tcp_client_bind_socket(hname, (int)sport, (int)*cport);
    while(sock<0 && i<range) {
        (*cport)++;
        if (*cport>max) *cport = ((int)*cport)%max + min - 1;
        sock = tcp_client_bind_socket(hname, (int)sport, (int)*cport);
        i++;
    }

    if (sock<=0) *cport = 0;

    return sock;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// Communication
// 

/**
int  udp_recv(int sock, char* rmsg, int size, struct addrinfo* sv_addr)

recvform() をラッピングした関数．UDPデータを受信する．

@param  sock    ソケット記述子
@param  rmsg    受信用データバッファ．関数内で初期化される．
@param  size    データバッファのサイズ
@param  sv_addr サーバの情報が格納された addrinfo 構造体へのポインタ．

@retval 1以上  受信したバイト数．
@retval 0      正常切断．
@retval JBXL_NET_RECV_ERROR  失敗
@retval JBXL_ARGS_ERROR      不正な引数
*/
int  udp_recv(int sock, char* rmsg, int size, struct addrinfo* sv_addr)
{
    int cc;
    socklen_t cadlen;

    if (sv_addr==NULL) return JBXL_ARGS_ERROR;

    cadlen = (int)sv_addr->ai_addrlen;
    memset(rmsg, 0, size);
    cc = recvfrom(sock, rmsg, size, 0, sv_addr->ai_addr, &cadlen);

    if (cc<0) cc = JBXL_NET_RECV_ERROR;
    return cc;
}


/**
int  udp_send(int sock, char* smsg, int size, struct addrinfo* sv_addr)

sendto() をラッピングした関数．UDP経由でデータを送る．

データ(smsg)のサイズ sizeに0以下を指定した場合は，smsgは文字列で
あると見なしてサイズを自動的に計算する．

@param  sock    ソケット記述子
@param  smsg    送信するデータ
@param  size    送信するデータ（smsg）のサイズ．サイズが 0以下の場合は smsgは文字列であるとみなす．
@param  sv_addr サーバの情報を格納する addrinfo 構造体へのポインタ．

@retval 0以上  送信したバイト数．
@retval JBXL_NET_SEND_ERROR  失敗
@retval JBXL_ARGS_ERROR      不正な引数
*/
int  udp_send(int sock, char* smsg, int size, struct addrinfo* sv_addr)
{
    int cc;

    if (sv_addr==NULL) return JBXL_ARGS_ERROR;

    if (size<=0) size = (int)strlen(smsg);
    cc = sendto(sock, smsg, size, 0, sv_addr->ai_addr, (int)sv_addr->ai_addrlen);

    if (cc<0) cc = JBXL_NET_SEND_ERROR;
    return cc;
}


/**
int  tcp_recv(int sock, char* rmsg, int size)

recv()をラッピングした関数．TCP経由でデータを受信する．

@param  sock   ソケット記述子
@param  rmsg   受信用データバッファ．関数内で初期化される．
@param  size   データバッファのサイズ

@retval 1以上 受信したバイト数．
@retval 0     正常切断．
@retval JBXL_NET_RECV_ERROR  失敗
*/
int  tcp_recv(int sock, char* rmsg, int size)
{
    int cc;

    memset(rmsg, 0, size);
    cc = recv(sock, rmsg, size, 0);

    if (cc<0) cc = JBXL_NET_RECV_ERROR;
    return cc;
}


/**
int  tcp_send(int sock, char* smsg, int size)

send()をラッピングした関数．TCP経由でデータを送る．

データ(smsg)のサイズ sizeに0以下を指定した場合は，smsgは文字列であると見なして,サイズを自動的に計算する．

@param  sock   ソケット記述子
@param  smsg   送信するデータ
@param  size   送信するデータ（smsg）のサイズ．サイズが 0以下の場合は smsgは文字列であるとみなす．

@retval  0以上 送信したバイト数．
@retval  JBXL_NET_SEND_ERROR  失敗
*/
int  tcp_send(int sock, char* smsg, int size)
{
    int cc;

    if (size<=0) size = (int)strlen(smsg);
    cc = send(sock, smsg, size, 0);

    if (cc<0) cc = JBXL_NET_SEND_ERROR;
    return cc;
}


/**
int  udp_recv_wait(int sock, char* rmsg, int size, struct addrinfo* sv_addr, int tm)

UDP経由でデータを受信する．

タイムアウトの設定が可能．タイムアウトに 0を指定した場合, recv_wait()
関数を呼び出した時点で読み込み可能データがなければすぐにタイムアウトとなる (JBXL_NET_RECV_TIMEOUT が返る)．

@param  sock     ソケット記述子
@param  rmsg     受信用データバッファ．関数内で初期化される．
@param  size     データバッファのサイズ
@param  sv_addr  サーバの情報が格納された addrinfo 構造体へのポインタ．
@param  tm       タイムアウト時間．秒単位．

@retval  1以上   受信したバイト数．
@retval  0       正常切断．
@retval  JBXL_NET_RECV_ERROR     失敗．
@retval  JBXL_NET_RECV_TIMEOUT   タイムアウト．
*/
int  udp_recv_wait(int sock, char* rmsg, int size, struct addrinfo* sv_addr, int tm)
{
    int  cc;

    if (recv_wait(sock, tm)) {
        cc = udp_recv(sock, rmsg, size, sv_addr); 
    }
    else {
        return JBXL_NET_RECV_TIMEOUT;
    }

    if (cc<0) cc = JBXL_NET_RECV_ERROR;
    return cc;
}


/**
int  tcp_recv_wait(int sock, char* mesg, int sz, int tm)

TCP経由でデータを受信する．

タイムアウトの設定が可能．タイムアウトに 0を指定した場合, recv_wait()
関数を呼び出した時点で読み込み可能データがなければすぐにタイムアウトとなる (JBXL_NET_RECV_TIMEOUT が返る)．

@param  sock    ソケット記述子
@param  mesg    受信用データバッファ．関数内で初期化される．
@param  sz      データバッファのサイズ
@param  tm      タイムアウト時間．秒単位．

@retval  1以上  受信したバイト数．
@retval  0      おそらくは相手側がセッションをクローズした．
@retval  JBXL_NET_RECV_ERROR     受信エラー．
@retval  JBXL_NET_RECV_TIMEOUT   タイムアウト．
*/
int  tcp_recv_wait(int sock, char* mesg, int sz, int tm)
{
    int  cc;

    memset(mesg, 0, sz);
    if (recv_wait(sock, tm)) {
        cc = recv(sock, mesg, sz, 0); 
    }
    else {
        return JBXL_NET_RECV_TIMEOUT;
    }

    if (cc<0) cc = JBXL_NET_RECV_ERROR;
    return cc;
}


/**
int  tcp_send_mesgln(int sock, char* mesg)

TCPメッセージ(文字列)に改行(@\r@\n)を付け加えて送信する．

@param  sock   ソケット記述子
@param  mesg   送信用メッセージ

@retval 0以上  送信したバイト数．
@retval JBXL_NET_SEND_ERROR  失敗．
*/
int  tcp_send_mesgln(int sock, char* mesg)
{
    int   cc, sz;
    char* buf;

    sz = (int)strlen(mesg) + 3;    /* CR+LF+0x00 */
    buf = (char*)malloc(sz);
    if (buf==NULL) return JBXL_NET_SEND_ERROR;

    strncpy(buf, mesg, sz);
    strncat(buf, "\r\n", 2);
    cc = send(sock, buf, (int)strlen(buf), 0);

    free(buf);
    if (cc<0) cc = JBXL_NET_SEND_ERROR;
    return cc;
}


/**
int  tcp_recv_mstream(int sock, char* mesg, int sz, mstream* sb, int tm)

TCP経由でメッセージ(文字列)を受信する．受信メッセージはメッセージストリームバッファに一旦バッファリングされ，
この関数により一行ずつ読み出される．mesgには最大 sz-1文字が格納される．
もし，バッファ中の一行のデータが sz-1より大きい場合は，はみ出した部分は捨てられる．

mesgに格納される時，行中の改行コードは削除され，行末には必ず '@\0' が入る．
タイムアウトの設定が可能でタイムアウトに 0を指定した場合, 呼び出した時点で
読み込み可能データがなければすぐにタイムアウトとなる (JBXL_NET_RECV_TIMEOUT が返る)．

メッセージストリームのバッファ部が確保されていない場合は，最初に呼び出された時点で確保される．
一旦この関数を使用して，受信データをバッファリングしたら，ソケットをクローズするまで，
読み取りには必ず同じストリームを使用してこの関数を呼び出さばければならない．
そうで無い場合は受信データの整合性は保証されない．

@param  sock    ソケット記述子
@param  mesg    受信用データバッファ．予め十分なメモリ領域を確保しておく．
@param  sz      データバッファのサイズ
@param  sb      リングバッファ型のストリームバッファ． バッファ部が確保されていなければ，自動的に確保される．
@param  tm      タイムアウト時間．秒単位．

@retval 1以上  mesgに格納したメッセージのバイト数．
@retval 0      おそらくは相手側がセッションをクローズした．
@retval JBXL_NET_RECV_ERROR     受信エラー．
@retval JBXL_ARGS_ERROR         引数に NULLのデータがある．
@retval JBXL_MALLOC_ERROR       メッセージバッファ部が無いので確保しようとしたが，確保に失敗した．
@retval JBXL_NET_BUF_ERROR      メッセージバッファにデータは存在するはずだが，原因不明の理由により獲得に失敗した．
@retval JBXL_NET_BUFSZ_ERROR    メッセージバッファ中のデータの長さが，mesgの長さより大きい．はみ出した部分は捨てられた．
@retval JBXL_NET_RECV_TIMEOUT   タイムアウト．
*/
int  tcp_recv_mstream(int sock, char* mesg, int sz, mstream* sb, int tm)
{
    int  cc;
    unsigned char* pp;

    if (mesg==NULL || sb==NULL) return JBXL_ARGS_ERROR;
    //memset(mesg, 0, sz);

    if (sb->buf==NULL) {
        *sb = make_mstream(RECVBUFSZ);
        if (sb->buf==NULL) return JBXL_MALLOC_ERROR;
    }

    while (sb->datano==0) {
        cc = tcp_recv_wait(sock, mesg, sz, tm);
        if (cc<=0) {
            if (cc<0) cc = JBXL_NET_RECV_ERROR;
            return cc;
        }
        put_mstream(sb, (unsigned char*)mesg);
        //memset(mesg, 0, sz);
    }

    pp = get_mstream(sb);
    if (pp==NULL) return JBXL_NET_BUF_ERROR;
    if (strlen((const char*)pp)>=(unsigned int)sz) {
        memcpy(mesg, pp, sz-1);
        free(pp);
        return JBXL_NET_BUFSZ_ERROR;
    }
    memcpy(mesg, pp, strlen((const char*)pp));

    free(pp);
    return (int)strlen(mesg);
}


/**
int   recv_wait(int sock, int tm)

簡易 受信 wait. ソケットにデータが受信されるまで待つ．スレッド対応

@param  sock  監視するソケット
@param  tm    タイムアウトの秒数

@retval TRUE  バッファにデータがある．
@retval FALSE タイムアウトした．
*/
int   recv_wait(int sock, int tm)
{
    int    nd;
    fd_set mask;
    struct timeval timeout;
    time_t otm, ntm;

    otm = time(NULL);
    do {
        timeout.tv_sec  = tm;
        timeout.tv_usec = 0;
        FD_ZERO(&mask);
        FD_SET(sock, &mask);

        //DEBUG_MESG("Waiting sock = %d for %ds.\n", sock, tm);
        nd  = select(sock+1, &mask, NULL, NULL, &timeout);
        ntm = time(NULL);
    } while ((nd<0 || !FD_ISSET(sock, &mask)) && (int)(ntm-otm)<=tm);

    return FD_ISSET(sock, &mask);     
}


/**
int   recv_wait_twin(int sock1, int sock2, int tm)

簡易 受信 wait. 二つのソケット sock1, sock2 にデータが受信されるまで待つ．スレッド対応

@param  sock1 監視するソケット その1
@param  sock2 監視するソケット その2
@param  tm    タイムアウトの秒数

@retval 0     タイムアウトした．
@retval 1     sock1 のバッファにデータがある．
@retval 2     sock2 のバッファにデータがある．
@retval 3     sock1 と sock2 のバッファにデータがある．
*/
int   recv_wait_twin(int sock1, int sock2, int tm)
{
    int    ret = 0;
    int    nm, nd;
    fd_set mask;
    struct timeval timeout;
    time_t otm, ntm;

    nm  = Max(sock1, sock2);
    otm = time(NULL);
    do {
        timeout.tv_sec  = tm;
        timeout.tv_usec = 0;
        FD_ZERO(&mask);
        FD_SET(sock1, &mask);
        FD_SET(sock2, &mask);
        nd  = select(nm+1, &mask, NULL, NULL, &timeout);
        ntm = time(NULL);
    } while ((nd<0 || (!FD_ISSET(sock1, &mask) && !FD_ISSET(sock2, &mask))) && (int)(ntm-otm)<=tm);

    if (FD_ISSET(sock1, &mask)) ret += 1;
    if (FD_ISSET(sock2, &mask)) ret += 2;

    return ret;
}


/**
int   send_wait(int sock, int tm)

簡易 送信 wait. データが送信可能になるまで待つ．スレッド対応

@param  sock  監視するソケット
@param  tm    タイムアウトの秒数

@retval TRUE  バッファにデータがある．
@retval FALSE タイムアウトした．
*/
int   send_wait(int sock, int tm)
{
    int    nd;
    fd_set mask;
    struct timeval timeout;
    time_t otm, ntm;

    otm = time(NULL);
    do {
        timeout.tv_sec  = tm;
        timeout.tv_usec = 0;
        FD_ZERO(&mask);
        FD_SET(sock, &mask);

        //DEBUG_MESG("Waiting sock = %d for %ds.\n", sock, tm);
        nd  = select(sock+1, NULL, &mask, NULL, &timeout);
        ntm = time(NULL);
    } while ((nd<0 || !FD_ISSET(sock, &mask)) && (int)(ntm-otm)<=tm);

    return FD_ISSET(sock, &mask);     
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IP address
//

/**
struct sockaddr*  make_sockaddr_bynum(unsigned char* addr, int port, int family)

バイナリの IPv4/IPv6 アドレスとポート番号から 有効な struct sockaddr を作り出す

@param  addr    バイナリのIPアドレス（IPv4/IPv6） 
@param  port    ポート番号
@param  family  プロトコルファミリー（AF_INET/AF_INET6)
@return sa      struct sockaddr へのポインタ．要 free
*/
struct sockaddr*  make_sockaddr_bynum(unsigned char* addr, int port, int family)
{
    struct sockaddr* sa = NULL;
    int len;

    len = sizeof(struct sockaddr);
    sa  = (struct sockaddr*)malloc(len);
    if (sa==NULL) return NULL;
    memset(sa, 0, len);

    if (family==AF_INET6) {     // IPv6
        struct sockaddr_in6* sa6;
        len = sizeof(struct sockaddr_in6);
        sa6 = (struct sockaddr_in6*)malloc(len);
        memset(sa6, 0, len);
        //
        sa6->sin6_family = AF_INET6;
        sa6->sin6_port   = htons(port);
        //memcpy(sa6->sin6_addr.s6_addr, addr, 16);
        memcpy(&(sa6->sin6_addr), addr, 16);
        sa = (struct sockaddr*)sa6;
    }
    //
    else {                      // IPv4
        struct sockaddr_in* sa4;
        len = sizeof(struct sockaddr_in);
        sa4 = (struct sockaddr_in*)malloc(len);
        memset(sa4, 0, len);
        //
        sa4->sin_family = AF_INET;
        sa4->sin_port   = htons(port);
        //memcpy(&(sa4->sin_addr.s_addr), addr, 4);
        memcpy(&(sa4->sin_addr), addr, 4);
        sa = (struct sockaddr*)sa4;
    }

    return sa;
}


/**
struct sockaddr*  make_sockaddr_bystr(const char* addr, int port)

文字列の IPv4/IPv6 アドレスとポート番号から 有効な struct sockaddr を作り出す

@param  addr    文字列のIPアドレス（IPv4/IPv6） 
@param  port    ポート番号
@return sa      struct sockaddr へのポインタ．要 free
*/
struct sockaddr*  make_sockaddr_bystr(const char* addr, int port)
{
    int   err, family = -1;
    char* pp;
    unsigned char num[16];
    struct sockaddr* sa;

    pp = strstr((char*)addr, ".");
    if (pp!=NULL) {
        family = AF_INET;
    }
    else {
        pp = strstr((char*)addr, ":");
        if (pp!=NULL) {
            family = AF_INET6;
        }
    }
    if (family<0) return NULL;

    memset(num, 0, 16);
    err = inet_pton(family, addr, num);
    if (err!=1) return NULL;

    sa  = make_sockaddr_bynum(num, port, family);
    return sa;
}


/**
char*  get_hostname_bynum(unsigned char* num, int family) 

IPv4/IPv6 アドレス（バイナリ）からホスト名を逆引きする． 

@param  num    IPv4/IPv6 アドレス（バイナリ） 
@param  family アドレスファミリー（AF_INET/AF_INET6） 
@return ホスト名（文字列）．要 free
*/
char*  get_hostname_bynum(unsigned char* num, int family) 
{
    int   len, err;
    char  htemp[LNAME];
    char* hname = NULL;
    struct sockaddr* sa;

    if (num==NULL) return NULL;
    if (family!=AF_INET6) family = AF_INET6;

    sa = make_sockaddr_bynum(num, 0, family);
    if (sa==NULL) return NULL;

    if (family==AF_INET6) len = sizeof(struct sockaddr_in6);
    else                  len = sizeof(struct sockaddr_in);
    err = getnameinfo(sa, len, htemp, LNAME-1, NULL, 0, 0);
    if (err!=0) {
        free(sa);
        return NULL;
    }

    len = (int)strlen(htemp);
    hname = (char*)malloc(len+1);
    if (hname==NULL) {
        free(sa);
        return NULL;
    }

    free(sa);
    memcpy(hname, htemp, len+1);
    return hname;    
}


/**
char*  get_hostname_bystr(const char* addr) 

IPv4/IPv6 アドレス（文字列）からホスト名を逆引きする． 

@param  addr IPv4/IPv6 アドレス（文字列） 
@return ホスト名（文字列）．要 free
*/
char*  get_hostname_bystr(const char* addr) 
{
    int   len, err;
    char  htemp[LNAME];
    char* hname = NULL;
    struct sockaddr* sa;

    if (addr==NULL) return NULL;

    sa = make_sockaddr_bystr(addr, 0);
    if (sa==NULL) return NULL;

    if (sa->sa_family==AF_INET6) len = sizeof(struct sockaddr_in6);
    else                         len = sizeof(struct sockaddr_in);
    err = getnameinfo(sa, len, htemp, LNAME-1, NULL, 0, 0);
    if (err!=0) {
        free(sa);
        return NULL;
    }

    len = (int)strlen(htemp);
    hname = (char*)malloc(len+1);
    if (hname==NULL) {
        free(sa);
        return NULL;
    }

    free(sa);
    memcpy(hname, htemp, len+1);
    return hname;    
}


/**
char*  _get_hostname(const char* addr, int family)

IPv4/IPv6 アドレス（文字列）からホスト名を逆引きする．

FQDNを指定した場合は，正引きして逆引きするので，FQDNのチェックに使用できる．

@param  addr   IPアドレス（文字列）または FQDN
@param  family プロトコルファミリー（AF_INET/AF_INET6)

@return ホスト名（文字列）．要 free
*/
char*  _get_hostname(const char* addr, int family)
{
    int   len, err;
    char  htemp[LNAME];
    char* hname = NULL;

    struct addrinfo  hints;
    struct addrinfo* address;
    struct sockaddr* sa;
 
    if (addr==NULL) return NULL;
    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;  

    err = getaddrinfo(addr, 0, &hints, &address);
    if (err!=0 || address==NULL) return NULL;
    sa = (struct sockaddr*)address->ai_addr;

    if (sa->sa_family==AF_INET6) len = sizeof(struct sockaddr_in6);
    else                         len = sizeof(struct sockaddr_in);
    err = getnameinfo(sa, len, htemp, LNAME-1, NULL, 0, 0);
    if (err!=0) {
        freeaddrinfo(address);
        return NULL;
    }

    len = (int)strlen(htemp);
    hname = (char*)malloc(len+1);
    if (hname==NULL) {
        freeaddrinfo(address);
        return NULL;
    }

    freeaddrinfo(address);
    memcpy(hname, htemp, len+1);
    return hname;    
}


/**
char*  get_ipaddr_byname(const char* hostname, int family)

ホスト名から IPv4/IPv6 アドレス（文字列）を獲得する．

@param  hostname ホスト名
@param  family IPのアドレスファミリー AF_INET/AF_INET6
@return IPv4/IPv6 アドレス（文字列）へのポインタ．要 free
*/
char*  get_ipaddr_byname(const char* hostname, int family)
{
    int   len;
    char* ip;
    unsigned char* ipnum;

    if (family!=AF_INET6) family = AF_INET;

    ipnum = get_ipaddr_byname_num(hostname, family);   // 4Byte or 16Byte
    if (ipnum==NULL) return NULL;

    if (family==AF_INET6) len = LEN_IPADDR6;
    else                  len = LEN_IPADDR;
    ip = (char*)malloc(len);
    if (ip==NULL) return NULL;
    memset(ip, 0, len);
        
    inet_ntop(family, ipnum, ip, len);
    free(ipnum);
    return ip;
}


/**
unsigned char*  get_ipaddr_byname_num(const char* hostname, int family)

ホスト名から IPv4/IPv6 アドレス（バイナリ）を獲得する．

@param  hostname ホスト名
@param  family IPのアドレスファミリー AF_INET/AF_INET6
@return IPv4/IPv6 アドレス（バイナリ）へのポインタ（長さ 4/16Byte）．要 free
*/
unsigned char*  get_ipaddr_byname_num(const char* hostname, int family)
{
    int err;
    struct addrinfo  hints;
    struct addrinfo* address;
    unsigned char* ip = NULL;
    unsigned char* pp = NULL;

    if (family!=AF_INET6) family = AF_INET;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family   = family;

    err = getaddrinfo(hostname, NULL, &hints, &address); 
    if (err!=0 || address==NULL) return NULL;

    if (family==AF_INET6) {  // IPv6
        ip = (unsigned char*)malloc(16);
        if (ip!=NULL) {
            //pp = (unsigned char*)(((struct sockaddr_in6*)(address->ai_addr))->sin6_addr.s6_addr);  // unsigned char s6_addr[16]; 
            pp = (unsigned char*)&(((struct sockaddr_in6*)(address->ai_addr))->sin6_addr);  // unsigned char s6_addr[16]; 
            memcpy(ip, pp, 16);
        }
    }
    //
    if (pp==NULL) {         // IPv4
        ip = (unsigned char*)malloc(4);
        if (ip!=NULL) {
            //pp = (unsigned char*)(&((struct sockaddr_in*)(address->ai_addr))->sin_addr.s_addr);    // ulong s_addr; 
            pp = (unsigned char*)&(((struct sockaddr_in*)(address->ai_addr))->sin_addr);    // ulong s_addr; 
            memcpy(ip, pp, 4);
        }
    }

    freeaddrinfo(address);
    return ip;
}


#ifdef WIN32

#include <iphlpapi.h>
#pragma comment( lib, "iphlpapi.lib" )

char* get_myipaddr(int family)
{
    int   len, prefix, fndflg;
    char* ipa;
    char  address[LNAME];
    IP_ADAPTER_ADDRESSES* pp, * pl;
    IP_ADAPTER_UNICAST_ADDRESS* pu;

    len = LEN_IPADDR6;
    if (family != AF_INET6) {
        len = LEN_IPADDR;
        family = AF_INET;
    }

    ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
    DWORD err, size = 0;

    err = GetAdaptersAddresses(family, flags, NULL, NULL, &size);
    if (err != ERROR_BUFFER_OVERFLOW) return NULL;

    pp = (IP_ADAPTER_ADDRESSES*)malloc(size);
    if (pp == NULL) return NULL;
    err = GetAdaptersAddresses(family, flags, NULL, pp, &size);
    if (err != ERROR_SUCCESS) {
        free(pp);
        return NULL;
    }

    prefix = 0;
    fndflg = OFF;
    pl = pp;
    while (pl != NULL) {
        pu = pl->FirstUnicastAddress;
        while (pu != NULL) {
            if ((long)pu->ValidLifetime >= 0) {
                prefix = pu->OnLinkPrefixLength;
                struct sockaddr* sa = pu->Address.lpSockaddr;
                if (family == AF_INET6) {
                    unsigned char* ch = (unsigned char*)&((struct sockaddr_in6*)sa)->sin6_addr;
                    if (!(ch[0] == 0xfe && ch[1] == 0x80)) {
                        fndflg = ON;
                        inet_ntop(AF_INET6, ch, address, LNAME);
                        break;
                    }
                }
                else {
                    unsigned char* ch = (unsigned char*)&((struct sockaddr_in*)sa)->sin_addr;
                    if (!(ch[0] == 169 && ch[1] == 254)) {
                        fndflg = ON;
                        inet_ntop(AF_INET, ch, address, LNAME);
                        break;
                    }
                }
            }
            pu = pu->Next;
        }
        if (fndflg == ON) break;
        pl = pl->Next;
    }
    free(pp);

    int addrlen = (int)strlen(address);
    snprintf(address + addrlen, LNAME - addrlen - 1, "/%d\0", prefix);

    ipa = (char*)malloc(len);
    if (ipa == NULL) return NULL;
    memset(ipa, 0, len);
    memcpy(ipa, address, strlen(address)+1);

    return ipa;
}

#else

#include <ifaddrs.h>

char*  get_myipaddr(int family) 
{
    int    err, flg, len, lst;
    struct ifaddrs *if_addr;
    struct ifaddrs *ifap;

    char  htemp[LNAME];
    char* haddr = NULL;

    if (family!=AF_INET6) {
        family = AF_INET;
         len = sizeof(struct sockaddr_in);
    }
    else {
         len = sizeof(struct sockaddr_in6);
    }
    //
    err = getifaddrs(&if_addr);
    if (err<0) return NULL;

    flg  = OFF;
    ifap = if_addr;
    while(ifap!=NULL) {
        if (ifap->ifa_addr!=NULL && family==ifap->ifa_addr->sa_family) {
            memset(htemp, 0, LNAME);
            err = getnameinfo(ifap->ifa_addr, len, htemp, LNAME, NULL, 0, NI_NUMERICHOST);
            if (err==0) {
                if (family==AF_INET) {
                    if (strncmp(htemp, "127.0.0.1", 9) && strncmp(htemp, "169.254.", 8)) {
                        struct sockaddr_in* sa = (struct sockaddr_in*)(ifap->ifa_addr);
                        unsigned char check = ((unsigned char*)&(sa->sin_addr))[0] & 0xf0;
                        if (check != 0xe) { // not multi cast
                            flg = ON;
                            break;
                        }
                    }
                }
                else {
                    if (strncmp(htemp, "::1", 3) && strncasecmp(htemp, "fe80:", 5)) {
                        flg = ON;
                        break;
                    }
                }
            }
        }
        ifap=ifap->ifa_next;
    }
    if (flg==OFF) {
        freeifaddrs(if_addr);
        return NULL;
    }
    
    freeifaddrs(if_addr);

    lst = strlen(htemp);
    if (LNAME-lst-1<=0) return NULL;
    htemp[lst++] = '/';

    err = getnameinfo(ifap->ifa_netmask, len, &(htemp[lst]), LNAME-lst, NULL, 0, NI_NUMERICHOST);
    if (err!=0) return NULL;

    len = strlen(htemp) + 1;
    haddr = (char*)malloc(len);
    if (haddr==NULL) return NULL;
    memset(haddr, 0, len);
    memcpy(haddr, htemp, len-1);

    return haddr;    
}

#endif


unsigned char*  get_myipaddr_num(int family) 
{
    int   len, err;
    char* pos;
    char* htemp;
    unsigned char* haddr = NULL;

    if (family!=AF_INET6) family = AF_INET;

    //
    htemp = get_myipaddr(family);
    if (htemp==NULL) return NULL;

    pos = strstr(htemp, "/");
    if (pos==NULL) {
        free(htemp);
        return NULL;
    }
    *pos = '\0';

    if (family==AF_INET6) len = 16;
    else                  len = 4;
    haddr = (unsigned char*)malloc(len*2);
    if (haddr==NULL) {
        free(htemp);
        return NULL;
    }

    err = inet_pton(family, htemp, haddr);
    if (err!=1) {
        free(haddr);
        return NULL;
    }

    err = inet_pton(family, pos+1, haddr+len);
    free(htemp);
    if (err!=1) {
        free(haddr);
        return NULL;
    }

    return haddr;    
}


/**
char*  _get_localip_bydest(const char* dest, int family)

接続先を指定して，そこへ接続する インターフェイスのIPアドレスを得る．自分自身の IPアドレスを得るもう一つの方法．

@param  dest   接続先の IPv4/IPv6 アドレス または FQDN
@param  family プロトコルファミリー（AF_INET/AF_INET6)

@return IPv4/IPv6 アドレス(文字列)へのポインタ．要 free
*/
char*  _get_localip_bydest(const char* dest, int family)
{
    int   sofd, err, len;
    char* addr;
    unsigned char* pp;
    struct addrinfo  hints;
    struct addrinfo* sv_addr;
    struct sockaddr* sa;             

    if (family!=AF_INET && family!=AF_INET6 && family!=AF_UNSPEC) family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = family;       
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;

    err = getaddrinfo(dest, "9999", &hints, &sv_addr);
    if (err != 0) {
        // if 10093 error is occured, execute init_network()
        return NULL;
    }

    sofd = (int)socket(sv_addr->ai_family, sv_addr->ai_socktype, sv_addr->ai_protocol);
    if (sofd<0) {
        freeaddrinfo(sv_addr);
        return NULL;
    }

    err = connect(sofd, sv_addr->ai_addr, (int)sv_addr->ai_addrlen);
    if (err<0) {
        socket_close(sofd);
        return NULL;
    }

    if (sv_addr->ai_family==AF_INET6) {
        len = sizeof(struct sockaddr_in6);
        sa  = (struct sockaddr*)malloc(len);
        memset(sa, 0, len);
        err = getsockname(sofd, (struct sockaddr *)sa, (socklen_t*)&len);
        //pp = (unsigned char*)(((struct sockaddr_in6*)sa)->sin6_addr.s6_addr);
        pp = (unsigned char*)&(((struct sockaddr_in6*)sa)->sin6_addr);
        len = LEN_IPADDR6;
    }
    else {
        len = sizeof(struct sockaddr_in);
        sa  = (struct sockaddr*)malloc(len);
        memset(sa, 0, len);
        err = getsockname(sofd, (struct sockaddr *)sa, (socklen_t*)&len);
        //pp = (unsigned char*)(&((struct sockaddr_in*)sa)->sin_addr.s_addr);
        pp = (unsigned char*)&(((struct sockaddr_in*)sa)->sin_addr);
        len = LEN_IPADDR;
    }

    freeaddrinfo(sv_addr);
    socket_close(sofd);

    addr = (char*)malloc(len);
    if (addr==NULL) {
        free(sa);
        return NULL;
    }

    inet_ntop(family, pp, addr, len);
    free(sa);
    return addr;
}


/**
char*  get_mynetaddr(int family)

自分のネットワークアドレスを返す

@param  family プロトコルファミリー（AF_INET/AF_INET6)

@return ネットワークアドレス(文字列)へのポインタ．要 free
*/
char*  get_mynetaddr(int family)
{
    int  len;
    char* net;
    unsigned char* pp;

    len = LEN_IPADDR6;
    if (family!=AF_INET6) {
        len = LEN_IPADDR;
        family = AF_INET;
    }

    pp = get_mynetaddr_num(family);
    if (pp==NULL) return NULL;

    net = (char*)malloc(len);
    if (net==NULL) {
        free(pp);
        return NULL;
    }
    memset(net, 0, len);

    inet_ntop(family, pp, net, len);
    free(pp);
    return net;
}


/**
unsigned char*  get_mynetaddr_num(int family)

自分のネットワークアドレスを返す

@return ネットワークアドレス(バイナリ)へのポインタ（長さ4byte）．要 free
*/
unsigned char*  get_mynetaddr_num(int family)
{
    int  i, len;
    unsigned char* net;
    unsigned char* ip;
    unsigned char* mk;
        
    len = 16;
    if (family!=AF_INET6) len = 4;

    ip = get_myipaddr_num(family);
    if (ip==NULL) return NULL;
    mk = &(ip[len]);

    net = (unsigned char*)malloc(len);
    if (net==NULL) {
        free(ip);
        return NULL;
    }

    for (i=0; i<len; i++) net[i] = ip[i] & mk[i];

    free(ip);
    return net;
}



//////////////////////////////////////////////////////////////////////////////////
// Tools
//

/**
int  is_same_sockaddr(struct sockaddr* addr1, struct sockaddr* addr2) 

addr1 と addr2が格納する IPアドレスとポート番号が同じかどうか検査する

@param  addr1  比較する sockaddr 構造体へのポインタ
@param  addr2  比較する sockaddr 構造体へのポインタ

@retval TRUE   同じホスト
@retval FALSE  違うホスト
*/
int  is_same_sockaddr(struct sockaddr* addr1, struct sockaddr* addr2) 
{
    int  i, len;
    unsigned char       *p1,   *p2;
    struct sockaddr_in  *sa1,  *sa2;
    struct sockaddr_in6 *sa61, *sa62;

    if (addr1==NULL || addr2==NULL) return FALSE;
    if (addr1->sa_family!=addr2->sa_family) return FALSE;

    if (addr1->sa_family==AF_INET) {
        sa1 = (struct sockaddr_in*)addr1;
        sa2 = (struct sockaddr_in*)addr2;

        p1  = (unsigned char*)&(sa1->sin_addr);
        p2  = (unsigned char*)&(sa2->sin_addr);
        len = sizeof(sa1->sin_addr);
        for (i=0; i<len; i++) {
            if (p1[i]!=p2[i]) return FALSE;
        }

        p1  = (unsigned char*)&(sa1->sin_port);
        p2  = (unsigned char*)&(sa2->sin_port);
        len = sizeof(sa1->sin_port);
        for (i=0; i<len; i++) {
            if (p1[i]!=p2[i]) return FALSE;
        }
    }
    else {
        sa61 = (struct sockaddr_in6*)addr1;
        sa62 = (struct sockaddr_in6*)addr2;

        p1  = (unsigned char*)&(sa61->sin6_addr);
        p2  = (unsigned char*)&(sa62->sin6_addr);
        len = sizeof(sa61->sin6_addr);
        for (i=0; i<len; i++) {
            if (p1[i]!=p2[i]) return FALSE;
        }

        p1  = (unsigned char*)&(sa61->sin6_port);
        p2  = (unsigned char*)&(sa62->sin6_port);
        len = sizeof(sa61->sin6_port);
        for (i=0; i<len; i++) {
            if (p1[i]!=p2[i]) return FALSE;
        }
    }

    return TRUE;
}


/**
int  is_same_network(char* addr1, char* addr2, char* mask)

文字型アドレス addr1 と addr2が同じネットワークに属しているかどうかチェックする．@n
addr1, addr2 は FQDNでも可．

@param  addr1  比較するアドレス（文字型）．FQDN は不可
@param  addr2  比較するアドレス（文字型）．FQDN は不可
@param  mask   ネットマスク（文字型）

@retval TRUE   同じネットワークである．
@retval FALSE  同じネットワークでない．
*/
int  is_same_network(char* addr1, char* addr2, char* mask)
{
    int  ret, family;
    unsigned char* a1;
    unsigned char* a2;
    unsigned char* mk;

    if (addr1==NULL || addr2==NULL || mask==NULL) return FALSE;

    a1 = (unsigned char*)strstr(addr1, ":");
    a2 = (unsigned char*)strstr(addr1, ":");
    mk = (unsigned char*)strstr(mask,  ":");
    if      (a1!=NULL && a2!=NULL && mask!=NULL)  family = AF_INET6;
    else if (a1==NULL && a2==NULL && mask==NULL)  family = AF_INET;
    else return FALSE;

    a1 = to_address_num(addr1, 0, 0, family);
    a2 = to_address_num(addr2, 0, 0, family);
    mk = to_address_num(mask,  0, 0, family);
    if (a1==NULL || a2==NULL || mk==NULL) {
        freeNull(a1);
        freeNull(a2);
        freeNull(mk);
        return FALSE;
    }

    ret = is_same_network_num(a1, a2, mk, family);
    freeNull(a1);
    freeNull(a2);
    freeNull(mk);
    
    return ret;
}


/**
int  is_same_network_num(unsigned char* addr1, unsigned char* addr2, unsigned char* mask, int family)

数字型アドレス addr1 と addr2が同じネットワークに属しているかどうかチェックする．

@param  addr1  比較するアドレス（数字型）4/16Byte
@param  addr2  比較するアドレス（数字型）4/16Byte
@param  mask   ネットマスク（数字型）4/16Byte
@param  family アドレスファミリー (AF_INET/AF_INET6)

@retval TRUE   同じネットワークである．
@retval FALSE  同じネットワークでない
*/
int  is_same_network_num(unsigned char* addr1, unsigned char* addr2, unsigned char* mask, int family)
{
    int i, len;

    if (addr1==NULL || addr2==NULL) return FALSE;

    len = 16;
    if (family!=AF_INET6) len = 4;

    if (mask==NULL) {
        for (i=0; i<len; i++) {
            if (addr1[i] != addr2[i]) return FALSE;
        }
    }
    else {
        for (i=0; i<len; i++) {
            if ((addr1[i] & mask[i]) != (addr2[i] & mask[i])) return FALSE;
        }
    }
    return TRUE;
}


/**
unsigned char*  to_address_num(char* addr, int mode, int mask, int family)

文字型のアドレス [IPアドレス]/[ネットマスク] (例：202.26.159.140/255.255.255.0）を
数字型のアドレス（unsigned char* num）8/32Byte へ変換する．

省略された場所は 0とみなされる（例：202.26/255 → 202.26.0.0/255.0.0.0） @n
ネットマスク部全体が省略された場合は IPアドレス部に依存する．(202.26. → 202.26.0.0/255.255.0.0) @n
CIDER形式にも対応．ただし，ネットマスク部の妥当性はチェックしない．

@param  addr   変換する文字型のIPアドレス または FQDN
@param  mode   @b 0:     IPアドレス以外のもの（英字）は NULL を返す．
@param  mode   @b 0以外: FQDNはIPアドレスに変換して返す（時間がかかる）
@param  mask   @b 0:     サブネットマスクの処理をしない．
@param  mask   @b 0以外: サブネットマスクの処理を行う．
@param  family アドレスファミリー (AF_INET/AF_INET6)

@return [IPv4/IPv6 アドレス],[ネットマスク] の長さ8/32Byteのバイナリ．要 free
*/
unsigned char*  to_address_num(char* addr, int mode, int mask, int family)
{
    unsigned char* num;
    char  deli;
    char* ps;
    char* pc;
    char* uc = NULL;
    int   i, len;

    if (addr==NULL) return NULL;

    deli = ':';
    len  = 16;
    if (family!=AF_INET6) {
        deli = ':';
        len  = 4;
        family = AF_INET;
    }

    // mode チェック
    if (mode==0) {
        i = (int)strlen(addr) - 1;
        while (i>0 && addr[i]==deli) i--;
        if (i>=0) {
            if (isalpha(addr[i])) return NULL;
        }   
    }

    num = (unsigned char*)malloc(len*2);
    if (num==NULL) return NULL;
    memset(num, 0, len*2);

    // IPアドレス部の変換
    ps = awk(addr, '/', 1);
    if (ps==NULL) {
        free(num);
        return NULL;
    }

    uc = (char*)get_ipaddr_byname_num(ps, family); 
    free(ps);
    if (uc==NULL) return NULL;
    memcpy(num, uc, len);
    free(uc);

    if (mask==0) return num;

    // 以下，サブネットマスク処理
    ps = awk(addr, '/', 2);

    // サブネットマスク部が省略された場合の処理 
    if (ps==NULL) {
        int f = OFF;
        for (i=len-1; i>=0; i--) {
            if (num[i]!=0 || f==ON) {
                num[i+len] = 0xff;
                f = ON;
            }
            else {
                num[i+len] = 0;
            }
        }
        return num;
    }

    // サブネットマスク部の処理
    if (family==AF_INET) {
        for (i=0; i<len; i++) {
            pc = awk(ps, deli, i+1);
            if (pc==NULL) break;
            num[i+len] = (unsigned char)atoi(pc);
            free(pc);
        }
    }
    else {
        for (i=0; i<len/2; i++) {
            pc = awk(ps, deli, i+1);
            if (pc==NULL) break;
            int nn = strtol(pc, NULL, 16);
            num[len+i*2]   = (unsigned char)(nn/256);
            num[len+i*2+1] = (unsigned char)(nn%256);
            free(pc);
        }
    }
    free(ps);

    // CIDER形式対応
    if (family==AF_INET && num[4]<=32) {
        int nn, cl = (int)num[4];
        for (i=0; i<4; i++) {
            nn = 8 - Max(0, Min(8, cl-8*i));
            num[i+4] = 0xff<<nn;
        }
    }
    else if (family==AF_INET6 && num[16]<16) {
        int nn, cl = num[16]%16*100 + num[17]/16*10 + num[17]%16;
        for (i=0; i<16; i++) {
            nn = 8 - Max(0, Min(8, cl-8*i));
            num[i+16] = 0xff<<nn;
        }
    }

    return num;
}


/**
char*  to_address_char(unsigned char* addr, int mask, int family)

数字型の IPv4/IPv6 アドレス（サブネット付き）8/32Byteを文字型の [IPアドレス]/[ネットマスク] に変換する．

addr は必ずネットマスクの情報を含み，長さが 8/32Byteでないといけない．@n
to_address_num()と対で使うのが安全．

@param  addr   変換するバイバリ型のIPアドレス
@param  mask   @b 0:     サブネットマスクの処理をしない．
@param  mask   @b 0以外: サブネットマスクの処理を行う．
@param  family アドレスファミリー (AF_INET/AF_INET6)

@return [IPv4/IPv6 アドレス]/[ネットマスク] 形式の文字列．要 free

@see to_address_num()
*/
char*  to_address_char(unsigned char* addr, int mask, int family)
{
    int mlen, plen, slen;
    char* str;
    if (addr==NULL) return NULL;

    plen = 16;
    mlen = LEN_IPADDR6*2;
    if (family!=AF_INET6) {
        family = AF_INET;
        plen = 4;
        mlen = LEN_IPADDR*2;
    }

    str = (char*)malloc(mlen);
    if (str==NULL) return NULL;
    memset(str, 0, mlen);

    inet_ntop(family, addr, str, mlen);
    if (mask==0) return str;

    slen = (int)strlen(str);
    str[slen++] = '/';
    inet_ntop(family, addr+plen, str+slen, mlen-slen);

    return str;
}


/**
void  udp_hole_punching(int sock, struct sockaddr_in addr, int nm)

sock を使って，addrへ nmバイトの NULLデータを送信する．

@param  sock ソケット記述子
@param  addr 相手のホストの情報が格納されて sockaddr_in 構造体
@param  nm   送信するデータ(NULL)の長さ
*/
void  udp_hole_punching(int sock, struct addrinfo* addr, int nm)
{
    char data[LBUF];

    if (nm<=0) nm = 4;                // for SLVoice
    else if (nm>LBUF) nm = LBUF;

    memset(data, 0, nm);
    udp_send(sock, data, nm, addr);

    return;
}



//////////////////////////////////////////////////////////////////////////////////
// for IPv4 only
//

int  udp_client_socket_sockaddr_in(char* hostname, int port, struct sockaddr_in* sv_addr)
{
    int  sofd;
    struct hostent* shost = NULL; 
 
    if (sv_addr==NULL) return JBXL_NET_INFO_ERROR;

    sofd = (int)socket(AF_INET, SOCK_DGRAM, 0);
    if (sofd<0) return JBXL_NET_SOCKET_ERROR;

    shost = gethostbyname(hostname); 
    if (shost==NULL) {
        socket_close(sofd);
        return JBXL_NET_INFO_ERROR;
    }

    memset(sv_addr, 0, sizeof(*sv_addr));
    sv_addr->sin_family = AF_INET;
    sv_addr->sin_port   = htons(port);
    memcpy(&(sv_addr->sin_addr), shost->h_addr, shost->h_length);
   
    return sofd;
}


int  udp_recv_sockaddr_in(int sock, char* rmsg, int size, struct sockaddr_in* sv_addr)
{
    int cc;
    socklen_t cadlen;

    if (sv_addr==NULL) return 0;

    cadlen = sizeof(*sv_addr);
    memset(rmsg, 0, size);
    cc = recvfrom(sock, rmsg, size, 0, (struct sockaddr*)sv_addr, &cadlen);

    return cc;
}


int  udp_send_sockaddr_in(int sock, char* smsg, int size, struct sockaddr_in* sv_addr)
{
    int cc;

    if (sv_addr==NULL) return 0;
    if (size<=0) size = (int)strlen(smsg);
    cc = sendto(sock, smsg, size, 0, (struct sockaddr*)sv_addr, sizeof(*sv_addr));

    return cc;
}


int  udp_recv_wait_sockaddr_in(int sock, char* rmsg, int size, struct sockaddr_in* sv_addr, int tm)
{
    int  cc;

    if (recv_wait(sock, tm)) {
        cc = udp_recv_sockaddr_in(sock, rmsg, size, sv_addr); 
    }
    else {
        return JBXL_NET_RECV_TIMEOUT;
    }
    return cc;
}


struct sockaddr_in  get_sockaddr_in(char* hostname, unsigned short cport)
{
    struct sockaddr_in  ss_addr;
    struct hostent *shost;

    // Serverの情報を ss_addrに格納
    shost = gethostbyname(hostname);
    if (shost==NULL) {
        ss_addr.sin_family = 0;
        ss_addr.sin_port   = 0;
        return ss_addr;
    }

    memset(&ss_addr, 0, sizeof(ss_addr));
    ss_addr.sin_family = AF_INET;
    ss_addr.sin_port   = htons(cport);
    memcpy(&(ss_addr.sin_addr), shost->h_addr, shost->h_length);

    return ss_addr;
}


struct sockaddr_in  get_sockaddr_in_bynum(char* ipnum, unsigned short cport)
{
    struct sockaddr_in  ss_addr;

    memset(&ss_addr, 0, sizeof(ss_addr));
    ss_addr.sin_family = AF_INET;
    ss_addr.sin_port   = htons(cport);
    memcpy(&(ss_addr.sin_addr), ipnum, 4);

    return ss_addr;
}


struct sockaddr_in  get_local_sockaddr_in(unsigned short cport)
{
    struct sockaddr_in  ss_addr;
    struct hostent *shost;

    // localhost の情報を ss_addrに格納
    shost = gethostbyname("127.0.0.1");

    memset(&ss_addr, 0, sizeof(ss_addr));
    ss_addr.sin_family = AF_INET;
    ss_addr.sin_port   = htons(cport);
    memcpy(&(ss_addr.sin_addr), shost->h_addr, shost->h_length);

    return ss_addr;
}


/**
char*  get_ipaddr_ipv4(struct in_addr sin_addr)

in_addr構造体から IPアドレスを獲得する．

@param  sin_addr  sockaddr_in 構造体の sin_addr メンバ．
@return IPアドレス(文字列)へのポインタ．要 free

@code
   struct sockaddr_in addr
   .........;
   ipaddr = get_ipaddr_ipv4(addr.sin_addr); 
@endcode
*/
char*  get_ipaddr_ipv4(struct in_addr sin_addr)
{
    char* ip;
    unsigned char* pp;

    pp = (unsigned char*)&(sin_addr);
    if (pp[0]==0) return NULL;

    ip = (char*)malloc(16);
    if (ip==NULL) return NULL;
    memset(ip, 0, 16);

    snprintf(ip, 15, "%d.%d.%d.%d", pp[0], pp[1], pp[2], pp[3]);

    return ip;
}


/**
unsigned char*  get_ipaddr_num_ipv4(struct in_addr sin_addr)

in_addr 構造体から IPアドレスを獲得する．

@param  sin_addr  sockaddr_in 構造体の sin_addr メンバ．
@return IPアドレス(バイナリ)へのポインタ（長さ4byte）．要 free

@code
    struct sockaddr_in addr
    .........;
    ipaddr = get_ipaddr_num(addr.sin_addr); 
@endcode
*/
unsigned char*  get_ipaddr_num_ipv4(struct in_addr sin_addr)
{
    unsigned char* ip;
    unsigned char* pp;

    ip = (unsigned char*)malloc(4);
    if (ip==NULL) return NULL;

    pp = (unsigned char*)&(sin_addr);
    memcpy(ip, pp, 4);

    return ip;
}


/**
void  udp_hole_punching_sockaddr_in(int sock, struct sockaddr_in addr, int nm)

sock を使って，addrへ nmバイトの NULLデータを送信する．

@param  sock ソケット記述子
@param  addr 相手のホストの情報が格納されて sockaddr_in 構造体
@param  nm   送信するデータ(NULL)の長さ
*/
void  udp_hole_punching_sockaddr_in(int sock, struct sockaddr_in addr, int nm)
{
    char data[LBUF];

    if (nm<=0) nm = 4;                // for SLVoice
    else if (nm>LBUF) nm = LBUF;

    memset(data, 0, nm);
    udp_send_sockaddr_in(sock, data, nm, &addr);

    return;
}


/** @def  get_ipaddr_byname_ipv4

ホスト名から IPv4 アドレスを獲得する．
*/


/** @def  get_localip_bydest_ipv4

接続先を指定して，そこへ接続する インターフェイスのIPアドレスを得る．自分自身の IPアドレスを得るもう一つの方法．
*/


/** @def  get_myipaddr_num_ipv4

ループバックでないインターフェイスのIPアドレスとネットマスクを返す．@n
*/


/** @def  get_myipaddr_ipv4

ループバックでないインターフェイスのIPアドレスと ネットマスクを返す．@n
ネットマスクが必要ないなら get_localip_ipv4() を使用しても良い．
*/


/** @def  get_mynetaddr_ipv4

自分のネットワークアドレスを返す
*/


/** @def  get_mynetaddr_num_ipv4

自分のネットワークアドレスを返す
*/


/** @def  is_same_network_num_ipv4

数字型アドレス が同じネットワークに属しているかどうかチェックする．
*/


/** @def  is_same_network

文字型アドレス が同じネットワークに属しているかどうかチェックする．@n
アドレスはFQDNでも可．
*/


/** @def  to_address_num8_ipv4

文字型のアドレス [IPアドレス]/[ネットマスク] (例：202.26.159.140/255.255.255.0）を
数字型のアドレス（unsigned char* num[8]）8Byte へ変換する．

省略された場所は 0とみなされる（例：202.26/255 → 202.26.0.0/255.0.0.0） @n
ネットマスク部全体が省略された場合は IPアドレス部に依存する．(202.26. → 202.26.0.0/255.255.0.0) @n
CIDER形式にも対応．ただし，ネットマスク部の妥当性はチェックしない．
*/


/** @def  to_address_char8_ipv4

数字型の IPアドレス n 8Byteを文字型の　[IPアドレス]/[ネットマスク]　に変換する．

n は必ずネットマスクの情報を含み，長さが 8Byteでないといけない．8Byteより少ない場合は
恐らくセグメンテーションエラーを起こす．@n
to_address_num8()と対で使うのが安全．
*/


/** @def  to_address_num4_ipv4

文字型のアドレス [IPアドレス] (例：202.26.159.140）を
数字型のアドレス（unsigned char* num[4]）4Byte へ変換する．

省略された場所は 0とみなされる（例：202.26 → 202.26.0.0）
*/


/** @def  to_address_char4_ipv4

数字型の IPアドレス 4Byteを文字型の [IPアドレス] に変換する．

IPアドレスは必ず長さが 4Byte（32bit）でないといけない．4Byteより少ない場合は恐らくセグメンテーションエラーを起こす．@n
to_address_num4() と対で使うのが安全．
*/

