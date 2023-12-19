
/**
@brief    JP2K TOOL with OpenJpeg-1.5

@file     jp2k_tool.c
@version  0.9
@date     2014 10/19
@author   Fumi.Iseki (C)

@attention
this software is based on OpenJPEG. http://www.openjpeg.org/
this software does not support OpenJpeg-1.3
*/


#include "jp2k_tool.h"
#include "jbxl_state.h"


#ifdef  ENABLE_OPENJPEG

#if OPENJPEG_VER <= JP2K_VER_15

void  init_jp2k(JP2KImage* jp)
{
    if (jp==NULL) return;
    //
    jp->xs    = 0;
    jp->ys    = 0;
    jp->col   = 0;
    jp->cmode = GRAPH_COLOR_RGBA;
    jp->state = JBXL_NORMAL;
    jp->image = NULL;
}


void  free_jp2k(JP2KImage* jp)
{
    if (jp==NULL) return;
    //
    if (jp->image!=NULL) {
        opj_image_destroy(jp->image);
    }
    init_jp2k(jp);
}



void  setup_jp2k(JP2KImage* jp)
{
    if (jp==NULL || jp->image==NULL || jp->image->comps==NULL) return;
    //
    jp->xs = jp->image->x1 - jp->image->x0;
    jp->ys = jp->image->y1 - jp->image->y0;

    int fac = (int)jp->image->comps->factor;
    jp->ws = (jp->xs + (1<<fac) - 1)>>fac;
    jp->hs = (jp->ys + (1<<fac) - 1)>>fac;

    jp->col = (int)jp->image->numcomps;
    if (jp->image->color_space==CLRSPC_SRGB) {
        jp->col = 3;
    }
    else if (jp->image->color_space==CLRSPC_GRAY) {
        jp->col = 1;
    }
/*
#if OPENJPEG_VER > JP2K_VER_12
    else if (jp->image->color_space==CLRSPC_UNSPECIFIED) {
        jp->col = (int)jp->image->numcomps;
    }
#endif
*/
    // 設定されないものについては，未対応
    jp->cmode = GRAPH_COLOR_UNKNOWN;
    int depth = (int)jp->image->comps->bpp;
    if (depth==0) {
        if      (jp->col==3) jp->cmode = GRAPH_COLOR_RGB;
        else if (jp->col==4) jp->cmode = GRAPH_COLOR_RGBA;
    }
    else if (depth==32) {
        if      (jp->col==3) jp->cmode = GRAPH_COLOR_RGB;
        else if (jp->col==4) jp->cmode = GRAPH_COLOR_RGBA;
    }
    else if (depth==24) {
        if      (jp->col==3) jp->cmode = GRAPH_COLOR_RGB;
    }
    else if (depth==16) {
        if      (jp->col==1) jp->cmode = GRAPH_COLOR_MONO16;
        else if (jp->col==3) jp->cmode = GRAPH_COLOR_RGB16;
        else if (jp->col==4) jp->cmode = GRAPH_COLOR_RGBA16;
    }
    else if (depth==8) {
        if      (jp->col==1) jp->cmode = GRAPH_COLOR_MONO;
    }

#if OPENJPEG_VER > JP2K_VER_12
    freeNull(jp->image->icc_profile_buf);
    jp->image->icc_profile_len = 0;
#endif
}



/**
JPEG 2000のヘッダからファイルの種類を返す．@n
ただし，ヘッダから JP2K_FMT_JPTであることは判別できないので，注意する．

@param buf ヘッダ情報の入った変数．最低12Byte必要．
@return    ファイル種別 (JP2K_FMT_NONE, JP2K_FMT_JP2, JP2K_FMT_J2K)
*/
int  get_jp2k_format(uByte* buf)
{
    int format = JP2K_FMT_NONE;

    if (!memcmp(buf, JP2K_MAGIC_RFC3745_JP2, 12) || !memcmp(buf, JP2K_MAGIC_JP2, 4)) {
        format = JP2K_FMT_JP2;
    }
    else if (!memcmp(buf, JP2K_MAGIC_J2K, 4)) {
        format = JP2K_FMT_J2K;
    }
    return format;
}



JP2KImage  read_jp2k_file(const char* fname)
{
    unsigned char head[12];
    JP2KImage jp;
    size_t rs;
    UNUSED(rs);

    init_jp2k(&jp);
    //
    if (fname==NULL) {
        jp.state = JBXL_GRAPH_IVDARG_ERROR;
        return jp;
    }

    int len = (int)file_size(fname);

    FILE* fp = fopen(fname, "rb");
    if (fp==NULL) {
        jp.state = JBXL_GRAPH_OPFILE_ERROR;
        return jp;
    }

    unsigned char* data = (unsigned char*)malloc(len);
    if (data==NULL) {
        fclose(fp);
        jp.state = JBXL_GRAPH_MEMORY_ERROR;
        return jp;
    }

    rs = fread(head, 12, 1, fp);
    fseek(fp, 0, 0);
    int format = get_jp2k_format(head);
    if (format==JP2K_FMT_NONE) format = JP2K_FMT_JPT;

    rs = fread(data, len, 1, fp);
    fclose(fp);

    jp = read_jp2k_data(data, len, format);
    free(data);

    return jp;
}



JP2KImage  read_jp2k_data(unsigned char* data, int len, int format)
{
    JP2KImage jp;
    init_jp2k(&jp);

    opj_dinfo_t* codec  = NULL;
    opj_cio_t*   stream = NULL;

    opj_dparameters_t parameters;   
    opj_set_default_decoder_parameters(&parameters);

    if (format==JP2K_FMT_J2K) {         // JPEG 2000 codestream
        codec = opj_create_decompress(CODEC_J2K);
    }   
    else if (format==JP2K_FMT_JP2) {    // JPEG 2000 compressed image data
        codec = opj_create_decompress(CODEC_JP2);
    }
    else if (format==JP2K_FMT_JPT) {    // JPEG 2000 JPIP
        codec = opj_create_decompress(CODEC_JPT);
    }
    else {
        PRINT_MESG("JBXL::readJPEG2KData: ERROR: unknown file format!\n");
        jp.state = JBXL_GRAPH_NODATA_ERROR;
        return jp;
    }

    opj_setup_decoder(codec, &parameters);
    stream = opj_cio_open((opj_common_ptr)codec, data, len);
    jp.image = opj_decode(codec, stream);
    if (jp.image==NULL) {
        opj_cio_close(stream);
        opj_destroy_decompress(codec);
        jp.state = JBXL_GRAPH_IVDDATA_ERROR;
        return jp;
    }

    setup_jp2k(&jp);

    opj_cio_close(stream);
    opj_destroy_decompress(codec);

    return jp;
}



BSGraph  jp2k_toBSGraph(JP2KImage jp)
{
    BSGraph vp;
    int i, j, k, yp, yz, zp;

    memset(&vp, 0, sizeof(BSGraph));
    if (jp.image==NULL || jp.image->comps==NULL) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    vp = make_BSGraph(jp.ws, jp.hs, jp.col);
    if (vp.gp==NULL) return vp;

    for (k=0; k<jp.col; k++) {
        zp = k*jp.ws*jp.hs;
        for (j=0; j<jp.hs; j++) {
            yp = j*jp.xs;
            yz = j*jp.ws + zp;
            for (i=0; i<jp.ws; i++) {
                vp.gp[yz + i] = (uByte)jp.image->comps[k].data[yp + i];  // as Byte境界
            }
        }
    }

    return vp;
}


#endif    // OPENJPEG_VER <= JP2K_VER_15
#endif    // ENABLE_OPENJPEG

