/**
@brief   汎用ツールプログラム
@file    tools.c
@author  Fumi.Iseki (C)
@date    2005 10/10
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include "tools.h"
#include "jbxl_state.h"


int   DebugMode     = OFF;
int   UnitTestMode  = OFF;

int   KanjiCode     = CODE_UTF8;                    ///< システムの漢字コード
int   HostEndian    = UNKNOWN_ENDIAN;               ///< システムの Endian

unsigned long int UsedMemoryBase = 0;               ///< メモリチェック用

//
unsigned char  LocalIPNum[4]   = {0x7f, 0x00, 0x00, 0x01};    ///< 127.0.0.1 のバイナリ
unsigned char  LocalIPNum6[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}; ///< ::1 のバイナリ


///////////////////////////////////////////////////////////////////////////////////////////
// 

// case of C++, isNull() is inline

#ifndef CPLUSPLUS

/**
int  isNull(void* p)

ポインタが NULLかどうかを検査する．@n
Windowsの場合，無効ポインタも検出する．

@param  p     検査するポインタ．

@retval TRUE  ポインタがNULL，または無効（Windowsの場合）
@retval FALSE ポインタはNULLではなく有効（Windowsの場合）である．

@attention
JunkBox_LIb++ の場合はこの関数ではなく，inline型の isNull() が使用される．
*/
int  isNull(void* p)
{
    if (p==NULL) return TRUE;

#ifdef WIN32
    if (p==WIN_DD_NULL) return TRUE;
#endif

    return FALSE;
}

#endif


///////////////////////////////////////////////////////////////////////////////////////////
// 

/**
int is_little_endian(void)

エンディアンの動的チェック

一度呼び出すと，大域変数 @b HostEndian に @b LITTLE_ENDIAN か @b BIG_ENDIAN が設定される．

@retval TRUE   Little Endian
@retval FALSE  Big Endian

@see HostEndian, is_big_endian()
*/
int is_little_endian(void)
{
    unsigned char x[] = {0x01, 0x00};

    if (HostEndian==LITTLE_ENDIAN) return TRUE;

    else if (HostEndian==BIG_ENDIAN) return FALSE;

    else {
        unsigned short* xx = (unsigned short*)x;
        if (*xx==1) {
            HostEndian = LITTLE_ENDIAN;
            return TRUE;
        }
        else {
            HostEndian = BIG_ENDIAN;
            return FALSE;
        }
    }
}


/**
int is_big_endian(void)

エンディアンの動的チェック

一度呼び出すと，大域変数 @b HostEndian に @b LITTLE_ENDIAN か @b BIG_ENDIAN が設定される．

@retval TRUE   Big Endian
@retval FALSE  Little Endian

@see HostEndian, is_little_endian()
*/
int is_big_endian(void)
{
    unsigned char x[] = {0x00, 0x01};

    if (HostEndian==BIG_ENDIAN) return TRUE;

    else if (HostEndian==LITTLE_ENDIAN) return FALSE;

    else {
        unsigned short* xx = (unsigned short*)x;
        if (*xx==1) {
            HostEndian = BIG_ENDIAN;
            return TRUE;
        }
        else {
            HostEndian = LITTLE_ENDIAN;
            return FALSE;
        }
    }
}


/**
void  check_endian(void)

大域変数 @b HostEndian に @b LITTLE_ENDIAN か @b BIG_ENDIAN を設定する．

@see HostEndian
*/
void  check_endian(void)
{
    if (HostEndian==UNKNOWN_ENDIAN) is_little_endian();
} 


/** double double_from_little_endian(void* ptr)

Little Endian形式で格納されている double型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した double型の値
*/
double double_from_little_endian(void* ptr)
{
    double ret = *((double*)ptr);
    
    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==BIG_ENDIAN) reverse_str((uByte*)(&ret), 8);
    return ret;
}


