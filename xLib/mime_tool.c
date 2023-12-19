
/**
@brief   MIMEツール
@file    mime_tool.c
@author  Fumi.Iseki (C)
@date    2005 12/25
*/

#include "protocol.h"
#include "mime_tool.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
char*  get_mime_boundary(tList* list)

ヘッダリスト list から mime boundary を探し出す．

@verbatim
mime boundary 自身は先頭に '--' が付加されて返される．
@endverbatim 

@param  list  ヘッダリスト
@return '&ndash;&ndash;' + MIME Boundary
*/
char*  get_mime_boundary(tList* list)
{
    int    i;
    char*  str;
    char*  pp;
    Buffer buf;

    buf = search_protocol_header_value(list, (char*)MIME_CONTENTTYPE_LINE, (char*)MIME_BOUNDARY_LINE, 1);
    if (buf.buf==NULL) return NULL;
    pp = (char*)buf.buf;

    pp += strlen(MIME_BOUNDARY_LINE);
    if (*pp=='\"') pp++;

    str = (char*)malloc(strlen((const char*)pp)+3);
    memset(str, 0, strlen((const char*)pp)+3);
    str[0] = '-';
    str[1] = '-';
    strncpy(str+2, pp, strlen((const char*)pp));

    i = 2;
    while (str[i]!='\0' && str[i]!='\"') i++;
    str[i] = '\0';

    free_Buffer(&buf);
    return str;
}



/**
tList*  get_mime_filename(FILE* fp, char* bndry)

ファイルから mime boundaryを探し出し，Content-Type行の内容をリストに格納して返す．
複数行に渡る場合も結合して返す．

@param  fp     ファイルポインタ
@param  bndry  '&ndash;&ndash;' + MIMEの境界文字列

@return キー部にファイル名の入ったリスト
*/
tList*  get_mime_filename(FILE* fp, char* bndry)
{
    tList* lp = NULL;
    tList* ln = NULL;
    char*  pt;
    Buffer mime, buf;


    fseek(fp, 0, SEEK_SET);

    buf = make_Buffer(LBUF);
    fgets_Buffer(&buf, fp);
    
    // ファイルから MIMEヘッダを抽出    
    while (!feof(fp)) {
        if (!strcmp((char*)buf.buf, bndry)) {   // mime boundary を見つけた
            fgets_Buffer(&buf, fp);
            while(!feof(fp)) {
                ln = lp;
                lp = get_protocol_header_list_seq(lp, buf, ';', FALSE, FALSE);
                fgets_Buffer(&buf, fp);
                if (ln==lp) break;
            } 
        }
        else {
            fgets_Buffer(&buf, fp);
        }
    }
    free_Buffer(&buf);
    lp = find_tList_top(lp);

    mime = init_Buffer();

    // MIME_CONTEMT_LINE を取り出す．
    if (lp!=NULL) {
        int  i, nn = 1;
        tList* lt = NULL;
        ln = NULL;

        Loop {
            mime = search_protocol_header(lp, (char*)MIME_CONTENT_LINE, nn);
            if (mime.buf==NULL) break;
            
            pt = strstrcase((char*)mime.buf, MIME_NAMEEQ_LINE);             // name=
            if (pt!=NULL) {
                pt += strlen(MIME_NAMEEQ_LINE);
            }
            else {
                pt = strstrcase((char*)mime.buf, MIME_FILENAMESTAR_LINE);   // filename*
                if (pt!=NULL) {
                    i = 0;
                    while(pt[i]!='\0' && pt[i]!='=') i++;
                    if (pt[i]=='=') pt += i + 1;
                }
            }

            if (pt!=NULL) {
                pt = decode_mime_string(pt);                // デコード
                if (pt!=NULL) {                             // リストに格納
                    lt = add_tList_node_str(lt, pt, NULL);
                    if (ln==NULL) ln = lt;
                    free(pt);
                }
            }

            nn++;
            free_Buffer(&mime);
        }
    }

    del_all_tList(&lp);
    return ln;
}



