/**
@brief    PNGグラフィックデータ ツール for C++ 
@file     JpegTool.cpp
@version  0.9
@date     2024 07/21
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
    xs = ys = col = length = 0;
    state = 0;
    type  = 0;
    //gp   = NULL;

    strct = NULL;
    info  = NULL;
    
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
    if (gp!=NULL) memset(gp, 0, length);
    return;
}


/**
void  PNGImage::fill(uByte v) 
*/
void  PNGImage::fill(uByte v) 
{
    if (gp!=NULL) memset(gp, v, length);
    return;
}


/**
void  PNGImage::free(void) 

グラフィックデータを開放する
*/
void  PNGImage::free(void) 
{  
    if (gp!=NULL) ::free(gp);
    gp = NULL;
    return;
}


/**
void   PNGImage::getm(int x, int y, int c)
*/
void   PNGImage::getm(int x, int y, int c)
{
    xs  = x;
    ys  = y;
    col = c;
    length = xs*ys*col;
    //
    gp = (uByte*)malloc(length);
    if (gp==NULL) {
        length = 0;
        xs = ys = col = 0;
        state = JBXL_GRAPH_MEMORY_ERROR;
        return;
    }

    memset(gp, 0, length);
    
    return;
}


/**
void  PNGImage::readData(FILE* fp)

PNGファイルを読み込んで，PNGImage Class へデータを格納する．

@param  fp  読み込むファイルの記述子

@return なし．終了時に @b state に情報が入る．

state: JBXL_NORMAL                正常終了 @n
state: JBXL_ERROR                 初期化エラー @n
state: JBXL_GRAPH_OPFILE_ERROR    ファイルディスクリプタ― が NULL @n
state: JBXL_GRAPH_HEADER_ERROR    不正ファイル（PNGファイルでない？）@n
state: JBXL_GRAPH_MEMORY_ERROR    メモリエラー @n
state: JBXL_GRAPH_IVDCOLOR_ERROR  サポート外のチャンネル（カラー）数 @n
*/
void  PNGImage::readData(FILE* fp)
{
    init();
    if (fp==NULL) {
        state = JBXL_GRAPH_OPFILE_ERROR;
        return;
    }

    // ヘッダチェック
    unsigned char png_sig[PNG_SIGNATURE_SIZE];
    unsigned int sz = (unsigned int)fread(png_sig, 1, PNG_SIGNATURE_SIZE, fp);
    if (sz!=PNG_SIGNATURE_SIZE || png_sig_cmp(png_sig, 0, PNG_SIGNATURE_SIZE)) {
        state = JBXL_GRAPH_HEADER_ERROR;
        return;
    }

    strct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (strct==NULL) {
        state = JBXL_ERROR;
        return;
    }
    info = png_create_info_struct(strct);
    if (info==NULL) {
        png_destroy_read_struct(&strct, NULL, NULL);
        strct = NULL;
        state = JBXL_ERROR;
        return;
    }

    png_init_io(strct, fp);
    png_set_sig_bytes(strct, PNG_SIGNATURE_SIZE);
    png_read_png(strct, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, NULL);

    xs   = png_get_image_width(strct, info);
    ys   = png_get_image_height(strct, info);
    type = png_get_color_type(strct, info);

    if      (type == PNG_COLOR_TYPE_GRAY) col = 1;
    else if (type == PNG_COLOR_TYPE_GA)   col = 2;
    else if (type == PNG_COLOR_TYPE_RGB)  col = 3;
    else if (type == PNG_COLOR_TYPE_RGBA) col = 4;
    else {
        png_destroy_read_struct(&strct, &info, NULL);
        strct = NULL;
        info  = NULL;
        state = JBXL_GRAPH_IVDCOLOR_ERROR;
        return;
    }
    length = xs*ys*col;

    gp = (uByte*)malloc(length);
    if (gp==NULL) {
        png_destroy_read_struct(&strct, &info, NULL);
        strct = NULL;
        info  = NULL;
        state = JBXL_GRAPH_MEMORY_ERROR;
        return;
    }        

    uByte** datap = png_get_rows(strct, info);
    int len = xs*col;
    for (int j=0; j<ys; j++) {
        memcpy(gp + j*len, datap[j], len);
    }

    png_destroy_read_struct(&strct, &info, NULL);
    strct = NULL;
    info  = NULL;
    state = JBXL_NORMAL;

    return;
}


