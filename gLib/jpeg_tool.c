
/**
@brief    JPEG TOOL
@file     jpeg_tool.c
@version  0.9
@date     2009 3/8
@author   Fumi.Iseki (C)

@attention
this software is based in part on the work of the Independent JPEG Group. http://www.ijg.org/
*/


#include "jpeg_tool.h"
#include "jbxl_state.h"


#ifdef  ENABLE_JPEGLIB


/**
JPEGImage  read_jpeg_file(const char* fname)

JPEGファイルを読み込んで，JPEGImage構造体へデータを格納する．

@param  fname  読み込むファイル名

@return JPEGImage データ．gp==NULL の場合，@b state に情報が入る．
@retval JBXL_GRAPH_OPFILE_ERROR @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR @b state: 不正ファイル（JPEGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR @b state: メモリエラー
*/
JPEGImage  read_jpeg_file(const char* fname)
{
    JPEGImage jp;
    int    xs, ys, col;
    FILE*  fp;

    struct jpeg_decompress_struct jdat;
    struct jpeg_error_mgr         jerr;

    memset(&jp, 0, sizeof(JPEGImage));

    jdat.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&jdat);

    // エラーハンドラ
/*  jdat.client_data = "Client Data";
    jerr.error_exit  = jpeg_error_exit;
    //jerr.output_message = print_message;
*/
    fp = fopen(fname, "rb");
    if (fp==NULL) {
        jp.state = JBXL_GRAPH_OPFILE_ERROR;
        return jp;
    }
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

    jp = make_JPEGImage(xs, ys, col);
    if (jp.gp==NULL) {
        jpeg_destroy_decompress(&jdat);
        fclose(fp);
        jp.state = JBXL_GRAPH_MEMORY_ERROR;
        return jp;
    }

    int rmn = jdat.output_height;
    while(rmn>0) {
        jpeg_read_scanlines(&jdat, jp.img+jdat.output_scanline, rmn);
        rmn = jdat.output_height - jdat.output_scanline;
    }
    jpeg_finish_decompress (&jdat);
    jpeg_destroy_decompress(&jdat);

    fclose(fp);

    return jp;
}



/**
int  write_jpeg_file(const char* fname, JPEGImage jp, int qulty)

jp の画像データを fnameに書き出す．

@param  fname  ファイル名
@param  jp     保存する JPEGデータ
@param  qulty  保存のクオリティ 0〜100  100が最高画質

@retval 0                   正常終了
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（JPEGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  jp にデータが無い
@retval JBXL_GRAPH_IVDARH_ERROR  ファイル名が NULL, or サポート外のチャンネル数（現在の所チャンネル数は 1か 3のみをサポート）
*/
int  write_jpeg_file(const char* fname, JPEGImage jp, int qulty)
{
    FILE*  fp;
    struct jpeg_compress_struct jdat;
    struct jpeg_error_mgr       jerr;


    if (fname==NULL) return JBXL_GRAPH_IVDARG_ERROR;
    if (jp.col!=1 && jp.col!=3) return JBXL_GRAPH_IVDARG_ERROR;
    if (jp.gp==NULL || jp.img==NULL) return JBXL_GRAPH_NODATA_ERROR;

    if (qulty>100)  qulty = 100;
    else if (qulty<0) qulty = 0;
    

    fp = fopen(fname, "wb");
    if (fp==NULL) {
        return JBXL_GRAPH_OPFILE_ERROR;
    }

    jdat.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&jdat);

    // エラーハンドラ
/*  jdat.client_data = "Client Data";
    jerr.error_exit  = jpeg_error_exit;
    //jerr.output_message = print_message;
*/
    jpeg_stdio_dest(&jdat, fp);

    jdat.image_width      = jp.xs;
    jdat.image_height     = jp.ys;
    jdat.input_components = jp.col;
    if (jp.col==1) jdat.in_color_space = JCS_GRAYSCALE;
    else           jdat.in_color_space = JCS_RGB;

    jpeg_set_quality (&jdat, qulty, TRUE);
    jpeg_set_defaults(&jdat);

    jpeg_start_compress (&jdat, TRUE);
    jpeg_write_scanlines(&jdat, jp.img, jp.ys);
    jpeg_finish_compress(&jdat);

    jpeg_destroy_compress(&jdat);
    fclose(fp); 

    return 0;
}



