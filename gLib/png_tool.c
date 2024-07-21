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
@return PNGImage データ．state に情報が入る．

@retval JBXL_NORMAL               @b state: 正常終了
@retval JBXL_ERROR                @b state: 初期化エラー
@retval JBXL_GRAPH_OPFILE_ERROR   @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR   @b state: 不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_IVDCOLOR_ERROR @b state: サポート外のチャンネル（カラー）数
@retval JBXL_GRAPH_MEMORY_ERROR   @b state: メモリエラー
*/
PNGImage  read_png_file(const char* fname)
{
    PNGImage png;
    memset(&png, 0, sizeof(PNGImage));

    FILE* fp = fopen(fname, "rb");
    if (fp==NULL) {
        png.state = JBXL_GRAPH_OPFILE_ERROR;
        return png;
    }

    // ヘッダチェック
    unsigned char png_sig[PNG_SIGNATURE_SIZE];
    int sz = fread(png_sig, 1, PNG_SIGNATURE_SIZE, fp);
    if (sz!=PNG_SIGNATURE_SIZE || png_sig_cmp(png_sig, 0, PNG_SIGNATURE_SIZE)) {
        png.state = JBXL_GRAPH_HEADER_ERROR;
        return png;
    }

    png_structp strct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (strct==NULL) {
        png.state = JBXL_ERROR;
        return png;
    }
    png_infop info = png_create_info_struct(strct);
    if (info==NULL) {
        png_destroy_read_struct(&strct, NULL, NULL);
        strct = NULL;
        png.state = JBXL_ERROR;
        return png;
    }

    png_init_io(strct, fp);
    png_set_sig_bytes(strct, PNG_SIGNATURE_SIZE);
    png_read_png(strct, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, NULL);

    png.xs   = png_get_image_width(strct, info);
    png.ys   = png_get_image_height(strct, info);
    png.type = png_get_color_type(strct, info);

    if      (png.type == PNG_COLOR_TYPE_GRAY) png.col = 1;
    else if (png.type == PNG_COLOR_TYPE_GA)   png.col = 2;
    else if (png.type == PNG_COLOR_TYPE_RGB)  png.col = 3;
    else if (png.type == PNG_COLOR_TYPE_RGBA) png.col = 4;
    else {
        png_destroy_read_struct(&strct, &info, NULL);
        png.state = JBXL_GRAPH_IVDCOLOR_ERROR;
        return png;
    }
    int length = png.xs*png.ys*png.col;

    png.gp = (uByte*)malloc(length);
    if (png.gp==NULL) {
        png_destroy_read_struct(&strct, &info, NULL);
        png.state = JBXL_GRAPH_MEMORY_ERROR;
        return png;
    }

    uByte** datap = png_get_rows(strct, info);
    int len = png.xs*png.col;
    for (int j=0; j<png.ys; j++) {
        memcpy(png.gp + j*len, datap[j], len);
    }

    png_destroy_read_struct(&strct, &info, NULL);
    png.state = JBXL_NORMAL;

    return png;
}


