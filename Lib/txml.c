/**
@brief   Tiny XML ライブラリ 
@file    txml.c
@version 1.0
@author  Fumi.Iseki (C)
@date    2009 2/10

@par タイトル
サブセット版 整形式XML 簡易パーサ

@attention
全てのパターンのパース可能性は保障しない．@n
複雑な XMLはパースできない．@n
<?xml ... ?> 以外の <? ..... ?> と <! ..... > は処理しない（読み込みは行う）@n
エンティティは処理しない．@n

@see     tXML
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include "txml.h"
#include "jbxl_state.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////
// Parser
//

/**
tXML*  xml_parse(char* pp)

文字列のXMLデータを解釈して，tXMLのツリーを生成する．

@param  pp  文字列の XMLデータへのポインタ．
@return XMLデータを格納した tXMLのアンカーへのポインタ．
@return エラーの場合，next以下のノードにはエラーを起こす直前までの内容が保存される
@retval 負数  エラーを起こした場合 stateに JBXL_XML_PARSED以外の値（0以下）が入る．

@code
    tXML* xml = xml_parse("<a t=\"x\"><b>aaaa<d>bbbb</d>cccc<xxxi/>ddddd</b><c><e></e></c></a>");
@endcode
*/
tXML*  xml_parse(char* pp)
{
    tXML* xml;
    tXML* node;

    xml = new_xml_node();                    // アンカー
    xml->ldat.id = XML_ANCHOR_NODE;
    xml->state   = JBXL_STATE_ANCHOR;
    xml->depth   = -1;

    // パース
    node = xml_main_parse(xml, pp, TRUE);
    if (node->state<0) return xml;

    // 元に戻ったか？
    if (xml==node) {
        xml->state = JBXL_XML_PARSED;
    }
    else {
        xml->state = JBXL_XML_NOT_CLOSED;
    }

    // XML rootの数
    if (xml->next!=NULL) {
        int n = 0;
        node = xml->next;
        while(node!=NULL) {
            if (node->ldat.id==XML_NAME_NODE) n++;
            node = node->ysis;
        }
        if (n!=1) xml->state = JBXL_XML_MULTI_ROOT;
    }
    else xml->state = JBXL_XML_DEFAULT_STATE;

    return xml;
}


/**
tXML*  xml_parse_file(const char* fn)

ファイルから読み込んでパースする．

@param  fn  読み込むファイル名
@return XMLデータを格納した tXMLのアンカーへのポインタ 
*/
tXML*  xml_parse_file(const char* fn)
{
    tXML*  xml = NULL;
    Buffer buf;

    buf = read_Buffer_file(fn);
    if (buf.buf!=NULL) {
        xml = xml_parse((char*)(buf.buf));
        free_Buffer(&buf);
    }

    return xml;
}


/**
tXML*  xml_parse_seq(tXML* xml, char* pp)

部分的な XMLデータを解釈して，tXMLのツリー構造に格納する．@n
完全な XMLデータでなくても解釈できるところまでは解釈する．@n
xmlが NULLの場合は自動的にツリーを作る．@n
断片的に XMLデータを入力する場合に使用する．

@param  xml  XMLデータを格納するツリー構造体の先頭へのポインタ．ツリーの途中のノードでも可
@param  pp   XMLデータへのポインタ．

@return XMLツリーデータのアンカーへのポインタ
@return @b altp に最後に処理したノードへのポインタが格納される．
@return 解釈が不完全の場合は @b state に状態の値が入る．

@code
    tXML* xml;
    xml = xml_parse_seq(NULL, "<A aaa=\"YYY\"><B>");
    xml = xml_parse_seq(xml->altp, "<Z zzz=\"99 9\"/><M>MM</M></B><C><M><X>XX</X><YY yyy=\"888\">XX");
    xml = xml_parse_seq(xml->altp, "YY</YY>MM</M><C bbb=\"777\" xxx=\"000i\" xxxx=\"000\" >");
    xml = xml_parse_seq(xml->altp, "<X></X><M><X></X>YY<Y>oooo");
    xml = xml_parse_seq(xml->altp, "</Y></M></C></C></A>");
    xml = xml_parse_seq(xml->altp, "<M>");
    close_xml(xml);
@endcode
*/
tXML*  xml_parse_seq(tXML* xml, char* pp)
{
    int   skip = FALSE;
    tXML* node;

    if (xml==NULL) {
        xml = new_tTree_anchor_node();
        //xml->ldat.id = XML_ANCHOR_NODE;
        //xml->depth = -1;
        skip = TRUE;
    }
    else {
        tXML* top = find_xml_top(xml);
        if (top==xml) top->state = JBXL_XML_DEFAULT_STATE;
    }

    node = xml_main_parse(xml, pp, skip);
    xml = find_xml_top(xml);
    xml->altp = node;

    if (xml!=node) return xml;

    if (node->state==JBXL_NORMAL) xml->state = JBXL_XML_PARSED;
    else                          xml->state = node->state;

    if (xml->next!=NULL) {
        int n = 0;
        node = xml->next;
        while(node!=NULL) {
            if (node->ldat.id==XML_NAME_NODE) n++;
            node = node->ysis;
        }
        if (n!=1) xml->state = JBXL_XML_MULTI_ROOT;
    }
    else xml->state = JBXL_XML_DEFAULT_STATE;

    return xml;
}


/**
tXML*  xml_main_parse(tXML* xml, char* pp, int skip)

部分的な XMLデータを解釈して，tXMLのツリー構造に格納する．@n
完全な XMLデータでなくても解釈できるところまでは解釈する．@n
パーサの内部的なメイン関数．ただし，ユーザが直接この関数を使用することは多分無い．

@param  xml   XMLデータを格納するツリー構造体の先頭へのポインタ．
@param  pp    XMLデータへのポインタ．
@param  skip  最初のデータ（タイトルなど）をスキップするか？ TRUE or FLASE

@return 最後に処理したノードノードへのポインタ．ppがNULLの場合は xml, xmlが NULLの場合は NULLが返る．
@retval 負数 @b state エラーの場合は stateに 負数の値が入る．
*/
tXML*  xml_main_parse(tXML* xml, char* pp, int skip)
{
    int   n, node_end;
    char* node_name = NULL;
    char* node_attr = NULL;
    char* value     = NULL;
    
    if (pp ==NULL) return xml;
    if (xml==NULL) return NULL;

    if (skip) while (*pp!='\0' && *pp!='<') pp++;

    while (*pp!='\0') {
        n = 1;

        if (*pp=='<') {
            // End TAG        </   >
            if (*(pp+1)=='/') {
                if (xml->state==JBXL_XML_NODE_OPENED) {
                    n = xml_parse_end_node(pp, &node_name);
                    if (n>0) {
                        if (node_name!=NULL && xml->ldat.key.buf!=NULL) {
                            if (!strcmp((const char*)node_name, (const char*)(xml->ldat.key.buf))) {
                                if (xml->next!=NULL) {
                                    xml->state = JBXL_XML_NODE_CLOSED;
                                }
                                else {
                                    xml->state = JBXL_XML_NODE_EMPTY;
                                }
                                //
                                if (xml->prev!=NULL) xml = xml->prev;
                                else n = xml->state = JBXL_XML_SEQUENCE_ERROR;
                            }
                            else n = xml->state = JBXL_XML_PARSE_ERROR;
                        }
                        else n = xml->state = JBXL_XML_PARSE_ERROR;
                    }
                    else xml->state = n;                    // エラー
                }
                else n = xml->state = JBXL_XML_PARSE_ERROR;
            }

            // Comment TAG        <!--   -->
            else if (!strncmp(pp+1, "!--", 3)) {
                n = xml_parse_comment_node(pp, &value);
                if (n>0) {
                    xml = add_tTree_node_bystr(xml, XML_COMMENT_NODE, 0, (char*)XML_COMMENT_NODE_KEY, value, NULL, 0);
                    xml->state = JBXL_XML_NODE_EMPTY;
                    if (xml->prev!=NULL) xml = xml->prev;
                    else n = xml->state = JBXL_XML_SEQUENCE_ERROR;
                }
            }

            // Data TAG            <!     >
            else if (*(pp+1)=='!') {
                n = xml_parse_data_node(pp, &value);
                if (n>0) {
                    xml = add_tTree_node_bystr(xml, XML_DATA_NODE, 0, (char*)XML_DATA_NODE_KEY, value, NULL, 0);
                    xml->state = JBXL_XML_NODE_EMPTY;
                    if (xml->prev!=NULL) xml = xml->prev;
                    else n = xml->state = JBXL_XML_SEQUENCE_ERROR;
                }
            }

            // Processing TAG      <?   ?>
            else if (*(pp+1)=='?') {
                n = xml_parse_processing_node(pp, &node_name, &node_attr);
                if (n>0) {
                    if (!strncasecmp("xml", node_name, 3)) {
                        tList* lp = xml_parse_attr(node_attr);
                        xml = add_tTree_node_bystr(xml, XML_DOC_NODE, 0, node_name, NULL, NULL, 0);
                        xml->ldat.lst = lp;
                    }
                    else {
                        xml = add_tTree_node_bystr(xml, XML_PROCESS_NODE, 0, node_name, node_attr, NULL, 0);
                    }
                    xml->state = JBXL_XML_NODE_EMPTY;
                    if (xml->prev!=NULL) xml = xml->prev;
                    else n = xml->state = JBXL_XML_SEQUENCE_ERROR;
                }
            }

            // Start TAG  <   > 
            else {
                n = xml_parse_start_node(pp, &node_name, &node_attr, &node_end);

                if (n>0) {
                    tList* lp = xml_parse_attr(node_attr);
                    xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 0, node_name, NULL, NULL, 0);
                    xml->ldat.lst = lp;

                    if (node_end) {
                        xml->state = JBXL_XML_NODE_EMPTY;
                        if (xml->prev!=NULL) xml = xml->prev;
                        else n = xml->state = JBXL_XML_SEQUENCE_ERROR;
                    }
                    else {
                        int m;
                        xml->state = JBXL_XML_NODE_OPENED;
                        // ノード値
                        m = xml_parse_content(pp+n, &value);            // 0 が返る可能性がある
                        if (m>=0) {
                            n += m;
                            if (value!=NULL) {
                                xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, value, NULL, NULL, 0);
                                xml->state = JBXL_XML_NODE_CLOSED;
                                if (xml->prev!=NULL) {
                                    xml = xml->prev;
                                    xml->ldat.lv++;
                                }
                                else n = xml->state = JBXL_XML_SEQUENCE_ERROR;
                            }
                        }
                        else n = xml->state = m;                    // エラー
                    }
                }
            }

            freeNull(node_name);
            freeNull(node_attr);
            freeNull(value);
        }

        // Content        (断片的に入力したXMLが content の途中で終わっている場合など)
        else {
            if (xml->state==JBXL_XML_NODE_OPENED) {
                n = xml_parse_content(pp, &value);
                if (n>0) {
                    if (value!=NULL) {
                        xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, value, NULL, NULL, 0);
                        xml->state = JBXL_XML_NODE_CLOSED;
                        if (xml->prev!=NULL) {
                            xml = xml->prev;
                            xml->ldat.lv++;
                        }
                        else n = xml->state = JBXL_XML_SEQUENCE_ERROR;
                    }
                }
                else n = xml->state = JBXL_XML_PARSE_ERROR;
                freeNull(value);
            }
            else n = xml->state = JBXL_XML_PARSE_ERROR;
        }

        // エラー
        if (n<=0) return xml;

        pp += n;
        while (*pp==' ' || *pp==CHAR_TAB || *pp==CHAR_LF || *pp==CHAR_CR) pp++;
    }

    return xml;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
// Paese of TAG
//

