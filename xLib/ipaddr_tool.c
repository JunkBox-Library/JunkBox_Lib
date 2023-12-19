
/**  
@brief   IP Address Tool
@file    ipaddr_tool.c
@author  Fumi Iseki 
@date    2005 09/21
*/


#include "ipaddr_tool.h"


/**
tList*  read_ipaddr_file(char* fn)

ファイルから IPアドレスを読み込んで，[IPアドレス]/[ネットマスク]　の形式に
変換して，リストに格納する．FQDNはIPアドレスに変換される．@n
FQDNでないドメイン名，文字列は / 以後を無視して文字列のままリストに格納する．

データの読み取り時に次の処理が行なわれる．
- 前後の空白,CR,LFは削除される
- 空行は無視される．
- # 以降は無視される．

@param  fn  ファイル名
@return リストへのポインタ
*/
tList*  read_ipaddr_file(char* fn)
{
    tList* lp;
    tList* lt;

    lp = read_tList_file(fn, 2);
    if (lp==NULL) return NULL;

    lt = to_ipaddress_list(lp);
    del_all_tList(&lp);

    return lt;
}


/**
tList*  read_ipaddr_fp(FILE* fp)

ファイルから IPアドレスを読み込んで，[IPアドレス]/[ネットマスク]　の形式に
変換して，リストに格納する．FQDNはIPアドレスに変換される．@n
FQDNでないドメイン名，文字列は / 以後を無視して文字列のままリストに格納する．

データの読み取り時に次の処理が行なわれる．
- 前後の空白,CR,LFは削除される
- 空行は無視される．
- # で始まる，または 空白(複数)# で始まる行は無視される．

@param  fp  ファイルポインタ
@return リストへのポインタ
*/
tList*  read_ipaddr_fp(FILE* fp)
{
    tList* lp;
    tList* lt;

    lp = read_tList_fp(fp, 1);
    if (lp==NULL) return NULL;

    lt = to_ipaddress_list(lp);
    del_all_tList(&lp);

    return lt;
}



/**
tList*  to_ipaddress_list(tList* lp)

リスト中の IPアドレスを [IPアドレス]/[ネットマスク]　の形式に
変換して，新しいリストに格納する．FQDNはIPアドレスに変換される．@n
変換に to_address_num8() を使用．CIDER対応．

FQDNでないドメイン名，文字列は / 以後を無視して文字列のまま 新しいリストに格納する．
後の is_host_in_list()などを高速に行なうための前処理．

@param  lp  変換前（ファイルなどから読み込んだまま）のデータの入ったリスト
@return 変換して新しく生成したリスト．
*/
tList*  to_ipaddress_list(tList* lp)
{
    unsigned char* num;
    char*          addr;
    tList* ls = NULL;
    tList* lt = NULL;

    if (lp==NULL) return NULL;
    if (lp->ldat.key.buf==NULL) return NULL;

    while (lp!=NULL) {
        if (lp->ldat.key.buf!=NULL && strlen((const char*)lp->ldat.key.buf)>0) {
            if ((lp->ldat.key.buf)[0]!='#') {
                num = to_address_num8_ipv4((char*)lp->ldat.key.buf, 1);
                if (num!=NULL) {
                    addr = to_address_char8_ipv4(num);
                    free(num);
                    if (addr!=NULL) {
                        ls = add_tList_node_str(ls, addr, NULL);
                        free(addr);
                    }   // ここで addr==NULL となる場合はメモリ不足のみ
                }
                else {
                    addr = awk((char*)lp->ldat.key.buf, '/', 1);
                    if (addr!=NULL) {
                        int i = 0;
                        while(addr[i]!='\0' && addr[i]!=',' && addr[i]!=' ') i++;
                        addr[i] = '\0';
                        ls = add_tList_node_str(ls, addr, NULL);
                        free(addr);
                    }
                }
                if (lt==NULL) lt = ls;
            }
        }
        lp = lp->next;
    }
    
    return lt;
}



