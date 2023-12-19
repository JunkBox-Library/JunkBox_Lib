/**
@brief   LDAP用ライブラリ
@file    ldap_tool.c
@author  Fumi.Iseki (C)

@par ライブラリ
--L/usr/lib -lldap 

@attention
このプログラムは OpenLDAP を使用しています．@n
This work is part of OpenLDAP Software <http://www.openldap.org/>.
*/


#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include "ldap_tool.h"
#include "jbxl_state.h"


#ifdef ENABLE_LDAP


#include "tlist.h"


/**
void  read_ldap_config(char* fn, JBXL_LDAP_Host* ldap_host, JBXL_LDAP_Dn* ldap_bind)

ファイル /etc/openldap/ldap.conf, /etc/ldap.conf, /etc/nslcd.conf, fn を順に読んで
JBXL_LDAP_Host* ldap_host, JBXL_LDAP_Dn* ldap_bind に情報を格納する．

@param  fn  設定の格納されたファイル名．
@param  ldap_host  サーバの情報を格納する JBXL_LDAP_Host へのポインタ
@param  ldap_bind  接続情報を格納する JBXL_LDAP_Dn へのポインタ
*/
void  read_ldap_config(char* fn, JBXL_LDAP_Host* ldap_host, JBXL_LDAP_Dn* ldap_bind)
{
    if (ldap_host==NULL || ldap_bind==NULL) return;

    tList* lp    = NULL;
    tList* cnfg1 = NULL;
    tList* cnfg2 = NULL;
    tList* cnfg3 = NULL;
    tList* cnfg4 = NULL;
    Buffer protocol = init_Buffer();

    //
    cnfg1 = read_index_tList_file("/etc/ldap.conf", ' ');
    cnfg2 = read_index_tList_file("/etc/openldap/ldap.conf", ' ');
    cnfg3 = read_index_tList_file("/etc/nslcd.conf", ' ');
    if (fn!=NULL) cnfg4 = read_index_tList_file(fn, ' ');

    lp = add_tList_end(cnfg1, cnfg2);
    lp = add_tList_end(lp,    cnfg3);
    lp = add_tList_end(lp,    cnfg4);
    if (lp==NULL) return;

    //
    Buffer uri = buffer_key_tList(lp, "uri", 1);
    if (uri.buf!=NULL) {
        decomp_url(uri, NULL, &protocol, &ldap_host->hostname, &ldap_host->port, NULL);
        if (!strcmp((const char*)protocol.buf, "ldaps")) {
            ldap_host->useSSL = TRUE;
        }
        free_Buffer(&protocol);
        free_Buffer(&uri);
    }

    ldap_bind->base   = buffer_key_tList(lp, "base",   1);
    ldap_bind->dnbind = buffer_key_tList(lp, "rootdn", 1);
    ldap_bind->passwd = buffer_key_tList(lp, "rootpw", 1);

    if (ldap_bind->dnbind.buf==NULL || ldap_bind->passwd.buf==NULL) {
        free_Buffer(&ldap_bind->dnbind);
        free_Buffer(&ldap_bind->passwd);
        ldap_bind->dnbind = buffer_key_tList(lp, "binddn", 1);
        ldap_bind->passwd = buffer_key_tList(lp, "bindpw", 1);
    }
    if (ldap_bind->base.buf==NULL) {
        ldap_bind->base = dup_Buffer(ldap_bind->dnbind);
    }

    //
    if (ldap_bind->dnbind.buf!=NULL) {
        Buffer tmp = erase_sBuffer(ldap_bind->dnbind, "\"\'");
        copy_Buffer(&tmp, &ldap_bind->dnbind);
        free_Buffer(&tmp);
    }
    if (ldap_bind->base.buf!=NULL) {
        Buffer tmp = erase_sBuffer(ldap_bind->base, "\"\'");
        copy_Buffer(&tmp, &ldap_bind->base);
        free_Buffer(&tmp);
    }
    if (ldap_host->port<=0) {
        if (ldap_host->useSSL==TRUE) ldap_host->port = 636;
        else                         ldap_host->port = 389;
   }

    // Parameters
    Buffer param = buffer_key_tList(lp, "TLS_REQCERT", 1);
    if (param.buf!=NULL) {
        if      (!strcasecmp((const char*)param.buf, "never"))  ldap_host->reqCert = LDAP_OPT_X_TLS_NEVER;
        else if (!strcasecmp((const char*)param.buf, "hard"))   ldap_host->reqCert = LDAP_OPT_X_TLS_HARD;
        else if (!strcasecmp((const char*)param.buf, "demand")) ldap_host->reqCert = LDAP_OPT_X_TLS_DEMAND;
        else if (!strcasecmp((const char*)param.buf, "allow"))  ldap_host->reqCert = LDAP_OPT_X_TLS_ALLOW;
        else if (!strcasecmp((const char*)param.buf, "try"))    ldap_host->reqCert = LDAP_OPT_X_TLS_TRY;
        free_Buffer(&param);
    }

    //
    //print_tList(stdout, lp);
    del_all_tList(&lp);

    return;
}



