
/**
@brief    グラフィック用ファイル入出力関数
@file     gio.c
@version  3.1
@date     2007 6/27
@author   Fumi.Iseki (C)
*/


#include "gio.h"
#include "jbxl_state.h"


IRBound  ExRBound;


///////////////////////////////////////////////////////////////////////////////
// ファイル名によるI/Oサポート

/**
int  write_wsg_file(const char* fname, WSGraph gr)

グラフィックデータgrを CTファイルとして書き出す．
- gr.zs<=1 なら CT_DATAとして保存される．
- gr.zs >1 なら CT_3DM として保存される．
- RZxy!=1.0 ならヘッダにその情報が埋め込まれる．

@param  *fname             保存する際のファイル名．
@param  gr                 保存するグラフィックデータ．

@retval 0                  正常終了．
@retval JBXL_GRAPH_OPFILE_ERROR ファイルオープンエラー
@retval JBXL_GRAPH_MEMORY_ERROR メモリエラー
*/
int  write_wsg_file(const char* fname, WSGraph gr)
{
    CmnHead hd;
    unsigned int  i;

    hd.xsize = gr.xs;
    hd.ysize = gr.ys;
    hd.zsize = gr.zs;
    hd.depth = 16;
    hd.grptr = (uByte*)gr.gp;
    hd.kind  = CT_3DM;

    if (hd.zsize<=1){
        hd.zsize = 1;
        hd.kind  = CT_DATA;
    }
    hd.lsize = (hd.depth+7)/8*hd.zsize*hd.ysize*hd.zsize;
    hd.bsize = sizeof(CTHead);
    hd.buf   = (uByte*)malloc(hd.bsize);
    if (hd.buf==NULL) {
        //fprintf(stderr, "WRITE_FILE: no more memory!! %d\n", hd.bsize);
        return JBXL_GRAPH_MEMORY_ERROR;
    }

    for (i=0; i<hd.bsize; i++) *(hd.buf+i) = (sByte)0;
    int ret = write_ct_file(fname, &hd);

    free(hd.buf);
    return ret;
}



/**
int  write_ras_file(const char* fname, WSGraph gr)

グラフィックデータ grを SUN RASTER形式でファイルに書き出す．モノクロ, 256階調．

@param  fname  保存する際のファイル名．
@param  gr     保存するグラフィックデータ．

@retval 0      正常終了．
@retval JBXL_GRAPH_OPFILE_ERROR ファイルオープンエラー
@retval JBXL_GRAPH_MEMORY_ERROR メモリエラー
*/
int  write_ras_file(const char* fname, WSGraph gr)
{
    CmnHead hd;

    init_CmnHead(&hd);
    hd.xsize = gr.xs;
    hd.ysize = gr.ys;
    hd.zsize = gr.zs;
    hd.depth = 16;
    hd.grptr = (uByte*)gr.gp;

    if (hd.zsize>1) {
        //fprintf(stderr,"WRITE_RAS_FILE: 3D SunRaster is not supported!!\n");
        return JBXL_GRAPH_HEADER_ERROR;
    }
    else {
        hd.zsize = Max(hd.zsize, 1);
        hd.kind  = CT_DATA;
        hd.lsize = (hd.depth+7)/8*hd.xsize*hd.ysize*hd.zsize;
        hd.bsize = sizeof(CTHead);
        hd.buf   = (uByte*)malloc(hd.bsize);
        if (hd.buf==NULL) {
            //fprintf(stderr,"WRITE_RAS_FILE: no more memory!! %d\n", hd.bsize);
            return JBXL_GRAPH_MEMORY_ERROR;
        }
        memset(hd.buf, 0, hd.bsize);
    }

    int ret = write_ras_file_obit(fname, &hd, 8);

    free(hd.buf);
    return ret;
}



/**
int  write_file_rb(const char* fname, WSGraph gr, IRBound rb)

グラフィックデータgrを 境界データ付き CTファイルとして書き出す．
- gr.zs<=1 なら CT_DATAとして保存される．
- gr.zs >1 なら CT_3DM として保存される．
- RZxy!=1.0 ならヘッダにその情報が埋め込まれる．

@param  *fname  保存する際のファイル名．
@param  gr      保存するグラフィックデータ．
@param  rb      保存する境界データ．

@retval 0       正常終了．
@retval JBXL_GRAPH_OPFILE_ERROR ファイルオープンエラー
@retval JBXL_GRAPH_MEMORY_ERROR メモリエラー
*/
int  write_file_rb(const char* fname, WSGraph gr, IRBound rb)
{
    CmnHead hd;
    CTHead* chd;
    unsigned int  i;

    init_CmnHead(&hd);
    hd.xsize = gr.xs;
    hd.ysize = gr.ys;
    hd.zsize = gr.zs;
    hd.depth = 16;
    hd.grptr = (uByte*)gr.gp;
    hd.kind  = CT_3DM | HAS_RBOUND;

    if (hd.zsize<=0) {
        hd.zsize = 1;
        hd.kind  = CT_DATA | HAS_RBOUND;
    }
    hd.lsize = (hd.depth+7)/8*hd.zsize*hd.ysize*hd.zsize;
    hd.bsize = sizeof(CTHead);
    hd.buf   = (uByte*)malloc(hd.bsize);
    if (hd.buf==NULL) {
        //fprintf(stderr, "WRITE_FILE_RB: no more memory!! %d\n", hd.bsize);
        return JBXL_GRAPH_MEMORY_ERROR;
    }

    for (i=0; i<hd.bsize; i++) *(hd.buf+i) = (sByte)0;
    chd = (CTHead*)hd.buf;
    chd->cutleft  = rb.xmin;
    chd->cutright = rb.xmax;
    chd->cutup    = rb.ymin;
    chd->cutdown  = rb.ymax;
    chd->ctmin    = rb.zmin;
    chd->ctmax    = rb.zmax;

    if (RZxy!=1.0 && RZxy<RZXY_RATE && chk_RZxy()) {
        chd->anydata[0] = (sWord)(RZxy*RZXY_RATE);
        chd->anydata[1] = (sWord)RZXY_RATE;
        hd.kind        |= HAS_RZXY;
    }       
            
    if (ZeroBase!=0) {
        chd->anydata[2] = (sWord)ZeroBase;
        hd.kind        |= HAS_BASE;
    }   

    int ret = write_ct_file(fname, &hd);

    free(hd.buf);
    return ret;
}



