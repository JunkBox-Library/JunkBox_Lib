
/**
@brief    JPEGグラフィックデータ ツール for C++ 
@file     JpegTool.cpp
@version  0.9
@date     2009 3/8
@author   Fumi.Iseki (C)

@attention
this software is based in part on the work of the Independent JPEG Group. http://www.ijg.org/
*/


#include "JpegTool.h"

#ifdef ENABLE_JPEGLIB


using namespace jbxl;



/**
void  JPEGImage::init(void) 

メンバ変数を初期化する．
*/
void  JPEGImage::init(void) 
{ 
    xs      = 0;
    ys      = 0;
    col     = 0; 
    state   = 0;
    gp      = NULL; 
    image   = NULL;
    return;
}


/**
bool  JPEGImage::isNull(void) 

グラフィックデータを持いないか？
*/
bool  JPEGImage::isNull(void) 
{   
    if (gp==NULL || image==NULL) return true; 
    return false;
}


/**
void  JPEGImage::clear(void) 
*/
void  JPEGImage::clear(void) 
{
    memset(gp, 0, sizeof(JSAMPLE)*xs*ys*col);
    return;
}


/**
void  JPEGImage::fill(JSAMPLE v) 
*/
void  JPEGImage::fill(JSAMPLE v) 
{
    int i;
    for (i=0; i<xs*ys*col; i++) gp[i] = v;

    return;
}


/**
void  JPEGImage::free(void) 

グラフィックデータを開放する
*/
void  JPEGImage::free(void) 
{  
    if (gp!=NULL)    ::free(gp); 
    if (image!=NULL) ::free(image); 
    init();

    return;
}


/**
void   JPEGImage::set(int x, int y, int c) 
*/
void   JPEGImage::set(int x, int y, int c) 
{ 
    if (c!=1) c = 3;

    getm(x, y, c); 
    if (gp==NULL) return;

    xs  = x;
    ys  = y;
    col = c;
    state = 0;

    memset(gp, 0, sizeof(JSAMPLE)*c*x*y);

    return;
}


/**
void   JPEGImage::getm(int x, int y, int c)
*/
void   JPEGImage::getm(int x, int y, int c)
{
    image = (JSAMPARRAY)malloc(sizeof(JSAMPROW)*y);
    if (image==NULL) {
        state = JBXL_GRAPH_MEMORY_ERROR;
        return;
    }

    gp  = (JSAMPLE*)malloc(sizeof(JSAMPLE)*c*x*y);
    if (gp==NULL) {
        freeNull(image);
        state = JBXL_GRAPH_MEMORY_ERROR;
        return;
    }
    
    int j;
    for (j=0; j<y; j++) image[j] = (JSAMPROW)&gp[j*c*x];

    return;
}



/////////////////////////////////////////////////////////////////////////////////////////

/**
JPEGImage  jbxl::readJPEGFile(const char* fname)

JPEGファイルを読み込んで，JPEGImage構造体へデータを格納する．

@param  fname  読み込むファイル名

@return JPEGImage データ．gp==NULL の場合，@b state に情報が入る．
@retval JBXL_GRAPH_OPFILE_ERROR @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR @b state: 不正ファイル（JPEGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR @b state: メモリエラー
*/
JPEGImage  jbxl::readJPEGFile(const char* fname)
{
    JPEGImage jp;
    FILE*  fp;

    fp = fopen(fname, "rb");
    if (fp==NULL) {
        jp.gp    = NULL;
        jp.state = JBXL_GRAPH_OPFILE_ERROR;
        return jp;
    }

    jp = readJPEGData(fp);
    fclose(fp);

    return jp;
}


/**
JPEGImage  jbxl::readJPEGData(FILE* fp)

JPEGファイルを読み込んで，JPEGImage構造体へデータを格納する．

@param  fp  読み込むファイルの記述子

@return JPEGImage データ．gp==NULL の場合，@b state に情報が入る．
@retval JBXL_GRAPH_OPFILE_ERROR @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR @b state: 不正ファイル（JPEGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR @b state: メモリエラー
*/
JPEGImage  jbxl::readJPEGData(FILE* fp)
{
    JPEGImage jp;
    int    xs, ys, col;
    struct jpeg_decompress_struct jdat;
    struct jpeg_error_mgr         jerr;

    fseek(fp, 0, 0);
    jp.init();
    jdat.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&jdat);

    // エラーハンドラー
