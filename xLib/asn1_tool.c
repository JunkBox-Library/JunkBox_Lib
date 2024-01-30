/**
@brief   ASN.1/DER 用ライブラリ
@file    asn1_tool.c
@author  Fumi.Iseki (C)
*/

#include "asn1_tool.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ASN.1/DER
//

/**
int  skip_DER_node(Buffer param, unsigned char node, int ls, int* lp) 

ノードオブジェクト paramの中の ls の位置から[ノード(node)]と[ノード値の長さ]を
スキップして，[ノード値]の位置(return値)とそのノード値の長さ *lp を返す．

@param      param  ノードオブジェクト全体
@param      node   現在のノード（確認のために指定）
@param      ls     ノードオブジェクト全体内での操作開始点  
@param[out] lp     関数終了時に，そのノードのノード値の長さが格納される．値は指定しない．

@return            そのノードのノード値の先頭位置．

@code
参考：                          ↓ls
    [ノードオブジェクト] = .....[ノード(node)][ノード値の長さ(*lp)][ノード値][ノード].......
                                                                      ↑return
    [ノード値]として，その中に[ノードオブジェクト]を含む場合もある（階層構造を取り得る）
@endcode
*/
int  skip_DER_node(Buffer param, unsigned char node, int ls, int* lp) 
{
    int i, sz=-1;

    if ((unsigned char)param.buf[ls]==node) {
        if ((unsigned char)param.buf[ls+1]>=0x80) {
            sz = (int)param.buf[ls+1] - 128;
            *lp = 0;
            for (i=ls+2; i<ls+sz+2; i++) {
                *lp *= 256;
                *lp += (unsigned char)param.buf[i];
            }    
        }
        else {
            sz = 0;
            *lp = (int)param.buf[ls+1];
        }
        //if ((unsigned char)param.buf[ls]==JBXL_ASN1_BIT) sz++;
        sz = ls + sz + 2;
    }

    return sz;
}


/**
Buffer  node2DER(Buffer pt, unsigned char node)

データを DER形式に変換する．
現在は JBXL_ASN1_INT, JBXL_ASN1_SEQ, JBXL_ASN1_BIT のみをサポート．

@param  pt    変換するデータ. JBXL_ASN1_BIT は 8bit区切りのみ対応．
@param  node  データ形式のノード．JBXL_ASN1_INT, JBXL_ASN1_SEQ, JBXL_ASN1_BIT をサポート．デフォルト動作は JBXL_ASN1_SEQ

@return DER形式へ変換されたデータ．
*/
Buffer  node2DER(Buffer pt, unsigned char node)
{
    int  sz, len, pp;
    unsigned char cnt;
    Buffer    buf;

    len = get_size_toDER(pt, node);  
    buf = make_Buffer(len);
    sz = pt.vldsz;
    pp = len - sz;                  // ノード値の格納場所の先頭
    memcpy(buf.buf+pp, pt.buf, sz);

    pp--;
    if (node==JBXL_ASN1_INT) {
/*
        int2bin_DER() で処理済み
        if (pt.buf[0]>=0x80) {
            sz++;
            buf.buf[pp--] = 0x00;
        }
*/
    }
    else if (node==JBXL_ASN1_BIT) {
        sz++;
        buf.buf[pp--] = 0x00;
    }

    buf.buf[0] = node;

    cnt = 0;
    if (sz>=128) {
        cnt++;
        while (sz>=256) {
            buf.buf[pp--] = sz % 256;
            sz >>= 8;
            cnt++;
        }
        buf.buf[pp] = (unsigned char)sz;
        buf.buf[1]  = 0x80 + cnt;
    }
    else {
        buf.buf[1] = (unsigned char)sz;
    }

    buf.vldsz = len;
    return buf;
}


