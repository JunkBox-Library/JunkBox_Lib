
/**
@brief    グラフィックデータ定義
@file     gdata.c
@version  3.0
@author   Fumi.Iseki (C) 
@date     2002 8/1
*/


#include "gdata.h"
#include "jbxl_state.h"


int     ZeroBase = 0;
double  RZxy     = 1.0;     // 3D 画像での x,y 対 z方向比　(z方向のピクセル間隔)/(x,y方向のピクセル間隔）
int     ChkRZxy  = OFF;     // RZxy　が設定されているなら ON, 設定されていないなら OFF 


/**
void  free_CmnHead(CmnHead* hd)

共通ヘッダのメモリ領域を開放する．cmnHead.kind は HEADER_NONE になる．
init_CmnHead()を内包している．

@param  hd  開放する共通ヘッダ
*/
void  free_CmnHead(CmnHead* hd)
{
    if (hd!=NULL) {
        if (hd->buf!=NULL)   free(hd->buf);
        if (hd->grptr!=NULL) free(hd->grptr);
        init_CmnHead(hd);
    }
}



/**
void  init_CmnHead(CmnHead* hd)

共通ヘッダのメモリ領域を初期化する．cmnHead.kind は HEADER_NONE になる．

@param  hd  初期化する共通ヘッダ
*/
void  init_CmnHead(CmnHead* hd)
{
    if (hd!=NULL) {
        memset(hd, 0, sizeof(CmnHead));
        hd->kind = HEADER_NONE;
    }
}



