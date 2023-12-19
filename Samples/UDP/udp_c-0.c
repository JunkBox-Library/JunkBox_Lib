
#include "network.h"

#define MLEN 128


int main()
{
    int  sock, cc;
    char rmsg[MLEN];
    char smsg[]="OK\r\n";

    struct addrinfo* sv_addr;
 
    sock = udp_client_socket("127.0.0.1", 8000, &sv_addr);
    if (sock<0) Error("udp_client_socket");
 
    memset(rmsg, 0, MLEN);

    cc = udp_send(sock, smsg, 0, sv_addr);
    if (cc<0) Error("udp_send");

    cc = udp_recv(sock, rmsg, MLEN, sv_addr);
    while (rmsg[0]!='\n') {
        printf("%s", rmsg);
        cc = udp_recv(sock, rmsg, MLEN, sv_addr);
    }

    freeaddrinfo(sv_addr);


/*
    struct sockaddr_in sv_addr;
 
    sock = udp_client_socket_in("202.26.159.210", 8000, &sv_addr);
    if (sock<0) Error("udp_client_socket");
 
    memset(rmsg, 0, MLEN);

    cc = udp_send_in(sock, smsg, 0, &sv_addr);
    if (cc<0) Error("udp_send");

    cc = udp_recv_in(sock, rmsg, MLEN, &sv_addr);
    while (rmsg[0]!='\n') {
        printf("%s", rmsg);
        cc = udp_recv_in(sock, rmsg, MLEN, &sv_addr);
    }
*/

    socket_close(sock);
}