/**
tList*  get_mime_filenameffn(char* fname, char* bndry)

ファイルから mime boundaryを探し出し，CONTENT-TYPE行の内容をリストに格納して返す．
複数行に渡る場合も結合して返す．

@param  fname  ファイル名
@param  bndry  '&ndash;&ndash;' + MIMEの境界文字列

@return キー部にファイル名の入ったリスト
*/
tList*  get_mime_filenameffn(char* fname, char* bndry)
{
    tList* lp;
    FILE*  fp;

    if (bndry==NULL) return NULL;

    fp = fopen(fname, "rb");
    if (fp==NULL) return NULL;

    lp = get_mime_filename(fp, bndry);
    fclose(fp);

    return lp;
}
    


/**
char*  decode_mime_rfc2047(char* mime)

RFC2047で定義された文字列をデコードする．文字列全体が，" " で括られていても可．

文字列中には余分な空白は無く，行間はCR+LF（またはLF）で区切られているものとする．
エンコードされていない文字列が混在してもデコード可．

@param  mime  デコードされる文字列．
@return デコードされた文字列．
*/
char*  decode_mime_rfc2047(char* mime)
{
    char* buf;
    char* ppb;
    char* dec;
    char* ppd;
    char* str;
    char* ret;
    int   i, j, len, decf=0, sz;
    
    if (mime==NULL) return NULL;

    buf = ppb = (char*)malloc(strlen((const char*)mime)+1);
    if (buf==NULL) return NULL;
    memcpy(buf, mime, strlen(mime));
    buf[strlen(mime)] = '\0';

    dec = ppd = (char*)malloc(strlen((const char*)mime)+1);
    if (dec==NULL) {
        free(buf);
        return NULL;
    }
    memset(dec, 0, strlen((const char*)mime)+1);

    if (ppb[0]=='"') ppb++;
    i = 0;
    while(ppb[i]!='\0' && ppb[i]!='"') i++;
    ppb[i] = '\0';

    while (ppb[0]!='\0') {
        str = awk(ppb, '?', 1);
        len = strlen((const char*)str);
        if (str[len-1]=='=') {  // エンコードの開始地点  =?
            len--;
            str[len] = '\0';
            for (i=0, j=0; i<len; i++) {
                if (str[i]!=CHAR_CR && str[i]!=CHAR_LF) ppd[j++] = str[i];
            }
            ppd += j;
            ppb += len + 2;
            
            free(str);
            str = awk(ppb, '?', 1);
            if (str==NULL) {
                free(dec);
                free(buf);
                return NULL;
            }
            if (strcasecmp(MIME_ISO2022JP_LINE, str)) {
                free(str);
                free(dec);
                free(buf);
                return NULL;
            }
            ppb += strlen(MIME_ISO2022JP_LINE) + 1;

            i++;
            free(str);
            str = awk(ppb, '?', 1);
            if (str==NULL) {
                free(dec);
                free(buf);
                return NULL;
            }
            if (!strcasecmp("B", str))      decf = 1;   // Base64
            else if (!strcasecmp("Q", str)) decf = 2;   // Quoted Printable
            else {
                free(str);
                free(dec);
                free(buf);
                return NULL;
            }
            ppb += 2;

            i++;
            free(str);
            str = awk(ppb, '?', 1);
            if (str==NULL) {
                free(dec);
                free(buf);
                return NULL;
            }
            len = strlen((const char*)str);
            if (ppb[len+1]!='=') {      // エンコード終了地点でない  ?=
                free(str);
                free(dec);
                free(buf);
                return NULL;
            }
            ppb += len + 2;

            ret = NULL;
            if (decf==1)      ret = (char*)decode_base64((unsigned char*)str, &sz);
            else if (decf==2) ret = (char*)decode_quoted_printable((unsigned char*)str, &sz);
            if (ret==NULL) {
                free(str);
                free(dec);
                free(buf);
                return NULL;
            }
            memcpy(ppd, ret, sz);
            ppd += sz;

            free(str);
            free(ret);
            decf = 0;
        }
        else {
            if (len==0) break;
            for (i=0, j=0; i<len; i++) {
                if (str[i]!=CHAR_CR && str[i]!=CHAR_LF) ppd[j++] = str[i];
            }
            ppd += j;
            ppb += len;
        }
    }

    return dec;
}



