
#ifndef  __JBXL_GRAPHIC_MATH_H_
#define  __JBXL_GRAPHIC_MATH_H_


/**
@brief    グラフィック用数学ライブラリ＆フィルタ ヘッダ
@file     gmt.h
@version  3.0
@author   Fumi.Iseki (C)
*/


#include "gdata.h"

/**
フィルタ用マスク
*/
typedef struct {
    int    mode;    ///< 2:2D, 3:3D 
    int    msize;   ///< size of mask 
    int    nfact;   ///< normalized facter 
    int*   imask;   ///< pointer to mask 
} FMask;


#define   ALL           0x0000
#define   NONE_SHAPE    0x0001
#define   PEAK          0x0002
#define   PIT           0x0004
#define   SADDLE_RIDGE  0x0008
#define   SADDLE_VALLEY 0x0010
#define   MINIMAL       0x0020
#define   RIDGE         0x0040
#define   VALLEY        0x0080
#define   FLAT          0x0100


#define   TOP_VIEW         0
#define   SIDEZ_VIEW       0
#define   SIDEX_VIEW       1
#define   SIDEY_VIEW       2
#define   TOP_VIEW_DEPTH   3
#define   SIDEZ_VIEW_DEPTH 3


WSGraph  xSobel(WSGraph vp); 
WSGraph  ySobel(WSGraph vp); 
WSGraph  zSobel(WSGraph vp); 
FSGraph  fxSobel(FSGraph vp);
FSGraph  fySobel(FSGraph vp);
FSGraph  fzSobel(FSGraph vp);

WSGraph  xxSobel(WSGraph vp);
WSGraph  yySobel(WSGraph vp);
WSGraph  zzSobel(WSGraph vp);
FSGraph  fxxSobel(FSGraph vp);
FSGraph  fyySobel(FSGraph vp);
FSGraph  fzzSobel(FSGraph vp);

WSGraph  Nabra (WSGraph vp); 
FSGraph  fNabra(FSGraph vp); 
VSGraph  vNabra(WSGraph vp); 
VSGraph  vfNabra(FSGraph vp);

WSGraph  Laplacian(WSGraph vp, int mode);

VSGraph  curvature(FSGraph vp);
VSGraph  curvature3D(FSGraph vp);
WSGraph  curv2WSGraph(VSGraph vp);
WSGraph  WSCurve(WSGraph gx, int mode, int cc);

WSGraph  euclid_distance(WSGraph vp, int* rr, int bc);
int      out_round(WSGraph, int, int, IRBound*, int);

WSGraph  edge_enhance(WSGraph gd, int mode);
WSGraph  median(WSGraph, int);
FMask    gauss_mask(double sig, int mode, int size);
WSGraph  imask(WSGraph, FMask);
WSGraph  to2d(WSGraph gd, int mode);

#define  distance2(vp, rr, bc)   euclid_distance(vp, rr, bc)
#define  mip(vp)                 to2d(vp, TOP_VIEW)
#define  free_mask(mk)           free((mk)->imask)


#endif      // __JBXL_GRAPHIC_MATH_H_

