
/*  
  TCP Relay Program     tcp_relay v 1.5
*/




#include "tools.h"
#include "network.h"


#define TIME_OUT 600


void trans(int sofd, char* hostname, int cport);

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

        if (fork()==0) trans(nsofd, hostname, cport);
        close(nsofd);
    }

    socket_close(sofd);
}








void trans(int sofd, char* hostname, int cport)
{
    int  i, cc, nm, nd, cofd;
    fd_set  mask;
    char msg[BUFSZ];
    struct timeval timeout;

    cofd  = tcp_client_socket(hostname, cport);
    nm = Max(sofd, cofd);
 
    do {
        timeout.tv_sec  = TIME_OUT;
        timeout.tv_usec = 0;
        FD_ZERO(&mask); 
        FD_SET(sofd, &mask);
        FD_SET(cofd, &mask);
        nd = select(nm+1, &mask, NULL, NULL, &timeout);
    } while (nd<0);

    while((FD_ISSET(cofd, &mask)||FD_ISSET(sofd, &mask))) {
        if (FD_ISSET(cofd, &mask)) {
            cc = tcp_recv(cofd, msg, BUFSZ);
            if (cc>0) {
                cc = tcp_send(sofd, msg, cc);
                if (debugMode==ON) {
                    fprintf(stdout,"\r\n/+++++++++ Server +++++++++/\r\n%s",msg);
                    //fprintf(stdout,"%s",msg);
                    fflush (stdout);
                }
                if (cc<=0) break;
            }
            else if (cc<0) break;
        }

        if (FD_ISSET(sofd, &mask)) {
            cc = tcp_recv(sofd, msg, BUFSZ);
            if (cc>0) {
                cc = tcp_send(cofd, msg, cc);
                if (debugMode==ON) {
                    fprintf(stdout,"\r\n/+++++++++ Client +++++++++/\r\n%s",msg);
                    //fprintf(stdout,"%s",msg);
                    fflush (stdout);
                }
                if (cc<=0) break;
            }
            else if (cc<0) break;
        }

        do {
            timeout.tv_sec  = TIME_OUT;
            timeout.tv_usec = 0;
            FD_ZERO(&mask); 
            FD_SET(sofd, &mask);
            FD_SET(cofd, &mask);
            nd = select(nm+1, &mask, NULL, NULL, &timeout);
        } while (nd<0);
    }


    if (debugMode==ON) {
        fprintf(stdout,"\n/************ One session closed ************/\n\n");
        fflush (stdout);
    }
    socket_close(cofd);
    socket_close(sofd);
    exit(0);
}
 





