/**
簡易データベースライブラリ for Berkeley BD4:  bdb_tool.c  v2.0

  ヘッダ
      #include "bdb_tool.h"

  ライブラリ
      -ldb-4.2  for Berkeley DB4


  Berkeley DBのマニュアル
        http://pybsddb.sourceforge.net/reftoc.html
*/


#include "bdb_tool.h"
#include "jbxl_state.h"


#ifdef  ENABLE_BERKELEYDB

/**
DB*  open_fdb(char* fname, unsigned int mode)
  
    機能: データベースのオープン．
          データベースファイル fnameをオープンする．
          存在しない場合はファイルを作成する．

    引数: fname -- データベースのファイル名(通常拡張子は除く)．

    戻り値: データベースファイルのファイルディスクリプタ．
            データベースファイルが存在せず,作成にも失敗した場合は NULL が返る．
*/
DB*  open_fdb(char* fname, unsigned int mode)
{
    DB* dp;

    if (fname==NULL) return NULL;

    int ret = db_create(&dp, NULL, 0);
    if (ret==0) {
        ret = dp->open(dp, NULL, fname, NULL, DB_HASH, (u_int32_t)mode, 0600);
    }

    if (ret!=0) {
        DEBUG_MODE {
            dp->err(dp, ret, "DB->open");
            PRINT_MESG("dbname is %s\n", fname);
        }
        return NULL;
    }
    return dp;
}


/**
void  close_fdb(DB** dp)

    機能：データベースのクローズ．データを全てフラッシュする．
*/
void  close_fdb(DB** dp)
{
    if (dp==NULL || *dp==NULL) return;

    (*dp)->close((*dp), 0);
    *dp = NULL;
    return;
}


/**
int  check_fdb_file(char* fname)
  
    機能: データベースのチェック．
          データベースファイル fnameが存在するかどうかをチェックする．
          存在しない場合はファイルを作成する．

    引数: fname -- データベースのファイル名(通常拡張子は除く)．

    戻り値: データベースファイルが存在する．または作成に成功した場合には TRUEが返る．
            データベースファイルが存在せず,作成にも失敗した場合は FALSEが返る．
*/
int  check_fdb_file(char* fname)
{
    DB* dp = open_fdb(fname, DB_CREATE);

    if (dp==NULL)  return FALSE;
    close_fdb(&dp);
    return TRUE;
}


