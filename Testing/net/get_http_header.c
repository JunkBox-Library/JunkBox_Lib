
#include "network.h"
#include "tlist.h"
#include "http_tool.h"


int main(int argc, char** argv)
{
    int  sock, cc, len;
    char buf[1024];
    tList* lp = NULL;

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

    cc = recv_http_header(sock, &lp, &len, 0, NULL, NULL);
    while(cc>0) {
        memset(buf, 0, 1024);
        cc = tcp_recv_wait(sock, buf, 1024, 10);
        if (cc>0) printf("%s", buf);
    }


    printf("\n-------------------------------\n");
    print_tList(stdout, lp);

    close(sock);
}

