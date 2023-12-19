
/**
@brief    グラフィック用数学ライブラリ＆フィルタ
@file     gmt.c
@version  3.0
@author   Fumi.Iseki (C)
*/


#include "gmt.h"
#include "jbxl_state.h"


/**
WSGraph  Laplacian(WSGraph vp, int mode)

2Dグラフィックデータのラプラシアンを計算する．

@param  vp    計算対象となるグラフィックデータ構造体．
@param  mode  モード． @b 4: 4近傍ラプラシアン
@param  mode  @b 8: 8近傍ラプラシアン
@param  mode  その他: Sobelのラプラシアン(24近傍)

@return 処理されたグラフィックデータ．
*/
WSGraph  Laplacian(WSGraph vp, int mode)
{
    int  i, j;
    int  da, db, dc, dd, de, df, dg, dh;
    WSGraph lp;

    lp = make_WSGraph(vp.xs, vp.ys, 1);
    if (lp.gp==NULL) return lp;

    if (vp.gp==NULL) {
        lp.state = JBXL_GRAPH_NODATA_ERROR;
        return lp;
    }

    if (mode==4) {
        for (j=1; j<vp.ys-1; j++) {
            for (i=1; i<vp.xs-1; i++) {
                da = Px(vp, i+1, j) + Px(vp, i-1, j);
                db = Px(vp, i, j);
                dc = Px(vp, i, j+1) + Px(vp, i, j-1);
                Px(lp, i, j) = da - 4*db + dc;
            }
        }
    }

    else if (mode==8) {
        for (j=1; j<vp.ys-1; j++) {
            for (i=1; i<vp.xs-1; i++) {
                da = Px(vp, i+1, j) + Px(vp, i-1, j);
                db = Px(vp, i, j+1) + Px(vp, i, j-1);
                dc = Px(vp, i, j);
                dd = Px(vp, i+1, j+1) + Px(vp, i-1, j+1);
                de = Px(vp, i+1, j-1) + Px(vp, i-1, j-1);
                Px(lp, i, j) = da + db - 8*dc + dd + de;
            }
        }
    }

    else {
        for (j=2; j<vp.ys-2; j++) {
            for (i=2; i<vp.xs-2; i++) {
                da = Px(vp, i, j);
                db = Px(vp, i+1, j)   + Px(vp, i-1, j) + Px(vp, i, j+1) + Px(vp, i, j-1);
                dc = Px(vp, i-1, j-2) + Px(vp, i, j-2) + Px(vp, i+1, j-2);
                dd = Px(vp, i-1, j+2) + Px(vp, i, j+2) + Px(vp, i+1, j+2);
                de = Px(vp, i-2, j-1) + Px(vp, i-2, j) + Px(vp, i-2, j+1);
                df = Px(vp, i+2, j-1) + Px(vp, i+2, j) + Px(vp, i+2, j+1);
                dg = Px(vp, i-2, j-2) + Px(vp, i+2, j-2);
                dh = Px(vp, i-2, j+2) + Px(vp, i+2, j+2);
                Px(lp, i, j) = (sWord)((-24*da-8*db+4*(dc+dd+de+df)+2*(dg+dh))/64);
            }
        }
    }

    return lp;
}



