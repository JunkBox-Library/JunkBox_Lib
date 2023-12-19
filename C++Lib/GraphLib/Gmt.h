#ifndef  __JBXL_CPP_GRAPHIC_MATH_H_
#define  __JBXL_CPP_GRAPHIC_MATH_H_


/**
@brief    MSGraph用 数学ライブラリ ヘッダ in Graph Library
@file     Gmt.h
@author   Fumi.Iseki (C)
*/



#include "Gdata.h"


//
namespace jbxl {


/*
point() は遅いので使用しない．

template <typename R, typename T>  MSGraph<R> Laplacian(MSGraph<T> vp, int mode=0);

template <typename R, typename T>  MSGraph<R>  xSobel(MSGraph<T> vp);
template <typename R, typename T>  MSGraph<R>  ySobel(MSGraph<T> vp);
template <typename R, typename T>  MSGraph<R>  zSobel(MSGraph<T> vp);
template <typename R, typename T>  MSGraph<R> xxSobel(MSGraph<T> vp);
template <typename R, typename T>  MSGraph<R> yySobel(MSGraph<T> vp);
template <typename R, typename T>  MSGraph<R> zzSobel(MSGraph<T> vp);

template <typename R, typename T>  MSGraph<Vector<R> >  vNabla(MSGraph<T> vp);
template <typename R, typename T>  MSGraph<R>            Nabla(MSGraph<T> vp);

template <typename R, typename T>  MSGraph<R>  edgeEnhance(MSGraph<T> gd, int mode=0);
template <typename T> MSGraph<T>   medianFilter(MSGraph<T> xp, int ms=3);
template <typename T> MSGraph<int> euclidDistance(MSGraph<T> vp, int bc, int& rr);

template <typename R, typename T>  MSGraph<R>  MSMaskFilter(MSGraph<R> vp, MSGraph<T> filter, int abs=FALSE)
*/



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   テンプレート定義

/**
template <typename R, typename T> MSGraph<R>  Laplacian(MSGraph<T> vp, int mode=0)

2Dグラフィックデータのラプラシアンを計算する．

@param  vp    計算対象となるグラフィックデータ構造体．
@param  mode  モード．@b 4: 4近傍ラプラシアン @n
                      @b 8: 8近傍ラプラシアン @n
                      @b その他: Sobelのラプラシアン(24近傍)

@return 処理されたグラフィックデータ．

@note 3D化したい．
*/
template <typename R, typename T> MSGraph<R>  Laplacian(MSGraph<T> vp, int mode=0)
{
    int i, j;
    int nx, ny, xs, xs2;
    R   da, db, dc, dd, de, df, dg, dh;
    MSGraph<R> lp;
    
    lp.mimicry(vp);
    lp.base = lp.zero = 0;

    if (lp.isNull()) {
        DEBUG_MODE PRINT_MESG("LAPLACIAN: No More Memory!!!\n");
        lp.state = JBXL_GRAPH_MEMORY_ERROR;
        return lp;
    }

    xs  = vp.xs;
    xs2 = 2*xs;

    if (mode==4) {
        for (j=1; j<vp.ys-1; j++) {
            ny = j*vp.xs;
            for (i=1; i<vp.xs-1; i++) {
                nx = ny + i;
                da = vp.gp[nx+1]  + vp.gp[nx-1];
                db = vp.gp[nx];
                dc = vp.gp[nx+xs] + vp.gp[nx-xs];
                lp.gp[nx] = (R)(da - 4.*db + dc);
                //da = vp.point(i+1, j) + vp.point(i-1, j);
                //db = vp.point(i, j);
                //dc = vp.point(i, j+1) + vp.point(i, j-1);
                //lp.point(i, j) = (R)(da - 4.*db + dc);
            }
        }
    }

    else if (mode==8) {
        for (j=1; j<vp.ys-1; j++) {
            ny = j*vp.xs;
            for (i=1; i<vp.xs-1; i++) {
                nx = ny + i;
                da = vp.gp[nx+1]    + vp.gp[nx-1];
                db = vp.gp[nx+xs]   + vp.gp[nx-xs];
                dc = vp.gp[nx];
                dd = vp.gp[nx+1+xs] + vp.gp[nx-1+xs];
                de = vp.gp[nx+1-xs] + vp.gp[nx-1-xs];
                lp.gp[nx] = (R)(da + db - 8.*dc + dd + de);
                //da = vp.point(i+1, j)   + vp.point(i-1, j);
                //db = vp.point(i, j+1)   + vp.point(i, j-1);
                //dc = vp.point(i, j);
                //dd = vp.point(i+1, j+1) + vp.point(i-1, j+1);
                //de = vp.point(i+1, j-1) + vp.point(i-1, j-1);
                //lp.point(i, j) = (R)(da + db - 8.*dc + dd + de);
            }
        }
    }

    else {
        for (j=2; j<vp.ys-2; j++) {
            ny = j*vp.xs;
            for (i=2; i<vp.xs-2; i++) {
                nx = ny + i;
                da = vp.gp[nx];
                db = vp.gp[nx+1]     + vp.gp[nx-1]   + vp.gp[nx+xs]   + vp.gp[nx-xs];
                dc = vp.gp[nx-1-xs2] + vp.gp[nx-xs2] + vp.gp[nx+1-xs2];
                dd = vp.gp[nx-1+xs2] + vp.gp[nx+xs2] + vp.gp[nx+1+xs2];
                de = vp.gp[nx-2-xs ] + vp.gp[nx-2]   + vp.gp[nx-2+xs];
                df = vp.gp[nx+2-xs ] + vp.gp[nx+2]   + vp.gp[nx+2+xs];
                dg = vp.gp[nx-2-xs2] + vp.gp[nx+2-xs2];
                dh = vp.gp[nx-2+xs2] + vp.gp[nx+2+xs2];
                lp.gp[nx] = (R)((-12.*da - 4.*db + 2.*(dc+dd+de+df) + dg + dh)/32.);

                //da = vp.point(i, j);
                //db = vp.point(i+1, j)   + vp.point(i-1, j) + vp.point(i,   j+1)   + vp.point(i, j-1);
                //dc = vp.point(i-1, j-2) + vp.point(i, j-2) + vp.point(i+1, j-2);
                //dd = vp.point(i-1, j+2) + vp.point(i, j+2) + vp.point(i+1, j+2);
                //de = vp.point(i-2, j-1) + vp.point(i-2, j) + vp.point(i-2, j+1);
                //df = vp.point(i+2, j-1) + vp.point(i+2, j) + vp.point(i+2, j+1);
                //dg = vp.point(i-2, j-2) + vp.point(i+2, j-2);
                //dh = vp.point(i-2, j+2) + vp.point(i+2, j+2);
                //lp.point(i, j) = (R)((-12.*da - 4.*db + 2.*(dc+dd+de+df) + dg + dh)/32.);
            }
        }
    }

    return lp;
}


/**
template <typename R, typename T>  MSGraph<R>  xSobel(MSGraph<T> vp)

グラフィックデータの X方向微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
template <typename R, typename T>  MSGraph<R>  xSobel(MSGraph<T> vp)
{
    int  i, j, k;
    int  pl, nx, ny, nz;
    R    da, db, dc, dd, de, nr;
    MSGraph<R> xp;
 
    xp.mimicry(vp);
    xp.base = xp.zero = 0;

    if (xp.isNull()) {
        DEBUG_MODE PRINT_MESG("XSOBEL: No More Memory!!!\n");
        xp.state = JBXL_GRAPH_MEMORY_ERROR;
        return xp;
    }

    // カウンタ
    CVCounter*   vcounter = NULL;
    if (vp.zs>2) vcounter = GetUsableGlobalCounter();
    if (vcounter!=NULL) vcounter->SetMax(vp.zs);

    pl = vp.xs*vp.ys;
    for (k=0; k<vp.zs; k++) {
        nz = k*pl;
        for (j=1; j<vp.ys-1; j++) {
            ny = nz + j*vp.xs;
            for (i=1; i<vp.xs-1; i++) {
                nx = ny + i;
                da = vp.gp[nx+1-vp.xs] - vp.gp[nx-1-vp.xs];                 //  1/4 * (da+dc)
                db = vp.gp[nx+1]       - vp.gp[nx-1];                       //  1/2 * db
                dc = vp.gp[nx+1+vp.xs] - vp.gp[nx-1+vp.xs];
                //da = vp.point(i+1, j-1, k) - vp.point(i-1, j-1, k);       //  1/4 * (da+dc)
                //db = vp.point(i+1, j,   k) - vp.point(i-1, j,   k);       //  1/2 * db
                //dc = vp.point(i+1, j+1, k) - vp.point(i-1, j+1, k);

                if (k==0 || k==vp.zs-1) {
                    dd = de = 0;
                    nr = 8;
                }
                else {
                    dd = vp.gp[nx+1-pl] - vp.gp[nx-1-pl];                   // 1/4 * (dd+de)
                    de = vp.gp[nx+1+pl] - vp.gp[nx-1+pl];
                    //dd = vp.point(i+1, j, k-1) - vp.point(i-1, j, k-1);   // 1/4 * (dd+de)
                    //de = vp.point(i+1, j, k+1) - vp.point(i-1, j, k+1);
                    nr = 12;
                }

                xp.gp[nx] = (R)((da + 2.*db + dc + dd + de)/nr);
            }
        }
        if (vcounter!=NULL) vcounter->StepIt();
    }

    return xp;
}


/**
template<typename R, typename T>  MSGraph<R>  ySobel(MSGraph<T> vp)

グラフィックデータの Y方向微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
template<typename R, typename T>  MSGraph<R>  ySobel(MSGraph<T> vp)
{
    int  i, j, k;
    int  pl, nx, ny, nz;
    R    da, db, dc, dd, de, nr;
    MSGraph<R> xp;
 
    xp.mimicry(vp);
    xp.base = xp.zero = 0;

    if (xp.isNull()) {
        DEBUG_MODE PRINT_MESG("YSOBEL: No More Memory!!\n");
        xp.state = JBXL_GRAPH_MEMORY_ERROR;
        return xp;
    }

    // カウンタ
    CVCounter*   vcounter = NULL;
    if (vp.zs>2) vcounter = GetUsableGlobalCounter();
    if (vcounter!=NULL) vcounter->SetMax(vp.zs);

    pl = vp.xs*vp.ys;
    for (k=0; k<vp.zs; k++) {
        nz = k*pl;
        for (j=1; j<vp.ys-1; j++) {
            ny = nz + j*vp.xs;
            for (i=1; i<vp.xs-1; i++) {
                nx = ny + i;
                da = vp.gp[nx-1+vp.xs] - vp.gp[nx-1-vp.xs];
                db = vp.gp[nx  +vp.xs] - vp.gp[nx  -vp.xs];
                dc = vp.gp[nx+1+vp.xs] - vp.gp[nx+1-vp.xs];
                //da = vp.point(i-1, j+1, k) - vp.point(i-1, j-1, k);
                //db = vp.point(i  , j+1, k) - vp.point(i,   j-1, k);
                //dc = vp.point(i+1, j+1, k) - vp.point(i+1, j-1, k);

                if (k==0 || k==vp.zs-1) {
                    dd = de = 0;
                    nr = 8;
                }
                else {
                    dd = vp.gp[nx+vp.xs-pl] - vp.gp[nx-vp.xs-pl];
                    de = vp.gp[nx+vp.xs+pl] - vp.gp[nx-vp.xs+pl];
                    //dd = vp.point(i, j+1, k-1) - vp.point(i, j-1, k-1);
                    //de = vp.point(i, j+1, k+1) - vp.point(i, j-1, k+1);
                    nr = 12;
                }   

                xp.gp[nx] = (R)((da + 2.*db + dc + dd + de)/nr);
            }
        }
        if (vcounter!=NULL) vcounter->StepIt();
    }

    return xp;
}


/**
template<typename R, typename T>  MSGraph<R>  zSobel(MSGraph<T> vp)

グラフィックデータの Z方向微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
template<typename R, typename T>  MSGraph<R>  zSobel(MSGraph<T> vp)
{
    int  i, j, k;
    int  pl, nx, ny, nz;
    R    da, db, dc, dd, de;
    MSGraph<R> xp;

    xp.mimicry(vp);
    xp.base = xp.zero = 0;

    if (xp.isNull()) {
        DEBUG_MODE PRINT_MESG("ZSOBEL: No More Memory!!\n");
        xp.state = JBXL_GRAPH_MEMORY_ERROR;
        return xp;
    }
    if (vp.zs<2) {
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;      // 0 のグラフィックデータを返す
    }

    // カウンタ
    CVCounter*   vcounter = NULL;
    if (vp.zs>2) vcounter = GetUsableGlobalCounter();
    if (vcounter!=NULL) vcounter->SetMax(vp.zs-1);

    pl = vp.xs*vp.ys;
    for (k=1; k<vp.zs-1; k++) {
        nz = k*pl;
        for (j=1; j<vp.ys-1; j++) {
            ny = nz + j*vp.xs;
            for (i=1; i<vp.xs-1; i++) {
                nx = ny +i;
                da = vp.gp[nx-1+pl]     - vp.gp[nx-1-pl];
                db = vp.gp[nx+1+pl]     - vp.gp[nx+1-pl];
                dc = vp.gp[nx  +pl]     - vp.gp[nx  -pl];
                dd = vp.gp[nx-vp.xs+pl] - vp.gp[nx-vp.xs-pl];
                de = vp.gp[nx+vp.xs+pl] - vp.gp[nx+vp.xs-pl];
                //da = vp.point(i-1, j, k+1) - vp.point(i-1, j, k-1);
                //db = vp.point(i+1, j, k+1) - vp.point(i+1, j, k-1);
                //dc = vp.point(i,   j, k+1) - vp.point(i,   j, k-1);
                //dd = vp.point(i, j-1, k+1) - vp.point(i, j-1, k-1);
                //de = vp.point(i, j+1, k+1) - vp.point(i, j+1, k-1);
                xp.gp[nx] = (R)((da + db + 2.*dc + dd + de)/12.);
            }
        }
        if (vcounter!=NULL) vcounter->StepIt();
    }

    // k==0 and k==vp.zs-1
    nz = (vp.zs-1)*pl;
    for (j=1; j<vp.ys-1; j++) {
        ny = j*vp.xs;
        for (i=1; i<vp.xs-1; i++) {
            nx = ny + i;
            da = vp.gp[nx];
            db = vp.gp[nx+pl];
            dc = vp.gp[nx+1    +pl] + vp.gp[nx-1    +pl];
            dd = vp.gp[nx+vp.xs+pl] + vp.gp[nx-vp.xs+pl];
            //da = vp.point(i, j, 0);
            //db = vp.point(i, j, 1);       
            //dc = vp.point(i+1, j, 1) + vp.point(i-1, j, 1);
            //dd = vp.point(i, j+1, 1) + vp.point(i, j-1, 1);
            xp.gp[nx] = (R)((2.*db + dc + dd)/6. - da);
        }

        ny = ny + nz;
        for (i=1; i<vp.xs-1; i++) {
            nx = ny + i;
            da = vp.gp[nx];
            db = vp.gp[nx-pl];
            dc = vp.gp[nx+1    -pl] + vp.gp[nx-1    -pl];
            dd = vp.gp[nx+vp.xs-pl] + vp.gp[nx-vp.xs-pl];
            //da = vp.point(i, j, vp.zs-1);
            //db = vp.point(i, j, vp.zs-2);
            //dc = vp.point(i+1, j, vp.zs-2) + vp.point(i-1, j, vp.zs-2);
            //dd = vp.point(i, j+1, vp.zs-2) + vp.point(i, j-1, vp.zs-2);
            xp.gp[nx] = (R)(da - (2.*db + dc + dd)/6.);
        }
    }

    if (vcounter!=NULL) vcounter->PutFill();

    return xp;
}


/**
template<typename R, typename T>  MSGraph<R>  xxSobel(MSGraph<T> vp)

グラフィックデータの X方向の 2階微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
template<typename R, typename T>  MSGraph<R>  xxSobel(MSGraph<T> vp)
{
    int  i, j, k;
    int  pl, nx, ny, nz, pl2, xs, xs2;
    R    da, db, dc, dd, de;
    R    df, dg, dh, di, dj, dk, dl, dm, nr;
    MSGraph<R> xp;
   
    xp.mimicry(vp);
    xp.base = xp.zero = 0;

    if (xp.isNull()) {
        DEBUG_MODE PRINT_MESG("XXSOBEL: No More Memory!!\n");
        xp.state = JBXL_GRAPH_MEMORY_ERROR;
        return xp;
    }
    
    // カウンタ
    CVCounter*   vcounter = NULL;
    if (vp.zs>2) vcounter = GetUsableGlobalCounter();
    if (vcounter!=NULL) vcounter->SetMax(vp.zs);

    pl  = vp.xs*vp.ys;
    pl2 = 2*pl;
    xs  = vp.xs;
    xs2 = 2*vp.xs;

    for (k=0; k<vp.zs; k++) {
        nz = k*pl;
        for (j=2; j<vp.ys-2; j++) {
            ny = nz + j*vp.xs;
            for (i=2; i<vp.xs-2; i++) {
                nx = ny + i;
                da = vp.gp[nx+2-xs2] - 2*vp.gp[nx-xs2] + vp.gp[nx-2-xs2];
                db = vp.gp[nx+2-xs ] - 2*vp.gp[nx-xs]  + vp.gp[nx-2-xs];
                dc = vp.gp[nx+2]     - 2*vp.gp[nx]     + vp.gp[nx-2];
                dd = vp.gp[nx+2+xs]  - 2*vp.gp[nx+xs]  + vp.gp[nx-2+xs];
                de = vp.gp[nx+2+xs2] - 2*vp.gp[nx+xs2] + vp.gp[nx-2+xs2];
                //da = vp.point(i+2, j-2, k) - 2*vp.point(i, j-2, k) + vp.point(i-2, j-2, k);
                //db = vp.point(i+2, j-1, k) - 2*vp.point(i, j-1, k) + vp.point(i-2, j-1, k);
                //dc = vp.point(i+2, j,   k) - 2*vp.point(i, j,   k) + vp.point(i-2, j,   k);
                //dd = vp.point(i+2, j+1, k) - 2*vp.point(i, j+1, k) + vp.point(i-2, j+1, k);
                //de = vp.point(i+2, j+2, k) - 2*vp.point(i, j+2, k) + vp.point(i-2, j+2, k);

                if (k==0 || k==vp.zs-1) {
                    dc = (R)(6.*dc);
                    df = dg = dh = di = dj = dk = dl = dm = 0;
                    nr = 64;
                }
                else {
                    dc = (R)(8.*dc);
                    df = vp.gp[nx+2-xs-pl] - 2*vp.gp[nx-xs-pl] + vp.gp[nx-2-xs-pl];
                    dg = vp.gp[nx+2   -pl] - 2*vp.gp[nx   -pl] + vp.gp[nx-2   -pl];
                    dh = vp.gp[nx+2+xs-pl] - 2*vp.gp[nx+xs-pl] + vp.gp[nx-2+xs-pl];
                    di = vp.gp[nx+2-xs+pl] - 2*vp.gp[nx-xs+pl] + vp.gp[nx-2-xs+pl];
                    dj = vp.gp[nx+2   +pl] - 2*vp.gp[nx   +pl] + vp.gp[nx-2   +pl];
                    dk = vp.gp[nx+2+xs+pl] - 2*vp.gp[nx+xs+pl] + vp.gp[nx-2+xs+pl];
                    //df = vp.point(i+2, j-1, k-1) - 2*vp.point(i, j-1, k-1) + vp.point(i-2, j-1, k-1);
                    //dg = vp.point(i+2, j,   k-1) - 2*vp.point(i, j ,  k-1) + vp.point(i-2, j,   k-1);
                    //dh = vp.point(i+2, j+1, k-1) - 2*vp.point(i, j+1, k-1) + vp.point(i-2, j+1, k-1);
                    //di = vp.point(i+2, j-1, k+1) - 2*vp.point(i, j-1, k+1) + vp.point(i-2, j-1, k+1);
                    //dj = vp.point(i+2, j,   k+1) - 2*vp.point(i, j,   k+1) + vp.point(i-2, j,   k+1);
                    //dk = vp.point(i+2, j+1, k+1) - 2*vp.point(i, j+1, k+1) + vp.point(i-2, j+1, k+1);

                    if (k==1 || k==vp.zs-2) {
                        dl = dm = 0;
                        nr = 136;
                    }
                    else {
                        dl = vp.gp[nx+2-pl2] - 2*vp.gp[nx-pl2] + vp.gp[nx-2-pl2];
                        dm = vp.gp[nx+2+pl2] - 2*vp.gp[nx+pl2] + vp.gp[nx-2+pl2];\
                        //dl = vp.point(i+2, j, k-2) - 2*vp.point(i, j, k-2) + vp.point(i-2, j, k-2);
                        //dm = vp.point(i+2, j, k+2) - 2*vp.point(i, j, k+2) + vp.point(i-2, j, k+2);
                        nr = 144;
                    }
                }
                xp.gp[nx] = (R)((dc + 4.*(db+dd+dg+dj) + 2.*(df+dh+di+dk) + da+de+dl+dm)/nr);
            }
        }
        if (vcounter!=NULL) vcounter->StepIt();
    }
    
    return xp;
}


/**
template<typename R, typename T>  MSGraph<R>  yySobel(MSGraph<T> vp)

グラフィックデータの Y方向の 2階微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
template<typename R, typename T>  MSGraph<R>  yySobel(MSGraph<T> vp)
{
    int  i, j, k;
    int  pl, nx, ny, nz, pl2, xs, xs2;
    R    da, db, dc, dd, de;
    R    df, dg, dh, di, dj, dk, dl, dm, nr;
    MSGraph<R> xp;
    
    xp.mimicry(vp);
    xp.base = xp.zero = 0;

    if (xp.isNull()) {
        DEBUG_MODE PRINT_MESG("YYSOBEL: No More Memory!!\n");
        xp.state = JBXL_GRAPH_MEMORY_ERROR;
        return xp;
    }
 
    // カウンタ
    CVCounter*   vcounter = NULL;
    if (vp.zs>2) vcounter = GetUsableGlobalCounter();
    if (vcounter!=NULL) vcounter->SetMax(vp.zs);

    pl  = vp.xs*vp.ys;
    pl2 = 2*pl;
    xs  = vp.xs;
    xs2 = 2*vp.xs;

    for (k=0; k<vp.zs; k++) {
        nz = k*pl;
        for (j=2; j<vp.ys-2; j++) {
            ny = nz + j*vp.xs;
            for (i=2; i<vp.xs-2; i++) {
                nx = ny + i;
                da = vp.gp[nx-2+xs2] - 2*vp.gp[nx-2] + vp.gp[nx-2-xs2];
                db = vp.gp[nx-1+xs2] - 2*vp.gp[nx-1] + vp.gp[nx-1-xs2];
                dc = vp.gp[nx  +xs2] - 2*vp.gp[nx]   + vp.gp[nx  -xs2];
                dd = vp.gp[nx+1+xs2] - 2*vp.gp[nx+1] + vp.gp[nx+1-xs2];
                de = vp.gp[nx+2+xs2] - 2*vp.gp[nx+2] + vp.gp[nx+2-xs2];
                //da = vp.point(i-2, j+2, k) - 2*vp.point(i-2, j, k) + vp.point(i-2, j-2, k);
                //db = vp.point(i-1, j+2, k) - 2*vp.point(i-1, j, k) + vp.point(i-1, j-2, k);
                //dc = vp.point(i,   j+2, k) - 2*vp.point(i,   j, k) + vp.point(i,   j-2, k);
                //dd = vp.point(i+1, j+2, k) - 2*vp.point(i+1, j, k) + vp.point(i+1, j-2, k);
                //de = vp.point(i+2, j+2, k) - 2*vp.point(i+2, j, k) + vp.point(i+2, j-2, k);

                if (k==0 || k==vp.zs-1) {
                    dc = (R)(6.*dc);
                    df = dg = dh = di = dj = dk = dl = dm = 0;
                    nr = 64;
                }
                else {
                    dc = (R)(8.*dc);
                    df = vp.gp[nx-1+xs2-pl] - 2*vp.gp[nx-1-pl] + vp.gp[nx-1-xs2-pl];
                    dg = vp.gp[nx  +xs2-pl] - 2*vp.gp[nx  -pl] + vp.gp[nx  -xs2-pl];
                    dh = vp.gp[nx+1+xs2-pl] - 2*vp.gp[nx+1-pl] + vp.gp[nx+1-xs2-pl];
                    di = vp.gp[nx-1+xs2+pl] - 2*vp.gp[nx-1+pl] + vp.gp[nx-1-xs2+pl];
                    dj = vp.gp[nx  +xs2+pl] - 2*vp.gp[nx  +pl] + vp.gp[nx  -xs2+pl];
                    dk = vp.gp[nx+1+xs2+pl] - 2*vp.gp[nx+1+pl] + vp.gp[nx+1-xs2+pl];                
                    //df = vp.point(i-1, j+2, k-1) - 2*vp.point(i-1, j, k-1) + vp.point(i-1, j-2, k-1);
                    //dg = vp.point(i,   j+2, k-1) - 2*vp.point(i,   j, k-1) + vp.point(i,   j-2, k-1);
                    //dh = vp.point(i+1, j+2, k-1) - 2*vp.point(i+1, j, k-1) + vp.point(i+1, j-2, k-1);
                    //di = vp.point(i-1, j+2, k+1) - 2*vp.point(i-1, j, k+1) + vp.point(i-1, j-2, k+1);
                    //dj = vp.point(i,   j+2, k+1) - 2*vp.point(i,   j, k+1) + vp.point(i,   j-2, k+1);
                    //dk = vp.point(i+1, j+2, k+1) - 2*vp.point(i+1, j, k+1) + vp.point(i+1, j-2, k+1);

                    if (k==1 || k==vp.zs-2) {
                        dl = dm = 0;
                        nr = 136;
                    }
                    else {
                        dl = vp.gp[nx+xs2-pl2] - 2*vp.gp[nx-pl2] + vp.gp[nx-xs2-pl2];
                        dm = vp.gp[nx+xs2+pl2] - 2*vp.gp[nx+pl2] + vp.gp[nx-xs2+pl2];
                        //dl = vp.point(i, j+2, k-2) - 2*vp.point(i, j, k-2) + vp.point(i, j-2, k-2);
                        //dm = vp.point(i, j+2, k+2) - 2*vp.point(i, j, k+2) + vp.point(i, j-2, k+2);
                        nr = 144;
                    }
                }
                xp.gp[nx] = (R)((dc + 4.*(db+dd+dg+dj) + 2.*(df+dh+di+dk) + da+de+dl+dm)/nr);
            }
        }
        if (vcounter!=NULL) vcounter->StepIt();
    }

    return xp;
}


/**
template<typename R, typename T>  MSGraph<R> zzSobel(MSGraph<T> vp)

グラフィックデータの Z方向の 2階微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
template<typename R, typename T>  MSGraph<R> zzSobel(MSGraph<T> vp)
{
    int  i, j, k;
    R    da, db, dc, dd, de;
    R    df, dg, dh, di, dj, dk, dl, dm;
    MSGraph<R>  pp, xp;
    
    if (vp.zs<2) {      // 0 のグラフィックデータを返す
        pp.mimicry(vp);
        pp.state = JBXL_GRAPH_NODATA_ERROR;
        return pp;
    }

    // カウンタ
    CVCounter* vcounter = NULL;
    CVCounter* ccounter = NULL;
    if (vp.zs>2) vcounter = GetUsableGlobalCounter();
    if (vcounter!=NULL) {
        vcounter->SetMax(200);
        ccounter = vcounter->MakeChildCounter(100);
        SetGlobalCounter(ccounter);
    }

    pp = zSobel<R>(vp);

    if (vcounter!=NULL) {
        vcounter->DeleteChildCounter();
        ccounter = vcounter->MakeChildCounter(100);
        SetGlobalCounter(ccounter);
    }

    if (!pp.isNull()) {
        xp = zSobel<R>(pp);
        pp.free();
    }
    else xp = pp;

    if (vcounter!=NULL) {
        vcounter->DeleteChildCounter();
        SetGlobalCounter(vcounter);
        vcounter->PutFill();
    }

/*  if (vp.zs<5) return xp;
    for (k=2; k<vp.zs-2; k++) {
        for (j=2; j<vp.ys-2; j++) {
        for (i=2; i<vp.xs-2; i++) {
            da = vp.point(i,   j,   k+2) - 2*vp.point(i,   j,   k) + vp.point(i,   j,   k-2);
            db = vp.point(i+1, j,   k+2) - 2*vp.point(i+1, j,   k) + vp.point(i+1, j,   k-2);
            dc = vp.point(i-1, j,   k+2) - 2*vp.point(i-1, j,   k) + vp.point(i-1, j,   k-2);
            dd = vp.point(i,   j+1, k+2) - 2*vp.point(i,   j+1, k) + vp.point(i,   j+1, k-2);
            de = vp.point(i,   j-1, k+2) - 2*vp.point(i,   j-1, k) + vp.point(i,   j-1, k-2);
            df = vp.point(i+1, j+1, k+2) - 2*vp.point(i+1, j+1, k) + vp.point(i+1, j+1, k-2);
            dg = vp.point(i+1, j-1, k+2) - 2*vp.point(i+1, j-1, k) + vp.point(i+1, j-1, k-2);
            dh = vp.point(i-1, j+1, k+2) - 2*vp.point(i-1, j+1, k) + vp.point(i-1, j+1, k-2);
            di = vp.point(i-1, j-1, k+2) - 2*vp.point(i-1, j-1, k) + vp.point(i-1, j-1, k-2);
            dj = vp.point(i+2, j,   k+2) - 2*vp.point(i+2, j,   k) + vp.point(i+2, j,   k-2);
            dk = vp.point(i-2, j,   k+2) - 2*vp.point(i-2, j,   k) + vp.point(i-2, j,   k-2);
            dl = vp.point(i,   j+2, k+2) - 2*vp.point(i,   j+2, k) + vp.point(i,   j+2, k-2);
            dm = vp.point(i,   j-2, k+2) - 2*vp.point(i,   j-2, k) + vp.point(i,   j-2, k-2);
            xp.point(i, j, k) = (R)((8.*da + 4.*(db+dc+dd+de) + 2.*(df+dg+dh+di) +dj+dk+dl+dm)/144.);
        }
        }
    }
*/
    return xp;
}


