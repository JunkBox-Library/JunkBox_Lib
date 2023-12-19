
#include "network.h"
#include <signal.h>
#include <sys/time.h>


void  trans(int, FILE*);


int main(int argc, char** argv)
{
    int  sport, sofd, nsofd,  cdlen;
    FILE* fp;
    struct sockaddr_in  cl_addr;

    if (argc<2) {
        fprintf(stderr,"Usage... %s port\n",argv[0]);
        exit(0);
    }

    if (argc==2) sport = atoi(argv[1]);

    sofd  = tcp_server_socket(sport);
    fp = fopen("tcp_server.log","aw");

    signal(SIGCLD, SIG_IGN);
    Loop {
        cdlen = sizeof(cl_addr);
        nsofd = accept(sofd, (struct sockaddr*)&cl_addr, &cdlen);
        if (fork()==0) trans(nsofd, fp);
        close(nsofd);
    }
    fclose(fp);

    socket_close(sofd);
}



void trans(int sofd, FILE* fp)
{
    int  i, cc, ef;
    fd_set  mask;
    char msg[RECVBUFSZ];
    struct timeval timeout;

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
                fprintf(fp,"%c",msg[i]);
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
    fflush(fp);

    if (ef==ON) tcp_send(sofd, "OK\n", 4);
    else        tcp_send(sofd, "Connection time out!!\n", 0);

    close(sofd);
    exit(0);
}
 

 
