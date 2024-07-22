#ifndef  __JBXL_CPP_JPEG_TOOl_H_
#define  __JBXL_CPP_JPEG_TOOl_H_

/**
@brief    JPEGグラフィックデータ定義用ヘッダ  
@file     JpegTool.h
@version  0.9
@date     2009 3/8
@author   Fumi.Iseki (C)

@attention
this software is based in part on the work of the Independent JPEG Group. http://www.ijg.org/
*/

#include "Gdata.h"
#include "xtools.h"

#ifndef HAVE_JPEGLIB_H
#ifndef DISABLE_JPEG
#define DISABLE_JPEG
#endif
#endif

#if  defined(DISABLE_JPEG) || defined(DISABLE_JPEGLIB)
#undef  ENABLE_JPEGLIB      // for old version
#undef  ENABLE_JPEG
#endif

#ifdef ENABLE_JPEGLIB
#define ENABLE_JPEG
#endif


#ifdef ENABLE_JPEG

#undef HAVE_STDLIB_H
#include <jpeglib.h>

#ifdef WIN32
#pragma  comment(lib, "libjpeg.lib")
#endif


//
namespace jbxl {

////////////////////////////////////////////////////////////////////////////////////////////

class JPEGImage 
{
public:
    int     xs;
    int     ys;
    int     col;
    int     state;

    JSAMPLE*    gp;
    JSAMPARRAY  image;

public:
    JPEGImage(void)  { init();}
    virtual ~JPEGImage(void) {}

    void    init(void);                                     ///< グラフィックデータは解放しない
    bool    isNull(void);                                   ///< グラフィックデータを持っていないか？
    void    clear(void);                                    ///< 全空間を画素値 0 にする
    void    fill(JSAMPLE v=(JSAMPLE)0);                     ///< 全空間を画素値 v にする
    void    free(void);                                     ///< グラフィックデータを開放する

    JSAMPLE& point(int x, int y, int c) { return image[y][x*col + c];}
    void    getm(int x, int y, int c);
    void    set (int x, int y, int c);
};


//////////////////////////////////////////////////////////////////////////////////////////////////

JPEGImage   readJPEGFile (const char* fname);
JPEGImage   readJPEGData (FILE* fp);
int         writeJPEGFile(const char* fname, JPEGImage* jp, int qulty);
int         writeJPEGData(FILE* fp, JPEGImage* jp, int qulty);

int         isJPEGHeader(Buffer buf);

CmnHead     JPEGImage2CmnHead(JPEGImage jp);
JPEGImage   CmnHead2JPEGImage(CmnHead hd);


// template <typename T>  MSGraph<T> JPEGImage2MSGraph(JPEGImage  jp)
// template <typename T>  JPEGImage  MSGraph2JPEGImage(MSGraph<T> vp)


/**
template <typename T>  MSGraph<T> JPEGImage2MSGraph(JPEGImage jp)

Jpegイメージデータを MSGraph型イメージデータに変換する

@param  jp  Jpegイメージデータ
@return MSGraphイメージデータ
@retval JBXL_GRAPH_NODATA_ERROR @b state データ無し
@retval JBXL_GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
*/
template <typename T>  MSGraph<T> JPEGImage2MSGraph(JPEGImage jp)
{
    MSGraph<T> vp;

    if (jp.isNull()) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    vp.set(jp.xs, jp.ys, jp.col);
    if (vp.isNull()) return vp;
    //if (jp.col==3) vp.color = GRAPH_COLOR_PRGB;
    if (jp.col==3) vp.color = GRAPH_COLOR_RGB;

    for (int k=0; k<jp.col; k++) {
        int zp = k*jp.xs*jp.ys;
        for (int j=0; j<jp.ys; j++) {
            int yp = zp + j*jp.xs;
            for (int i=0; i<jp.xs; i++) {
                vp.gp[yp + i] = (T)jp.point(i, j, k);
            }
        }
    }

    return vp;
}


/**
template <typename T>  JPEGImage  MSGraph2JPEGImage(MSGraph<T> vp)

MSGraph型イメージデータを Jpegイメージデータに変換する

@param  vp  MSGraph型イメージデータ
@return Jpegイメージデータ
@retval JBXL_GRAPH_NODATA_ERROR @b state データ無し
@retval JBXL_GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
*/
template <typename T>  JPEGImage  MSGraph2JPEGImage(MSGraph<T> vp)
{
    JPEGImage jp;

    if (vp.isNull()) {
        jp.state = JBXL_GRAPH_NODATA_ERROR;
        return jp;
    }

    jp.set(vp.xs, vp.ys, vp.zs);
    if (jp.isNull()) return jp;

    for (int k=0; k<jp.col; k++) {
        int zp = k*jp.xs*jp.ys;
        for (int j=0; j<jp.ys; j++) {
            int yp = zp + j*jp.xs;
            for (int i=0; i<jp.xs; i++) {
                jp.point(i, j, k) = (JSAMPLE)vp.gp[yp + i];
            }
        }
    }

    return jp;
}


}       // namespace


#endif  // ENABLE_JPEG

#endif  // __JBXL_CPP_JPEG_TOOl_H_


 
