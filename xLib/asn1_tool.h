
#ifndef  __JBXL_ASN1_TOOL_H_
#define  __JBXL_ASN1_TOOL_H_


#include "xtools.h"
#include "ttree.h"

#include "asn1_node.h"


/**
@brief   ASN.1/DER 用ライブラリヘッダ
@file    asn1_tool.h
@author  Fumi.Iseki (C)
*/ 

typedef  tTree  tDER;
/*
@code
tDER {
    tList_data  lvat {              データを格納
        int                 id      識別子
        int                 lv      データ全体のサイズ（実際に格納されているサイズ）
        Buffer              key     未使用
        Buffer  val {               コンテンツを格納
            .....
            int             vldsz   コンテンツのサイズ. （実際に格納されているサイズ）
            int             state   コンテンツの状態
            unsigned char*  buf     コンテンツ
        }
        .....
    }
    .....
}
@endcode
*/


// ASN1
//
// int2DER() -> int2bin_DER()
// toDER() -> node2DER()
// get_toDER_length() -> get_size_toDER()
// skip_DERtag() -> skip_DER_node()
//

Buffer   int2bin_DER(long int n);
long int bin2int_DER(Buffer buf);
Buffer   node2DER(Buffer, unsigned char);
int      get_size_toDER(Buffer, unsigned char);
int      skip_DER_node(Buffer, unsigned char, int, int*);


#define  new_DER_node()         new_tTree_node()
#define  del_DER_node(a)        del_tTree_node((a))
#define  del_DER(a)             del_tTree((a))
#define  add_DER_node(a, n)     add_tTree_node((a), (n))

tDER*    DER_parse(tDER* der, Buffer* buf);
void     _DER_parse_children(tDER* der, Buffer* buf);

int      set_DER_node(tDER* ser, unsigned char* buf);
int      get_DER_size(unsigned char* buf, int* len);

/*
最初の行はアンカー (ff)

04 05 69 73 65 6b 69
  ↓
[04] UNIVERSAL PRIMITIVE OCTET_STRING 7, 5 : "iseki"
  7: 全体長
  5: iseki の長さ

[ID] CLASS PRIMITIVE/CONSTRUCTED TAG/(num) data_size, contents_size : contents
*/
void     print_tDER(FILE* fp, tDER* pp);

//Buffer* DER_inverse_parse(tDER* pp); 



#endif        // __JBXL_ASN1_TOOL_H_