/*  jdat.client_data = "Client Data";
    jerr.error_exit  = jpeg_error_exit;
    jerr.output_message = ptint_message;
*/
    jpeg_stdio_src(&jdat, fp);
    jpeg_read_header(&jdat, TRUE);
    jpeg_start_decompress(&jdat);

    xs  = jdat.output_width;
    ys  = jdat.output_height;
    col = jdat.output_components;
    if (xs<=0 || ys<=0 || col<=0) {
        jpeg_destroy_decompress(&jdat);
        fclose(fp);
        jp.state = JBXL_GRAPH_HEADER_ERROR;
        return jp;
    }

    jp.set(xs, ys, col);
    if (jp.state) return jp;

    int rmn = jdat.output_height;
    while(rmn>0) {
        jpeg_read_scanlines(&jdat, jp.image+jdat.output_scanline, rmn);
        rmn = jdat.output_height - jdat.output_scanline;
    }
    jpeg_finish_decompress (&jdat);
    jpeg_destroy_decompress(&jdat);

    return jp;
}


/**
int  jbxl::writeJPEGFile(const char* fname, JPEGImage jp, int qulty)

jp の画像データを fnameに書き出す．

@param  fname  ファイル名
@param  jp     保存する JPEGデータ
@param  qulty  保存のクオリティ 0〜100  100が最高画質

@retval 0                   正常終了
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（JPEGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  jp にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR  ファイル名が NULL, or サポート外のチャンネル数（現在の所チャンネル数は 1か 3のみをサポート）
*/
int  jbxl::writeJPEGFile(const char* fname, JPEGImage jp, int qulty)
{
    FILE*  fp;
    int    ret;

    if (fname==NULL) return JBXL_GRAPH_IVDARG_ERROR;
    if (jp.col!=1 && jp.col!=3) return JBXL_GRAPH_IVDARG_ERROR;
    if (jp.gp==NULL || jp.image==NULL) return JBXL_GRAPH_NODATA_ERROR;

    fp = fopen(fname, "wb");
    if (fp==NULL) {
        return JBXL_GRAPH_OPFILE_ERROR;
    }

    ret = writeJPEGData(fp, jp, qulty);
    fclose(fp); 

    return ret;
}



/**
int  jbxl::writeJPEGData(FILE* fp, JPEGImage jp, int qulty)

jp の画像データを fpに書き出す．

@param  fp     ファイル記述子
@param  jp     保存する JPEGデータ
@param  qulty  保存のクオリティ 0〜100  100が最高画質

@retval 0                   正常終了
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（JPEGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  jp にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR  サポート外のチャンネル数（現在の所チャンネル数は 1か 3のみをサポート）
*/
int  jbxl::writeJPEGData(FILE* fp, JPEGImage jp, int qulty)
{
    struct jpeg_compress_struct jdat;
    struct jpeg_error_mgr       jerr;

    if (fp==NULL) return JBXL_GRAPH_OPFILE_ERROR;
    if (jp.col!=1 && jp.col!=3) return JBXL_GRAPH_IVDARG_ERROR;
    if (jp.gp==NULL || jp.image==NULL) return JBXL_GRAPH_NODATA_ERROR;

    if (qulty>100)    qulty = 100;
    else if (qulty<0) qulty = 0;

    jdat.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&jdat);

    // エラーハンドラ
