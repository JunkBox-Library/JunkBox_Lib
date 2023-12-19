
#ifndef  __JBXL_CPP_WINDOW_H_
#define  __JBXL_CPP_WINDOW_H_



/**
@brief    グラフィック用ワールド座標系サポートヘッダ
@file     Window.h
@version  2.1
@author   Fumi.Iseki (C)
*/



#include "Graph.h"



//
namespace jbxl {


/*
template <typename T> void  wSetPixel(MSGraph<T> vp, double x, double y, int cc)
template <typename T> T     wGetPixel(MSGraph<T> vp, double x, double y)
template <typename T> void  wLine(MSGraph<T> vp, double x1, double y1, double x2, double y2, int cc)

template <typename T> void  wDraw(MSGraph<T>* vp, double x, double y, int cc)
template <typename T> void  wDraw_rel(MSGraph<T>* vp, double x, double y, int cc)

template <typename T> void  wSetPixel3D(MSGraph<T> vp, double x, double y, double z, int cc)
template <typename T> T     wGetPixel3D(MSGraph<T> vp, double x, double y, double z)
template <typename T> void  wLine3D(MSGraph<T> vp, double x1, double y1, double z1, double x2, double y2, double z2, int cc)

template <typename T> void  wDraw3D(MSGraph<T>* vp, double x, double y, double z, int cc)
template <typename T> void  wDraw_rel3D(MSGraph<T>* vp, double x, double y, double z, int cc)
*/



/**
template <typename T> void  wSetPixel(MSGraph<T> vp, double x, double y, int cc)

ワールド座標系に点を打つ．

@param  vp  操作対象のグラフィックデータ．
@param  x   点の x座標(ワールド座標系)．
@param  y   点の y座標(ワールド座標系)．
@param  cc  点の輝度値．
*/
template <typename T> void  wSetPixel(MSGraph<T> vp, double x, double y, int cc)
{
    int i = (int)((x - vp.wZeroX)*vp.wRateX);
    int j = (int)((vp.wZeroY - y)*vp.wRateY);

    if (i>=0 && i<vp.xs && j>=0 && j<vp.ys) vp.point(i, j) = (T)cc;
}



/**
template <typename T> void  wSetPixel3D(MSGraph<T> vp, double x, double y, double z, int cc)

3次元のワールド座標系に点を打つ．

@param  vp  操作対象のグラフィックデータ．
@param  x   点の x座標(ワールド座標系)．
@param  y   点の y座標(ワールド座標系)．
@param  z   点の z座標(ワールド座標系)．
@param  cc  点の輝度値．
*/
template <typename T> void  wSetPixel3D(MSGraph<T> vp, double x, double y, double z, int cc)
{
    int i = (int)((vp.wZeroX - x)*vp.wRateX);
    int j = (int)((y - vp.wZeroY)*vp.wRateY);
    int k = (int)((vp.wZeroZ - z)*vp.wRateZ);

    if (i>=0 && i<vp.xs && j>=0 && j<vp.ys && k>=0 && k<vp.zs) vp.point(i, j, k) = (T)cc;
}



/**
template <typename T> T wGetPixel(MSGraph<T> vp, double x, double y)

ワールド座標系の点のデータを得る．

@param  vp  操作対象のグラフィックデータ．
@param  x   点の x座標(ワールド座標系)．
@param  y   点の y座標(ワールド座標系)．
*/
template <typename T> T wGetPixel(MSGraph<T> vp, double x, double y)
{
    int i = (int)((x - vp.wZeroX)*vp.wRateX);
    int j = (int)((vp.wZeroY - y)*vp.wRateY);

    if (i>=0 && i<vp.xs && j>=0 && j<vp.ys) return vp.point(i, j);
    else  return vp.zero;
}



/**
template <typename T> T     wGetPixel3D(MSGraph<T> vp, double x, double y, double z)

3次元のワールド座標系の点のデータを得る．

@param  vp  操作対象のグラフィックデータ．
@param  x   点の x座標(ワールド座標系)．
@param  y   点の y座標(ワールド座標系)．
@param  z   点の z座標(ワールド座標系)．
*/
template <typename T> T     wGetPixel3D(MSGraph<T> vp, double x, double y, double z)
{
    int i = (int)((vp.wZeroX - x)*vp.wRateX);
    int j = (int)((y - vp.wZeroY)*vp.wRateY);
    int k = (int)((vp.wZeroZ - z)*vp.wRateZ);

    if (i>=0 && i<vp.xs && j>=0 && j<vp.ys && k>=0 && k<vp.zs) return vp.point(i, j, k);
    else  return vp.zero;
}



/**
template <typename T> void  wLine(MSGraph<T> vp, double x1, double y1, double x2, double y2, int cc)

ワールド座標系に線を引く．

@param  vp  操作対象のグラフィックデータ．
@param  x1  線の始点の x座標(ワールド座標系)．
@param  y1  線の始点の y座標(ワールド座標系)．
@param  x2  線の終点の x座標(ワールド座標系)．
@param  y2  線の終点の y座標(ワールド座標系)．
@param  cc  線の輝度値．
*/
template <typename T> void  wLine(MSGraph<T> vp, double x1, double y1, double x2, double y2, int cc)
{
    int  i1, j1, i2, j2;

    i1 = (int)((x1 - vp.wZeroX)*vp.wRateX);
    i2 = (int)((x2 - vp.wZeroX)*vp.wRateX);
    j1 = (int)((vp.wZeroY - y1)*vp.wRateY);
    j2 = (int)((vp.wZeroY - y2)*vp.wRateY);

    MSGraph_Line<T>(vp, i1, j1, i2, j2, cc);
}



/**
template <typename T> void  wLine3D(MSGraph<T> vp, double x1, double y1, double z1, double x2, double y2, double z2, int cc)

3次元のワールド座標系に線を引く．

@param  vp  操作対象のグラフィックデータ．
@param  x1  線の始点の x座標(ワールド座標系)．
@param  y1  線の始点の y座標(ワールド座標系)．
@param  z1  線の始点の z座標(ワールド座標系)．
@param  x2  線の終点の x座標(ワールド座標系)．
@param  y2  線の終点の y座標(ワールド座標系)．
@param  z2  線の終点の z座標(ワールド座標系)．
@param  cc  線の輝度値．
*/
template <typename T> void  wLine3D(MSGraph<T> vp, double x1, double y1, double z1, double x2, double y2, double z2, int cc)
{
    int  i1, j1, k1, i2, j2, k2;

    i1 = (int)((vp.wZeroX - x1)*vp.wRateX);
    i2 = (int)((vp.wZeroX - x2)*vp.wRateX);
    j1 = (int)((y1 - vp.wZeroY)*vp.wRateY);
    j2 = (int)((y2 - vp.wZeroY)*vp.wRateY);
    k1 = (int)((vp.wZeroZ - z1)*vp.wRateZ);
    k2 = (int)((vp.wZeroZ - z2)*vp.wRateZ);

    MSGraph_Line3D<T>(vp, i1, j1, k1, i2, j2, k2, cc);
}



template <typename T> void  wCircle3D(MSGraph<T> vp, Vector<double> ox, Vector<double> ex, double rr, int cc, int mode)
{
    double rate = Max(vp.wRateX, vp.wRateY);
    rate = Max(rate, vp.wRateZ);
    int r = (int)(rr*rate);

    ox.x = (vp.wZeroX - ox.x)*vp.wRateX;
    ox.y = (ox.y - vp.wZeroY)*vp.wRateY;
    ox.z = (vp.wZeroZ - ox.z)*vp.wRateZ;

    ex.x = -ex.x*vp.wRateX;
    ex.y =  ex.y*vp.wRateY;
    ex.z = -ex.z*vp.wRateZ;

    MSGraph_Circle3D(vp, ox, ex, r, cc, mode);
}



/**
template <typename T> void  wDraw(MSGraph<T>* vp, double x, double y, int cc)

ワールド座標系で現地点から指定した地点へ線を引く．指定した地点が現地点となる．

@param  vp  操作対象のグラフィックデータ．
@param  x   線の終点の x座標(ワールド座標系)．
@param  y   線の終点の y座標(ワールド座標系)．
@param  cc  線の輝度値．
*/
template <typename T> void  wDraw(MSGraph<T>* vp, double x, double y, int cc)
{
    wLine<T>(*vp, vp->wNowX, vp->wNowY, x, y, cc);
    vp->wMove(x, y);
}



/**
template <typename T> void  wDraw3D(MSGraph<T>* vp, double x, double y, double z, int cc)

3次元のワールド座標系で現地点から指定した地点へ線を引く．指定した地点が現地点となる．

@param  vp  操作対象のグラフィックデータ．
@param  x   線の終点の x座標(ワールド座標系)．
@param  y   線の終点の y座標(ワールド座標系)．
@param  z   線の終点の z座標(ワールド座標系)．
@param  cc  線の輝度値．
*/
template <typename T> void  wDraw3D(MSGraph<T>* vp, double x, double y, double z, int cc)
{
    wLine3D<T>(*vp, vp->wNowX, vp->wNowY, vp->wNowZ, x, y, z, cc);
    vp->wMove(x, y, z);
}



/**
template <typename T> void  wDraw_rel(MSGraph<T>* vp, double x, double y, int cc)

ワールド座標系で現地点を起点として相対的に線を引く．線の終点が現地点となる．

@param  vp  操作対象のグラフィックデータ．
@param  x   現地点から x方向への移動距離(ワールド座標系)．
@param  y   現地点から y方向への移動距離(ワールド座標系)．
@param  cc  線の輝度値．
*/
template <typename T> void  wDraw_rel(MSGraph<T>* vp, double x, double y, int cc)
{
    double x2 = vp->wNowX + x;
    double y2 = vp->wNowY + y;

    wLine<T>(*vp, vp->wNowX, vp->wNowY, x2, y2, cc);
    vp->wMove(x2, y2);
}



/**
template <typename T> void  wDraw_rel3D(MSGraph<T>* vp, double x, double y, double z, int cc)

3次元のワールド座標系で現地点を起点として相対的に線を引く．線の終点が現地点となる．

@param  vp  操作対象のグラフィックデータ．
@param  x   現地点から x方向への移動距離(ワールド座標系)．
@param  y   現地点から y方向への移動距離(ワールド座標系)．
@param  z   現地点から z方向への移動距離(ワールド座標系)．
@param  cc  線の輝度値．
*/
template <typename T> void  wDraw_rel3D(MSGraph<T>* vp, double x, double y, double z, int cc)
{
    double x2 = vp->wNowX + x;
    double y2 = vp->wNowY + y;
    double z2 = vp->wNowZ + z;

    wLine3D<T>(*vp, vp->wNowX, vp->wNowY, vp->wNowZ, x2, y2, z2, cc);
    vp->wMove(x2, y2, z2);
}



}       // namespace


#endif
