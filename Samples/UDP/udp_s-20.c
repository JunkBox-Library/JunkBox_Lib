/*
   cc  XXXX.c -o XXXX -lcrypt

*/



#include "network.h"
#include <crypt.h>

#define  BUFMAX 2048


int main()
{
    int  sock, cc;
    char rmsg[BUFMAX];
    char smsg[BUFMAX];
    char key[3];
    char* cryptpr;

    //struct sockaddr_in cl_addr;
    struct addrinfo* cl_addr;
 
    sock = udp_server_socket(9000, &cl_addr);
    if (sock<0) Error("udp_server_socket");
 
    Loop{
        memset(rmsg, 0, BUFMAX);
        cc = udp_recv(sock, rmsg, BUFMAX, cl_addr);

        if (cc>2) {
            key[0] = rmsg[0];
            key[1] = rmsg[1];
            key[2] = '\0';
            cryptpr = crypt(rmsg+2, key);
            strcat(rmsg, " -> ");
            strcat(rmsg, cryptpr);
        }

        udp_send(sock, rmsg, 0, cl_addr);
        udp_send(sock, "\r\n", 0, cl_addr);

        printf("%s\r\n",rmsg);
    }
}


