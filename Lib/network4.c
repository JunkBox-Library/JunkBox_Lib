/** 
@brief   ネットワーク用ライブラリ
@file    network4.c  for IPv4
@author  Fumi.Iseki (C)

このファイルは現在使用されていません．
バージョンのチェックの為だけに残されています．
新しいファイルは network.c (for IPv4/IPv6) をご覧ください．
*/

#include "network.h"
#include <time.h>


/**
int  udp_server_socket(int port)   

UDPのサーバソケットを作り,接続待ち状態になる．

この関数内で呼び出されるネットワーク関数: socket(), bind()

@param  port ポート番号

@retval  0以上 作成されたソケット記述子．
@retval -1     ソケットが作成できなかった．
@retval -3     バインドに失敗．
*/
int  udp_server_socket(int port) 
{
    int  sofd, err;
    struct sockaddr_in sv_addr;
 
    sofd = (int)socket(AF_INET, SOCK_DGRAM, 0);
    if (sofd<0) return -1;
 
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // TIME_WAIT しない設定．
/*    struct linger lg;
    lg.l_onoff  = 1;
    lg.l_linger = 0;
    err = setsockopt(sofd, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg));
    if (err<0) {
        socket_close(sofd);
        return -2;
    } 
*/
    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    return sofd;
}


/**
int  udp_server_socket_setopt(int port, int opt, const void* optval, int optlen) 

UDPのサーバソケットを作り,接続待ち状態になる．オプションの指定（setsockopt）が可能．

この関数内で呼び出されるネットワーク関数: socket(), setsockopt(), bind()

@param  port    ポート番号
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ

@retval  0以上  作成されたソケット記述子．
@retval -1      ソケットが作成できなかった．
@retval -2      ソケットオプションの設定に失敗．
@retval -3      バインドに失敗．
*/
int  udp_server_socket_setopt(int port, int opt, const void* optval, int optlen) 
{
    int  sofd, err;
    struct sockaddr_in sv_addr;
 
    sofd = (int)socket(AF_INET, SOCK_DGRAM, 0);
    if (sofd<0) return -1;
 
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            socket_close(sofd);
            return -2;
        } 
    }

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    return sofd;
}


/**
int  udp_client_socket(char* hostname, int port, struct sockaddr_in* sv_addr)

UDPのクライアントソケットを作る．
正常終了した場合, *sv_addrにサーバの情報が格納される．

この関数内で呼び出されるネットワーク関数: socket(), gethostbyname()

@param  hostname    サーバ名
@param  port        サーバポート番号
@param  sv_addr     sockaddr_in 構造体へのポインタ

@retval  0以上   作成されたソケット記述子．
@retval -1       ソケットが作成できなかった．
@retval -4       サーバ情報の取得に失敗．
*/
int  udp_client_socket(char* hostname, int port, struct sockaddr_in* sv_addr)
{
    int  sofd;
    struct hostent* shost;
 
    sofd = (int)socket(AF_INET, SOCK_DGRAM, 0);
    if (sofd<0) return -1;

    shost = gethostbyname(hostname); 
    if (shost==NULL) {
        socket_close(sofd);
        return -4
    }

    memset(sv_addr, 0, sizeof(*sv_addr));
    sv_addr->sin_family = AF_INET;
    sv_addr->sin_port   = htons(port);
    memcpy(&(sv_addr->sin_addr), shost->h_addr, shost->h_length);
   
    return sofd;
}


/**
int  udp_bind(int sofd, int port)  

UDPソケットにポートをバインドする．

@param  sofd  バインドするソケットの記述子．
@param  port  ポート番号

@retval sofd  成功
@retval -3    失敗
*/
int  udp_bind(int sofd, int port) 
{
    int  err;
    struct sockaddr_in sv_addr;
 
    if (sofd<=0) return sofd;

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    return sofd;
}


/**
int  udp_bind_setopt(int sofd, int port, int opt, const void* optval, int optlen) 

UDPソケットにポートをバインドする．オプションの指定（setsockopt）が可能．

@param  sofd    バインドするソケットの記述子．
@param  port    ポート番号
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ

@retval  0以上  作成されたソケット記述子．
@retval -2      ソケットオプションの設定に失敗
@retval -3      バインドに失敗．
 */
int  udp_bind_setopt(int sofd, int port, int opt, const void* optval, int optlen) 
{
    int  err;
    struct sockaddr_in sv_addr;
 
    if (sofd<=0) return sofd;

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            socket_close(sofd);
            return -2;
        } 
    }

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    return sofd;
}


/**
int  tcp_server_socket(int port)

TCPのサーバソケットを作り，リッスンにより接続要求を受けられる状態にする．@n
この関数に続いて accept()関数を呼ぶ事により,実際の接続待ち状態となる．

この関数内で呼び出されるネットワーク関数は socket(), bind(), listen()

@param  port ポート番号．

@retval  0以上 作成されたソケット記述子．
@retval -1     ソケットの作成に失敗．
@retval -3     バインドに失敗．
@retval -6     リッスンの設定に失敗．
*/
int  tcp_server_socket(int port) 
{
    int  sofd, err;
    struct sockaddr_in sv_addr;
 
    sofd = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sofd<0) return -1;
 
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    err = listen(sofd, 10);
    if (err==-1) {
        socket_close(sofd);
        return -6;
    }

    return sofd;
}


