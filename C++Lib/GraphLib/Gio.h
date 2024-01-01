
#ifndef  __JBXL_CPP_GRAPHIC_IO_H_
#define  __JBXL_CPP_GRAPHIC_IO_H_


/**
@brief    グラフィック用ファイル入出力関数ヘッダ  
@file     Gio.h
@author   Fumi.Iseki (C)

CTファイルへの入出力もサポートする．また，ファイル入出力の統合関数もサポート@n
JPEG については JpegTool.cpp 参照．
*/


#ifdef WIN32
#pragma warning(disable:4995)
#endif


#include "Gdata.h"



//
namespace jbxl {


/**
@file   Gio.h

ファイル形式とヘッダ種別
---
@par Common形式
@code
    CmnHead  cmhd   : common header. 36Byte. 
    Graphic  Header : 画像データ固有のヘッダ
    Graphic  Data   : データ
@endcode
任意の画像データを保存する時，その画像データに識別ヘッダ(CmnHead) を付けて保存したもの．

@par UN_KNOWN_DATA

@par MOON_DATA

@par DICOM_DATA

@par USERSET_DATA

@par RAS_DATA

@par JPEG_RGB_DATA

@par JPEG8_DATA

@par CT_DATA
@code
    CmnHead  cmhd : common header. 36Byte. （省略可）
    CTHead   cthd : CT header (Moon Header). 64Byte. メンバは Word型. 
    Graphic  Data : データ
@endcode
Common Headerは省略可．@n
いわゆる Moon型のヘッダを持つ CT画像データ．現在は CT画像を保存する場合は，デフォルトでこの型で保存される．@n
CTHead の cthd.anydata[0]に画素間隔，cthd.anydata[1]に画像間隔が 1/RZXY_RATE mm単位で保存されている場合がある．@n
ファイルから読み込まれ，CmnHead に格納された場合は，CT_3DMと区別されない．  

@par CT_3DM    
@code
    CmnHead  cmhd : common header. 36Byte. 
    CTHead   cthd : CT header (Moon Header). 64Byte. 
    Graphic  Data : データ
@endcode
3Dの CTデータを保存する場合に使用されるデータの型．
CTHead の cthd.anydata[0]に画素間隔，cthd.anydata[1]に画像間隔が 1/RZXY_RATE mm単位で保存されている場合がある．@n
メモリ内で使用する場合（CmnHeadに格納された場合）は CT_Data と区別されない．

@par CT_3D_VOL

@bug Common形式の画素深度が 24,32bitの場合のエンディアン処理が未実装
@bug x86 と x64 では CmnHead のサイズが異なるので，x86 と x64 でデータファイルには基本的に互換性がない．@n
参考：sizeof(CmnHead) = x86: 32Byte, x64: 44Byte ただしパッティングで 48Byte @n
現状は readXHead(), readXHeadFile() および readCmnHeadFile() については 小手先でごまかしている．

*/    
    

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  for Simple
//
//template <typename T>  int  writeRasFile(const char *fname, MSGraph<T> vp);
//template <typename T>  MSGraph<T>  readRasFile(const char *fname);


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  for SUN Raster & User Define
CmnHead readUserSetData(FILE* fp, CmnHead* ch, bool cnt=false); ///< read_user_data() for C  // ユーザ指定(ch)のデータ形式でファイルを読み込む
CmnHead readRasData(FILE* fp);                                  ///< read_ras_data()  for C  // SUN RASTER形式のファイルを読み込む 
int     writeRasData(FILE* fp, CmnHead* ch, int obit=8);        ///< write_ras_data() for C


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  for CT画像
CmnHead readMoonFile(const char* fn, bool no_ntoh=false);   
CmnHead readMoonData(FILE* fp, unsigned int fsz=0, bool no_ntoh=false); // MOON_DATA形式のファイルを読み込む．

int     dicomHeader(FILE* fp, int fsize, int* dsize, int* xsize, int* ysize, int* depth, double* rzxy);
MSGraph<sWord> readDicomFile(const char* fn);
MSGraph<sWord> readDicomData(FILE* fp, int fsz);


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  for I/O 統合  多種ファイル対応
//template <typename T> MSGraph<T> readGraphicFile  (const char* fname, CmnHead* chd=NULL, bool cnt=false);
//template <typename T> MSGraph<T> readGraphicSlices(const char* frmt, int fst, int fnd, CmnHead* chd);
//template <typename T> int writeGraphicFile(const char *fname, MSGraph<T> vp, int kd=0, int fn=0, int tn=-1, bool cnt=false);

CmnHead readXHead      (const char* fn, CmnHead* ch=NULL);                 ///< ヘッダ部分のみ読み込み 
CmnHead readXHeadFile  (const char* fn, CmnHead* ch=NULL, bool cnt=false); ///< 拡張read関数．ファイルを自動判別して読み込む．
CmnHead readCmnHeadFile(const char* fn, CmnHead* ch, bool cnt=false);      ///< 拡張read関数．ファイル種別を指定して読み込む．

int     writeCmnHeadFile(const char* fn, CmnHead* hd, bool cnt=false);
int     writeCmnHeadData(FILE* fp, CmnHead* hd, bool cnt=false);


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  C言語との関数対応
/*
CmnHead    readRasData()            read_ras_data()  for C 
CmnHead    readUserSetData()        read_user_data() for C 
int        writeRasData()           write_ras_data() for C
int        writeRasFile()           write_ras_file() for C
MSGraph<T> readRasFile()            read_ras_file()  for C
int        dicomHeader()            dicom_header()   for C
*/


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ファイル名によるI/Oサポート
/**
template <typename T>  MSGraph<T> readGraphicFile(const char* fname, CmnHead* chd=NULL, bool cnt=false)

色々なグラフィックファイルを読み込んで，MSGraph<T>に格納．@n

サポートしているデータ形式は, 
- Common 形式    : Common 形式にしたがって読み込む．(CT_RGN_SLは未サポート)
- Sun Raster     : Common 形式に変換する．ヘッダは RAS_DATA になる 
- Moon(CT)       : Common 形式に変換する．ヘッダは MOON_DATA になる．
- USERSET_DATA   : CmnHead* chd でヘッダ情報を指定して，読み込みを行う．ヘッダは USERSET_DATAになる．
- DICOM_DATA     : DICOM ファイル
- JPEG_RGB_DATA  : JPEG ファイル
- JPEG_MONO_DATA : モノクロ JPEG
- unkonwn        : ファイルをそのまま，hd->grptrへ読み込む．ヘッダは UN_KNOWN_DATA になる．

ただし，UN_KNOWN_DATAの場合は CmnHaedの hd->kind, hd->lsize, hd->gptr のみ有効．

@param      fname  読み込むを行うファイル名．
@param[in]  chd    USERSET_DATA指定の場合，ファイルのヘッダ情報を入れた CmnHead
@param[out] chd    CmnHeadにも格納したい場合は，そのアドレスを指定．            
@param      cnt    カウンタ（プログレスバー）を使用するかどうか？
            
@return 読み込んだグラフィックデータと，それを記述する共通ヘッダ．

エラーなら MSGraph.isNull()が真，または  CmnHead.kind = HEADER_NONE となる．@n
またエラーの種類により MSGraph.stateが変化する.@n

@retval JBXL_GRAPH_HEADER_ERROR  @b state ファイルのヘッダ読み取りエラー．
@retval JBXL_GRAPH_MEMORY_ERROR  @b state メモリエラー．

@par CmnHeadのヘッダ情報 (chd!=NULLのときのみ) @b chd->kind 
- HEADER_NONE:  データ読み込み失敗．
- UN_KNOWN_DATA: ヘッダが解析できないのでファイルイメージのまま chd->grptrに格納した．
    - chd->kind, chd->lsize, chd->gptrのみ有効．MSGraph<T>.gp は NULL．
- その他: MSGraph<T> に画像データが入る．*chd にはヘッダ情報のみが入る．
    - chd->buf, chd->gptr は NULL.  CT_DATA, RAS_DATA, CT_3DM など．

@bug Common 形式の画素深度が 24,32bitの場合のエンディアン処理が未実装
*/
template <typename T>  MSGraph<T> readGraphicFile(const char* fname, CmnHead* chd=NULL, bool cnt=false)
{
    MSGraph<T> vp;
    CmnHead    hd;
    CVCounter* counter = NULL;

    counter = GetUsableGlobalCounter();
    if (counter!=NULL) {
        counter->SetMax(100);
        counter->MakeChildCounter(80);
    }

    vp.init();

    hd = readXHeadFile(fname, chd, cnt);
    if (counter!=NULL) counter->DeleteChildCounter();   

    // Error
    if (hd.kind==HEADER_NONE) {
        free_CmnHead(&hd);
        if (hd.xsize<0) vp.state = hd.xsize;
        else            vp.state = JBXL_GRAPH_HEADER_ERROR;
        return vp;
    }

    // Unknown Data
    int chk = hd.kind & 0x00ff;
    if (chk==UN_KNOWN_DATA) {
        if (chd!=NULL) *chd = hd;
        else  free_CmnHead(&hd);
        vp.state = JBXL_GRAPH_HEADER_ERROR;
        return vp;
    }

    if (counter!=NULL) counter->MakeChildCounter(80);

    if (cnt) {
        CVCounter* counter = GetUsableGlobalCounter();
        if (counter!=NULL) counter->SetTitle("画像データへ変換");
    } 

    if (chd!=NULL) {
        *chd = hd;
        chd->grptr = chd->buf = NULL;
    }

    vp = copyCmnHead2MSGraph<T>(hd, CH2MG_NORMAL, cnt);
    free_CmnHead(&hd);
    if (vp.gp==NULL) return vp;

    if (ZeroBase!=0) {
        vp.base += ZeroBase;
        for (int i=0; i<vp.xs*vp.ys*vp.zs; i++) vp.gp[i] += (T)ZeroBase;
    }

    if (counter!=NULL){
        counter->DeleteChildCounter();
        counter->PutFill();
    }

    return  vp;
}


/**
template <typename T>  MSGraph<T>  readGraphicSlices(const char* fmt, int fst, int fnd, CmnHead* chd=NULL, bool cnt=false)

色々なグラフィックファイルをスライスデータとして読み込んで，3D画像として
MSGraph<T>に格納．また，chd!=NULL ならCommon(CmnHead)形式にも変換する．

Z方向の歪率をサポート（CmnHead, Moon形式のファイルのみ）@n
グローバルカウンタが使用可能．カウンタによるキャンセル機能を持つ．

readGraphicFile()と殆ど同じ手法を用いているので，その他は readGraphicFile()に準じる．

@param fmt  読み込むを行うファイル名（%dなどの入った書式）．
@param fst  最初に読み込むファイルの番号（ファイル名の中にその数字がなければならない）
@param fnd  最後に読み込むファイルの番号（ファイル名の中にその数字がなければならない）
@param chd  CmnHeadにもデータを格納したい場合はそのアドレスを指定．@n
            または DICOM_DATA, USERSET_DATA指定時のヘッダ情報を格納する．
@param cnt  仮想カウンタを使用するか?

@return MSGraph型の読み込んだ画像データ

@par 例
@code
    vp = readGraphicSlices("i%02d", 0, 10); // i00 〜 i10 までの11枚の画像データを読み込んで，vpに格納．
@endcode
*/
template <typename T>  MSGraph<T>  readGraphicSlices(const char* fmt, int fst, int fnd, CmnHead* chd=NULL, bool cnt=false)
{
    int   i, k, l, m, chk, kind;
    int   xsize, ysize, zsize, nsize, psize;    // zsize: 1枚の画像のZサイズ．
                                                // nsize: 出来上がりの 3D画像のZサイズ
    char* fr_name;                      
    CmnHead    dhd, hd;
    MSGraph<T> vp, gd;

    if (chd!=NULL) {
        DEBUG_MESG("readGraphicSlices: Called with kind = 0x%04x\n", chd->kind);
    }
    else {
        DEBUG_MESG("readGraphicSlices: Called with NULL Header\n");
    }

    gd.init();
    vp.init();

    // 1枚目（ヘッダ情報）の読み出し
    fr_name = numbering_name(fmt, fst);
    dhd = readXHead(fr_name, chd);
    freeNull(fr_name);

    chk = dhd.kind & 0x00ff;
    if (dhd.kind==HEADER_NONE || chk==UN_KNOWN_DATA) {
        DEBUG_MODE PRINT_MESG("readGraphicSlices: ヘッダ読み込みエラー： kind = %d\n", dhd.kind);
        free_CmnHead(&dhd);
        vp.state = JBXL_GRAPH_HEADER_ERROR;
        vp.zs = fst;
        return vp;
    }
    kind  = dhd.kind;
    xsize = dhd.xsize;
    ysize = dhd.ysize;
    zsize = Max(dhd.zsize, 1);
    nsize = zsize*(Xabs(fnd - fst) + 1);
    psize = xsize*ysize*zsize;
    if (dhd.depth==16) gd.color = GRAPH_COLOR_MONO16;
    else               gd.color = GRAPH_COLOR_MONO;

    // カウンタ
    CVCounter* counter = NULL;
    if (nsize>=10 && cnt) {
        counter = GetUsableGlobalCounter();
        if (counter!=NULL) {
            //counter->SetTitle("マルチファイル読み込み中");
            counter->SetMax(nsize/10);
            counter->SetPos(1);
        }
    }

    // Z軸の歪, ベース，rbound の設定
    if (dhd.buf!=NULL && (chk==CT_DATA || chk==CT_3DM || chk==DICOM_DATA)) {
        sWord* rz  = (sWord*)dhd.buf;
        if (rz[9]==RZXY_RATE || checkBit(dhd.kind, HAS_RZXY)) {
            double rzm = rz[8]/(double)rz[9];
            if (rzm<5.0 && rzm>0.) gd.RZxy = rzm;
        }

        if (rz[10]!=0 && checkBit(dhd.kind, HAS_BASE)) {
            gd.base = (T)rz[10];
        }

        if (checkBit(dhd.kind, HAS_RBOUND)) {
            gd.rbound.xmin = rz[6];
            gd.rbound.xmax = rz[7];
            gd.rbound.ymin = rz[4];
            gd.rbound.ymax = rz[5];
            gd.rbound.zmin = rz[2];
            gd.rbound.zmax = rz[3];
        }
    }
    free_CmnHead(&dhd);

    dhd.kind = kind;
    if (chd!=NULL) dhd.kind |= chd->kind & 0xff00;      // オプション
    *chd = getinfo_CmnHead(dhd);

    // データ読み込み（Z軸方向） 1枚目から
    gd.getm(xsize, ysize, nsize);
    if (gd.isNull()) return gd;

    l = 0;
    k = fst;
    m = Sign(fnd - fst);
    while(k != fnd+m){
        fr_name = numbering_name(fmt, k);
        hd = readCmnHeadFile(fr_name, &dhd, false); // カウンタ無効
        free(fr_name);

        chk = hd.kind & 0x00ff;
        if (hd.kind==HEADER_NONE || chk==UN_KNOWN_DATA) {
            DEBUG_MESG("readGraphicSlices: データ読み込みエラー： n = %d, kind = %d\n", k, hd.kind);
            free_CmnHead(&hd);
            if (l==0) {     // 1枚目でエラー
                gd.free();
                vp.state = JBXL_GRAPH_HEADER_ERROR;
                vp.zs = k;
                return vp;
            }
            else {          // 途中までは読み込み完了
                gd.state = l;
                gd.zs = l*zsize;
                break;
            }
        }

        vp = copyCmnHead2MSGraph<T>(hd, CH2MG_NOPARM, false);   // カウンタ無効
        if (vp.isNull()) {
            free_CmnHead(&hd);
            gd.free();
            vp.free();
            vp.state = JBXL_GRAPH_MEMORY_ERROR;
            vp.zs = k;
            return vp;
        }

        // gd へコピー
        for (i=0; i<psize; i++) gd.gp[i + psize*l] = vp.gp[i] + ZeroBase;
        if (l==0) {
            gd.min = vp.min;
            gd.max = vp.max;
        }
        else {
            gd.min = Min(gd.min, vp.min);
            gd.max = Max(gd.max, vp.max);
        }

        // 後始末
        free_CmnHead(&hd);
        vp.free();
        k += m;
        l++;

        // カウンタ
        if (counter!=NULL && l%10==0) {
            counter->StepIt();
            if (counter->isCanceled()) {    // キャンセル
                gd.free();
                vp.state = JBXL_GRAPH_CANCEL;
                vp.zs = k;
                return vp;
            }
        }
    }
    gd.state = l;
    gd.base += ZeroBase;

    if (!checkBit(kind, HAS_RBOUND)) gd.rbound.set(0, gd.xs-1, 0, gd.ys-1, 0, gd.zs-1);
    if (chd!=NULL) {
        if ((chd->kind&0x00ff)==USERSET_DATA) {
            CVCounter* counter = GetUsableGlobalCounter();
            if (counter!=NULL) counter->SetTitle("Converting Data");
            *chd = copyMSGraph2CmnHead(gd);
        }
    }
    if (counter!=NULL) {
        counter->PutFill();
        //counter->Stop();
    }
   
    return gd;
}


/**
template <typename T>  int  writeGraphicFile(const char *fname, MSGraph<T> vp, int kind=0, int mlt=FALSE, int fnum=0, int tnum=0, bool cnt=false)

グラフィックデータvpをファイルとして書き出す．

kind でファイルの種別を指定可能．ただし kind==0 の場合は
- vp.zs<=1 なら CT_DATAとして保存される．
- vp.zs >1 なら CT_3DM として保存される．

サポートするデータ形式 @n
@b CT_DATA, @b CT_3DM, @b MOON_DATA, @b RAS_DATA, @b JPEG_RGB_DATA, @b JPEG_MONO_DATA

RZxy!=1.0 ならヘッダにその情報が埋め込まれる．@n
fnum, tnum を指定するとグラフィックデータの一部を保存できる．

@param  fname  保存する際のファイル名．
@param  vp     保存するグラフィックデータ．
@param  kind   データ形式 + 属性ビット
@param  mlt    3Dデータとして保存しない
@param  fnum   保存するデータの始まりの Z座標．デフォルトは 0
@param  tnum   保存するデータの終わりの Z座標．デフォルトは vp.zs-1
@param  cnt    カウンタ（プログレスバー）を表示するか？

@retval 1以上  正常終了．
@retval JBXL_GRAPH_OPFILE_ERROR  ファイルオープンエラー．
@retval JBXL_GRAPH_MEMORY_ERROR  メモリエラー．
@retval JBXL_GRAPH_WRFILE_ERROR  書き込みファイルサイズ不一致
@retval JBXL_GRAPH_RDFILE_ERROR  書き込みファイルの再読み取り不可
@retval JBXL_GRAPH_HEADER_ERROR  書き込みファイルのヘッダ異常
@retval JBXL_GRAPH_CANCEL  キャンセル
*/
template <typename T>  int  writeGraphicFile(const char *fname, MSGraph<T> vp, int kind=0, int mlt=FALSE, int fnum=0, int tnum=0, bool cnt=false)
{
    int     ret;
    CmnHead hd;
    CTHead* chd;

    if (fnum<0)            fnum = 0;
    else if (fnum>vp.zs-1) fnum = vp.zs - 1;
    if (tnum<fnum)         tnum = fnum;
    else if (tnum>vp.zs-1) tnum = vp.zs - 1;

    // for JPEG
    if ((kind&0x00ff)==JPEG_RGB_DATA) {
        if      (vp.color==GRAPH_COLOR_ARGB)   kind = (kind&0xff00) + JPEG_ARGB_DATA;
        else if (vp.color==GRAPH_COLOR_RGBA)   kind = (kind&0xff00) + JPEG_RGBA_DATA;

        if      (vp.color==GRAPH_COLOR_RGB16)  kind = (kind&0xff00) + JPEG16_RGB_DATA;
        else if (vp.color==GRAPH_COLOR_ARGB16) kind = (kind&0xff00) + JPEG16_ARGB_DATA;
        else if (vp.color==GRAPH_COLOR_RGBA16) kind = (kind&0xff00) + JPEG16_RGBA_DATA;
    }

    //
    hd.xsize = vp.xs;
    hd.ysize = vp.ys;
    hd.zsize = tnum - fnum + 1; //vp.zs;
    hd.depth = sizeof(T)*8;
    hd.grptr = (uByte*)(vp.gp + vp.xs*vp.ys*fnum);
    if (kind==0) {
        hd.kind  = CT_3DM;
        if (hd.zsize==1) hd.kind = CT_DATA;
    }
    else hd.kind = kind;

    hd.lsize = hd.zsize*hd.ysize*hd.zsize*2;
    hd.bsize = sizeof(CTHead);
    hd.buf   = (uByte*)malloc(hd.bsize);
    if (hd.buf==NULL) return JBXL_GRAPH_OPFILE_ERROR;
    memset(hd.buf, 0, hd.bsize);

    chd           = (CTHead*)hd.buf;
    chd->xsize    = (sWord)vp.xs;
    chd->ysize    = (sWord)vp.ys;
    chd->ctmin    = (sWord)vp.rbound.zmin;
    chd->ctmax    = (sWord)vp.rbound.zmax;
    chd->cutup    = (sWord)vp.rbound.ymin;
    chd->cutdown  = (sWord)vp.rbound.ymax;
    chd->cutleft  = (sWord)vp.rbound.xmin;
    chd->cutright = (sWord)vp.rbound.xmax;
    hd.kind      |= HAS_RBOUND;

    if (vp.RZxy!=1.0) {
        chd->anydata[0] = (sWord)(vp.RZxy*RZXY_RATE);
        chd->anydata[1] = (sWord)RZXY_RATE;
        hd.kind        |= HAS_RZXY;
    }

    if (vp.base!=0) {
        chd->anydata[2] = (sWord)vp.base + TempBase;
        hd.kind        |= HAS_BASE;
    }

    // 通常の保存
    if (!mlt) {
        ret = writeCmnHeadFile(fname, &hd, cnt);
    }

    // マルチスライス
    else {
        // カウンタ
        CVCounter* counter = NULL;
        if (hd.zsize>=10 && cnt) {
            counter = GetUsableGlobalCounter();
            if (counter!=NULL) {
                counter->SetMax((hd.zsize+1)/10);
                counter->SetPos(1);
            }
        }

        char filename[LNAME];
        int num = hd.zsize;
        int cnt = fnum;
        hd.zsize = 1;

        for (int i=0; i<num; i++) {
            snprintf(filename, LNAME-2, fname, cnt);
            hd.grptr = (uByte*)(vp.gp + vp.xs*vp.ys*cnt);
            ret = writeCmnHeadFile(filename, &hd, false);
            
            if (counter!=NULL && i%10==0) {
                counter->StepIt();
                if (counter->isCanceled()) {    // キャンセル
                    if (hd.buf!=NULL) free(hd.buf);
                    return JBXL_GRAPH_CANCEL;
                }
            }
            if (ret<0) break;
            cnt++;
        }
        if (counter!=NULL) counter->PutFill();
    }

    if (hd.buf!=NULL) free(hd.buf);
    
    return ret;
}


/**
template <typename T>  int  writeRasFile(const char *fname, MSGraph<T> vp)
*/
template <typename T>  int  writeRasFile(const char *fname, MSGraph<T> vp)
{
    if (fname==NULL) return JBXL_GRAPH_IVDARG_ERROR;

    FILE* fp = fopen(fname, "wb");
    if (fp==NULL) return JBXL_GRAPH_OPFILE_ERROR;

    CmnHead hd = copyMSGraph2CmnHead(vp);
    writeRasData(fp, &hd);

    fclose(fp);
    return JBXL_NORMAL;
}


/**
template <typename T>  MSGraph<T>  readRasFile(const char *fname)
*/
template <typename T>  MSGraph<T>  readRasFile(const char *fname)
{
    MSGraph<T> vp;
    CmnHead    hd;
    FILE* fp;

    vp.init();
    fp = fopen(fname, "rb");
    if (fp==NULL) return vp;

    hd = readRasData(fp);
    fclose(fp);

    vp = copyCmnHead2MSGraph<T>(hd, CH2MG_NORMAL);
    return vp;
}


}       // namespace


#endif
