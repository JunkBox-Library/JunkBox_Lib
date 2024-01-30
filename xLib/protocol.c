/**
@brief   プロトコル解析ライブラリ
@file    protocol.c
@author  Fumi.Iseki (C)
@date    2009 2/5
*/

#include "protocol.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Protocol Header
//

/**
tList*  get_protocol_header_list(Buffer buf, char deli, int fstline, int rcntnt)

buf内の プロトコルヘッダ（key'deli' data の形式）を分解してリストに格納する．'deli'は境界文字．
fstlineが TRUEの場合，ヘッダの一行目は HDLIST_FIRST_LINE_KEY のキーをつけてリストに格納する．

また，この関数はバッファリングなどは行わないので，ヘッダにコンテンツの最初の部分が紛れ込む可能性がある．
rcntntが TRUEの場合は，HDLIST_CONTENTS_KEY をキーにしてコンテンツをリストに格納する．FALSEの場合は無視．
コンテンツのサイズが大きく，順次読み込まれる場合，HDLIST_CONTENTS_KEYノードは複数生成される．

ヘッダの値が複数行になる場合，ヘッダ種別 HDLIST_CONTINUE として，次のノードに格納される．

@param  buf      ヘッダを格納した変数
@param  deli     ヘッダの見出し(Key)との境界文字．HTTPや SMTPでは ':'
@param  fstline  一行目を特別扱いにするか？ 一行目が key'deli' data の形式でないプロトコル用, ex) HTTP, SIP
@param  rcntnt   コンテンツも読むか？

@return ヘッダ情報を格納したリストへのポインタ．
*/
tList*  get_protocol_header_list(Buffer buf, char deli, int fstline, int rcntnt)
{
    tList* lp;

    if (buf.buf==NULL) return NULL;

    lp = get_protocol_header_list_seq(NULL, buf, deli, fstline, rcntnt);
    if (lp!=NULL) lp = find_tList_top(lp);

    return lp;
}


/**
Buffer  restore_protocol_header(tList* list, char* deli, int mode, int* hdsz)
 
リスト listに保存されたデータからヘッダデータを復元する．
mode==ON なら listに紛れ込んでいるコンテンツの内容も加えて返す．

get_protocol_header_list() の逆．
*/
Buffer  restore_protocol_header(tList* list, char* deli, int mode, int* hdsz)
{
    Buffer buf;

    buf = init_Buffer();
    if (list==NULL) return buf;

    buf = make_Buffer(RECVBUFSZ);

    while(list!=NULL) {
        if (!strcmp((const char*)(list->ldat.key.buf), HDLIST_FIRST_LINE_KEY)) {
            copy_Buffer(&(list->ldat.val), &buf);
            cat_s2Buffer("\r\n", &buf);
        }
        else if (!strcmp((const char*)(list->ldat.key.buf), HDLIST_CONTINUE)) {
            buf.buf[buf.vldsz] = CHAR_TAB;
            buf.vldsz++;
            cat_Buffer(&(list->ldat.val), &buf);
            cat_s2Buffer("\r\n", &buf);
        }
        else if (!strcmp((const char*)(list->ldat.key.buf), HDLIST_END_KEY)) {
            break;
        }
        else if (strcmp((const char*)(list->ldat.key.buf), HDLIST_CONTENTS_KEY)) {
            cat_Buffer(&(list->ldat.key), &buf);
            cat_s2Buffer(deli, &buf);
            cat_Buffer(&(list->ldat.val), &buf);
            cat_s2Buffer("\r\n", &buf);
        }

        list = list->next;
    }
    if (buf.vldsz>0) cat_s2Buffer("\r\n", &buf);

    if (hdsz!=NULL) *hdsz = buf.vldsz;

    if (mode==ON) {
        int nn = 1;
        tList* pl = strncmp_tList(list, (char*)HDLIST_CONTENTS_KEY, 0, nn);
        while (pl!=NULL && pl->ldat.val.buf!=NULL) {
            cat_Buffer(&(pl->ldat.val), &buf);
            pl = strncmp_tList(list, (char*)HDLIST_CONTENTS_KEY, 0, ++nn);
        }
    }

    return buf;
}


/**
Buffer  restore_protocol_contents(tList* list)
 
リスト listに保存されたデータからコンテンツを復元する．
*/
Buffer  restore_protocol_contents(tList* list)
{
    Buffer buf;
    tList* lp;
    int nn = 1;

    buf = make_Buffer(BUFSZ);

    lp = strncmp_tList(list, (char*)HDLIST_CONTENTS_KEY, 0, nn);
    while (lp!=NULL && lp->ldat.val.buf!=NULL) {
        cat_Buffer(&(lp->ldat.val), &buf);
        lp = strncmp_tList(list, (char*)HDLIST_CONTENTS_KEY, 0, ++nn);
    }

    return buf;
}


