
#ifndef  __JBXL_CPP_MORPHOLOGY_H_
#define  __JBXL_CPP_MORPHOLOGY_H_


/**
@brief    モルフォリジーライブラリ
@file     Morph.h
@author   Fumi.Iseki (C)
@date     2004 8/31

@attention
このライブラリは，チェックと見直しが必要 '04 8/31
 */


#include "Gdata.h"


//
namespace jbxl {


#define  OPENING   0
#define  CLOSING   1
#define  DILATION  2

#define  SPHERE_ELEMENT    1


/*
template <typename T>  MSGraph<T>  Morphology(MSGraph<T> vp, MSGraph<T> xp, int cc, int mode);
template <typename T>  MSGraph<T>  opening_morph(MSGraph<T> vp, MSGraph<T> xp, int cc);
template <typename T>  bool  point_open_morph(MSGraph<T> vp, int x, int y, int z, MSGraph<T> xp, int cc);
template <typename T>  void  copy_morph_element(MSGraph<T> vp, int x, int y, int z, MSGraph<T> xp);
template <typename T>  MSGraph<T>  fat_object_morph(MSGraph<T> vp, MSGraph<T> xp, int cc);
template <typename T>  bool  point_fat_object_morph(MSGraph<T> vp, int x, int y, int z, MSGraph<T> xp, int cc);
template <typename T>  MSGraph<T>  make_element_morph(Parameter x);
*/


/**
template <typename T>  MSGraph<T> Morphology(MSGraph<T> vp, MSGraph<T> xp, int cc, int mode)

画像vpに対して，エレメントxpで，modeのモルフォロジー演算を行う．@n
現在は OPENING のmodeしかサポートしていない．

@param  vp    処理対象の元データ
@param  xp    エレメント画像
@param  cc    閾値．この輝度値以上の画素に対して演算を行う
@param  mode  演算の種類を指定

@return 処理結果の画像
*/
template <typename T>  MSGraph<T> Morphology(MSGraph<T> vp, MSGraph<T> xp, int cc, int mode)
{
    MSGraph<T> wp;

    if (mode==OPENING) wp = opening_morph(vp, xp, cc);
    else {
        DEBUG_MODE PRINT_MESG("MORPHOLOGY: サポートしていない演算です = %d\n", mode);
    }
    return wp;
}


/**
template <typename T>  MSGraph<T>  opening_morph(MSGraph<T> vp, MSGraph<T> xp, int cc)

画像vpに対して，エレメントxpで，modeのモルフォロジー演算のオープニングを行う．@n
元画像は保持される．

@param  vp  処理対象の元データ
@param  xp  エレメント画像
@param  cc  閾値．この輝度値以上の画素に対して演算を行う

@return 処理結果の画像
*/
template <typename T>  MSGraph<T>  opening_morph(MSGraph<T> vp, MSGraph<T> xp, int cc)
{
    int  i, j, k, ks, cz, cy, cx, ps;
    bool err;
    MSGraph<T> wp(vp.xs, vp.ys, vp.zs);
    if (wp.isNull()) return wp;

    ps = vp.xs*vp.ys;
    for (k=0; k<vp.zs; k++) {
        cz = k*ps;
        for (j=0; j<vp.ys; j++) {
            cy = cz + j*vp.xs;
            for (i=0; i<vp.xs; i++) {
                cx = cy + i;
                if (vp.gp[cx]>=cc) {
                    err = point_open_morph(vp, i, j, k, xp, cc);
                    if (err) copy_morph_element(wp, i, j, k, xp);
                }
            }
        }
    }

    return wp;
}


/**
template <typename T>  bool  point_open_morph(MSGraph<T> vp, int x, int y, int z, MSGraph<T> xp, int cc)

オープニング可能かどうかチェックする．
*/
template <typename T>  bool  point_open_morph(MSGraph<T> vp, int x, int y, int z, MSGraph<T> xp, int cc)
{
    int  i, j, k, cx, cy, cz;
    int  xx, yy, zz, ax, ay, az;

    for (k=0; k<xp.zs; k++) {
        zz = z + k - xp.zs/2;
        if (zz<0)       zz = 0;
        if (zz>vp.zs-1) zz = vp.zs-1;
        az = vp.xs*vp.ys*zz;
        cz = xp.xs*xp.ys*k; 
        for (j=0; j<xp.ys; j++) {
            yy = y + j - xp.ys/2;
            ay = az + vp.xs*yy; 
            cy = cz + xp.xs*j;  
            for (i=0; i<xp.xs; i++) {
                xx = x + i - xp.xs/2;
                ax = ay + xx;   
                cx = cy + i;    
                if (xp.gp[cx]>0) {
                    if (!(xx>=0&&xx<vp.xs&&yy>=0&&yy<vp.ys)) return false;
                    if (vp.gp[ax]<cc) return false;
                }
            }
        }
    }
   
    return true;
}


/**
template <typename T>  void  copy_morph_element(MSGraph<T> vp, int x, int y, int z, MSGraph<T> xp)

エレメントを対象画像にコピーする．
*/
template <typename T>  void  copy_morph_element(MSGraph<T> vp, int x, int y, int z, MSGraph<T> xp)
{
    int  i, j, k, cx, cy, cz;
    int  xx, yy, zz, ax, ay, az;

    for (k=0; k<xp.zs; k++) {
        zz = z + k - xp.zs/2;
        az = vp.xs*vp.ys*zz;
        cz = xp.xs*xp.ys*k; 
        for (j=0; j<xp.ys; j++) {
            yy = y + j - xp.ys/2;
            ay = az + vp.xs*yy; 
            cy = cz + xp.xs*j;  
            for (i=0; i<xp.xs; i++) {
                xx = x + i - xp.xs/2;
                ax = ay + xx;   
                cx = cy + i;    
                if (xx>=0&&xx<vp.xs&&yy>=0&&yy<vp.ys&&zz>=0&&zz<vp.zs){
                    if (xp.gp[cx]>0) vp.gp[ax] = xp.gp[cx];
                }
            }
        }
    }
}


/**
template <typename T> MSGraph<T>  fat_object_morph(MSGraph<T> vp, MSGraph<T> xp, int cc)

画像を太らせる処理（何て言うんだっけ？）
*/
template <typename T> MSGraph<T>  fat_object_morph(MSGraph<T> vp, MSGraph<T> xp, int cc)
{
    int  i, j, k, ps, cz, cy, cx;
    bool err;
    MSGraph<T> wp(vp.xs, vp.ys, vp.zs);
    if (wp.isNull()) return wp;

    ps = vp.xs*vp.ys;
    for (k=0; k<vp.zs; k++) {
        cz = k*ps;
        for (j=0; j<vp.ys; j++) {
            cy = cz + j*vp.xs;
            for (i=0; i<vp.xs; i++) {
                cx = cy + i;
                if (vp.gp[cx]>=cc) {
                    err = point_fat_object_morph(vp, i, j, k, xp, cc);
                    if (err) copy_morph_element(wp, i, j, k, xp);
                }
            }
        }
    }

    return wp;
}


/**
template <typename T>  bool  point_fat_object_morph(MSGraph<T> vp, int x, int y, int z, MSGraph<T> xp, int cc)
*/
template <typename T>  bool  point_fat_object_morph(MSGraph<T> vp, int x, int y, int z, MSGraph<T> xp, int cc)
{
    int  i, j, k, cx, cy, cz;
    int  xx, yy, zz, ax, ay, az;

    for (k=0; k<xp.zs; k++) {
        zz = z + k - xp.zs/2;
        if (zz<0)       zz = 0;
        if (zz>vp.zs-1) zz = vp.zs-1;
        az = vp.xs*vp.ys*zz;
        cz = xp.xs*xp.ys*k; 
        for (j=0; j<xp.ys; j++) {
            yy = y + j - xp.ys/2;
            ay = az + vp.xs*yy; 
            cy = cz + xp.xs*j;  
            for (i=0; i<xp.xs; i++) {
                xx = x + i - xp.xs/2;
                ax = ay + xx;   
                cx = cy + i;    
                if (xp.gp[cx]>0) {
                    if (xx>=0&&xx<vp.xs&&yy>=0&&yy<vp.ys) {
                        if (vp.gp[ax]>=cc)  return true;
                    }
                }
            }
        }
    }
   
    return false;
}


/**
template <typename T>  MSGraph<T>  make_element_morph(Parameter x)

モルフォロジー演算のエレメントを生成する．
*/
template <typename T>  MSGraph<T>  make_element_morph(Parameter32 x)
{
    int    *p, n, m, r, c, i, j, k, z;
    int    nn, xs, ir;
    double s, sn, cs, dr, dt, df, th, fi, rx, zz;
    MSGraph<T> vp, xp;
  
    p = (int*)x.pm; 
    m = p[0];               // mode 
    r = p[1];               // hankei 
    c = p[2];               // color
    s = p[3]/p[4];          // RZxy;

    if (m==SPHERE_ELEMENT) {
        xs = 2*r + 1;
        vp.getm(xs, xs, (int)(xs*s+0.5));
        if (vp.isNull()) return vp;

        xp.xs = xs; 
        xp.ys = xs; 
        xp.zs = 1;

        for (z=0; z<vp.zs; z++) {
            xp.gp = &vp.gp[z*vp.xs*vp.ys];
            zz = Min(r, Xabs(((vp.zs-1.)/2.-z)/s));
            ir = (int)(sqrt(Max(r*r-zz*zz, 0.0))+0.5); 
            circle(xp, r, r, ir, c, ON);
        }
    } 
    else {
        DEBUG_MODE PRINT_MESG("MAKE_ELEMENT_MORPH: no support mode = %d\n",m);
        exit(1);
    }

    return vp;
}


}       // namespace


#endif
 
