
Buffer* Base64_DHspki   = NULL;
Buffer* Base64_RSAspki  = NULL;

Buffer* CRYPT_SharedKey = NULL;
EVP_CIPHER* CRYPT_Type  = NULL;
DH*     DHkey           = NULL;

Buffer* User_ID         = NULL;
Buffer* User_Passwd     = NULL;
Buffer* User_Salt       = NULL;

int  No_isNet_Chlng     = FALSE;    // チャレンジキーを使用する (サーバ側の状態を表す)
int  Use_isNet_Ldap     = FALSE;
int  Use_isNet_Crypt    = FALSE;



#ifdef ENABLE_LDAP
JBXL_LDAP_Host* LdapHost = NULL;
JBXL_LDAP_Dn*   LdapBind = NULL;
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// サーバ側でのコマンド受信処理
//

/**
int  command_USERID(Buffer operand, Buffer comment, int sock)

USERIDコマンドを処理する．サーバ側処理．

チャレンジコードを要求しない場合，パスワードが生で流れる可能性があるので，必ず暗号化すること．

@param  operand  コマンドのオペランド
@param  comment  コマンドのコメント
@param  sock     クライアントへのソケット

@retval 0        成功
@retval 0以外    エラー番号

@par エラー番号
120 - 129
*/
int  command_USERID(Buffer operand, Buffer comment, int sock)
{
    char* pass  = NULL;
    char* salt0 = NULL;
    char* salt1 = NULL;
    char* salt2 = NULL;
    Buffer  buf;

    UNUSED(comment);

    pass = get_passwd((char*)operand.buf);
    if (pass==NULL) {
        tcp_send_crypt_mesg(sock, (char*)"ERR 121 passwd get error.\r\n", CRYPT_SharedKey, CRYPT_Type);
        DEBUG_MODE PRINT_MESG("ERR 121 passwd get error.\n");
        return 121;
    }

    if (No_isNet_Chlng==TRUE && Use_isNet_Crypt==FALSE) {
        tcp_send_crypt_mesg(sock, (char*)"ERR 122 both No Challenge Key mode and No Crypt mode are not used.\r\n", CRYPT_SharedKey, CRYPT_Type);
        DEBUG_MODE PRINT_MESG("ERR 122 both No Challenge Key mode and No Crypt mode are not used.\n");
        return 122;
    }

    if (No_isNet_Chlng==FALSE && Use_isNet_Ldap==TRUE) {
        tcp_send_crypt_mesg(sock, (char*)"ERR 123 both Challenge Key mode and Ldap mode are not used.\r\n", CRYPT_SharedKey, CRYPT_Type);
        DEBUG_MODE PRINT_MESG("ERR 123 both Challenge Key mode and Ldap mode are not used.\n");
        return 123;
    }

    if (User_ID!=NULL)     del_Buffer(&User_ID);
    if (User_Passwd!=NULL) del_Buffer(&User_Passwd);
    if (User_Salt!=NULL)   del_Buffer(&User_Salt);
    User_ID      = new_Buffer(0);
    User_Passwd  = new_Buffer(0);
    User_Salt    = new_Buffer(0);
    *User_ID     = dup_Buffer(operand);
    *User_Passwd = make_Buffer_bystr(pass);

    //
    if (strlen(pass)<=2) {
#ifdef ENABLE_LDAP
        int  cc;
        if (!strcmp((const char*)User_Passwd->buf, "*") && Use_isNet_Ldap) {
            LdapHost = new_LDAP_Host();
            LdapBind = new_LDAP_Dn();
            read_ldap_config(NULL, LdapHost, LdapBind);
            LDAP* ld = open_ldap_connection(LdapHost, LdapBind);
            //
            if (ld!=NULL) {
                cc = simple_check_ldap_passwd(ld, (char*)User_ID->buf, NULL, LdapBind);
                close_ldap_connection(ld, &LdapHost, &LdapBind);
                if (cc==1) {
                    cc = tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
                    return 0;
                }
                else {
                    cc = tcp_send_crypt_mesg(sock, (char*)"ERR 128 USERID error with LDAP.\r\n", CRYPT_SharedKey, CRYPT_Type);
                    DEBUG_MODE PRINT_MESG("ERR 128 USERID error with LDAP.\n");
                    return 128;
                }
            }
            else {
                cc = tcp_send_crypt_mesg(sock, (char*)"ERR 127 USERID connection error with LDAP.\r\n", CRYPT_SharedKey, CRYPT_Type);
                DEBUG_MODE PRINT_MESG("ERR 127 USERID connection error with LDAP.\n");
                return 127;
            }
        }
#endif
        tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
        return 0;
    }

    //
    if (pass[0]=='$') {
        int lsalt = 0;
        if      (pass[1]=='1') lsalt = LEN_DOLLAR_SALT;
        //else if (pass[1]=='2') lsalt = LEN_DOLLAR2_SALT;  // Blowfish 未対応
        else if (pass[1]=='5') lsalt = LEN_DOLLAR5_SALT;
        else if (pass[1]=='6') lsalt = LEN_DOLLAR6_SALT;

        if (lsalt!=0) {
            *User_Salt = make_Buffer(lsalt*2 + 5);    // \r\n \r\n \0 で +5文字
            salt0 = cut_str(pass, 0, 2);
            salt1 = cut_str(pass, 0, lsalt-1);
            salt2 = randstr(lsalt-4);
            copy_s2Buffer(salt1, User_Salt);
            cat_s2Buffer("\r\n", User_Salt);
            cat_s2Buffer(salt0,  User_Salt);
            cat_s2Buffer(salt2,  User_Salt);
            cat_s2Buffer("$",    User_Salt);
        }
    }
    // DES
    else if (strlen((const char*)pass)==LEN_DESPASS) {
        *User_Salt = make_Buffer(LEN_DESSALT*2 + 5);
        salt1 = cut_str(pass, 0, LEN_DESSALT-1);
        salt2 = randstr(LEN_DESSALT);
        copy_s2Buffer(salt1,  User_Salt);
        cat_s2Buffer ("\r\n", User_Salt);
        cat_s2Buffer (salt2,  User_Salt);
    }

    freeNull(pass);
    freeNull(salt0);
    freeNull(salt1);
    freeNull(salt2);

    if (User_Salt->buf!=NULL) {
        tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
        if (No_isNet_Chlng==FALSE) {
            buf = make_Buffer(LBUF);
            copy_Buffer(User_Salt,  &buf);
            cat_s2Buffer("\r\nEND\r\n", &buf);
            tcp_send_crypt_sBuffer(sock, &buf, CRYPT_SharedKey, CRYPT_Type);
            free_Buffer(&buf);
        }
    }
    else {
        tcp_send_crypt_mesg(sock, (char*)"ERR 129 USERID error.\r\n", CRYPT_SharedKey, CRYPT_Type);
        DEBUG_MODE PRINT_MESG("ERR 129 USERID error.\n");
        return 129;
    }

    return 0;
}



