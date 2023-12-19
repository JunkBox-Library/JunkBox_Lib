
/**
@brief   Diffie-Hellman用 ライブラリ
@file    ssl_tool.c
@author  Fumi.Iseki (C)

@par ライブラリ
--L/usr/local/ssl/lib -lssl -lcrypto 
--L/usr/lib64 -lssl -lcrypto -lcrypt

@par 転送データ（テキスト）形式
@code
通常転送データ：            [データ][\r][\n]
通常転送データ（複数行）：  [データ][\r][\n][データ][\r][\n]...[\r][\n][END][\r][\n]
暗号化時の転送データ：      [暗号化データ][\r][\n] （一行）
                                   |→復号→ [データ][\r][\n]....[\r][\n][END][\r][\n]
注）データを転送するとき，\r\n が生のデータの最後につくのか，暗号化されたデータの最後につくのか，よく注意しないといけない．
@endcode

@attention
このプログラムは OpenSSL を使用しています．@n
This product includes software developed by the OpenSSL Project
for use in the OpenSSL Toolkit. (http://www.openssl.org/)
*/

#include "dh_tool.h"


#ifdef ENABLE_SSL


/*
###############################################################################################
for OpenSSL3
see https://stackoverflow.com/questions/71551116/openssl-3-diffie-hellman-key-exchange-c
see https://github.com/eugen15/diffie-hellman-cpp
###############################################################################################
*/



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Diffie-Hellman 鍵交換法
//

/**
int  save_DHspki_with_private(Buffer pki, FILE* fp)

Diffie-Hellman の公開鍵情報（X.509の SubjectPublicKeyInfo）pkiとプライベート鍵
(dhkeyより取得)をファイルに保存する．公開鍵情報，プライベート鍵の順で書き込まれる．
既にファイルが存在する場合，その内容は上書きされる．

@param  pki   保存する鍵情報（DER）．
@param  fp    ファイルポインタ 
@param  dhkey DH鍵．

@retval TRUE  成功．
@retval FALSE 失敗．ファイルの内容は保証されない．
*/
int  save_DHspki_with_private(Buffer pki, FILE* fp, DH* dhkey)
{    
    unsigned int  md;
    Buffer pv;

    if (fp==NULL || dhkey==NULL) return FALSE;

    md = JBXL_FIO_SPKI | JBXL_FIO_ORIGINAL;
    if (!save_tagged_Buffer(pki, fp, md, FALSE)) return FALSE;

    pv = get_DHprivatekey(dhkey);
    if (pv.buf==NULL) return FALSE;

    md = JBXL_FIO_PRIV_KEY | JBXL_FIO_ORIGINAL;
    if (!save_tagged_Buffer(pv,  fp, md, FALSE)) return FALSE;
    free_Buffer(&pv);

    return  TRUE;
}



