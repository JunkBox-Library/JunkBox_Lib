/** 
@brief   HTTP ツールライブラリ
@file    http_tool.c
@author  Fumi.Iseki (C)

@attention
opensslライブラリが無くても使用できるように https_tool とは分離
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include "http_tool.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic Function.  
//

/**
int  send_http_header(int sofd, tList* pp, int mode)

ヘッダリストの内容を HTTP通信で送信する．
ヘッダ中の Content-Length の値は操作しない．

@param  sofd    接続相手へのソケット
@param  pp      送信するヘッダが格納されるリストへのポインタ．
@param  mode    @b ON:  ヘッダリスト内にコンテンツの一部があればそれも送信する．
@param  mode    @b OFF: ヘッダリスト内にコンテンツの一部があっても送信しない．

@retval  0以上  mode==OFF: 送信した全データバイト数．
                mode==ON : 送信したコンテンツのバイト数．
@retval  0未満  エラー
@retval  JBXL_ARGS_ERROR  リストへのポインタが NULL

@code
    tList* http_header = NULL;
    tList* http_length = NULL;
    tList* http_data   = NULL;
    tList* pp = NULL;

    pp = http_header = add_tList_node_bystr(pp, 0, 0, HDLIST_FIRST_LINE_KEY, "POST /test.php HTTP/1.1", NULL, 0);
    pp               = add_tList_node_bystr(pp, 0, 0, "Content-Type", "application/x-www-form-urlencoded", NULL, 0);
    pp               = add_tList_node_bystr(pp, 0, 0, "Connection", "close",  NULL, 0);
    pp = http_length = add_tList_node_bystr(pp, 0, 0, "Content-Length", "", NULL, 0);
    pp               = add_tList_node_bystr(pp, 0, 0, HDLIST_END_KEY, "",  NULL, 0);
    pp = http_data   = add_tList_node_bystr(pp, 0, 0, HDLIST_CONTENTS_KEY,  "", NULL, 0);

    copy_i2Buffer((int)buf.vldsz, &(http_length->ldat.val));
    copy_Buffer(&buf, &(http_data->ldat.val));
    send_http_header(sock, http_header, ON);
@endcode
*/
int  send_http_header(int sofd, tList* pp, int mode)
{
    int    hs=0, sz=0;
    Buffer buf;

    if (pp==NULL) return JBXL_ARGS_ERROR;

    buf = restore_protocol_header(pp, (char*)": ", mode, &hs);
    if (buf.vldsz>0) {
        sz  = tcp_send_Buffer(sofd, &buf);
        if (mode==ON) sz = sz - hs;
    }

    free_Buffer(&buf);
    return sz;
}


Buffer  rebuild_http_Buffer(tList* pl, Buffer* buf)
{
    Buffer rbd;
    int nobody = FALSE;

    if (pl==NULL) {
        rbd = dup_Buffer(*buf);
        return rbd;
    }
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

    rbd = restore_protocol_header(pl, (char*)": ", OFF, NULL);
    if (!nobody) cat_Buffer(buf, &rbd);

    return rbd;
}


/**
int  send_http_Buffer(int sofd, tList* pl, Buffer* buf)

ヘッダとバッファの内容を HTTP通信で送信する．

@param  sofd   接続相手へのソケット
@param  pl     ファイルに先立って送信されるヘッダが格納されるリストへのポインタ．
@param  buf    送信するコンテンツが格納された Buffer変数へのポインタ．

@retval  0以上 送信したデータの全サイズ(Byte)
@retval  JBXL_ARGS_ERROR  リストへのポインタが NULL
*/
int  send_http_Buffer(int sofd, tList* pl, Buffer* buf)
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
    sz  = tcp_send_Buffer(sofd, &snd);
    free_Buffer(&snd);

    return sz;
}


/**
int  send_http_file(int sofd, tList* pp, const char* fname)

ヘッダとファイルの内容を HTTP通信で送信する．

@param  sofd   接続相手へのソケット
@param  pl     ファイルに先立って送信されるヘッダが格納されるリストへのポインタ．
@param  fname  送信するファイル名．ヘッダのみ送る場合は NULL

@retval  0以上 送信したデータの全サイズ(Byte)
@retval  JBXL_ARGS_ERROR    リストへのポインタが NULL
@retval  JBXL_MALLOC_ERROR  メモリ確保エラー
*/
int  send_http_file(int sofd, tList* pl, const char* fname)
{
    int   sz = 0;
    FILE* fp = NULL;
    char  num[20];
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

    sz = tcp_send_Buffer(sofd, &buf);
    free_Buffer(&buf);

    return sz;
}