/**
int  write_jpeg_mem(unsigned char** buf, unsigned long* len, JPEGImage jp, int qulty)

jp の画像データを *bufに書き出す．*bufは要 free

@param[out]  buf    画像データが格納される．要 free
@param[out]  len    buf の長さ（Byte）が格納される．
@param       jp     保存する JPEGデータ
@param       qulty  保存のクオリティ 0〜100  100が最高画質

@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR  不正ファイル（JPEGファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR  jp にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR  buf が NULL, or サポート外のチャンネル数（現在の所チャンネル数は 1か 3のみをサポート）
*/
int  write_jpeg_mem(unsigned char** buf, unsigned long* len, JPEGImage jp, int qulty)
{
    struct jpeg_compress_struct jdat;
    struct jpeg_error_mgr       jerr;

    if (buf==NULL || len==NULL) return JBXL_GRAPH_IVDARG_ERROR;
    if (jp.col!=1 && jp.col!=3) return JBXL_GRAPH_IVDARG_ERROR;
    if (jp.gp==NULL || jp.img==NULL) return JBXL_GRAPH_NODATA_ERROR;

    *len = jp.xs*jp.ys*jp.col;
    if (*len<=0) return JBXL_GRAPH_IVDARG_ERROR;

    if (qulty>100)  qulty = 100;
    else if (qulty<0) qulty = 0;

    *buf = (unsigned char*)malloc(*len);
    if (*buf==NULL) {
        return JBXL_GRAPH_MEMORY_ERROR;
    }

    jdat.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&jdat);

    jpeg_mem_dest(&jdat, buf, len);

    jdat.image_width      = jp.xs;
    jdat.image_height     = jp.ys;
    jdat.input_components = jp.col;
    if (jp.col==1) jdat.in_color_space = JCS_GRAYSCALE;
    else           jdat.in_color_space = JCS_RGB;

    jpeg_set_quality (&jdat, qulty, TRUE);
    jpeg_set_defaults(&jdat);

    jpeg_start_compress (&jdat, TRUE);
    jpeg_write_scanlines(&jdat, jp.img, jp.ys);
    jpeg_finish_compress(&jdat);
    jpeg_destroy_compress(&jdat);

    if (*len<=0) {
        freeNull(*buf);
        return JBXL_GRAPH_ERROR;
    }

    return 0;
}



/**
WSGraph  JPEGImage2WSGraph(JPEGImage jp)

JPEGデータをチャンネル分解して，WSGraphに格納する．
*/
WSGraph  JPEGImage2WSGraph(JPEGImage jp)
{
    WSGraph vp;
    int  i, j, k, yp, zp;

    memset(&vp, 0, sizeof(WSGraph));
    if (jp.gp==NULL || jp.img==NULL) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    vp = make_WSGraph(jp.xs, jp.ys, jp.col);
    if (vp.gp==NULL) return vp;
    
    for (k=0; k<jp.col; k++) {
        zp = k*jp.xs*jp.ys;
        for (j=0; j<jp.ys; j++) {
            yp = zp + j*jp.xs;
            for (i=0; i<jp.xs; i++) {
                vp.gp[yp + i] = (sWord)(jp.img[j][i*jp.col + k]);   // チャンネルに分解
            }
        }
    }

    vp.state = JBXL_NORMAL;
    return vp;
}



/**
BSGraph  JPEGImage2BSGraph(JPEGImage jp)

JPEGデータをチャンネル分解して，BSGraphに格納する．
*/
BSGraph  JPEGImage2BSGraph(JPEGImage jp)
{
    BSGraph vp;
    int  i, j, k, yp, zp;

    memset(&vp, 0, sizeof(BSGraph));
    if (jp.gp==NULL || jp.img==NULL) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    vp = make_BSGraph(jp.xs, jp.ys, jp.col);
    if (vp.gp==NULL) return vp;
    
    for (k=0; k<jp.col; k++) {
        zp = k*jp.xs*jp.ys;
        for (j=0; j<jp.ys; j++) {
            yp = zp + j*jp.xs;
            for (i=0; i<jp.xs; i++) {
                vp.gp[yp + i] = (uByte)(jp.img[j][i*jp.col + k]);   // チャンネルに分解
            }
        }
    }

    vp.state = JBXL_NORMAL;
    return vp;
}



/**
JPEGImage  WSGraph2JPEGImage(WSGraph vp)
*/
JPEGImage  WSGraph2JPEGImage(WSGraph vp)
{
    JPEGImage jp;
    int  i, j, k, yp, zp;

    memset(&jp, 0, sizeof(JPEGImage));
    if (vp.gp==NULL) {
        jp.state = JBXL_GRAPH_NODATA_ERROR;
        return jp;
    }

    jp = make_JPEGImage(vp.xs, vp.ys, vp.zs);
    if (jp.gp==NULL || jp.img==NULL) return jp;

    for (k=0; k<vp.zs; k++) {
        zp = k*vp.xs*vp.ys;
        for (j=0; j<vp.ys; j++) {
            yp = zp + j*vp.xs;
            for (i=0; i<vp.xs; i++) {
                jp.img[j][i*vp.zs + k] = vp.gp[yp + i];
            }
        }
    }

    return jp;
}