/**
int  PNGImage::writeData(FILE* fp)

png の画像データを fpに書き出す．

@param  fp     ファイル記述子

@retval JBXL_NORMAL                正常終了
@retval JBXL_ERROR                 初期化エラー
@retval JBXL_GRAPH_OPFILE_ERROR    ファイルディスクリプタ― が NULL
@retval JBXL_GRAPH_MEMORY_ERROR    メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR    png にデータが無い
@retval JBXL_GRAPH_IVDCOLOR_ERROR  サポート外のチャンネル（カラー）数
*/
int  PNGImage::writeData(FILE* fp)
{
    if (fp==NULL) {
        state = JBXL_GRAPH_OPFILE_ERROR;
        return state;
    }
    if (state!=JBXL_NORMAL || gp==NULL) {
        state = JBXL_GRAPH_NODATA_ERROR;
        return state;
    }
    //
    strct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (strct==NULL) {
        state = JBXL_ERROR;
        return state;
    }
    info = png_create_info_struct(strct);
    if (info==NULL) {
        png_destroy_write_struct(&strct, NULL);
        strct = NULL;
        state = JBXL_ERROR;
        return state;
    }

    if      (col==1) type = PNG_COLOR_TYPE_GRAY;
    else if (col==2) type = PNG_COLOR_TYPE_GA;
    else if (col==3) type = PNG_COLOR_TYPE_RGB;
    else if (col==4) type = PNG_COLOR_TYPE_RGBA;
    else {
        png_destroy_write_struct(&strct, &info);
        strct = NULL;
        info  = NULL;
        state = JBXL_GRAPH_IVDCOLOR_ERROR;
        return state;
    }

    png_init_io(strct, fp);
    png_set_IHDR(strct, info, xs, ys, 8, type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    uByte** datap = (uByte**)png_malloc(strct, sizeof(uByte*) * ys);
    if (datap==NULL) {
        png_destroy_write_struct(&strct, &info);
        strct = NULL;
        info  = NULL;
        state = JBXL_GRAPH_MEMORY_ERROR;
        return state;
    }
    png_set_rows(strct, info, datap);

    int len = xs*col;
    for (int j=0; j<ys; j++) {
        datap[j] = (uByte*)malloc(len);
        if (datap[j]==NULL) {
            for (int i=0; i<j; i++) png_free(strct, datap[i]);
            png_free(strct, datap);
            png_destroy_write_struct(&strct, &info);
            strct = NULL;
            info  = NULL;
            state = JBXL_GRAPH_MEMORY_ERROR;
            return state;
        }
        memcpy(datap[j], gp + j*len, len);
    }
    //
    png_write_png(strct, info, PNG_TRANSFORM_IDENTITY, NULL);

    for (int j=0; j<ys; j++) png_free(strct, datap[j]);
    png_free(strct, datap);
    png_destroy_write_struct(&strct, &info);
    strct = NULL;
    info  = NULL;
    state = JBXL_NORMAL;
    return state;
}



/////////////////////////////////////////////////////////////////////////////////////////

/**
PNGImage  jbxl::readPNGFile(const char* fname)

PNGファイルを読み込んで，PNGImage Class へデータを格納する．

@param  fname  読み込むファイル名

@return PNGImage データ．state に情報が入る．

@retval JBXL_NORMAL                @b state: 正常終了
@retval JBXL_ERROR                 @b state: 初期化エラー
@retval JBXL_GRAPH_OPFILE_ERROR    @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR    @b state: 不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_IVDARG_ERROR    @b state: ファイル名が NULL
@retval JBXL_GRAPH_IVDCOLOR_ERROR  @b state: サポート外のチャンネル（カラー）数
@retval JBXL_GRAPH_MEMORY_ERROR    @b state: メモリエラー
*/
PNGImage  jbxl::readPNGFile(const char* fname)
{
    PNGImage png;

    if (fname==NULL) {
        png.state = JBXL_GRAPH_IVDARG_ERROR;
        return png;
    }
    //
    FILE* fp = fopen(fname, "rb");
    if (fp==NULL) {
        png.state = JBXL_GRAPH_OPFILE_ERROR;
        return png;
    }
    //
    png = readPNGData(fp);
    fclose(fp);

    return png;
}


/**
PNGImage  jbxl::readPNGData(FILE* fp)

PNGファイルを読み込んで，PNGImage Class へデータを格納する．

@param  fp  読み込むファイルの記述子

@return PNGImage データ．state に情報が入る．

@retval JBXL_NORMAL                @b state: 正常終了
@retval JBXL_ERROR                 @b state: 初期化エラー
@retval JBXL_GRAPH_OPFILE_ERROR    @b state: ファイルディスクリプタ― が NULL
@retval JBXL_GRAPH_HEADER_ERROR    @b state: 不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_IVDCOLOR_ERROR  @b state: サポート外のチャンネル（カラー）数
@retval JBXL_GRAPH_MEMORY_ERROR    @b state: メモリエラー
*/
PNGImage  jbxl::readPNGData(FILE* fp)
{
    PNGImage png;

    if (fp==NULL) {
        png.state = JBXL_GRAPH_OPFILE_ERROR;
        return png;
    }
    //
    png.readData(fp);

    return png;
}


/**
int  jbxl::writePNGFile(const char* fname, PNGImage* png)

png の画像データを fnameに書き出す．

@param  fname  ファイル名
@param  png    保存する PNGデータへのポインタ

@retval 0                          正常終了
@retval JBXL_ERROR                 初期化エラー
@retval JBXL_GRAPH_OPFILE_ERROR    ファイルオープンエラー
@retval JBXL_GRAPH_MEMORY_ERROR    メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR    png にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR    ファイル名が NULL
@retval JBXL_GRAPH_IVDCOLOR_ERROR  サポート外のチャンネル（カラー）数
*/
int  jbxl::writePNGFile(const char* fname, PNGImage* png)
{
    if (fname==NULL) return JBXL_GRAPH_IVDARG_ERROR;
    if (png->state!=JBXL_NORMAL || png->gp==NULL) return JBXL_GRAPH_NODATA_ERROR;

    FILE* fp = fopen(fname, "wb");
    if (fp==NULL) {
        return JBXL_GRAPH_OPFILE_ERROR;
    }
    int ret = writePNGData(fp, png);
    fclose(fp); 

    return ret;
}


/**
int  jbxl::writePNGData(FILE* fp, PNGImage* png)

png の画像データを fpに書き出す．

@param  fp     ファイル記述子
@param  png    保存する PNGデータへのポインタ

@retval 0                          正常終了
@retval JBXL_ERROR                 初期化エラー
@retval JBXL_GRAPH_OPFILE_ERROR    ファイルディスクリプタ が NULL
@retval JBXL_GRAPH_MEMORY_ERROR    メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR    png にデータが無い
@retval JBXL_GRAPH_IVDCOLOR_ERROR  サポート外のチャンネル（カラー）数
*/
int  jbxl::writePNGData(FILE* fp, PNGImage* png)
{
    if (fp==NULL) return JBXL_GRAPH_OPFILE_ERROR;
    if (png->state!=JBXL_NORMAL || png->gp==NULL) return JBXL_GRAPH_NODATA_ERROR;
    //
    int ret = png->writeData(fp);
    //
    if (ret==JBXL_NORMAL) ret = 0;
    else if (ret==0) ret = JBXL_ERROR;            // 情報なし？
    return ret;
}


#endif      // ENABLE_PNG

