
#ifndef  DISABLE_MYSQL

#ifndef  __JBXL_MYSQL_TOOL_H_
#define  __JBXL_MYSQL_TOOL_H_

/**
@brief   MySQL用ライブラリ ヘッダ
@file    mysql_tool.h
@author  Fumi.Iseki (C)

@par コンパイルオプション
--I/usr/local/mysql/include -L/usr/local/mysql/lib/mysql -lmysqlclient
*/ 



//#include "common.h"
//#include "tlist.h"
#include "xtools.h"

#include <mysql/mysql.h>


#define SQL_CONF_FILE        "/etc/my.cnf"
#define SQL_DEFAULT_TIMEOUT  10


MYSQL* sql_open(char* hostname, char* dbname, char* username, char* passwd, unsigned int tmot);
void   sql_close(MYSQL* mysql);


#endif      // _MYSQL_TOOL_H
#endif      // DISABLE_MYSQL

