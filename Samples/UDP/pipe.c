/*  
  pipe v 1.2
*/


#include "tools.h"
#include "network.h"

#define TIME_OUT 10


int main(int argc, char** argv)
{
    int  i, err, nbyte, sport, cport;
    int  sofd, nsofd, cofd, cdlen;
    char *p, hostname[128], portno[7];
    char rmsg[BUFSZ], fbuf[BUFSZ];
    struct sockaddr_in  sv_addr, cl_addr;

    if (argc<2) {
        fprintf(stderr,"Usage... %s host_name[:port] port\n",argv[0]);
        exit(0);
    }

    p = argv[1];
    i = 0;
    while(*p!='\0' && *p!=':') hostname[i++] = *(p++); 
    hostname[i] = '\0';
    if (*p=='\0') cport = 0;
    else          cport = atoi(++p);
/*
    else {
        p++;
        i = 0;
        while(*p!='\0') portno[i++] = *(p++); 
        portno[i] = '\0';
        cport = atoi(portno);
    }
*/

    if (argc==3) sport = atoi(argv[2]);
    else         sport = cport;

    signal(SIGCLD, SIG_IGN);

    sofd  = tcp_server_socket(sport);
    while(1) {
        cdlen = sizeof(cl_addr);
        nsofd = accept(sofd, (struct sockaddr*)&cl_addr, (socklen_t*)&cdlen);
        if (nsofd<0) Error("accept");

        if (fork()==0) trans(nsofd, hostname, cport);
        close(nsofd);
    }

    socket_close(sofd);
}



trans(int sofd, char* hostname, int cport)
{
    int  i, cc, cofd, err=1;
    fd_set  mask;
    char msg[BUFSZ];
    struct timeval timeout;

    cofd  = tcp_client_socket(hostname, cport);
 
    timeout.tv_sec  = TIME_OUT;
    timeout.tv_usec = 0;
    FD_ZERO(&mask); 
    FD_SET(sofd, &mask);
    FD_SET(cofd, &mask);
    select(cofd+1, &mask, NULL, NULL, &timeout);

    while((FD_ISSET(cofd, &mask)||FD_ISSET(sofd, &mask)) && err) {
        if (FD_ISSET(cofd, &mask)) {
            cc = tcp_recv(cofd, msg, BUFSZ);
            if (cc>0) {
                cc = tcp_send(sofd, msg, cc);
                fprintf(stdout,"\n/+++++++++ Server +++++++++/\n%s",msg);
                fflush(stdout);
            }
            else err = 0;
        }

        if (FD_ISSET(sofd, &mask)) {
            cc = tcp_recv(sofd, msg, BUFSZ);
            if (cc>0) {
                cc = tcp_send(cofd, msg, cc);
                fprintf(stdout,"\n/+++++++++ Client +++++++++/\n%s",msg);
                fflush(stdout);
            }
            else err = 0;
        }

        FD_SET(sofd, &mask);
        FD_SET(cofd, &mask);
        select(cofd+1, &mask, NULL, NULL, &timeout);
    }

    fprintf(stdout,"\n/************ One session closed ************/\n\n");
    close(cofd);
    exit(0);
}
 