/**
int  command_PASSWD(Buffer operand, Buffer comment, int sock)

PASSWDコマンドを処理する．サーバ側処理．

@param  operand  コマンドのオペランド
@param  comment  コマンドのコメント
@param  sock     クライアントへのソケット

@retval 0        成功
@retval 0以外    エラー番号

@par エラー番号
130 - 139
*/
int  command_PASSWD(Buffer operand, Buffer comment, int sock)
{
    char* opass = NULL;        // Password that from system
    char* npass = NULL;        // Password that crypted opass 
    char* salt  = NULL;

    UNUSED(comment);
    
    if (User_Passwd==NULL) {
        tcp_send_crypt_mesg(sock, (char*)"ERR 131 system has not a your password.\r\n", CRYPT_SharedKey, CRYPT_Type);
        DEBUG_MODE PRINT_MESG("ERR 131 system has not a your password.\n");
        return 131;
    }

#ifdef ENABLE_LDAP
    int  cc;
    if (!strcmp((const char*)User_Passwd->buf, "*") && Use_isNet_Ldap) {
        LdapHost = new_LDAP_Host();
        LdapBind = new_LDAP_Dn();
        read_ldap_config(NULL, LdapHost, LdapBind);
        LDAP* ld = open_ldap_connection(LdapHost, LdapBind);
        cc = simple_check_ldap_passwd(ld, (char*)User_ID->buf, (char*)operand.buf, LdapBind);
        close_ldap_connection(ld, &LdapHost, &LdapBind);
        //
        if (cc==0) {
            cc = tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
            return 0;
        }
        else {
            cc = tcp_send_crypt_mesg(sock, (char*)"ERR 138 PASSWD error.\r\n", CRYPT_SharedKey, CRYPT_Type);
            DEBUG_MODE PRINT_MESG("ERR 138 PASSWD error.\n");
            return 138;
        }
    }
#endif
            
    int lsalt = 0;
    if (User_Passwd->buf[0]=='$') {
        if      (User_Passwd->buf[1]=='1') lsalt = LEN_DOLLAR_SALT;
        //else if (User_Passwd->buf[1]=='2') lsalt = LEN_DOLLAR2_SALT;
        else if (User_Passwd->buf[1]=='5') lsalt = LEN_DOLLAR5_SALT;
        else if (User_Passwd->buf[1]=='6') lsalt = LEN_DOLLAR6_SALT;

        opass = (char*)&(User_Passwd->buf[lsalt]);
    }
    else if (strlen((const char*)User_Passwd->buf)==LEN_DESPASS) {
        lsalt = 2;
        opass = (char*)&(User_Passwd->buf[lsalt]);
    }

    //
    if (No_isNet_Chlng==FALSE) {    // Use Challenge Keys
        salt = get_line((char*)User_Salt->buf, 2);
        if (salt!=NULL && opass!=NULL) {
            npass = (char*)crypt((const char*)opass, (const char*)salt);
            npass = &(npass[lsalt]);
        }
        if (npass!=NULL) {
            if (!strcmp((const char*)operand.buf, (const char*)npass)) {
                tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
                return 0;
            }
        }
    }
    else {    // Not Use Challenge Keys
        salt = get_line((char*)User_Salt->buf, 1);
        if (salt!=NULL && operand.buf!=NULL) {
            npass = (char*)crypt((const char*)operand.buf, (const char*)salt);
            npass = &(npass[lsalt]);
            free(salt);
        }
        if (npass!=NULL && opass!=NULL) {
            if (!strcmp((const char*)opass, (const char*)npass)) {
                tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
                return 0;
            }
        }
    }

    tcp_send_crypt_mesg(sock, (char*)"ERR 139 PASSWD error.\r\n", CRYPT_SharedKey, CRYPT_Type);
    DEBUG_MODE PRINT_MESG("ERR 139 PASSWD error.\n");
    return 139;
}