/**
WSGraph  read_wsg_file(const char* fname)

グラフィックファイルを読み込む．

サポートしているデータ形式は, 
@b COMMON, @b CT_DATA, @b CT_3DM, @b RAS_DATA, @b USERSET_DATA, @b ANY_DATA @n
ただし,ANY_DATAは hd->bufと hd->bsizeのみ有効．

@param  fname  読み込むを行うファイル名．

@return 読み込んだグラフィックデータ．
@retval NULL @b gpメンバ: エラー
        JBXL_GRAPH_OPFILE_ERROR @b stateメンバ: ファイルオープンエラー
        JBXL_GRAPH_MEMORY_ERROR @b stateメンバ: メモリエラー
*/
WSGraph  read_wsg_file(const char* fname)
{
    WSGraph  gr;
    CmnHead  hd;
    int  i, xs, ys, zs;
    double rzm;
    sWord* rz;

    memset(&gr, 0, sizeof(WSGraph));

    hd = read_xxx_file(fname);
    if (hd.kind==HEADER_NONE) {
        //fprintf(stderr,"READ_FILE: read file %s open error!!\n",fname);
        gr.state = JBXL_GRAPH_HEADER_ERROR;
        return gr;
    }

    if (checkBit(hd.kind, HAS_RBOUND)) {
        //fprintf(stderr,"read_wsg_file: this file %s has RBound data.\n",fname);
        //fprintf(stderr,"read_wsg_file: I call read_wsg_file_rb() function.\n");
        //fprintf(stderr,"read_wsg_file: RBound data is in ExRBound.\n");
        gr = read_wsg_file_rb(fname, &ExRBound);
        return gr;
    }
    
    xs = hd.xsize;
    ys = hd.ysize;
    zs = Max(hd.zsize, 1);

    if (hd.depth==16){
        gr.gp = (sWord*)hd.grptr;
    }
    else if (hd.depth<16){
        gr.gp = (sWord*)malloc(xs*ys*zs*sizeof(sWord));
        if (gr.gp==NULL) {
            //fprintf(stderr,"READ_FILE: no more memory! %d\n",xs*ys*zs*sizeof(sWord));
            free_CmnHead(&hd);
            gr.state = JBXL_GRAPH_MEMORY_ERROR;
            return gr;
        }
        for (i=0; i<xs*ys*zs; i++) gr.gp[i] = (uByte)hd.grptr[i];
        free(hd.grptr);
    }
    else if (hd.depth>16){
        gr.gp = (sWord*)malloc(xs*ys*zs*sizeof(sWord));
        if (gr.gp==NULL) {
            //fprintf(stderr,"READ_FILE: no more memory! %d\n",xs*ys*zs*sizeof(sWord));
            free_CmnHead(&hd);
            gr.state = JBXL_GRAPH_MEMORY_ERROR;
            return gr;
        }
        for (i=0;i<xs*ys*zs;i++) gr.gp[i]=(sWord)((uByte)hd.grptr[i]>>(hd.depth-15));
        free(hd.grptr);
    }

    gr.xs = xs;
    gr.ys = ys;
    gr.zs = zs;
    gr.state = JBXL_NORMAL;

    if (hd.bsize>0 && checkBit(hd.kind, CT_DATA)) {
        rz  = (sWord*)hd.buf;
        if (rz[9]==RZXY_RATE || checkBit(hd.kind, HAS_RZXY)) {
            rzm = (double)(rz[8])/rz[9];
            if (rzm<1.0 && rzm>0.1) set_RZxy(rzm);
            //else fprintf(stderr,"READ_FILE: RZxy = %f ?????\n",rzm);
        }
        if (rz[10]!=0 && checkBit(hd.kind, HAS_BASE)) {
            ZeroBase = rz[10];
        }
    }
    free(hd.buf);

    return  gr;
}



/**
WSGraph  read_ras_file(const char* fn)

SUN RASTER型グラフィックファイルを読み込む．

@param  fn  読み込むを行う SUN RASTERのファイル名．

@return 読み込んだグラフィックデータ．
@retval NULL @b gpメンバ: エラー
        JBXL_GRAPH_OPFILE_ERROR @b stateメンバ: ファイルオープンエラー
        JBXL_GRAPH_MEMORY_ERROR @b stateメンバ: メモリエラー
        JBXL_GRAPH_HEADER_ERROR @b stateメンバ: ファイル種別を認識できない．
*/
WSGraph  read_ras_file(const char* fn)
{
    int  i, sz;
    FILE *fp;
    CmnHead hd;
    WSGraph gd;
    size_t rs;
    UNUSED(rs);

    memset(&gd, 0, sizeof(WSGraph));

    if ((fp=fopen(fn,"rb"))==NULL) {
        //fprintf(stderr,"READ_RAS_FILE: read file open error!! %s\n",fn);
        gd.state = JBXL_GRAPH_OPFILE_ERROR;
        return gd;
    }

    rs = fread(&hd, sizeof(CmnHead), 1, fp);
    ntoh_st(&hd, 4);
    if (hd.kind!=RAS_MAGIC) {
        //fprintf(stderr,"READ_RAS_FILE: %s is not sunraster. kind = %d\n",fn,hd.kind);
        gd.state = JBXL_GRAPH_HEADER_ERROR;
        return gd;
    }

    hd = read_ras_data(fp); 
    fclose(fp);
    if (hd.kind==HEADER_NONE) {
        free_CmnHead(&hd);
        gd.state = JBXL_GRAPH_HEADER_ERROR;
        return gd;
    }

    gd.xs = hd.xsize;
    gd.ys = hd.ysize;
    gd.zs = 1;

    sz  = gd.xs*gd.ys*gd.zs;
    gd.gp = (sWord*)malloc(sz*sizeof(sWord));
    if (gd.gp==NULL) {
        //fprintf(stderr,"READ_RAS_FILE: no more memory!! %d\n", sz);
        memset(&gd, 0, sizeof(WSGraph));
        gd.state = JBXL_GRAPH_MEMORY_ERROR;
        return gd;
    }

    for (i=0; i<sz; i++) gd.gp[i] = (uByte)hd.grptr[i];
    free_CmnHead(&hd);

    gd.state = JBXL_NORMAL;
    return gd;
}



