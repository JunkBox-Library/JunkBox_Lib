
/*  
  SSL Relay Program     ssl_relay v 1.0
*/




#include "tools.h"
#include "network.h"

#include "ssl_tool.h"


#define TIME_OUT 10


void ssl_relay(int sofd, char* hostname, int cport);


static int debugMode = OFF;


int main(int argc, char** argv)
{
    int  i, err, nbyte, sport, cport;
    int  sofd, nsofd, cofd, cdlen;
    char *p, hostname[LNAME], portno[7];
    char rmsg[BUFSZ], fbuf[BUFSZ];
    struct sockaddr_in  sv_addr, cl_addr;

    if (argc<2) {
        fprintf(stderr,"Usage... %s host_name[:port] [port] [-d]\n",argv[0]);
        exit(0);
    }

    p = argv[1];
    i = 0;
    while(*p!='\0' && *p!=':' && i<LNAME-1) hostname[i++] = *(p++); 
    hostname[i] = '\0';
    if (*p=='\0') cport = 0;
    else          cport = atoi(++p);

    if (argc>=3) {
        sport = atoi(argv[2]);
        if (argc==4) {
            if (!strcmp("-d", argv[3])) debugMode = ON;
        }
    }
    else sport = cport;


    signal(SIGCLD, SIG_IGN);

    sofd  = tcp_server_socket(sport);
    while(1) {
        cdlen = sizeof(cl_addr);
        nsofd = accept(sofd, (struct sockaddr*)&cl_addr, (socklen_t*)&cdlen);
        if (nsofd<0) Error("accept");

        if (fork()==0) {
            ssl_relay(nsofd, hostname, cport);
            exit(0);
        }
        close(nsofd);
    }

    socket_close(sofd);
}



void  ssl_relay(int sofd, char* hostname, int cport)
{
    int  i, nd, nm, cc, cofd;
    fd_set  mask;
    char msg[BUFSZ];
    struct timeval timeout;
    
    SSL* ssl;

    ssl_init();
    SSL_CTX* ctx = ssl_server_setup("cert.pem", "skey.pem", NULL);
    ssl = ssl_server_socket(sofd, ctx);
    if (ssl==NULL) {
        socket_close(sofd);
        return;
    }
    cc = SSL_accept(ssl);
    if (cc!=1) {
        ssl_close(ssl);
        socket_close(sofd);
        return;
    }

    cofd = tcp_client_socket(hostname, cport);

    nm = Max(sofd, cofd);

    while(1) {
        do {
            timeout.tv_sec  = TIME_OUT;
            timeout.tv_usec = 0;
            FD_ZERO(&mask); 
            FD_SET(sofd, &mask);
            FD_SET(cofd, &mask);
            nd = select(nm+1, &mask, NULL, NULL, &timeout);
        } while (nd<0);

        if (FD_ISSET(cofd, &mask)) {
            cc = tcp_recv(cofd, msg, BUFSZ);
            if (cc>0) {
                cc = ssl_send(ssl, msg, cc);
                if (cc<0) break;
            }
            else break;
        }

        if (FD_ISSET(sofd, &mask)) {
            cc = ssl_recv(ssl, msg, BUFSZ);
            if (cc>0) {
                cc = tcp_send(cofd, msg, cc);
                if (cc<0) break;
            }
            else break;
        }
    }

    SSL_CTX_free(ctx);
    ssl_close(ssl);
    socket_close(cofd);
    socket_close(sofd);

    return;
}
 





