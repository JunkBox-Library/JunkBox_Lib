#include "network.h"


int main(int argc, char** argv)
{
    int  i, err, nbyte, sofd, nport;
    char *p, hostname[128], portno[7];
    char rmsg[BUFMAX], fbuf[BUFMAX];
    FILE *fp;

    if (argc<3) {
        fprintf(stderr,"Usage... %s host_name file_name\n",argv[0]);
        exit(0);
    }

    p = argv[1];
    i = 0;
    while(*p!='\0' && *p!=':') hostname[i++] = *(p++); 
    hostname[i] = '\0';

    if (*p=='\0') nport = 0;
    else {
        p++;
        i = 0;
        while(*p!='\0') portno[i++] = *(p++); 
        portno[i] = '\0';
        nport = atoi(portno);
    }

    fp = fopen(argv[2], "r");
    if (fp==NULL) {
        Error("fopen");
        exit(1);
    }

    sofd = tcp_client_socket(hostname, nport);

    bzero(fbuf, BUFMAX);
    fgets(fbuf, BUFMAX-1, fp);
    while(!feof(fp)){
    err = tcp_send(sofd, fbuf, 0); 
    if (err<0) perror("send");
        bzero(fbuf, BUFMAX);
        fgets(fbuf, BUFMAX-1, fp);
    }
    err = tcp_send(sofd, "\n\n\n\n", 0); 

    bzero(rmsg, BUFMAX);
    err = tcp_recv(sofd, rmsg, BUFMAX);
    while(err>0){
    fprintf(stdout,"%s",rmsg);
        bzero(rmsg, BUFMAX);
        err = tcp_recv(sofd, rmsg, BUFMAX);
    }

    fclose(fp);
    socket_close(sofd);
}