/**
WSGraph  xSobel(WSGraph vp) 

グラフィックデータの x方向微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
WSGraph  xSobel(WSGraph vp) 
{
    int  i, j, k;
    int  da, db, dc, dd, de, nr;
    WSGraph xp;
    
    memset(&xp, 0, sizeof(WSGraph));
    if (vp.gp==NULL) {
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }
    xp.state = JBXL_NORMAL;

    if (vp.zs<=0) vp.zs = 1; 
    xp = make_WSGraph(vp.xs, vp.ys, vp.zs);
    if (xp.gp==NULL) return xp;

    for (k=0; k<vp.zs; k++) {
        for (j=1; j<vp.ys-1; j++) {
            for (i=1; i<vp.xs-1; i++) {
                da = Vx(vp, i+1, j-1, k) - Vx(vp, i-1, j-1, k);
                db = Vx(vp, i+1, j,   k) - Vx(vp, i-1, j,   k);
                dc = Vx(vp, i+1, j+1, k) - Vx(vp, i-1, j+1, k);
                if (k==0 || k==vp.zs-1) {
                    dd = de = 0;
                    nr = 8;
                }
                else {
                    dd = Vx(vp, i+1, j, k-1) - Vx(vp, i-1, j, k-1);
                    de = Vx(vp, i+1, j, k+1) - Vx(vp, i-1, j, k+1);
                    nr = 12;
                }
                Vx(xp, i, j, k) = (sWord)((da + 2*db + dc + dd + de)/nr);
            }
        }
    }

    return xp;
}



/**
FSGraph  fxSobel(FSGraph vp) 

グラフィックデータの x方向微分(Sobel)を実数計算する．
  精度が上昇するが時間がかかる．

@param  vp  計算対象となる double型グラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
FSGraph  fxSobel(FSGraph vp) 
{
    int    i, j, k;
    double da, db, dc, dd, de, nr;
    FSGraph xp;
    
    memset(&xp, 0, sizeof(FSGraph));
    if (vp.gp==NULL) {
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }

    if (vp.zs<=0) vp.zs = 1; 
    xp = make_FSGraph(vp.xs, vp.ys, vp.zs);
    if (xp.gp==NULL) return xp;

    for (k=0; k<vp.zs; k++) {
        for (j=1; j<vp.ys-1; j++) {
            for (i=1; i<vp.xs-1; i++) {
                da = Vx(vp, i+1, j-1, k) - Vx(vp, i-1, j-1, k);
                db = Vx(vp, i+1, j,   k) - Vx(vp, i-1, j,   k);
                dc = Vx(vp, i+1, j+1, k) - Vx(vp, i-1, j+1, k);
                if (k==0 || k==vp.zs-1) {
                    dd = de = 0.;
                    nr = 8.;
                }
                else {
                    dd = Vx(vp, i+1, j, k-1) - Vx(vp, i-1, j, k-1);
                    de = Vx(vp, i+1, j, k+1) - Vx(vp, i-1, j, k+1);
                    nr = 12.;
                }
                Vx(xp, i, j, k) = (da + 2.*db + dc + dd + de)/nr;
            }
        }
    }

    return xp;
}



/**
WSGraph  ySobel(WSGraph vp) 

グラフィックデータの y方向微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
WSGraph  ySobel(WSGraph vp) 
{
    int  i, j, k;
    int  da, db, dc, dd, de, nr;
    WSGraph xp;
    
    memset(&xp, 0, sizeof(WSGraph));
    if (vp.gp==NULL) {
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }

    if (vp.zs<=0) vp.zs = 1; 
    xp = make_WSGraph(vp.xs, vp.ys, vp.zs);
    if (xp.gp==NULL) return xp;

    for (k=0; k<vp.zs; k++) {
        for (j=1; j<vp.ys-1; j++) {
            for (i=1; i<vp.xs-1; i++) {
                da = Vx(vp, i-1, j+1, k) - Vx(vp, i-1, j-1, k);
                db = Vx(vp, i,   j+1, k) - Vx(vp, i,   j-1, k);
                dc = Vx(vp, i+1, j+1, k) - Vx(vp, i+1, j-1, k);
                if (k==0 || k==vp.zs-1) {
                    dd = de = 0;
                    nr = 8;
                }
                else {
                    dd = Vx(vp, i, j+1, k-1) - Vx(vp, i, j-1, k-1);
                    de = Vx(vp, i, j+1, k+1) - Vx(vp, i, j-1, k+1);
                    nr = 12;
                }
                Vx(xp, i, j, k) = (sWord)((da + 2*db + dc + dd + de)/nr);
            }
        }
    }

    return xp;
}



/**
FSGraph  fySobel(FSGraph vp) 

グラフィックデータの y方向微分(Sobel)を実数計算する．
精度が上昇するが時間がかかる．

@param  vp  計算対象となる double型グラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
FSGraph  fySobel(FSGraph vp) 
{
    int    i, j, k;
    double da, db, dc, dd, de, nr;
    FSGraph xp;
    
    memset(&xp, 0, sizeof(FSGraph));
    if (vp.gp==NULL) {
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }

    if (vp.zs<=0) vp.zs = 1; 
    xp = make_FSGraph(vp.xs, vp.ys, vp.zs);
    if (xp.gp==NULL) return xp;

    for (k=0; k<vp.zs; k++) {
        for (j=1; j<vp.ys-1; j++) {
            for (i=1; i<vp.xs-1; i++) {
                da = Vx(vp, i-1, j+1, k) - Vx(vp, i-1, j-1, k);
                db = Vx(vp, i,   j+1, k) - Vx(vp, i,   j-1, k);
                dc = Vx(vp, i+1, j+1, k) - Vx(vp, i+1, j-1, k);
                if (k==0 || k==vp.zs-1) {
                    dd = de = 0.;
                    nr = 8.;
                }
                else {
                    dd = Vx(vp, i, j+1, k-1) - Vx(vp, i, j-1, k-1);
                    de = Vx(vp, i, j+1, k+1) - Vx(vp, i, j-1, k+1);
                    nr = 12.;
                }
                Vx(xp, i, j, k) = (da + 2.*db + dc + dd + de)/nr;
            }
        }
    }

    return xp;
}



/**
WSGraph  zSobel(WSGraph vp) 

グラフィックデータの z方向微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
WSGraph  zSobel(WSGraph vp) 
{
    int  i, j, k;
    int  da, db, dc, dd, de;
    WSGraph xp;
    
    memset(&xp, 0, sizeof(WSGraph));
    if (vp.gp==NULL) {
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }

    if (vp.zs<=1) {
        //fprintf(stderr,"ZSOBEL: no 3D data inputed.\n");
        xp.state = JBXL_GRAPH_IVDARG_ERROR;
        return xp;
    }

    xp = make_WSGraph(vp.xs, vp.ys, vp.zs);
    if (xp.gp==NULL) return xp;

    for (k=1; k<vp.zs-1; k++) {
        for (j=1; j<vp.ys-1; j++) {
            for (i=1; i<vp.xs-1; i++) {
                da = Vx(vp, i-1, j, k+1) - Vx(vp, i-1, j, k-1);
                db = Vx(vp, i+1, j, k+1) - Vx(vp, i+1, j, k-1);
                dc = Vx(vp, i,   j, k+1) - Vx(vp, i,   j, k-1);
                dd = Vx(vp, i, j-1, k+1) - Vx(vp, i, j-1, k-1);
                de = Vx(vp, i, j+1, k+1) - Vx(vp, i, j+1, k-1);
                Vx(xp, i, j, k) = (sWord)((da + db + 2*dc + dd + de)/12);
            }
        }
    }

    for (j=1; j<vp.ys-1; j++) {
        for (i=1; i<vp.xs-1; i++) {
            da = Vx(vp, i-1, j, 1) - Vx(vp, i-1, j, 0);
            db = Vx(vp, i+1, j, 1) - Vx(vp, i+1, j, 0);
            dc = Vx(vp, i,   j, 1) - Vx(vp, i,   j, 0);
            dd = Vx(vp, i, j-1, 1) - Vx(vp, i, j-1, 0);
            de = Vx(vp, i, j+1, 1) - Vx(vp, i, j+1, 0);
            Vx(xp, i, j, 0) = (sWord)((da + db + 2*dc + dd + de)/12);

            da = Vx(vp, i-1, j, vp.zs-1) - Vx(vp, i-1, j, vp.zs-2);
            db = Vx(vp, i+1, j, vp.zs-1) - Vx(vp, i+1, j, vp.zs-2);
            dc = Vx(vp, i,   j, vp.zs-1) - Vx(vp, i,   j, vp.zs-2);
            dd = Vx(vp, i, j-1, vp.zs-1) - Vx(vp, i, j-1, vp.zs-2);
            de = Vx(vp, i, j+1, vp.zs-1) - Vx(vp, i, j+1, vp.zs-2);
            Vx(xp, i, j, vp.zs-1) = (sWord)((da + db + 2*dc + dd + de)/12);
        }
    }

    return xp;
}



/**
FSGraph  fzSobel(FSGraph vp) 

グラフィックデータの z方向微分(Sobel)を実数計算する．
精度が上昇するが時間がかかる．

@param  vp  計算対象となる double型グラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
FSGraph  fzSobel(FSGraph vp) 
{
    int    i, j, k;
    double da, db, dc, dd, de;
    FSGraph xp;
    
    memset(&xp, 0, sizeof(FSGraph));
    if (vp.gp==NULL) {
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }

    if (vp.zs<=1) {
        //fprintf(stderr,"FZSOBEL: no 3D data inputed.\n");
        xp.state = JBXL_GRAPH_IVDARG_ERROR;
        return xp;
    }

    xp = make_FSGraph(vp.xs, vp.ys, vp.zs);
    if (xp.gp==NULL) return xp;

    for (k=1; k<vp.zs-1; k++) {
        for (j=1; j<vp.ys-1; j++) {
            for (i=1; i<vp.xs-1; i++) {
                da = Vx(vp, i-1, j, k+1) - Vx(vp, i-1, j, k-1);
                db = Vx(vp, i+1, j, k+1) - Vx(vp, i+1, j, k-1);
                dc = Vx(vp, i,   j, k+1) - Vx(vp, i,   j, k-1);
                dd = Vx(vp, i, j-1, k+1) - Vx(vp, i, j-1, k-1);
                de = Vx(vp, i, j+1, k+1) - Vx(vp, i, j+1, k-1);
                Vx(xp, i, j, k) = (da + db + 2.*dc + dd + de)/12.;
            }
        }
    }

    for (j=1; j<vp.ys-1; j++) {
        for (i=1; i<vp.xs-1; i++) {
            da = Vx(vp, i-1, j, 1) - Vx(vp, i-1, j, 0);
            db = Vx(vp, i+1, j, 1) - Vx(vp, i+1, j, 0);
            dc = Vx(vp, i,   j, 1) - Vx(vp, i,   j, 0);
            dd = Vx(vp, i, j-1, 1) - Vx(vp, i, j-1, 0);
            de = Vx(vp, i, j+1, 1) - Vx(vp, i, j+1, 0);
            Vx(xp, i, j, 0) = (da + db + 2.*dc + dd + de)/12.;

            da = Vx(vp, i-1, j, vp.zs-1) - Vx(vp, i-1, j, vp.zs-2);
            db = Vx(vp, i+1, j, vp.zs-1) - Vx(vp, i+1, j, vp.zs-2);
            dc = Vx(vp, i,   j, vp.zs-1) - Vx(vp, i,   j, vp.zs-2);
            dd = Vx(vp, i, j-1, vp.zs-1) - Vx(vp, i, j-1, vp.zs-2);
            de = Vx(vp, i, j+1, vp.zs-1) - Vx(vp, i, j+1, vp.zs-2);
            Vx(xp, i, j, vp.zs-1) = (da + db + 2.*dc + dd + de)/12.;
        }
    }

    return xp;
}



/**
WSGraph  xxSobel(WSGraph vp) 

グラフィックデータの x方向の2階微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
WSGraph  xxSobel(WSGraph vp)
{
    int  x, y, z, xs, ys, zs, cx, cy, cz, ps;
    int  da, db, dc, dd, de;
    int  df, dg, dh, di, dj, dk, dl, dm;
    WSGraph px;

    memset(&px, 0, sizeof(WSGraph));
    if (vp.gp==NULL) {
        px.state = JBXL_GRAPH_NODATA_ERROR;
        return px;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    ps = xs*ys;

    if (zs<5) { 
        px = make_WSGraph(xs, ys, 1);
        if (px.gp==NULL) return px;

        for (y=2; y<ys-2; y++) {
            cy = y*xs;
            for (x=2; x<xs-2; x++) {
                cx = cy + x;
                da = vp.gp[cx-2*xs+2] - 2*vp.gp[cx-2*xs] + vp.gp[cx-2*xs-2];
                db = vp.gp[cx  -xs+2] - 2*vp.gp[cx  -xs] + vp.gp[cx  -xs-2];
                dc = vp.gp[cx     +2] - 2*vp.gp[cx]      + vp.gp[cx     -2];
                dd = vp.gp[cx  +xs+2] - 2*vp.gp[cx  +xs] + vp.gp[cx  +xs-2];
                de = vp.gp[cx+2*xs+2] - 2*vp.gp[cx+2*xs] + vp.gp[cx+2*xs-2];
                px.gp[cx] = (sWord)((da + 4*db + 6*dc + 4*dd + de)/64);
            }
        }
    }

    else {
        px = make_WSGraph(xs, ys, zs);
        if (px.gp==NULL) return px;

        for (z=2; z<zs-2; z++) {
            cz = z*ps;
            for (y=2; y<ys-2; y++) {
                cy = cz + y*xs;
                for (x=2; x<xs-2; x++) {
                    cx = cy + x;
                    da = vp.gp[cx   +2]    - 2*vp.gp[cx]       + vp.gp[cx-2];    
                    db = vp.gp[cx+xs+2]    - 2*vp.gp[cx+xs]    + vp.gp[cx+xs-2];     
                    dc = vp.gp[cx-xs+2]    - 2*vp.gp[cx-xs]    + vp.gp[cx-xs-2];    
                    dd = vp.gp[cx+ps+2]    - 2*vp.gp[cx+ps]    + vp.gp[cx+ps-2];     
                    de = vp.gp[cx-ps+2]    - 2*vp.gp[cx-ps]    + vp.gp[cx-ps-2];     
                    df = vp.gp[cx+xs+ps+2] - 2*vp.gp[cx+xs+ps] + vp.gp[cx+xs+ps-2];
                    dg = vp.gp[cx+xs-ps+2] - 2*vp.gp[cx+xs-ps] + vp.gp[cx+xs-ps-2];
                    dh = vp.gp[cx-xs+ps+2] - 2*vp.gp[cx-xs+ps] + vp.gp[cx-xs+ps-2];
                    di = vp.gp[cx-xs-ps+2] - 2*vp.gp[cx-xs-ps] + vp.gp[cx-xs-ps-2];
                    dj = vp.gp[cx+2*xs+2]  - 2*vp.gp[cx+2*xs]  + vp.gp[cx+2*xs-2];  
                    dk = vp.gp[cx-2*xs+2]  - 2*vp.gp[cx-2*xs]  + vp.gp[cx-2*xs-2];  
                    dl = vp.gp[cx+2*ps+2]  - 2*vp.gp[cx+2*ps]  + vp.gp[cx+2*ps-2]; 
                    dm = vp.gp[cx-2*ps+2]  - 2*vp.gp[cx-2*ps]  + vp.gp[cx-2*ps-2]; 
                    px.gp[cx] = (sWord)((8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144);
                }
            }
        }
    }

    return px;
}



/**
FSGraph  fxxSobel(FSGraph vp) 

グラフィックデータの x方向の2階微分(Sobel)を実数計算する．
精度が上昇するが時間がかかる．

@param  vp  計算対象となる double型グラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
FSGraph  fxxSobel(FSGraph vp)
{
    int  x, y, z, xs, ys, zs, cx, cy, cz, ps;
    double da, db, dc, dd, de;
    double df, dg, dh, di, dj, dk, dl, dm;
    FSGraph px;

    memset(&px, 0, sizeof(FSGraph));
    if (vp.gp==NULL) {
        px.state = JBXL_GRAPH_NODATA_ERROR;
        return px;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    ps = xs*ys;

    if (zs<5) { 
        px = make_FSGraph(xs, ys, 1);
        if (px.gp==NULL) return px;

        for (y=2; y<ys-2; y++) {
            cy = y*xs;
            for (x=2; x<xs-2; x++) {
                cx = cy + x;
                da = vp.gp[cx-2*xs+2] - 2*vp.gp[cx-2*xs] + vp.gp[cx-2*xs-2];
                db = vp.gp[cx  -xs+2] - 2*vp.gp[cx  -xs] + vp.gp[cx  -xs-2];
                dc = vp.gp[cx     +2] - 2*vp.gp[cx]      + vp.gp[cx     -2];
                dd = vp.gp[cx  +xs+2] - 2*vp.gp[cx  +xs] + vp.gp[cx  +xs-2];
                de = vp.gp[cx+2*xs+2] - 2*vp.gp[cx+2*xs] + vp.gp[cx+2*xs-2];
                px.gp[cx] = (da + 4*db + 6*dc + 4*dd + de)/64.;
            }
        }
    }

    else {
        px = make_FSGraph(xs, ys, zs);
        if (px.gp==NULL) return px;

        for (z=2; z<zs-2; z++) {
            cz = z*ps;
            for (y=2; y<ys-2; y++) {
                cy = cz + y*xs;
                for (x=2; x<xs-2; x++) {
                    cx = cy + x;
                    da = vp.gp[cx   +2]    - 2*vp.gp[cx]       + vp.gp[cx-2];    
                    db = vp.gp[cx+xs+2]    - 2*vp.gp[cx+xs]    + vp.gp[cx+xs-2];     
                    dc = vp.gp[cx-xs+2]    - 2*vp.gp[cx-xs]    + vp.gp[cx-xs-2];    
                    dd = vp.gp[cx+ps+2]    - 2*vp.gp[cx+ps]    + vp.gp[cx+ps-2];     
                    de = vp.gp[cx-ps+2]    - 2*vp.gp[cx-ps]    + vp.gp[cx-ps-2];     
                    df = vp.gp[cx+xs+ps+2] - 2*vp.gp[cx+xs+ps] + vp.gp[cx+xs+ps-2];
                    dg = vp.gp[cx+xs-ps+2] - 2*vp.gp[cx+xs-ps] + vp.gp[cx+xs-ps-2];
                    dh = vp.gp[cx-xs+ps+2] - 2*vp.gp[cx-xs+ps] + vp.gp[cx-xs+ps-2];
                    di = vp.gp[cx-xs-ps+2] - 2*vp.gp[cx-xs-ps] + vp.gp[cx-xs-ps-2];
                    dj = vp.gp[cx+2*xs+2]  - 2*vp.gp[cx+2*xs]  + vp.gp[cx+2*xs-2];  
                    dk = vp.gp[cx-2*xs+2]  - 2*vp.gp[cx-2*xs]  + vp.gp[cx-2*xs-2];  
                    dl = vp.gp[cx+2*ps+2]  - 2*vp.gp[cx+2*ps]  + vp.gp[cx+2*ps-2]; 
                    dm = vp.gp[cx-2*ps+2]  - 2*vp.gp[cx-2*ps]  + vp.gp[cx-2*ps-2]; 
                    px.gp[cx] = (8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144.;
                }
            }
        }
    }

    return px;
}



/**
WSGraph  yySobel(WSGraph vp) 

グラフィックデータの y方向の2階微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
WSGraph  yySobel(WSGraph vp)
{
    int  x, y, z, xs, ys, zs, cx, cy, cz, ps;
    int  da, db, dc, dd, de;
    int  df, dg, dh, di, dj, dk, dl, dm;
    WSGraph py;
    
    memset(&py, 0, sizeof(WSGraph));
    if (vp.gp==NULL) {
        py.state = JBXL_GRAPH_NODATA_ERROR;
        return py;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    ps = xs*ys;

    if (zs<5) { 
        py = make_WSGraph(xs, ys, 1);
        if (py.gp==NULL) return py;

        for (y=2; y<ys-2; y++) {
            cy = y*xs;
            for (x=2; x<xs-2; x++) {
                cx = cy + x;
                da = vp.gp[cx+2*xs-2] - 2*vp.gp[cx-2] + vp.gp[cx-2*xs-2];
                db = vp.gp[cx+2*xs-1] - 2*vp.gp[cx-1] + vp.gp[cx-2*xs-1]; 
                dc = vp.gp[cx+2*xs]   - 2*vp.gp[cx]   + vp.gp[cx-2*xs]; 
                dd = vp.gp[cx+2*xs+1] - 2*vp.gp[cx+1] + vp.gp[cx-2*xs+1]; 
                de = vp.gp[cx+2*xs+2] - 2*vp.gp[cx+2] + vp.gp[cx-2*xs+2];
                py.gp[cx] = (sWord)((da + 4*db + 6*dc + 4*dd + de)/64);
            }
        }
    }
    else {
        py = make_WSGraph(xs, ys, zs);
        if (py.gp==NULL) return py;

        for (z=2; z<zs-2; z++) {
            cz = z*ps;
            for (y=2; y<ys-2; y++) {
                cy = cz + y*xs;
                for (x=2; x<xs-2; x++) {
                    cx = cy + x;
                    da = vp.gp[cx+2*xs]      - 2*vp.gp[cx]      + vp.gp[cx-2*xs];    
                    db = vp.gp[cx+1+2*xs]    - 2*vp.gp[cx+1]    + vp.gp[cx+1-2*xs]; 
                    dc = vp.gp[cx-1+2*xs]    - 2*vp.gp[cx-1]    + vp.gp[cx-1-2*xs];
                    dd = vp.gp[cx+ps+2*xs]   - 2*vp.gp[cx+ps]   + vp.gp[cx+ps-2*xs]; 
                    de = vp.gp[cx-ps+2*xs]   - 2*vp.gp[cx-ps]   + vp.gp[cx-ps-2*xs]; 
                    df = vp.gp[cx+1+ps+2*xs] - 2*vp.gp[cx+1+ps] + vp.gp[cx+1+ps-2*xs]; 
                    dg = vp.gp[cx+1-ps+2*xs] - 2*vp.gp[cx+1-ps] + vp.gp[cx+1-ps-2*xs]; 
                    dh = vp.gp[cx-1+ps+2*xs] - 2*vp.gp[cx-1+ps] + vp.gp[cx-1+ps-2*xs]; 
                    di = vp.gp[cx-1-ps+2*xs] - 2*vp.gp[cx-1-ps] + vp.gp[cx-1-ps-2*xs]; 
                    dj = vp.gp[cx+2+2*xs]    - 2*vp.gp[cx+2]    + vp.gp[cx+2-2*xs];
                    dk = vp.gp[cx-2+2*xs]    - 2*vp.gp[cx-2]    + vp.gp[cx-2-2*xs];
                    dl = vp.gp[cx+2*ps+2*xs] - 2*vp.gp[cx+2*ps] + vp.gp[cx+2*ps-2*xs];
                    dm = vp.gp[cx-2*ps+2*xs] - 2*vp.gp[cx-2*ps] + vp.gp[cx-2*ps-2*xs];
                    py.gp[cx] = (sWord)((8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144);
                }
            }
        }
    }

    return py;
}



/**
FSGraph  fyySobel(FSGraph vp) 

グラフィックデータの y方向の2階微分(Sobel)を実数計算する．
精度が上昇するが時間がかかる．

@param  vp  計算対象となる double型グラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
FSGraph  fyySobel(FSGraph vp)
{
    int  x, y, z, xs, ys, zs, cx, cy, cz, ps;
    double da, db, dc, dd, de;
    double df, dg, dh, di, dj, dk, dl, dm;
    FSGraph py;

    memset(&py, 0, sizeof(FSGraph));
    if (vp.gp==NULL) {
        py.state = JBXL_GRAPH_NODATA_ERROR;
        return py;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    ps = xs*ys;

    if (zs<5) { 
        py = make_FSGraph(xs, ys, 1);
        if (py.gp==NULL) return py;

        for (y=2; y<ys-2; y++) {
            cy = y*xs;
            for (x=2; x<xs-2; x++) {
                cx = cy + x;
                da = vp.gp[cx+2*xs-2] - 2*vp.gp[cx-2] + vp.gp[cx-2*xs-2];
                db = vp.gp[cx+2*xs-1] - 2*vp.gp[cx-1] + vp.gp[cx-2*xs-1]; 
                dc = vp.gp[cx+2*xs]   - 2*vp.gp[cx]   + vp.gp[cx-2*xs]; 
                dd = vp.gp[cx+2*xs+1] - 2*vp.gp[cx+1] + vp.gp[cx-2*xs+1]; 
                de = vp.gp[cx+2*xs+2] - 2*vp.gp[cx+2] + vp.gp[cx-2*xs+2];
                py.gp[cx] = (da + 4*db + 6*dc + 4*dd + de)/64.;
            }
        }
    }
    else {
        py = make_FSGraph(xs, ys, zs);
        if (py.gp==NULL) return py;

        for (z=2; z<zs-2; z++) {
            cz = z*ps;
            for (y=2; y<ys-2; y++) {
                cy = cz + y*xs;
                for (x=2; x<xs-2; x++) {
                    cx = cy + x;
                    da = vp.gp[cx+2*xs]      - 2*vp.gp[cx]      + vp.gp[cx-2*xs];    
                    db = vp.gp[cx+1+2*xs]    - 2*vp.gp[cx+1]    + vp.gp[cx+1-2*xs]; 
                    dc = vp.gp[cx-1+2*xs]    - 2*vp.gp[cx-1]    + vp.gp[cx-1-2*xs];
                    dd = vp.gp[cx+ps+2*xs]   - 2*vp.gp[cx+ps]   + vp.gp[cx+ps-2*xs]; 
                    de = vp.gp[cx-ps+2*xs]   - 2*vp.gp[cx-ps]   + vp.gp[cx-ps-2*xs]; 
                    df = vp.gp[cx+1+ps+2*xs] - 2*vp.gp[cx+1+ps] + vp.gp[cx+1+ps-2*xs]; 
                    dg = vp.gp[cx+1-ps+2*xs] - 2*vp.gp[cx+1-ps] + vp.gp[cx+1-ps-2*xs]; 
                    dh = vp.gp[cx-1+ps+2*xs] - 2*vp.gp[cx-1+ps] + vp.gp[cx-1+ps-2*xs]; 
                    di = vp.gp[cx-1-ps+2*xs] - 2*vp.gp[cx-1-ps] + vp.gp[cx-1-ps-2*xs]; 
                    dj = vp.gp[cx+2+2*xs]    - 2*vp.gp[cx+2]    + vp.gp[cx+2-2*xs];
                    dk = vp.gp[cx-2+2*xs]    - 2*vp.gp[cx-2]    + vp.gp[cx-2-2*xs];
                    dl = vp.gp[cx+2*ps+2*xs] - 2*vp.gp[cx+2*ps] + vp.gp[cx+2*ps-2*xs];
                    dm = vp.gp[cx-2*ps+2*xs] - 2*vp.gp[cx-2*ps] + vp.gp[cx-2*ps-2*xs];
                    py.gp[cx] = (8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144.;
                }
            }
        }
    }

    return py;
}



/**
WSGraph  zzSobel(WSGraph vp) 

  グラフィックデータの z方向の2階微分(Sobel)を計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
WSGraph  zzSobel(WSGraph vp)
{
    int  x, y, z, xs, ys, zs, cx, cy, cz, ps;
    int  da, db, dc, dd, de;
    int  df, dg, dh, di, dj, dk, dl, dm;
    WSGraph pz;
    
    memset(&pz, 0, sizeof(WSGraph));
    if (vp.gp==NULL) {
        pz.state = JBXL_GRAPH_NODATA_ERROR;
        return pz;
    }

    if (vp.zs<5) {
        //fprintf(stderr,"ZZSOBEL: no 3D data inputed.\n");
        pz.state = JBXL_GRAPH_IVDARG_ERROR;
        return pz;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    ps = xs*ys;
    pz = make_WSGraph(xs, ys, zs);
    if (pz.gp==NULL) return pz;

    for (z=2; z<zs-2; z++) {
        cz = z*ps;
        for (y=2; y<ys-2; y++) {
            cy = cz + y*xs;
            for (x=2; x<xs-2; x++) {
                cx = cy + x;
                da = vp.gp[cx   +2*ps]   - 2*vp.gp[cx]       + vp.gp[cx   -2*ps];    
                db = vp.gp[cx+1 +2*ps]   - 2*vp.gp[cx+1]     + vp.gp[cx+1 -2*ps]; 
                dc = vp.gp[cx-1 +2*ps]   - 2*vp.gp[cx-1]     + vp.gp[cx-1 -2*ps];
                dd = vp.gp[cx+xs+2*ps]   - 2*vp.gp[cx+xs]    + vp.gp[cx+xs-2*ps]; 
                de = vp.gp[cx-xs+2*ps]   - 2*vp.gp[cx-xs]    + vp.gp[cx-xs-2*ps]; 
                df = vp.gp[cx+1+xs+2*ps] - 2*vp.gp[cx+1+xs]  + vp.gp[cx+1+xs-2*ps]; 
                dg = vp.gp[cx+1-xs+2*ps] - 2*vp.gp[cx+1-xs]  + vp.gp[cx+1-xs-2*ps]; 
                dh = vp.gp[cx-1+xs+2*ps] - 2*vp.gp[cx-1+xs]  + vp.gp[cx-1+xs-2*ps]; 
                di = vp.gp[cx-1-xs+2*ps] - 2*vp.gp[cx-1-xs]  + vp.gp[cx-1-xs-2*ps]; 
                dj = vp.gp[cx+2   +2*ps] - 2*vp.gp[cx+2]     + vp.gp[cx+2   -2*ps];
                dk = vp.gp[cx-2   +2*ps] - 2*vp.gp[cx-2]     + vp.gp[cx-2   -2*ps];
                dl = vp.gp[cx+2*xs+2*ps] - 2*vp.gp[cx+2*xs]  + vp.gp[cx+2*xs-2*ps];
                dm = vp.gp[cx-2*xs+2*ps] - 2*vp.gp[cx-2*xs]  + vp.gp[cx-2*xs-2*ps];
                pz.gp[cx] = (sWord)((8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144);
            }
        }
    }

    cz = ps;
    for (y=2; y<ys-2; y++) {
        cy = cz + y*xs;
        for (x=2; x<xs-2; x++) {
            cx = cy + x;
            da = vp.gp[cx   +2*ps]   - 2*vp.gp[cx];  
            db = vp.gp[cx+1 +2*ps]   - 2*vp.gp[cx+1]; 
            dc = vp.gp[cx-1 +2*ps]   - 2*vp.gp[cx-1];
            dd = vp.gp[cx+xs+2*ps]   - 2*vp.gp[cx+xs]; 
            de = vp.gp[cx-xs+2*ps]   - 2*vp.gp[cx-xs]; 
            df = vp.gp[cx+1+xs+2*ps] - 2*vp.gp[cx+1+xs]; 
            dg = vp.gp[cx+1-xs+2*ps] - 2*vp.gp[cx+1-xs]; 
            dh = vp.gp[cx-1+xs+2*ps] - 2*vp.gp[cx-1+xs]; 
            di = vp.gp[cx-1-xs+2*ps] - 2*vp.gp[cx-1-xs]; 
            dj = vp.gp[cx+2   +2*ps] - 2*vp.gp[cx+2];
            dk = vp.gp[cx-2   +2*ps] - 2*vp.gp[cx-2];
            dl = vp.gp[cx+2*xs+2*ps] - 2*vp.gp[cx+2*xs];
            dm = vp.gp[cx-2*xs+2*ps] - 2*vp.gp[cx-2*xs];
            pz.gp[cx] = (sWord)((8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144);
        }
    }

    cz = (zs-2)*ps;
    for (y=2; y<ys-2; y++) {
        cy = cz + y*xs;
        for (x=2; x<xs-2; x++) {
            cx = cy + x;
            da = - 2*vp.gp[cx]      + vp.gp[cx   -2*ps];     
            db = - 2*vp.gp[cx+1]    + vp.gp[cx+1 -2*ps]; 
            dc = - 2*vp.gp[cx-1]    + vp.gp[cx-1 -2*ps];
            dd = - 2*vp.gp[cx+xs]   + vp.gp[cx+xs-2*ps]; 
            de = - 2*vp.gp[cx-xs]   + vp.gp[cx-xs-2*ps]; 
            df = - 2*vp.gp[cx+1+xs] + vp.gp[cx+1+xs-2*ps]; 
            dg = - 2*vp.gp[cx+1-xs] + vp.gp[cx+1-xs-2*ps]; 
            dh = - 2*vp.gp[cx-1+xs] + vp.gp[cx-1+xs-2*ps]; 
            di = - 2*vp.gp[cx-1-xs] + vp.gp[cx-1-xs-2*ps]; 
            dj = - 2*vp.gp[cx+2]    + vp.gp[cx+2   -2*ps];
            dk = - 2*vp.gp[cx-2]    + vp.gp[cx-2   -2*ps];
            dl = - 2*vp.gp[cx+2*xs] + vp.gp[cx+2*xs-2*ps];
            dm = - 2*vp.gp[cx-2*xs] + vp.gp[cx-2*xs-2*ps];
            pz.gp[cx] = (sWord)((8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144);
        }
    }

    return pz;
}



/**
FSGraph  fzzSobel(FSGraph vp) 

グラフィックデータの z方向の2階微分(Sobel)を実数計算する．
精度が上昇するが時間がかかる．

@param  vp  計算対象となる double型グラフィックデータ構造体．
@return 処理されたグラフィックデータ．
*/
FSGraph  fzzSobel(FSGraph vp)
{
    int  x, y, z, xs, ys, zs, cx, cy, cz, ps;
    double da, db, dc, dd, de;
    double df, dg, dh, di, dj, dk, dl, dm;
    FSGraph pz;
    
    memset(&pz, 0, sizeof(FSGraph));
    if (vp.gp==NULL) {
        pz.state = JBXL_GRAPH_NODATA_ERROR;
        return pz;
    }

    if (vp.zs<5) {
        //fprintf(stderr,"FZZSOBEL: no 3D data inputed.\n");
        pz.state = JBXL_GRAPH_IVDARG_ERROR;
        return pz;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    ps = xs*ys;
    pz = make_FSGraph(xs, ys, zs);
    if (pz.gp==NULL) return pz;

    for (z=2; z<zs-2; z++) {
        cz = z*ps;
        for (y=2; y<ys-2; y++) {
            cy = cz + y*xs;
            for (x=2; x<xs-2; x++) {
                cx = cy + x;
                da = vp.gp[cx     +2*ps] - 2*vp.gp[cx]      + vp.gp[cx     -2*ps];   
                db = vp.gp[cx+1   +2*ps] - 2*vp.gp[cx+1]    + vp.gp[cx+1   -2*ps];
                dc = vp.gp[cx-1   +2*ps] - 2*vp.gp[cx-1]    + vp.gp[cx-1   -2*ps];
                dd = vp.gp[cx  +xs+2*ps] - 2*vp.gp[cx  +xs] + vp.gp[cx  +xs-2*ps]; 
                de = vp.gp[cx  -xs+2*ps] - 2*vp.gp[cx  -xs] + vp.gp[cx  -xs-2*ps]; 
                df = vp.gp[cx+1+xs+2*ps] - 2*vp.gp[cx+1+xs] + vp.gp[cx+1+xs-2*ps]; 
                dg = vp.gp[cx+1-xs+2*ps] - 2*vp.gp[cx+1-xs] + vp.gp[cx+1-xs-2*ps]; 
                dh = vp.gp[cx-1+xs+2*ps] - 2*vp.gp[cx-1+xs] + vp.gp[cx-1+xs-2*ps]; 
                di = vp.gp[cx-1-xs+2*ps] - 2*vp.gp[cx-1-xs] + vp.gp[cx-1-xs-2*ps]; 
                dj = vp.gp[cx+2   +2*ps] - 2*vp.gp[cx+2]    + vp.gp[cx+2   -2*ps];
                dk = vp.gp[cx-2   +2*ps] - 2*vp.gp[cx-2]    + vp.gp[cx-2   -2*ps];
                dl = vp.gp[cx+2*xs+2*ps] - 2*vp.gp[cx+2*xs] + vp.gp[cx+2*xs-2*ps];
                dm = vp.gp[cx-2*xs+2*ps] - 2*vp.gp[cx-2*xs] + vp.gp[cx-2*xs-2*ps];
                pz.gp[cx] = (8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144.;
            }
        }
    }

    cz = ps;
    for (y=2; y<ys-2; y++) {
        cy = cz + y*xs;
        for (x=2; x<xs-2; x++) {
            cx = cy + x;
            da = vp.gp[cx   +2*ps]   - 2*vp.gp[cx];  
            db = vp.gp[cx+1 +2*ps]   - 2*vp.gp[cx+1]; 
            dc = vp.gp[cx-1 +2*ps]   - 2*vp.gp[cx-1];
            dd = vp.gp[cx+xs+2*ps]   - 2*vp.gp[cx+xs]; 
            de = vp.gp[cx-xs+2*ps]   - 2*vp.gp[cx-xs]; 
            df = vp.gp[cx+1+xs+2*ps] - 2*vp.gp[cx+1+xs]; 
            dg = vp.gp[cx+1-xs+2*ps] - 2*vp.gp[cx+1-xs]; 
            dh = vp.gp[cx-1+xs+2*ps] - 2*vp.gp[cx-1+xs]; 
            di = vp.gp[cx-1-xs+2*ps] - 2*vp.gp[cx-1-xs]; 
            dj = vp.gp[cx+2   +2*ps] - 2*vp.gp[cx+2];
            dk = vp.gp[cx-2   +2*ps] - 2*vp.gp[cx-2];
            dl = vp.gp[cx+2*xs+2*ps] - 2*vp.gp[cx+2*xs];
            dm = vp.gp[cx-2*xs+2*ps] - 2*vp.gp[cx-2*xs];
            pz.gp[cx] = (8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144.;
        }
    }

    cz = (zs-2)*ps;
    for (y=2; y<ys-2; y++) {
        cy = cz + y*xs;
        for (x=2; x<xs-2; x++) {
            cx = cy + x;
            da = - 2*vp.gp[cx]      + vp.gp[cx   -2*ps];     
            db = - 2*vp.gp[cx+1]    + vp.gp[cx+1 -2*ps]; 
            dc = - 2*vp.gp[cx-1]    + vp.gp[cx-1 -2*ps];
            dd = - 2*vp.gp[cx+xs]   + vp.gp[cx+xs-2*ps]; 
            de = - 2*vp.gp[cx-xs]   + vp.gp[cx-xs-2*ps]; 
            df = - 2*vp.gp[cx+1+xs] + vp.gp[cx+1+xs-2*ps]; 
            dg = - 2*vp.gp[cx+1-xs] + vp.gp[cx+1-xs-2*ps]; 
            dh = - 2*vp.gp[cx-1+xs] + vp.gp[cx-1+xs-2*ps]; 
            di = - 2*vp.gp[cx-1-xs] + vp.gp[cx-1-xs-2*ps]; 
            dj = - 2*vp.gp[cx+2]    + vp.gp[cx+2   -2*ps];
            dk = - 2*vp.gp[cx-2]    + vp.gp[cx-2   -2*ps];
            dl = - 2*vp.gp[cx+2*xs] + vp.gp[cx+2*xs-2*ps];
            dm = - 2*vp.gp[cx-2*xs] + vp.gp[cx-2*xs-2*ps];
            pz.gp[cx] = (8*da+4*(db+dc+dd+de)+2*(df+dg+dh+di)+dj+dk+dl+dm)/144.;
        }
    }

    return pz;
}



/**
VSGraph  vNabra(WSGraph vp) 

グラフィックデータの ナブラを計算する(Sobel)．

@param  vp  計算対象となるグラフィックデータ構造体．
@return ナブラ．ベクトル型グラフィックデータ．
*/
VSGraph  vNabra(WSGraph vp)  // Sobel 
{
    int   i, xs, ys, zs;
    double  xx, yy, zz;
    WSGraph px, py, pz;
    VSGraph pn;
     
    memset(&pn, 0, sizeof(VSGraph));
    if (vp.gp==NULL) {
        pn.state = JBXL_GRAPH_NODATA_ERROR;
        return pn;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    pn = make_VSGraph(xs, ys, zs);
    if (pn.gp==NULL) return pn;

    px = xSobel(vp);
    if (px.gp==NULL) {
        free_VSGraph(&pn);
        pn.state = px.state;
        return pn;
    }
    py = ySobel(vp);
    if (py.gp==NULL) {
        free_VSGraph(&pn);
        free(px.gp);
        pn.state = py.state;
        return pn;
    }

    if (vp.zs<3) {
        for (i=0; i<xs*ys; i++) {
            xx = px.gp[i];
            yy = py.gp[i];
            pn.gp[i] = set_vector(xx, yy, 0.0);
            unit_vector(pn.gp[i]);
        }
    }
    else {
        pz = zSobel(vp);
        if (pz.gp==NULL) {
            free_VSGraph(&pn);
            free(px.gp);
            free(py.gp);
            pn.state = pz.state;
            return pn;
        }

        for (i=0; i<xs*ys*zs; i++) {
            xx = px.gp[i];
            yy = py.gp[i];
            zz = pz.gp[i];
            pn.gp[i] = set_vector(xx, yy, zz);
            unit_vector(pn.gp[i]);
        }
        free(pz.gp);
    }

    free(px.gp);
    free(py.gp);

    return pn;
}



/**
VSGraph  vfNabra(FSGraph vp) 

グラフィックデータの ナブラを実数計算する(Sobel)．
精度が上昇するが時間がかかる．

@param  vp  計算対象となるグラフィックデータ構造体．
@return ナブラ．ベクトル型グラフィックデータ．
*/
VSGraph  vfNabra(FSGraph vp)  // Sobel 
{
    int    i, xs, ys, zs;
    double xx, yy, zz;
    FSGraph px, py, pz;
    VSGraph pn;
     
    memset(&pn, 0, sizeof(VSGraph));
    if (vp.gp==NULL) {
        pn.state = JBXL_GRAPH_NODATA_ERROR;
        return pn;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    pn = make_VSGraph(xs, ys, zs);
    if (pn.gp==NULL) return pn;

    px = fxSobel(vp);
    if (px.gp==NULL) {
        free_VSGraph(&pn);
        pn.state = px.state;
        return pn;
    }
    py = fySobel(vp);
    if (py.gp==NULL) {
        free_VSGraph(&pn);
        free(px.gp);
        pn.state = py.state;
        return pn;
    }

    if (vp.zs<3) {
        for (i=0; i<xs*ys; i++) {
            xx = px.gp[i];
            yy = py.gp[i];
            pn.gp[i] = set_vector(xx, yy, 0.0);
            unit_vector(pn.gp[i]);
        }
    }
    else {
        pz = fzSobel(vp);
        if (pz.gp==NULL) {
            free_VSGraph(&pn);
            free(px.gp);
            free(py.gp);
            pn.state = pz.state;
            return pn;
        }

        for (i=0; i<xs*ys*zs; i++) {
            xx = px.gp[i];
            yy = py.gp[i];
            zz = pz.gp[i];
            pn.gp[i] = set_vector(xx, yy, zz);
            unit_vector(pn.gp[i]);
        }
        free(pz.gp);
    }

    free(px.gp);
    free(py.gp);

    return pn;
}



/**
WSGraph  Nabra(WSGraph vp) 

グラフィックデータの ナブラの絶対値を計算する(Sobel)．

@param  vp  計算対象となるグラフィックデータ構造体．
@return ナブラ．sWord型グラフィックデータ．
*/
WSGraph  Nabra(WSGraph vp)  // Sobel 
{
    int   i, xs, ys, zs;
    int   xx, yy, zz;
    WSGraph px, py, pz, pn;
     
    memset(&pn, 0, sizeof(WSGraph));
    if (vp.gp==NULL) {
        pn.state = JBXL_GRAPH_NODATA_ERROR;
        return pn;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    pn = make_WSGraph(xs, ys, zs);
    if (pn.gp==NULL) return pn;

    px = xSobel(vp);
    if (px.gp==NULL) {
        free_WSGraph(&pn);
        pn.state = px.state;
        return pn;
    }
    py = ySobel(vp);
    if (py.gp==NULL) {
        free_WSGraph(&pn);
        free(px.gp);
        pn.state = py.state;
        return pn;
    }

    if (vp.zs<3) {
        for (i=0; i<xs*ys; i++) {
            xx = px.gp[i];
            yy = py.gp[i];
            pn.gp[i] = (sWord)sqrt(xx*xx + yy*yy);
        }
    }
    else {
        pz = zSobel(vp);
        if (pz.gp==NULL) {
            free_WSGraph(&pn);
            free(px.gp);
            free(py.gp);
            pn.state = pz.state;
            return pn;
        }

        for (i=0; i<xs*ys*zs; i++) {
            xx = px.gp[i];
            yy = py.gp[i];
            zz = pz.gp[i];
            pn.gp[i] = (sWord)sqrt(xx*xx + yy*yy + zz*zz);
        }
        free(pz.gp);
    }

    free(px.gp);
    free(py.gp);

    return pn;
}



/**
WSGraph  fNabra(WSGraph vp) 

グラフィックデータの ナブラの絶対値を実数計算する(Sobel)．
精度が上昇するが時間がかかる．

@param  vp  計算対象となるグラフィックデータ構造体．
@return ナブラ．double型グラフィックデータ．
*/
FSGraph  fNabra(FSGraph vp)  // Sobel 
{
    int    i, xs, ys, zs;
    double xx, yy, zz;
    FSGraph px, py, pz, pn;
     
    memset(&pn, 0, sizeof(FSGraph));
    if (vp.gp==NULL) {
        pn.state = JBXL_GRAPH_NODATA_ERROR;
        return pn;
    }

    xs = vp.xs;
    ys = vp.ys;
    zs = vp.zs;
    pn = make_FSGraph(xs, ys, zs);
    if (pn.gp==NULL) return pn;

    px = fxSobel(vp);
    if (px.gp==NULL) {
        free_FSGraph(&pn);
        pn.state = px.state;
        return pn;
    }
    py = fySobel(vp);
    if (py.gp==NULL) {
        free_FSGraph(&pn);
        free(px.gp);
        pn.state = py.state;
        return pn;
    }

    if (vp.zs<3) {
        for (i=0; i<xs*ys; i++) {
            xx = px.gp[i];
            yy = py.gp[i];
            pn.gp[i] = sqrt(xx*xx + yy*yy);
        }
    }
    else {
        pz = fzSobel(vp);
        if (pz.gp==NULL) {
            free_FSGraph(&pn);
            free(px.gp);
            free(py.gp);
            pn.state = pz.state;
            return pn;
        }

        for (i=0; i<xs*ys*zs; i++) {
            xx = px.gp[i];
            yy = py.gp[i];
            zz = pz.gp[i];
            pn.gp[i] = sqrt(xx*xx + yy*yy + zz*zz);
        }
        free(pz.gp);
    }

    free(px.gp);
    free(py.gp);

    return pn;
}



/**
VSGraph  curvature3D(FSGraph vp)

3Dグラフィックデータの4次元曲率を実数計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 4次元曲率を代入した vector型グラフィックデータ(K,Hの2次元)．
*/
VSGraph  curvature3D(FSGraph vp)
{
    int  i;
    double alph, beta, gamm, K, H;
    double fx, fy, fz, fxy, fyz, fzx, fxx, fyy, fzz, nb;
    FSGraph px, py, pz, pxy, pyz, pzx, pxx, pyy, pzz, nab;
    VSGraph pp;

    memset(&pp, 0, sizeof(VSGraph));
    if (vp.gp==NULL) {
        pp.state = JBXL_GRAPH_NODATA_ERROR;
        return pp;
    }

    if (vp.zs<5) {
        //fprintf(stderr,"CURVATURE3D: z dimension is < 5.\n");
        pp.state = JBXL_GRAPH_IVDARG_ERROR;
        return pp;
    }

    pp = make_VSGraph(vp.xs, vp.ys, vp.zs);
    if (pp.gp==NULL) return pp;

    nab = fNabra(vp);
    px  = fxSobel(vp);
    py  = fySobel(vp);
    pz  = fzSobel(vp);
    pxy = fySobel(px);
    pyz = fzSobel(py);
    pzx = fxSobel(pz);
    pxx = fxxSobel(vp);
    pyy = fyySobel(vp);
    pzz = fzzSobel(vp);

    if (nab.gp==NULL || px.gp==NULL  || py.gp==NULL  || pz.gp==NULL  ||
                        pxy.gp==NULL || pyz.gp==NULL || pzx.gp==NULL ||
                        pxx.gp==NULL || pyy.gp==NULL || pzz.gp==NULL) {
        freeNull(px.gp);
        freeNull(py.gp);
        freeNull(pz.gp);
        freeNull(pxy.gp);
        freeNull(pyz.gp);
        freeNull(pzx.gp);
        freeNull(pxx.gp);
        freeNull(pyy.gp);
        freeNull(pzz.gp);
        freeNull(nab.gp);
        free_VSGraph(&pp);
        pp.state = JBXL_GRAPH_ERROR;
        return pp;
    }

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
        nb  = nab.gp[i];
        fx  = px.gp[i];
        fy  = py.gp[i];
        fz  = pz.gp[i];
        fxy = pxy.gp[i];
        fyz = pyz.gp[i];
        fzx = pzx.gp[i];
        fxx = pxx.gp[i];
        fyy = pyy.gp[i];
        fzz = pzz.gp[i];

        if (nb*(fx*fx+fy*fy) !=0) {
            alph = (2*fx*fy*fxy - fx*fx*fyy - fy*fy*fxx)/(fx*fx+fy*fy);
            beta = (2*fz*(fx*fx+fy*fy)*(fx*fzx+fy*fyz) - 2*fx*fy*fz*fz*fxy
                        - fx*fx*fz*fz*fxx - fy*fy*fz*fz*fyy 
                        - (fx*fx+fy*fy)*(fx*fx+fy*fy)*fzz)/(nb*nb*(fx*fx+fy*fy));
            gamm = ((fx*fx+fy*fy)*(fy*fzx-fx*fyz) + (fx*fx-fy*fy)*fz*fxy
                        - fx*fy*fz*(fxx-fyy))/(nb*(fx*fx+fy*fy));

            K = alph*beta - gamm*gamm;
            H = (alph + beta)/2;
            pp.gp[i] = set_vector(K, H, 0.0);
        }
    }

    free(px.gp);
    free(py.gp);
    free(pz.gp);
    free(pxy.gp);
    free(pyz.gp);
    free(pzx.gp);
    free(pxx.gp);
    free(pyy.gp);
    free(pzz.gp);
    free(nab.gp);

    return pp;
}



/**
VSGraph  curvature(FSGraph vp)

2Dグラフィックデータの3次元曲率を実数計算する．

@param  vp  計算対象となるグラフィックデータ構造体．
@return 3次元曲率を代入した vector型グラフィックデータ(K,Hの2次元)．
*/
VSGraph  curvature(FSGraph vp)
{
    int   i;
    double K, H, d;
    double Ix, Ixx, Iy, Iyy, Ixy;
    FSGraph px, py, pxy, pxx, pyy;
    VSGraph pp;

    memset(&pp, 0, sizeof(VSGraph));
    if (vp.gp==NULL) {
        pp.state = JBXL_GRAPH_NODATA_ERROR;
        return pp;
    }

    if (vp.zs>1) {
        //fprintf(stderr,"CURVATURE: z dimension is > 1.\n");
        pp.state = JBXL_GRAPH_IVDARG_ERROR;
        return pp;
    }

    pp = make_VSGraph(vp.xs, vp.ys, vp.zs);
    if (pp.gp==NULL) return pp;

    px  = fxSobel(vp);
    py  = fySobel(vp);
    pxy = fySobel(px);
    pxx = fxxSobel(vp);
    pyy = fyySobel(vp);

    if (px.gp==NULL||py.gp==NULL||pxy.gp==NULL||pxx.gp==NULL||pyy.gp==NULL) {
        freeNull(px.gp);
        freeNull(py.gp);
        freeNull(pxy.gp);
        freeNull(pxx.gp);
        freeNull(pyy.gp);
        free_VSGraph(&pp);
        pp.state = JBXL_GRAPH_ERROR;
        return pp;
    }

    for (i=0; i<vp.xs*vp.ys; i++) {
        Ix  = px.gp[i];
        Iy  = py.gp[i];
        Ixy = pxy.gp[i];
        Ixx = pxx.gp[i];
        Iyy = pyy.gp[i];
        d   = 1. + Ix*Ix + Iy*Iy;

        K = (Ixx*Iyy-Ixy*Ixy)/(d*d); 
        H = (Ixx+Ixx*Iy*Iy+Iyy+Iyy*Ix*Ix-2*Ix*Ixy*Iy)/(2.*d*sqrt(d));
        pp.gp[i] = set_vector(K, H, 0.0);
    }

    free(px.gp);
    free(py.gp);
    free(pxy.gp);
    free(pxx.gp);
    free(pyy.gp);

    return pp;
}



/**
WSGraph  curv2WSGraph(VSGraph xp)

3次元曲率データから曲面の形状を判定する．

@param  xp  操作対象となる3次元曲率の入ったグラフィックデータ．
@return 曲面の形状情報を代入した sWord型グラフィックデータ．

@note
曲面の形状の種類は@n 
PEAK, PIT, SADDLE_RIDGE, SADDLE_VALLEY, NONE_SHAPE, MINIMAL, RIDGE, VALLEY, FLAT
*/
WSGraph  curv2WSGraph(VSGraph xp)
{
    int  i;
    WSGraph vp;
    double  K, H;
    
    memset(&vp, 0, sizeof(WSGraph));
    if (xp.gp==NULL) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    vp = make_WSGraph(xp.xs, xp.ys, xp.zs);
    if (vp.gp==NULL) return vp;

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
        K = xp.gp[i].x;
        H = xp.gp[i].y;
        if      (K>0  && H<0)  vp.gp[i] = PEAK;
        else if (K>0  && H>0)  vp.gp[i] = PIT;
        else if (K<0  && H<0)  vp.gp[i] = SADDLE_RIDGE;
        else if (K<0  && H>0)  vp.gp[i] = SADDLE_VALLEY;
        else if (K>0  && H==0) vp.gp[i] = NONE_SHAPE;
        else if (K<0  && H==0) vp.gp[i] = MINIMAL;
        else if (K==0 && H<0)  vp.gp[i] = RIDGE;
        else if (K==0 && H>0)  vp.gp[i] = VALLEY;
        else if (K==0 && H==0) vp.gp[i] = FLAT;
    }

    return vp;
}



/**
WSGraph  WSCurve(WSGraph gx, int mode, int cc)

曲面の形状をグラフィックデータ(輝度値)に変換する．

@param  gx   操作対象となる曲面形状情報3の入ったグラフィックデータ．
@param  mode 輝度値に変換する曲面の形状．指定できる形状は @b ALL, @b FLAT, @b PIT, @b SADDLE_RIDGE, 
             @b SADDLE_VALLEY, @b NONE_SHAPE, @b MINIMAL, @b RIDGE, @b VALLEY, @b PEAK. @n
             ALLを指定した場合,形状は違った輝度値に変換される．
@param  cc   modeに ALLを指定しなかった場合,指定された曲面をこの輝度値に変換する．

@return 曲面の形状情報を代入した sWord型グラフィックデータ．

@note
FLAT=500, PIT=1000, SADDLE_RIDGE=1500, 
SADDLE_VALLEY=2000, NONE_SHAPE=2500, MINIMAL=3000, 
RIDGE=3500, VALLEY=4000, PEAK=4500
*/
WSGraph  WSCurve(WSGraph gx, int mode, int cc)
{
    int  i;
    FSGraph wp;
    VSGraph xp;
    WSGraph vp;

    memset(&vp, 0, sizeof(WSGraph));
    if (gx.gp==NULL) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    wp = W2FSGraph(gx);
    if (gx.zs<5) xp = curvature(wp);
    else         xp = curvature3D(wp);

    vp = curv2WSGraph(xp);
    freeNull(wp.gp);
    freeNull(xp.gp);
    if (vp.gp==NULL) return vp;

    if (mode==ALL) {
        for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
            if    (vp.gp[i]==FLAT)            vp.gp[i] = 500;
            else if (vp.gp[i]==PIT)           vp.gp[i] = 1000;
            else if (vp.gp[i]==SADDLE_RIDGE)  vp.gp[i] = 1500;
            else if (vp.gp[i]==SADDLE_VALLEY) vp.gp[i] = 2000;
            else if (vp.gp[i]==NONE_SHAPE)    vp.gp[i] = 2500;
            else if (vp.gp[i]==MINIMAL)       vp.gp[i] = 3000;
            else if (vp.gp[i]==RIDGE)         vp.gp[i] = 3500;
            else if (vp.gp[i]==VALLEY)        vp.gp[i] = 4000;
            else if (vp.gp[i]==PEAK)          vp.gp[i] = 4500;
            else                              vp.gp[i] = 0;
        }
    }
    else {
        for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
            if ((vp.gp[i]&mode)!=0) vp.gp[i] = cc;
            else                    vp.gp[i] = 0;
        }
    }
    return vp;
}



/**
WSGraph  edge_enhance(WSGraph gd, int mode)

2Dグラフィックデータのラプラシアンを使ったエッジ強調．

@param  gd    計算対象となるグラフィックデータ構造体．
@param  mode  モード．@n
              @b 4: 4近傍ラプラシアン．@n
              @b 8: 8近傍ラプラシアン  3x3 @n 
              @b その他: Sobelのラプラシアン(24近傍) 5x5 @n

@return  処理されたグラフィックデータ．
*/
WSGraph  edge_enhance(WSGraph gd, int mode)
{
    int  i;
    WSGraph la, vp;

    memset(&vp, 0, sizeof(WSGraph));
    if (gd.gp==NULL) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    la = Laplacian(gd, mode);  
    if (la.gp==NULL) return la;

    vp = make_WSGraph(gd.xs, gd.ys, 1);
    if (vp.gp==NULL) return vp;

    for (i=0; i<vp.xs*vp.ys; i++) vp.gp[i] = gd.gp[i] - la.gp[i];
    return vp;
}




/////////////////////////////////////////////////////////////////////////////
// Filter

/**
FMask  gauss_mask(double sig, int ms, int md) 

ガウシアン処理用のフィルタをつくり出す．

@param  sig  ガウス関数のσ．
@param  ms   フィルタの大きさ．  
@param  md   モード．@b 2: 2次元．@b その他: 3次元

@return ガウシアン用フィルタ
*/
FMask  gauss_mask(double sig, int ms, int md) 
{
    int    xx, yy, zz, ns, cp, dx, ps, sw;
    double min, *fm;
    FMask mask;
    
    mask.mode  = 0;
    mask.msize = 0;
    mask.imask = NULL;

    if (md<=2) {    // 2D 
        md = 2;
        ps = ms*ms;
        sw = 0;
    }
    else {          // 3D 
        md = 3;
        ps = ms*ms*ms;
        sw = 1;
    }

    ns  = ms/2;
    min = (double)SINTMAX;
    fm = (double*)malloc(ps*sizeof(double));
    mask.imask = (int*)malloc(ps*sizeof(int));
    if (fm==NULL || mask.imask==NULL) {
        free(fm);
        free(mask.imask);
        memset(&mask, 0, sizeof(FMask));
        return mask;
    }

    for (zz=-ns*sw; zz<=ns*sw; zz++) {
        for (yy=-ns; yy<=ns; yy++) {
            for (xx=-ns; xx<=ns; xx++) {
                cp = (zz+ns)*ms*ms*sw + (yy+ns)*ms + (xx+ns);
                fm[cp] = exp(-(xx*xx+yy*yy+zz*zz)/(sig*sig));
                if (fm[cp]!=0.0) min = Min(min, fm[cp]);
            }
        }
    }

    dx = 0;
    for (xx=0; xx<ps; xx++) {
        mask.imask[xx] = (int)(fm[xx]/min+0.5);
        dx += mask.imask[xx];
    }

    mask.msize = ms;
    mask.nfact = dx;
    mask.mode  = md;

    free(fm);
    return mask; 
}



/**
WSGraph  imask(WSGraph xp, FMask mask) 

フィルタ処理を行なう．

@param  xp    対象となるグラフィックデータ構造体．
@param  mask  処理用マスク．

@return マスク処理されたグラフィックデータ．
*/
WSGraph  imask(WSGraph xp, FMask mask)
{
    int    i, x, y, z, cx;
    int    xx, yy, zz, cp, cw, sw;
    int    kc, xc, xs, ps, pm, mz, zc;
    int    ms, nf, min;
    double dd;
    WSGraph vp;

    memset(&vp, 0, sizeof(WSGraph));
    if (xp.gp==NULL) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    if (xp.zs<=1 && mask.mode>2) {
        //fprintf(stderr, "IMASK: mismach mask dimension %d %d\n", xp.zs, mask.mode);
        vp.state = JBXL_GRAPH_IVDARG_ERROR;
        return vp;
    }

    nf = mask.nfact;
    ms = mask.msize;
    if (mask.mode==2) {
        sw = 0;
        pm = ms*ms;
    }
    else {
        sw = 1;
        pm = ms*ms*ms;
    }

    mz = Min(ms, xp.zs);
    kc = pm/2;
    zc = mz/2;
    xc = ms/2;
    xs = xp.xs;
    ps = xp.xs*xp.ys;

    min = SINTMAX;
    for (i=0; i<xp.xs*xp.ys; i++) min = Min(min, xp.gp[i]);
    vp = make_WSGraph(xp.xs, xp.ys, 1);
    if (vp.gp==NULL) return vp;

    for (i=0; i<vp.xs*vp.ys; i++) vp.gp[i] = min;

    z = xp.zs/2; 
    for (y=xc; y<xp.ys-xc; y++) 
    for (x=xc; x<xp.xs-xc; x++) {
        cx = z*ps + y*xs + x;
        dd = 0.0;
        for (zz=-zc*sw; zz<=zc*sw; zz++)
        for (yy=-xc; yy<=xc; yy++)
        for (xx=-xc; xx<=xc; xx++) {
            cp = kc + xx + yy*ms + zz*ms*ms;
            cw = cx + xx + yy*xs + zz*ps;
            dd = dd + (double)xp.gp[cw]*mask.imask[cp];
        }
        vp.gp[y*xs + x] = (sWord)(dd/nf);
    }
    return vp;
}



/**
WSGraph  median(WSGraph xp, int ms)

メディアンフィルタ処理を行なう．3D処理可．

@param  xp  対象となるグラフィックデータ構造体．
@param  ms  フィルタの大きさ．

@return メディアンフィルタ処理されたグラフィックデータ．
*/
WSGraph  median(WSGraph xp, int ms)  /* 3D */
{
    int   i, j, x, y, z;
    int   xx, yy, zz, cw, ux, mz;
    int   kc, xc, zc, xs, ps, cx;
    WSGraph vp;
    sWord   *me;

    memset(&vp, 0, sizeof(WSGraph));
    if (xp.gp==NULL) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    mz = Min(ms, xp.zs);
    me = (sWord*)malloc(ms*ms*mz*sizeof(sWord));
    if (me==NULL) {
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    kc = ms*ms*mz/2;
    xc = ms/2;
    zc = mz/2;
    xs = xp.xs;
    ps = xp.xs*xp.ys;
    vp = make_WSGraph(vp.xs, vp.ys, vp.zs);
    if (vp.gp==NULL) {
        free(me);
        return vp;
    }
    
    z = xp.zs/2;
    for(y=xc; y<xp.ys-xc; y++) {
        for(x=xc; x<xp.xs-xc; x++) {
            cx = z*ps + y*xs + x;
            i  = 0;
            for (zz=-zc; zz<=zc; zz++) {
                for (yy=-xc; yy<=xc; yy++) {
                    for (xx=-xc; xx<=xc; xx++) {
                        cw = cx + xx + yy*xs + zz*ps;
                        me[i++] = xp.gp[cw];
                    }
                }
            }

            for (i=0; i<ms*ms*mz-1; i++) {
                for (j=i+1; j<ms*ms*mz; j++) {
                    if (me[i]<me[j]) {
                        ux  = me[i];
                        me[i] = me[j];
                        me[j] = ux;
                    }
                }
            }
            vp.gp[cx-z*ps] = me[kc];
        }
    }

    free(me);
    return vp;
}



/**
WSGraph  to2d(WSGraph gd, int mode)

3Dグラフィックを2Dへ射影する(MIP画像)．

@param  gd    操作対象となる3Dグラフィックデータ構造体．
@param  mode  モード． 
              @b SIDEX_VIEW: x方向から射影する．@n
              @b SIDEY_VIEW: y方向から射影する．@n
              @b SIDEZ_VIEW: z方向から射影する．@n
              @b TOP_VIEW:   z方向から射影する．@n
              @b TOP_VIEW_DEPTH: z方向から射影する．ただし,z軸に比例して画像に濃淡を付加する．@n

@return 射影された2Dグラフィックデータ(WSGraph, MIP画像)
*/
WSGraph  to2d(WSGraph gd, int mode)
{
    int  i, j, k, psize;
    int  cx, cy, cz, cw, xx, yy;
    WSGraph vp;

    memset(&vp, 0, sizeof(WSGraph));
    if (gd.gp==NULL) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }

    psize = gd.xs*gd.ys;

    if (mode==TOP_VIEW) {
        vp = make_WSGraph(gd.xs, gd.ys, 1);
        if (vp.gp==NULL) return vp;

        for (k=0; k<gd.zs; k++) {
            cz = k*psize;
            for (j=0; j<gd.ys; j++) {
                cy = j*gd.xs;
                for (i=0; i<gd.xs; i++) {
                    cx = cz + cy + i;
                    cw = cy + i;
                    if (gd.gp[cx]!=0) vp.gp[cw] = Max(vp.gp[cw], gd.gp[cx]);
                }
            }
        }
    }

    else if (mode==TOP_VIEW_DEPTH) {
        vp = make_WSGraph(gd.xs, gd.ys, 1);
        if (vp.gp==NULL) return vp;

        for (k=0; k<gd.zs; k++) {
            cz = k*psize;
            for (j=0; j<gd.ys; j++) {
                cy = j*gd.xs;
                for (i=0; i<gd.xs; i++) {
                    cx = cz + cy + i;
                    cw = cy + i;
                    if (gd.gp[cx]!=0) vp.gp[cw] = Max(vp.gp[cw], (gd.zs-k)+100);
                }
            }
        }
    }

    else if (mode==SIDEX_VIEW) {
        vp = make_WSGraph(gd.ys, gd.zs, 1);
        if (vp.gp==NULL) return vp;

        for (k=0; k<gd.zs; k++) {
            cz = k*psize;
            yy = k;
            for (j=0; j<gd.ys; j++) {
                cy = j*gd.xs;
                xx = gd.ys - 1 - j;
                for (i=0; i<gd.xs; i++) {
                    cx = cz + cy + i;
                    cw = yy*vp.xs + xx;
                    if (gd.gp[cx]!=0) vp.gp[cw] = Max(vp.gp[cw], gd.gp[cx]);
                }
            }
        }
    }

    else if (mode==SIDEY_VIEW) {
        vp = make_WSGraph(gd.xs, gd.zs, 1);
        if (vp.gp==NULL) return vp;

        for (k=0; k<gd.zs; k++) {
            cz = k*psize;
            yy = k;
            for (j=0; j<gd.ys; j++) {
                cy = j*gd.xs;
                for (i=0; i<gd.xs; i++) {
                    cx = cz + cy + i;
                    xx = i;
                    cw = yy*vp.xs + xx;
                    if (gd.gp[cx]!=0) vp.gp[cw] = Max(vp.gp[cw], gd.gp[cx]);
                }
            }
        }
    }

    else {
        memset(&vp, 0, sizeof(WSGraph));
        vp.state = JBXL_GRAPH_IVDARG_ERROR;
        //fprintf(stderr,"TO2D: unknown mode = %d.\n",mode);
    }

    return vp;
}



/**
WSGraph  euclid_distance(WSGraph vp, int* rr, int bc)

WSGグラフィック上を2値化し,各点における輝度値0の点からのユークリッド距離の最小を求める．

@param  vp   操作対象となるグラフィックデータ構造体．
@param  *rr  指定しない．画像中のユークリッド距離の最大値が入る．
@param  bc   輝度値の2値化の値．これより小さいものは0,これ以上は1．
 
@return 輝度値の代わりにユークリッド距離が記入されたグラフィックデータ．
*/
WSGraph  euclid_distance(WSGraph vp, int* rr, int bc)
{
    int  i, j, k, l;
    int  df, db, d, w; 
    int  rmax, rstart, rend;
    WSGraph wp;
    ISGraph pp, buff;

    memset(&wp, 0, sizeof(WSGraph));
    if (vp.gp==NULL) {
        wp.state = JBXL_GRAPH_NODATA_ERROR;
        return wp;
    }

    wp = make_WSGraph(vp.xs, vp.ys, vp.zs);
    if (wp.gp==NULL) return wp;

    pp = make_ISGraph(vp.xs, vp.ys, vp.zs);
    if (pp.gp==NULL) {
        free_WSGraph(&wp);
        wp.state = pp.state;
        return wp;
    }

    for (i=0; i<vp.xs*vp.ys*vp.zs; i++) {
         if (vp.gp[i]>=bc) pp.gp[i] = 1;
         else              pp.gp[i] = 0;
    }

    for (k=1; k<=vp.zs; k++) {
        for (j=1; j<=vp.ys; j++) {
            df = vp.xs;
            for (i=1; i<=vp.xs; i++) {
                if (Vxt(pp, i, j, k)!=0) df = df + 1;
                else                     df = 0;
                Vxt(pp, i, j, k) = df*df;
            }
        }
    }
    
    for (k=1; k<=vp.zs; k++) {
        for (j=1; j<=vp.ys; j++) {
            db = vp.xs;
            for (i=vp.xs; i>=1; i--) {
                if (Vxt(pp, i, j, k)!=0) db = db + 1;
                else                     db = 0;
                Vxt(pp, i, j, k) = Min(Vxt(pp, i, j, k), db*db);
            }
        }
    }

    buff = make_ISGraph(vp.ys, 1, 1);
    for (k=1; k<=vp.zs; k++) {
        for (i=1; i<=vp.xs; i++) {
            for (j=1; j<=vp.ys; j++) {
                Lxt(buff, j) = Vxt(pp, i, j, k);
            }
            for (j=1; j<=vp.ys; j++) {
                d = Lxt(buff, j);
                if (d!=0) {
                    rmax   = (int)sqrt((double)d) + 1;
                    rstart = Min(rmax, j-1);
                    rend   = Min(rmax, vp.ys-j);
                    for (l=-rstart; l<=rend; l++) {
                        w = Lxt(buff, j+l) + l*l;
                        if (w<d) d = w;
                    }
                }
                Vxt(pp, i, j, k) = d;
            }
        }
    }
    free(buff.gp);

    *rr = 0;
    buff = make_ISGraph(vp.zs, 1, 1);
    for (j=1; j<=vp.ys; j++) {
        for (i=1; i<=vp.xs; i++) {
            for (k=1; k<=vp.zs; k++) {
                Lxt(buff, k) = Vxt(pp, i, j, k);
            }
            for (k=1; k<=vp.zs; k++) {
                d = Lxt(buff, k);
                if (d!=0) {
                    rmax   = (int)sqrt((double)d) + 1;
                    rstart = Min(rmax, k-1);
                    rend   = Min(rmax, vp.zs-k);
                    for (l=-rstart; l<=rend; l++) {
                        w = Lxt(buff, k+l) + l*l;
                        if (w<d) d = w;
                    }
                    *rr = Max(*rr, d);
                }
                Vxt(pp, i, j, k) = d;
            }
        }
    }
    free(buff.gp);

    for (i=0; i<wp.xs*wp.ys*wp.zs; i++) {
        wp.gp[i] = (sWord)pp.gp[i];
        if (pp.gp[i]>32767) {
            fprintf(stderr,"EUCLID_DISTANCE: WARNING: distance is too long = %d!\n",pp.gp[i]);
        }
    }
    free(pp.gp);

    return wp;
}



/**
int  out_round(WSGraph vp, int x, int y, IRBound* rb, int mode)

2Dグラフィックデータ構造体vpの(x,y)にあるオブジェクトの周囲長を得る．

@param       vp    操作対象となる 2D グラフィックデータ構造体．
@param       x, y  情報を得たいオブジェクトの左上縁の座標．
                   この座標の左横に情報を得たいオブジェクトの一部が在ってはいけない．
@param[out]  rb    オブジェクトの情報を格納する境界構造体．@n
                   rb->xmin: オブジェクトの x座標の最小値．@n
                   rb->xmax: オブジェクトの x座標の最大値．@n
                   rb->ymin: オブジェクトの y座標の最小値．@n
                   rb->ymax: オブジェクトの y座標の最大値．@n
                   rb->misc: 8近傍モード時の斜めの距離の回数．
                   周囲長を 戻り値 + rb->misc*{sqrt(2.)-1} で計算する場合もある．
@param       mode  モード．@b 8: 8近傍探索．@b その他: 4近傍探索．

@return オブジェクトの周囲長．ただし，8近傍モードの場合，斜めの距離も1と数える．rb->misc 参照．

@attention
注: 1ドットの長さは1と数える．プログラム中で EGMAX+1 を使用．
*/
int  out_round(WSGraph vp, int x, int y, IRBound* rb, int mode)
{
    int  i, j, sp, cp, w, ll, ss;
    int  xx, yy, vx, vy, ix, eflg=OFF;
    int  r8[8]={-1, 1, -1, -1, 1, -1, 1, 1};
    int  r4[8]={ 0, 1, -1,  0, 0, -1, 1, 0};
    int* cc;

    if (vp.gp==NULL) return JBXL_GRAPH_IVDARG_ERROR;

    i = y*vp.xs + x;
    rb->xmax = rb->xmin = x;
    rb->ymax = rb->ymin = y;
    sp = cp = i;
    ss = 0;
    ll = 0;
    vx = 1;
    vy = 0;

    if (vp.gp[sp]==0 || sp==0) {
        //fprintf(stderr,"OUT_ROUND: irregular start point!! sp = %d\n",sp);
        return JBXL_GRAPH_IVDPARAM_ERROR;
    }

    if (mode==8){
        ix = 8;
        cc = r8;
    }
    else if (mode==4) {
        ix = 4;
        cc = r4;
    }
    else {
        //fprintf(stderr,"OUT_ROUND: invalid mode = %d!!\n",mode);
        return JBXL_GRAPH_IVDMODE_ERROR;
    }

    do {
        w  = abs(vx)+abs(vy);
        xx = (vx*cc[0]+vy*cc[1])/w;
        yy = (vx*cc[2]+vy*cc[3])/w;
        for (j=1; j<=ix; j++) {
            if (vp.gp[cp+yy*vp.xs+xx]!=0) {
                vx = xx;
                vy = yy;
                cp = cp + yy*vp.xs + xx;
                xx = cp%vp.xs;
                yy = (cp-xx)/vp.xs;
                rb->xmax = Max(rb->xmax, xx);
                rb->ymax = Max(rb->ymax, yy);
                rb->xmin = Min(rb->xmin, xx);
                rb->ymin = Min(rb->ymin, yy);
                break;
            }
            else {
                if(sp==cp && xx==-1 && yy==0) {
                    eflg = ON;
                    break;
                }
                w  = abs(xx)+abs(yy);
                vx = (xx*cc[4]+yy*cc[5])/w;
                vy = (xx*cc[6]+yy*cc[7])/w;
                xx = vx;
                yy = vy;
            }
        }
        ll++;
        if (abs(vx)+abs(vy)==2) ss++;
        //
    } while(eflg==OFF);

    if (mode==4) ss = 0;
    (rb->xmax)++;
    (rb->ymax)++;
    (rb->xmin)--;
    (rb->ymin)--;
    rb->misc = ss;

    return ll;
}

