
#ifndef  __JBXL_SIP_TOOL_H_
#define  __JBXL_SIP_TOOL_H_


#include "protocol.h"


/**
@brief   SIP用ライブラリヘッダ
@file    sip_tool.h
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


#define  SIP_NOMAXFORWARDS      1000


//////////////////////////////////////////////////////////////////////////
// Functions

#define get_sip_header_list(b)      get_protocol_header_list((b), ':', TRUE, TRUE)
#define restore_sip_header(l)       restore_protocol_header((l), ": ", ON, NULL)
#define restore_sip_contents(l)     restore_protocol_contents((l))

void    set_sip_contents(tList* ls, Buffer sdp);

// SIP Header
Buffer  get_sip_via_address(tList* lp, int no);
void    insert_sip_via(tList* lp, char* host, unsigned short port, char* branch, int mode);
void    del_sip_via(tList* lp, char* host, unsigned short port);

void    insert_sip_record_route(tList* lp, char* host, unsigned short port, int mode);
void    del_sip_record_route(tList* lp, char* host, unsigned short port);
void    del_sip_record_route_all(tList* lp);

void    replace_sip_contact(tList* lp, char* host, unsigned short port);
Buffer  get_sip_contact_uri(tList* lp);
Buffer  get_sip_domain(Buffer cturi);

int     get_max_forwards(tList* lp);
void    set_max_forwards(tList* lp, int nm);

// SDP Body
tList*  get_sdp_body_list(tList* lp);
Buffer  restore_sdp_body (tList* ls);

int     replace_sdp_invite_addr(tList*lp, tList* ls, char* host, unsigned short port, int del_candi);

// RTP
unsigned short  get_valid_rtp_pair_sockets(int min, int max, int* rtp, int* rtcp);

// Junk 
Buffer  replace_sip_via(tList* lp, char* host, unsigned short port, int no);
int     replace_sip_contact_dstipport(tList* lp, char* ipaddr, unsigned short port);

#endif
