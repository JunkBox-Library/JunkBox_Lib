
#include "network.h"

#define  BUFL  1024
#define  MLEN  128


int main()
{
    int  sock, cc;
    char*  ex;
    char buf[BUFL];
    char rmsg[MLEN];

    struct addrinfo* sv_addr;
 
    sock = udp_client_socket("127.0.0.1", 8001, &sv_addr);
    if (sock<0) Error("udp_client_socket");
 
    ex = fgets(buf, BUFL, stdin);
    while (buf[0]!='\n') {
        memset(rmsg, 0, MLEN);
        cc = udp_send(sock, buf, 0, sv_addr);
        cc = udp_recv(sock, rmsg, MLEN, sv_addr);
        printf("%s", rmsg);
        ex = fgets(buf, BUFL, stdin);
    }

    freeaddrinfo(sv_addr);
/*
    struct sockaddr_in sv_addr;
 
    sock = udp_client_socket_in("202.26.159.210", 8001, &sv_addr);
    if (sock<0) Error("udp_client_socket");
 
    ex = fgets(buf, BUFL, stdin);

    while (buf[0]!='\n') {
        memset(rmsg, 0, MLEN);
        cc = udp_send_in(sock, buf, 0, &sv_addr);
        cc = udp_recv_in(sock, rmsg, MLEN, &sv_addr);
        printf("%s", rmsg);
        ex = fgets(buf, BUFL, stdin);
    }
*/

    socket_close(sock);
}


