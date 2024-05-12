/** 
@brief   HTTPS ツールライブラリ
@file    https_tool.c
@author  Fumi.Iseki (C)
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include "https_tool.h"


#ifdef ENABLE_SSL

/**
int  send_https_header(int sofd, SSL* ssl, tList* pp, int mode)

ヘッダリストの内容を HTTPS通信で送信する．
ヘッダ中の Content-Length の値は操作しない．

@param  sofd    接続相手へのソケット
@param  ssl     接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param  pp      送信するヘッダが格納されるリストへのポインタ．
@param  mode    @b ON:  ヘッダリスト内にコンテンツの一部があればそれも送信する．
@param  mode    @b OFF: ヘッダリスト内にコンテンツの一部があっても送信しない．

@retval 0以上  mode==OFF: 送信した全データバイト数．
               mode==ON : 送信したコンテンツのバイト数．
@retval 0未満  エラー
@retval JBXL_ARGS_ERROR  リストへのポインタが NULL
*/
int  send_https_header(int sofd, SSL* ssl, tList* pp, int mode)
{
    int    hs=0, sz=0;
    Buffer buf;

    if (pp==NULL) return JBXL_ARGS_ERROR;

    buf = restore_protocol_header(pp, (char*)": ", mode, &hs);
    if (buf.vldsz>0) {
        sz  = ssl_tcp_send_Buffer(sofd, ssl, &buf);
        if (mode==ON) sz = sz - hs;
    }

    free_Buffer(&buf);
    return sz;
}


/**
int  send_https_Buffer(int sofd, SSL* ssl, tList* pl, Buffer* buf)

ヘッダとバッファの内容を HTTP通信で送信する．

@param  sofd   接続相手へのソケット
@param  ssl    接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param  pl     ファイルに先立って送信されるヘッダが格納されるリストへのポインタ．
@param  buf    送信するコンテンツが格納された Buffer変数へのポインタ．

@retval 0以上 送信したデータの全サイズ(Byte)
@retval JBXL_ARGS_ERROR  リストへのポインタが NULL
*/
int  send_https_Buffer(int sofd, SSL* ssl, tList* pl, Buffer* buf)
{
    int sz;
    Buffer snd;
 /*
    int nobody = FALSE;

    if (pl==NULL) return JBXL_ARGS_ERROR;
    if (buf==NULL || buf->vldsz<0 || buf->buf==NULL) nobody = TRUE;

    if (!nobody) {
        // Content-Length の書き換え
        tList* pp = pl;
        while(pp!=NULL) {
            if (!strcasecmp((const char*)(pp->ldat.key.buf), "Content-Length")) {
                copy_i2Buffer(buf->vldsz, &(pp->ldat.val));
                break;
            }
            pp = pp->next;
        }
    }

    snd = restore_protocol_header(pl, (char*)": ", OFF, NULL);
    if (!nobody) cat_Buffer(buf, &snd);
*/
    snd = rebuild_http_Buffer(pl, buf);
    sz  = ssl_tcp_send_Buffer(sofd, ssl, &snd);
    free_Buffer(&snd);

    return sz;
}


/**
int  send_https_file(int sofd, SSL* ssl, tList* pl, const char* fname)

ヘッダとファイルの内容を HTTPS通信で送信する．

@param  sofd   接続相手へのソケット
@param  ssl    接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param  pl     ファイルに先立って送信されるヘッダが格納されるリストへのポインタ．
@param  fname  送信するファイル名．ヘッダのみ送る場合は NULL

@retval 0以上 送信したデータの全サイズ(Byte)
@retval JBXL_ARGS_ERROR    リストへのポインタが NULL
@retval JBXL_MALLOC_ERROR  メモリ確保エラー
*/
int  send_https_file(int sofd, SSL* ssl, tList* pl, const char* fname)
{
    int   sz = 0;
    FILE* fp = NULL;
    char num[20];
    tList* pp;
    unsigned char* html;
    Buffer  buf;

    size_t rs;
    UNUSED(rs);

    if (pl==NULL) return JBXL_ARGS_ERROR;
    if (fname!=NULL) sz = file_size((char*)fname);
    if (sz<=0) sz = 0;
    snprintf(num, 18, "%d", sz);

    // Content-Length の書き換え
    pp = pl;
    while(pp!=NULL) {
        if (!strcasecmp((const char*)(pp->ldat.key.buf), "Content-Length")) {
            copy_s2Buffer(num, &(pp->ldat.val));
            break;
        }
        pp = pp->next;
    }

    buf = restore_protocol_header(pl, (char*)": ", OFF, NULL);

    if (fname!=NULL && sz!=0) fp = fopen(fname, "rb");
    if (fp!=NULL) {
        html = (unsigned char*)malloc(sz+1);
        if (html==NULL) {
            fclose(fp);
            free_Buffer(&buf);
            return JBXL_MALLOC_ERROR;
        }

        memset(html, 0, sz+1);
        rs = fread(html, sz, 1, fp); 
        fclose(fp);

        cat_b2Buffer(html, &buf, sz);
        free(html);
    }

    sz = ssl_tcp_send_Buffer(sofd, ssl, &buf);
    free_Buffer(&buf);

    return sz;
}