/**
int   set_fdb_data(DB* dp, Buffer id, Buffer dbdata)

    機能: ディスクリプタを使ったデータベースへの登録．
          データベースディスクリプタ dp にキー id で dbdata を登録する．
          同じキーで既にデータが登録されている場合は dbdataで置き代わる．
          ファイルのオープンとクローズは行わない．

    引数: dp     -- データベースディスクリプタ
          id     -- データベースのキー．
          dbdata -- データベースに登録するデータ．

    戻り値: データの登録に成功した場合はデータの長さが返る．
            入力データのサイズが分らない場合は JBXL_BDB_DATASZ_ERROR が返る．
            データの登録に失敗した場合は JBXL_BDB_PUT_ERROR が返る．
*/
int   set_fdb_data(DB* dp, Buffer id, Buffer dbdata)
{
    DBT  key, data;
    int  ret;
 
    memset(&key,  0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.data = id.buf;
    key.size = id.vldsz;
    if (key.size<=0) {
        key.size = strlen((char*)id.buf);
        if (key.size<=0) return JBXL_BDB_DATASZ_ERROR;
    }

    data.data = dbdata.buf;
    data.size = dbdata.vldsz;
    if (data.size<=0) return JBXL_BDB_DATASZ_ERROR;

    ret = dp->put(dp, NULL, &key, &data, 0);
    if (ret!=0) {
        DEBUG_MODE dp->err(dp, ret, "DB->put");
        return JBXL_BDB_PUT_ERROR;
    }
    return data.size;
}


/**
int   set_fdb_data_file(char* fname, Buffer id, Buffer dbdata)

    機能: データベースへの登録．
          データベースファイル fname にキー id で dbdata を登録する．
          同じキーで既にデータが登録されている場合は dbdataで置き代わる．
          ファイルのオープンとクローズを行う．

    引数: fname  -- データベースのファイル名(通常拡張子は除く)．
          id     -- データベースのキー．
          dbdata -- データベースに登録するデータ．

    戻り値: データの登録に成功した場合はデータの長さが返る．
            データベースファイルのオープンに失敗した場合は JBXL_BDB_OPEN_ERROR が返る．
            入力データのサイズが分らない場合は JBXL_BDB_DATASZ_ERROR が返る．
            データの登録に失敗した場合は JBXL_BDB_PUT_ERROR が返る．
*/
int   set_fdb_data_file(char* fname, Buffer id, Buffer dbdata)
{
    DBT  key, data;
    DB*  dp;
    int  ret;
 
    dp = open_fdb(fname, DB_CREATE);
    if (dp==NULL) return JBXL_BDB_OPEN_ERROR;

    memset(&key,  0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.data  = (char*)(id.buf);
    key.size = id.vldsz;
    if (key.size<=0) {
        key.size = strlen((char*)id.buf);
        if (key.size<=0) return JBXL_BDB_KEY_ERROR;
    }

    data.data = (char*)dbdata.buf;
    data.size = dbdata.vldsz;
    if (data.size<=0) return JBXL_BDB_DATASZ_ERROR;

    ret = dp->put(dp, NULL, &key, &data, 0);
    close_fdb(&dp);

    if (ret!=0) {
        DEBUG_MODE dp->err(dp, ret, "DB->put");
        return JBXL_BDB_PUT_ERROR;
    }
    return data.size;
}


/**
Buffer  get_fdb_data(DB* dp, Buffer id)

    機能: データベースからデータを取得．
          データベースディスクリプタ dp からキー id でデータを獲得する．
          キー idは文字列でなければならない．
          ファイルのオープンとクローズは行わない．

          成功した場合には, 戻り値として Buffer型データへのポインタを返す．
          読み取ったデータの型はプログラマが把握していなければならない．
          読み取ったデータは使用が終ったら free_Buffer()関数等により開放されなければならない．

          データの取得に失敗した場合は JBXL_BDB_GET_ERROR が,メモリの確保に失敗した場合は JBXL_BDB_MOMERY_ERROR が stateに格納されて返される．
          失敗した場合はいずれの場合も, メンバ変数 bufの値がNULLになって返される．

    引数: dp     -- データベースのファイルディスクリプタ．
          id     -- 読み取るデータのキー．

    戻り値:  読み取ったデータ(Buffer型データ)へのポインタを返す．

    メモ：data.dataは freeしてはいけない．
*/
Buffer  get_fdb_data(DB* dp, Buffer id)
{
    int  ret;
    DBT  key, data;
    Buffer dbdata;

    memset(&key,  0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    dbdata   = init_Buffer();
    key.data = (void*)id.buf;
    key.size = (u_int32_t)id.vldsz;
    if (key.size<=0) {
        key.size = strlen((char*)id.buf);
        if (key.size<=0) {
            dbdata.state = JBXL_BDB_MEMOEY_ERROR;
            return dbdata;
        }
    }

    ret = dp->get(dp, NULL, &key, &data, 0);
    if (ret!=0) {
        dbdata.state = JBXL_DBD_MEMORY_ERROR;
        return dbdata;
    }

    dbdata = make_Buffer(data.size+1);        // +1 is for string
    if (dbdata.buf==NULL) {
        dbdata.state = JBXL_BDB_GET_ERROR;
        return dbdata;
    }
    memcpy(dbdata.buf, data.data, data.size);
    dbdata.vldsz = data.size;
    dbdata.buf[data.size] = '\0';            // for string

    return dbdata;
}


/**
Buffer  get_fdb_data_file(char* fname, Buffer id)

    機能: データベースからデータを取得．
          データベースファイル fname からキー id でデータを獲得する．
          キー idは文字列でなければならない．
          ファイルのオープンとクローズを行う．

          成功した場合には, 戻り値として Buffer型データへのポインタを返す．
          読み取ったデータの型はプログラマが把握していなければならない．
          読み取ったデータは使用が終ったら free_Buffer()関数等により開放されなければならない．

          データベースファイル fnameのオープンに失敗した場合は Buffer型変数の
          メンバ変数 stateに JBXL_BDB_OPEN_ERROR が格納されて返される．データの取得に失敗した
          場合は JBXL_BDB_GET_ERROR が,メモリの確保に失敗した場合は JBXL_DBD_MEMORY_ERROR が格納されて返される．
          失敗した場合はいずれの場合も,メンバ変数bufの値がNULLになって返される．

    引数: fname  -- データベースのファイル名(通常拡張子は除く)．
          id     -- 読み取るデータのキー．

    戻り値:  読み取ったデータ(Buffer型データ)へのポインタを返す．

    メモ：data.dataは freeしてはいけない．
*/
Buffer  get_fdb_data_file(char* fname, Buffer id)
{
    int  ret;
    DBT  key, data;
    DB*  dp;
    Buffer dbdata;

    dbdata = init_Buffer();
    dp = open_fdb(fname, DB_RDONLY);
    if (dp==NULL) {
        dbdata.state = JBXL_BDB_OPEN_ERROR;
        return dbdata;
    }

    memset(&key,  0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.data = (char*)(id.buf);
    key.size = id.vldsz;
    if (key.size<=0) {
        key.size = strlen((char*)id.buf);
        if (key.size<=0) {
            dbdata.state = JBXL_BDB_KEY_ERROR;
            return dbdata;
        }
    }

    ret = dp->get(dp, NULL, &key, &data, 0);
    close_fdb(&dp);
    if (ret!=0) {
        dbdata.state = JBXL_BDB_GET_ERROR;
        return dbdata;
    }

    dbdata = make_Buffer(data.size+1);        // +1 is for string
    if (dbdata.buf==NULL) {
        dbdata.state = JBXL_BDB_MEMORY_ERROR;
        return dbdata;
    }

    dbdata.vldsz = data.size;
    memcpy(dbdata.buf, data.data, data.size);
    dbdata.buf[data.size] = '\0';            // for string

    return dbdata;
}


/**
int   del_fdb_data(DB* dp, Buffer id)

    機能: データベースからの削除．
          データベースディスクリプタ dp からキー id のデータを削除する．
          キー idは文字列でなければならない．
          ファイルのオープンとクローズは行わない．

    引数: dp    -- データベースディスクリプタ
          id    -- 削除するデータのキー．

    戻り値: データの削除に成功した場合は TRUEが返る．
            データの削除に失敗した場合は FALSEが返る．
*/
int   del_fdb_data(DB* dp, Buffer id)
{
    DBT  key;
    int  ret;
 
    memset(&key, 0, sizeof(DBT));
    key.data = (char*)(id.buf);
    key.size = id.vldsz;
    if (key.size<=0) {
        key.size = strlen((char*)id.buf);
        if (key.size<=0) return FALSE;
    }
    ret = dp->del(dp, NULL, &key, 0);

    if (ret!=0) {
        DEBUG_MODE dp->err(dp, ret, "DB->del");
        return FALSE;
    }
    return TRUE;
}


/**
int   del_fdb_data_file(char* fname, Buffer id)

    機能: データベースからの削除．
          データベースファイル fname からキー id のデータを削除する．
          キー idは文字列でなければならない．
          ファイルのオープンとクローズを行う．

    引数: fname -- データベースのファイル名(通常拡張子は除く)．
          id    -- 削除するデータのキー．

    戻り値: データの削除に成功した場合は TRUEが返る．
            データの削除に失敗した場合は FALSEが返る．
*/
int   del_fdb_data_file(char* fname, Buffer id)
{
    DBT  key;
    DB*  dp;
    int  ret;
 
    dp = open_fdb(fname, 0);
    if (dp==NULL) return FALSE;

    memset(&key, 0, sizeof(DBT));
    key.data = (char*)(id.buf);
    key.size = id.vldsz;
    if (key.size<=0) {
        key.size = strlen((char*)id.buf);
        if (key.size<=0) return FALSE;
    }

    ret = dp->del(dp, NULL, &key, 0);
    close_fdb(&dp);

    if (ret!=0) {
        DEBUG_MODE dp->err(dp, ret, "DB->del");
        return FALSE;
    }
    return TRUE;
}


/**
tList*  get_all_fdb_data(DB* dp)

    機能: データベース全体の取得 

    引数: db -- データベース識別子．
*/
tList*  get_all_fdb_data(DB* dp)
{
    int  ret;
    DBT  key, data;
    DBC* crsp;

    tList* lt = NULL;
    tList* lp = NULL;
    Buffer id, dt;

    if (dp==NULL) return NULL;

    dp->cursor(dp, NULL, &crsp, 0);
    if (crsp==NULL) return NULL;
    
    memset(&key,  0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    ret = crsp->c_get(crsp, &key, &data, DB_NEXT);
    while (ret==0) {
        id = make_Buffer_bin(key.data,  key.size);
        dt = make_Buffer_bin(data.data, data.size);
        lt = add_tList_node_Buffer(lt, id, dt);
        if (lp==NULL) lp = lt;
        free_Buffer(&id);
        free_Buffer(&dt);

        ret = crsp->c_get(crsp, &key, &data, DB_NEXT);
    }

    crsp->c_close(crsp);

    return lp;
}


/**
tList*  get_all_fdb_data_file(char* fname)

    機能: データベース全体の取得 

    引数: *fname -- データベースのファイル名
*/
tList*  get_all_fdb_data_file(char* fname)
{
    DB*    dp;
    tList* lp = NULL;

    if (fname==NULL) return NULL;

    dp = open_fdb(fname, DB_RDONLY);
    if (dp==NULL) return NULL;

    lp = get_all_fdb_data(dp);
    close_fdb(&dp);

    return lp;
}


#endif
