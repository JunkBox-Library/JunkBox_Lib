/** 
@brief   汎用拡張ツールプログラム 
@file    xtools.c
@version 1.1
@author  Fumi.Iseki (C)
@date    2009 2/13

@par 内容
主に，Buffer型, tList型とネットワークのライブラリ@n
滅多に使わないけれど，あれば便利な関数．

@see Buffer, _tList
*/

#include "xtools.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Networks for IPv4 互換用
//

/**
int  udp_recv_Buffer_sockaddr_in(int sock, Buffer* str, struct sockaddr_in* sv_addr)

recvform()をラッピングした関数．UDPデータを受信する．IPv4専用．

@param  sock     ソケット記述子
@param  str      受信用メッセージバッファへのポインタ．予めバッファ部を確保しておく．
@param  sv_addr  サーバの情報が格納された sockaddr_in 構造体へのポインタ．
 
@retval 1以上  受信したバイト数．
@retval 0      正常切断．
@retval JBXL_ARGS_ERROR      不正な引数
@retval JBXL_NET_RECV_ERROR  受信失敗

@see recvform()
*/
int  udp_recv_Buffer_sockaddr_in(int sock, Buffer* str, struct sockaddr_in* sv_addr)
{
    int cc, cadlen;

    if (sv_addr==NULL) return JBXL_ARGS_ERROR;

    cadlen = sizeof(*sv_addr);
    memset(str->buf, 0, str->bufsz);
    str->vldsz = 0;
    str->state = JBXL_NORMAL;
    cc = recvfrom(sock, (char*)str->buf, str->bufsz, 0, (struct sockaddr*)sv_addr, (socklen_t*)&cadlen);

    if (cc>=0) str->vldsz = cc;
    else       str->state = cc = JBXL_NET_RECV_ERROR;

    return cc;
}


/**
int  udp_send_Buffer_sockaddr_in(int sock, Buffer* str, struct sockaddr_in* sv_addr)

sendto() をラッピングした関数．UDP経由でデータを送る．IPv4専用．

@param  sock     ソケット記述子
@param  str      送信するデータ
@param  sv_addr  サーバの情報を格納する sockaddr_in 構造体へのポインタ．

@retval 0以上  送信したバイト数．
@retval JBXL_ARGS_ERROR      不正な引数
@retval JBXL_NET_SEND_ERROR  送信失敗

@see sendto()
*/
int  udp_send_Buffer_sockaddr_in(int sock, Buffer* str, struct sockaddr_in* sv_addr)
{
    int cc;

    if (sv_addr==NULL) return JBXL_ARGS_ERROR;
    
    if (str->vldsz<0) str->vldsz = (int)strlen((const char*)str->buf);
    cc = sendto(sock, (char*)str->buf, str->vldsz, 0, (struct sockaddr*)sv_addr, sizeof(*sv_addr));

    if (cc<0) cc = JBXL_NET_SEND_ERROR;
    return cc;
}


/**
int  udp_send_sBuffer_sockaddr_in(int sock, Buffer* str, struct sockaddr_in* sv_addr)

UDP経由で文字列データを送る．IPv4専用．

@param  sock     ソケット記述子
@param  str      送信するデータ
@param  sv_addr  サーバの情報を格納する sockaddr_in 構造体へのポインタ．

@retval 0以上  送信したバイト数．
@retval JBXL_ARGS_ERROR      不正な引数
@retval JBXL_NET_SEND_ERROR  送信失敗
*/
int  udp_send_sBuffer_sockaddr_in(int sock, Buffer* str, struct sockaddr_in* sv_addr)
{
    int cc;

    if (sv_addr==NULL) return JBXL_ARGS_ERROR;
    
    cc = sendto(sock, (char*)str->buf, (int)strlen((const char*)str->buf), 0, (struct sockaddr*)sv_addr, sizeof(*sv_addr));
    if (cc<0) cc = JBXL_NET_SEND_ERROR;

    return cc;
}


/**
int  udp_recv_Buffer_wait_sockaddr_in(int sock, Buffer* str, struct sockaddr_in* sv_addr, int tm)

UDP経由でデータを受信する．

タイムアウトの設定が可能．タイムアウトに 0を指定した場合, recv_wait()関数を呼び出した時点で読み込み可能データがなければ
すぐにタイムアウトとなる (JBXL_NET_RECV_TIMEOUT が返る)．

@param  sock     ソケット記述子
@param  str      受信用メッセージバッファ．予めメモリを確保しておく．
@param  sv_addr  サーバの情報が格納された sockaddar_in 構造体へのポインタ．
@param  tm       タイムアウト時間．秒単位．
 
@retval  0以上   受信したバイト数．
@retval  JBXL_ARGS_ERROR        不正な引数
@retval  JBXL_NET_RECV_ERROR    受信失敗．
@retval  JBXL_NET_RECV_TIMEOUT  タイムアウト．
*/
int  udp_recv_Buffer_wait_sockaddr_in(int sock, Buffer* str, struct sockaddr_in* sv_addr, int tm)
{
    int cc;

    if (sv_addr==NULL) return JBXL_ARGS_ERROR;

    //memset(str->buf, 0, str->bufsz);
    str->vldsz = 0;
    cc = udp_recv_wait_sockaddr_in(sock, (char*)str->buf, str->bufsz, sv_addr, tm);
    if (cc>=0) str->vldsz = cc;

    return cc;
}


/**
struct sockaddr_in  get_sockaddr_in_Buffer(Buffer buf)

FQDN:port または IPaddress:port の形式の Buffer変数から ソケット情報を得る．@n
get_sockaddr_in() (in network.c) の Buffer版

@param  buf  "FQDN:ポート番号" または "IPアドレス:ポート番号" 形式のデータ．
@return sockaddr_in 構造体型のソケット情報

@see get_sockaddr_in()
*/
struct sockaddr_in  get_sockaddr_in_Buffer(Buffer buf)
{
    int i, port = 0;
    struct sockaddr_in  addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    if (buf.buf==NULL) return addr;
    
    i = 0;
    while (buf.buf[i]!='\0' && buf.buf[i]!=':') i++;
    buf.buf[i] = '\0';

    port = atoi((char*)&buf.buf[i+1]);
    addr = get_sockaddr_in((char*)buf.buf, port);

    return addr;
}


/*
Buffer get_hostport_sockaddr_in(struct sockaddr_in addr)
{
    Buffer buf;
    char  hostport[LBUF];
    char* ip = get_ipaddr_ipv4(addr.sin_addr);
    unsigned short port = ntohs(addr.sin_port);

    snprintf(hostport, LBUF, "%s:%d", ip, port);
    
    buf = make_Buffer_bystr(hostport);
    return buf;
}
*/



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Networks for IPv4/IPv6 
//

/**
int  udp_recv_Buffer(int sock, Buffer* str, struct addrinfo* sv_addr)

recvform()をラッピングした関数．UDPデータを受信する．

@param  sock     ソケット記述子
@param  str      受信用メッセージバッファへのポインタ．予めバッファ部を確保しておく．
@param  sv_addr  サーバの情報が格納された addrinfo 構造体へのポインタ．
 
@retval 1以上  受信したバイト数．
@retval 0      正常切断．
@retval JBXL_ARGS_ERROR      不正な引数
@retval JBXL_NET_RECV_ERROR  受信失敗

@see recvform()
*/
int  udp_recv_Buffer(int sock, Buffer* str, struct addrinfo* sv_addr)
{
    int cc;
    socklen_t cadlen;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;
    if (sv_addr==NULL) return JBXL_ARGS_ERROR;

    cadlen = (int)sv_addr->ai_addrlen;
    memset(str->buf, 0, str->bufsz);
    str->vldsz = 0;
    str->state = JBXL_NORMAL;
    cc = recvfrom(sock, (char*)str->buf, str->bufsz, 0, sv_addr->ai_addr, &cadlen);

    if (cc>=0) str->vldsz = cc;
    else       str->state = cc = JBXL_NET_RECV_ERROR;

    return cc;
}


/**
int  udp_send_Buffer(int sock, Buffer* str, struct addrinfo* sv_addr)

sendto() をラッピングした関数．UDP経由でデータを送る．

@param  sock     ソケット記述子
@param  str      送信するデータ
@param  sv_addr  サーバの情報を格納する addrinfo 構造体へのポインタ．

@retval 0以上  送信したバイト数．
@retval JBXL_ARGS_ERROR      不正な引数
@retval JBXL_NET_SEND_ERROR  送信失敗

@see sendto()
*/
int  udp_send_Buffer(int sock, Buffer* str, struct addrinfo* sv_addr)
{
    int cc;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;
    if (sv_addr==NULL) return JBXL_ARGS_ERROR;
    
    if (str->vldsz<0) str->vldsz = (int)strlen((const char*)str->buf);
    cc = sendto(sock, (char*)str->buf, str->vldsz, 0, sv_addr->ai_addr, (int)sv_addr->ai_addrlen);

    if (cc<0) cc = JBXL_NET_SEND_ERROR;

    return cc;
}