/**
int  xml_parse_start_node(char* pp, char** node_name, char** node_attr, int* node_end)

XMLの開始ノードを処理する．処理できるノードの型は
@code
<node_name attr1="value1" attr2="value2">
<node_name attr1="value1" attr2="value2" />
@endcode

@attention
注）この関数では，属性部分(*node_attr)の解釈は行われないので，それは xml_parse_attr() で行う必要がある．

@param      pp            ノードへのポインタ．'<' を指すポインタ．
@param[out] *node_name    ノードの名前を格納するポインタ．要 free
@param[out] *node_attr    ノードの属性部分を格納するポインタ．処理（解釈）は行われない．要 free
@param[out] *node_end     ノードが単独で閉じている場合(Empty Tag)は TRUE, そうでない場合は FALSEが格納される．要 free

@retval  1以上            ノード全体の長さ．0 が返ることは無い．
@retval  JBXL_XML_PARSE_ERROR  パースエラー
@retval  JBXL_MALLOC_ERROR     メモリエラー
@retval  JBXL_ARGS_ERROR       引数エラー
*/
int  xml_parse_start_node(char* pp, char** node_name, char** node_attr, int* node_end)
{
    int   nn, mm;
    char* pt;

    if (node_name==NULL || node_attr==NULL || node_end==NULL) return JBXL_ARGS_ERROR;
    *node_end  = FALSE;
    *node_name = NULL;
    *node_attr = NULL;

    if (*pp!='<') return JBXL_XML_PARSE_ERROR;
    pt = pp = pp + 1;
    pt = skip_chars(pt, " />");
    if (pt==NULL) return JBXL_XML_PARSE_ERROR;

    nn = mm = (int)(pt - pp);
    if (mm==0) return JBXL_XML_PARSE_ERROR;

    nn = nn + 1;
    *node_name = (char*)malloc((size_t)(mm + 1));
    if (*node_name==NULL) return JBXL_MALLOC_ERROR;
    memcpy(*node_name, pp, mm);
    (*node_name)[mm] = '\0';
    //PRINT_MESG("START --> %s  %d\n", *node_name, (int)strlen(*node_name));

    if (isnot_xml_name((unsigned char*)*node_name)) {
        freeNull(*node_name);
        return JBXL_XML_PARSE_ERROR;
    }

    // Empty TAG
    if (*pt=='/') {
        if (*(pt+1)!='>') {
            freeNull(*node_name);
            return JBXL_XML_PARSE_ERROR;
        }
        nn += 1;
        *node_end = TRUE;
        //PRINT_MESG("END   --> %s  %d\n", *node_name, (int)strlen(*node_name));
    }

    // Attribute
    else if (*pt==' ') {
        pp = pt = pt + 1;
        while (*pt==' ' || *pt==CHAR_TAB || *pt==CHAR_LF || *pt==CHAR_CR) pt++;
        nn += (int)(pt - pp) + 1;

        pp = pt;
        pt = skip_chars(pt, "/>");
        if (pt==NULL) {
            freeNull(*node_name);
            return JBXL_XML_PARSE_ERROR;
        }

        mm = (int)(pt - pp);
        if (mm>0) {
            *node_attr = (char*)malloc((size_t)(mm + 1));
            if (*node_attr==NULL) {
                freeNull(*node_name);
                return JBXL_MALLOC_ERROR;
            }
            memcpy(*node_attr, pp, mm);
            (*node_attr)[mm] = '\0';
            nn += mm;
            //PRINT_MESG("ATTR  --> %s  %d\n", *node_attr, (int)strlen(*node_attr));
        }

        if (*pt=='/') {
            if (*(pt+1)!='>') {
                freeNull(*node_name);
                freeNull(*node_attr);
                return JBXL_XML_PARSE_ERROR;
            }
            nn += 1;
            *node_end = TRUE;
            //PRINT_MESG("END   --> %s  %d\n", *node_name, (int)strlen(*node_name));
        }
    }
    
    //else {}   *pt=='>'

    return nn + 1;
}


/**
int  xml_parse_end_node(char* pp, char** node_name)

XMLの終了ノードを処理する．処理できるノードの型は @</node_name>
    
@param      pp           ノードへのポインタ．'<' を指すポインタ．
@param[out] *node_name   ノードの名前を格納するポインタ．要 free

@retval 1以上            ノード全体の長さ．0 が返ることは無い．
@retval JBXL_XML_PARSE_ERROR  パースエラー
@retval JBXL_MALLOC_ERROR     メモリエラー
*/
int  xml_parse_end_node(char* pp, char** node_name)
{
    int   nn, mm;
    char* pt;
    char* work;

    if (pp==NULL) return JBXL_XML_PARSE_ERROR;
    *node_name = NULL;

    if (strncmp(pp ,"</", 2)) return JBXL_XML_PARSE_ERROR;

    pt = pp = pp + 2;
    pt = skip_chars(pt, ">");
    if (pt==NULL) return JBXL_XML_PARSE_ERROR;

    nn = mm = (int)(pt - pp);
    if (mm==0) return JBXL_XML_PARSE_ERROR;

    nn += 2;
    work = (char*)malloc((size_t)(mm+1));
    if (work==NULL) return JBXL_MALLOC_ERROR;
    memcpy(work, pp, mm);
    work[mm] = '\0';
    *node_name = pack_head_tail_char(work, ' ');
    free(work);
    //PRINT_MESG("END   --> %s  %d\n", *node_name, (int)strlen(*node_name));

    return nn + 1;
}


/**
int  xml_parse_content(char* pp, char** content)

XMLのコンテントを処理する． @<node_name attr="node_attr">content@</node_name>@n
値の前後の空白，TAB, LF, CRは削除される．
    
@param      pp           コンテントの先頭へのポインタ．Start Tagの '>' の次を指すポインタ．
@param[in]  *content     コンテントを格納するポインタ．コンテントがない場合は NULLでも可．
@param[out] *content     コンテントが格納される．前後の 空白, TAB, LF, CRは削除されている．要 free

@retval 0以上            ノード値部の全体の長さ．0 が返る可能性もある（@<x>@</x>の場合）
@retval JBXL_XML_PARSE_ERROR  パースエラー
@retval JBXL_MALLOC_ERROR     メモリエラー
*/
int  xml_parse_content(char* pp, char** content)
{
    int   nn=0, mm;
    char* pt;
    char* work;

    if (pp==NULL || content==NULL) return JBXL_XML_PARSE_ERROR;
    *content = NULL;

    while (*pp==' ' || *pp==CHAR_TAB || *pp==CHAR_LF || *pp==CHAR_CR) {
        pp++;
        nn++;
    }
    if (*pp=='<') return nn;            // <node><

    pt = pp;
    while (*pt!='<' && *pt!='\0') pt++;

    mm = (int)(pt - pp);
    if (mm==0) return nn;
    nn += mm;

    work = (char*)malloc((size_t)(mm+1));
    if (work==NULL) return JBXL_MALLOC_ERROR;
    memcpy(work, pp, mm);
    work[mm] = '\0';

    *content = pack_head_tail_char(work, ' ');
    free(work);
    //PRINT_MESG("VALUE --> %s  %d\n", *content, (int)strlen(*content));

    return nn;
}


/**
tList*  xml_parse_attr(char* pp)

XMLのノード属性を解釈して，リスト(tList)にする． 解釈する形式は  AAA="GGG" xxxx="1234"

@param  pp  テキストで保存されている属性へのポインタ．

@return 生成したリストの先頭へのポインタ．
@retval JBXL_XML_PARSE_ERR  @b state パースエラーの場合は state に JBXL_XML_PARSE_ERROR が設定される．
@retval JBXL_XML_NODE_CLOSED  @b state 正常に終了した場合は JBXL_XML_NODE_CLOSED が設定される．
*/
tList*  xml_parse_attr(char* pp)
{
    int    sz;
    char*  nm;
    char*  vl;
    char*  pt;
    char   qt;
    tList* lp = NULL;
    tList* lt = NULL;

    // 作業メモリの確保
    if (pp==NULL) return NULL;
    sz = (int)strlen((const char*)pp) + 1;
    nm = (char*)malloc(sz);
    if (nm==NULL) return NULL;
    vl = (char*)malloc(sz);
    if (vl==NULL) {
        free(nm);
        return NULL;
    }

    // Parse for AAA="BBB" CCC="DDDD"
    while (*pp==' ' && *pp!='\0') pp++;
    while (*pp!='\0') {
        pt = pp;
        while (*pt!='=' && *pt!='\0') pt++;
        if (*pt=='\0') {
            if (lt!=NULL) lt->state = JBXL_XML_PARSE_ERROR;
            break;
        }

        sz = (int)(pt - pp);
        memcpy(nm, pp, (size_t)sz);
        nm[sz] = '\0';

        pt++;
        if (*pt!='"' && *pt!='\'') {
            if (lt!=NULL) lt->state = JBXL_XML_PARSE_ERROR;
            break;
        }
        else qt = *pt;

        pt++;
        pp = pt;
        while(*pt!=qt && *pt!='\0') pt++;
        if (*pt=='\0') {
            if (lt!=NULL) lt->state = JBXL_XML_PARSE_ERROR;
            break;
        }

        sz = (int)(pt - pp);
        memcpy(vl+1, pp, (size_t)sz);
        vl[0]     = qt;
        vl[sz+1] = qt;
        vl[sz+2] = '\0';

        lp = add_tList_node_str(lp, nm, vl);
        if (lt==NULL) lt = lp;
        pp = pt + 1;

        if (*pp!=' ' && *pp!='\0') {
            lt->state = JBXL_XML_PARSE_ERROR;
            break;
        }
        while (*pp==' ' && *pp!='\0') pp++;
    }

    //
    free(nm);
    free(vl);

    if (lt!=NULL) {
        if (lt->state!=JBXL_XML_PARSE_ERROR) lt->state = JBXL_XML_NODE_CLOSED;
    }
    return lt;
}


/**
int  xml_parse_processing_node(char* pp, char** node_name, char** node_attr)

XMLのプロセッシングノードを処理する．処理できるノードの型は <?processing?>@n
プロセッシング部分は解釈せずに，そのまま node_name, node_attrに格納される．

@param      pp           ノードへのポインタ．'<' を指すポインタ．
@param[out] *node_name    プロセッシングの名前部分が格納される．要free
@param[out] *node_attr    プロセッシングの属性部分が格納される．要free

@retval 1以上            ノード全体の長さ．0 が返ることは無い．
@retval JBXL_XML_PARSE_ERROR  パースエラー
@retval JBXL_MALLOC_ERROR     メモリエラー
*/
int  xml_parse_processing_node(char* pp, char** node_name, char** node_attr)
{
    int   nn, mm;
    char* pt;

    *node_name = NULL;
    *node_attr = NULL;

    if (pp==NULL) return JBXL_XML_PARSE_ERROR;
    if (strncmp(pp, "<?", 2)) return JBXL_XML_PARSE_ERROR;

    pt = pp = pp + 2;
    pt = skip_chars(pt, " ?");
    if (pt==NULL) return JBXL_XML_PARSE_ERROR;

    nn = mm = (int)(pt - pp);
    if (mm==0) return JBXL_XML_PARSE_ERROR;

    nn += 2;
    *node_name = (char*)malloc((size_t)(mm + 1));
    if (*node_name==NULL) return JBXL_MALLOC_ERROR;
    memcpy(*node_name, pp, mm);
    (*node_name)[mm] = '\0';

    if (isnot_xml_name((unsigned char*)*node_name)) return JBXL_XML_PARSE_ERROR;

    // for attribute
    if (*pt==' ') {
        pp = pt = pt + 1;
        while (*pt==' ' || *pt==CHAR_TAB || *pt==CHAR_LF || *pt==CHAR_CR) pt++;
        nn += (int)(pt - pp) + 1;

        pp = pt;
        pt = skip_chars(pt, "?");
        if (pt==NULL) {
            freeNull(*node_name);
            return JBXL_XML_PARSE_ERROR;
        }

        mm = (int)(pt - pp);
        if (mm>0) {
            *node_attr = (char*)malloc((size_t)(mm + 1));
            if (*node_attr==NULL) {
                freeNull(*node_name);
                return JBXL_MALLOC_ERROR;
            }
            memcpy(*node_attr, pp, mm);
            (*node_attr)[mm] = '\0';
            nn += mm;
            //PRINT_MESG("ATTR  --> %s  %d\n", *node_attr, (int)strlen(*node_attr));
        }
    }
    
    if (strncmp(pt, "?>", 2)) {
        freeNull(*node_name);
        freeNull(*node_attr);
        return JBXL_XML_PARSE_ERROR;
    }

    return nn + 2;
}


/**
int  xml_parse_comment_node(char* pp, char** comment)

XMLのコメントノードを処理する．処理できるノードの型は '<!--comment-->' @n
xml_parse_data_node() に先立って適用させる必要がある．

@param      pp           ノードへのポインタ．'<' を指すポインタ．
@param[out] *comment     コメント部分がそのまま格納される．要 free

@retval 1以上            ノード全体の長さ．0 が返ることは無い．
@retval JBXL_XML_PARSE_ERROR  パースエラー
@retval JBXL_MALLOC_ERROR     メモリエラー
*/
int  xml_parse_comment_node(char* pp, char** comment)
{
    int   nn, mm;
    char* pt;

    *comment = NULL;
    if (pp==NULL) return JBXL_XML_PARSE_ERROR;
    if (strncmp(pp, "<!--", 4)) return JBXL_XML_PARSE_ERROR;

    pt = pp = pp + 4;
    pt = skip_chars(pt, ">");
    if (pt==NULL) return JBXL_XML_PARSE_ERROR;

    pt -= 2;
    if (strncmp(pt, "-->", 3)) return JBXL_XML_PARSE_ERROR;

    nn = mm = (int)(pt - pp);
    if (mm==0) return JBXL_XML_PARSE_ERROR;

    nn += 4;
    *comment = (char*)malloc((size_t)(mm+1));
    if (*comment==NULL) return JBXL_MALLOC_ERROR;
    memcpy(*comment, pp, mm);
    (*comment)[mm] = '\0';
    //PRINT_MESG("COMMENT-> %s  %d\n", *comment, (int)strlen(*comment));

    return nn + 3;
}


/**
int  xml_parse_data_node(char* pp, char** data)

XMLのデータノード（その他エレメントノードなど）を処理する．処理できるノードの型は <!data>@n
データ部分は解釈せずに，そのまま dataに格納される．

@param      pp           ノードへのポインタ．'<' を指すポインタ．
@param[out] *data        データ部分がそのまま格納される．要 free

@retval 1以上            ノード全体の長さ．0 が返ることは無い．
@retval JBXL_XML_PARSE_ERROR  パースエラー
@retval JBXL_MALLOC_ERROR メモリエラー
*/
int  xml_parse_data_node(char* pp, char** data)
{
    int   nn, mm;
    char* pt;

    *data = NULL;
    if (pp==NULL) return JBXL_XML_PARSE_ERROR;
    if (strncmp(pp, "<!", 2)) return JBXL_XML_PARSE_ERROR;

    pt = pp = pp + 2;
    pt = skip_chars(pt, ">");
    if (pt==NULL) return JBXL_XML_PARSE_ERROR;

    nn = mm = (int)(pt - pp);
    if (mm==0) return JBXL_XML_PARSE_ERROR;

    nn += 2;
    *data = (char*)malloc((size_t)(mm+1));
    if (*data==NULL) return JBXL_MALLOC_ERROR;
    memcpy(*data, pp, mm);
    (*data)[mm] = '\0';
    //PRINT_MESG("DATA ---> %s  %d\n", *data, (int)strlen(*data));

    return nn + 1;
}


