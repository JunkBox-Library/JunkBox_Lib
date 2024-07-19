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

#ifndef HAVE_PNGLIB_H
#ifndef DISABLE_PNG
#define DISABLE_PNG
#endif
#endif

#ifdef  DISABLE_PNG
#undef  ENABLE_PNG
#endif


#ifdef ENABLE_PNG

#include <png.h>

#ifdef WIN32
#pragma  comment(lib, "libpng.lib")
#endif


#define  PNG_HEADER_SIZE  18
#define  PNG_FOOTER_SIZE  26
#define  PNG_FOOTER_STR   "TRUEVISION-XFILE."


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

    uByte   hd[PNG_HEADER_SIZE];
    uByte   ft[PNG_FOOTER_SIZE];
    uByte*  gp;                 // BGRA, BGR, MONO, MA

public:
    PNGImage(void)  { init();}
    virtual ~PNGImage(void) {}

    void    init(void);                                     ///< グラフィックデータは解放しない
    bool    isNull(void);                                   ///< グラフィックデータを持っていないか？
    void    clear(void);                                    ///< 全空間を画素値 0 にする
    void    fill(uByte v=(uByte)0);                         ///< 全空間を画素値 v にする
    void    free(void);                                     ///< グラフィックデータを開放する

    uByte&  point(int x, int y, int c) { return gp[col*(y*xs + x) + c];}
    void    getm(int x, int y, int c);
    void    setzero(int x, int y, int c);
};



//////////////////////////////////////////////////////////////////////////////////////////////////

PNGImage    readPNGFile (const char* fname);
PNGImage    readPNGData (FILE* fp);
int         writePNGFile(const char* fname, PNGImage tga);
int         writePNGData(FILE* fp, PNGImage tga);

int         setupPNGData(PNGImage* tga, bool rle);

//int        isPNGHeader(Buffer buf);

// template <typename T>  MSGraph<T> PNGImage2MSGraph<T>(PNGImage  tga)
// template <typename T>  PNGImage  MSGraph2PNGImage(MSGraph<T> vp)


