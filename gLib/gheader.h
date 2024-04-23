
#ifndef  __JBXL_GRAPHIC_HEADER_H_
#define  __JBXL_GRAPHIC_HEADER_H_


/**
@brief    グラフィックデータヘッダ定義
@file     gheader.h
@version  3.0
@author   Fumi.Iseki (C)
@date     2002 8/27


ファイル形式とヘッダ種別
---
@par Common形式
@code
    CmnHead_Entry   cmhd   : common header. 36Byte. 
    Graphic　       Header : 画像データ固有のヘッダ
    Graphic         Data   : データ
@endcode
任意の画像データを保存する時，その画像データに識別ヘッダ(CmnHead) を付けて保存したもの．

@par UN_KNOWN_DATA
@code
    CmnHead_Entry   cmhd    : common header. 36Byte.　（省略可）
    Any Graphic             : 任意のグラフィックデータ
    [Graphic　      Header] : 画像データ固有のヘッダ
    [Graphic        Data]   : データ
@endcode
Common Headerは省略可（省略した場合は，本来のファイル形式）@n
画像フォーマットを解析できない場合のファイル形式．従って，cmhd はkind以外意味がない．@n
この形式で保存することも，ヘッダを統一的に扱うと言う以外に特に意味はない．

@par RAS_DATA
@code
    CmnHead_Entry   cmhd : common header. 36Byte. （省略可）
    RasHead         rshd : SunRasterのヘッダー．32Byte. メンバは int型. 
    Graphic         Data : データ
@endcode
Common Headerは省略可．Sun Rasterの一番単純な型(Starndard型)

@par USERSET_DATA
@code
    CmnHead_Entry   cmhd : common header. 36Byte. （省略可）
    ANY HEADER           : ユーザ指定のヘッダ． 
    Graphic         Data : データ
@endcode
Common Headeri Entry は省略可．また，ANY HEADERが無い場合もある．@n
ヘッダ形式をオペレータ（ユーザ）が指定する場合の型．@n
ヘッダ情報を設定して，読み込み・書き込みの関数に渡す必要がある．

@attention
注）CTファイルに関しては Gio.h を見よ．
*/


#include "tools.h"


///////////////////////////////////////////////////////////////////////////////////////
// CT(Moon) format ヘッダ
// 
typedef struct _CTHead {
    sWord  xsize;
    sWord  ysize;
    sWord  ctmin;
    sWord  ctmax;
    sWord  cutup; 
    sWord  cutdown;
    sWord  cutleft;
    sWord  cutright;
    sWord  anydata[23];
    sWord  img_flag;
} CTHead;


///////////////////////////////////////////////////////////////////////////////////////
// SUN RASTER File
// see...   man 5 rasterfile 
//
typedef struct _RasHead {
    int  ras_magic;
    int  ras_width;
    int  ras_height;
    int  ras_depth;
    int  ras_length;
    int  ras_type;
    int  ras_maptype;
    int  ras_maplength;
} RasHead;


#define RAS_MAGIC           0x59a66a95
#define RAS_RLE             0x80
#define RT_OLD              0  
#define RT_STANDARD         1 
#define RT_BYTE_ENCODED     2 
#define RT_FORMAT_RGB       3
#define RMT_NONE            0
#define RMT_EQUAL_RGB       1
#define RMT_RAW             2


///////////////////////////////////////////////////////////////////////////////////////
// 共通ヘッダ
//

// ファイル用
typedef struct _CmnHead_Entry {
    int kind;                   ///< Kind of Graphics Format
    int xsize;                  ///< Width of Graphics
    int ysize;                  ///< Height of Graphics
    int zsize;                  ///< For 3D Data
    int depth;                  ///< Color Depth of Graphics       (bit  unit)  
    unsigned int bsize;         ///< Fllowing buf size or Any Data (byte unit) 
    unsigned int lsize;         ///< Size of Graphics Data         (byte unit) 
    int option;                 ///< General purpose (pating)
} CmnHead_Entry;

// 作業用
// x86 と x64 でサイズが変化するので注意！
typedef struct _CmnHead {
    union {
        CmnHead_Entry entry;
        struct {
            int kind;           ///< Kind of Graphics Format
            int xsize;          ///< Width of Graphics
            int ysize;          ///< Height of Graphics
            int zsize;          ///< For 3D Data
            int depth;          ///< Color Depth of Graphics       (bit  unit)  
            unsigned int bsize; ///< Fllowing buf size or Any Data (byte unit) 
            unsigned int lsize; ///< Size of Graphics Data         (byte unit) 
            int option;         ///< General purpose (pating)
        };
    };
    uByte*    buf;              ///< Ture Header buffer  
    uByte*    grptr;            ///< Pointer to Data 
} CmnHead;