/**
template <typename R, typename T>  MSGraph<Vector<R> >  vNabla(MSGraph<T> vp)  

グラフィックデータの ナブラを計算する(Sobel)．

@param  vp  計算対象となるグラフィックデータ構造体．
@return ナブラ．ベクトル型グラフィックデータ．
*/
template <typename R, typename T>  MSGraph<Vector<R> >  vNabla(MSGraph<T> vp)  
{
    int   i;
    MSGraph<R>  px, py, pz;
    MSGraph<Vector<R> > nv;

    //MSGraph<Vector<R> > nv(vp.xs, vp.ys, vp.zs);
    nv.xs = vp.xs;
    nv.ys = vp.ys;
    nv.zs = vp.zs;
    nv.zero.set(vp.zero, vp.zero, vp.zero);
    nv.base.set(vp.base, vp.base, vp.base);
    nv.RZxy = vp.RZxy;
    nv.rbound = vp.rbound;

    nv.gp = (Vector<R>*)malloc(sizeof(Vector<R>)*nv.xs*nv.ys*nv.zs);
    if (nv.isNull()) {
        DEBUG_MODE PRINT_MESG("vNabla: No More Memory!!\n");
        nv.state = JBXL_GRAPH_MEMORY_ERROR;
        return nv;
    }
    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
        nv.gp[i] = nv.base;
    }
         
    px = xSobel<R>(vp);
    if (px.gp==NULL) {
        nv.state = px.state;
        return nv;
    }

    py = ySobel<R>(vp);
    if (py.gp==NULL) {
        px.free();
        nv.state = py.state;
        return nv;
    }

    pz = zSobel<R>(vp);     // 2Dなら 0が入る
    if (pz.gp==NULL) {
        px.free();
        py.free();
        nv.state = pz.state;
        return nv;
    }

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
        (nv.gp[i])->set_Vector(px.gp[i], py.gp[i], pz.gp[i]);
    }

    px.free();
    py.free();
    pz.free();

    return nv;
}


