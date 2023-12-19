
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// クライアント側でのコマンド送信処理
//

/**
int  send_command_recv_ans(int sock, char* command, int tm, Buffer* key, EVP_CIPHER* cipher)

サーバにコマンド（@\r@\nを自動的に添付）を送り，"OK" か "ERR" の返答を待つ．

@param  sock     サーバのソケット．
@param  command  サーバへ送るコマンド．
@param  tm       タイムアウト時間．秒単位．
@param  key      暗号鍵
@param  cipher   暗号方式

@retval 0       正常終了  （サーバから "OK" を受信した） 
@retval 1以上   エラー番号（サーバから "ERR"を受信した）．
@retval JBXL_NET_RECV_ERROR   通信エラー
@retval JBXL_ARGS_ERROR       コマンドが NULL．
@retval JBXL_ISNET_CMD_ERROR  サーバから "OK", "ERR" 以外のものを受信した．
*/
int  send_command_recv_ans(int sock, char* command, int tm, Buffer* key, EVP_CIPHER* cipher)
{
    int  cc, nn;
    Buffer  ret, ans, buf, eno;
    
    if (command==NULL) return JBXL_ARGS_ERROR;

    buf = make_Buffer(LNAME);
    copy_s2Buffer(command, &buf);
    cat_s2Buffer("\r\n", &buf);
    cc = tcp_send_crypt_sBuffer(sock, &buf, key, cipher);
    free_Buffer(&buf);

    ret = make_Buffer(LNAME);
    cc = tcp_recv_Buffer_wait(sock, &ret, tm);    
    if (cc<=0) {
        free_Buffer(&ret);
        return cc;
    }
    buf = get_plain_sBuffer(ret, key, cipher);
    chomp_Buffer(&buf);
    ans = get_command(buf);

    if      (!strcmp("OK",  (const char*)ans.buf)) nn = 0;
    else if (!strcmp("ERR", (const char*)ans.buf)) {
        eno = get_operand(buf);
        nn  = atoi((const char*)eno.buf);
        free_Buffer(&eno);
        DEBUG_MODE PRINT_MESG("SEND_COMMAND_RECV_ANS: error = %d\n", nn);
    }
    else nn = JBXL_ISNET_CMD_ERROR;

    free_Buffer(&ret);
    free_Buffer(&buf);
    free_Buffer(&ans);

    return nn;
}



/** 
Buffer  recv_mesg_until_end(int sock, int tm, Buffer* key, EVP_CIPHER* cipher)

非暗号化データの場合は "END" が送られて来るまで，メッセージを受信し，
バッファに追加する（簡易バッファリング機能あり）．@n
暗号化データの場合は，改行コード(@\r@\n)が来るまで受信する．

@param  sock    サーバのソケット．
@param  tm      タイムアウト時間．秒単位．
@param  key     暗号鍵
@param  cipher  暗号方式

@return 受信データ．"END"を含む．暗号の場合は復号化される．

*/
Buffer  recv_mesg_until_end(int sock, int tm, Buffer* key, EVP_CIPHER* cipher)
{
    int  cc;
    Buffer ret, buf, dec;
    
    ret = make_Buffer(LBUF);
    buf = make_Buffer(RECVBUFSZ);

    do {
        cc = tcp_recv_Buffer_wait(sock, &buf, tm);
        if (cc>0) {
            cat_Buffer(&buf, &ret);
            clear_Buffer(&buf);
            if (key!=NULL && cipher!=NULL) {
                if (!strnrvscmp("\r\n", (const char*)ret.buf, 2)) break; 
            }
            else {
                if (!strnrvscmp("\r\nEND\r\n", (const char*)ret.buf, 7) ||
                    !strnrvscmp("\nEND\n", (const char*)ret.buf, 5)) break;
            }
        }
    } while(cc>0);

    dec = get_plain_sBuffer(ret, key, cipher);
    free_Buffer(&buf);
    free_Buffer(&ret);
    return dec;
}



