
#include "network.h"

#include <time.h>
#include <signal.h>
#include <sys/time.h>

void trans(int sofd, struct sockaddr_in* cl_addr);


int main(int argc, char** argv)
{
    int  sport, sofd, nsofd, cdlen;
    struct sockaddr_in  sv_addr, cl_addr;

    if (argc<2) {
    fprintf(stderr,"Usage... %s port\n",argv[0]);
        exit(0);
    }
    if (argc==2) sport = atoi(argv[1]);
    signal(SIGCLD, SIG_IGN);

    sofd = tcp_server_socket(sport);
    Loop {
        cdlen = sizeof(cl_addr);
        nsofd = accept(sofd,  (struct sockaddr*)&cl_addr, &cdlen);
        if (nsofd<0) Error("accept");
        if (fork()==0) trans(nsofd, &cl_addr);
        close(nsofd);
    }

    socket_close(sofd);
}



void trans(int sofd, struct sockaddr_in* cl_addr)
{
    char msg[RECVBUFSZ];

    int  i, cc, ef;
    fd_set  mask;
    struct timeval timeout;

    time_t tsec;
    struct tm *tmx;

    ef = OFF;
    cc = 1;
    timeout.tv_sec = 5;
    timeout.tv_usec = 500000;

    FD_ZERO(&mask); 
    FD_SET(sofd, &mask);
    select(sofd+1, &mask, NULL, NULL, &timeout); 
    while(ef==OFF && FD_ISSET(sofd, &mask) && cc>0){
        bzero(msg, RECVBUFSZ);
        cc = tcp_recv(sofd, msg, RECVBUFSZ-1);
        for (i=0; i<cc; i++) {
            if (msg[i]!='\0') {
                fprintf(stdout,"%c",msg[i]);
                if (msg[i]==0x0a) {
                    ef = ON; 
                    break;
                }
            }
        }
        if (ef==ON) break;
        FD_SET(sofd, &mask);
        select(sofd+1, &mask, NULL, NULL, &timeout); 
    }

    time(&tsec);
    tmx = localtime(&tsec);
    tmx->tm_year += 1900;
    tmx->tm_mon  += 1; 

    sprintf(msg, "Hello %d/%d/%d %d:%d\n", tmx->tm_year,tmx->tm_mon,tmx->tm_mday,tmx->tm_hour,tmx->tm_min);
    cc = tcp_send(sofd, msg, 0);

    close(sofd);
    exit(0);
}
 

 
