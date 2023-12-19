
#ifndef  __JBXL_GRAPHIC_HEADER_
#define  __JBXL_GRAPHIC_HEADER_


/**
@brief    2D & 3D グラフィックライブラリ ヘッダ
@file     graph.h
@version  3.0
@author   Fumi.Iseki (C)
*/


#include "gdata.h"


void  local2world(WSGraph gd, WSGraph vp, vector ox, vector oz, vector ex, double* csf, double* snf);
void  topola(vector nv, double* cst, double* snt, double* csf, double* snf);
void  set_around(WSGraph vp, int cc);

void  bline    (BSGraph vp, int xs, int ys, int xe, int ye, int c);  
void  bline3d  (BSGraph vp, int xs, int ys, int zs, int xe, int ye, int ze, int c);

void  line     (WSGraph vp, int xs, int ys, int xe, int ye, int c);  
void  line3d   (WSGraph vp, int xs, int ys, int zs, int xe, int ye, int ze, int c);
void  circle   (WSGraph vp, int x, int y, int r, int c, int mode);
void  paint    (WSGraph vp, int x, int y, int mn, int mx, int c, int m);
void  paint3d  (WSGraph vp, int x, int y, int z, int mn, int mx, int c, int m);
void  _paint_3d(WSGraph vp, int x, int y, int z, int mn, int mx, int c, int m);

void  box     (WSGraph vp, int xs, int ys, int xe, int ye ,int c, int m); 
void  triangle(WSGraph vp, int x1, int y1, int x2, int y2, int x3, int y3, int c, int m); 

void  circle3d(WSGraph vp, vector a, vector b, int rr, int cc, int mode);  
void  sphere  (WSGraph vp, vector a, int r, int c, int mode);
void  pool    (WSGraph vp, vector a, vector b, int rr, int c);
void  torus   (WSGraph vp, vector a, vector b, int rr, int ra, int c);

WSGraph  x_reverse_wsg(WSGraph vp);
WSGraph  cut_object  (WSGraph vp, int cc, IRBound* rb, int blank);
WSGraph  zoom_WSGraph(WSGraph vp, int zm, int mode);
WSGraph  grab_WSGraph(WSGraph vp, int x1, int y1, int x2, int y2);
void     copy_WSGraph(WSGraph src, WSGraph dst);

int   get_wdat(WSGraph, double, double, double, IRBound);
int   get_idat(WSGraph, int, int, int);
void  set_wdat(WSGraph, double, double, double, int, IRBound);
void  set_idat(WSGraph, int, int, int, int);

int   get_bdat(BSGraph, int, int, int);
void  set_bdat(BSGraph, int, int, int, int);

int   isinctri(int x1, int y1, int x2, int y2, int x3, int y3, int xx, int yy);

#define  set_around_space(vp)    set_around((vp), 0)


/**
void SetPixel(WSGraph vp, int x, int y, int cc)

2Dグラフィックデータ構造体 vpの任意の 1pixelに値を設定する．@n
座標範囲のチェックあり．

@param  vp    操作対象となる2Dグラフィックデータ構造体．
@param  x, y  値を設定する xy座標．
@param  cc    点(x,y)に設定する点の値(濃度)．

使用例
@code
    SetPixel(vp, 5, 5, 100);      // 座標(5,5)に 100を設定
    SetPixel(vp, 100, 100, 200);  // 座標(100,100)に 200を設定
    SetPixel(vp, 100, 200, 10);   // 座標(100,200)に 10を設定
@endcode
*/
#define  SetPixel(vp, x, y, cc)  set_idat((vp), (x), (y), 0, (cc))


/**
int   GetPixel(WSGraph vp, int x, int y);

2Dグラフィックデータ構造体 vpの任意の 1pixelの値(濃度)を返す．@n
座標範囲のチェックあり．

@param  vp    操作対象となる2Dグラフィックデータ構造体．
@param  x, y  知りたい値の xy座標．

使用例
@code
    SetPixel(vp, 5, 5, 100);
    SetPixel(vp, 100, 100, 200);
    SetPixel(vp, 100, 200, 10);
    val = GetPixel(vp, 100, 100);  // 当然 val の値は 200
@endcode
*/
#define  GetPixel(vp, x, y)      get_idat((vp), (x), (y), 0)


/**
int  isCrossLine(x1, y1, x2, y2, x3, y3, x4, y4);

(x1,y1)-(x2,y2) の直線と (x3,y3)-(x4,y4) の線分が交差しているか判定する

@retval 1以上  交差しない
@retval 0      3か4 は直線上にある
@retval 0未満  交差する
*/
#define  isCrossLine(x1, y1, x2, y2, x3, y3, x4, y4) (((x1)-(x2))*((y3)-(y1))+((y1)-(y2))*((x1)-(x3)))*(((x1)-(x2))*((y4)-(y1))+((y1)-(y2))*((x1)-(x4)))


#endif      // __JBXL_GRAPHIC_HEADER_

