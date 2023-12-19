/**
@brief JPEG 2000グラフィックデータ定義 for C++  (with OpenJpeg-1.5)

現在は JPEG 2000の読み込みのみ．@n
何枚もの画像が含まれていたり，オフセットが指定されている場合には未対応．

@file     Jpeg2KTool.cpp
@version  0.9
@date     2014 10/19 
@author   Fumi.Iseki (C)

@attention
this software is based on OpenJPEG. http://www.openjpeg.org/
this software does not support OpenJpeg-1.3
*/


#include "Jpeg2KTool.h"


#ifdef ENABLE_OPENJPEG

#if OPENJPEG_VER <= JP2K_VER_15


using namespace jbxl;


void  JPEG2KImage::init(void)
{
    xs    = 0;
    ys    = 0;
    ws    = 0;
    hs    = 0;
    col   = 0;
    cmode = GRAPH_COLOR_RGBA;
    state = 0;
    image = NULL;
}


bool  JPEG2KImage::isNull(void)
{
    if (image!=NULL && image->comps!=NULL) return false;
    return true;
}


void  JPEG2KImage::free(void)
{
    if (image!=NULL) {
        opj_image_destroy(image);
    }

    init();
}


void  JPEG2KImage::fill(int v)
{
    if (!isNull()) {
        for (int k=0; k<col; k++) {
            if (image->comps[k].data!=NULL) {   
                for (int i=0; i<xs*ys; i++) {
                    image->comps[k].data[i] = v;
                }
            }
        }
    }
}


void  JPEG2KImage::setup_image(void)
{
    if (!isNull()) {
        //
        xs = image->x1 - image->x0;
        ys = image->y1 - image->y0;

        int fac = (int)image->comps->factor;
        ws = (xs + (1<<fac) -1)>>fac;
        hs = (ys + (1<<fac) -1)>>fac;

        col = (int)image->numcomps;
        if (image->color_space==CLRSPC_SRGB) {
            col = 3;
        }
        else if (image->color_space==CLRSPC_GRAY) {
            col = 1;
        }
/*
#if OPENJPEG_VER > JP2K_VER_12
        else if (image->color_space==CLRSPC_UNSPECIFIED) {
            col = (int)image->numcomps;
        }
#endif
        else if (image->color_space==CLRSPC_SYCC) {
            color_sycc_to_rgb(image);
            col = 3;
        }
*/
        // 設定されないものについては，未対応
        cmode = GRAPH_COLOR_UNKNOWN;
        int depth = (int)image->comps->bpp;
        if (depth==0) {
            if      (col==3) cmode = GRAPH_COLOR_RGB;
            else if (col==4) cmode = GRAPH_COLOR_RGBA;
            else if (col==1) cmode = GRAPH_COLOR_MONO;
        }
        else if (depth==32) {
            if      (col==3) cmode = GRAPH_COLOR_RGB;
            else if (col==4) cmode = GRAPH_COLOR_RGBA;
        }
        else if (depth==24) {
            if      (col==3) cmode = GRAPH_COLOR_RGB;
        }
        else if (depth==16) {
            if      (col==1) cmode = GRAPH_COLOR_MONO16;
            else if (col==3) cmode = GRAPH_COLOR_RGB16;
            else if (col==4) cmode = GRAPH_COLOR_RGBA16;
        }
        else if (depth==8) {
            if      (col==1) cmode = GRAPH_COLOR_MONO;
        }
  
        //
#if OPENJPEG_VER > JP2K_VER_12
        freeNull(image->icc_profile_buf);
        image->icc_profile_len = 0;
#endif

/*
        DEBUG_MODE {
            PRINT_MESG("JPEG2KImage::setup_image: OFFSET  %d %d %d %d\n", image->x0, image->y0, image->x1, image->y1);
            PRINT_MESG("JPEG2KImage::setup_image: NUMCOMP %d\n", image->numcomps);
            PRINT_MESG("JPEG2KImage::setup_image: COLORSP %d\n", image->color_space);
            PRINT_MESG("JPEG2KImage::setup_image: XS YS   %d %d\n", xs, ys);
            PRINT_MESG("JPEG2KImage::setup_image: COLOR   %d\n", col);
            PRINT_MESG("JPEG2KImage::setup_image: CMODE   %d\n", cmode);
            PRINT_MESG("\n");
            for (int i=0; i<(int)image->numcomps; i++) {
                PRINT_MESG("JPEG2KImage::setup_image: DX DY %d %d\n", image->comps[i].dx, image->comps[i].dy);
                PRINT_MESG("JPEG2KImage::setup_image: W  H  %d %d\n", image->comps[i].w,  image->comps[i].h);
                PRINT_MESG("JPEG2KImage::setup_image: X0 Y0 %d %d\n", image->comps[i].x0, image->comps[i].y0);
                PRINT_MESG("JPEG2KImage::setup_image: PREC  %d\n", image->comps[i].prec);
                PRINT_MESG("JPEG2KImage::setup_image: DEPTH %d\n", image->comps[i].bpp);
                PRINT_MESG("JPEG2KImage::setup_image: SGND  %d\n", image->comps[i].sgnd);
                PRINT_MESG("JPEG2KImage::setup_image: RESNO %d\n", image->comps[i].resno_decoded);
                PRINT_MESG("JPEG2KImage::setup_image: FACT  %d\n", image->comps[i].factor);
                PRINT_MESG("\n");
            }
        }
*/
    }
}
    


//////////////////////////////////////////////////////////////////////

JPEG2KImage  jbxl::readJPEG2KFile(const char* fname)
{
    JPEG2KImage jp;
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

    Buffer buf = read_Buffer_data(fp, 12);
    if (buf.vldsz<12) {
        free_Buffer(&buf);
        jp.state = JBXL_GRAPH_FILESZ_ERROR;
        fclose(fp);
        return jp;
    }

    fseek(fp, 0, 0);
    int format = isJPEG2KHeader(buf);
    if (format==JP2K_FMT_NONE) format = JP2K_FMT_JPT;
    free_Buffer(&buf);

    fread(data, len, 1, fp);
    fclose(fp);

    jp = readJPEG2KData(data, len, format);
    ::free(data);

    return jp;
}


JPEG2KImage  jbxl::readJPEG2KData(unsigned char* data, int len, int format)
{
    JPEG2KImage jp;

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

    jp.setup_image();

    opj_cio_close(stream);
    opj_destroy_decompress(codec);

    return jp;
}


/**
JPEG 2000のヘッダからファイルの種類を返す．@n
ただし，ヘッダから JP2K_FMT_JPTであることは判別できないので，注意する．

@param buf ヘッダ情報の入った Buffer変数．最低12Byte必要．
@return    ファイル種別 (JP2K_FMT_NONE, JP2K_FMT_JP2, JP2K_FMT_J2K)
*/
int  jbxl::isJPEG2KHeader(Buffer buf)
{
    int format = JP2K_FMT_NONE;

    if (!memcmp(buf.buf, JP2K_MAGIC_RFC3745_JP2, 12) || !memcmp(buf.buf, JP2K_MAGIC_JP2, 4)) {
        format = JP2K_FMT_JP2;
    }
    else if (!memcmp(buf.buf, JP2K_MAGIC_J2K, 4)) {
        format = JP2K_FMT_J2K;
    }
    return format;
}


#endif      // OPENJPEG_VER
#endif      // ENABLE_OPENJPEG