/**
void    set_protocol_contents(tList* list, Buffer contents)

@attention プロトコルに依存するので，コンテンツサイズの再計算は行わない．
*/
void    set_protocol_contents(tList* list, Buffer contents)
{
    while (list!=NULL) {
        if (list->ldat.key.buf!=NULL && !strcmp((char*)list->ldat.key.buf, HDLIST_CONTENTS_KEY)) {
            free_Buffer(&list->ldat.val);
            list->ldat.val = dup_Buffer(contents);

            while (is_header_continue(list)) del_tList_node(&(list->next));
            break;
        }

        list = list->next;
    }

    return;
}


/**
tList*  get_protocol_header_list_seq(tList* lp, Buffer buf, char deli, int fstline, int rcntnt)

buf内の プロトコルヘッダ（key'deli' data の形式）を分解してリストに格納する．'deli'は境界文字．
シーケンシャルに随時呼び出すことが可能．@n
lp==NULL で状態（静的変数）がリセットされので，最初は lpを NULLにすること．

fstlineが TRUEの場合，ヘッダの一行目は HDLIST_FIRST_LINE_KEY のキーをつけてリストに格納する．

また，この関数はバッファリングなどは行わないので，ヘッダにコンテンツの最初の部分が紛れ込む可能性がある．
rcntntが TRUEの場合は，HDLIST_CONTENTS_KEY をキーにしてコンテンツをリストに格納する．FALSEの場合は無視．
コンテンツのサイズが大きく，順次読み込まれる場合，HDLIST_CONTENTS_KEYノードは複数生成される．

ヘッダの値が複数行になる場合，ヘッダ種別 HDLIST_CONTINUE として，次のノードに格納される．

lp!=NULL または fstlineがFALSE の場合は一行目の処理は行わない．

@param  lp       ヘッダ情報を格納するリストへのポインタ．
                 NULLの場合はリストが新しく作成される．NULLでない場合はそのリストにヘッダ情報が追加される．
@param  buf      ヘッダを格納した変数
@param  deli     ヘッダの見出し(Key)との境界文字．HTTPや SMTPでは ':'
@param  fstline  一行目を特別扱いにするか？ 一行目が key'deli' data の形式でないプロトコル用, ex) HTTP, SIP
@param  rcntnt   コンテンツも読むか？

@return 一番最後に作成したリストノードへのポインタ．
        リストのトップを得るには find_tList_top(tList* pp) を用いる．

@par 変数
@b in_contents  コンテンツ内を処理中であることを表す．@n
@b crlfCount    処理中の CR,LFの数
*/
tList*  get_protocol_header_list_seq(tList* lp, Buffer buf, char deli, int fstline, int rcntnt)
{
    static int crlfCount  = 0;
    static int inContents = FALSE;
    int i=0, n=0, size;
    Buffer key, data;

    if (buf.buf==NULL) return NULL;

    size = buf.vldsz;
    data = make_Buffer(size+1);
    key  = make_Buffer(LBUF);

    if (lp==NULL) {
        crlfCount  = 0;
        inContents = FALSE;
    }

    // FIRST LINE 
    if (fstline && lp==NULL) {
        while(buf.buf[i]!=0x0a && buf.buf[i]!='\0' && i<size && n<size) {
            data.buf[n++] = buf.buf[i++];
        }
        if (data.buf[n-1]==0x0d) {
            i--;
            n--;
        }

        data.buf[n] = '\0';
        data.vldsz  = n;
        copy_s2Buffer(HDLIST_FIRST_LINE_KEY, &key);
        lp = add_tList_node_Buffer(NULL, key, data);

        if (buf.buf[i]=='\0' || i==size) {
            free_Buffer(&key);
            free_Buffer(&data);
            return lp;
        }
        clear_Buffer(&key);
        clear_Buffer(&data);
    }

    // HEADER
    while(buf.buf[i]!='\0' && i<size && !inContents) {
        // Check Previous Line's CR & LF
        if (i+1<size && buf.buf[i]==0x0d && buf.buf[i+1]==0x0a) {
            i = i + 2;
            if (crlfCount==1) {         // for previous called function
                copy_s2Buffer(HDLIST_END_KEY, &key);
                lp = add_tList_node_Buffer(lp, key, data);
                crlfCount  = 0;
                inContents = TRUE;
                break;  
            }
            else crlfCount = 1;
        }
        else if (buf.buf[i]==0x0a) {
            i = i + 1;
            if (crlfCount==1) {         // for previous called function
                copy_s2Buffer(HDLIST_END_KEY, &key);
                lp = add_tList_node_Buffer(lp, key, data);
                crlfCount  = 0;
                inContents = TRUE;
                break;  
            }
            else crlfCount = 1;
        }

        // 
        if (i+1<size && buf.buf[i]==0x0d && buf.buf[i+1]==0x0a) {
            i = i + 2;
            if (crlfCount==1) {
                copy_s2Buffer(HDLIST_END_KEY, &key);
                lp = add_tList_node_Buffer(lp, key, data);
                crlfCount  = 0;
                inContents = TRUE;
                break;  
            }
        }
        else if (i<size && buf.buf[i]==0x0a) {
            i = i + 1;
            if (crlfCount==1) {
                copy_s2Buffer(HDLIST_END_KEY, &key);
                lp = add_tList_node_Buffer(lp, key, data);
                crlfCount  = 0;
                inContents = TRUE;
                break;  
            }
        }

        if (buf.buf[i]=='\0' || i==size) break;

        // HEADER KEY
        n = 0;
        if (buf.buf[i]==CHAR_TAB || buf.buf[i]==' ') {
            copy_s2Buffer(HDLIST_CONTINUE, &key);
        }
        else {
            while(buf.buf[i]!=deli && buf.buf[i]!='\0' && i<size && n<LBUF) {
                key.buf[n++] = buf.buf[i++];
            }
            key.buf[n] = '\0';
            key.vldsz  = n;
        }
        while ((buf.buf[i]==deli ||buf.buf[i]==' '||buf.buf[i]==CHAR_TAB) && i<size) i++;

        // HEADER VALUE
        n = 0;
        while(buf.buf[i]!=0x0a && buf.buf[i]!='\0' && i<size && n<size) {
            data.buf[n++] = buf.buf[i++];
        }
        if (data.buf[n-1]==0x0d) {
            i--;
            n--;
        }
        data.buf[n] = '\0';
        data.vldsz  = n;

        lp = add_tList_node_Buffer(lp, key, data);

        crlfCount = 0;
        clear_Buffer(&key);
        clear_Buffer(&data);
    }

    // CONTENTS (Text or Binary)
    if (rcntnt && i<size && inContents) {
        crlfCount = 0;
        clear_Buffer(&key);
        clear_Buffer(&data);
        copy_s2Buffer(HDLIST_CONTENTS_KEY, &key);

        n = 0;
        while(i<size && n<size) {
            data.buf[n++] = buf.buf[i++];
        }
        data.vldsz = n;

        lp = add_tList_node_Buffer(lp, key, data);
    }
    
    free_Buffer(&key);
    free_Buffer(&data);

    return lp;
}