/**
int  write_png_file(const char* fname, PNGImage png)

png の画像データを fnameに書き出す．

@param  fname  ファイル名
@param  png    保存する PNGデータ

@retval 0                          正常終了
@retval JBXL_GRAPH_OPFILE_ERROR    ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR    不正ファイル（PNGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR    メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR    png にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR    ファイル名が NULL
@retval JBXL_GRAPH_IVDCOLOR_ERROR  サポート外のチャンネル（カラー）数
*/
int  write_png_file(const char* fname, PNGImage png)
{
    if (fname==NULL) return JBXL_GRAPH_IVDARG_ERROR;

    FILE* fp = fopen(fname, "wb");
    if (fp==NULL) return JBXL_GRAPH_OPFILE_ERROR;
    if (png.state!=JBXL_NORMAL || png.gp==NULL) return JBXL_GRAPH_NODATA_ERROR;
    //
    png_structp strct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (strct==NULL) return JBXL_ERROR;

    png_infop info = png_create_info_struct(strct);
    if (info==NULL) {
        png_destroy_write_struct(&strct, NULL);
        return JBXL_ERROR;
    }

    if      (png.col==1) png.type = PNG_COLOR_TYPE_GRAY;
    else if (png.col==2) png.type = PNG_COLOR_TYPE_GA;
    else if (png.col==3) png.type = PNG_COLOR_TYPE_RGB;
    else if (png.col==4) png.type = PNG_COLOR_TYPE_RGBA;
    else {
        png_destroy_write_struct(&strct, &info);
        return JBXL_GRAPH_IVDCOLOR_ERROR;
    }

    png_init_io(strct, fp);
    png_set_IHDR(strct, info, png.xs, png.ys, 8, png.type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    uByte** datap = (uByte**)png_malloc(strct, sizeof(uByte*) * png.ys);
    if (datap==NULL) {
        png_destroy_write_struct(&strct, &info);
        return JBXL_GRAPH_MEMORY_ERROR;
    }
    png_set_rows(strct, info, datap);

    int len = png.xs*png.col;
    for (int j=0; j<png.ys; j++) {
        datap[j] = (uByte*)malloc(len);
        if (datap[j]==NULL) {
            for (int i=0; i<j; i++) png_free(strct, datap[i]);
            png_free(strct, datap);
            png_destroy_write_struct(&strct, &info);
            return JBXL_GRAPH_MEMORY_ERROR;
        }
        memcpy(datap[j], png.gp + j*len, len);
    }
    //
    png_write_png(strct, info, PNG_TRANSFORM_IDENTITY, NULL);

    for (int j=0; j<png.ys; j++) png_free(strct, datap[j]);
    png_free(strct, datap);
    png_destroy_write_struct(&strct, &info);
    return 0;
}


/**
WSGraph  PNGImage2WSGraph(PNGImage png)

PNGデータをチャンネル分解して，WSGraphに格納する．
*/
WSGraph  PNGImage2WSGraph(PNGImage png)
{
    WSGraph vp = make_WSGraph(png.xs, png.ys, png.col);
    if (vp.state!=JBXL_NORMAL || vp.gp==NULL) return vp;

    int i, j, k;
    for (k=0; k<png.col; k++) {
        int kp = k*png.xs*png.ys;
        for (j=0; j<png.ys;  j++) {
            int jp = j*png.xs;
            int ip = jp + kp;
            int cp = jp*png.col + k;
            for (i=0; i<png.xs;  i++) {
                //vp.gp[k*png.xs*png.ys + j*png.xs + i] = png.gp[png.col(j*png.xs + i) + k];
                vp.gp[ip + i] = (sWord)png.gp[cp + png.col*i];
            }
        }
    }
    return vp;
}


/**
BSGraph  PNGImage2BSGraph(PNGImage png)

PNGデータをチャンネル分解して，BSGraphに格納する．
*/
BSGraph  PNGImage2BSGraph(PNGImage png)
{
    BSGraph vp = make_BSGraph(png.xs, png.ys, png.col);
    if (vp.state!=JBXL_NORMAL || vp.gp==NULL) return vp;

    int i, j, k;
    for (k=0; k<png.col; k++) {
        int kp = k*png.xs*png.ys;
        for (j=0; j<png.ys;  j++) {
            int jp = j*png.xs;
            int ip = jp + kp;
            int cp = jp*png.col + k;
            for (i=0; i<png.xs;  i++) {
                vp.gp[ip + i] = (uByte)png.gp[cp + png.col*i];
            }
        }
    }
    return vp;
}


/**
PNGImage  WSGraph2PNGImage(WSGraph vp)
*/
PNGImage  WSGraph2PNGImage(WSGraph vp)
{
    PNGImage png;
    memset(&png, 0, sizeof(PNGImage));

    png.xs  = vp.xs;
    png.ys  = vp.ys;
    png.col = vp.zs;
    int length = png.xs*png.ys*png.col;
    
    if      (vp.zs==1) png.type = PNG_COLOR_TYPE_GRAY;
    else if (vp.zs==2) png.type = PNG_COLOR_TYPE_GA;
    else if (vp.zs==3) png.type = PNG_COLOR_TYPE_RGB;
    else if (vp.zs==4) png.type = PNG_COLOR_TYPE_RGBA;
    else {
        memset(&png, 0, sizeof(PNGImage));
        png.state = JBXL_GRAPH_IVDCOLOR_ERROR;
        return png;
    }
    png.gp = (uByte*)malloc(sizeof(uByte)*length);
    if (png.gp==NULL) {
        memset(&png, 0, sizeof(PNGImage));
        png.state = JBXL_GRAPH_MEMORY_ERROR;
        return png;
    }
        
    int i, j, k;
    for (k=0; k<png.col; k++) {
        int kp = k*png.xs*png.ys;
        for (j=0; j<png.ys;  j++) {
            int jp = j*png.xs;
            int ip = jp + kp;
            int cp = jp*png.col + k;
            for (i=0; i<png.xs;  i++) {
                png.gp[cp + png.col*i] = (uByte)vp.gp[ip + i];
            }
        }
    }
    png.state = JBXL_NORMAL;
    return png;
}


/**
PNGImage  BSGraph2PNGImage(BSGraph vp)
*/
PNGImage  BSGraph2PNGImage(BSGraph vp)
{
    PNGImage png;
    memset(&png, 0, sizeof(PNGImage));

    png.xs  = vp.xs;
    png.ys  = vp.ys;
    png.col = vp.zs;
    int length = png.xs*png.ys*png.col;
    
    if      (vp.zs==1) png.type = PNG_COLOR_TYPE_GRAY;
    else if (vp.zs==2) png.type = PNG_COLOR_TYPE_GA;
    else if (vp.zs==3) png.type = PNG_COLOR_TYPE_RGB;
    else if (vp.zs==4) png.type = PNG_COLOR_TYPE_RGBA;
    else {
        memset(&png, 0, sizeof(PNGImage));
        png.state = JBXL_GRAPH_IVDCOLOR_ERROR;
        return png;
    }

    png.gp = (uByte*)malloc(sizeof(uByte)*length);
    if (png.gp==NULL) {
        memset(&png, 0, sizeof(PNGImage));
        png.state = JBXL_GRAPH_MEMORY_ERROR;
        return png;
    }
        
    int i, j, k;
    for (k=0; k<png.col; k++) {
        int kp = k*png.xs*png.ys;
        for (j=0; j<png.ys;  j++) {
            int jp = j*png.xs;
            int ip = jp + kp;
            int cp = jp*png.col + k;
            for (i=0; i<png.xs;  i++) {
                png.gp[cp + png.col*i] = vp.gp[ip + i];
            }
        }
    }
    png.state = JBXL_NORMAL;
    return png;
}


/**
void  free_PNGImage(PNGImage* png)
*/
void  free_PNGImage(PNGImage* png)
{
    if (png==NULL) return;
    if (png->gp!=NULL) free(png->gp);
    memset(png, 0, sizeof(PNGImage));

    return;
}



#endif  // DISABLE_PNG

