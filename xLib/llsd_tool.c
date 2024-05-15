/**
@brief   LLSD用ライブラリヘッダ（サブセット版）
@file    llsd_tool.c
@author  Fumi.Iseki (C)
@sa https://wiki.secondlife.com/wiki/LLSD
@sa https://polaris.star-dust.jp/pukiwiki/?LLSD
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif

#include "llsd_tool.h"
#include <math.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary LLSD
//

/**
@brief ストリングマーカーの要素を返す．ポインタは次に進む．

@param *ptr LLSD_MAKER_STR ('s') の次のデータを指すポインタ
*/
Buffer  llsd_bin_get_str(uByte** ptr)
{
    Buffer buf = init_Buffer();

    if (ptr==NULL) return buf;

    int size = ntohl(*(int*)(*ptr));
    (*ptr) += 4;
    
    char* str = (char*)malloc(size + 1);
    if (str!=NULL) {
        memcpy(str, *ptr, size);
        str[size] = '\0';
        buf = make_Buffer_bystr(str);
        free(str);
    }
    (*ptr) += size;

    return buf;
}


/**
@brief 整数マーカーの要素を返す．ポインタは次に進む．

@param *ptr LLSD_MAKER_INT ('i') の次のデータを指すポインタ
*/
int  llsd_bin_get_int(uByte** ptr)
{
    if (ptr==NULL) return 0;

    int value = ntohl(*(int*)(*ptr));
    (*ptr) += 4;

    return value;
}


/**
@brief デイトマーカーの要素を返す．ポインタは次に進む．

@param *ptr LLSD_MAKER_DATE ('d') の次のデータを指すポインタ
*/
unsigned long long int llsd_bin_get_date(uByte** ptr)
{
    if (ptr==NULL) return 0ULL;

    unsigned long long int value = ntohull(*(unsigned long long int*)(*ptr));
    (*ptr) += 8;

    return value;
}


/**
@brief 実数マーカーの要素を返す．ポインタは次に進む．

@param *ptr LLSD_MAKER_REAL ('r') の次のデータを指すポインタ
*/
double  llsd_bin_get_real(uByte** ptr)
{
    if (ptr==NULL) return 0.0;

#ifdef WIN32
    long long int tmp;//  __attribute__((may_alias));
#else
    long long int tmp  __attribute__((may_alias));
#endif

    tmp = ntohull(*(long long int*)(*ptr));

    double* valuep = (double*)&tmp;
    double  value  = *valuep;
    if (isnan(value)) value = 0.0;

    (*ptr) += 8;

    return value;
}


/**
@brief UUIDマーカーの要素を返す．ポインタは次に進む．

@param *ptr LLSD_MAKER_UUID ('u') の次のデータを指すポインタ
*/
Buffer  llsd_bin_get_uuid(uByte** ptr)
{
    Buffer buf = init_Buffer();

    if (ptr==NULL) return buf;

    char* str = (char*)malloc(16);
    if (str!=NULL) {
        memcpy(str, *ptr, 16);
        buf = set_Buffer(str, 16);
        free(str);
    }
    (*ptr) += 16;

    return buf;
}


/**
@brief バイナリマーカーの要素を返す．ポインタは次に進む．

@param *ptr LLSD_MAKER_BIN ('b') の次のデータを指すポインタ
*/
Buffer  llsd_bin_get_bin(uByte** ptr)
{
    Buffer buf = init_Buffer();

    if (ptr==NULL) return buf;

    int size = ntohl(*(int*)(*ptr));
    (*ptr) += 4;
    
    uByte* bin = (uByte*)malloc(size);
    if (bin!=NULL) {
        memcpy(bin, *ptr, size);
        buf = set_Buffer(bin, size);
        free(bin);
    }
    (*ptr) += size;

    return buf;
}


