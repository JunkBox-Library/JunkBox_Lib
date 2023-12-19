
#include "network.h"

#define  BUFMAX 2048

int main()
{
    int  i, sock, cc;
    char rmsg[BUFMAX];
    char smsg[BUFMAX];
    char* cryptpr;

    struct addrinfo* cl_addr;
 
    sock = udp_server_socket(9000, &cl_addr);
    if (sock<0) Error("udp_server_socket");
 
    Loop{
        memset(rmsg, 0, BUFMAX);
        cc = udp_recv(sock, rmsg, BUFMAX, cl_addr);

        for (i=0; i<cc; i++) smsg[i] = rmsg[i] - 'A' + 'a';

        smsg[cc] = '\0';
        strcat(rmsg, " -> ");
        strcat(rmsg, smsg);
        strcat(rmsg, "\r\n");

        udp_send(sock, rmsg, 0, cl_addr);
        printf("%s",rmsg);
    }
}