/**
JPEGImage  BSGraph2JPEGImage(BSGraph vp)
*/
JPEGImage  BSGraph2JPEGImage(BSGraph vp)
{
    JPEGImage jp;
    int  i, j, k, yp, zp;

    memset(&jp, 0, sizeof(JPEGImage));
    if (vp.gp==NULL) {
        jp.state = JBXL_GRAPH_NODATA_ERROR;
        return jp;
    }

    jp = make_JPEGImage(vp.xs, vp.ys, vp.zs);
    if (jp.gp==NULL || jp.img==NULL) return jp;

    for (k=0; k<vp.zs; k++) {
        zp = k*vp.xs*vp.ys;
        for (j=0; j<vp.ys; j++) {
            yp = zp + j*vp.xs;
            for (i=0; i<vp.xs; i++) {
                jp.img[j][i*vp.zs + k] = vp.gp[yp + i];
            }
        }
    }

    return jp;
}



/**
JPEGImage  make_JPEGImage(int xs, int ys, int col)
*/
JPEGImage  make_JPEGImage(int xs, int ys, int col)
{
    int  j;
    JPEGImage jp;

    memset(&jp, 0, sizeof(JPEGImage));
    if (xs==0 || ys==0) {
        jp.state = JBXL_GRAPH_IVDARG_ERROR;
        return jp;
    }
    if (col<1) col = 3;

    jp.img = (JSAMPARRAY)malloc(sizeof(JSAMPROW)*ys);
    if (jp.img==NULL) {
        jp.state = JBXL_GRAPH_MEMORY_ERROR;
        return jp;
    }

    jp.gp  = (JSAMPLE*)malloc(sizeof(JSAMPLE)*col*xs*ys);
    if (jp.gp==NULL) {
        freeNull(jp.img);
        jp.state = JBXL_GRAPH_MEMORY_ERROR;
        return jp;
    }
    
    for (j=0; j<ys; j++) {
        jp.img[j] = (JSAMPROW)&jp.gp[j*col*xs];
    }

    jp.xs  = xs;
    jp.ys  = ys;
    jp.col = col;

    return jp;
}



/**
void  free_JPEGImage(JPEGImage* jp)
*/
void  free_JPEGImage(JPEGImage* jp)
{
    if (jp==NULL) return;

    freeNull(jp->gp);
    freeNull(jp->img);
    
    jp->xs = jp->ys = jp->col = 0;
    return;
}




////////////////////////////////////////////////////////////////////////////////////////
// for libjpeg 6b (http://www.ijg.org/)
//
#if JPEG_LIB_VERSION < 80


METHODDEF(void)  mem_init_destination(j_compress_ptr cinfo)
{
    UNUSED(cinfo);
}



METHODDEF(boolean)  mem_empty_output_buffer(j_compress_ptr cinfo)
{
    size_t nextsize;
    JOCTET * nextbuffer;
    my_mem_dest_ptr dest = (my_mem_dest_ptr)cinfo->dest;

    nextsize = dest->bufsize * 2;
    nextbuffer = (JOCTET*)malloc(nextsize);

    if (nextbuffer == NULL) return FALSE;

    memcpy(nextbuffer, dest->buffer, dest->bufsize);

    if (dest->newbuffer != NULL) free(dest->newbuffer);

    dest->newbuffer = nextbuffer;
    dest->pub.next_output_byte = nextbuffer + dest->bufsize;
    dest->pub.free_in_buffer   = dest->bufsize;
    dest->buffer  = nextbuffer;
    dest->bufsize = nextsize;

    return TRUE;
}



METHODDEF(void)  mem_term_destination(j_compress_ptr cinfo)
{
    my_mem_dest_ptr dest = (my_mem_dest_ptr)cinfo->dest;

    *dest->outbuffer = dest->buffer;
    *dest->outsize   = dest->bufsize - dest->pub.free_in_buffer;
}



GLOBAL(void)  jpeg_mem_dest(j_compress_ptr cinfo, unsigned char** buf, unsigned long* len)
{
    //memory_dest_ptr dest;
    my_mem_dest_ptr dest;

    if (cinfo->dest == NULL) {  // first time for this JPEG object? 
        cinfo->dest = (struct jpeg_destination_mgr*)
                (*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(my_mem_destination_mgr));
    }

    dest = (my_mem_dest_ptr)cinfo->dest;
    dest->pub.init_destination = mem_init_destination;
    dest->pub.empty_output_buffer = mem_empty_output_buffer;
    dest->pub.term_destination = mem_term_destination;
    dest->pub.next_output_byte = *buf;
    dest->pub.free_in_buffer   = *len;
    dest->pub.next_output_byte = dest->buffer  = *buf;
    dest->pub.free_in_buffer   = dest->bufsize = *len;
    dest->outbuffer = buf;
    dest->outsize   = len;
    dest->newbuffer = NULL;
}

#endif  // JPEG_LIB_VERSION

#endif  // DISABLE_JPEGLIB