/**
LDAP*  open_ldap_connection(JBXL_LDAP_Host* ldap_host, JBXL_LDAP_Dn* ldap_bind)
    
設定ファイルを読み込み後，LDAPサーバに接続する

@param  ldap_host  サーバの情報を格納する JBXL_LDAP_Host へのポインタ
@param  ldap_bind  接続情報を格納する JBXL_LDAP_Dn へのポインタ

@return LDAPサーバへのセッションハンドラ．接続に失敗した場合は NULL
*/
LDAP*  open_ldap_connection(JBXL_LDAP_Host* ldap_host, JBXL_LDAP_Dn* ldap_bind)
{
    if (ldap_host==NULL) return NULL;
    if (ldap_bind==NULL) return NULL;
    //
    if (ldap_bind->dnbind.buf==NULL)    return NULL;
    if (ldap_bind->passwd.buf==NULL)    return NULL;
    if (ldap_bind->passwd.buf[0]=='\0') return NULL;
    if (ldap_host->hostname.buf==NULL)  return NULL;
    if (ldap_host->port<=0)             return NULL;

    int ret;
    LDAP* ld = NULL;

    if (ldap_host->useSSL!=TRUE || ldap_host->port==389) {
        DEBUG_MODE PRINT_MESG("INFO LDAP NORMAL Mode\n");
        ld = ldap_init((char*)ldap_host->hostname.buf, ldap_host->port);
        if (ld==NULL) {
            DEBUG_MODE PRINT_MESG("ERR  LDAP Init error.\n");
            return NULL;
        }

        if (ldap_host->useSSL==TRUE) {    // STARTTLS (動作未確認)
            DEBUG_MODE PRINT_MESG("INFO LDAP STARTTLS Mode\n");
            ret = ldap_set_option(NULL, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_host->reqCert);
            if (ret!=LDAP_SUCCESS) {
                DEBUG_MODE PRINT_MESG("ERR  LDAP STARTTLS Require Cert = %s\n", ldap_err2string(ret));
                ldap_unbind_s(ld);
                return NULL;
            }

            int ldap_vers = LDAP_VERSION3;
            ret = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &ldap_vers);
            if (ret!=LDAP_SUCCESS) {
                DEBUG_MODE PRINT_MESG("ERR  LDAP STARTTLS Version = %s\n", ldap_err2string(ret));
                ldap_unbind_s(ld);
                return NULL;
            }
            //
            ret = ldap_start_tls_s(ld, NULL, NULL);
            if (ret!=LDAP_SUCCESS) {
                DEBUG_MODE PRINT_MESG("ERR  LDAP STARTTLS Start = %s\n", ldap_err2string(ret));
                ldap_unbind_s(ld);
                return NULL;
            }
        }
    }
    // 
    else {            // LDAP over SSL
        DEBUG_MODE PRINT_MESG("INFO LDAP Over SSL Mode\n");
        Buffer url = make_Buffer_bystr("ldaps://");
        cat_Buffer(&ldap_host->hostname, &url);
        cat_s2Buffer(":", &url);
        char* str = itostr_ts(ldap_host->port);
        cat_s2Buffer(str, &url);
        freeNull(str);
        DEBUG_MODE PRINT_MESG("INFO LDAP SSL URL = %s\n", (char*)url.buf);
        //
        ret = ldap_initialize(&ld, (char*)url.buf);
        free_Buffer(&url);
        if (ret!=LDAP_SUCCESS) {
            DEBUG_MODE PRINT_MESG("ERR  LDAP SSL Init = %s\n", ldap_err2string(ret));
            return NULL;
        }
        //
        ret = ldap_set_option(NULL, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_host->reqCert);
        if (ret!=LDAP_SUCCESS) {
            DEBUG_MODE PRINT_MESG("ERR  LDAP SSL Require Cert = %s\n", ldap_err2string(ret));
            ldap_unbind_s(ld);
            return NULL;
        }
    }

    ret = ldap_simple_bind_s(ld, (char*)ldap_bind->dnbind.buf, (char*)ldap_bind->passwd.buf);
    if (ret!=LDAP_SUCCESS) {
        DEBUG_MODE PRINT_MESG("ERR  LDAP Bind = %s\n", ldap_err2string(ret));
        ldap_unbind_s(ld);
        return NULL;
    }

    return ld;
}