/**
Buffer  int2bin_DER(long int n)

整数を DER形式のバイナリに変換する．ただし,INTノードは付けない．

@param  n  変換する整数値
@return DERのバナリ形式．

@bug この関数では, long int型の範囲の数値しか扱えない．
*/
Buffer  int2bin_DER(long int n)
{
    int  ii, cnt;
    unsigned long int div;
    Buffer bin;

    cnt = 1;
    div = n;
    while(div>=256) {
        div >>= 8;
        cnt++;
    }
    bin = make_Buffer(cnt+1);           // cnt : バイト数
    ii = cnt - 1;

    while(n>=256) {
        bin.buf[ii--] = (unsigned char)(n % 256);
        n >>= 8;
    }
    bin.buf[0] = (unsigned char)n;

    // 負数ではない場合 先頭に 0x00 を追加
    if (n>0 && bin.buf[0]>=0x80) {     
        for (ii=cnt; ii>0; ii--) bin.buf[ii] = bin.buf[ii-1];
        bin.buf[0] = 0x00;
        cnt++;
    }

    bin.vldsz  = cnt;

    return bin;
}


long int  bin2int_DER(Buffer buf)
{
    if (buf.buf==NULL || buf.vldsz==0) return 0;

    int i;
    int sz = buf.vldsz;
    unsigned char* pp = buf.buf;

    int minus = OFF;
    if (*pp & 0x80) {   // 負数
        minus = ON;
        for (i=0; i<sz; i++) pp[i] = pp[i] ^ 0xff;  // bit反転
    }

    long int ret = pp[sz-1];
    for (i=sz-2; i>=0; i--) {
        ret += pp[i] * 256;
    }
    if (minus) ret = - (ret + 1);

    return ret;
}


/**
int  get_size_toDER(Buffer pt, unsigned char node)

データをDER形式に変換した場合の長さを計算する．
整数 JBXL_ASN1_INT の場合は，先に int2bin_DER() でバイナリ化しておかなければならない．
 
@param  pt    計算対象のデータ．
@param  node  データ形式のノード．JBXL_ASN1_INT, JBXL_ASN1_SEQ, JBXL_ASN1_BIT をサポート．デフォルト動作は JBXL_ASN1_SEQ

@return データをDER形式に変換した場合の長さ．エラーの場合は 0．
*/
int  get_size_toDER(Buffer pt, unsigned char node)
{
    int  sz, div, cnt;

    sz = pt.vldsz;
    if (node==JBXL_ASN1_BIT) sz++;    // 未使用bit数を格納する先頭データ用

    cnt = 1;                    // ノード長のバイト数
    div = sz;
    if (div>=128) cnt++;        // ノード長のバイト数が2以上の場合
    while (div>=256) {
        div >>= 8;
        cnt++;
    }
    sz = sz + cnt + 1;          // 1=>node
    return sz;
}



//////////////////////////////////////////////////////////////////////////////////////
//
//

/**
tDER*  DER_parse(tDER* der, Buffer* buf)

DER形式のバイナリをパースして，tDER (tTree) 形式のツリー構造に格納する@n

@param  der      生成したツリー構造を繋げるツリー．NULLなら新規のツリーを作る（トップのノードは JBXL_ASN1_ANCHOR）
@param  buf      解析を行う DER形式のバイナリー

@retval NULL以外 木構造 tDER のトップへのポインタ．
@retval NULL     処理不能．

@attention この関数によって取得した木構造 tDER は必ず del_DER() で消去すること．
*/
tDER*  DER_parse(tDER* der, Buffer* buf)
{
    if (buf==NULL || buf->buf==NULL) return NULL;

    if (der==NULL) {
        der = new_DER_node();
        der->ldat.id = JBXL_ASN1_ANCHOR;
        der->ldat.lv = buf->vldsz;
        der->state   = JBXL_STATE_ANCHOR;
    }
    
    int sz = get_DER_size(buf->buf, NULL);
    if (sz > buf->vldsz) return NULL;     // データが短い

    _DER_parse_children(der, buf);
    
    return der;
}


void  _DER_parse_children(tDER* der, Buffer* buf)
{
    if (der==NULL) return;
    if (buf==NULL) buf = &(der->ldat.val);
    if (buf->buf==NULL) return;

    unsigned char* pp = buf->buf;

    int pos = 0;
    do {
        int sz = get_DER_size(pp+pos, NULL);
        if (sz > buf->vldsz - pos) return; 

        tDER* tmp = new_DER_node();
        int len = set_DER_node(tmp, pp+pos); 
        if (len==0) {
            del_DER_node(&tmp);
            return;
        }
        //
        add_DER_node(der, tmp);
        if (tmp->ldat.id & JBXL_ASN1_CNSTRCTD) {
            _DER_parse_children(tmp, NULL);
            free_Buffer(&(tmp->ldat.val));      // メモリが勿体ないので解放
        }
        pos += len;

    } while (pos<buf->vldsz);
    
    return;
}