/**
Buffer  read_DHspki_with_private(FILE* fp, DH** p_dhkey)

Diffie-Hellman の公開鍵情報（X.509の SubjectPublicKeyInfo）とプライペート鍵をファイルから読み込む．

@param  fp      ファイルポインタ 
@param  p_dhkey DHの鍵パラメータが返る．要 DH_free().
@return 読み込んだ鍵情報．
*/
Buffer  read_DHspki_with_private(FILE* fp, DH** p_dhkey)
{
    int  n = 0, code;
    unsigned int  md;
    Buffer  pp, pv, pk, gk, yk;

    pp = init_Buffer();
    if (fp==NULL) return pp;

    md = JBXL_FIO_SPKI | JBXL_FIO_ORIGINAL;
    pp = read_tagged_Buffer(fp, &md);
    if (pp.buf==NULL) return pp;

    if (*p_dhkey!=NULL) DH_free(*p_dhkey);
    *p_dhkey = DH_new();
    if (*p_dhkey==NULL) {
        free_Buffer(&pp);
        return pp;
    }
    
    pk = get_DHPkey(pp);
    gk = get_DHGkey(pp);
    yk = get_DHYkey(pp);

#if OPENSSL_VERSION_NUMBER < 0x10101000L
    // v1.1.0
    (*p_dhkey)->p = BN_bin2bn((const unsigned char*)(pk.buf), pk.vldsz, NULL);
    (*p_dhkey)->g = BN_bin2bn((const unsigned char*)(gk.buf), gk.vldsz, NULL);
    (*p_dhkey)->pub_key = BN_bin2bn((const unsigned char*)(yk.buf), yk.vldsz, NULL);
#else
    // v1.1.1
    BIGNUM* dhp_bn = BN_bin2bn((const unsigned char*)(pk.buf), pk.vldsz, NULL);
    BIGNUM* dhg_bn = BN_bin2bn((const unsigned char*)(gk.buf), gk.vldsz, NULL);
    BIGNUM* dhy_bn = BN_bin2bn((const unsigned char*)(gk.buf), gk.vldsz, NULL);

    if (dhp_bn!=NULL && dhg_bn!=NULL && dhy_bn!=NULL) {
        DH_set0_pqg(*p_dhkey, dhp_bn, NULL, dhg_bn);
        DH_set0_key(*p_dhkey, dhy_bn, NULL);
    }
    else {
        DH_free(*p_dhkey);
        *p_dhkey = NULL;
    }
#endif

    free_Buffer(&pk);
    free_Buffer(&gk);
    free_Buffer(&yk);

    // 鍵のチェック
    if (*p_dhkey!=NULL) n = DH_check(*p_dhkey, &code);
    if (n!=1 || code!=0) {
        if (*p_dhkey!=NULL) DH_free(*p_dhkey);
        free_Buffer(&pp);
        return pp;
    }

    // Private KEY の読み込み
    md = JBXL_FIO_PRIV_KEY | JBXL_FIO_ORIGINAL;
    pv = read_tagged_Buffer(fp, &md);
    if (pv.buf==NULL) {
        if (*p_dhkey!=NULL) DH_free(*p_dhkey);
        free_Buffer(&pp);
        return pp;
    }

#if OPENSSL_VERSION_NUMBER < 0x10101000L
    (*p_dhkey)->priv_key = BN_bin2bn((const unsigned char*)(pv.buf), pv.vldsz, NULL);
#else
    BIGNUM* priv_key = BN_bin2bn((const unsigned char*)(pv.buf), pv.vldsz, NULL);
    DH_set0_key(*p_dhkey, NULL, priv_key);
#endif

    free_Buffer(&pv);

    return  pp;
}



/**
Buffer  get_DHspki_ff(char* filename, int ks, DH** p_dhkey)

Generate DH SPKI from File.

ファイルから Diffie-Hellman の鍵を読み込む．

ただし以下の場合にはksのサイズの鍵を新たに生成し，ファイルに保存してから返す．
-# ファイルが存在しないか，鍵を読み込めない
-# ファイルにプライベート鍵が保存されていない
-# ファイルの中の鍵の長さが ksより短い

p_dhkey に DHの鍵パラメータが返る．

@param  filename  鍵が保存されているファイル
@param  ks        新たに生成する場合の鍵長(Bit)．
@param  p_dhkey   DHの鍵パラメータが返る．要 DH_free().

@return DER形式の公開鍵情報．
*/
Buffer  get_DHspki_ff(char* filename, int ks, DH** p_dhkey)
{
    Buffer  pki;
    FILE*   fp;

    pki = init_Buffer();
    if (filename==NULL || p_dhkey==NULL) return pki;

    if (file_exist(filename)) {
        //DEBUG_MODE PRINT_MESG("Load DH public key. ");
        fp = fopen(filename, "rb");
        pki = read_DHspki_with_private(fp, p_dhkey);
        fclose(fp);
        //DEBUG_MODE PRINT_MESG("... done.\n");
        if (pki.buf!=NULL) {
#if OPENSSL_VERSION_NUMBER < 0x10101000L
            if (DH_size(*p_dhkey)<(ks+7)/8 || (*p_dhkey)->priv_key==NULL) free_Buffer(&pki);    // v1.1.0
#else
            const BIGNUM* priv_key = DH_get0_priv_key(*p_dhkey);
            if (DH_size(*p_dhkey)<(ks+7)/8 || priv_key==NULL) free_Buffer(&pki);              // v1.1.0
#endif
        }
    }

    if (pki.buf==NULL) {
        //DEBUG_MODE PRINT_MESG("Generate DH public key.");
        pki = gen_DHspki(ks, p_dhkey);
        //DEBUG_MODE PRINT_MESG("... done.\n");

        fp = file_chmod_open(filename, (char*)"w", S_IRUSR | S_IWUSR);
        if (fp!=NULL) {
            save_DHspki_with_private(pki, fp, *p_dhkey);
            fclose(fp);
        }
    }

    //DEBUG_MODE {
    //    Buffer enc;
    //    enc = encode_base64_Buffer(pki);
    //    DEBUG_MESG("SPKI = [%s]\n", enc.buf);
    //    free_Buffer(&enc);
    //}
    return pki;
}