/**
int  recv_https_header(int sofd, SSL* ssl, tList** pl, int* len, int tm, FILE* fp, int* state)

HTTPS通信のヘッダ部分を受信して，リストに格納する．@n
NULLでない fp が指定された場合，受信した全てのデータをファイルに保存する．
受信したデータが HTTPSでない場合は，*pl の HDLIST_CONTENTS_KEY の値として受信データを格納する．

@param       sofd   接続相手へのソケット
@param       ssl    接続相手への SSLソケット．SSL通信でない場合は NULLを指定．
@param[out]  *pl    ヘッダ情報が格納されるリストへのポインタ．NULLなら格納いない．
                    (*pl)->ldat.id に HTTPデータのコマンド種別が入る．(HTTP_GET_METHOD など)
@param[out]  len    ヘッダ中の "Content-Length" の値（正の値）. 
                    "Content-Length" が存在しなければ @b HTTP_HEADER_UNKNOWN_LEN (0未満)．
                    チャンク形式なら @b HTTP_HEADER_CHUNKED (0未満)．
                    引き続く1回のセッション内でボディを転送するのなら，@b HTTP_HEADER_CLOSED_SESSION (0未満)．
                    受信データが HTTP でないなら @b HTTP_HEADER_NOT_HTTP @b (0未満)．
@param       fp     受信した全てのデータを保存するためのファイルポインタ．NULLの場合は保存しない．
@param[out]  state  サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval 0以上  受信した全バイト数
@retval 0      正常切断
@retval JBXL_SSL_RECV_ERROR  受信エラー
@retval JBXL_ARGS_ERROR      無効な sofdを指定した または len にNULL を指定した
*/
int  recv_https_header(int sofd, SSL* ssl, tList** pl, int* len, int tm, FILE* fp, int* state)
{
    int  cc, sz, com;
    Buffer mbuf;
    tList* lp;
    int  connect = TRUE;

    if (sofd<=0)   return JBXL_ARGS_ERROR;
    if (len==NULL) return JBXL_ARGS_ERROR;
    *len = HTTP_HEADER_UNKNOWN_LEN;

    mbuf = make_Buffer(RECVBUFSZ);

    sz  = 0;
    lp  = NULL;
    *pl = NULL;

    cc  = ssl_tcp_recv_Buffer_wait(sofd, ssl, &mbuf, tm);
    com = get_http_method((char*)mbuf.buf);

    if (com>HTTP_UNKNOWN_METHOD) {
        Loop {
            if (cc>0) {
                if (fp!=NULL) fwrite(mbuf.buf, cc, 1, fp);
                lp = get_protocol_header_list_seq(lp, mbuf, ':', TRUE, TRUE);
                sz += cc;
                /*
                if (sz==cc) {       // is HTTPS?
                    tList* ll = find_tList_top(lp);
                    if (get_http_header_method(ll)==HTTP_UNKNOWN_METHOD) {
                        connect = HTTP_HEADER_NOT_HTTP;
                        *pl = ll;
                        break;
                    }
                }*/
            }
            else {
                connect = FALSE;
                break;
            }

            *pl = find_tList_top(lp);
            if (strncasecmp_tList(*pl, (char*)HDLIST_END_KEY, 0, 1)!=NULL) break;
            //
            cc = ssl_tcp_recv_Buffer_wait(sofd, ssl, &mbuf, tm);
        }
    }
    else {
        if (mbuf.vldsz>0) {
            Buffer key = make_Buffer_str(HDLIST_CONTENTS_KEY);
            *pl = add_tList_node_Buffer(NULL, key, mbuf);
            free_Buffer(&key);
        }
        *len = HTTP_HEADER_NOT_HTTP;
        connect = FALSE;
    }

    free_Buffer(&mbuf);
    if (*pl!=NULL) (*pl)->ldat.id = com;
    if (state!=NULL) *state = connect;
    //
    if (sz==0 && cc==0) return 0;      // 正常切断
    if (sz==0 && cc<0)  return JBXL_SSL_RECV_ERROR;
    if (*pl==NULL)      return JBXL_SSL_RECV_ERROR;
    if (*len==HTTP_HEADER_NOT_HTTP) return cc;

    // コンテンツの長さを得る．
    /*
    Buffer hbuf = search_protocol_header(*pl, (char*)HDLIST_FIRST_LINE_KEY, 1);
    if (hbuf.buf!=NULL) {
        if (!strncasecmp((char*)hbuf.buf, "GET ", 4)) {
            *len = 0;
        }
        free_Buffer(&hbuf);
    }*/

    if (com==HTTP_GET_METHOD) *len = 0;

    if (*len==HTTP_HEADER_UNKNOWN_LEN) {
        if (is_http_header_field(*pl, (char*)"Transfer-Encoding", (char*)"chunked", 1)) {
            *len = HTTP_HEADER_CHUNKED;
        }
    }

    if (*len==HTTP_HEADER_UNKNOWN_LEN) {
        Buffer hbuf = search_protocol_header(*pl, (char*)"Content-Length", 1);
        if (hbuf.buf!=NULL) {
            *len = atoi((const char*)hbuf.buf);
            free_Buffer(&hbuf);
        }
    }

    if (*len==HTTP_HEADER_UNKNOWN_LEN) {
        if (get_http_version_num(*pl)<=1.0 && get_http_status_num(*pl)>=200) {
            if (is_http_header_field(*pl, (char*)"Connection", (char*)"close", 1)) {
                *len = HTTP_HEADER_CLOSED_SESSION;
            }
        }
    }

    return sz;
}