/**
int  isnot_xml_name(unsigned char* pp)

XML名として不適切かどうかを大まかに判断する．@n
かなりいい加減なので，FALSEだからといって，厳密にXML名として適切であるとは限らない．

@param  pp     検査する XML名へのポインタ
@retval TRUE   XML名として不適切
@retval FALSE  XML名として適切かもしれない
*/
int  isnot_xml_name(unsigned char* pp)
{
    if (pp==NULL)  return TRUE;
    if (*pp=='\0') return TRUE;

    while (*pp!='\0') {
        if (*pp<=0x2c) return TRUE;                 //  !"#$%&'()*+,
        if (*pp==0x2f) return TRUE;                 // /
        if (*pp>=0x3b && *pp<=0x40) return TRUE;    // ;<=>?@
        if (*pp>=0x5b && *pp<=0x5e) return TRUE;    // [\]^
        if (*pp==0x60) return TRUE;                 // `
        if (*pp>=0x7b && *pp<=0x7f) return TRUE;    // {|}~DEL
        pp++;
    }
    
    return FALSE;
}


/**
void   close_xml(tXML* pp)
    
パースエラーを起こしたツリーを強制的に正常化する．

@param pp 正常化するツリーへのポインタ．
*/
void   close_xml(tXML* pp)
{
    if (pp!=NULL) {

        // アンカー
         if (pp->ldat.id==XML_ANCHOR_NODE) {
            pp->state = JBXL_XML_PARSED;
            pp->depth = -1;
            if (pp->next!=NULL) {
                pp->next->depth = 0;
                adjust_tTree_depth(pp->next);
                close_xml(pp->next);
            }
            return;
        }

        while(pp->esis!=NULL) pp = pp->esis;
        do {
            if (pp->next==NULL) pp->state = JBXL_XML_NODE_EMPTY;
            //
            if (pp->state!=JBXL_XML_NODE_EMPTY) pp->state = JBXL_XML_NODE_CLOSED;
            //if (pp->ldat.lst!=NULL) {
            //    if ((pp->ldat.lst)->state!=JBXL_XML_NODE_EMPTY) (pp->ldat.lst)->state = JBXL_XML_NODE_CLOSED;
            //}
            //
            if (pp->next!=NULL) close_xml(pp->next);
       
            pp = pp->ysis;
        } while(pp!=NULL);
    }  
       
    return;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// Inverse Parse
//

/**
Buffer  xml_inverse_parse(tXML* pp, int mode)

ppに格納された XMLデータを元の書式に戻して Bufferに格納する．xml_parse() の逆．@n
@b XML_CRLF_FORMAT, @b XML_INDENT_FORMAT でノード値がある場合は，値を囲むノードは改行しない．

@param  pp    XMLデータの格納されたツリーへのポインタ
@param  mode  元のXMLへ戻す時の書式
@param  mode  @b XML_ONELINE_FORMAT 改行なしの一行にする．
@param  mode  @b XML_CRLF_FORMAT    ノードの終わりを CR(0x0d), LF(0x0a)で改行する．
@param  mode  @b XML_INDENT_FORMAT  先頭にインデント(TAB)をつけ，ノードごとに改行 CR LF (0x0d,0x0a)する．

@return 変換したXMLデータを格納した Buffer変数．
*/
Buffer  xml_inverse_parse(tXML* pp, int mode)
{
    int cnt;
    Buffer buf;

    buf = init_Buffer();
    if (pp==NULL) return buf;
    if (pp->ldat.id==XML_ANCHOR_NODE) pp = pp->next;
    if (pp==NULL) return buf;

    cnt = count_tTree(pp);
    buf = make_Buffer(cnt*LMDATA);
    if (buf.buf==NULL) return buf;

    while (pp->esis!=NULL) pp = pp->esis;
    xml_to_Buffer(pp, &buf, mode, pp->depth);

    return buf;
}


/**
void  xml_to_Buffer(tXML* pp, Buffer* buf, int mode, int indent)

xml_inverse_parse()用の補助関数．
ppに格納された XMLデータを元の書式に戻して Bufferに格納する．

@param  pp      XMLデータの格納されたツリーへのポインタ
@param  buf     変換したXMLデータを格納する Buffer変数．データ格納領域は予め確保しておく.
@param  mode    元のXMLへ戻す時の書式
@param  mode    @b XML_ONELINE_FORMAT 改行なしの一行にする．
@param  mode    @b XML_CRLF_FORMAT    ノードの終わりを CR(0x0d), LF(0x0a)で改行する．
@param  mode    @b XML_INDENT_FORMAT  先頭にインデント(TAB)をつけ，ノードごとに改行する．
@param  indent  インデントを付け始める深さ．modeが @b XML_INDENT_MODE のときのみ有効．
*/
void  xml_to_Buffer(tXML* pp, Buffer* buf, int mode, int indent)
{
    do {
        xml_open_node_Buffer(pp, buf, mode, indent);
        if (pp->next!=NULL) {
            xml_to_Buffer(pp->next, buf, mode, indent);
            xml_close_node_Buffer(pp, buf, mode, indent);
        }
        
        pp = pp->ysis;
    } while(pp!=NULL);

    return;
}


/**
void  xml_open_node_Buffer(tXML* pp, Buffer* buf, int mode, int indent)

ツリー中のXMLのオープンノードのデータを元の書式に戻して Bufferに格納する．

@param  pp      XMLデータの格納されたツリーのノードへのポインタ
@param  buf     変換したXMLデータを格納した Buffer変数．
@param  mode    元のXMLへ戻す時の書式
@param  mode    @b XML_ONELINE_FORMAT 改行なしの一行にする．
@param  mode    @b XML_CRLF_FORMAT    ノードの終わりを CR(0x0d), LF(0x0a)で改行する．
@param  mode    @b XML_INDENT_FORMAT  先頭にインデント(TAB)をつけ，ノードごとに改行する．
@param  indent  インデントを付け始める深さ．modeが XML_INDENT_MODE のときのみ有効．
*/
void  xml_open_node_Buffer(tXML* pp, Buffer* buf, int mode, int indent)
{
    int i;

    // Name TAG
    if (pp->ldat.id==XML_NAME_NODE) {
        if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) {
            if (buf->vldsz>0 && buf->buf[buf->vldsz-1]!='\n') cat_s2Buffer("\r\n", buf);
        }
        //
        if (mode==XML_INDENT_FORMAT) {
            char* tabs = (char*)malloc(pp->depth-indent+1);
            if (tabs!=NULL) {
                for (i=indent; i<pp->depth; i++) tabs[i-indent] = '\t';
                tabs[pp->depth-indent] = '\0';
                cat_s2Buffer(tabs, buf);
                free(tabs);
            }
        }

        cat_s2Buffer("<", buf);
        cat_Buffer(&(pp->ldat.key), buf);
        if (pp->ldat.lst!=NULL) {
            xml_attr_to_Buffer(pp->ldat.lst, buf);
        }
        if (pp->next==NULL) {
            if (pp->state==JBXL_XML_NODE_EMPTY) {
                cat_s2Buffer(" />", buf);
            }
            else {
                cat_s2Buffer("></", buf);
                cat_Buffer(&(pp->ldat.key), buf);
                cat_s2Buffer(">", buf);
            }
            if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) {
                cat_s2Buffer("\r\n", buf);
            }
        }
        else {
            cat_s2Buffer(">", buf);
        }
    }
    
    // Content
    else if (pp->ldat.id==XML_CONTENT_NODE) {
        if (buf->buf[buf->vldsz-1]=='\n') {
            buf->buf[buf->vldsz-2] = '\0';
            buf->vldsz -= 2;
        }
        //
        if (pp->esis!=NULL || pp->ysis!=NULL) {
            cat_s2Buffer("\r\n", buf); 
            char* tabs = (char*)malloc(pp->depth-indent+1);
            if (tabs!=NULL) {
                for (i=indent; i<pp->depth; i++) tabs[i-indent] = '\t';
                tabs[pp->depth-indent] = '\0';
                cat_s2Buffer(tabs, buf);
                free(tabs);
            }
        }
        //
        cat_Buffer(&(pp->ldat.key), buf);
        //
        if (pp->esis!=NULL) cat_s2Buffer("\r\n", buf); 
    }

    // xml TAG
    else if (pp->ldat.id==XML_DOC_NODE) {
        if (mode==XML_INDENT_FORMAT) {
            char* tabs = (char*)malloc(pp->depth-indent+1);
            if (tabs!=NULL) {
                for (i=indent; i<pp->depth; i++) tabs[i-indent] = '\t';
                tabs[pp->depth-indent] = '\0';
                cat_s2Buffer(tabs, buf);
                free(tabs);
            }
        }

        cat_s2Buffer("<?", buf);
        if (pp->ldat.key.buf!=NULL) {
            cat_Buffer(&(pp->ldat.key), buf);
        }
        if (pp->ldat.lst!=NULL) {
            xml_attr_to_Buffer(pp->ldat.lst, buf);
        }
        cat_s2Buffer("?>", buf);

        if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) cat_s2Buffer("\r\n", buf);
    }

    // Comment TAG
    else if (pp->ldat.id==XML_COMMENT_NODE) {
        if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) {
            if (buf->buf[buf->vldsz-1]!='\n') cat_s2Buffer("\r\n", buf);
        }

        if (mode==XML_INDENT_FORMAT) {
            char* tabs = (char*)malloc(pp->depth-indent+1);
            if (tabs!=NULL) {
                for (i=indent; i<pp->depth; i++) tabs[i-indent] = '\t';
                tabs[pp->depth-indent] = '\0';
                cat_s2Buffer(tabs, buf);
                free(tabs);
            }
        }

        cat_s2Buffer("<!--", buf);
        if (pp->ldat.val.buf!=NULL) {
            cat_Buffer(&(pp->ldat.val), buf);
        }
        cat_s2Buffer("-->", buf);

        if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) cat_s2Buffer("\r\n", buf);
    }

    // Data TAG
    else if (pp->ldat.id==XML_DATA_NODE) {
        if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) {
            if (buf->buf[buf->vldsz-1]!='\n') cat_s2Buffer("\r\n", buf);
        }
        //
        if (mode==XML_INDENT_FORMAT) {
            char* tabs = (char*)malloc(pp->depth-indent+1);
            if (tabs!=NULL) {
                for (i=indent; i<pp->depth; i++) tabs[i-indent] = '\t';
                tabs[pp->depth-indent] = '\0';
                cat_s2Buffer(tabs, buf);
                free(tabs);
            }
        }

        cat_s2Buffer("<!", buf);
        if (pp->ldat.val.buf!=NULL) {
            cat_Buffer(&(pp->ldat.val), buf);
        }
        cat_s2Buffer(">", buf);

        if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) cat_s2Buffer("\r\n", buf);
    }

    // Processing TAG
    else if (pp->ldat.id==XML_PROCESS_NODE) {
        if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) {
            if (buf->buf[buf->vldsz-1]!='\n') cat_s2Buffer("\r\n", buf);
        }
        //
        if (mode==XML_INDENT_FORMAT) {
            char* tabs = (char*)malloc(pp->depth-indent+1);
            if (tabs!=NULL) {
                for (i=indent; i<pp->depth; i++) tabs[i-indent] = '\t';
                tabs[pp->depth-indent] = '\0';
                cat_s2Buffer(tabs, buf);
                free(tabs);
            }
        }

        cat_s2Buffer("<?", buf);
        if (pp->ldat.key.buf!=NULL) {
            cat_Buffer(&(pp->ldat.key), buf);
        }
        if (pp->ldat.lst!=NULL) {
            xml_attr_to_Buffer(pp->ldat.lst, buf);
        }
        cat_s2Buffer(" ?>", buf);

        if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) cat_s2Buffer("\r\n", buf);
    }

    return;
}


/**
void  xml_close_node_Buffer(tXML* pp, Buffer* buf, int mode, int indent)

ツリー中のXMLのクローズノードのデータを元の書式に戻して Bufferに格納する．

@param  pp      XMLデータの格納されたツリーのノードへのポインタ
@param  buf     変換したXMLデータを格納した Buffer変数．
@param  mode    元のXMLへ戻す時の書式
@param  mode    @b XML_ONELINE_FORMAT 改行なしの一行にする．
@param  mode    @b XML_CRLF_FORMAT    ノードの終わりを CR(0x0d), LF(0x0a)で改行する．
@param  mode    @b XML_INDENT_FORMAT  先頭にインデント(TAB)をつけ，ノードごとに改行する．
@param  indent  インデントを付け始める深さ．modeが XML_INDENT_MODE のときのみ有効．
*/
void  xml_close_node_Buffer(tXML* pp, Buffer* buf, int mode, int indent)
{
    int i;

    if (pp->ldat.id==XML_NAME_NODE) {
        if (pp->next!=NULL) {
            if (mode==XML_INDENT_FORMAT) {
                if (pp->next->ldat.id!=XML_CONTENT_NODE || pp->next->ysis!=NULL) {
                    char* tabs = (char*)malloc(pp->depth-indent+1);
                    if (tabs!=NULL) {
                        for (i=indent; i<pp->depth; i++) tabs[i-indent] = '\t';
                        tabs[pp->depth-indent] = '\0';
                        cat_s2Buffer(tabs, buf);
                        free(tabs);
                    }
                }
            }

            cat_s2Buffer("</", buf);
            cat_Buffer(&(pp->ldat.key), buf);
            cat_s2Buffer(">", buf);

            if (mode==XML_INDENT_FORMAT || mode==XML_CRLF_FORMAT) cat_s2Buffer("\r\n", buf);
        }
    }

    return;
}