/**
WSGraph  read_wsg_file_rb(const char* fname, IRBound* rb)

境界データ付きとして,グラフィックファイルを読み込む．

サポートしているデータ形式は, 
@b COMMON, @b CT_DATA, @b CT_3DM, @b RAS_DATA, @b USERSET_DATA, @b ANY_DATA @n
ただし, ANY_DATAは hd->bufと hd->bsizeのみ有効．

@param  fname  読み込むを行うファイル名．
@param  rb     設定しない．境界データが入る．

@return 読み込んだグラフィックデータ．
@retval NULL @b gpメンバ: エラー
        JBXL_GRAPH_OPFILE_ERROR @b stateメンバ: ファイルオープンエラー
        JBXL_GRAPH_MEMORY_ERROR @b stateメンバ: メモリエラー
*/
WSGraph  read_wsg_file_rb(const char* fname, IRBound* rb)
{
    int  i, xs, ys, zs;
    double  rzm;
    WSGraph  gr;
    CmnHead  hd;
    CTHead*  chd;
    sWord* rz;
  
    memset(&gr, 0, sizeof(WSGraph));

    hd = read_xxx_file(fname);
    if (hd.kind==HEADER_NONE) {
        //fprintf(stderr,"READ_FILE_RB: read file %s open error!!\n",fname);
        //fprintf(stderr,"\n");
        gr.state = JBXL_GRAPH_OPFILE_ERROR;
        return gr;
    }
    
    xs = hd.xsize;
    ys = hd.ysize;
    zs = Max(hd.zsize, 1);
    rb->xmin = rb->ymin = rb->zmin = 0;
    rb->xmax = xs - 1;
    rb->ymax = ys - 1;
    rb->zmax = zs - 1;

    if (hd.depth==16){
        gr.gp = (sWord*)hd.grptr;
    }
    else if (hd.depth<16){
        gr.gp = (sWord*)malloc(xs*ys*zs*sizeof(sWord));
        if (gr.gp==NULL) {
            //fprintf(stderr,"READ_FILE_RB: no more memory! %d\n",xs*ys*zs*sizeof(sWord));
            gr.state = JBXL_GRAPH_MEMORY_ERROR;
            return gr;
        }
        for (i=0; i<xs*ys*zs; i++) gr.gp[i] = (uByte)hd.grptr[i];
        free(hd.grptr);
    }
    else if (hd.depth>16){
        gr.gp = (sWord*)malloc(xs*ys*zs*sizeof(sWord));
        if (gr.gp==NULL) { 
            //fprintf(stderr,"READ_FILE_RB: no more miemory! %d\n",xs*ys*zs*sizeof(sWord));
            gr.state = JBXL_GRAPH_MEMORY_ERROR;
            return gr;
        }
        for (i=0;i<xs*ys*zs;i++) gr.gp[i]=(sWord)((uByte)hd.grptr[i]>>(hd.depth-15));
        free(hd.grptr);
    }

    gr.xs = xs;
    gr.ys = ys;
    gr.zs = zs;

    if (hd.bsize>0 && checkBit(hd.kind, CT_DATA)) {
        if (checkBit(hd.kind, HAS_RBOUND)) {
            chd = (CTHead*)hd.buf;
            rb->xmin = chd->cutleft;
            rb->xmax = chd->cutright;
            rb->ymin = chd->cutup;
            rb->ymax = chd->cutdown;
            rb->zmin = chd->ctmin;
            rb->zmax = chd->ctmax;
        }
        rz  = (sWord*)hd.buf;
        if (rz[9]==RZXY_RATE || checkBit(hd.kind, HAS_RZXY)) {
            rzm = (double)(rz[8])/rz[9];
            if (rzm<1.0 && rzm>0.1) set_RZxy(rzm);
            //else fprintf(stderr,"READ_FILE_RB: RZxy = %f ?????\n",rzm);
        }
        if (rz[10]!=0 && checkBit(hd.kind, HAS_BASE)) {
            ZeroBase = rz[10];
        }
    }
    free(hd.buf);

    gr.state = JBXL_NORMAL;
    return gr;
}




///////////////////////////////////////////////////////////////////////////////
// ファイル名と共通ヘッダによるI/Oサポート