/**
int  recv_https_content(int sofd, SSL* ssl, Buffer* buf, int len, int tm, FILE* fp, int* state)

HTTPSメッセージで recv_https_header() で受信したヘッダに引き続いて，コンテンツを受信する．

fp がNULLでなければ，受信データ（コンテンツ）はファイルにも保存される．

@param          sofd   接続相手へのソケット
@param          ssl    接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param[in,out]  buf    全コンテンツを保存する変数．最初に，recv_https_header()で受信したコンテンツ部分を入れて置く．
@param          len    受信したヘッダの "Content-Length" の値．受信データのサイズのチェックに使用する．
@param          tm     タイムアウト秒数．
@param          fp     受信したコンテンツを保存するファイルディスクリプタ．NULLなら保存しない．
@param[out]     state  サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval 0以上   全コンテンツのサイズ(Byte)．recv_https_header()で受信したコンテンツ部分を含む．
@retval 0未満   エラー
*/
int  recv_https_content(int sofd, SSL* ssl, Buffer* buf, int len, int tm, FILE* fp, int* state)
{
    int cc, sz;
    Buffer rcv;

    if (state!=NULL) *state = TRUE;
    sz = buf->vldsz;
    if (sz>0 && fp!=NULL) fwrite(buf->buf, sz, 1, fp);

    // コンテンツの残りを受信
    rcv = make_Buffer(RECVBUFSZ);
    while(sz<len) {
        //
        cc = ssl_tcp_recv_Buffer_wait(sofd, ssl, &rcv, tm);
        if (cc>0) {
            if (fp!=NULL) fwrite(rcv.buf, cc, 1, fp);
            cat_Buffer(&rcv, buf);
            sz += cc;
        }
        else {
            if (state!=NULL) *state = FALSE;
            sz = cc;
            break;
        }
        //memset(rcv.buf, 0, cc);
    }

    free_Buffer(&rcv);
    return sz;
}


