
#include "network.h"

int main(int argc, char** argv)
{
    struct sockaddr_in addr;
    int  port, sock, sofd, cdlen;

    if (argc<2) {
        print_message("Usage..... %s port_num\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);
    sock = tcp_server_socket(port);
    if (sock<0) {
        print_message("socket open errror!!   port = %d\n", port);
        exit(1);
    }

    Loop {      // while(1)
        cdlen = sizeof(addr);
        sofd = accept_intr(sock, (struct sockaddr*)&addr, (socklen_t*)&cdlen);
        if (sofd>0) {       
            tcp_send_mesg(sofd, "\nWelcome Hell World!!\n\n");
            close(sofd);
        }
    }
}
