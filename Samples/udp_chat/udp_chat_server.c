
/*
演習用　簡易UDP_CHATシステム サーバ v2.0.1
                        by Fumi.Iseki '07 6/4

    IP & Port ベース
    改行コード \n, \r, \r\n 対応
*/



#include "udp_chat_server.h"



int main(int argc, char** argv)
{
    int  i, sock, cc, len, sport=0, rport;
    char rmsg[RECVBUFSZ], smsg[RECVBUFSZ], ipaddr[20];
    char *name, *ipad, *mesg, *operand, command;
    char *dname, *cprt;
    unsigned char* p;

    struct sockaddr_in cl_addr;
    struct host_list* client;
    struct host_list* cltptr;
    struct host_list* nextcl;
    struct host_list* host;

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i],"-p")) {if (i!=argc-1) sport = atoi(argv[i+1]);}
    }
    if (sport==0) sport = PORTNO;

    sock = udp_server_socket(sport, NULL);
    if (sock<0) Error("udp_server_socket");
    fprintf(stderr,"簡易UDP_CHATサーバ：ポート番号 %d で接続を待ち受けます．\n\n", sport);

    client = make_list();
    while (1) {
        memset(rmsg, 0, RECVBUFSZ);
        memset(smsg, 0, RECVBUFSZ);

        cc = udp_recv_sockaddr_in(sock, rmsg, RECVBUFSZ-1, &cl_addr);

        if (cc>0) {
            host = get_host(client, cl_addr);                   // IPアドレスからhostを検索
            if (host==NULL) dname = unknownuser;
            else dname = host->name;

            // 受信データ処理
            p = (unsigned char*)&(cl_addr.sin_addr);
            sprintf(ipaddr, "%d.%d.%d.%d",p[0],p[1],p[2],p[3]); // IPアドレス
            len = strlen(rmsg);
            if (len>=1 && (rmsg[len-1]=='\n' || rmsg[len-1]=='\r')) rmsg[len-1] = '\0';
            if (len>=2 && rmsg[len-2]=='\r') rmsg[len-2] = '\0';
            //if (host!=NULL && host->kcode==CODE_SJIS) kanji_convert_sjis2euc((unsigned char*)rmsg);

            // 受信データを表示
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
                    
                        dname = name;
                        if (name==NULL) dname = nonname;

                        rport = 0;
                        if (cprt!=NULL) {
                            rport = atoi(cprt);
                            free(cprt);
                        }

                        //add_list(client, dname, rport, kcode, cl_addr);
                        add_list(client, dname, rport, 0, cl_addr);

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
                if (operand!=NULL && operand[0]!='\0') {
                    host->times = -1; // すぐに +1されて 0になる．
                    snprintf(smsg, RECVBUFSZ-1, "%s: %s> %s\r\n", ipaddr, host->name, operand);
                    udp_send_all(sock, smsg, client);
                }
                del_host(sock, client, MAX_TIMESOUT); 
            }

            // ログインメンバー名表示 
            else if (command=='L'){
                cltptr = client;
                while (cltptr->np!=NULL) {
                    nextcl = cltptr->np;
                    snprintf(smsg, RECVBUFSZ-1, "%s %s\r\n", frmserv, nextcl->name);
                    udp_send_message(sock, smsg, host);
                    cltptr = nextcl;
                }
            }

            // 受信ポート指定 
            else if (command=='P'){
                if (operand!=NULL) {
                    rport = atoi(operand);
                    if (rport>0) host->rport = rport;
                }
            }

            // 接続継続
            else if (command=='C'){
                host->times = 0;
            }

            // ログオフ
            else if (command=='E'){
                snprintf(smsg, RECVBUFSZ-1, "%s %s %s\r\n", frmserv, host->name, byebye);
                udp_send_all(sock, smsg, client);
                int ret = del_list(client, cl_addr);
                while (ret==0) ret = del_list(client, cl_addr);
            }       

            // 理解できないコマンド 
            else {
                snprintf(smsg, RECVBUFSZ-1, "%s コマンドエラーです．%s\r\n", frmserv, rmsg);
                udp_send_message(sock, smsg, host);
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
    unsigned char* p1;
    unsigned char* p2;

    while (hl->np!=NULL) {
/*
		p1 = get_ipaddr_ipv4(hl->cl_addr.sin_addr);
		p2 = get_ipaddr_ipv4(cl_addr.sin_addr);
		if (!strcmp(p1, p2)) {
			struct host_list* pl = hl;
			hl = hl->np;
			del_list(pl, pl->cl_addr);
		}
*/
		hl = hl->np;
	}

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
        //if (ip==(nx->cl_addr).sin_addr.s_addr && pt==(nx->cl_addr).sin_port) break;
        if (ip==(nx->cl_addr).sin_addr.s_addr) break;
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

    memcpy((char*)&sl_addr, (char*)&(nx->cl_addr), sizeof(sl_addr));
    if (nx->rport!=0) sl_addr.sin_port = htons(nx->rport);

    udp_send_sockaddr_in(sock, sendmesg, 0, &sl_addr);
    free(sendmesg);
}



