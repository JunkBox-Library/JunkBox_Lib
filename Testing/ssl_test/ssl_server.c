/*  
    SSL Server TEST Program     ssl_server v 1.0
*/



#include "tools.h"
#include "network.h"

#define ENABLE_SSL

#include "ssl_tool.h"


#define TIME_OUT 10


void ssl_server(int sport);


static int debugMode = ON;


int main(int argc, char** argv)
{
    int   sport;
    char* p;

    if (argc<2) {
        fprintf(stderr,"Usage... %s port\n",argv[0]);
        exit(0);
    }
    sport = atoi(argv[1]);

    init_rand();
    ssl_server(sport);
    
    return 0;
}



void ssl_server(int sport)
{
    int  err, sock, cofd, cdlen;
    SSL* ssl;
    char request[BUFSZ];
    long ret;
    struct sockaddr_in addr;
    tList* lp;

    sock = tcp_server_socket(sport);
    if (sock<0) {
        printf("TCPソケットの作成失敗\n");
        exit(1);
    }

    cdlen = sizeof(addr);
    cofd = accept_intr(sock, (struct sockaddr*)&addr, (socklen_t*)&cdlen);

    ssl_init();
    SSL_CTX* ctx = ssl_server_setup("crt.pem", "key.pem", NULL);
    ssl = ssl_server_socket(cofd, ctx);
    if (ssl==NULL) {
        close(cofd);
        printf("SSLソケットの作成失敗\n");
        exit(1);
    }

    /* ssl_server_socket() 内で実行
    err = SSL_accept(ssl);
    if (err!=1) {
        ERR_print_errors_fp(stderr);
        SSL_get_error(ssl, ret);
        printf("SSL_accept error [%d]\n", err);
        exit(1);
    }
    */

    printf("\n");
    lp = ssl_get_cert_info(ssl); 
    if (lp!=NULL) print_tList(stdout, lp);

    printf("クライアントリクエスト\n-------------------------------\n");
    while (1){
        char buf[BUFSZ];
        int  sz;

        sz = SSL_read(ssl, buf, sizeof(buf)-1);
        if (sz>0 && sz<BUFSZ) {
            buf[sz] = '\0';
            write(1, buf, sz);
        }
        else {
            break;
        }
    }
    
    close(cofd);
    ssl_close(ssl); 

    return;
}

 