/*  jdat.client_data = "Client Data";
    jerr.error_exit  = jpeg_error_exit;
    jerr.output_message = ptint_message;
*/
    fseek(fp, 0, 0);
    jpeg_stdio_dest(&jdat, fp);

    jdat.image_width      = jp.xs;
    jdat.image_height     = jp.ys;
    jdat.input_components = jp.col;
    if (jp.col==1) jdat.in_color_space = JCS_GRAYSCALE;
    else           jdat.in_color_space = JCS_RGB;

    jpeg_set_quality (&jdat, qulty, TRUE);
    jpeg_set_defaults(&jdat);

    jpeg_start_compress (&jdat, TRUE);
    jpeg_write_scanlines(&jdat, jp.image, jp.ys);
    jpeg_finish_compress(&jdat);

    jpeg_destroy_compress(&jdat);

    return 0;
}



////////////////////////////////////////////////////////////////////////////////////////////

/**
CmnHead  jbxl::JPEGImage2CmnHead(JPEGImage jp)

Jpegイメージデータの画像を 共通ヘッダの画像に変換する．

@param  jp  Jpegイメージデータ 

@return 共通ヘッダ
@retval JBXL_GRAPH_NODATA_ERROR @b xsixe  データ無し
@retval JBXL_GRAPH_MEMORY_ERROR @b xsize メモリ確保エラー 
*/
CmnHead  jbxl::JPEGImage2CmnHead(JPEGImage jp)
{
    CmnHead hd;
    int i, j, k, yp, zp;

    memset(&hd, 0, sizeof(CmnHead));

    if (jp.isNull()) {
        hd.kind  = HEADER_NONE;
        hd.xsize = JBXL_GRAPH_NODATA_ERROR;
        return hd;
    }

    // color
    if (jp.col==1) {
        hd.kind = JPEG_MONO_DATA;
    }
    else if (jp.col==3) {
        hd.kind = JPEG_RGB_DATA;
    }
    else {
        hd.kind  = HEADER_NONE;
        hd.xsize = JBXL_GRAPH_IVDARG_ERROR;
        return hd;
    }

    hd.xsize = jp.xs;
    hd.ysize = jp.ys;
    hd.zsize = jp.col;
    hd.depth = 8;
    hd.bsize = 0;
    hd.lsize = jp.xs*jp.ys*jp.col*((hd.depth+7)/8);
    hd.buf   = NULL;
    hd.grptr = (uByte*)malloc(hd.lsize);
    if (hd.grptr==NULL) {
        hd.kind  = HEADER_NONE;
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }

    for (k=0; k<jp.col; k++) {
        zp = k*jp.xs*jp.ys;
        for (j=0; j<jp.ys; j++) {
            yp = zp + j*jp.xs;
            for (i=0; i<jp.xs; i++) {
                hd.grptr[yp + i] = (uByte)jp.point(i, j, k);
            }
        }
    }

    return hd;
}