/**
int  tcp_recv_Buffer(int sock, Buffer* str)

TCP経由でデータを受信する．バッファリングなし．

@param  sock  ソケット記述子
@param  str   受信用メッセージバッファへのポインタ．予めバッファ部を確保しておく．

@retval 1以上  受信したバイト数．
@retval 0      正常切断．
@retval JBXL_NET_RECV_ERROR  受信失敗
*/
int  tcp_recv_Buffer(int sock, Buffer* str)
{
    int cc;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;

    memset(str->buf, 0, str->bufsz);
    str->vldsz = 0;
    str->state = JBXL_NORMAL;
    cc = recv(sock, (char*)str->buf, str->bufsz, 0);

    if (cc>=0) str->vldsz = cc;
    else       str->state = cc = JBXL_NET_RECV_ERROR;

    return cc;
}


/**
int  tcp_send_Buffer(int sock, Buffer* str)

TCP経由でデータを送信する．

@param  sock  ソケット記述子
@param  str   送信用データバッファ．

@retval 0以上  送信したバイト数．
@retval JBXL_NET_SEND_ERROR  送信失敗
*/
int  tcp_send_Buffer(int sock, Buffer* str)
{
    int cc;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;

    if (str->vldsz<0) str->vldsz = (int)strlen((const char*)str->buf);
    cc = send(sock, (char*)str->buf, str->vldsz, 0);

    if (cc<0) cc = JBXL_NET_SEND_ERROR;
    return cc;
}


/**
int  udp_recv_Buffer_wait(int sock, Buffer* str, struct addrinfo* sv_addr, int tm)

UDP経由でデータを受信する．待ち時間（タイムアウト）を指定できる．

タイムアウトの設定が可能．タイムアウトに 0を指定した場合, recv_wait()関数を呼び出した時点で読み込み可能データがなければ
すぐにタイムアウトとなる (JBXL_NET_RECV_TIMEOUT が返る)．

@param  sock     ソケット記述子
@param  str      受信用メッセージバッファ．予めメモリを確保しておく．
@param  sv_addr  サーバの情報が格納された sockaddar_in 構造体へのポインタ．
@param  tm       タイムアウト時間．秒単位．
 
@retval  0以上   受信したバイト数．
@retval  JBXL_ARGS_ERROR        不正な引数
@retval  JBXL_NET_RECV_ERROR    受信失敗．
@retval  JBXL_NET_RECV_TIMEOUT  タイムアウト．
*/
int  udp_recv_Buffer_wait(int sock, Buffer* str, struct addrinfo* sv_addr, int tm)
{
    int cc;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;
    if (sv_addr==NULL) return JBXL_ARGS_ERROR;

    //memset(str->buf, 0, str->bufsz);
    str->vldsz = 0;
    cc = udp_recv_wait(sock, (char*)str->buf, str->bufsz, sv_addr, tm);
    if (cc>=0) str->vldsz = cc;

    return cc;
}


/**
int  tcp_recv_Buffer_wait(int sock, Buffer* str, int tm)

TCP経由でデータを受信する．待ち時間（タイムアウト）を指定できる．

タイムアウトの設定が可能．タイムアウトに 0を指定した場合, recv_wait()関数を呼び出した時点で
読み込み可能データがなければすぐにタイムアウトとなる (JBXL_NET_RECV_TIMEOUT が返る)．

@param  sock    ソケット記述子
@param  str     受信用メッセージバッファへのポインタ．予めバッファ部を確保しておく．
@param  tm      タイムアウト時間．秒単位．

@retval  1以上  受信されたバイト数．
@retval  0      受信エラー．おそらくは相手側がセッションをクローズした．
@retval  JBXL_NET_RECV_ERROR   受信エラー．
@retval  JBXL_NET_RECV_TIMEOUT タイムアウト．
*/
int  tcp_recv_Buffer_wait(int sock, Buffer* str, int tm)
{
    int cc;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;

    //memset(str->buf, 0, str->bufsz);
    str->vldsz = 0;
    cc = tcp_recv_wait(sock, (char*)str->buf, str->bufsz, tm);
    if (cc>=0) str->vldsz = cc;

    return cc;
}


/**
int  tcp_recv_Buffer_tosize(int sock, Buffer* str, Buffer* mod, int size)

TCP経由でデータを現データと合わせて size バイトまで受信する．

@param  sock   ソケット記述子
@param  str    受信用メッセージバッファへのポインタ．予めバッファ部を確保しておく．
@param  mod    size 以上受信した場合はここに保存する．予めメモリを確保しておく．NULL でも OK
@param  size   目標バイト数．

@retval 1以上  受信したバイト数．
@retval 0      正常切断．
@retval JBXL_ARGS_ERROR      引数に NULLのデータがある．
@retval JBXL_NET_RECV_ERROR  受信失敗
*/
int  tcp_recv_Buffer_tosize(int sock, Buffer* str, Buffer* mod, int size)
{
    int cc, sz;
    int modon = FALSE;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;
    if (mod!=NULL && mod->buf!=NULL) modon = TRUE;

    if (modon) memset(mod->buf, 0, mod->vldsz);
    Buffer buf = make_Buffer(RECVBUFSZ);

    sz = str->vldsz;
    while (sz<size) {
        cc = tcp_recv_Buffer(sock, &buf);
        if (cc>0) {
            cat_Buffer(&buf, str);
            sz += cc;
        }
        else {
            if (cc<0) sz = cc;
            break;
        }
    }
    free_Buffer(&buf);

    if (sz>size && modon) {
        copy_b2Buffer((void*)(str->buf+size), mod, sz-size);
        str->vldsz = size;
    }
    return sz;
}


/**
int  tcp_recv_Buffer_tosize_wait(int sock, Buffer* str, Buffer* mod, int size, int tm)

TCP経由でデータを  現データと合わせて size バイトまで受信する．待ち時間（タイムアウト）を指定できる．

タイムアウトの設定が可能．タイムアウトに 0を指定した場合, recv_wait()関数を呼び出した時点で
読み込み可能データがなければすぐにタイムアウトとなる (JBXL_NET_RECV_TIMEOUT が返る)．

@param  sock   ソケット記述子
@param  str    受信用メッセージバッファへのポインタ．予めバッファ部を確保しておく．
@param  mod    size 以上受信した場合はここに保存する．予めメモリを確保しておく．NULL でも OK
@param  size   目標バイト数．
@param  tm     タイムアウト時間．秒単位．

@retval 1以上  受信されたバイト数．
@retval 0      受信エラー．おそらくは相手側がセッションをクローズした．
@retval JBXL_ARGS_ERROR       引数に NULLのデータがある．
@retval JBXL_NET_RECV_ERROR   受信エラー．
@retval JBXL_NET_RECV_TIMEOUT タイムアウト．
*/
int  tcp_recv_Buffer_tosize_wait(int sock, Buffer* str, Buffer* mod, int size, int tm)
{
    int cc, sz;
    int modon = FALSE;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;
    if (mod!=NULL && mod->buf!=NULL) modon = TRUE;

    if (modon) memset(mod->buf, 0, mod->vldsz);
    Buffer buf = make_Buffer(RECVBUFSZ);

    sz = str->vldsz;
    while (sz<size) {
        cc = tcp_recv_Buffer_wait(sock, &buf, tm);
        if (cc>0) {
            cat_Buffer(&buf, str);
            sz += cc;
        }
        else {
            if (cc<0) sz = cc;
            break;
        }
    }
    free_Buffer(&buf);

    if (sz>size && modon) {
        copy_b2Buffer((void*)(str->buf+size), mod, sz-size);
        str->vldsz = size;
    }
    return sz;
}


/**
int  udp_send_sBuffer(int sock, Buffer* str, struct addrinfo* sv_addr)

UDP経由で文字列データを送る．

@param  sock     ソケット記述子
@param  str      送信するデータ
@param  sv_addr  サーバの情報を格納する addrinfo 構造体へのポインタ．

@retval 0以上  送信したバイト数．
@retval JBXL_ARGS_ERROR      不正な引数
@retval JBXL_NET_SEND_ERROR  送信失敗
*/
int  udp_send_sBuffer(int sock, Buffer* str, struct addrinfo* sv_addr)
{
    int cc;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;
    if (sv_addr==NULL) return JBXL_ARGS_ERROR;
    
    cc = sendto(sock, (char*)str->buf, (int)strlen((const char*)str->buf), 0, sv_addr->ai_addr, (int)sv_addr->ai_addrlen);
    if (cc<0) cc = JBXL_NET_SEND_ERROR;

    return cc;
}


