
#ifndef  __JBXL_BERKELEY_DB4_H__
#define  __JBXL_BERKELEY_DB4_H__


/**
@brief   簡易データベースライブラリヘッダ for Berkeley BD4
@file    bdb_tool.h 
@version 2.0
@author  Fumi.Iseki (C)

@par ライブラリ
--ldb-4.2

@par Berkeley DBのマニュアル
- http://pybsddb.sourceforge.net/reftoc.html
*/

#include "tlist.h"

//
#ifdef DISABLE_BERKELEYDB
  #ifdef ENABLE_BERKELEYDB
    #undef ENABLE_BERKELEYDB
  #endif
#endif

//
#ifdef ENABLE_BERKELEYDB
  #ifndef HAVE_DB_H 
    #ifndef DISABLE_BERKELEYDB
      #define DISABLE_BERKELEYDB
    #endif
    #undef  ENABLE_BERKELEYDB
  #endif
#endif


//
#ifdef ENABLE_BERKELEYDB



#if ! defined (HAVE_DB_H) && ! defined (HAVE_LIBDB_4_2)
    #error  fdb.h needs Berkley 
#endif


#define DB_DBM_HSEARCH  1
#include <db.h>

#include <fcntl.h>



///////////////////////////////////////////////////////////////////////////////////
//
DB*     open_fdb(char* fname, unsigned int mode);
void    close_fdb(DB** dp);

#define    flush_fdb(d)    (d)->sync((d), 0)

int     set_fdb_data(DB* dp, Buffer id, Buffer dbdata);
int     del_fdb_data(DB* dp, Buffer id);
Buffer  get_fdb_data(DB* dp, Buffer id);


///////////////////////////////////////////////////////////////////////
// for file
int     check_fdb_file(char* fname);
int     set_fdb_data_file(char* fname, Buffer id, Buffer dbdata);
int     del_fdb_data_file(char* fname, Buffer id);
Buffer  get_fdb_data_file(char* fname, Buffer id);


///////////////////////////////////////////////////////////////////////
//
tList*    get_all_fdb_data(DB* dp);
tList*    get_all_fdb_data_file(char* fname);


#endif    // ENABLE_BERKELEYDB

#endif    // __JBXL_BERKELEY_DB4_H__