/**
Buffer  gen_DHspki(int ks, DH** p_dhkey)

Diffie-Hellman の鍵を生成し,公開鍵の完全な情報（X.509の SubjectPublicKeyInfo）のDER形式で返す．@n
p_dhkey に DHの鍵パラメータが返る．

@param  ks      鍵長(Bit)．512/1024/2048 を指定．
@param  p_dhkey DHの鍵パラメータが返る．要 DH_free().
@return DER形式の公開鍵情報．
*/
Buffer  gen_DHspki(int ks, DH** p_dhkey)
{
    int  sz, n, code;
    Buffer px, pp, pk;

    pk = init_Buffer();
    if (p_dhkey==NULL) return pk;

    //if (!RAND_load_file("/dev/random", 1024)) return pk;
    //DEBUG_MODE PRINT_MESG("Load /dev/urandom.\n");
    if (!RAND_load_file("/dev/urandom", 1024)) return pk;

    //DEBUG_MODE PRINT_MESG("Generate parameters.\n");
    do {
        if (*p_dhkey!=NULL) DH_free(*p_dhkey);
        *p_dhkey = DH_new();

#if OPENSSL_VERSION_NUMBER < 0x10101000L
        *p_dhkey = DH_generate_parameters(ks, DH_GENERATOR_2, NULL, NULL);
#else
        n = DH_generate_parameters_ex(*p_dhkey, ks, DH_GENERATOR_2, NULL);
#endif
        n = DH_check(*p_dhkey, &code);
    } while (n!=1 || code!=0);

    //DEBUG_MODE PRINT_MESG("Generate key.\n");
    sz = DH_generate_key(*p_dhkey);                 // 公開鍵(DH->pub_key)と秘密鍵(DH->priv_key)の生成
    if (sz==0) {
        DH_free(*p_dhkey);
        *p_dhkey = NULL;
        return pk;
    }

    //DEBUG_MODE PRINT_MESG("Check key size.\n");
    sz = i2d_DHparams(*p_dhkey, NULL);              // パラメタのサイズを検査 
    pp = px = make_Buffer(sz);                      // パラメタを入れるメモリを確保 
    if (pp.buf==NULL) {
        DH_free(*p_dhkey);
        *p_dhkey = NULL;
        return pk;
    }
    pp.vldsz = i2d_DHparams(*p_dhkey, &(px.buf));   // パラメタ(P,G鍵)を DER形式へ．pp.bufに格納． 
                                                    // px.bufは破壊される．

#if OPENSSL_VERSION_NUMBER < 0x10101000L
    sz = BN_num_bytes((*p_dhkey)->pub_key);
#else
    const BIGNUM* pub_key = DH_get0_pub_key(*p_dhkey);
    sz = BN_num_bytes(pub_key);
#endif

    px = make_Buffer(sz);
    if (px.buf==NULL) {
        DH_free(*p_dhkey);
        *p_dhkey = NULL;
        free_Buffer(&pp);
        return pk;
    }

#if OPENSSL_VERSION_NUMBER < 0x10101000L
    px.vldsz = BN_bn2bin((*p_dhkey)->pub_key, px.buf);
#else
    px.vldsz = BN_bn2bin(pub_key, px.buf);
#endif

    pk = join_DHpubkey(pp, px);                    // pp -> DHパラメタ(P,G鍵),  px-> Y鍵

    free_Buffer(&pp);
    free_Buffer(&px);

    return pk;
}



