
/**
@brief    グラフィックデータ定義
@file     Gdata.cpp   
@author   Fumi.Iseki (C)
*/

#include "Gdata.h"


using namespace jbxl;


int  jbxl::ZeroBase = 0;
int  jbxl::TempBase = 0;


/**
void  jbxl::free_CmnHead(CmnHead* hd)

共通ヘッダのメモリ領域を開放する．cmnHead.kind は HEADER_ERRになる@n
initCmnHead()を内包している．

@param  hd  開放する共通ヘッダ
*/
void  jbxl::free_CmnHead(CmnHead* hd)
{
    if (hd!=NULL) {
        if (hd->buf!=NULL)   free(hd->buf);
        if (hd->grptr!=NULL) free(hd->grptr);
        init_CmnHead(hd);
    }
}


/**
void  jbxl::init_CmnHead(CmnHead* hd)
   
共通ヘッダのメモリ領域を初期化する．cmnHead.kind は HEADER_ERRになる
   
@param  hd  初期化する共通ヘッダ
*/
void  jbxl::init_CmnHead(CmnHead* hd)
{  
    if (hd!=NULL) {
        hd->buf   = hd->grptr = NULL;
        hd->kind  = HEADER_NONE;
        hd->xsize = hd->ysize = hd->zsize = 0;
        hd->depth = hd->bsize = hd->lsize = 0;
    }
} 


/**
CmnHead  jbxl::getinfo_CmnHead(CmnHead hd)

ヘッダ情報のみをコピーする．
*/
CmnHead  jbxl::getinfo_CmnHead(CmnHead hd)
{
    CmnHead cmnhd = hd;

    cmnhd.bsize = 0;
    cmnhd.buf   = NULL;
    cmnhd.grptr = NULL;

    return cmnhd;
}
 

