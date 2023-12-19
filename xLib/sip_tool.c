/**
@brief   SIP用ライブラリ
@file    sip_tool.c
@author  Fumi.Iseki (C)
@date    2009 2/15

@par Example) How to use header decompose and compose functions.
@code
    tList* lp  = get_sip_header_list(buf);
    Buffer sdp = restore_sip_contents(lp);
    .........
    .........
    .........
    set_sip_contents(lp, sdp);
    Buffer snd = restore_sip_header(lp);

    free_Buffer(&sdp);
    del_tList(&lp);
@endcode
or
@code
    tList* lp  = get_sip_header_list(buf);
    tList* ls  = get_sdp_body_list(lp);
    .........
    .........   // SDPリストに対する処理が可能
    .........
    Buffer sdp = restore_sdp_body(ls);
    .........
    .........
    .........
    set_sip_contents(lp, sdp);
    Buffer snd = restore_sip_header(lp);

    free_Buffer(&sdp);
    del_tList(&ls);
    del_tList(&lp);
@endcode
*/


#include "sip_tool.h"


////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
void  set_sip_contents(tList* lp, Buffer sdp)

SDPボディをSIPのヘッダリストに再登録する．サイズの設定も行う．
*/
void  set_sip_contents(tList* lp, Buffer sdp)
{
    char  num[LEN_INT];

    set_protocol_contents(lp, sdp);

    // reset size of contents
    snprintf(num, LEN_INT, "%d", sdp.vldsz);
    num[LEN_INT-1] = '\0';
    set_protocol_header(lp, (char*)"Content-Length", num, 1, ON);

    return;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// SIP Header
//


/////////////////////////////////////////////////////////////////////////
// Via

/**
Buffer  get_sip_via_address(tList* lp, int no)

no番目の Viaヘッダから有効なレスポンスアドレスを獲得する．received=,rport= 対応．

@return host:port 形式のアドレス．
*/
Buffer  get_sip_via_address(tList* lp, int no)
{
    char*  pp;
    Buffer buf = init_Buffer();

    if (lp==NULL) return buf;
    if (no<=0) no = 1;

    buf = search_protocol_header_item(lp, (char*)"Via", no, ' ', 2);
    if (buf.buf!=NULL) {
        // received, rport
        Buffer wrk = dup_Buffer(buf);
        pp = strstrcase((char*)wrk.buf, "received=");
        if (pp!=NULL) {
            char* pa = pp = pp + 9;
            while (*pp!=';' && *pp!='\0') pp++;
            if (*pp==';') *pp = '\0';

            pp = strstrcase((char*)buf.buf, "rport=");
            if (pp!=NULL) {
                char* pt = pp = pp + 6;
                while (*pp!=';' && *pp!='\0') pp++;
                if (*pp==';') *pp = '\0';

                Buffer hostport = comp_hostport(pa, (unsigned short)atoi(pt));
                free_Buffer(&buf);
                free_Buffer(&wrk);
                return hostport;
            }
        }
        free_Buffer(&wrk);

        //
        pp = (char*)buf.buf;
        while (*pp!=';' && *pp!='\0') pp++;
        if (*pp==';') *pp = '\0';
    }

    return buf;
}


/**
void  insert_sip_via(tList* lp, char* host, unsigned short port, char* branch, int add_mode)

Viaヘッダを追加する．received=,rport= 未対応．
*/
void  insert_sip_via(tList* lp, char* host, unsigned short port, char* branch, int add_mode)
{
    char   sipver[] = "SIP/2.0/UDP ";
    char   hostport[LBUF];
    Buffer buf;
    tList* pm;

    if (lp==NULL || host==NULL) return;

    pm = strncasecmp_tList(lp, (char*)"Via", 0, 1);
    if (pm==NULL) {
        if (add_mode) pm = strncmp_tList(lp, (char*)HDLIST_FIRST_LINE_KEY, 0, 1);
        if (pm==NULL) return;
    }
    else if (pm->prev!=NULL) pm = pm->prev;
    else return;

    buf = make_Buffer(LBUF);
    if (port!=0) snprintf(hostport, LBUF, "%s:%d", host, port);
    else         snprintf(hostport, LBUF, "%s",    host);
    hostport[LBUF-1] = '\0';        // to make sure

    copy_s2Buffer(sipver,   &buf);
    cat_s2Buffer (hostport, &buf);
    if (branch!=NULL) {
        cat_s2Buffer(";branch=z9hG4bK", &buf);
        cat_s2Buffer(branch, &buf);
    }

    add_tList_node_str(pm, "Via", (char*)buf.buf);
    free_Buffer(&buf);

    return;
}


/**
void   del_sip_via(tList* lp, char* host, unsigned short port)

Viaヘッダでアドレス部が host:port (port==0の場合は host) で始まるものを全て削除する．
*/
void   del_sip_via(tList* lp, char* host, unsigned short port)
{
    char   hostport[LBUF];

    if (lp==NULL || host==NULL) return;

    if (port!=0) snprintf(hostport, LBUF, "%s:%d", host, port);
    else         snprintf(hostport, LBUF, "%s",    host);
    hostport[LBUF-1] = '\0';        // to make sure

    while (lp!=NULL) {
        if (lp->ldat.key.buf!=NULL && !strcasecmp((char*)lp->ldat.key.buf, "Via")) {
            Buffer via_hostport = cawk_Buffer(lp->ldat.val, ' ', 2);
            if (!strncmp(hostport, (char*)via_hostport.buf, strlen(hostport))) {
                lp = del_tList_node(&lp);
            }
            else lp = lp->next;
            free_Buffer(&via_hostport);
        }
        else lp = lp->next;
    }

    return;
}


/////////////////////////////////////////////////////////////////////////
// Record-Route

/**
void  insert_sip_record_route(tList* lp, char* host, unsigned short port, int add_mode)

Record-Route ヘッダを追加する．
*/
void  insert_sip_record_route(tList* lp, char* host, unsigned short port, int add_mode)
{
    char   ftag[LBUF];
    char   sipuri[LBUF];
    tList* pm;

    if (lp==NULL || host==NULL) return;

    ftag[0] = '\0';

    pm = strncasecmp_tList(lp, (char*)"Record-Route", 0, 1);
    if (pm==NULL) {
        if (add_mode) pm = strncmp_tList(lp, (char*)HDLIST_FIRST_LINE_KEY, 0, 1);
        if (pm==NULL) return;
    }
    else if (pm->prev!=NULL) {
        char* pp = strstrcase((char*)pm->ldat.val.buf, "ftag=");
        if (pp!=NULL) {
            pp = pp + 5;
            int i = 0;
            while (pp[i]!='\0' && pp[i]!=';' && pp[i]!='>' && i<LBUF) {
                ftag[i] = pp[i];
                i++;
            }
            ftag[i] = '\0';
        }
        pm = pm->prev;
    }
    else return;

    if (port!=0) {
        if (ftag[0]!='\0') snprintf(sipuri, LBUF, "<sip:%s:%d;ftag=%s;lr=on>", host, port, ftag);
        else               snprintf(sipuri, LBUF, "<sip:%s:%d;lr=on>", host, port);
    }
    else {
        if (ftag[0]!='\0') snprintf(sipuri, LBUF, "<sip:%s;ftag=%s;lr=on>", host, ftag);
        else               snprintf(sipuri, LBUF, "<sip:%s;lr=on>", host);
    }
    sipuri[LBUF-1] = '\0';      // to make sure

    add_tList_node_str(pm, "Record-Route", sipuri);

    return;
}


/**
void   del_sip_record_route(tList* lp, char* host, unsigned short port)

Record-Routeヘッダでアドレス部が host:port (port==0の場合は host) で始まるものを全て削除する．
*/
void   del_sip_record_route(tList* lp, char* host, unsigned short port)
{
    char  sipuri[LBUF];

    if (lp==NULL || host==NULL) return;

    if (port!=0) snprintf(sipuri, LBUF, "sip:%s:%d", host, port);
    else         snprintf(sipuri, LBUF, "sip:%s",    host);
    sipuri[LBUF-1] = '\0';      // to make sure

    while (lp!=NULL) {
        if (lp->ldat.key.buf!=NULL && !strcasecmp((char*)lp->ldat.key.buf, "Record-Route")) {
            char* rrt = (char*)lp->ldat.val.buf;
            if (rrt[0]=='<') rrt++;

            if (!strncmp(sipuri, rrt, strlen(sipuri))) {
                lp = del_tList_node(&lp);
            }
            else lp = lp->next;
        }
        else lp = lp->next;
    }

    return;
}


/**
void  del_sip_record_route_all(tList* lp)

全ての Record-Routeヘッダを削除する．余計な情報を中継しないように．
*/
void  del_sip_record_route_all(tList* lp)
{
    while (lp!=NULL) {
        if (lp->ldat.key.buf!=NULL && !strcasecmp((char*)lp->ldat.key.buf, "Record-Route")) {
            lp = del_tList_node(&lp);
        }
        else lp = lp->next;
    }
}



/////////////////////////////////////////////////////////////////////////
// Cotact

/**
int  replace_sip_contact(tList* lp, char* host, unsigned short port)

Contactヘッダのアドレス部を host:portまたは hostに書き換える．
*/
void  replace_sip_contact(tList* lp, char* host, unsigned short port)
{
    char  contct[LBUF];
    char  hostport[LBUF];

    if (lp==NULL || host==NULL) return;

    tList* lc = strncasecmp_tList(lp, (char*)"Contact", 0, 1);
    if (lc==NULL || lc->ldat.val.buf==NULL)  return;

    char* pa = (char*)lc->ldat.val.buf;
    char* ps = strstrcase(pa, "sip:");

    while (ps!=NULL) {
        char* pp = strstr(ps, "@");
        if (pp==NULL) pp = ps + 3;

        pp++;
        //int i, cnt=0;
        int i;
        for (i=0; i<LBUF-1; i++) {
            //if (*pp==':') cnt++;
            if (*pp=='\0' || *pp=='>' || *pp==' ' || *pp==';') break;
            contct[i] = *pp;
            pp++;
        }
        contct[i] = '\0';

        if (i<LBUF-1) {
            //if (cnt==1 && port!=0) snprintf(hostport, LBUF, "%s:%d", host, port);
            //else                   snprintf(hostport, LBUF, "%s",    host);
            if (port!=0) snprintf(hostport, LBUF, "%s:%d", host, port);
            else         snprintf(hostport, LBUF, "%s",    host);
            hostport[LBUF-1] = '\0';

            Buffer buf = replace_sBuffer(lc->ldat.val, contct, hostport); 
            free_Buffer(&lc->ldat.val);
            lc->ldat.val = buf;
        }

        ps = strstrcase(pp, "sip:");
    }
    
    return;
}


/**
Buffer  get_sip_contact_uri(tList* lp)

Contactヘッダの内容から SIP URI部分を抜き出す． 

    Contact: <sip:ABCDEFG@WWW.XXX.YYY.ZZZ:12345>;...... の <sip:ABCDEFG@WWW.XXX.YYY.ZZZ:12345> 部分を得る．

@param  lp  SIPヘッダの情報のリスト形式
@return SIP URI
*/
Buffer  get_sip_contact_uri(tList* lp)
{
    Buffer cturi = init_Buffer();  
    Buffer cntct = search_protocol_header(lp, (char*)"Contact", 1);

    if (cntct.buf!=NULL) {
        int i = 0;
        while (cntct.buf[i]!='\0' && cntct.buf[i]!=';' && cntct.buf[i]!=',' && cntct.buf[i]!=CHAR_LF && cntct.buf[i]!=CHAR_CR) i++;
        cntct.buf[i] = '\0';

        cturi = cntct;
    }

    return cturi;
}


/**
Buffer  get_sip_domain(Buffer sipuri)

SIP URI からドメイン情報を抜き出す．

    　<sip:ABCDEFG@WWW.XXX.YYY.ZZZ:12345> のドメイン部分 WWW.XXX.YYY.ZZZ:12345 を得る．

@param  sipuri  SIP URI　例：<sip:ABCDEFG@WWW.XXX.YYY.ZZZ:12345>
@return SIP URIのドメイン部分．
*/
Buffer  get_sip_domain(Buffer sipuri)
{
    Buffer sipdmn = init_Buffer();
    Buffer siptmp = dup_Buffer(sipuri);

    if (siptmp.buf!=NULL) {
        int i = 0;
        char* pp = NULL;
        while (siptmp.buf[i]!='\0' && siptmp.buf[i]!='>' && siptmp.buf[i]!=';' && 
                                      siptmp.buf[i]!=',' && siptmp.buf[i]!=CHAR_LF && siptmp.buf[i]!=CHAR_CR) {
            if (siptmp.buf[i]=='@') pp = (char*)&siptmp.buf[i+1];
            i++;
        }
        siptmp.buf[i] = '\0';

        if (pp!=NULL) sipdmn = make_Buffer_bystr(pp);
        free_Buffer(&siptmp);
    }

    return sipdmn;
}



/////////////////////////////////////////////////////////////////////////
// Max-Forwards

/**
int  get_max_forwards(tList* lp)

Max-Forwardsヘッダの値を得る．ヘッダが無い場合は SIP_NOMAXFORWARDS が返る．
*/
int  get_max_forwards(tList* lp)
{
    int  mxfd = SIP_NOMAXFORWARDS;

    if (lp==NULL) return mxfd;

    tList* lt = strncasecmp_tList(lp, (char*)"Max-Forwards", 0, 1);
    if (lt!=NULL) {
        mxfd = atoi((char*)lt->ldat.val.buf);
    }

    return mxfd;
}


/**
void  set_max_forwards(tList* lp, int nm)

Max-Forwardsヘッダがあれば，nmを設定する．
*/
void  set_max_forwards(tList* lp, int nm)
{
    char mxfd[LEN_INT];

    if (lp==NULL) return;

    tList* lt = strncasecmp_tList(lp, (char*)"Max-Forwards", 0, 1);
    if (lt!=NULL) {
        snprintf(mxfd, LEN_INT, "%d", nm);
        mxfd[LEN_INT-1] = '\0';
        free_Buffer(&lt->ldat.val);
        lt->ldat.val = make_Buffer_bystr(mxfd);
    }

    return;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// SDP Body
//

/**
tList*  get_sdp_body_list(tList* lp)

SDPボディをリスト構造に分解する．

*/
tList*  get_sdp_body_list(tList* lp)
{
    Buffer cnt = restore_protocol_contents(lp);
    tList* ls  = get_protocol_header_list(cnt, '=', FALSE, FALSE);

    free_Buffer(&cnt);
    return ls;
}


/**
Buffer  restore_sdp_body(tList* ls)

リストからSDPボディを復元する．
*/
Buffer  restore_sdp_body(tList* ls)
{
    Buffer buf = init_Buffer();

    if (ls==NULL) return buf;

    buf = restore_protocol_header(ls, (char*)"=", OFF, NULL);
    if (buf.buf!=NULL) {
        buf.vldsz  = buf.vldsz - 2;
        buf.buf[buf.vldsz] = '\0';
    }

    return buf;
}


/**
int   replace_sdp_invite_addr(tList*lp, tList* ls, char* host, unsigned short port, int del_candi)

INVITE用 SDPボディ内のアドレス情報を書き換える．

@param  lp        SIPデータのリスト構造．
@param  ls        SDPデータのリスト構造．NULLの場合は lpから得る．
@param  host      書き換え先のIPアドレス
@param  port      書き換え先のポート番号
@param  del_candi TRUE: a=candidate行を削除する．(余計な情報を相手に送らないため)
*/
int   replace_sdp_invite_addr(tList*lp, tList* ls, char* host, unsigned short port, int del_candi)
{
    tList*  lsdp;
    Buffer  num, sdp;

    if (lp==NULL || host==NULL || port==0) return FALSE;

    num = make_Buffer(LEN_INT);
    copy_i2Buffer((int)port, &num);
    
    if (ls==NULL) lsdp = get_sdp_body_list(lp);
    else          lsdp = ls;

    if (del_candi) {
        tList* pm = lsdp;
        while (pm!=NULL) {
            if (pm->ldat.key.buf!=NULL && !strcasecmp("a", (char*)pm->ldat.key.buf)) {
                if (pm->ldat.val.buf!=NULL && !strncasecmp("candidate ", (char*)pm->ldat.val.buf, 10)) {
                    pm = del_tList_node(&pm);
                }
                else pm = pm->next;
            }
            else pm = pm->next;
        }
    }

    set_protocol_header_item(lsdp, (char*)"o", 1, ' ', 6, host);
    set_protocol_header_item(lsdp, (char*)"c", 1, ' ', 3, host);
    set_protocol_header_item(lsdp, (char*)"m", 1, ' ', 2, (char*)num.buf);

    sdp = restore_sdp_body(lsdp);
    set_sip_contents(lp, sdp);

    if (ls==NULL) del_tList(&lsdp);
    free_Buffer(&sdp);
    free_Buffer(&num);

    return TRUE;
}



//////////////////////////////////////////////////////////////////////////////////////////
// RTP
//

/**
unsigned short  get_valid_rtp_pair_sockets(int min, int max, int* rtp, int* rtcp)

min〜max でランダムなポート番号を持った RTPとRTCPのソケットを生成する．

@param  min  使用可能なポート番号の最小値
@param  max  使用可能なポート番号の最大値
@param  rtp  RTP のソケット番号が入る．
@param  rtcp RTCPのソケット番号が入る．

@return RTPのポート番号（偶数）．RTCPのポート番号は + 1 する．
*/
unsigned short  get_valid_rtp_pair_sockets(int min, int max, int* rtp, int* rtcp)
{
    int  i = 1;
    int  range, sock1, sock2, port;

    max = max - 1;
    min = min + 1;

    range = max - min + 1;
    port  = ((rand()%range + min)/2)*2;

    sock1 = udp_server_socket(port, NULL);            // RTP
    sock2 = udp_server_socket(port+1, NULL);          // RTCP
    if (sock1<=0 || sock2<=0) {
        if (sock1>0) socket_close(sock1);
        if (sock2>0) socket_close(sock2);
        sock1 = sock2 = 0;
    }

    while(sock1==0 && i<range) {
        port = port + 2;
        if (port>max) port = ((port%max + min - 1)/2)*2;

        sock1 = udp_server_socket(port, NULL);        // RTP
        sock2 = udp_server_socket(port+1, NULL);      // RTCP
        if (sock1<=0 || sock2<=0) {
            if (sock1>0) socket_close(sock1);
            if (sock2>0) socket_close(sock2);
            sock1 = sock2 = 0;
        }
        i = i + 2;
    }

    if (sock1==0) {
        port = 0;
        *rtp = *rtcp =0;
    }
    else {
        *rtp  = sock1;
        *rtcp = sock2;
    }
    
    return (unsigned short)port;
}   



//////////////////////////////////////////////////////////////////////////////
// Junk

/**
Buffer  replace_sip_via(tList* lp, char* host, unsigned short port, int no)

Viaヘッダーの IP:Port情報を書き換える

@return 書き換えられた "IP:Port" or "FQDN:Port"
*/
Buffer  replace_sip_via(tList* lp, char* host, unsigned short port, int no)
{
    Buffer buf = init_Buffer();

    if (lp==NULL || host==NULL || port==0) return buf;
    if (no<=0) no = 1;

    buf = search_protocol_header_item(lp, (char*)"Via", no, ' ', 2);
    if (buf.buf!=NULL) {
        int  i = 0;
        char hostport[LBUF];

        while (buf.buf[i]!=';' && buf.buf[i]!='\0') i++;
        if (buf.buf[i]!='\0') buf.buf[i] = '\0';

        snprintf(hostport, LBUF, "%s:%d", host, port);
        hostport[LBUF-1] = '\0';        // to make sure

        replace_protocol_header(lp, (char*)"Via", 1, (char*)buf.buf, hostport);
    }

    return buf;
}


/**
int   replace_sip_contact_dstipport(tList* lp, char* ipaddr, unsigned short port)
*/
int   replace_sip_contact_dstipport(tList* lp, char* ipaddr, unsigned short port)
{
    int    i;
    char   dstip[]   = "dstip=";
    char   dstport[] = "dstport=";
    char*  ip = NULL;
    char*  pt = NULL;

    Buffer frip, frpt;
    Buffer toip, topt;

    if (lp==NULL || ipaddr==NULL || port==0) return FALSE;

    tList* lc = strncasecmp_tList(lp, (char*)"Contact", 0, 1);
    if (lc==NULL || lc->ldat.val.buf==NULL)  return FALSE;

    frip = search_protocol_header_partvalue(lp, (char*)"Contact", dstip,   1);
    frpt = search_protocol_header_partvalue(lp, (char*)"Contact", dstport, 1);
    if (frip.buf==NULL || frpt.buf==NULL) {
        free_Buffer(&frip);
        free_Buffer(&frpt);
        return FALSE;
    }

    i = 0;
    ip = strstr((char*)frip.buf, dstip);
    while (ip[i]!='\0' && ip[i]!=';' && ip[i]!='"') i++;
    ip[i] = '\0';

    i = 0;
    pt = strstr((char*)frpt.buf, dstport);
    while (pt[i]!='\0' && pt[i]!=';' && pt[i]!='"') i++;
    pt[i] = '\0';

    toip = make_Buffer(LBUF);
    topt = make_Buffer(LBUF);

    copy_s2Buffer(dstip,   &toip);
    cat_s2Buffer (ipaddr,  &toip);
    copy_s2Buffer(dstport, &topt);
    cat_i2Buffer (port,    &topt);
 
    Buffer buftemp = replace_sBuffer(lc->ldat.val, ip, (char*)toip.buf);
    Buffer nwcntct = replace_sBuffer(buftemp, pt, (char*)topt.buf);

    free_Buffer(&lc->ldat.val);
    lc->ldat.val = nwcntct;

    free_Buffer(&frip);
    free_Buffer(&frpt);
    free_Buffer(&toip);
    free_Buffer(&topt);
    free_Buffer(&buftemp);

    return TRUE;
}