/**
int  tcp_server_socket_setopt(int port, int opt, const void* optval, int optlen) 

TCPのサーバソケットを作り，リッスンにより接続要求を受けられる状態にする．@n
この関数に続いて accept()関数を呼ぶ事により,実際の接続待ち状態となる．
オプションの指定（setsockopt）が可能．

この関数内で呼び出されるネットワーク関数は socket(), setsockopt(), bind(), listen()

@param  port    ポート番号
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ

@retval  0以上  作成されたソケット記述子．
@retval -1      ソケットの作成に失敗．
@retval -2      ソケットオプションの設定に失敗．
@retval -3      バインドに失敗．
@retval -6      リッスンの設定に失敗．
*/
int  tcp_server_socket_setopt(int port, int opt, const void* optval, int optlen) 
{
    int  sofd, err;
    struct sockaddr_in sv_addr;
 
    sofd = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sofd<0) return -1;
 
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            socket_close(sofd);
            return -2;
        }
    }

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    err = listen(sofd, 10);
    if (err==-1) {
        socket_close(sofd);
        return -6;
    }

    return sofd;
}


/**
int  tcp_server_bind(int port)

TCPのサーバのソケット記述子を作り，バインドする．
tcp_server_socket() との違いは listen()関数を呼び出さない点にある．

この関数内で呼び出されるネットワーク関数: socket(), bind()

@param  port   ポート番号．

@retval  0以上 作成されたソケット記述子．
@retval -1     ソケットの作成に失敗．
@retval -3     バインドに失敗．
*/
int  tcp_server_bind(int port) 
{
    int  sofd, err;
    struct sockaddr_in sv_addr;
 
    sofd = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sofd<0) return -1;
 
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    return sofd;
}


/**
int  tcp_server_bind_setopt(int port, int opt, const void* optval, int optlen) 

TCPのサーバのソケット記述子を作り，バインドする．オプションの指定（setsockopt）が可能．
tcp_server_socket() との違いは listen()関数を呼び出さない点にある．

この関数内で呼び出されるネットワーク関数: socket(), setsockopt(), bind()

@param  port    ポート番号
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ

@retval  0以上  作成されたソケット記述子．
@retval -1      ソケットの作成に失敗．
@retval -2      ソケットオプションの設定に失敗．
@retval -3      バインドに失敗．
 */
int  tcp_server_bind_setopt(int port, int opt, const void* optval, int optlen) 
{
    int  sofd, err;
    struct sockaddr_in sv_addr;
 
    sofd = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sofd<0) return -1;
 
    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            socket_close(sofd);
            return -2;
        }
    }

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    return sofd;
}


/**
int  tcp_client_socket(char* hostname, int port)

TCPのクライアントソケットを作り，サーバに接続する．

この関数内で呼び出されるネットワーク関数: socket(), gethostbyname(), connect()

@param  hostname サーバ名
@param  port     サーバポート番号

@retval  0以上   作成されたソケット記述子．
@retval -1       ソケットの作成に失敗．
@retval -4       サーバ情報の取得に失敗．
@retval -5       サーバへの接続に失敗．
  */
int  tcp_client_socket(char* hostname, int port) 
{
    int sofd, err;
    struct hostent     *shost;
    struct sockaddr_in sv_addr;

    sofd = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sofd<0) return -1;
 
    shost = gethostbyname(hostname); 
    if (shost==NULL) {
        socket_close(sofd);
        return -4;
    }

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    memcpy(&sv_addr.sin_addr, shost->h_addr, shost->h_length);

    err = connect(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -5;
    }

    return sofd;
}


/**
int  tcp_client_bind_socket(char* hostname, int sport, int cport)  

TCPのクライアントソケットを作り，サーバに接続する．

この関数内で呼び出されるネットワーク関数: socket(), bind(), gethostbyname(), connect()

@param  hostname サーバ名
@param  sport    サーバポート番号
@param  cport    自分のポート番号

@retval  0以上   作成されたソケット記述子．
@retval -1       ソケットの作成に失敗．
@retval -3       バインドに失敗．
@retval -4       サーバ情報の取得に失敗．
@retval -5       サーバへの接続に失敗．
 */
int  tcp_client_bind_socket(char* hostname, int sport, int cport)  
{
    int sofd, err;
    struct hostent  *shost;
    struct sockaddr_in sv_addr;

    sofd = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sofd<0) return -1;
 
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(cport);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    shost = gethostbyname(hostname); 
    if (shost==NULL) {
        socket_close(sofd);
        return -4;
    }

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(sport);
    memcpy(&sv_addr.sin_addr, shost->h_addr, shost->h_length);

    err = connect(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -5;
    }

    return sofd;
}


/**
int  tcp_bind(int sofd, int port)  

TCPソケットにポートをバインドする．

@param  sofd  バインドするソケットの記述子．
@param  port  ポート番号

@retval sofd  成功
@retval -3    失敗
*/
int  tcp_bind(int sofd, int port)  
{
    int  err;
    struct sockaddr_in sv_addr;
 
    if (sofd<=0) return sofd;

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    return sofd;
}


