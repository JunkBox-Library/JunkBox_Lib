/*
  簡易UDP_CHAT 表示プログラム
*/


#include "tools.h"
#include "network.h"

int main(int argc, char** argv)
{
    int  cc, sock, port;
    char rmsg[RECVBUFSZ]; 
    struct sockaddr_in cl_addr;
    
    if (argc<=1) {
        fprintf(stderr, "Usage..... %s  port_no \n", argv[0]);
        exit(0);
    }   

    port = atoi(argv[1]);
    sock = udp_server_socket(port, NULL);
    if (sock<0) Error("udp_server_socket");
    fprintf(stderr,"簡易UDP_CHATシステム：表示クライアント．　%d 番ポートで待ち受けます\n\n", port);
    fflush(stderr);
 
    Loop{
        memset(rmsg, 0, RECVBUFSZ);
        cc = udp_recv_sockaddr_in(sock, rmsg, RECVBUFSZ-1, &cl_addr);
        fprintf(stdout, "%s", rmsg);
        fflush(stdout);
    }

    return 0;
}


