
/**
@brief   グラフィック用ワールド座標系サポート
@file    window.c
@version 2.1
@author  Fumi.Iseki (C)
*/


#include "window.h"


double  X_Dx, X_Dy, X_Wx, X_Wy;
double  X_Now = 0.0, Y_Now = 0.0;


/**
void  setWindow(WSGraph vp, double x1, double y1, double x2, double y2)

2Dのワールド座標系に表示用の窓(ビューポート)を設定する．

@param  vp  窓(ビューポート)を割り付ける 2Dグラフィックデータ．
@param  x1  窓(ビューポート)の一つの角の座標の x成分．
@param  y1  窓(ビューポート)の一つの角の座標の y成分．
@param  x2  窓(ビューポート)の(x1,y1)の対角の座標の x成分．
@param  y2  窓(ビューポート)の(x1,y1)の対角の座標の y成分．
*/
void  setWindow(WSGraph vp, double x1, double y1, double x2, double y2)
{
    if (x1==x2 || y1==y2) return;

    X_Dx = (vp.xs - 1)/Xabs(x2 - x1);
    X_Dy = (vp.ys - 1)/Xabs(y2 - y1);
    X_Wx = Min(x1, x2);
    X_Wy = Max(y1, y2);
}



/**
void  wSetPixel(WSGraph vp, double x, double y, int cc)

ワールド座標系に点を打つ．

@param  vp  操作対象のグラフィックデータ．
@param  x   点の x座標(ワールド座標系)．
@param  y   点の y座標(ワールド座標系)．
@param  cc  点の輝度値．
*/
void  wSetPixel(WSGraph vp, double x, double y, int cc)
{
    int  i, j;

    i = (int)((x - X_Wx)*X_Dx + 0.5);
    j = (int)((X_Wy - y)*X_Dy + 0.5);

    SetPixel(vp, i, j, cc);
}



/**
int  wGetPixel(WSGraph vp, double x, double y)

ワールド座標系に点を打つ．

@param  vp  操作対象のグラフィックデータ．
@param  x   点の x座標(ワールド座標系)．
@param  y   点の y座標(ワールド座標系)．

@return 点の輝度値．
*/
int  wGetPixel(WSGraph vp, double x, double y)
{
    int  i, j;

    i = (int)((x - X_Wx)*X_Dx + 0.5);
    j = (int)((X_Wy - y)*X_Dy + 0.5);

    return GetPixel(vp, i, j);
}



/**
void  wLine(WSGraph vp, double x1, double y1, double x2, double y2, int cc)

ワールド座標系に線を引く．

@param  vp  操作対象のグラフィックデータ．
@param  x1  線の始点の x座標(ワールド座標系)．
@param  y1  線の始点の y座標(ワールド座標系)．
@param  x2  線の終点の x座標(ワールド座標系)．
@param  y2  線の終点の y座標(ワールド座標系)．
@param  cc  線の輝度値．
*/
void  wLine(WSGraph vp, double x1, double y1, double x2, double y2, int cc)
{
    int  i1, j1, i2, j2;

    i1 = (int)((x1 - X_Wx)*X_Dx + 0.5);
    i2 = (int)((x2 - X_Wx)*X_Dx + 0.5);
    j1 = (int)((X_Wy - y1)*X_Dy + 0.5);
    j2 = (int)((X_Wy - y2)*X_Dy + 0.5);

    line(vp, i1, j1, i2, j2, cc);
}



/**
void  wMove(double x1, double y1) 

ワールド座標系で現地点を移動する(線は引かない)．

@param  x1  移動する地点の x座標(ワールド座標系)．
@param  y1  移動する地点の y座標(ワールド座標系)．
*/
void  wMove(double x1, double y1) 
{
    X_Now = x1;
    Y_Now = y1;
}



/**
void  wDraw(WSGraph vp, double x1, double y1, int cc)

ワールド座標系で現地点から指定した地点へ線を引く．
指定した地点が現地点となる．

@param  vp  操作対象のグラフィックデータ．
@param  x1  線の終点の x座標(ワールド座標系)．
@param  y1  線の終点の y座標(ワールド座標系)．
@param  cc  線の輝度値．
*/
void  wDraw(WSGraph vp, double x1, double y1, int cc)
{
    wLine(vp, X_Now, Y_Now, x1, y1, cc);

    X_Now = x1;
    Y_Now = y1;
}



/**
void  wMove_Rel(double x1, double y1) 

ワールド座標系で相対的に現地点を移動する(線は引かない)．

@param  x1  現地点から x方向への移動距離(ワールド座標系)．
@param  y1  現地点から y方向への移動距離(ワールド座標系)．
*/
void  wMove_Rel(double x1, double y1) 
{
    X_Now += x1;
    Y_Now += y1;
}



/**
void  wDraw_Rel(WSGraph vp, double x1, double y1, int cc)

ワールド座標系で現地点を起点として相対的に線を引く．
線の終点が現地点となる．

@param  vp  操作対象のグラフィックデータ．
@param  x1  現地点から x方向への移動距離(ワールド座標系)．
@param  y1  現地点から x方向への移動距離(ワールド座標系)．
@param  cc  線の輝度値．
*/
void  wDraw_Rel(WSGraph vp, double x1, double y1, int cc)
{
    double x2, y2;
 
    x2 = X_Now + x1;
    y2 = Y_Now + y1;

    wLine(vp, X_Now, Y_Now, x2, y2, cc);

    X_Now = x2;
    Y_Now = y2;
}


