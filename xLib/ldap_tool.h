
#ifndef  __JBXL_LDAP_TOOL_H_
#define  __JBXL_LDAP_TOOL_H_


/**
@brief   LDAP用ライブラリ ヘッダ
@file    ldap_tool.h
@author  Fumi.Iseki (C)
@date    2023 10/18

@par コンパイルオプション
--I/usr/include -L/usr/lib -lldap

@par ライブラリ
--L/usr/lib -lldap -llber

@attention
このプログラムは OpenLDAP を使用しています．@n
This work is part of OpenLDAP Software <http://www.openldap.org/>.
*/

#include "xtools.h"


#ifndef HAVE_LDAP_H
#ifndef DISABLE_LDAP
#define DISABLE_LDAP
#endif
#endif

#ifdef DISABLE_LDAP
#undef ENABLE_LDAP
#endif


//
#ifdef ENABLE_LDAP


#ifndef LDAP_DEPRECATED
    #define LDAP_DEPRECATED 1
#endif

#include <ldap.h>


struct  _jbl_ldap_host {
    Buffer         hostname;
    unsigned short port;
    int            useSSL;
    int            reqCert;
};


struct  _jbl_ldap_dn {
    Buffer  base;
    Buffer  dnbind;
    Buffer  passwd;
};


typedef  struct _jbl_ldap_host  JBXL_LDAP_Host;
typedef  struct _jbl_ldap_dn    JBXL_LDAP_Dn;

//
void  read_ldap_config(char* fn, JBXL_LDAP_Host* ldap_host, JBXL_LDAP_Dn* ldap_bind);
LDAP* open_ldap_connection(JBXL_LDAP_Host* ldap_host, JBXL_LDAP_Dn* ldap_bind);

int   simple_check_ldap_passwd(LDAP* ld, char* userid, char* passwd, JBXL_LDAP_Dn* ldap_bind);
int   check_ldap_passwd(LDAP* ld, JBXL_LDAP_Dn* user, JBXL_LDAP_Dn* ldap_bind);
void  close_ldap_connection(LDAP* ld, JBXL_LDAP_Host** p_ldap_host, JBXL_LDAP_Dn** p_ldap_bind);

void  init_LDAP_Host(JBXL_LDAP_Host* host);
void  init_LDAP_Dn(JBXL_LDAP_Dn* dn);

void  free_LDAP_Host(JBXL_LDAP_Host* host);
void  free_LDAP_Dn(JBXL_LDAP_Dn* dn);

JBXL_LDAP_Host* new_LDAP_Host(void);
JBXL_LDAP_Dn*   new_LDAP_Dn(void);

void  del_LDAP_Host(JBXL_LDAP_Host** p_host);
void  del_LDAP_Dn(JBXL_LDAP_Dn** p_dn);


#endif        // ENABLE_LDAP

#endif        // __JBXL_LDAP_TOOL_H_