/**
int  command_HELLO(Buffer operand, Buffer comment, int sock)

HELLOコマンドを処理する．サーバ側処理．

@param  operand  コマンドのオペランド
@param  comment  コマンドのコメント
@param  sock     クライアントへのソケット

@retval 0        成功
@retval 0以外    エラー番号

@par エラー番号
100 - 109
*/
int  command_HELLO(Buffer operand, Buffer comment, int sock)
{
    int  cc;

    UNUSED(operand);
    UNUSED(comment);
    //
    cc = tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
    return cc;
}



/**
int  command_BYE(Buffer operand, Buffer comment, int sock)

BYEコマンドを処理する．サーバ側処理．

@param  operand  コマンドのオペランド
@param  comment  コマンドのコメント
@param  sock     クライアントへのソケット

@retval 0        成功
@retval 0以外    エラー番号

@par エラー番号
110 - 119
*/
int  command_BYE(Buffer operand, Buffer comment, int sock)
{
    int  cc;

    UNUSED(operand);
    UNUSED(comment);
    //
    cc = tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
    //clear_CRYPT_parameter();
    return cc;
}



/**
int  command_KEYEX(Buffer operand, Buffer comment, int sock)

KEYEXコマンドを処理する．サーバ側処理．

パスワード（及びその他の情報）の交換方法を指定する．
オペランドに公開鍵暗号のアルゴリズムが指定されたときに転送するサーバ側の SPKI は外部で
計算済みでなければならない（主にコマンド起動時）．

鍵交換アルゴリズムの指定の後，KEYEX SKPI が正常に処理されると，この関数は共有鍵を生成する．
生成された共有鍵は デフォルト共有鍵となる．

@param  operand  @b DH:      パスワード情報を暗号化するための，暗号化鍵の交換アルゴリズムとして 
                             Diffie-Hellman を指定する．サーバは続いて，自己のSPKIとして Diffie-Hellnan 
                             の SPKI(Base64_DHspki) をクラインとに転送する．
@param  operand  @b RSA:     鍵交換アルゴリズムに RSA を設定し，サーバはSPKIとして RSAの SPKI(Base64_RSAspki)
                             をクライアントに転送する．（多分 ssl_tool.c 中でまだ実装されていない）
@param  operand  @b SPKI:    クライアントは，続いて自己のSPKIをサーバへ転送する．サーバはそれを受け取り，
                             直前に指定された鍵交換のアルゴリズムに従って，共有鍵を生成する．
@param  operand  @b CHLNG:   パスワードの交換にチャレンジキーを使用する．デフォルト．
@param  operand  @b NOCHLNG: パスワードの交換にチャレンジキーを使用しない．この場合は必ず鍵交換
                             アルゴリズムを指定して暗号化鍵を交換し，通信を暗号化しなければならない．
@param  comment  コマンドのコメント
@param  sock     クライアントへのソケット

@retval 0        成功
@retval 0以外    エラー番号

@par エラー番号
160 - 169

@see get_CRYPT_SharedKey(), exchange_CRYPT_SharedKey() 
*/
int  command_KEYEX(Buffer operand, Buffer comment, int sock)
{
    static int keyex;
    Buffer buf;

    UNUSED(comment);

    if (!strcmp("DH", (const char*)operand.buf)) {
        if (Base64_DHspki!=NULL) {
            Use_isNet_Crypt = TRUE;
            keyex = SSL_DH;
            tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);

            buf = make_Buffer(LBUF);
            copy_Buffer(Base64_DHspki,  &buf);
            cat_s2Buffer("\r\nEND\r\n", &buf);
            tcp_send_crypt_sBuffer(sock, &buf, CRYPT_SharedKey, CRYPT_Type);
            free_Buffer(&buf);
        }
        else {
            tcp_send_crypt_mesg(sock, (char*)"ERR 161 KEYEX DH Error.\r\n", CRYPT_SharedKey, CRYPT_Type);
            DEBUG_MODE PRINT_MESG("ERR 161 KEYEX DH Error.\n");
            return 161;
        }
    }

    else if (!strcmp("RSA", (const char*)operand.buf)) {
        if (Base64_RSAspki!=NULL) {
            Use_isNet_Crypt = TRUE;
            keyex = SSL_RSA;

            tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
            buf = make_Buffer(LBUF);
            copy_Buffer(Base64_RSAspki, &buf);
            cat_s2Buffer("\r\nEND\r\n", &buf);
            tcp_send_crypt_sBuffer(sock, &buf, CRYPT_SharedKey, CRYPT_Type);
            free_Buffer(&buf);
        }
        else {
            tcp_send_crypt_mesg(sock, (char*)"ERR 162 KEYEX RSA Error.\r\n", CRYPT_SharedKey, CRYPT_Type);
            DEBUG_MODE PRINT_MESG("ERR 162 KEYEX RSA Error.\n");
            return 162;
        }
    }

    else if (!strcmp("SPKI", (const char*)operand.buf)) {
        Buffer dkey;

        tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
        buf = make_Buffer(LBUF);
        tcp_recv_Buffer_wait(sock, &buf, 20);
        dkey = get_plain_sBuffer(buf, CRYPT_SharedKey, CRYPT_Type);
        free_Buffer(&buf);
        chomp_Buffer(&dkey);
        buf = decode_base64_Buffer(dkey);
        free_Buffer(&dkey);

        if (CRYPT_SharedKey!=NULL) free_Buffer(CRYPT_SharedKey);
        CRYPT_SharedKey = new_Buffer(0);

        if (gen_CRYPT_SharedKey(keyex, buf, CRYPT_SharedKey, (void*)DHkey)) {
            tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
        }
        else {
            tcp_send_crypt_mesg(sock, (char*)"ERR 163 cannot generate Shared KEY.\r\n", CRYPT_SharedKey, CRYPT_Type);
            DEBUG_MODE PRINT_MESG("ERR 163 cannot generate Shared KEY.\n");
            return 163;
        }
        free_Buffer(&buf);
    }

    else if (!strcmp("CHLNG", (const char*)operand.buf)) {
        if (!Use_isNet_Ldap) {
            No_isNet_Chlng = FALSE;
            tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
        }
        else {
            tcp_send_crypt_mesg(sock, (char*)"ERR 164 cannot use Challenge Key mode with LDAP mode.\r\n", CRYPT_SharedKey, CRYPT_Type);
            DEBUG_MODE PRINT_MESG("ERR 164 cannot use Challenge Key mode with LDAP mode.\n");
            return 164;
        }
    }

    else if (!strcmp("NOCHLNG", (const char*)operand.buf)) {
        if (Use_isNet_Crypt==TRUE) {
            No_isNet_Chlng = TRUE;
            tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
        }
        else {
            tcp_send_crypt_mesg(sock, (char*)"ERR 165 both No Challenge Key mode and No Crypt mode are not used.\r\n", CRYPT_SharedKey, CRYPT_Type);
            DEBUG_MODE PRINT_MESG("ERR 165 both No Challenge Key mode and No Crypt mode are not used.\n");
            return 165;
        }
    }

    else {
        tcp_send_crypt_mesg(sock, (char*)"ERR 169 Unkown KEYEX operand.\n\r", CRYPT_SharedKey, CRYPT_Type);
        DEBUG_MODE PRINT_MESG("ERR 169 Unkown KEYEX operand.\r");
        return 169;
    }

    return 0;
}



