/**
@brief    PNG TOOL
@file     png_tool.c
@version  0.9
@date     2024 7/19
@author   Fumi.Iseki (C)

@attention
*/

#include "png_tool.h"
#include "jbxl_state.h"


#ifdef  ENABLE_PNG


/**
PNGImage  read_png_file(const char* fname)

PNGファイルを読み込んで，PNGImage構造体へデータを格納する．

@param  fname  読み込むファイル名

@return PNGImage データ．gp==NULL の場合，@b state に情報が入る．
@retval JBXL_GRAPH_OPFILE_ERROR @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR @b state: 不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR @b state: メモリエラー
*/
PNGImage  read_png_file(const char* fname)
{
    UNUSED(fname);
    PNGImage png;
    memset(&png, 0, sizeof(PNGImage));

    print_message("read_png_file: not implement yet!\n");
    return png;
}


/**
int  write_png_file(const char* fname, PNGImage png, int qulty)

png の画像データを fnameに書き出す．

@param  fname  ファイル名
@param  png    保存する PNGデータ
@param  qulty  保存のクオリティ 0-100  100が最高画質

@retval 0                   正常終了
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  png にデータが無い
@retval JBXL_GRAPH_IVDARH_ERROR  ファイル名が NULL, or サポート外のチャンネル数（現在の所チャンネル数は 1か 3のみをサポート）
*/
int  write_png_file(const char* fname, PNGImage png, int qulty)
{
    UNUSED(fname);
    UNUSED(png);
    UNUSED(qulty);

    print_message("write_png_file: not implement yet!\n");
    return 0;
}


/**
int  write_png_mem(unsigned char** buf, unsigned long* len, PNGImage png, int qulty)

png の画像データを *bufに書き出す．*bufは要 free

@param[out]  buf    画像データが格納される．要 free
@param[out]  len    buf の長さ（Byte）が格納される．
@param       png     保存する PNGデータ
@param       qulty  保存のクオリティ 0〜100  100が最高画質

@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  png にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR  buf が NULL, or サポート外のチャンネル数（現在の所チャンネル数は 1か 3のみをサポート）
*/
int  write_png_mem(unsigned char** buf, unsigned long* len, PNGImage png, int qulty)
{
    UNUSED(buf);
    UNUSED(len);
    UNUSED(png);
    UNUSED(qulty);

    return 0;
}


/**
WSGraph  PNGImage2WSGraph(PNGImage png)

PNGデータをチャンネル分解して，WSGraphに格納する．
*/
WSGraph  PNGImage2WSGraph(PNGImage png)
{
    UNUSED(png);
    WSGraph vp;
    memset(&vp, 0, sizeof(WSGraph));
    return vp;
}


/**
BSGraph  PNGImage2BSGraph(PNGImage png)

PNGデータをチャンネル分解して，BSGraphに格納する．
*/
BSGraph  PNGImage2BSGraph(PNGImage png)
{
    UNUSED(png);
    BSGraph vp;
    memset(&vp, 0, sizeof(BSGraph));
    return vp;
}


/**
PNGImage  WSGraph2PNGImage(WSGraph vp)
*/
PNGImage  WSGraph2PNGImage(WSGraph vp)
{
    UNUSED(vp);
    PNGImage png;
    memset(&png, 0, sizeof(PNGImage));
    return png;
}


/**
PNGImage  BSGraph2PNGImage(BSGraph vp)
*/
PNGImage  BSGraph2PNGImage(BSGraph vp)
{
    UNUSED(vp);
    PNGImage png;
    memset(&png, 0, sizeof(PNGImage));
    return png;
}


/**
PNGImage  make_PNGImage(int xs, int ys, int col)
*/
PNGImage  make_PNGImage(int xs, int ys, int col)
{
    UNUSED(xs);
    UNUSED(ys);
    UNUSED(col);
    PNGImage png;
    memset(&png, 0, sizeof(PNGImage));
    return png;
}


/**
void  free_PNGImage(PNGImage* png)
*/
void  free_PNGImage(PNGImage* png)
{
    if (png==NULL) return;
    return;
}



#endif  // DISABLE_PNG