/*
typedef struct _CmnHead {
    int kind;                   ///< Kind of Graphics Format
    int xsize;                  ///< Width of Graphics
    int ysize;                  ///< Height of Graphics
    int zsize;                  ///< For 3D Data
    int depth;                  ///< Color Depth of Graphics       (bit  unit)  
    unsigned int bsize;         ///< Fllowing buf size or Any Data (byte unit) 
    unsigned int lsize;         ///< Size of Graphics Data         (byte unit) 
    uByte* buf;                 ///< Ture Header buffer  
    uByte* grptr;               ///< Pointer to Data 
} CmnHead;
*/


///////////////////////////////////////////////////////////////////////////////////////
//
typedef struct _RGB24Data
{
    uByte  R;
    uByte  G;
    uByte  B;
} RGB24Data;


///////////////////////////////////////////////////////////////////////////////////////
// ヘッダ型
// 0x0000 - 0x0080 
#define  UN_KNOWN_DATA          0       ///< 0x0000 // 知らないデータ形式（システムにお任せ）
#define  PROC_DATA              1       ///< 0x0001 // 内部(DLL)処理されたデータ形式
#define  CREATE_DATA            2       ///< 0x0002 // 内部(DLL)で作成されたデータ形式
#define  MOON_DATA              3       ///< 0x0003 // CT DATA
#define  DICOM_DATA             4       ///< 0x0004 // DICOM Header
#define  USERSET_DATA           5       ///< 0x0005 // ユーザ指定のデータ形式（ヘッダ形式指定の場合，使用する）

#define  RAS_DATA               8       ///< 0x0008 // SUN RASTER 8bit
#define  RAS8_DATA              8       ///< 0x0008 // SUN RASTER 8bit
#define  RAS16_DATA             9       ///< 0x0009 // SUN RASTER 16bit
#define  RAS24_DATA             9       ///< 0x0001 // SUN RASTER 24bit

// for CT Data
#define  CT_DATA                16      ///< 0x0010 // CT DATA (Moon形式) 
#define  CT_3DM                 18      ///< 0x0012 // 3D CT DATA（マルチスライス）
#define  CT_3D_VOL              19      ///< 0x0013 // CT ボリュームデータ
#define  CT_RGN_SL              20      ///< 0x0014 // CT Region Slice DATA

// for Well Known Graphic Data
#define  JPEG_DATA              32      ///< 0x0020 // JPEG RGB
#define  JPEG_RGB_DATA          32      ///< 0x0020 // JPEG RGB
#define  JPEG_MONO_DATA         33      ///< 0x0021 // JPEG MONO
#define  JPEG16_RGB_DATA        34      ///< 0x0022 // 書き込み用(16bit->32bit変換)
#define  JPEG16_ARGB_DATA       35      ///< 0x0023 // 書き込み用(16bit->32bit変換)
#define  JPEG16_RGBA_DATA       36      ///< 0x0024 // 書き込み用(16bit->32bit変換)
#define  JPEG_ARGB_DATA         37      ///< 0x0025 // 書き込み用(アルファチャンネルは削除される)
#define  JPEG_RGBA_DATA         38      ///< 0x0026 // 書き込み用(アルファチャンネルは削除される)

#define  TIFF_DATA              48      ///< 0x0030 // TIFF
#define  PNG_DATA               64      ///< 0x0040 // PNG

#define  TGA_DATA               80      ///< 0x0050 // TGA

// 0x0100 - 0x4000 パラメータ 
#define  HAS_DUMMY1             256     ///< 0x0100
#define  HAS_DUMMY2             512     ///< 0x0200   
#define  HAS_BASE               1024    ///< 0x0400 // 基底（底上げ）値を持つ
#define  HAS_ZSPACE             2048    ///< 0x0800 
#define  HAS_RZXY               4096    ///< 0x1000 // with RZxy data 
#define  HAS_RBOUND             8192    ///< 0x2000 // with RBound data 
#define  HAS_LENDIAN            16384   ///< 0x4000 // リトルエンディアン

#define  HEADER_NONE            32768   ///< 0x8000 // ヘッダ種別の指定なし
#define  NUM_KDATA              32767   ///< 0x7fff // Max Numver of Data Format

#define  RZXY_RATE              10000   ///< RZxy をファイルのヘッダに埋め込む際の比率（倍率）． 
#define  ZEROCT_BASE            4096

