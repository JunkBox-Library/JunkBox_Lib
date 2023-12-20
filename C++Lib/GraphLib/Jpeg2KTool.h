
#ifndef  __JBXL_CPP_JPEG2K_TOOl_H_
#define  __JBXL_CPP_JPEG2K_TOOl_H_


/**
@brief JPEG 2000グラフィックデータ定義用ヘッダ  

現在は JPEG 2000の読み込みのみ．@n
何枚もの画像が含まれていたり，オフセットが指定されている場合には未対応．

@file     Jpeg2KTool.h
@version  0.9
@date     2014 09/24
@author   Fumi.Iseki (C)

@attention
this software is based on OpenJPEG. http://www.openjpeg.org/
*/


#include "Gdata.h"
#include "xtools.h"

/*
#ifndef HAVE_OPENJPEG_H
#ifndef DISABLE_OPENJPEG
#define DISABLE_OPENJPEG
#endif
#endif
*/

#ifdef DISABLE_OPENJPEG
#undef ENABLE_OPENJPEG
#endif


#ifdef ENABLE_OPENJPEG

#ifndef OPENJPEG_VER
  #define OPENJPEG_VER  12
#endif

#define JP2K_VER_12     12
#define JP2K_VER_14     14
#define JP2K_VER_15     15
#define JP2K_VER_20     20
#define JP2K_VER_21     21
#define JP2K_VER_25     25

#undef HAVE_STDLIB_H

#ifdef WIN32
  #if OPENJPEG_VER <= JP2K_VER_15
    #undef OFF
    #ifdef _DEBUG
      #pragma  comment(lib, "OpenJPEGd.lib")    // DLLを使用
    #else
      #pragma  comment(lib, "OpenJPEG.lib")     // DLLを使用
    #endif
  #elif OPENJPEG_VER >= JP2K_VER_21
    #pragma  comment(lib, "openjp2.lib")        // DLLを使用
  #else   // default
    #pragma  comment(lib, "openjp2.lib")        // DLLを使用
  #endif
#endif
 

#include <openjpeg.h>


#define JP2K_MAGIC_RFC3745_JP2  "\x00\x00\x00\x0c\x6a\x50\x20\x20\x0d\x0a\x87\x0a"
#define JP2K_MAGIC_JP2          "\x0d\x0a\x87\x0a"
#define JP2K_MAGIC_J2K          "\xff\x4f\xff\x51"

#define JP2K_FMT_NONE           0
#define JP2K_FMT_JP2            1
#define JP2K_FMT_J2K            2
#define JP2K_FMT_JPT            3


//
namespace jbxl {

////////////////////////////////////////////////////////////////////////////////////////////

class JPEG2KImage 
{
public:
    int     xs;
    int     ys;
    int     ws;
    int     hs;
    int     col;
    int     cmode;
    int     state;

    opj_image_t* image;

public:
    JPEG2KImage(void)  { init();}
    virtual ~JPEG2KImage(void) {}

    void    init(void);                                     ///< グラフィックデータは解放しない
    bool    isNull(void);                                   ///< グラフィックデータを持っていないか？
    void    clear(void) { fill(0);}                         ///< 全空間を画素値 0 にする
    void    fill(int v=0);                                  ///< 全空間を画素値 v にする
    void    free(void);                                     ///< グラフィックデータを開放する

    void    setup_image(void);

    int&    point(int x, int y, int c) { return image->comps->data[c*xs*ys+y*xs+x];}    // 高速化のため，チェックなし．

};



////////////////////////////////////////////////////////////////////////////////////////////

JPEG2KImage readJPEG2KFile(const char* fname);

#if   OPENJPEG_VER <= JP2K_VER_15
JPEG2KImage readJPEG2KData(unsigned char* data, int len, int format);
#elif OPENJPEG_VER >= JP2K_VER_21
JPEG2KImage readJPEG2KData(const char* fname, int format);
#else               //JP2K_VER_20
JPEG2KImage readJPEG2KData(FILE* fp, int format);
#endif

//int       writeJPEG2KFile(const char* fname, JPEG2KImage jp);
//int       writeJPEG2KData(FILE* fp,    JPEG2KImage jp);

int         isJPEG2KHeader(Buffer buf);

//CmnHead       JPEG2KImage2CmnHead(JPEG2KImage jp);
//JPEG2KImage   CmnHead2JPEGImage(CmnHead hd);

// template <typename T>  MSGraph<T>   JPEG2KImage2MSGraph(JPEG2KImage  jp)
// template <typename T>  JPEG2KImage  MSGraph2JPEG2KImage(MSGraph<T> vp)


/**
template <typename T>  MSGraph<T> JPEG2KImage2MSGraph(JPEG2KImage jp)

JPEG 2000イメージデータを MSGraph型イメージデータに変換する

@param  jp  JPEG 2000イメージデータ

@return MSGraphイメージデータ
@retval GRAPH_NODATA_ERROR @b state データ無し
@retval GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
*/
template <typename T>  MSGraph<T> JPEG2KImage2MSGraph(JPEG2KImage jp)
{
    MSGraph<T> vp;

    if (jp.isNull()) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    vp.set(jp.ws, jp.hs, jp.col);
    if (vp.isNull()) return vp;
    vp.color = jp.cmode;

    for (int k=0; k<jp.col; k++) {
        int ps = k*jp.ws*jp.hs;
        for (int j=0; j<jp.hs; j++) {
            int kk = j*jp.ws + ps;
            int yy = j*jp.xs;
            for (int i=0; i<jp.ws; i++) {
                vp.gp[kk + i] = (T)(jp.image->comps[k].data[yy + i]);      // as Byte境界
            }
        }
    }

    return vp;
}


/**
template <typename T>  JPEG2KImage  MSGraph2JPEG2KImage(MSGraph<T> vp)

MSGraph型イメージデータを JPEG 2000イメージデータに変換する

@param  vp  MSGraph型イメージデータ

@return Jpegイメージデータ
@retval GRAPH_NODATA_ERROR @b state データ無し
@retval GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
*/
/*
template <typename T>  JPEG2KImage  MSGraph2JPEG2KImage(MSGraph<T> vp)
{
//  image の初期化とメモリ確保が良く分らん．

    JPEG2KImage jp;

    if (vp.isNull()) {
        jp.state = GRAPH_NODATA_ERROR;
        return jp;
    }

    for (int k=0; k<jp.col; k++) {
        int zp = k*jp.xs*jp.ys;
        for (int j=0; j<jp.ys; j++) {
            int yp = j*jp.xs;
            int yz = yp + zp;
            for (int i=0; i<jp.xs; i++) {
                jp.image->comps[k].data[yp + i] = (int)vp.gp[yz + i];
            }
        }
    }

    return jp;
}
*/


}       // namespace