/**
int  write_ras_file_obit(const char* fn, CmnHead* hd, int obit)

色々な画像データをSUN RASTER型式でファイルに保存する．

@param  fn    保存するファイル名．
@param  hd    保存するデータを既述した共通ヘッダ．
@param  obit  SUN RASTERの 1ピクセルのビット数(8, 24をサポート)．

@retval 0     正常終了．
@retval JBXL_GRAPH_OPFILE_ERROR ファイルオープンエラー
@retval JBXL_GRAPH_MEMORY_ERROR メモリエラー
*/
int  write_ras_file_obit(const char* fn, CmnHead* hd, int obit)
{
    FILE *fp;
    RasHead shd;
    int  i, j, k, l, linebyte, databyte, depth, lsize;
    uByte null=0x00, *ptr, *buf;

    if ((fp=fopen(fn,"wb"))==NULL) return JBXL_GRAPH_OPFILE_ERROR;
    obit = Xabs(obit);

    if (hd->kind == RAS_DATA) {
        unsigned int u;
        memcpy((sByte*)&shd, hd->buf, hd->bsize);
        ptr = (uByte*)malloc(hd->lsize);
        if (ptr==NULL)  return JBXL_GRAPH_MEMORY_ERROR;
        lsize = hd->lsize;
        for (u=0; u<hd->lsize; u++) ptr[u] = hd->grptr[u];
    }
    else { // CT -> RAS 
        if (hd->depth==16) {

            if (obit==8) depth = 8;
            else         depth = 24;

            lsize = hd->xsize*hd->ysize*depth/8;
            buf = (uByte*)malloc(lsize);
            if (buf==NULL)  return JBXL_GRAPH_MEMORY_ERROR;

            if (obit==8) {
                int  max = 255; // 8bit mode での最大値 
                uWord* wp = (uWord*)hd->grptr;
                for (i=0; i<hd->xsize*hd->ysize; i++) {
                    max = Max(max, wp[i]);
                }
                for (i=0; i<hd->ysize*hd->xsize; i++) {
                    buf[i] = 255*wp[i]/max;
                }
            }
            else {
                k = l = 0;
                for (i=0; i<hd->ysize*hd->xsize; i++) {
                    buf[k++] = hd->grptr[l++];
                    buf[k++] = hd->grptr[l++];
                    buf[k++] = null;
                }
            }
        }
        else {
            depth = hd->depth;
            lsize = hd->lsize;
            buf   = (uByte*)hd->grptr;
        }

        databyte = hd->xsize*depth/8;
        linebyte = lsize/hd->ysize;
        if (linebyte%2==1) {
            linebyte++;
            lsize = linebyte*hd->ysize;
        }

        shd.ras_magic    = RAS_MAGIC;
        shd.ras_width    = hd->xsize;
        shd.ras_height   = hd->ysize;
        shd.ras_depth    = depth;
        shd.ras_length   = lsize;
        shd.ras_type     = RT_STANDARD;
        shd.ras_maptype  = RMT_NONE;
        shd.ras_maplength= 0;

        ptr = (uByte*)malloc(lsize);
        if (ptr==NULL) {
            if (hd->depth==16) free(buf);
            return JBXL_GRAPH_MEMORY_ERROR;
        }

        k = l = 0;
        for (i=0 ; i<hd->ysize; i++) {
            for (j=0; j<databyte; j++)        ptr[k++] = buf[l++];
            for (j=0; j<linebyte-databyte; j++) ptr[k++] = null;
        }

        if (hd->depth==16) free(buf);
    }

    hton_st(&shd, 4);
    fwrite(&shd, sizeof(RasHead), 1, fp);
    fwrite(ptr, lsize, 1, fp);

    fclose(fp);
    free(ptr);
    return  0;
}



/**
int  write_ct_file(const char* fn, CmnHead* hd)

共通画像データをファイルに保存する．

元の共通画像データのヘッダ種別が CT_DATA または UN_KNOWNの場合は CT(MOON)形式で保存する．@n
CT_3DM 場合はそのまま共通形式(ヘッダ種別 CT_3DM)で保存する．

ヘッダに必要な情報:
@code
    UN_KNOWN -> CT(Moon)   need  hd->xsize, ysize, depth
    CT_DATA  -> CT(Moon)   need  hd->kind, xsize, ysize, (buf)
    CT_3DM   -> CT_3DM     need  hd->kind, xsize, ysize, zsize, (buf)
    ( )は省略可.
@endcode

@param  fn  保存するファイル名．
@param  hd  保存するデータを既述した共通ヘッダ．

@retval 0   正常終了．
@retval JBXL_GRAPH_OPFILE_ERROR ファイルオープンエラー
@retval JBXL_GRAPH_MEMORY_ERROR メモリエラー
*/
int  write_ct_file(const char* fn, CmnHead* hd)
{
    CTHead  chd;
    CmnHead cmd;
    FILE*  fp;
    sByte* ptr;
    sWord* wp;
    sByte  null=0x00;
    int i, j, k, l, lsize;
    int dbyte=2;   //  sWord TYPE data 

    if ((fp=fopen(fn,"wb"))==NULL) return JBXL_GRAPH_OPFILE_ERROR;

    hd->zsize = Max(hd->zsize, 1); 
    lsize = hd->xsize*hd->ysize*hd->zsize*dbyte;
    hd->lsize = lsize;
    ptr = (sByte*)malloc(lsize);
    if (ptr==NULL) return JBXL_GRAPH_MEMORY_ERROR;

    if (checkBit(hd->kind, CT_DATA) || checkBit(hd->kind, CT_3DM)) {
        memcpy((sByte*)&chd, hd->buf, hd->bsize);
        hton_st(&chd, 2);
        chd.xsize = htons((sWord)hd->xsize);
        chd.ysize = htons((sWord)hd->ysize);
        for(i=0; i<lsize; i++) ptr[i] = hd->grptr[i];
    }
    else { // ANY DATA -> CT_DATA 
        chd.xsize   = htons((sWord)hd->xsize);
        chd.ysize   = htons((sWord)hd->ysize);
        chd.ctmin   = 0;
        chd.ctmax   = 0;
        chd.cutup   = 0;
        chd.cutdown = 0;
        chd.cutleft = 0;
        chd.cutright= 0;

        k = l = 0;
        if (hd->depth < dbyte*8){   
            for (i=0; i<hd->xsize*hd->ysize; i++) {
                for (j=0; j<hd->depth/8; j++) ptr[k++] = null;
                for (j=hd->depth/8; j<dbyte; j++) 
                ptr[k++] = hd->grptr[l++];
            }
        }
        else if (hd->depth == dbyte*8) {
            for (i=0; i<hd->xsize*hd->ysize; i++) {
                for (j=0; j<dbyte; j++) ptr[k++] = hd->grptr[l++];
            }
        }
        else {
            for (i=0; i<hd->xsize*hd->ysize; i++) {
                for (j=0; j<dbyte; j++) ptr[k++] = hd->grptr[l++];
                l += (hd->depth)/8 - dbyte;
            }
        }
     
    }

    if (RZxy!=1.0 && RZxy<RZXY_RATE && chk_RZxy()) {
        chd.anydata[0] = htons((sWord)(RZxy*RZXY_RATE));
        chd.anydata[1] = htons((sWord)RZXY_RATE);
        hd->kind |= HAS_RZXY;
    }

    if (ZeroBase!=0) {
        chd.anydata[2] = htons((sWord)ZeroBase);
        hd->kind |= HAS_BASE;
    }

    fseek(fp, 0, 0);
    if (checkBit(hd->kind, CT_3DM)) {
        cmd = *hd;
        hton_st(&cmd, 4);
        fwrite(&cmd, sizeof(CmnHead), 1, fp);
    }
    fwrite(&chd, sizeof(CTHead), 1, fp);

    wp = (sWord*)ptr;
    hton_ar(wp, lsize);
    fwrite(ptr, lsize, 1, fp);

    fclose(fp);
    free(ptr);
    
    return  0;
}



