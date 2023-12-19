#include "network.h"
#include "tlist.h"
#include "http_tool.h"


// HTTPサーバのドキュメントルート．適当なディレクトリを入れる．
#define  DOC_ROOT  "/home/iseki/public_html"
#define  BUF_SZ 100



// 関数のプロトタイプ宣言
void  http_send_response(int sofd, tList* hd);
void  send_file(int sofd, char* fname, int sz);



int main(int argc, char** argv)
{
    struct sockaddr_in addr;
    int  port, sock, sofd, cdlen, len;
    tList* hd;

    // 引数のチェック
    if (argc<2) {
        print_message("Usage..... %s port_num\n", argv[0]);
        exit(1);
    }

    // サーバ用ソケットのオープン
    port = atoi(argv[1]);
    sock = tcp_server_socket(port);
    if (sock<0) {
        print_message("socket open errror!!  port = %d\n", port);
        exit(1);
    }


    signal(SIGCHLD, SIG_IGN);

    // ループ  while(1)
    Loop {
        // 接続待ち
        cdlen = sizeof(addr);
        sofd = accept_intr(sock, (struct sockaddr*)&addr, (socklen_t*)&cdlen);
        if (sofd>0) {      
            if (fork()==0) {
                // 接続
                recv_http_header(sofd, &hd, &len, 5, NULL, NULL);
                print_tList(stderr, hd);
            
                // リクエストを処理して，レスポンスを返す．
                http_send_response(sofd, hd);

                // 接続をクローズ
                close(sofd);
                del_all_tList(&hd);
                exit(0);
            }
            close(sofd);
        }
    }
}



//
// リクエストの入ったリストhd からファイル名を取り出して，sofd に送信する．
//
void  http_send_response(int sofd, tList* hd)
{
    int sz;
    Buffer fullpath;
    char*  fname = NULL;

    if (hd==NULL) return;

    while (hd!=NULL) {
        if (!strcmp((char*)hd->ldat.key.buf, "FIRST_LINE")) {
            fname = cawk((char*)hd->ldat.val.buf, ' ', 2);
            break;
        }
    }
    if (fname==NULL) return;

    // リクエストのあったファイルのフルパスを求める．
    fullpath = make_Buffer_bystr(DOC_ROOT);
    cat_s2Buffer(fname, &fullpath);         // 文字列を結合．

    print_message("Requested File = %s\n", fullpath.buf);
    print_message("------------------------------------------------\n");

    // ファイルサイズを検査
    sz = file_size((char*)fullpath.buf);
    if (sz<=0) {
        // ファイルが存在しない場合の処理をここに書く
    }
    else {
        char  buf[BUF_SZ];
        memset(buf, 0, BUF_SZ);
        sprintf(buf, "Content-Length: %d\r\n", sz);

        tcp_send_mesg(sofd, "HTTP/1.1 200 OK\r\n");
        tcp_send_mesg(sofd, buf);
        tcp_send_mesg(sofd, "Content-Type: text/html\r\n\r\n");

        send_file(sofd, (char*)fullpath.buf, sz);
    }

    free_Buffer(&fullpath);
}



//
// ファイル名 fname のファイルを送信する．
//
void  send_file(int sofd, char* fname, int sz)
{
    FILE* fp = fopen(fname, "r");

    if (fp!=NULL) {
        char* buf = (char*)malloc(sz);
        fread(buf, sz, 1, fp);
        tcp_send(sofd, buf, sz);

        free(buf);
        fclose(fp);
    }
}