/**
Buffer  gen_DHspki_fs(Buffer pki, DH** p_dhkey)

Generate DH SPKI from Server's SPKI.

サーバの公開鍵の情報（X.509の SubjectPublicKeyInfo, DER形式）の中の P,G鍵から
自分の公開鍵情報（X.509 SPKI, DER形式）を作り出す．@n

@param  pki     相手の公開鍵情報（DER形式）
@param  p_dhkey DHの鍵パラメータが返る．要 DH_free().
@return 自分の公開鍵の情報（X.509の SubjectPublicKeyInfo, DER形式）
*/
Buffer  gen_DHspki_fs(Buffer pki, DH** p_dhkey)
{
    int  sz, n = 0, code;
    Buffer px, pp, pk;
    Buffer pkey, gkey;

    //
    pk = init_Buffer();
    if (p_dhkey==NULL) return pk;
    //
    if (*p_dhkey!=NULL) DH_free(*p_dhkey);
    *p_dhkey = DH_new();
    if (*p_dhkey==NULL) return pk;

    pkey = get_DHPkey(pki);
    gkey = get_DHGkey(pki);
    if (pkey.buf==NULL || gkey.buf==NULL) {
        free_Buffer(&pkey);
        free_Buffer(&gkey);
        DH_free(*p_dhkey);
        *p_dhkey = NULL;
        return pk;
    }

#if OPENSSL_VERSION_NUMBER < 0x10101000L
    (*p_dhkey)->p = BN_bin2bn((const unsigned char*)(pkey.buf), pkey.vldsz, NULL);
    (*p_dhkey)->g = BN_bin2bn((const unsigned char*)(gkey.buf), gkey.vldsz, NULL);
#else
    BIGNUM* dhp_bn = BN_bin2bn((const unsigned char*)(pkey.buf), pkey.vldsz, NULL);
    BIGNUM* dhg_bn = BN_bin2bn((const unsigned char*)(gkey.buf), gkey.vldsz, NULL);
    //
    if (dhp_bn!=NULL && dhg_bn!=NULL) {
        DH_set0_pqg(*p_dhkey, dhp_bn, NULL, dhg_bn);
    }
    else {
        DH_free(*p_dhkey);
        *p_dhkey = NULL;
    }
    //BN_free(dhp_bn);  // DHkeyの中身も消えてしまうので ここでは freeしない
    //BN_free(dhg_bn);
#endif

    free_Buffer(&pkey);
    free_Buffer(&gkey);

    //
    if (*p_dhkey!=NULL) n = DH_check(*p_dhkey, &code);
    if (n!=1 || code!=0) {
        if (*p_dhkey!=NULL) DH_free(*p_dhkey);
        *p_dhkey = NULL;
        return pk;
    }

    sz = DH_generate_key(*p_dhkey);
    if (sz==0) {
        DH_free(*p_dhkey);
        *p_dhkey = NULL;
        return pk;
    }

    //
    sz = i2d_DHparams(*p_dhkey, NULL);                  // パラメタのサイズを検査 
    pp = px = make_Buffer(sz);                          // パラメタを入れるメモリを確保 
    if (pp.buf==NULL) {
        DH_free(*p_dhkey);
        *p_dhkey = NULL;
        return pk;
    }
    pp.vldsz = i2d_DHparams(*p_dhkey, &(px.buf));       // パラメタ(P,G鍵)を DER形式へ．pp.bufに格納． 
                                                        // px.bufは破壊される．
#if OPENSSL_VERSION_NUMBER < 0x10101000L
    sz = BN_num_bytes((*p_dhkey)->pub_key);
#else
    const BIGNUM* pub_key = DH_get0_pub_key(*p_dhkey);
    sz = BN_num_bytes(pub_key);
#endif

    px = make_Buffer(sz);
    if (px.buf==NULL) {
        DH_free(*p_dhkey);
        *p_dhkey = NULL;
        free_Buffer(&pp);
        return px;
    }

#if OPENSSL_VERSION_NUMBER < 0x10101000L
    px.vldsz = BN_bn2bin((*p_dhkey)->pub_key, px.buf);  // 公開鍵(Y鍵)の DER形式 
#else
    px.vldsz = BN_bn2bin(pub_key, px.buf);              // 公開鍵(Y鍵)の DER形式 
#endif

    pk = join_DHpubkey(pp, px);                         // pp -> DHパラメタ(P,G鍵),  px-> Y鍵

    free_Buffer(&pp);
    free_Buffer(&px);

    return pk;
}



