
#include "network.h"

#define MLEN 128


int main(int argc, char** argv)
{
    int  sock, cc;
    char rmsg[MLEN];
    char smsg[]="OK\r\n";
    //struct sockaddr_in sv_addr;
    struct addrinfo* sv_addr;

    if (argc<3) {
        fprintf(stderr, "Usage.... %s ip port\n",argv[0]);
        exit(1);
    }

    char* ip = argv[1];
    int port = atoi(argv[2]);
 
    sock = udp_client_socket(ip, port, &sv_addr);
    if (sock<0) Error("udp_client_socket");
 
    memset(rmsg, 0, MLEN);

    cc = udp_send(sock, smsg, 0, sv_addr);
    if (cc<0) Error("udp_send");

    cc = udp_recv(sock, rmsg, MLEN, sv_addr);
    while (rmsg[0]!='\n' && rmsg[0]!='\r') {
        printf("%s", rmsg);
        cc = udp_recv(sock, rmsg, MLEN, sv_addr);
    }
    printf("%s", rmsg);

    freeaddrinfo(sv_addr);
    socket_close(sock);
}


