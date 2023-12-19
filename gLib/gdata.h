
#ifndef  __JBXL_GRAPHIC_DATA_H_
#define  __JBXL_GRAPHIC_DATA_H_


/**
@brief    グラフィックデータ定義用ヘッダ
@file     gdata.h
@version  3.0
@author   Fumi.Iseki (C) 
@date     2002 8/27
*/


#include "gheader.h"
#include "matrix.h"


////////////////////////////////////////////////////////////////////////////////////////////////
// グラフィックデータの構造 (C)

/**
BSGRraph :Memory type (メモリ内で使用する型）

符号有りバイト型(1Byte)シンプルグラフィックデータ． 
*/
typedef struct {
    int    xs;          ///< xサイズ. 4Byte.
    int    ys;          ///< yサイズ. 4Byte.
    int    zs;          ///< zサイズ. 4Byte. 2Dの場合は 1.
    int    state;       ///< 状態
    uByte* gp;          ///< グラフィックデータへのポインタ. xs*ys*zs*1Byte.
} BSGraph;


/**
WSGRraph :Memory type (メモリ内で使用する型）

符号有りワード型(2Byte)シンプルグラフィックデータ．@n
プログラム中での画像データのデフォルト．
*/
typedef struct {
    int    xs;          ///< xサイズ. 4Byte.
    int    ys;          ///< yサイズ. 4Byte.
    int    zs;          ///< zサイズ. 4Byte. 2Dの場合は 1.
    int    state;       ///< 状態
    sWord* gp;          ///< グラフィックデータへのポインタ. xs*ys*zs*2Byte.
} WSGraph;


/**
ISGRraph :Memory type (メモリ内で使用する型）

整数型シンプルグラフィックデータ． 
*/
typedef struct {
    int    xs;          ///< xサイズ. 4Byte. 
    int    ys;          ///< yサイズ. 4Byte.
    int    zs;          ///< zサイズ. 4Byte. 2Dの場合は 1.
    int    state;       ///< 状態
    int*   gp;          ///< グラフィックデータへのポインタ. xs*ys*zs*4Byte
} ISGraph;


/**
FSGRraph :Memory type (メモリ内で使用する型）

実数型シンプルグラフィックデータ． 
*/
typedef struct {
    int    xs;          ///< xサイズ. 4Byte. 
    int    ys;          ///< yサイズ. 4Byte.
    int    zs;          ///< zサイズ. 4Byte. 2Dの場合は 1.
    int    state;       ///< 状態
    double* gp;         ///< グラフィックデータへのポインタ. xs*ys*zs*sizeof(double)
} FSGraph;


/**
VSGRraph :Memory type (メモリ内で使用する型）

ベクトル型シンプルグラフィックデータ． 
*/
typedef struct {
    int    xs;          ///< xサイズ. 4Byte.
    int    ys;          ///< yサイズ. 4Byte.
    int    zs;          ///< zサイズ. 4Byte. 2Dの場合は 1.
    int    state;       ///< 状態
    vector *gp;         ///< グラフィックデータへのポインタ. xs*ys*zs*sizeof(vector).
} VSGraph;


/**
MSGRraph :Memory type (メモリ内で使用する型）

汎用型シンプルグラフィックデータ． 
*/
typedef struct {
    int    xs;          ///< xサイズ. 4Byte.
    int    ys;          ///< yサイズ. 4Byte.
    int    zs;          ///< zサイズ. 4Byte. 2Dの場合は 1.
    int    depth;       ///< Color Depth
    int    state;       ///< 状態
    unsigned char *gp;  ///< グラフィックデータへのポインタ. xs*ys*zs*depth.
} MSGraph;


/**
境界構造体  IRBound 

グラフィックデータの様々な境界情報を保存する型． 
*/
typedef struct {
    int    xmin;        ///< x軸境界の最小値．
    int    xmax;        ///< x軸境界の最大値．
    int    ymin;        ///< y軸境界の最小値．
    int    ymax;        ///< y軸境界の最大値．
    int    zmin;        ///< z軸境界の最小値．
    int    zmax;        ///< z軸境界の最大値．
    int    misc;        ///< 多目的用．
} IRBound;


typedef struct {
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    int    misc;
} DRBound;


extern  int     ZeroBase;
extern  double  RZxy;
extern  int     ChkRZxy;


WSGraph B2WSGraph(BSGraph vp);
WSGraph I2WSGraph(ISGraph vp);
WSGraph F2WSGraph(FSGraph vp);
ISGraph W2ISGraph(WSGraph vp);
FSGraph W2FSGraph(WSGraph vp);
WSGraph V2WSGraph(VSGraph vp);
FSGraph V2FSGraph(VSGraph vp);

BSGraph make_BSGraph(int xs, int ys, int zs);
WSGraph make_WSGraph(int xs, int ys, int zs);
FSGraph make_FSGraph(int xs, int ys, int zs);
ISGraph make_ISGraph(int xs, int ys, int zs);
VSGraph make_VSGraph(int xs, int ys, int zs);
MSGraph make_MSGraph(int xs, int ys, int zs, int depth);

BSGraph* new_BSGraph(int xs, int ys, int zs);
WSGraph* new_WSGraph(int xs, int ys, int zs);
FSGraph* new_FSGraph(int xs, int ys, int zs);
ISGraph* new_ISGraph(int xs, int ys, int zs);
VSGraph* new_VSGraph(int xs, int ys, int zs);
MSGraph* new_MSGraph(int xs, int ys, int zs, int depth);

