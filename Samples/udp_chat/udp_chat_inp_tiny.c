
#include "network.h"

#define  BUFL  1024


int main()
{
    int  sock, cc;
    char buf[BUFL];
    struct addrinfo* sv_addr;
 
    sock = udp_client_socket("202.26.159.135", 9000, &sv_addr);
    if (sock<0) Error("udp_client_socket");
 
    fgets(buf, BUFL, stdin);
    while (buf[0]!='\n') {
        cc = udp_send(sock, buf, 0, sv_addr);
        fgets(buf, BUFL, stdin);
    }

    freeaddrinfo(sv_addr);
    socket_close(sock);
}