/**
int  tcp_bind_setopt(int sofd, int port, int opt, const void* optval, int optlen)  

TCPソケットにポートをバインドする．オプションの指定（setsockopt）が可能．

@param  sofd    バインドするソケットの記述子．
@param  port    ポート番号
@param  opt     オプションの種類
@param  optval  オプションデータ
@param  optlen  optval の長さ

@retval  0以上  作成されたソケット記述子．
@retval -2      ソケットオプションの設定に失敗．
@retval -3      バインドに失敗．
*/
int  tcp_bind_setopt(int sofd, int port, int opt, const void* optval, int optlen)  
{
    int  err;
    struct sockaddr_in sv_addr;
 
    if (sofd<=0) return sofd;

    if (opt>0) {
        #ifdef WIN32
            err = setsockopt(sofd, SOL_SOCKET, opt, (const char*)optval, optlen);
        #else
            err = setsockopt(sofd, SOL_SOCKET, opt, optval, optlen);
        #endif
        if (err<0) {
            socket_close(sofd);
            return -2;
        } 
    }

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    err = bind(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(sofd);
        return -3;
    }

    return sofd;
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


/*
int  tcp_connect(int sofd, char* hostname, int port)

TCPのクライアントソケットを通して，サーバに接続する．

この関数内で呼び出される関数: gethostbyname(), connect()

@param  sofd      ソケット記述子
@param  hostname  サーバ名
@param  port      サーバポート番号

@retval  0  成功
@retval -4  サーバ情報の取得に失敗．
@retval -5  サーバへの接続に失敗．
 */
int  tcp_connect(int sofd, char* hostname, int port) 
{
    int  err;
    struct hostent  *shost;
    struct sockaddr_in sv_addr;

    shost = gethostbyname(hostname); 
    if (shost==NULL) return -4;

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    memcpy(&sv_addr.sin_addr, shost->h_addr, shost->h_length);

    err = connect(sofd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) return -5;
    return 0;
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



/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//

/**
int  udp_recv(int sock, char* rmsg, int size, struct sockaddr_in* sv_addr)

recvform() をラッピングした関数．UDPデータを受信する．

@param  sock    ソケット記述子
@param  rmsg    受信用データバッファ
@param  size    データバッファのサイズ
@param  sv_addr サーバの情報が格納された sockaddr_in 構造体へのポインタ．

@retval  1以上  受信したバイト数．
@retval  0      正常切断．
@retval -1      失敗
*/
int  udp_recv(int sock, char* rmsg, int size, struct sockaddr_in* sv_addr)
{
    int cc;
     socklen_t cadlen;

    cadlen = sizeof(*sv_addr);
    memset(rmsg, 0, size);
    cc = recvfrom(sock, rmsg, size, 0, (struct sockaddr*)sv_addr, &cadlen);

    return cc;
}


/**
int  udp_send(int sock, char* smsg, int size, struct sockaddr_in* sv_addr)

sendto() をラッピングした関数．UDP経由でデータを送る．

データ(smsg)のサイズ sizeに0以下を指定した場合は，smsgは文字列で
あると見なしてサイズを自動的に計算する．

@param  sock    ソケット記述子
@param  smsg    送信するデータ
@param  size    送信するデータ（smsg）のサイズ．サイズが 0以下の場合は smsgは文字列であるとみなす．
@param  sv_addr サーバの情報を格納する sockaddar_in 構造体へのポインタ．

@retval  0以上  送信したバイト数．
@retval -1      失敗
*/
int  udp_send(int sock, char* smsg, int size, struct sockaddr_in* sv_addr)
{
    int cc;

    if (size<=0) size = (int)strlen(smsg);
//    hton_ar(smsg, size);
    cc = sendto(sock, smsg, size, 0, (struct sockaddr*)sv_addr, sizeof(*sv_addr));

    return cc;
}


/**
int  tcp_recv(int sock, char* rmsg, int size)

recv()をラッピングした関数．TCP経由でデータを受信する．

@param  sock   ソケット記述子
@param  rmsg   受信用データバッファ
@param  size   データバッファのサイズ

@retval  1以上 受信したバイト数．
@retval  0     正常切断．
@retval -1     失敗
*/
int  tcp_recv(int sock, char* rmsg, int size)
{
    int cc;

    memset(rmsg, 0, size);
    cc = recv(sock, rmsg, size, 0);

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
@retval -1     失敗．
*/
int  tcp_send(int sock, char* smsg, int size)
{
    int cc;

    if (size<=0) size = (int)strlen(smsg);
    cc = send(sock, smsg, size, 0);

    return cc;
}


/**
int  udp_recv_wait(int sock, char* rmsg, int size, struct sockaddr_in* sv_addr, int tm)

UDP経由でデータを受信する．

タイムアウトの設定が可能．タイムアウトに 0を指定した場合, recv_wait()
関数を呼び出した時点で読み込み可能データがなければすぐにタイムアウトとなる (RECV_TIMEOUTED が返る)．

@param  sock     ソケット記述子
@param  rmsg     受信用データバッファ
@param  size     データバッファのサイズ
@param  sv_addr  サーバの情報が格納された sockaddar_in 構造体へのポインタ．
@param  tm         タイムアウト時間．秒単位．

@retval  1以上   受信したバイト数．
@retval  0       正常切断．
@retval -1       失敗．
@retval RECV_TIMEOUTED    タイムアウト．
*/
int  udp_recv_wait(int sock, char* rmsg, int size, struct sockaddr_in* sv_addr, int tm)
{
    int  cc;

    if (recv_wait(sock, tm)) {
        cc = udp_recv(sock, rmsg, size, sv_addr); 
    }
    else {
        return RECV_TIMEOUTED;
    }
    return cc;
}


/**
int  tcp_recv_wait(int sock, char* mesg, int sz, int tm)

TCP経由でデータを受信する．

タイムアウトの設定が可能．タイムアウトに 0を指定した場合, recv_wait()
関数を呼び出した時点で読み込み可能データがなければすぐにタイムアウトとなる (RECV_TIMEOUTED が返る)．

@param  sock    ソケット記述子
@param     mesg    受信用データバッファ
@param     sz        データバッファのサイズ
@param    tm        タイムアウト時間．秒単位．

@retval  1以上  受信したバイト数．
@retval  0        おそらくは相手側がセッションをクローズした．
@retval -1        受信エラー．
@retval RECV_TIMEOUTED  タイムアウト．
*/
int  tcp_recv_wait(int sock, char* mesg, int sz, int tm)
{
    int  cc;

    if (recv_wait(sock, tm)) {
        cc = recv(sock, mesg, sz, 0); 
    }
    else {
        return RECV_TIMEOUTED;
    }
    return cc;
}


/**
int  tcp_send_mesgln(int sock, char* mesg)

TCPメッセージ(文字列)に改行(@\r@\n)を付け加えて送信する．

@param  sock    ソケット記述子
@param  mesg    送信用メッセージ

@retval  0以上  送信したバイト数．
@retval -1      失敗．
*/
int  tcp_send_mesgln(int sock, char* mesg)
{
    int   cc, sz;
    char* buf;

    sz = (int)strlen(mesg) + 3;    /* CR+LF+0x00 */
    buf = (char*)malloc(sz);
    if (buf==NULL) return -1;

    strncpy(buf, mesg, sz);
    strncat(buf, "\r\n", 2);
//    hton_ar(buf, strlen(buf));
    cc = send(sock, buf, (int)strlen(buf), 0);

    free(buf);
    return cc;
}


/**
int  tcp_recv_mstream(int sock, char* mesg, int sz, mstream* sb, int tm)

TCP経由でメッセージ(文字列)を受信する．受信メッセージはメッセージストリームバッファに一旦バッファリングされ，
この関数により一行ずつ読み出される．mesgには最大 sz-1文字が格納される．
もし，バッファ中の一行のデータが sz-1より大きい場合は，はみ出した部分は捨てられる．

mesgに格納される時，行中の改行コードは削除され，行末には必ず '@\0' が入る．
タイムアウトの設定が可能でタイムアウトに 0を指定した場合, 呼び出した時点で
読み込み可能データがなければすぐにタイムアウトとなる (RECV_TIMEOUTED が返る)．

メッセージストリームのバッファ部が確保されていない場合は，最初に呼び出された時点で確保される．
一旦この関数を使用して，受信データをバッファリングしたら，ソケットをクローズするまで，
読み取りには必ず同じストリームを使用してこの関数を呼び出さばければならない．
そうで無い場合は受信データの整合性は保証されない．

@param  sock    ソケット記述子
@param  mesg    受信用データバッファ．予め十分なメモリ領域を確保しておく．
@param  sz      データバッファのサイズ
@param  sb      リングバッファ型のストリームバッファ． バッファ部が確保されていなければ，自動的に確保される．
@param  tm      タイムアウト時間．秒単位．

@retval  1以上  mesgに格納したメッセージのバイト数．
@retval  0      おそらくは相手側がセッションをクローズした．
@retval -1      受信エラー．
@retval -2      引数に NULLのデータがある．
@retval -3      メッセージバッファ部が無いので確保しようとしたが，確保に失敗した．
@retval -4      メッセージバッファにデータは存在するはずだが，原因不明の理由により獲得に失敗した．
@retval -5      メッセージバッファ中のデータの長さが，mesgの長さより大きい．はみ出した部分は捨てられた．
@retval RECV_TIMEOUTED  タイムアウト．
*/
int  tcp_recv_mstream(int sock, char* mesg, int sz, mstream* sb, int tm)
{
    int  cc;
    unsigned char* pp;

    if (mesg==NULL || sb==NULL) return -2;
    memset(mesg, 0, sz);

    if (sb->buf==NULL) {
        *sb = make_mstream(RECVBUFSZ);
        if (sb->buf==NULL) return -3;
    }

    while (sb->datano==0) {
        cc = tcp_recv_wait(sock, mesg, sz, tm);
        if (cc<=0) return cc;
        put_mstream(sb, (unsigned char*)mesg);
        memset(mesg, 0, sz);
    }

    pp = get_mstream(sb);
    if (pp==NULL) return -4;
    if (strlen((const char*)pp)>=(unsigned int)sz) {
        memcpy(mesg, pp, sz-1);
        free(pp);
        return -5;
    }
    memcpy(mesg, pp, strlen((const char*)pp));

    free(pp);
    return (int)strlen(mesg);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// アドレス処理
//

/**
char*  get_hostname_bynum(unsigned char* num) 

バイナリのIPアドレス（長さ4byte）からホスト名を逆引きする． 

@param  num  バイナリのIPアドレス（長さ4byte）． 
@return ホスト名（文字列）．要 free
*/
char*  get_hostname_bynum(unsigned char* num) 
{
    int   len;
    char* hname;
    struct hostent* hent;

    #ifdef WIN32
        hent = gethostbyaddr((const char*)num, 4, AF_INET);
    #else
        hent = gethostbyaddr(num, 4, AF_INET);
    #endif
    
    if (hent==NULL) return NULL;

    len = (int)strlen(hent->h_name);
    hname = (char*)malloc(len+1);
    if (hname==NULL) {
        free(hent);
        return NULL;
    }
    memcpy(hname, hent->h_name, len+1);

    return hname;    
}


/**
char*  get_hostname(char* ipaddr) 

IPアドレス（文字列）からホスト名を逆引きする． 

FQDNを指定した場合は，正引きして逆引きするので，FQDNのチェックに使用できる．

@param  ipaddr IPアドレス（文字列）または FQDN 
@return ホスト名（文字列）．要 free
*/
char*  get_hostname(char* ipaddr) 
{
    char*  name;
    unsigned char* num;

    num  = to_address_num4(ipaddr, 1);
    if (num==NULL) return NULL;

    name = get_hostname_bynum(num); 
    free(num);

    return name;    
}


#ifdef WIN32

/**
unsigned char*  get_myipaddr_num() 

ローカルなインターフェイスの バイナリの IPアドレスとネットマスクを返す．

@return  [IPアドレス],[ネットマスク] の長さ8byteのバイナリ．要 free

@attention Windows の場合は ネットマスクは取得できない．
@see get_localip(), get_localip_bydest()
*/
unsigned char*  get_myipaddr_num() 
{
    unsigned char* ipnum  = NULL;
    char* ipaddr = get_localip();

    if (ipaddr!=NULL) {
        ipnum = to_address_num8(ipaddr, 0);
        free(ipaddr);
    }

    return ipnum;
}

// #define get_myipaddr()  get_localip_bydest("202.26.158.1")

#else

/**
unsigned char*  get_myipaddr_num() 

ネットワークインターフェイスを検索し，一番最初に見つけた UP状態で，
ループバックでないインターフェイスのIPアドレスとネットマスクを返す．@n
ただし，検索するインターフェイスの数は MAXIFNO 以内であり，それ以上は無視する．

自分自身のIPアドレスを知る方法としては， gethostname() と gethostbyname() を使う方法もある．

@code
    struct hostent  *shost;
    .....................
    gethostname(hostname, sizeof(hostname));
    shost = gethostbyname(hostname);
@endcode

また，get_localip() の方法も使える．

@return  [IPアドレス],[ネットマスク] の長さ8byteのバイナリ．要 free

@see get_localip(), get_localip_bydest()
*/
unsigned char*  get_myipaddr_num() 
{
    int    i, sofd, len;

    unsigned char*  addr = NULL;
    struct ifconf  ifc;
    struct ifreq*  ifq;
    struct ifreq*  ifp;


    len = sizeof(struct ifreq)*MAXIFNO;
    ifp = ifq = (struct ifreq*)malloc(len);
    if (ifq==NULL) return NULL;
    ifc.ifc_len = len;
    ifc.ifc_req = ifq;

    sofd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sofd<0) {
          perror("get_myipaddr_num: socket");
        return NULL;
    }


    // インターフェイス名の取得
    if (ioctl(sofd, SIOCGIFCONF, &ifc)<0) {
        socket_close(sofd);
        free(ifq);
        return NULL;
    }


    ifp = ifq;
    for (i=0; i<MAXIFNO; i++) {

        // インターフェイスのフラグ取得
        if (ioctl(sofd, SIOCGIFFLAGS, ifp)<0) {
            socket_close(sofd);
            free(ifq);
            return NULL;
        }

        if ((ifp->ifr_flags&IFF_UP) && !(ifp->ifr_flags&IFF_LOOPBACK)) {
            int j;
            unsigned char* p = (unsigned char*)ifp->ifr_addr.sa_data;

            addr = (unsigned char*)malloc(8);
            if (addr==NULL) {
                socket_close(sofd);
                free(ifq);
                return NULL;
            }

            // アドレス取得
            if (ioctl(sofd, SIOCGIFADDR, ifp)<0) {
                socket_close(sofd);
                free(ifq);
                free(addr);
                return NULL;
            }
            for (j=0; j<4; j++) addr[j] = p[j+2];

            // ネットマスク取得
            if (ioctl(sofd, SIOCGIFNETMASK, ifp)<0) {
                socket_close(sofd);
                free(ifq);
                free(addr);
                return NULL;
            }
            for (j=0; j<4; j++) addr[j+4] = p[j+2];

            return addr;
        }
        ifp++;
    }
    
    socket_close(sofd);
    free(ifq);
    return NULL;
}


/**
char*  get_myipaddr() 

ネットワークインターフェイスを検索し，一番最初に見つけた UP状態で，
ループバックでないインターフェイスのIPアドレスと ネットマスクを返す．@n
ただし，検索するインターフェイスの数は MAXIFNO 以内であり，それより多いものは無視する．

ネットマスクが必要ないなら get_localip() を使用しても良い．

@return [IPアドレス]/[ネットマスク] の文字列．要 free

@attention Windows の場合は ネットマスクは取得できない．
*/
char*  get_myipaddr() 
{
    char* addr;
    unsigned char* n;

    n = get_myipaddr_num();
    if (n==NULL) return NULL;

    addr = (char*)malloc(32);
    if (addr==NULL) {
        free(n);
        return NULL;
    }
    memset(addr, 0, 32);

    snprintf(addr, 31, "%d.%d.%d.%d/%d.%d.%d.%d", n[0], n[1], n[2], n[3], n[4], n[5], n[6], n[7]);

    free(n);
    return addr;
}

#endif


/**
char*  get_localip_bydest(char* dest)

接続先を指定して，そこへ接続する インターフェイスのIPアドレスを得る．自分自身の IPアドレスを得るもう一つの方法．

@param  dest 接続先の IPアドレス または FQDN

@return IPアドレス(文字列)へのポインタ．要 free
*/
char*  get_localip_bydest(char* dest)
{
    int   err, sofd;
    socklen_t len;
    char* addr;
    char* ip;
    struct sockaddr_in localif;  
    struct sockaddr_in remote;             
    
    if (dest==NULL) return NULL;     
    ip = get_ipaddr_byname(dest);
    if (ip ==NULL)  return NULL;

    memset(&localif, 0, sizeof(struct sockaddr_in));  
    memset(&remote,  0, sizeof(struct sockaddr_in));  
    remote.sin_family      = AF_INET;  
    remote.sin_addr.s_addr = inet_addr(ip);  
    remote.sin_port        = htons(9999);             
    free(ip);

    sofd = (int)socket(AF_INET, SOCK_DGRAM, 0);  
    if (sofd<0) return NULL;

      err = connect(sofd, (struct sockaddr*)&remote, sizeof(remote));
      if (err<0) {
          socket_close(sofd);
         return NULL;
    }

      len = sizeof(localif);
      err = getsockname(sofd, (struct sockaddr *)&localif, &len);
    if (err<0) {
          socket_close(sofd);
          return NULL;
    }
      socket_close(sofd);

    addr = get_ipaddr(localif.sin_addr);
      return addr;
}
           


/**
char*  get_ipaddr(struct in_addr sin_addr)

in_addr構造体から IPアドレスを獲得する．

@param  sin_addr  sockaddr_in 構造体の sin_addr メンバ．
@return IPアドレス(文字列)へのポインタ．要 free

@code
   struct sockaddr_in addr
   .........;
   ipaddr = get_ipaddr(addr.sin_addr); 
@endcode
*/
char*  get_ipaddr(struct in_addr sin_addr)
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
unsigned char*  get_ipaddr_num(struct in_addr sin_addr)

in_addr 構造体から IPアドレスを獲得する．

@param  sin_addr  sockaddr_in 構造体の sin_addr メンバ．
@return IPアドレス(バイナリ)へのポインタ（長さ4byte）．要 free

@code
    struct sockaddr_in addr
    .........;
    ipaddr = get_ipaddr_num(addr.sin_addr); 
@endcode
*/
unsigned char*  get_ipaddr_num(struct in_addr sin_addr)
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
char*  get_ipaddr_byname(char* hostname)

ホスト名から IPアドレスを獲得する．

@param  hostname ホスト名
@return IPアドレス(文字列)へのポインタ．要 free
*/
char*  get_ipaddr_byname(char* hostname)
{
    struct hostent *host;
    char* ip;
    unsigned char* pp;

    host = gethostbyname(hostname); 
    if (host==NULL) return NULL;

    ip = (char*)malloc(16);
    if (ip==NULL) return NULL;
    memset(ip, 0, 16);

    pp = (unsigned char*)(host->h_addr);
    snprintf(ip, 15, "%d.%d.%d.%d", pp[0], pp[1], pp[2], pp[3]);

    return ip;
}


/**
unsigned char*  get_ipaddr_byname_num(char* hostname)

ホスト名から IPアドレスを獲得する．

@param  hostname ホスト名
@return IPアドレス(バイナリ)へのポインタ（長さ4byte）．要 free
*/
unsigned char*  get_ipaddr_byname_num(char* hostname)
{
    struct hostent *host;
    unsigned char* ip;
    unsigned char* pp;

    host = gethostbyname(hostname); 
    if (host==NULL) return NULL;
    ip = (unsigned char*)malloc(4);
    if (ip==NULL) return NULL;

    pp = (unsigned char*)(host->h_addr);
    memcpy(ip, pp, 4);

    return ip;
}


/**
char*  get_mynetaddr()

自分のネットワークアドレスを返す

@return ネットワークアドレス(文字列)へのポインタ．要 free
*/
char*  get_mynetaddr()
{
    char* net;
    unsigned char* pp;

    pp = get_mynetaddr_num();
    if (pp==NULL) return NULL;

    net = (char*)malloc(16);
    if (net==NULL) {
        free(pp);
        return NULL;
    }
    memset(net, 0, 16);

    snprintf(net, 15, "%d.%d.%d.%d", pp[0], pp[1], pp[2], pp[3]);

    free(pp);
    return net;
}


/**
unsigned char*  get_mynetaddr_num()

自分のネットワークアドレスを返す

@return ネットワークアドレス(バイナリ)へのポインタ（長さ4byte）．要 free
*/
unsigned char*  get_mynetaddr_num()
{
    int  i;
    unsigned char* net;
    unsigned char* ip;
    unsigned char* mk;
        
    ip = get_myipaddr_num();
    if (ip==NULL) return NULL;
    mk = &(ip[4]);

    net = (unsigned char*)malloc(4);
    if (net==NULL) {
        free(ip);
        return NULL;
    }

    for (i=0; i<4; i++) net[i] = ip[i] & mk[i];

    free(ip);
    return net;
}


/**

*/
struct sockaddr_in  get_sockaddr(char* hostname, unsigned short cport)
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


struct sockaddr_in  get_sockaddr_bynum(char* ipnum, unsigned short cport)
{
    struct sockaddr_in  ss_addr;

    memset(&ss_addr, 0, sizeof(ss_addr));
    ss_addr.sin_family = AF_INET;
    ss_addr.sin_port   = htons(cport);
    memcpy(&(ss_addr.sin_addr), ipnum, 4);

    return ss_addr;
}


struct sockaddr_in  get_local_sockaddr(unsigned short cport)
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
int  is_same_sockaddr(struct sockaddr_in addr1, struct sockaddr_in addr2) 

addr1 と addr2が格納する IPアドレスとポート番号が同じかどうか検査する

@param  addr1  比較する sockaddr_in 構造体
@param  addr2  比較する sockaddr_in 構造体

@retval TRUE   同じホスト
@retval FALSE  違うホスト
*/
int  is_same_sockaddr(struct sockaddr_in addr1, struct sockaddr_in addr2) 
{
    int i, len;
    unsigned char *p1, *p2;

    p1  = (unsigned char*)&(addr1.sin_addr);
    p2  = (unsigned char*)&(addr2.sin_addr);
    len = sizeof(addr1.sin_addr);

    for (i=0; i<len; i++) {
        if (p1[i]!=p2[i]) return FALSE;
    }

    p1  = (unsigned char*)&(addr1.sin_port);
    p2  = (unsigned char*)&(addr2.sin_port);
    len = sizeof(addr1.sin_port);

    for (i=0; i<len; i++) {
        if (p1[i]!=p2[i]) return FALSE;
    }

    return TRUE;
}


/**
int  is_same_network_num(unsigned char* addr1, unsigned char* addr2, unsigned char* mask)

数字型アドレス addr1 と addr2が同じネットワークに属しているかどうかチェックする．

@param  addr1  比較するアドレス（数字型）4Byte
@param  addr2  比較するアドレス（数字型）4Byte
@param  mask   ネットマスク（数字型）4Byte

@retval TRUE   同じネットワークである．
@retval FALSE  同じネットワークでない
*/
int  is_same_network_num(unsigned char* addr1, unsigned char* addr2, unsigned char* mask)
{
    int i;

    if (addr1==NULL || addr2==NULL) return FALSE;

    if (mask==NULL) {
        for (i=0; i<4; i++) {
            if (addr1[i] != addr2[i]) return FALSE;
        }
    }
    else {
        for (i=0; i<4; i++) {
            if ((addr1[i]&mask[i]) != (addr2[i]&mask[i])) return FALSE;
        }
    }
    return TRUE;
}


/**
int  is_same_network(char* addr1, char* addr2, char* mask)

文字型アドレス addr1 と addr2が同じネットワークに属しているかどうかチェックする．@n
addr1, addr2 は FQDNでも可．

@param  addr1  比較するアドレス（文字型）
@param  addr2  比較するアドレス（文字型）
@param  mask   ネットマスク（文字型）

@retval TRUE   同じネットワークである．
@retval FALSE  同じネットワークでない．
*/
int  is_same_network(char* addr1, char* addr2, char* mask)
{
    int  ret;
    unsigned char* a1;
    unsigned char* a2;
    unsigned char* mk;

    if (addr1==NULL || addr2==NULL || mask==NULL) return FALSE;

    a1 = to_address_num4(addr1, 1);
    a2 = to_address_num4(addr2, 1);
    mk = to_address_num4(mask,  0);
    if (a1==NULL || a2==NULL || mk==NULL) {
        freeNull(a1);
        freeNull(a2);
        freeNull(mk);
        return FALSE;
    }

    ret = is_same_network_num(a1, a2, mk);
    freeNull(a1);
    freeNull(a2);
    freeNull(mk);
    
    return ret;
}


/**
unsigned char*  to_address_num8(char* addr, int mode)

文字型のアドレス [IPアドレス]/[ネットマスク] (例：202.26.159.140/255.255.255.0）を
数字型のアドレス（unsigned char* num[8]）8Byte へ変換する．

省略された場所は 0とみなされる（例：202.26/255 → 202.26.0.0/255.0.0.0） @n
ネットマスク部全体が省略された場合は IPアドレス部に依存する．(202.26. → 202.26.0.0/255.255.0.0) @n
CIDER形式にも対応．ただし，ネットマスク部の妥当性はチェックしない．

@param  addr 変換する文字型のIPアドレス
@param  mode @b 0:     IPアドレス以外のもの（英字）は NULL を返す．
@param  mode @b 0以外: FQDNはIPアドレスに変換して返す（時間がかかる）

@return [IPアドレス],[ネットマスク] の長さ8byteのバイナリ．要 free
*/
unsigned char*  to_address_num8(char* addr, int mode)
{
    unsigned char* num;
    char* ps;
    char* pc;
    char* uc = NULL;
    int   i;

    if (addr==NULL) return NULL;

    num = (unsigned char*)malloc(8);
    if (num==NULL) return NULL;
    memset(num, 0, 8);


    // IPアドレス部の変換
     ps = awk(addr, '/', 1);
    if (ps==NULL) {
        free(num);
        return NULL;
    }

    i = (int)strlen(ps) - 1;
    while (i>0 && ps[i]=='.') i--;
    if (i>0) {
        if (isalpha(ps[i])) {
            if (mode==0) { // mode==0なら FQDN->IPアドレス は行なわない．
                free(num);
                free(ps);
                return NULL;
            }
            uc = (char*)get_ipaddr_byname_num(ps);
            if (uc==NULL) {
                free(num);
                free(ps);
                return NULL;
            }
        }
    }

    if (uc==NULL) {                         // IPアドレス表記の処理    
        for (i=0; i<4; i++) {
            pc = awk(ps, '.', i+1);
            if (pc==NULL) break;
            num[i] = (unsigned char)atoi(pc);
            free(pc);
        }
    }
    else {                                     // ホスト名表記の処理
        for (i=0; i<4; i++) num[i] = uc[i];
        free(uc);
    }
    free(ps);

    ps = awk(addr, '/', 2);

    // サブネットマスク部が省略された場合の処理
    if (ps==NULL) {
        int f = OFF;
        for (i=3; i>=0; i--) {
            if (num[i]!=0 || f==ON) {
                num[i+4] = 0xff;
                f = ON;
            }
            else {
                num[i+4] = 0;
            }
        }
        return num;
    }

    // サブネットマスク部の処理
    for (i=0; i<4; i++) {
        pc = awk(ps, '.', i+1);
        if (pc==NULL) break;
        num[i+4] = (unsigned char)atoi(pc);
        free(pc);
    }
    free(ps);

    // CIDER形式対応
    if (num[4]<=32 && num[5]==0 && num[6]==0 && num[7]==0) {
        int nn, cl = (int)num[4];
        for (i=0; i<4; i++) {
            nn = 8 - Max(0, Min(8, cl-8*i));
            num[i+4] = 0xff<<nn;
        }
    }

    return  num;
}


/**
char*  to_address_char8(unsigned char* n)

数字型の IPアドレス n 8Byteを文字型の　[IPアドレス]/[ネットマスク]　に変換する．

n は必ずネットマスクの情報を含み，長さが 8Byteでないといけない．8Byteより少ない場合は
恐らくセグメンテーションエラーを起こす．@n
to_address_num()と対で使うのが安全．

@param  n  長さが 8byteの バイナリの [IPアドレス],[ネットマスク]
@return [IPアドレス]/[ネットマスク] 形式の文字列．要 free

@see to_address_num()
*/
char*  to_address_char8(unsigned char* n)
{
    char* addr;
    if (n==NULL) return NULL;

    addr = (char*)malloc(32);
    if (addr==NULL) return NULL;
    memset(addr, 0, 32);

    snprintf(addr, 31, "%d.%d.%d.%d/%d.%d.%d.%d", n[0], n[1], n[2], n[3], n[4], n[5], n[6], n[7]);

    return addr;
}


/**
unsigned char*  to_address_num4(char* addr, int mode)

文字型のアドレス [IPアドレス] (例：202.26.159.140）を
数字型のアドレス（unsigned char* num[4]）4Byte へ変換する．

省略された場所は 0とみなされる（例：202.26 → 202.26.0.0）

@param  addr  変換する文字型のIPアドレス
@param  mode  @b 0:     IPアドレス以外のもの（英字）は NULL を返す．
@param  mode  @b 0以外: FQDNはIPアドレスに変換して返す（時間がかかる）

@return [IPアドレス] の長さ4byteのバイナリ．要 free
*/
unsigned char*  to_address_num4(char* addr, int mode)
{
    unsigned char* num;
    char* pc;
    char* uc = NULL;
    int   i;

    if (addr==NULL) return NULL;

    num = (unsigned char*)malloc(4);
    if (num==NULL) return NULL;
    memset(num, 0, 4);

    // IPアドレス部の変換
    i = (int)strlen(addr) - 1;
    while (i>0 && addr[i]=='.') i--;
    if (i>0) {
        if (isalpha(addr[i])) {
            if (mode==0) { // mode==0なら FQDN->IPアドレス は行なわない．
                free(num);
                return NULL;
            }
            uc = (char*)get_ipaddr_byname_num(addr);
            if (uc==NULL) {
                free(num);
                return NULL;
            }
        }
    }

    if (uc==NULL) {                         // IPアドレス表記の処理    
        for (i=0; i<4; i++) {
            pc = awk(addr, '.', i+1);
            if (pc==NULL) break;
            num[i] = (unsigned char)atoi(pc);
            free(pc);
        }
    }
    else {                                     // ホスト名表記の処理
        for (i=0; i<4; i++) num[i] = uc[i];
        free(uc);
    }

    return  num;
}


/**
char*  to_address_char4(unsigned char* n)

数字型の IPアドレス n 4Byteを文字型の [IPアドレス] に変換する．

n は必ず長さが 4Byteでないといけない．4Byteより少ない場合は恐らくセグメンテーションエラーを起こす．@n
to_address_num4()と対で使うのが安全．

@param  n  長さが 4byteのバイナリの [IPアドレス]
@return [IPアドレス] 形式の文字列．要 free

@see to_address_num4()
*/
char*  to_address_char4(unsigned char* n)
{
    char* addr;
    if (n==NULL) return NULL;

    addr = (char*)malloc(16);
    if (addr==NULL) return NULL;
    memset(addr, 0, 16);

    snprintf(addr, 15, "%d.%d.%d.%d", n[0], n[1], n[2], n[3]);

    return addr;
}


/**
int   recv_wait(int sock, int tm)

簡易 受信wait. ソケットにデータが受信されるまで待つ．スレッド対応

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


int   recv_wait_twin(int sock1, int sock2, int tm)
{
    int       ret = 0;
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

簡易 送信wait. データが送信可能になるまで待つ．スレッド対応

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


/**
void  udp_hole_punching(int sock, struct sockaddr_in addr, int nm)

sock を使って，addrへ nmバイトの NULLデータを送信する．

@param  sock ソケット記述子
@param  addr 相手のホストの情報が格納されて sockaddr_in 構造体
@param  nm   送信するデータ(NULL)の長さ
*/
void  udp_hole_punching(int sock, struct sockaddr_in addr, int nm)
{
    char data[LBUF];

    if (nm<=0) nm = 4;                // for SLVoice
    else if (nm>LBUF) nm = LBUF;

    memset(data, 0, nm);
    udp_send(sock, data, nm, &addr);

    return;
}



////////////////////////////////////////////////////////////////////////////////////////
// get_valid_..._socket
//

int  get_valid_udp_socket(int min, int max, unsigned short* port)
{
    int  i, sock, range;

    range = max - min + 1;
    *port = rand()%range + min;

    i = 1;
    sock = udp_server_socket((int)*port);
    while(sock<=0 && i<range) {
        (*port)++;
        if (*port>max) *port = ((int)*port)%max + min - 1;
        sock = udp_server_socket((int)*port);
        i++;
    }

    if (sock<=0) *port = 0;

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



//////////////////////////////////////////////////////////////////////////////////
//
// for WinSock
//

#ifdef WIN32
    WSADATA WsaData;
#endif


int  init_network(void)
{
    int ret = 0;

#ifdef WIN32
    ret = WSAStartup(MAKEWORD(2,0), &WsaData);
    if (ret!=0) WSACleanup();
#endif

    return  ret;
}


void  cleanup_network(void)
{
#ifdef WIN32
    WSACleanup();
#endif
}

