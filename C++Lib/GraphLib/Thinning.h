
#ifndef  __JBXL_CPP_THINNING_H_
#define  __JBXL_CPP_THINNING_H_


/**
@brief    細線化関数
@file     Thinning.h
@author   Fumi.Iseki (C)

@see 信学論D-II, Vol.J79-D-II, pp.1675-1685, 1996.
*/


#include "Gmt.h"
#include "tlist.h"


//
namespace jbxl {


template <typename T>  MSGraph<T>  CenterLine(MSGraph<T> gx, int mode);
template <typename T>  int  nonZeroBoxel(MSGraph<T> vp, int n); 
template <typename T>  bool deletable(MSGraph<T> vp, int n, int c, int d);
    
int     connectNumber(int* w, int c, int d);
bool    deletable_s(int* v);
bool    deletable_4(int* v);
bool    deletable_5(int* v);


/**
template <typename T>  MSGraph<T>  centerLine(MSGraph<T> gx, int mode)

3D画像に対して，連結数に基づいた細線化を行う．

@param  gx    3D画像．
@param  mode  @b 26: 26近傍，@b その他: 6近傍

@return 細線化された画像データ．

@see 信学論D-II, Vol.J79-D-II, pp.1675-1685, 1996.
 */
template <typename T>  MSGraph<T>  centerLine(MSGraph<T> gx, int mode)
{
    int   i, j, k, l, m, n, w, b, nn, mm; 
    int   rr, dd, xs, ps;
    
    bool  dt;

    MSGraph<int>   gd;
    MSGraph<T>   vp;
    tList *pp, *px;
    tList_data ld;

    if (mode!=26)  mode = 6;
    xs = gx.xs;
    ps = gx.xs*gx.ys;
    ld = init_tList_data();

    // STEP 1 
    //DEBUG_Warning("center_line: start step1");
    gd = euclidDistance(gx, gd.zero+1, rr);

    dd = SINTMAX;
    rr = 0;
    for (i=0; i<gd.xs*gd.ys*gd.zs; i++) {
        if (gd.gp[i]!=0) {
            gd.gp[i] += 20;
            dd = Min(dd, gd.gp[i]);
            rr = Max(rr, gd.gp[i]);
        }
    }

    // STEP 2
    //DEBUG_Warning("center_line: start step2");
    pp = px = new_tList_node();

    for (k=1; k<gd.zs-1; k++) {
        l = k*ps;
        for (j=1; j<gd.ys-1; j++) {
            m = l + j*xs;
            for (i=1; i<gd.xs-1; i++) {
                n = m + i;
                if (gd.gp[n]>20) {
                    w = gd.gp[n+1]*gd.gp[n-1]*gd.gp[n+xs]*gd.gp[n-xs]   
                                    *gd.gp[n+ps]*gd.gp[n-ps];
                    if(w==0) {
                        ld.id = n;
                        ld.lv = (int)gd.gp[n];
                        px = add_tList_node_bydata(px, ld);
                        gd.gp[n] = 1;
                    }
                }
            }
        }
    }

    //DEBUG_Warning("center_line: start step3, step4 and step5");
    do {
        // STEP 3
        //DEBUG_Warning("center_line: start step3");
        px = pp->next;
        while(px!=NULL) {
            if (px->ldat.lv<=dd) {
                dt = deletable(gd, px->ldat.id, mode, 3);
                if (dt) {
                    m = nonZeroBoxel(gd, px->ldat.id);
                    if (m==1) {
                        tList* pv = px->prev;
                        del_tList_node(&px);
                        px = pv;
                    }
                    else px->ldat.lv = m/3 + 7;
                }
                else {
                    px->ldat.lv = 16;
                }
            }
            px = px->next;
        } 

        // STEP 4
        //DEBUG_Warning("center_line: start step4");
        for (b=7; b<=15; b++) {
            px = pp->next;
            while(px!=NULL) {
                if (px->ldat.lv==b) {
                    dt = deletable(gd, px->ldat.id, mode, 3);
                    if (dt) {
                        m = nonZeroBoxel(gd, px->ldat.id);
                        if (m==1) {
                            tList* pv = px->prev;
                            del_tList_node(&px);
                            px = pv;
                        }
                        else  {
                            i = px->ldat.id;
                            gd.gp[i] = 0;

                            tList* pv = px->prev;
                            del_tList_node(&px);
                            px = pv;

                            if (gd.gp[i+1]>20) {
                                ld.id = i+1;
                                ld.lv = (int)gd.gp[ld.id];
                                px = add_tList_node_bydata(px, ld);
                                gd.gp[ld.id] = 1;
                            }
                            if (gd.gp[i-1]>20) {
                                ld.id = i-1;
                                ld.lv = (int)gd.gp[ld.id];
                                px = add_tList_node_bydata(px, ld);
                                gd.gp[ld.id] = 1;
                            }
                            if (gd.gp[i+xs]>20) {
                                ld.id = i+xs;
                                ld.lv = (int)gd.gp[ld.id];
                                px = add_tList_node_bydata(px, ld);
                                gd.gp[ld.id] = 1;
                            }
                            if (gd.gp[i-xs]>20) {
                                ld.id = i-xs;
                                ld.lv = (int)gd.gp[ld.id];
                                px = add_tList_node_bydata(px, ld);
                                gd.gp[ld.id] = 1;
                            }
                            if (gd.gp[i+ps]>20) {
                                ld.id = i+ps;
                                ld.lv = (int)gd.gp[ld.id];
                                px = add_tList_node_bydata(px, ld);
                                gd.gp[ld.id] = 1;
                            }
                            if (gd.gp[i-ps]>20) {
                                ld.id = i-ps;
                                ld.lv = (int)gd.gp[ld.id];
                                px = add_tList_node_bydata(px, ld);
                                gd.gp[ld.id] = 1;
                            }
                        }
                    }
                    else {
                        px->ldat.lv = 16;
                    }
                }
                px = px->next;
            }
        } 

        // STEP 5
        //DEBUG_Warning("center_line: start step5");
        dd = rr;
        px = pp->next;
        while(px!=NULL) {
            if (px->ldat.lv>20) {
                dd = Min(dd, px->ldat.lv);
            }
            px = px->next;
        }
        mm = nn = 0;
        px = pp->next;
        while(px!=NULL) {
            nn++;
            if (px->ldat.lv==16) mm++;
            px = px->next;
        }

    } while (dd<rr || mm!=nn);

    vp.setup(gd.xs, gd.ys, gd.zs);
    for (i=0; i<gd.xs*gd.ys*gd.zs; i++) vp.gp[i] = (T)gd.gp[i];
    gd.free();

    return  vp;
}


template <typename T>  int  nonZeroBoxel(MSGraph<T> vp, int n) 
{
    int  m, xs, ps;

    ps = vp.xs*vp.ys;
    xs = vp.xs;

    m = 0;
    if (vp.gp[n+1]      !=0) m++;
    if (vp.gp[n-1]      !=0) m++;
    if (vp.gp[n+xs]     !=0) m++;
    if (vp.gp[n-xs]     !=0) m++;
    if (vp.gp[n+ps]     !=0) m++;
    if (vp.gp[n-ps]     !=0) m++;
    if (vp.gp[n+1+xs]   !=0) m++;
    if (vp.gp[n+1-xs]   !=0) m++;
    if (vp.gp[n-1+xs]   !=0) m++;
    if (vp.gp[n-1-xs]   !=0) m++;
    if (vp.gp[n+1+ps]   !=0) m++;
    if (vp.gp[n+1-ps]   !=0) m++;
    if (vp.gp[n-1+ps]   !=0) m++;
    if (vp.gp[n-1-ps]   !=0) m++;
    if (vp.gp[n+xs+ps]  !=0) m++;
    if (vp.gp[n+xs-ps]  !=0) m++;
    if (vp.gp[n-xs+ps]  !=0) m++;
    if (vp.gp[n-xs-ps]  !=0) m++;
    if (vp.gp[n+1+xs+ps]!=0) m++;
    if (vp.gp[n+1+xs-ps]!=0) m++;
    if (vp.gp[n+1-xs+ps]!=0) m++;
    if (vp.gp[n+1-xs-ps]!=0) m++;
    if (vp.gp[n-1+xs+ps]!=0) m++;
    if (vp.gp[n-1+xs-ps]!=0) m++;
    if (vp.gp[n-1-xs+ps]!=0) m++;
    if (vp.gp[n-1-xs-ps]!=0) m++;

    return m;
}


/**
template <typename T>  bool deletable(MSGraph<T> vp, int n, int c, int d)

n で示されるボクセルが削除可能かどうか判定する．

@param  vp  画像データ
@param  n   ボクセルのID
@param  c   近傍数
@param  d   次元数

@return true  削除可能
@return false 削除不能
*/
template <typename T>  bool deletable(MSGraph<T> vp, int n, int c, int d)
{
    int  i, j, k, l, m, lz, ly, mz, my;
    int  cn, mm, nh;
    int  v[27];
    bool ret;

    if (d<3) {                          // 2D-   YET NO IMPLIMENT 
        mm = 9;
        for (j=-1; j<=1; j++) {
            ly = (j+1)*3;
            my = n + j*vp.xs;
            for (i=-1; i<=1; i++) {
                l = ly + (i+1);
                m = my + i;
                if (vp.gp[m]!=0) v[l] = 1;
                else             v[l] = 0;
            }
        }
        DEBUG_MODE PRINT_MESG("DELETABLE: 2D mode is not supported.\n");
        return false;
    }
    else {                              // for 3D 
        mm = 27;
        for (k=-1; k<=1; k++) {
            lz = (k+1)*9;
            mz = n + k*vp.xs*vp.ys;
            for (j=-1; j<=1; j++) {
                ly = lz + (j+1)*3;
                my = mz + j*vp.xs;
                for (i=-1; i<=1; i++) {
                    l = ly + (i+1);
                    m = my + i;
                    if (vp.gp[m]>0) v[l] = 1;
                    else            v[l] = 0;
                }
            }
        }
    }

    cn = connectNumber(v, c, d);
    if (cn==-1 || cn!=1) return false;

    if (c==26 || c==8) {
        v[c/2] = 1 - v[c/2];
        for(i=0; i<mm; i++) v[i] = 1 - v[i];
    }

    nh = v[10] + v[12] + v[14] + v[16] + v[4] + v[22];
    if    (nh<=3) ret = true;
    else if (nh==4) ret = deletable_s(v);
    else if (nh==5) ret = deletable_4(v);
    else if (nh==6) ret = deletable_5(v);

    return  ret;
}



}       // namespace


#endif
 