/**
char*  decode_mime_rfc2231(char* mime)

mime の内容を MIME + RFC2231 としてデコードする．

mimeには余分な空白，ダブルクォーテーション は無く，行間は ;+CR+LF（またはCR+LF，またはLF）で
区切られているものとする． つまり，mime は filename*#*= 以下の部分に相当．

@param  mime  デコードされる文字列．
@return デコードされた文字列．
*/
char*  decode_mime_rfc2231(char* mime)
{
    char* buf;
    char* dec;
    int   i, j, sz;
    
    if (mime==NULL) return NULL;

    buf = awk(mime, '\'', 1);
    if (buf==NULL) return NULL;
    if (strcasecmp(MIME_ISO2022JP_LINE, buf)) {
        free(buf);
        return NULL;
    }
    mime += strlen(MIME_ISO2022JP_LINE) + 1;
    free(buf);

    buf = awk(mime, '\'', 1);
    if (buf==NULL) return NULL;
    if (strcasecmp("JA", buf)) {
        return NULL;
    }
    mime += strlen("JA") + 1;
    free(buf);

    buf = (char*)malloc(strlen((const char*)mime)+1);
    if (buf==NULL) return NULL;
    memset(buf, 0, strlen((const char*)mime)+1);

    i = j = 0;
    while(mime[i]!='\0') {
        if (mime[i]!=' ' && mime[i]!=';' && mime[i]!=CHAR_CR && mime[i]!=CHAR_LF) buf[j++] = mime[i];
        i++;
    }

    dec = (char*)decode_urlenc((unsigned char*)buf, &sz);
    if (dec!=NULL) dec[sz] = '\0';
    free(buf);

    return dec;
}



/**
char*  decode_mime_string(char* mime)

MIME文字列をデコードする．文字列が " で括られている場合は " を削除する．
また，文字列中の CR, LF は無視する．
*/
char*  decode_mime_string(char* mime)
{
    int   kind; 
    char* buf=NULL;

    kind = get_mime_enckind(mime);
    if (kind==MIME_ERR_ENCODE) return NULL;

    if (kind==MIME_UNKNOWN_ENCODE) {
        int   i, j, len;
        char* str;

        len = strlen((const char*)mime);
        str = (char*)malloc(len+1);
        if (str==NULL) return NULL;

        i = j = 0;
        while(mime[i]!='\0') {
            if (mime[i]!=CHAR_CR && mime[i]!=CHAR_LF) str[j++] = mime[i];
            i++;
        }
        str[j] = '\0';

        len = strlen(str);
        buf = (char*)malloc(len+1);
        if (buf==NULL) {
            free(str);
            return NULL;
        }
        if (str[0]=='\"' && str[len-1]=='\"' && str[len-2]!='\\') {
            memcpy(buf, str+1, len-2);
            buf[len-2] = '\0';
        }
        else {
            memcpy(buf, str, len);
            buf[len] = '\0';
        }
        free(str);
    }
    //
    else if (kind==MIME_BASE64_ENCODE || kind==MIME_QUTDPRNTBL_ENCODE) {
        buf = decode_mime_rfc2047(mime);
    }
    //
    else if (kind==MIME_URL_ENCODE) {
        int   i, j;
        char* str;

        str = (char*)malloc(strlen(mime)+1);
        if (str!=NULL) {
            memset(str, 0, strlen(mime)+1); 
            i = j = 0;  
            while(mime[i]!='\0') {
                str[j++] = mime[i];
                if (mime[i]==CHAR_LF) {
                    while(mime[i]!='\0' && mime[i]!='=') i++;       
                    if (mime[i]=='=') i++;
                }
                else i++;
            }
            buf = decode_mime_rfc2231(str);
            free(str);
        }
    }

    return buf;
}