/**
int  llsd_bin_get_length(uByte* ptr, int sz)

データの長さとフォーマットを検証する．

@param   ptr  　データへのポインタ
@param   sz     データのファイルサイズ
@retval  解析したヘッダサイズ．
*/
int  llsd_bin_get_length(uByte* ptr, int sz)
{
    if (ptr==NULL) return 0;

    int cc = 0;
    Buffer stack = make_Buffer(LSDATA);
    uByte* buf   = ptr;

    while (buf<ptr+sz) {
        //
        if      (*buf==LLSD_MAKER_UNDEF) buf++;
        else if (*buf==LLSD_MAKER_TRUE)  buf++;
        else if (*buf==LLSD_MAKER_FALSE) buf++;
        else if (*buf==LLSD_MAKER_INT)   buf += 5;
        else if (*buf==LLSD_MAKER_REAL)  buf += 9;
        else if (*buf==LLSD_MAKER_UUID)  buf += 17;
        else if (*buf==LLSD_MAKER_BIN)   buf += ntohl(*(int*)(buf + 1)) + 5;
        else if (*buf==LLSD_MAKER_STR)   buf += ntohl(*(int*)(buf + 1)) + 5;
        else if (*buf==LLSD_MAKER_URI)   buf += ntohl(*(int*)(buf + 1)) + 5;
        else if (*buf==LLSD_MAKER_KEY)   buf += ntohl(*(int*)(buf + 1)) + 5;
        else if (*buf==LLSD_MAKER_DATE)  buf += 9;
        else if (*buf!=LLSD_MAKER_MAP && *buf!=LLSD_MAKER_ARRAY &&
                 *buf!=LLSD_MAKER_MAP_END && *buf!=LLSD_MAKER_ARRAY_END) {
            if (ptr==buf) return 0;
            PRINT_MESG("WARNING: llsd_bin_get_length: unknown marker %c: %04x\n", *buf, *buf);
            break;
        }

        if (*buf==LLSD_MAKER_MAP) {
            push_char_ringStack(&stack, LLSD_MAKER_MAP_END);
            cc++;
            buf += 5;
        }
        else if (*buf==LLSD_MAKER_ARRAY) {
            push_char_ringStack(&stack, LLSD_MAKER_ARRAY_END);
            cc++;
            buf += 5;
        }
        else if (*buf==LLSD_MAKER_MAP_END || *buf==LLSD_MAKER_ARRAY_END) {
            unsigned char marker = (unsigned char)pop_char_ringStack(&stack);
            if (marker!=*buf) PRINT_MESG("WARNING: llsd_bin_get_length: missmatch stack data of [ or {\n");
            cc--;
            buf++;
        }

        if (cc==0) break;
    }   

    free_Buffer(&stack);

    return (int)(buf - ptr);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary LLSD Parser
//

/**
@brief  LLSDヘッダ のバイナリデータを tXMLのデータに格納する．ボディについては別途処理する．

@param  ptr  LLSDのバイナリデータへのポインタ
@param  sz   データのサイズ

@return XMLデータを格納した tXMLのアンカーへのポインタ．
@return エラーの場合，next以下のノードにはエラーを起こす直前までの内容が保存される
@retval 0以下 エラーを起こした場合 stateに JBXL_XML_PARSED以外の値（負数）が入る．
*/
tXML*  llsd_bin_parse(uByte* ptr, int sz)
{
    tXML* xml;
    tXML* node;

    xml = new_xml_node();                   // アンカー
    xml->ldat.id = XML_ANCHOR_NODE;

    // パース
    node = llsd_bin_main_parse(xml, ptr, sz);
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
@brief  LLSD のバイナリデータをパースするメイン関数．

@param  xml  XMLデータを格納するツリー構造体の先頭へのポインタ．
@param  ptr  LLSDのバイナリデータへのポインタ
@param  sz   データのサイズ

@return 最後に処理したノードへのポインタ．ppがNULLの場合は xml, xmlが NULLの場合は NULLが返る．
@retval 負数 @b state エラーの場合は stateに 負数の値が入る．
*/
tXML*  llsd_bin_main_parse(tXML* xml, uByte* ptr, int sz)
{
    if (ptr==NULL) return xml;
    if (xml==NULL) return NULL;

    tXML* snode = xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 0, "llsd", NULL, NULL, 0);

    uByte* end = ptr + sz;
    while (ptr<end  && !(xml==snode && *ptr!=LLSD_MAKER_MAP && *ptr!=LLSD_MAKER_ARRAY)) {
        //
        if (*ptr==LLSD_MAKER_MAP) {
            ptr++;
            int num = llsd_bin_get_map(&ptr);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, num, "map", NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_OPENED;
        }
        //
        else if (*ptr==LLSD_MAKER_ARRAY) {
            ptr++;
            int num = llsd_bin_get_map(&ptr);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, num, "array", NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_OPENED;
        }
        //
        else if (*ptr==LLSD_MAKER_MAP_END) {
            ptr++;
            if (xml->state==JBXL_XML_NODE_OPENED) {
                xml->state = JBXL_XML_NODE_CLOSED;
                xml = xml->prev;
            }   
        }
        //
        else if (*ptr==LLSD_MAKER_ARRAY_END) {
            ptr++;
            if (xml->state==JBXL_XML_NODE_OPENED) {
                xml->state = JBXL_XML_NODE_CLOSED;
                xml = xml->prev;
            }   
        }
        //
        else if (*ptr==LLSD_MAKER_KEY) {
            ptr++;
            Buffer key = llsd_bin_get_key(&ptr);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "key", NULL, NULL, 0);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, (char*)key.buf, NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            free_Buffer(&key);
        }
        //
        else if (*ptr==LLSD_MAKER_STR) {
            ptr++;
            Buffer str = llsd_bin_get_str(&ptr);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "string", NULL, NULL, 0);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, (char*)str.buf, NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            free_Buffer(&str);
        }
        //
        else if (*ptr==LLSD_MAKER_UUID) {
            ptr++;
            Buffer uuid = llsd_bin_get_uri(&ptr);
            char*  guid = (char*)uuid2guid(uuid.buf);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "uuid", NULL, NULL, 0);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, guid, NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            free(guid);
            free_Buffer(&uuid);
        }
        //
        else if (*ptr==LLSD_MAKER_URI) {
            ptr++;
            Buffer str = llsd_bin_get_uri(&ptr);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "uri", NULL, NULL, 0);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, (char*)str.buf, NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            free_Buffer(&str);
        }
        //
        else if (*ptr==LLSD_MAKER_INT) {
            ptr++;
            int val = llsd_bin_get_int(&ptr);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "integer", NULL, NULL, 0);
            char* str = itostr_ts(val);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, str, NULL, NULL, 0);
            freeNull(str);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
        }
        //
        else if (*ptr==LLSD_MAKER_DATE) {
            ptr++;
            unsigned long long int val = llsd_bin_get_date(&ptr);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "date", NULL, NULL, 0);
            char* str = ulltostr_ts(val);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, str, NULL, NULL, 0);
            freeNull(str);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
        }
        //
        else if (*ptr==LLSD_MAKER_REAL) {
            ptr++;
            double val = llsd_bin_get_real(&ptr);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "real", NULL, NULL, 0);
            char* str = dtostr_ts(val);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, str, NULL, NULL, 0);
            freeNull(str);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
        }
        //
        else if (*ptr==LLSD_MAKER_BIN) {
            ptr++;
            Buffer bin = llsd_bin_get_bin(&ptr);
            Buffer b64 = encode_base64_Buffer(bin);
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "binary", NULL, NULL, 0);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, (char*)b64.buf, NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            free_Buffer(&bin);
            free_Buffer(&b64);
        }
        //
        else if (*ptr==LLSD_MAKER_TRUE) {
            ptr++;
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "boolean", NULL, NULL, 0);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, "true", NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
        }
        //
        else if (*ptr==LLSD_MAKER_FALSE) {
            ptr++;
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 1, "boolean", NULL, NULL, 0);
            xml = add_tTree_node_bystr(xml, XML_CONTENT_NODE, 0, "false", NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
        }
        //
        else if (*ptr==LLSD_MAKER_UNDEF) {
            ptr++;
            xml = add_tTree_node_bystr(xml, XML_NAME_NODE, 0, "undef", NULL, NULL, 0);
            xml->state = JBXL_XML_NODE_CLOSED;
            xml = xml->prev;
        }
        else {
            PRINT_MESG("ERROR: llsd_bin_main_parse: unknown marker: %c (%04x)\n", *ptr, *ptr);
            ptr++;
        }
    }

    //
    if (snode==xml) xml->state = JBXL_XML_NODE_CLOSED;
    else            xml->state = JBXL_XML_NOT_CLOSED;

    xml = xml->prev;

    return xml;
}


