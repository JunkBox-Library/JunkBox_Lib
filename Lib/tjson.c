
/**
@brief   Tiny JSON ライブラリ 
@file    tjson.c
@version 1.2.1
@author  Fumi.Iseki (C)
@date    2021 8/19

@par タイトル
サブセット版 JSON 簡易 Parser

@attention
全てのパターンのパース可能性は保障しない．@n

@see     tJson
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include "tjson.h"
#include "jbxl_state.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////
// Parser
//

/**
tJson*  json_parse(char* pp, int num)

文字列のJSONデータを解釈して，tJsonのツリーを生成する．
ツリーのトップは JSON_ANCHOR_NODE となる．

@param  pp   文字列の JSONデータへのポインタ．
@param  num  0 配列を処理しない．高速．@n
             1 配列を処するが，配列の中の JSONデータは処理しない．@n
             2 配列の中の { } を処理する．@n
             N 配列処理 + 再帰処理 の段数．
@return JSONデータを格納した tJsonのアンカーへのポインタ．
@return エラーの場合，next以下のノードにはエラーを起こす直前までの内容が保存される
@retval state: エラーを起こした場合 stateに JBXL_JSON_PARSED 以外の値（負数）が入る．

@code
    tJson* json = json_parse("{\"A\": \"XYZ\"}");
@endcode
*/
tJson*  json_parse(char* pp, int num)
{
    while(*pp!='{' && *pp!='\0') pp++;
    if (*pp=='\0') return NULL;

    tJson* json = new_json_anchor_node();                    // アンカー
    //json->ldat.id = JSON_ANCHOR_NODE;
    //json->ldat.lv = JSON_VALUE_OBJ;
    //json->state   = JBXL_STATE_ANCHOR;
    //json->depth   = -1;

    // パース
    tJson* node = json_parse_prop(json, pp, num);
    if (node->state<0) return node;
    if (node->state==JBXL_JSON_PARSE_TERM) return node;     // 中途半端な入力

    // 元に戻ったか？
    if (json==node) {
        json->state = JBXL_JSON_PARSED;
    }
    else {
        json->state = JBXL_JSON_NOT_CLOSED;
    }

    // JSON rootの数
    if (json->next!=NULL) {
        int n = 0;
        node = json->next;
        while(node!=NULL) {
            if (node->ldat.id==JSON_BRACKET_NODE) n++;
            node = node->ysis;
        }
        if (n!=1) json->state = JBXL_JSON_MULTI_ROOT;
    }
    else json->state = JBXL_JSON_DEFAULT_STATE;

    return json;
}