/**
tList*  get_protocol_header_list_file(char* fname, char deli, int fstline, int rcntnt)

ファイル fname からヘッダ情報を読み込んで分解し，リストに格納して返す．

@param  fname    読み込むファイル名
@param  deli     ヘッダの見出し(Key)との境界文字．HTTPや SMTPでは ':'
@param  fstline  一行目を特別扱いにするか？ 一行目が key'境界文字' data の形式でないプロトコル用, ex) HTTP, SIP
@param  rcntnt   コンテンツも読むか？

@return ヘッダ情報を格納したリストへのポインタ．
*/
tList*  get_protocol_header_list_file(char* fname, char deli, int fstline, int rcntnt)
{
    Buffer buf;
    tList* lp;

    if (fname==NULL) return NULL;

    buf = read_Buffer_file(fname);
    lp  = get_protocol_header_list(buf, deli, fstline, rcntnt);

    return lp;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// Search

/**
Buffer  search_protocol_header(tList* list, char* key, int no) 

ヘッダリストの中から no番目の keyノードを探し出し，値（ldat.val.buf）のコピーを返す．
値が複数行に渡る場合は，@\r@\n を挿入して行を結合する．
検索では keyの長さに合わせる．また, keyはケースインセンシティブ．

@param  list  検索対象のヘッダ方向を格納したリスト
@param  key   ヘッダ種別．
@param  no    同じヘッダ種別のノードが複数ある場合，何番目のノードかを指定する．1から数える．

@return ldat.val.buf のコピー．
*/
Buffer  search_protocol_header(tList* list, char* key, int no) 
{
    tList* pp;
    Buffer buf;

    buf = init_Buffer();
    if (list==NULL || key==NULL) return buf;
    
    pp = strncasecmp_tList(list, key, 0, no);       // 完全一致
    if (pp!=NULL) {
        buf = dup_Buffer(pp->ldat.val);

        while (is_header_continue(pp)) {
            cat_s2Buffer("\r\n", &buf);
            pp = pp->next;
            cat_Buffer(&(pp->ldat.val), &buf);
        }
    }
    
    return buf;
}


/**
Buffer  search_protocol_header_item(tList* list, char* key, int no, char deli, int nm) 

key をキーにした no番目のノードの値の中で，deliを区切りにした nm番目の項目（文字列）を返す．

@param  list  検索対象のヘッダ方向を格納したリスト
@param  key   ヘッダ種別．
@param  no    同じヘッダ種別が複数ある場合，何番目のノードかを指定する．1から数える．
@param  deli  ノード値（文字列）の区切り文字．
@param  nm    deli を区切り文字として何番目の項目か？ 1から数える．

@return 指定した項目（文字列）のコピー．
*/
Buffer  search_protocol_header_item(tList* list, char* key, int no, char deli, int nm) 
{
    Buffer buf, itm;

    buf = search_protocol_header(list, key, no);
    if (buf.buf==NULL) return buf;
    
    itm = cawk_Buffer(buf, deli, nm);
    free_Buffer(&buf);

    return itm;
}


/**
Buffer  search_protocol_header_value(tList* list, char* key, char* data, int no) 

ヘッダリストの中から no番目の keyノードを探し出し，dataで始まるノードの値（ldat.val.buf）のコピーを返す．

key, dataはケースインセンシティブ．data が複数行に継続している場合は，継続している行も単独の行として検査される．

@param  list  検索対象のヘッダ方向を格納したリスト
@param  key   ヘッダ種別．
@param  data  検索するヘッダ値の最初の文字．NULL なら全てと一致．
@param  no    同じヘッダ種別のノードが複数ある場合，何番目のノードかを指定する．1から数える．

@return 一致したノードのノード値のコピー．
*/
Buffer  search_protocol_header_value(tList* list, char* key, char* data, int no) 
{
    Buffer buf;
    char*  str;
    int    len, nm;

    buf = init_Buffer();
    if (list==NULL || key==NULL) return buf;

    if (data==NULL) {
        buf = search_protocol_header(list, key, no);
        return buf;
    }

    buf = init_Buffer();
    len = (int)strlen(data); 
    
    nm = 0;
    while (list!=NULL) {
        if (list->ldat.key.buf!=NULL && !strcasecmp((char*)list->ldat.key.buf, key)) {
            str = (char*)list->ldat.val.buf;

            if (str!=NULL && !strncasecmp(str, data, len)) {
                nm++;
                if (no==nm) {
                    buf = make_Buffer_bystr(str);
                    return buf;
                }
            }

            while (is_header_continue(list)) {
                list = list->next;
                str  = (char*)list->ldat.val.buf;
                if (str!=NULL && !strncasecmp(str, data, len)) {
                    nm++;
                    if (no==nm) {
                        buf = make_Buffer_bystr(str);
                        return buf;
                    }
                }
            }
        }

        list = list->next;
    }

    return buf;
}


/**
Buffer  search_protocol_header_partvalue(tList* list, char* key, char* data, int no) 

ヘッダリストの中から no番目の keyノードを探し出し，dataの文字列を含むノードの値（ldat.val.buf）のコピーを返す．

key, dataはケースインセンシティブ．data が複数行に継続している場合は，継続している行も単独の行として検査される．

@param  list  検索対象のヘッダ方向を格納したリスト
@param  key   ヘッダ種別．
@param  data  検索するヘッダ値の最初の文字．NULL なら全てと一致．
@param  no    同じヘッダ種別のノードが複数ある場合，何番目のノードかを指定する．1から数える．

@return 一致したノードのノード値のコピー．
*/
Buffer  search_protocol_header_partvalue(tList* list, char* key, char* data, int no) 
{
    Buffer buf;
    char*  str;
    int    nm;

    buf = init_Buffer();
    if (list==NULL || key==NULL) return buf;

    if (data==NULL) {
        buf = search_protocol_header(list, key, no);
        return buf;
    }

    buf = init_Buffer();
    //len = (int)strlen(data); 
    
    nm = 0;
    while (list!=NULL) {
        if (list->ldat.key.buf!=NULL && !strcasecmp((char*)list->ldat.key.buf, key)) {
            str = (char*)list->ldat.val.buf;

            if (str!=NULL && strstrcase(str, data)) {
                nm++;
                if (no==nm) {
                    buf = make_Buffer_bystr(str);
                    return buf;
                }
            }

            while (is_header_continue(list)) {
                list = list->next;
                str  = (char*)list->ldat.val.buf;
                if (str!=NULL && strstrcase(str, data)) {
                    nm++;
                    if (no==nm) {
                        buf = make_Buffer_bystr(str);
                        return buf;
                    }
                }
            }
        }

        list = list->next;
    }

    return buf;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// Set

/*
int  set_protocol_header(tList* list, char* key, char* value, int no, int add_mode)

リスト(lt)中の no番目の keyのノード値に valueを設定する．

no が 0以下の場合は，全ての keyノードの値に対して設定が行われる．
keyノードが存在せず，かつ mode==ON の場合は，リストの最後（コンテンツの前）に追加される．

set_value_tList() との違いは，追加時の追加の仕方のみ．(in Lib/tlist.c)

@param  list     処理対象のリスト
@param  key      設定を行うノードのキー部
@param  value    設定される文字列．
@param  no       何個目のノードに対して設定を行うか．1から数える．
                 0以下の場合はkeyが一致するすべてのノードに対して設定を行う．
@param  add_mod  この値がON かつ指定したノードが無い場合，ノードリストの最後（ただしコンテンツの前）に追加する．

@retval 0以上    設定されたノードの数．指定されたノードが存在しない場合は（追加された場合も）0
@retval 負数     エラー．
*/
int  set_protocol_header(tList* list, char* key, char* value, int no, int add_mode)
{
    int cn = set_value_tList(list, key, no, value, OFF);

    // Not Found
    if (add_mode==ON && cn==0) {
        tList* pm = strncmp_tList(list, (char*)HDLIST_END_KEY, 0, 1);
        if (pm!=NULL && pm->prev!=NULL) {
            add_tList_node_str(pm->prev, key, value);
        }
        else {
            add_tList_node_str(list, key, value);
        }
    }

    return cn;
}


/**
int  search_crlfcrlf(char* mesg) 

文字列中の空行を探す．改行コードは 0x0d, 0x0a または 0x0a

@return 空行後の次の行の先頭の位置．0 の場合は空行無し．
*/
int  search_crlfcrlf(char* mesg) 
{
    int cr = 0;     // dummy
    int lf = 0;
    int i;

    if (mesg==NULL)    return JBXL_ARGS_ERROR;
    if (mesg[0]==0x0a) return 2;
    if (mesg[0]==0x0d && mesg[1]==0x0a) return 2;

    i = 0;
    while(mesg[i]!='\0') {
        if      (mesg[i]==0x0d) cr++;   // not used
        else if (mesg[i]==0x0a) lf++;
        else {
            cr = lf = 0;
        }

        if (lf==2) return i+1;
        i++;
    }
    return 0;
}


/**
int  is_header_continue(tList* pp)

pp が指しているヘッダ値が次のリストへ続いているかどうか判定する．
プロトコル上では，ヘッダ値が複数行に渡る場合に相当する．
*/
int  is_header_continue(tList* pp)
{
    if (pp==NULL || pp->next==NULL || pp->next->ldat.key.buf==NULL) return FALSE;
    if (!strcmp((const char*)pp->next->ldat.key.buf, HDLIST_CONTINUE)) return TRUE;
    return FALSE;
}


/**
int  print_protocol_header(tList* pp)

pp のヘッダ部分（以外）を stderr に出力する．
*/
void  print_protocol_header(tList* pp, int content)
{
    if (pp==NULL) return;

    if (pp->ldat.id==TLIST_ANCHOR_NODE) pp = pp->next;
    while (pp!=NULL) { /*&& !ex_strcmp(HDLIST_END_KEY,      (char*)pp->ldat.key.buf) */
                    //&& !ex_strcmp(HDLIST_CONTENTS_KEY, (char*)pp->ldat.key.buf)) {
        if (content==ON || !ex_strcmp(HDLIST_CONTENTS_KEY, (char*)pp->ldat.key.buf)) {
            print_message("[%s] [%s]\n", (char*)pp->ldat.key.buf, (char*)pp->ldat.val.buf);
        }
        pp = pp->next;
    }
    return;
}


/**
HDLIST_END_KEY のひとつ前のリストへのリンクを返す．
HDLIST_END_KEY が無ければ HDLIST_CONTENTS_KEY のひとつ前，HDLIST_CONTENTS_KEY もなければ最後のリストへのリンクを返す．
*/

tList*  find_protocol_end(tList* lp)
{
    tList* end = search_key_tList(lp, HDLIST_END_KEY, 1);
    if (end==NULL) end = search_key_tList(lp, HDLIST_CONTENTS_KEY, 1);
    //
    if (end!=NULL) end = end->prev;
    else           end = find_tList_end(lp);

    return end;
}