/**
void  xml_attr_to_Buffer(tList* pp, Buffer* buf)

リストに保存されたノードの属性をテキストへ戻す．

@param  pp   属性データが格納されたリストへのポインタ．
@param  buf  変換した属性データを格納する Buffer変数．
*/
void  xml_attr_to_Buffer(tList* pp, Buffer* buf)
{
    while (pp!=NULL) {
        cat_s2Buffer(" ", buf);
        cat_Buffer(&(pp->ldat.key), buf);

        if (pp->ldat.val.buf!=NULL) {
            cat_s2Buffer("=", buf);
            cat_Buffer(&(pp->ldat.val), buf);
        }
        pp = pp->next;
    }
}



///////////////////////////////////////////////////////////////////////////////////////
// Operation
//

/**
tXML*  init_xml_doc()

XML のドキュメントヘッダを作成する
*/
tXML*  init_xml_doc()
{
    tXML* xml = xml_parse((char*)"<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    return xml;
}


/**
tXML*  add_xml_node(tXML* xml, const char* name)

XMLツリーのxml の直下（姉妹の一番下：末っ子）にノード（属性は指定しない）を挿入する．

@param  xml 挿入するポイント
@param  name 挿入するノードの名前
@return 挿入したノードへのポインタ
*/
tXML*  add_xml_node(tXML* xml, const char* name)
{
    tXML* pp;

    if (name==NULL) return NULL;

    pp = add_tTree_node_bystr(xml, XML_NAME_NODE, 0, name, NULL, NULL, 0);
    pp->state = JBXL_XML_NODE_EMPTY;
    if (pp->prev!=NULL) pp->prev->ldat.lv++;

    return pp;
}


/**
tXML*  insert_xml_node(tXML* xml, const char* name)

XMLツリーのxml の直下（姉妹の一番上：長子）にノード（属性は指定しない）を挿入する．

@param  xml 挿入するポイント
@param  name 挿入するノードの名前
@return 挿入したノードへのポインタ
*/
tXML*  insert_xml_node(tXML* xml, const char* name)
{
    tList* pp;

    if (xml==NULL || name==NULL) return NULL;

    pp = insert_tTree_node_bystr(xml, XML_NAME_NODE, 0, name, NULL, NULL, 0);
    pp->state = JBXL_XML_NODE_EMPTY;
    if (pp->prev!=NULL) pp->prev->ldat.lv++;

    return pp;
}


/**
tXML*  add_xml_content(tXML* xml, const char* content)

XMLツリーのxml の直下にコンテントを挿入する．@n
コンテントが複数ある場合は末っ子のノードとして挿入する．@n
xmlがノード名ノードでなければエラー（NULLが返る）
*/
tXML*  add_xml_content(tXML* xml, const char* content)
{
    tList* pp;

    if (xml==NULL || content==NULL) return NULL;
    if (xml->ldat.id!=XML_NAME_NODE) return NULL;

    pp = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, content, NULL, NULL, 0);
    pp->state = JBXL_XML_NODE_CLOSED;
    if (pp->prev!=NULL) pp->prev->ldat.lv++;
  
    return pp;
}


/**
tXML*  append_xml_content(tXML* xml, const char* content)

XMLツリーのxml の直下のコンテントノードにコンテンツを追加する．@n
コンテントノードが無ければ，末っ子のノードにとしてコンテントノードを追加する．@n
*/
tXML*  append_xml_content(tXML* xml, const char* content)
{
    tList* pp;

    if (xml==NULL || content==NULL) return NULL;
    if (xml->ldat.id!=XML_NAME_NODE) return NULL;
    
    pp = xml->next;
    while (pp!=NULL && pp->ldat.id!=XML_CONTENT_NODE) pp = pp->ysis;

    if (pp==NULL) {
        pp = add_xml_content(xml, content);
    }
    else {
        if (pp->ldat.key.vldsz!=0) cat_s2Buffer(" ", &(pp->ldat.key));
        cat_s2Buffer(content, &(pp->ldat.key));
    }

    return pp;
}


/**
int  add_xml_content_area(tXML* xml, int len)

xml に空のコンテントノードを追加する．

@param xml コンテントノードを追加するノード．
@param len コンテンツノードの大きさ（バイト）
@return  TRUE: 成功，FALSE: 失敗
*/
int  add_xml_content_area(tXML* xml, int len)
{
    if (xml==NULL || len<=0) return FALSE;
    
    char* area = (char*)malloc(len);
    if (area==NULL) return FALSE;
    memset(area, (int)' ', len-1);
    area[len-1] = '\0';

    tXML* pp = add_xml_content(xml, area);
    if (pp!=NULL) {
        pp->ldat.key.buf[0] = '\0';
        pp->ldat.key.vldsz  = 0;
    }
    free(area);

    return TRUE;
}


/**
tXML*  add_xml_attr(tXML* xml, const char* attr)

xml に属性ノード（attr）を追加する．
*/
tXML*  add_xml_attr(tXML* xml, const char* attr)
{
    if (xml==NULL || attr==NULL) return NULL;
    if (xml->ldat.id!=XML_NAME_NODE) return NULL;

    tList* lp = xml_parse_attr((char*)attr);

    if (xml->ldat.lst==NULL) xml->ldat.lst = lp;
    else add_tList_end(xml->ldat.lst, lp);

    return xml;
}


/**
tXML*  add_xml_attr_str(tXML* xml, const char* name, const char* value)

xml に属性名 name, 文字列の属性値 value を持つノードを追加する．
*/
tXML*  add_xml_attr_str(tXML* xml, const char* name, const char* value)
{
    if (xml==NULL || name==NULL) return NULL;

    Buffer attr = make_xml_attr_str(name, value);
    add_xml_attr(xml, (char*)attr.buf);
    free_Buffer(&attr);

    return xml;
}


/**
tXML*  add_xml_attr_int(tXML* xml, const char* name, int value)

xml に属性名 name, 整数の属性値 value を持つノードを追加する．
*/
tXML*  add_xml_attr_int(tXML* xml, const char* name, int value)
{
    if (xml==NULL || name==NULL) return NULL;
    //
    Buffer attr = make_xml_attr_int(name, value);
    add_xml_attr(xml, (char*)attr.buf);
    free_Buffer(&attr);

    return xml;
}


/**
tXML*  add_xml_attr_float(tXML* xml, const char* name, float value)

xml に属性名 name, float型実数の属性値 value を持つノードを追加する．
*/
tXML*  add_xml_attr_float(tXML* xml, const char* name, float value)
{
    if (xml==NULL || name==NULL) return NULL;
    //
    Buffer attr = make_xml_attr_float(name, value);
    add_xml_attr(xml, (char*)attr.buf);
    free_Buffer(&attr);

    return xml;
}


/**
tXML*  add_xml_attr_double(tXML* xml, const char* name, double value)

xml に属性名 name, double型実数の属性値 value を持つノードを追加する．
*/
tXML*  add_xml_attr_double(tXML* xml, const char* name, double value)
{
    if (xml==NULL || name==NULL) return NULL;
    //
    Buffer attr = make_xml_attr_double(name, value);
    add_xml_attr(xml, (char*)attr.buf);
    free_Buffer(&attr);

    return xml;
}


/**
char*  get_first_xml_nodename(tXML* xml)

XMLツリーの最初のノードの名前を返す．

@param  xml  XMLツリー
@return XMLツリーの最初のノードの名前．free() してはいけない．
*/
char*  get_first_xml_nodename(tXML* xml)
{  
    char* nodename = NULL;
   
    if (xml==NULL) return NULL;
    tList* lp = xml;
    if (lp->ldat.id==XML_ANCHOR_NODE) lp = lp->next;
   
    while (lp!=NULL) {
        if (lp->ldat.id==XML_NAME_NODE) {
            if (lp->ldat.key.buf!=NULL) {
                nodename = (char*)lp->ldat.key.buf;
                break;
            }
        }
        lp = lp->ysis;
    }
    return nodename;
}


/**
Buffer make_xml_attr_bystr(const char* name, const char* value)

属性名 name, 文字列の属性値 value を持つ Buffer型変数を作成する．
*/
Buffer make_xml_attr_bystr(const char* name, const char* value)
{
    Buffer buf = make_Buffer_bystr(name);
    cat_s2Buffer("=\"", &buf);
    cat_s2Buffer(value, &buf);
    cat_s2Buffer("\"",  &buf);

    return buf;
}


/**
Buffer make_xml_attr_byint(const char* name, int value)

属性名 name, 整数の属性値 value を持つ Buffer型変数を作成する．
*/
Buffer make_xml_attr_byint(const char* name, int value)
{
    char* str = itostr_ts(value);

    Buffer buf = make_Buffer_bystr(name);
    cat_s2Buffer("=\"", &buf);
    cat_s2Buffer(str, &buf);
    cat_s2Buffer("\"",  &buf);

    freeNull(str);
    return buf;
}


/**
Buffer make_xml_attr_byfloat(const char* name, float value)

属性名 name, float型実数の属性値 value を持つ Buffer型変数を作成する．
*/
Buffer make_xml_attr_byfloat(const char* name, float value)
{
    char* str = ftostr_ts(value);

    Buffer buf = make_Buffer_bystr(name);
    cat_s2Buffer("=\"", &buf);
    cat_s2Buffer(str, &buf);
    cat_s2Buffer("\"",  &buf);

    freeNull(str);
    return buf;
}


/**
Buffer make_xml_attr_bydouble(const char* name, double value)

属性名 name, double型実数の属性値 value を持つ Buffer型変数を作成する．
*/
Buffer make_xml_attr_bydouble(const char* name, double value)
{
    char* str = dtostr_ts(value);

    Buffer buf = make_Buffer_bystr(name);
    cat_s2Buffer("=\"", &buf);
    cat_s2Buffer(str, &buf);
    cat_s2Buffer("\"",  &buf);

    freeNull(str);
    return buf;
}


/**
char* xml_get_node_content(tXML* node)

コンテントへのポインタを返す．free() してはいけない．
*/
char* xml_get_node_content(tXML* node)
{
    if (node==NULL || node->next==NULL) return NULL;

    if (node->ldat.id==XML_NAME_NODE) {
        if (node->next->ldat.id==XML_CONTENT_NODE) return (char*)(node->next->ldat.key.buf);
    }
    return NULL;
}


/**
char* xml_get_node_attr(tXML* node, const char* attr)

属性値を持つノードのポインタから，指定された属性値へのポインタを返す．free() してはいけない．
*/
char* xml_get_node_attr(tXML* node, const char* attr)
{
    if (node==NULL) return NULL;

    if (node->ldat.id==XML_NAME_NODE) {
        tList* lt = node->ldat.lst;
        while (lt!=NULL) {
            if (!strcmp((const char*)lt->ldat.key.buf, attr)) return (char*)(lt->ldat.val.buf);
            lt = lt->next;
        }
    }
    return NULL;
}


/**
tXML*  dup_merge_xml(tXML* pp, tXML* tp)

XMLツリー ppの直下に（Yunger Sister として）XMLツリー tpを複製する．

@param  pp  複製されたツリーのトップとなるノード
@param  tp  複製するツリー

@return 複製された XMLツリーへのポインタ．
@return pp がNULLでない場合は pp．pp がNULLの場合は，tpを複製したツリーのトップ．
*/
tXML*  dup_merge_xml(tXML* pp, tXML* tp)
{
    if (tp==NULL) return pp;

    if (tp->ldat.id==XML_ANCHOR_NODE) {
        if (tp->next!=NULL) tp = tp->next;
        else return pp;
    }

    pp = dup_merge_tTree(pp, tp);

/*
    tXML* pt;
    if (pp->ldat.id!=XML_ANCHOR_NODE) {
        pt = new_tTree_node();
        pt->ldat.id = XML_ANCHOR_NODE;
        pt->depth = -1;
        add_tTree(pt, pp);
        close_xml(pt);
        return pt;
    }
*/
    return pp;
}


/** @def del_xml 
指定したノード以下のXMLツリー（ppの姉妹は含まない）を削除する．
*/


/** @def del_sisters_xml 
指定したノード以下のXMLツリー（ppの姉妹を含む）を削除する．
*/


/** @def del_sisters_children_xml 
指定したノードの姉妹XMLツリー，子XMLツリーを削除する．指定したXMLノードも削除する．

@attention
注）再帰処理用．親ノードに対する処理は行わないので，別途呼び出し側で行うこと．
*/


/** @def del_all_xml
XMLツリーの全ノードの削除．ポインタのノードを含むXMLツリー全体を削除する．@n
ポインタはツリー中であれば，どこを指していても良い．
*/


/**
void   print_xml(FILE* fp, tXML* pp, int mode)
    
XMLの表示（出力）．

@param  fp    出力するファイルへのポインタ．NULLの場合は stderr
@param  pp    表示を開始するXMLノードへのポインタ．
@param  mode  @b XML_ONELINE_FORMAT, @b XML_CRLF_FORMAT, @b XML_INDENT_FORMAT
*/
void   print_xml(FILE* fp, tXML* pp, int mode)
{
    if (fp==NULL) fp = stderr;

    if (pp->ldat.id==XML_ANCHOR_NODE) pp = pp->next;

    if (pp!=NULL) {
        Buffer buf = xml_inverse_parse(pp, mode);
        if (buf.buf!=NULL) {
            fprintf(fp, "%s", buf.buf);
            free_Buffer(&buf);
        }
        else fprintf(fp, "(XML is NULL)\n");
    }  
    else {
        fprintf(fp, "(XML is NULL)\n");
    } 
    fflush(fp);
       
    return;
}