WSGraph zoom_WSGraph(WSGraph vp, int zm, int mode);
WSGraph grab_WSGraph(WSGraph vp, int x1, int y1, int x2, int y2);
void    copy_WSGraph(WSGraph src, WSGraph dst);

int     chk_RZxy(void);
void    set_RZxy(double rzm);
void    init_IRBound(IRBound* rb);
void    init_DRBound(DRBound* rb);
#define unset_RZxy()    {ChkRZxy=OFF;}

void    free_CmnHead(CmnHead* hd);      // 共通ヘッダのメモリ領域を開放する
void    init_CmnHead(CmnHead* hd);


#define  free_Graph(v)   {if((v)!=NULL){if((v)->gp)free((v)->gp); (v)->gp=NULL; (v)->xs=(v)->ys=(v)->zs=0; (v)->state=JBXL_NORMAL;}}    ///< vのバッファ部を解放する． 
#define  free_WSGraph(v) {if((v)!=NULL){if((v)->gp)free((v)->gp); (v)->gp=NULL; (v)->xs=(v)->ys=(v)->zs=0; (v)->state=JBXL_NORMAL;}}
#define  free_BSGraph(v) {if((v)!=NULL){if((v)->gp)free((v)->gp); (v)->gp=NULL; (v)->xs=(v)->ys=(v)->zs=0; (v)->state=JBXL_NORMAL;}}
#define  free_FSGraph(v) {if((v)!=NULL){if((v)->gp)free((v)->gp); (v)->gp=NULL; (v)->xs=(v)->ys=(v)->zs=0; (v)->state=JBXL_NORMAL;}}
#define  free_ISGraph(v) {if((v)!=NULL){if((v)->gp)free((v)->gp); (v)->gp=NULL; (v)->xs=(v)->ys=(v)->zs=0; (v)->state=JBXL_NORMAL;}}
#define  free_VSGraph(v) {if((v)!=NULL){if((v)->gp)free((v)->gp); (v)->gp=NULL; (v)->xs=(v)->ys=(v)->zs=0; (v)->state=JBXL_NORMAL;}}
#define  free_MSGraph(v) {if((v)!=NULL){if((v)->gp)free((v)->gp); (v)->gp=NULL; (v)->xs=(v)->ys=(v)->zs=(v)->depth=0; (v)->state=JBXL_NORMAL;}}

#define  del_Graph(v)    {if((v)!=NULL && *(v)!=NULL) {if((*(v))->gp)free((*(v))->gp); free((*(v)); *(v)=NULL}}
#define  del_WSGraph(v)  {if((v)!=NULL && *(v)!=NULL) {if((*(v))->gp)free((*(v))->gp); free((*(v)); *(v)=NULL}} 
#define  del_BSGraph(v)  {if((v)!=NULL && *(v)!=NULL) {if((*(v))->gp)free((*(v))->gp); free((*(v)); *(v)=NULL}}
#define  del_FSGraph(v)  {if((v)!=NULL && *(v)!=NULL) {if((*(v))->gp)free((*(v))->gp); free((*(v)); *(v)=NULL}}
#define  del_ISGraph(v)  {if((v)!=NULL && *(v)!=NULL) {if((*(v))->gp)free((*(v))->gp); free((*(v)); *(v)=NULL}}
#define  del_VSGraph(v)  {if((v)!=NULL && *(v)!=NULL) {if((*(v))->gp)free((*(v))->gp); free((*(v)); *(v)=NULL}}
#define  del_MSGraph(v)  {if((v)!=NULL && *(v)!=NULL) {if((*(v))->gp)free((*(v))->gp); free((*(v)); *(v)=NULL}}

#define  Lx(v, i)           ((v).gp[(i)])                                   ///< 1次元画像データ vの (i) のデータを参照する．
#define  Px(v, i, j)        ((v).gp[(i)+(v).xs*(j)])                        ///< 2次元画像データ vの (i, j) のデータを参照する．
#define  Vx(v, i, j, k)     ((v).gp[(i)+(v).xs*(j)+(v).xs*(v).ys*(k)])      ///< 3次元画像データ vの (i, j, k) のデータを参照する．

#define  pLx(v, i)          ((v)->gp[(i)])                                  ///< 1次元画像データ vの (i) のデータを参照する．
#define  pPx(v, i, j)       ((v)->gp[(i)+(v)->xs*(j)])                      ///< 2次元画像データ vの (i, j) のデータを参照する．
#define  pVx(v, i, j, k)    ((v)->gp[(i)+(v)->xs*(j)+(v)->xs*(v)->ys*(k)])  ///< 3次元画像データ vの (i, j, k) のデータを参照する．

#define  Lxt(v, i)          ((v).gp[((i)-1)])                               ///< 座標を 1 から数える．
#define  Pxt(v, i, j)       ((v).gp[((i)-1)+(v).xs*((j)-1)])
#define  Vxt(v, i, j, k)    ((v).gp[((i)-1)+(v).xs*((j)-1)+(v).xs*(v).ys*((k)-1)])

#define  pLxt(v, i)         ((v)->gp[((i)-1)])                               ///< 座標を 1 から数える．
#define  pPxt(v, i, j)      ((v)->gp[((i)-1)+(v)->xs*((j)-1)])
#define  pVxt(v, i, j, k)   ((v)->gp[((i)-1)+(v)->xs*((j)-1)+(v)->xs*(v).ys->((k)-1)])


#endif      // __JBXL_GRAPHIC_DATA_H_

