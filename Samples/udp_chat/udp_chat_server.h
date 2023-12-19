/**
  簡易UDP_CHAT サーバ  v2.0
            by Fumi.Iseki  '06 6/7
*/



#include "tools.h"
#include "network.h"

#define  PORTNO  9100       // デフォルトポート番号 
#define  MAX_TIMESOUT 100   // アイドル回数


// クライアントの情報を格納するためのリスト 
struct host_list {
    char name[LNAME];             // ユーザの名前 
    int  rport;
    int  kcode;
    int  times;
    struct sockaddr_in cl_addr;   // クライアントのアドレス情報 
    struct host_list* np;         // 次のリストへのポインタ 
};


char  welcome[] = "さんがログインしました．ようこそ．";
char  byebye [] = "さんがログアウトしました．バイバイ．";
char  frmserv[] = "チャットサーバ>";
char  unknownuser[] = "unknown user";
char  nonname[] = "名無し";



//空のリストを一つ作る 
struct host_list* make_list(void); 

//リストの最後に name, cl_addrを持つリストを追加する 
int    add_list(struct host_list* hl, char* name, int rport, int kcode, struct sockaddr_in cl_addr); 

//アドレス情報 cl_addrを持つリストを削除する 
int    del_list(struct host_list* hl, struct sockaddr_in cl_addr); 

//アドレス,ポート情報 cl_addrを持つリストから該当hostを返す 
struct host_list*  get_host(struct host_list* hl, struct sockaddr_in cl_addr); 

//全クライアントのIPアドレスとユーザ名をstderrに出力 
int    print_list(struct host_list* hl); 

//全クライアントに mesgを送信 
void   udp_send_all(int sock, char* mesg, struct host_list* hl);

//個々のクライアントに mesgを送信 
void   udp_send_message(int sock, char* mesg, struct host_list* nx);

// max_time回入力のないクライアントを削除
void  del_host(int sock, struct host_list* cltptr, int max_time);