/**
Buffer  send_algor_recv_spki(int sock, char* algor, int tm, Buffer* key, EVP_CIPHER* cipher)

サーバへ公開鍵暗号アルゴリズム algorを使用することを通知し，サーバから SPKIをもらう．

@param  sock    サーバへのソケット
@param  algor   使用アルゴリズム．現在サポートしているのは "DH" のみ．
@param  tm      タイムアウト(sec)
@param  key     暗号鍵
@param  cipher  暗号方式

@return サーバの X.509 SPKI (DER)
*/
Buffer  send_algor_recv_spki(int sock, char* algor, int tm, Buffer* key, EVP_CIPHER* cipher)
{
    int  cc;
    Buffer com, ret, buf;

    ret = init_Buffer();                                // 初期化
    if (algor==NULL) return ret;

    com = make_Buffer_bystr((char*)"KEYEX ");
    cat_s2Buffer(algor, &com);
    cc = send_command_recv_ans(sock, (char*)com.buf, tm, key, cipher);
    free_Buffer(&com);
    if (cc!=0) { ret.state = cc; return ret; }

    ret = recv_mesg_until_end(sock, tm, key, cipher);   // サーバの返答
    buf = get_line_Buffer(ret, 1);                      // SPKI(BASE64)
    free_Buffer(&ret);

    ret = decode_base64_Buffer(buf);                    // 鍵をデコードする．
    free_Buffer(&buf);

    return ret;
}



