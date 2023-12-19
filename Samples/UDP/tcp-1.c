#include "network.h"

int main(int argc, char** argv)
{
    int  sock, cc;
    char buf[1024];

    if (argc<2) {
        fprintf(stderr, "Usage.....  %s URL\n", argv[0]);
        exit(1);
    }

    sock = tcp_client_socket(argv[1], 80);
    if (sock<=0) {
        fprintf(stderr, "Can not connect to %s\n", argv[1]);
        exit(1);
    }

    tcp_send_mesg(sock, "GET / HTTP/1.0\r\n\r\n");

    cc = tcp_recv_wait(sock, buf, 1024, 10);
    while(cc>0) {
        printf("%s", buf);
        cc = tcp_recv_wait(sock, buf, 1024, 10);
    }
    close(sock);
    return 0;
}