/**
int  simple_check_ldap_passwd(LDAP* ld, char* userid, char* passwd, JBXL_LDAP_Dn* ldap_bind)

LDAPを使用してユーザ認証を行う．@n
データは JBXL_LDAP_Dn型の変数に格納されてから検証される．

@param  ld      LDAPサーバへのセッションハンドラ
@param  userid  ユーザID
@param  passwd  パスワード
@param  ldap_bind  LDAP の BIND情報

@retval 0      正常終了．
@retval JBXL_LDAP_PASSWD_ERROR  ユーザ認証失敗(ユーザは存在するが，パスワードが一致しない)
@retval JBXL_LDAP_USER_ERROR    ユーザ認証失敗(ユーザが存在しない)
@retval JBXL_LDAP_BASE_ERROR    BASE名が不明
@retval JBXL_ARGS_ERROR         LDAPサーバへのセッションハンドラが NULL
*/
int  simple_check_ldap_passwd(LDAP* ld, char* userid, char* passwd, JBXL_LDAP_Dn* ldap_bind)
{
    JBXL_LDAP_Dn user;
    init_LDAP_Dn(&user);

    if (userid!=NULL) user.dnbind = make_Buffer_bystr(userid);
    if (passwd!=NULL) user.passwd = make_Buffer_bystr(passwd);
    user.base = dup_Buffer(ldap_bind->base);

    int ret = check_ldap_passwd(ld, &user, ldap_bind);
    free_LDAP_Dn(&user);

    return ret;
}


/**
int  check_ldap_passwd(LDAP* ld, JBXL_LDAP_Dn* user, JBXL_LDAP_Dn* ldap_bind)

LDAP を使用してユーザ認証を行う．

@param  ld    LDAPサーバへのセッションハンドラ
@param  user  ユーザ情報が格納された JBXL_LDAP_Dn
@param  ldap_bind  LDAP の BIND情報

@retval 0    正常終了．
@retval JBXL_LDAP_PASSWD_ERROR  ユーザ認証失敗(ユーザは存在するが，パスワードが一致しない)
@retval JBXL_LDAP_USER_ERROR    ユーザ認証失敗(ユーザが存在しない)
@retval JBXL_LDAP_BASE_ERROR    BASE名が不明
@retval JBXL_ARGS_ERROR         LDAPサーバへのセッションハンドラが NULL
*/
int  check_ldap_passwd(LDAP* ld, JBXL_LDAP_Dn* user, JBXL_LDAP_Dn* ldap_bind)
{
    //char* dn_attr[] = {_tochar("distinguishedName"), NULL};
    char* dn_attr[] = {_tochar("distinguishedName"), _tochar("commonName"), NULL};

    if (ld==NULL) return JBXL_ARGS_ERROR;

    if (user->base.buf==NULL) user->base = dup_Buffer(ldap_bind->base);
    if (user->base.buf==NULL) return JBXL_LDAP_BASE_ERROR;

    //
    if (user->dnbind.buf==NULL) {
        return JBXL_LDAP_NO_USER_ERROR;
    }
    else {
        Buffer tmp = erase_sBuffer(user->dnbind, "*");
        copy_Buffer(&tmp, &user->dnbind);
        free_Buffer(&tmp);
    }
    if (user->dnbind.buf[0]=='\0') {
        return JBXL_LDAP_NO_USER_ERROR;
    }

    Buffer cond = make_Buffer_bystr("uid=");
    cat_Buffer(&user->dnbind, &cond);

    LDAPMessage* res = NULL;
    ldap_search_s(ld, (char*)user->base.buf, LDAP_SCOPE_SUBTREE, (char*)cond.buf, dn_attr, 0, &res);
    free_Buffer(&cond);
    if (res==NULL) {
        return JBXL_LDAP_USER_ERROR;
    }

    char* attr;
    LDAPMessage* ent = NULL;
    BerElement*  ber = NULL;

/*  for debug
    for (ent = ldap_first_entry(ld, res); ent != NULL; ent = ldap_next_entry(ld, ent)) {
        for (attr = ldap_first_attribute(ld, ent, &ber); attr != NULL; attr = ldap_next_attribute(ld, ent, ber)) {
            char** dn = ldap_get_values(ld, ent, attr);
            print_message("%s => %s\n", attr, *dn);
            ldap_memfree(attr);
            free(*dn);
        }
        ber_free(ber, 0);
    } 
    ldap_msgfree(res);
    return 0;
*/

    ent = ldap_first_entry(ld, res);
    if (ent==NULL) {
        ldap_msgfree(res);
        return JBXL_LDAP_NO_ENTRY_ERROR;
    }

    attr = ldap_first_attribute(ld, ent, &ber);
    ber_free(ber, 0);
    if (attr==NULL) {
        ldap_msgfree(res);
        return JBXL_LDAP_NO_ATTR_ERROR;
    }

    char** dn = ldap_get_values(ld, ent, attr); 
    if (dn==NULL || *dn==NULL) {
        ldap_memfree(attr);
        ldap_msgfree(res);
        return JBXL_LDAP_NO_VAL_ERROR;
    }
    
    // ユーザチェック   Password "" is OK!! Ohhh GeroGero!!
    if (user->passwd.buf==NULL || user->passwd.buf[0]=='\0') return JBXL_LDAP_PASSWD_ERROR;

    Buffer dName = make_Buffer_bystr(*dn);
    // distinguishedName 情報がない場合に，DNを生成する．
    if (!strncasecmp(attr, "commonName", 10)) {
        ins_s2Buffer("cn=", &dName);
        cat_s2Buffer(",", &dName);
        cat_Buffer(&ldap_bind->base, &dName);
    }
    ldap_memfree(attr);
    ldap_msgfree(res);
    free(*dn);
        
    // パスワード確認
    int ret = ldap_simple_bind_s(ld, (const char*)dName.buf, (char*)user->passwd.buf);
    free_Buffer(&dName);
    if (ret!=LDAP_SUCCESS) return JBXL_LDAP_PASSWD_ERROR;

    // 念のため，セッションを確認
    //ret = ldap_compare_s(ld, *dn, "name", (char*)user->dnbind.buf);
    //if (ret!=LDAP_COMPARE_TRUE) return 1;

    return 0;
}