/**
tJson*  json_parse_prop(tJson* json, char* pp, int num)

JSON Main パーサ．@n
先頭に姉妹ノードがない場合は json にNULLを指定しても可．@n
処理に json->ctrl を使用（書き換えられる）．@n

@param  json JSONデータへのポインタ．NULLでない場合は，このデータの後に結果が付加される．@n
@param  pp   パースする文字列．
@param  num  0 配列を処理しない．高速．@n
             1 配列を処するが，配列の中の JSONデータは処理しない．@n
             2 配列の中の { } を処理する．@n
             N 配列処理 + 再帰処理 の段数．
@return JSONデータを格納したポインタ．完全にパースできれば，トップを指している筈．
*/
tJson*  json_parse_prop(tJson* json, char* pp, int num)
{
    char*  pt   = NULL;
    tJson* node = NULL;

    if (json==NULL) {
        json = new_json_node();
        json->ldat.id = JSON_TEMP_NODE;
    }

    while (*pp!='\0') {
        // 
        if (*pp=='{') {
            pp++;
            if (json->ctrl!=JBXL_JSON_NODE_OPENED) {
                // open {
                if (json->prev==NULL) {
                    node = new_json_node();
                    node->ldat.id = JSON_BRACKET_NODE;
                    node->ldat.lv = JSON_VALUE_OBJ;
                    json = add_tTree_node(json, node);
                }
                else {
                    if (json->yngr!=NULL) {
                        json = json->yngr;
                        json->ldat.lv = JSON_VALUE_OBJ;
                    }
                }
            }
            json->ctrl = JBXL_NONE;

            // 最初の \", \', } を見つける
            pt = pp;
            while(*pt!='\0') {
                while(*pt=='\\') pt += 2;
                if (*pt!='\0') {
                    if (*pt=='\'' || *pt=='\"' || *pt=='}') break;
                    pt++;
                }
            }
            if (*pt=='\0') {
                json = _json_parse_term(json, NULL, NULL, "{");
                json->ctrl = JBXL_JSON_NODE_OPENED;
                return json;
            }
            //

            pp = pt;
            if (*pp!='}') {     // \" or \'
                char ch = '\"';
                if (*pt=='\'') ch = '\''; 
                //
                pt = pp + 1;
                while(*pt!='\0') {
                    while(*pt=='\\') pt += 2;
                    if (*pt!='\0') {
                        if (*pt==ch) break;
                        pt++;
                    }
                }
                if (*pt=='\0') {
                    json = _json_parse_term(json, pp, pt, "{");
                    json->ctrl = JBXL_JSON_NODE_OPENED;
                    return json;
                }

                int len = (int)(pt - pp) - 1 ;
                node = new_json_node();
                node->ldat.key = set_Buffer(pp + 1, len);
                node->ldat.id  = JSON_DATA_NODE;
                node->ldat.lv  = JSON_VALUE_NULL;
                add_tTree_node(json, node);

                pt = pt + 1; 
                while(*pt!=',' && *pt!=':' && *pt!='}' && *pt!='\0') pt++; 
                if (*pt=='\0') {
                    json->state = JBXL_JSON_PARSE_TERM;
                    return json; 
                }
                pp = pt;
            }
        }

        //
        else if (*pp=='[') {
            pt = skip_char_pair(pp, '[', ']');
            if (*pt=='\0') {
                json = _json_parse_term(json, pp, pt, "[");
                return json;
            }

            int len = (int)(pt - pp) + 1;
            Buffer temp = set_Buffer(pp, len);
            json->yngr->ldat.val = pack_Buffer(temp, '\0');
            json->yngr->ldat.lv  = JSON_VALUE_ARRAY;
            free_Buffer(&temp);

            pt++; 
            while(*pt!=',' && *pt!='}' && *pt!='\0') pt++; 
            if (*pt=='\0') {
                json->state = JBXL_JSON_PARSE_TERM;
                return json;    
            }
            //
            if (num>0) _json_array_parse(json->yngr, num-1);

            pp = pt;
        }

        //
        else if (*pp==',') {
            // 最初の \", \', } を見つける
            pt = pp;
            while(*pt!='\0') {
                while(*pt=='\\') pt += 2;
                if (*pt!='\0') {
                    if (*pt=='\'' || *pt=='\"' || *pt=='}') break;
                    pt++;
                }
            }
            if (*pt=='\0') {
                json = _json_parse_term(json, NULL, NULL, ",");
                return json;
            }

            pp = pt;
            if (*pp!='}') {     // \" or \'
                char ch = '\"';
                if (*pt=='\'') ch = '\''; 
                //
                pt = pp + 1;
                while(*pt!='\0') {
                    while(*pt=='\\') pt += 2;
                    if (*pt!='\0') {
                        if (*pt==ch) break;
                        pt++;
                    }
                }
                if (*pt=='\0') {
                    json = _json_parse_term(json, pp, pt, ",");
                    return json;    
                }

                int len = (int)(pt - pp) - 1 ;
                node = new_json_node();
                node->ldat.key = set_Buffer(pp + 1, len);
                node->ldat.id  = JSON_DATA_NODE;
                node->ldat.lv  = JSON_VALUE_NULL;
                add_tTree_node(json, node);

                pt = pt + 1; 
                while(*pt!=',' && *pt!=':' && *pt!='}' && *pt!='\0') pt++; 
                if (*pt=='\0') {
                    json->state = JBXL_JSON_PARSE_TERM;
                    return json;    
                }
                pp = pt;
            }
        }

        //
        else if (*pp==':') {
            pt = pp + 1;
            while (*pt==' ' && *pt!='\0') pt++;
            if (*pt=='\0') {
                json = _json_parse_term(json, NULL, NULL, ":");
                return json;
            }
            pp = pt;

            if (*pp!='{' && *pp!='[') {
                while(*pt==' ') pt++;
                if (*pt!='\0') while (*pt=='\\') pt += 2;
                //
                if (*pp=='\'' || *pp=='\"') {
                    pt = skip_string_end(pt);
                    if (*pt!='\0') pt++;
                }
                while (*pt!=',' && *pt!='}' && *pt!='\0') pt++;

                if (*pt=='\0') {
                    json = _json_parse_term(json, pp, pt, ":");
                    return json;    
                }
                pt--;
                while (*pt==' ' || *pt==0x0a || *pt==0x0d) pt--;

                int len = (int)(pt - pp) + 1 ;
                Buffer temp = set_Buffer(pp, len);
                json->yngr->ldat.val = pack_Buffer(temp, '\0');
                free_Buffer(&temp);
                //
                if (*pp=='\"' || *pp=='\'') json->yngr->ldat.lv = JSON_VALUE_STR;
                else {
                    const char* val = (const char*)json->yngr->ldat.val.buf;
                    if (!strcasecmp("true", val) || !strcasecmp("false", val)) json->yngr->ldat.lv = JSON_VALUE_BOOL;
                    else  json->yngr->ldat.lv = JSON_VALUE_UNRESOLV;
                }

                pt++;
                while (*pt==' ') pt++;
                pp = pt;
            }
        }

        //
        else if (*pp=='}') {
            if (json->prev!=NULL) {
                json = json->prev;
            }
            pt = pp = pp + 1;
            while (*pt!=',' && *pt!='}' && *pt!='{' && *pt!='\0') pt++;
            if (*pt=='\0' && json->depth>0) {
                json->state = JBXL_JSON_PARSE_TERM;
                return json;    
            }
            pp = pt;
        }

        //
        else {
            pp++;
        }
    }

    if (json->ldat.id==JSON_TEMP_NODE) {
        tJson* temp = json->next;
        del_json_node(&json);
        json = temp;
    }

    json->state = JBXL_JSON_PARSED;
    return json;
}



/**
tJson*  _json_array_parse(tJson* json, int num)

JSONデータの 配列ノードの値（配列データ）を処理する．

@param   json  処理を行う JSON ノードデータ．
@param   num   処理の残り段数．
@return  処理された JSON ノードデータ．
*/
tJson*  _json_array_parse(tJson* json, int num)
{
    char* pp = (char*)(json->ldat.val.buf);
    //
    json = json_array_parse(json, pp, num);
    free_Buffer(&(json->ldat.val));

    return json;
}