/**
int  tcp_send_sBuffer(int sock, Buffer* str)

TCP経由で文字列データを送信する．

@param  sock    ソケット記述子
@param  str     送信用データバッファ．

@retval  0以上  送信バイト数．
@retval -1      失敗 (send()の戻り値)．
*/
int  tcp_send_sBuffer(int sock, Buffer* str)
{
    int cc;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;

    cc = send(sock, (char*)str->buf, (int)strlen((const char*)str->buf), 0);
    return cc;
}


/**
int  tcp_send_sBufferln(int sock, Buffer* str)

TCPメッセージ(文字列)に改行(@\r@\n)を付け加えて送信する．

@param  sock    ソケット記述子
@param  str     送信用メッセージバッファ．

@retval  0以上  送信バイト数（改行を含む）．
@retval - 1     失敗 (send()の戻り値)．
*/
int  tcp_send_sBufferln(int sock, Buffer* str)
{
    int   cc;
    Buffer buf;

    if (str==NULL || str->buf==NULL) return JBXL_ARGS_ERROR;

    buf = dup_Buffer(*str);
    cat_s2Buffer("\r\n", &buf);
    cc = send(sock, (char*)buf.buf, (int)strlen((const char*)buf.buf), 0);
    free_Buffer(&buf);

    return cc;
}


/**
int  tcp_recv_mstream_Buffer(int sock, Buffer* mesg, mstream* sb, int tm)

TCP経由でメッセージを受信する．

受信メッセージはメッセージストリーム に一旦バッファリングされ，この関数により一行ずつ読み出される．@n
mesgに格納される時，行中の改行コードは削除され，行末には必ず '@\0' が入る．

タイムアウトの設定が可能でタイムアウトに 0を指定した場合, 呼び出した時点で
読み込み可能データがなければすぐにタイムアウトとなる (JBXL_NET_RECV_TIMEOUT が返る)．@n
メッセージストリームのバッファ部が確保されていない場合は，最初に呼び出された時点で確保される．

一旦この関数を使用して，受信データをバッファリングしたら，ソケットをクローズするまで，
読み取りには必ず同じストリームを使用してこの関数を呼び出さばければならない．@n
そうで無い場合は受信データの整合性は保証されない．

@param  sock    ソケット記述子
@param  mesg    受信用データバッファ．バッファ部を確保する必要はない．
@param  sb      メッセージバッファ（リング型のストリームバッファ）
                バッファ部が確保さえていなければ，自動的に確保される．
@param  tm      タイムアウト時間．秒単位．

@retval  1以上  mesgに格納したメッセージのバイト数．
@retval  0      おそらくは相手側がセッションをクローズした．
@retval  JBXL_NET_RECV_ERROR    受信エラー．
@retval  JBXL_ARGS_ERROR        引数に NULLのデータがある．
@retval  JBXL_MALLOC_ERROR      メッセージバッファ部が無いので確保しようとしたが，確保に失敗した．
@retval  JBXL_NET_BUF_ERROR     メッセージバッファにデータは存在するはずだが，原因不明の理由により獲得に失敗した．
@retval  JBXL_NET_RECV_TIMEOUT  タイムアウト．
*/
int  tcp_recv_mstream_Buffer(int sock, Buffer* mesg, mstream* sb, int tm)
{
    int    cc;
    unsigned char* pp;

    if (mesg==NULL || sb==NULL) return JBXL_ARGS_ERROR;
    *mesg = make_Buffer(LBUF);

    if (sb->buf==NULL) {
        *sb = make_mstream(RECVBUFSZ);
        if (sb->buf==NULL) return JBXL_MALLOC_ERROR;
    }

    while (sb->datano==0) {
        cc = tcp_recv_Buffer_wait(sock, mesg, tm);
        if (cc<=0) {
            free_Buffer(mesg);
            return cc;
        }
        put_mstream(sb, mesg->buf);
        clear_Buffer(mesg);
    }

    pp = get_mstream(sb);
    if (pp==NULL) return JBXL_NET_BUF_ERROR;
    copy_s2Buffer((char*)pp, mesg);
    free(pp);

    return mesg->vldsz;
}