/**
void  close_ldap_connection(LDAP* ld, JBXL_LDAP_Host** p_ldap_host, JBXL_LDAP_Dn** p_ldap_bind)

LDAPサーバとの接続を閉じる

@param  ld     LDAPサーバへのセッションハンドラ
@param  p_ldap_host  LDAP の ホスト情報
@param  p_ldap_bind  LDAP の BIND情報
*/
void  close_ldap_connection(LDAP* ld, JBXL_LDAP_Host** p_ldap_host, JBXL_LDAP_Dn** p_ldap_bind)
{
    del_LDAP_Host(p_ldap_host);
    del_LDAP_Dn  (p_ldap_bind);

    ldap_unbind_s(ld);
}



////////////////////////////////////////////////////////////////////////

void  init_LDAP_Host(JBXL_LDAP_Host* host)
{
    if (host==NULL) return;

    host->hostname = init_Buffer();
    host->port     = 0;
    host->useSSL   = FALSE;
    host->reqCert  = LDAP_OPT_X_TLS_HARD;
}


void  init_LDAP_Dn(JBXL_LDAP_Dn* dn)
{
    if (dn==NULL) return;

    dn->base   = init_Buffer();
    dn->dnbind = init_Buffer();
    dn->passwd = init_Buffer();
}


void  free_LDAP_Host(JBXL_LDAP_Host* host)
{
    if (host==NULL) return;

    free_Buffer(&(host->hostname));
    init_LDAP_Host(host);
}


void  free_LDAP_Dn(JBXL_LDAP_Dn* dn)
{
    if (dn==NULL) return;

    free_Buffer(&(dn->base));
    free_Buffer(&(dn->dnbind));
    free_Buffer(&(dn->passwd));
}


JBXL_LDAP_Host*  new_LDAP_Host(void)
{
    JBXL_LDAP_Host* host = (JBXL_LDAP_Host*)malloc(sizeof(JBXL_LDAP_Host));
    init_LDAP_Host(host);

    return host;
}


JBXL_LDAP_Dn*  new_LDAP_Dn(void)
{
    JBXL_LDAP_Dn* dn = (JBXL_LDAP_Dn*)malloc(sizeof(JBXL_LDAP_Dn));
    init_LDAP_Dn(dn);

    return dn;
}


void  del_LDAP_Host(JBXL_LDAP_Host** p_host)
{
    if (p_host==NULL) return;

    free_LDAP_Host(*p_host);
    if (*p_host!=NULL) free(*p_host);
    *p_host = NULL;
}


void  del_LDAP_Dn(JBXL_LDAP_Dn** p_dn)
{
    if (p_dn==NULL) return;

    free_LDAP_Dn(*p_dn);
    if (*p_dn!=NULL) free(*p_dn);
    *p_dn = NULL;
}


#endif        // DISABLE_LDAP