// Data Name
#define  UN_KNOWN_DATA_STR      "UNKNOWN DATA"
#define  PROC_DATA_STR          "PROC DATA"
#define  CREATE_DATA_STR        "CREATE DATA"
#define  USERSET_DATA_STR       "USERSET DATA"
#define  DICOM_DATA_STR         "DICOM"
#define  RAS_DATA_STR           "SUN RASTER"
#define  CT_DATA_STR            "Common Header"
#define  CT_3DM_STR             "Common 3D"
#define  MOON_DATA_STR          "CT (Moon)"
#define  JPEG_RGB_DATA_STR      "JPEG RGB"
#define  JPEG_MONO_DATA_STR     "JPEG MONO"
#define  PNG_DATA_STR           "PNG"
#define  TIFF_DATA_STR          "TIFF"
#define  CT_3D_VOL_STR          "3D VOL"

#define  STATE_GRAPH_NOERR      0

/**
カラータイプ
   ピクセルデータ毎にカラーの情報が入っているか，
   カラー毎に PLANEを持っているか（PLANEモード）は zsize(col) の値で判断する．
*/

#define  GRAPH_COLOR_MONO       0       ///< 0x0000   no PLANEモード only
#define  GRAPH_COLOR_MONO8      0
#define  GRAPH_COLOR_GRAY       0

#define  GRAPH_COLOR_MONO16     16      ///< 0x0010   no PLANEモード only
#define  GRAPH_COLOR_GRAY16     16

#define  GRAPH_COLOR_R5G6B5     17      ///< 0x0011   no PLANEモード only
#define  GRAPH_COLOR_RGB16      17

#define  GRAPH_COLOR_R4G4B4A4   18      ///< 0x0012   no PLANEモード only
#define  GRAPH_COLOR_RGBA16     18

#define  GRAPH_COLOR_A4R4G4B4   19      ///< 0x0013   no PLANEモード only
#define  GRAPH_COLOR_ARGB16     19
 
// 2 PLANE or 16bit Pixcel
#define  GRAPH_COLOR_MA         20      ///< 0x0014
#define  GRAPH_COLOR_M8A8       20
#define  GRAPH_COLOR_MA16       20

// 3 PLNAE or 24bit Pixcel
#define  GRAPH_COLOR_R8G8B8     32      ///< 0x0020
#define  GRAPH_COLOR_RGB24      32
#define  GRAPH_COLOR_RGB        32

#define  GRAPH_COLOR_B8G8R8     48      ///< 0x0030
#define  GRAPH_COLOR_BGR24      48
#define  GRAPH_COLOR_BGR        48

// 4 Plane or 32bit Pixcel
#define  GRAPH_COLOR_A8R8G8B8   64      ///< 0x0040
#define  GRAPH_COLOR_ARGB32     64
#define  GRAPH_COLOR_ARGB       64

#define  GRAPH_COLOR_R8G8B8A8   65      ///< 0x0041
#define  GRAPH_COLOR_RGBA32     65
#define  GRAPH_COLOR_RGBA       65

#define  GRAPH_COLOR_X8R8G8B8   66      ///< 0x0042
#define  GRAPH_COLOR_XRGB32     66
#define  GRAPH_COLOR_XRGB       66

#define  GRAPH_COLOR_A8B8G8R8   80      ///< 0x0050
#define  GRAPH_COLOR_ABGR32     80
#define  GRAPH_COLOR_ABGR       80

#define  GRAPH_COLOR_B8G8R8A8   81      ///< 0x0051
#define  GRAPH_COLOR_BGRA32     81
#define  GRAPH_COLOR_BGRA       81

#define  GRAPH_COLOR_X8B8G8R8   82      ///< 0x0052
#define  GRAPH_COLOR_XBGR32     82
#define  GRAPH_COLOR_XBGR       82


#define  GRAPH_COLOR_UNKNOWN    112     ///< 0x0070


/////////////////////////////////////////////////////////////////////////////
// DICOM
#define  DICOM_STUDY_GROUP      0x0020
#define  DICOM_IMAGE_GROUP      0x0028
#define  DICOM_PIXEL_GROUP      0x7fe0

#define  DICOM_PIXEL_ELEMENT    0x0010
#define  DICOM_XSIZE_ELEMENT    0x0011
#define  DICOM_YSIZE_ELEMENT    0x0010
#define  DICOM_PXLSPC_ELEMENT   0x0030
#define  DICOM_DEPTH_ELEMENT    0x0100
#define  DICOM_GRYLV_ELEMENT    0x0101

#define  DICOM_PIXCEL_VR        0x574f  ///< Not
#define  DICOM_STR_VR           0x5344  ///< Z歪
#define  DICOM_INT_VR           0x5355  ///< X, Y


#endif      // __JBXL_GRAPHIC_HEADER_H_

