
/**
@brief    JP2K TOOL2 with OpenJpeg-2.x

@file     jp2k_tool2.c
@version  0.9
@date     2014 9/24
@author   Fumi.Iseki (C)

@attention
this software is based on OpenJPEG. http://www.openjpeg.org/
*/


#include "jp2k_tool.h"
#include "jbxl_state.h"


#ifdef  ENABLE_OPENJPEG

#if OPENJPEG_VER >= JP2K_VER_20


void  init_jp2k(JP2KImage* jp)
{
    if (jp==NULL) return;
    //
    jp->xs    = 0;
    jp->ys    = 0;
    jp->ws    = 0;
    jp->hs    = 0;
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
    if (jp->image->color_space==OPJ_CLRSPC_SRGB) {
        jp->col = 3;
    }
    else if (jp->image->color_space==OPJ_CLRSPC_GRAY) {
        jp->col = 1;
    }
/*
    else if (jp->image->color_space==OPJ_CLRSPC_UNSPECIFIED) {
        jp->col = (int)jp->image->numcomps;
    }
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

    FILE* fp = fopen(fname, "rb");
    if (fp==NULL) {
        jp.state = JBXL_GRAPH_OPFILE_ERROR;
        return jp;
    }

    rs = fread(head, 12, 1, fp);
    fseek(fp, 0, 0);
    int format = get_jp2k_format(head);
    if (format==JP2K_FMT_NONE) format = JP2K_FMT_JPT;

#if OPENJPEG_VER < JP2K_VER_21
    jp = read_jp2k_data(fp, format);
    fclose(fp);
#else
    fclose(fp);
    jp = read_jp2k_data(fname, format);
#endif

    return jp;
}



#if OPENJPEG_VER < JP2K_VER_21
JP2KImage  read_jp2k_data(FILE* fp, int format)
#else
JP2KImage  read_jp2k_data(const char* fname, int format)
#endif
{
    JP2KImage jp;
    init_jp2k(&jp);

    opj_stream_t* stream = NULL;
    opj_codec_t*  codec  = NULL;

    opj_dparameters_t parameters;   
    opj_set_default_decoder_parameters(&parameters);

#if OPENJPEG_VER < JP2K_VER_21
    stream = opj_stream_create_default_file_stream(fp, 1);      // 2.0.0
#else
    stream = opj_stream_create_default_file_stream(fname, 1);   // 2.1.0
#endif

    if (stream==NULL){
        jp.state = JBXL_GRAPH_RDFILE_ERROR;
        return jp;
    }

    if (format==JP2K_FMT_J2K) {         // JPEG 2000 codestream
        codec = opj_create_decompress(OPJ_CODEC_J2K);
    }   
    else if (format==JP2K_FMT_JP2) {    // JPEG 2000 compressed image data
        codec = opj_create_decompress(OPJ_CODEC_JP2);
    }
    else if (format==JP2K_FMT_JPT) {    // JPEG 2000 JPIP
        codec = opj_create_decompress(OPJ_CODEC_JPT);
    }
    else {
        PRINT_MESG("JBXL::readJPEG2KData: ERROR: unknown file format!\n");
        opj_stream_destroy(stream);
        return jp;
    }

    if (!opj_setup_decoder(codec, &parameters) ){
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        jp.state = JBXL_GRAPH_ERROR;
        return jp;
    }
    if (!opj_read_header(stream, codec, &jp.image)){
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        jp.state = JBXL_GRAPH_ERROR;
        return jp;
    }
    if (!opj_set_decode_area(codec, jp.image, 0, 0, 0, 0)){
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free_jp2k(&jp);
        jp.state = JBXL_GRAPH_ERROR;
        return jp;
    }
    if (!(opj_decode(codec, stream, jp.image) && opj_end_decompress(codec, stream))) {
        opj_destroy_codec(codec);
        opj_stream_destroy(stream);
        free_jp2k(&jp);
        jp.state = JBXL_GRAPH_ERROR;
        return jp;
    }

    setup_jp2k(&jp);

    opj_stream_destroy(stream);
    opj_destroy_codec(codec);

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


#endif      // OPENJPEG_VER >= JP2K_VER_20
#endif      // ENABLE_OPENJPEG

