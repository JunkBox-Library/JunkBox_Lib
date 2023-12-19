/*  
    SSL Client TEST Program     ssl_client v 1.0
*/




#include "tools.h"
#include "network.h"

#define  ENABLE_SSL
#include "ssl_tool.h"



#define TIME_OUT 10


void ssl_client(char* hostname, int sport);


static int debugMode = ON;


int main(int argc, char** argv)
{
    int  i, sport;
    char *p, hostname[LNAME];

    if (argc<2) {
        fprintf(stderr,"Usage... %s host_name[:port]\n",argv[0]);
        exit(0);
    }

    p = argv[1];
    i = 0;
    while(*p!='\0' && *p!=':' && i<LNAME-1) hostname[i++] = *(p++); 
    hostname[i] = '\0';
    if (*p=='\0') sport = 80;
    else          sport = atoi(++p);

    init_rand();
    ssl_client(hostname, sport);
}



void ssl_client(char* hostname, int sport)
{
    int  err, cofd;
    SSL* ssl;
    char request[BUFSZ];
    long ret;
    tList* lp;

    print_message("connect to %s:%d\n", hostname, sport);

    cofd = tcp_client_socket(hostname, sport);

    ssl_init();
    SSL_CTX* ctx = ssl_client_setup(NULL);
    ssl = ssl_client_connect(cofd, ctx, OFF);
    //ssl  = ssl_client_connect(cofd, NULL, OFF);
    if (ssl==NULL) {
        close(cofd);
        printf("接続失敗\n");
        exit(1);
    }
        
    printf("\n");
    //lp = ssl_get_cert_info(ssl); 
    //print_tList(stdout, lp);

/*
    printf("\n");
    ret = SSL_get_verify_result(ssl);
    if (ret==X509_V_OK){
        printf("認証成功．\n");
    }
    else {
        printf("認証失敗．\n");
    }
    printf("\n");
*/


    sprintf(request, "GET / HTTP/1.1\r\n\r\n");
    err = SSL_write(ssl, request, strlen(request));

    printf("サーバレスポンス\n-------------------------------\n");
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

 