/**
template <typename R, typename T>  MSGraph<R>  Nabla(MSGraph<T> vp)  

グラフィックデータの ナブラの絶対値を計算する(Sobel)．

@param  vp  計算対象となるグラフィックデータ構造体．
@return ナブラ．スカラ型グラフィックデータ．
*/
template <typename R, typename T>  MSGraph<R>  Nabla(MSGraph<T> vp)  
{
    int   i;
    R     xx, yy, zz;
    MSGraph<R> px, py, pz, nv;

    nv.mimicry(vp);
    if (nv.isNull()) {
        DEBUG_MODE PRINT_MESG("Nabla: No More Memory!!\n");
        nv.state = JBXL_GRAPH_MEMORY_ERROR;
        return nv;
    }

    px = xSobel<R>(vp);
    if (px.gp==NULL) {
        nv.state = px.state;
        return nv;
    }

    py = ySobel<R>(vp);
    if (py.gp==NULL) {
        px.free();
        nv.state = py.state;
        return nv;
    }

    pz = zSobel<R>(vp);
    if (pz.gp==NULL) {
        px.free();
        py.free();
        nv.state = pz.state;
        return nv;
    }

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
        xx = px.gp[i];
        yy = py.gp[i];
        zz = pz.gp[i];
        nv.gp[i] = (R)sqrt((double)xx*xx + yy*yy + zz*zz);
    }

    px.free();
    py.free();
    pz.free();

    return nv;
}