/**
template <typename T>  MSGraph<T> PNGImage2MSGraph(PNGImage tga)

PNGイメージデータを MSGraph型イメージデータに変換する

@param  tga  PNGイメージデータ
@return MSGraphイメージデータ
@retval JBXL_GRAPH_NODATA_ERROR @b state データ無し
@retval JBXL_GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
*/
template <typename T>  MSGraph<T> PNGImage2MSGraph(PNGImage tga)
{
    MSGraph<T> vp;

    if (tga.isNull()) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    vp.set(tga.xs, tga.ys, tga.col);
    if (vp.isNull()) return vp;
    //
    if      (tga.col==4) vp.color = GRAPH_COLOR_BGRA;
    else if (tga.col==3) vp.color = GRAPH_COLOR_BGR;
    else if (tga.col==2) vp.color = GRAPH_COLOR_MA;
    else if (tga.col==1) vp.color = GRAPH_COLOR_GRAY;
    else {
        vp.state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    uByte dirx = tga.hd[17] & 0x10;         // X方向 0: Left -> Right
    uByte diry = tga.hd[17] & 0x20;         // Y方向 0: Down -> Top

    if (dirx==0x00 && diry==0x00) {         // Left->Right, Down->Top
        for (int k=0; k<tga.col; k++) {
            int zp = k*tga.xs*tga.ys;
            for (int j=0; j<tga.ys; j++) {
                int yp = zp + j*tga.xs;
                for (int i=0; i<tga.xs; i++) {
                    vp.gp[yp + i] = (T)tga.point(i, tga.ys-1-j, k);
                }
            }
        }
    }
    else if (dirx==0x00 && diry==0x20) {    // Left->Right, Top->Down
        for (int k=0; k<tga.col; k++) {
            int zp = k*tga.xs*tga.ys;
            for (int j=0; j<tga.ys; j++) {
                int yp = zp + j*tga.xs;
                for (int i=0; i<tga.xs; i++) {
                    vp.gp[yp + i] = (T)tga.point(i, j, k);
                }
            }
        }
    }
    else if (dirx==0x10 && diry==0x00) {    // Right->Left, Down->Top
        for (int k=0; k<tga.col; k++) {
            int zp = k*tga.xs*tga.ys;
            for (int j=0; j<tga.ys; j++) {
                int yp = zp + j*tga.xs;
                for (int i=0; i<tga.xs; i++) {
                    vp.gp[yp + i] = (T)tga.point(tga.xs-1-i, tga.ys-1-j, k);
                }
            }
        }
    }
    else {
        for (int k=0; k<tga.col; k++) {     // Right->Left, Top->Down
            int zp = k*tga.xs*tga.ys;
            for (int j=0; j<tga.ys; j++) {
                int yp = zp + j*tga.xs;
                for (int i=0; i<tga.xs; i++) {
                    vp.gp[yp + i] = (T)tga.point(tga.xs-1-i, j, k);
                }
            }
        }
    }
    return vp;
}


/**
template <typename T>  PNGImage  MSGraph2PNGImage(MSGraph<T> vp, bool rle)

MSGraph型イメージデータを PNGイメージデータに変換する
ヘッダ情報は変換しない（別途変換する）．

@param  vp   MSGraph型イメージデータ
@param  rle  RLE（連長圧縮）を行うかどうか

@return PNGイメージデータ
@retval JBXL_GRAPH_NODATA_ERROR @b state データ無し
@retval JBXL_GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
*/
template <typename T>  PNGImage  MSGraph2PNGImage(MSGraph<T> vp, bool rle)
{
    PNGImage tga;
    tga.init();

    if (vp.isNull()) {
        tga.state = JBXL_GRAPH_NODATA_ERROR;
        return tga;
    }

    tga.setzero(vp.xs, vp.ys, vp.zs);
    tga.length = tga.xs*tga.ys*tga.col;
    if (tga.isNull()) return tga;

    if (vp.color==GRAPH_COLOR_UNKNOWN) {
        if      (vp.zs==1) vp.color = GRAPH_COLOR_GRAY;
        else if (vp.zs==2) vp.color = GRAPH_COLOR_MA;
        else if (vp.zs==3) vp.color = GRAPH_COLOR_RGB;
        else if (vp.zs==4) vp.color = GRAPH_COLOR_RGBA;
    }

    //
    if (vp.color==GRAPH_COLOR_BGRA || vp.color==GRAPH_COLOR_BGR || vp.color==GRAPH_COLOR_GRAY || vp.color==GRAPH_COLOR_MA) { 
        for (int k=0; k<tga.col; k++) {
            int zp = k*tga.xs*tga.ys;
            for (int j=0; j<tga.ys; j++) {
                int yp = zp + j*tga.xs;
                for (int i=0; i<tga.xs; i++) {
                    tga.point(i, j, k) = (uByte)vp.gp[yp + i];
                }
            }
        }
    }
    //
    else if (vp.color==GRAPH_COLOR_RGB || vp.color==GRAPH_COLOR_RGBA) { 
        for (int k=0; k<3; k++) {
            int zp = (2-k)*tga.xs*tga.ys;
            for (int j=0; j<tga.ys; j++) {
                int yp = zp + j*tga.xs;
                for (int i=0; i<tga.xs; i++) {
                    tga.point(i, j, k) = (uByte)vp.gp[yp + i];
                }
            }
        }
        if (vp.color==GRAPH_COLOR_RGBA) {   // αチャンネル
            int zp = 3*tga.xs*tga.ys;
            for (int j=0; j<tga.ys; j++) {
                int yp = zp + j*tga.xs;
                for (int i=0; i<tga.xs; i++) {
                    tga.point(i, j, 3) = (uByte)vp.gp[yp + i];
                }
            }
        }
    }
    //
    else if (vp.color==GRAPH_COLOR_ABGR) { 
        for (int j=0; j<tga.ys; j++) {      // αチャンネル
            int yp = j*tga.xs;
            for (int i=0; i<tga.xs; i++) {
                tga.point(i, j, 3) = (uByte)vp.gp[yp + i];
            }
        }
        for (int k=1; k<4; k++) {
            int zp = k*tga.xs*tga.ys;
            for (int j=0; j<tga.ys; j++) {
                int yp = zp + j*tga.xs;
                for (int i=0; i<tga.xs; i++) {
                    tga.point(i, j, k-1) = (uByte)vp.gp[yp + i];
                }
            }
        }
    }
    //
    else if (vp.color==GRAPH_COLOR_ARGB) { 
        for (int j=0; j<tga.ys; j++) {      // αチャンネル
            int yp = j*tga.xs;
            for (int i=0; i<tga.xs; i++) {
                tga.point(i, j, 3) = (uByte)vp.gp[yp + i];
            }
        }
        for (int k=1; k<4; k++) {
            int zp = (4-k)*tga.xs*tga.ys;
            for (int j=0; j<tga.ys; j++) {
                int yp = zp + j*tga.xs;
                for (int i=0; i<tga.xs; i++) {
                    tga.point(i, j, k-1) = (uByte)vp.gp[yp + i];
                }
            }
        }
    }
    else {
        tga.state = JBXL_GRAPH_IVDARG_ERROR;
        tga.free();
    }

    if (tga.state==0) setupPNGData(&tga, rle);

    return tga;
}


}       // namespace


#endif  // ENABLE_PNG

#endif  // __JBXL_CPP_PNG_TOOl_H_