/**
int  write_cmn_file(const char* fn, CmnHead* hd)

共通ヘッダとそのデータ部をそのまま書き込む．

@param  fn  書き込みを行うファイル名．
@param  hd  データを記述した共通ヘッダ．

@retval 0   正常終了．
@retval JBXL_GRAPH_OPFILE_ERROR ファイルオープンエラー
@retval JBXL_GRAPH_MEMORY_ERROR メモリエラー
*/
int  write_cmn_file(const char* fn, CmnHead* hd)
{
    FILE*   fp;
    CmnHead cd;

    if ((fp=fopen(fn,"wb"))==NULL) return JBXL_GRAPH_OPFILE_ERROR;

    cd = *hd;
    hton_st(&cd, 4);
    fwrite(&cd, sizeof(CmnHead), 1, fp);
    if (hd->bsize>0) fwrite(hd->buf, hd->bsize, 1, fp);
    if (hd->lsize>0) fwrite(hd->grptr, hd->lsize, 1, fp);

    return 0;
}



/**
CmnHead  read_xxx_file(const char* fn)

拡張read関数．色々なグラフィックファイルを読み込む．

サポートしているデータ形式は, 
@b COMMON, @b CT_DATA, @b CT_3DM, @b RAS_DATA, @b USERSET_DATA, @b ANY_DATA @n
ただし, ANY_DATAは hd->bufと hd->bsizeのみ有効．

@param  fn  読み込むを行うファイル名．

@return 読み込んだグラフィックデータの共通ヘッダ．
@retval HEADER_NONE @b kindメンバ: ヘッダ種別無し
@retval JBXL_GRAPH_OPFILE_ERROR @b xsizeメンバ: ファイルオープンエラー．
@retval JBXL_GRAPH_MEMORY_ERROR @b xsizeメンバ: メモリエラー．
@retval JBXL_GRAPH_HEADER_ERROR @b xsizeメンバ: ヘッダエラー．
*/
CmnHead  read_xxx_file(const char* fn)
{
    FILE*   fp;
    int  i, fsz, hsz;
    sWord*  wptr;
    CmnHead hd;
    size_t rs;
    UNUSED(rs);

    init_CmnHead(&hd);

    fsz = file_size(fn);
    if ((fp=fopen(fn,"rb"))==NULL) {
        hd.xsize = JBXL_GRAPH_OPFILE_ERROR;
        return hd;
    }

    hsz = sizeof(CmnHead);
    fseek(fp,0,0);
    rs = fread(&hd, hsz, 1, fp);
    ntoh_st(&hd, 4);

    if (hd.kind>=0 && hd.kind<=NUM_KDATA) { 
        hd.zsize = Max(hd.zsize, 1);
        if (fsz == (int)(hsz+hd.bsize+hd.lsize)) {
            fseek(fp, hsz, 0);
            hd.buf   = (uByte*)malloc(hd.bsize);
            hd.grptr = (uByte*)malloc(hd.lsize);
            if ((hd.bsize>0&&hd.buf==NULL) || hd.grptr==NULL) {
                init_CmnHead(&hd);
                hd.xsize = JBXL_GRAPH_HEADER_ERROR;
                return hd;
            }

            rs = fread(hd.buf, hd.bsize, 1, fp);
            wptr = (sWord*)hd.buf;
            ntoh_ar(wptr, hd.bsize);

            rs = fread(hd.grptr, hd.lsize, 1, fp);
            if (hd.depth==16) {
                wptr = (sWord*)hd.grptr;
                for(i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) wptr[i] = ntohs(wptr[i]);
            }
            fclose(fp);
            return hd;
        }
    }

    if (hd.kind==RAS_MAGIC) {
        hd = read_ras_data(fp); 
        fclose(fp);
        return hd;
    }
    
    hd = read_ct_data(fp);
    if (fsz != (int)(hd.lsize+hd.bsize)) {
        hd.xsize = hd.ysize = hd.depth = 0;
        hd = read_user_data(fp, &hd);
    }
    fclose(fp);

    return hd;
}