/**
int  recv_https_chunked(int sofd, SSL* ssl, Buffer* buf, int tm, FILE* fp, int* state)

HTTPメッセージで recv_https_header() で受信したヘッダに引き続いて，Chunkモードのコンテンツデータを受信する．

fp がNULLでなければ，受信データ（コンテンツ）はファイルにも保存される．

@param          sofd   接続相手へのソケット
@param          ssl    接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param[in,out]  buf    全コンテンツを保存する変数．最初に，recv_https_header()で受信したコンテンツ部分を入れて置く．
@param          tm     タイムアウト秒数．
@param          fp     受信したコンテンツを保存するファイルディスクリプタ．NULLなら保存しない．
@param[out]     state  サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval 0以上   全コンテンツのサイズ(Byte)．recv_https_header()で受信したコンテンツ部分を含む．
@retval 0未満   エラー
*/
int  recv_https_chunked(int sofd, SSL* ssl, Buffer* buf, int tm, FILE* fp, int* state)
{
    int cc, sz, i;
    int chnksz, hdsz, tlsz;
    if (state!=NULL) *state = TRUE;

    Buffer tmp = make_Buffer(RECVBUFSZ);

    sz = buf->vldsz;
    if (sz<=0) {    // chunk のサイズを含んだデータを読み込む
        cc = ssl_tcp_recv_Buffer_wait(sofd, ssl, &tmp, tm);
        if (cc<=0) {
            if (state!=NULL) *state = FALSE;
            free_Buffer(&tmp);
            return cc;
        }
    }
    else {          // 既にコンテンツがある場合は tmpへ移動 
        cat_Buffer(buf, &tmp);
        clear_Buffer(buf);
    }

    //
    chnksz = sz = get_chunked_size((char*)tmp.buf, &hdsz, &tlsz);

    while (chnksz>0) {
        //
        if (chnksz+hdsz+tlsz > tmp.vldsz) {
            cc = recv_https_chunked_remain(sofd, ssl, &tmp, chnksz+hdsz+tlsz, tm);
            if (cc<=0) {
                sz = cc;
                break;
            }
        }
    
        Buffer cat = tmp;
        cat.buf = tmp.buf + hdsz;
        cat.vldsz = chnksz;
        cat_Buffer(&cat, buf);
        //tmp.buf[tmp.vldsz] = '\0';

        // 次の chunk用にデータをつめる
        for (i=0; i<tmp.vldsz-chnksz-hdsz-tlsz; i++) {
            tmp.buf[i] = tmp.buf[chnksz + hdsz + tlsz + i];
        }
        tmp.vldsz = tmp.vldsz - chnksz - hdsz - tlsz;
        //tmp.buf[tmp.vldsz] = '\0';

        if (tmp.vldsz==0) {
            cc = ssl_tcp_recv_Buffer_wait(sofd, ssl, &tmp, tm);
            if (cc<=0) {
                sz = cc;
                break;
            }
        }
        chnksz = get_chunked_size((char*)tmp.buf, &hdsz, &tlsz);
        sz += chnksz;
    }

    if (fp!=NULL) fwrite(buf->buf, buf->vldsz, 1, fp);

    free_Buffer(&tmp);
    return sz;
}


/**
int  recv_https_chunked_remain(int sofd, SSL* ssl, Buffer* buf, int chnksz, int tm)

chunk の残りのデータを読み込む．次の chunkのデータも読み込まれる可能性がある．

@param          sofd    接続相手へのソケット
@param          ssl     接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param[in,out]  buf     残りのコンテンツを保存する変数．最初に，recv_http_header()で受信したコンテンツ部分を入れて置く．
@param          chnksz  最低限読み込むべきサイズ．chunkのサイズ
@param          tm      タイムアウト秒数．

@retval 1以上   読み込んだコンテンツのサイズ(Byte)．recv_http_header()で受信したコンテンツ部分を含む．
@retval 0以下   エラー
*/
int  recv_https_chunked_remain(int sofd, SSL* ssl, Buffer* buf, int chnksz, int tm)
{
    int cc = 0;
    int sz = buf->vldsz;
    if (chnksz<=sz) return 0;

    Buffer rcv = make_Buffer(RECVBUFSZ);

    while (chnksz>sz) {
        //memset(rcv.buf, 0, cc);
        cc = ssl_tcp_recv_Buffer_wait(sofd, ssl, &rcv, tm);
        if (cc<=0) {
            sz = cc;
            break;
        }
        cat_Buffer(&rcv, buf);
        sz += cc;
    }

    free_Buffer(&rcv);
    return sz;
}


