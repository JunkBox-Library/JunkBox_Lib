/**
@brief    STL ファイル入出力用ライブラリ
@file     STL.cpp
@author   Fumi.Iseki (C)

@note
エンディアン無変換
*/

#include "STL.h"


using namespace jbxl;


/**
STLData*  jbxl::readSTLFile(char* fname, unsigned int* fno)

STLファイルを読み込むラッパー関数．アスキー，バイナリを自動判別．

読み込んだファセットの数(fno)を返す．
*/
DllExport STLData*  jbxl::readSTLFile(char* fname, long int* fno)
{
    FILE*  fp;
    struct stat stbuf;
    STLData* stldata;
    unsigned int temp;

    fp = fopen(fname, "rb");
    if (fp==NULL) return NULL;
    fseek(fp, 80, SEEK_SET);
    fread(&temp, 4, 1, fp);
    fclose(fp);
    *fno = (long int)temp;

    stat(fname, &stbuf);
    if (stbuf.st_size == (*fno)*50+84) {    // ファイルサイズのチェック
        stldata = readSTLFileB(fname, fno); // バイナリファイル
    }
    else {
        stldata = readSTLFileA(fname, fno); // アスキーファイル
    }

    if (stldata==NULL) {
        if (*fno!=0) {
            DEBUG_MODE PRINT_MESG("readSTLFile: メモリの確保に失敗．\n");
        }
        else {
            DEBUG_MODE PRINT_MESG("readSTLFile: データの読み込みに失敗．\n");
        }
    }
    else {
        DEBUG_MODE PRINT_MESG("readSTLFile: file name = %s\n", fname);
        DEBUG_MODE PRINT_MESG("readSTLFile: read facet No. = %d\n", *fno);
    }

    return  stldata;
}


/**
int  jbxl::writeSTLFile (char* fname, BREP_SOLID* solid, bool ascii)

STLファイルを書き込むラッパー関数．

アスキー，バイナリを指定する．デフォルトはバイナリ(ascii==false)．
書き込んだファセットの数を返す．
*/
DllExport int  jbxl::writeSTLFile (char* fname, BREP_SOLID* solid, bool ascii)
{
    int nn;

    if (ascii) nn = writeSTLFileA(fname, solid);
    else       nn = writeSTLFileB(fname, solid);

    if (nn<0) DEBUG_MODE PRINT_MESG("writeSTLFile: ファイルオープンエラー．\n");
    return nn;
}


/**
DllExport int  jbxl::writeSTLFile (char* fname, BREP_SOLID_LIST solid_list, bool ascii)

STLファイルを書き込むラッパー関数．

アスキー，バイナリを指定する．デフォルトはバイナリ(ascii==false)．
書き込んだファセットの数を返す．
*/
DllExport int  jbxl::writeSTLFile (char* fname, BREP_SOLID_LIST solid_list, bool ascii)
{
    int nn;

    if (ascii) nn = writeSTLFileA(fname, solid_list);
    else       nn = writeSTLFileB(fname, solid_list);

    if (nn<0) DEBUG_MODE PRINT_MESG("writeSTLFile: ファイルオープンエラー．\n");
    return nn;
}


/**
STLData*  jbxl::readSTLFileA(char* fname, long int* fno)

アスキー形式の STLファイルを読み込み，読み込んだファセットの数を返す．@n
ファイル中の facet, vertex 以外は無視．
*/
DllExport STLData*  jbxl::readSTLFileA(char* fname, long int* fno)
{
    FILE* fp;
    STLData*  stldata = NULL;
    float  vect[3];
    char   buffer[LBUF], dummy[LBUF], *pbuf;
    int    vno=0;

    // ファイルをオープンしてデータ数を数える．
    *fno = 0;
    fp = fopen(fname, "r");
    if (fp==NULL) return NULL;

    fgets(buffer, LBUF, fp);
    while (!feof(fp)) {
        pbuf = buffer;
        while(*pbuf==' '||*pbuf==CHAR_TAB) pbuf++;

        if (!strncasecmp(pbuf, "facet ",  6)) (*fno)++;
        if (!strncasecmp(pbuf, "vertex ", 7)) vno++;
        fgets(buffer, LBUF, fp);
    }
    fclose(fp);

    // メモリの確保
    if (vno!=0 && vno==(*fno)*3) {
        stldata = (STLData*)malloc(sizeof(STLData)*(*fno));
    }
    if (stldata==NULL) {
        *fno = 0;
        return NULL;
    }

    // もう一度ファイルをオープンして，データを読み込む．
    int i=0, j=0;
    fp = fopen(fname, "r");
    if (fp==NULL) return NULL;

    fgets(buffer, LBUF, fp);
    while (!feof(fp)) {
        pbuf = buffer;
        while(*pbuf==' '||*pbuf==CHAR_TAB) pbuf++;

        if (!strncasecmp(pbuf, "facet ", 6)) {                      // 法線ベクトル
            sscanf(buffer, "%s %s %f %f %f", dummy, dummy, &vect[0], &vect[1], &vect[2]);
            for (int k=0; k<3; k++) stldata[i].vect[k] = vect[k];
            j = 3;
        }
        else if (!strncasecmp(pbuf, "vertex ", 7) && j>=3 && j<12) {// 座標
            sscanf(buffer, "%s %f %f %f", dummy, &vect[0], &vect[1], &vect[2]);
            for (int k=0; k<3; k++) stldata[i].vect[j+k] = vect[k];
            j += 3;
            if (j==12) i++;
        }
        fgets(buffer, LBUF, fp);
    }
    fclose(fp);

    return  stldata;
}