/**
Buffer get_DHsharedkey(Buffer pki, DH* dhkey)

相手の公開鍵の情報（X.509の SubjectPublicKeyInfo, DER形式）とDHkey（自分の鍵情報）を元に共通かぎを作り出す．

dhkkey には DH_generate_key(DHkey)（gen_DHspki() または gen_DHspki_fs()でも可）によって全てのパラメータ
（P,G,Y,秘密鍵）が設定されていなければならない． 

@param  pki    相手の公開鍵情報
@param  dhkey  自分の DH鍵
@return 共通鍵（バイナリ）

参考：man -M /usr/local/ssl/man bn
*/
Buffer get_DHsharedkey(Buffer pki, DH* dhkey)
{
    Buffer  ykey, skey;
    
    skey = init_Buffer();
    ykey = get_DHYkey(pki);
    if (ykey.buf==NULL) return skey;

    skey = get_DHsharedkey_fY(ykey, dhkey);
    
    free_Buffer(&ykey);
    return skey;
}



/**
Buffer  get_DHsharedkey_fY(Buffer ykey, DH* dhkey)

相手の（Diffie-Hellman の）Y鍵とDH Key（自分の鍵情報）を元に共通鍵を作り出す．

dhkkey には DH_generate_key(DHkey)（gen_DHspki() または gen_DHspki_fs()でも可）によって全てのパラメータ
（P,G,Y,秘密鍵）が設定されていなければならない． 

@param  ykey   相手の Y鍵（公開鍵）
@param  dhkey  自分の DH鍵
@return 共通鍵（バイナリ）

参考：man -M /usr/local/ssl/man bn
*/
Buffer  get_DHsharedkey_fY(Buffer ykey, DH* dhkey)
{
    int sz;
    Buffer  buf;

    buf = init_Buffer();
    if (dhkey==NULL) return buf;

    BIGNUM* yk = BN_bin2bn((const unsigned char*)(ykey.buf), ykey.vldsz, NULL);

    sz  = DH_size(dhkey);
    buf = make_Buffer(sz);
    buf.vldsz = sz;

    BN_free(yk);
    return buf;
}



/**
Buffer  get_DHYkey(Buffer param)

X.509の SubjectPublicKeyInfo のDER形式から，Diffie-Hellman のY鍵を取り出す． 

@param  param  X.509の SubjectPublicKeyInfo のDER形式
@return Y鍵（バイナリ）

参考：
@code
    [SEQ  [SEQ  [OBJ  Algor]
                [SEQ  [INT  P-key]
                      [INT  G-key]
                      [INT  P-keysize - 1]
                ]
          ]
          [BIT  Seed(0x00)  [INT  Y-key]]
    ]
@endcode
*/
Buffer  get_DHYkey(Buffer param)
{
    int     i, lp, sz=0;
    Buffer  pp;
    pp = init_Buffer();

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;
    sz = sz + lp;

    sz = skip_DER_node(param, JBXL_ASN1_BIT, sz, &lp);
    if (sz<0) return pp;
    sz++;                        // for Seed(0x00)

    sz = skip_DER_node(param, JBXL_ASN1_INT, sz, &lp);
    if (sz<0) return pp;

    pp = make_Buffer(lp);
    if (pp.buf==NULL) return pp;
    for (i=0; i<lp; i++) pp.buf[i] = param.buf[sz+i];
    pp.vldsz = lp;
    return pp;
}



/**
Buffer  get_DHPkey(Buffer param)

X.509の SubjectPublicKeyInfo のDER形式から，Diffie-Hellman のP鍵を取り出す． 

@param  param  X.509の SubjectPublicKeyInfo のDER形式
@return P鍵（バイナリ）

参考：
@code
    [SEQ  [SEQ  [OBJ  Algor]
                [SEQ  [INT  P-key]
                      [INT  G-key]
                      [INT  P-keysize - 1]
                ]
          ]
          [BIT  Seed(0x00)  [INT  Y-key]]
    ]
@endcode
*/
Buffer  get_DHPkey(Buffer param)
{
    int     i, lp, sz=0;
    Buffer  pp;
    pp = init_Buffer();

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;

    sz = skip_DER_node(param, JBXL_ASN1_OBJ, sz, &lp);
    if (sz<0) return pp;
    sz = sz + lp;

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;

    sz = skip_DER_node(param, JBXL_ASN1_INT, sz, &lp);
    if (sz<0) return pp;

    pp = make_Buffer(lp);
    if (pp.buf==NULL) return pp;
    for (i=0; i<lp; i++) pp.buf[i] = param.buf[sz+i];
    pp.vldsz = lp;
    return pp;
}