/**
CmnHead  read_cmn_header(const char* fn)

共通ヘッダの読み込み．データ部の読み込みは行わない．

@param  fn  読み込みを行うファイル名．

@return 読み込んだグラフィックデータの共通ヘッダ．
@retval HEADER_NONE @b kindメンバ: ヘッダ種別無し
@retval JBXL_GRAPH_OPFILE_ERROR @b xsizeメンバ: ファイルオープンエラー．
@retval JBXL_GRAPH_MEMORY_ERROR @b xsizeメンバ: メモリエラー．
*/
CmnHead  read_cmn_header(const char* fn)
{
    FILE*   fp;
    CmnHead hd;
    size_t rs;
    UNUSED(rs);

    init_CmnHead(&hd);

    if ((fp=fopen(fn,"rb"))==NULL) {
        hd.xsize = JBXL_GRAPH_OPFILE_ERROR;
        return hd;
    }

    rs = fread(&hd, sizeof(CmnHead), 1, fp);
    fclose(fp);
    ntoh_st(&hd, 4);

    if (hd.bsize>0) {
        hd.buf = (uByte*)malloc(hd.bsize);
        if (hd.buf==NULL) {
            init_CmnHead(&hd);
            hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
            return hd;
        }
        rs = fread(hd.buf, hd.bsize, 1, fp);
    }
    else  {
        hd.bsize = 0;
        hd.buf = NULL;
    }

    return hd;
}



/**
CmnHead  read_cmn_file(const char* fn)

共通ヘッダを持ったファイルの読み込み．

@param  fn  読み込みを行うファイル名．

@return 読み込んだグラフィックデータの共通ヘッダ．
@retval HEADER_NONE @b kindメンバ: ヘッダ種別無し
@retval JBXL_GRAPH_OPFILE_ERROR @b xsizeメンバ: ファイルオープンエラー．
@retval JBXL_GRAPH_MEMORY_ERROR @b xsizeメンバ: メモリエラー．
*/
CmnHead  read_cmn_file(const char* fn)
{
    FILE*   fp;
    CmnHead hd;
    size_t rs;
    UNUSED(rs);

    init_CmnHead(&hd);

    if ((fp=fopen(fn,"rb"))==NULL) {
        hd.xsize = JBXL_GRAPH_OPFILE_ERROR;
        return hd;
    }
    
    rs = fread(&hd, sizeof(CmnHead), 1, fp);
    fclose(fp);
    ntoh_st(&hd, 4);

    if (hd.bsize>0) {
        hd.buf = (uByte*)malloc(hd.bsize);
        if (hd.buf==NULL) {
            init_CmnHead(&hd);
            hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
            return hd;
        }
        rs = fread(hd.buf, hd.bsize, 1, fp);
    }
    else  {
        hd.bsize = 0;
        hd.buf = NULL;
    }

    if (hd.lsize<=0) hd.lsize = file_size(fn)-sizeof(CmnHead)-hd.bsize;
    hd.grptr = (uByte*)malloc(hd.lsize);
    if (hd.grptr==NULL) {
        free_CmnHead(&hd);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }
    rs = fread(hd.grptr, hd.lsize, 1, fp);

    return hd;
}




///////////////////////////////////////////////////////////////////////////////
// ファイル識別子と共通ヘッダによるI/Oサポート

/**
CmnHead  read_user_data(FILE* fp, CmnHead* chd)

オペレータ指定のヘッダ形式にしたがってグラフィックファイルを読み込む．

ヘッダ形式のチェック（ファイルサイズの整合性など）は行わないので，
それらは呼び出し側の責任となる．@n
リトルエンディアン対応．

@param  fp   読み込むを行うファイルのファイル識別子．
@param  chd  読み込むファイルのヘッダ情報を入れた CmnHead.  kind, bsize, lsize 必須． 

@return 読み込んだグラフィックデータの共通ヘッダ．
@retval HEADER_NONE @b kindメンバ: ヘッダ種別無し
@retval JBXL_GRAPH_HEADER_ERROR @b xsizeメンバ: オペレータ指定のヘッダがない．
@retval JBXL_GRAPH_MEMORY_ERROR @b xsizeメンバ: メモリエラー．
@retval JBXL_GRAPH_CANCEL @b xsizeメンバ: キャンセル．

@bug 24ビット以上のエンディアン処理は未実装
*/
CmnHead  read_user_data(FILE* fp, CmnHead* chd)
{
    int  i;
    CmnHead hd;
    size_t rs;
    UNUSED(rs);

    init_CmnHead(&hd);

    if (chd==NULL) {
        hd.xsize = JBXL_GRAPH_HEADER_ERROR;
        return hd;
    }
    else if (!checkBit(chd->kind, USERSET_DATA)) {
        hd.xsize = JBXL_GRAPH_HEADER_ERROR;
        return hd;
    }
    hd = *chd;

    // ヘッダ読み込み
    fseek(fp, 0, 0);
    hd.buf = (uByte*)malloc(hd.bsize);
    if (hd.buf==NULL) {
        init_CmnHead(&hd);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }
    rs = fread((void*)hd.buf, hd.bsize, 1, fp);

    hd.grptr = (uByte*)malloc(hd.lsize);
    if (hd.grptr==NULL) {
        free_CmnHead(&hd);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }

    fseek(fp, hd.bsize, 0);
    rs = fread(hd.grptr, hd.lsize, 1, fp);
    //int psize = hd.xsize*hd.ysize*((hd.depth+7)/8);
    //for (int i=0; i<hd.zsize; i++) {
    //    rs = fread(hd.grptr+i*psize, psize, 1, fp);
    //}

    if (is_little_endian()) {
        if (hd.depth==16 && !checkBit(chd->kind, HAS_LENDIAN)) {
            sWord* wptr = (sWord*)hd.grptr;
            for (i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) wptr[i] = ntohs(wptr[i]);
        }
    }
    else {
        if (hd.depth==16 && checkBit(chd->kind, HAS_LENDIAN)) {
            uWord* wptr = (uWord*)hd.grptr;
            for (i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) wptr[i] = swaps(wptr[i]);
        }
    }

    return hd;
}



