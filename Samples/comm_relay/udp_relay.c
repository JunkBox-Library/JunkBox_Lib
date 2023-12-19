
/*  
  UDP Relay Program     udp_relay v 1.0
*/


#include "tools.h"
#include "network.h"

#define TIME_OUT 10

void  udp_relay(char* hostname, int sport, int cport, int cmode, int dmode, char* endmsg);
int   is_same_process(struct sockaddr_in addr1, struct sockaddr_in addr2); 


int main(int argc, char** argv)
{
    int  i, sport, cport, dmode=OFF;
    char *p, hostname[LNAME], portno[7];

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
            if (!strcmp("-d", argv[3])) dmode = ON;
        }
    }
    else sport = cport;

    signal(SIGCLD, SIG_IGN);

    udp_relay(hostname, cport, sport, ON, dmode, "END");
}



/**

*/
void  udp_relay(char* hostname, int cport, int sport, int cmode, int dmode, char* endmsg)
{
    int  cc, nd, nm, sofd, cofd, cnct=OFF;
    char msg[BUFSZ];
    struct sockaddr_in  sv_addr, cl_addr, dm_addr;

    fd_set  mask;
    struct timeval timeout;

    sofd = udp_server_socket(sport, NULL);
    cofd = udp_client_socket_sockaddr_in(hostname, cport, &cl_addr);
    if (cofd<=0) {
        fprintf(stderr,"サーバへの接続失敗 %s:%d\n",hostname,cport);
        fflush(stderr);
        exit(1);
    }
    nm = Max(sofd, cofd);
 
    do {
        timeout.tv_sec  = TIME_OUT;
        timeout.tv_usec = 0;
        FD_ZERO(&mask); 
        FD_SET(sofd, &mask);
        FD_SET(cofd, &mask);
        nd = select(nm+1, &mask, NULL, NULL, &timeout);
    } while (nd<0);

    fprintf(stderr,"サーバ %s:%d\n",hostname,cport);
    fflush(stderr);
    Loop {
        if (FD_ISSET(cofd, &mask)) {
            cc = udp_recv_sockaddr_in(cofd, msg, BUFSZ, &cl_addr);
            if (cc>0 && cnct==ON) {
                cc = udp_send_sockaddr_in(sofd, msg, cc, &sv_addr);
                if (dmode==ON) {
                    fprintf(stdout,"\r\n/+++++++++ Server +++++++++/\r\n%s",msg);
                    fflush (stdout);
                }
            }
        }

        if (FD_ISSET(sofd, &mask)) {
            cc = udp_recv_sockaddr_in(sofd, msg, BUFSZ, &dm_addr);
            if (cc>0) {
                if (cmode==ON) {    // process check mode
                    if (cnct==OFF) {
                        cnct = ON;
                        memcpy(&sv_addr, &dm_addr, sizeof(dm_addr));
                    }
                    if (endmsg!=NULL && !strncmp(msg, endmsg, strlen(endmsg))) break;
    
                    if (is_same_process(sv_addr, dm_addr)) {
                        cc = udp_send_sockaddr_in(cofd, msg, cc, &cl_addr);
                        if (dmode==ON) {
                            fprintf(stdout,"\r\n/+++++++++ Client +++++++++/\r\n%s",msg);
                            fflush (stdout);
                        }
                    }
                    else {
                        if (dmode==ON) {
                            fprintf(stdout,"\r\ndifferent process connected!!\r\n");
                            fflush (stdout);
                        }
                    }   
                }
                // non check mode
                else {
                    cc = udp_send_sockaddr_in(cofd, msg, cc, &cl_addr);
                    if (dmode==ON) {
                        fprintf(stdout,"\r\n/+++++++++ Client +++++++++/\r\n%s",msg);
                        fflush (stdout);
                    }
                }
            }
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

    if (dmode==ON) {
        fprintf(stdout,"\n/************ One session closed ************/\n\n");
        fflush (stdout);
    }
    socket_close(cofd);
    socket_close(sofd);
    exit(0);
}
 


/**
int  is_same_process(struct sockaddr_in addr1, struct sockaddr_in addr2) 

    addr1 と addr2が格納する IPアドレスとポート番号が同じかどうか検査する
*/
int  is_same_process(struct sockaddr_in addr1, struct sockaddr_in addr2) 
{
    int i, len;
    unsigned char *p1, *p2;

    p1  = (unsigned char*)&(addr1.sin_addr);
    p2  = (unsigned char*)&(addr2.sin_addr);
    len = sizeof(addr1.sin_addr);

    for (i=0; i<len; i++) {
        if (p1[i]!=p2[i]) return FALSE;
    }

    p1  = (unsigned char*)&(addr1.sin_port);
    p2  = (unsigned char*)&(addr2.sin_port);
    len = sizeof(addr1.sin_port);

    for (i=0; i<len; i++) {
        if (p1[i]!=p2[i]) return FALSE;
    }

    return TRUE;
}


