
/*
  簡易UDP_CHAT 入力クライアント
*/


#include "tools.h"
#include "network.h"

#define  PORTNO 9100


int main(int argc, char** argv)
{
    int    cc, sock, sport=0, rport=0;
    char   *hname, *cport, *name;
    char   smsg[RECVBUFSZ], chat_msg[RECVBUFSZ];
    struct sockaddr_in sv_addr;
 
    if (argc<2) {
        fprintf(stderr, "Usage...  %s server_name:port [recv_port]\n", argv[0]);
        exit(1);
    }

    hname = awk(argv[1],':',1);
    cport = awk(argv[1],':',2);
    if (cport!=NULL) {
        sport = atoi(cport);
        free(cport);
    }
    if (sport==0) sport = PORTNO;

    if (argc>=3)  rport = atoi(argv[2]);
    if (rport==0) rport = sport + 1;
    
    
    sock = udp_client_socket_sockaddr_in(hname, sport, &sv_addr);
    if (sock<0) Error("udp_client_socket");
    free(hname);

    fgets(chat_msg, RECVBUFSZ-4, stdin);
    while(!feof(stdin)) {
        cc = udp_send_sockaddr_in(sock, chat_msg, 0, &sv_addr);
        fgets(chat_msg, RECVBUFSZ-4, stdin);
    }

    strncpy(smsg, "E\r\n", 4);
    cc = udp_send_sockaddr_in(sock, smsg, 0, &sv_addr);
}