/**
CmnHead  read_ras_data(FILE* fp)

SUN RASTER形式のグラフィックファイルを読み込む．

@param  fp  読み込むを行うファイルのファイル識別子．

@return 読み込んだグラフィックデータの共通ヘッダ．
@retval HEADER_NONE @b kindメンバ: ヘッダ種別無し
@retval JBXL_GRAPH_MEMORY_ERROR @b xsizeメンバ: メモリエラー．
*/
CmnHead  read_ras_data(FILE* fp)
{
    RasHead rhd;
    CmnHead  hd;
    int  i, linebyte, databyte;
    uByte null[2], *buf;
    sWord* wptr;
    size_t rs;
    UNUSED(rs);

    init_CmnHead(&hd);
    hd.kind  = RAS_DATA;
    hd.bsize = 32;

    fseek(fp, 0, 0);
    rs = fread(&rhd, hd.bsize, 1, fp);
    ntoh_st(&rhd, 4);

    hd.xsize = rhd.ras_width;
    hd.ysize = rhd.ras_height;
    hd.zsize = 1;
    hd.depth = rhd.ras_depth;
    hd.lsize = hd.xsize*hd.ysize*((hd.depth+7)/8);
    hd.buf   = (uByte*)malloc(hd.bsize);
    if (hd.buf==NULL) {
        init_CmnHead(&hd);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }
    memcpy(hd.buf, (uByte*)&rhd, hd.bsize);

    hd.grptr = (uByte*)malloc(hd.lsize);
    if (hd.grptr==NULL) {
        free_CmnHead(&hd);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }
    buf = (uByte*)hd.grptr;

    fseek(fp, rhd.ras_maplength, 1); 

    databyte = hd.xsize*((hd.depth+7)/8);
    linebyte = rhd.ras_length/hd.ysize;

    if (databyte==linebyte) {
        rs = fread(buf, hd.lsize, 1, fp);
    }
    else {
        for (i=0; i<hd.ysize; i++) {
            rs = fread(buf, databyte, 1, fp);
            rs = fread(null, linebyte-databyte, 1, fp);
            buf += databyte;
        }
    }

    if (hd.depth==16){
        wptr = (sWord*)hd.grptr;
        for(i=0; i<hd.xsize*hd.ysize; i++) wptr[i] = ntohs(wptr[i]);
    }

    return hd;
}



/**
CmnHead  read_ct_data(FILE* fp)

CT_DATA形式のグラフィックファイルを読み込む．

@param  fp  読み込むを行うファイルのファイル識別子．

@return 読み込んだグラフィックデータの共通ヘッダ．
@retval HEADER_NONE @b kindメンバ: ヘッダ種別無し
@retval JBXL_GRAPH_MEMORY_ERROR @b xsizeメンバ: メモリエラー．
*/
CmnHead  read_ct_data(FILE* fp)
{
    int i;
    CTHead chd;
    CmnHead hd;
    sWord *wptr;
    size_t rs;
    UNUSED(rs);

    init_CmnHead(&hd);

    // ヘッダ読み込み
    fseek(fp, 0, 0);
    hd.bsize = 64;
    rs = fread((void*)&chd, hd.bsize, 1, fp);
    ntoh_st(&chd, 2);

    hd.xsize = chd.xsize - chd.cutleft - chd.cutright;
    hd.ysize = chd.ysize - chd.cutup   - chd.cutdown;
    hd.zsize = 1;
    hd.depth = 16;
    hd.lsize = hd.xsize*hd.ysize*(hd.depth/8);

    hd.buf   = (uByte*)malloc(hd.bsize);
    if (hd.buf==NULL) {
        init_CmnHead(&hd);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }
    hd.kind  = CT_DATA;
    memcpy(hd.buf, &chd, hd.bsize);

    hd.grptr = (uByte*)malloc(hd.lsize);
    if (hd.grptr==NULL) {
        free_CmnHead(&hd);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }

    fseek(fp, hd.bsize, 0);
    rs = fread(hd.grptr, hd.lsize, 1, fp);

    wptr = (sWord*)hd.grptr;
    for (i=0; i<hd.xsize*hd.ysize; i++) wptr[i] = ntohs(wptr[i]);

    return hd;
}




///////////////////////////////////////////////////////////////////////////////
// DICOM