/**
STLData*  jbxl::readSTLFileB(char* fname, long int* fno)

バイナリ形式の STLファイルを読み込み，読み込んだファセットの数を返す．
*/
DllExport STLData*  jbxl::readSTLFileB(char* fname, long int* fno)
{
    FILE* fp;
    char  message[81];          // STLのファイルメッセージ
    tmpSTLData* tmp_stldata;
    STLData*    stldata;
    STLData*    exdata;

    fp = fopen(fname, "rb");
    if (fp==NULL) return NULL;
    fread(message, 80, 1, fp);
    message[80] = '\0';         // STLのファイルメッセージ(message[]）80Byteは未使用

    fread(fno, 4, 1, fp);
    tmp_stldata = (tmpSTLData*)malloc(sizeof(tmpSTLData)*(*fno));
    if (tmp_stldata==NULL) return NULL;
    fread(tmp_stldata, sizeof(tmpSTLData), *fno, fp);
    fclose(fp);

    stldata = (STLData*)malloc(sizeof(STLData)*(*fno));
    if (stldata==NULL) {
        free(tmp_stldata);
        return NULL;
    }

    for (int i=0; i<(*fno); i++) {
        exdata = (STLData*)(&tmp_stldata[i]);       // アドレスの読み替え（50Byteの構造体を作れないため）
        for (int j=0; j<12; j++) {
            stldata[i].vect[j] = exdata->vect[j];   // データの移動
        }
    }

    free(tmp_stldata);
    return  stldata;
}


/**
int  jbxl::writeSTLFileA(char* fname, BREP_SOLID* solid)

BREP_SOLID のデータをアスキー形式の STLファイルとして書き込む
*/
DllExport int  jbxl::writeSTLFileA(char* fname, BREP_SOLID* solid)
{
    FILE* fp;
    int   nn = 0;

    fp = fopen(fname, "w");
    if (fp==NULL) return -1;

    fprintf(fp, "solid %s\n", fname);

    BREP_CONTOUR_LIST::iterator icon;
    for (icon=solid->contours.begin(); icon!=solid->contours.end(); icon++){
        fprintf(fp, "facet normal %g %g %g\n", (*icon)->normal.x, (*icon)->normal.y, (*icon)->normal.z);
        fprintf(fp, "    outer loop\n");

        BREP_WING* wing = (*icon)->wing;
        for (int i=0; i<3; i++) {
            Vector<double> vect = wing->vertex->point;
            fprintf(fp, "        vertex %g %g %g\n", vect.x, vect.y, vect.z);
            wing = wing->next;
        }
        fprintf(fp,"    endloop\n");
        fprintf(fp,"endfacet\n");
        nn++;
    }
    fprintf(fp, "endsolid %s\n", fname);
    fclose(fp);

    return nn;
}


/**
int  jbxl::writeSTLFileB(char* fname, BREP_SOLID* solid)

BREP_SOLID のデータをバイナリ形式の STLファイルとして書き込む
*/
DllExport int  jbxl::writeSTLFileB(char* fname, BREP_SOLID* solid)
{
    FILE* fp;
    int   fno;
    char  message[80]="STL Binary Data Program  by Fumi.Iseki";
    STLData  stldata;

    fp = fopen(fname, "wb");
    if (fp==NULL) return -1;

    fno = (int)solid->contours.size();
    fwrite(message, 80, 1, fp);
    fwrite(&fno, 4, 1, fp);

    BREP_CONTOUR_LIST::iterator icon;
    for (icon=solid->contours.begin(); icon!=solid->contours.end(); icon++){
        stldata.vect[0] = (float)(*icon)->normal.x;
        stldata.vect[1] = (float)(*icon)->normal.y;
        stldata.vect[2] = (float)(*icon)->normal.z;

        BREP_WING* wing = (*icon)->wing;
        for (int i=0; i<3; i++) {
            Vector<double> vect = wing->vertex->point;
            stldata.vect[3*i+3] = (float)vect.x;
            stldata.vect[3*i+4] = (float)vect.y;
            stldata.vect[3*i+5] = (float)vect.z;
            wing = wing->next;
        }
        fwrite(&stldata, 50, 1, fp);
    }
    fclose(fp);

    return fno;
}


