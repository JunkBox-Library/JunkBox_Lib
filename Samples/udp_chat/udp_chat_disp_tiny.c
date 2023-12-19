
#include "network.h"

#define  MLEN  128


int main()
{
    int  sock, cc;
    char rmsg[MLEN];
    struct sockaddr_in sv_addr;
 
    sock = udp_server_socket(9100, NULL);
    if (sock<0) Error("udp_server_socket");
 
    while (1) {
        memset(rmsg, 0, MLEN);
        cc = udp_recv_sockaddr_in(sock, rmsg, MLEN, &sv_addr);
        printf("%s", rmsg);
    }

    socket_close(sock);
}


