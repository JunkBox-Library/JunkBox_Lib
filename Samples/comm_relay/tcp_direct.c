
/*  
  TCP Redirect Program  tcp_redirect v 1.5
*/


#include "tools.h"
#include "network.h"


#define TIME_OUT 10

static int debugMode = OFF;

void trans(char* hostname, int cport, char* filename);


int main(int argc, char** argv)
{
    int  i, cport;
    char *p, *filename=NULL, hostname[LNAME];

    if (argc<=1) {
        fprintf(stderr,"Usage... %s host_name[:port] [filename]\n", argv[0]);
        exit(0);
    }

    p = argv[1];
    i = 0;
    while(*p!='\0' && *p!=':' && i<LNAME-1) hostname[i++] = *(p++); 
    hostname[i] = '\0';
    if (*p=='\0') cport = 80;
    else          cport = atoi(++p);

    if (argc>=3) filename = argv[2];

    trans(hostname, cport, filename);
}



void trans(char* hostname, int cport, char* filename)
{
    int  i, nd, cc, cofd;
    fd_set  mask;
    char msg[BUFSZ];
    struct timeval timeout;
    FILE* fp;

    if (filename==NULL) fp = stdin;
    else  fp = fopen(filename, "r");
    if (fp==NULL) return;

    cofd  = tcp_client_socket(hostname, cport);
    if (cofd<0) return;

    memset(msg, 0x00, BUFSZ);
    fgets(msg, BUFSZ, fp);
    while(!feof(fp)) {
        tcp_send_mesg(cofd, msg);
        //fprintf(stdout, "%s", msg);
        memset(msg, 0x00, BUFSZ);
        fgets(msg, BUFSZ, fp);
    }
    if (filename!=NULL) fclose(fp);
 
    do {
        timeout.tv_sec  = TIME_OUT;
        timeout.tv_usec = 0;
        FD_ZERO(&mask); 
        FD_SET(cofd, &mask);
        nd = select(cofd+1, &mask, NULL, NULL, &timeout);
    } while (nd<0);

    while((FD_ISSET(cofd, &mask))) {
        cc = tcp_recv(cofd, msg, BUFSZ);
        if (cc>0) {
            fprintf(stdout,"%s",msg);
            fflush (stdout);
        }
        else break;

        do {
            timeout.tv_sec  = TIME_OUT;
            timeout.tv_usec = 0;
            FD_ZERO(&mask); 
            FD_SET(cofd, &mask);
            nd = select(cofd+1, &mask, NULL, NULL, &timeout);
        } while (nd<0);
    }

    socket_close(cofd);
    return;
}
 

