
/** 
@brief   パスワードライブラリ
@file    password.c
@author  Fumi.Iseki (C)
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include  "password.h"


/**
char*   get_passwd(char* user_id)

ユーザ user_id のパスワードを得る．@n
パスワードの検索順は　/etc/passwd, /etc/shadow, NIS

@param  user_id  ユーザ名

@return ユーザの暗号化（ハッシュ値化）されたパスワード
@retval NULL  失敗
*/
char*   get_passwd(char* user_id)
{
    struct passwd* pw;
    struct spwd*   sp;
    char*  pass;

    pass = (char*)malloc(LPASS+1);
    if (pass==NULL) return NULL;
    memset(pass, 0, LPASS+1);

    // for /etc/passwd
    pw = getpwnam((const char*)user_id);
    if (pw==NULL) {
        free(pass);
        return NULL;
    }
    strncpy(pass, pw->pw_passwd, LPASS);
    if (strcmp(pass, "*") && strcmp(pass, "x") && strcmp(pass, "!")) return pass;

#ifdef HAVE_GETSPNAM
    // for /etc/shadow
    sp = getspnam((const char*)user_id);
    if (sp!=NULL) {
        strncpy(pass, sp->sp_pwdp, LPASS);
        return pass;
    }
#endif

#ifdef HAVE_RPCSVC_YPCLNT_H
    // for NIS
    pw = getnisnam(user_id);
    if (pw!=NULL) {
        strncpy(pass, pw->pw_passwd, LPASS);
        free_pw(pw);
        return pass;
    }
#endif

    return NULL;
}



#ifdef HAVE_RPCSVC_YPCLNT_H

/**
char* get_nis_passwdf(char* usrid)

usrid の NIS のパスワードファイル情報を得る．

@param  userid  調べるユーザ名
@return userid のパスワードファイル情報（パスワードファイルの１行）．要 free
*/
char* get_nis_passwdf(char* usrid)
{
    const char* inmap="passwd.byname";
    int   ret, keylen, vallen, usrlen;
    char* domainname;
    char* key;
    char* val;
    char* uname;
    char* nis;

    usrlen = strlen(usrid);
    uname = (char*)malloc(usrlen + 2);
    if (uname==NULL) return NULL;
    strncpy(uname, usrid, usrlen);
    uname[usrlen]   = ':';
    uname[usrlen+1] = '\0';

    yp_get_default_domain(&domainname);

    ret = yp_first(domainname, inmap, &key, &keylen, &val, &vallen);   
    while(ret==0){
        if (!strncmp(val, uname, usrlen+1)) {
            free(uname);
            nis = (char*)malloc(strlen(val)+1);
            if (nis==NULL) return NULL;
            strncpy(nis, val, strlen(val)+1);
            return nis;
        }
        ret = yp_next(domainname, inmap, key, keylen, &key, &keylen, &val, &vallen);   
    }

    free(uname);
    return NULL;
}



/**
struct passwd* getnisnam(char* usrid)

NIS から usrid のパスワードを得る．

@param  userid  調べるユーザ名
@return userid のパスワード構造体へのポインタ．要 free
*/

struct passwd* getnisnam(char* usrid)
{
    struct passwd* pw;
    char*  buf;
    char*  nis;

    nis = get_nis_passwdf(usrid);
    if (nis==NULL) return NULL;
    pw = (struct passwd*)malloc(sizeof(struct passwd));
    if (pw==NULL) {
        free(nis);
        return NULL;
    }

    pw->pw_name   = awk(nis, ':', 1);
    pw->pw_passwd = awk(nis, ':', 2);
    pw->pw_gecos  = awk(nis, ':', 5);
    pw->pw_dir      = awk(nis, ':', 6);
    pw->pw_shell  = awk(nis, ':', 7);

    buf = awk(nis, ':', 3);
    if (buf!=NULL) pw->pw_uid = atoi(buf);
    free(buf);

    buf = awk(nis, ':', 4);
    if (buf!=NULL) pw->pw_gid = atoi(buf);
    free(buf);

    free(nis);
    return pw;
}
        
#endif        // HAVE_RPCSVC_YPCLNT_H



/**
void  free_pw(struct passwd* pw)

パスワード構造体 struct pw* を free する．getnisnam()の返す構造体にのみ適用すること．@n
一般のUNIXライブラリの返すパスワード構造体に対して適用してはいけない（セグメンテーションフォルトを起こす）．

@param  pw  開放する struct pw型変数へのポインタ．

@see struct pw
*/
void  free_pw(struct passwd* pw)
{
    if (pw==NULL) return;

    freeNull(pw->pw_name);
    freeNull(pw->pw_passwd);
    freeNull(pw->pw_gecos);
    freeNull(pw->pw_dir);
    freeNull(pw->pw_shell);
    freeNull(pw);
    return;
}