/**
char*  encode_mime_string(char* str, int kind)

MIME でエンコードする（一行分のみ）．

@param  str   エンコードする文字データ．
@param  kind  @b MIME_BASE64_ENCODE      MIME + BASE64
@param  kind  @b MIME_QUTDPRNTBL_ENCODE  MIME + quoted printable
@param  kind  @b MIME_URL_ENCODE         RFC2231 (URL encode)

@return エンコードされた文字列．要 free
*/
char*  encode_mime_string(char* str, int kind)
{
    char*   buf;
    Buffer  mime;

    mime = make_Buffer(LBUF);
    if (mime.buf==NULL) return NULL;

    if (kind==MIME_BASE64_ENCODE) {
        buf = (char*)encode_base64((unsigned char*)str, -1);
        if (buf==NULL) {
            free_Buffer(&mime);
            return NULL;
        }
        copy_s2Buffer(MIME_BASE64, &mime);
        cat_s2Buffer(buf, &mime);
        cat_s2Buffer("?=", &mime);
        free(buf);
    }
    //
    else if (kind==MIME_QUTDPRNTBL_ENCODE) {
        buf = (char*)encode_quoted_printable((unsigned char*)str, -1);
        if (buf==NULL) {
            free_Buffer(&mime);
            return NULL;
        }
        copy_s2Buffer(MIME_QUTDPRNTBL, &mime);
        cat_s2Buffer(buf, &mime);
        cat_s2Buffer("?=", &mime);
        free(buf);
    }
    //
    else if (kind==MIME_URL_ENCODE) {
        buf = (char*)encode_urlenc((unsigned char*)str, -1);
        if (buf==NULL) {
            free_Buffer(&mime);
            return NULL;
        }
        copy_s2Buffer(MIME_RFC2231, &mime);
        cat_s2Buffer(buf, &mime);
        free(buf);
    }
    //
    else {
        copy_s2Buffer(str, &mime);
    }

    return (char*)mime.buf;
}



/**
int  get_mime_enckind(char* mime)

MIME文字列 からエンコードの種類を獲得する．文字列は " " で括られていても可．

文字列の間には余分な空白は無く，行間は（存在すれば）CR+LF（またはLF）で区切
られているものとする．

@param  mime  検査される文字列．

@return エンコードの種類．
@retval MIME_ERR_ENCDE         (-1) エラー
@retval MIME_UNKNOWN_ENCODE    ( 0) 不明もしくはエンコードなし．
@retval MIME_BASE64_ENCODE     ( 1) MIME + BASE64
@retval MIME_QUTDPRNTBL_ENCODE ( 2) MIME + quoted printable
@retval MIME_URL_ENCODE        ( 3) RFC2231 (URL encode)
*/
int  get_mime_enckind(char* mime)
{
    unsigned char* ps;
    unsigned char* pe;

    if (mime==NULL) return MIME_ERR_ENCODE;

    if ((ps=(unsigned char*)strstrcase((const char*)mime, MIME_BASE64))!=NULL) {
        if ((pe=(unsigned char*)strstr((const char*)mime, "?="))!=NULL) {
            if (pe>ps+strlen(MIME_BASE64)) {
                return MIME_BASE64_ENCODE;
            }
        }
    }

    if (strstrcase(mime, MIME_QUTDPRNTBL)!=NULL) {
        return MIME_QUTDPRNTBL_ENCODE;
    }

    if (strstrcase(mime, MIME_RFC2231)!=NULL) {
        return MIME_URL_ENCODE;
    }

    return MIME_UNKNOWN_ENCODE;
}