/**
tJson*  json_array_parse(tJson* json, char* pp, int num)

JSONデータの 配列ノードの値（配列データ）を処理する．@n
先頭に姉妹ノードがない場合は json にNULLを指定しても可．@n

ただし配列なので，先頭は姉妹ノードだらけ．アンカーノードが必要．@n
その状態で それでもNULLを指定した場合は，この関数外で TOPの JSON_TEMP_NODE を上手く処理すること．

@param   json  JSON ノードデータ．NULLでない場合は，このデータの後に結果が付加される．@n
               NULLでも可．
@param   pp    処理を行うデータ．
@param   num   処理の残り段数．
@return  処理された JSON ノードデータ．
*/
tJson*  json_array_parse(tJson* json, char* pp, int num)
{
    char* pt;
    if (*pp!='[') return json;

    if (json==NULL) {
        json = new_json_node();
        json->ldat.id = JSON_TEMP_NODE;
    }
    else {
        json->ldat.id = JSON_ARRAY_NODE;
    }
    json->ldat.lv = JSON_VALUE_NULL;

    //
    pp++;
    while (*pp!='\0') {
        while (*pp==' ') pp++;
        if (*pp!='\0') while (*pp=='\\') pp += 2;

        //
        if (*pp==']') break;

        //
        else if (*pp=='\'' || *pp=='\"') {
            char ch = *pp;
            pt = pp + 1;
            while (*pt!='\0') {
                while (*pt=='\\') pt += 2;
                if (*pt!='\0') {
                    if (*pt==ch) break;
                    pt++;
                }
            }

            if (*pt!='\0') {
                int len = (int)(pt - pp) + 1 ;
                tJson* node = new_json_node();
                Buffer temp    = set_Buffer(pp, len);
                node->ldat.key = make_Buffer_bystr("ARRAY_VALUE");
                node->ldat.val = pack_Buffer(temp, '\0');
                node->ldat.id  = JSON_ARRAY_VALUE_NODE;
                node->ldat.lv  = JSON_VALUE_STR;
                free_Buffer(&temp);
                add_tTree_node(json, node);
            
                pp = pt + 1;
            }
        }

        //
        else if (*pp=='{') {
            pt = skip_char_pair(pp, '{', '}');

            int len = (int)(pt - pp) + 1 ;
            tJson* node = new_json_node();
            Buffer temp    = set_Buffer(pp, len);
            node->ldat.val = pack_Buffer(temp, '\0');
            node->ldat.id  = JSON_ARRAY_VALUE_NODE;
            node->ldat.lv  = JSON_VALUE_OBJ;
            free_Buffer(&temp);
            
            if (num>0) {
                node = json_parse_prop(node, (char*)node->ldat.val.buf, num-1);
                add_tTree_node(json, node);
                del_json_node(&node);           // ノードを詰める
            }
            else {
                add_tTree_node(json, node);
            }

            pp = pt + 1;
        }

        else if (*pp==',') {
            pp++;
        }

        else {
            pt = skip_chars(pp, ",}]");

            int len = (int)(pt - pp);
            tJson* node = new_json_node();
            Buffer temp    = set_Buffer(pp, len);
            node->ldat.val = pack_Buffer(temp, '\0');
            node->ldat.id  = JSON_ARRAY_VALUE_NODE;
            node->ldat.lv  = JSON_VALUE_UNRESOLV;
            free_Buffer(&temp);
            add_tTree_node(json, node);

            pp = pt;
        }
    }

    return json;
}



/**
tJson*  _json_parse_term(tJson* json, char* st, char* ed, const char* com)

入力データが途中で終了した場合の処理
*/
tJson*  _json_parse_term(tJson* json, char* st, char* ed, const char* com)
{
    if (json==NULL) return NULL;

    json->state = JBXL_JSON_PARSE_TERM;
    if (com!=NULL) {
        json->ldat.val = set_Buffer((void*)com, -1);
        if (st!=NULL && ed!=NULL) {
            int len = (int)(ed - st) + 1;
            cat_b2Buffer(st, &(json->ldat.val), len);
            json->ldat.val.vldsz = (int)strlen((char*)json->ldat.val.buf);
        }
    }    
    return json;
}



