
#include "network.h"


#define  MLEN  12


int main()
{
    int  sock, cc, no;
    unsigned char* p;
    char rmsg[MLEN];

    char smsg[] = "おめでとう．転送成功です．\n";
    struct addrinfo* cl_addr;
    
    sock = udp_server_socket(8000, &cl_addr);
    if (sock<0) Error("udp_server_socket");
 
    Loop{
        memset(rmsg, 0, MLEN);
        cc = udp_recv(sock, rmsg, 4, cl_addr);
        fprintf(stderr, "RECV = %04x %04x %04x %04x\n", rmsg[0], rmsg[1], rmsg[2], rmsg[3]);
        fflush(stderr);

        if (!strncmp("OK\r\n", rmsg, 4)) {
            fprintf(stderr, "connect!!\n");
            fflush(stderr); 
            udp_send(sock, smsg, 0, cl_addr);
            udp_send(sock, "\n", 0, cl_addr);
        }
    }
}


