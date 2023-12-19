
/*  
  WEB Relay Program     ssl_relay v 1.0
*/




#include "tools.h"
#include "network.h"

#include "http_tool.h"


#define TIME_OUT 10



static int debugMode = ON;


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

    char* myip = get_localip();
    sofd  = tcp_server_socket(sport);
    while(1) {
        cdlen = sizeof(cl_addr);
        nsofd = accept(sofd, (struct sockaddr*)&cl_addr, (socklen_t*)&cdlen);
        if (nsofd<0) Error("accept");

        if (fork()==0) simple_web_proxy(nsofd, myip, 100);
        close(nsofd);
    }

    free(myip);
    socket_close(sofd);
}







