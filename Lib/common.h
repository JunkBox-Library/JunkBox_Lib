#ifndef  __JBXL_COMMON_HEADER_
#define  __JBXL_COMMON_HEADER_

/** 
@brief   汎用共通ヘッダ
@file    common.h
@version 5.2
@author  Fumi.Iseki (C)
@date    2020 11/22
*/

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif


#define _INCLUDE_POSIX_SOURCE
#define __EXTENSIONS__                // for Solaris


#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS       // for Windows
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/////////////////////////////////////////////////////////////////////////
// 動作環境

#ifdef __MINGW32__
    #define MINGW32
#elif defined __CYGWIN__
    #define CYGWIN
#endif


/////////////////////////////////////////////////////////////////////////
// for Windows VC++

#ifdef  WIN32   // for Windows
    #include <io.h>
    #include <direct.h>

    #ifndef _MFC_APP_
        #define _WINSOCKAPI_    // network.h で winsock2.h を読み込むため
        #include <windows.h>
    #endif

    #define  Intel
    #define  scanf       scanf_s 
    #define  unlink      _unlink
    #define  snprintf    _snprintf
    #define  vsnprintf   _vsnprintf
    #define  strcasecmp  _stricmp 
    #define  strncasecmp _strnicmp
//  #define  close(p)    _close(p)
    #define  mkdir(d, m) _mkdir(d)
    #define  mode_t      int
    #define  bzero(p, n)    ZeroMemory((p), (n))
    #define  bcopy(f, t, n) CopyMemory((t), (f), (n))

    // for Unicode
    #ifdef   _UNICODE
        #ifndef  _ttof
            #define  _ttof   _wtof
        #endif
    #else
        #ifndef  _ttof
            #define  _ttof   atof
        #endif
    #endif    // Unicode

    #define  tcscmp      _tcscmp       ///< Unicode用 strcmp
    #define  tcsncmp     _tcsncmp      ///< Unicode用 strncmp
    #define  tcsncpy     _tcsncpy      ///< Unicode用 strncpy
    #define  tcslen      _tcslen       ///< Unicode用 strlen
    #define  tunlink     _tunlink      ///< Unicode用 unlink
    #define  tfopen      _tfopen       ///< Unicode用 fopen
    #define  sntprintf   _sntprintf_s  ///< Unicode用 snprintf
    #define  ttoi        _ttoi         ///< Unicode用 atoi
    #define  ttof        _ttof         ///< Unicode用 atof
    #define  tprintf     _tprintf      ///< Unicode用 printf
    #define  ftprintf    _ftprintf     ///< Unicode用 fprintf

    #ifdef _WINDLL               
        #define DllExport __declspec(dllexport)
        #define DllImport __declspec(dllimport)
    #endif

    #ifdef WIN64
        #define WIN_DD_NULL   (void*)0xdddddddddddddddd
    #else
        #define WIN_DD_NULL   (void*)0xdddddddd
    #endif

#endif    // WIN32


#ifndef DllExport   
    #define DllExport
    #define DllImport
#endif


/////////////////////////////////////////////////////////////////////////
// for Solaris

#ifndef LOG_PERROR
    #define LOG_PERROR 0
#endif


/////////////////////////////////////////////////////////////////////////
// for not Windows

#ifndef  WIN32  
    #define  Sleep(t)  usleep((t)*1000)     ///< usleep()
#endif


/////////////////////////////////////////////////////////////////////////
//
#define  MAXBUFSZ  4194304      ///<   4M
#define  BUFSZ2M   2087152      ///<   2M
#define  HHHBUFSZ  2087152      ///<   2M
#define  BUFSZ1M   1048576      ///<   1M
#define  HHBUFSZ   1048576      ///<   1M
#define  HBUFSZ     524288      ///< 512K
#define  RECVBUFSZ  262144      ///< 256K
#define  LLLBUFSZ   131072      ///< 128K
#define  LLBUFSZ     65536      ///<  64K
#define  LBUFSZ      32768      ///<  32K
#define  BUFSZ       16384      ///<  16K
#define  MBUFSZ       8192      ///<   8K
#define  SBUFSZ       4096      ///<   4K

#define  LFRAME       2048
#define  LBUF         1024
#define  LMESG         512
#define  LPATH         256
#define  LDATA         256
#define  LNAME         128
#define  L_128         128
#define  LMDATA        128
#define  LPASS         128        ///< $6$ -> 98文字
#define  LMNAME         64
#define  LSDATA         64
#define  L_64           64
#define  LGUID          40        ///< 16*2 + 4(-) + 1('@\0') = 37 以上の 4の倍数
#define  LUUID          36        ///< 16*2 + 1('@\0') = 33 以上の 4の倍数
#define  L_32           32
#define  LADDR          32
#define  LADDR6        128
#define  LSNAME         32
#define  L_ID           16
#define  L_16           16
#define  L_OCT           8