/**
void   print_xml_node(FILE* fp, tXML* pp)

XMLのノード情報を表示する．デバッグ用．@n
*/
void   print_xml_node(FILE* fp, tXML* pp)
{
    if (fp==NULL) fp = stderr;

    //fprintf(fp, "深さ: ノード種別 [状態] ノード名 (属性数)\n");

    if (pp!=NULL) {
        if (pp->ldat.id==XML_ANCHOR_NODE) pp = pp->next;
        if (pp!=NULL) {
            int num = count_tList((tList*)pp->ldat.lst);
            fprintf(fp, "%d: %d [%d] %s (%d)", pp->depth, pp->ldat.id, pp->state, pp->ldat.key.buf, num);
            fprintf(fp, "\n");
        }
        else {
            fprintf(fp, "(XML is ANCHOR only)\n");
        }
    }  
    else {
        fprintf(fp, "(XML is NULL)\n");
    }  
    fflush(fp);

    return;
}


/**
void   print_xml_tree(FILE* fp, tXML* pp, const char* space)
    
XMLツリーをそのままツリー表示する．デバッグ用．@n
ポインタ pp以降の全てのノードのキー部のバッファを標準エラー出力に表示する．@n

表示：入れ子の深さ: ノード種別 [ノード状態] ノード名 (属性の個数)

@param  fp     出力するファイルへのポインタ．NULLの場合は stderr
@param  pp     表示を開始するノードへのポインタ．
@param  space  出力の書式を揃えるための空白（インデント）を指定する．例 "    "
*/
void   print_xml_tree(FILE* fp, tXML* pp, const char* space)
{
    int i;
    if (fp==NULL) fp = stderr;

    //fprintf(fp, "深さ: ノード種別 [状態] ノード名 (属性数)\n");

    if (pp!=NULL) {
        if (pp->ldat.id==XML_ANCHOR_NODE) pp = pp->next;
        if (pp!=NULL) {
            for(i=0; i<pp->depth; i++)   fprintf(fp, "%s", space);
            for(i=0; i<pp->depth-1; i++) fprintf(fp, "%s", "   ");       // for "-> "
            print_sister_xml_tree(fp, pp, space);
            fprintf(fp, "\n");
        }
        else {
            fprintf(fp, "(XML is ANCHOR only)\n");
        }
    }  
    else {
        fprintf(fp, "(XML is NULL)\n");
    }  
    fflush(fp);

    return;
}


