
/*
演習用　簡易UDP_CHATシステム サーバ v2.0
                        by Fumi.Iseki '06 6/7

    IP & Port ベース
    漢字コード SJIS, EUC 対応
    改行コード \n, \r, \r\n 対応
*/



#include "udp_chat_server.h"



int main(int argc, char** argv)
{
    int  i, sock, cc, len, sport=0, rport, kcode;
    char rmsg[RECVBUFSZ], smsg[RECVBUFSZ], ipaddr[20];
    char *name, *ipad, *mesg, *operand, command;
    char *dname, *knji, *cprt;
    unsigned char* p;

    struct sockaddr_in cl_addr;
    struct host_list* client;
    struct host_list* cltptr;
    struct host_list* nextcl;
    struct host_list* host;

    FILE* fp;

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i],"-p")) {if (i!=argc-1) sport = atoi(argv[i+1]);}
    }
    if (sport==0) sport = PORTNO;

    sock = udp_server_socket(sport, NULL);
    if (sock<0) Error("udp_server_socket");
    fprintf(stderr,"簡易UDP_CHATサーバ：ポート番号 %d で接続を待ち受けます．\n\n", sport);

    fp = fopen("log", "w");
 
    client = make_list();
    Loop{
        memset(rmsg, 0, RECVBUFSZ);
        memset(smsg, 0, RECVBUFSZ);

        cc = udp_recv_sockaddr_in(sock, rmsg, RECVBUFSZ-1, &cl_addr);

        if (cc>0) {
fprintf(fp, "%s\n", ipaddr, dname, rmsg);
fflush(fp);
            host = get_host(client, cl_addr);                   // IPアドレスからhostを検索
            if (host==NULL) dname = unknownuser;
            else dname = host->name;

            // 受信データ処理
            p = (unsigned char*)&(cl_addr.sin_addr);
            sprintf(ipaddr, "%d.%d.%d.%d",p[0],p[1],p[2],p[3]); // IPアドレス
            len = strlen(rmsg);
            if (len>=1 && (rmsg[len-1]=='\n' || rmsg[len-1]=='\r')) rmsg[len-1] = '\0';
            if (len>=2 && rmsg[len-2]=='\r') rmsg[len-2] = '\0';
            if (host!=NULL && host->kcode==CODE_SJIS) kanji_convert_sjis2euc((unsigned char*)rmsg);

            // 受信データを表示
fprintf(fp, "%s: %s> %s\r\n", ipaddr, dname, rmsg);
fflush(fp);
            fprintf(stderr, "%s: %s> %s\r\n", ipaddr, dname, rmsg);
            fflush(stderr);


            command = rmsg[0];                       // コマンド部 
            if (rmsg[1]!='\0') operand = &(rmsg[1]); // オペランド部 
            else               operand = NULL;

            // 接続開始 
            if (command=='S') {
                if (operand!=NULL){
                    if (host==NULL) {
                        name = awk(operand, ':', 1);
                        cprt = awk(operand, ':', 2);
                        knji = awk(operand, ':', 3);
                    
                        dname = name;
                        if (name==NULL) dname = nonname;

                        rport = 0;
                        if (cprt!=NULL) {
                            rport = atoi(cprt);
                            free(cprt);
                        }

                        kcode = CODE_EUC;
                        if (knji!=NULL) {
                            if (!strncasecmp(knji, "SJIS", 4)) {
                                if (name!=NULL) kanji_convert_sjis2euc((unsigned char*)name);
                                kcode = CODE_SJIS;
                            }
                            free(knji);
                        }

                        add_list(client, dname, rport, kcode, cl_addr);

                        snprintf(smsg, RECVBUFSZ-1, "%s %s %s\r\n", frmserv, dname, welcome);
                        udp_send_all(sock, smsg, client);
                        if (name!=NULL) free(name);                     
                    }
                    else {
                        host->times = 0;
                        snprintf(smsg, RECVBUFSZ-1, "%s %s さん．あなたは既にログイン済です．\r\n", frmserv, host->name);
                        udp_send_message(sock, smsg, host);
                    }
                }
            }


            // 未ログイン
            else if (host==NULL) {
                snprintf(smsg, RECVBUFSZ-1, "UDPCHAT_Server> You are not yet login!!\r\n");
                udp_send_sockaddr_in(sock, smsg, 0, &cl_addr);
            }


            // メッセージ転送 
            else if (command=='M'){
fprintf(fp, "in  M\n");
fflush(fp);
if (operand==NULL) {
fprintf(fp, "in  M NULLLLLLL\n");
fflush(fp);
}
                if (operand[0]!='\0') {
fprintf(fp, "in  M1\n");
fflush(fp);
                    host->times = -1; // すぐに +1されて 0になる．
fprintf(fp, "in  M2\n");
fflush(fp);
                    snprintf(smsg, RECVBUFSZ-1, "%s: %s> %s\r\n", ipaddr, host->name, operand);
fprintf(fp, "in  M3\n");
fflush(fp);
                    udp_send_all(sock, smsg, client);
fprintf(fp, "in  M4\n");
fflush(fp);
                }
fprintf(fp, "in2 M\n");
fflush(fp);
                del_host(sock, client, MAX_TIMESOUT); 
fprintf(fp, "out M\n");
fflush(fp);
            }


            // ログインメンバー名表示 
            else if (command=='L'){
fprintf(fp, "in  L\n");
fflush(fp);
                cltptr = client;
                while (cltptr->np!=NULL) {
                    nextcl = cltptr->np;
                    snprintf(smsg, RECVBUFSZ-1, "%s %s\r\n", frmserv, nextcl->name);
                    udp_send_message(sock, smsg, host);
                    cltptr = nextcl;
                }
fprintf(fp, "out L\n");
fflush(fp);
            }


            // 漢字コード指定 
            else if (command=='K'){
fprintf(fp, "in  K\n");
fflush(fp);
                if (operand!=NULL) {
                    if (!strncasecmp(operand, "SJIS", 4)) {
                        host->kcode = CODE_SJIS;
                    }
                    else if (!strncasecmp(operand, "EUC", 3)) {
                        host->kcode = CODE_EUC;
                    }
                }
fprintf(fp, "out K\n");
fflush(fp);
            }


            // 受信ポート指定 
            else if (command=='P'){
fprintf(fp, "in  P\n");
fflush(fp);
                if (operand!=NULL) {
                    rport = atoi(operand);
                    if (rport>0) host->rport = rport;
                }
fprintf(fp, "out P\n");
fflush(fp);
            }


            // 接続継続
            else if (command=='C'){
fprintf(fp, "in  C\n");
fflush(fp);
                host->times = 0;
fprintf(fp, "out C\n");
fflush(fp);
            }


            // ログオフ
            else if (command=='E'){
                snprintf(smsg, RECVBUFSZ-1, "%s %s %s\r\n", frmserv, host->name, byebye);
fprintf(fp, "in  E\n");
fflush(fp);
                udp_send_all(sock, smsg, client);
                del_list(client, cl_addr);
fprintf(fp, "out E\n");
fflush(fp);
            }       


            // 理解できないコマンド 
            else {
fprintf(fp, "in  U\n");
fflush(fp);
                snprintf(smsg, RECVBUFSZ-1, "%s コマンドエラーです．%s\r\n", frmserv, rmsg);
                udp_send_message(sock, smsg, host);
fprintf(fp, "out U\n");
fflush(fp);
            }
        }
    }
}