/**
char*   x2crypt(char* pass, char* bsalt)

pass を bsalt で2回暗号化（ハッシュ値化）する．@n
bsaltには改行コード(@\nまたは @\r@\n)で区切られた2個のsaltが入っている必要がある．

@param  pass   ハッシュ値化するパスワード
@param  bsalt  改行コード(@\nまたは @\r@\n)で区切られた2個の salt．

@return 2度ハッシュ値化された文字列．要 free

@bug bsaltに 2個のslatが入っていなかったときの動作は? -> 不明
*/
char*   x2crypt(char* pass, char* bsalt)
{
    char* cpass;
    char* dpass = NULL;
    char* csalt;

    if (pass==NULL || bsalt==NULL) return NULL;

    csalt = get_line(bsalt, 1);
    cpass = crypt(pass, csalt);
    freeNull(csalt);
    csalt = get_line(bsalt, 2);
    if (csalt==NULL) return NULL;

    if (cpass[0]=='$') {
        int lsalt = 0;
        if      (cpass[1]=='1') lsalt = LEN_DOLLAR_SALT;
        else if (cpass[1]=='2') lsalt = LEN_DOLLAR2_SALT;
        else if (cpass[1]=='5') lsalt = LEN_DOLLAR5_SALT;
        else if (cpass[1]=='6') lsalt = LEN_DOLLAR6_SALT;

        if (lsalt!=0) {
            int passlen = strlen(cpass);
            dpass = cut_str(cpass, lsalt, passlen-1);
            cpass = crypt(dpass, csalt);
            freeNull(dpass);
            dpass = cut_str(cpass, lsalt, passlen-1);
        }
    }
    else if (strlen(cpass)==LEN_DESPASS) {
        dpass = cut_str(cpass, LEN_DESSALT, LEN_DESPASS-1);
        freeNull(cpass);
        cpass = crypt(dpass, csalt);
        freeNull(dpass);
        dpass = cut_str(cpass, LEN_DESSALT, LEN_DESPASS-1);
    }

    freeNull(csalt);
    return dpass;
}



/**
int  check_passwd(char* passwd, char* cryptpass)

生パスワード passwd と暗号化されたパスワード cryptass(salt付き)が同じものであるかどうかチェックする．@n
使用できる暗号化（ハッシュ値化）は DES と $#$（自動判別）

@param  passwd     検査する生パスワード
@param  cryptpass  比較対象のハッシュ値化されたパスワード

@retval TRUE  同じ
@retval FALSE 違う
*/
int  check_passwd(char* passwd, char* cryptpass)
{
    int   samepass=FALSE;
    char* salt;

    if (passwd==NULL || cryptpass==NULL) return FALSE;

//  if (!strncmp("$1$", cryptpass, 3) && strlen(cryptpass)==LEN_MD5PASS) {
//  if (cryptpass[0]=='$' && cryptpass[2]=='$' && cryptpass[11]=='$') {
    if (cryptpass[0]=='$') {
        int lsalt = 0;
        if      (cryptpass[1]=='1') lsalt = LEN_DOLLAR_SALT;
        else if (cryptpass[1]=='2') lsalt = LEN_DOLLAR2_SALT;
        else if (cryptpass[1]=='5') lsalt = LEN_DOLLAR5_SALT;
        else if (cryptpass[1]=='6') lsalt = LEN_DOLLAR6_SALT;

        if (lsalt!=0) {
            salt = cut_str(cryptpass, 0, lsalt-1);
            if (!strcmp(crypt(passwd, salt), cryptpass)) {
                samepass = TRUE;
            }
            free(salt);
        }
    }
    else if (strlen(cryptpass)==LEN_DESPASS) {
        // DES
        salt = cut_str(cryptpass, 0, LEN_DESSALT-1);
        if (!strcmp(crypt(passwd, salt), cryptpass)) {
            samepass = TRUE;
        }
        free(salt);
    }

    return samepass;
}



/**
int  check_salt(char* passwd)

パスワード passwd のsaltをチェックする

DES -- 0
$#$ -- #
saltn無し -- -1

@param  passwd   チェックする文字列（パスワード）

@retval slat の種類 
@retval -1 saltが無い
@retval 0  DES
@retval number  $#$........$ の #
*/
int  check_salt(char* passwd)
{
    int ret = -1;
    int len = strlen(passwd);

    if (LEN_DESPASS<len) {
        if (passwd[0]=='$' && passwd[2]=='$' && passwd[11]=='$') {
            ret = (short int)passwd[1] - (short int)'1' + 1;
        }
    }
    else if (LEN_DESPASS==len) {
        ret = 0;
    }

    return ret;
}

