#ifndef  __JBXL_PASSWORD_H_
#define  __JBXL_PASSWORD_H_
 
/** 
@brief   パスワードライブラリ ヘッダ
@file    password.h
@author  Fumi.Iseki (C)
*/

/*
#ifndef _XOPEN_SOURCE
    #define _XOPEN_SOURCE
#endif
*/

#include "tools.h"
#include <pwd.h>


#ifdef HAVE_SHADOW_H
    #include  <shadow.h>
#endif

#ifdef HAVE_UNISTD_H
    #ifndef _XOPEN_SOURCE
        #define _XOPEN_SOURCE
    #endif
    #include <unistd.h>
#endif

#ifdef HAVE_CRYPT_H
    #include  <crypt.h>
#endif

#ifdef HAVE_RPCSVC_YPCLNT_H
    #include  <rpcsvc/ypclnt.h>
#endif

#ifdef AIX
    struct dom_binding {int dummy;};
#endif


#define    LEN_DOLLAR_SALT  12
#define    LEN_DOLLAR2_SALT 29
#define    LEN_DOLLAR5_SALT 20
#define    LEN_DOLLAR6_SALT 20

#define    LEN_MD5PASS      34
#define    LEN_MD5SALT      12
#define    LEN_DESPASS      13
#define    LEN_DESSALT       2


char*   get_passwd(char* user_id);
void    free_pw(struct passwd* pw);
int     check_passwd(char* passwd, char* cryptpass);
int     check_salt(char* passwd);

char*   x2crypt(char* pass, char* salt);


#ifdef HAVE_RPCSVC_YPCLNT_H
char*   get_nis_passwdf(char* usrid);
struct  passwd* getnisnam(char* usrid);
#endif


#endif