#define  LEN_INT        22        ///< log 2^64 + '@\0' + 1(予備)
#define  LEN_IPADDR     17        ///< strlen("AAA.BBB.CCC.DDD") + '@\0' + 1(予備)
#define  LEN_IPADDR6    41        ///< strlen("1111:2222:333:4444:5555:6666:7777:8888") + '@\0' + 1(予備)
#define  LEN_PORT        7        ///< strlen("65536") + '@\0' + 1(予備)
#define  LEN_IPPORT     23        ///< strlen("AAA.BBB.CCC.DDD:65536") + '@\0' + 1(予備) 

#define  STRLEN_GUID    36        ///< GUID の長さ

#define  WORK_FILENAME_LEN  16


#define  PI          3.1415926535897932385
#define  PI15        4.7123889803846898577
#define  PI2         6.2831853071795864769
#define  PI_DIV2     1.5707963267948966192
#define  PI_DIV2_3   4.7123889803846898577
#define  PI_DIV3     1.0471975511965977462
#define  PI_DIV3_2   2.0943951023931954923
#define  PI_DIV4     0.78539816339744830962
#define  PI_DIV6     0.52359877559829887308
#define  LN_10       2.3025850929940456840
#define  EXP_1       2.71828182845904523536

#define  DEGREE2RAD  0.0174532925199432957692
#define  RAD2DEGREE  57.2957795130823208768

#define  SQROOT_05   0.707106781186547524401
#define  SQROOT_2    1.4142135623730950488
#define  SQROOT_3    1.7320508075688772935
#define  SQROOT_5    2.2360679774997896964
#define  SQROOT_7    2.6457513110645905905


#define  SINTMAX     2147483647
#define  SINTMIN    -2147483647        // 2147483648 of -2147483648 is over flow from INT MAX 
#define  UINTMAX     4294967295
#define  SWORDMAX         32767
#define  SWORDMIN        -32768
#define  UWORDMAX         65535

#ifndef RAND_MAX
#ifdef  WIN32   // for Windows    
    #define  RAND_MAX  32767
#else
    #define  RAND_MAX  2147483647
#endif
#endif


//////////////////////////////////////////////////

#ifndef  FALSE
    #define  FALSE  0
#endif
#ifndef  TRUE
    #define  TRUE   1
#endif


#define  ON       1
#define  OFF      0
#define  OK       1
#define  NG       0
#define  YES      1
#define  NO       0


#ifndef  Max
    #define  Max(x, y)  (((x)<(y)) ? (y):(x))
#endif
#ifndef  Min
    #define  Min(x, y)  (((x)>(y)) ? (y):(x))
#endif
#ifndef  Sign
    #define  Sign(x)    (((x)>=0)  ? (1):(-1))
#endif

#define  Loop               while(1)
#define  Xabs(x)            (((x)>=0)  ? (x):(-(x)))
#define  getBit(x, n)       ((sByte)((*(x+(n)/8)>>(7+(n)/8*8-(n)))&0x01))
#define  setBit(x, n, i)    ((i)?(*(x+(n)/8)|=(0x80>>((n)-(n)/8*8))):(*((x)+(n)/8)&=~(0x80>>((n)-(n)/8*8))))
#define  checkBit(dat, bit) (((dat)&(bit))==(bit))
#define  maskBit(dat, bit)  ((dat)&(0xff>>(8-(bit))))

#ifndef  UNUSED
    #define  UNUSED(x)      (void)((x))
#endif


#define  Frand()    (rand()/((double)RAND_MAX+1.0))                      // 0.0〜x未満 の実数を生成
#define  Drand(x)   ((int)(rand()/((double)RAND_MAX+1.0)*((x)+1)))       // 0〜x までの整数を生成
#define  LDrand(x)  ((long int)(rand()/((double)RAND_MAX+1.0)*((x)+1)))  // 0〜x までの long int を生成


#ifndef  WIN32 
#ifndef  Error
    #define  Error(s) {perror(s); return -1;}
#endif
#endif


// 
// for conflict of other Library
#define  JBXL_ON         1
#define  JBXL_OFF        0
#define  JBXL_OK         1
#define  JBXL_NG         0
#define  JBXL_YES        1
#define  JBXL_NO         0
#define  JBXL_TRUE       1
#define  JBXL_FALSE      0

#define  JBXL_Max(x, y)  (((x)<(y)) ? (y):(x))
#define  JBXL_Min(x, y)  (((x)>(y)) ? (y):(x))
#define  JBXL_Sign(x)    (((x)>=0)  ? (1):(-1))

#define  JBXL_EPS        1.0e-6



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
typedef  unsigned char  uByte;
typedef  char           sByte;
typedef  unsigned short uWord;
typedef  short          sWord;
typedef  unsigned int   uDWord;
typedef  int            sDWord;


/** Parameter

汎用パラメター   32/64Byte
*/
typedef struct _parameter32 {
    sByte pm[32];
} Parameter32;


typedef struct _parameter64 {
    sByte pm[64];
} Parameter64;



#endif  // __JBXL_COMMON_HEADER_