/**
float float_from_little_endian(void* ptr)

Little Endian形式で格納されている float型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した float型の値
*/
float float_from_little_endian(void* ptr)
{
    float ret = *((float*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==BIG_ENDIAN) reverse_str((uByte*)(&ret), 4);
    return ret;
}


/**
int  int_from_little_endian(void* ptr)

Little Endian形式で格納されている int型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した int型の値
*/
int  int_from_little_endian(void* ptr)
{
    int ret = *((int*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==BIG_ENDIAN) reverse_str((uByte*)(&ret), 4);
    return ret;
}


/**
unsigned int  uint_from_little_endian(void* ptr)

Little Endian形式で格納されている unsigned int型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した unsigned int型の値
*/
unsigned int  uint_from_little_endian(void* ptr)
{
    unsigned int ret = *((unsigned int*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==BIG_ENDIAN) reverse_str((uByte*)(&ret), 4);
    return ret;
}


/**
short short_from_little_endian(void* ptr)

Little Endian形式で格納されている short int型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した short int型の値
*/
short short_from_little_endian(void* ptr)
{
    short ret = *((short*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==BIG_ENDIAN) reverse_str((uByte*)(&ret), 2);
    return ret;
}


/**
unsigned short ushort_from_little_endian(void* ptr)

Little Endian形式で格納されている unsigned short int型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した unsigned short int型の値
*/
unsigned short ushort_from_little_endian(void* ptr)
{
    unsigned short ret = *((unsigned short*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==BIG_ENDIAN) reverse_str((uByte*)(&ret), 2);
    return ret;
}


/**
double double_from_big_endian(void* ptr)

Big Endian形式で格納されている double型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した double型の値
*/
double double_from_big_endian(void* ptr)
{
    double ret = *((double*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==LITTLE_ENDIAN) reverse_str((uByte*)(&ret), 8);
    return ret;
}


/**
float float_from_big_endian(void* ptr)

Big Endian形式で格納されている float型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した float型の値
*/
float float_from_big_endian(void* ptr)
{
    float ret = *((float*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==LITTLE_ENDIAN) reverse_str((uByte*)(&ret), 4);
    return ret;
}


/**
int  int_from_big_endian(void* ptr)

Big Endian形式で格納されている int型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した int型の値
*/
int  int_from_big_endian(void* ptr)
{
    int ret = *((int*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==LITTLE_ENDIAN) reverse_str((uByte*)(&ret), 4);
    return ret;
}


/**
unsigned int  uint_from_big_endian(void* ptr)

Big Endian形式で格納されている unsigned int型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した unsigned int型の値
*/
unsigned int  uint_from_big_endian(void* ptr)
{
    int ret = *((unsigned int*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==LITTLE_ENDIAN) reverse_str((uByte*)(&ret), 4);
    return ret;
}


/**
short short_from_big_endian(void* ptr)

Big Endian形式で格納されている short int型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した short int型の値
*/
short short_from_big_endian(void* ptr)
{
    short ret = *((short*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==LITTLE_ENDIAN) reverse_str((uByte*)(&ret), 2);
    return ret;
}


/**
unsigned short ushort_from_big_endian(void* ptr)

Big Endian形式で格納されている unsigned short int型の値を取り出す．

@param  ptr  変数が格納されているメモリへのポインタ
@return 取り出した unsigned short int型の値
*/
unsigned short ushort_from_big_endian(void* ptr)
{
    unsigned short ret = *((unsigned short*)ptr);

    if (HostEndian==UNKNOWN_ENDIAN) check_endian();
    if (HostEndian==LITTLE_ENDIAN) reverse_str((uByte*)(&ret), 2);
    return ret;
}


///////////////////////////////////////////////////////////////////////////////////////////
// Time

//char  _Local_Time[64];      ///< 作業用（ローカルタイム）


/**
char*  get_localtime(char c1, char c2, char c3, char c4)

非推奨．use get_local_timestamp(time(0), "%Y/%m/%dT%H:%M:%SZ")

ローカルタイムを "年c1月c1日c2時c3分c3秒c4" の形式で返す．@n
返ってきた char* ポインタは free() してはいけない.

@return "年c1月c1日c2時c3分c3秒c4" 形式のローカルタイム

@code
    printf("%s\n", get_localtime('/', 'T', ':', 'Z'));

    2021/08/16T15:04:56Z
@endcode
*/
/*
char*  get_localtime(char c1, char c2, char c3, char c4)
{
    struct tm* pt;
    time_t tm;

    tm = time(NULL);
    pt = localtime(&tm);

    snprintf(_Local_Time, 63, "%04d%c%02d%c%02d%c%02d%c%02d%c%02d%c",
                pt->tm_year+1900, c1, pt->tm_mon+1, c1, pt->tm_mday, c2,
                pt->tm_hour, c3, pt->tm_min, c3, pt->tm_sec, c4);

    return _Local_Time;
}
*/


/**
char*  get_localtime_ts(char c1, char c2, char c3, char c4)

非推奨．use get_local_timestamp(time(0), "%Y/%m/%dT%H:%M:%SZ")

ローカルタイムを "年c1月c1日c2時c3分c3秒c4" の形式で返す．@n
返ってきた char* ポインタは free() すること．

@return "年c1月c1日c2時c3分c4秒c5" 形式のローカルタイム．要 free
*/
/*
char*  get_localtime_ts(char c1, char c2, char c3, char c4)
{
    struct tm* pt;
    time_t tm;
    char*  local_tm = NULL;

    local_tm = (char*)malloc(64);
    if (local_tm==NULL) return NULL;
    memset(local_tm, 0, 64);

    tm = time(NULL);
    pt = localtime(&tm);

    snprintf(local_tm, 63, "%04d%c%02d%c%02d%c%02d%c%02d%c%02d%c", 
                pt->tm_year+1900, c1, pt->tm_mon+1, c1, pt->tm_mday, c2, 
                pt->tm_hour, c3, pt->tm_min, c3, pt->tm_sec, c4);

    return local_tm;
}
*/


// 要 free
// char* ltime = get_local_timestamp(time(0), "%Y/%m/%dT%H:%M:%SZ")
//
char*  get_local_timestamp(time_t date, const char* format)
{
    char* buf = (char*)malloc(64);
    memset(buf, 0, 64);

    //struct tm tm = *gmtime(&date);
    struct tm tm = *localtime(&date);
    strftime(buf, 63, format, &tm);

    return buf;
}


// 要 free
char*  get_gmt_timestamp(time_t date, const char* format)
{
    char* buf = (char*)malloc(64);
    memset(buf, 0, 64);

    struct tm tm = *gmtime(&date);
    //struct tm tm = *localtime(&date);
    strftime(buf, 63, format, &tm);

    return buf;
}


///////////////////////////////////////////////////////////////////////////////////////////

/**
char*  get_line(char* buf, int n)

文字型データstrのバッファ内の n行目を取り出す．改行コードは削除される．

取り出した行を char型変数のバッファに格納して返す．@n
バッファの最後が '@\0' で終わっている場合，バッファを越えて行を読もうとした場合は NULL を返す．@n
'@\0' で終わっていない場合，バッファが何処で終わりかを知る事は（簡単な方法では）できない．

返されたデータは free()する必要がある．

@param  buf  操作対象文の字列型データ(含む改行)
@param  n    行の指定．1 から数える．
@return      指定された n行目の文字列．改行は含まない．要 free
*/
char*  get_line(char* buf, int n)
{
    int   i, j, pos, cnt;
    char* ret = NULL;

    if (buf==NULL) return NULL;

    for(i=0,j=0; j<n-1; j++) {
        while (buf[i]!=CHAR_LF && buf[i]!=CHAR_CR && buf[i]!='\0') i++;
        while (buf[i]==CHAR_LF || buf[i]==CHAR_CR) i++;
    }
    if (buf[i]=='\0') return  NULL;

    pos = i;
    while (buf[i]!=CHAR_LF && buf[i]!=CHAR_CR && buf[i]!='\0') i++;
    cnt = i - pos;

    if (cnt>=0) {  
        ret = (char*)malloc(cnt+1);
        if (ret==NULL) return NULL;

        for(i=0; i<cnt; i++) ret[i] = buf[pos+i];
        ret[cnt] = '\0';
    }
    return  ret;
}


/**
char*  get_seq_data(char* buf, int* ptr)

文字型データ bufのバッファ内の行データをシーケンシャルに取り出す．

改行コードは削除される．連続して呼び出す場合，一番最初は *ptrに 0を設定し，以後は *ptrの内容は変えない．@n 
取り出した行を char型変数のバッファに格納して返す．@n
バッファの最後が '@\0' で終わっている場合，バッファを越えて行を読もうとした時は NULL を返す．@n
'@\0' で終わっていない場合，バッファが何処で終わりかを知る事は（簡単な方法では）できない．

返されたデータは free() する必要がある．

@param[in]   buf  操作対象文の字列型データ(含む改行)
@param[in]   ptr  読み込みを開始するデータの位置（バイト）． 
@param[out]  ptr  次のデータが始まる位置（バイト）． 
@return      指定されたポインタ以下の一行分の文字列．改行は含まない．要 free
*/
char*  get_seq_data(char* buf, int* ptr)
{
    int   pos, cnt;
    char* ret = NULL;

    if (buf==NULL || buf[*ptr]=='\0') return NULL;

    pos = *ptr;
    while (buf[*ptr]!=CHAR_LF && buf[*ptr]!=CHAR_CR && buf[*ptr]!='\0') (*ptr)++;
    cnt = *ptr - pos;

    if (cnt>=0) {  
        ret = (char*)malloc(cnt+1);
        if (ret==NULL) return NULL;

        int i;
        for(i=0; i<cnt; i++) ret[i] = buf[pos+i];
        ret[cnt] = '\0';
    }

    while (buf[*ptr]==CHAR_LF || buf[*ptr]==CHAR_CR) (*ptr)++;
    return  ret;
}


/**
char*  awk(char* buf, char cc, int n)

ccを区切り記号として, strのバッファ内の n番目の項目を返す．
返されたデータは free()する必要がある．

@param  buf  操作対象文字列．
@param  cc   区切り文字．
@param  n    項目の指定． 1から数える．

@return 指定された n番目の項目の文字列．要 free
@see cawk()
*/
char*  awk(char* buf, char cc, int n)
{
    int i, j, pos, cnt;
    char*  item = NULL;

    if (buf==NULL) return NULL;

    for(i=0,j=0; j<n-1; j++) {
        while (buf[i]!='\0' && buf[i]!=cc) i++;
        if (buf[i]==cc) i++;
    }
    if (buf[i]=='\0')  return NULL;

    pos = i;
    while (buf[i]!='\0' && buf[i]!=cc) i++;
    cnt = i - pos;

    item = (char*)malloc(cnt+1);
    if (item==NULL) return NULL;

    for (i=0; i<cnt; i++) item[i] = buf[pos+i];
    item[cnt] = '\0';

    return item;
}


/**
char*  cawk(char* buf, char cc, int n)

ccを区切り記号として, strのバッファ内の n番目の項目を返す．

n は 1から数える．連続する cc(区切り)は一つの区切りとみなす．
返されたデータは free()する必要がある．

@param  buf  操作対象文字列．
@param  cc   区切り文字．
@param  n    項目の指定．

@return 指定された n番目の項目の文字列．要 free.
@see awk()
*/
char*  cawk(char* buf, char cc, int n)
{
    int i, j, pos, cnt;
    char*  item = NULL;

    if (buf==NULL) return NULL;

    for(i=0,j=0; j<n-1; j++) {
        while (buf[i]!='\0' && buf[i]!=cc) i++;
        while (buf[i]!='\0' && buf[i]==cc) i++;
    }
    if (buf[i]=='\0') return NULL;

    pos = i;
    while (buf[i]!='\0' && buf[i]!=cc) i++;
    cnt = i - pos;

    item = (char*)malloc(cnt+1);
    if (item==NULL) return NULL;

    for (i=0; i<cnt; i++) item[i] = buf[pos+i];
    item[cnt] = '\0';

    return item;
}


/**
int  bincmp(unsigned char* b1, unsigned char* b2, int n) 

バイナリデータ s1と s2 nバイト比較する．

@param  b1  比較するバイナリデータ1
@param  b2  比較するバイナリデータ2
@param  n   比較するバイト数

@retval 0   一致する
@retval 1   一致しない

@attention strncmp()の戻り値の定義とは若干異なるので注意すること．
*/
int  bincmp(unsigned char* b1, unsigned char* b2, int n) 
{
    int i;

    if (b1==NULL || b2==NULL) return 1;

    for (i=0; i<n; i++) {
        if (b1[i]!=b2[i]) return 1;
    }

    return 0;
}


/**
int  strnrvscmp(const char* s1, const char* s2, int n) 

文字列 s1と s2を後ろから n文字比較する．

@retval 0  一致する
@retval 1  一致しない

@attention strncmp()の戻り値の定義とは若干異なるので注意すること．
*/
int  strnrvscmp(const char* s1, const char* s2, int n) 
{
    int j1, j2;
    
    if (s1==NULL || s2==NULL) return 1;

    j1 = (int)strlen(s1) - 1;
    j2 = (int)strlen(s2) - 1;

    while (n>0 && j1>=0 && j2>=0) {
        if (s1[j1--] != s2[j2--]) return 1;
        n--;
    }

    if (n==0) return 0;
    else      return 1;
}


/**
int  strncaservscmp(const char* s1, const char* s2, int n) 

文字列 s1と s2を後ろから n文字比較する．

@retval 0  一致する
@retval 1  一致しない

@attention strncmp()の戻り値の定義とは若干異なるので注意すること．
*/
int  strncaservscmp(const char* s1, const char* s2, int n) 
{
    int j1, j2;
    
    if (s1==NULL || s2==NULL) return 1;

    j1 = (int)strlen(s1) - 1;
    j2 = (int)strlen(s2) - 1;

    while (n>0 && j1>=0 && j2>=0) {
        if (toupper(s1[j1--]) != toupper(s2[j2--])) return 1;
        n--;
    }

    if (n==0) return 0;
    else      return 1;
}


/**
char*  strstrcase(const char* buf, const char* nd)

文字列 bufの中に部分文字列 ndがあるかどうかをチェックする．大文字小文字を区別しない．    

大文字小文字を区別しない点を除けば，strstr() と同じ．@n
つまり strcasestr() と同じ．標準で strcasestr()が存在しない場合に使用．

@param  buf  検索対象の文字列
@param  nd   検索する部分文字列
@return 部分文字列の開始を指すポインタ．部分文字列が見つからない場合は NULL

@see strstr(), strcasestr()
*/
char*  strstrcase(const char* buf, const char* nd)
{
    char* pp;
    char* pb;
    char* pn;

    if (buf==NULL || nd==NULL) return NULL;

    pb = (char*)malloc(strlen(buf)+1);
    if (pb==NULL) return NULL;
    memcpy(pb, buf, (int)strlen(buf)+1);

    pn = (char*)malloc(strlen(nd)+1);
    if (pn==NULL) {
        free(pb);
        return NULL;
    }
    memcpy(pn, nd, (int)strlen(nd)+1);

    upper_string(pb);
    upper_string(pn);

    pp = strstr(pb, pn);
    if (pp!=NULL) {
        pp = (pp - pb) + (char*)buf;
    }
    free(pb);
    free(pn);

    return pp;
}


/**
int  ex_strncmp(const char* dat, const char* key, int len) 

拡張文字比較

@param  dat   比べる文字列1．
@param  key   比べる文字列2．
@param  len    1以上 一致させる長さ．
@param  len    0     完全一致．
@param  len   -1     dat の長さに合わせる．
@param  len   -2     key の長さに合わせる．
  
@retval TRUE  一致した
@retval FALSE 一致しなかった
*/
int  ex_strncmp(const char* dat, const char* key, int len) 
{
    if (dat==NULL || key==NULL) return FALSE;

    if (len==0) {
        if (!strcmp(dat, key)) return TRUE;
    }
    else if (len>0) {
        if (!strncmp(dat, key, len)) return TRUE;
    }
    else if (len<0) {
        int sz;
        if (len==-1) sz = (int)strlen(dat);
        else         sz = (int)strlen(key);
        if (!strncmp(dat, key, sz)) return TRUE;
    }
    
    return FALSE;
}


/**
int  ex_strncasecmp(const char* dat, const char* key, int len) 

拡張文字比較．ケース無視．

@param  dat   比べる文字列1．
@param  key   比べる文字列2．
@param  len    1以上 一致させる長さ．
@param  len    0     完全一致．
@param  len   -1     dat の長さに合わせる．
@param  len   -2     key の長さに合わせる．
  
@retval TRUE  一致した
@retval FALSE 一致しなかった
*/
int  ex_strncasecmp(const char* dat, const char* key, int len) 
{
    if (dat==NULL || key==NULL) return FALSE;

    if (len==0) {
        if (!strcasecmp(dat, key)) return TRUE;
    }
    else if (len>0) {
        if (!strncasecmp(dat, key, len)) return TRUE;
    }
    else if (len<0) {
        int sz;
        if (len==-1) sz = (int)strlen(dat);
        else         sz = (int)strlen(key);
        if (!strncasecmp(dat, key, sz)) return TRUE;
    }
    
    return FALSE;
}


/**
int  ex_strnrvscmp(const char* dat, const char* key, int len) 

拡張文字比較．後ろから比べる．

@param  dat   比べる文字列1．
@param  key   比べる文字列2．
@param  len    1以上 一致させる長さ．
@param  len    0     完全一致．
@param  len   -1     dat の長さに合わせる．
@param  len   -2     key の長さに合わせる．
   
@retval TRUE  一致した
@retval FALSE 一致しなかった
*/
int  ex_strnrvscmp(const char* dat, const char* key, int len) 
{
    if (dat==NULL || key==NULL) return FALSE;

    if (len==0) {
        if (!strcmp(dat, key)) return TRUE;
    }
    else if (len>0) {
        if (!strnrvscmp(dat, key, len)) return TRUE;
    }
    else if (len<0) {
        int sz;
        if (len==-1) sz = (int)strlen(dat);
        else         sz = (int)strlen(key);
        if (!strnrvscmp(dat, key, sz)) return TRUE;
    }
    
    return FALSE;
}


/**
int  ex_strncaervscmp(const char* dat, const char* key, int len) 

拡張文字比較．後ろから比べる．ケース無視．

@param  dat   比べる文字列1．
@param  key   比べる文字列2．
@param  len    1以上 一致させる長さ．
@param  len    0     完全一致．
@param  len   -1     dat の長さに合わせる．
@param  len   -2     key の長さに合わせる．

@retval TRUE  一致した
@retval FALSE 一致しなかった
*/
int  ex_strncaservscmp(const char* dat, const char* key, int len) 
{
    if (dat==NULL || key==NULL) return FALSE;

    if (len==0) {
        if (!strcasecmp(dat, key)) return TRUE;
    }
    else if (len>0) {
        if (!strncaservscmp(dat, key, len)) return TRUE;
    }
    else if (len<0) {
        int sz;
        if (len==-1) sz = (int)strlen(dat);
        else         sz = (int)strlen(key);
        if (!strncaservscmp(dat, key, sz)) return TRUE;
    }
    
    return FALSE;
}


/**
char*  chomp(char* buf)

最初の改行コード以降を無視する．最初の改行コードのある場所に '@\0' を代入する．

@param[in,out]  buf  改行を無効にする文字列．
@return 処理された buf[] へのポインタ．
*/
char*  chomp(char* buf)
{
    int i, len;
    
    if (buf==NULL) return NULL;

    len = (int)strlen(buf);
    for (i=0; i<len; i++) {
        if (buf[i]==CHAR_LF || buf[i]==CHAR_CR) {
            buf[i] = '\0';
            return buf;
        }
    }
    return buf;
}


/**
char*  skip_chars(char* pp, const char* check)

ppの指す文字列中で，check[]に格納されたいずれかの文字までポインタをスキップさせる．@n
ただし，" ", ' ' 内に該当文字があった場合，それらは無視される．

@param  pp     文字列の先頭ポインタ
@param  check  スキップする文字を格納した文字列．
@return check[]の中で最初に見つかった文字へのポインタ．
*/
char*  skip_chars(char* pp, const char* check)
{
    int  n, sqf = 0, dqf = 0;

    n = (int)strlen(check);

    while (*pp!='\0') {
        while (*pp=='\\') pp += 2;
        //
        if (*pp!='\0') {
            if      (!sqf && *pp=='\"') dqf = 1 - dqf;
            else if (!dqf && *pp=='\'') sqf = 1 - sqf;

            else if (!sqf && !dqf) {
                int i;
                for (i=0; i<n; i++) {
                    if (*pp==check[i]) return pp;
                }
            }
            pp++;
        }
    }

    return NULL;
}


/**
char*  skip_char(char* pp, char cc)

文字列 pp の中で，次の cc までポインタをスキップさせる．@n
ただし，" ", ' ' 内に該当文字があった場合，それらは無視される．

@param  pp   文字列の先頭ポインタ
@param  cc   スキップする文字を格納した文字列．
@return check[]の中で最初に見つかった文字へのポインタ．
*/
char*  skip_char(char* pp, char cc)
{
    int  sqf = 0, dqf = 0;

    while (*pp!='\0') {
        while (*pp=='\\') pp += 2;
        //
        if (*pp!='\0') {
            if      (!sqf && *pp=='\"') dqf = 1 - dqf;
            else if (!dqf && *pp=='\'') sqf = 1 - sqf;
            else if (!sqf && !dqf && *pp==cc) return pp;
            pp++;
        }
    }

    return pp;
}


/**
char*   skip_char_pair(char* pp, char pair, char end)

pair から始まって end で閉じるまで，ポインタをスキップさせる．ただし " " と ' ' 内は完全スキップ@n
pair と end は対になっている必要がある．@n
pp は pair の位置を指しているのが望ましい．指していなくても動くけど．．．．@n

ペアの end がない場合は '@\0' へのポインタを返す．

@param pp    操作対象の文字列
@param pair  ペアとなる最初の文字 
@param end   ペアとなる最後の文字
@return ペアとなった最後の文字へのポインタ

@code
pt = skip_char_pair(pp, '<', '>');
@endcode
*/
char*   skip_char_pair(char* pp, char pair, char end)
{
    int cnt = 0, sqf = 0, dqf = 0;

    if (*pp==pair) {
        cnt = 1;
        pp++;
    }

    while (*pp!='\0') {
        while (*pp=='\\') pp += 2;
        //
        if (*pp!='\0') {
            if      (!sqf && *pp=='\"') dqf = 1 - dqf;
            else if (!dqf && *pp=='\'') sqf = 1 - sqf;

            else if (!sqf && !dqf) {
                if      (*pp==end)  cnt--;
                else if (*pp==pair) cnt++;
                if (cnt==0) return pp;
            }
            pp++;
        }
    }

    return pp;
}


/**
char*  skip_string_end(char* pp)

文字列の最後までポインタをスキップする．@n
ppの指す文字列 "〜" または '〜' の最後の クォーテーションへのポインタ

@param  pp   文字列の先頭の クォーテーションへのポインタ 
@return      文字列の最後の クォーテーションへのポインタ 
*/
char*  skip_string_end(char* pp)
{
    char ch = *pp;

    if (*pp=='\"' || *pp=='\'') {
        pp++;
        //
        while (*pp!='\0') {
            while (*pp=='\\') pp += 2;
            //
            if (*pp!='\0') {
                if (*pp==ch) return pp;
            }
            pp++;
        }
    } 
    return pp;
}


/**
char*  pack_head_tail_char(char* mesg, char cc)

文字列の先頭のcc(複数)，終わりのcc(複数)，TAB, CR, LF を削除

文字列の先頭，終わり部分でない場所にある ccは削除されない．
返されたデータは free()する必要がある．

@param  mesg  操作対象の文字列
@param  cc    削除する文字
@return 変換後の文字列．要 free
*/
char*  pack_head_tail_char(char* mesg, char cc)
{
    int   sz;
    char* ps;
    char* pe;
    char* pp;

    if (mesg==NULL) return NULL;

    ps = mesg;
    while(*ps==cc || *ps==CHAR_TAB || *ps==CHAR_LF || *ps==CHAR_CR) ps++;
    pe = ps;
    while(*pe!='\0') pe++;
    pe--;
    while(*pe==cc || *pe==CHAR_TAB || *pe==CHAR_LF || *pe==CHAR_CR) pe--;

    sz = (int)(pe - ps) + 1;
    pp = (char*)malloc((size_t)(sz + 1));
    memcpy(pp, ps, sz);
    pp[sz] = '\0';

    return pp;        
}


/**
char*  pack_char(char* mesg, char cc)

文字列中及び，先頭のcc(複数)，終わりのcc(複数)，CR, LF を削除

タブは空白に変換．メッセージ中の連続する cc は 1個に変換．
返されたデータは free()する必要がある．

CR, LF も削除（無視）されるので注意．

@param  mesg  操作対象の文字列
@param  cc    削除する文字
@return 変換後の文字列．要 free
*/
/*
char*  pack_char(char* mesg, char cc)
{
    return pack_char_len(mesg, cc, -1);
}
*/


/**
char*  pack_char_len(char* mesg, char cc, int len)

文字列の一部の先頭のcc(複数)，終わりのcc(複数)，CR, LF を削除

タブは空白に変換．メッセージ中の連続する cc は 1個に変換．
返されたデータは free()する必要がある．

CR, LF も削除（無視）されるので注意．

@param  mesg  操作対象の文字列
@param  cc    削除する文字
@param  len   操作する文字列の長さ
@return 変換後の文字列．要 free
*/
char*  pack_char_len(char* mesg, char cc, int len)
{
    int i, j;
    int sf = OFF;    // 連続 cc 用のフラグ
    int ff = ON;     // 先頭の cc を無視するためのフラグ
    char* pp;

    if (mesg==NULL) return NULL;

    if (len==0) return NULL;
    if (len<0)  len = (int)strlen(mesg);
    pp = (char*)malloc(len+1);
    if (pp==NULL) return NULL;
    memset(pp, 0, len+1);

    i = j = 0;
    while(mesg[i]!='\0' && i<len) {
        if (mesg[i]==CHAR_TAB)  mesg[i] = ' ';
        if (mesg[i]!=CHAR_CR && mesg[i]!=CHAR_LF) {        // CR LF を読み飛ばす．
            if (mesg[i]==cc) {
                if (sf==OFF) {
                    sf = ON;
                    if (ff==ON) ff = OFF;
                    else  pp[j++] = mesg[i];
                }
            }
            else {
                sf = ff = OFF;
                pp[j++] = mesg[i];
            }
        }
        i++;
    }
    pp[j] = '\0';

    // 末尾の cc を削除
    i = (int)strlen(pp) - 1;
    while(i>=0) {
        if (pp[i]==cc) i--; 
        else  break;
    }
    pp[i+1] = '\0';

    return pp;
}


/**
char*  change_esc(char* mesg)

文字列中の CR, LF を @\r, @\n に変換する．
返されたデータは free()する必要がある．

@param  mesg  操作対象の文字列
@return 変換後の文字列．要 free
*/
char*  change_esc(char* mesg)
{
    int i, j;
    char* pp;

    if (mesg==NULL) return NULL;

    pp = (char*)malloc((strlen(mesg)+1)*2);
    if (pp==NULL) return NULL;
    memset(pp, 0, (strlen(mesg)+1)*2);

    i = j = 0;
    while(mesg[i]!='\0') {
        if (mesg[i]==CHAR_CR) {
            pp[j++] = '\\';
            pp[j++] = 'r';
        }
        else if (mesg[i]==CHAR_LF) {
            pp[j++] = '\\';
            pp[j++] = 'n';
        }
        else {
            pp[j++] = mesg[i];
        }
        i++;
    }
    pp[j] = '\0';
    
    return pp;
}


/**
void   replace_char(unsigned char* buf, int len, unsigned char frm, unsigned char toc)

buf中の frm (char) を tosで置き換える．

@param      buf  操作対象のデータ．バイナリも可．
@param      len  bufの領域の大きさ（長さ）'@\0'を含む．<= 0 の場合は buf を文字列として扱う．
@param      frm  変換するバイトデータ
@param      tos  変換後の倍とデータ
*/
void   replace_char(unsigned char* buf, int len, unsigned char frm, unsigned char toc)
{
    if (buf==NULL) return;
    if (len<=0) len = (int)strlen((char*)buf) + 1;

    int i;
    for (i=0; i<len; i++) {
        if (buf[i]==frm) buf[i] = toc;
    }

    return;
}


/** 
char*  replace_str(char* buf, int len, const char* frm, const char* tos)

buf中の文字列 frmを tosで置き換える．

bufには十分な領域がなければならない．frmは bufとメモリ領域が重なってはいけない．@n
失敗した場合は NULLを返し，bufの内容は変化しない．

@param[in]  buf  操作対象の文字列
@param[out] buf  変換された文字列
@param      len  buf中の変換対象のbufの領域の大きさ（長さ）
@param      frm  変換する文字列
@param      tos  変換後の文字列

@return buf へのポインタ．失敗した場合は NULL．freeしてはいけない．
*/
char*  replace_str(char* buf, int len, const char* frm, const char* tos)
{
    char* wrk = NULL;
    int i, j, k, slen, flen, tlen;

    if (buf==NULL || frm==NULL || tos==NULL) return NULL;
    if (len<=0) len = (int)strlen(buf) + 1;

    wrk = (char*)malloc(len);
    if (wrk==NULL) return NULL;
    memset(wrk, 0, len);

    slen = (int)strlen(buf);
    flen = (int)strlen(frm);
    tlen = (int)strlen(tos);

    i = j = 0;
    while (i<slen && j<len) {
        if (!strncmp((const char*)(buf+i), frm, flen)){
            for (k=0; k<tlen; k++) {
                wrk[j+k] = tos[k];
            }
            i += flen;
            j += tlen;
        }
        else {
            wrk[j++] = buf[i++];
        }
    }

    //
    if (j>=len) {
        free(wrk);
        return NULL;
    }

    wrk[j] = '\0';
    for (k=0; k<=j; k++) buf[k] = wrk[k];
    free(wrk);

    return buf;
}


/**
char*  cut_str(char* buf, int ls, int le)

buf[ls] 〜 buf[le] を切り出してメモリに格納して返す．
返されたデータは free()する必要がある．

@param  buf  操作対象の文字列
@param  ls   切り出し開始の位置
@param  le   切り出し終了の位置

@return 切り出した文字列．要 free
*/
char*  cut_str(char* buf, int ls, int le)
{
    int   i, len;
    char* ret;

    if (buf==NULL) return NULL;

    ls  = Max(ls, 0);
    le  = Min(le, (int)strlen((const char*)buf)); 
    len = le - ls + 1;
    if (len<=0) return NULL;

    ret = (char*)malloc(len+1);
    if (ret==NULL) return NULL;
    memset(ret, 0, len+1);

    for (i=0; i<len; i++) ret[i] = buf[i+ls];

    return ret;
}


/**
char*  dup_str(char* buf)

文字列 bufの複製を作り出す．返されたデータは free()する必要がある．

@param  buf  コピー元の文字列
@return 複製された文字列．要 free
*/
char*  dup_str(char* buf)
{
    int   len;
    char* ret;

    if (buf==NULL) return NULL;

    len = (int)strlen(buf);
    ret = (char*)malloc(len+1);
    if (ret==NULL) return NULL;

    memcpy(ret, buf, len);
    ret[len] = '\0';

    return ret;
}


/**
int   is_number(unsigned char* str)

文字列が数字かどうか反転する．

@param  検査する文字列
@return 0: 数字でない．1: 整数．2: 小数点付き数字．
*/
int   is_number(unsigned char* str)
{
    if (str==NULL) return 0;
    int i, len = (int)strlen((char*)str);

    int cnt = 0;
    for (i=0; i<len; i++) {
        if (str[i]=='.') cnt += 1;
        else if (str[i]<'0' || str[i]>'9') return 0;
    }
    if (cnt>1) return 0;
    else if (cnt>0) return 2;
    return 1;
}


static char  _ToSTR[LDATA];        ///< 作業用（文字変換）

/**
char*  itostr(int n)

int型 nを文字列にして返す．

@param  n  文字列に変換する値
@return 変換された文字列．free() してはいけない．
*/
char*  itostr(int n)
{
    memset(_ToSTR, 0, LDATA);
    snprintf(_ToSTR, LDATA-1, "%d", n);
    return _ToSTR;
}


/**
char*  ltostr(long int n)

long int型 nを文字列にして返す．

@param  n  文字列に変換する値
@return 変換された文字列．free() してはいけない．
*/
char*  ltostr(long int n)
{
    memset(_ToSTR, 0, LDATA);
    snprintf(_ToSTR, LDATA-1, "%ld", n);
    return _ToSTR;
}


/**
char*  ultostr(unsigned long int n)

unsigned long int型 nを文字列にして返す．

@param  n  文字列に変換する値
@return 変換された文字列．free() してはいけない．
*/
char*  ultostr(unsigned long int n)
{
    memset(_ToSTR, 0, LDATA);
    snprintf(_ToSTR, LDATA-1, "%lu", n);
    return _ToSTR;
}


/**
char*  lltostr(long long int n)

long long int型 nを文字列にして返す．

@param  n  文字列に変換する値
@return 変換された文字列．free() してはいけない．
*/
char*  lltostr(long long int n)
{
    memset(_ToSTR, 0, LDATA);
    snprintf(_ToSTR, LDATA-1, "%lld", n);
    return _ToSTR;
}


/**
char*  ulltostr(unsigned long long int n)

unsigned long long int型 nを文字列にして返す．

@param  n  文字列に変換する値
@return 変換された文字列．free() してはいけない．
*/
char*  ulltostr(unsigned long long int n)
{
    memset(_ToSTR, 0, LDATA);
    snprintf(_ToSTR, LDATA-1, "%llu", n);
    return _ToSTR;
}


/**
char*  ftostr(float n)

float型 nを文字列にして返す．

@param  n  文字列に変換する値
@return 変換された文字列．free() してはいけない．
*/
char*  ftostr(float n)
{
    memset(_ToSTR, 0, LDATA);
    snprintf(_ToSTR, LDATA-1, "%f", n);
    return _ToSTR;
}


/**
char*  dtostr(double n)

double型 nを文字列にして返す．

@param  n  文字列に変換する値
@return 変換された文字列．free() してはいけない．
*/
char*  dtostr(double n)
{
    memset(_ToSTR, 0, LDATA);
    snprintf(_ToSTR, LDATA-1, "%f", n);
    return _ToSTR;
}


/**
char*  itostr_ts(int n)

int型 nを文字列にして返す．Thread Safe

@param  n  文字列に変換する値
@return 変換された文字列．要 free．
*/
char*  itostr_ts(int n)
{
    char* str = NULL;
    str = (char*)malloc(LDATA);
    if (str==NULL) return NULL;

    memset(str, 0, LDATA);
    snprintf(str, LDATA-1, "%d", n);
    return str;
}


/**
char*  ltostr_ts(long int n)

long int型 nを文字列にして返す．Thread Safe

@param  n  文字列に変換する値
@return 変換された文字列．要 free．
*/
char*  ltostr_ts(long int n)
{
    char* str = NULL;
    str = (char*)malloc(LDATA);
    if (str==NULL) return NULL;

    memset(str, 0, LDATA);
    snprintf(str, LDATA-1, "%ld", n);
    return str;
}


/**
char*  ultostr_ts(unsigned long int n)

unsigned long int型 nを文字列にして返す．Thread Safe

@param  n  文字列に変換する値
@return 変換された文字列．要 free．
*/
char*  ultostr_ts(unsigned long int n)
{
    char* str = NULL;
    str = (char*)malloc(LDATA);
    if (str==NULL) return NULL;

    memset(str, 0, LDATA);
    snprintf(str, LDATA-1, "%lu", n);
    return str;
}


/**
char*  lltostr_ts(long long int n)

long long int型 nを文字列にして返す．Thread Safe

@param  n  文字列に変換する値
@return 変換された文字列．要 free．
*/
char*  lltostr_ts(long long int n)
{
    char* str = NULL;
    str = (char*)malloc(LDATA);
    if (str==NULL) return NULL;

    memset(str, 0, LDATA);
    snprintf(str, LDATA-1, "%lld", n);
    return str;
}


/**
char*  ulltostr_ts(unsigned long long int n)

unsigned long long int型 nを文字列にして返す． Thread Safe

@param  n  文字列に変換する値
@return 変換された文字列．要 free．
*/
char*  ulltostr_ts(unsigned long long int n)
{
    char* str = NULL;
    str = (char*)malloc(LDATA);
    if (str==NULL) return NULL;

    memset(str, 0, LDATA);
    snprintf(str, LDATA-1, "%llu", n);
    return str;
}


/**
char*  ftostr_ts(float n)

float型 nを文字列にして返す． Thread Safe

@param  n  文字列に変換する値
@return 変換された文字列．要 free．
*/
char*  ftostr_ts(float n)
{
    char* str = NULL;
    str = (char*)malloc(LDATA);
    if (str==NULL) return NULL;

    memset(str, 0, LDATA);
    snprintf(str, LDATA-1, "%f", n);
    return str;
}


/**
char*  dtostr_ts(double n)

double型 nを文字列にして返す． Thread Safe

@param  n  文字列に変換する値
@return 変換された文字列．要 free．
*/
char*  dtostr_ts(double n)
{
    char* str = NULL;
    str = (char*)malloc(LDATA);
    if (str==NULL) return NULL;

    memset(str, 0, LDATA);
    snprintf(str, LDATA-1, "%f", n);
    return str;
}


/**
int  count_lines(const char* buf)

文字列データ bufの行数を数える．
行頭が '@\0' であっても 1行と数える

@param buf 行数を数える文字列データ
@return 行数．
*/
int  count_lines(const char* buf)
{
    int i    = 0;
    int line = 0;
    
    if (buf==NULL) return 0;
    line++;

    while(buf[i]!='\0') {
        if (buf[i]==CHAR_CR) {
            line++;
            if (buf[i+1]==CHAR_LF) i++;
        }
        else if (buf[i]==CHAR_LF) {
            line++;
        }
        i++;
    }

    return line;
}


/**
int hexstr2i(const char* str)

16進数の文字列を整数に変換する．

空白は無視する．空白以外の16進数でない文字が出現したら，そこで変換を止める

@param  str  整数に変換する 16進の文字列 
@return 変換された整数
*/
int hexstr2i(const char* str)
{
    int i, ret = 0;
    if (str==NULL) return ret;

    int len = (int)strlen(str);

    for (i=0; i<len; i++) {
        if (str[i]>='0' && str[i]<='9') {
            ret = ret*16;
            ret += (int)(str[i] - '0');
        }
        else if (str[i]>='A' && str[i]<='F') {
            ret = ret*16;
            ret += (int)(str[i] - 'A') + 10;
        }
        else if (str[i]>='a' && str[i]<='f') {
            ret = ret*16;
            ret += (int)(str[i] - 'a') + 10;
        }
        else if (str[i]!=' ') {
            break;
        }
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////

#ifndef WIN32

/**
void  init_rand(void)

/dev/urandom から乱数の系列を初期化する

/dev/urandom が読めない場合は，呼び出された時の時間(秒数)によって乱数の系列を初期化する．

@attention Windows では使用できない
*/
void  init_rand(void)
{
    FILE* fp;
    unsigned int seed;
    size_t sz;
    UNUSED(sz);

    fp = fopen("/dev/urandom", "rb");
    if (fp==NULL) {
        srand(time(NULL));
        return;
    }
    sz = fread(&seed, sizeof(unsigned int), 1, fp);
    fclose(fp);

    srand(seed);
}

#endif


/**
char*  randstr(int n)

ランダムに A-Za-z0-9 までの文字を n文字生成する．
返されたデータは free()する必要がある．

@param  n  出力する文字数．
@return ランダムに生成された n文字の文字列．要 free
*/
char*  randstr(int n)
{
    char   base[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    char*  pass;
    int    i, sz;

    if (n<=0) return NULL;
    pass = (char*)malloc(n+1);
    if (pass==NULL) return pass;

    sz = (int)strlen(base);
    for (i=0; i<n; i++) pass[i] = base[rand()%sz];
    pass[n] = '\0';

    return pass;
}


/**
unsigned char*  randbit(int n)

ランダムに n bitのバイナリを生成する．
返されたデータは free()する必要がある．

@param  n  出力する bit数．
@return ランダムに生成された n bitのデータ．要 free
*/
unsigned char*  randbit(int n)
{
    int   i, sz;
    unsigned char* bin;

    sz = (n+7)/8;
    bin = (unsigned char*)malloc(sz);
    if (bin==NULL) return NULL;

    memset(bin, 0, sz);
    for (i=0; i<n; i++) setBit(bin, i, rand()%2);

    return bin;    
}


///////////////////////////////////////////////////////////////////////////////

//
// 64bit 変換
unsigned long long int ntohull(unsigned long long int s)
{
    if (is_little_endian()) {
        long long int d;
        uByte* src = (uByte*)&s;
        uByte* dst = (uByte*)&d;
        dst[0] = src[7];
        dst[1] = src[6];
        dst[2] = src[5];
        dst[3] = src[4];
        dst[4] = src[3];
        dst[5] = src[2];
        dst[6] = src[1];
        dst[7] = src[0];
        return d;
    }
    return s;
}


/** void  swap_byte(void* p, int s, int c)

上位バイトと下位バイトを逆順にする．

pが指す長さ sのデータを cバイトづつ逆順にする．

@param[in,out] p  データへのポインタ．
@param         s  データの長さ．バイト単位．
@param         c  逆順にするデータの長さ．バイト単位．
*/
void  swap_byte(void* p, int s, int c)
{
    int    i;
    uByte* u = (uByte*)p;
    for (i=0; i<s; i+=c) reverse_str(&u[i], c);
}

 

/**
short  swaps(unsigned short p)

short型（16bit）pの上下8bitを入れ替える．
ntohs(), htons()の代替関数．Windows用．

@param  p  入れ替える値
@return 処理後の値
*/
short  swaps(unsigned short p)
{
    short  r;
    uByte* t;
    uByte* f;
     
    t = (uByte*)&r;
    f = (uByte*)&p;
    t[0] = f[1];
    t[1] = f[0];

    return r;
}


/** int  swapl(unsigned int p)

int型 32bit pを8bitづつ逆順にする

ntohl(), htonl()の代替関数．Windows用．

@param  p  入れ替える値
@return 処理後の値
*/
int  swapl(unsigned int p)
{
    int    r;
    uByte* t;
    uByte* f;
     
    t = (uByte*)&r;
    f = (uByte*)&p;
    t[0] = f[3];
    t[1] = f[2];
    t[2] = f[1];
    t[3] = f[0];

    return r;
}


/** 
long long int swapd(unsigned long long int p)

int型 64bit pを8bitづつ逆順にする

ntohl(), htonl()の代替関数．Windows用．

@param  p  入れ替える値
@return 処理後の値
*/
long long int swapd(unsigned long long int p)
{
    long long int r;
    uByte* t;
    uByte* f;
     
    t = (uByte*)&r;
    f = (uByte*)&p;
    t[0] = f[7];
    t[1] = f[6];
    t[2] = f[5];
    t[3] = f[4];
    t[4] = f[3];
    t[5] = f[2];
    t[6] = f[1];
    t[7] = f[0];

    return r;
}


/**
void  reverse_str(uByte* p, int s)

バイト列を逆順にする．

@param[in,out] p  バイトデータへのポインタ．
@param s バイトデータの長さ．
*/
void  reverse_str(uByte* p, int s)
{
    int    i;
    uByte  c;
     
    if (s<=0) s = (int)strlen((const char*)p);

    for (i=0; i<s/2; i++) {
        c        = p[s-1-i];
        p[s-1-i] = p[i];
        p[i]     = c;
    }
}   



///////////////////////////////////////////////////////////////////////////////

/**
int  file_from_to(const char* src, const char* dst, const char* mode)

modeに従って,ファイル srcを dstにコピーする．

@param  src   コピー元(ソース)ファイル名
@param  dst   コピー先(ディスティネーション)ファイル名
@param  mode  コピー先ファイルのオープンモード．fopenに同じ．"r", "w", "a", "r+", "w+", "a+", ....

@retval 0以上                     コピーしたサイズ
@retval JBXL_MALLOC_ERROR         作業用メモリが足りない．
@retval JBXL_FILE_EXIST_ERROR     ソースファイルが存在しない．(シンボリックリンクも×)
@retval JBXL_FILE_OPEN_ERROR      ソースファイルのオープン失敗
@retval JBXL_FILE_DESTOPEN_ERROR  ディスティネーションファイルのオープン失敗
 */
int  file_from_to(const char* src, const char* dst, const char* mode)
{
    long int  sz;
    unsigned char* buf;
    FILE*  fp;
    size_t rs;
    UNUSED(rs);

    sz = file_size(src);
    if (sz<0) return JBXL_FILE_EXIST_ERROR;
    buf = (unsigned char*)malloc(sz);
    if (buf==NULL) return JBXL_MALLOC_ERROR;
    memset(buf, 0, sz);

    fp = fopen(src, "rb");
    if (fp==NULL) {
        free(buf);
        return JBXL_FILE_OPEN_ERROR;
    }
    rs = fread(buf, sz, 1, fp);
    fclose(fp);

    fp = fopen(dst, mode);
    if (fp==NULL) {
        free(buf);
        return JBXL_FILE_DESTOPEN_ERROR;
    }
    fwrite(buf, sz, 1, fp);
    fclose(fp);

    free(buf);
    return sz;
}


/**
int  fp_from_to(FILE* src, FILE* dst, long int sz)

ファイル srcを dstにコピーする．

@param  src  コピー元(ソース)のファイルポインタ
@param  dst  コピー先(ディスティネーション)のファイルポインタ
@param  sz   コピー元(ソース)のサイズ

@retval 0以上                     コピーしたサイズ
@retval JBXL_MALLOC_ERROR         作業用メモリが足りない．
@retval JBXL_ARGS_ERROR           ファイルサイズの指定がおかしい．
@retval JBXL_FILE_OPEN_ERROR      ソースファイルのオープン失敗．
@retval JBXL_FILE_DESTOPEN_ERROR  ディスティネーションファイルのオープン失敗．
 */
int  fp_from_to(FILE* src, FILE* dst, long int sz)
{
    unsigned char* buf;
    size_t rs;
    UNUSED(rs);

    if (sz<=0)     return JBXL_ARGS_ERROR;
    if (src==NULL) return JBXL_FILE_OPEN_ERROR;
    if (dst==NULL) return JBXL_FILE_DESTOPEN_ERROR;

    buf = (unsigned char*)malloc(sz);
    if (buf==NULL) return JBXL_MALLOC_ERROR;
    memset(buf, 0, sz);

    rs = fread(buf, sz, 1, src);
    sz = (int)fwrite(buf, sz, 1, dst);

    free(buf);
    return sz;
}


/**
char* get_file_name(const char* str)

フルパスからファイル名へのポインタを取り出す．
str中のポインタを返すので，free() してはいけない．

@param  str  ファイルのフルパス
@return str中のファイル名へのポインタ
*/
char* get_file_name(const char* str)
{
    if (str==NULL) return NULL;

#ifdef WIN32
    char  cc = '\\';
#else
    char  cc = '/';
#endif

    int len = (int)strlen(str);
    int sz  = 0;
    while (sz<len && str[len-1-sz]!=cc) sz++;
    char* fnm = (char*)str + len - sz; 

    return fnm;
}


/**
char* del_file_name(const char* str)

フルパスからファイル名を削除する．
strの中身を書き換えて，strを返す．free() してはいけない．

@param  str  ファイルのフルパス
@return str中のファイル名へのポインタ
*/
char* del_file_name(const char* str)
{
    if (str==NULL) return NULL;
    
#ifdef WIN32
    char  cc = '\\';
#else
    char  cc = '/';
#endif

    char* path = (char*)str;
    int len = (int)strlen(str);
    if (len==1) {
        if (path[0]==cc) return path;
        else return NULL;
    }

    int sz = len - 1;
    while (sz>=0 && str[sz]!=cc) sz--;

    if (sz<0) {
        path[0] = '.';
        path[1] = cc;
        path[2] = '\0';
    }
    else {
        path[sz+1] = '\0';
    }

    return path;
}


/**
char* make_file_path(const char* str)

フルパスからパスを生成する．strは変化しない．要 free．

非推奨：get_file_path() を使え．

@param  str  ファイルのフルパス
@return ファイルのフルパス（ファイル名を含まない）．
*/
/*
char* make_file_path(const char* str)
{
    if (str==NULL || str[0]=='\0') return NULL;

    int len = (int)strlen(str);
    char* path = (char*)malloc(len+1);
    memcpy(path, str, len+1);

#ifdef WIN32
    char  cc = '\\';
#else
    char  cc = '/';
#endif

    if (len==1) {
        if (path[0]==cc) return path;
        else {
            free(path);
            return NULL;
        }
    }

    int sz = len - 1;
    while (sz>=0 && path[sz]!=cc) sz--;

    if (sz<0) {
        if (len>=2) {
            path[0] = '.';
            path[1] = cc;
            path[2] = '\0';
        }
        else {
            free(path);
            path = NULL;
        }
    }
    else {
        path[sz+1] = '\0';
    }

    return path;
}
*/


/**
char* get_file_path(const char* str)

ファイルパスからパス部分のみを得る．strは変化しない．要 free．

@param  str  ファイルのパス
@return ファイルのパス（ファイル名を含まない）．最後の '/' まはた '\' 付き
*/
char* get_file_path(const char* str)
{
    if (str==NULL || str[0]=='\0') return NULL;

    int len = (int)strlen(str);
    char* path = (char*)malloc(len + 1);
    memcpy(path, str, len + 1);

    char  cc1 = '\\';
    char  cc2 = '/';

    int sz = len - 1;
    while (sz>=0 && path[sz]!=cc1 && path[sz]!=cc2) sz--;

    if (sz<0) {
        free(path);
        path = NULL;
    }
    else {
        path[sz+1] = '\0';
    }

    return path;
}


/**
char* get_file_extension(const char* str)

フルパスからファイル名の拡張子へのポインタを取り出す．

@param  str  ファイルのフルパス
@return str中のファイル名の拡張子（.を含まない）へのポインタ
*/
char* get_file_extension(const char* str)
{
    int   sz, len;
    char* ext;

    if (str==NULL) return NULL;

    char  pd  = '.';
    char  cc1 = '\\';
    char  cc2 = '/';

    len = (int)strlen(str);
    sz  = 0;
    while (sz<len && str[len-1-sz]!=cc1 && str[len-1-sz]!=cc2 && str[len-1-sz]!=pd) sz++;

    if (str[len-1-sz]!=pd) return NULL;
    ext = (char*)str + len - sz; 

    return ext;
}


/**
char* del_file_extension(const char* str)

フルパスから拡張子を削除する．
strの中身を書き換えて，strを返す．free() してはいけない．

@param  str  ファイルのフルパス
@return str中のファイル名へのポインタ
*/
char* del_file_extension(const char* str)
{
    if (str==NULL) return NULL;
    
    char  cc = '.';
    char* path = (char*)str;

    int len = (int)strlen(path);
    int sz = len - 1;
    while (sz>=0 && path[sz]!=cc) sz--;

    if (sz<1) return path;
    path[sz] = '\0';

    return path;
}


/**
char* cut_file_extension(const char* str)

拡張子を削除したパス名を生成する．strは変化しない．要 free．

@param  str ファイルのフルパス
@return str中のファイル名へのポインタ
*/
char* cut_file_extension(const char* str)
{
    if (str==NULL) return NULL;

    char  cc = '.';
    int len = (int)strlen(str);
    char* path = (char*)malloc(len+1);
    memcpy(path, str, len+1);

    int sz = len - 1;
    while (sz>=0 && path[sz]!=cc) sz--;

    if (sz<1) return path;
    path[sz] = '\0';

    return path;
}


/**
unsigned long int  file_size(const char* fn)                                              

通常のファイルの大きさを返す．シンボリックリンクなどは 0．

@param  fn  ファイル名
@return ファイルのサイズ．ファイルが異常の場合は -2. fn が NULL の場合は -1
 */
unsigned long int  file_size(const char* fn)
{
    if (fn==NULL) return -1;

    struct stat stbuf;                                               
    stat(fn, &stbuf);

#ifdef WIN32
    return stbuf.st_size;
#else
    if (S_ISREG(stbuf.st_mode)) return stbuf.st_size;
    else                        return -2;
#endif
}


/**
int  file_exist(const char* fn)

指定したファイルが存在して読み込み可能であれば「真」をそうでなければ「偽」を返す．

ファイルを一度オープンして確認するので，時間が掛かる．
読み込み可能なファイルを確認するのであれば，file_size()の方が（たぶん）早い．

@param  fn    ファイル名
@retval TRUE  ファイルが存在し,読みこみ可能．
@retval FALSE ファイルが存在しないか,または読みこみ不可能． 
*/
int  file_exist(const char* fn)
{
    FILE* fp;

    if (fn==NULL) return FALSE;

    if ((fp=fopen(fn,"rb"))==NULL) {
        return FALSE;
    }
    fclose(fp);
    return TRUE;
}


/**
FILE*  file_chmod_open(const char* fn, const char* fm, mode_t mode)

指定したファイルが存在して読み込み可能であれば，属性をmode へ変更した後，ファイルを fmモードでオープンしなおして，ファイルポインタを返す．@n
存在しなければ，ファイルを作成して属性を変更し，その後，ファイルを fmモードでオープンしなおして，
ファイルポインタを返す．@n
属性の指定方法については man 2 chmod を参照すること．

@param fn        ファイル名
@param fm        ファイルモード，"r","w","a".....
@param mode      属性．see...  man 2 chmod

@retval NULL以外 オープンしたファイルポインタ
@retval NULL     失敗． 
 */
FILE*  file_chmod_open(const char* fn, const char* fm, mode_t mode)
{
    FILE* fp;

    if (!file_exist(fn)) {
        if ((fp=fopen(fn,"wb"))==NULL) return NULL;
        fclose(fp);
    }

#ifndef WIN32
    fp = NULL;
    if (chmod(fn, mode)) return fp;
#endif

    fp = fopen(fn, fm);
    return fp;
}


/**
char*  temp_filename(const char* dir, int flen)

/dev/urandom を利用して作業用ファイルのランダムなファイル名を得る．

/dev/urandom が読めない場合は randstr() を利用する．
返されたデータは free()する必要がある．

@param  dir   作業ファイルを作るディレクトリ
@param  flen  生成するファイル名の長さ
@return 作業用ファイル名のフルパス．要 free
 */
char*  temp_filename(const char* dir, int flen)
{
    FILE* fp;
    int   i, dlen=0;
    char  nostr = '\0';
    char* fname;
    char* fnbin;
    char* fnb64;
    size_t rs;
    UNUSED(rs);

    if (dir==NULL) dir = &nostr;
    dlen = (int)strlen(dir);
    if (flen<=0)   flen = 15;

    fname = (char*)malloc(dlen+flen+1);
    if (fname==NULL) return NULL;
    memset(fname, 0, dlen+flen+1);

    fnbin = (char*)malloc(flen);
    if (fnbin==NULL) {
        free(fname);
        return NULL;
    }
    memset(fnbin, 0, flen);

    fnb64 = NULL;
    fp = fopen("/dev/urandom", "rb");

    do {
        if (fp!=NULL) {
            rs = fread(fnbin, flen, 1, fp);
            fnb64 = (char*)encode_base64((unsigned char*)fnbin, flen);
            if (fnb64!=NULL) fnb64[flen] = '\0';
        }
        if (fnb64==NULL) fnb64 = randstr(flen);

        for (i=0; i<flen; i++) {
            if (fnb64[i]=='/') fnb64[i] = 'x';
            if (fnb64[i]=='+') fnb64[i] = 'X';
        }

        memset (fname, 0, dlen+flen+1);
        strncpy(fname, dir, dlen);
        strncat(fname, fnb64, flen);
        free(fnb64);
    } while(file_exist(fname)); 

    if (fp!=NULL) fclose(fp);
    free(fnbin);
    return fname;
}


/**
unsigned char*  read_file(const char* fname, long unsigned int* size)

ファイルを読み込んで，データへのポインタを返す．

@param  fname ファイル名
@param  size 読み込んだデータのサイズを返す．
@return 読み込んだデータへのポインタ．
*/
unsigned char*  read_file(const char* fname, long unsigned int* size)
{
    size_t rs;
    UNUSED(rs);

    if (size==NULL) return NULL;

    *size = file_size(fname);
    if (*size<=0) return NULL;

    unsigned char* buf = (unsigned char*)malloc(*size);
    if (buf==NULL) return NULL;
    memset(buf, 0, *size);

    FILE* fp  = fopen(fname, "rb");
    if (fp==NULL) {
        free(buf);
        return NULL;
    }

    rs = fread(buf, *size, 1, fp);
    fclose(fp);

    return buf;
}


/**
unsigned long int  write_file(const char* fname, unsigned char* buf, long unsigned int size)

ファイルにデータを書き込む

@param   fname ファイル名．
@param   buf  書き込むデータへのポインタ．
@param   size 書き込むデータのサイズ．
@return  実際に書き込んだデータサイズ
*/
unsigned long int  write_file(const char* fname, unsigned char* buf, long unsigned int size)
{
    if (size==0) return 0;

    FILE* fp  = fopen(fname, "wb");
    if (fp==NULL) return 0;

    size = (long int)fwrite(buf, size, 1, fp);
    fclose(fp);

    return size;
}


int  mkdirp(const char* path, mode_t mode)
{
    if (path==NULL) return JBXL_ARGS_ERROR;

    long unsigned int lpath  = strlen(path);
    char* file_name = (char*)malloc(lpath + 1);
    memcpy(file_name, path, lpath);
    file_name[lpath] = '\0';
    //
    long unsigned int size = 0;
    int  mark = 1;

    char* wrk = (char*)malloc(lpath + 1);
    while (size < lpath) {
        memcpy(wrk, file_name, lpath + 1);
        int dirs = 0;
        for (long unsigned int ptr=0; ptr<=lpath; ptr++) {
#ifdef WIN32
            if (wrk[ptr]=='\\') dirs++;
#else
            if (wrk[ptr]=='/')  dirs++;
#endif
            if (mark==dirs) {
                wrk[ptr] = '\0';
                struct stat stbuf;
                int ret = stat(wrk, &stbuf);
                if (ret>=0) {
#ifdef WIN32
                    if (!(stbuf.st_mode & _S_IFDIR)) {
#else
                    if (!(stbuf.st_mode & S_IFDIR)) {
#endif
                        return JBXL_DIR_MAKE_ERROR;
                    }
                    break;
                }
                ret = mkdir(wrk, mode);
                if (ret<0) return JBXL_DIR_MAKE_ERROR;
                break;
            }
            size = ptr;
        }
        mark++;
    }
    free(wrk);
    
    return JBXL_NORMAL;
}


/**
char* double_bs(char* str) 

文字列中の バックスペース @\を 2重 @\@\ に変えた文字列を返す．
返されたデータは free()する必要がある．

@param  str  操作対象の文字列
@return 変換された文字列．失敗した場合は NULL．要 free
*/
char* double_bs(char* str) 
{
    int  i, j;
    int  len, bsn=0;
    char* buf;

    len = (int)strlen(str);
    for (i=0; i<len; i++) if (str[i]=='\\') bsn++;
    buf = (char*)malloc(len+bsn+1);
    if (buf==NULL) return NULL;
    memset(buf, 0, len+bsn+1);

    for (i=0,j=0; i<len; i++) {
        buf[j++] = str[i];
        if (str[i]=='\\') buf[j++] = '\\';
    }
    buf[j] = '\0';

    return buf;
}


/**
char*  numbering_name(const char* fmt, int n)

フォーマット fmtに従って，nを含む文字列を作り出す．
返されたデータは free()する必要がある．

@param  fmt  フォーマット用文字列．ex. "%d %f\n" see printf()
@param  n    文字に変換する整数．

@return 出力された文字列．要 free
@see printf()
*/
char*  numbering_name(const char* fmt, int n)
{
    int    len;
    char   fname[LNAME], *fo;

    memset(fname, 0, LNAME);
    snprintf(fname, LNAME-1, fmt, n);
    len = (int)strlen(fname);

    fo = (char*)malloc(len+1);
    if (fo==NULL) return NULL;

    memset(fo, 0, len+1);
    strncpy(fo, fname, len);
    return fo;
}



///////////////////////////////////////////////////////////////////////////////

/**
void kanji_convert(unsigned char* mesg)

大域変数 KnjiCodeに従って漢字コードを変換する．

@param[in,out] mesg  変換する文字列を持つ unsigned char*型データ．内容が書き換えられる．

@attention 現在は KanjiCodeが SJISの場合のみ EUCを SJISに変換する．
@see KanjiCode
 */
void kanji_convert(unsigned char* mesg)
{
    if (KanjiCode==CODE_SJIS) {
        kanji_convert_euc2sjis(mesg);
    }
}


/**
void kanji_convert_euc2sjis(unsigned char* mesg)

EUCをSJISに変換する．

@param[in,out] mesg  変換する文字列を持つ unsigned char*型データ．内容が書き換えられる．
*/
void kanji_convert_euc2sjis(unsigned char* mesg)
{
    int i = 0;
    while(mesg[i]!='\0' && mesg[i+1]!='\0'){
        if (mesg[i]>=0xa1 && mesg[i]<=0xfe){ // EUC
            euc2sjis(&mesg[i], &mesg[i+1]);
            i = i + 2;
        } 
        else  i++;
    }
}


/**
void kanji_convert_sjis2euc(unsigned char* mesg)

SJISをEUCに変換する．

@param[in,out] mesg  変換する文字列を持つ unsigned char*型データ．内容が書き換えられる．
*/
void kanji_convert_sjis2euc(unsigned char* mesg)
{
    int i = 0;
    while(mesg[i]!='\0' && mesg[i+1]!='\0'){
        // if文が変則的なのは，コンパイラで警告が出るため   (GCC) 3.3.2 20031218 
        if ((mesg[i]>=0x81 && mesg[i]<=0x9f) ||
            (mesg[i]>=0xe0 && mesg[i]<=0xfe) || mesg[i]==0xff) { // SJIS
            sjis2euc(&mesg[i], &mesg[i+1]);
            i = i + 2;
        } 
        else  i++;
    }
}


/**
void  euc2sjis(unsigned char *c1, unsigned char *c2)

漢字コード変換．EUC → SJIS
  
@param[in,out] c1  変換する文字の第一バイト．内容が書き換えられる．
@param[in,out] c2  変換する文字の第二バイト．内容が書き換えられる．
*/
void  euc2sjis(unsigned char *c1, unsigned char *c2)
{
    if (*c1%2==0) *c2 -= 0x02;
    else{
        *c2 -= 0x61;
        if (*c2>0x7e) (*c2)++;
    }

    if (*c1<0xdf) {
        (*c1)++;
        *c1 /= 2;
        *c1 += 0x30;
    }
    else {
        (*c1)++;
        *c1 /= 2;
        *c1 += 0x70;
    }
}


/**
void  sjis2euc(unsigned char *c1, unsigned char *c2)

漢字コード変換．SJIS → EUC

@param[in,out] c1  変換する文字の第一バイト．内容が書き換えられる．
@param[in,out] c2  変換する文字の第二バイト．内容が書き換えられる．
*/
void  sjis2euc(unsigned char *c1, unsigned char *c2)
{
    if (*c2<0x9f) {
        if (*c1 < 0xa0) {
            *c1 -= 0x81;
            *c1 *= 2;
            *c1 += 0xa1;
        }
            
        else {
            *c1 -= 0xe0;
            *c1 *= 2;
            *c1 += 0xdf;
        }
        if (*c2>0x7f) (*c2)--;
        *c2 += 0x61;
    }
    else {
        if (*c1<0xa0) {
            *c1 -= 0x81;
            *c1 *= 2;
            *c1 += 0xa2;
        }
        else {
            *c1 -= 0xe0;
            *c1 *= 2;
            *c1 += 0xe0;
        }
        *c2 += 2;
     }
}


/**
void  upper_string(char* str)

str中の小文字を大文字に変換する

@param[in,out] str  変換する文字列へのポインタ．内容が書き換えられる．
*/
void  upper_string(char* str)
{
    int  i;
    for (i=0; i<(int)strlen((const char*)str); i++) {
        if (str[i]>='a' && str[i]<='z') str[i] += 'A' - 'a';
    }
}


/**
unsigned char*  decode_base64(unsigned char* buf, int* sz)

bufを base64からデコードする．

変換は bufの先頭から順次行われる．A-Za-z0-9+/ 以外は無視する(例えば改行コード)．@n
入力バッファ部が 4byte(6bit*4)の場合, 出力バッファ部は 3byte(8bit*3)となる．
入力バッファ部のバイト数が 4の倍数でない場合(不正なデータ), 足りない入力バッファ部には '='が挿入されているものとみなす．

返されたデータは free()する必要がある．

@param         buf  base64からデコードする文字列．
@param[in,out] sz   in:  sz デコードされたデータの長さを格納する intへのポインタ．NULLでも良い．@n
                    out: デコードされたデータのサイズ．
@return base64からデコードされたデータ．要 free

@par 例
@code
    "QQ" をデコードすると 'A',0x00 となる．
@endcode
*/
unsigned char*  decode_base64(unsigned char* buf, int* sz)
{
    int   i, j, cc=0, bas, lt, ln;
    unsigned char* dcd;

    lt = 0;
    ln = (int)strlen((const char*)buf);
    while (buf[lt]!='=' && lt<ln) {
        lt++;
        if (lt==ln) break;
    }

    int len = lt/4*3 + (lt%4)*3/4;
    if (sz!=NULL) *sz = len;

    dcd = (unsigned char*)malloc(len+1);
    if (dcd==NULL) return NULL;
    memset(dcd, 0, len+1);

    for (i=0; i<lt; i++) {
        if      (buf[i]>='A' && buf[i]<='Z') cc = buf[i] - 'A';
        else if (buf[i]>='a' && buf[i]<='z') cc = buf[i] - 'a' + 26;
        else if (buf[i]>='0' && buf[i]<='9') cc = buf[i] - '0' + 52;
        else if (buf[i]=='+')                cc = 62;
        else if (buf[i]=='/')                cc = 63;

        if (cc<64) {
            bas = 32;
            for (j=0; j<6; j++) {
                setBit(dcd, i*6+j, cc/bas);
                cc = cc%bas;
                bas = bas/2;
            }
        }
    }
    return dcd;
}


/**
unsigned char*  encode_base64(unsigned char* buf, int sz)

バイナリデータ bufを base64にエンコードする．

入力バッファ部が 3byte(8bit*3)の場合, 出力バッファ部は 4byte(6bit*4)となる．
入力バッファ部が 6bit境界でない場合, 6bit境界 まで 0が付加されたと見される．@n
出力バッファ部が 4byte境界でない場合, 4byte境界まで '='を付加して出力する． 
また,出力バッファ部での 60byte毎の改行は行わない．@n
一般に n byte 入力された場合, base64の出力の文字数は (n+2)/3*4 byte となる．

返されたデータは free()する必要がある．

@param  buf  base64にエンコードするデータ．
@param  sz   データの長さを示す．-1以下の場合は buf は文字列とみなす．
@return base64にエンコードされた文字列．要 free

@par 例
@code
    'A',0x00,0x01 を文字列(sz=1)とみなして符号化すると "QQ==" となる．
    'A',0x00,0x01 の 3byteを符号化(sz=3)すると "QQAB" となる．
@endcode
*/
unsigned char*  encode_base64(unsigned char* buf, int sz)
{
    unsigned char  base[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned char  bas;
    int   i, j, len;
    unsigned char*  ecd;
    
    if (buf==NULL) return NULL;
    if (sz<0) len = (int)strlen((const char*)buf);
    else      len = sz;
    if (len<=0) return NULL;

    sz  = ((len+2)/3)*4;
    ecd = (unsigned char*)malloc(sz+1);
    if (ecd==NULL) return NULL;
    memset(ecd, 0, sz+1);

    for (i=0; i<sz; i++) {
        bas = 0x00;
        if (len*8 > i*6) {
            for (j=0; j<6; j++) {
                if (len*8 > i*6+j) setBit(&bas, j+2, getBit(buf, i*6+j));
                //else               setBit(&bas, j+2, 0);
            }
            ecd[i] = base[(int)bas];
        }
        else {
            ecd[i] = '=';
        }
    }
    return ecd;
}


/**
unsigned char*  decode_base64_filename(unsigned char* buf, int* sz, unsigned char cc)

bufを Base64で decodeしてバイナリデータを取り出す．ただし cc は '/' として扱う．
*/
unsigned char*  decode_base64_filename(unsigned char* buf, int* sz, unsigned char cc)
{
    if (cc==0x00) cc = '-';

    int i, len = (int)strlen((char*)buf);
    for (i=0; i<len; i++) {
        if (buf[i]==cc) buf[i] = '/';
    }

    unsigned char* dec = decode_base64(buf, sz);

    return dec;
}


/**
unsigned char*  encode_base64_filename(unsigned char* buf, int sz, unsiged char cc)

バイナリデータ bufを Base64で encodeしてファイル名を作る．ただし '/' は cc として扱う．
*/
unsigned char*  encode_base64_filename(unsigned char* buf, int sz, unsigned char cc)
{
    if (cc==0x00) cc = '-';

    unsigned char* enc = encode_base64(buf, sz);

    int i, len = (int)strlen((char*)enc);
    for (i=0; i<len; i++) {
        if (enc[i]=='/') enc[i] = cc;
    }

    return enc;
}


/**
unsigned char*  decode_urlenc(unsigned char* buf, int* sz)

buf を URLエンコードからデコードする．
返されたデータは free()する必要がある．

@param      buf  デコードする文字列．
@param[out] sz   デコードされたデータの長さを格納する変数．
@return デコードされたデータ．要 free
*/
unsigned char*  decode_urlenc(unsigned char* buf, int* sz)
{
    int i, j, len;
    unsigned char* dec;

    if (buf==NULL) return NULL;

    len = (int)strlen((const char*)buf) + 1;
    dec = (unsigned char*)malloc(len);
    if (dec==NULL) return NULL;
    memset(dec, 0, len);

    i = j = 0;
    while (buf[i]!='\0') {
        if (buf[i]=='%') {
            if (buf[i+1]=='\0') return dec;
            if (buf[i+2]=='\0') return dec;
            dec[j++] = decode_hex(buf[i+1], buf[i+2]);
            i += 2;
        }
        else {
            if (buf[i]=='+') dec[j++] = ' ';
            else             dec[j++] = buf[i];
        }
        i++;
    }

    *sz = j;
    return dec;
}
                


/**
unsigned char*  encode_urlenc(unsigned char* buf, int sz)

バイナリデータ bufを URLエンコードする．

エンコードしない文字については色々な考え方があるが，最大公約数的に考えてこのプログラムでは以下のようにする．@n
エンコードしない文字  0-9, A-Z, a-z, -._

返されたデータは free()する必要がある．

@param  buf  エンコードするデータ．
@param  sz   エンコードするデータの長さ．-1以下の場合は buf は文字列とみなす．
@return エンコードされた文字列．要 free
*/
unsigned char*  encode_urlenc(unsigned char* buf, int sz)
{
    int i, j, len;
    unsigned char* enc;

    if (buf==NULL) return NULL;

    if (sz<0) sz = (int)strlen((const char*)buf);
    len = sz*3 + 1;
    enc = (unsigned char*)malloc(len);
    if (enc==NULL) return NULL;
    memset(enc, 0, len);

    for(i=0, j=0; i<sz; i++) {
        if (buf[i]==' ') {                                        // 半角スペース
            enc[j++] = '+';
        }
        else if ((buf[i]>=0x30 && buf[i]<=0x39) ||                // 0-9
                 (buf[i]>=0x41 && buf[i]<=0x5a) ||                // A-Z
                 (buf[i]>=0x61 && buf[i]<=0x7a) ||                // a-z
                  buf[i]=='-' || buf[i]=='.' || buf[i]=='_'){     // - . _
            enc[j++] = buf[i];
        }
        else {
            unsigned char* asc = encode_hex(buf[i]);
            if (asc==NULL) {
                free(enc);
                return NULL;
            }
            enc[j++] = '%';
            enc[j++] = asc[0];
            enc[j++] = asc[1];
            free(asc);
        }
    }

    return enc;
}    


/**
unsigned char*  decode_quoted_printable(unsigned char* buf, int* sz)

buf を quoted printableからデコードする．
返されたデータは free()する必要がある．

@param      buf  デコードする文字列．
@param[out] sz   デコードされたデータの長さを格納する変数．
@return デコードされたデータ
*/
unsigned char*  decode_quoted_printable(unsigned char* buf, int* sz)
{
    int    i, j, len;
    unsigned char*  dec;

    len = (int)strlen((const char*)buf) + 1;
    dec = (unsigned char*)malloc(len);
    if (dec==NULL) return NULL;
    memset(dec, 0, len);

    i = j = 0;
    while (buf[i]!='\0') {
        if (buf[i]=='=') {
            if (buf[i+1]=='\0') return dec;
            if (buf[i+2]=='\0') return dec;
            dec[j++] = decode_hex(buf[i+1], buf[i+2]);
            i += 2;
        }
        else {
            if (buf[i]=='_') dec[j++] = ' ';            // RFC2047
            else             dec[j++] = buf[i];
        }
        i++;
    }

    *sz = j;
    return dec;
}


/**
unsigned char*  encode_quoted_printable(unsigned char* buf, int sz)

バイナリデータ bufを quoted printable にエンコードする．

RFC2047 に従い，' ' は '_' にエンコードする．@n
エンコードしない文字 '=', '_' 以外の 0x21-0x7e @n
エンコードする文字   '=', '_', 0x00-0x20, 0x7f-0xff

返されたデータは free()する必要がある．

@param  buf  エンコードするデータ．
@param  sz   エンコードするデータの長さ．-1以下の場合は buf は文字列とみなす．
@return エンコードされた文字列．要 free
*/
unsigned char*  encode_quoted_printable(unsigned char* buf, int sz)
{
    int    i, j, len;
    unsigned char*  enc;

    if (sz<0) sz = (int)strlen((const char*)buf);
    len = sz*3 + 1;
    enc = (unsigned char*)malloc(len);
    if (enc==NULL) return NULL;
    memset(enc, 0, len);

    for (i=0, j=0; i<sz; i++) {
        if (buf[i]=='=') {        // 0x3d
            enc[j++] = '=';
            enc[j++] = '3';
            enc[j++] = 'D';
        }
        else if (buf[i]=='_') {        // 0x5f
            enc[j++] = '=';
            enc[j++] = '5';
            enc[j++] = 'F';
        }
        else if (buf[i]==' ') {        // 0x20  RFC2047
            enc[j++] = '_';
        }
        else if (buf[i]<=0x1f || buf[i]>=0x7f) {
            unsigned char* asc = encode_hex(buf[i]);
            if (asc==NULL) {
                free(enc);
                return NULL;
            }
            enc[j++] = '=';
            enc[j++] = asc[0];
            enc[j++] = asc[1];
            free(asc);
        }
        else {
            enc[j++] = buf[i];
        }
    }

    return enc;
}    


/**
unsigned char*  encode_hex(unsigned char cc)

キャラクタを16進コードの文字列に変換する．
返されたデータは free()する必要がある．

@param cc 変換するキャラクタ．

@return 新しく確保された 3byteの領域
@return [0]  16進コードの上位4Bit． 0-9,A-F
@return [1]  16進コードの下位4Bit． 0-9,A-F
@return [2]  '@\0'
@retval NULL メモリ不足．

@par 例
@code
    encode_hex(' ')  は "20" を返す．
@endcode
*/
unsigned char*  encode_hex(unsigned char cc)
{
    unsigned char* ret;
    unsigned char  pp1, pp2;
    unsigned char  base[] = "0123456789ABCDEF";

    ret = (unsigned char*)malloc(3);
    if (ret==NULL) return NULL;

    pp1 = cc >> 4;
    pp2 = cc & 0x0f;

    ret[0] = base[pp1];
    ret[1] = base[pp2];
    ret[2] = '\0';

    return ret;
}


/**
unsigned char  decode_hex(unsigned char pp1, unsigned char pp2)

16進コードをキャラクタに変換する．O-9,A-F以外が指定された場合は0とみなす．

@param  pp1  16進コードの上位4Bit． 0-9,A-F
@param  pp2  16進コードの下位4Bit． 0-9,A-F
@return 指定されたASCIIコードのキャラクタ

@code
例：decode_hex('2', '0')  は ' ' を返す．
@endcode
*/
unsigned char  decode_hex(unsigned char pp1, unsigned char pp2)
{
    unsigned char ret;

    if (pp1>=0x30 && pp1<=0x39)      pp1 -= 0x30;    // 0-9
    else if (pp1>=0x41 && pp1<=0x46) pp1 -= 0x37;    // A-F
    else if (pp1>=0x61 && pp1<=0x66) pp1 -= 0x57;    // a-f
    else                             pp1 =  0x00;

    if (pp2>=0x30 && pp2<=0x39)      pp2 -= 0x30;    // 0-9
    else if (pp2>=0x41 && pp2<=0x46) pp2 -= 0x37;    // A-F
    else if (pp2>=0x41 && pp2<=0x46) pp2 -= 0x57;    // a-f
    else                             pp1 =  0x00;

    ret = (pp1<<4) + pp2;
    return ret;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  バイト型 リングバッファ
//

/**
ringBuffer*  new_ringBuffer(int sz)

リングバッファ型変数を生成する．@n
返されたデータは del_ringBuffer() する必要がある．

@return 空のリングバッファ変数．要 free
@see ringBuffer, del_ringBuffer()
*/
ringBuffer*  new_ringBuffer(int sz)
{
    ringBuffer* rb;
    
    rb = (ringBuffer*)malloc(sizeof(ringBuffer));
    if (rb==NULL) return NULL;
    memset(rb, 0, sizeof(ringBuffer));
    rb->state = JBXL_NORMAL;
    
    if (sz>0) {
        rb->buf = (unsigned char*)malloc(sz);
        if (rb->buf==NULL) return rb;
        memset(rb->buf, 0, sz);
        rb->bufsz = sz;
    }
    return rb;
}


/**
void  del_ringBuffer(ringBuffer** rb)

new_ringBuffer()で作り出したリングバッファを削除する．

@param[in,out] rb  in: 削除するリングバッファポインタへのポインタ．@n
                   out: *rb はNULLになる．
@see ringBuffer, new_ringBuffer()
*/
void  del_ringBuffer(ringBuffer** rb)
{
    if (rb!=NULL && *rb!=NULL) {
        freeNull((*rb)->buf);
        free(*rb);
        *rb = NULL;
    }
}


/**
ringBuffer  init_ringBuffer()

初期化された ringBuffer変数を返す．

@return 期化された ringBuffer変数
@see ringBuffer
*/
ringBuffer  init_ringBuffer()
{
    ringBuffer  rb;

    memset(&rb, 0, sizeof(ringBuffer));
    rb.state = JBXL_NORMAL;

    return rb;
}


/**
ringBuffer  make_ringBuffer(int sz)

バッファ部が存在するリングバッファを作り出す．
バッファ部は free()する必要がある．

@param  sz  リングバッファのバッファ部のサイズ
@return 作成された ringBuffer変数．バッファ部は 要 free
*/
ringBuffer  make_ringBuffer(int sz)
{
    ringBuffer  rb;

    memset(&rb, 0, sizeof(ringBuffer));
    
    if (sz>0) {
        rb.buf = (unsigned char*)malloc(sz);
        if (rb.buf==NULL) return rb;
        memset(rb.buf, 0, sz);
        rb.bufsz = sz;
        rb.state = JBXL_NORMAL;
    }

    return rb;
}


/**
void  free_ringBuffer(ringBuffer* rb)

リングバッファのバッファ部を開放する．

@param  rb  リングバッファへのポインタ
*/
void  free_ringBuffer(ringBuffer* rb)
{
    if (rb!=NULL) {
        freeNull(rb->buf);
        memset(rb, 0, sizeof(ringBuffer));
        rb->state = JBXL_NORMAL;
    }
}


/**
void   clear_ringBuffer(ringBuffer* rb)

リングバッファ rb のデータをクリア（初期化）する．

@param  rb  リングバッファへのポインタ
*/
void   clear_ringBuffer(ringBuffer* rb)
{
    if (rb!=NULL) {
        int sz = rb->bufsz;
        unsigned char* ptr = rb->buf;
        memset(rb->buf, 0, rb->bufsz);
        memset(rb, 0, sizeof(ringBuffer));
        rb->buf   = ptr;
        rb->state = JBXL_NORMAL;
        rb->bufsz = sz;
    }
}


/**
int  put_ringBuffer(ringBuffer* rb, unsigned char* pp, int sz)

リングバッファ rb へデータを szバイト格納する．

@param[in]  rb  リングバッファへのポインタ
@param[out] rb  @b state  JBXL_NORMAL: バッファは正常状態．JBXL_ERROR: バッファは異常な状態にある．
@param      pp  格納するデータへのポインタ．
@param      sz  格納するデータのサイズ（Byte）

@retval 0以上  書き込んだバイト数．
@retval JBXL_ARGS_ERROR         引数にNULLのデータがある．
@retval JBXL_TOOLS_BUF_ERROR    バッファ（データ格納）部の領域がない．
@retval JBXL_TOOLS_BUFSZ_ERROR  バッファ（データ格納）部の大きさがたりない．データはputされなかった．
*/
int  put_ringBuffer(ringBuffer* rb, unsigned char* pp, int sz)
{
    if (rb==NULL) return JBXL_ARGS_ERROR;
    rb->state = JBXL_NORMAL;

    if (pp==NULL)      return JBXL_ARGS_ERROR;
    if (rb->buf==NULL) return JBXL_TOOLS_BUF_ERROR;
    if (rb->datasz+sz>rb->bufsz) {
        rb->state = JBXL_ERROR;
        return JBXL_TOOLS_BUFSZ_ERROR;
    }

    if (rb->epoint+sz<=rb->bufsz) {
        memcpy(&(rb->buf[rb->epoint]), pp, sz);
        rb->epoint = rb->epoint + sz;
        if (rb->epoint==rb->bufsz) rb->epoint = 0;
    }
    else {
        memcpy(&(rb->buf[rb->epoint]), pp, rb->bufsz-rb->epoint);
        memcpy(rb->buf, &(pp[rb->bufsz-rb->epoint]), sz-(rb->bufsz-rb->epoint));
        rb->epoint = rb->epoint + sz - rb->bufsz; 
    }

    rb->datasz += sz;
    return sz;
}    


/**
unsigned char*  get_ringBuffer(ringBuffer* rb, int sz)

リングバッファ rb から szバイトのデータを取り出す．@n
返されたデータは free()する必要がある．

@param[in]  rb   リングバッファへのポインタ
@param[out] rb   @b state JBXL_NORMAL: バッファは正常状態．JBXL_ERROR: バッファは異常な状態にある．
@param      sz   取り出すバイト数．

@retval NULL以外 取り出したデータへのポインタ．データサイズは sz + 1．要  free
@retval NULL     データ取得失敗．現在有効なデータサイズはszより小さい．または単に失敗した．
*/
unsigned char*  get_ringBuffer(ringBuffer* rb, int sz)
{
    unsigned char* pp;

    if (rb==NULL) return NULL;
    if (rb->buf==NULL) return NULL;
    if (sz>rb->datasz) return NULL;

    pp = (unsigned char*)malloc(sz+1);
    if (pp==NULL)  return NULL;
    memset(pp, 0, sz+1);

    if (rb->spoint+sz<=rb->bufsz) {
        memcpy(pp, &(rb->buf[rb->spoint]), sz);
        rb->spoint = rb->spoint + sz;
        if (rb->spoint==rb->bufsz) rb->spoint = 0;
    }
    else {
        memcpy(pp, &(rb->buf[rb->spoint]), rb->bufsz-rb->spoint);
        memcpy(&(pp[rb->bufsz-rb->spoint]), rb->buf, sz-(rb->bufsz-rb->spoint));
        rb->spoint = rb->spoint + sz - rb->bufsz; 
    }

    rb->datasz -= sz;
    return pp;
}    


/**
int  seek_ringBuffer(ringBuffer* rb, int sz)

リングバッファ rb の読み取り開始ポインタ spoint を sz 移動させる．@n

@param    rb   リングバッファへのポインタ
@param    sz   開始ポインタを移動させる量．負数（バック）も可．

@return   TRUE  : 移動に成功．
@return   FALSE : 移動に失敗．

@bug バッファ長を超える移動はできない．移動させた先にデータが存在しているかは保障されない．
*/
int  seek_ringBuffer(ringBuffer* rb, int sz)
{
    if (rb==NULL) return FALSE;
    if (rb->buf==NULL) return FALSE;
    if (sz>rb->datasz) return FALSE;

    if  (rb->spoint+sz>=rb->bufsz) {
        rb->spoint = rb->spoint + sz - rb->bufsz;
    }
    else if (rb->spoint+sz<0) {
        rb->spoint = rb->bufsz + rb->spoint + sz;
    }
    else  {
        rb->spoint = rb->spoint + sz;
    }

    rb->datasz -= sz;
    return TRUE;
}    


/**
unsigned char*  ref_ringBuffer(ringBuffer* rb, int pos)

リングバッファ rb の spoint + pos の位置にあるデータへの参照．@n

@param  rb   リングバッファへのポインタ
@param  pos  参照するデータの spoint からの位置．

@retval 参照データへのポインタ．
*/
unsigned char*  ref_ringBuffer(ringBuffer* rb, int pos)
{
    unsigned char* pp;

    if (rb==NULL) return NULL;
    if (rb->buf==NULL) return NULL;
    if (pos>rb->datasz) return NULL;

    int sz = rb->spoint + pos;
    pp = rb->buf + sz;
    if (sz >= rb->bufsz) pp -= rb->bufsz;

    return pp;
}    



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// バイト型 メッセージストリームバッファ
//

/**
int  put_mstream(mstream* sb, unsigned char* mesg)

メッセージ（文字列）ストリーム sb へメッセージ（の一部）を格納する．@n
ここで，メッセージとは @\r@\n, @\r または @\n で区切られている文字列でのことである．

@param[in]  sb    メッセージストリームへのポインタ
@param[out] sb    @b state  JBXL_NORMAL: ストリームは正常状態．JBXL_ERROR: ストリームは異常な状態にある．
@param      mesg  格納するメッセージ，またはその一部

@retval 0以上  書き込んだバイト数（CRLFを含む）
@retval JBXL_ARGS_ERROR         引数にNULLのデータがある．
@retval JBXL_TOOLS_BUF_ERROR    バッファ（データ格納）部の領域がない．
@retval JBXL_TOOLS_BUFSZ_ERROR  バッファ（データ格納）部の大きさがたりない．
*/
int  put_mstream(mstream* sb, unsigned char* mesg)
{
    int  i, n, m, cc, np=0, len;

    if (sb==NULL)   return JBXL_ARGS_ERROR;
    sb->state = JBXL_NORMAL;
    if (mesg==NULL) return JBXL_ARGS_ERROR;
    if (sb->buf==NULL) return JBXL_TOOLS_BUF_ERROR;

    len = (int)strlen((const char*)mesg);
    for (i=0, n=0, m=0; i<len; i++) {
        if (mesg[i]==CHAR_LF || (mesg[i]==CHAR_CR && mesg[i+1]!=CHAR_LF)) {
            if (m==0) m = i;
            n++;
        }
    }
    if (sb->datano==0 && n>=1) np = (sb->epoint + m + 1) % sb->bufsz;

    cc = put_ringBuffer(sb, mesg, len);
    if (cc>=0) {
        if (sb->datano==0 && n>=1) sb->npoint = np;
        sb->datano += n;
    }

    if (cc<0) sb->state = JBXL_ERROR;
    return cc;
}


/**
unsigned char*  get_mstream(mstream* sb)

メッセージ（文字列）ストリーム sb から次のメッセージを取り出す．改行コードは削除される．@n
ここで，メッセージとは @\r@\n, @\r または @\n で区切られている文字列でのことである．

@param[in]  sb   メッセージストリームへのポインタ
@param[out] sb   @b state  JBXL_NORMAL: ストリームは正常状態．JBXL_ERROR: ストリームは異常な状態にある．（未実装）

@retval NULL以外 取り出したストリームデータへのポインタ．要 free
@retval NULL     データ取得失敗．現在有効なメッセージデータは無い．または単に失敗した．
*/
unsigned char*  get_mstream(mstream* sb)
{
    int  n, len;
    unsigned char* pp;

    if (sb==NULL) return NULL;
    sb->state = JBXL_NORMAL;
    if (sb->buf==NULL) return NULL;
    if (sb->datano==0) return NULL;

    len = sb->npoint - sb->spoint;
    if (len==0) return NULL;
    else if (len<0) len += sb->bufsz;

    pp = get_ringBuffer(sb, len);
    if (pp==NULL) {
        sb->state = JBXL_ERROR;
        return NULL;
    }
    chomp((char*)pp);

    sb->datano += -1;
    sb->spoint = sb->npoint;

    if (sb->datano>=1) {
        int m;
        n = sb->spoint;
        while (n!=sb->epoint) {
            m = (n + 1) % sb->bufsz;
            if (sb->buf[n]==CHAR_LF || (sb->buf[n]==CHAR_CR && sb->buf[m]!=CHAR_LF)) {
                sb->npoint = m;
                break;
            }
            n = m;
        }
    }

    return pp;
}


/**
unsigned char*  fgets_mstream(unsigned char* mesg, mstream* sb)

メッセージストリームからメッセージを一行だけ取り出す．

メッセージ mesg はメッセージストリームに一旦バッファリングされ，この関数により一行ずつ読み出される．@n
結果が返される時，行中の改行コードは削除され，行末には必ず @\0 が入る．@n
メッセージストリームのバッファ部が確保されていない場合は，最初に呼び出された時点で確保される．@n
一旦この関数を使用して，受信データをバッファリングしたら，最後まで読み取りには必ず同じストリームを使用して@n
この関数を呼び出さばければならない．そうで無い場合は受信データの整合性は保証されない．

@param      mesg  バッファに一旦格納されるメッセージ．NULLでも可．
@param[in]  sb    ストリームバッファ（リングバッファ型のストリームバッファ）．
@param[out] sb    @b sb->buf バッファ部が確保さえていなければ，自動的に確保される．
@param[out] sb    @b sb->state JBXL_ERROR:  メッセージストリーム操作中は異常な状態にある．
@param[out] sb    @b sb->state JBXL_NODATA: メッセージストリーム中に有効なデータがない．return は NULL

@return 取り出した文字列へのポインタ
*/
unsigned char*  fgets_mstream(unsigned char* mesg, mstream* sb)
{
    int state = JBXL_NORMAL;
    unsigned char* pp;

    if (sb==NULL) return NULL;
    if (sb->buf==NULL) {
        *sb = make_mstream(RECVBUFSZ);
        if (sb->buf==NULL) return NULL;
    }

    if (mesg!=NULL) put_mstream(sb, mesg);
    if (sb->state<0) state = sb->state;

    if (sb->datano==0) {
        sb->state = JBXL_NODATA;
        return NULL;
    }
    pp = get_mstream(sb);
    
    if (sb->state>=0 && state<0) sb->state = state;
    return pp;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
unsigned char*  uuid2guid(unsigned char* p)

バイナリの UUID を テキストのGUIDに変換する．要 free()

@param  p バイナリの UUID. 128bit
@return テキストの GUID. 要 free
*/
unsigned char*  uuid2guid(unsigned char* p)
{
    unsigned char* guid;

    if (p==NULL) return NULL;

    guid = (unsigned char*)malloc(LGUID);    // 40 = 32+4+1(\0) + α
    if (guid==NULL) return NULL;
    memset(guid, 0, LGUID);

    snprintf((char*)guid, 37, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                    p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15]);

    return guid;
}


/**
unsigned char*  guid2uuid(unsigned char* p)

テキストのGUID をバイナリのUUID に変換する．要 free() @n
入力が GUIDの形式でない時は，処理結果は保証されない．

@param  p  テキストの GUID
@return バイナリの UUIDへのポインタ．128bit. 要 free
*/
unsigned char*  guid2uuid(unsigned char* p)
{
    int   i, j;
    unsigned char* uuid;

    if (p==NULL) return NULL;

    uuid = (unsigned char*)malloc(16);
    if (uuid==NULL) return NULL;
    memset(uuid, 0, 16);

    i = j = 0;
    while (p[i]!='\0' && j<16) {
        if (p[i]!='-' && p[i+1]!='\0') {
            unsigned char hb, lb;

            if      (p[i]>='0' && p[i]<='9')     hb = p[i] - '0';
            else if (p[i]>='a' && p[i]<='f')     hb = p[i] - 'a' + 10;
            else if (p[i]>='A' && p[i]<='F')     hb = p[i] - 'A' + 10;
            else hb = '\0';

            if      (p[i+1]>='0' && p[i+1]<='9') lb = p[i+1] - '0';
            else if (p[i+1]>='a' && p[i+1]<='f') lb = p[i+1] - 'a' + 10;
            else if (p[i+1]>='A' && p[i+1]<='F') lb = p[i+1] - 'A' + 10;
            else lb = '\0';
            
            uuid[j] = (hb<<4) + lb;
            i++;
            j++;
        }
        i++;
    }

    return uuid;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

#ifndef WIN32

/**
unsigned long int get_used_memory(void)

使用中のメモリサイズを得る．
*/
unsigned long int get_used_memory(void)
{
    struct sysinfo info;
    sysinfo(&info);

    // キャッシュサイズが引けていない．
    unsigned long int mem = info.totalram - info.freeram - info.bufferram - info.sharedram;
    return mem*info.mem_unit/1024;
}


/**
unsigned long int get_free_memory(void)

未使用のメモリサイズを得る．
*/
unsigned long int get_free_memory(void)
{
    struct sysinfo info;
    sysinfo(&info);

    unsigned long int mem = info.freeram;
    return mem*info.mem_unit/1024;
}


void  memory_check_start(void)
{
    UsedMemoryBase = get_used_memory();
}


unsigned long int  memory_check(void)
{
    return get_used_memory() - UsedMemoryBase;
}


#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シグナル処理
//
#ifndef WIN32

#ifdef SIGCHLD

/**
void  set_sigterm_child(void (*handler)(int))

child プロセス終了時の処理を設定．

@param  (*handler)()  チャイルドプロセス終了時に呼び出される関数へのポインタ\@
        NULL の場合は void sigterm_child(int signal) が設定される．

@attention
シグナル関係の関数は Windowsでは使用できない
*/
void  set_sigterm_child(void (*handler)(int))
{    
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    if (handler!=NULL) sa.sa_handler = handler;
    else               sa.sa_handler = sigterm_child;

    sa.sa_flags = SA_NOCLDSTOP | SA_RESTART; 
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);     // SIGCHLD 処理中は SIGINT  をブロック
    sigaddset(&sa.sa_mask, SIGHUP);     // SIGCHLD 処理中は SIGHUP  をブロック
    sigaddset(&sa.sa_mask, SIGTERM);    // SIGCHLD 処理中は SIGTERM をブロック
    sigaction(SIGCHLD, &sa, NULL);
}


/**
void  sigterm_child(int signal)

child プロセス終了時の処理

@param  signal  ハンドラのシグナル番号．システムによって設定される．
*/
void  sigterm_child(int signal)
{
    UNUSED(signal);

    pid_t pid = 0;
    int ret;

    //DEBUG_MODE PRINT_MESG("%d is waiting for children. signal = %d\n", getpid(), signal);
    //signal = 0;        // dummy for warning of compiler

    do {    // チャイルドプロセスの終了を待つ    
        pid = waitpid(-1, &ret, WNOHANG);
    } while(pid>0);
}


/**
void  ignore_sigterm_child()

以後 child プロセスの終了を無視する．
*/
void  ignore_sigterm_child()
{    
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags   = 0; 
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);
}


/**
void  set_sigsegv_handler(void (*handler)(int))

セグメンテーションエラー時のハンドラを設定．@n
引数に NULLを指定した場合は，ハンドラ関数として trap_segmentation_falt() を使用．

@param  (*handler)()  セグメンテーションエラー時に呼び出される関数へのポインタ
@see trap_segmentation_falt()
*/
void  set_sigsegv_handler(void (*handler)(int))
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    if (handler!=NULL) sa.sa_handler = handler;
    else               sa.sa_handler = trap_segmentation_falt;
    
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
}


/**
void  trap_segmentation_falt(int signal)

セグメンテーションエラー時にデフォルトで呼び出される関数．

@param  signal  ハンドラのシグナル番号．システムによって設定される．
*/
void  trap_segmentation_falt(int signal)
{
    PRINT_MESG("****************************************************************\n");
    PRINT_MESG("* Segmentation Falt in [%d] !!!!!\n", getpid());
    PRINT_MESG("****************************************************************\n");

    exit(signal);
}


#endif        // SIGCHLD

#endif        // NOT WIN32


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for MS Windows
//

#ifdef WIN32

/* see common.h

void  bzero(char* p, unsigned int n)
{
    for (unsigned int i=0; i<n; i++) *(p++) = 0x00;
}


void  bcopy(char* f, char* t, unsigned int n)
{
    for (unsigned int i=0; i<n; i++) *(t++) = *(f++);
}
*/

#endif   // WIN32



//////////////////////////////////////////////////////////////////////////////////////////////////////
// 入出力＆DEBUG
//

FILE*  FP_LogFile = NULL;
FILE*  FP_ErrFile = NULL;


/**
void  open_logfile(void)

デフォルトのログファイルをオープンする
*/
void  open_logfile(void)
{
    if (FP_LogFile!=NULL) return;

    FP_LogFile = fopen(JBXL_LOG_FILE, "a");
    return;
}


/**
void  close_logfile(void)

デフォルトのログファイルをクローズする
*/
void  close_logfile(void)
{
    if (FP_LogFile==NULL) return;

    fclose(FP_LogFile);
    FP_LogFile = NULL;

    return;
}


/**
void  print_logfile(const char* fmt, ...)

デフォルトのログファイルにデータを書き込む
*/
void  print_logfile(const char* fmt, ...)
{
    char*  nfmt;
    int    len;
    int    slfopn = FALSE;

    //
    if (FP_LogFile==NULL) {
        FP_LogFile = fopen(JBXL_LOG_FILE, "a");
        if (FP_LogFile==NULL) return;
        slfopn = TRUE;
    }

    va_list  args;
    va_start(args, fmt);

    len  = (int)strlen(fmt);    
    nfmt = (char*)malloc(len + 2);
    if (nfmt==NULL) return;

    strncpy(nfmt, fmt, len);
    nfmt[len]   = '\n';
    nfmt[len+1] = '\0';

    vfprintf(FP_LogFile, nfmt, args); 
    fflush(FP_LogFile);
    free(nfmt);

    va_end(args);

    //
    if (slfopn && FP_LogFile!=NULL) {
        fclose(FP_LogFile);
        FP_LogFile = NULL;
    }

    return;
}


/**
void  open_errfile(void)

デフォルトのエラーファイルをオープンする
*/
void  open_errfile(void)
{
    if (FP_ErrFile!=NULL) return;

    FP_LogFile = fopen(JBXL_ERR_FILE, "a");
    return;
}


/**
void  close_errfile(void)

デフォルトのエラーファイルをクローズする
*/
void  close_errfile(void)
{
    if (FP_ErrFile==NULL) return;

    fclose(FP_ErrFile);
    FP_ErrFile = NULL;

    return;
}


/**
void  print_errfile(const char* fmt, ...)

デフォルトのエラーファイルにデータを書き込む
*/
void  print_errfile(const char* fmt, ...)
{
    char*  nfmt;
    int    len;
    int    slfopn = FALSE;

    //
    if (FP_ErrFile==NULL) {
        FP_ErrFile = fopen(JBXL_ERR_FILE, "a");
        if (FP_ErrFile==NULL) return;
        slfopn = TRUE;
    }

    va_list  args;
    va_start(args, fmt);

    len  = (int)strlen(fmt);    
    nfmt = (char*)malloc(len + 2);
    if (nfmt==NULL) return;

    strncpy(nfmt, fmt, len);
    nfmt[len]   = '\n';
    nfmt[len+1] = '\0';

    vfprintf(FP_ErrFile, nfmt, args); 
    fflush(FP_ErrFile);
    free(nfmt);

    va_end(args);

    //
    if (slfopn && FP_ErrFile!=NULL) {
        fclose(FP_ErrFile);
        FP_ErrFile = NULL;
    }

    return;
}


/**
void  print_message(const char* fmt, ...)

バッファリング無しのメッセージ出力（stderr）

@param  fmt  出力フォーマット．printf() に準拠．
@see printf()
*/
void  print_message(const char* fmt, ...)
{
    char*  nfmt;
    int    len;

    va_list  args;
    va_start(args, fmt);

    len  = (int)strlen(fmt);    
    nfmt = (char*)malloc(len + 1);
    if (nfmt==NULL) return;

    strncpy(nfmt, fmt, len);
    nfmt[len] = '\0';

    vfprintf(stderr, nfmt, args); 
    fflush(stderr);
    free(nfmt);

    va_end(args);

    return;
}


/**
void  fprint_message(FILE *fp, const char* fmt, ...)

バッファリング無しのメッセージ出力

@param  fp   出力先のファイルポインタ
@param  fmt  出力フォーマット．printf() に準拠．
@see printf()
*/
void  fprint_message(FILE* fp, const char* fmt, ...)
{
    char*  nfmt;
    int    len;

    va_list  args;
    va_start(args, fmt);

    len  = (int)strlen(fmt);    
    nfmt = (char*)malloc(len + 1);
    if (nfmt==NULL) return;

    strncpy(nfmt, fmt, len);
    nfmt[len] = '\0';

    vfprintf(fp, nfmt, args); 
    fflush(fp);
    free(nfmt);

    va_end(args);

    return;
}


/**
void  print_escape(const char* fmt, const char* mesg)

エスケープ文字を含むメッセージの出力．(stderr)

現在は CR, LF のみ @\r, @\n に変換する．see change_esc()

@param  fmt   出力フォーマット．printf() に準拠．
@param  mesg  エスケープ文字を含む文字列．
@see change_esc(), printf()
*/
void  print_escape(const char* fmt, char* mesg)
{
    char* pp;

    if (fmt==NULL || mesg==NULL) return;
    
    pp = change_esc(mesg);
    if (pp==NULL) return;

    PRINT_MESG(fmt, pp);
    free(pp);
}


/**
void  fprint_escape(FILE* fp, char* fmt, char* mesg)

エスケープ文字を含むメッセージの出力．

現在は CR, LF のみ @\r, @\n に変換する．

@param  fp    出力先のファイルポインタ
@param  fmt   出力フォーマット．printf() に準拠．
@param  mesg  エスケープ文字を含む文字列．

@see change_esc(), printf()
*/
void  fprint_escape(FILE* fp, char* fmt, char* mesg)
{
    char* pp;

    if (fmt==NULL || mesg==NULL) return;
    
    pp = change_esc(mesg);
    if (pp==NULL) return;

    fprint_message(fp, fmt, pp);
    free(pp);
}


/**
void  fdump(FILE* fp, unsigned char* mesg, int n)

デバッグ用 16進ダンプ出力．

mesgの nバイトを16進表示する．n<0 の場合は 文字列とみなす．

@param  fp    出力するファイルへのポインタ．NULLの場合は stderr
@param  mesg  表示するデータ．
@param  n     表示するバイト数
*/
void  fdump(FILE* fp, unsigned char* mesg, int n)
{
    int  i, j, mod, len;
    char ascii[16];

    if (fp==NULL) fp = stderr;

    if (mesg==NULL) {
        fprintf(fp, "(Dump Data is NULL)\n");
        return;
    }

    if (n<0) len = (int)strlen((const char*)mesg);
    else     len = n;

    memset(ascii, 0x2e, 16);
    for (i=0, j=0; i<len; i++) {
        fprintf(fp, "%02x ", mesg[i]);
        if (mesg[i]>=0x20 && mesg[i]<=0x7e) ascii[j] = mesg[i];
        j++;
        if (j==16) {
            fprintf(fp, "   ");
            for (j=0; j<16; j++) {
                fprintf(fp, "%c ", ascii[j]);
            }
            fprintf(fp, "\n");
            memset(ascii, 0x2e, 16);
            j = 0;
        }
    }
                
    mod = len%16;
    if (mod>0) {                           
        for (i=0; i<17-mod; i++) fprintf(fp, "   ");
        for (i=0; i<mod; i++)    fprintf(fp, "%c ", ascii[i]);
    }                    
    fprintf(fp, "\n");
}


/**
void  print_16x(FILE* fp, unsigned char* mesg, int n)

デバッグ用 16進一行出力．

mesgの nバイトの16進を一行，空白なしで表示する．n<0 の場合は 文字列とみなす．

@param  fp    出力するファイルへのポインタ．NULLの場合は stderr
@param  mesg  表示するデータ．
@param  n     表示するバイト数
*/
void  print_16x(FILE* fp, unsigned char* mesg, int n)
{
    int  i, len;

    if (fp==NULL) fp = stderr;

    if (n<0) len = (int)strlen((const char*)mesg);
    else     len = n;

    for (i=0; i<len; i++) {
        fprintf(fp, "%02x", mesg[i]);
    }
    fprintf(fp, "\n");
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
// グラフィック
//

static  char  _GraphicFileExtension[L_OCT];

char*    get_graphic_extension(uWord tex)
{
    memset(_GraphicFileExtension, 0, L_OCT);

    uWord file = tex & 0x00f0;
    if      (file==JBXL_TEXTURE_JPEG) memcpy(_GraphicFileExtension, ".jpeg", 5);
    else if (file==JBXL_TEXTURE_TIFF) memcpy(_GraphicFileExtension, ".tiff", 5);
    else if (file==JBXL_TEXTURE_PNG)  memcpy(_GraphicFileExtension, ".png",  4);
    else if (file==JBXL_TEXTURE_TGA)  memcpy(_GraphicFileExtension, ".tga",  4);
    else if (file==JBXL_TEXTURE_JP2K) memcpy(_GraphicFileExtension, ".jp2",  4);
    else if (file==JBXL_TEXTURE_RAS)  memcpy(_GraphicFileExtension, ".ras",  4);

    return _GraphicFileExtension;
}