/**
void   print_sister_xml_tree(FILE* fp, tXML* pp, const char* space)
    
XMLツリーの表示．ppの姉妹ノードも出力する．@n
ポインタ pp以降の全てのノードのキー部のバッファを標準エラー出力に表示する．@n
表示：入れ子の深さ，ID, ノード状態, ノード名(ノード値)，ノード属性の個数

@param  fp     出力するファイルへのポインタ．NULLの場合は stderr
@param  pp       表示を開始するノードへのポインタ．(姉妹ノードも出力する）
@param  space  出力の書式を揃えるための空白（インデント）を指定する．例 "    "
*/
void   print_sister_xml_tree(FILE* fp, tXML* pp, const char* space)
{
    if (fp==NULL) fp = stderr;

    if (pp!=NULL) {
        if (pp->ldat.id==XML_ANCHOR_NODE) {
            if (pp->next!=NULL) pp = pp->next;
        }

        while(pp->esis!=NULL) pp = pp->esis;
        //
        while(pp!=NULL) {
            int i;
            tList_data ld = pp->ldat;

            if (pp->depth>=0) {
                if (pp->esis!=NULL || pp->depth>=0) fprintf(fp, "\n");
                if (pp->depth>0) {
                    for(i=0; i<pp->depth; i++) fprintf(fp, "%s", space);
                    //for(i=0; i<pp->depth-1; i++) fprintf(fp, "%s", "   ");
                    fprintf(fp, "-> ");
                }
            }
            int num = count_tList((tList*)ld.lst);
            fprintf(fp, "%d: %d [%d] %s (%d)", pp->depth, ld.id, pp->state, ld.key.buf, num);

            if (pp->next!=NULL) print_sister_xml_tree(fp, pp->next, space);
       
            pp = pp->ysis;
        };
    }  
    else {
        fprintf(fp, "(XML is NULL)\n");
    } 
    fflush(fp);
       
    return;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// Search and Get/Set
//

/**
tXML*  get_xml_node(tXML* pp, tXML* pt)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに最初に一致した枝の，ptの最後のノードに対応したノードへのポインタを返す．@n
pp の姉妹ツリーも検索するので注意．@n

また，pt の中で ctrl が @b TREE_NOCMP_NODE または @b TREE_NOCMP_NODE となっているノードは比較されない．@n
返ってきた tXML* は free() してはいけない．

@param  pp  検索対象のXMLツリー．姉妹ツリーも検索する．
@param  pt  検索するパターン
@return ptの最後のノードに対応するノードへのポインタ．free() してはいけない．

@par 例
以下の場合，pp の A->C->M->X の Xへのポインタが返る．
@code
    pp                         pt
    A --> B --> M              C --> M --> X
      --> C --> M --> X               
      --> Y --> Z   
            --> N
@endcode
*/
tXML*  get_xml_node(tXML* pp, tXML* pt)
{
    int fnd;
    tXML* tt;
    tXML* pm;

    if (pp==NULL || pt==NULL) return NULL;

    pm = pp;
    if (pp->ldat.id==XML_ANCHOR_NODE) {
        if (pp->next!=NULL) pp = pp->next;
        else return NULL;
    }
    if (pt->ldat.id==XML_ANCHOR_NODE) {
        if (pt->next!=NULL) pt = pt->next;
        else return NULL;
    }

    tt = find_xml_end(pt);
    if (tt==NULL) return FALSE;

    while(pp->esis!=NULL) pp = pp->esis;
    fnd = find_match_xml(pp, pt);
    if (fnd) tt = tt->altp;
    else     tt = NULL;

    clear_tTree_ctrl(pm);

    return tt;
}


/**
int set_xml_node(tXML* pp, tXML* pt, const char* name)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに最初に一致した枝の，ptの最後のノード対応したノードにノード名をコピーする．@n
pp の姉妹ツリーも検索するので注意．@n

pt の中で ctrl が TREE_NOCMP_NODE または TREE_NOCMP_COPY_NODE となっている ノードは比較されない．
    
@param  pp    検索対象のXMLツリー．姉妹ツリーも検索する．
@param  pt    検索パターン
@param  name  ptの最後のノードに対応するノードにコピーするノード名．

@retval TRUE  設定するノードノードを見つけた．正常に設定れたかどうかは不明．
@retval FALSE 設定するノードノードを見つけられなかった．
*/
int set_xml_node(tXML* pp, tXML* pt, const char* name)
{
    tXML* tt;

    if (pp==NULL || pt==NULL || name==NULL) return FALSE;

    tt = get_xml_node(pp, pt);
    if (tt==NULL) return FALSE;
    
    copy_s2Buffer(name, &(tt->ldat.key));

    return TRUE;
}


/**
int set_xml_end_node(tXML* pp, tXML* pt)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに最初に一致した枝の，
ptの最後のノードに対応したノードに ptの最後のノードの値（含む属性）をコピーする．@n
pp の姉妹ツリーも検索するので注意．@n

ただし，ptの最後のノードに関しては比較されない(コピー用だから)．@n
pt の中で ctrl が @b TREE_NOCMP_NODE または @b TREE_NOCMP_COPY_NODE となっているノードは比較されない．@n
これらのノードは必ず一致する．コピーを行うのは ptの最後のノードのみである．

コピーする属性は @b ldat.id, @b ldat.lv, @b ldat.sz, @b ldat.key, l@b dat.val, @b ldat.ptr, @b ldat.lst @n
@b ldat.val, @b ldat.ptr, @b ldat.lst については，ptで値が設定されていなければ，置き換えを行わない．
    
@param  pp  検索対象のXMLツリー．姉妹ツリーも検索する．
@param  pt  検索パターン

@retval TRUE  設定する枝を見つけた．正常に設定れたかどうかは不明．
@retval FALSE 設定する枝を見つけられなかった．
*/
int set_xml_end_node(tXML* pp, tXML* pt)
{
    int      ret;
    tXML* tt;
    tXML* pm;


    if (pp==NULL || pt==NULL) return FALSE;

    pm = pp;
    if (pp->ldat.id==XML_ANCHOR_NODE) {
        if (pp->next!=NULL) pp = pp->next;
        else return FALSE;
    }
    if (pt->ldat.id==XML_ANCHOR_NODE) {
        if (pt->next!=NULL) pt = pt->next;
        else return FALSE;
    }

    tt = find_xml_end(pt);
    if (tt==NULL) return FALSE;
    tt->ctrl = TREE_NOCMP_COPY_NODE;        // 比べない．最後にコピー．

    while(pp->esis!=NULL) pp = pp->esis;
    ret = find_match_xml(pp, pt);
    if (ret) {
        copy_tTree_byctrl(pt);
        adjust_tTree_depth(pp);
    }

    clear_tTree_ctrl(pm);

    return ret;
}


/**
tXML*  get_xml_content(tXML* pp, tXML* pt)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに最初に一致した枝の，
ptの最後のノードに対応したノードのコンテントへのポインタを返す．@n
pp の姉妹ツリーも検索するので注意．@n

pt の中で ctrl が TREE_NOCMP_NODE または TREE_NOCMP_COPY_NODE となっているノードは比較されない．@n
これらのノードは必ず一致する．

返ってきた tXML* は free() してはいけない．

@param  pp  検索対象のXMLツリー．姉妹ツリーも検索する．
@param  pt  検索するパターン
@return ptの最後のノードの次のノードに対応するコンテントへのポインタ．free() してはいけない．

@par 例
以下の場合，Z へのポインタが返る．
@code
    pp                            pt
    A --> B --> M                 C --> M --> Y
      --> C --> M --> X               
                  --> Y --> Z   
            --> N
@endcode
*/
tXML*  get_xml_content(tXML* pp, tXML* pt)
{
    int fnd;
    tXML* tt = NULL;
    tXML* pm;
    tXML* dm;

    if (pp==NULL || pt==NULL) return NULL;

    pm = pp;
    if (pp->ldat.id==XML_ANCHOR_NODE) {
        if (pp->next!=NULL) pp = pp->next;
        else return NULL;
    }
    if (pt->ldat.id==XML_ANCHOR_NODE) {
        if (pt->next!=NULL) pt = pt->next;
        else return NULL;
    }

    tt = find_xml_end(pt);
    if (tt==NULL) return NULL;
    dm = add_xml_content(tt, (char*)"DUMMY");
    if (dm==NULL) return NULL;
    dm->ctrl = TREE_NOCMP_NODE;

    while(pp->esis!=NULL) pp = pp->esis;

    fnd = find_match_xml(pp, pt);
    if (fnd) tt = dm->altp;
    else     tt = NULL;

    del_xml(&dm);
    clear_tTree_ctrl(pm);

    return tt;
}


/**
int set_xml_content(tXML* pp, tXML* pt, const char* content)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに最初に一致した枝の，
ptの最後ノードに対応したのノードのコンテントを contentで置き換える．@n
pp の姉妹ツリーも検索するので注意．@n

pt の中で ctrl が TREE_NOCMP_NODE または TREE_NOCMP_COPY_NODE となっているノードは
比較されない．これらのノードは必ず一致する．
    
@param  pp       検索対象のツリー．姉妹ツリーも検索する．
@param  pt       検索パターン
@param  content  ptの最後のノードに対応するノードにコピーするノード値．

@retval TRUE     設定する枝を見つけた．正常に設定れたかどうかは不明．
@retval FALSE    設定する枝を見つけられなかった．
*/
int set_xml_content(tXML* pp, tXML* pt, const char* content)
{
    tXML* tt;

    if (pp==NULL || pt==NULL || content==NULL) return FALSE;

    tt = get_xml_content(pp, pt);
    if (tt==NULL) return FALSE;
    
    copy_s2Buffer(content, &(tt->ldat.key));
    return TRUE;
}


/**
int  get_xml_int_content(tXML* pp, tXML* pt)

get_xml_content() を使用し，XML コンテンツの内容を int型で返す．
*/
int  get_xml_int_content(tXML* pp, tXML* pt)
{
    int ret = 0;

    tXML* node = get_xml_content(pp, pt);
    if (node!=NULL) ret = atoi((char*)(node->ldat.key.buf));
    return ret;
}


/**
float  get_xml_float_content(tXML* pp, tXML* pt)

get_xml_content() を使用し，XML コンテンツの内容を float型で返す．
*/
float  get_xml_float_content(tXML* pp, tXML* pt)
{
    float ret = 0.0;

    tXML* node = get_xml_content(pp, pt);
    if (node!=NULL) ret = (float)atof((char*)(node->ldat.key.buf));
    return ret;
}


/**
double  get_xml_double_content(tXML* pp, tXML* pt)

get_xml_content() を使用し，XML コンテンツの内容を double型で返す．
*/
double  get_xml_double_content(tXML* pp, tXML* pt)
{
    double ret = 0.0;

    tXML* node = get_xml_content(pp, pt);
    if (node!=NULL) ret = atof((char*)(node->ldat.key.buf));
    return ret;
}


/**
int  get_xml_char_content(tXML* pp, tXML* pt)

get_xml_content() を使用し，XML コンテンツの内容を char*型で返す．free() してはいけない
*/
char*   get_xml_char_content(tXML* pp, tXML* pt)
{
    char* ret = NULL;

    tXML* node = get_xml_content(pp, pt);
    if (node!=NULL) ret = (char*)(node->ldat.key.buf);
    return ret;
}


/**
tList*  get_xml_attr(tXML* pp, tXML* pt)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに最初に一致した枝の，
ptの最後のノードに対応したノードのノード属性値へのリストを返す．@n
pp の姉妹ツリーも検索するので注意．@n

pt の中で ctrl が @b TREE_NOCMP_NODE または @b TREE_NOCMP_COPY_NODE となっているノードは
比較されない．これらのノードは必ず一致する．

@param  pp  検索対象のツリー．姉妹ツリーも検索する．
@param  pt  検索するパターン
@return ptの最後のノード対応にする，ppのノードの属性値（リスト）
*/
tList*  get_xml_attr(tXML* pp, tXML* pt)
{
    tList* lp = NULL;
    tXML*  tt;

    if (pp==NULL || pt==NULL) return NULL;

    tt  = get_xml_node(pp, pt);
    if (tt!=NULL) lp = tt->ldat.lst;    
    return lp;
}


/**
char*  get_xml_char_attr(tXML* pp, tXML* pt, const char* attr)

get_xml_node() で検索したノードから，属性値 attrの値を char*型で取り出す．free() してはいけない．
*/
char*  get_xml_char_attr(tXML* pp, tXML* pt, const char* attr)
{
    char* ret = NULL;
    if (pp==NULL || pt==NULL || attr==NULL) return ret;

    tXML* node = get_xml_node(pp, pt);
    if (node!=NULL) ret = xml_get_node_attr(node, attr);
    return ret;
}


/**
int  get_xml_int_attr(tXML* pp, tXML* pt, const char* attr)

get_xml_node() で検索したノードから，属性値 attrの値を int型で取り出す．@n
値は " " で括られていても良い．
*/
int  get_xml_int_attr(tXML* pp, tXML* pt, const char* attr)
{
    int ret = 0;
    if (pp==NULL || pt==NULL || attr==NULL) return ret;

    tXML* node = get_xml_node(pp, pt);
    if (node!=NULL) {
        char* val = xml_get_node_attr(node, attr);
        if (val!=NULL) {
            Buffer buf = make_Buffer_bystr(val);
            char* ptr  = (char*)buf.buf;
            if (ptr[strlen(ptr)-1]=='"') ptr[strlen(ptr)-1] = '\0';
            if (ptr[0]=='"') ptr++; 
            ret = atoi(ptr);
            free_Buffer(&buf);
        }
    }
    return ret;
}


/**
double  get_xml_double_attr(tXML* pp, tXML* pt, const char* attr)

get_xml_node() で検索したノードから，属性値 attrの値を double型で取り出す．@n
値は " " で括られていても良い．
*/
double  get_xml_double_attr(tXML* pp, tXML* pt, const char* attr)
{
    double ret = 0.0;
    if (pp==NULL || pt==NULL || attr==NULL) return ret;

    tXML* node = get_xml_node(pp, pt);
    if (node!=NULL) {
        char* val = xml_get_node_attr(node, attr);
        if (val!=NULL) {
            Buffer buf = make_Buffer_bystr(val);
            char* ptr  = (char*)buf.buf;
            if (ptr[strlen(ptr)-1]=='"') ptr[strlen(ptr)-1] = '\0';
            if (ptr[0]=='"') ptr++; 
            ret = atof(ptr);
            free_Buffer(&buf);
        }
    }
    return ret;
}


/**
int  set_xml_attr(tXML* pp, tXML* pt, tList* at)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに最初に一致した枝の，
ptの最後のノードに対応したノードのノードの属性として atの値をコピーする．@n
pp の姉妹ツリーも検索するので注意．@n

pt の中で ctrl が TREE_NOCMP_NODE または TREE_NOCMP_COPY_NODE となっているノードは
比較されない．これらのノードは必ず一致する．

@param  pp    検索対象のツリー．姉妹ツリーも検索する．
@param  pt    検索パターン
@param  at    設定する属性の格納されたリスト．ldat.key に属性名，ldat.val に属性値("",''付き)

@retval TRUE  設定する枝を見つけた．正常に設置されたかどうかは不明．
@retval FALSE 設定する枝を見つけられなかった．
*/
int  set_xml_attr(tXML* pp, tXML* pt, tList* at)
{
    tXML* tt;

    if (pp==NULL || pt==NULL || at==NULL) return FALSE;

    tt  = get_xml_node(pp, pt);
    if (tt!=NULL) {
        del_all_tList(&(tt->ldat.lst));
        tt->ldat.lst = dup_tList(at);
    }
    else return FALSE;
    return TRUE;
}


/**
int  replace_xml_content(tXML* pp, tXML* pt, const char* src, const char* dst)

get_xml_content() を使用し，XMLキーの src部分を dstで書き換える．
*/
int  replace_xml_content(tXML* pp, tXML* pt, const char* src, const char* dst)
{
    tXML*  tt;
    Buffer tg;

    if (pp==NULL || pt==NULL || src==NULL || dst==NULL) return FALSE;

    tt = get_xml_content(pp, pt);
    if (tt==NULL) return FALSE;

    tg = replace_sBuffer_bystr(tt->ldat.key, src, dst);
    if (tg.buf==NULL) return FALSE;

    free_Buffer(&(tt->ldat.key));
    tt->ldat.key = tg;
    
    return TRUE;
}


/**
tXML*  get_xml_node_bystr(tXML* pp, const char* str)

get_xml_node(tXML* pp, tXML* pt) の _bystr バージョン
*/
tXML*  get_xml_node_bystr(tXML* pp, const char* str)
{
    tXML* tx;
    tXML* tt;

    if (pp==NULL || str==NULL) return FALSE;

    tx = xml_parse((char*)str);
    tt = get_xml_node(pp, tx);
    del_xml(&tx);
    return tt;
}


/**
int  set_xml_node_bystr(tXML* pp, const char* str, const char* val)

set_xml_node(tXML* pp, tXML* pt, const char* val) の _bystr バージョン
*/
int  set_xml_node_bystr(tXML* pp, const char* str, const char* val)
{
    tXML*  tx;
    int    rt;

    if (pp==NULL || str==NULL || val==NULL) return FALSE;

    tx = xml_parse((char*)str);
    rt = set_xml_node(pp, tx, val);
    del_xml(&tx);
    return rt;
}


/**
int  set_xml_end_node_bystr(tXML* pp, const char* str)

set_xml_end_node(tXML* pp, tXML* pt) の _bystr バージョン
*/
int  set_xml_end_node_bystr(tXML* pp, const char* str)
{
    tXML*  tx;
    int    rt;

    if (pp==NULL || str==NULL) return FALSE;

    tx = xml_parse((char*)str);
    rt = set_xml_end_node(pp, tx);
    del_xml(&tx);
    return rt;
}


/**
tXML*  get_xml_content_bystr(tXML* pp, const char* str)

get_xml_content(tXML* pp, tXML* pt) の _bystr バージョン
*/
tXML*  get_xml_content_bystr(tXML* pp, const char* str)
{
    tXML* tx;
    tXML* tt;

    if (pp==NULL || str==NULL) return FALSE;

    tx = xml_parse((char*)str);
    tt = get_xml_content(pp, tx);
    del_xml(&tx);
    return tt;
}


/**
int  get_xml_int_content_bystr(tXML* pp, const char* str)

get_xml_int_content(tXML* pp, tXML* pt) の _bystr バージョン．@n
get_xml_content()_bystr を使用し，XML コンテンツの内容を int型で返す．
*/
int  get_xml_int_content_bystr(tXML* pp, const char* str)
{
    int ret = 0;

    tXML* node = get_xml_content_bystr(pp, str);
    if (node!=NULL) ret = atoi((char*)(node->ldat.key.buf));
    return ret;
}


/**
float  get_xml_float_content_bystr(tXML* pp, const char* str)

get_xml_float_content(tXML* pp, tXML* pt) の _bystr バージョン．@n
get_xml_content_bystr() を使用し，XML コンテンツの内容を float型で返す．
*/
float  get_xml_float_content_bystr(tXML* pp, const char* str)
{
    float ret = 0.0;

    tXML* node = get_xml_content_bystr(pp, str);
    if (node!=NULL) ret = (float)atof((char*)(node->ldat.key.buf));
    return ret;
}


/**
double  get_xml_double_content_bystr(tXML* pp, const char* str);

get_xml_double_content(tXML* pp, tXML* pt) の _bystr バージョン．@n
get_xml_content_bystr() を使用し，XML コンテンツの内容を double型で返す．
*/
double  get_xml_double_content_bystr(tXML* pp, const char* str)
{
    double ret = 0.0;

    tXML* node = get_xml_content_bystr(pp, str);
    if (node!=NULL) ret = atof((char*)(node->ldat.key.buf));
    return ret;
}


/**
char*   get_xml_char_content_bystr(tXML* pp, const char* str);

get_xml_char_content(tXML* pp, tXML* pt) の _bystr バージョン．@n
get_xml_content_bystr() を使用し，XML コンテンツの内容を char*型で返す．@n
free() してはいけない．
*/
char*   get_xml_char_content_bystr(tXML* pp, const char* str)
{
    char* ret = NULL;

    tXML* node = get_xml_content_bystr(pp, str);
    if (node!=NULL) ret = (char*)(node->ldat.key.buf);
    return ret;
}


/**
int  set_xml_content_bystr(tXML* pp, const char* str, const char* val)

set_xml_content(tXML* pp, tXML* pt, char* val) の _bystr バージョン
*/
int  set_xml_content_bystr(tXML* pp, const char* str, const char* val)
{
    tXML*  tx;
    int    rt;

    if (pp==NULL || str==NULL || val==NULL) return FALSE;

    tx = xml_parse((char*)str);
    rt = set_xml_content(pp, tx, val);
    del_xml(&tx);
    return rt;
}


/**
tList*  get_xml_attr_bystr(tXML* pp, const char* str)

get_xml_attr(tXML* pp, tXML* pt) の _bystr バージョン
*/
tList*  get_xml_attr_bystr(tXML* pp, const char* str)
{
    tXML*  tx;
    tList* tt;

    if (pp==NULL || str==NULL) return NULL;

    tx = xml_parse((char*)str);
    tt = get_xml_attr(pp, tx);
    del_xml(&tx);
    return tt;
}


/**
char*  get_xml_char_attr_bystr(tXML* pp, const char* str, const char* attr)

get_xml_char_attr() の _bystr バージョン．@n
get_xml_node_bystr() で検索したノードから，属性値 attrの値を char*型で取り出す．free() してはいけない．
*/
char*  get_xml_char_attr_bystr(tXML* pp, const char* str, const char* attr)
{
    char* ret = NULL;
    if (pp==NULL || str==NULL || attr==NULL) return ret;

    tXML* node = get_xml_node_bystr(pp, str);
    if (node!=NULL) ret = xml_get_node_attr(node, attr);
    return ret;
}


/**
int  get_xml_int_attr_bystr(tXML* pp, const char* str, const char* attr)

get_xml_int_attr() の _bystr バージョン．@n
get_xml_node_bystr() で検索したノードから，属性値 attrの値を int型で取り出す．@n
値は " " で括られていても良い．
*/
int  get_xml_int_attr_bystr(tXML* pp, const char* str, const char* attr)
{
    int ret = 0;
    if (pp==NULL || str==NULL || attr==NULL) return ret;

    tXML* node = get_xml_node_bystr(pp, str);
    if (node!=NULL) {
        char* val = xml_get_node_attr(node, attr);
        if (val!=NULL) {
            Buffer buf = make_Buffer_bystr(val);
            char* ptr  = (char*)buf.buf;
            if (ptr[strlen(ptr)-1]=='"') ptr[strlen(ptr)-1] = '\0';
            if (ptr[0]=='"') ptr++; 
            ret = atoi(ptr);
            free_Buffer(&buf);
        }
    }
    return ret;
}


/**
double  get_xml_double_attr_bystr(tXML* pp, const char* str, const char* attr)

get_xml_double_attr() の _bystr バージョン．@n
get_xml_node_bystr() で検索したノードから，属性値 attrの値を double型で取り出す．@n
値は クォーテーションで括られていても良い．
*/
double  get_xml_double_attr_bystr(tXML* pp, const char* str, const char* attr)
{
    double ret = 0.0;
    if (pp==NULL || str==NULL || attr==NULL) return ret;

    tXML* node = get_xml_node_bystr(pp, str);
    if (node!=NULL) {
        char* val = xml_get_node_attr(node, attr);
        if (val!=NULL) {
            Buffer buf = make_Buffer_bystr(val);
            char* ptr  = (char*)buf.buf;
            if (ptr[strlen(ptr)-1]=='"') ptr[strlen(ptr)-1] = '\0';
            if (ptr[0]=='"') ptr++; 
            ret = atof(ptr);
            free_Buffer(&buf);
        }
    }
    return ret;
}


/**
int  set_xml_attr_bystr(tXML* pp, const char* str, tList* at)

set_xml_attr() の _bystr バージョン．
*/
int  set_xml_attr_bystr(tXML* pp, const char* str, tList* at)
{
    tXML* tx;
    int   rt;

    if (pp==NULL || str==NULL || at==NULL) return FALSE;

    tx = xml_parse((char*)str);
    rt = set_xml_attr(pp, tx, at);
    del_xml(&tx);
    return rt;
}


/**
int  replace_xml_content_bystr(tXML*pp, const char* str, const char* src, const char* dst)

replace_xml_content() の _bystr バージョン．
*/
int  replace_xml_content_bystr(tXML*pp, const char* str, const char* src, const char* dst)
{
    tXML* tx;
    int   rt;

    if (pp==NULL || src==NULL || src==NULL || dst==NULL) return FALSE;

    tx = xml_parse((char*)str);
    rt = replace_xml_content(pp, tx, src, dst);
    del_xml(&tx);
    return rt;
}



/////////////////////////////////////////////////////////////////////////////////////
// Operation for Multi Tags
//

/**
tList*  get_xml_node_list(tXML* pp, tXML* pt)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに一致した枝の，ptの最後のノードに対応するノードへの
ポインタをリストに格納して返す．ポインタはリストの altpに格納される．

pt の中で ctrl が TREE_NOCMP_NODE または TREE_NOCMP_COPY_NODE となっているノードは
比較されない．これらのノードは必ず一致する．

@param  pp  検索対象のツリー
@param  pt  検索するパターン
@return 該当ノードへのポインタ情報(altp)を含んだリストへのポインタ
*/
tList*  get_xml_node_list(tXML* pp, tXML* pt)
{
    tList* lp;

    if (pp==NULL || pt==NULL) return NULL;

    if (pp->ldat.id==XML_ANCHOR_NODE) {
        if (pp->next!=NULL) pp = pp->next;
        else return NULL;
    }
    if (pt->ldat.id==XML_ANCHOR_NODE) {
        if (pt->next!=NULL) pt = pt->next;
        else return NULL;
    }

    lp = find_match_xml_endlist(pp, pt);

    return lp;
}


/**
tList*  get_xml_content_list(tXML* pp, tXML* pt)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptに一致した枝の，ptの最後のノードに対応するノードの
コンテントへのポインタをリストに格納して返す．ポインタはリストの altpに格納される．

pt の中で ctrl が TREE_NOCMP_NODE または TREE_NOCMP_COPY_NODE となっているノードは比較されない．@n
これらのノードは必ず一致する．

@param  pp  検索対象のツリー
@param  pt  検索するパターン

@return 該当ノードへのポインタ情報(altp)を含んだリストへのポインタ
*/
tList*  get_xml_content_list(tXML* pp, tXML* pt)
{
    tList* lp;
    tList* dm;

    if (pp==NULL || pt==NULL) return NULL;

    if (pp->ldat.id==XML_ANCHOR_NODE) {
        if (pp->next!=NULL) pp = pp->next;
        else return NULL;
    }
    if (pt->ldat.id==XML_ANCHOR_NODE) {
        if (pt->next!=NULL) pt = pt->next;
        else return NULL;
    }

    dm = find_xml_end(pt);
    if (dm==NULL) return NULL;
    dm = add_xml_content(dm, (char*)"DUMMY");
    if (dm==NULL) return NULL;
    dm->ctrl = TREE_NOCMP_NODE;

    lp = find_match_xml_endlist(pp, pt);
    del_xml(&dm);

    return lp;
}


/**
int  set_xml_content_list(tXML* pp, tXML* pt, const char* content)

get_xml_content_list() で検出したコンテントを, content で置き換える．

@param  pp       対象のXMLツリー
@param  pt       検索パターン
@param  content  書き換えのノード値  

@return 書き換えたノードの個数
*/
int  set_xml_content_list(tXML* pp, tXML* pt, const char* content)
{
    int    num = 0;
    tList* lt;
    tList* lp;

    if (pp==NULL  || pt==NULL || content==NULL)  return 0;

    lp = lt = get_xml_content_list(pp, pt);
    if (lt==NULL) return 0;

    while (lt!=NULL) {
        if (lt->altp!=NULL) {
            copy_s2Buffer(content, &(lt->altp->ldat.key));
            num++;
        }
        lt = lt->next;
    }
    del_tList(&lp);

    return num;
}


/**
tList*  get_xml_node_list_bystr(tXML* pp, const char* str)

get_xml_node_list() の _bystr バージョン．
*/
tList*  get_xml_node_list_bystr(tXML* pp, const char* str)
{
    tXML*  tx;
    tList* tt;

    if (pp==NULL || str==NULL) return NULL;

    tx = xml_parse((char*)str);
    tt = get_xml_node_list(pp, tx);
    del_xml(&tx);

    return tt;
}


/**
tList*  get_xml_conetnt_list_bystr(tXML* pp, const char* str)

get_xml_conetnt_list_bystr() の _bystr バージョン．
*/
tList*  get_xml_content_list_bystr(tXML* pp, const char* str)
{
    tXML*  tx;
    tList* tt;

    if (pp==NULL || str==NULL) return NULL;

    tx = xml_parse((char*)str);
    tt = get_xml_content_list(pp, tx);
    del_xml(&tx);

    return tt;
}


/**
int  set_xml_content_list_bystr(tXML* pp, const char* str, const char* content)

set_xml_content_list_bystr() の _bystr バージョン．
*/
int  set_xml_content_list_bystr(tXML* pp, const char* str, const char* content)
{
    tXML* tx;
    int   rt;

    if (pp==NULL  || str==NULL || content==NULL) return 0;

    tx = xml_parse((char*)str);
    rt = set_xml_content_list(pp, tx, content);
    del_xml(&tx);

    return rt;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// Search Function for XML
//         tTree 用の関数とほぼ同じ
//

/**
int  find_match_xml(tXML* pp, tXML* pt)

ツリー pp内で ツリー ptと同じパターンの枝を探す．姉妹ツリーも検索するので注意．@n 

同じパターンの探索では キー値のみを比較し，ノード値は比較しない．@n
ただし，pt->ctrl が TREE_NOCMP_NODE または TREE_NOCMP_COPY_NODE のノードは比べない(常に一致とする)．
    
もし同じツリーパターンがある場合，trの各ノードの altpには，一番最初に見つかった対応する ppの各ノードへ
のポインタが格納される．

check_match_xml() との違い．
- check_match_xml() では比べる枝の開始ノードはppに固定される．
- find_match_xml() は pp内を移動しながら検索できる．

@param  pp     検索対象のツリー．姉妹ツリーも検索する．
@param  pt     検索パターンのツリー

@retval TRUE   pp中に pt同じいツリーパターンが存在する．
               ptの各ノードの altpには，一番最初に見つかった対応する ppの各ノードへのポインタが格納される．
@retval FALSE  ppに同じツリーパターンは無い．この場合，ptのaltpの値は不定となる．
*/
int  find_match_xml(tXML* pp, tXML* pt)
{
    int   ret;
    tXML* pm;

    pm = pp;
    while(pp!=NULL) {
        ret = check_match_xml(pp, pt);
        if (ret) return TRUE;

        if (pp->next!=NULL) {
            ret = find_match_xml(pp->next, pt);
            if (ret) {
                clear_tTree_ctrl(pm);
                return TRUE;
            }
        }
        pp = pp->ysis;
    }

    return FALSE;
}


/**
tXML*  find_match_xml_endlist(tXML* pp, tXML* pt)

ツリー pp内で ツリー ptと同じパターンの枝を全て探して，その枝のptの最後のノードに対応するノードの情報をリストにして返す．@n
該当ノードへのポインタは 返された各リストのaltp が保持している． 

比較では キー値のみを比較し，ノード値は比較しない．@n
また，pt->ctrl が @b TREE_NOCMP_NODE または @b TREE_NOCMP_COPY_NODE のノードは比べない(常に一致とする)．
    
@param  pp  検索対象のツリー．姉妹ツリーも検索する．
@param  pt  検索パターンのツリー

@return 該当ノードへのポインタを保持するリスト．
*/
tXML*  find_match_xml_endlist(tXML* pp, tXML* pt)
{
    tXML*  te;
    tList* lp;

    te = find_xml_end(pt);
    while(pp->esis!=NULL) pp = pp->esis;

    lp = find_match_xml_endlist_rcsv(pp, pt, te);
    if (lp!=NULL) clear_tTree_ctrl(pp);
    
    return lp;
}


/**
tList*  find_match_xml_endlist_rcsv(tXML* pp, tXML* pt, tXML* te)

find_match_xml_endlist() の補助関数

*/
tList*  find_match_xml_endlist_rcsv(tXML* pp, tXML* pt, tXML* te)
{
    tList* lt = NULL;
    tList* lp = NULL;
                    
    while(pp!=NULL) {             
        int ret = check_match_xml(pp, pt);
        if (ret && te->altp!=NULL) {
            tList* lm = new_tList_node();
            lm->altp = te->altp;  
            lt = insert_tList(lt, lm);
            if (lp==NULL) lp = lt;
            te->altp = NULL;
        }  
                            
        if (pp->next!=NULL) {     
            tList* lm = find_match_xml_endlist_rcsv(pp->next, pt, te);
            if (lm!=NULL) {       
                lt = insert_tList(lt, lm);
                if (lp==NULL) lp = lt;
                clear_tTree_ctrl(pp->next);
            }
        }
                                   
        if (!ret) pp = pp->ysis;    // 見つかった場合はもう一度．見つからなかった場合へ次へ．
    }

    return lp;
}


/**
tXML*  find_match_xml_end_node(tXML* pp, tXML* pt)

XMLツリー pp内で XMLツリー ptと同じパターンの枝を探し，ptの最後のノードに対応する pp内のノードへのポインタを返す．@n
ノードは ノード名，コンテントのどちらでも可．

@param  pp  検索対象のXML
@param  pt  検索パターンの文字列．
@return strの最後のノードに対応する pp内のノード．
*/
tXML*  find_match_xml_end_node(tXML* pp, tXML* pt)
{
    int    ret;
    tXML*  tt = NULL;
    tXML*  pm;

    if (pp==NULL || pt==NULL) return NULL;

    pm  = pp;
    ret = find_match_tTree(pp, pt);
    if (ret) {
        tt = find_xml_end(pt);
        if (tt!=NULL) {
            tt = tt->altp;
        }
    }

    clear_tTree_ctrl(pm);

    return tt;
}


/**
int  check_match_xml(tXML* tp, tXML* tr)

XMLツリー tpが XMLツリー trと同じかどうかを検査する．

tp のトップと tr のトップはキー値が一致している必要がある．一致していなければ，同じパターンは無しとする．@n
ただし，tr->ctrl が @b TREE_NOCMP_NODE または @b TREE_NOCMP_COPY_NODE のノードは比べない(常に一致とする)．

一度見つけた tpの枝の最後のノードに対しては ctrlを @b TREE_ALREADY_FOUND_NODE を設定するので，続けてチェックする
場合などは ctrl をクリアする必要がある．

もし同じツリーパターンがある場合，trの各ノードの altpには，一番最初に見つかった対応する tpの各ノードへのポインタが格納される．

@param  tp     検索対象のツリー
@param  tr     検索パターンのツリー

@retval TRUE   tp中に trと同じいツリーパターンが存在する．
@retval TRUE   trの各ノードの altpには，一番最初に見つかった対応する tpの各ノードへのポインタが格納される．
@retval FALSE  tpに同じツリーパターンは無い．この場合，trの altpの値は不定となる．
*/
int  check_match_xml(tXML* tp, tXML* tr)
{
    int   ret;
    tXML* te;
    tXML* ts;

    tXML* tt;
    tXML* ta;
    tXML* tb;

    if (tp==NULL || tr==NULL) return FALSE;

    te = find_tList_end(tr);

    ts = tp;
    while (ts!=NULL) {
        tt = cmp_sisters_xml(ts, tr);        // その階層で trと全て一致している部分があるかを確認
        if (tt==NULL) return FALSE;          // 一致していなければ，FALSE
        
        ta  = tt;                            // 比べられるツリー
        tb  = tr;                            // 比べるパターン
        ret = TRUE;
        while (tb!=NULL && ret) {
            if (tb->next==NULL) ret = TRUE;
            // ->ta, ->tb->tx: FALSE
            else if (tb->next!=NULL && ta->next==NULL) ret = FALSE;
            // ->ta->xa, ->tb->xb: xaとxbをチェック
            else ret = check_match_xml(ta->next, tb->next);

            ta = ta->ysis;
            tb = tb->ysis;
        }

        if (ret) {
            if (tr==te) tt->ctrl = TREE_ALREADY_FOUND_NODE;
            return TRUE;
        }

        ts = tt->ysis;
    }

    return FALSE;
}


/**
tXML*  cmp_sisters_xml(tXML* tp, tXML* tr)

XMLノード tpの姉妹ノードが trの姉妹ノードと同じ XMLノードまたはコンテントであるかを比較する．

ただし，tr->ctrl が @b TREE_NOCMP_NODE または @b TREE_NOCMP_COPY_NODE のノードは比べない(常に一致とする)．@n
また tp->ctrl が @b TREE_ALREADY_FOUND_NODE の場合は，常に一致しない．

もし同じノードパターンがある場合，trの各ノードの altpには対応する tpの各ノードへのポインタが格納される．@n
また tpの各ノードの ctrlには @b TREE_ALREADY_FOUND_NODE が設定される．

@param  tp   比べる姉妹ノードの長女ノード
@param  tr   探す姉妹ノードパターンの長女ノード

@return tp中で trと同じパターンが始まるノードへのポインタ．
        trの各ノードの altpには対応する tpの各ノードへのポインタが格納される．
@retval NULL tpに同じ姉妹パターンは無い．

@par 例
以下の場合，cmp_sisters_xml(tp, tr) は (3)へのポインタを返す．@n
また trの Aノードの altp には (3) へのポインタが，trの Xノードのaltpには(4)へのポインタが格納される．@n
最初に見つかったパターンのみ評価される．
@code
      tp                 tr
      --> A (1)          --> A         A, B, X は キー値(ldat.key.buf)
      --> B (2)          --> X
      --> A (3)
      --> X (4)
      --> A (5)
      --> X (6)
@endcode
*/
tXML*  cmp_sisters_xml(tXML* tp, tXML* tr)
{
    tXML* ta;
    tXML* tb = NULL;
    tXML* ts;

    ts = tp;
    while (ts!=NULL){
        ta = ts;
        tb = tr;
        while (ta!=NULL && tb!=NULL) {
            // 一致しない場合 break
            if (ta->ctrl==TREE_ALREADY_FOUND_NODE) break;
            if (tb->ctrl!=TREE_NOCMP_NODE && tb->ctrl!=TREE_NOCMP_COPY_NODE) {
                if ((ta->ldat).key.buf!=NULL && (tb->ldat).key.buf!=NULL) {
                    if (ta->ldat.id!=tb->ldat.id || strcmp((char*)((ta->ldat).key.buf), (char*)((tb->ldat).key.buf))) break;
                }
                else break;
            }

            // ta と tb は一致
            tb->altp = ta;
            ta = ta->ysis;
            tb = tb->ysis;
        }

        // ts と tr は完全に一致
        if (tb==NULL) return ts;

        ts = ts->ysis;
    }

    return NULL;
}



//////////////////////////////////////////////////////////////////////////////////////
// Simple Node Functions  (単純な構造のノードの操作)
//

/**
Buffer  get_node_content(tXML* tp, const char* name, int no)

tp 中のXMLデータから no番目のノード '@<name>node_content@</name>' のnode_contentのコピーを得る．@n
ノードが複数のコンテントを持つ場合は，最初のコンテントのみを返す．

@param  tp   探索を行う XMLデータ
@param  name ノード名
@param  no   一致した物の中で何番目の物を返すか指定する．1から数える．
*/
Buffer  get_node_content(tXML* tp, const char* name, int no)
{
    Buffer content;
    tXML*  tt;
    
    content = init_Buffer();
    if (tp==NULL || name==NULL) return content;

    if (no<=0) no = 1;
    tt = strncmp_tTree(tp, name, 0, no);
    if (tt!=NULL) {
        if (tt->next!=NULL) {
            tt = tt->next;
            while (tt!=NULL && tt->ldat.id!=XML_CONTENT_NODE) tt = tt->ysis;
            if (tt==NULL) return content;
            content = dup_Buffer(tt->ldat.key);
        }
    }

    return content;
}


/**
int  get_node_integer(tXML* tp, const char* name, int no)

tp中のXMLデータから no番目のノード '@<node_name>node_content(整数)@</node_name>' の node_content(整数)を得る．
*/
int  get_node_integer(tXML* tp, const char* name, int no)
{
    int       ret;
    Buffer content;

    content = get_node_content(tp, name, no);

    if (content.buf==NULL) ret = 0;
    else                   ret = atoi((const char*)content.buf);
    free_Buffer(&content);

    return  ret;
}


/**
int  return_exist_node(tXML* tp, const char* name, int no, Buffer* content)

bufの中に ノード '@<name>content@</name>' が存在するかどうかチェックする．@n
存在する場合はそのノードのコンテントのコピーを content に格納し，TRUE を返す．
*/
int   return_exist_node(tXML* tp, const char* name, int no, Buffer* content) 
{
    Buffer buf;
    
    buf = get_node_content(tp, name, no);
    if (buf.buf==NULL) return FALSE;

    if (content!=NULL) *content = buf;
    return TRUE;
}


/**
int  replace_all_node_integer(tXML* tp, const char* name, int src, int dst)

XMLツリー pp内で ノード名が nameである全てのノードのコンテント（整数）を，srcから dstに書き換える．
            
@param  tp    置換対象のツリー
@param  name  置き換えを行うノードのノード名
@param  src   置換対象のノードのコンテント（整数）
@param  dst   置換後のノードのコンテント（整数）

@return 置き換えたノードの数
*/
int  replace_all_node_integer(tXML* tp, const char* name, int src, int dst)
{
    int  n;
    char nums[20], numd[20];

    snprintf(nums, 10, "%d", src);
    snprintf(numd, 10, "%d", dst);

    n = replace_all_node_contents(tp, name, nums, numd);

    return n;
}


/**
int  replace_all_node_contents(tXML* pp, const char* name, const char* src, const char* dst)

XMLツリー pp内で ノード名が nameである全てのノードのコンテントを，srcから dstに書き換える．@n
一つのノードに複数のコンテントがある場合にも対応．
            
@param  pp    置換対象のツリー
@param  name  置き換えを行うノードのノード名．NULLの場合は全ての文字列．
@param  src   置換対象のノードのコンテント（の一部でも可）． NULL の場合は全ての文字列
@param  dst   置換後のノードのコンテント

@return 置き換えたノードの数
*/
int  replace_all_node_contents(tXML* pp, const char* name, const char* src, const char* dst)
{
    int  n;

    if (pp==NULL || dst==NULL) return 0;

    if (pp->ldat.id==XML_ANCHOR_NODE) {
        if (pp->next!=NULL) pp = pp->next;
        else return 0;
    }
    while(pp->esis!=NULL) pp = pp->esis;

    n = replace_all_node_contents_rcsv(pp, name, src, dst);

    return n;
}


/**
int  replace_all_node_contents_rcsv(tXML* pp, const char* name, const char* src, const char* dst)

replace_all_node_content() の補助関数
*/
int  replace_all_node_contents_rcsv(tXML* pp, const char* name, const char* src, const char* dst)
{
    int  n = 0;

    while(pp!=NULL) {
        if (pp->ldat.key.buf!=NULL && pp->ldat.id==XML_NAME_NODE && pp->ldat.lv>0) {
            if (name==NULL || !strcmp(name, (const char*)(pp->ldat.key.buf))) {
                tXML* tt = pp->next;

                // 複数のコンテント
                while (tt!=NULL) {
                    if (tt->ldat.id==XML_CONTENT_NODE) {
                        if (src==NULL) {
                            copy_s2Buffer(dst, &(tt->ldat.key));
                            n++;
                        }
                        else if (tt->ldat.key.buf!=NULL && strstr((const char*)tt->ldat.key.buf, src)!=NULL) {
                            Buffer buf = replace_sBuffer_bystr(tt->ldat.key, src, dst);
                            free_Buffer(&(tt->ldat.key));
                            tt->ldat.key = buf;
                            n++;
                        }
                    }
                    tt = tt->ysis;
                }
            }
        }

        if (pp->next!=NULL) n += replace_all_node_contents_rcsv(pp->next, name, src, dst);

        pp = pp->ysis;
    }

    return n;
}


/**
int  replace_all_node_byid(tXML* pp, const char* src, const char* dst, int id)

XMLツリー pp内で ノードの種別が idである全てのノードの内容を srcから dstに書き換える．
            
@param  pp   置換対象のツリー
@param  src  置換対象のノードのコンテント（の一部でも可）． NULL の場合は全ての文字列
@param  dst  置換後のノードのコンテント
@param  id   置き換え対象のノードのID．id<0 なら全てのノード

@return 置き換えたノードの数
*/
int  replace_all_node_byid(tXML* pp, const char* src, const char* dst, int id)
{
    int  n;

    if (pp==NULL || dst==NULL) return 0;

    if (pp->ldat.id==XML_ANCHOR_NODE) {
        if (pp->next!=NULL) pp = pp->next;
        else return 0;
    }
    while(pp->esis!=NULL) pp = pp->esis;

    n = replace_all_node_byid_rcsv(pp, src, dst, id);

    return n;
}


/**
int  replace_all_node_byid_rcsv(tXML* pp, const char* src, const char* dst, int id)

replace_all_node_byid() の補助関数
*/
int  replace_all_node_byid_rcsv(tXML* pp, const char* src, const char* dst, int id)
{
    int  n = 0;

    while(pp!=NULL) {
        if (id<0 || pp->ldat.id==id) {
            if (src==NULL) {
                copy_s2Buffer(dst, &(pp->ldat.key));
                n++;
            }
            else if (pp->ldat.key.buf!=NULL && strstr((const char*)pp->ldat.key.buf, src)!=NULL) {
                Buffer buf = replace_sBuffer_bystr(pp->ldat.key, src, dst);
                free_Buffer(&(pp->ldat.key));
                pp->ldat.key = buf;    
                n++;
            }
        }

        if (pp->next!=NULL) n += replace_all_node_byid_rcsv(pp->next, src, dst, id);

        pp = pp->ysis;
    }

    return n;
}



/////////////////////////////////////////////////////////////////////////////////////////////
// XML-RPC

/**
XML-RPC用のデータを生成する．@n

@param   name  XML-RPC のメソッド名
@param   xml   送信用データが格納された XMLデータ．
@return  XML-RPC の送信用データ．

@code
    tXML* xml   = NULL;
    tXML* array = NULL;

    xml = xml_rpc_add_member(NULL, "host", "server",      "string");
    xml = xml_rpc_add_member(xml,  "id",   "xyb12265092", "string");
    xml = xml_rpc_end_member(xml);
    array = xml_rpc_add_array(NULL, xml);

    xml = xml_rpc_add_member(NULL, "host", "client",      "string");
    xml = xml_rpc_add_member(xml,  "id",   "abc162aa890", "string");
    xml = xml_rpc_end_member(xml);
    array = xml_rpc_add_array(array, xml);

    xml = xml_rpc_add_member(NULL, "host", "localhost",   "string");
    xml = xml_rpc_add_member(xml,  "id",   "shbR1222201", "string");
    xml = xml_rpc_end_member(xml);
    array = xml_rpc_add_array(array, xml);

    array = xml_rpc_end_array(array);

    Buffer buf = xml_rpc_request_pack("mod_mdlds_write_nbdata", array);
@endcode
*/
Buffer  xml_rpc_request_pack(const char* name, tXML* xml)
{
    tXML* ptr = init_xml_doc();
    tXML* top = add_xml_node(ptr, "methodCall");
    tXML* mnm = add_xml_node(top, "methodName");
    tXML* prs = add_xml_node(top, "params");
    tXML* prm = add_xml_node(prs, "param");
    add_xml_content(mnm, name);

    join_xml(prm, xml);
    Buffer buf = xml_inverse_parse(ptr, XML_ONELINE_FORMAT);

    return buf;
}


/**
tXML*   xml_rpc_add_member(tXML* xml, char* name, char* value, char* kind)

XML-RPC の @<member> データを作って，繋げていく．@b
先頭には @<struct> ノードが付く．

@param   xml    データを繋げるためのポインタ．
@param   name   @<name> ノードの値
@param   value  @<value><...> ノードの値
@param   kind   value のデータの種類．NULL または "" の場合は "string" になる．
@return  @<member> データ

@code
<struct><member><name>name</name><value><kind>value</kind></value></member><member>....</member>
@endcode
*/
tXML*   xml_rpc_add_member(tXML* xml, char* name, char* value, char* kind)
{
    if (name==NULL) return xml;
  
    if (xml==NULL) xml = add_xml_node(NULL, "struct");
    
    tXML* mem = add_xml_node(xml, "member");
    tXML* mnm = add_xml_node(mem, "name");
    tXML* val = add_xml_node(mem, "value");
    tXML* knd;
    if (kind==NULL)         knd = add_xml_node(val, "string");
    else if (kind[0]=='\0') knd = add_xml_node(val, "string");
    else                    knd = add_xml_node(val, kind);

    add_xml_content(mnm, name);
    add_xml_content(knd, value);

    return xml;
}


/**
tXML*   xml_rpc_end_member(tXML* xml)

@<struct>@<member> データを閉じて，@<value> データを作り出す．@n
配列データにしない場合は，このデータを xml_rpc_request_pack() に渡して，送信用データを生成することも可能．

@param   xml    @<struct> データ
@return  @<vlue> データ

@code
<value><strcut><member>...</member><member>...</member>...</struct></value>  
@endcode

@code
    tXML* xml = NULL;
    xml = xml_rpc_add_member(xml, "session",  info->session, "");
    xml = xml_rpc_add_member(xml, "message",  info->message, "");
    ......
    xml = xml_rpc_end_member(xml);
    Buffer buf = xml_rpc_request_pack(Moodle_Service, xml);
@endcode
*/
tXML*   xml_rpc_end_member(tXML* xml)
{
    tXML* ptr = add_xml_node(NULL, "value");
    join_xml(ptr, xml);

    return ptr;
}


/**
tXML*   xml_rpc_add_array(tXML* xml, tXML* array)

@param   xml    データを繋げるためのポインタ．
@param   array  @<value> データ
@return  @<data> データ

@<value>@<struct> データから 配列データ @<data>...@</data> を作って繋げていく．
*/
tXML*   xml_rpc_add_array(tXML* xml, tXML* array)
{
    if (xml==NULL) xml = add_xml_node(NULL, "data");
    join_xml(xml, array);
    
    return xml;
}


/**
tXML*   xml_rpc_end_array(tXML* xml)

配列データを閉じて，送信用データを生成する．

@param   xml  @<data> ノードのポンタ
@return  @<value>@<arrau> データ

@code
<value><array><data>...</data><data>...</data>....</array></value>
@endcode
*/
tXML*   xml_rpc_end_array(tXML* xml)
{
    tXML* ptr = add_xml_node(NULL, "value");
    tXML* val = add_xml_node(ptr,  "array");
    join_xml(val, xml);

    return ptr;
}

