
/**
@brief    TGAグラフィックデータ ツール for C++ 
@file     TgaTool.cpp
@version  0.9
@date     2015 5/9
@author   Fumi.Iseki (C)
*/


#include "TgaTool.h"


using namespace jbxl;


/**
void  TGAImage::init(void) 

メンバ変数を初期化する．
*/
void  TGAImage::init(void) 
{ 
    xs      = 0;
    ys      = 0;
    col     = 0; 
    state   = 0;
    gp      = NULL;

    memset(hd, 0, TGA_HEADER_SIZE);
    return;
}


/**
bool  TGAImage::isNull(void) 

グラフィックデータを持いないか？
*/
bool  TGAImage::isNull(void) 
{   
    if (gp==NULL) return true; 
    return false;
}


/**
void  TGAImage::clear(void) 
*/
void  TGAImage::clear(void) 
{
    memset(gp, 0, xs*ys*col);
    return;
}


/**
void  TGAImage::fill(uByte v) 
*/
void  TGAImage::fill(uByte v) 
{
    int i;
    for (i=0; i<xs*ys*col; i++) gp[i] = v;

    return;
}


/**
void  TGAImage::free(void) 

グラフィックデータを開放する
*/
void  TGAImage::free(void) 
{  
    if (gp!=NULL) ::free(gp); 
    init();

    return;
}


/**
void   TGAImage::set(int x, int y, int c) 
*/
void   TGAImage::set(int x, int y, int c) 
{ 
    getm(x, y, c); 
    if (gp==NULL) return;

    xs  = x;
    ys  = y;
    col = c;
    state = 0;

    memset(gp, 0, xs*ys*col);

    return;
}


/**
void   TGAImage::getm(int x, int y, int c)
*/
void   TGAImage::getm(int x, int y, int c)
{
    gp = (uByte*)malloc(x*y*c);
    if (gp==NULL) {
        state = JBXL_GRAPH_MEMORY_ERROR;
        return;
    }
    
    return;
}



/////////////////////////////////////////////////////////////////////////////////////////

/**
TGAImage  jbxl::readTGAFile(const char* fname)

TGAファイルを読み込んで，TGAImage構造体へデータを格納する．

@param  fname  読み込むファイル名

@return TGAImage データ．gp==NULL の場合，@b state に情報が入る．
@retval JBXL_GRAPH_OPFILE_ERROR @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR @b state: 不正ファイル（TGAファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR @b state: メモリエラー
*/
TGAImage  jbxl::readTGAFile(const char* fname)
{
    TGAImage tga;
    FILE*  fp;

    fp = fopen(fname, "rb");
    if (fp==NULL) {
        tga.gp   = NULL;
        tga.state = JBXL_GRAPH_OPFILE_ERROR;
        return tga;
    }

    tga = readTGAData(fp);
    fclose(fp);

    return tga;
}


/**
TGAImage  jbxl::readTGAData(FILE* fp)

TGAファイルを読み込んで，TGAImage構造体へデータを格納する．

@param  fp  読み込むファイルの記述子

@return TGAImage データ．gp==NULL の場合，@b state に情報が入る．
@retval JBXL_GRAPH_OPFILE_ERROR @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR @b state: 不正ファイル（TGAファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR @b state: メモリエラー
*/
TGAImage  jbxl::readTGAData(FILE* fp)
{
    TGAImage tga;

    fseek(fp, 0, 0);
    tga.free();

    /**********************************************************************/
    PRINT_MESG("**********************************************\n");
    PRINT_MESG("ERROR: jbxl::readTGAData() is not implemeted!!\n");
    PRINT_MESG("**********************************************\n");
    tga.state = JBXL_GRAPH_IVDARG_ERROR;
    /**********************************************************************/

    return tga;
}


/**
int  jbxl::writeTGAFile(const char* fname, TGAImage tga)

tga の画像データを fnameに書き出す．

@param  fname  ファイル名
@param  tga    保存する TGAデータ

@retval 0                   正常終了
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（TGAファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  tga にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR  ファイル名が NULL, or サポート外のチャンネル数
*/
int  jbxl::writeTGAFile(const char* fname, TGAImage tga)
{
    FILE*  fp;
    int    ret;

    if (fname==NULL) return JBXL_GRAPH_IVDARG_ERROR;
    if (tga.col<=0 || tga.col>4) return JBXL_GRAPH_IVDARG_ERROR;
    if (tga.gp==NULL) return JBXL_GRAPH_NODATA_ERROR;

    fp = fopen(fname, "wb");
    if (fp==NULL) {
        return JBXL_GRAPH_OPFILE_ERROR;
    }

    ret = writeTGAData(fp, tga);
    fclose(fp); 

    return ret;
}


/**
int  jbxl::writeTGAData(FILE* fp, TGAImage tga)

tga の画像データを fpに書き出す．

@param  fp     ファイル記述子
@param  tga    保存する TGAデータ

@retval 0                   正常終了
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（TGAファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  tga にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR  サポート外のチャンネル数
*/
int  jbxl::writeTGAData(FILE* fp, TGAImage tga)
{
    if (fp==NULL) return JBXL_GRAPH_OPFILE_ERROR;
    if (tga.col<=0 || tga.col>4) return JBXL_GRAPH_IVDARG_ERROR;
    if (tga.gp==NULL) return JBXL_GRAPH_NODATA_ERROR;

    // Header  http://3dtech.jp/wiki/index.php?TGA画像フォーマット詳細
    memset(tga.hd, 0, TGA_HEADER_SIZE);
    if (tga.col==3 || tga.col==4) tga.hd[2] = 2;    // Full Color
    else                          tga.hd[2] = 3;    // Gray Scale

    unsigned short int* size = (unsigned short int*)&(tga.hd[12]);
    if (is_little_endian()) {
        size[0] = tga.xs;
        size[1] = tga.ys;
    }
    else {
        size[0] = htons(tga.xs);
        size[1] = htons(tga.ys);
    }

    tga.hd[16] = tga.col*8;
    tga.hd[17] = 0x08 | 0x20;       // 0x0x8: αチャンネル深度, 0x20: Y方向:Top->Down

    fwrite(tga.hd, TGA_HEADER_SIZE, 1, fp);

    // Data
    int len = tga.xs*tga.ys*tga.col;
    fwrite(tga.gp, len, 1, fp);

    return 0;
}