/**
int  send_spki_recv_ans(int sock, Buffer mkey, int tm, Buffer* key, EVP_CIPHER* cipher)

サーバへ自分の SPKI(DER)を送信して，返答を待つ．

@param  sock    サーバへのソケット
@param  mkey    自分の SPKI (DER)
@param  tm      タイムアウト(sec)
@param  key     暗号鍵
@param  cipher  暗号方式

@retval 0      正常終了
@retval 0以外  サーバからのエラー番号
*/
int  send_spki_recv_ans(int sock, Buffer mkey, int tm, Buffer* key, EVP_CIPHER* cipher)
{
    int  cc;
    Buffer enc;

    cc = send_command_recv_ans(sock, (char*)"KEYEX SPKI", tm, key, cipher);
    if (cc!=0) return cc;

    enc = encode_base64_Buffer(mkey);                // 自分の公開鍵情報をエンコード
    cc = send_command_recv_ans(sock, (char*)enc.buf, tm, key, cipher);
    free_Buffer(&enc);

    return cc;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// クライアント側での認証処理
//

/** 
int  check_auth(Buffer ahost, int aport, Buffer userid, Buffer passwd, int keyex, int cryptm, char* cfn, int cont)

認証サーバに接続し，認証を行なう．

keyex, cryptm が両方指定してある場合には暗号化通信が行なわれる．
暗号化通信を行ない，かつ cfn がNULLで無い場合は，サーバの確認を行う．

@param  ahost   認証ホストの名前．
@param  aport   認証ホストのポート番号．
@param  userid  ユーザID 
@param  passwd  パスワード
@param  chmode  チャレンジキー交換モード．OFF:チャレンジキーを交換しない．ON:チャレンジキーを交換する．
                暗号化通信が行われない場合は，強制的にチャレンジキーを交換する(ONになる)．
@param  keyex   鍵交換アルゴリズム
@param  cryptm  共有鍵暗号アルゴリズム
@param  cfn     認証ホストの公開鍵が保存されたファイル．認証ホスト検証用．NULLの場合は検証しない．
@param  cont    暗号合意，またはサーバ確認に失敗した場合に処理を継続するかどうか．
                TRUE:処理を続ける．FALSE:エラーを返す．

@retval 0      正常終了．
@retval JBXL_ISNET_PASSWD_ERROR     ユーザ認証失敗(ユーザは存在するが，パスワードが一致しない)
@retval JBXL_ISNET_USER_ERROR       ユーザ認証失敗(ユーザが存在しない)
@retval JBXL_ISNET_CONNECT_ERROR    認証サーバとの接続エラー
@retval JBXL_ISNET_START_ERROR      認証サーバとの通信開始エラー （相手は認証サーバでない？）  
@retval JBXL_ISNET_AGREE_ERROR      暗号合意エラー（サーバ認証を含む）
@retval JBXL_ISNET_CHALLENGE_ERROR  チャレンジキー合意エラー
*/
int  check_auth(Buffer ahost, int aport, Buffer userid, Buffer passwd, int chmode, int keyex, int cryptm, char* cfn, int cont)
{
    int  cryptf, sock, cc, tm = 10;
    Buffer ipaddr, buf, salt;
    Buffer shdkey;
    char*  pass;
    char*  chip;

    if (ahost.buf==NULL) return JBXL_ARGS_ERROR;

    // 認証サーバへ接続 
    sock = tcp_client_socket((char*)ahost.buf, aport);
    if (sock<0) {
        DEBUG_MODE PRINT_MESG("CHECK_AUTH: auth server open error.\n");
        return JBXL_ISNET_CONNECT_ERROR;
    }
    DEBUG_MODE PRINT_MESG("CHECK_AUTH: auth server opened.\n");

    // 通信開始
    cc = send_command_recv_ans(sock, (char*)"HELLO", tm, NULL, NULL);
    if (cc!=0) {
        socket_close(sock);
        return JBXL_ISNET_START_ERROR;
    }

    shdkey = init_Buffer();
    // supported only DH now
    //if (keyex!=SSL_DH) return JBXL_ISNET_PUBKEYALG_ERROR;

    // 暗号合意
    cryptf = FALSE;
    EVP_CIPHER* cipher = NULL;
    if (keyex!=0 && cryptm!=0) {
        cryptf = TRUE;
        chip   = get_ipaddr_byname((char*)ahost.buf, AF_INET);
        ipaddr = make_Buffer_bystr(chip);
        freeNull(chip);
        cc = start_CRYPT_transfer(sock, keyex, cryptm, ipaddr, cfn, tm, &shdkey, &cipher);
        free_Buffer(&ipaddr);
        if (cc!=0 && !cont) {
            free_EVP_CIPHER(&cipher);
            socket_close(sock);
            return cc;
        }
        // 以後暗号化通信
    }

    // チャレンジキー合意
    if (cryptf==FALSE) chmode = ON;

    if (chmode==ON) {
        cc = send_command_recv_ans(sock, (char*)"KEYEX CHLNG", tm, &shdkey, cipher);
        if (cc!=0) {
            socket_close(sock);
            return JBXL_ISNET_CHALLENGE_ERROR;
        }
    }
    else {
        cc = send_command_recv_ans(sock, (char*)"KEYEX NOCHLNG", tm, &shdkey, cipher);
        if (cc!=0) {
            if (cc==169) {        // Unkown KEYEX operand. Support for old version.
                chmode = ON;
            }
            else {
                socket_close(sock);
                return JBXL_ISNET_CHALLENGE_ERROR;
            }
        }
    }

    /////////////////////////////////////////
    // ユーザIDを送る． 
    buf = make_Buffer(LBUF);
    copy_s2Buffer("USERID ", &buf);
    cat_Buffer(&userid, &buf);
    cc = send_command_recv_ans(sock, (char*)buf.buf, tm, &shdkey, cipher);
    if (cc!=0) {
        free_Buffer(&buf);
        free_EVP_CIPHER(&cipher);
        socket_close(sock);
        return JBXL_ISNET_USER_ERROR;
    }

    // SALTを得る． 
    if (chmode==ON) {
        salt = recv_mesg_until_end(sock, tm, &shdkey, cipher);
        //DEBUG_MODE PRINT_ESC("SALT = [%s]\n", (char*)salt.buf);
    }
    clear_Buffer(&buf);

    // パスワードを計算して送る． 
    copy_s2Buffer("PASSWD ", &buf);
    if (chmode==ON) {
        pass = x2crypt((char*)passwd.buf, (char*)salt.buf);
        cat_s2Buffer(pass, &buf);
        freeNull(pass);
    }
    else {
        cat_s2Buffer((char*)passwd.buf, &buf);
    }
    cc = send_command_recv_ans(sock, (char*)buf.buf, tm, &shdkey, cipher);
    free_Buffer(&buf);

    if (cc) {
        free_EVP_CIPHER(&cipher);
        socket_close(sock);
        return JBXL_ISNET_PASSWD_ERROR;
    }

    // 暗号化通信終了
    if (cryptf) cc = stop_CRYPT_transfer(sock, tm, &shdkey, cipher);

    cc = send_command_recv_ans(sock, (char*)"BYE", tm, &shdkey, cipher);

    socket_close(sock);
    DEBUG_MODE PRINT_MESG("CHECK_AUTH: auth server normally closed.\n");

    free_EVP_CIPHER(&cipher);
    if (cryptf) free_Buffer(&shdkey);

    return 0;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// クライアント側での暗号開始・終了処理
//

/**
int  start_CRYPT_transfer(int sock, int keyex, int cryptm, Buffer ipaddr, char* cfn, int tm, Buffer* shdkey, EVP_CIPHER** cipher)

サーバ側と暗号化通信を行うためのネゴシエーションを行う．

ネゴシエーションが成功した場合，この関数終了後から暗号化通信が始まる．

@param  sock    認証ホストへのソケット．
@param  keyex   鍵交換アルゴリズムを指定（現在の所，SSL_DH のみ）．
@param  cryptm  共通鍵暗号のアルゴリズム 
@param  ipaddr  認証ホストのIPアドレス．認証ホスト検証用．
@param  cfn     認証ホストの公開鍵が保存されたファイル．認証ホスト検証用．NULLの場合は検証しない．
@param  tm      タイムアウト(sec)
@param[out]  shdkey  共通鍵が返る．
@param[out]  cipher  暗号タイプが返る． 

@retval 0      成功．これ以後，暗号化による通信を行う．
@retval JBXL_ISNET_SERVER_ERROR     認証ホストの検証エラー
@retval JBXL_ISNET_PUBKEYALG_ERROR  知らない公開鍵暗号アルゴリズム
@retval JBXL_ISNET_CMNKEYALG_ERROR  知らない共通鍵暗号アルゴリズム
@retval JBXL_ISNET_NULLANS_ERROR    サーバからの返答が NULL
*/
int  start_CRYPT_transfer(int sock, int keyex, int cryptm, Buffer ipaddr, char* cfn, int tm, Buffer* shdkey, EVP_CIPHER** cipher)
{
    Buffer spki, mpki;

    if (cfn!=NULL && ipaddr.buf!=NULL) {
        if (!check_server_spki(ipaddr, spki, cfn)) return JBXL_ISNET_SERVER_ERROR;
    }

    if (keyex==SSL_DH) {
        spki = send_algor_recv_spki(sock, (char*)"DH", tm, NULL, NULL);
        //
        if (spki.buf!=NULL) {
            DH* dhkey = NULL;
            mpki = gen_DHspki_fs(spki, &dhkey);
            if (dhkey!=NULL) {
                gen_CRYPT_SharedKey(keyex, spki, shdkey, (void*)dhkey);
                DH_free(dhkey);
            }
            else {
                PRINT_MESG("ERROR: start_CRYPT_transfer: DH Key is NULL\n");
                return JBXL_ISNET_DHKEY_ERROR;
            }
        }
        else {
            PRINT_MESG("ERROR: start_CRYPT_transfer: spki is NULL\n");
            return JBXL_ISNET_NULLANS_ERROR;
        }
    }
    //else if (keyex==SSL_RSA) spki = send_algor_recv_spki(sock, (char*)"RSA", tm);
    else return JBXL_ISNET_PUBKEYALG_ERROR;

    /*
    DEBUG_MODE {
        Buffer enc;
        enc = encode_base64_Buffer(spki);
        PRINT_MESG("SERVER SPKI = [%s]\n", enc.buf);
        free_Buffer(&enc);
    }*/

    int cc = send_spki_recv_ans(sock, mpki, tm, NULL, NULL);
    if (cc==0) {
        if (cryptm==SSL_AES128CBC) {
            cc = send_command_recv_ans(sock, (char*)"CRYPT AES128CBC", tm, NULL, NULL);
            *cipher = init_EVPAPI_Buffer(SSL_AES128CBC);
        }
        else if (cryptm==SSL_3DES3CBC) {
            cc = send_command_recv_ans(sock, (char*)"CRYPT 3DES3CBC",  tm, NULL, NULL);
            *cipher = init_EVPAPI_Buffer(SSL_3DES3CBC);
        }
    }
    
    free_Buffer(&spki);
    free_Buffer(&mpki);
    if (cc!=0) {
        free_Buffer(shdkey);
        *cipher = NULL;
        return cc;
    }

    return 0;
}



/**
int  stop_CRYPT_transfer(int sock, int tm, Buffer* shdkey, EVP_CIPHER* cipher)

クライアント側での暗号化通信終了処理．

サーバに暗号使用終了を伝え，自分も使用を終了する．

@param  sock    サーバへのソケット
@param  tm      タイムアウト時間(sec)
@param  shdkey  共通鍵
@param  cipher  暗号タイプが返る． 

@retval 0     正常終了  （サーバから"OK"を受信した） 
@retval 1以上 エラー番号（サーバから"ERR"を受信した）．
@retval 負数  エラー
*/
int  stop_CRYPT_transfer(int sock, int tm, Buffer* shdkey, EVP_CIPHER* cipher)
{
    int cc;

    cc = send_command_recv_ans(sock, (char*)"CRYPT RESET", tm, shdkey, cipher);
    return cc;
}


