
#include "network.h"


#define  MLEN  12


int main()
{
    int  sock, cc, no;
    unsigned char* p;
    unsigned char rmsg[MLEN];

    char smsg[] = "おめでとう．転送成功です．\n";
    struct addrinfo* cl_addr;
    
    sock = udp_server_socket(8001, &cl_addr);
    if (sock<0) Error("udp_server_socket");

    while (1) {
        memset(rmsg, 0, MLEN);
        cc = udp_recv(sock, rmsg, 8, cl_addr);
        fprintf(stdout, "RECV = %c%c%c%c%c%c %02x %02x <br/>\n", rmsg[0],rmsg[1],rmsg[2],rmsg[3],rmsg[4],rmsg[5],rmsg[6],rmsg[7]);
        fflush(stdout);

        if (!strncmp("\r\n", rmsg+6, 2)) {
            fprintf(stderr, "connect!!\n");
            fflush(stderr); 
            udp_send(sock, smsg, 0, cl_addr);
            udp_send(sock, "\n", 0, cl_addr);
        }
        else {
            fprintf(stderr, "ng!!\n");
            fflush(stderr); 
            udp_send(sock, "\n", 0, cl_addr);
        }
    }

    freeaddrinfo(cl_addr);
}


