
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
 
    if (argc<3) {
        fprintf(stderr, "Usage...  %s server_name:port my_name [recv_port]\n", argv[0]);
        exit(1);
    }

    hname = awk(argv[1],':',1);
    cport = awk(argv[1],':',2);
    if (cport!=NULL) {
        sport = atoi(cport);
        free(cport);
    }
    if (sport==0) sport = PORTNO;
    name = argv[2];

    if (argc>=4)  rport = atoi(argv[3]);
    if (rport==0) rport = sport + 1;
    
    
    sock = udp_client_socket_sockaddr_in(hname, sport, &sv_addr);
    if (sock<0) Error("udp_client_socket");
    free(hname);

    snprintf(smsg, RECVBUFSZ-1, "S%s:%d:EUC\r\n", name, rport);
    cc = udp_send_sockaddr_in(sock, smsg, 0, &sv_addr);

    //cc = udp_recvi_in(sock, smsg, RECVBUFSZ-1, &sv_addr);
    //printf("%s", smsg);


    fgets(chat_msg, RECVBUFSZ-4, stdin);
    while(!feof(stdin)) {
        if (chat_msg[0]=='L') {
            cc = udp_send_sockaddr_in(sock, "L\r\n", 0, &sv_addr);
        }
        else if (chat_msg[0]=='C') {
            cc = udp_send_sockaddr_in(sock, "C\r\n", 0, &sv_addr);
        }
        else if (chat_msg[0]=='E') {
            cc = udp_send_sockaddr_in(sock, "E\r\n", 0, &sv_addr);
        }
        else {
            snprintf(smsg, RECVBUFSZ-1, "M%s", chat_msg);
            cc = udp_send_sockaddr_in(sock, smsg, 0, &sv_addr);
        }
        fgets(chat_msg, RECVBUFSZ-4, stdin);
    }

    strncpy(smsg, "E\r\n", 4);
    cc = udp_send_sockaddr_in(sock, smsg, 0, &sv_addr);

    return 0;
}


