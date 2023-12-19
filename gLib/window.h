
#ifndef  __JBXL_WINDOW_H_
#define  __JBXL_WINDOW_H_


/**
@brief   グラフィック用ワールド座標系サポート ヘッダ
@file    window.h
@version 2.1
@author  Fumi.Iseki (C)
 */


#include "graph.h"


extern double  X_Dx;        ///< ワールド座標系の x成分1ドットに対するスクリーン座標系のドット数．
extern double  X_Dy;        ///< ワールド座標系の y成分1ドットに対するスクリーン座標系のドット数．
extern double  X_Wx;        ///< スクリーン座標系の原点に対するワールド座標系の x成分．
extern double  X_Wy;        ///< スクリーン座標系の原点に対するワールド座標系の y成分．

extern double  X_Now;       ///< ペンの現地点の x成分(ワールド座標系)
extern double  Y_Now;       ///< ペンの現地点の y成分(ワールド座標系)

void  setWindow(WSGraph vp, double x1, double y1, double x2, double y2);
void  wSetPixel(WSGraph vp, double x1, double y1, int cc);
int   wGetPixel(WSGraph vp, double x1, double y1);
void  wLine(WSGraph vp, double x1, double y1, double x2, double y2, int cc);

void  wMove(double x1, double y1);
void  wDraw(WSGraph vp, double x1, double y1, int cc);
void  wMove_Rel(double x1, double y1);
void  wDraw_Rel(WSGraph vp, double x1, double y1, int cc);


#endif  // __JBXL_WINDOW_H_