/**
int  is_host_in_list(tList* lp, unsigned char* num, char* hname)

リストlpの キー部に格納されている ホスト情報と num, hname が同じネットワークに属するか
どうかをチェックする．num と hname はどちらか一方が指定されていれば十分である．

先ず，lpのキー部（FQDN）と hname が比較される．つまり，キー部がドメイン名の一部の場合は 
hnameと比較する（後方一致）．@n
次に，lpのキー部がドメイン名でない場合（IPアドレスの場合）は num と比較される．
    
@param  lp     チェックするIPアドレス（文字列）の格納されているリスト．
@param  num    比較するIPアドレス（1byte unsigned 整数型）4Byte．
               8Byte型(addr/mask)でも作動するが，ネットマスク部はされる．
@param  hname  比較するホスト名（文字列）．通常は numから逆引きされたもの．

@retval TRUE   リスト中に同じネットワークに属するエントリが存在する．
@retval FALSE  リスト中に同じネットワークに属するエントリは存在しない．
*/
int  is_host_in_list(tList* lp, unsigned char* num, char* hname)
{
    unsigned char* addr;

    if (lp==NULL) return FALSE;
    if (lp->ldat.key.buf==NULL) return FALSE;
    if (hname==NULL && num==NULL) return FALSE;

    while (lp!=NULL) {
        if (lp->ldat.key.buf!=NULL && strlen((const char*)lp->ldat.key.buf)>0) {
            addr = to_address_num8_ipv4((char*)lp->ldat.key.buf, 0);
            if (addr==NULL && hname!=NULL) {
                if (!strnrvscmp((const char*)lp->ldat.key.buf, hname, (int)strlen((const char*)lp->ldat.key.buf))) return TRUE;
            }
            else if(addr!=NULL && num!=NULL) {
                if (is_same_network_num_ipv4(addr, num, &(addr[4]))) return TRUE;
            }
        }
        lp = lp->next;
    }
    
    return FALSE;
}



/**
int  is_ipaddr_num_in_list(tList* lp, unsigned char* num)

リストlpの キー部に格納されている ホスト情報と num が同じネットワークに属するかどうかをチェックする．@n
リストのキー部がFQDNであってもチェック可能．

@param  lp    チェックするIPアドレス（文字列）の格納されているリスト．
@param  num   比較するIPアドレス（バイナリ）4Byte．
              8Byte型(addr/mask)でも作動するが，ネットマスク部は無視される．

@retval TRUE  リスト中に同じネットワークに属するエントリが存在する．
@retval FALSE リスト中に同じネットワークに属するエントリは存在しない．
*/
int  is_ipaddr_num_in_list(tList* lp, unsigned char* num)
{
    unsigned char* addr;

    if (lp==NULL || num==NULL) return FALSE;
    if (lp->ldat.key.buf==NULL) return FALSE;

    while (lp!=NULL) {
        if (lp->ldat.key.buf!=NULL && strlen((const char*)lp->ldat.key.buf)>0) {
            addr = to_address_num8_ipv4((char*)lp->ldat.key.buf, 1);
            if (addr!=NULL) {
                if (is_same_network_num_ipv4(num, addr, &(addr[4]))) return TRUE;
            }
        }
        lp = lp->next;
    }
    
    return FALSE;
}



/**
void  print_address_in_list(FILE* fp, tList* lp)

リスト中のドメイン名，またはIPアドレスを表示する．
FQDNはIPアドレスに変換して表示する．

@param  fp  出力先のファイル記述子
@param  lp  ドメイン名またはIPアドレスをキー部に格納したリストへのポインタ
*/
void  print_address_in_list(FILE* fp, tList* lp)
{
    unsigned char* addr;

    while (lp!=NULL) {
        if (lp->ldat.key.buf!=NULL && strlen((const char*)lp->ldat.key.buf)>0) {
            addr = to_address_num8_ipv4((char*)lp->ldat.key.buf, 1);
            if (addr!=NULL) {   
                fprintf(fp, "[%d.%d.%d.%d/%d.%d.%d.%d]\n", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5],addr[6],addr[7]);
            }
            else {      // 恐らくドメイン名
                fprintf(fp, "[%s]\n", lp->ldat.key.buf);
            }
        }
        lp = lp->next;
    }
    return;
}