/**
DllExport int  jbxl::writeSTLFileA(char* fname, BREP_SOLID_LIST solid_list)

BREP_SOLID のデータをアスキー形式の STLファイルとして書き込む
*/
DllExport int  jbxl::writeSTLFileA(char* fname, BREP_SOLID_LIST solid_list)
{
    FILE* fp;
    int   nn = 0;

    fp = fopen(fname, "wa");
    if (fp==NULL) return -1;

    fprintf(fp, "solid %s\n", fname);

    BREP_SOLID_LIST::iterator isolid;
    for (isolid=solid_list.begin(); isolid!=solid_list.end(); isolid++){
        //
        BREP_CONTOUR_LIST::iterator icon;
        for (icon=(*isolid)->contours.begin(); icon!=(*isolid)->contours.end(); icon++){
            fprintf(fp, "facet normal %g %g %g\n", (*icon)->normal.x, (*icon)->normal.y, (*icon)->normal.z);
            fprintf(fp, "    outer loop\n");

            BREP_WING* wing = (*icon)->wing;
            for (int i=0; i<3; i++) {
                Vector<double> vect = wing->vertex->point;
                fprintf(fp, "        vertex %g %g %g\n", vect.x, vect.y, vect.z);
                wing = wing->next;
            }
            fprintf(fp,"    endloop\n");
            fprintf(fp,"endfacet\n");
            nn++;
        }
    }
    fprintf(fp, "endsolid %s\n", fname);
    fclose(fp);

    return nn;
}


/**
DllExport int  jbxl::writeSTLFileB(char* fname, BREP_SOLID_LIST solidi_list)

BREP_SOLID のデータをバイナリ形式の STLファイルとして書き込む
*/
DllExport int  jbxl::writeSTLFileB(char* fname, BREP_SOLID_LIST solid_list)
{
    FILE* fp;
    int   fno = 0;
    char  message[80]="STL Binary Data Program  by Fumi.Iseki";
    STLData  stldata;

    fp = fopen(fname, "wb");
    if (fp==NULL) return -1;

    BREP_SOLID_LIST::iterator isolid;
    for (isolid=solid_list.begin(); isolid!=solid_list.end(); isolid++){
        fno += (int)((*isolid)->contours.size());
    }
    fwrite(message, 80, 1, fp);
    fwrite(&fno, 4, 1, fp);

    //
    for (isolid=solid_list.begin(); isolid!=solid_list.end(); isolid++){
        //
        BREP_CONTOUR_LIST::iterator icon;
        for (icon=(*isolid)->contours.begin(); icon!=(*isolid)->contours.end(); icon++){
            stldata.vect[0] = (float)(*icon)->normal.x;
            stldata.vect[1] = (float)(*icon)->normal.y;
            stldata.vect[2] = (float)(*icon)->normal.z;

            BREP_WING* wing = (*icon)->wing;
            for (int i=0; i<3; i++) {
                Vector<double> vect = wing->vertex->point;
                stldata.vect[3*i+3] = (float)vect.x;
                stldata.vect[3*i+4] = (float)vect.y;
                stldata.vect[3*i+5] = (float)vect.z;
                wing = wing->next;
            }
            fwrite(&stldata, 50, 1, fp);
        }
    }
    fclose(fp);

    return fno;
}


/**
void  jbxl::println_FacetAsciiSTL(BREP_CONTOUR* contour)

contourの情報を STLのアスキー形式で表示する
readSTLFileA()で読み込み可
*/
DllExport void  jbxl::println_FacetAsciiSTL(BREP_CONTOUR* contour)
{
    BREP_WING* wing = contour->wing;

    PRINT_MESG("facet normal %g %g %g\n", contour->normal.x, contour->normal.y, contour->normal.z);
    PRINT_MESG("outer loop\n");
    for (int i=0; i<3; i++) {
        Vector<double> vect = wing->vertex->point;
        PRINT_MESG("vertex %g %g %g\n", vect.x, vect.y, vect.z);
        wing = wing->next;
    }
    PRINT_MESG("endloop\n");
    PRINT_MESG("endfacet\n");
}


DllExport void  jbxl::freeSTL(STLData* stldata)
{
    if (stldata!=NULL) free(stldata);
}

