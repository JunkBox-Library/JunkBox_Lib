#ifndef  __JBXL_CPP_TGA_TOOl_H_
#define  __JBXL_CPP_TGA_TOOl_H_

/**
@brief    TGAグラフィックデータ定義用ヘッダ  
@file     TgaTool.h
@version  1.0β
@date     2024 2/15
@author   Fumi.Iseki (C)
*/

#include "Gdata.h"
#include "xtools.h"


#define  TGA_HEADER_SIZE  18
#define  TGA_FOOTER_SIZE  26
#define  TGA_FOOTER_STR   "TRUEVISION-XFILE."


//
namespace jbxl {


////////////////////////////////////////////////////////////////////////////////////////////

class TGAImage 
{
public:
    int     xs;
    int     ys;
    int     col;
    int     length;
    int     state;

    uByte   hd[TGA_HEADER_SIZE];
    uByte   ft[TGA_FOOTER_SIZE];
    uByte*  gp;                 // BGRA, BGR, MONO, MA

public:
    TGAImage(void)  { init();}
    virtual ~TGAImage(void) {}

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

TGAImage    readTGAFile (const char* fname);
TGAImage    readTGAData (FILE* fp);
int         writeTGAFile(const char* fname, TGAImage tga);
int         writeTGAData(FILE* fp, TGAImage tga);

int         setupTGAData(TGAImage* tga, bool rle);

//int        isTGAHeader(Buffer buf);

// template <typename T>  MSGraph<T> TGAImage2MSGraph<T>(TGAImage  tga)
// template <typename T>  TGAImage  MSGraph2TGAImage(MSGraph<T> vp)


/**
template <typename T>  MSGraph<T> TGAImage2MSGraph(TGAImage tga)

TGAイメージデータを MSGraph型イメージデータに変換する

@param  tga  TGAイメージデータ
@return MSGraphイメージデータ
@retval JBXL_GRAPH_NODATA_ERROR @b state データ無し
@retval JBXL_GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
*/
template <typename T>  MSGraph<T> TGAImage2MSGraph(TGAImage tga)
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
template <typename T>  TGAImage  MSGraph2TGAImage(MSGraph<T> vp, bool rle)

MSGraph型イメージデータを TGAイメージデータに変換する
ヘッダ情報は変換しない（別途変換する）．

@param  vp   MSGraph型イメージデータ
@param  rle  RLE（連長圧縮）を行うかどうか

@return TGAイメージデータ
@retval JBXL_GRAPH_NODATA_ERROR @b state データ無し
@retval JBXL_GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
*/
template <typename T>  TGAImage  MSGraph2TGAImage(MSGraph<T> vp, bool rle)
{
    TGAImage tga;
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

    if (tga.state==0) setupTGAData(&tga, rle);

    return tga;
}


}       // namespace


#endif
