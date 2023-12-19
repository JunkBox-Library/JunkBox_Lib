
#ifndef  __JBXL_MIME_TOOL_H_
#define  __JBXL_MIME_TOOL_H_

/**
@brief   MIMEツール ヘッダ
@file    mime_tool.h
@author  Fumi.Iseki (C)
@date    2005 12/25
*/


#include "xtools.h"

#define  MIME_CONTENT_LINE          "Content-"
#define  MIME_CONTENTTYPE_LINE      "Content-Type"
#define  MIME_CONTENTDSPO_LINE      "Content-Disposition"
#define  MIME_NAMEEQ_LINE           "name="
#define  MIME_FILENAMEEQ_LINE       "filename="
#define  MIME_FILENAMESTAR_LINE     "filename*"
#define  MIME_BOUNDARY_LINE         "boundary="
#define  MIME_ISO2022JP_LINE        "ISO-2022-JP"
#define  MIME_BASE64                "=?ISO-2022-JP?B?"
#define  MIME_QUTDPRNTBL            ""
#define  MIME_RFC2231               "iso-2022-jp'ja'"
#define  MIME_CHARSET_ISO2022JP     "Content-Type: text/plain; charset=iso-2022-jp"

#define  MIME_ERR_ENCODE            -1
#define  MIME_UNKNOWN_ENCODE        0
#define  MIME_BASE64_ENCODE         1
#define  MIME_QUTDPRNTBL_ENCODE     2
#define  MIME_URL_ENCODE            3


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

char*  get_mime_boundary(tList* list);
tList* get_mime_filename(FILE* fp, char* bndry);
tList* get_mime_filenameffn(char* fn, char* bndry);

char*  decode_mime_string (char* str);
char*  decode_mime_rfc2047(char* str);
char*  decode_mime_rfc2231(char* str);

char*  encode_mime_string(char* str, int kind);
int    get_mime_enckind(char* str);


#endif



