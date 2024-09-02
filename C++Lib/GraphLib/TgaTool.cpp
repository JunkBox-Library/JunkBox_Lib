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
    length  = 0;
    state   = 0;
    gp      = NULL;

    memset(hd, 0, TGA_HEADER_SIZE);
    memset(ft, 0, TGA_FOOTER_SIZE);

    int len = (int)strlen(TGA_FOOTER_STR) + 1;
    int pos = TGA_FOOTER_SIZE - len;
    memcpy(ft + pos, TGA_FOOTER_STR, len);

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
    if (gp!=NULL) memset(gp, 0, xs*ys*col);
    return;
}


/**
void  TGAImage::fill(uByte v) 
*/
void  TGAImage::fill(uByte v) 
{
    if (gp!=NULL) memset(gp, v, xs*ys*col);
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
void   TGAImage::setzero(int x, int y, int c) 
*/
void   TGAImage::setzero(int x, int y, int c) 
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
    memset(gp, 0, x*y*c);
    
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
読み込んだデータは非圧縮状態．

@param  fp  読み込むファイルの記述子

@return TGAImage データ．gp==NULL の場合，@b state に情報が入る．
@retval JBXL_GRAPH_OPFILE_ERROR @b state: ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR @b state: 不正ファイル（TGAファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR @b state: メモリエラー
@retval JBXL_GRAPH_IVDFMT_ERROR @b state: サポート外のデータ形式
*/
TGAImage  jbxl::readTGAData(FILE* fp)
{
    TGAImage tga;
    int rle = FALSE;

    fseek(fp, 0, 0);
    tga.free();

    size_t ret = fread(&tga.hd, TGA_HEADER_SIZE, 1, fp);
    if (ret<=0) {
        DEBUG_MODE PRINT_MESG("JBXL::readTGAData: ERROR: File read Error\n");
        tga.state = JBXL_FILE_READ_ERROR;
        return tga;
    }

    int kind = (int)tga.hd[2];
    if (kind == 2) {
        tga.col = 3;
    }
    else if (kind == 3) {
        tga.col = 1;
    }
    else if (kind == 10) {
        tga.col = 3;
        rle = TRUE;
    }
    else if (kind == 11) {
        tga.col = 1;
        rle = TRUE;
    }
    else {
        DEBUG_MODE PRINT_MESG("JBXL::readTGAData: ERROR: no supported File Format (%d)\n", kind);
        tga.state = JBXL_GRAPH_IVDFMT_ERROR;
        return tga;
    }    

    if (is_little_endian()) {
        tga.xs = tga.hd[12] + tga.hd[13]*256;
        tga.ys = tga.hd[14] + tga.hd[15]*256;
    }
    else {
        tga.xs = tga.hd[12]*256 + tga.hd[13];
        tga.ys = tga.hd[14]*256 + tga.hd[15];
    }

    uByte alpha = tga.hd[17] & 0x0f;        // αチャンネル
    if (alpha != 0x08 && alpha != 0x00) {
        DEBUG_MODE PRINT_MESG("JBXL::readTGAData: ERROR: unkonwn Alpha Channel (0x%02x)\n", alpha);
        tga.state = JBXL_GRAPH_IVDFMT_ERROR;
        return tga;
    }
    else if (alpha == 0x08) {
        tga.col++;
    }
    if (tga.col*8 != (int)tga.hd[16]) {
        tga.col = (int)tga.hd[16]/8;
        PRINT_MESG("JBXL::readTGAData: Warning: Not Match Color Num! set color num = %d\n", tga.col);
    }
    PRINT_MESG("JBXL::readTGAData: TGA File (%d, %d, %d)\n", tga.xs, tga.ys, tga.col);

    int datasize = tga.xs*tga.ys*tga.col;
    tga.length = datasize;

    tga.gp = (uByte*)malloc(datasize);
    if (tga.gp==NULL) {
        PRINT_MESG("JBXL::readTGAData: ERROR: out of Memory!\n");
        tga.state = JBXL_GRAPH_MEMORY_ERROR;
        return tga;
    }
    memset(tga.gp, 0, datasize);

    fseek(fp, (int)tga.hd[0], SEEK_CUR);
    if (rle) {
        // RLE
        int   size = 0;
        uByte chunk;
        uByte buf[LBUF];

        while (size<datasize && !feof(fp)) {
            ret = fread(&chunk, 1, 1, fp);
            if (ret<=0) {
                DEBUG_MODE PRINT_MESG("JBXL::readTGAData: ERROR: File read Error\n");
                tga.state = JBXL_FILE_READ_ERROR;
                return tga;
            }
            int rep = (int)(chunk & 0x7f) + 1;
            if (chunk & 0x80) {
                ret = fread(buf, tga.col, 1, fp);
                if (ret<=0) {
                    tga.state = JBXL_FILE_READ_ERROR;
                    return tga;
                }
                for (int j=0; j<rep; j++) {
                    for (int i=0; i<tga.col; i++) tga.gp[size++] = buf[i];
                }
            }
            else {
                if (tga.col*rep<LBUF) {
                    ret = fread(buf, tga.col*rep, 1, fp);
                    if (ret<=0) {
                        tga.state = JBXL_FILE_READ_ERROR;
                        return tga;
                    }
                    for (int i=0; i<tga.col*rep; i++) tga.gp[size++] = buf[i];
                }
                else {
                    for (int j=0; j<rep; j++) {
                        ret = fread(buf, tga.col, 1, fp);
                        if (ret<=0) {
                            tga.state = JBXL_FILE_READ_ERROR;
                            return tga;
                        }
                        for (int i=0; i<tga.col; i++) tga.gp[size++] = buf[i];
                    }
                }
            }
        }
        if (size!=datasize) {
            DEBUG_MODE PRINT_MESG("JBXL::readTGAData: ERROR: unpack RLE failed. (%d != %d), format = %d\n", size, datasize, kind);
            tga.state = JBXL_GRAPH_FILESZ_ERROR;
            return tga;
        }
        tga.hd[2] &= ~0x08; //  非圧縮状態
    }
    else {
        ret = fread(tga.gp, datasize, 1, fp);
        if (ret<=0) {
            tga.state = JBXL_FILE_READ_ERROR;
            return tga;
        }
    }
    if (!feof(fp)) {
        ret = fread(tga.ft, TGA_FOOTER_SIZE, 1, fp);
        if (ret<=0) {
            tga.state = JBXL_FILE_READ_ERROR;
            return tga;
        }
    }
    tga.state = 0;
    return tga;
}


/**
int  jbxl::writeTGAFile(const char* fname, TGAImage* tga)

tga の画像データを fnameに書き出す．

@param  fname  ファイル名
@param  tga    保存する TGAデータ

@retval 0                          正常終了
@retval JBXL_GRAPH_OPFILE_ERROR    ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR    不正ファイル（TGAファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR    メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR    tga にデータが無い
@retval JBXL_GRAPH_IVDARG_ERROR    ファイル名が NULL
@retval JBXL_GRAPH_IVDCOLOR_ERROR  サポート外のチャンネル数
*/
int  jbxl::writeTGAFile(const char* fname, TGAImage* tga)
{
    FILE*  fp;
    int    ret;

    if (fname==NULL) return JBXL_GRAPH_IVDARG_ERROR;
    if (tga->col<=0 || tga->col>4) return JBXL_GRAPH_IVDCOLOR_ERROR;
    if (tga->gp==NULL) return JBXL_GRAPH_NODATA_ERROR;

    fp = fopen(fname, "wb");
    if (fp==NULL) {
        return JBXL_GRAPH_OPFILE_ERROR;
    }

    ret = writeTGAData(fp, tga);
    fclose(fp); 

    return ret;
}


/**
int  jbxl::writeTGAData(FILE* fp, TGAImage* tga)

tga の画像データを fpに書き出す．

@param  fp     ファイル記述子
@param  tga    保存する TGAデータ

@retval 0                          正常終了
@retval JBXL_GRAPH_OPFILE_ERROR    ファイルオープンエラー
@retval JBXL_GRAPH_HEADER_ERROR    不正ファイル（TGAファイルでない？）
@retval JBXL_GRAPH_MEMORY_ERROR    メモリエラー
@retval JBXL_GRAPH_NODATA_ERROR    tga にデータが無い
@retval JBXL_GRAPH_IVDCOLOR_ERROR  サポート外のチャンネル数
@retval JBXL_GRAPH_IVDFMT_ERROR    サポート外のデータ形式
*/
int  jbxl::writeTGAData(FILE* fp, TGAImage* tga)
{
    if (fp==NULL) return JBXL_GRAPH_OPFILE_ERROR;
    if (tga->col<=0 || tga->col>4) return JBXL_GRAPH_IVDCOLOR_ERROR;
    if (tga->gp==NULL) return JBXL_GRAPH_NODATA_ERROR;

    fwrite(tga->hd, TGA_HEADER_SIZE, 1, fp);
    fwrite(tga->gp, tga->length, 1, fp);
    fwrite(tga->ft, TGA_FOOTER_SIZE, 1, fp);

    return 0;
}


/**
int  jbxl::setupTGAData(TGAImage* tga, bool rle)

TGAのヘッダを設定し直して，必要なら RLEを行う．

@param  tga    TGAデータへのポインタ
@param  rle    RLE（連長圧縮）を行うかどうか

@retval 0      正常終了
@retval <0     エラーコード
*/
int  jbxl::setupTGAData(TGAImage* tga, bool rle)
{
    if (tga->col<=0 || tga->col>4) return JBXL_GRAPH_IVDCOLOR_ERROR;

    memset(tga->hd, 0, TGA_HEADER_SIZE);
    if      (tga->col==3 || tga->col==4) tga->hd[2] = 2;    // Full Color
    else if (tga->col==1 || tga->col==2) tga->hd[2] = 3;    // Gray Scale
    else return JBXL_GRAPH_IVDFMT_ERROR;

    if (is_little_endian()) {
        tga->hd[12] = (uByte)(tga->xs%256);
        tga->hd[13] = (uByte)(tga->xs/256);
        tga->hd[14] = (uByte)(tga->ys%256);
        tga->hd[15] = (uByte)(tga->ys/256);
    }
    else {
        tga->hd[12] = (uByte)(tga->xs/256);
        tga->hd[13] = (uByte)(tga->xs%256);
        tga->hd[14] = (uByte)(tga->ys/256);
        tga->hd[15] = (uByte)(tga->ys%256);
    }

    tga->hd[16] = tga->col*8;                   // depth
    tga->hd[17] = 0x20;                         // 0x20: Y方向:Top->Down
    tga->hd[10] = tga->hd[14];                  // Y posion for 0x20
    tga->hd[11] = tga->hd[15];                  // 
    if (tga->col==2 || tga->col==4) {
        tga->hd[17] |= 0x08;                    // 0x08: αチャンネル深度
    }
    tga->length = tga->xs*tga->ys*tga->col;     // データ（gp）長

    if (rle && tga->gp!=NULL) {
        int len = tga->xs*tga->ys;
        uByte* index = (uByte*)malloc(len);
        if (index==NULL) return JBXL_GRAPH_MEMORY_ERROR;
        memset(index, 0, len);

        int idx = 0;
        int p = 0;
        while (p<len-1) {
            if (!memcmp(tga->gp + p*tga->col, tga->gp + (p+1)*tga->col, tga->col)) {
                index[idx]++;
                if (index[idx]==127) {
                    idx = p + 2;
                    p++;
                }
            }
            else {
                idx = p + 1;
            }
            p++;
        }

        // 変換後のサイズを計算
        int n = 0;
        p = 0;
        while (p < len-1) {
            n += tga->col + 1;
            if (index[p]!=0x00) p += index[p];
            p++;
        }

        if ((float)n < len*tga->col*0.8f) {
            DEBUG_MODE PRINT_MESG("JBXL::writeTGAData: exec RLE (%d -> %d)\n", len*tga->col, n);
            uByte* buf = (uByte*)malloc(len*tga->col);
            if (buf==NULL) {
                ::free(index);
                return JBXL_GRAPH_MEMORY_ERROR;
            }
            memset(buf, 0, len*tga->col);
            //
            int i = 0;
            int j = 0;
            while (i < len-1) {
                if (index[i]!=0x00) {
                    buf[j++] = 0x80 + index[i];
                    memcpy(buf+j, tga->gp + i*tga->col, tga->col);
                    j += tga->col;
                    i += index[i];
                }
                else {
                    buf[j++] = 0x00;
                    memcpy(buf+j, tga->gp + i*tga->col, tga->col);
                    j += tga->col;
                }
                i++;
            }
            //
            if (j==n) {
                tga->hd[2] |= 0x08;
                tga->length = n;
                ::free(tga->gp);
                tga->gp = buf;
            }
            else {
                DEBUG_MODE PRINT_MESG("JBXL::writeTGAData: Warning: missmatch RLE size (%d != %d)\n", j, n);
                DEBUG_MODE PRINT_MESG("JBXL::writeTGAData: Warning: stop RLE!\n");
                ::free(buf);
            }
            ::free(index);
        }
    }
    return 0;
}

