/**
@brief    PNGグラフィックデータ ツール for C++ 
@file     JpegTool.cpp
@version  0.9
@date     2009 3/8
@author   Fumi.Iseki (C)

@attention
*/

#include "PngTool.h"

#ifdef ENABLE_PNG


using namespace jbxl;



/**
void  PNGImage::init(void) 

メンバ変数を初期化する．
*/
void  PNGImage::init(void) 
{ 
    return;
}


/**
bool  PNGImage::isNull(void) 

グラフィックデータを持いないか？
*/
bool  PNGImage::isNull(void) 
{   
    if (gp==NULL) return true; 
    return false;
}


/**
void  PNGImage::clear(void) 
*/
void  PNGImage::clear(void) 
{
    return;
}


/**
void  PNGImage::fill(uByte v) 
*/
void  PNGImage::fill(uByte v) 
{
    UNUSED(v);

    return;
}


/**
void  PNGImage::free(void) 

グラフィックデータを開放する
*/
void  PNGImage::free(void) 
{  
    return;
}


/**
void   PNGImage::setzero(int x, int y, int c) 
*/
void   PNGImage::setzero(int x, int y, int c) 
{ 
    UNUSED(x, y, c);
    return;
}


/**
void   PNGImage::getm(int x, int y, int c)
*/
void   PNGImage::getm(int x, int y, int c)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(c);
    
    return;
}



/////////////////////////////////////////////////////////////////////////////////////////

/**
PNGImage  jbxl::readPNGFile(const char* fname)

PNGファイルを読み込んで，PNGImage構造体へデータを格納する．

@param  fname  読み込むファイル名

@return PNGImage データ．gp==NULL の場合，@b state に情報が入る．
@retval JBXL_GRAPH_OPFILE_ERROR @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR @b state: 不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR @b state: メモリエラー
*/
PNGImage  jbxl::readPNGFile(const char* fname)
{
    PNGImage png;
    FILE*  fp;

    fp = fopen(fname, "rb");
    if (fp==NULL) {
        return png;
    }
    fclose(fp);

    return png;
}


/**
PNGImage  jbxl::readPNGData(FILE* fp)

PNGファイルを読み込んで，PNGImage構造体へデータを格納する．
読み込んだデータは非圧縮状態．

@param  fp  読み込むファイルの記述子

@return PNGImage データ．gp==NULL の場合，@b state に情報が入る．
@retval JBXL_GRAPH_OPFILE_ERROR @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR @b state: 不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR @b state: メモリエラー
@retval JBXL_GRAPH_IVDFMT_ERROR @b state: サポート外のデータ形式
*/
PNGImage  jbxl::readPNGData(FILE* fp)
{
    UNUSED(fp);
    PNGImage png;
    memset(&png, 0, sizeof(PNGImage));
    return png;
}


/**
int  jbxl::writePNGFile(const char* fname, PNGImage png)

png の画像データを fnameに書き出す．

@param  fname  ファイル名
@param  png    保存する PNGデータ

@retval 0                        正常終了
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  png にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR  ファイル名が NULL, or サポート外のチャンネル数
*/
int  jbxl::writePNGFile(const char* fname, PNGImage png)
{
    UNUSED(png);
    FILE*  fp;
    int    ret;

    if (fname==NULL) return JBXL_GRAPH_IVDARG_ERROR;
    //if (png.col<=0 || png.col>4) return JBXL_GRAPH_IVDARG_ERROR;
    //if (png.gp==NULL) return JBXL_GRAPH_NODATA_ERROR;

    fp = fopen(fname, "wb");
    if (fp==NULL) {
        return JBXL_GRAPH_OPFILE_ERROR;
    }

    fclose(fp); 
    ret = 0;

    return ret;
}


/**
int  jbxl::writePNGData(FILE* fp, PNGImage png, bool rle)

png の画像データを fpに書き出す．

@param  fp     ファイル記述子
@param  png    保存する PNGデータ

@retval 0                        正常終了
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  png にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR  サポート外のチャンネル数
@retval JBXL_GRAPH_IVDFMT_ERROR  サポート外のデータ形式
*/
int  jbxl::writePNGData(FILE* fp, PNGImage png)
{
    UNUSED(png);
    if (fp==NULL) return JBXL_GRAPH_OPFILE_ERROR;
    //if (png.col<=0 || png.col>4) return JBXL_GRAPH_IVDARG_ERROR;
    //if (png.gp==NULL) return JBXL_GRAPH_NODATA_ERROR;

    //fwrite(png.hd, PNG_HEADER_SIZE, 1, fp);
    //fwrite(png.gp, png.length, 1, fp);
    //fwrite(png.ft, PNG_FOOTER_SIZE, 1, fp);
    return 0;
}


/**
int  jbxl::setupPNGData(PNGImage* png, bool rle)

PNGのヘッダを設定し直して，必要なら RLEを行う．

@param  png    PNGデータへのポインタ
@param  rle    RLE（連長圧縮）を行うかどうか

@retval 0      正常終了
@retval <0     エラーコード
*/
int  jbxl::setupPNGData(PNGImage* png, bool rle)
{
    UNUSED(png);
    UNUSED(rle);
    return 0;
}


#endif      // ENABLE_PNG