/**
int  recv_https_closed(int sofd, SSL* ssl, Buffer* buf, int tm, FILE* fp)

HTTPSメッセージで recv_https_header() で受信したヘッダに引き続いて，長さ不明のコンテンツデータを受信する．
この命令で残りの全てのヘッダを読み込む．

fp がNULLでなければ，受信データ（コンテンツ）はファイルにも保存される．

@param          sofd   接続相手へのソケット
@param          ssl    接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param[in,out]  buf    全コンテンツを保存する変数．最初に，recv_https_header()で受信したコンテンツ部分を入れて置く．
@param          tm     タイムアウト秒数．
@param          fp     受信したコンテンツを保存するファイルディスクリプタ．NULLなら保存しない．

@retval 0以上   全コンテンツのサイズ(Byte)．recv_https_header()で受信したコンテンツ部分を含む．
@retval 0未満   エラー
*/
int  recv_https_closed(int sofd, SSL* ssl, Buffer* buf, int tm, FILE* fp)
{
    int cc, sz, tout;
    Buffer rcv;

    sz = buf->vldsz;

    rcv = make_Buffer(RECVBUFSZ);
    while ((tout=recv_wait(sofd, tm))) {
        cc = ssl_tcp_recv_Buffer_wait(sofd, ssl, &rcv, tm);
        if (cc>0) {
            if (fp!=NULL) fwrite(rcv.buf, cc, 1, fp);
            cat_Buffer(&rcv, buf);
            sz += cc;
        }
        else {
            break;
        }
        //memset(rcv.buf, 0, cc);
    }
    free_Buffer(&rcv);

    if (!tout) return JBXL_NET_RECV_TIMEOUT;

    return sz;
}


/**
int  recv_https_Buffer(int sofd, SSL* ssl, tList** pl, Buffer* buf, int tsecond, int* hdonly, int* state, int nochunk)

HTTPSメッセージを受信して，Buffer変数 *bufに保存する．gzipのエンコード処理は行わない．
bufのバッファ領域は予め確保されていること．

@param       sofd      接続相手へのソケット
@param       ssl       接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param[out]  *pl       受信したヘッダが格納されるリストへのポインタ．必要ないなら NULLでも良い．
@param[out]  buf       コンテンツを格納する Buffer変数へのポインタ．バッファ部は予め確保して置く． 
@param       tsecond   最初の受信までのタイムアウト(s)
@param[out]  hdonly    データがヘッダのみの場合は TRUE, コンテンツもある場合は FALSE．NULLを指定しても良い．
@param[out]  state     サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．
@param       nochunk   chunkデータを処理しない

@retval 0以上   受信したコンテンツのサイズ(Byte)
@retval JBXL_SSL_RECV_ERROR    受信エラー
@retval JBXL_ARGS_ERROR        buf エラー
@retval JBXL_NET_RECV_TIMEOUT  タイムアウトした．
*/
int  recv_https_Buffer(int sofd, SSL* ssl, tList** pl, Buffer* buf, int tsecond, int* hdonly, int* state, int nochunk)
{
    int    cc=0, hs, len;
    int    connect;
    Buffer cnt;
    tList* lp;
    
    if (buf==NULL || buf->buf==NULL) return JBXL_ARGS_ERROR;
    if (hdonly!=NULL) *hdonly = FALSE;

    clear_Buffer(buf);

    // ヘッダの受信
    hs = recv_https_header(sofd, ssl, &lp, &len, tsecond, NULL, &connect);
    if (state!=NULL) *state = connect;
    if (hs<=0) return hs;                                           // エラー
    if (len==0 || len==HTTP_HEADER_UNKNOWN_LEN) {                   // ヘッダのみ
        if (hdonly!=NULL) *hdonly = TRUE;
        if (pl!=NULL) *pl = lp;
        else del_tList(&lp);
        return hs;
    }

    // ヘッダ中に紛れ込んだコンテンツの取り出し
    cnt = search_protocol_header(lp, (char*)HDLIST_CONTENTS_KEY, 1);
    if (cnt.buf!=NULL) {
        cc = cnt.vldsz;
        hs = hs - cc;
        copy_Buffer(&cnt, buf);
        free_Buffer(&cnt);
    }

    if (lp!=NULL && lp->ldat.id==HTTP_UNKNOWN_METHOD) {             // not HTTP
        *pl = lp;
        return cc;
    }

    // コンテンツの受信
    if (connect) {
        if (len>0) {
            cc = recv_https_content(sofd, ssl, buf, len, tsecond, NULL, &connect);
        }
        else if (len==HTTP_HEADER_CHUNKED) {
            connect = HTTP_HEADER_CHUNKED;
            if (!nochunk) {
                cc = recv_https_chunked(sofd, ssl, buf, tsecond, NULL, &connect);
            }
        }
        else {  //if (len==HTTP_HEADER_CLOSED_SESSION) {
            cc = recv_https_closed(sofd, ssl, buf, tsecond, NULL);
            connect = FALSE;
        }
    }

    // for Chunked
    if (!nochunk && is_http_header_field(lp, (char*)"Transfer-Encoding", (char*)"chunked", 1)) {
        delete_protocol_header(&lp, (char*)"Transfer-Encoding", 0); 
        char* str = itostr_ts(cc);
        add_protocol_header(lp, (char*)"Content-Length", str);   
        freeNull(str);
    }

    if (pl!=NULL) *pl = lp;
    else del_tList(&lp);

    if (cc>0) cc = cc + hs;

    if (state!=NULL) *state = connect;
    return cc;
}