/**
int  dicom_header(FILE* fp, int fsize, int* dsize, int* xsize, int* ysize, int* depth, double* rzxy)

DICOM型式のファイルからヘッダを読みだす．

@param       fp     DICOM型式ファイルへのファイル識別子
@param       fsize  ファイルのサイズ．
@param[out]  dsize  グラフィックデータの大きさが格納される．
@param[out]  xsize  Xサイズが格納される．
@param[out]  ysize  Yサイズが格納される．
@param[out]  depth  デプス(bit)が格納される．
@param[out]  rzxy   Z軸の歪が格納される．0.0 なら不明．

@retval 1以上       読みこんだヘッダの大きさ (Byte単位)．
@retval 0           ヘッダの区切りを識別できなかった．
@retval JBXL_GRAPH_INDARG_ERROR  不正な引数 (fp, fsize, dsize)
@retval JBXL_GRAPH_MEMORY_ERROR  メモリ不足
*/
int  dicom_header(FILE* fp, int fsize, int* dsize, int* xsize, int* ysize, int* depth, double* rzxy)
{
    int    i, j, sz, rp, ef;
    int    ln, hsize;
    int    xs = 0, ys = 0, dp = 0;
    double rz = 0.0;

    uWord  dt, cn[2];
    uWord* wp;

    if (fp==NULL || fsize<=0 || dsize==NULL) return JBXL_GRAPH_IVDARG_ERROR;

    *dsize = 0;
    if (rzxy!=NULL) *rzxy = 0.0;
    ef = OFF;
    fseek(fp, 0, 0);

    /////////////////////////////////////////////////////////////////////
    // ヘッダの区切りを検出
    rp = (int)fread(&dt, sizeof(uWord), 1, fp);
    while(!feof(fp) && ef!=ON) {
        //if ((uWord)ntohs(dt)==0xe07f) {
        if (dt==DICOM_PIXEL_GROUP) {
            rp += (int)fread(&dt, sizeof(uWord), 1, fp);
            //if ((uWord)ntohs(dt)==0x1000) {
            if (dt==DICOM_PIXEL_ELEMENT) {
                rp += (int)fread(&cn, sizeof(uWord), 2, fp);
                //if ((uWord)ntohs(cn[0])==0x4f57 && cn[1]==0x0000) {
                if (cn[0]==DICOM_PIXCEL_VR && cn[1]==0x0000) {
                    rp += (int)fread(&cn, sizeof(uWord), 2, fp);
                    *dsize = *(int*)cn;
                    hsize = rp*sizeof(uWord);
                    if (fsize>=*dsize+hsize) {      // footer
                        ef = ON;
                        break;
                    }
                }
                else {
                    *dsize = *(int*)cn;
                    hsize = rp*sizeof(uWord);
                    if (fsize>=*dsize+hsize) {      // footer
                        ef = ON;
                        break;
                    }
                }
            }
        }
        rp += (int)fread(&dt, sizeof(uWord), 1, fp);
    }

    if (!ef) return 0;

    sz = rp*sizeof(uWord);
    wp = (uWord*)malloc(sz);
    if (wp==NULL) return JBXL_GRAPH_MEMORY_ERROR;

    /////////////////////////////////////////////////////////////////////
    // ヘッダ読み込み
    size_t rs;
    UNUSED(rs);

    fseek(fp, 0, 0);
    rs = fread(wp, sz, 1, fp);

    for (i=0; i<sz/2-3; i++) {
        //if ((uWord)ntohs(wp[i])==0x2800 && (uWord)ntohs(wp[i+1])==0x3000) {
        if (wp[i]==DICOM_IMAGE_GROUP) {

            // Z方向の歪
            if (wp[i+1]==DICOM_PXLSPC_ELEMENT) {
                char   rx[LNAME], ry[LNAME];
                uByte* bp;

                memset(rx, 0, LNAME);
                memset(ry, 0, LNAME);

                if (wp[i+2]==DICOM_STR_VR) ln = wp[i+3];
                else                       ln = *(int*)&wp[i+2];

                if (ln<LNAME-1) {
                    bp = (uByte*)&wp[i+4];
                    j  = 0;
                    while (j<ln-1 && bp[j]!='\\') {
                        rx[j] = bp[j];
                        j++;
                    }
                    ln -= j + 1;
                    bp += j + 1;
                    j   = 0;
                    while (j<ln-1 && bp[j]!=' ') {
                        ry[j] = bp[j];
                        j++;
                    }
                    if (!strcmp(rx, ry)) {
                        rz = atof(rx);
                    } 
                }
            }

            // X サイズ
            else if (wp[i+1]==DICOM_XSIZE_ELEMENT) {
                if (wp[i+2]==DICOM_INT_VR) ln = wp[i+3];
                else                       ln = *(int*)&wp[i+2];
                if (ln==2) {
                    xs = (int)*(sWord*)&wp[i+4];
                }
                else if (ln==4) {
                    xs = *(int*)&wp[i+4];
                }
            }

            // Y サイズ
            else if (wp[i+1]==DICOM_YSIZE_ELEMENT) {
                if (wp[i+2]==DICOM_INT_VR) ln = wp[i+3];
                else                       ln = *(int*)&wp[i+2];
                if (ln==2) {
                    ys = (int)*(sWord*)&wp[i+4];
                }
                else if (ln==4) {
                    ys = *(int*)&wp[i+4];
                }
            }

            // Depth
            else if (wp[i+1]==DICOM_DEPTH_ELEMENT) {
                if (wp[i+2]==DICOM_INT_VR) ln = wp[i+3];
                else                       ln = *(int*)&wp[i+2];
                if (ln==2) {
                    dp = (int)*(sWord*)&wp[i+4];
                }
                else if (ln==4) {
                    dp = *(int*)&wp[i+4];
                }
            }
        }
    }
    free(wp);
    
    if (rzxy!=NULL)  *rzxy  = rz;
    if (xsize!=NULL) *xsize = xs;
    if (ysize!=NULL) *ysize = ys;
    if (depth!=NULL) *depth = dp;
    
    return sz;
}



/**
WSGraph  read_dicom_file(const char* fn)

*/
WSGraph  read_dicom_file(const char* fn)
{
    int  sz, fsize, dsize, xsize, ysize, depth;
    double rzxy;
    FILE*  fp;
    size_t rs;
    UNUSED(rs);

    WSGraph vp;

    memset(&vp, 0, sizeof(WSGraph));

    fsize = file_size(fn);
    if (fsize<=0) {
        vp.state = JBXL_GRAPH_NOFILE_ERROR;
        return vp;
    }

    if ((fp=fopen(fn,"rb"))==NULL) {
        vp.state = JBXL_GRAPH_OPFILE_ERROR;
        return vp;
    }

    sz = dicom_header(fp, fsize, &dsize, &xsize, &ysize, &depth, &rzxy);
    if (sz<=0) {
        fclose(fp);
        vp.state = JBXL_GRAPH_HEADER_ERROR;
        return vp;
    }

    DEBUG_MODE PRINT_MESG("read_dicom_file: xsize = %d, ysize = %d, depth = %d, RZxy = %f\n", xsize, ysize, depth, rzxy);

    // Read Data
    if (dsize!=xsize*ysize*((depth+7)/8)) {
        unset_RZxy();
        fclose(fp);
        vp.state = JBXL_GRAPH_HEADER_ERROR;
        return vp;
    }

    if ((depth+7)/8!=2) {
        unset_RZxy();
        fclose(fp);
        vp.state = JBXL_GRAPH_HEADER_ERROR;
        return vp;
    }

    vp = make_WSGraph(xsize, ysize, 1);
    if (vp.gp==NULL) {
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    fseek(fp, sz, 0);
    rs = fread(vp.gp, dsize, 1, fp);
    fclose(fp);

    if (rzxy>0.0) set_RZxy(rzxy);   
    vp.state = JBXL_NORMAL;
    return vp;
}