/**
Buffer  get_DHGkey(Buffer param)

X.509の SubjectPublicKeyInfo のDER形式から，Diffie-Hellman のG鍵を取り出す． 
普通は 2 か 5．

@param  param  X.509の SubjectPublicKeyInfo のDER形式
@return G鍵（バイナリ）

参考：
@code
    [SEQ  [SEQ  [OBJ  Algor]
                [SEQ  [INT  P-key]
                      [INT  G-key]
                      [INT  P-keysize - 1]
                ]
          ]
          [BIT  Seed(0x00)  [INT  Y-key]]
    ] 
@endcode
*/
Buffer  get_DHGkey(Buffer param)
{
    int     i, lp, sz=0;
    Buffer  pp;
    pp = init_Buffer();

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;

    sz = skip_DER_node(param, JBXL_ASN1_OBJ, sz, &lp);
    if (sz<0) return pp;
    sz = sz + lp;

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;

    sz = skip_DER_node(param, JBXL_ASN1_INT, sz, &lp);
    if (sz<0) return pp;
    sz = sz + lp;

    sz = skip_DER_node(param, JBXL_ASN1_INT, sz, &lp);
    if (sz<0) return pp;

    pp = make_Buffer(lp);
    if (pp.buf==NULL) return pp;
    for (i=0; i<lp; i++) pp.buf[i] = param.buf[sz+i];
    pp.vldsz = lp;
    return pp;
}



/**
Buffer  get_DHalgorism(Buffer param)

X.509の SubjectPublicKeyInfo のDER形式から，アルゴリズム(Algor)を取り出す． 

@param  param  X.509の SubjectPublicKeyInfo のDER形式
@return アルゴリズム（バイナリ）

参考：
@code
    [SEQ  [SEQ  [OBJ  Algor]
                [SEQ  [INT  P-key]
                      [INT  G-key]
                      [INT  P-keysize - 1]
                ]
          ]
          [BIT  Seed(0x00)  [INT  Y-key]]
    ]  
@endcode
*/
Buffer  get_DHalgorism(Buffer param)
{
    int     i, lp, sz=0;
    Buffer  pp;
    pp = init_Buffer();

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;

    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, sz, &lp);
    if (sz<0) return pp;

    sz = skip_DER_node(param, JBXL_ASN1_OBJ, sz, &lp);
    if (sz<0) return pp;

    pp = make_Buffer(lp);
    if (pp.buf==NULL) return pp;
    for (i=0; i<lp; i++) pp.buf[i] = param.buf[sz+i];
    pp.vldsz = lp;
    return pp;
}


/**
Buffer  get_DHprivatekey(DH* dhkey)

DH鍵 dh からプライベート鍵を取り出して返す．

@param  dhkey  DH鍵．
@return プライベート鍵
*/
Buffer  get_DHprivatekey(DH* dhkey)
{
    int    sz;
    Buffer pv;
 
#if OPENSSL_VERSION_NUMBER < 0x10101000L
    sz = BN_num_bytes(dhkey->priv_key);
//#elif OPENSSL_VERSION_NUMBER < 0x30000000L
#else
    const BIGNUM* priv_key = DH_get0_pub_key(dhkey);
    sz = BN_num_bytes(priv_key);
#endif

    pv = make_Buffer(sz);
    if (pv.buf==NULL) return pv;

#if OPENSSL_VERSION_NUMBER < 0x10101000L
    pv.vldsz = BN_bn2bin(dhkey->priv_key, pv.buf);
//#elif OPENSSL_VERSION_NUMBER < 0xF0000000L
#else
    pv.vldsz = BN_bn2bin(priv_key, pv.buf);
#endif

    return pv;
}