/**
int  recv_https_file(int sofd, SSL* ssl, tList** pl, const char* fname, const char* wdir, int tsecond, int* hdonly, int* state)

HTTPSメッセージを受信して，コンテンツをファイルに保存する．Encodingが gzipの場合は展開する．

@param       sofd      接続相手へのソケット
@param       ssl       接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param[out]  *pl       受信したヘッダが格納されるリストへのポインタ．
@param       fname     保存するファイル名
@param       wdir      作業用ディレクトリを指定．NULLの場合は /tmp になる．
@param       tsecond   最初の受信までのタイムアウト(s)
@param[out]  hdonly    データがヘッダのみの場合は TRUE, コンテンツもある場合は FALSE．NULLを指定しても良い．
@param[out]  state     サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval 1以上  受信したファイルのサイズ(Byte)（ヘッダを含む）
@retval 0      正常切断
@retval JBXL_SSL_RECV_ERROR    受信エラー
@retval JBXL_NET_RECV_TIMEOUT  タイムアウトした．
*/
int  recv_https_file(int sofd, SSL* ssl, tList** pl, const char* fname, const char* wdir, int tsecond, int* hdonly, int* state)
{
    int    cc=0, hs, len;
    Buffer buf, cnt;
    FILE*  fp = NULL;
    const char tmpd[] = "/tmp";
    int   connect;

    if (hdonly!=NULL) *hdonly = FALSE;

    // ヘッダの受信
    hs = recv_https_header(sofd, ssl, pl, &len, tsecond, NULL, &connect);
    if (state!=NULL) *state = connect;
    if (hs<=0) return hs;                                           // エラー
    if (len==0 || len==HTTP_HEADER_UNKNOWN_LEN) {                   // ヘッダのみ
        if (hdonly!=NULL) *hdonly = TRUE;
        return hs;
    }

    //PRINT_MESG("++ RECV HEADER ++++++++++\n");
    //print_tList(stderr, *pl);
    //PRINT_MESG("+++++++++++++++++++++++++\n");

    // ヘッダ中に紛れ込んだコンテンツの取り出し
    buf = make_Buffer(RECVBUFSZ);
    cnt = search_protocol_header(*pl, (char*)HDLIST_CONTENTS_KEY, 1);
    if (cnt.buf!=NULL) {
        /*DEBUG_MODE {
            if (len==HTTP_HEADER_CHUNKED) {
                char* line = get_line((char*)cnt.buf, 1);
                int chnksz = hexstr2i(line);
                PRINT_MESG("RECV_HTTPS_FILE: INFO: Chunk Size in Header = %d\n", chnksz);
                freeNull(line);
            }
        }*/
        cc = cnt.vldsz;
        hs = hs - cc;
        copy_Buffer(&cnt, &buf);
        free_Buffer(&cnt);
    }

    // コンテンツの受信
    if (connect) {
        if (len>0) {
            cc = recv_https_content(sofd, ssl, &buf, len, tsecond, NULL, &connect);
        }
        else if (len==HTTP_HEADER_CHUNKED) {
            cc = recv_https_chunked(sofd, ssl, &buf, tsecond, NULL, &connect);
            //PRINT_MESG("+++++++++ RECVSZ = %d\n", buf.vldsz);
        }
        else { //if (len==HTTP_HEADER_CLOSED_SESSION) {
            cc = recv_https_closed(sofd, ssl, &buf, tsecond, NULL);
            connect = FALSE;
        }
    }

    if (cc>0) {
        // コンテンツをファイルへ保存
        if (fname!=NULL) fp = fopen(fname, "wb");
        if (fp!=NULL) {
            fwrite(buf.buf, buf.vldsz, 1, fp);
            fclose(fp);
        }
        free_Buffer(&buf);

        //
        if (fname!=NULL) {
            buf = search_protocol_header(*pl, (char*)"Content-Encoding", 1);

            // Encoding 処理
            if (buf.buf!=NULL) {
                // gzip or deflate
                if (!strncasecmp((const char*)buf.buf, "gzip",    4) ||
                    !strncasecmp((const char*)buf.buf, "deflate", 7)) {
                    #ifdef DISABLE_ZLIB
                        DEBUG_MODE PRINT_MESG("RECV_HTTPS_FILE: WARNING: Content-Encoding is [%s]. But zlib is not installed!!\n", buf.buf);
                    #else
                        if (wdir==NULL) wdir = tmpd;
                        cc = gz_decode_file_replace(fname, wdir);
                        delete_protocol_header(pl, (char*)"Content-Encoding",  0);

                        // for chunked
                        if (is_http_header_field(*pl, (char*)"Transfer-Encoding", (char*)"chunked", 1)) {
                            delete_protocol_header(pl, (char*)"Transfer-Encoding", 0);  
                            char* str = itostr_ts(cc);
                            add_protocol_header(*pl, (char*)"Content-Length", str);  
                            freeNull(str);
                        }
                    #endif
                }

                else { 
                    DEBUG_MODE PRINT_MESG("RECV_HTTPS_FILE: WARNING: unknown Content-Encoding [%s]\n", buf.buf);
                }
                free_Buffer(&buf);
            }
            
            // No Encofing
            else {
                // for chunked
                if (is_http_header_field(*pl, (char*)"Transfer-Encoding", (char*)"chunked", 1)) {
                    delete_protocol_header(pl, (char*)"Transfer-Encoding", 0);  
                    char* str = itostr_ts(cc);
                    add_protocol_header(*pl, (char*)"Content-Length", str);  
                    freeNull(str);
                }
            }
        }
        cc = cc + hs;
    }

    if (state!=NULL) *state = connect;
    return cc;
}