/**
int  command_CRYPT(Buffer operand, Buffer comment, int sock)

CRYPTコマンドを処理する．サーバ側処理．

暗号化方法を変えた場合は，この関数の終了（OK返答）直後から新しい手法に変わる．

@param  operand  コマンドのオペランド
@param  comment  コマンドのコメント
@param  sock     クライアントへのソケット

@retval 0        成功
@retval 0以外    エラー番号

@par エラー番号
170 - 179
*/
int  command_CRYPT(Buffer operand, Buffer comment, int sock)
{
    UNUSED(comment);

    if (CRYPT_SharedKey==NULL) {
        tcp_send_crypt_mesg(sock, (char*)"ERR 171 No Shared Key.\r\n", NULL, NULL);
        DEBUG_MODE PRINT_MESG("ERR 171 No Shared Key.\n");
        return 171;
    }

    if (!strcmp("AES128CBC", (const char*)operand.buf)) {            // AES 128bit CBC
        tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
        free(CRYPT_Type);
        CRYPT_Type = init_EVPAPI_Buffer(SSL_AES128CBC);
    }
    else if (!strcmp("3DES3CBC", (const char*)operand.buf)) {        // 3DES 3key CBC
        tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
        CRYPT_Type = init_EVPAPI_Buffer(SSL_3DES3CBC);
    }
    else if (!strcmp("RESET", (const char*)operand.buf)) {            // RESET: 暗号化中止
        tcp_send_crypt_mesg(sock, (char*)"OK\r\n", CRYPT_SharedKey, CRYPT_Type);
        //free_EVPAPI_Buffer();
    }
    else {
        tcp_send_crypt_mesg(sock, (char*)"ERR 179 Unkown CRYPT operand.\r\n", CRYPT_SharedKey, CRYPT_Type);
        DEBUG_MODE PRINT_MESG("ERR 179 Unkown CRYPT operand.\n");
        return 179;
    }

    return 0;
}