/**
int  recv_http_header(int sofd, tList** pl, int* len, int tm, FILE* fp, int* state)

HTTP通信のヘッダ部分を受信して，リストに格納する．@n
NULLでない fp が指定された場合，受信した全てのデータをファイルに保存する．
受信したデータが HTTPでない場合は，*pl の HDLIST_CONTENTS_KEY の値として受信データを格納する．

@param       sofd   接続相手へのソケット
@param[out]  *pl    ヘッダ情報が格納されるリストへのポインタ．NULLなら格納しない．
                    (*pl)->ldat.id に HTTPデータのコマンド種別が入る．(HTTP_GET_METHOD など)
@param[out]  len    ヘッダ中の "Content-Length" の値（正の値）. 
                    "Content-Length" が存在しなければ @b HTTP_HEADER_UNKNOWN_LEN (0未満)．
                    チャンク形式なら @b HTTP_HEADER_CHUNKED (0未満)．
                    引き続く1回のセッション内でボディを転送するのなら，@b HTTP_HEADER_CLOSED_SESSION (0未満)．
                    受信データが HTTP でないなら @b HTTP_HEADER_NOT_HTTP @b (0未満)．
@param       tm     タイムアウト秒数．
@param       fp     受信した全てのデータを保存するためのファイルポインタ．NULLの場合は保存しない．
@param[out]  state  サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval 0以上  受信した全バイト数
@retval 0      正常切断
@retval JBXL_NET_RECV_ERROR  受信エラー
@retval JBXL_ARGS_ERROR      無効な sofdを指定した または len にNULL を指定した
*/
int  recv_http_header(int sofd, tList** pl, int* len, int tm, FILE* fp, int* state)
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

    cc  = tcp_recv_Buffer_wait(sofd, &mbuf, tm);
    com = get_http_method((char*)mbuf.buf);

    if (com>HTTP_UNKNOWN_METHOD) {
        Loop {
            if (cc>0) {
                if (fp!=NULL) fwrite(mbuf.buf, cc, 1, fp);
                lp = get_protocol_header_list_seq(lp, mbuf, ':', TRUE, TRUE);
                sz += cc;
                /*
                if (sz==cc) {       // is HTTP ?
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
            //
            *pl = find_tList_top(lp);
            if (strncasecmp_tList(*pl, (char*)HDLIST_END_KEY, 0, 1)!=NULL) break;
            //
            cc  = tcp_recv_Buffer_wait(sofd, &mbuf, tm);
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
    if (sz==0 && cc<0)  return JBXL_NET_RECV_ERROR;
    if (*pl==NULL)      return JBXL_NET_RECV_ERROR;
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
int  recv_http_content(int sofd, Buffer* buf, int len, int tm, FILE* fp, int* state)

HTTPメッセージで recv_http_header() で受信したヘッダに引き続いて，コンテンツを受信する．

fp がNULLでなければ，受信データ（コンテンツ）はファイルにも保存される．

@param          sofd   接続相手へのソケット
@param[in,out]  buf    全コンテンツを保存する変数．最初に，recv_https_header()で受信したコンテンツ部分を入れて置く．
@param          len    受信したヘッダの "Content-Length" の値．受信データのサイズのチェックに使用する．
@param          tm     タイムアウト秒数．
@param          fp     受信したコンテンツを保存するファイルディスクリプタ．NULLなら保存しない．
@param[out]     state  サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval 0以上   全コンテンツのサイズ(Byte)．recv_https_header()で受信したコンテンツ部分を含む．
@retval 0未満   エラー
*/
int  recv_http_content(int sofd, Buffer* buf, int len, int tm, FILE* fp, int* state)
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
        cc = tcp_recv_Buffer_wait(sofd, &rcv, tm);
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
int  recv_http_chunked(int sofd, Buffer* buf, int tm, FILE* fp, int* state)

HTTPメッセージで recv_http_header() で受信したヘッダに引き続いて，Chunkモードのコンテンツデータを受信する．

fp がNULLでなければ，受信データ（コンテンツ）はファイルにも保存される．

@param          sofd   接続相手へのソケット
@param[in,out]  buf    全コンテンツを保存する変数．最初に，recv_https_header()で受信したコンテンツ部分を入れて置く．
@param          tm     タイムアウト秒数．
@param          fp     受信したコンテンツを保存するファイルディスクリプタ．NULLなら保存しない．
@param[out]     state  サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval 0以上   全コンテンツのサイズ(Byte)．recv_https_header()で受信したコンテンツ部分を含む．
@retval 0未満   エラー
*/
int  recv_http_chunked(int sofd, Buffer* buf, int tm, FILE* fp, int* state)
{
    int cc, sz, i;
    int chnksz, hdsz, tlsz;
    if (state!=NULL) *state = TRUE;

    Buffer tmp = make_Buffer(RECVBUFSZ);

    sz = buf->vldsz;
    if (sz<=0) {    // chunk のサイズを含んだデータを読み込む
        cc = tcp_recv_Buffer_wait(sofd, &tmp, tm);
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
            cc = recv_http_chunked_remain(sofd, &tmp, chnksz+hdsz+tlsz, tm);
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
            cc = tcp_recv_Buffer_wait(sofd, &tmp, tm);
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
int  recv_http_chunked_remain(int sofd, Buffer* buf, int chnksz, int tm)

chunk の残りのデータを読み込む．次の chunkのデータも読み込まれる可能性がある．

@param          sofd    接続相手へのソケット
@param[in,out]  buf     残りのコンテンツを保存する変数．最初に，recv_http_header()で受信したコンテンツ部分を入れて置く．
@param          chnksz  最低限読み込むべきサイズ．chunkのサイズ
@param          tm      タイムアウト秒数．

@retval 1以上   読み込んだコンテンツのサイズ(Byte)．recv_http_header()で受信したコンテンツ部分を含む．
@retval 0以下   エラー
*/
int  recv_http_chunked_remain(int sofd, Buffer* buf, int chnksz, int tm)
{
    int cc = 0;
    int sz = buf->vldsz;
    if (chnksz<=sz) return 0;

    Buffer rcv = make_Buffer(RECVBUFSZ);

    while (chnksz>sz) {
        //memset(rcv.buf, 0, cc);
        cc = tcp_recv_Buffer_wait(sofd, &rcv, tm);
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
int  recv_http_closed(int sofd, Buffer* buf, int tm, FILE* fp)

HTTPメッセージで recv_http_header() で受信したヘッダに引き続いて，長さ不明のコンテンツデータを受信する．
この命令で残りの全てのヘッダを読み込む．

fp がNULLでなければ，受信データ（コンテンツ）はファイルにも保存される．

@param          sofd   接続相手へのソケット
@param[in,out]  buf    全コンテンツを保存する変数．最初に，recv_https_header()で受信したコンテンツ部分を入れて置く．
@param          tm     タイムアウト秒数．
@param          fp     受信したコンテンツを保存するファイルディスクリプタ．NULLなら保存しない．

@retval 0以上   全コンテンツのサイズ(Byte)．recv_https_header()で受信したコンテンツ部分を含む．
@retval 0未満   エラー
*/
int  recv_http_closed(int sofd, Buffer* buf, int tm, FILE* fp)
{
    int cc, sz, tout;
    Buffer rcv;

    sz = buf->vldsz;

    rcv = make_Buffer(RECVBUFSZ);
    while ((tout=recv_wait(sofd, tm))){
        cc = tcp_recv_Buffer(sofd, &rcv);
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
int  recv_http_Buffer(int sofd, tList** pl, Buffer* buf, int tsecond, int* hdonly, int* state, int nochunk)

HTTPメッセージを受信して，Buffer変数 *bufに保存する．gzipのエンコード処理は行わない．

bufのバッファ領域は予め確保されていること．

@param       sofd      接続相手へのソケット
@param[out]  *pl       受信したヘッダが格納されるリストへのポインタ．必要ないなら NULLでも良い．
@param[out]  buf       コンテンツを格納する Buffer変数へのポインタ．バッファ部は予め確保して置く． 
@param       tsecond   最初の受信までのタイムアウト(s)
@param[out]  hdonly    データがヘッダのみの場合は TRUE, コンテンツもある場合は FALSE．NULLを指定しても良い．
@param[out]  state     サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．
@param       nochunk   chunkデータを処理しない

@retval 0以上         受信したコンテンツのサイズ(Byte)
@retval JBXL_NET_RECV_ERROR   受信エラー
@retval JBXL_ARGS_ERROR        buf エラー
@retval JBXL_NET_RECV_TIMEOUT  タイムアウトした．
*/
int  recv_http_Buffer(int sofd, tList** pl, Buffer* buf, int tsecond, int* hdonly, int* state, int nochunk)
{
    int    cc=0, hs, len;
    int    connect;
    Buffer cnt;
    tList* lp;

    if (buf==NULL || buf->buf==NULL) return JBXL_ARGS_ERROR;
    if (hdonly!=NULL) *hdonly = FALSE;

    clear_Buffer(buf);

    // ヘッダの受信
    hs = recv_http_header(sofd, &lp, &len, tsecond, NULL, &connect);
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
            cc = recv_http_content(sofd, buf, len, tsecond, NULL, &connect);
        }
        else if (len==HTTP_HEADER_CHUNKED) {
            connect = HTTP_HEADER_CHUNKED;
            if (!nochunk) {
                cc = recv_http_chunked(sofd, buf, tsecond, NULL, &connect);
            }
        }
        else {  //if (len==HTTP_HEADER_CLOSED_SESSION) {
            cc = recv_http_closed(sofd, buf, tsecond, NULL);
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
int  recv_http_file(int sofd, tList** pl, const char* fname, const char* wdir, int tsecond, int* hdonly, int* state)

HTTPメッセージを受信して，コンテンツをファイルに保存する．Encodingが gzipの場合は展開する．

@param       sofd      接続相手へのソケット
@param[out]  *pl       受信したヘッダが格納されるリストへのポインタ．
@param       fname     保存するファイル名
@param       wdir      作業用ディレクトリを指定．NULLの場合は /tmp になる．
@param       tsecond   最初の受信までのタイムアウト(s)
@param[out]  hdonly    データがヘッダのみの場合は TRUE, コンテンツもある場合は FALSE．NULLを指定しても良い．
@param[out]  state     サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval  1以上  受信したファイルのサイズ(Byte)（ヘッダを含む）
@retval  0      正常切断
@retval  JBXL_NET_RECV_ERROR    受信エラー
@retval  JBXL_NET_RECV_TIMEOUT  タイムアウトした．
*/
int  recv_http_file(int sofd, tList** pl, const char* fname, const char* wdir, int tsecond, int* hdonly, int* state)
{
    int    cc=0, hs, len;
    Buffer buf, cnt;
    FILE*  fp = NULL;
    const char tmpd[] = "/tmp";
    int   connect;

    if (hdonly!=NULL) *hdonly = FALSE;

    // ヘッダの受信
    hs = recv_http_header(sofd, pl, &len, tsecond, NULL, &connect);
    if (state!=NULL) *state = connect;
    if (hs<=0)  return hs;                                          // エラー
    if (len==0 || len==HTTP_HEADER_UNKNOWN_LEN) {                   // ヘッダのみ
        if (hdonly!=NULL) *hdonly = TRUE;
        return hs;
    }

    // ヘッダ中に紛れ込んだコンテンツの取り出し
    buf = make_Buffer(RECVBUFSZ);
    cnt = search_protocol_header(*pl, (char*)HDLIST_CONTENTS_KEY, 1);
    if (cnt.buf!=NULL) {
        /*DEBUG_MODE {
            if (len==HTTP_HEADER_CHUNKED) {
                char* line = get_line((char*)cnt.buf, 1);
                int chnksz = hexstr2i(line);
                PRINT_MESG("RECV_HTTP_FILE: INFO: Chunk Size in Header = %d\n", chnksz);
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
            cc = recv_http_content(sofd, &buf, len, tsecond, NULL, &connect);
        }
        else if (len==HTTP_HEADER_CHUNKED) {
            cc = recv_http_chunked(sofd, &buf, tsecond, NULL, &connect);
        }
        else {  //if (len==HTTP_HEADER_CLOSED_SESSION) {
            cc = recv_http_closed(sofd, &buf, tsecond, NULL);
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
                        DEBUG_MODE PRINT_MESG("RECV_HTTP_FILE: WARNING: Content-Encoding is [%s]. But zlib is not installed!!\n", buf.buf);
                    #else
                        if (wdir==NULL) wdir = tmpd;
                        cc = gz_decode_file_replace(fname, wdir);
                        delete_protocol_header(pl, (char*)"Content-Encoding", 0);

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
                    DEBUG_MODE PRINT_MESG("RECV_HTTP_FILE: WARNING: unknown Content-Encoding [%s]\n", buf.buf);
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
int   save_http_xml(int cofd, tList** pl, tXML** xml, char** recvfn, const char* wdir, int timeout, int* state)

受信した XML通信データをファイルに保存

@param       cofd      接続相手へのソケット
@param[out]  *pl       受信したヘッダが格納されるリストへのポインタ．
@param[out]  *xml      パースされたデータが格納されるXML構造体へのポインタ 
@param[out]  *recvfn   保存するファイル名
@param       wdir      作業用ディレクトリを指定する．NULLの場合は /tmp になる．
@param       timeout   最初の受信までのタイムアウト(s)
@param[out]  state     サーバとの接続状態．接続中なら TRUE．切断した場合は FALSE．NULLを指定しても良い．

@retval 1以上  受信したファイルのサイズ(Byte)（ヘッダを含む）
@retval 0      正常切断
@retval JBXL_NET_RECV_ERROR   受信エラー
@retval JBXL_NET_RECV_TIMEOUT  タイムアウトした．
*/
int   save_http_xml(int cofd, tList** pl, tXML** xml, char** recvfn, const char* wdir, int timeout, int* state)
{
    int header;

    if (pl==NULL || recvfn==NULL) return 0;

    *pl = NULL;
    if (xml!=NULL) *xml = NULL;
    *recvfn = temp_filename(wdir, WORK_FILENAME_LEN);
   
    int cc = recv_http_file(cofd, pl, *recvfn, wdir, timeout, &header, state);
    if (cc<=0 || *pl==NULL) {
        free(*recvfn);
        *recvfn = NULL;
        if (cc==0) return 0;
        return JBXL_NET_RECV_ERROR;
    }

    if (header) {
        free(*recvfn);
        *recvfn = NULL;
    }
    else {
        if (xml!=NULL && *pl!=NULL && *recvfn!=NULL && cc>0) {
            Buffer buf = search_protocol_header(*pl, (char*)"Content-Type", 1);
            *xml = xml_parse_file(*recvfn);
            if (*xml!=NULL && (*xml)->state<0) del_xml(xml);
            free_Buffer(&buf);
        }
    }
      
    return  cc;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tools
//

int  get_http_method(char* data)
{
    if      (ex_strcmp("GET ", data))       return HTTP_GET_METHOD;
    else if (ex_strcmp("PUT ", data))       return HTTP_PUT_METHOD;
    else if (ex_strcmp("POST ", data))      return HTTP_POST_METHOD;
    else if (ex_strcmp("DELETE ", data))    return HTTP_DELETE_METHOD;
    else if (ex_strcmp("OPTIONS ", data))   return HTTP_OPTIONS_METHOD;
    else if (ex_strcmp("HEAD ", data))      return HTTP_HEAD_METHOD;
    else if (ex_strcmp("TRACE ", data))     return HTTP_TRACE_METHOD;
    else if (ex_strcmp("CONNECT ", data))   return HTTP_CONNECT_METHOD;
    else if (ex_strcmp("PATCH ", data))     return HTTP_PATCH_METHOD;
    else if (ex_strcmp("LINK ", data))      return HTTP_LINK_METHOD;
    else if (ex_strcmp("UNLINK ", data))    return HTTP_UNLINK_METHOD;
    else if (ex_strcmp("HTTP/", data))      return HTTP_RESPONSE_METHOD;
    // WebDAV
    else if (ex_strcmp("PROPFIND ", data))  return HTTP_PROPFIND_METHOD;
    else if (ex_strcmp("PROPPATCH ", data)) return HTTP_PROPPATCH_METHOD;
    else if (ex_strcmp("MKCOL ", data))     return HTTP_MKCOL_METHOD;
    else if (ex_strcmp("COPY ", data))      return HTTP_COPY_METHOD;
    else if (ex_strcmp("MOVE ", data))      return HTTP_MOVE_METHOD;
    else if (ex_strcmp("LOCK ", data))      return HTTP_LOCK_METHOD;
    else if (ex_strcmp("UNLOCK ", data))    return HTTP_UNLOCK_METHOD;

    return HTTP_UNKNOWN_METHOD;
}


/**
int  is_http_header_field(tList* pl, char* field, char* value, int n)

HTTPのヘッダ部分で n番目の filed の値が valueなら TRUEを返し，違っていれば FALSEを返す．

@param  pl     ヘッダ情報が格納されるリストへのポインタ
@param  field  ヘッダ中の フィールド名．
@param  value  比較するフィールドの値．
@param  n      フィールドが複数あった場合は n番目のフィールドと比較する

@retval TRUE   フィールドの値が一致した．
@retval FALSE  フィールドの値が一致しなかった．
*/
int  is_http_header_field(tList* pl, char* field, char* value, int n)
{
    int ret = FALSE;

    Buffer hbuf = search_protocol_header(pl, field, n);
    if (hbuf.buf!=NULL) {
        if (!strcasecmp((char*)hbuf.buf, value)) {
            ret = TRUE;
        }
        free_Buffer(&hbuf);
    }

    return ret;
}


/**
int  get_http_header_method(tList* pl)

ヘッダ情報の格納された plから，HTTPコマンドとパスを取り出す．
*/
int  get_http_header_method(tList* pl)
{
    int ret = HTTP_UNKNOWN_METHOD;

    if (pl==NULL) return ret;

    Buffer hbuf = search_protocol_header(pl, (char*)HDLIST_FIRST_LINE_KEY, 1);
    if (hbuf.buf!=NULL) ret = get_http_method((char*)hbuf.buf);

    return ret;
}


/**
void  set_http_host_header(tList* pl, const char* hname, unsigned short hport)

HTTPのヘッダ(pl)の Hostを hname:hport に書き換える．
Hostヘッダが無い場合は，最後に追加．
*/
void  set_http_host_header(tList* pl, char* hname, unsigned short hport)
{
    Buffer buf;
    char   num[LEN_INT];
    
    if (pl==NULL) return;

    buf = make_Buffer(LBUF);
    snprintf(num, LEN_INT-1, "%d", hport);
    copy_s2Buffer(hname, &buf);
    cat_s2Buffer (":",   &buf);
    cat_s2Buffer (num,   &buf);

    set_protocol_header(pl, (char*)"Host", (char*)buf.buf, 1, ON);

    free_Buffer(&buf);
    return;
}


/**
void  dump_http_header(FILE* fp, tList* pp)

HTTPのヘッダ(pp)の内容をダンプする．
*/
void  dump_http_header(FILE* fp, tList* pp)
{
    if (fp==NULL) fp = stderr;

    if (pp!=NULL) {
        int image = FALSE;
        while(pp!=NULL) {
            tList_data ld = pp->ldat;
            if (!strcasecmp((const char*)ld.key.buf, "Content-Type")) {
                if (!strncasecmp((const char*)ld.val.buf, "image", 5)) image = TRUE;
                //else if (!strncasecmp((unsigned char*)ld.val.buf, "xxxx", 5)) image = TRUE;
            }

            if (!strcasecmp((const char*)ld.key.buf, HDLIST_CONTENTS_KEY) && image==TRUE) {
                fprintf(fp, "[%d] [%d] [%s] [%d]\n", ld.id, ld.lv, ld.key.buf, ld.val.vldsz);
                fdump(fp, (unsigned char*)ld.val.buf, ld.val.vldsz);
            }
            else {
                fprintf(fp, "[%d] [%d] [%s] [%s]\n", ld.id, ld.lv, ld.key.buf, ld.val.buf);
            }
            pp = pp->next;
        }
    }
    else {
        fprintf(fp, "(List is NULL)\n");
    }
    return;
}


/**
int  get_chunked_size(char* ptr, int* hdsz, int* tlsz)

データの先頭のチャンクのサイズを得る．
hdsz にChunkで使用されるデータのヘッダのサイズが入る．
tlsz にChunkで使用されるデータのトレーラのサイズが入る．

*hdsz + ck + *tlsz が全データサイズ
*/
int  get_chunked_size(char* ptr, int* hdsz, int* tlsz)
{
    int  ck = 0, hd = 0, tl = 0;
    char hex[L_32];

    int i = 0;
    while (ptr[i]!=0x0d && i<L_32) {
        hex[i] = ptr[i];
        i++;
    }
    if (i==L_32) {
        if (hdsz!=NULL) *hdsz = 0;
        if (tlsz!=NULL) *tlsz = 0;
        return -1;
    }

    if (ptr[i+1]==0x0a) hd = i + 2;
    else                hd = i + 1;
    if (hdsz!=NULL) *hdsz = hd;

    hex[i] = '\0';
    ck = hexstr2i(hex);

    if (ptr[ck+hd]==0x0d) {
        tl++;
        if (ptr[ck+hd+tl]==0x0a) tl++;
    }
    if (tlsz!=NULL) *tlsz = tl;

    return ck; 
}


// need free()
char*  get_http_header_date(time_t date) 
{
    char* buf = get_gmt_timestamp(date, "%a, %d %b %Y %H:%M:%S %Z");

    return buf;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// HTTP Simple Proxy
//

/**
void  simple_web_proxy(int bofd, char* myip, int tmout)

簡易WEBプロキシ．WWW通信をリレーする．

全てのHTTPプロトコルをサポートしている訳ではない．
接続はできる限りの範囲でしか維持しない．

@param  bofd   ブラウザとの通信ソケット
@param  myip   自分のIPアドレス．
@param  tmout  タイムアウト(sec)

@attention
呼び出し側でソケット bofdをクローズすること．
これを使用する特別の理由が無い限りは「ヤリイカ」などを使用してください．
*/
void  simple_web_proxy(int bofd, char* myip, int tmout)
{
    int    wofd = 0;
    int    len, nd, cc, state;
    int    btm, wtm, keep;
    char*  ipa;
    tList* pl;

    unsigned short sport = 0;
    Buffer  server, com;

    fd_set  mask;
    struct timeval timeout;

    //DEBUG_MODE PRINT_MESG("[%d] SIMPLE_WEB_PROXY: Start Simple WEB Proxy.\n", (int)getpid());

    //init_rand();
    if (tmout<0) tmout = HTTP_PROXY_TIMEOUT;
    btm = wtm = tmout;
    keep = TRUE;

    do {
        timeout.tv_sec  = btm*10;
        timeout.tv_usec = 0;
        FD_ZERO(&mask); 
        FD_SET(bofd, &mask);
        nd = select(bofd+1, &mask, NULL, NULL, &timeout);
    } while (nd<0);

    while (FD_ISSET(bofd, &mask) && keep) {
        cc = recv_http_header(bofd, &pl, &len, tmout, NULL, &state);
        if (cc<=0) break;
        if (pl==NULL) break;

        /*
        DEBUG_MODE {
            PRINT_MESG("[%d] --- Web Proxy Request ---\n", getpid());
            print_tList(stderr, pl);
            PRINT_MESG("\n");
        }*/

        com = http_proxy_header_analyze(pl, &server, &sport, &btm, &keep);
        if (btm==0) btm = tmout;
        else        btm = Min(btm, 60);

        wofd = 0;
        if (server.buf!=NULL && sport!=0) {
            ipa = get_ipaddr_byname((char*)server.buf, AF_INET);
            if (ipa!=NULL && !strcmp(ipa, myip)) {
                free_Buffer(&server);
                server = make_Buffer_bystr("127.0.0.1");
            }
            freeNull(ipa);

            wofd = tcp_client_socket((char*)server.buf, sport);
            free_Buffer(&server);
        }

        if (wofd<=0) {
            free_Buffer(&com);
            del_tList(&pl);
            break;
        }

        // for HTTPS
        if (com.buf!=NULL && !strcasecmp((char*)com.buf, "CONNECT")) {
            cc = tcp_send(bofd, (char*)"HTTP/1.1 200 Connection established.\r\nProxy-Connection: close\r\n\r\n", 0);
            cc = tcp_relay(bofd, wofd, 5);

            del_tList(&pl);
            free_Buffer(&com);
            break;
        }
        free_Buffer(&com);

        cc = send_http_header(wofd, pl, ON);
        if (cc<len && state) {
            Buffer buf = make_Buffer(RECVBUFSZ);
            cc = recv_http_content(bofd, &buf, len-cc, wtm, NULL, &state);
            if (cc<=0) {
                del_tList(&pl);
                break;
            }

            tcp_send_Buffer(wofd, &buf);
            free_Buffer(&buf);
        }
        del_tList(&pl);

        cc = www2browser_relay(bofd, wofd, btm, wtm, keep);
        if (cc<=0) break;

        do {
            timeout.tv_sec  = btm;
            timeout.tv_usec = 0;
            FD_ZERO(&mask); 
            FD_SET(bofd, &mask);
            nd = select(bofd+1, &mask, NULL, NULL, &timeout);
        } while (nd<0);
    }

    socket_close(wofd);

    return;
}


/**
Buffer  http_proxy_header_analyze(tList* pl, Buffer* server, unsigned short* sport, int* timeout, int* keep)

ブラウザからのプロキシ用リクエストを解析する．

@param       pl       解析するヘッダ情報の入ったリストへのポインタ
@param[out]  server   接続相手の名前またはIPアドレスが代入される
@param[out]  sport    接続相手のポート番号が代入される．
@param[out]  timeout  ブラウザからの Keep-Alive の値が代入される．
@param[out]  keep     接続を維持するかどうかが代入される．(TRUE:維持する/FALSE:維持しない)

@return ブラウザからのコマンド (GET, POST, CONNECT, ......)
*/
Buffer  http_proxy_header_analyze(tList* pl, Buffer* server, unsigned short* sport, int* timeout, int* keep)
{
    tList*  frst;
    tList*  lst;
    Buffer  cmnd, url, ptcl;
    Buffer  srvdir, bufport;

    cmnd     = init_Buffer();
    srvdir   = init_Buffer();
    *server  = init_Buffer();
    *sport   = 0;
    *timeout = 0;
    *keep    = FALSE;

    frst = strncasecmp_tList(pl, (char*)HDLIST_FIRST_LINE_KEY, 0, 1);
    if (frst!=NULL) {
        cmnd = cawk_Buffer(frst->ldat.val, ' ', 1);
        url  = cawk_Buffer(frst->ldat.val, ' ', 2);
        ptcl = cawk_Buffer(frst->ldat.val, ' ', 3);
        decomp_url(url, NULL, NULL, server, sport, &srvdir);
    }
    else return cmnd;

    if (server->buf==NULL) {
        lst = strncasecmp_tList(pl, (char*)"Host", 0, 1);
        if (lst!=NULL) {
            *server = cawk_Buffer(lst->ldat.val, ':', 1);
            bufport = cawk_Buffer(lst->ldat.val, ':', 2);
            if (bufport.buf!=NULL) *sport = atoi((char*)bufport.buf);
            free_Buffer(&bufport);
        }
    }
    if (*sport==0) *sport = 80;

    lst = strncasecmp_tList(pl, (char*)"Keep-Alive", 0, 1);
    if (lst!=NULL) {
        if (lst->ldat.val.buf!=NULL) *timeout = atoi((char*)(lst->ldat.val.buf));
        del_tList_node(&lst);
    }

    lst = strncasecmp_tList(pl, (char*)"Proxy-Connection", 0, 1);
    if (lst!=NULL) {
        if (!strcasecmp((char*)(lst->ldat.val.buf), "keep-alive")) *keep = TRUE;
        del_tList_node(&lst);
    }

    free_Buffer(&(frst->ldat.val));
    copy_Buffer (&cmnd, &(frst->ldat.val));
    cat_s2Buffer(" ", &(frst->ldat.val));
    cat_Buffer  (&srvdir, &(frst->ldat.val));
    cat_s2Buffer(" ", &(frst->ldat.val));
    cat_Buffer  (&ptcl, &(frst->ldat.val));

    free_Buffer(&url);
    free_Buffer(&ptcl);
    free_Buffer(&srvdir);

    return cmnd;
}


/**
int  www2browser_relay(int bofd, int wofd, int btm, int wtm, int keep)

WWW -> Browser の通信をリレーする．

@param  bofd  ブラウザ側のソケット
@param  wofd  WWWサーバ側のソケット
@param  btm   ブラウザとのタイムアウト
@param  wtm   WWWサーバとのタイムアウト
@param  keep  @b TRUE  接続を（可能な限り）維持する
@param  keep  @b FALSE 接続を維持しない．

@retval 0未満 エラー
@retval JBXL_NET_RECV_TIMEOUT タイムアウト
*/
int  www2browser_relay(int bofd, int wofd, int btm, int wtm, int keep)
{
    int    len, cc, cx, sz, tout, state;
    Buffer buf;
    tList* pl;
    tList* lst;

    if (!recv_wait(wofd, wtm*10)) return JBXL_NET_RECV_TIMEOUT;
    cc = recv_http_header(wofd, &pl, &len, wtm, NULL, &state);
    if (cc<=0) {
        if (pl!=NULL) del_tList(&pl);
        return cc;
    }
    if (pl==NULL) return JBXL_NET_RECV_ERROR;

    /*
    DEBUG_MODE {
        PRINT_MESG("[%d] --- Web Proxy Response --\n", getpid());
        print_tList(stderr, pl);
        PRINT_MESG("\n");
    }*/

    lst = strncasecmp_tList(pl, (char*)HDLIST_END_KEY, 0, 1);
    if (lst!=NULL && lst->prev!=NULL) {
        if (keep) {
            char   pxcn[48];
            snprintf(pxcn, 46, "keep-alive, timeout=%d", btm);
            add_tList_node_str(lst->prev, "Proxy-Connection", pxcn);
        }
        else {
            add_tList_node_str(lst->prev, "Proxy-Connection", "close");
        }
    }

    sz = cx = send_http_header(bofd, pl, ON);
    del_tList(&pl);

    if (len>0) {
        buf = make_Buffer(RECVBUFSZ);
        while(sz<len) {
            if (!recv_wait(wofd, wtm)) {
                free_Buffer(&buf);
                return JBXL_NET_RECV_TIMEOUT;
            }
            cc = tcp_recv_Buffer(wofd, &buf);
            if (cc>0) {
                sz += cc;
                cx = tcp_send_Buffer(bofd, &buf);
                if (cx<=0) break;
            }
            else break;
        }

        free_Buffer(&buf);
        if (cc<=0 || (cc>0&&cx<=0)) return 0;
    }

    else if (len==HTTP_HEADER_CHUNKED || len==HTTP_HEADER_CLOSED_SESSION) {
        buf = make_Buffer(RECVBUFSZ);
        while ((tout=recv_wait(wofd, 1))){
            cc = tcp_recv_Buffer(wofd, &buf);
            if (cc>0) {
                sz += cc;
                cx = tcp_send_Buffer(bofd, &buf);
                if (cx<=0) break;
            }
            else break;
        }

        free_Buffer(&buf);
        if (!tout) return JBXL_NET_RECV_TIMEOUT;
        return 0;
    }

    return sz;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// Alternative Tools
//

/**
int  tcp_relay(int sofd, int cofd, int tm)

TCP通信をリレーする．

@param  sofd           クライアント側ソケット
@param  cofd           サーバ側ソケット
@param  tm             タイムアウト時間（sec）

@retval 0以上          転送したバイト数
@retval 0未満          エラー
@retval JBXL_NET_RECV_TIMEOUT タイムアウト
*/
int  tcp_relay(int sofd, int cofd, int tm)
{
    int     num, nd, cx, cc = JBXL_NET_RECV_TIMEOUT;
    Buffer  buf;
    fd_set  mask;
    struct timeval timeout;

    buf = make_Buffer(RECVBUFSZ);
    num = Max(sofd, cofd);

    do {
        timeout.tv_sec  = tm;
        timeout.tv_usec = 0;
        FD_ZERO(&mask); 
        FD_SET(sofd, &mask);
        FD_SET(cofd, &mask);
        nd = select(num+1, &mask, NULL, NULL, &timeout);
    } while (nd<0);

    //while (FD_ISSET(cofd, &mask) || FD_ISSET(sofd, &mask)) {
    while (nd>0 && (FD_ISSET(cofd, &mask) || FD_ISSET(sofd, &mask))) {

        if (FD_ISSET(cofd, &mask)) {
            cc = tcp_recv_Buffer(cofd, &buf);
            if (cc>0) {
                cx = tcp_send_Buffer(sofd, &buf);
                if (cx<=0) {
                    cc = cx;
                    break;
                }
            }
            else break;
            //memset(buf.buf, 0, cc);
        }

        if (FD_ISSET(sofd, &mask)) {
            cc = tcp_recv_Buffer(sofd, &buf);
            if (cc>0) {
                cx = tcp_send_Buffer(cofd, &buf);
                if (cx<=0) {
                    cc = cx;
                    break;
                }
            }
            else break;
            //memset(buf.buf, 0, cc);
        }

        do {
            timeout.tv_sec  = tm;
            timeout.tv_usec = 0;
            FD_ZERO(&mask); 
            FD_SET(sofd, &mask);
            FD_SET(cofd, &mask);
            nd = select(num+1, &mask, NULL, NULL, &timeout);
        } while (nd<0);
    }

    free_Buffer(&buf);

    if (!FD_ISSET(cofd, &mask) && !FD_ISSET(sofd, &mask)) return JBXL_NET_RECV_TIMEOUT;
    return cc;
}


int  get_http_status_num(tList* pl)
{
    int pnum = 0;

    Buffer dat = search_protocol_header(pl, (char*)HDLIST_FIRST_LINE_KEY, 1);
    if (dat.buf!=NULL) {
        if (!strncasecmp((const char*)dat.buf, "HTTP/", 5)) {
            Buffer num = cawk_Buffer(dat, ' ', 2);
            if (num.buf!=NULL) {
                pnum = atoi((const char*)num.buf);
                free_Buffer(&num);
            }
        }
        free_Buffer(&dat);
    }

    return pnum;
}


float  get_http_version_num(tList* pl)
{
    float ver = 0.0;

    Buffer dat = search_protocol_header(pl, (char*)HDLIST_FIRST_LINE_KEY, 1);
    if (dat.buf!=NULL) {
        if (!strncasecmp((const char*)dat.buf, "HTTP/", 5)) {
            Buffer num = cawk_Buffer(dat, ' ', 1);
            if (num.buf!=NULL) {
                ver = (float)atof((char*)&num.buf[5]);
                free_Buffer(&num);
            }
        }
        else {
            Buffer buf = cawk_Buffer(dat, ' ', 3);
            if (!strncasecmp((const char*)buf.buf, "HTTP/", 5)) {
                Buffer num = cawk_Buffer(buf, ' ', 1);
                if (num.buf!=NULL) {
                    ver = (float)atof((char*)&num.buf[5]);
                    free_Buffer(&num);
                }
            }
            free_Buffer(&buf);
        }
        free_Buffer(&dat);
    }

    return ver;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server Side Code  (Junk)
//

int  send_http_res_file(int sofd, char* fname, int mode)
{
    int   sz;
    FILE* fp;
    char  buf[LBUF];
    char* html;
    size_t rs;
    UNUSED(rs);

    sz = file_size(fname);
    if (sz<=0) {
        sz = 0;
        mode = HTTP_CONNECTION_CLOSE;
    }

    tcp_send(sofd, (char*)"HTTP/1.1 200 OK\r\n", 0);
    tcp_send(sofd, (char*)"Accept-Ranges: bytes\r\n", 0);
    tcp_send(sofd, (char*)"Pragma: no-cache\r\n", 0);
    tcp_send(sofd, (char*)"Cache-Control: no-cache\r\n", 0);
    memset(buf, 0, LBUF);
    snprintf(buf, LBUF-2, "Content-Length: %d\r\n", sz);
    tcp_send(sofd, buf, 0);
    if (mode==HTTP_CONNECTION_CLOSE) tcp_send(sofd, (char*)"Connection: Close\r\n", 0);
    else {
        tcp_send(sofd, (char*)"Keep-Alive: timeout=1800, max=100\r\n", 0);
        tcp_send(sofd, (char*)"Connection: Keep-Alive\r\n", 0);
    }
    tcp_send(sofd, (char*)"\r\n", 0);

    if (sz!=0) fp = fopen(fname, "rb");
    else       fp = NULL;
    if (fp==NULL) return JBXL_FILE_OPEN_ERROR;

    html = (char*)malloc(sz);
    if (html==NULL) {
        fclose(fp);
        return JBXL_MALLOC_ERROR;
    }

    rs = fread(html, sz, 1, fp); 
    tcp_send(sofd, html, sz);
    fclose(fp);

    free(html);
    return sz;
}


void  send_http_passwd_req(int sofd)
{
    tcp_send(sofd, (char*)"HTTP/1.1 401 Authorization Required\r\n", 0);
    tcp_send(sofd, (char*)"WWW-Authenticate: Basic realm=\"TTS\"\r\n", 0);
    tcp_send(sofd, (char*)"Pragma: no-cache\r\n", 0);
    tcp_send(sofd, (char*)"Cache-Control: no-cache\r\n", 0);
    tcp_send(sofd, (char*)"Connection: Close\r\n", 0);
    tcp_send(sofd, (char*)"Content-Length: 0\r\n", 0);
    tcp_send(sofd, (char*)"Content-Type: text/html\r\n\r\n", 0);
    
    return;
}

