#ifndef  __JBXL_CPP_PNG_TOOl_H_
#define  __JBXL_CPP_PNG_TOOl_H_

/**
@brief    PNGグラフィックデータ定義用ヘッダ  
@file     JpegTool.h
@version  0.9
@date     2009 3/8
@author   Fumi.Iseki (C)

@attention
*/

#include "Gdata.h"
#include "xtools.h"

/*
#ifndef HAVE_PNG_H
#ifndef DISABLE_PNG
#define DISABLE_PNG
#endif
#endif
*/

#ifdef  DISABLE_PNG
#undef  ENABLE_PNG
#endif


#ifdef ENABLE_PNG

#include <png.h>

/*
#ifdef WIN32
#if defined(_DEBUG)
#pragma  comment(lib, "libpng16d.lib")
#else
#pragma  comment(lib, "libpng16.lib")
#endif
#endif
*/

/*
type
#define PNG_COLOR_TYPE_GRAY 0
#define PNG_COLOR_TYPE_PALETTE    (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_RGB        (PNG_COLOR_MASK_COLOR)
#define PNG_COLOR_TYPE_RGB_ALPHA  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
// aliases 
#define PNG_COLOR_TYPE_RGBA  PNG_COLOR_TYPE_RGB_ALPHA
#define PNG_COLOR_TYPE_GA    PNG_COLOR_TYPE_GRAY_ALPHA
*/


#ifndef PNG_SIGNATURE_SIZE
#define  PNG_SIGNATURE_SIZE  8
#endif


//
namespace jbxl {


////////////////////////////////////////////////////////////////////////////////////////////

class PNGImage 
{
public:
    int     xs;
    int     ys;
    int     col;
    int     length;
    int     state;

    uByte   type;
    uByte*  gp;         // Bitmap

private:
    png_structp strct;
    png_infop   info;

public:
    PNGImage(void)  { init();}
    virtual ~PNGImage(void) {}

    void    init(void);                 ///< 初期化．グラフィックデータは解放しない
    bool    isNull(void);               ///< グラフィックデータを持っていないか？
    void    clear(void);                ///< 全空間を画素値 0 にする
    void    fill(uByte v=(uByte)0);     ///< 全空間を画素値 v にする
    void    free(void);                 ///< グラフィックデータを開放する

    uByte&  point(int x, int y, int c)   { return gp[col*(y*xs + x) + c];}
    void    getm(int x, int y, int c);
    void    setzero(int x, int y, int c) { getm(x, y, c); clear();}

    void    readData(FILE* fp);
    int     writeData(FILE* fp);
};



//////////////////////////////////////////////////////////////////////////////////////////////////

PNGImage    readPNGFile (const char* fname);
PNGImage    readPNGData (FILE* fp);
int         writePNGFile(const char* fname, PNGImage* png);
int         writePNGData(FILE* fp, PNGImage* png);

//int        isPNGHeader(Buffer buf);

// template <typename T>  MSGraph<T> PNGImage2MSGraph<T>(PNGImage  png)
// template <typename T>  PNGImage  MSGraph2PNGImage(MSGraph<T> vp)


/**
template <typename T>  MSGraph<T> PNGImage2MSGraph(PNGImage png)

PNGイメージデータを MSGraph型イメージデータに変換する

@param  png  PNGイメージデータ
@return MSGraphイメージデータ
@retval JBXL_GRAPH_NODATA_ERROR   @b state データ無し
@retval JBXL_GRAPH_MEMORY_ERROR   @b state メモリ確保エラー 
@retval JBXL_GRAPH_IVDCOLOR_ERROR @b state 無効なカラー指定
*/
template <typename T>  MSGraph<T> PNGImage2MSGraph(PNGImage png)
{
    MSGraph<T> vp;

    if (png.isNull()) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    vp.set(png.xs, png.ys, png.col);
    if (vp.isNull()) {
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }
    //
    if      (png.col==4) vp.color = GRAPH_COLOR_BGRA;
    else if (png.col==3) vp.color = GRAPH_COLOR_BGR;
    else if (png.col==2) vp.color = GRAPH_COLOR_GA;
    else if (png.col==1) vp.color = GRAPH_COLOR_GRAY;
    else {
        vp.free();
        vp.init();
        vp.state = JBXL_GRAPH_IVDCOLOR_ERROR;
        return vp;
    }

    for (int k=0; k<png.col; k++) {
        int zp = k*png.xs*png.ys;
        for (int j=0; j<png.ys; j++) {
            int yp = zp + j*png.xs;
            for (int i=0; i<png.xs; i++) {
                vp.gp[yp + i] = (T)png.point(i, png.ys-1-j, k);
            }
        }
    }

    return vp;
}


/**
template <typename T>  PNGImage  MSGraph2PNGImage(MSGraph<T> vp)

MSGraph型イメージデータを PNGイメージデータに変換する
ヘッダ情報は変換しない（別途変換する）．

@param  vp   MSGraph型イメージデータ
@param  rle  RLE（連長圧縮）を行うかどうか

@return PNGイメージデータ
@retval JBXL_GRAPH_NODATA_ERROR   @b state データ無し
@retval JBXL_GRAPH_MEMORY_ERROR   @b state メモリ確保エラー 
@retval JBXL_GRAPH_IVDCOLOR_ERROR @b state 無効なカラー指定
*/
template <typename T>  PNGImage  MSGraph2PNGImage(MSGraph<T> vp)
{
    PNGImage png;
    png.init();

    if (vp.isNull()) {
        png.state = JBXL_GRAPH_NODATA_ERROR;
        return png;
    }

    png.setzero(vp.xs, vp.ys, vp.zs);
    if (png.isNull()) {
        png.init();
        png.state = JBXL_GRAPH_MEMORY_ERROR;
        return png;
    }

    if (vp.color==GRAPH_COLOR_UNKNOWN) {
        if      (vp.zs==1) vp.color = GRAPH_COLOR_GRAY;
        else if (vp.zs==2) vp.color = GRAPH_COLOR_GA;
        else if (vp.zs==3) vp.color = GRAPH_COLOR_RGB;
        else if (vp.zs==4) vp.color = GRAPH_COLOR_RGBA;
    }
    if      (vp.color==GRAPH_COLOR_GRAY) png.type = PNG_COLOR_TYPE_GRAY;
    else if (vp.color==GRAPH_COLOR_GA)   png.type = PNG_COLOR_TYPE_GA;
    else if (vp.color==GRAPH_COLOR_RGB)  png.type = PNG_COLOR_TYPE_RGB;
    else if (vp.color==GRAPH_COLOR_RGBA) png.type = PNG_COLOR_TYPE_RGBA;
    else {
        png.free();
        png.init();
        png.state = JBXL_GRAPH_IVDCOLOR_ERROR;
        return png;
    }

    for (int k=0; k<png.col; k++) {
       int zp = k*png.xs*png.ys;
        for (int j=0; j<png.ys; j++) {
            int yp = zp + j*png.xs;
            for (int i=0; i<png.xs; i++) {
                png.point(i, j, k) = (uByte)vp.gp[yp + i];
            }
        }
    }

    png.state = JBXL_NORMAL;
    return png;
}


}       // namespace


#endif  // ENABLE_PNG

#endif  // __JBXL_CPP_PNG_TOOl_H_