/**
JPEGImage  jbxl::CmnHead2JPEGImage(CmnHead hd)

共通ヘッダの画像を Jpegイメージデータに変換する．

@param  hd  画像データが入った 共通ヘッダ

@return Jpegイメージデータ
@retval JBXL_GRAPH_NODATA_ERROR @b state データ無し
@retval JBXL_GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
@retval JBXL_GRAPH_IVDARG_ERROR @b state パラメータ異常
*/
JPEGImage  jbxl::CmnHead2JPEGImage(CmnHead hd)
{
    JPEGImage jp;
    int i, j, k, yp, zp, col;
    int kind = hd.kind & 0x00ff;

    jp.init();

    if (hd.grptr==NULL) {
        jp.state = JBXL_GRAPH_NODATA_ERROR;
        return jp;
    }
    if (hd.depth!=8 && hd.depth!=16) {  // 24bit, 32bit は未サポート
        jp.state = JBXL_GRAPH_IVDARG_ERROR;
        return jp;
    }

    // カラー
    if (kind==JPEG_MONO_DATA && hd.zsize==1) col = 1;
    else if (kind==JPEG_RGB_DATA && hd.zsize==3) col = 3;
    else if ((kind==JPEG_ARGB_DATA  || kind==JPEG_RGBA_DATA) && hd.zsize==4) col = 3;
    else if ((kind==JPEG16_RGB_DATA || kind==JPEG16_ARGB_DATA || kind==JPEG16_RGBA_DATA) && hd.zsize==1 && hd.depth==16) col = 3;
    else {
        jp.state = JBXL_GRAPH_IVDARG_ERROR;
        return jp;
    }

    jp.set(hd.xsize, hd.ysize, col);
    if (jp.isNull()) return jp;

    // JPEG16_RGB_DATA: R5G6B5 -> R8G8B8
    if (kind==JPEG16_RGB_DATA && hd.depth==16) {
        for (j=0; j<jp.ys; j++) {
            yp = j*jp.xs;
            for (i=0; i<jp.xs; i++) {
                uWord* ptr = (uWord*)&hd.grptr[(yp+i)*2];
                if (isLittleEndian) {
                    jp.point(i, j, 0) = (JSAMPLE)(((int)(((*ptr)&0xf800)>>11)*255)/32);
                    jp.point(i, j, 1) = (JSAMPLE)(((int)(((*ptr)&0x07e0)>>5) *255)/64);
                    jp.point(i, j, 2) = (JSAMPLE)(((int)(((*ptr)&0x001f))    *255)/32);
                }
                else {
                    jp.point(i, j, 0) = (JSAMPLE)(((int)(((*ptr)&0x001f))    *255)/32);
                    jp.point(i, j, 1) = (JSAMPLE)(((int)(((*ptr)&0x07e0)>>5) *255)/64);
                    jp.point(i, j, 2) = (JSAMPLE)(((int)(((*ptr)&0xf800)>>11)*255)/32);
                }
            }
        }
    }

    // JPEG16_ARGB_DATA: A4R4G4B4 -> R8G8B8
    else if (kind==JPEG16_ARGB_DATA && hd.depth==16) {
        for (j=0; j<jp.ys; j++) {
            yp = j*jp.xs;
            for (i=0; i<jp.xs; i++) {
                uWord* ptr = (uWord*)&hd.grptr[(yp+i)*2];
                if (isLittleEndian) {
                    jp.point(i, j, 0) = (JSAMPLE)(((int)(((*ptr)&0x0f00)>>8) *255)/16);
                    jp.point(i, j, 1) = (JSAMPLE)(((int)(((*ptr)&0x00f0)>>4) *255)/16);
                    jp.point(i, j, 2) = (JSAMPLE)(((int)(((*ptr)&0x000f))    *255)/16);
                }
                else {
                    jp.point(i, j, 0) = (JSAMPLE)(((int)(((*ptr)&0x00f0)>>4) *255)/16);
                    jp.point(i, j, 1) = (JSAMPLE)(((int)(((*ptr)&0x0f00)>>8) *255)/16);
                    jp.point(i, j, 2) = (JSAMPLE)(((int)(((*ptr)&0xf000)>>12)*255)/16);
                }
            }
        }
    }

    // JPEG16_ARGB_DATA: R4G4B4A4 -> R8G8B8
    else if (kind==JPEG16_RGBA_DATA && hd.depth==16) {
        for (j=0; j<jp.ys; j++) {
            yp = j*jp.xs;
            for (i=0; i<jp.xs; i++) {
                uWord* ptr = (uWord*)&hd.grptr[(yp+i)*2];
                if (isLittleEndian) {
                    jp.point(i, j, 0) = (JSAMPLE)(((int)(((*ptr)&0xf000)>>12)*255)/16);
                    jp.point(i, j, 1) = (JSAMPLE)(((int)(((*ptr)&0x0f00)>>8) *255)/16);
                    jp.point(i, j, 2) = (JSAMPLE)(((int)(((*ptr)&0x00f0)>>4) *255)/16);
                }
                else {
                    jp.point(i, j, 0) = (JSAMPLE)(((int)(((*ptr)&0x000f))    *255)/16);
                    jp.point(i, j, 1) = (JSAMPLE)(((int)(((*ptr)&0x00f0)>>4) *255)/16);
                    jp.point(i, j, 2) = (JSAMPLE)(((int)(((*ptr)&0x0f00)>>8) *255)/16);
                }
            }
        }
    }

    // JPEG_ARGB_DATA (アルファチャンネルの削除): A8R8G8B8 -> R8G8B8
    else if (kind==JPEG_ARGB_DATA) {
        if (hd.depth==8) {
            for (k=1; k<=3; k++) {
                zp = k*jp.xs*jp.ys;
                for (j=0; j<jp.ys; j++) {
                    yp = zp + j*jp.xs;
                    for (i=0; i<jp.xs; i++) {
                        jp.point(i, j, k-1) = (JSAMPLE)hd.grptr[yp+i];
                    }
                }
            }
        }

        // 16bit->8bit 変換
        else if (hd.depth==16) {
            int    max = 0;
            uWord* ptr = (uWord*)hd.grptr;
            for (i=hd.xsize*hd.ysize; i<hd.xsize*hd.ysize*hd.zsize; i++) {
                if (max<ptr[i]) max = ptr[i];
            }

            for (k=1; k<=3; k++) {
                zp = k*jp.xs*jp.ys;
                for (j=0; j<jp.ys; j++) {
                    yp = zp + j*jp.xs;
                    for (i=0; i<jp.xs; i++) {
                        if (max<=255) { 
                            if (isLittleEndian) jp.point(i, j, k-1) = (JSAMPLE)hd.grptr[(yp+i)*2];
                            else                jp.point(i, j, k-1) = (JSAMPLE)hd.grptr[(yp+i)*2 + 1];
                        }
                        else {
                            jp.point(i, j, k-1) = (JSAMPLE)((ptr[yp+i]*255)/max); 
                        }
                    }
                }
            }
        }
    }

    // Default: JPEG_RGB_DATA/JPEG_MONO_DATA/JPEG_RGBA_DATA
    else {
        if (hd.depth==8) {
            for (k=0; k<jp.col; k++) {
                zp = k*jp.xs*jp.ys;
                for (j=0; j<jp.ys; j++) {
                    yp = zp + j*jp.xs;
                    for (i=0; i<jp.xs; i++) {
                        jp.point(i, j, k) = (JSAMPLE)hd.grptr[yp+i];
                    }
                }
            }
        }

        // 16bit->8bit 変換
        else if (hd.depth==16) {
            int    max = 0;
            uWord* ptr = (uWord*)hd.grptr;
            for (i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) {
                if (max<ptr[i]) max = ptr[i];
            }

            for (k=0; k<jp.col; k++) {
                zp = k*jp.xs*jp.ys;
                for (j=0; j<jp.ys; j++) {
                    yp = zp + j*jp.xs;
                    for (i=0; i<jp.xs; i++) {
                        if (max<=255) { 
                            if (isLittleEndian) jp.point(i, j, k) = (JSAMPLE)hd.grptr[(yp+i)*2];
                            else                jp.point(i, j, k) = (JSAMPLE)hd.grptr[(yp+i)*2 + 1];
                        }
                        else {
                            jp.point(i, j, k) = (JSAMPLE)((ptr[yp+i]*255)/max); 
                        }
                    }
                }
            }
        }
    }
    
    return jp;
}



////////////////////////////////////////////////////////////////////////////////////////////

/**
int  jbxl::isJPEGHeader(Buffer buf);

ファイルヘッダの一部から Jpegデータかどうか判断する．@n
head は 10Byte 以上必要
*/
int  jbxl::isJPEGHeader(Buffer buf)
{
    if (buf.vldsz<10) return FALSE;

    unsigned char* head = buf.buf;

    if (head[0]==0xff && head[1]==0xd8) {
        if (head[2]==0xff) return TRUE;
        /*
        if (head[2]==0xff && head[3]==0xe0) {
            if (!strncmp((const char*)&head[6], "JFIF", 4)) {
                return TRUE;
            }
        }
        if (head[2]==0xff && head[3]==0xe1) {
            if (!strncmp((const char*)&head[6], "EXIF", 4)) {
                return TRUE;
            }
        }
        else if (head[2]==0xff && head[3]==0xdb) {
            return TRUE;
        }*/
    }

    return FALSE;
}


#endif      // ENABLE_JPEGLIB