// アイドルクライアントの削除
void  del_host(int sock, struct host_list* client, int max_time) 
{
    struct host_list* nextcl;
    struct host_list* cltptr;
    char smsg[RECVBUFSZ];

    cltptr = client;
    while (cltptr->np!=NULL) {
        nextcl = cltptr->np;

        nextcl->times++;
        if (nextcl->times > max_time) {
            memset(smsg, 0, RECVBUFSZ);
            snprintf(smsg, RECVBUFSZ-1, 
                "%s %s さんからは一定期間入力がなかったので，接続を切断します．\r\n", frmserv, nextcl->name);
            udp_send_all(sock, smsg, client);
            cltptr->np = nextcl->np;
            free(nextcl);
        }
        else {
            cltptr = nextcl;
        }
    }
}






struct host_list* make_list(void) 
{
    struct host_list* hl;

    hl = (struct host_list*)malloc(sizeof(struct host_list));
    memset(hl, 0, sizeof(struct host_list));
    hl->np = NULL;

    return hl;
}




int  add_list(struct host_list* hl, char* name, int rport, int kcode, struct sockaddr_in cl_addr) 
{
    unsigned char* p;
    while (hl->np!=NULL) hl = hl->np;

    hl->np = make_list();
    hl = hl->np;
    if (hl==NULL) return -1;

    strncpy(hl->name, name, LNAME-1);
    hl->rport = rport;
    hl->kcode = kcode;
    hl->times = 0;
    memcpy((char*)&(hl->cl_addr), (char*)&cl_addr, sizeof(cl_addr));
    return 0;
}




