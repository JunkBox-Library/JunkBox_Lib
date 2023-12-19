/**
@brief   MySQL用ライブラリ
@file    mysql_tool.c
@author  Fumi.Iseki (C)

@par ライブラリ
--L/usr/local/mysql/lib/mysql -lmysqlclient 
*/


#include "mysql_tool.h"


MYSQL*  sql_open(char* hostname, char* dbname, char* username, char* passwd, unsigned int tmot)
{
    MYSQL* mysql;
    MYSQL* cnnct;

    Buffer host;
    unsigned short port;

    if (hostname==NULL || dbname==NULL || username==NULL || passwd==NULL) return NULL;

    Buffer wrk = make_Buffer_bystr(hostname);
    decomp_hostport(wrk, &host, &port);
    free_Buffer(&wrk);
    if (host.buf==NULL) return NULL;

    if (tmot==0) tmot = SQL_DEFAULT_TIMEOUT;

    mysql = mysql_init(NULL);
    if (mysql==NULL) return NULL;

    if (file_exist(SQL_CONF_FILE)) {
        mysql_options(mysql, MYSQL_READ_DEFAULT_FILE, SQL_CONF_FILE);
    }
    mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&tmot);

    cnnct = mysql_real_connect(mysql, (char*)host.buf, username, passwd, dbname, (int)port, NULL, 0);
    if (cnnct==NULL) {
        mysql_close(mysql);
        mysql = NULL;
        return NULL;
    }

    return mysql;
}


void  sql_close(MYSQL* mysql)
{
    if (mysql!=NULL) mysql_close(mysql);

    return;
}

