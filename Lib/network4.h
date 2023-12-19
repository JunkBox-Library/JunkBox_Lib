
#ifndef  __JBXL_NETWORK_H_
#define  __JBXL_NETWORK_H_


/** 
@brief   ネットワーク用ライブラリヘッダ
@file    network.h
@author  Fumi.Iseki (C)

このファイルは現在使用されていません．
バージョンのチェックの為だけに残されています．
新しいファイルは network.h (network6.h) をご覧ください．
*/



#include "tools.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef WIN32
    //#include <winsock2.h>    see tools.h
    #include <fcntl.h>

    #ifndef socklen_t        
        #define socklen_t  int
    #endif
    extern WSADATA WsaData;

    #define _WINSOCK_DEPRECATED_NO_WARNINGS

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


#define  SEND_TIMEOUTED     -97
#define  RECV_TIMEOUTED     -98
#define  TRANS_TIMEOUTED    -99



//#define  LIPADDR  32      // IPアドレスの長さ 
#define  MAXIFNO  10        ///< 取り扱うネットワークインターフェイスの最大数



/////////////////////////////////////////////////////////////////////////////////////////////
//
int   init_network(void);
void  cleanup_network(void);


/////////////////////////////////////////////////////////////////////////////////////////////
// Berkeley Socket

int   udp_server_socket(int);                                       ///< call socket(), bind()
int   udp_server_socket_setopt(int, int, const void*, int);         ///< call socket(), setsockopt(), bind()
int   udp_client_socket(char*, int, struct sockaddr_in*);           ///< call socket()
int   udp_bind(int, int);                                           ///< call bind()
int   udp_bind_setopt(int, int, int, const void*, int);             ///< call setsockopt(), bind()

int   tcp_server_socket(int port);                                  ///< call socket(), bind(), listen()
int   tcp_server_socket_setopt(int port, int, const void*, int);    ///< call socket(), setsockopt(), bind(), listen()
int   tcp_server_bind(int port);                                    ///< call socket(), bind()
int   tcp_server_bind_setopt(int port, int, const void*, int);      ///< call socket(), setsockopt(), bind()

int   tcp_client_socket(char* hname, int port);                     ///< call socket(), connect()
int   tcp_client_bind_socket(char* host, int sport, int cport);     ///< call socket(), bind(), connect()
int   tcp_bind(int, int);                                           ///< call    bind()
int   tcp_bind_setopt(int, int, int, const void*, int);             ///< call    setsockopt(), bind()
int   tcp_connect(int sock, char* host, int port);                  ///< call connect()

int   accept_intr(int, struct sockaddr*, socklen_t*);               ///< call accept()
int   socket_close(int sock);                                       ///< call shutdown(), close()

int   get_valid_udp_socket(int min, int max, unsigned short* port);
int   get_valid_tcp_server_socket(int min, int max, unsigned short* port);
int   get_valid_tcp_client_socket(int min, int max, char* hostname, unsigned short sport, unsigned short* cport);




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Communication

int   udp_recv(int, char*, int, struct sockaddr_in*);
int   udp_send(int, char*, int, struct sockaddr_in*);
int   tcp_recv(int, char*, int);
int   tcp_send(int, char*, int);

int   tcp_recv_wait(int sock, char* data, int sz, int tm);
int   udp_recv_wait(int sock, char* data, int size, struct sockaddr_in* sv_addr, int tm);

int   tcp_send_mesgln(int, char*); 

int   tcp_recv_mstream(int sock, char* mesg, int sz, mstream* sb, int tm);

int   recv_wait(int sock, int tm);
int   recv_wait_twin(int sock1, int sock2, int tm);
int   send_wait(int sock, int tm);

void  udp_hole_punching(int sock, struct sockaddr_in addr, int nm);


/**
int  udp_send_mesg(int sock, char* smsg, struct sockaddr_in* sv_addr)

UDPで文字列データを送る．
@param  sock    ソケット記述子
@param  smsg    送信するデータ
@param  sv_addr サーバの情報を格納する sockaddar_in 構造体へのポインタ．
@retval  0以上  送信したバイト数．
@retval -1      失敗．
*/
#define  udp_send_mesg(sock, smsg, sv_addr)        udp_send((sock), (smsg), 0, (sv_addr))


/**
int  tcp_send_mesg(int sock, char* smsg)

TCP経由でメッセージ(文字列)を送信する．
@param  sock    ソケット記述子
@param  smsg    送信用メッセージ
@retval  0以上  送信したバイト数．
@retval -1      失敗．
*/
#define  tcp_send_mesg(sock, smsg)        tcp_send((sock), (smsg), 0)



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IP address

char* get_ipaddr(struct in_addr);                        ///< 構造体　 → IPv4アドレス（文字列）
char* get_ipaddr_byname(char*);                          ///< ホスト名 → IPv4アドレス（文字列）　
char* get_mynetaddr(void);                               ///< 自分の IPv4ネットワークアドレス（文字列）
char* get_localip_bydest(char* dest);                    ///< 自分の IPv4アドレス（文字列）

char* get_hostname_bynum(unsigned char*);                ///< IPv4アドレス（数字）  → ホスト名（文字列）
char* get_hostname(char*);                               ///< IPv4アドレス（文字列）→ ホスト名（文字列）

unsigned char* get_ipaddr_num(struct in_addr);           ///< 構造体　 → IPv4アドレス（数字4byte）
unsigned char* get_ipaddr_byname_num(char*);             ///< ホスト名 → IPv4アドレス（数字4byte）
unsigned char* get_mynetaddr_num(void);                  ///< 自分の IPv4ネットワークアドレス（数字）


/**
char*  get_localip(void)

自分自身の IPアドレスを得るもう一つの方法．@n
リモートマシンに接続（ダミー）を試み，そのインターフェイス情報を獲得する．
@return IPアドレス(文字列)へのポインタ．要 free
 */
#define get_localip() get_localip_bydest("202.26.158.1") ///< 自分の IPアドレス（文字列）


#ifdef WIN32
    #define get_myipaddr() get_localip_bydest("202.26.158.1")
#else
    char* get_myipaddr(void);                            ///< 自分の [IPアドレス]/[ネットマスク]（文字列）
#endif
unsigned char* get_myipaddr_num(void);                   ///< 自分の [IPアドレス],[ネットマスク]（数字）


struct sockaddr_in get_sockaddr(char* hostname, unsigned short cport);
struct sockaddr_in get_sockaddr_bynum(char* ipnum, unsigned short cport);
struct sockaddr_in get_local_sockaddr(unsigned short cport);
int    is_same_sockaddr(struct sockaddr_in addr1, struct sockaddr_in addr2);


unsigned char* to_address_num8(char* addr, int mode);    ///< IPアドレス（文字列）→ [IPアドレス],[ネットマスク]（数字8byte）
char*          to_address_char8(unsigned char* addr);    ///< [IPアドレス],[ネットマスク]（数字8byte）→ IPアドレス（文字列）
unsigned char* to_address_num4(char* addr, int mode);    ///< IPアドレス（文字列）→ [IPアドレス]（数字4byte）
char*          to_address_char4(unsigned char* addr);    ///< [IPアドレス]（数字4byte）→ IPアドレス（文字列）


int  is_same_network_num(unsigned char* addr1, unsigned char* addr2, unsigned char* mask);
int  is_same_network(char* addr1, char* addr2, char* mask);


#endif



