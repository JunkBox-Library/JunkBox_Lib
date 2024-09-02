/**
@brief    グラフィック用（含CT画像）ファイル入出力関数  
@file     Gio.cpp
@author   Fumi.Iseki (C)
*/

#include "tools++.h"
#include "Gio.h"
#include "JpegTool.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ファイル識別子と共通ヘッダによるI/Oサポート

/**
CmnHead  jbxl::readRasData(FILE* fp)

SUN RASTER形式のグラフィックファイルを読み込む．

@param  fp  読み込むを行うファイルのファイル識別子．

@return 読み込んだグラフィックデータの共通ヘッダ．エラー場合 kindメンバは HEADER_NONEとなる．
@retval JBXL_GRAPH_MEMORY_ERROR @b xsize メモリエラー．

@bug 24ビット以上のエンディアン処理は未実装
*/
CmnHead  jbxl::readRasData(FILE* fp)
{
    RasHead rhd;
    CmnHead  hd;
    int  i, linebyte, databyte;
    uByte null[2], *buf;
    sWord* wptr;

    hd.kind  = RAS_DATA;
    hd.bsize = 32;
    hd.buf   = NULL;
    hd.grptr = NULL;

    fseek(fp, 0, 0);
    size_t ret = fread(&rhd, hd.bsize, 1, fp);
    ntoh_st(&rhd, 4);
    if (ret==0) {
        hd.xsize = JBXL_FILE_READ_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }

    hd.xsize = rhd.ras_width;
    hd.ysize = rhd.ras_height;
    hd.zsize = 1;
    hd.depth = rhd.ras_depth;
    hd.lsize = hd.xsize*hd.ysize*((hd.depth+7)/8);
    hd.buf   = (uByte*)malloc(hd.bsize);
    if (hd.buf==NULL) {
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }
    memcpy(hd.buf, (uByte*)&rhd, hd.bsize);

    hd.grptr = (uByte*)malloc(hd.lsize);
    if (hd.grptr==NULL) {
        freeNull(hd.buf);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }
    memset(hd.grptr, 0, hd.lsize);
    buf = (uByte*)hd.grptr;

    fseek(fp, rhd.ras_maplength, 1); 
    databyte = hd.xsize*((hd.depth+7)/8);
    linebyte = rhd.ras_length/hd.ysize;

    if (databyte==linebyte) {
        ret = fread(buf, hd.lsize, 1, fp);
        if (ret==0) {
            hd.xsize = JBXL_FILE_READ_ERROR;
            hd.kind  = HEADER_NONE;
            return hd;
        }
    }
    else {
        for (i=0; i<hd.ysize; i++) {
            ret = fread(buf, databyte, 1, fp);
            if (ret>0) ret = fread(null, linebyte - databyte, 1, fp);
            if (ret==0) {
                hd.xsize = JBXL_FILE_READ_ERROR;
                hd.kind  = HEADER_NONE;
                return hd;
            }
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
int  jbxl::writeRasData(FILE* fp, CmnHead* hd, int obit)

色々な画像データをSUN RASTER型式でファイルに保存する．

@param  fp   保存するファイルポインタ．
@param  hd   保存するデータを既述した共通ヘッダ．
@param  obit SUN RASTERの 1ピクセルのビット数(8, 24をサポート)．

@retval >0    書き込んだバイト数．
@retval JBXL_GRAPH_MEMORY_ERROR   メモリエラー．
*/
int  jbxl::writeRasData(FILE* fp, CmnHead* hd, int obit)
{
    RasHead shd;
    int  i, j, k, l, linebyte, databyte, depth, lsize;
    uByte null=0x00, *ptr, *buf;

    obit = Xabs(obit);
    if (hd->kind == RAS_DATA) {
        unsigned int u;
        memcpy((sByte*)&shd, hd->buf, hd->bsize);
        ptr = (uByte*)malloc(hd->lsize);
        if (ptr==NULL) return JBXL_GRAPH_MEMORY_ERROR;
        for (u=0; u<hd->lsize; u++) ptr[u] = hd->grptr[u];
        lsize = hd->lsize;
    }
    else { // CT -> RAS 
        if (hd->depth==16) {

            if (obit==8) depth = 8;
            else         depth = 24;

            lsize = hd->xsize*hd->ysize*depth/8;
            buf = (uByte*)malloc(lsize);
            if (buf==NULL) return JBXL_GRAPH_MEMORY_ERROR;
            memset(buf, 0, lsize);

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
            return -2;
        }
        memset(ptr, 0, lsize);

        k = l = 0;
        for (i=0 ; i<hd->ysize; i++) {
            for (j=0; j<databyte; j++)          ptr[k++] = buf[l++];
            for (j=0; j<linebyte-databyte; j++) ptr[k++] = null;
        }

        if (hd->depth==16) free(buf);
    }

    hton_st(&shd, 4);
    fwrite(&shd, sizeof(RasHead), 1, fp);
    fwrite(ptr, lsize, 1, fp);

    free(ptr);
    return  sizeof(RasHead)+lsize;
}


/**
CmnHead  jbxl::readUserSetData(FILE* fp, CmnHead* chd, bool cnt=false);

オペレータ指定のヘッダ形式にしたがってグラフィックファイルを読み込む．

ヘッダ形式のチェック（ファイルサイズの整合性など）は行わないので，それらは呼び出し側の責任となる．@n
リトルエンディアン対応．カウンタ制御の場合は遅くなる．

@param     fp   読み込むを行うファイルのファイル識別子．
@param[in] chd  読み込むファイルのヘッダ情報を入れた CmnHead.@n 
                カウンタを使用する場合：  kind, xsize, ysize, zsize, bsize 必須．@n
                カウンタを使用しない場合：kind, bsize, lseize 必須．
@param cnt      読み込み状況カウンタを使用するかどうか．@b true: 使用する．

@return 読み込んだグラフィックデータの共通ヘッダ．@n 
        エラー場合 kindメンバは HEADER_NONEとなる．またエラーの種類により xsizeが変化する．
@retval JBXL_GRAPH_HEADER_ERROR  @b xsize オペレータ指定のヘッダがない．
@retval JBXL_GRAPH_MEMORY_ERROR  @b xsize メモリエラー．
@retval JBXL_GRAPH_CANCEL  @b xsize キャンセル．

@bug 24ビット以上のエンディアン処理は未実装
*/
CmnHead  jbxl::readUserSetData(FILE* fp, CmnHead* chd, bool cnt)
{
    CmnHead hd;
    init_CmnHead(&hd);
    CVCounter* counter = NULL;

    if (chd==NULL) {
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }
    else if ((chd->kind&0x0ff)!=USERSET_DATA) {
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }
    hd = *chd;

    // ヘッダ読み込み
    fseek(fp, 0, 0);
    hd.buf = (uByte*)malloc(hd.bsize);
    if (hd.buf==NULL) {
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }
    memset(hd.buf, 0, hd.bsize);
    size_t ret = fread((void*)hd.buf, hd.bsize, 1, fp);
    if (ret==0) {
        hd.xsize = JBXL_FILE_READ_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    } 

    // カウンタ
    if (hd.zsize>=10 && cnt) {
        counter = GetUsableGlobalCounter();
        if (counter!=NULL) {
            //counter->SetTitle("ユーザ指定ファイル読み込み中");
            counter->SetMax((hd.zsize+1)/10);
            counter->SetPos(1);
        }
    }

    hd.grptr = (uByte*)malloc(hd.lsize);
    if (hd.grptr==NULL) {
        freeNull(hd.buf);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }
    memset(hd.grptr, 0, hd.lsize);

    fseek(fp, hd.bsize, 0);

    if (counter==NULL) {
        ret = fread(hd.grptr, hd.lsize, 1, fp);
        if (ret==0) {
            hd.xsize = JBXL_FILE_READ_ERROR;
            hd.kind  = HEADER_NONE;
            return hd;
        }
    }
    else {
        int psize = hd.xsize*hd.ysize*((hd.depth+7)/8);
        for (int i=0; i<hd.zsize; i++) {
            ret = fread(hd.grptr+i*psize, psize, 1, fp);
            if (ret==0) {
                hd.xsize = JBXL_FILE_READ_ERROR;
                hd.kind  = HEADER_NONE;
                return hd;
            }
            if (i%10==0) {
                counter->StepIt();
                if (counter->isCanceled()) {    // キャンセル
                    free_CmnHead(&hd);
                    hd.xsize = JBXL_GRAPH_CANCEL;
                    hd.kind  = HEADER_NONE;
                    return hd;
                }
            }
        }
    }

    if (hd.depth==16) {
        if (checkBit(chd->kind, HAS_LENDIAN) && is_little_endian()) {
            // CPU とファイルが同じ Little Endianなので，何もしない．
        }
        else {
            sWord* wptr = (sWord*)hd.grptr;
            for (int i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) wptr[i] = ntohs(wptr[i]);
        }
    }

    if (counter!=NULL) counter->PutFill();
    return hd;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTファイルサポート
//

/**
CmnHead  jbxl::readMoonFile(const char* fn, bool no_ntoh)

MOON_DATA形式（16bit）のグラフィックファイルを読み込む．

@param  fn       読み込むを行うファイル名．
@param  no_ntoh  trueの場合，エンディアンの変換を行わない．

@return 読み込んだグラフィックデータの共通ヘッダ．@n
        エラー場合 kindメンバは HEADER_NONEとなる．またエラーの種類により xsizeが変化する．

@retval JBXL_GRAPH_MEMORY_ERROR @b xsize メモリエラー．
*/
CmnHead  jbxl::readMoonFile(const char* fn, bool no_ntoh)
{
    CmnHead hd;
    FILE* fp;
    unsigned int fsz;

    memset(&hd, 0, sizeof(CmnHead));
    hd.kind  = HEADER_NONE;

    fsz = file_size(fn);
    if (fsz==0) {
        hd.xsize = JBXL_GRAPH_NOFILE_ERROR;
        return hd;
    }
    if ((fp=fopen(fn,"rb"))==NULL) {
        hd.xsize = JBXL_GRAPH_OPFILE_ERROR;
        return hd;
    }

    hd = readMoonData(fp, fsz, no_ntoh);
    fclose(fp);
    return hd;
}


/**
CmnHead  jbxl::readMoonData(FILE* fp, unsigned int fsz, bool no_ntoh)

MOON_DATA形式（16bit）のグラフィックファイルを読み込む．

@param  fp       読み込むを行うファイルのファイル識別子．
@param  fsz      ファイルのサイズ（チェックに使用, 0以下ならチェックしない）
@param  no_ntoh  @b trueの場合，エンディアンの変換を行わない．

@return 読み込んだグラフィックデータの共通ヘッダ． @n
        エラー場合 kindメンバは HEADER_NONEとなる．またエラーの種類により xsizeが変化する．
@retval -2 @b xsize メモリエラー．
*/
CmnHead  jbxl::readMoonData(FILE* fp, unsigned int fsz, bool no_ntoh)
{
    int i;
    size_t ret;
    CTHead chd;
    CmnHead hd;

    hd.kind  = HEADER_NONE;
    hd.bsize = 64;      // Moon形式のヘッダサイズ

    // ヘッダ読み込み
    fseek(fp, 0, 0);
    ret = fread((void*)&chd, hd.bsize, 1, fp);
    ntoh_st(&chd, 2);
    if (ret==0) {
        hd.xsize = JBXL_FILE_READ_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }

    hd.xsize = chd.xsize - chd.cutleft - chd.cutright;
    hd.ysize = chd.ysize - chd.cutup   - chd.cutdown;
    hd.zsize = 1;
    hd.depth = 16;
    hd.lsize = hd.xsize*hd.ysize*(hd.depth/8);
    hd.buf   = NULL;
    hd.grptr = NULL;
    if (fsz>0 && fsz!=hd.bsize+hd.lsize) return hd;
    
    hd.buf   = (uByte*)malloc(hd.bsize);
    if (hd.buf==NULL) {
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }
    memcpy(hd.buf, &chd, hd.bsize);
    hd.kind  = MOON_DATA;

    hd.grptr = (uByte*)malloc(hd.lsize);
    if (hd.grptr==NULL) {
        freeNull(hd.buf);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }
    memset(hd.grptr, 0, hd.lsize);

    fseek(fp, hd.bsize, 0);
    ret = fread(hd.grptr, hd.lsize, 1, fp);
    if (ret==0) {
        hd.xsize = JBXL_FILE_READ_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }
    if (!no_ntoh) {
        sWord* wptr = (sWord*)hd.grptr;
        for (i=0; i<hd.xsize*hd.ysize; i++) wptr[i] = ntohs(wptr[i]);
    }
    return hd;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DICOM

/**
int  jbxl::dicomHeader(FILE* fp, int fsize, int* dsize, int* xsize, int* ysize, int* depth, double* rzxy)
 
DICOM型式のファイルからヘッダを読みだす．

@param      fp     DICOM型式ファイルへのファイル識別子
@param      fsize  ファイルのサイズ．
@param[out] dsize  グラフィックデータの大きさが格納される．
@param[out] xsize  グラフィックデータの Xサイズが格納される．
@param[out] ysize  グラフィックデータの Yサイズが格納される．
@param[out] depth  グラフィックデータのデプス(Bit)が格納される．
@param[out] rzxy   Z軸の歪．0.0 なら不明．

@retval >0  読みこんだヘッダの大きさ (Byte単位)．
@retval 0   ヘッダの区切りを識別できなかった．
@retval JBXL_GRAPH_INVARG_ERROR  不正な引数 (fp, fsize, dsize)
@retval JBXL_GRAPH_MEMORY_ERROR  メモリ不足
*/
int  jbxl::dicomHeader(FILE* fp, int fsize, int* dsize, int* xsize, int* ysize, int* depth, double* rzxy)
{
    int   i, j, sz, rp, ef;
    int   ln, hsize;
    int   xs = 0, ys = 0, dp = 0;
    double rz = 0.0;

#ifdef WIN32
    uWord  cn[2];
#else
    uWord  cn[2] __attribute__((may_alias));
#endif
    uWord  dt;
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
                }
                //*dsize = *((int*)cn);
                int* sizep = (int*)cn;
                *dsize = *sizep;
                hsize = rp*sizeof(uWord);
                if (fsize>=*dsize+hsize) {      // footer
                    ef = ON;
                    break;
                }
            }
        }
        rp += (int)fread(&dt, sizeof(uWord), 1, fp);
    }

    if (!ef) return 0;

    sz = rp*sizeof(uWord);
    wp = (uWord*)malloc(sz);
    if (wp==NULL) return JBXL_GRAPH_MEMORY_ERROR;
    memset(wp, 0, sz);

    /////////////////////////////////////////////////////////////////////
    // ヘッダ読み込み
    fseek(fp, 0, 0);
    size_t ret = fread(wp, sz, 1, fp);
    if (ret==0) {
        return JBXL_FILE_READ_ERROR;
    }

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
MSGraph<sWord>   jbxl::readDicomFile(const char* fn)

DICOM形式のデータを読み込む．

@attention
現在はまだ限定的な機能しか持たない．

@bug depth = 16bit 固定
*/
MSGraph<sWord>   jbxl::readDicomFile(const char* fn)
{
    int fsize;
    FILE*  fp;
    MSGraph<sWord> vp;

    memset(&vp, 0, sizeof(MSGraph<sWord>));

    fsize = file_size(fn);
    if (fsize<=0) {
        vp.xs = JBXL_GRAPH_NOFILE_ERROR;
        return vp;
    }

    if ((fp=fopen(fn,"r"))==NULL) {
        vp.xs = JBXL_GRAPH_OPFILE_ERROR;
        return vp;
    }

    vp = readDicomData(fp, fsize);

    fclose(fp);
    return vp;
}


MSGraph<sWord>   jbxl::readDicomData(FILE* fp, int fsize)
{
    int   sz, dsize;
    int   xsize, ysize, depth;
    double rzxy;
    MSGraph<sWord> vp;

    memset(&vp, 0, sizeof(MSGraph<sWord>));

    sz = dicomHeader(fp, fsize, &dsize, &xsize, &ysize, &depth, &rzxy);
    if (sz<=0) {
        vp.xs = JBXL_GRAPH_HEADER_ERROR;
        return vp;
    }

    if (dsize!=xsize*ysize*((depth+7)/8)) {
        vp.xs = JBXL_GRAPH_HEADER_ERROR;
        return vp;
    }

    if ((depth+7)/8!=2) {
        vp.xs = JBXL_GRAPH_HEADER_ERROR;
        return vp;
    }

    vp.set(xsize, ysize, 1, 0, 0, rzxy);

    fseek(fp, sz, 0);
    size_t ret = fread(vp.gp, dsize, 1, fp);
    if (ret==0) vp.xs = JBXL_FILE_READ_ERROR;

    return vp;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ファイル名と共通ヘッダによるI/O統合

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ファイル名と共通ヘッダ

/**
CmnHead  jbxl::readXHead(const char* fn, CmnHead* chd)

ファイルの Commonヘッダ情報を読み込む．データ本体は読み込まない．
ファイル種別の識別に使用する．

@param  fn       ヘッダの読み込みを行うファイル名．
@param[out] chd  ヘッダ情報を格納する．

@retval 読み込んだグラフィックデータを記述する共通ヘッダ（データ本体は無し）．@n
        エラーなら kind=HEADER_ERROR となる．
*/
CmnHead  jbxl::readXHead(const char* fn, CmnHead* chd)
{
    FILE    *fp;
    int    fsz, csz;
    CmnHead hd;

    memset(&hd, 0, sizeof(CmnHead));
    hd.kind = HEADER_NONE;

    fsz = (int)file_size(fn);
    if (fsz<=0) {
        hd.xsize = JBXL_GRAPH_NOFILE_ERROR;
        return hd;
    }

    ///////////////////////////////////////////////////////////////////////
    // オペレータ指定のデータ形式を確認（予めヘッダに情報が設定されている）
    //
    if (chd!=NULL && (chd->kind&0x00ff)==USERSET_DATA) {
        if (chd->zsize<=0) chd->zsize = 1;
        chd->lsize = chd->xsize*chd->ysize*chd->zsize*((chd->depth+7)/8);
        if (fsz==(int)(chd->bsize+chd->lsize)) {    // ファイルサイズのチェック
            return *chd;
        }
    }

    if ((fp=fopen(fn,"rb"))==NULL) {
        hd.xsize = JBXL_GRAPH_OPFILE_ERROR;
        return hd;
    }

    ///////////////////////////////////////////////////////////////////////
    // 共通ヘッダの読み込み
    //

    int hsz = sizeof(CmnHead_Entry);
    fseek(fp, 0, 0);
    size_t ret = fread(&hd.entry, hsz, 1, fp);
    hd.buf   = NULL;
    hd.grptr = NULL;
    ntoh_st(&hd, 4);
    if (ret==0) {
        hd.xsize = JBXL_FILE_READ_ERROR;
        hd.kind  = HEADER_NONE;
        fclose(fp);
        return hd;
    }

    // Sun Raster
    if (hd.kind==RAS_MAGIC) {
        hd.kind = RAS_DATA;
        DEBUG_MODE PRINT_MESG("JBXL::readXHead: Sun Raster File\n");
        hd.depth = hd.zsize;
        hd.zsize = 1;
        hd.lsize = file_size(fn) - sizeof(RasHead);
        hd.bsize = 0;
        fclose(fp);
        return hd;
    }

    // Common ヘッダ
    if (hd.kind > 0 && hd.kind <= NUM_KDATA) {
        if (fsz == (int)(hsz + hd.bsize + hd.lsize) + 4 ) hsz = 36;     // x86 file
        if (fsz == (int)(hsz + hd.bsize + hd.lsize) + 16) hsz = 48;     // x64 file
        //
        if (fsz == (int)(hsz + hd.bsize + hd.lsize)) {  
            if (hd.zsize<=0) hd.zsize = 1;
            if (hd.bsize>0) {
                hd.buf = (uByte*)malloc(hd.bsize);
                if (hd.buf==NULL) {
                    free_CmnHead(&hd);
                    hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
                    hd.kind  = HEADER_NONE;
                    fclose(fp);
                    return hd;
                }
                memset(hd.buf, 0, hd.bsize);

                fseek(fp, hsz, 0);
                ret = fread(hd.buf, hd.bsize, 1, fp);
                ntoh_ar((sWord*)hd.buf, hd.bsize); 
                hd.grptr = NULL;
                if (ret==0) {
                    hd.xsize = JBXL_FILE_READ_ERROR;
                    hd.kind  = HEADER_NONE;
                    fclose(fp);
                    return hd;
                }
            }

            DEBUG_MODE {
                PRINT_MESG("JBXL::readXHead: Common ヘッダ\n");
                PRINT_MESG("JBXL::readXHead: ヘッダ種別     kind  = %d\n", hd.kind);
                PRINT_MESG("JBXL::readXHead: ファイルサイズ fsz   = %d\n", fsz);
                PRINT_MESG("JBXL::readXHead: ヘッダサイズ   hsz   = %d\n", sizeof(CmnHead_Entry));
                PRINT_MESG("JBXL::readXHead: ヘッダバッファ bsize = %d\n", hd.bsize);
                PRINT_MESG("JBXL::readXHead: データサイズ   lsize = %d\n", hd.lsize);
                PRINT_MESG("JBXL::readXHead: サイズ         %dx%dx%d %d\n", hd.xsize, hd.ysize, hd.zsize, hd.depth);
            }
            fclose(fp);
            return hd;
        }
    }
    memset(&hd, 0, sizeof(CmnHead));

    ///////////////////////////////////////////////////////////////////////
    // CT File (Moon)
    //
    CTHead cthd;
    csz = sizeof(CTHead);
    fseek(fp, 0, 0);
    ret = fread(&cthd, csz, 1, fp);
    ntoh_st(&cthd, 2);
    if (ret==0) {
        hd.xsize = JBXL_FILE_READ_ERROR;
        hd.kind  = HEADER_NONE;
        fclose(fp);
        return hd;
    }
    hd.xsize = cthd.xsize - cthd.cutleft - cthd.cutright;
    hd.ysize = cthd.ysize - cthd.cutup   - cthd.cutdown;
    hd.zsize = 1;
    hd.depth = 16;
    hd.bsize = csz;
    hd.lsize = hd.xsize*hd.ysize*hd.zsize*((hd.depth+7)/8);

    if (fsz==(int)(hd.bsize+hd.lsize)) {
        hd.kind = MOON_DATA;
        hd.buf  = (uByte*)malloc(csz);
        if (hd.buf==NULL) {
            free_CmnHead(&hd);
            hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
            hd.kind  = HEADER_NONE;
            fclose(fp);
            return hd;
        }
        memcpy(hd.buf, &cthd, csz);

        DEBUG_MODE {
            PRINT_MESG("JBXL::readXHead: CT ファイル\n");
            PRINT_MESG("JBXL::readXHead: ヘッダ種別     kind  = %d\n", hd.kind);
            PRINT_MESG("JBXL::readXHead: ファイルサイズ fsz   = %d\n", fsz);
            PRINT_MESG("JBXL::readXHead: ヘッダサイズ   hsz   = %d\n", sizeof(CmnHead_Entry));
            PRINT_MESG("JBXL::readXHead: ヘッダバッファ bsize = %d\n", hd.bsize);
            PRINT_MESG("JBXL::readXHead: データサイズ   lsize = %d\n", hd.lsize);
            PRINT_MESG("JBXL::readXHead: サイズ         %dx%dx%d %d\n", hd.xsize, hd.ysize, hd.zsize, hd.depth);
        }
        fclose(fp);
        return hd;
    }
    memset(&hd, 0, sizeof(CmnHead));

    ///////////////////////////////////////////////////////////////////////
    // Dicom
    //
    int   ls, sz, dsize, xsize, ysize, depth;
    double rzxy;

    sz = dicomHeader(fp, fsz, &dsize, &xsize, &ysize, &depth, &rzxy);
    ls = xsize*ysize*((depth+7)/8);
    if (sz>0 && dsize==ls) {
        hd.kind  = DICOM_DATA;
        hd.xsize = xsize;
        hd.ysize = ysize;
        hd.zsize = 1;
        hd.depth = depth;
        hd.bsize = sizeof(CTHead);
        hd.lsize = ls;
        hd.buf   = (uByte*)malloc(hd.bsize);
        hd.grptr = NULL; 
                                 
        CTHead* pcthd = (CTHead*)hd.buf;
        memset(pcthd, 0, hd.bsize);
        pcthd->xsize  = hd.xsize;
        pcthd->ysize  = hd.ysize;

        if (rzxy>0.0) {
            pcthd->anydata[0] = (sWord)(rzxy*RZXY_RATE);
            pcthd->anydata[1] = RZXY_RATE;
            hd.kind |= HAS_RZXY;
        }

        fclose(fp);
        return hd;
    }

    ///////////////////////////////////////////////////////////////////////
    // Well Known ファイル
    //
    fseek(fp, 0, 0);
    Buffer buf = read_Buffer_data(fp, 32);

    // TIFF
    /*
    if (head[0]=='I' && head[1]=='I') {
        if (head[2]==0x2a && head[3]==0x00) {
            DEBUG_MODE PRINT_MESG("JBXL::readXHead: TIFFデータ形式\n");
            hd.kind = TIFF_DATA;
            fclose(fp);
            return hd;
        }
    }
    if (head[0]=='M' && head[1]=='M') {
        if (head[2]==0x00 && head[3]==0x2a) {
            DEBUG_MODE PRINT_MESG("JBXL::readXHead: TIFFデータ形式\n");
            hd.kind = TIFF_DATA;
            fclose(fp);
            return hd;
        }
    }*/

#ifdef  ENABLE_JPEGLIB
    // JPEG
    if (isJPEGHeader(buf)) {
        DEBUG_MODE PRINT_MESG("JBXL::readXHead: JPEGデータ形式\n");
        hd.kind = JPEG_RGB_DATA;    // JPEG_RGB_DATA or JPEG_MONO_DATA
        free_Buffer(&buf);
        fclose(fp);
        return hd;
    }
#endif

    free_Buffer(&buf);

    ///////////////////////////////////////////////////////////////////////
    // Another Unknown File Format
    //
    DEBUG_MODE PRINT_MESG("JBXL::readXHead: 未知のデータ形式\n");
    memset(&hd, 0, sizeof(CmnHead));
    hd.kind  = UN_KNOWN_DATA;
    hd.lsize = fsz;

    fclose(fp);
    return hd;
}


/**
CmnHead  jbxl::readXHeadFile(const char* fn, CmnHead* chd, bool cnt=false)

データ種別を自動判定して，グラフィックデータを読み込み，Common 形式に変換する．

USERSET_DATAの場合は chd を指定する．また chdの kind にはオプションの情報も入れられる（現在はエンディアンのみ）

サポートしているデータ形式は, 
- Common 形式  : Common 形式にしたがって読み込む．(CT_RGN_SLは未サポート)
- Sun Raster　 : Common 形式に変換する．ヘッダは RAS_DATA になる 
- Moon(CT)　　 : Common 形式に変換する．ヘッダは MOON_DATA になる．
- DICOM        : Common 形式に変換する．512x512xZ固定．ヘッダは DICOM_DATAになる．
- USERSET_DATA : CmnHead* chd でヘッダ情報を指定して，読み込みを行う．ヘッダは USERSET_DATAになる．
- Unkonwn 　　 : ファイルをそのまま，hd->grptrへ読み込む．ヘッダは UN_KNOWN_DATA になる．

ただし，UN_KNOWN_DATAの場合は CmnHaedの hd->kind, hd->lsize(ファイル長), hd->gptrのみ有効．

@param         fn   読み込むを行うファイル名．
@param[in,out] chd  USERSET_DATA指定の場合，ファイルのヘッダ情報を入れる．@n
                    またはファイルに関するオプション情報も指定できる（現在はエンディアンの情報のみ）．
@param         cnt  仮想カウンタを使用するか？

@return 読み込んだグラフィックデータを記述する共通ヘッダ．@n
        エラーなら kind = HEADER_NONE となる．またエラーの種類により xsizeが変化する．
@retval JBXL_GRAPH_OPFILE_ERROR  @b xsize ファイルオープンエラー．
@retval JBXL_GRAPH_MEMORY_ERROR  @b xsize メモリエラー．
@retval JBXL_GRAPH_CANCEL  @b xsize キャンセル by ユーザ

@bug Common形式の画素深度が 24,32bitの場合のエンディアン処理が未実装 
*/
CmnHead  jbxl::readXHeadFile(const char* fn, CmnHead* chd, bool cnt)
{
    FILE* fp;
    int   fsz;
    bool  no_ntoh = false;
    CmnHead hd;
    CVCounter* counter = NULL;

    memset(&hd, 0, sizeof(CmnHead));
    hd.kind = HEADER_NONE;

    fsz = (int)file_size(fn);
    if (fsz==0) {
        hd.xsize = JBXL_GRAPH_NOFILE_ERROR;
        return hd;
    }

    if ((fp=fopen(fn,"rb"))==NULL) {
        hd.xsize = JBXL_GRAPH_OPFILE_ERROR;
        return hd;
    }

    ///////////////////////////////////////////////////////////////////////
    // オペレータ指定のデータ形式を確認（予めヘッダに情報が設定されている）
    //
    if (chd!=NULL) {
        // ファイルとCPUのエンディアンが同じ
        if (checkBit(chd->kind, HAS_LENDIAN) && is_little_endian()) {
            no_ntoh = true;
        }

        // オペレータ指定のデータ形式．カウンタ未サポート
        if ((chd->kind&0x00ff)==USERSET_DATA) {
            if (chd->zsize<=0) chd->zsize = 1;
            chd->lsize = chd->xsize*chd->ysize*chd->zsize*((chd->depth+7)/8);
            if (fsz==(int)(chd->bsize+chd->lsize)) {        // ファイルサイズのチェック
                DEBUG_MODE PRINT_MESG("JBXL::readXHeadFile: オペレータ指定のデータ形式\n");
                hd = readUserSetData(fp, chd, true);        // カウンタ有効 

                // 24, 32bit 未対応
                if (hd.depth==16 && !no_ntoh) {
                    sWord* wptr = (sWord*)hd.grptr;
                    for (int i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) {
                        wptr[i] = ntohs(wptr[i]);
                    }
                }

                fclose(fp);
                return hd;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////
    // 共通ヘッダの読み込み
    //
    int hsz = sizeof(CmnHead_Entry);
    fseek(fp, 0, 0);
    size_t ret = fread(&hd.entry, hsz, 1, fp);
    hd.buf = NULL;
    hd.grptr = NULL;
    ntoh_st(&hd, 4);
    if (ret==0) {
        hd.xsize = JBXL_FILE_READ_ERROR;
        hd.kind  = HEADER_NONE;
        fclose(fp);
        return hd;
    }

    // Sun Raster
    if (hd.kind==RAS_MAGIC) {
        DEBUG_MODE PRINT_MESG("JBXL::readXHeadFile: Sun Rasterデータ形式\n");
        hd = readRasData(fp); 
        fclose(fp);
        return hd;
    }

    // Common形式 
    // データ読み取りでは hd.lsize==0 のファイルサイズ無効（CT_RGN_SL）はまだサポートされていない
    if (hd.kind>0 && hd.kind<=NUM_KDATA) {
        if (fsz == (int)(hsz + hd.bsize + hd.lsize) + 4 ) hsz = 36;     // x86 file
        if (fsz == (int)(hsz + hd.bsize + hd.lsize) + 16) hsz = 48;     // x64 file
        //
        if (fsz == (int)(hsz + hd.bsize + hd.lsize)) {
            DEBUG_MODE PRINT_MESG("JBXL::readXHeadFile: Commonデータ形式\n");
            if (hd.zsize <= 0) hd.zsize = 1;

            // カウンタ
            if (hd.zsize >= 10 && cnt) {
                counter = GetUsableGlobalCounter();
                if (counter != NULL) {
                    //counter->SetTitle("Commonファイル読み込み中");
                    counter->SetMax(hd.zsize / 10);
                    counter->SetPos(0);
                }
            }

            if (hd.bsize > 0) {
                hd.buf = (uByte*)malloc(hd.bsize);
                if (hd.buf!=NULL) memset(hd.buf, 0, hd.bsize);
            }
            hd.grptr = (uByte*)malloc(hd.lsize);
            if ((hd.bsize > 0 && hd.buf == NULL) || hd.grptr == NULL) {
                free_CmnHead(&hd);
                hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
                hd.kind  = HEADER_NONE;
                fclose(fp);
                return hd;
            }
            memset(hd.grptr, 0, hd.lsize);
            fseek(fp, hsz, 0);
            if (hd.bsize > 0) {
                size_t ret = fread(hd.buf, hd.bsize, 1, fp);
                ntoh_ar((sWord*)hd.buf, hd.bsize);
                if (ret==0) {
                    hd.xsize = JBXL_FILE_READ_ERROR;
                    hd.kind  = HEADER_NONE;
                    fclose(fp);
                    return hd;
                }
            }
        }
        
        // ヘッダのみ
        if (hd.lsize==0) {
            if (counter!=NULL) counter->PutFill();
            fclose(fp);
            return hd;
        }

        // データ本体
        if (counter==NULL) {
            size_t ret = fread(hd.grptr, hd.lsize, 1, fp);
            if (ret==0) {
                hd.xsize = JBXL_FILE_READ_ERROR;
                hd.kind  = HEADER_NONE;
                fclose(fp);
                return hd;
            }
        }
        else {
            int psize = hd.xsize*hd.ysize*((hd.depth+7)/8);
            for (int i=0; i<hd.zsize; i++) {
                size_t ret = fread(hd.grptr+i*psize, psize, 1, fp);
                if (ret==0) {
                    hd.xsize = JBXL_FILE_READ_ERROR;
                    hd.kind  = HEADER_NONE;
                    fclose(fp);
                    return hd;
                }
                if (i%10==0) {
                    counter->StepIt();
                    if (counter->isCanceled()) {    // キャンセル
                        //counter->Stop();
                        free_CmnHead(&hd);
                        hd.xsize = JBXL_GRAPH_CANCEL;
                        hd.kind  = HEADER_NONE;
                        fclose(fp);
                        return hd;
                    }
                }
            }
        }

        // 24, 32bit 未対応
        if (hd.depth==16 && !no_ntoh) {
            sWord* wptr = (sWord*)hd.grptr;
            for (int i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) {
                wptr[i] = ntohs(wptr[i]);
            }
        }

        if (counter!=NULL) counter->PutFill();
        fclose(fp);
        return hd;
    }
    memset(&hd, 0, sizeof(CmnHead));

    ///////////////////////////////////////////////////////////////////////
    // Moon形式 16bit
    //
    hd = readMoonData(fp, fsz, no_ntoh);
    if ((hd.kind & 0x00ff)==MOON_DATA) {
        DEBUG_MODE PRINT_MESG("JBXL::readXHeadFile: Moonデータ形式\n");
        fclose(fp);
        return hd;
    }
    memset(&hd, 0, sizeof(CmnHead));

    ///////////////////////////////////////////////////////////////////////
    // DICOMファイル
    //
    MSGraph<sWord> vp = readDicomData(fp, fsz);
    if (vp.gp!=NULL) {
        DEBUG_MODE PRINT_MESG("JBXL::readXHeadFile: DICOMデータ形式\n");
        hd.kind  = DICOM_DATA;
        hd.xsize = vp.xs;
        hd.ysize = vp.ys;
        hd.zsize = vp.zs;
        hd.depth = 16;
        hd.bsize = sizeof(CTHead);
        hd.lsize = vp.xs*vp.ys*vp.zs*2;
        hd.buf   = (uByte*)malloc(hd.bsize);
        hd.grptr = (uByte*)vp.gp;
                
        CTHead* pcthd = (CTHead*)hd.buf;
        memset(pcthd, 0, hd.bsize);
        pcthd->xsize  = (sWord)hd.xsize;
        pcthd->ysize  = (sWord)hd.ysize;
        //pcthd->ctmin  = 0;
        //pcthd->ctmax  = 0;
        //pcthd->cutup  = 0;
        //pcthd->cutdown  = 0;
        //pcthd->cutleft  = 0;
        //pcthd->cutright = 0;
        
        if (vp.RZxy>0.0) {
            pcthd->anydata[0] = (sWord)(vp.RZxy*RZXY_RATE);
            pcthd->anydata[1] = RZXY_RATE;
            hd.kind |= HAS_RZXY;
        }

        if (!no_ntoh) {
            sWord* wptr = (sWord*)hd.grptr;
            for (int i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) {
                wptr[i] = ntohs(wptr[i]);
            }
        }

        fclose(fp);
        return hd;
    }

    ///////////////////////////////////////////////////////////////////////
    // Well Known ファイル
    //
    fseek(fp, 0, 0);
    Buffer buf = read_Buffer_data(fp, 32);

    ///////////////////////////////////////////////////////////////////////
    // TIFF
    /*
    size_t ret = fread(head, 32, 1, fp);
    if (ret==0) {
        hd.xsize = JBXL_FILE_READ_ERROR;
        hd.kind  = HEADER_NONE;
        fclose(fp);
        return hd;
    }
    if (head[0]=='I' && head[1]=='I') {
        if (head[2]==0x2a && head[3]==0x00) {
            DEBUG_MODE PRINT_MESG("JBXL::readXHeadFile: TIFFデータ形式\n");
            hd.kind = TIFF_DATA;
            //fclose(fp);
            //return hd;
        }
    }
    if (head[0]=='M' && head[1]=='M') {
        if (head[2]==0x00 && head[3]==0x2a) {
            DEBUG_MODE PRINT_MESG("JBXL::readXHeadFile: TIFFデータ形式\n");
            hd.kind = TIFF_DATA;
            //fclose(fp);
            //return hd;
        }
    }*/

#ifdef  ENABLE_JPEGLIB
    // JPEG
    if (isJPEGHeader(buf)) {
        DEBUG_MODE PRINT_MESG("JBXL::readXHeadFile: JPEGデータ形式\n");
        JPEGImage jpg = readJPEGData(fp);
        hd = JPEGImage2CmnHead(jpg);

        if (hd.zsize==1) hd.kind = JPEG_MONO_DATA;
        jpg.free();
        free_Buffer(&buf);
        fclose(fp);
        return hd;
    }
#endif

    free_Buffer(&buf);

    ///////////////////////////////////////////////////////////////////////
    // 解析不能．データのまま読み込み　UN_KNOWN_DATA
    //
    DEBUG_MODE PRINT_MESG("JBXL::readXHeadFile: 未知のデータ形式\n");
    memset(&hd, 0, sizeof(CmnHead));

    hd.grptr = (uByte*)malloc(fsz);
    if (hd.grptr==NULL) {
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        hd.kind  = HEADER_NONE;
        fclose(fp);
        return hd;
    }
    memset(hd.grptr, 0, fsz);

    fseek(fp, 0, 0);
    ret = fread(hd.grptr, fsz, 1, fp);
    if (ret>0) {
        hd.kind  = UN_KNOWN_DATA;
        hd.lsize = fsz;
    }
    else {
        hd.xsize = JBXL_FILE_READ_ERROR;
        hd.kind  = HEADER_NONE;
    }

    fclose(fp);
    return hd;
}


/**
CmnHead  jbxl::readCmnHeadFile(const char* fn, CmnHead* chd, bool cnt=false)

データ種別を chdで指定して，グラフィックデータを読み込み，Common 形式に変換する．

サポートしているデータ形式は, 
- Common 形式  : Common 形式にしたがって読み込む．(CT_RGN_SLは未サポート)
- Sun Raster　 : Common 形式に変換する．ヘッダは RAS_DATA になる 
- Moon(CT)　　 : Common 形式に変換する．ヘッダは MOON_DATA になる．
- DICOM        : Common 形式に変換する．512x512xZ固定．ヘッダは DICOM_DATAになる．
- USERSET_DATA : CmnHead* chd でヘッダ情報を指定して，読み込みを行う．ヘッダは USERSET_DATAになる．
- Jpeg         : Common 形式に変換する．ヘッダは JPEG_RGB_DATAになる．ただしモノクロJPEGの場合は JPEG_MONO_DATA
- Tiff         : 未サポート．Common 形式に変換する．ヘッダは TIFF_DATAになる．

@param         fn   読み込むを行うファイル名．
@param[in,out] chd  ファイルのヘッダ情報を入れる．
@param         cnt  仮想カウンタを使用するか？

@return 読み込んだグラフィックデータを記述する共通ヘッダ．@n
        エラーなら kind = HEADER_NONE となる．またエラーの種類により xsizeが変化する．
@retval JBXL_GRAPH_OPFILE_ERROR  @b xsize ファイルオープンエラー．
@retval JBXL_GRAPH_MEMORY_ERROR  @b xsize メモリエラー．
@retval JBXL_GRAPH_CANCEL  @b xsize キャンセル by ユーザ
@bug Common形式の画素深度が 24,32bitの場合のエンディアン処理が未実装
*/
CmnHead  jbxl::readCmnHeadFile(const char* fn, CmnHead* chd, bool cnt)
{
    FILE* fp;
    int   kind, fsz;
    bool  no_ntoh = false;
    CmnHead hd;
    CVCounter* counter = NULL;

    memset(&hd, 0, sizeof(CmnHead));
    hd.kind  = HEADER_NONE;
    hd.xsize = JBXL_GRAPH_HEADER_ERROR;

    if (chd==NULL) return hd;

    fsz = (int)file_size(fn);
    if (fsz==0) {
        hd.xsize = JBXL_GRAPH_NOFILE_ERROR;
        return hd;
    }

    if ((fp=fopen(fn,"rb"))==NULL) {
        hd.xsize = JBXL_GRAPH_OPFILE_ERROR;
        return hd;
    }

    // ファイルとCPUのエンディアンが同じ
    if (checkBit(chd->kind, HAS_LENDIAN) && is_little_endian()) {
        no_ntoh = true;
    }
    kind = chd->kind & 0x00ff;

    // オペレータ指定のデータ形式．カウンタ未サポート
    if (kind == USERSET_DATA) {
        if (chd->zsize<=0) chd->zsize = 1;
        chd->lsize = chd->xsize*chd->ysize*chd->zsize*((chd->depth+7)/8);
        if (fsz==(int)(chd->bsize+chd->lsize)) {        // ファイルサイズのチェック
            DEBUG_MODE PRINT_MESG("JBXL::readCmnHeadFile: オペレータ指定のデータ形式\n");
            hd = readUserSetData(fp, chd, true);        // カウンタ有効 

            // 24, 32bit 未対応
            if (hd.depth==16 && !no_ntoh) {
                sWord* wptr = (sWord*)hd.grptr;
                for (int i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) {
                    wptr[i] = ntohs(wptr[i]);
                }
            }
        }
    }

    // DICOMファイル．
    else if (kind==DICOM_DATA) {
        MSGraph<sWord> vp = readDicomData(fp, fsz);
        if (vp.gp!=NULL) {
            DEBUG_MODE PRINT_MESG("JBXL::readCmnHeadFile: DICOMデータ形式\n");
            hd.kind  = DICOM_DATA;
            hd.xsize = vp.xs;
            hd.ysize = vp.ys;
            hd.zsize = vp.zs;
            hd.depth = 16;
            hd.bsize = sizeof(CTHead);
            hd.lsize = vp.xs*vp.ys*vp.zs*2;
            hd.buf   = (uByte*)malloc(hd.bsize);
            hd.grptr = (uByte*)vp.gp;
                
            CTHead* pcthd = (CTHead*)hd.buf;
            memset(pcthd, 0, hd.bsize);
            pcthd->xsize  = (sWord)hd.xsize;
            pcthd->ysize  = (sWord)hd.ysize;
            //pcthd->ctmin  = 0;
            //pcthd->ctmax  = 0;
            //pcthd->cutup  = 0;
            //pcthd->cutdown  = 0;
            //pcthd->cutleft  = 0;
            //pcthd->cutright = 0;

            if (vp.RZxy>0.0) {
                pcthd->anydata[0] = (sWord)(vp.RZxy*RZXY_RATE);
                pcthd->anydata[1] = RZXY_RATE;
                hd.kind |= HAS_RZXY;
            }

            if (!no_ntoh) {
                sWord* wptr = (sWord*)hd.grptr;
                for (int i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) {
                    wptr[i] = ntohs(wptr[i]);
                }
            }
        }
    }

    // Sun Raster
    else if (kind==RAS_DATA) {
        DEBUG_MODE PRINT_MESG("JBXL::readCmnHeadFile: Sun Rasterデータ形式\n");
        hd = readRasData(fp); 
    }
    
    // Moon形式 16bit
    else if (kind==MOON_DATA) {
        DEBUG_MODE PRINT_MESG("JBXL::readCmnHeadFile: Moonデータ形式\n");
        hd = readMoonData(fp, fsz, no_ntoh);
    }

#ifdef  ENABLE_JPEGLIB
    // JPEG
    else if (kind==JPEG_RGB_DATA || kind==JPEG_MONO_DATA) {
        DEBUG_MODE PRINT_MESG("JBXL::readCmnHeadFile: JPEGデータ形式\n");
        JPEGImage jpg = readJPEGData(fp);
        hd = JPEGImage2CmnHead(jpg);
        jpg.free();
    }
#endif

    // TIFF
    /*else if (kind == TIFF_DATA) {

    }*/

    // Common形式 
    else if (kind!=UN_KNOWN_DATA) {
        // データ読み取りでは hd.lsize==0 のファイルサイズ無効（CT_RGN_SL）はまだサポートされていない
        DEBUG_MODE PRINT_MESG("JBXL::readCmnHeadFile: Commonデータ形式\n");

        int hsz = sizeof(CmnHead_Entry);
        fseek(fp, 0, 0);
        size_t ret = fread(&hd.entry, hsz, 1, fp);
        hd.buf = NULL;
        hd.grptr = NULL;
        ntoh_st(&hd, 4);
        if (ret==0) {
            hd.xsize = JBXL_FILE_READ_ERROR;
            hd.kind  = HEADER_NONE;
            fclose(fp);
            return hd;
        }
        if (hd.zsize<=0) hd.zsize = 1;

        if (fsz == (int)(hsz + hd.bsize + hd.lsize) + 4 ) hsz = 36;     // x86 file
        if (fsz == (int)(hsz + hd.bsize + hd.lsize) + 16) hsz = 48;     // x64 file
        
        // カウンタ
        if (hd.zsize>=10 && cnt) {
            counter = GetUsableGlobalCounter();
            if (counter!=NULL) {
                //counter->SetTitle("Commonファイル読み込み中");
                counter->SetMax(hd.zsize/10);
                counter->SetPos(0);
            }
        }

        if (hd.bsize>0) {
            hd.buf = (uByte*)malloc(hd.bsize);
            if (hd.buf!=NULL) memset(hd.buf, 0, hd.bsize);
        }
        hd.grptr = (uByte*)malloc(hd.lsize);
        if ((hd.bsize>0&&hd.buf==NULL) || hd.grptr==NULL) {
            free_CmnHead(&hd);
            hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
            hd.kind  = HEADER_NONE;
            fclose(fp);
            return hd;
        }
        memset(hd.grptr, 0, hd.lsize);

        fseek(fp, hsz, 0);
        if (hd.bsize>0) {
            ret = fread(hd.buf, hd.bsize, 1, fp);
            ntoh_ar((sWord*)hd.buf, hd.bsize);
            if (ret==0) {
                hd.xsize = JBXL_FILE_READ_ERROR;
                hd.kind  = HEADER_NONE;
                fclose(fp);
                return hd;
            }
        }
        
        // ヘッダのみ
        if (hd.lsize==0) {
            if (counter!=NULL) counter->PutFill();
        }
        else {
            // データ本体
            if (counter==NULL) {
                ret = fread(hd.grptr, hd.lsize, 1, fp);
                if (ret==0) {
                    hd.xsize = JBXL_FILE_READ_ERROR;
                    hd.kind  = HEADER_NONE;
                    fclose(fp);
                    return hd;
                }
            }
            else {
                int psize = hd.xsize*hd.ysize*((hd.depth+7)/8);
                for (int i=0; i<hd.zsize; i++) {
                    ret = fread(hd.grptr+i*psize, psize, 1, fp);
                    if (ret==0) {
                        hd.xsize = JBXL_FILE_READ_ERROR;
                        hd.kind  = HEADER_NONE;
                        fclose(fp);
                        return hd;
                    }
                    if (i%10==0) {
                        counter->StepIt();
                        if (counter->isCanceled()) {    // キャンセル
                            //counter->Stop();
                            free_CmnHead(&hd);
                            hd.xsize = JBXL_GRAPH_CANCEL;
                            hd.kind  = HEADER_NONE;
                            fclose(fp);
                            return hd;
                        }
                    }
                }
            }

            // 24, 32bit 未対応
            if (hd.depth==16 && !no_ntoh) {
                sWord* wptr = (sWord*)hd.grptr;
                for (int i=0; i<hd.xsize*hd.ysize*hd.zsize; i++) {
                    wptr[i] = ntohs(wptr[i]);
                }
            }

            if (counter!=NULL) counter->PutFill();
        }
    }

    else {
        PRINT_MESG("JBXL::readCmnHeadFile: 未知のデータ形式が指定された．(%04x, %04x)\n", chd->kind, (uWord)kind);
    }

    fclose(fp);
    return hd;
}


/**
int  jbxl::writeCmnHeadFile(const char* fn, CmnHead* hd, bool cnt)

共通画像データをCTファイルとして保存する．

元の共通画像データのヘッダ種別が MOON_DATA または USERSET_DATAの場合は CT(MOON)形式で保存する．@n
RAS_DATA, JPEG_RGB_DATA, JPEG_MONO_DATA, TIFF_DATAの場合はそれぞれの形式で保存する．(TIFF_DATAは未サポート) @n
CT_DATA, CT_3DM, CT_3D_VOL 場合はそのまま共通形式 (ヘッダはそのまま)で保存する．

サポートしているヘッダ種別は @n
@b CT_DATA, @b USERSET_DATA, @b CT_3DM, @b CT_3D_VOL, @b RAS_DATA, @b MOON_DATA, @b JPEG_RGB_DATA, @b JPEG_MONO_DATA, @b JPEG16_XXX_DATA

ヘッダに必要な情報は
@b hd->kind, @b xsize, @b ysize, @b zsize, @b depth, @b grptr, @b (buf)　　　　( )は省略可.

@param  fn   保存するファイル名．
@param  hd   保存するデータを既述した共通ヘッダ．
@param  cnt  仮想カウンタを使用するか？

@retval >0   書き込んだサイズ．
@retval JBXL_GRAPH_IVDARG_ERROR  引数不正．
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー．
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー．
@retval JBXL_GRAPH_WRFILE_ERROR  書き込みファイルサイズ不一致
@retval JBXL_GRAPH_RDFILE_ERROR  書き込みファイルの再読み取り不可
@retval JBXL_GRAPH_HEADER_ERROR  書き込みファイルのヘッダ異常
*/
int  jbxl::writeCmnHeadFile(const char* fn, CmnHead* hd, bool cnt)
{
    CmnHead cmd;
    FILE* fp;
    int  csize, psize;
    int  kind = hd->kind & 0x0ff;

    if (kind==UN_KNOWN_DATA) return JBXL_GRAPH_HEADER_ERROR;
    if (hd==NULL || hd->lsize==0) return JBXL_GRAPH_IVDARG_ERROR;

    if ((fp=fopen(fn,"wb"))==NULL) {
        PRINT_MESG("JBXL::writeCmnHeadFile: ERROR：ファイルオープン失敗\n");
        return JBXL_GRAPH_OPFILE_ERROR;
    }
    DEBUG_MODE PRINT_MESG("JBXL::writeCmnHeadFile: ファイル種別 = %d で書き込み中．%dx%dx%d\n", hd->kind, hd->xsize, hd->ysize, hd->zsize);

    // Write File
    csize = writeCmnHeadData(fp, hd, cnt);
    fclose(fp);
    if (csize<0) return (int)csize;
    
    // SUN RASTER
    if (kind==RAS_DATA) return csize;

    // JPEG
    if (kind==JPEG_RGB_DATA   || kind==JPEG_MONO_DATA) return csize;
    if (kind==JPEG_ARGB_DATA  || kind==JPEG_RGBA_DATA) return csize;
    if (kind==JPEG16_RGB_DATA || kind==JPEG16_ARGB_DATA || kind==JPEG16_RGBA_DATA) return csize;

    // TIFF
    //if (kind==TIFF_DATA) return csize;

    // 書き込みチェック
    int fsz = (int)file_size(fn);
    if (kind==MOON_DATA) psize = hd->bsize + hd->lsize;
    else psize = sizeof(CmnHead_Entry) + hd->bsize + hd->lsize;

    if (fsz!=psize) {
        PRINT_MESG("JBXL::writeCmnHeadFile: ERROR：書き込みファイルのサイズが合わない %d != %d\n", psize, fsz);
        return JBXL_GRAPH_WRFILE_ERROR;
    }
    if (kind==MOON_DATA) return psize;

    if ((fp=fopen(fn,"rb"))==NULL) {
        PRINT_MESG("JBXL::writeCmnHeadFile: ERROR：ファイル検査：再オープン失敗\n");
        return JBXL_GRAPH_RDFILE_ERROR;
    }
 
    size_t ret = fread((sByte*)&cmd.entry, sizeof(CmnHead_Entry), 1, fp);
    fclose(fp);
    ntoh_st(&cmd.entry, 4);
    if (ret==0) {
        PRINT_MESG("JBXL::writeCmnHeadFile: ERROR：ファイルヘッダ検査：ヘッダ異常\n");
        return JBXL_FILE_READ_ERROR;
    }
    if (cmd.xsize!=hd->xsize || cmd.ysize!=hd->ysize || cmd.zsize!=hd->zsize ||
        cmd.bsize!=hd->bsize || cmd.lsize!=hd->lsize || cmd.depth!=hd->depth || cmd.kind!=hd->kind) {
        PRINT_MESG("JBXL::writeCmnHeadFile: ERROR：ファイルヘッダ検査：ヘッダ異常\n");
        return JBXL_GRAPH_HEADER_ERROR;
    }

    return  csize;
}


/**
int  jbxl::writeCmnHeadData(File* fp, CmnHead* hd, bool cnt)
 
共通画像データをファイルとして保存する．

元の共通画像データのヘッダ種別が MOON_DATA または USERSET_DATAの場合は CT(MOON)形式で保存する．@n
RAS_DATA, JPEG_RGB_DATA, JPEG_MONO_DATA, TIFF_DATAの場合はそれぞれの形式で保存する．(TIFF_DATAは未サポート)@n
CT_DATA, CT_3DM, CT_3D_VOL 場合はそのまま共通形式(ヘッダ種別はそのまま)で保存する．@n
カウンタサポートあり．

サポートしているヘッダ種別は @n
@b CT_DATA, @b USERSET_DATA, @b CT_3DM, @b CT_3D_VOL, @b RAS_DATA, @b MOON_DATA, @b JPEG_RGB_DATA, @b JPEG_MONO_DATA

ヘッダに必要な情報は @n
@b hd->kind, @b xsize, @b ysize, @b zsize, @b depth, @b grptr, @b (buf)　　　　( )は省略可.

@param  fp   保存するファイルの識別子．
@param  hd   保存するデータを既述した共通ヘッダ．
@param  cnt  仮想カウンタを使用するか？

@retval >0  書き込んだサイズ．
@retval JBXL_GRAPH_HEADER_ERROR  ヘッダエラー
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー
@retval JBXL_GRAPH_CANCEL  キャンセル
*/
int  jbxl::writeCmnHeadData(FILE* fp, CmnHead* hd, bool cnt)
{
    CTHead  chd;
    CmnHead cmd;
    CVCounter* counter = NULL;
    sByte* ptr;
    int i, j, k, l;
    int csize, psize;
    int kind = hd->kind & 0x00ff;

    if (kind==UN_KNOWN_DATA) return JBXL_GRAPH_HEADER_ERROR;

    // SUN RASTER
    if (kind==RAS_DATA) {
        //csize = writeRasData(fp, hd, hd->depth);
        csize = writeRasData(fp, hd, 8);
        return csize;
    }

#ifdef  ENABLE_JPEGLIB
    // JPEG
    if (kind==JPEG_RGB_DATA   || kind==JPEG_MONO_DATA   || kind==JPEG_ARGB_DATA || kind==JPEG_RGBA_DATA ||
        kind==JPEG16_RGB_DATA || kind==JPEG16_ARGB_DATA || kind==JPEG16_RGBA_DATA) {
        JPEGImage jpg = CmnHead2JPEGImage(*hd);
        csize = writeJPEGData(fp, &jpg, 100);
        jpg.free();
        return csize;
    }
#endif

// Not Implement yet
#ifdef  ENABLE_PGN
    // PNG
#endif

    // TIFF
    /*if (kind==TIFF_DATA) {
        csize = 
        return csize;
    }*/

    // TGA


    ////////////////////////////////////////////////////////////////////////
    // CT : ヘッダの準備
    if (hd->zsize<=0) hd->zsize = 1;
    if (hd->depth<=0) hd->depth = 16;

    psize = hd->xsize*hd->ysize*((hd->depth+7)/8); 
    hd->lsize = psize*hd->zsize;
    ptr = (sByte*)malloc(hd->lsize);
    if (ptr==NULL) return JBXL_GRAPH_MEMORY_ERROR;
    memset(ptr, 0, hd->lsize);

    // CTHead chd を作る．
    if (kind==CT_DATA || kind==CT_3DM || kind==CT_3D_VOL) {
        DEBUG_MODE {
            PRINT_MESG("JBXL::writeCmnHeadData: CTデータ\n");
            PRINT_MESG("JBXL::writeCmnHeadData: ヘッダバッファ bsize = %d\n", hd->bsize);
            PRINT_MESG("JBXL::writeCmnHeadData: データサイズ   lsize = %d\n", hd->lsize);
            PRINT_MESG("JBXL::writeCmnHeadData: サイズ         %dx%dx%d %d\n", hd->xsize, hd->ysize, hd->zsize, hd->depth);
        }
        memcpy(&chd, hd->buf, hd->bsize);
        chd.anydata[2] += TempBase;
        hton_st(&chd, 2);

        memcpy(ptr, hd->grptr, hd->lsize);
    }
    else if (kind==MOON_DATA || kind==USERSET_DATA) {   
        chd.xsize   = htons((sWord)hd->xsize);
        chd.ysize   = htons((sWord)hd->ysize);
        chd.ctmin   = 0;
        chd.ctmax   = 0;
        chd.cutup   = 0;
        chd.cutdown = 0;
        chd.cutleft = 0;
        chd.cutright= 0;

        k = l = 0;
        if (hd->depth<16){   
            for (i=0; i<hd->xsize*hd->ysize; i++) {
                for (j=0; j<hd->depth/8; j++) ptr[k++] = 0x00;
                for (j=hd->depth/8; j<2; j++) ptr[k++] = hd->grptr[l++];
            }
        }
        else if (hd->depth==16) {
            for (i=0; i<hd->xsize*hd->ysize; i++) {
                for (j=0; j<2; j++) ptr[k++] = hd->grptr[l++];
            }
        }
        else {
            for (i=0; i<hd->xsize*hd->ysize; i++) {
                for (j=0; j<2; j++) ptr[k++] = hd->grptr[l++];
                l += (hd->depth)/8 - 2;
            }
        }
    }
    else {
        PRINT_MESG("JBXL::writeCmnHeadData: サポートしていないヘッダタイプ %d\n", hd->kind);
        free(ptr);
        return 0;
    }

    ////////////////////////////////////////////////////////////////////////
    // データ書き込み
    csize = 0;
    // 共通ヘッダ書き込み
    if (kind==CT_DATA || kind==CT_3DM || kind==CT_3D_VOL) {
        // カウンタ
        if (hd->zsize>=10 && cnt) {
            counter = GetUsableGlobalCounter();
            if (counter!=NULL) {
                //counter->SetTitle("Commonファイル書き込み中");
                counter->SetMax((hd->zsize+1)/10);
                counter->SetPos(1);
            }
        }

        cmd = *hd;
        cmd.grptr = NULL;
        hton_st(&cmd, 4);
        csize = sizeof(CmnHead_Entry);
        fwrite(&cmd.entry, csize, 1, fp);
    }

    // CTヘッダ書き込み
    fwrite(&chd, sizeof(CTHead), 1, fp);
    csize += sizeof(CTHead);

    // 本体データ書き込み
    hton_ar((sWord*)ptr, hd->lsize);

    if (counter==NULL) {
        fwrite(ptr, hd->lsize, 1, fp);
        csize += hd->lsize;
    }
    else {
        for (i=0; i<hd->zsize; i++) {
            fseek(fp, csize, 0);
            fwrite(ptr+i*psize, psize, 1, fp);
            csize += psize;
            if (i%10==0) {
                counter->StepIt();
                if (counter->isCanceled()) {    // キャンセル
                    free(ptr);
                    return JBXL_GRAPH_CANCEL;
                }
            }
        }
    }

    free(ptr);
    if (counter!=NULL) counter->PutFill();

    return  csize;
}