/**
template <typename R, typename T>  MSGraph<R>  edgeEnhance(MSGraph<T> gd, int mode=0)

グラフィックデータのラプラシアンを使ったエッジ強調．(2Dのみ)

@param  gd    計算対象となるグラフィックデータ構造体．
@param  mode  モード．@b 4: 4近傍ラプラシアン  @n
                      @b 8: 8近傍ラプラシアン  3x3 @n
                      @b その他: Sobelのラプラシアン(24近傍) 5x5  @n

@return 処理されたグラフィックデータ．
*/
template <typename R, typename T>  MSGraph<R>  edgeEnhance(MSGraph<T> gd, int mode=0)
{
    int  i;
    MSGraph<R> la, vp;

    vp.mimicry(gd);
    if (vp.isNull()) {
        DEBUG_MODE PRINT_MESG("edgeEnhance: No More Memory!!\n");
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    la = Laplacian<R>(gd, mode);  
    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
        vp.gp[i] = gd.gp[i] - la.gp[i];
    }
    la.free();
    
    return vp;
}


/**
template <typename T>  MSGraph<T>  medianFilter(WSGraph<T> xp, int ms=3) 

メディアンフィルタ処理を行なう．３D処理可．

@param  xp  対象となるグラフィックデータ構造体．
@param  ms  フィルタの大きさ．

@return メディアンフィルタ処理されたグラフィックデータ．
*/
template <typename T>  MSGraph<T>  medianFilter(MSGraph<T> xp, int ms=3) 
{
    int   i, j, x, y, z, cx;
    int   xx, yy, zz, cw, ux, mz;
    int   kc, xc, zc, xs, ps;
    T*    me;
    MSGraph<T> vp;

    mz = Min(ms, xp.zs);
    me = (T*)malloc(ms*ms*mz*sizeof(T));

    vp.mimicry(xp);
    if (vp.isNull()) {
        free(me);
        DEBUG_MODE PRINT_MESG("medianFilter: No More Memory!!\n");
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    kc = ms*ms*mz/2;
    xc = ms/2;
    zc = mz/2;
    xs = xp.xs;
    ps = xp.xs*xp.ys;
    z  = xp.zs/2;
    for(y=xc; y<xp.ys-xc; y++) 
    for(x=xc; x<xp.xs-xc; x++) {
        cx = z*ps + y*xs + x;
        i  = 0;
        for (zz=-zc; zz<=zc; zz++)
        for (yy=-xc; yy<=xc; yy++)
        for (xx=-xc; xx<=xc; xx++) {
            cw = cx + xx + yy*xs + zz*ps;
            me[i++] = xp.gp[cw];
        }
        for (i=0; i<ms*ms*mz-1; i++) 
        for (j=i+1; j<ms*ms*mz; j++) {
            if (me[i]<me[j]) {
                ux    = me[i];
                me[i] = me[j];
                me[j] = ux;
            }
        }
        vp.gp[cx-z*ps] = me[kc];
    }

    free(me);
    return vp;
}


/**
template <typename T>  MSGraph<int> euclidDistance(MSGraph<T> vp, int bc, int& rr)

WSGグラフィック上を 2値化し,各点における輝度値0の点からの
ユークリッド距離の 2乗の最小を求める．
         
@param  vp  操作対象となるグラフィックデータ構造体．
@param  bc  輝度値の 2値化の値．これより小さいものは０,これ以上は 1．
@param  rr  指定しない．画像中のユークリッド距離の 2乗の最大値が入る．
  
@return 輝度値の代わりにユークリッド距離が記入されたグラフィックデータ．
*/
template <typename T>  MSGraph<int> euclidDistance(MSGraph<T> vp, int bc, int& rr)
{
    int  i, j, k, l, df, d, w;
    int  rmax, rstart, rend;
    int  nx, ny, nz, pl;

    rr = -1;
    MSGraph<int>  pp(vp.xs, vp.ys, vp.zs, (int)vp.zero, (int)vp.base, vp.RZxy);
    if (pp.isNull()) {
        DEBUG_MODE PRINT_MESG("euclidDistance: No More Memory!! E1\n");
        pp.state = JBXL_GRAPH_MEMORY_ERROR;
        return pp;
    }

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
         if (vp.gp[i]>=bc) pp.gp[i] = 1;
         else              pp.gp[i] = 0;
    }

    pl = vp.xs*vp.ys;

    for (k=0; k<vp.zs; k++) {
        nz = k*pl;
        for (j=0; j<vp.ys; j++) {
            df = vp.xs;
            ny = nz + j*vp.xs;
            for (i=0; i<vp.xs; i++) {
                nx = ny + i;
                if (pp.gp[nx]!=0) df = df + 1;
                else              df = 0;
                pp.gp[nx] = df*df;
            }
        }
    }
        
    for (k=0; k<vp.zs; k++) {
        nz = k*pl;
        for (j=0; j<vp.ys; j++) {
            df = vp.xs;
            ny = nz + j*vp.xs;
            for (i=vp.xs-1; i>=0; i--) {
                nx = ny + i;
                if (pp.gp[nx]!=0) df = df + 1;
                else              df = 0;
                pp.gp[nx] = Min(pp.gp[nx], df*df);
            }
        }
    }

    rmax = Max(vp.ys, vp.zs);
    MSGraph<int>  buf(rmax);
    if (buf.isNull()) {
        pp.free();
        DEBUG_MODE PRINT_MESG("euclidDistance: No More Memory!! E2\n");
        pp.state = JBXL_GRAPH_MEMORY_ERROR;
        return pp;
    }

    for (k=0; k<vp.zs; k++) {
        nz = k*pl;
        for (i=0; i<vp.xs; i++) {
            nx = nz + i;
            for (j=0; j<vp.ys; j++)  buf.gp[j] = pp.gp[nx+j*vp.xs];
            for (j=0; j<vp.ys; j++) {
                ny = nx + j*vp.xs;
                d = buf.gp[j];
                if (d!=0) {
                    rmax   = (int)sqrt((double)d) + 1;
                    rstart = Min(rmax, j);
                    rend   = Min(rmax, vp.ys-j-1);
                    for (l=-rstart; l<=rend; l++) {
                        w = buf.gp[j+l] + l*l;
                        if (w<d) d = w;
                    }
                }
                pp.gp[ny] = d;
            }
        }
    }
    buf.clear();

    rr = 0;
    for (j=0; j<vp.ys; j++) {
        ny = j*vp.xs;
        for (i=0; i<vp.xs; i++) {
            nx = ny + i;
            for (k=0; k<vp.zs; k++)  buf.gp[k] = pp.gp[nx+k*pl];
            for (k=0; k<vp.zs; k++) {
                nz = nx + k*pl;
                d = buf.gp[k];
                if (d!=0) {
                    rmax   = (int)sqrt((double)d) + 1;
                    rstart = Min(rmax, k);
                    rend   = Min(rmax, vp.zs-k-1);
                    for (l=-rstart; l<=rend; l++) {
                        w = buf.gp[k+l] + l*l;
                        if (w<d) d = w;
                    }
                    rr = Max(rr, d);
                }
                pp.gp[nz] = d;
            }
        }
    }
    buf.free();

    return pp;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// フィルター
//

#define   FILTER_NON        0   ///< 何もしない
#define   FILTER_ABS        1   ///< 絶対値
#define   FILTER_MINMAX     2   ///< 元のデータの範囲に限定
#define   FILTER_NORM       3   ///< 元のデータの範囲に伸張


//
//  mode: 
//
template <typename R, typename T> MSGraph<R>  MSMaskFilter(MSGraph<R> vp, MSGraph<T> filter, int mode=FILTER_NON)
{
    MSGraph<R> xp;

    if (vp.xs<filter.xs || vp.ys<filter.ys || vp.zs<filter.zs) {
        DEBUG_MODE PRINT_MESG("MSMaskFilter: Error: mismach filter dimension!!\n");
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }
    if (filter.norm==0.0) {
        DEBUG_MODE PRINT_MESG("MSMaskFilter: Error: norm of filter is zero!!\n");
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }

    xp.mimicry(vp);

    int xs = filter.xs/2;
    int ys = filter.ys/2;
    int zs = filter.zs/2;

    for (int k=zs; k<xp.zs-zs; k++) {
        for (int j=ys; j<xp.ys-ys; j++) {
            for (int i=xs; i<xp.xs-xs; i++) {

                T conv = (T)0;
                for (int n=-zs; n<=zs; n++) {
                    for (int m=-ys; m<=ys; m++) {
                        for (int l=-xs; l<=xs; l++) {
                            conv += filter.point(xs+l, ys+m, zs+n) * vp.point(i+l, j+m, k+n);
                        }
                    }
                }

                R pt = (R)(conv/filter.norm);

                if (mode==FILTER_ABS && pt<(R)0) pt = -pt;
                else if (mode==FILTER_MINMAX) {
                    if (pt<(R)vp.min)      pt = (R)vp.min;
                    else if (pt>(R)vp.max) pt = (R)vp.max;
                }
                xp.point(i, j, k) = pt;
            }
        }
    }

    xp.get_minmax();
    if (mode==FILTER_NORM && xp.max!=xp.min) {
        for (int i=0; i<xp.xs*xp.ys*xp.zs; i++) {
            xp.gp[i] = (R)(((T)(xp.gp[i]-xp.min)*(vp.max-vp.min))/(xp.max-xp.min) + vp.min);
        }
        xp.get_minmax();
    }

    return xp;
}


}       // namespace



#endif
 