/**
Buffer  join_DHpubkey(Buffer param, Buffer key)

Diffie-Hellman の鍵交換のための X.509の SubjectPublicKeyInfo を取得する．
DHパラメタ(P+G)とDH鍵(Y)のDER形式を結合して,完全な鍵情報のDER形式をつくる．

@param  param  DHパラメータ(P,G鍵)．i2d_DHparams()による DER形式．
@param  key    DH公開鍵(Y鍵)の DER形式．
@return DER形式の公開鍵情報．[[Algor + [P + G + PKeySize]] + [BIT [Y]]]


参考： param のデータ形式は
@code
    [SEQ  [INT  P-key]
          [INT  G-key]
    ]
@endcode
である．これと Y-key から 下記のデータを生成する．
@code
    [SEQ  [SEQ  [OBJ  Algor]
                [SEQ  [INT  P-key]
                      [INT  G-key]
                      [INT  P-keysize - 1]
                ]  
          ]
          [BIT(未使用bit無し)  [INT  Y-key]]
    ]  
@endcode

@attention
DHのアルゴリズムは {0x06,0x07,0x2a,0x86,0x48,0xce,0x3e,0x02,0x01} だと思うのだが,RSAのアルゴリズムで動くようだ？
*/
Buffer  join_DHpubkey(Buffer param, Buffer key)
{
//    unsigned char dh_algor[]={0x06,0x07,0x2a,0x86,0x48,0xce,0x3e,0x02,0x01};            // DH  ?
    unsigned char dh_algor[]={0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x03,0x01};    // RSA ?
                                            // ↑ RSAアルゴリズム？ 
    int len_dh_algor = 11;

    int ls, lp, la, sz;
    Buffer  px, pp, pm;

    // [P鍵 + G鍵] → pp 
    pp = init_Buffer();
    sz = skip_DER_node(param, JBXL_ASN1_SEQ_CNSTRCTD, 0, &lp);
    if (sz<0) return pp;

    pp.buf   = param.buf   + sz;
    pp.bufsz = param.bufsz - sz;
    pp.vldsz = param.vldsz - sz;

    // [鍵サイズ] → pm 
    px = int2bin_DER(key.vldsz*8-1);
    pm = node2DER(px, JBXL_ASN1_INT);
    ls = pm.vldsz;
    free_Buffer(&px);

    // pp[P鍵 + G鍵] + pm[鍵サイズ] → pm 
    px = make_Buffer(lp+ls); 
    memcpy(px.buf,    pp.buf, lp);
    memcpy(px.buf+lp, pm.buf, ls);  
    px.vldsz = lp + ls;
    free_Buffer(&pm);              // ppを freeしてはいけない 
    pm = node2DER(px, JBXL_ASN1_SEQ_CNSTRCTD); 
    ls = pm.vldsz;
    free_Buffer(&px);

    // dh_algor[アルゴリズム] + pm[P鍵 + G鍵 + 鍵サイズ] → px 
    la = len_dh_algor; 
    pp = make_Buffer(ls+la); 
    memcpy(pp.buf,  dh_algor, la);
    memcpy(pp.buf+la, pm.buf, ls);
    pp.vldsz = ls + la;
    free_Buffer(&pm);         
    px = node2DER(pp, JBXL_ASN1_SEQ_CNSTRCTD);
    ls = px.vldsz;
    free_Buffer(&pp);  

    // px[アルゴリズム + P鍵 + G鍵 + 鍵サイズ] + pp[Y鍵] → pm 
    pm = node2DER(key, JBXL_ASN1_INT);
    pp = node2DER(pm,  JBXL_ASN1_BIT);
    lp = pp.vldsz;
    free_Buffer(&pm); 
    pm = make_Buffer(ls+lp);
    memcpy(pm.buf,    px.buf, ls);
    memcpy(pm.buf+ls, pp.buf, lp);
    pm.vldsz = ls + lp;
    free_Buffer(&px);
    free_Buffer(&pp);  

    pp = node2DER(pm, JBXL_ASN1_SEQ_CNSTRCTD);
    free_Buffer(&pm);
    return pp;
}


#endif
