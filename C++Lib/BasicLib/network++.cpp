
/**
@brief    ネットワーク用C++ライブラリ
@file     network++.cpp
@author   Fumi.Iseki (C)
*/


#include  "network++.h"


using namespace jbxl;



UDP::UDP(char* host, int port)
{
    struct hostent* shost;

    init();
    clear();
    //
    _socket = (int)socket(AF_INET, SOCK_DGRAM, 0);
    if (_socket<0) {
        errNum  = -1;
        errFlag = ON;
        return;
    }

    shost = gethostbyname(host);
    if (shost==NULL) {
        close();
        errNum  = -2;
        errFlag = ON;
        return;
    }

    //memset(&_sv_addr, 0, sizeof(_sv_addr));
    _sv_addr.sin_family = AF_INET;
    _sv_addr.sin_port   = htons(port);
    memcpy(&(_sv_addr.sin_addr), shost->h_addr, shost->h_length);

    return;
}


UDP::UDP(int port)
{
    init();
    clear();
    //
    _socket = (int)socket(AF_INET, SOCK_DGRAM, 0);
    if (_socket<0) {
        errNum  = -1;
        errFlag = ON;
        return;
    }

    _sv_addr.sin_family = AF_INET;
    _sv_addr.sin_port   = htons(port);
    _sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int err = bind(_socket, (struct sockaddr*)&_sv_addr, sizeof(_sv_addr));
    if (err<0) {
        close();
        errNum  = -3;
        errFlag = ON;
    }
    return;
}


void  UDP::init(void)
{
    _socket = -1;
    memset(&_sv_addr, 0, sizeof(_sv_addr)); 
    errNum  = 0;
    errFlag = OFF;
}


void  UDP::clear(void)
{
    recvSize = 0;
    memset(recvBuf, 0, BUFSZ); 
}


void  UDP::close(void)
{
    if (_socket>0) socket_close(_socket);
    clear();
    init();
}


int   UDP::getSocket()
{
    return _socket;
}


int  UDP::recvData(void)
{
    clear();
    //
    int sz = udp_recv_sockaddr_in(_socket, recvBuf, BUFSZ-1, &_sv_addr);
    if (sz<0) {
        clear();
        errNum  = sz;
        errFlag = ON;
    }
    else {
        recvSize = sz;
    }
    return sz;
}


int  UDP::sendData(char* data, int size)
{
    int sz = udp_send_sockaddr_in(_socket, data, size, &_sv_addr);
    if (sz<0) {
        errNum  = sz;
        errFlag = ON;
    }
    return sz;
}



//////////////////////////////////////////////////////////////////////////////
//

TCP_Socket::TCP_Socket(char* host, int port)
{
    struct hostent      *shost;
    struct sockaddr_in  sv_addr;

    init();
    clear();
    //
    _socket = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (_socket<0) {
        errNum  = -1;
        errFlag = ON;
        return;
    }

    shost = gethostbyname(host);
    if (shost==NULL) {
        socket_close(_socket);
        init();
        errNum  = -2;
        errFlag = ON;
        return;
    }

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    memcpy(&sv_addr.sin_addr, shost->h_addr, shost->h_length);

    int err = connect(_socket, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(_socket);
        init();
        errNum  = -3;
        errFlag = ON;
        return;
    }
}


TCP_Socket::TCP_Socket(int socket)
{
    init();
    clear();
    //
    _socket = socket;
}


void  TCP_Socket::init(void)
{
    errNum  = 0;
    errFlag = OFF;
}


void  TCP_Socket::clear(void)
{
    recvSize = 0;
    memset(recvBuf, 0, BUFSZ); 
}


void  TCP_Socket::close(void)
{
    if (_socket>0) socket_close(_socket);
    clear();
    init();
}


int   TCP_Socket::getSocket()
{
    return _socket;
}


int   TCP_Socket::sendData(char* smsg, int size)
{
    int cc;

    if (size<=0) size = (int)strlen(smsg);
    cc = send(_socket, smsg, size, 0);
    if (cc<0) {
        errNum  = cc;
        errFlag = ON;
    }

    return cc;
}


int   TCP_Socket::recvData(void)
{
    int cc;

    clear();

    cc = recv(_socket, recvBuf, BUFSZ-1, 0);
    if (cc<0) {
        clear();
        errNum  = cc;
        errFlag = ON;
    }
    else {
        recvSize = cc;
    }

    return cc;
}


int   TCP_Socket::recvData(int timeout)
{
    int cc;

    clear();

    if (recv_wait(_socket, timeout)) {
        cc = recv(_socket, recvBuf, BUFSZ-1, 0);
        if (cc<0) {
            clear();
            errNum  = cc;
            errFlag = ON;
        }
        else {
            recvSize = cc;
        }
    }
    else {
        return JBXL_NET_RECV_TIMEOUT;
    }

    return cc;
}



//////////////////////////////////////////////////////////////////////////////
//

TCP_Server::TCP_Server(int port)
{
    struct sockaddr_in sv_addr;

    init();
    clear();
    //
    _socket = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (_socket<0) {
        errNum  = -1;
        errFlag = ON;
        return;
    }

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port   = htons(port);
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int err = bind(_socket, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (err<0) {
        socket_close(_socket);
        init();
        errNum  = -3;
        errFlag = ON;
        return;
    }

    err = listen(_socket, 10);
    if (err==-1) {
        socket_close(_socket);
        init();
        errNum  = -4;
        errFlag = ON;
        return;
    }

    return;
}


void  TCP_Server::init(void)
{
    _socket = -1;
    errNum  = 0;
    errFlag = OFF;
}


void  TCP_Server::clear(void)
{
}


void  TCP_Server::close(void)
{
    if (_socket>0) socket_close(_socket);
    clear();
    init();
}


int   TCP_Server::getSocket()
{
    return _socket;
}


TCP_Socket*  TCP_Server::accept(void)
{
     struct sockaddr_in sv_addr;
     int cdlen = sizeof(sv_addr);
     int sofd  = accept_intr(_socket, (struct sockaddr*)&sv_addr, (socklen_t*)&cdlen);

     TCP_Socket* tcp = new TCP_Socket(sofd);
     return tcp;
}