/**
int   save_https_xml(int cofd, SSL* ssl, tList** pl, tXML** xml, char** recvfn, const char* wdir, int timeout, int* state)

受信した XML通信データをファイルに保存

@param       cofd      接続相手へのソケット
@param       ssl       接続相手への SSLソケット．SSL通信でない場合は NULL を指定．
@param[out]  *pl       受信したヘッダが格納されるリストへのポインタ．
@param[out]  *xml      パースされたデータが格納されるXML構造体へのポインタ 
@param[out]  *recvfn   保存するファイル名
@param       wdir      作業用ディレクトリを指定する．NULLの場合は /tmp になる．
@param       timeout   最初の受信までのタイムアウト(s)
@param[out]  state     サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval 1以上  受信したファイルのサイズ(Byte)（ヘッダを含む）
@retval 0      正常切断
@retval JBXL_SSL_RECV_ERROR    受信エラー
@retval JBXL_NET_RECV_TIMEOUT  タイムアウトした．
*/
int   save_https_xml(int cofd, SSL* ssl, tList** pl, tXML** xml, char** recvfn, const char* wdir, int timeout, int* state)
{
    int header;

    if (pl==NULL || recvfn==NULL) return 0;

    *pl = NULL;
    if (xml!=NULL) *xml = NULL;
    *recvfn = temp_filename(wdir, WORK_FILENAME_LEN);
   
    int cc = recv_https_file(cofd, ssl, pl, *recvfn, wdir, timeout, &header, state);
    if (cc<=0 || *pl==NULL) {
        free(*recvfn);
        *recvfn = NULL;
        if (cc==0) return 0;
        return JBXL_SSL_RECV_ERROR;
    }

    if (header) {
        free(*recvfn);
        *recvfn = NULL;
    }
    else {

        if (xml!=NULL && *pl!=NULL && *recvfn!=NULL && cc>0) {
            Buffer buf = search_protocol_header(*pl, (char*)"Content-Type", 1);
            *xml = xml_parse_file(*recvfn);
            if (*xml!=NULL && (*xml)->state<0) del_all_xml(xml);
            free_Buffer(&buf);
        }
    }

    return  cc;
}


#endif  //  ENABLE_SSL
