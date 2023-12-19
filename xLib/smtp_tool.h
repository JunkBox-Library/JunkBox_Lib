
#ifndef  __JBXL_SMTP_TOOLS_H_
#define  __JBXL_SMTP_TOLLS_H_


/**
@brief   SMTPツールプログラム ヘッダ
@file    smtp_tool.h
@author  Fumi.Iseki (C)
@date    2005 12/25
*/


#include  "protocol.h"


char*  get_smtp_rcpt(char* mesg);
char*  get_smtp_mailbox(char* mesg);
int    is_smtp_onecommand(char* mesg, char* com);

int    smtp_check_dot(char* mesg); 


#endif