/**
tJson*  json_parse_seq(tJson* json, char* pp, int num)

断片化した JSONデータを読み込んで処理する．@n
処理途中の戻り値は色々な場所を指すが，最期までパースできれば，戻り値はトップに戻る．

@code
    tJson* json;
    json = json_parse_seq(NULL, pp1, 99);
    json = json_parse_seq(next, pp2, 99);
    json = json_parse_seq(next, pp3, 99);
@endcode
*/
tJson*  json_parse_seq(tJson* json, char* pp, int num)
{
    if (json==NULL) {
        json = json_parse_prop(NULL, pp, num);
        return json;
    }

    Buffer buf = dup_Buffer(json->ldat.val);
    free_Buffer(&(json->ldat.val));
    cat_s2Buffer(pp, &buf);

    json->state = JBXL_JSON_DEFAULT_STATE;
    json = json_parse_prop(json, (char*)buf.buf, num);
    free_Buffer(&buf);

    return json;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
// 逆 Parser
//

/**
Buffer  json_inverse_parse(tJson* pp, int mode)

ppに格納された tJsonデータを mode に従って，元の書式に戻して Bufferに格納する．json_parse() の逆．@n

@param  pp     tJsonデータが格納されたツリーへのポインタ
@param  mode   @b JSON_ONELINE_FORMAT 改行なしの一行にする．
@param  mode   @b JSON_CRLF_FORMAT    ノードの終わりを CR(0x0d), LF(0x0a)で改行する．
@param  mode   @b JSON_INDENT_FORMAT  先頭にインデント("    ")をつけ，ノードごとに改行 CR LF (0x0d,0x0a)する．
@return 変換したJSONデータを格納した Buffer変数．
*/
Buffer  json_inverse_parse(tJson* pp, int mode)
{
    int cnt;
    Buffer buf;

    buf = init_Buffer();
    if (pp==NULL) return buf;
    if (pp->ldat.id==JSON_ANCHOR_NODE) pp = pp->next;
    if (pp==NULL) return buf;

    cnt = count_tTree(pp);
    buf = make_Buffer(cnt*LSDATA);
    if (buf.buf==NULL) return buf;

    if (pp->ctrl!=TREE_NOSIS_NODE) while (pp->esis!=NULL) pp = pp->esis;

    if (mode==JSON_CRLF_FORMAT) {
        _json_to_Buffer(pp, &buf, "\r\n", "");
    }
    else if (mode==JSON_INDENT_FORMAT) {
        _json_to_Buffer(pp, &buf, "\r\n", "    ");
    }
    else {
        _json_to_Buffer(pp, &buf, "", "");
    }

    return buf;
}



/**
Buffer  json_inverse_parse_opt(tJson* pp, const char* crlf, const char* space)

ppに格納された tJsonデータを指定にしたっがて元の書式に戻して Bufferに格納する．

@param  pp      tJsonデータが格納されたツリーへのポインタ
@param  crlf    json へ戻す時の改行コード
@param  space   インデントを付ける場合の空白やTAB
@return 変換したJSONデータを格納した Buffer変数．
*/
Buffer  json_inverse_parse_opt(tJson* pp, const char* crlf, const char* space)
{
    int cnt;
    Buffer buf;

    buf = init_Buffer();
    if (pp==NULL) return buf;
    if (pp->ldat.id==JSON_ANCHOR_NODE) pp = pp->next;
    if (pp==NULL) return buf;

    cnt = count_tTree(pp);
    buf = make_Buffer(cnt*LSDATA);
    if (buf.buf==NULL) return buf;

    if (pp->ctrl!=TREE_NOSIS_NODE) while (pp->esis!=NULL) pp = pp->esis;
    _json_to_Buffer(pp, &buf, crlf, space);

    return buf;
}



/**
void  _json_to_Buffer(tJson* pp, Buffer* buf, const char* crlf, const char* space)

json_inverse_parse()用の補助関数．
ppに格納された JSONデータを元の書式に戻して Bufferに格納する．

@param  pp     JSONデータが格納されたツリーへのポインタ
@param  buf    変換したJSONデータを格納する Buffer変数．データ格納領域は予め確保しておく.
@param  crlf   JSONへ戻す時の改行コード
@param  space  インデントを付ける場合の空白やTAB
*/
void  _json_to_Buffer(tJson* pp, Buffer* buf, const char* crlf, const char* space)
{
    if (pp==NULL) return;
    if (buf==NULL || buf->buf==NULL) return;
    //
    if (pp->ldat.id==JSON_ANCHOR_NODE) pp = pp->next;

    if (pp!=NULL) {
        if (pp->ctrl!=TREE_NOSIS_NODE) while(pp->esis!=NULL) pp = pp->esis; // 長女から逆パースを開始．
        do {
            int i;
            if (space[0]!='\0') for(i=0; i<pp->depth; i++) cat_s2Buffer(space, buf);
            //
            if (pp->ldat.id==JSON_BRACKET_NODE) {
                cat_s2Buffer("{", buf);
                if (pp->next!=NULL) {
                    if (crlf[0]!='\0') cat_s2Buffer(crlf, buf);
                    _json_to_Buffer(pp->next, buf, crlf, space);
                    if (space[0]!='\0') for(i=0; i<pp->depth; i++) cat_s2Buffer(space, buf);
                }
                //if (space[0]!='\0') for(i=0; i<pp->depth; i++) cat_s2Buffer(space, buf);
                cat_s2Buffer("}", buf); 
            }
            //
            else if (pp->ldat.id==JSON_DATA_NODE) {
                cat_s2Buffer("\"", buf);
                cat_s2Buffer(pp->ldat.key.buf, buf);
                cat_s2Buffer("\"", buf);
                //
                if (pp->ldat.lv==JSON_VALUE_OBJ) {
                    cat_s2Buffer(": {", buf);
                    if (pp->next!=NULL) {
                        if (crlf[0]!='\0') cat_s2Buffer(crlf, buf);
                        _json_to_Buffer(pp->next, buf, crlf, space);
                        if (space[0]!='\0') for(i=0; i<pp->depth; i++) cat_s2Buffer(space, buf);
                    }
                    //if (space[0]!='\0') for(i=0; i<pp->depth; i++) cat_s2Buffer(space, buf);
                    cat_s2Buffer("}", buf); 
                }
                else {
                    if (pp->ldat.lv==JSON_VALUE_NULL) {
                        cat_s2Buffer(": null", buf);
                    }
                    else  {
                        cat_s2Buffer(": ", buf);
                        cat_s2Buffer(pp->ldat.val.buf, buf);
                    }
                }
            }
            //
            // array
            else if (pp->ldat.id==JSON_ARRAY_NODE) {
                cat_s2Buffer("\"", buf);
                cat_s2Buffer(pp->ldat.key.buf, buf);
                cat_s2Buffer("\": [", buf);
                if (pp->next!=NULL) {
                    if (crlf[0]!='\0') cat_s2Buffer(crlf, buf);
                    _json_to_Buffer(pp->next, buf, crlf, space);
                    if (space[0]!='\0') for(i=0; i<pp->depth; i++) cat_s2Buffer(space, buf);
                }
                //if (space[0]!='\0') for(i=0; i<pp->depth; i++) cat_s2Buffer(space, buf);
                cat_s2Buffer("]", buf); 
            }

            else if (pp->ldat.id==JSON_ARRAY_VALUE_NODE) {
                if (pp->ldat.lv==JSON_VALUE_OBJ) {
                    if (pp->ldat.val.buf==NULL) {
                        cat_s2Buffer("{", buf);
                        if (pp->next!=NULL) {
                            if (crlf[0]!='\0') cat_s2Buffer(crlf, buf);
                            _json_to_Buffer(pp->next, buf, crlf, space);
                            if (space[0]!='\0') for(i=0; i<pp->depth; i++) cat_s2Buffer(space, buf);
                        }
                        //if (space[0]!='\0') for(i=0; i<pp->depth; i++) cat_s2Buffer(space, buf);
                        cat_s2Buffer("}", buf); 
                    }
                    else {
                        cat_Buffer(&pp->ldat.val, buf);
                    }
                }
                else {
                    if (pp->ldat.lv==JSON_VALUE_NULL) {
                        cat_s2Buffer("null", buf);
                    }
                    else  {
                        cat_s2Buffer(pp->ldat.val.buf, buf);
                    }
                }
            }

            if (pp->ctrl!=TREE_NOSIS_NODE && pp->ysis!=NULL && 
                pp->prev!=NULL && pp->prev->ldat.id!=JSON_ANCHOR_NODE) cat_s2Buffer(",", buf);
            if (crlf[0]!='\0') cat_s2Buffer(crlf, buf);

            if (pp->ctrl!=TREE_NOSIS_NODE) pp = pp->ysis;
            else                           pp = NULL;
        } while(pp!=NULL);
    }

    return;
}



/**
void  print_json(FILE* fp, tJson* json, int mode)

tJsonデータを modeに従って，fp に出力する.

@param  fp     出力先のファイルポインタ
@param  json   tJsonデータが格納されたツリーへのポインタ
@param  mode   @b JSON_ONELINE_FORMAT 改行なしの一行にする．
@param  mode   @b JSON_CRLF_FORMAT    ノードの終わりを CR LF(0x0d, 0x0a) で改行する．
@param  mode   @b JSON_INDENT_FORMAT  先頭にインデント(TAB 0x09)をつけ，ノードごとに改行 CR LF (0x0d, 0x0a)する．
*/
void  print_json(FILE* fp, tJson* json, int mode)
{
    Buffer buf = json_inverse_parse(json, mode);
    if (buf.buf!=NULL) {
        fprintf(fp, "%s", buf.buf);
        free_Buffer(&buf);
    }

    return;
}



/**
void  print_json_opt(FILE* fp, tJson* json, const char* crlf, const char* space)

tJsonデータを fp に出力する.

@param  fp      出力先のファイルポインタ
@param  json    tJsonデータが格納されたツリーへのポインタ
@param  crlf    json へ戻す時の改行コード
@param  space   インデントを付ける場合の空白やTAB
*/
void  print_json_opt(FILE* fp, tJson* json, const char* crlf, const char* space)
{
    Buffer buf = json_inverse_parse_opt(json, crlf, space);
    if (buf.buf!=NULL) {
        fprintf(fp, "%s", buf.buf);
        free_Buffer(&buf);
    }

    return;
}




/////////////////////////////////////////////////////////////////////////////////////////////
// Tools

/**
tJson*  json_parse_file(const char* fn, int num)

ファイルから読み込んでパースする．

@param  fn  読み込むファイル名
@param  num  0 配列を処理しない．高速．@n
             1 配列を処するが，配列の中の JSONデータは処理しない．@n
             2 配列の中の { } を処理する．@n
             N 配列処理 + 再帰処理 の段数．
@return JSONデータを格納した tJsonのアンカーへのポインタ 
*/
tJson*  json_parse_file(const char* fn, int num)
{
    tJson* json = NULL;
    Buffer buf;

    buf = read_Buffer_file(fn);
    if (buf.buf!=NULL) {
        json = json_parse((char*)(buf.buf), num);
        free_Buffer(&buf);
    }

    return json;
}



/**
void  json_set_str_val(tJson* json, char* str)

json ノードに文字列の属性値を設定する．
*/
void  json_set_str_val(tJson* json, char* str)
{
    if (json==NULL || str==NULL) return;

    copy_s2Buffer(str, &(json->ldat.val));
    json->ldat.lv = JSON_VALUE_STR;

    return;
}



/**
void  json_copy_val(tJson* f_json, tJson* t_json)

JSON ノードの f_json から t_json へ属性値をコピーする．
*/
void  json_copy_val(tJson* f_json, tJson* t_json)
{
    if (f_json==NULL || t_json==NULL) return;

    t_json->ldat.lv = f_json->ldat.lv;
    copy_Buffer(&(f_json->ldat.val), &(t_json->ldat.val));

    return;
}



/**
void  json_copy_data(tJson* f_json, tJson* t_json)

JSON ノードの f_json から t_json へ属性名と属性値をコピーする．
*/
void  json_copy_data(tJson* f_json, tJson* t_json)
{
    if (f_json==NULL || t_json==NULL) return;

    t_json->ldat.id = f_json->ldat.id;
    t_json->ldat.lv = f_json->ldat.lv;
    copy_Buffer(&(f_json->ldat.key), &(t_json->ldat.key));
    copy_Buffer(&(f_json->ldat.val), &(t_json->ldat.val));

    return;
}



/**
void  insert_json_nodes(tJson* parent, tJson* child)

json ツリー parent に json ツリー child のノードを挿入する．

parent が { の場合
   child の { は破棄されて，それ以下のノードが parent の子（姉妹）として結合される．
   child は破壊される．
parent が [ の場合
   child はそのまま配列の要素として追加される．
parent がそれ外の場合
   何の処理も行われない．
*/
void  insert_json_nodes(tJson* parent, tJson* child)
{
    if (parent==NULL) return;
    if (child ==NULL) return;
    if (parent->ldat.id!=JSON_BRACKET_NODE && parent->ldat.id!=JSON_ARRAY_NODE) return;
    if (child->ldat.id !=JSON_BRACKET_NODE) return;
    
    if (parent->ldat.id==JSON_BRACKET_NODE) {
        tJson* cp = child->next;
        while (cp!=NULL) {
            add_tTree(parent, cp);
            cp = cp->ysis;
        }
        free_tList_data(&child->ldat);
        free(child);
    }

    if (parent->ldat.id==JSON_ARRAY_NODE) {
        add_tTree(parent, child);
    }

    return;
}



/**
tJson*   join_json(tJson* parent, tJson** child)

parent の子として child そのものを結合する．
child が ANCHORノードの場合，ANCHORノードは削除され，*child は書き換えられる．

@param       parent  結合対象の JSONノード
@param[in]   child   結合するJSONノード
@param[out]  child   child のTOPがANCHORの場合，ANCHORを削除したjsonツリーのTOP.
@return      結合結果の JSON Treeの TOP
*/
tJson*   join_json(tJson* parent, tJson** child)
{
    if (*child==NULL) return parent;
    if (parent==NULL) return *child;            // この場合 ANCHOR はそのまま

    if ((*child)->ldat.id==JSON_ANCHOR_NODE || (*child)->ldat.id==JSON_TEMP_NODE) {  // 子として繋げる場合，ANCHOR, TEMP_NODE は削除
        tJson* jtmp = (*child)->next;
        del_json_node(child);                       // ANCHOR, TEMP_NODE を削除してつめる．
        *child = jtmp;
    }

    add_tTree(parent, *child); 

    return parent;
}



/////////////////////////////////////////////////////////////////////////////////////////////
// Search

/**
tJson*  search_top_bracket_json(tJson* pp, int nn)

ツリーが複数のルート(TOP)を持つ場合（state==JBXL_JSON_MULTI_ROOT），指定された順番のTOPへのポインタを返す．

@param   nn 何番目のTOPを返すか指定する．nn<=0 は nn==1 とみなす．
@return  見つかったノードへのポインタ．見つからない場合は，NULL
*/
tJson*  search_top_bracket_json(tJson* pp, int nn)
{
    if (pp==NULL) return NULL;
    if (nn<=0) nn = 1;

    while (pp->prev!=NULL) pp = pp->prev;

    if (pp->ldat.id==JSON_ANCHOR_NODE) {
        pp = pp->next;
    }
    if (pp->ldat.id==JSON_BRACKET_NODE) {
        while(pp->esis!=NULL) pp = pp->esis;
    }
    else {
        return NULL;
    }

    int i = 1;
    while (i<nn && pp!=NULL) {
        pp = pp->ysis;
        i++;
    }

    return pp;
}



/**
tJson*  search_key_json(tJson* pp, char* key, int needval, int nn)

pp が指すノード以下で，名前（属性名）が key である nn番目のノードへのポインタを返す．@n
needval が TRUE の場合は，値（属性値）を持っている場合のみカウントする．
pp の姉妹ノードは探索しない．

@param   pp   探索を開始するノード．ppがオブジェクト要素（JSON_VALUE_OBJ）なら，次のノードから探索する．
@param   key  探索するノード名．
@param   needval  TRUE の場合，ノードが値（属性値）を持っていない場合は無視する．"", '' の場合も無視する．
@param   nn   一致するノード内，何番目を返すか指定する．nn<=0 は nn==1 とみなす．
@return  見つかったノードへのポインタ．見つからない場合は，NULL
*/
tJson*  search_key_json(tJson* pp, char* key, int needval, int nn)
{
    if (pp==NULL || key==NULL) return NULL;
    if (nn<=0) nn = 1;

    // 開始ノードのチェック
    if (!(pp->ldat.lv==JSON_VALUE_OBJ && needval)){
        nn = _json_check_node_bykey(pp, key, needval, nn);
    }
    // 子ノード
    if (nn>0 && pp->next!=NULL) {
        pp = _search_key_json(pp->next, key, needval, &nn);
    }
    else return NULL;

    return pp; 
}



/**
tJson*   search_sister_json(tJson* pp, int nn)

nn個先の sister ノードを返す．正数の場合は younger sister，負数の場合は elder sister を検索する．

@param   pp   探索を開始するノード．
@param   nn   何個先の sister かを指定する．正数の場合は younger sister，負数の場合は elder sister．
@return  sister ノードへのポインタ．存在しない場合は NULL
*/

tJson*   search_sister_json(tJson* pp, int nn)
{
    if (pp==NULL) return NULL;

    if (nn>0) {
        int i = 0;
        while (pp!=NULL && i<nn) {
            pp = pp->ysis;
            i++;
        }
    }
    else if (nn<0) {
        nn = -nn;
        int i = 0;
        while (pp!=NULL && i<nn) {
            pp = pp->esis;
            i++;
        }
    }
    return pp;
}



/**
tJson*  search_key_child_json(tJson* pp, char* key, int needval)

pp が指すノードの子（の姉妹）で，名前（属性名）が key であるノードへのポインタを返す．
探索対象は探索を開始した子の姉妹ノードのみ．
needval が TRUE の場合は，値（属性値）を持っている場合のみカウントする．

@param   pp   探索を開始するノード．
@param   key  探索するノード名．
@param   needval  TRUE の場合，ノードが値（属性値）を持っていない場合は無視する．"", '' の場合も無視する．
@return  見つかったノードへのポインタ．見つからない場合は NULL
*/
tJson*  search_key_child_json(tJson* pp, char* key, int needval)
{
    if (pp!=NULL && pp->ldat.id==JSON_ANCHOR_NODE) pp = pp->next;
    if (pp==NULL || pp->next==NULL) return NULL;

    tJson* json = search_key_sister_json(pp->next, key, needval);

    return json;
}



/**
tJson*  search_key_sister_json(tJson* pp, char* key, int needval)

pp が指すノードの姉妹で，名前（属性名）が key であるノードへのポインタを返す．
探索対象は探索を開始した姉妹ノードのみ．
needval が TRUE の場合は，値（属性値）を持っている場合のみカウントする．

@param   pp   探索を開始するノード．
@param   key  探索するノード名．
@param   needval  TRUE の場合，ノードが値（属性値）を持っていない場合は無視する．"", '' の場合も無視する．
@return  見つかったノードへのポインタ．見つからない場合は，NULL
*/
tJson*  search_key_sister_json(tJson* pp, char* key, int needval)
{
    if (pp==NULL || key==NULL) return NULL;
    while(pp->esis!=NULL) pp = pp->esis;

    while(pp!=NULL) {
        int nn = _json_check_node_bykey(pp, key, needval, 1);
        if (nn==0) return pp;
        pp = pp->ysis;
    }
    
    return NULL; 
}



/**
tJson*  search_key_json_obj(tJson* pp, char* key, int nn)

pp が指すノード以下で，名前（属性名）が key である nn番目のオブジェクトノードへのポインタを返す．
pp の姉妹ノードは探索しない．
search_key_json() よりは少し早い．たぶん．

@param   pp   探索を開始するノード．
@param   key  探索するノード名．
@param   nn   一致するノード内，何番目を返すか指定する．nn<=0 は nn==1 とみなす．
@return  見つかったオブジェクトノードへのポインタ．見つからない場合は，NULL
*/
tJson*  search_key_json_obj(tJson* pp, char* key, int nn)
{
    if (pp==NULL || key==NULL) return NULL;
    if (nn<=0) nn = 1;

    // 開始ノードのチェック
    if (pp->ldat.lv==JSON_VALUE_OBJ) {
        nn = _json_check_node_bykey(pp, key, FALSE, nn);
    }
    if (nn==0) return pp;

    // 子ノード
    if (pp->next!=NULL) {
        pp = _search_key_json_obj(pp->next, key, &nn);
    }
    else return NULL;

    return pp; 
}



/**
tJson*  search_double_key_json(tJson* pp, char* key1, char* key2, int needval)

key1 -> key2 の親子関係を持つ，key2ノードのポインタを返す．

@param   pp    探索を開始するノード．
@param   key1  探索するノード名．
@param   key2  探索するノード名．
@param   needval  TRUEの時，ky2 ノードが値（属性値）を持っていない場合は無視する．"", '' の場合も無視する．
@return  見つかったノードへのポインタ．見つからない場合は，NULL
*/
tJson*  search_double_key_json(tJson* pp, char* key1, char* key2, int needval)
{
    if (pp==NULL || key1==NULL || key2==NULL) return NULL;

    pp = search_key_json_obj(pp, key1, 1);
    if (pp==NULL) return NULL;

    if (pp->next!=NULL) {
        pp = search_key_json(pp, key2, needval, 1);
    }
    else return NULL;

    return pp; 
}



/*
tJson*  _search_key_json(tJson* pp, char* key, int needval, int* nn)

search_key_json() の補助関数
pp が指すノード以下で，名前（属性名）が key である nn番目のノードへのポインタを返す．@n
needval が TRUE の場合は，値（属性値）を持っている場合のみカウントする．
search_key_json() との違いは pp の姉妹ノードも探索することである．

@param   pp   探索を開始するノード．ppがオブジェクト要素（JSON_VALUE_OBJ）なら，次のノードから探索する．
@param   key  探索するノード名．
@param   needval  TRUE の場合，ノードが値（属性値）を持っていない場合は無視する．"", '' の場合も無視する．
@param   nn   一致するノード内，何番目を返すか指定する．nn<=0 は nn==1 とみなす．
@return  見つかったノードへのポインタ．見つからない場合は，NULL
*/
tJson*  _search_key_json(tJson* pp, char* key, int needval, int* nn)
{
    while(pp->esis!=NULL) pp = pp->esis;
    tJson* esis = pp;

    while(pp!=NULL) {
        *nn = _json_check_node_bykey(pp, key, needval, *nn);
        if ((*nn)<=0) return pp;
        pp = pp->ysis;
    }

    // 子ノード
    pp = esis;
    while(pp!=NULL) {
        if (pp->next!=NULL) {
            tJson* json = _search_key_json(pp->next, key, needval, nn);
            if (json!=NULL) return json;
        }
        pp = pp->ysis;
    }
    
    return NULL; 
}


 
/*
tJson*  _search_key_json_obj(tJson* pp, char* key, int* nn)

search_key_json_obj() の補助関数
pp が指すノード以下で，名前（属性名）が key である nn番目のオブジェクトノードへのポインタを返す．
pp の姉妹ノードは探索しない．
_search_key_json() よりは少し早い．たぶん．
search_key_json_obj() との違いは pp の姉妹ノードも探索することである．

@param   pp   探索を開始するノード．
@param   key  探索するノード名．
@param   nn   一致するノード内，何番目を返すか指定する．nn<=0 は nn==1 とみなす．
@return  見つかったオブジェクトノードへのポインタ．見つからない場合は，NULL
*/
tJson*  _search_key_json_obj(tJson* pp, char* key, int* nn)
{
    if (pp==NULL) return NULL;
    while(pp->esis!=NULL) pp = pp->esis;
    tJson* esis = pp;

    while(pp!=NULL) {
        if (pp->ldat.lv==JSON_VALUE_OBJ) {
            *nn = _json_check_node_bykey(pp, key, FALSE, *nn);
        }
        if ((*nn)<=0) return pp;
        pp = pp->ysis;
    }

    // 子ノード
    pp = esis;
    while(pp!=NULL) {
        if (pp->next!=NULL) {
            tJson* json = _search_key_json_obj(pp->next, key, nn);
            if (json!=NULL) return json;
        }
        pp = pp->ysis;
    }
    
    return NULL; 
}



/**
int  _json_check_node_bykey(tJson* pp, char* key, int needval, int nn)

pp が指すノードの名前（属性名）が key である場合，nnを 1減算して返す．
needval が TRUE の場合は，値（属性値）を持っている場合のみ減算する．
*/
int  _json_check_node_bykey(tJson* pp, char* key, int needval, int nn)
{
    if (pp->ldat.key.buf!=NULL) {
        if (!strcmp(key, (char*)pp->ldat.key.buf)) {
            if (needval) {
                unsigned char* pm = pp->ldat.val.buf;
                if (pm!=NULL) { 
                    if (!((pm[0]=='\'' && pm[1]=='\'') || (pm[0]=='"' && pm[1]=='"')) && pm[0]!='\0') nn--; 
                }
            }
            else {
                nn--;
            }
        }
    }
    return nn;
}



/**
tList*   search_all_node_strval_json(tJson* pp, char* name, char* val)

指定した条件に会う全てのノードへのポインタを，リストに格納して返す．@n
検索条件は，属性名 name, 属性値 val ("name": "val") を持つノード．

@param   pp 検索する JSONデータ．
@param   name 属性名
@param   val  属性値
@return  検索結果を ldat.altp に格納した リスト．
*/
tList*   search_all_node_strval_json(tJson* pp, char* name, char* val)
{
    if (pp!=NULL && pp->ldat.id==JSON_ANCHOR_NODE) pp = pp->next;
    if (pp==NULL) return NULL;

    tList* list = new_json_node();
    _search_all_node_strval_json(list, pp, name, val);

    return list;
}



tList*   _search_all_node_strval_json(tList* list, tJson* pp, char* name, char* val)
{
    while (pp->esis!=NULL) pp = pp->esis;
    do {
        if (pp->ldat.id==JSON_DATA_NODE) {
            if (!strncasecmp(name, (char*)pp->ldat.key.buf, strlen(name))) {
                if (pp->ldat.lv==JSON_VALUE_STR) {
                    if (!strncasecmp(val, (char*)(pp->ldat.val.buf+1), strlen(val))) {
                        list->altp = pp;
                        tList* temp = new_tList_node();
                        temp->ldat.id = list->ldat.id + 1;
                        add_tList_node(list, temp);
                        list = temp;
                    }
                }
            }
            else if (pp->ldat.id==JSON_VALUE_OBJ) {
                if (pp->next!=NULL) {
                    list = _search_all_node_strval_json(list, pp->next, name, val);
                }
            }
        }
        else if (pp->ldat.id==JSON_BRACKET_NODE || pp->ldat.id==JSON_ARRAY_NODE) {
            if (pp->next!=NULL) {
                list = _search_all_node_strval_json(list, pp->next, name, val);
            }
        }
        pp = pp->ysis;
    } while (pp!=NULL);

    return list;
}




Buffer  get_json_val(tJson* json)
{
    Buffer val = init_Buffer();

    if (json!=NULL) {
        char* pp = (char*)json->ldat.val.buf;
        if ((*pp=='\"') || (*pp=='\'')) {
            val = set_Buffer(pp+1, strlen(pp)-1);
            val.buf[val.vldsz-1] = '\0';
            val.vldsz = (int)strlen((const char*)val.buf);
        }
        else {
            val = set_Buffer(pp, strlen(pp));
        }
    }
    return val;
}



/**
Buffer  get_key_json_val(tJson* pp, char* key, int nn)

pp が指すノード以下で，名前（属性名）が key である nn番目のノードへの属性値を返す．@n
属性値が文字列の場合，先頭と最後の " または ' は削除する．
pp の姉妹ノードは探索しない．

@param   pp   探索を開始するノード．ppがオブジェクト要素（JSON_VALUE_OBJ）なら，次のノードから探索する．
@param   key  探索するノード名．
@param   nn   一致するノード内，何番目を返すか指定する．nn<=0 は nn==1 とみなす．
@return  見つかったノードの属性値を格納した Buffer 変数．
*/
Buffer  get_key_json_val(tJson* pp, char* key, int nn)
{
    tJson* json = search_key_json(pp, key, TRUE, nn);
    Buffer val  = get_json_val(json);

    return val;
}



/**
Buffer  get_key_sister_json_val(tJson* pp, char* key)

pp が指すノードの姉妹で，名前（属性名）が key である nn番目のノードの属性値を返す．
探索対象は探索を開始した姉妹ノードのみ．
属性値が文字列の場合，先頭と最後の " または ' は削除する．

@param   pp   探索を開始するノード．
@param   key  探索するノード名．
@param   needval  ノードが値（属性値）を持っていない場合は無視する．"", '' の場合も無視する．
@return  見つかったノードの属性値を格納した Buffer 変数．
*/
Buffer  get_key_sister_json_val(tJson* pp, char* key)
{
    tJson* json = search_key_sister_json(pp, key, TRUE);
    Buffer val  = get_json_val(json);

    return val;
}



/**
Buffer  get_double_key_json_val(tJson* pp, char* key1, char* key2)

key1 -> key2 の親子関係を持つ，key2ノードの属性値を返す．
属性値が文字列の場合，先頭と最後の " または ' は削除する．

@param   pp    探索を開始するノード．
@param   key1  探索するノード名．
@param   key2  探索するノード名．
@return  見つかったノードの属性値を格納した Buffer 変数．
*/
Buffer  get_double_key_json_val(tJson* pp, char* key1, char* key2)
{
    tJson* json = search_double_key_json(pp, key1, key2, TRUE);
    Buffer val  = get_json_val(json);

    return val;
}




/**
Buffer  get_Buffer_from_json(tJson* json)

JSON データのノード値の文字列をを含む Buffer変数を返す．
"" または '' で囲まれている場合は，その内部のデータ（"", ''の中味）の返す．
*/
Buffer  get_Buffer_from_json(tJson* json)
{
    Buffer buf = init_Buffer();

    if (json==NULL) return buf;

    char* pp = (char*)json->ldat.val.buf;

    if (pp!=NULL && json->ldat.lv!=JSON_VALUE_ARRAY) {
        if (*pp=='\"' || *pp=='\'') {
            char* pt = (char*)&(json->ldat.val.buf[json->ldat.val.vldsz-1]);
            if (*pp==*pt) {
                pp++;
                char bkup = *pt;
                *pt = '\0';
                buf = make_Buffer_str(pp);
                *pt = bkup;
            }
        }
        else {
            buf = make_Buffer_str(pp);
        }
    }

    return buf;
}



/**
char*  get_string_from_json(tJson* json)

JSON データのノード値の文字列を返す．
"" または '' で囲まれている場合は，その内部のデータ（"", ''の中味）の返す．
要 free
*/
char*  get_string_from_json(tJson* json)
{
    if (json==NULL) return NULL;

    char* str = NULL;
    char* pp  = (char*)json->ldat.val.buf;

    if (pp!=NULL) {
        if (json->ldat.lv!=JSON_VALUE_ARRAY) {
            if (*pp=='\"' || *pp=='\'') {
                char* pt = (char*)&(json->ldat.val.buf[json->ldat.val.vldsz-1]);
                if (*pp==*pt) {
                    pp++;
                    char bkup = *pt;
                    *pt = '\0';
                    str = dup_str(pp);
                    *pt = bkup;
                }
            }
        }
        if (str==NULL) str = dup_str(pp);
    }


    return str;
}