#ifndef  DISABLE_ZLIB


/**
tXML*  llsd_bin_get_block_data(uByte* buf, int sz, const char* key)

llmeshファイルのヘッダ部分の keyを参照し，圧縮されたボディデータから該当ブロックデータを取り出してXML形式に変換する．

@param buf  llmeshファイルのヘッダ部分のバイナリデータ
@param sz   buf のサイズ
@param key  取り出すブロックデータのキー．Ex.) "hight_lod", "medium_lod", "low_lod", "lowest_lod", "physics_convex", "skin", etc...
@sa https://wiki.secondlife.com/wiki/LLSD

@return  指定された keyのデータの XML形式．
*/
tXML*  llsd_bin_get_block_data(uByte* buf, int sz, const char* key)
{
    int hdsz  = llsd_bin_get_length(buf, sz);
    tXML* xml = llsd_bin_parse(buf, hdsz);

    int ofst = -1, size = -1;
    if (llsd_xml_contain_key(xml, key)){
        ofst = llsd_xml_get_content_int(xml, key, "offset");
        size = llsd_xml_get_content_int(xml, key, "size");
    }
    del_all_xml(&xml);
    if (ofst<0 || size<=0) return NULL;

    //
    Buffer enc = set_Buffer(buf+ofst+hdsz, size);
    Buffer dec = gz_decode_data(enc);

    hdsz = llsd_bin_get_length(dec.buf, dec.vldsz);
    if (dec.vldsz!=hdsz) PRINT_MESG("Warning: llsd_bin_get_block_data:i mismarch of data size (%d != %d)\n", hdsz, dec.vldsz);
    xml  = llsd_bin_parse(dec.buf, hdsz);

    return xml;
}


