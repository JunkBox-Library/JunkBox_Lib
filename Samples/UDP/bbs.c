
#include "network.h"
#include <time.h>
#include <signal.h>
#include <sys/time.h>

//#define LOGF  "/home/iseki/public_html/bbs.html"
#define LOGF  "/home/apache/htdocs/bbs.html"
#define SSNEND 4


void trans(int sofd, struct sockaddr_in* cl_addr, FILE* fp);


int main(int argc, char** argv)
{
    FILE* fp;
    int  i, err, nbyte, sport;
    int  sofd, nsofd, cofd, cdlen;
    char rmsg[RECVBUFSZ], fbuf[RECVBUFSZ];
    struct sockaddr_in  sv_addr, cl_addr;

    if (argc<2) {
        fprintf(stderr,"Usage... %s port\n",argv[0]);
        exit(0);
    }
    if (argc==2) sport = atoi(argv[1]);

    sofd = tcp_server_socket(sport);
    fp = fopen(LOGF, "aw");

    signal(SIGCHLD, SIG_IGN);
    while(1) {
        cdlen = sizeof(cl_addr);
        nsofd = accept(sofd, (struct sockaddr*)&cl_addr, &cdlen);
        if (nsofd<0) Error("accept");

        if (fork()==0) trans(nsofd, &cl_addr, fp);
        close(nsofd);
    }
    fclose(fp);

    socket_close(sofd);
}



void trans(int sofd, struct sockaddr_in* cl_addr, FILE* fp)
{
    int  i, cc, ef;
    int tflag=OFF;
    fd_set  mask;
    char msg[RECVBUFSZ];
    unsigned char* p;

    struct timeval timeout;
    time_t tsec;
    struct tm *tmx;

    time(&tsec);
    tmx = localtime(&tsec);
    tmx->tm_year += 1900;
    tmx->tm_mon  += 1; 
    p = (unsigned char*)&(cl_addr->sin_addr);

    fprintf(stderr, "connect from %d.%d.%d.%d\n", p[0], p[1], p[2], p[3]);

    FD_ZERO(&mask); 
    FD_SET(sofd, &mask);

    ef = 0;
    cc = 1;
    timeout.tv_sec  = 10;
    timeout.tv_usec = 0;

    select(sofd+1, &mask, NULL, NULL, &timeout); 
    if (FD_ISSET(sofd, &mask)) {
        fprintf(fp, "<font color=blue>"); 
        fprintf(fp, "%d.%d.%d.%d, %d/%d/%d %d:%d<br /></font>\n", p[0], p[1], p[2], p[3], 
                         tmx->tm_year, tmx->tm_mon, tmx->tm_mday, tmx->tm_hour, tmx->tm_min);
        fprintf(fp, "<pre>\n");
        tflag = ON;
    }

    while(ef<SSNEND && FD_ISSET(sofd, &mask) && cc>0){
        bzero(msg, RECVBUFSZ);
        cc = tcp_recv(sofd, msg, RECVBUFSZ-1);
        for (i=0; i<cc; i++) {
            if (msg[i]!='\0') {
                fprintf(stdout, "%c", msg[i]);
                fprintf(fp,     "%c", msg[i]);
                if (msg[i]==0x0d || msg[i]==0x0a) ef++;
                else                              ef = 0;
            }
        }
        if (ef>=SSNEND) break;

        FD_SET(sofd, &mask);
        select(sofd+1, &mask, NULL, NULL, &timeout); 
    }

    if (ef>=SSNEND) {
        tcp_send(sofd, "OK\n", 0);
    }
    else {
        tcp_send(sofd, "Connection time out!!\n", 0);
    }

    if (tflag==ON) {
        fprintf(fp, "</pre>\n<hr size=3 />\n");
    }

    fflush(fp);

    socket_close(sofd);
    exit(0);
}
 