/**
BSGraph  make_BSGraph(int xs, int ys, int zs)

Byte型単純グラフィックデータ(BSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．
 
@return BSGraphグラフィックデータ構造体．
        メモリ領域を獲得できなかった場合,メモリ領域へのポインタは NULLとなる．
*/
BSGraph  make_BSGraph(int xs, int ys, int zs)
{
    int  i;
    BSGraph  vp;

    memset(&vp, 0, sizeof(BSGraph));
    if (xs==0 || ys==0) {
        vp.state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp.xs = xs;
    vp.ys = ys;
    if (zs>0) vp.zs = zs;
    else      vp.zs = 1;
    vp.state = JBXL_NORMAL;

    vp.gp = (uByte*)malloc(vp.xs*vp.ys*vp.zs*sizeof(uByte));
    if (vp.gp==NULL) {
        memset(&vp, 0, sizeof(BSGraph));
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) vp.gp[i] = 0;

    return vp;
}



/**
WSGraph  make_WSGraph(int xs, int ys, int zs)

sWord型単純グラフィックデータ(WSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．
 
@return WSGraphグラフィックデータ構造体．メモリ領域を獲得できなかった場合,
        メモリ領域へのポインタは NULLとなる．
*/
WSGraph  make_WSGraph(int xs, int ys, int zs)
{
    int  i;
    WSGraph  vp;

    memset(&vp, 0, sizeof(WSGraph));
    if (xs==0 || ys==0) {
        vp.state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp.xs = xs;
    vp.ys = ys;
    if (zs>0) vp.zs = zs;
    else      vp.zs = 1;
    vp.state = JBXL_NORMAL;

    vp.gp = (sWord*)malloc(vp.xs*vp.ys*vp.zs*sizeof(sWord));
    if (vp.gp==NULL) {
        memset(&vp, 0, sizeof(WSGraph));
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) vp.gp[i] = 0;

    return vp;
}



/**
FSGraph  make_FSGraph(int xs, int ys, int zs)

double 型単純グラフィックデータ(FSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．
 
@return FSGraphグラフィックデータ構造体．メモリ領域を獲得できなかった場合,
        メモリ領域へのポインタは NULLとなる．
*/
FSGraph  make_FSGraph(int xs, int ys, int zs)
{
    int  i;
    FSGraph  vp;

    memset(&vp, 0, sizeof(FSGraph));
    if (xs==0 || ys==0) {
        vp.state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp.xs = xs;
    vp.ys = ys;
    if (zs>0) vp.zs = zs;
    else      vp.zs = 1;
    vp.state = JBXL_NORMAL;

    vp.gp = (double*)malloc(vp.xs*vp.ys*vp.zs*sizeof(double));
    if (vp.gp==NULL) {
        memset(&vp, 0, sizeof(FSGraph));
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) vp.gp[i] = 0.0;

    return vp;
}



/**
ISGraph  make_ISGraph(int xs, int ys, int zs)

int型単純グラフィックデータ(ISGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．
 
@return ISGraphグラフィックデータ構造体．メモリ領域を獲得できなかった場合,
        メモリ領域へのポインタは NULLとなる．
*/
ISGraph  make_ISGraph(int xs, int ys, int zs)
{
    int  i;
    ISGraph  vp;

    memset(&vp, 0, sizeof(ISGraph));
    if (xs==0 || ys==0) {
        vp.state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp.xs = xs;
    vp.ys = ys;
    if (zs>0) vp.zs = zs;
    else      vp.zs = 1;
    vp.state = JBXL_NORMAL;

    vp.gp = (int*)malloc(vp.xs*vp.ys*vp.zs*sizeof(int));
    if (vp.gp==NULL) {
        memset(&vp, 0, sizeof(ISGraph));
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) vp.gp[i] = 0;

    return vp;
}



/**
VSGraph  make_VSGraph(int xs, int ys, int zs)

vector型単純グラフィックデータ(VSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．

@return VSGraphグラフィックデータ構造体．メモリ領域を獲得できなかった場合,
        メモリ領域へのポインタは NULLとなる．
*/
VSGraph  make_VSGraph(int xs, int ys, int zs)
{
    int  i;
    VSGraph  vp;

    memset(&vp, 0, sizeof(VSGraph));
    if (xs==0 || ys==0) {
        vp.state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp.xs = xs;
    vp.ys = ys;
    if (zs>0) vp.zs = zs;
    else      vp.zs = 1;
    vp.state = JBXL_NORMAL;

    vp.gp = (vector*)malloc(xs*ys*zs*sizeof(vector));
    if (vp.gp==NULL) {
        memset(&vp, 0, sizeof(VSGraph));
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<xs*ys*zs; i++) vp.gp[i] = set_vector(0.0, 0.0, 0.0);

    return vp;
}



/**
MSGraph  make_MSGraph(int xs, int ys, int zs, int depth)

単純グラフィックデータ(MSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs     確保するグラフィックデータの x方向のサイズ．
@param  ys     確保するグラフィックデータの y方向のサイズ．
@param  zs     確保するグラフィックデータの z方向のサイズ．
@param  depth  カラーデプス（bit)

@return MSGraphグラフィックデータ構造体．メモリ領域を獲得できなかった場合,
        メモリ領域へのポインタは NULLとなる．
*/
MSGraph  make_MSGraph(int xs, int ys, int zs, int depth)
{
    MSGraph  vp;

    memset(&vp, 0, sizeof(MSGraph));
    if (xs==0 || ys==0) {
        vp.state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp.xs = xs;
    vp.ys = ys;
    if (zs>0) vp.zs = zs;
    else      vp.zs = 1;
    if (depth>0) vp.depth = depth;
    else         vp.depth = 1;
    vp.state = JBXL_NORMAL;

    vp.gp = (unsigned char*)malloc(xs*ys*zs*((depth+7)/8));
    if (vp.gp==NULL) {
        memset(&vp, 0, sizeof(MSGraph));
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    memset(vp.gp, 0, xs*ys*zs*((depth+7)/8));
    return vp;
}



/**
BSGraph* new_BSGraph(int xs, int ys, int zs)

Byte型単純グラフィックデータ(BSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．
 
@return BSGraphグラフィックデータ構造体へのポインタ．要 del_
        メモリ領域を獲得できなかった場合は NULLとなる．
*/
BSGraph* new_BSGraph(int xs, int ys, int zs)
{
    int  i;
    BSGraph* vp;

    vp = (BSGraph*)malloc(sizeof(BSGraph));
    if (vp==NULL) return NULL;
    memset(vp, 0, sizeof(BSGraph));

    if (xs==0 || ys==0) {
        vp->state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp->xs = xs;
    vp->ys = ys;
    if (zs>0) vp->zs = zs;
    else      vp->zs = 1;
    vp->state = JBXL_NORMAL;

    vp->gp = (uByte*)malloc(vp->xs*vp->ys*vp->zs*sizeof(uByte));
    if (vp->gp==NULL) {
        memset(vp, 0, sizeof(BSGraph));
        vp->state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<vp->xs*vp->ys*vp->zs; i++) vp->gp[i] = 0;

    return vp;
}



/**
WSGraph* new_WSGraph(int xs, int ys, int zs)

sWord型単純グラフィックデータ(WSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．
 
@return WSGraphグラフィックデータ構造体へのポインタ．
        メモリ領域を獲得できなかった場合は NULLとなる．
*/
WSGraph* new_WSGraph(int xs, int ys, int zs)
{
    int  i;
    WSGraph* vp;

    vp = (WSGraph*)malloc(sizeof(WSGraph));
    if (vp==NULL) return NULL;
    memset(vp, 0, sizeof(WSGraph));

    if (xs==0 || ys==0) {
        vp->state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp->xs = xs;
    vp->ys = ys;
    if (zs>0) vp->zs = zs;
    else      vp->zs = 1;
    vp->state = JBXL_NORMAL;

    vp->gp = (sWord*)malloc(vp->xs*vp->ys*vp->zs*sizeof(sWord));
    if (vp->gp==NULL) {
        memset(vp, 0, sizeof(WSGraph));
        vp->state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<vp->xs*vp->ys*vp->zs; i++) vp->gp[i] = 0;

    return vp;
}



/**
FSGraph* new_FSGraph(int xs, int ys, int zs)

double 型単純グラフィックデータ(FSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．
 
@return FSGraphグラフィックデータ構造体へのポインタ．
        メモリ領域を獲得できなかった場合は NULLとなる．
*/
FSGraph* new_FSGraph(int xs, int ys, int zs)
{
    int  i;
    FSGraph* vp;

    vp = (FSGraph*)malloc(sizeof(FSGraph));
    if (vp==NULL) return NULL;
    memset(vp, 0, sizeof(FSGraph));

    if (xs==0 || ys==0) {
        vp->state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp->xs = xs;
    vp->ys = ys;
    if (zs>0) vp->zs = zs;
    else      vp->zs = 1;
    vp->state = JBXL_NORMAL;

    vp->gp = (double*)malloc(vp->xs*vp->ys*vp->zs*sizeof(double));
    if (vp->gp==NULL) {
        memset(vp, 0, sizeof(FSGraph));
        vp->state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<vp->xs*vp->ys*vp->zs; i++) vp->gp[i] = 0.0;

    return vp;
}



/**
ISGraph* new_ISGraph(int xs, int ys, int zs)

int型単純グラフィックデータ(ISGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．
 
@return ISGraphグラフィックデータ構造体へのポインタ．
        メモリ領域を獲得できなかった場合は NULLとなる．
*/
ISGraph* new_ISGraph(int xs, int ys, int zs)
{
    int  i;
    ISGraph* vp;

    vp = (ISGraph*)malloc(sizeof(ISGraph));
    if (vp==NULL) return NULL;

    memset(vp, 0, sizeof(ISGraph));
    if (xs==0 || ys==0) {
        vp->state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp->xs = xs;
    vp->ys = ys;
    if (zs>0) vp->zs = zs;
    else      vp->zs = 1;
    vp->state = JBXL_NORMAL;

    vp->gp = (int*)malloc(vp->xs*vp->ys*vp->zs*sizeof(int));
    if (vp->gp==NULL) {
        memset(vp, 0, sizeof(ISGraph));
        vp->state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<vp->xs*vp->ys*vp->zs; i++) vp->gp[i] = 0;

    return vp;
}



/**
VSGraph* new_VSGraph(int xs, int ys, int zs)

vector型単純グラフィックデータ(VSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs  確保するグラフィックデータの x方向のサイズ．
@param  ys  確保するグラフィックデータの y方向のサイズ．
@param  zs  確保するグラフィックデータの z方向のサイズ．

@return VSGraphグラフィックデータ構造体へのポインタ．
        メモリ領域を獲得できなかった場合は NULLとなる．
*/
VSGraph* new_VSGraph(int xs, int ys, int zs)
{
    int  i;
    VSGraph* vp;

    vp = (VSGraph*)malloc(sizeof(VSGraph));
    if (vp==NULL) return NULL;
    memset(vp, 0, sizeof(VSGraph));

    if (xs==0 || ys==0) {
        vp->state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp->xs = xs;
    vp->ys = ys;
    if (zs>0) vp->zs = zs;
    else      vp->zs = 1;
    vp->state = JBXL_NORMAL;

    vp->gp = (vector*)malloc(vp->xs*vp->ys*vp->zs*sizeof(vector));
    if (vp->gp==NULL) {
        memset(vp, 0, sizeof(VSGraph));
        vp->state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (i=0; i<vp->xs*vp->ys*vp->zs; i++) vp->gp[i] = set_vector(0.0, 0.0, 0.0);

    return vp;
}



/**
MSGraph* new_MSGraph(int xs, int ys, int zs, int depth)

単純グラフィックデータ(MSGraph)のメモリ領域を確保する．
グラフィックデータ部分は 0クリアされる．

@param  xs     確保するグラフィックデータの x方向のサイズ．
@param  ys     確保するグラフィックデータの y方向のサイズ．
@param  zs     確保するグラフィックデータの z方向のサイズ．
@param  depth  カラーデプス（bit)

@return MSGraphグラフィックデータ構造体へのポインタ．
        メモリ領域を獲得できなかった場合は NULLとなる．
*/
MSGraph* new_MSGraph(int xs, int ys, int zs, int depth)
{
    MSGraph* vp;

    vp = (MSGraph*)malloc(sizeof(MSGraph));
    if (vp==NULL) return NULL;
    memset(vp, 0, sizeof(MSGraph));

    if (xs==0 || ys==0) {
        vp->state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    vp->xs = xs;
    vp->ys = ys;
    if (zs>0) vp->zs = zs;
    else      vp->zs = 1;
    if (depth>0) vp->depth = depth;
    else         vp->depth = 1;
    vp->state = JBXL_NORMAL;

    vp->gp = (unsigned char*)malloc(vp->xs*vp->ys*vp->zs*((depth+7)/8));
    if (vp->gp==NULL) {
        memset(vp, 0, sizeof(MSGraph));
        vp->state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    memset(vp->gp, 0, vp->xs*vp->ys*vp->zs*((depth+7)/8));
    return vp;
}



/**
ISGraph  W2ISGraph(WSGraph vp)

sWord型単純グラフィックデータ(WSGraph)を int型単純グラフィック
データ(ISGraph)に変換する．

@param  vp  変換する sWord型単純グラフィックデータ
@return 変換された int型単純グラフィックデータ(ISGraph)．
*/
ISGraph  W2ISGraph(WSGraph vp)
{
    int  i;
    ISGraph  ix;

    ix.xs = vp.xs;
    ix.ys = vp.ys;
    ix.zs = vp.zs;
    ix.state = vp.state;

    ix.gp = (int*)malloc(ix.xs*ix.ys*ix.zs*sizeof(int));
    if (ix.gp==NULL) {
        memset(&ix, 0, sizeof(ISGraph));
        ix.state = JBXL_GRAPH_MEMORY_ERROR;
        return ix;
    }

    for (i=0; i<ix.xs*ix.ys*ix.zs; i++) ix.gp[i] = (int)vp.gp[i];
    return ix;
}



/**
FSGraph  W2FSGraph(WSGraph vp)

sWord型単純グラフィックデータ(WSGraph)を double型単純グラフィック
データ(FSGraph)に変換する．

@param  vp  変換する sWord型単純グラフィックデータ
@return 変換された double型単純グラフィックデータ(FSGraph)．
*/
FSGraph  W2FSGraph(WSGraph vp)
{
    int  i;
    FSGraph  ix;

    ix.xs = vp.xs;
    ix.ys = vp.ys;
    ix.zs = vp.zs;
    ix.state = vp.state;

    ix.gp = (double*)malloc(ix.xs*ix.ys*ix.zs*sizeof(double));
    if (ix.gp==NULL) {
        memset(&ix, 0, sizeof(FSGraph));
        ix.state = JBXL_GRAPH_MEMORY_ERROR;
        return ix;
    }

    for (i=0; i<ix.xs*ix.ys*ix.zs; i++) ix.gp[i]=(double)vp.gp[i];
    return ix;
}



/**
WSGraph  B2WSGraph(BSGraph vp)

uByte型単純グラフィックデータ(ISGraph)を sWord型単純グラフィック
データ(WSGraph)に変換する．

@param  vp  変換する int型単純グラフィックデータ
@return 変換された sWord型単純グラフィックデータ(WSGraph)．
*/
WSGraph  B2WSGraph(BSGraph vp)
{
    int  i;
    WSGraph  ix;

    ix.xs = vp.xs;
    ix.ys = vp.ys;
    ix.zs = vp.zs;
    ix.state = vp.state;

    ix.gp = (sWord*)malloc(ix.xs*ix.ys*ix.zs*sizeof(sWord));
    if (ix.gp==NULL) {
        memset(&ix, 0, sizeof(WSGraph));
        ix.state = JBXL_GRAPH_MEMORY_ERROR;
        return ix;
    }

    for (i=0; i<ix.xs*ix.ys*ix.zs; i++) ix.gp[i] = (sWord)vp.gp[i];
    return ix;
}



/**
WSGraph  I2WSGraph(ISGraph vp)

int型単純グラフィックデータ(ISGraph)を sWord型単純グラフィック
データ(WSGraph)に変換する．

@param  vp   変換する int型単純グラフィックデータ
@return 変換された sWord型単純グラフィックデータ(WSGraph)．
*/
WSGraph  I2WSGraph(ISGraph vp)
{
    int  i;
    WSGraph  ix;

    ix.xs = vp.xs;
    ix.ys = vp.ys;
    ix.zs = vp.zs;
    ix.state = vp.state;

    ix.gp = (sWord*)malloc(ix.xs*ix.ys*ix.zs*sizeof(sWord));
    if (ix.gp==NULL) {
        memset(&ix, 0, sizeof(WSGraph));
        ix.state = JBXL_GRAPH_MEMORY_ERROR;
        return ix;
    }

    for (i=0; i<ix.xs*ix.ys*ix.zs; i++) ix.gp[i] = (sWord)vp.gp[i];
    return ix;
}



/**
WSGraph  F2WSGraph(FSGraph vp)

double型単純グラフィックデータ(FSGraph)を sWord型単純グラフィック
データ(WSGraph)に変換する．

@param  vp  変換する double型単純グラフィックデータ
@return 変換された sWord型単純グラフィックデータ(WSGraph)．
*/
WSGraph  F2WSGraph(FSGraph vp)
{
    int  i;
    WSGraph ix;

    ix.xs = vp.xs;
    ix.ys = vp.ys;
    ix.zs = vp.zs;
    ix.state = vp.state;

    ix.gp = (sWord*)malloc(ix.xs*ix.ys*ix.zs*sizeof(sWord));
    if (ix.gp==NULL) {
        memset(&ix, 0, sizeof(WSGraph));
        ix.state = JBXL_GRAPH_MEMORY_ERROR;
        return ix;
    }

    for (i=0; i<ix.xs*ix.ys*ix.zs; i++) ix.gp[i] = (sWord)vp.gp[i];
    return ix;
}



/**
WSGraph  V2FSGraph(VSGraph vp)

vector型単純グラフィックデータ(VSGraph)を double型単純グラフィック
データ(FSGraph)に変換する．

@param  vp   変換する vector型単純グラフィックデータ
@return 変換された double型単純グラフィックデータ(FSGraph)．
*/
FSGraph  V2FSGraph(VSGraph vp)
{
    int  i;
    FSGraph ix;

    ix = make_FSGraph(vp.xs, vp.ys, vp.zs);
    if (ix.gp==NULL) return ix;

    for (i=0; i<ix.xs*ix.ys*ix.zs; i++) ix.gp[i] = vp.gp[i].n;
    ix.state = vp.state;

    return ix;
}



/**
WSGraph  V2WSGraph(VSGraph vp)

vector型単純グラフィックデータ(VSGraph)を sWord型単純グラフィックデータ(WSGraph)に変換する．

@param  vp   変換する vector型単純グラフィックデータ
@return 変換された sWord型単純グラフィックデータ(WSGraph)．
*/
WSGraph  V2WSGraph(VSGraph vp)
{
    int  i;
    WSGraph  ix;

    ix = make_WSGraph(vp.xs, vp.ys, vp.zs);
    if (ix.gp==NULL) return ix;

    for (i=0; i<ix.xs*ix.ys*ix.zs; i++) ix.gp[i] = (sWord)vp.gp[i].n;
    ix.state = vp.state;

    return ix;
}



/**
void  init_IRBound(IRBound* rb)

境界データ構造体を初期化する．

@param[out]  rb  初期化する境界データ．
*/
void  init_IRBound(IRBound* rb)
{
    rb->xmax = rb->ymax = rb->zmax = 0;
    rb->xmin = rb->ymin = rb->zmin = SINTMAX;
    rb->misc = OFF;
}



/**
void  init_DRBound(DRBound* rb)

2D境界データ構造体を初期化する．

@param[out]  rb  初期化する境界データ．
*/
void  init_DRBound(DRBound* rb)
{
    rb->xmax = rb->ymax = rb->zmax = 0.0;
    rb->xmin = rb->ymin = rb->zmin = DBL_MAX;
    rb->misc = OFF;
}



/**
void  set_RZxy(double rzm)

z軸のひずみを設定する．デフォルトでは 1.0に設定されている．
設定を無効にするには void unset_RZxy(void) を使用する．

@param  rzm  ひずみの値．
*/
void  set_RZxy(double rzm)
{
    ChkRZxy = ON;
    RZxy = rzm;
    //fprintf(stderr,"set_RZxy: set RZxy to %f\n", rzm);
}



/**
int  chk_RZxy(void)   

z軸のひずみを設定が設定されているかどうかをチェックする．

*/
int  chk_RZxy(void)   
{
    if (ChkRZxy==OFF) {
        DEBUG_MODE PRINT_MESG("CHK_RZXY: WARNING: undefined RZxy!!!\n");
        //exit(1);
        return FALSE;
    }
    return TRUE;
}