/**
uWord*  llsd_bin_get_skin_weight(uByte* buf, int sz, int vertex_num, int* joints_num)

llmesh の LODデータから weight データを抜き出して，uWord の 2次元配列に格納する．
ちょっとメモリの無駄遣いをしている．

@param  buf : Weghtデータが格納されたバイナリデータ．
@param  sz  : buf のサイズ．
@param  vertex_num : 対応する頂点の数．

@retval 2Byte の Weightデータ．weight[頂点*(*joints_num) + joint]. 
*/
uWord*  llsd_bin_get_skin_weight(uByte* buf, int sz, int vertex_num, int* joints_num)
{
    if (buf==NULL) return NULL;

    int max_joints = 4;      // 一つの頂点が持つ重み情報の数の最大値（対象Jointの最大数）．

    if (joints_num!=NULL) *joints_num = 0;
    int jnum   = 0;         // Jointのカウンター
    int vertex = 0;         // 頂点の数
    int invrtx = 0;         // 一個の Jointに対する重みデータの数
    int pos    = 0;         // 処理中のデータの位置
    //
    // joint の数を数える and データの整合性をチェック．
    while (pos < sz && vertex < vertex_num) {
        uByte joint = *(buf + pos);
        pos++;

        if (joint==0xff) {
            invrtx = 0;
            vertex++;
        }
        else {
            if ((int)joint>jnum) jnum = (int)joint;
            invrtx++;
            pos += 2;
            if (invrtx%max_joints==0) {
                invrtx = 0;
                vertex++;
            }
        }
    }

    if (pos!=sz || vertex!=vertex_num) {
        PRINT_MESG("WARNING: llsd_bin_get_skin_weight: missmatch length %d != %d or %d != %d\n", pos, sz, vertex, vertex_num);
        //max_joints = jnum + 1;  // 事実上，無限
    }
    DEBUG_MODE {
        PRINT_MESG("llsd_bin_get_skin_weight: joints_num = %d\n", jnum + 1);
    }

    int len = sizeof(uWord)*(jnum+1)*vertex_num;
    uWord* weight = (uWord*)malloc(len);
    if (weight==NULL) {
        PRINT_MESG("ERROR: llsd_bin_get_skin_weight: no more memory (%d)\n", len);
        return NULL;
    }
    memset(weight, 0, len);

    invrtx = 0;
    pos = 0;
    vertex = 0;
    //
    while (pos < sz && vertex < vertex_num) {
        uByte joint = *(buf + pos);
        pos++;

        if (joint==0xff) {
            invrtx = 0;
            vertex++;
        }
        else {
            invrtx++;
            weight[vertex*jnum + (int)joint] = *(uWord*)(buf + pos);
            pos += 2;
            if (invrtx%max_joints==0) {
                invrtx = 0;
                vertex++;
            }
        }
    }

    if (pos!=sz || vertex!=vertex_num) {
        PRINT_MESG("ERROR: llsd_bin_get_skin_weight: missmatch length %d != %d\n", vertex, vertex_num);
        free(weight);
        weight = NULL;
        jnum = 0;
    }

    if (joints_num!=NULL) *joints_num = jnum + 1;
    return weight;
}