/**
int  set_DER_node(tDER* der, unsigned char* buf)

DER形式のバイナリ buf に格納されている最初のデータを tDER ツリーの最期に格納する．@n

@param   der    格納先の tDER ツリー
@param   buf    格納との tDER形式のバイナリー

@return  bufの格納されたデータの長さ．
*/
int  set_DER_node(tDER* der, unsigned char* buf)
{
    if (der==NULL || buf==NULL) return 0;

    int len = 0;
    int cnt = get_DER_size(buf, &len);

    der->ldat.id  = (int)(*buf);         // buf[0] Tag
    der->ldat.lv  = cnt;
    der->ldat.val = set_Buffer((void*)(buf+cnt-len), len);

    return cnt;
}


/**
int  get_DER_size(unsigned char* buf, int* valsz)

DER形式のバイナリ buf に格納されている最初のデータの，全体の長さとノード値の長さを返す．@n
返された値は計算上の値であり，bufがそれだけのデータを持っていない可能性もある@n

@param[in]   buf      解析を行う DER形式のバイナリー
@param[out]  valsz    ノード値のデータの長さを格納する変数．NULLなら計算しない．

@return          データ全体の長さ
@retval val      ノード値のデータの長さ
*/
int  get_DER_size(unsigned char* buf, int* valsz)
{
    if (buf==NULL) return 0;

    int cnt = 0;
    int len = 0;
    cnt++;
    buf++;
    if (*buf>=0x80) {
        int i;
        int sz = (int)(*buf - 0x80);
        cnt++;
        buf++;
        for (i=0; i<sz; i++) {
            len = len*256 + (int)(*buf);
            cnt++;
            buf++;
        } 
    }
    else {
        len  = (int)(*buf);
        cnt++;
        buf++;
    }
    
    if (valsz!=NULL) *valsz = len;
    cnt += len;

    return cnt;
}


/**
void  print_tDER(FILE* fp, tDER* pp)

tDERツリーの表示．ポインタ pp以降の全てのノードを fp に出力する．

@param  fp     出力するファイルへのポインタ．NULLの場合は stderr
@param  pp     表示を開始するノードへのポインタ．

///////////////////////////////////////////////
最初の行はアンカー

04 05 69 73 65 6b 69
[04] UNIVERSAL PRIMITIVE OCTET_STRING 7, 5 : "iseki" 
  7: 全体長
  5: iseki の長さ

[ID] CLASS PRIMITIVE/CONSTRUCTED TAG/(num) data_size, contents_size : contents
*/
void  print_tDER(FILE* fp, tDER* pp)
{
    if (fp==NULL) fp = stderr;

    if (pp!=NULL) {
        while(pp->esis!=NULL) pp = pp->esis;
        do {
            int i;
            tList_data ld = pp->ldat;

            for(i=0; i<pp->depth; i++) fprintf(fp, "    ");
            if (pp->depth>0) fprintf(fp, " -> ");
            fprintf(fp, "%d: ", pp->depth);
            asn1_print_id(fp, ld.id);
            fprintf(fp, "%d, %d ", ld.lv, ld.val.vldsz);
            asn1_print_tag_value(fp, ld.id, ld.val);
            fprintf(fp, "\n");

            if (pp->next!=NULL) print_tDER(fp, pp->next);

            pp = pp->ysis;
            //if (pp!=NULL) {
            //    for(i=1; i<pp->depth-1; i++) fprintf(fp, "           ");      // for " -> "
            //}
        } while(pp!=NULL);
    }
    else {
        fprintf(fp, "(Tree is NULL)\n");
    }
    fflush(fp);

    return;
}


/*
Buffer* DER_inverse_parse(tDER* pp);
{
    Buffer* buf = new_Buffer(0);

    return buf;
}
*/
