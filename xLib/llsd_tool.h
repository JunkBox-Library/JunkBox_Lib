#ifndef  __JBXL_LLSD_TOOL_H_
#define  __JBXL_LLSD_TOOL_H_

/**
@brief   LLSD用ライブラリヘッダ（サブセット版）
@file    llsd_tool.h
@author  Fumi.Iseki (C)
@sa https://wiki.secondlife.com/wiki/LLSD
@sa http://www.nsl.tuis.ac.jp/xoops/modules/xpwiki/?LLSD
*/ 

#include "xtools.h"
#include "txml.h"
#include "jbxl_state.h"

#ifndef  DISABLE_ZLIB
    #include "gz_tool.h"
#endif


#define  LLSD_MAKER_UNDEF       '!'
#define  LLSD_MAKER_TRUE        '1'
#define  LLSD_MAKER_FALSE       '0'
#define  LLSD_MAKER_INT         'i'     // 4Byte, need ntohl()
#define  LLSD_MAKER_REAL        'r'     // 8Byte, need ntohull()
#define  LLSD_MAKER_UUID        'u'     // 16Byte
#define  LLSD_MAKER_BIN         'b'     // + ntohl(binary.length) + binary
#define  LLSD_MAKER_STR         's'     // + ntohl(string.length) + string
#define  LLSD_MAKER_URI         'l'     // + ntohl(uri.length) + uri
#define  LLSD_MAKER_KEY         'k'     // + ntohl(key.length) + string 
#define  LLSD_MAKER_DATE        'd'     // + ntohull(seconds_since_epoch)
#define  LLSD_MAKER_ARRAY       '['     // '[' + ntohl(element.number) + (child0, child1, ...) + ']' 
#define  LLSD_MAKER_ARRAY_END   ']'     // 
#define  LLSD_MAKER_MAP         '{'     // '{' + htonl(element.number) + ((key0, value0), (key1, value1), ...)+ '}' 
#define  LLSD_MAKER_MAP_END     '}'     // 


///< *ptr はマーカーの次のデータを指すポインタ．ポインタは次に進む．
Buffer  llsd_bin_get_str (uByte** ptr);
int     llsd_bin_get_int (uByte** ptr);
double  llsd_bin_get_real(uByte** ptr);
Buffer  llsd_bin_get_uuid(uByte** ptr);
Buffer  llsd_bin_get_bin (uByte** ptr);
unsigned long long int llsd_bin_get_date(uByte** ptr);  // 64bit

#define llsd_bin_get_key(p)     llsd_bin_get_str((p))   
#define llsd_bin_get_uri(p)     llsd_bin_get_str((p))   
#define llsd_bin_get_map(p)     llsd_bin_get_int((p))   ///< 要素数を返す．
#define llsd_bin_get_array(p)   llsd_bin_get_int((p))   ///< 要素数を返す．

int     llsd_bin_get_length(uByte* ptr, int sz);
tXML*   llsd_bin_parse(uByte* ptr, int sz);
tXML*   llsd_bin_main_parse(tXML* xml, uByte* ptr, int sz);

#ifndef  DISABLE_ZLIB
tXML*   llsd_bin_get_blockdata(uByte* buf, int sz, const char* key);
#endif

// for Sihgle Data
int     llsd_xml_contain_key(tXML* xml, const char* key);
int     llsd_xml_get_content_int (tXML* xml, const char* key, const char* item);
double  llsd_xml_get_content_real(tXML* xml, const char* key, const char* item);
Buffer  llsd_xml_get_content_str (tXML* xml, const char* key, const char* item);
Buffer  llsd_xml_get_content_bin (tXML* xml, const char* key, const char* item);


#endif      // __JBXL_LLSD_TOOL_H_