#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// XML LLSD of Single Data 
//  

int  llsd_xml_contain_key(tXML* xml, const char* key)
{
    if (xml==NULL || key==NULL) return FALSE;

    Buffer buf = make_Buffer_bystr("<llsd><map><key>");
    cat_s2Buffer(key, &buf);
    cat_s2Buffer("</key>", &buf);

    tXML* node = get_xml_node_bystr(xml, (char*)buf.buf);
    free_Buffer(&buf);

    if (node!=NULL) return TRUE;
    return FALSE;
}


/**
*/
int  llsd_xml_get_content_int(tXML* xml, const char* key, const char* item)
{
    if (xml==NULL || key==NULL || item==NULL) return 0;

    Buffer buf = make_Buffer_bystr("<llsd><map><key>");
    cat_s2Buffer(key, &buf);
    cat_s2Buffer("</key><map><key>", &buf);
    cat_s2Buffer(item, &buf);
    cat_s2Buffer("</key><integer>", &buf);

    int ret = get_xml_int_content_bystr(xml, (char*)buf.buf);
    free_Buffer(&buf);

    return ret;
}


double  llsd_xml_get_content_real(tXML* xml, const char* key, const char* item)
{
    if (xml==NULL || key==NULL || item==NULL) return 0.0;

    Buffer buf = make_Buffer_bystr("<llsd><map><key>");
    cat_s2Buffer(key, &buf);
    cat_s2Buffer("</key><map><key>", &buf);
    cat_s2Buffer(item, &buf);
    cat_s2Buffer("</key><real>", &buf);

    double ret = get_xml_double_content_bystr(xml, (char*)buf.buf);
    free_Buffer(&buf);

    return ret;
}


Buffer  llsd_xml_get_content_str(tXML* xml, const char* key, const char* item)
{
    Buffer buf = init_Buffer();
    if (xml==NULL || key==NULL || item==NULL) return buf;

    buf = make_Buffer_bystr("<llsd><map><key>");
    cat_s2Buffer(key, &buf);
    cat_s2Buffer("</key><map><key>", &buf);
    cat_s2Buffer(item, &buf);
    cat_s2Buffer("</key><string>", &buf);

    char* ret = get_xml_char_content_bystr(xml, (char*)buf.buf);
    free_Buffer(&buf);
    buf = make_Buffer_bystr(ret);

    return buf;
}


Buffer  llsd_xml_get_content_bin(tXML* xml, const char* key, const char* item)
{
    Buffer buf = init_Buffer();
    if (xml==NULL || key==NULL || item==NULL) return buf;

    buf = make_Buffer_bystr("<llsd><map><key>");
    cat_s2Buffer(key, &buf);
    cat_s2Buffer("</key><map><key>", &buf);
    cat_s2Buffer(item, &buf);
    cat_s2Buffer("</key><binary>", &buf);

    char* ret = get_xml_char_content_bystr(xml, (char*)buf.buf);
    free_Buffer(&buf);
    buf = make_Buffer_bystr(ret);
    
    Buffer bin = decode_base64_Buffer(buf);
    free_Buffer(&buf);

    return bin;
}