int  del_list(struct host_list* hl, struct sockaddr_in cl_addr) 
{
    unsigned int ip;
    unsigned short int pt;
    struct host_list* nx;

    ip = cl_addr.sin_addr.s_addr;
    pt = cl_addr.sin_port;          // 送信元ポート

    while (hl->np!=NULL) {
        nx = hl->np;
        if (ip==(nx->cl_addr).sin_addr.s_addr && pt==(nx->cl_addr).sin_port) break;
        hl = nx;
    }
    if (hl->np==NULL) return -1;

    hl->np = nx->np;
    free(nx);
    return 0;
}




struct host_list*  get_host(struct host_list* hl, struct sockaddr_in cl_addr) 
{
    unsigned int ip;
    unsigned short int pt;
    struct host_list* nx;

    ip = cl_addr.sin_addr.s_addr;   // 送信元アドレス
    pt = cl_addr.sin_port;          // 送信元ポート

    while (hl->np!=NULL) {
        nx = hl->np;
        if (ip==(nx->cl_addr).sin_addr.s_addr && pt==(nx->cl_addr).sin_port) break;
        hl = nx;
    }

    if (hl->np==NULL) return NULL;
    return nx;
}




int  print_list(struct host_list* hl) 
{
    unsigned char* ip;
    unsigned short int pt;
    struct host_list* nx;

    while (hl->np!=NULL) {
        nx = hl->np;
        ip = (unsigned char*)&((nx->cl_addr).sin_addr);
        pt = (nx->cl_addr).sin_port;
        fprintf(stderr, "%d.%d.%d.%d:%d  %s\n",ip[0],ip[1],ip[2],ip[3], ntohs(pt), nx->name);
        hl = nx;
    }
}   





void  udp_send_all(int sock, char* mesg, struct host_list* hl)
{
    int cc;
    struct host_list* nx;

    while (hl->np!=NULL) {
        nx = hl->np;
        udp_send_message(sock, mesg, nx);
        hl = nx;
    }
}




void  udp_send_message(int sock, char* mesg, struct host_list* nx)
{
    struct sockaddr_in sl_addr;
    int len = strlen(mesg)+1;
    char* sendmesg = (char*)malloc(len);

    memcpy(sendmesg, mesg, len); 
    if (nx->kcode==CODE_SJIS) kanji_convert_euc2sjis((unsigned char*)sendmesg); 

    memcpy((char*)&sl_addr, (char*)&(nx->cl_addr), sizeof(sl_addr));
    if (nx->rport!=0) sl_addr.sin_port = htons(nx->rport);

    udp_send_sockaddr_in(sock, sendmesg, 0, &sl_addr);
    free(sendmesg);
}