#endif      // ENABLE_OPENJPEG

#endif      // __JBXL_CPP_JPEG2K_TOOl_H_



 
/*
/////////////////////////////////////////////////////
// Data Structure reffered from opnejpeg.h
/////////////////////////////////////////////////////

OpenJpeg-1.5
typedef struct opj_image
{
    int x0;                     // XOsiz: horizontal offset from the origin of the reference grid to the left side of the image area 
    int y0;                     // YOsiz: vertical offset from the origin of the reference grid to the top side of the image area 
    int x1;                     // Xsiz: width of the reference grid 
    int y1;                     // Ysiz: height of the reference grid 
    int numcomps;               // number of components in the image 
    OPJ_COLOR_SPACE color_space;// color space: sRGB, Greyscale or YUV 
    opj_image_comp_t *comps;    // image components 
    unsigned char *icc_profile_buf; // 'restricted' ICC profile 
    int icc_profile_len;        // size of ICC profile 
} opj_image_t;


OpenJpeg-2.x
typedef struct opj_image 
{
    OPJ_UINT32 x0;              // XOsiz: horizontal offset from the origin of the reference grid to the left side of the image area 
    OPJ_UINT32 y0;              // YOsiz: vertical offset from the origin of the reference grid to the top side of the image area 
    OPJ_UINT32 x1;              // Xsiz: width of the reference grid 
    OPJ_UINT32 y1;              // Ysiz: height of the reference grid 
    OPJ_UINT32 numcomps;        // number of components in the image 
    OPJ_COLOR_SPACE color_space;// color space: sRGB, Greyscale or YUV 
    opj_image_comp_t *comps;    // image components 
    OPJ_BYTE *icc_profile_buf;  // 'restricted' ICC profile 
    OPJ_UINT32 icc_profile_len; // size of ICC profile 
} opj_image_t;


OpenJpeg-1.5
typedef struct opj_image_comp 
{
    int dx;                     // XRsiz: horizontal separation of a sample of ith component with respect to the reference grid 
    int dy;                     // YRsiz: vertical separation of a sample of ith component with respect to the reference grid 
    int w;                      // data width 
    int h;                      // data height 
    int x0;                     // x component offset compared to the whole image 
    int y0;                     // y component offset compared to the whole image 
    int prec;                   // precision 
    int bpp;                    // image depth in bits 
    int sgnd;                   // signed (1) / unsigned (0) 
    int resno_decoded;          // number of decoded resolution 
    int factor;                 // number of division by 2 of the out image compared to the original size of image 
    int *data;                  // image component data 
} opj_image_comp_t; 


OpenJpeg-2.x
typedef struct opj_image_comp 
{   
    OPJ_UINT32 dx;              // XRsiz: horizontal separation of a sample of ith component with respect to the reference grid 
    OPJ_UINT32 dy;              // YRsiz: vertical separation of a sample of ith component with respect to the reference grid 
    OPJ_UINT32 w;               // data width 
    OPJ_UINT32 h;               // data height 
    OPJ_UINT32 x0;              // x component offset compared to the whole image 
    OPJ_UINT32 y0;              // y component offset compared to the whole image 
    OPJ_UINT32 prec;            // precision 
    OPJ_UINT32 bpp;             // image depth in bits 
    OPJ_UINT32 sgnd;            // signed (1) / unsigned (0) 
    OPJ_UINT32 resno_decoded;   // number of decoded resolution 
    OPJ_UINT32 factor;          // number of division by 2 of the out image compared to the original size of image 
    OPJ_INT32 *data;            // image component data 
} opj_image_comp_t;


OpenJpeg-1.5
typedef enum COLOR_SPACE 
{
    CLRSPC_UNKNOWN = -1,        // not supported by the library 
    CLRSPC_UNSPECIFIED = 0,     // not specified in the codestream 
    CLRSPC_SRGB = 1,            // sRGB 
    CLRSPC_GRAY = 2,            // grayscale 
    CLRSPC_SYCC = 3             // YUV 
} OPJ_COLOR_SPACE;


OpenJpeg-2.x
typedef enum COLOR_SPACE 
{
    OPJ_CLRSPC_UNKNOWN = -1,    // not supported by the library 
    OPJ_CLRSPC_UNSPECIFIED = 0, // not specified in the codestream 
    OPJ_CLRSPC_SRGB = 1,        // sRGB 
    OPJ_CLRSPC_GRAY = 2,        // grayscale 
    OPJ_CLRSPC_SYCC = 3         // YUV 
} OPJ_COLOR_SPACE;

*/