/**
int  tcp_recv_lines_Buffer(int sock, Buffer* mesg, int tm)

TCP経由でメッセージを受信する．

複数行分（@\nで終わることが保証される）のデータを取り出さすことのできる簡易バッファ機能付き．
ここからさらに一行分のデータを取り出すには，get_line() などを使用する．@n
また，ネットワークより直接一行づつ取り出すには，tcp_recv_mstream_Buffer() を使うほうが良い．

@param  sock    ソケット記述子
@param  mesg    受信用データバッファ．予めメモリを確保しておく．
@param  tm      タイムアウト時間．秒単位．

@retval  1以上  受信されたバイト数．
@retval  0      おそらくは相手側がセッションをクローズした．
@retval  JBXL_NET_RECV_ERROR    受信エラー．
@retval  JBXL_ARGS_ERROR        bufがNULL
@retval  JBXL_NET_RECV_TIMEOUT  タイムアウト．

@bug 受信データが必ず LFで終わることが保障されている場合にしか使用できない．
     つまり汎用的には「使えない」
*/
int  tcp_recv_lines_Buffer(int sock, Buffer* mesg, int tm)
{
    int    cc;
    Buffer buf;

    if (mesg==NULL) return JBXL_ARGS_ERROR;
    buf = make_Buffer(LBUF);

    cc = tcp_recv_Buffer_wait(sock, &buf, tm);
    while (cc>0) {
        cat_Buffer(&buf, mesg);
        if (buf.buf[cc-1]==CHAR_LF) break;
        clear_Buffer(&buf);
        cc = tcp_recv_Buffer_wait(sock, &buf, tm);
    }
    free_Buffer(&buf);
        
    if (cc<=0) {
        free_Buffer(mesg);
        return cc;
    }

    return mesg->vldsz;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// for URL
//

/**
Buffer  comp_hostport(char* host, unsigned short port)

ホスト名とポート番号から，"ホスト名:ポート番号" の文字列を生成する．

@param  host  ホスト名，またはIPアドレス
@param  port  ポート番号

@return "ホスト名:ポート番号" の文字列が格納されたバッファ型データ．
*/
Buffer  comp_hostport(char* host, unsigned short port)
{
    Buffer buf;
    char   hostport[LBUF];

    hostport[0] = '\0';

    if (host!=NULL) {
        if (port!=0) snprintf(hostport, LBUF-1, "%s:%d", host, port);
        else         snprintf(hostport, LBUF-1, "%s", host);
    }
    else {
        if (port!=0) snprintf(hostport, LBUF-1, "%d", port);
    }

    buf = make_Buffer_bystr(hostport);

    return buf;
}


/**
int  decomp_hostport(Buffer buf, Buffer* host, unsigned short* port)

"ホスト名:ポート番号" 形式の文字列から，ホスト名とポート番号を分離する．

@param      buf   "ホスト名:ポート番号" 形式の文字列の入ったBufferデータ
@param[out] host  "ホスト名" が格納されて返る．
@param[out] port  ポート番号 が格納されて返る．

@retval TRUE      分離成功
@retval FALSE     分離失敗
*/
int  decomp_hostport(Buffer buf, Buffer* host, unsigned short* port)
{
    if (buf.buf==NULL) return FALSE;

    if (port!=NULL) {
        int i = 0;
        while (buf.buf[i]!='\0' && buf.buf[i]!=':') i++;
        if (buf.buf[i]!='\0') *port = (unsigned short)atoi((char*)&buf.buf[i+1]);
        else *port = 0;
    }

    if (host!=NULL) {
        Buffer hostport = dup_Buffer(buf);
    
        int i = 0;
        while (hostport.buf[i]!='\0' && hostport.buf[i]!=':') i++;
        hostport.buf[i] = '\0';
        hostport.vldsz  = (int)strlen((char*)hostport.buf);
        *host = hostport;
    }

    return TRUE;
}


/**
Buffer  comp_url(char* protocol, char* host, unsigned short port, char* dir)

"プロトコル://ホスト名:ポート番号/ディレクトリ名" の文字列を生成する．

@param  protocol  プロトコル名
@param  host      ホスト名
@param  port      ポート番号
@param  dir       ディレクトリ

@return "プロトコル名://ホスト名:ポート番号/ディレクトリ名" の文字列が格納されたバッファ型データ．
*/
Buffer  comp_url(char* protocol, char* host, unsigned short port, char* dir)
{
    Buffer url;
    char   num[20];

    url = make_Buffer(LNAME);

    if (protocol!=NULL) {
        copy_s2Buffer(protocol, &url);
        cat_s2Buffer("://", &url);
    }

    cat_s2Buffer(host, &url);

    if (port!=0) {
        snprintf(num, 10, ":%d", port);
        cat_s2Buffer(num, &url);
    }

    if (dir!=NULL) {
        if (dir[0]!='/') cat_s2Buffer("/", &url);
        cat_s2Buffer(dir, &url);
    }

    return url;
}


/**
int  decomp_url(Buffer url, Buffer* srvurl, Buffer* protocol, Buffer* srvfqdn, unsigned short* sport, Buffer* srvdir)

URLを分解する．

分解できるURLの形式は
- protocol://www.xxx.yyy.zzz:80/AA/BB
- www.xxx.yyy.zzz:80/AA/BB

必要ない情報には NULLを指定可能．

@param url 分解するURLの入った Buffer変数．
       url が https://AAA.BBB.CCC:80/xxx/yyy/zzz の場合
@param[out]    srvurl    https://AAA.BBB.CCC:80   要 free
@param[out]    protocol  https                    要 free
@param[out]    srvfqdn   AAA.BBB.CCC              要 free
@param[out]    sport     80
@param[out]    srvdir    /xxx/yyy/zzz             要 free

@retval TRUE   分解成功
@retval FALSE  分解失敗
*/
int  decomp_url(Buffer url, Buffer* srvurl, Buffer* protocol, Buffer* srvfqdn, unsigned short* sport, Buffer* srvdir)
{
    Buffer item1, item2, item3, item4, wrk;

    if (srvurl  !=NULL) *srvurl   = init_Buffer();
    if (protocol!=NULL) *protocol = init_Buffer();
    if (srvfqdn !=NULL) *srvfqdn  = init_Buffer();
    if (srvdir  !=NULL) *srvdir   = init_Buffer();
    if (sport   !=NULL) *sport    = 0;
    if (url.buf ==NULL) return FALSE;

    item1 = init_Buffer();
    item2 = init_Buffer();
    item3 = init_Buffer();
    item4 = init_Buffer();
     
    if (strstr((char*)url.buf, "://")!=NULL) {
        // http(s)://www.tuis.ac.jp:8100/..... 
        item1 = awk_Buffer(url, '/', 1);    // http(s):
        item2 = awk_Buffer(url, '/', 3);    // www.tuis.ac.jp:8100
        if (protocol!=NULL) {
            *protocol = dup_Buffer(item1);
            protocol->buf[protocol->vldsz-1] = '\0';
            protocol->vldsz--;
        }
    }
    else {
        // www.tuis.ac.jp:8100/..... 
        item2 = awk_Buffer(url, '/', 1);    // www.tuis.ac.jp:8100
    }

    if (item2.buf!=NULL) {
        item3 = awk_Buffer(item2, ':', 1);  // www.tuis.ac.jp
        if (item3.buf==NULL) {
            free_Buffer(&item1);
            free_Buffer(&item2);
            return FALSE;
        }
        item4 = awk_Buffer(item2, ':', 2);  // 8100
    }
    else {
        free_Buffer(&item1);
        return FALSE;
    }

    if (item4.buf!=NULL && sport!=NULL) {
        *sport = (unsigned short)atoi((char*)item4.buf);
    }
     
    wrk = make_Buffer(LBUF);
    if (item1.buf!=NULL) {
        copy_Buffer(&item1, &wrk);
        cat_s2Buffer("//",  &wrk);
    }
    cat_Buffer(&item2,  &wrk);

    if (item3.buf!=NULL) {
        if (srvfqdn!=NULL) *srvfqdn = item3;
        else free_Buffer(&item3);
    }
     
    if (srvdir!=NULL) {
        *srvdir = make_Buffer_bystr((char*)(url.buf+strlen((char*)(wrk.buf))));
        if ((srvdir->buf)[0]=='\0') copy_s2Buffer("/", srvdir);
    }

    if (srvurl!=NULL) *srvurl = wrk;
    else free_Buffer(&wrk);

    free_Buffer(&item1);
    free_Buffer(&item2);
    free_Buffer(&item4);
     
    return TRUE;
}                                           


Buffer  make_form_urlenc(const char* key, const char* val)
{
    Buffer buf = init_Buffer();
    if (key==NULL) return buf;

    buf = make_Buffer_str(key);
    cat_s2Buffer("=", &buf);

    if (val!=NULL) {
        unsigned char* val_enc = encode_urlenc((unsigned char*)val, -1);
        if (val_enc!=NULL) {
            cat_s2Buffer(val_enc, &buf);
            free(val_enc);
        }
    }
    
    return buf;
}


void   add_form_urlenc(Buffer* buf, const char* key, const char* val)
{
    if (buf==NULL || key==NULL) return;

    cat_s2Buffer("&", buf);
    cat_s2Buffer(key, buf);
    cat_s2Buffer("=", buf);

    if (val!=NULL) {
        unsigned char* val_enc = encode_urlenc((unsigned char*)val, -1);
        if (val_enc!=NULL) {
            cat_s2Buffer(val_enc, buf);
            free(val_enc);
        }
    }

    return;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// File I/O 
//

/**
int   save_tagged_Buffer(Buffer buf, FILE* fp, unsigned int mode, int prfm)

Bufferを指定された形式に従ってタグ付きでファイルに保存する．

@param  buf    保存する情報．
@param  fp     ファイルポインタ．
@param  mode   保存するデータの種別と保存方法を JBXL_FIO_ マクロで指定する．
               ただし，保存方法は現在のところ @b JBXL_FIO_ORIGINAL, @b JBXL_FIO_BASE64 のみサポート．
@param  prfm   保存方法に @b JBXL_FIO_ORIGINAL 以外を指定した場合，その形式に変換するかどうかを指定する．
               この関数外で，既に変換が完了している場合に FALSE を指定する．
               (TERUE: 変換する． FALSE: 変換しない)

@retval TRUE   成功．
@retval FALSE  失敗．ファイルの内容は保証されない．

@attention 結局どの形式で保存されるかは，プログラマが保証しなければならない．
           できるだけデータ形式を指定すべきである．

参考：
@code
tag部　 tag.buf[0]  保存データの種別
        tag.buf[1]  保存形式　　        JBXL_FIO_ORIGINAL, JBXL_FIO_BASE64 をサポート
@endcode

@see  xtool.h
*/
int   save_tagged_Buffer(Buffer buf, FILE* fp, unsigned int mode, int prfm)
{    
    int     cc;
    Buffer  tag, enc;

    tag = make_Buffer(2);
    if (tag.buf==NULL) return FALSE;
    tag.buf[0] = mode & 0x0f;            // データの種別
    tag.buf[1] = mode & 0xf0;            // データの形式

    // JBXL_FIO_ORIGINAL, JBXL_FIO_BASE64 以外は未サポート
    if (tag.buf[1]==JBXL_FIO_ORIGINAL || !prfm) {
        enc = dup_Buffer(buf);
    }
    else if (tag.buf[1]==JBXL_FIO_BASE64) {
        enc = encode_base64_Buffer(buf);
    }
    else {                            
        enc = dup_Buffer(buf);
    }

    tag.vldsz = 2;
    cc = save_Buffer2_fp(tag, enc, fp); 
    free_Buffer(&enc);
    free_Buffer(&tag);

    if (!cc) return FALSE;
    return  TRUE;
}


/**
Buffer  read_tagged_Buffer(FILE* fp, unsigned int* mode)

save_tagged_Buffer() で保存したファイルから，Buffer をタグに従って読み込む．

@param      fp    ファイルポインタ．
@param[in]  mode  読み出したいデータの種別と読み出し時のデータ形式を JBXL_FIO_ マクロで指定する．
                  保存方されているデータと読み出したいデータの種別が一致しない場合は，データを返さない．
                  ただし，@b JBXL_FIO_ANY を指定した場合は一致しなくともデータを返す．
@param[out] mode  読み出し時のデータ形式は現在のところ @b JBXL_FIO_ORIGINAL, @b JBXL_FIO_BASE64 のみサポート．
                  リターン時に，保存されていたデータの種別のが JBXL_FIO_ マクロで格納される．

@retval TRUE      成功．
@retval FALSE     失敗．

@attention データ形式が，JBXL_FIO_ORIGINAL, JBXL_FIO_BIN, JBXL_FIO_DER等で保存されている場合は，そのデータ形式に
           対して結局はプログラマが責任を持たなければならない．

参考：  
@code
tag部　 tag.buf[0]  保存データの種別
        tag.buf[1]  保存形式　　        JBXL_FIO_ORIGINAL, JBXL_FIO_BASE64 をサポート
@endcode

@see  xtool.h
*/
Buffer  read_tagged_Buffer(FILE* fp, unsigned int* mode)
{    
    unsigned char mthd, kind;
    Buffer   tmp, tag, enc, buf;

    buf = init_Buffer();
    if (mode==NULL) return buf;

    if (!read_Buffer2_fp(&tag, &enc, fp)) return buf;

    kind = *mode & 0x0f;                // データの種別
    mthd = *mode & 0xf0;                // データの形式
    if (kind!=tag.buf[0] && kind!=JBXL_FIO_ANY) {
        free_Buffer(&enc);
        free_Buffer(&tag);
        return buf;
    }

    // JBXL_FIO_ORIGINAL, JBXL_FIO_BASE64 以外は未サポート
    if (mthd==tag.buf[1]) {                // 保存形式と呼び出し形式は同じ
        buf = dup_Buffer(enc);
    }
    else {
        // 保存データをオリジナルへ
        if (tag.buf[1]==JBXL_FIO_BASE64) {
            tmp = decode_base64_Buffer(enc);
        }
        else {
            tmp = dup_Buffer(enc);
        }

        // オリジナルを呼び出し形式へ
        if (mthd==JBXL_FIO_BASE64) {
            buf = encode_base64_Buffer(tmp);
        }
        else {
            buf = dup_Buffer(tmp);
        }
        free_Buffer(&tmp);
    }

    *mode = (unsigned int)tag.buf[0];    // データ種別
    free_Buffer(&enc);
    free_Buffer(&tag);

    return  buf;
}


/**
Buffer  fgets_mstream_Buffer(Buffer buf, mstream* sb)

メッセージ buf.buf はメッセージストリームに一旦バッファリングされ，この関数により一行ずつ読み出される．@n
結果が返される時，行中の改行コードは削除され，行末には必ず @\0 が入る．

メッセージストリームのバッファ部が確保されていない場合は，最初に呼び出された時点で確保される．@n
一旦この関数を使用して，受信データをバッファリングしたら，最後まで読み取りには
必ず同じストリームを使用してこの関数を呼び出さばければならない．そうで無い場合は受信データ
の整合性は保証されない．

@param      buf  バッファに一旦格納されるメッセージの入ったBuffer型変数．buf.bufはNULLでも可．
@param[in]  sb   ストリームバッファ（リングバッファ型のストリームバッファ）．バッファ部が確保さえていなければ，自動的に確保される．
@param[out] sb   @b state JBXL_ERROR : メッセージストリーム操作中にエラー
@param[out] sb   @b state JBXL_NODATA: メッセージストリーム中に有効なデータがない．return は NULL

@return 取り出した文字列へのポインタ
*/
Buffer  fgets_mstream_Buffer(Buffer buf, mstream* sb)
{
    Buffer   ret;
    unsigned char* pp;

    ret = init_Buffer();

    pp = fgets_mstream(buf.buf, sb);
    if (pp!=NULL) {
        ret = make_Buffer(LBUF);
        copy_s2Buffer((char*)pp, &ret);
        free(pp);
    }
    return ret;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
// Run Length
//

/**
int  get_runlength_byte(unsigned char* buf, int len, int pos)

圧縮されていない状態で pos(Byte)の位置が，0の連長圧縮されているデータではどの位置に来るかを計算する．

@param  buf     連長圧縮(0)されているバイナリデータ
@param  len     buf の長さ
@param  pos     圧縮されていない場合の位置．

@return 0以上  連長圧縮されているデータでの位置．
@retval JBXL_ERROR  位置を見つけられなかった．
@retval JBXL_XTOOLS_RUNLEN_ERROR   0x00 で終わっている．
@retval JBXL_XTOOLS_RUNLEN2_ERROR  0x00, 0x00 が存在する（連長圧縮のデータではないのでは？）
*/
int  get_runlength_byte(unsigned char* buf, int len, int pos)
{
    int i, sz;

    sz = -1;
    for (i=0; i<len; i++) {
        if (buf[i]==0x00) {
            if (i+1>=len) return JBXL_XTOOLS_RUNLEN_ERROR;
            if (buf[i+1]==0x00) return JBXL_XTOOLS_RUNLEN2_ERROR;
            sz += buf[i+1];
            if (sz>=pos) return i;
            i++;
        }
        else sz++;

        if (sz>=pos) return i;
    }

    return JBXL_ERROR;
}


/**
Buffer  decode_runlength(unsigned char* buf, int len, int sz)

0の連長圧縮された bufから szバイトの通常のデータを取り出す．

@param  buf  連長圧縮(0)されているバイナリデータ
@param  len  buf の長さ（チェック用）
@param  sz   取り出すサイズ．0の場合は全て．

@return 解凍されたデータを含んだ Buffer型変数
*/
Buffer  decode_runlength(unsigned char* buf, int len, int sz)
{
    int    i, j, k;
    Buffer ret = init_Buffer();

    // 長さを数える
    i = j = 0;
    if (sz<=0) {
        while (j<len) {
            if (buf[j]==0x00) {
                if (j+1<len) {
                    if (buf[j+1]==0x00) return ret;
                    i += (int)buf[j+1];
                    j += 2;
                }
                else break;
            }
            else {
                i++;
                j++;
            }
        }

        if (j==len) sz = i;
        else return ret;
    }
        
    ret = make_Buffer(sz);
    if (ret.buf==NULL) return ret;

    i = j = 0;
    while (i<sz && j<len) {
        if (buf[j]==0x00) {
            if (j+1<len) {
                for (k=0; k<(int)buf[j+1]; k++) ret.buf[i++] = 0x00;
                j += 2;
            }
            else break;
        }    
        else ret.buf[i++] = buf[j++];
    }

    if (i==sz) ret.vldsz = sz;
    else free_Buffer(&ret);

    return ret;
}


Buffer  encode_runlength(unsigned char* buf, int len)
{
    int    i, j, k;
    Buffer ret = init_Buffer();

    ret = make_Buffer(2*len);
    if (ret.buf==NULL) return ret;

    i = j = 0;
    while (j<len) {
        ret.buf[i] = buf[j];

        if (buf[j]==0x00) {
            k = 0;
            while (buf[j]==0x00 && j<len) {
                k++;
                j++;
            }
            ret.buf[i+1] = (unsigned char)k;
            i += 2;
        }
        else {
            i++;
            j++;
        }
    }

    ret.vldsz = i;
    return ret;
}



////////////////////////////////////////////////////////////////////////////////////
// Ring Buffer
// 

/**
int  put_Buffer_ringBuffer(ringBuffer* rb, Buffer* buf)

リングバッファ rb へ Buffer データを vldszバイト格納する．

@param[in]  rb   リングバッファへのポインタ
@param[out] rb   @b state  JBXL_NORMAL: バッファは正常状態．JBXL_ERROR: バッファは異常な状態にある．
@param      buf  格納する Bufferデータへのポインタ．

@retval 0以上  書き込んだバイト数．
@retval JBXL_ARGS_ERROR         引数にNULLのデータがある．
@retval JBXL_TOOLS_BUF_ERROR    バッファ（データ格納）部の領域がない．
@retval JBXL_TOOLS_BUFSZ_ERROR  バッファ（データ格納）部の大きさがたりない．データはputされなかった．
*/
int  put_Buffer_ringBuffer(ringBuffer* rb, Buffer* buf)
{
    if (rb==NULL) return JBXL_ARGS_ERROR;
    rb->state = JBXL_NORMAL;

    if (buf==NULL)     return JBXL_ARGS_ERROR;
    if (rb->buf==NULL) return JBXL_TOOLS_BUF_ERROR;
    if (rb->datasz + buf->vldsz > rb->bufsz) {
        rb->state = JBXL_ERROR;
        return JBXL_TOOLS_BUFSZ_ERROR;
    }

    if (rb->epoint + buf->vldsz <= rb->bufsz) {
        memcpy(&(rb->buf[rb->epoint]), buf->buf, buf->vldsz);
        rb->epoint = rb->epoint + buf->vldsz;
        if (rb->epoint==rb->bufsz) rb->epoint = 0;
    }
    else {
        memcpy(&(rb->buf[rb->epoint]), buf->buf, rb->bufsz - rb->epoint);
        memcpy(rb->buf, &(buf->buf[rb->bufsz - rb->epoint]), buf->vldsz - (rb->bufsz - rb->epoint));
        rb->epoint = rb->epoint + buf->vldsz - rb->bufsz;
    }

    rb->datasz += buf->vldsz;
    return buf->vldsz;
}


/**
Buffer*  get_Buffer_ringBuffer(ringBuffer* rb, int sz)

リングバッファ rb から szバイトのデータを取り出し，Buffer型データに格納する．@n
返されたデータは del_Buffer() する必要がある．

@param[in]  rb   リングバッファへのポインタ
@param[out] rb   @b state JBXL_NORMAL: バッファは正常状態．JBXL_ERROR: バッファは異常な状態にある．
@param      sz   取り出すバイト数．

@retval NULL以外 取り出した Bufferデータへのポインタ．要 del_Buffer() 
@retval NULL     データ取得失敗．現在有効なデータサイズはszより小さい．または単に失敗した．
*/
Buffer*  get_Buffer_ringBuffer(ringBuffer* rb, int sz)
{
    if (rb==NULL) return NULL;
    if (rb->buf==NULL) return NULL;
    if (sz>rb->datasz) return NULL;

    Buffer* buf = new_Buffer(sz);

    if (rb->spoint + sz <= rb->bufsz) {
        copy_b2Buffer(&(rb->buf[rb->spoint]), buf, sz);
        rb->spoint = rb->spoint + sz;
        if (rb->spoint==rb->bufsz) rb->spoint = 0;
    }
    else {
        copy_b2Buffer(&(rb->buf[rb->spoint]), buf, rb->bufsz - rb->spoint);
        cat_b2Buffer(rb->buf, buf, sz - (rb->bufsz - rb->spoint));
        rb->spoint = rb->spoint + sz - rb->bufsz;
    }

    rb->datasz -= sz;
    return buf;
}



////////////////////////////////////////////////////////////////////////////////////
//  Buffer 配列
// 

/**
Buffer*  get_Buffer_dim_tList(tList* lp)

リストを分解して，キー部(lp->ldat.key)の配列を造る

@param  lp  処理対象リスト

@return リストのキー部(lp->ldat.key)をコピーしたBuffer型配列（Buffer[0],Buffer[1],.....）へのポインタ．
        各要素の stateにその要素以降の要素数（自分も含む）が格納される．
        すなわち，0番目の要素の stateには配列の大きさが格納される．
*/
Buffer*  get_Buffer_dim_tList(tList* lp)
{
    int     nn, mx;
    tList*  lt;
    Buffer* buf;
    Buffer* ret;

    if (lp==NULL) return NULL;

    // リスト中のノードの数を数える．
    lt = lp;
    nn = 0;
    while(lt!=NULL && lt->ldat.key.buf!=NULL) {
        nn++;
        lt = lt->next;
    }
    if (nn==0) return NULL;

    mx = nn;
    buf = ret = (Buffer*)malloc(sizeof(Buffer)*mx);
    if (ret==NULL) return NULL;
    
    nn = 0;
    while(lp!=NULL && lp->ldat.key.buf!=NULL) {
        *buf = dup_Buffer(lp->ldat.key);
        buf->state = mx - nn;
        nn++;
        buf++;
        lp = lp->next;
    }
        
    return ret;
}


/**
Buffer*  get_Buffer_dim_tList_value(tList* lp)

リストを分解して，バリュー部(lp->ldat.val)の配列を造る

@param  lp  処理対象リスト

@return リストのバリュー部(lp->ldat.val)をコピーしたBuffer型配列へのポインタ．
        各要素の stateにその要素以降の要素数（自分も含む）が格納される．
        すなわち，0番目の要素の stateには配列の大きさが格納される．
*/
Buffer*  get_Buffer_dim_tList_value(tList* lp)
{
    int     nn, mx;
    tList*  lt;
    Buffer* buf;
    Buffer* ret;

    if (lp==NULL) return NULL;

    lt = lp;
    nn = 0;
    while(lt!=NULL && lt->ldat.val.buf!=NULL) {
        nn++;
        lt = lt->next;
    }
    if (nn==0) return NULL;

    mx = nn;
    buf = ret = (Buffer*)malloc(sizeof(Buffer)*mx);
    if (ret==NULL) return NULL;
    
    nn = 0;
    while(lp!=NULL && lp->ldat.val.buf!=NULL) {
        *buf = dup_Buffer(lp->ldat.val);
        buf->state = mx - nn;
        nn++;
        buf++;
        lp = lp->next;
    }
        
    return ret;
}


/**
Buffer*  awk_Buffer_dim(Buffer buf, char cc)

ccを区切り記号として, バッファ(buf.buf)内の項目を配列にして返す．

@param  buf  処理対象 Buffer型変数．
@param  cc   区切り文字．

@return ccで区切られた項目をbuf部に格納したBuffer型配列へのポインタ．
        各要素の stateにその要素以降の要素数（自分も含む）が格納される．
        すなわち，0番目の要素の stateには配列の大きさが格納される．

@see awk()
*/
Buffer*  awk_Buffer_dim(Buffer buf, char cc)
{
    Buffer* bf = NULL;

    tList*  lp = awk_Buffer_tList(buf, cc);
    if (lp!=NULL) bf = get_Buffer_dim_tList(lp);
    del_all_tList(&lp);
    
    return  bf;
}


/**
Buffer*  cawk_Buffer_dim(Buffer buf, char cc)

ccを区切り記号として, バッファ(buf.buf)内の項目を配列にして返す．
連続する cc(区切り)は一つの区切りとみなす．

@param  buf  処理対象 Buffer型変数．
@param  cc   区切り文字．

@return ccで区切られた項目をbuf部に格納したBuffer型配列へのポインタ．
        各要素の stateにその要素以降の要素数（自分も含む）が格納される．
        すなわち，0番目の要素の stateには配列の大きさが格納される．
    
@see cawk()
*/
Buffer*  cawk_Buffer_dim(Buffer buf, char cc)
{
    Buffer* bf = NULL;

    tList*  lp = cawk_Buffer_tList(buf, cc);
    if (lp!=NULL) bf = get_Buffer_dim_tList(lp);
    del_all_tList(&lp);
    
    return  bf;
}


/**
Buffer*  decompline_Buffer_dim(Buffer buf, int mode)

buf.buf を行に分解する．行の区切りは CRLF, CR, LF 

バイナリデータが混じっている場合は，バイナリデータと思われるデータ以降を一行として返す．
従って最後のデータはバイナリである可能性がある．

@param  buf   分解するデータの入っている Buffer変数
@param  mode  @b ON: 行末の CR, LFを削除しない．@b OFF: 行末の CR, LFを削除する．

@return 行へのポインタが格納された配列へのポインタ．各データの state に配列の大きさが入る．
        使い終わったら del_Buffer_dim()でメモリを解放する．
*/
Buffer*  decompline_Buffer_dim(Buffer buf, int mode)
{
    Buffer* dim;
    int i, n, m;

    if (buf.buf==NULL) return NULL;

    n = i = 0;
    while(buf.buf[i]!='\0' && i<buf.vldsz) {
        if (buf.buf[i]==CHAR_CR) {
            n++;
            if (i+1<buf.vldsz && buf.buf[i+1]==CHAR_LF) i++;
        }
        else if (buf.buf[i]==CHAR_LF) n++;
        else if (i+1<buf.vldsz  && buf.buf[i+1]=='\0') n++;
        else if (i+1==buf.vldsz || buf.buf[i+1]=='\0') n++;
        i++;
    }
    if (n==0) n = 1;            // buf.buf[0]=='\0' の場合

    dim = (Buffer*)malloc(sizeof(Buffer)*n);
    if (dim==NULL) return NULL;

    int nxt = 0;
    for (m=0; m<n-1; m++) {
        int cnt = 0;
        int fwd = 0;
        while (buf.buf[nxt+cnt]!=CHAR_CR && buf.buf[nxt+cnt]!=CHAR_LF && nxt+cnt<buf.vldsz)  cnt++;

        if (buf.buf[nxt+cnt]==CHAR_CR || buf.buf[nxt+cnt]==CHAR_LF) fwd++;
        if (buf.buf[nxt+cnt]==CHAR_CR && nxt+cnt+1<buf.vldsz && buf.buf[nxt+cnt+1]==CHAR_LF) fwd++;
        if (mode==ON) cnt += fwd;

        dim[m] = make_Buffer(cnt);
        for (i=0; i<cnt; i++) {
            dim[m].buf[i] = buf.buf[nxt+i];
        }
        dim[m].vldsz = cnt;
        dim[m].state = n;

        if (mode==ON) nxt += cnt;
        else          nxt += cnt + fwd;
    }

    // m == n-1  一番最後の領域
    dim[n-1] = make_Buffer(buf.vldsz-nxt);
    for (i=0; i<buf.vldsz-nxt; i++) {
        dim[n-1].buf[i] = buf.buf[nxt+i];
    }
    dim[n-1].vldsz = buf.vldsz - nxt;
    dim[n-1].state = n;

    return dim;
}


/**
Buffer  join_Buffer_dim(Buffer* dim, const char* deli)
    
Buffer配列の内容を繋げて一個のデータにする．
*/
Buffer  join_Buffer_dim(Buffer* dim, const char* deli)
{
    int    i, nn;
    Buffer buf;

    buf = init_Buffer();
    if (dim==NULL) return buf;

    buf = make_Buffer(LBUF);
    nn  = dim->state;

    for (i=0; i<nn; i++) {
        cat_Buffer(dim, &buf);
        if (deli!=NULL && i!=nn-1) cat_s2Buffer(deli, &buf);
        dim++;
    }

    return buf;
}


/**
void  del_Buffer_dim(Buffer** dim)

動的に確保された Bufferの配列を削除する．
*/
void  del_Buffer_dim(Buffer** dim)
{
    int  i, nn;
    Buffer* buf;

    if (dim==NULL || *dim==NULL) return;
    buf = *dim;
    
    nn = buf->state;
    if (nn<=0) free_Buffer(buf);

    for (i=0; i<nn; i++) {
        free_Buffer(buf);
        buf++;
    }

    free(*dim);
    *dim = NULL;
    return;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
//  リスト Buffer 
// 

/**
tList*  get_tList_line_Buffer(Buffer buf, int n)

dataのn行目を分解してリストに格納して返す．

@param buf  文字データが格納されたBuffer型変数
@param n    読み出す行数
@return     アイテムを格納したリストへのポインタ．失敗した場合は NULL
*/
tList*  get_tList_line_Buffer(Buffer buf, int n)
{
    if (buf.buf==NULL || n<=0) return NULL;

    Buffer line = get_line_Buffer(buf, n);
    if (line.buf==NULL) return NULL;

    char*  packline = pack_char((char*)line.buf, ' ');
    free_Buffer(&line);
    if (packline==NULL) return NULL;

    tList* list = awk_tList(packline, ' ');
    free(packline);

    return list;
}


/**
tList*  get_tList_seq_data_Buffer(Buffer buf, int* ptr)

@param buf  文字データが格納されたBuffer型変数
@param ptr  読み出開始場所へのポインタ．
@return     アイテムを格納したリストへのポインタ．失敗した場合は NULL
*/
tList*  get_tList_seq_data_Buffer(Buffer buf, int* ptr)
{
    if (buf.buf==NULL || *ptr<0) return NULL;
    if (*ptr>=buf.vldsz) return NULL;

    Buffer line = get_seq_data_Buffer(buf, ptr);
    if (line.buf==NULL) return NULL;

    char*  packline = pack_char((char*)line.buf, ' ');
    free_Buffer(&line);
    if (packline==NULL) return NULL;

    tList* list = awk_tList(packline, ' ');
    free(packline);

    return list;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
// Extend tList
//

/**
int  set_item_tList(tList* list, char* key, int no, char deli, int nm, char* value)

key をキーにした no番目のノードの値の中で，deliを区切りにした nm番目の項目（文字列）に
value を設定する．

@param  list   検索対象のヘッダ方向を格納したリスト
@param  key    ヘッダ種別．
@param  no     同じヘッダ種別が複数ある場合，何番目のノードかを指定する．１から数える．
               noが 0以下の場合は keyが一致する全てのノードに対して操作を行う．
@param  deli   ノード値（文字列）の区切り文字．
@param  nm     deli を区切り文字として何番目の項目か？ 1から数える．
@param  value  設定する文字列．

@return 設定を行ったノード数．
*/
int  set_item_tList(tList* list, char* key, int no, char deli, int nm, char* value)
{
    int    cn = 0;
    tList* lp;

    if (list==NULL || key==NULL || value==NULL) return JBXL_ARGS_ERROR;

    if (no>0) {
        lp = strncasecmp_tList(list, key, 0, no);
        if (lp!=NULL) {
            int rep = set_item_tList_node(lp, deli, nm, value);
            if (rep) cn = 1;
        }
    }   
    else {      // no<=0
        int nn = 1;
        cn = 0;
        lp = strncasecmp_tList(list, key, 0, nn);
        while (lp!=NULL) {
            int rep = set_item_tList_node(lp, deli, nm, value);
            if (rep) cn++;
            lp = strncasecmp_tList(list, key, 0, ++nn);
        }
    }

    return cn;
}


int  set_item_tList_node(tList* lp, char deli, int nm, char* value)
{
    int  rep = FALSE;
    char dl[2] = " ";
    Buffer* bp;

    if (lp==NULL || value==NULL) return FALSE;

    dl[0] = deli;

    bp = cawk_Buffer_dim(lp->ldat.val, deli);
    if (bp!=NULL && bp->state>=nm) {
        Buffer buf = make_Buffer_bystr(value);
        free_Buffer(&bp[nm-1]);
        bp[nm-1] = buf;

        free_Buffer(&lp->ldat.val);
        lp->ldat.val = join_Buffer_dim(bp, dl);
        rep = TRUE;
    }
    del_Buffer_dim(&bp);

    return rep;
}


/**
int  replace_item_tList(tList* list, char* key, int no, char deli, int nm, char* srcval, char* value)

key をキーにした no番目のノードの値の中で，deliを区切りにした nm番目の項目（文字列）の
srcval部分を valueで置き換える．

@param  list    検索対象のヘッダ情報を格納したリスト
@param  key     ヘッダ種別．
@param  no      同じヘッダ種別が複数ある場合，何番目のノードかを指定する．1から数える．
                noが 0以下の場合は keyが一致する全てのノードに対して操作を行う．
@param  deli    ノード値（文字列）の区切り文字．
@param  nm      deli を区切り文字として何番目の項目か？ 1から数える．
@param  srcval  置き換え対象の文字列．NULLなら指定した項目の文字列全体
@param  value   置き換える文字列．

@return 置き換えを行ったノード数．
*/
int  replace_item_tList(tList* list, char* key, int no, char deli, int nm, char* srcval, char* value)
{
    int    cn = 0;
    tList* lp;

    if (list==NULL || key==NULL || value==NULL) return JBXL_ARGS_ERROR;
    if (srcval==NULL) {
        return     set_item_tList(list, key, no, deli, nm, value);
    }

    if (no>0) {
        lp = strncasecmp_tList(list, key, 0, no);
        if (lp!=NULL) {
            int rep = replace_item_tList_node(lp, deli, nm, srcval, value);
            if (rep) cn = 1;
        }
    }   
    else {      // no<=0
        int nn = 1;
        cn = 0;
        lp = strncasecmp_tList(list, key, 0, nn);
        while (lp!=NULL) {
            int rep = replace_item_tList_node(lp, deli, nm, srcval, value);
            if (rep) cn++;
            lp = strncasecmp_tList(list, key, 0, ++nn);
        }
    }

    return cn;
}


int  replace_item_tList_node(tList* lp, char deli, int nm, char* srcval, char* value)
{
    int  rep = FALSE;
    char dl[2] = " ";
    Buffer* bp;
    Buffer  buf;

    if (lp==NULL || value==NULL) return FALSE;
    if (nm<=0) nm = 1;
    if (srcval==NULL) {
        return set_item_tList_node(lp, deli, nm, value);
    }

    dl[0] = deli;

    bp = cawk_Buffer_dim(lp->ldat.val, deli);
    if (bp!=NULL && bp->state>=nm) {
           buf = replace_sBuffer(bp[nm-1], srcval, value);
        free_Buffer(&bp[nm-1]);
        bp[nm-1] = buf;
        free_Buffer(&lp->ldat.val);
        lp->ldat.val = join_Buffer_dim(bp, dl);
        rep = TRUE;
    }
    del_Buffer_dim(&bp);

    return rep;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
// File/Directory Tools
//

/**
tList*  get_dir_files(const char* dirn)

指定されたディレクトリにあるファイル名の一覧を取得．@n
Windows の場合は 非 Unicode専用

@param  dirn  ディレクトリ名
@return ファイル名をバッファ部（ldat.val）に格納したリストへのポインタ．

@attention ファイルの種類は区別しない．
*/
tList*  get_dir_files(const char* dirn)
{
    tList* lp = NULL;
    tList* ln = NULL;

#ifdef WIN32
//    WIN32_FIND_DATA  FindFileData;
    WIN32_FIND_DATAA FindFileData;
    HANDLE hFind;

    Buffer pth = make_Buffer_bystr(dirn);
    if (pth.buf[strlen((char*)pth.buf)-1]!='\\') cat_s2Buffer("\\", &pth);
    Buffer buf = dup_Buffer(pth);
    cat_s2Buffer("*", &buf);

//    hFind = FindFirstFile((LPCTSTR)(buf.buf), &FindFileData);
    hFind = FindFirstFileA((LPCSTR)(buf.buf), &FindFileData);
    if (hFind!=INVALID_HANDLE_VALUE) {
        Buffer tmp;    
        do {
            if (strcmp(".", FindFileData.cFileName) && strcmp("..", FindFileData.cFileName)) { 
                tmp = dup_Buffer(pth);
                cat_s2Buffer(FindFileData.cFileName, &tmp);
                ln = add_tList_node_str(ln, NULL, tmp.buf);
                if (lp==NULL) lp = ln;
                free_Buffer(&tmp);
            }
        } while (FindNextFileA(hFind, &FindFileData));

        FindClose(hFind);
    }
    free_Buffer(&buf);
    free_Buffer(&pth);

#else

    DIR *dp;
    struct dirent *dir;
    
    Buffer pth = make_Buffer_bystr(dirn);
    if (pth.buf[strlen((char*)pth.buf)-1]!='/') cat_s2Buffer("/", &pth);

    dp = opendir(dirn);
    if (dp!=NULL ){
        Buffer tmp;
        dir = readdir(dp);
        while (dir != NULL ){
            if (strcmp(".", dir->d_name) && strcmp("..", dir->d_name)) { 
                tmp = dup_Buffer(pth);
                cat_s2Buffer(dir->d_name, &tmp);
                ln = add_tList_node_str(ln, NULL, (char*)tmp.buf);
                if (lp==NULL) lp = ln;
                free_Buffer(&tmp);
            }
            dir = readdir(dp);
        }

        closedir(dp);
    }
    free_Buffer(&pth);

#endif

    // sort
    ln = lp;
    while (ln!=NULL) {
        tList* lo = ln->next;
        while (lo!=NULL) {
            if (strcmp((char*)ln->ldat.val.buf, (char*)lo->ldat.val.buf)>0) {
                swap_tList_node(ln, lo);
            }
            lo = lo->next;
        }
        ln = ln->next;
    }

    return lp;
}


tList*  get_dir_files_rcsv(const char* dirn)
{
    tList* lp = get_dir_files(dirn);
    if (lp==NULL) return NULL;

    tList* lt = dup_tList(lp);
    while (lt!=NULL) {
        tList* lc = get_dir_files_rcsv((char*)lt->ldat.val.buf);
        if (lc!=NULL) add_tList_end(lp, lc);
        lt = lt->next;
    }
    del_tList(&lt);

    return lp;
}


void  del_file_extension_Buffer(Buffer* path)
{
    if (path==NULL) return;

    int count = path->vldsz - 1;
    while (count>=0) {
        if (path->buf[count]=='.') {
            path->buf[count] = '\0';
            path->vldsz = (int)strlen((char*)path->buf);
            break;
        }
        count--;
    }
}


/**
ファイルの拡張子を extにする．ファイルに拡張子が無い場合は extを付加する．
*/
void  change_file_extension_Buffer(Buffer* path, const char* ext)
{
    if (path==NULL) return;

    int count = path->vldsz - 1;
    while (count>=0) {
        if (path->buf[count]=='.') {
            path->buf[count] = '\0';
            path->vldsz = (int)strlen((char*)path->buf);
            if (ext[0]!='.') cat_s2Buffer(".", path);
            cat_s2Buffer(ext, path);
            break;
        }
        else if (path->buf[count]=='/' || path->buf[count]=='\\') {
            count = -1;
            break;
        }
        count--;
    }
    //
    if (count<0) {
        if (ext[0]!='.') cat_s2Buffer(".", path);
        cat_s2Buffer(ext, path);
    }
}


/*
Buffer  relative_path_Buffer(Buffer src, Buffer dst)

src から dst への相対パスを返す．

//// や \\\\\\ など，ディレクトリ区切り文字が連続した場合の処理は不正確になる．
片方が絶対パスでもう片方が相対パスの場合，結果は不正確になる．
*/
Buffer  relative_path_Buffer(Buffer src, Buffer dst)
{
    Buffer rlt = init_Buffer();
    if (src.buf==NULL || dst.buf==NULL) return rlt; 
    if (src.buf[0]=='\0') {
        rlt = dup_Buffer(dst);
        return rlt;
    }

    Buffer path_a = dup_Buffer(src);
    Buffer path_b = dup_Buffer(dst);
    //  区切り文字の追加
    if (path_a.buf[path_a.vldsz-1]!=CHAR_DELI_DIR) cat_s2Buffer(STR_DELI_DIR, &path_a);
    if (path_b.buf[path_b.vldsz-1]!=CHAR_DELI_DIR) cat_s2Buffer(STR_DELI_DIR, &path_b);

    int  i = 0;
    char a = path_a.buf[0];
    char b = path_b.buf[0];
    while (a==b && a!='\0') {
        i++;
        a = path_a.buf[i];
        b = path_b.buf[i];
    }   // i : pat_a と path_b の違っている箇所
    if (a==CHAR_DELI_DIR && i>0) a = path_a.buf[--i];
    //
    // 手前の区切り文字
    while (i>=0 && a!=CHAR_DELI_DIR) a = path_a.buf[--i];
    i++;    // 区切り文字の次

    int n = 0, h = i;
    char c = path_a.buf[h];
    while(c!='\0') {
        if (c==CHAR_DELI_DIR) n++;
        c = path_a.buf[++h];
    }
    if (h!=i) n++;

    if (n==0) {
        rlt = make_Buffer_str(STR_CRNT_DIR);
    }
    else {
        for (int j=0; j<n; j++) {
            cat_s2Buffer(STR_UPST_DIR, &rlt);
        }
        cat_s2Buffer(&path_b.buf[i], &rlt);
    }

    free_Buffer(&path_a);
    free_Buffer(&path_b);
    
    return rlt;
}


/**
void  canonical_filename_Buffer(Buffer* fname)

fname 中の問題になりそうな ASCII文字を '_' に変換する．
*/
void  canonical_filename_Buffer(Buffer* fname)
{
    if (fname==NULL || fname->buf==NULL) return;

    rewrite_Buffer_bychar(fname, ' ', '_');
    rewrite_Buffer_bychar(fname, '!', '_');
    rewrite_Buffer_bychar(fname, '&', '_');
    rewrite_Buffer_bychar(fname, '~', '_');
    rewrite_Buffer_bychar(fname, '/', '_');
    rewrite_Buffer_bychar(fname, '|', '_');
    rewrite_Buffer_bychar(fname, ':', '_');
    rewrite_Buffer_bychar(fname, ';', '_');
    rewrite_Buffer_bychar(fname, '*', '_');
    rewrite_Buffer_bychar(fname, '?', '_');
    rewrite_Buffer_bychar(fname, '`', '_');
    rewrite_Buffer_bychar(fname, '"', '_');
    rewrite_Buffer_bychar(fname, '\'', '_');
    rewrite_Buffer_bychar(fname, '\\', '_');
    rewrite_Buffer_bychar(fname, '@', '_');
    rewrite_Buffer_bychar(fname, '[', '_');
    rewrite_Buffer_bychar(fname, ']', '_');
    rewrite_Buffer_bychar(fname, '(', '_');
    rewrite_Buffer_bychar(fname, ')', '_');
    rewrite_Buffer_bychar(fname, '<', '_');
    rewrite_Buffer_bychar(fname, '>', '_');

    return;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// ファイルリスト

/**
tList*  add_resource_list(const char* path, int keylen, tList* list, tList* extn)

ディレクトリ pathを検索して，リソースリストにファイルを追加し，リストの先頭を返す．@n
リソースリストのキーは，リソースのファイル名の先頭 keylen文字とする．keylenが 0以下ならファイル名全体をキーとする．

@param path   検索するディレクトリ名
@param keylen キー長．0以下ならファイル名全体．
@param list   追加操作を行うリスト．NULLなら新しいリストそ作成して返す．
@param extn   除外拡張子のリスト．
@return リソースのリスト
*/
tList*  add_resource_list(const char* path, int keylen, tList* list, tList* extn)
{
    if (path==NULL) return list;

    tList* lp = get_dir_files(path);
    tList* pp = find_tList_end(list);

    // Generate Key. ファイル名の先頭 keylen文字をキーにする．
    while (lp!=NULL) {
        Buffer fn = make_Buffer_bystr(get_file_name((char*)lp->ldat.val.buf));
        char* ext = get_file_extension((char*)fn.buf);
        //
        if (extn==NULL || strncasecmp_tList(extn, ext, 0, 1)==NULL) {
            if (keylen<=0) {
                lp->ldat.key = make_Buffer_bystr((char*)fn.buf);
                pp = add_tList_node_bydata(pp, lp->ldat);
            }
            else if (fn.vldsz>=keylen) {
                fn.buf[keylen] = '\0';
                fn.vldsz = keylen;
                lp->ldat.key = make_Buffer_bystr((char*)fn.buf);
                pp = add_tList_node_bydata(pp, lp->ldat);
            }
        }

        if (list==NULL) list = pp;
        lp = lp->next;
        free_Buffer(&fn);
    }

    return list;
}


/**
char*  get_resource_path(char* name, tList* lp)

リソース名（ldat.key.buf）の値から，対象のパス値を得る．戻り値は freeしてはいけない．

@param name リソース名
@param lp   リソースリスト．key部に名前，val部にパスが格納されている．
@return リソース名に対応したパス．
*/
char*  get_resource_path(char* name, tList* lp)
{
    if (name==NULL) return NULL;

    while (lp!=NULL) {
        if (!strcasecmp((char*)lp->ldat.key.buf, name)) {
            return (char*)lp->ldat.val.buf;
        }
        lp = lp->next;
    }

    return NULL;
}

