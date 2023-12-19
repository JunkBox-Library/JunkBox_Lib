#include "network.h"


int main(int argc, char** argv)
{
    int  err, portno, sofd;
    char* hostname;
    char rmsg[BUFMAX];

    if (argc<3) {
    fprintf(stderr,"Usage... %s host_name port_no\n",argv[0]);
        exit(0);
    }

    hostname = argv[1];
    portno   = atoi(argv[2]);
    bzero(rmsg, BUFMAX);

    sofd = tcp_client_socket(hostname, portno);
//  err = tcp_send(sofd, "Hello Iseki\n", 0); 
    err = tcp_recv(sofd, rmsg, BUFMAX);
    socket_close(sofd);

    fprintf(stdout, "%s", rmsg);
}



