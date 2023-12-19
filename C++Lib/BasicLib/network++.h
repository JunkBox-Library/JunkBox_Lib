
#ifndef  __JBXL_CPP_NETWORK_H_
#define  __JBXL_CPP_NETWORK_H_


/**
@brief    ネットワーク用C++ライブラリ
@file     network++.h
@author   Fumi.Iseki (C)
*/


#include "tools++.h"
#include "network.h"


namespace jbxl {


///////////////////////////////////////////////////////////////////
// UDP
// 

class  UDP
{
public:
    UDP(void) { init();}
    UDP(char* host, int port);
    UDP(int port); 
    virtual ~UDP(void)  { free();}

private:
    int  _socket;
    struct sockaddr_in  _sv_addr;

public:
    int   errNum;
    int   errFlag;

    int   recvSize;
    char  recvBuf[BUFSZ];

private:
    void  init(void);
    void  clear(void);
    void  free(void) { close();}

public:
    int   getSocket(void);

    int   sendData(char* data, int size);
    int   sendMesg(char* data) { return sendData(data, strlen(data));}
    int   recvData(void);
    int   recvMesg(void) { return recvData();}

    void  close(void);
};


///////////////////////////////////////////////////////////////////
// TCP
// 

class  TCP_Socket
{
public:
    TCP_Socket(void) { init();}
    TCP_Socket(char* host, int port);
    TCP_Socket(int socket);
    virtual ~TCP_Socket(void)  { free();}

private:
    int  _socket;
//    struct hostent      *_shost;
//    struct sockaddr_in  _sv_addr;

public:
    int   errNum;
    int   errFlag;

    int   recvSize;
    char  recvBuf[BUFSZ];

private:
    void  init(void);
    void  clear(void);
    void  free(void) { close();}

public:
    int   getSocket(void);

    int   sendData(char* data, int size);
    int   sendMesg(char* data) { return sendData(data, strlen(data));}

    int   recvData(void);
    int   recvMesg(void) { return recvData();}
    int   recvData(int timeout);
    int   recvMesg(int timeout) { return recvData(timeout);}
    void  close(void);
};


class  TCP_Server
{
public:
    TCP_Server(void) { init();}
    TCP_Server(int port); 
    virtual ~TCP_Server(void)  { free();}

private:
    int  _socket;
//    struct sockaddr_in  _sv_addr;

public:
    int   errNum;
    int   errFlag;

private:
    void  init(void);
    void  clear(void);
    void  free(void) { close();}

public:
    TCP_Socket*  accept(void);

    int   getSocket(void);
    void  close(void);
};


}        // namespace


#endif   // __JBXL_CPP_NETWORK_H_
