
/**
@brief    グラフィックライブラリ for C++
@file     Graph.cpp
@author   Fumi.Iseki (C)

*/


#include "Graph.h"
 

using namespace jbxl;


/**
void  jbxl::rotate_point(int& x, int& y, double sxc, double syc, double dxc, double dyc, double cst, double snt)

@param[in,out] x    in: X座標．out: 回転後のX座標．
@param[in,out] y    in: Y座標．out: 回転後のY座標．
@param         sxc  回転前の座標の中心の X座標
@param         syc  回転前の座標の中心の Y座標
@param         dxc  回転後の座標の中心の X座標
@param         dyc  回転後の座標の中心の Y座標
@param         cst  回転角 cosθ
@param         snt  回転角 sinθ
*/
void  jbxl::rotate_point(int& x, int& y, double sxc, double syc, double dxc, double dyc, double cst, double snt)
{
    double a, b, u, t;

    a = x - sxc;
    b = syc - y;
    u = a*cst - b*snt;
    t = a*snt + b*cst;
    x = (int)(u + dxc + 0.5);
    y = (int)(dyc - t + 0.5);

    return;
}



/**
void  jbxl::rotate_point_angle(int& x, int& y, double sxc, double syc, double dxc, double dyc, double th)

@param[in,out] x    in: X座標．out: 回転後のX座標．
@param[in,out] y    in: Y座標．out: 回転後のY座標．
@param         sxc  回転前の座標の中心の X座標
@param         syc  回転前の座標の中心の Y座標
@param         dxc  回転後の座標の中心の X座標
@param         dyc  回転後の座標の中心の Y座標
@param         th   回転角（ラジアン）
*/
void  jbxl::rotate_point_angle(int& x, int& y, double sxc, double syc, double dxc, double dyc, double th)
{
    double cst = (double)cos(th);
    double snt = (double)sin(th);

    rotate_point(x, y, sxc, syc, dxc, dyc, cst, snt);

    return;
}


