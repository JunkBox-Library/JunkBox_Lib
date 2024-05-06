#ifndef  __JBXL_CPP_MEMORY_SIMPLE_GRAPH_H_
#define  __JBXL_CPP_MEMORY_SIMPLE_GRAPH_H_

/**
@brief    グラフィックデータ定義用ヘッダ for C++
@file     Graph.h
@author   Fumi.Iseki (C)
*/

#include "Gdata.h"


//
namespace jbxl {


void  rotate_point      (int& x, int& y, double sxc, double syc, double dxc, double dyc, double cst, double snt);
void  rotate_point_angle(int& x, int& y, double sxc, double syc, double dxc, double dyc, double th);

/*
template <typename T> RBound<int>     out_around_MSGraph(MSGraph<T> vp, int x, int y, int mode=8);
template <typename T> Vector<double>  dgree_circle_MSGraph(MSGraph<T> vp);
template <typename T> double          count_around_MSGraph(MSGraph<T> vp);
template <typename T> int             count_area_MSGraph(MSGraph<T> xp);
template <typename T> int             count_object_MSGraph(MSGraph<T> xp, int mn, int mx);
template <typename T> RBound<int>     get_boundary_MSGraph(MSGraph<T> vp, int mn, int mx);

template <typename T, typename R> void  cat_MSGraph (MSGraph<R>  src, MSGraph<T>& dst);
template <typename T, typename R> void  cat_MSGraph (MSGraph<R>* src, MSGraph<T>* dst);
template <typename T, typename R> void  copy_MSGraph(MSGraph<R>  src, MSGraph<T>& dst);
template <typename T, typename R> void  copy_MSGraph(MSGraph<R>* src, MSGraph<T>* dst);
template <typename T> MSGraph<T>        dup_MSGraph(MSGraph<T>  src);
template <typename T> MSGraph<T>*       dup_MSGraph(MSGraph<T>* src);

template <typename T> void              ToPola(Vector<T> nv, double& cst, double& snt, double& csf, double& snf, double pcsf=0.0, double psnf=1.0)
template <typename T, typename R> void  Local2World(MSGraph<T> gd, MSGraph<T> vp, Vector<R> oq, Vector<R> op, Vector<R> ex);

template <typename T> void  MSGraph_chngColor(MSGraph<T> vp, int f, int t);
template <typename T> int   MSGraph_Paint    (MSGraph<T> vp, int x, int y, int mn, int mx, int cc, int mode=8);
template <typename T> int   MSGraph_Paint3D  (MSGraph<T> vp, int x, int y, int z, int mn, int mx, int cc, int mode=8);
template <typename T> void  MSGraph_Line     (MSGraph<T> vp, int x1, int y1, int x2, int y2, int cc);
template <typename T> void  MSGraph_Line3D   (MSGraph<T> vp, int x1, int y1, int z1, int x2, int y2, int z2, int cc);
template <typename T> void  MSGraph_Triangle (MSGraph<T> vp, int x1, int y1, int x2, int y2, int x3, int y3, int cc, int mode=OFF);
template <typename T> void  MSGraph_Box      (MSGraph<T> vp, int x1, int y1, int x2, int y2, int cc, int mode=OFF);
template <typename T> void  MSGraph_Circle   (MSGraph<T> vp, int x, int y, int r, int cc, int mode=OFF);
template <typename T> void  MSGraph_Circle3D (MSGraph<T> vp, Vector<> ox, Vector<> ex, int rr, int cc, int mode=OFF);
template <typename T> void  MSGraph_Pool     (MSGraph<T> vp, Vector<> a, Vector<> b, int rr, int cc);
template <typename T> void  MSGraph_Torus    (MSGraph<T> vp, Vector<> ox, Vector<> ex, int rr, int ra, int cc);
template <typename T> void  MSGraph_Sphere   (MSGraph<T> vp, Vector<> a, int r, int cc, int mode=1);

template <typename T> MSGraph<T> cut_object_MSGraph(MSGraph<T> vp, int mn, int mx, int blank=BOUNDARY_BLANK, bool ecnt=false);
template <typename T> MSGraph<T> cut_object_MSGraph(MSGraph<T> vp, int mn, int mx, RBound<int> rbound, int blank=BOUNDARY_BLANK, bool ecnt=false);
template <typename T> MSGraph<T> cut_object_MSGraph(MSGraph<T> vp, RBound<int> rbound, bool ecnt=false);

template <typename T> MSGraph<T> zoom_MSGraph  (MSGraph<T> vp, double zm, int mode=ON);
template <typename T> MSGraph<T> reduce_MSGraph(MSGraph<T> vp, double rc, int mode=ON)
template <typename T> MSGraph<T> rotate_MSGraph(MSGraph<T> vp, int xs, int ys, double cst, double snt, int mode=ON);
template <typename T> MSGraph<T> rotate_MSGraph(MSGraph<T> vp, double th, int mode=ON);

template <typename T> MSGraph<T>  x_reverse_MSGraph(MSGraph<T> vp);
template <typename T> MSGraph<T>  grab_MSGraph(MSGraph<T>  vp, int x1, int y1, int x2, int y2);
template <typename T> MSGraph<T>* grab_MSGraph(MSGraph<T>* vp, int x1, int y1, int x2, int y2);
template <typename T> void  set_around_MSGraph(MSGraph<T> vp, int cc);

template <typename T> int         addPaint_MSGraph(MSGraph<T> xp, int x, int y, int mn, int mx, int add, int mode=8);
template <typename T> MSGraph<T>  Density_Filter(MSGraph<T> vp, int size, double rate, int mode=8, int work_color=0);
template <typename T> MSGraph<T>  Density_Mask(MSGraph<T> vp, double rate, int mode=8, int work_color=0);
template <typename T> void        delete_noise_MSGraph(MSGraph<T> vp, int size, int mode=8, int work_color=0)
*/


/**
template <typename T>  Vector<double>  dgree_circle_MSGraph(MSGraph<T> vp)

2D画像において，vp.zero以外の輝度値のオブジェクトの分散度（円形度の逆数）を計算する．@n
穴空きのオブジェクトも計算可能．独自アルゴリズム．

@param   vp        画像データ

@retval  Vector.x  外周
@retval  Vector.y  面積
@retval  Vector.z  分散度

@bug 縁にかかる画像にかんしては正確に外周を計算できない．=> 縁をvp.zeroで埋めること．
*/
template <typename T>  Vector<double>  dgree_circle_MSGraph(MSGraph<T> vp)
{
    Vector<double> vt;
    double fac = 1.113757;
//  double fac = 1.182;

/*
    MSGraph<T> pp;
    pp.set(vp.xs+2, vp.ys+2, 1);
    for (int j=0; j<vp.ys; j++) {
        for (int i=0; i<vp.xs; i++) {
            pp.gp[(j+1)*pp.xs+i+1] = vp.gp[j*vp.xs+i];
        }
    }
*/
    vt.z  = -1.0;
    vt.x  = count_around_MSGraph(vp);
    vt.y  = count_area_MSGraph(vp);
    if (vt.y!=0) vt.z = (vt.x*vt.x)/(4.*PI*vt.y)/fac;

    //pp.free();

    return vt;
}


/**
template <typename T>  Vector<double>  object_feature_MSGraph(MSGraph<T> vp, int mn, int mx)

2D画像において，mn〜mxの輝度値のオブジェクトの分散度（円形度の逆数）を計算する．@n
穴空きのオブジェクトも計算可能．独自アルゴリズム．

@param   vp  画像データ
@param   mn  輝度値の最小
@param   mx  輝度値の最大

@retval  Vector<double>.x  領域の個数
@retval  Vector<double>.y  全面積
@retval  Vector<double>.z  分散度
*/
template <typename T>  Vector<double>  object_feature_MSGraph(MSGraph<T> vp, int mn, int mx)
{
    int  cnt = 0;
    int  nx, ny;
    MSGraph<T> pp;
    Vector<double> vt;

    pp.mimicry(vp);
    for (int i=0; i<pp.xs*pp.ys*pp.zs; i++) pp.gp[i] = vp.gp[i];

    MSGraph_Paint(pp, 0, 0, pp.zero, pp.zero, mx+1, 4);

    for (int j=0; j<pp.ys; j++) {
        ny = pp.xs*j;
        for (int i=0; i<pp.xs; i++) {
            nx = ny + i;
            if (pp.gp[nx]==pp.zero) {
                MSGraph_Paint(pp, i, j, pp.zero, pp.zero, mx, 8);
            }
        }
    }

    MSGraph_Paint(pp, 0, 0, mx+1, mx+1, pp.zero, 4);

    vt = dgree_circle_MSGraph<T>(pp);

    for (int j=0; j<pp.ys; j++) {
        ny = pp.xs*j;
        for (int i=0; i<pp.xs; i++) {
            nx = ny + i;
            if (pp.gp[nx]>=mn && pp.gp[nx]<=mx) {
                 MSGraph_Paint(pp, i, j, mn, mx, pp.zero, 8);
                 cnt++;
            }
        }
    }
    pp.free();
    vt.x = (double)cnt;

    return vt;
}


/*
template <typename T>  double  count_around_MSGraph(MSGraph<T> vp)

2D画像において，vp.zero以外の輝度値のオブジェクトの周囲長を測る．@n
穴空きのオブジェクトも計算可能．独自アルゴリズム(C) 2002 

面積が 4以下では不正確になる．@n
縁にかかる画像に関しては正確に計算できない．=> 縁をvp.zeroで埋めること．

@return 周囲長
*/
template <typename T>  double  count_around_MSGraph(MSGraph<T> vp)
{
    int  i, j, m, n, cc;
    bool    sflg;
    double  cnt;
    const double qs = 0.70710678118654752440084436210;  // sqrt(2.)/2.
    MSGraph<T> xp;

    xp = zoom_MSGraph(vp, 2, OFF);
    if (xp.isNull()) return -1.0;

    cnt = 0.0;
    for (j=1; j<xp.ys-1; j+=2) {
        for (i=1; i<xp.xs-1; i+=2) {
            cc = 0;
            sflg = false;
            for (n=0; n<2; n++) { 
                for (m=0; m<2; m++) {
                    if (xp.point(i+m, j+n)!=vp.zero) {
                        cc++;
                        if (n>0) if (xp.point(i+(1-m), j)!=vp.zero) sflg = true;    // 対角チェック
                    }
                }
            }

            if (cc==1 || cc==3)  cnt += qs;
            else if (cc==2) { 
                if (sflg==false) cnt += 1;
                else             cnt += 2;
            }
        }
    }
    xp.free();

    return cnt;
}


/**
template <typename T>  int  count_area_MSGraph(MSGraph<T> xp)

面積を測る．
*/
template <typename T>  int  count_area_MSGraph(MSGraph<T> xp)
{
    int ss = 0;

    for (int i=0; i<xp.xs*xp.ys*xp.zs; i++) {
        if (xp.gp[i]!=xp.zero) ss++;
    }
    return ss;
}


template <typename T>  int  count_object_MSGraph(MSGraph<T> xp, int mn, int mx)
{
    int  cnt = 0;
    int  nx, ny;
    MSGraph<T> pp;

    pp.mimicry(xp);
    for (int i=0; i<pp.xs*pp.ys*pp.zs; i++) pp.gp[i] = xp.gp[i];

    MSGraph_Paint(pp, 0, 0, pp.zero, pp.zero, mx+1, 4);

    for (int j=0; j<pp.ys; j++) {
        ny = pp.xs*j;
        for (int i=0; i<pp.xs; i++) {
            nx = ny + i;
            if (pp.gp[nx]==pp.zero) {
                MSGraph_Paint(pp, i, j, pp.zero, pp.zero, mx, 8);
            }
        }
    }

    MSGraph_Paint(pp, 0, 0, mx+1, mx+1, pp.zero, 4);

    for (int j=0; j<pp.ys; j++) {
        ny = pp.xs*j;
        for (int i=0; i<pp.xs; i++) {
            nx = ny + i;
            if (pp.gp[nx]>=mn && pp.gp[nx]<=mx) {
                 MSGraph_Paint(pp, i, j, mn, mx, pp.zero, 8);
                 cnt++;
            }
        }
    }
    pp.free();

    return cnt;
}


/**
template <typename T>  RBound<int>  out_around_MSGraph(MSGraph<T> vp, int x, int y, int mode=8)

2Dグラフィックデータ構造体 vpの(x,y)にあるオブジェクトの周囲長を得る．

@param  vp    操作対象となる 2D グラフィックデータ構造体．
@param  x, y  情報を得たいオブジェクト内の左上縁（境界）の座標．@n
    　        または，左上方向のオブジェクト外の座標（この場合，上記の座標を探す）@n
              この座標の左横に情報を得たいオブジェクトの一部が在ってはいけない．
@param mode   モード．@b 4: 4近傍探索．@b その他:8近傍探索．

@return 境界構造体rb   
@retval rb.xmax オブジェクトの x座標の最大値．
@retval rb.xmin オブジェクトの x座標の最小値．
@retval rb.ymax オブジェクトの y座標の最大値．
@retval rb.ymin オブジェクトの y座標の最小値．
@retval rb.zmax 8近傍モード時の斜めの距離の回数．
@retval rb.zmin オブジェクトの周囲長．ただし，8近傍モードの場合，斜めの距離も1と数える．@n
                周囲長を rb.zmin + rb.zmax*{sqrt(2.)-1} で計算する場合もある．
@attention
1ドットの長さは 1と数える．
*/  
template <typename T>  RBound<int>  out_around_MSGraph(MSGraph<T> vp, int x, int y, int mode=8)
{
    int   i, j, sp, cp, w, ll, ss;
    int   xx, yy, vx, vy, ix;
    bool  eflg=false;

    int   r8[8]={-1, 1, -1, -1, 1, -1, 1, 1};
    int   r4[8]={ 0, 1, -1,  0, 0, -1, 1, 0};
    int*  cc;

    i = x;
    // オブジェクトを探す
    for (j=y; j<vp.ys-1; j++) { 
        for (i=x; i<vp.xs-1; i++) {
            if (vp.gp[i+(j)*vp.xs]!=vp.zero) {
                eflg = true;
                break;
            }
        }
        if (eflg) break;
    }
    x = i;
    y = j;
    eflg = false;

    RBound<int>  rb(x, x, y, y);
    i = y*vp.xs + x;

    sp = cp = i;
    ss = ll = 0;
    vx = 1;
    vy = 0;

    if (mode==4) {
        ix = 4;
        cc = r4;
    }
    else {
        ix = 8;
        cc = r8;
    }

    do {
        w  = abs(vx)+abs(vy);
        xx = (vx*cc[0]+vy*cc[1])/w;
        yy = (vx*cc[2]+vy*cc[3])/w;
        for (j=1; j<=ix; j++) {
            if (vp.gp[cp+yy*vp.xs+xx]!=vp.zero) {
                vx = xx;
                vy = yy;
                cp = cp + yy*vp.xs + xx;
                xx = cp%vp.xs;
                yy = (cp-xx)/vp.xs;
                rb.xmax = Max(rb.xmax, xx);
                rb.ymax = Max(rb.ymax, yy);
                rb.xmin = Min(rb.xmin, xx);
                rb.ymin = Min(rb.ymin, yy);
                break;
            }
            else {
                if(sp==cp && xx==-1 && yy==vp.zero) {
                    eflg = true;
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
    } while(!eflg);

    if (mode==4) ss = 0;
    (rb.xmax)++;
    (rb.ymax)++;
    (rb.xmin)--;
    (rb.ymin)--;
    rb.zmax = ss;
    rb.zmin = ll;
    return  rb;
}


/**
template <typename T>  RBound<int>  get_boundary_MSGraph(MSGraph<T> vp, T mn, T mx)

3Dオブジェクト境界を得る．
*/
template <typename T>  RBound<int>  get_boundary_MSGraph(MSGraph<T> vp, T mn, T mx)
{
    int  nx, ny, nz, ps;
    RBound<int> rb;

    rb.set(vp.xs, 0, vp.ys, 0, vp.zs, 0);

    ps = vp.xs*vp.ys;
    for (int k=0; k<vp.zs; k++) {
        nz = k*ps;
        for (int j=0; j<vp.ys; j++) {
            ny = j*vp.xs + nz;
            for (int i=0; i<vp.xs; i++) {
                nx = i + ny;
                if (vp.gp[nx]>=mn && vp.gp[nx]<=mx) {
                    rb.fusion((T)i, (T)j, (T)k);
                }
            }
        }
    }
    rb.cutdown(vp.rbound);

    return rb;
}


/**
template <typename T, typename R>  void  cat_MSGraph(MSGraph<R> src, MSGraph<T>& dst)

MSGraph<R>型データのバッファ部を MSGraph<T>型のバッファ部へ上書き（追加）する．@n
位置，サイズは自動調整される．

@param  src  コピー元グラフィックデータ
@param  dst  コピー先グラフィックデータ
*/
template <typename T, typename R>  void  cat_MSGraph(MSGraph<R> src, MSGraph<T>& dst)
{
    if ((void*)src.gp==(void*)dst.gp) return;

    int  i, x, y, z;
    MSGraph<T>   vp;
    RBound<int>  rb;

    rb.xmin = Min(src.rbound.xmin, dst.rbound.xmin);
    rb.ymin = Min(src.rbound.ymin, dst.rbound.ymin);
    rb.zmin = Min(src.rbound.zmin, dst.rbound.zmin);
    rb.xmax = Max(src.rbound.xmax, dst.rbound.xmax);
    rb.ymax = Max(src.rbound.ymax, dst.rbound.ymax);
    rb.zmax = Max(src.rbound.zmax, dst.rbound.zmax);

    vp.set(rb.xmax-rb.xmin+1, rb.ymax-rb.ymin+1, rb.zmax-rb.zmin+1, dst.zero, dst.base, dst.RZxy);
    if (vp.gp==NULL) return;
    vp.rbound = rb;
    vp.max = Max((T)src.max, dst.max); 
    vp.min = Min((T)src.min, dst.min); 

    for (i=0; i<dst.xs*dst.ys*dst.zs; i++) {
        if (dst.gp[i]!=dst.zero) {
            x = i%dst.xs          + dst.rbound.xmin - rb.xmin;
            y = (i/dst.xs)%dst.ys + dst.rbound.ymin - rb.ymin;
            z = i/(dst.xs*dst.ys) + dst.rbound.zmin - rb.zmin;
            vp.gp[z*vp.xs*vp.ys + y*vp.xs + x] = dst.gp[i];
        }
    }

    for (i=0; i<src.xs*src.ys*src.zs; i++) {
        if (src.gp[i]!=src.zero) {
            x = i%src.xs          + src.rbound.xmin - rb.xmin;
            y = (i/src.xs)%src.ys + src.rbound.ymin - rb.ymin;
            z = i/(src.xs*src.ys) + src.rbound.zmin - rb.zmin;
            vp.gp[z*vp.xs*vp.ys + y*vp.xs + x] = (T)src.gp[i];
        }
    }

    dst.free();
    dst = vp;
    return;
}


/**
template <typename T, typename R>  void  cat_MSGraph(MSGraph<R>* src, MSGraph<T>* dst)

MSGraph<R>型データのバッファ部を MSGraph<T>型のバッファ部へ上書き（追加）する．@n
位置，サイズは自動調整される．

@param  src  コピー元グラフィックデータ
@param  dst  コピー先グラフィックデータ
*/
template <typename T, typename R>  void  cat_MSGraph(MSGraph<R>* src, MSGraph<T>* dst)
{
    if (src==NULL || dst==NULL) return;
    if ((void*)src->gp==(void*)dst->gp) return;

    int  i, x, y, z;
    MSGraph<T>   vp;
    RBound<int>  rb;

    rb.xmin = Min(src->rbound.xmin, dst->rbound.xmin);
    rb.ymin = Min(src->rbound.ymin, dst->rbound.ymin);
    rb.zmin = Min(src->rbound.zmin, dst->rbound.zmin);
    rb.xmax = Max(src->rbound.xmax, dst->rbound.xmax);
    rb.ymax = Max(src->rbound.ymax, dst->rbound.ymax);
    rb.zmax = Max(src->rbound.zmax, dst->rbound.zmax);

    vp.set(rb.xmax-rb.xmin+1, rb.ymax-rb.ymin+1, rb.zmax-rb.zmin+1, dst->zero, dst->base, dst->RZxy);
    if (vp.gp==NULL) return;
    vp.rbound = rb;
    vp.max = Max((T)src->max, dst->max); 
    vp.min = Min((T)src->min, dst->min); 

    for (i=0; i<dst->xs*dst->ys*dst->zs; i++) {
        if (dst->gp[i]!=dst->zero) {
            x = i%dst->xs           + dst->rbound.xmin - rb.xmin;
            y = (i/dst->xs)%dst->ys + dst->rbound.ymin - rb.ymin;
            z = i/(dst->xs*dst->ys) + dst->rbound.zmin - rb.zmin;
            vp.gp[z*vp.xs*vp.ys + y*vp.xs + x] = dst->gp[i];
        }
    }

    for (i=0; i<src->xs*src->ys*src->zs; i++) {
        if (src->gp[i]!=src->zero) {
            x = i%src->xs           + src->rbound.xmin - rb.xmin;
            y = (i/src->xs)%src->ys + src->rbound.ymin - rb.ymin;
            z = i/(src->xs*src->ys) + src->rbound.zmin - rb.zmin;
            vp.gp[z*vp.xs*vp.ys + y*vp.xs + x] = (T)src->gp[i];
        }
    }

    dst->free();
    *dst = vp;
    return;
}


/**
template <typename R, typename T>  void  copy_MSGraph(MSGraph<R> src, MSGraph<T>& dst)

MSGraph<R>型データのバッファ部を MSGraph<T>型のバッファ部へコピーする．@n
dstのデータ部がある場合は破棄される．

@param  src  コピー元グラフィックデータ
@param  dst  コピー先グラフィックデータ
*/
template <typename R, typename T>  void  copy_MSGraph(MSGraph<R> src, MSGraph<T>& dst)
{
    if ((void*)src.gp==(void*)dst.gp) return;

    MSGraph<T> vp;
    
    vp.getm(src.xs, src.ys, src.zs, (T)src.zero);
    if (vp.gp==NULL) {
        dst.free();
        dst.gp = NULL;
        dst.state = JBXL_GRAPH_MEMORY_ERROR;
        return;
    }

    vp.max   = (T)src.max;
    vp.min   = (T)src.min;
    vp.base  = (T)src.base;
    vp.color = src.color;
    vp.state = src.state;
    for (int i=0; i<vp.xs*vp.ys*vp.zs; i++) vp.gp[i] = (T)src.gp[i];
    
    dst.free();
    dst = vp;
    return;
}


/**
template <typename R, typename T>  void  copy_MSGraph(MSGraph<R>* src, MSGraph<T>* dst)

MSGraph<R>型データのバッファ部を MSGraph<T>型のバッファ部へコピーする．@n
dstのデータ部がある場合は破棄される．

@param  src  コピー元グラフィックデータ
@param  dst  コピー先グラフィックデータ
*/
template <typename R, typename T>  void  copy_MSGraph(MSGraph<R>* src, MSGraph<T>* dst)
{
    if (src==NULL || dst==NULL) return;
    if ((void*)src->gp==(void*)dst->gp) return;

    MSGraph<T> vp;
    
    vp.getm(src->xs, src->ys, src->zs, (T)src->zero);
    if (vp.gp==NULL) {
        dst->free();
        dst->gp = NULL;
        dst->state = JBXL_GRAPH_MEMORY_ERROR;
        return;
    }

    vp.max   = (T)src->max;
    vp.min   = (T)src->min;
    vp.base  = (T)src->base;
    vp.color = src->color;
    vp.state = src->state;
    for (int i=0; i<vp.xs*vp.ys*vp.zs; i++) vp.gp[i] = (T)src->gp[i];
    
    dst->free();
    *dst = vp;
    return;
}


/**
template <typename T>  MSGraph<T>  dup_MSGraph(MSGraph<R> src)

MSGraph<T>型データのコピーを作成する．

@param  src  コピー元グラフィックデータ
@return コピーされた MSGraph<T>型のデータ
*/
template <typename T>  MSGraph<T>  dup_MSGraph(MSGraph<T> src)
{
    MSGraph<T> vp;

    vp.init();
    if (src.isNull()) {
        vp.state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }
    
    vp = src;
    vp.getm(src.xs, src.ys, src.zs, src.zero);
    if (vp.gp==NULL) {
        vp.init();
        vp.state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (int i=0; i<vp.xs*vp.ys*vp.zs; i++) vp.gp[i] = src.gp[i];
    
    return vp;
}


/**
template <typename T>  MSGraph<T>*  dup_MSGraph(MSGraph<T>* src)

MSGraph<T>型データのコピーを作成する．

@param  src  コピー元グラフィックデータ
@return コピーされた MSGraph<T>型のデータ
*/
template <typename T>  MSGraph<T>*  dup_MSGraph(MSGraph<T>* src)
{
    if (src==NULL) return NULL;
    MSGraph<T>* vp = new MSGraph<T>();

    vp->init();
    if (src->isNull()) {
        vp->state = JBXL_GRAPH_NODATA_ERROR;
        return vp;
    }
    
    *vp = *src;
    vp->getm(src->xs, src->ys, src->zs, src->zero);
    if (vp->gp==NULL) {
        vp->init();
        vp->state = JBXL_GRAPH_MEMORY_ERROR;
        return vp;
    }

    for (int i=0; i<vp->xs*vp->ys*vp->zs; i++) vp->gp[i] = src->gp[i];
    
    return vp;
}


/**
template <typename T>  void  ToPola(Vector<T> nv, double& cst, double& snt, double& csf, double& snf, double pcsf=0.0, double psnf=1.0)

ベクトル nvの単位ベクトルを極座標 (1,θ,φ) へ変換する．

ただし，θφは sin, cosの値として返される．．ベクトル nvのノルムは計算されている必要がある．

@param[in]   nv   方向ベクトル．
@param[out]  cst  指定しない．cosθの値が入る．
@param[out]  snt  指定しない．sinθの値が入る．
@param[out]  csf  通常は cosφの値が入る．sinθ≒0 の場合は pcsfの値が代入される．
@param[out]  snf  通常は sinφの値が入る．sinθ≒0 の場合は psnfの値が代入される．
@param[in]   pcsf sinθ≒0 の場合に csfに代入される．連続計算の場合に直前の cosφの値を指定する．
@param[in]   psnf sinθ≒0 の場合に snfに代入される．連続計算の場合に直前の sinφの値を指定する．
*/
template <typename T>  void  ToPola(Vector<T> nv, double& cst, double& snt, double& csf, double& snf, double pcsf=0.0, double psnf=1.0)
{
    if (nv.n!=1.0) nv.normalize();
    if (nv.z<-1.0) nv.z = -1.0;
    if (nv.z> 1.0) nv.z =  1.0;
    cst = nv.z;
    snt = sqrt(1.0-nv.z*nv.z);

    if (snt<Sin_Tolerance) {
        cst = Sign(cst);
        snt = 0.0;
        csf = pcsf;
        snf = psnf;
    }
    else {
        csf = nv.x/snt;
        snf = nv.y/snt;
    }
/*  
    if (*snt<Sin_Tolerance) {
        *cst = Sign(*cst);
        *snt = 0.0;
    }

    double rr = sqrt(nv.x*nv.x + nv.y*nv.y);
    if (rr<=Zero_Eps) {
        *csf = 0.0;
        *snf = 1.0;
    }
    else {
        *csf = nv.x/rr;
        *snf = nv.y/rr;
    }
*/  
    return;
}


/**
template <typename T, typename R>  
void  Local2World(MSGraph<T> gd, MSGraph<T> vp, Vector<R> oq, Vector<R> op, Vector<R> ex, double pcsf=0.0, double psnf=1.0)

ローカルな観測座標系のグラフィック vpをグローバル座標系のグラフィック gd に埋めこむ．

ローカルな観測座標系　→　ローカルな極座標系　→　グローバル座標系@n
変換先の点の周りの格子点全てに値を設定する．

@param  gd    グローバル座標系のグラフィック空間．
@param  vp    観測座標系のグラフィックデータ（左上が原点）．
@param  oq    グローバル座標系から見たローカルな極座標系の原点の座標．
@param  op    観測座標系から見たローカルな極座標系の原点の座標．
@param  ex    ローカルな極座標系から見た観測座標系のx軸（ローカルな極座標系でのz軸）の向き．
@param  pcsf  sinθ≒0 の場合に *csfに代入される．連続計算の場合に直前の cosφの値を指定する．
@param  psnf  sinθ≒0 の場合に *snfに代入される．連続計算の場合に直前の sinφの値を指定する．
*/
template <typename T, typename R>  
    void  Local2World(MSGraph<T> gd, MSGraph<T> vp, Vector<R> oq, Vector<R> op, Vector<R> ex, double* pcsf=NULL, double* psnf=NULL)
{
    int x, y, z, cx, cy, cz;
    double px, py, pz, xx, yy, zz;
    double cst, snt, csf=0.0, snf=1.0;

    if (pcsf!=NULL && psnf!=NULL) {
        csf = *pcsf;
        snf = *psnf;
    }

    ToPola(ex, cst, snt, csf, snf, csf, snf);

    for (z=0; z<vp.zs; z++) {
        cz = z*vp.xs*vp.ys;
        for (y=0; y<vp.ys; y++) {
            cy = cz + y*vp.xs;
            for (x=0; x<vp.xs; x++) {
                cx = cy + x;
                if (vp.gp[cx]!=vp.zero) {
                    px = z - op.z;  // ローカルな極座標系の原点へ
                    py = op.x - x;
                    pz = op.y - y;
                    xx = px*snt*csf - py*snf - pz*cst*csf + oq.x;
                    yy = px*snt*snf + py*csf - pz*cst*snf + oq.y;
                    zz = px*cst           + pz*snt   + oq.z;
                    gd.set_vPoint(xx, yy, zz, vp.gp[cx], ON);
                }
            }
        }
    }

    if (pcsf!=NULL && psnf!=NULL) {
        *pcsf = csf;
        *psnf = snf;
    }
}


template <typename T>  void  MSGraph_changeColor(MSGraph<T> vp, int f, int t)
{
    for (int i=0; i<vp.xs*vp.ys*vp.zs; i++) {
        if (vp.gp[i]==(T)f) vp.gp[i] = (T)t;
    }
}


/**
template <typename T>  int  MSGraph_Paint(MSGraph<T> vp, int x, int y, int mn, int mx, int cc, int mode=8)

2Dグラフィックの塗りつぶし．@n
2Dグラフィック vpの点(x,y)から始めて, mn〜mx の輝度値を cc で塗りつぶす．

@param  vp    グラフィックデータ構造体．
@param  x     塗りつぶしを始める点の x座標．
@param  y     塗りつぶしを始める点の y座標．
@param  mn    塗りつぶされる輝度値の最小値
@param  mx    塗りつぶされる輝度値の最大値．
@param  cc    塗りつぶしの輝度値．
@param  mode  モード． @b 8：8近傍の塗りつぶし． @b その他：4近傍の塗りつぶし 

@return 塗りつぶした面積（ピクセル数）
*/
template <typename T>  int  MSGraph_Paint(MSGraph<T> vp, int x, int y, int mn, int mx, int cc, int mode=8)
{
    int  i, j, k, ss;

    if (cc<=mx && cc>=mn) {
        DEBUG_MODE PRINT_MESG("MSGRAPH_PAINT: WARNING: c = %d. Not be %d< c <%d\n", cc, mn, mx); 
        return 0;
    }

    if (x<0 || x>=vp.xs || y<0 || y>=vp.ys) return 0;
    if (vp.gp[y*vp.xs+x]>(T)mx || vp.gp[y*vp.xs+x]<(T)mn) return 0;

    while(x>0) {
        if (vp.gp[y*vp.xs+x-1]>(T)mx || vp.gp[y*vp.xs+x-1]<(T)mn) break;
        x--; 
    }
    k = x;

    ss = 0;
    while(k<vp.xs) {
        if (vp.gp[y*vp.xs+k]>(T)mx || vp.gp[y*vp.xs+k]<(T)mn) break;
        vp.gp[y*vp.xs+k] = (T)cc; 
        ss++;
        k++;
    }
    k--;

    for (i=x; i<=k; i++){ 
        if (y-1>=0 && y-1<vp.ys){ 
            j = (y-1)*vp.xs+i;
            if (vp.gp[j]<=(T)mx && vp.gp[j]>=(T)mn) {
                ss += MSGraph_Paint(vp, i, y-1, mn, mx, cc, mode);
            }

            if (Xabs(mode)==8) {  // 8-neighborhood 
                if (i-1>=0) {
                    if (vp.gp[j-1]<=(T)mx && vp.gp[j-1]>=(T)mn) {
                        ss += MSGraph_Paint(vp, i-1, y-1, mn, mx, cc, mode);
                    }
                }
                if (i+1<vp.xs) {
                    if (vp.gp[j+1]<=(T)mx && vp.gp[j+1]>=(T)mn) {
                        ss += MSGraph_Paint(vp, i+1, y-1, mn, mx, cc, mode);
                    }
                }
            }
        }

        if (y+1>=0 && y+1<vp.ys){
            j = (y+1)*vp.xs+i;
            if (vp.gp[j]<=(T)mx && vp.gp[j]>=(T)mn) {
                ss += MSGraph_Paint(vp, i, y+1, mn, mx, cc, mode);
            }

            if (Xabs(mode)==8) {    // 8-neighborhood 
                if (i-1>=0) {
                    if (vp.gp[j-1]<=(T)mx && vp.gp[j-1]>=(T)mn) {
                        ss += MSGraph_Paint(vp, i-1, y+1, mn, mx, cc, mode);
                    }
                }
                if (i+1<vp.xs) {
                    if (vp.gp[j+1]<=(T)mx && vp.gp[j+1]>=(T)mn) {
                        ss += MSGraph_Paint(vp, i+1, y+1, mn, mx, cc, mode);
                    }
                }
            }
        }
    }
    return ss;
}


/**
template <typename T>  int  MSGraph_Paint3D(MSGraph<T> vp, int x, int y, int z, int mn, int mx, int cc, int mode=8)

3Dグラフィックの塗りつぶし．@n
3Dグラフィック vpの点(x,y,z)から始めて, mn〜mx の輝度値を cc で塗りつぶす．

@param  vp    グラフィックデータ構造体．
@param  x     塗りつぶしを始める点の x座標．
@param  y     塗りつぶしを始める点の y座標．
@param  z     塗りつぶしを始める点の z座標．
@param  mn    塗りつぶされる輝度値の最小値
@param  mx    塗りつぶされる輝度値の最大値．
@param  cc    塗りつぶしの輝度値．
@param  mode  モード．マイナスの場合は途中経過（z）を表示．@n
              @b +-8：平面上で8近傍の塗りつぶし． @b その他：平面上で4近傍の塗りつぶし

@return 塗りつぶした体積（ボクセル数）
*/
template <typename T>  int  MSGraph_Paint3D(MSGraph<T> vp, int x, int y, int z, int mn, int mx, int cc, int mode=8)
{
    int  i, j, ps, ss;
    MSGraph<T> xp;

    ss = 0;
    ps = vp.xs*vp.ys;
    xp = vp;
    xp.zs = 1;
    xp.gp = &(vp.gp[z*ps]);

    if (xp.gp[y*xp.xs+x]>(T)mx || xp.gp[y*xp.xs+x]<(T)mn) return 0;
    ss += MSGraph_Paint(xp, x, y, mn, mx, cc, mode);
    if (mode<0) {
        DEBUG_MODE PRINT_MESG("MSGRAPH_PAINT3D: zz = %d\n", z);
    }

    for (i=0; i<ps; i++) {
        if (xp.gp[i]==(T)cc) {
            x = i%vp.xs;
            y = i/vp.xs;
            if (z-1>=0) {
                j = (z-1)*ps+y*vp.xs+x;
                if (vp.gp[j]<=(T)mx && vp.gp[j]>=(T)mn) {
                    ss += MSGraph_Paint3D(vp, x, y, z-1, mn, mx, cc, mode);
                }
            }
            if (z+1<vp.zs) {
                j = (z+1)*ps+y*vp.xs+x;
                if (vp.gp[j]<=(T)mx && vp.gp[j]>=(T)mn) {
                    ss += MSGraph_Paint3D(vp, x, y, z+1, mn, mx, cc, mode);
                }
            }
        }
    }

    return ss;
}


/**
template <typename T>  void  MSGraph_Line(MSGraph<T> vp, int x1, int y1, int x2, int y2, int cc)

2Dのラインの描画．点(x1,y1) から点(x2,y2)へ濃度 cc の線を引く．@n
座標の範囲チェックあり．

@param  vp      操作対象となるグラフィックデータ構造体．
@param  x1, y1  線の始点の座標．
@param  x2, y2  線の終点の座標．
@param  cc      線の濃度．

@par 使用例
@code
    MSGraph_Line(vp, 100, 200, 300, 300, 200);   // (100,200)から(300,300)へ濃度 200の線を引く 
@endcode
*/
template <typename T>  void  MSGraph_Line(MSGraph<T> vp, int x1, int y1, int x2, int y2, int cc)
{
    int  thresh=0, index;
    int  xunit=1;
    int  yunit=1;
    int  xdiff=x2-x1;
    int  ydiff=y2-y1;

    if (xdiff<0) {
        xdiff = -xdiff;
        xunit = -1;
    }
    if (ydiff<0) {
        ydiff = -ydiff;
        yunit = -1;
    }

    if (xdiff>ydiff) {
        for (index=0; index<xdiff+1; index++) {
            if (x1>=0 && x1<vp.xs && y1>=0 && y1<vp.ys) {
                vp.point(x1, y1) = (T)cc;
            }
            x1 = x1 + xunit;
            thresh = thresh + ydiff;
            if (thresh>=xdiff) {
                thresh = thresh - xdiff;
                y1 = y1 + yunit;
            }
        }
    }
    else {
        for (index=0; index<ydiff+1; index++) {
            if (x1>=0 && x1<vp.xs && y1>=0 && y1<vp.ys) {
                vp.point(x1, y1) = (T)cc;
            }
            y1 = y1 + yunit;
            thresh = thresh + xdiff;
            if (thresh>=ydiff) {
                thresh = thresh - ydiff;
                x1 = x1 + xunit;
            }
        }
    }
}   


/**
template <typename T>  void  MSGraph_Triangle(MSGraph<T> vp, int x1, int y1, int x2, int y2, int x3, int y3, int cc, int mode=OFF) 

2Dの三角形の描画．

点(x1,y1),(x2,y2),(x3,y3)の三点を頂点とした三角形を描く．@n
線の濃度は cc． modeが 1以上なら，三角形の内部を塗りつぶす．

@param  vp     操作対象となるグラフィックデータ構造体．
@param  x1,y1  三角形の頂点1の座標．
@param  x2,y2  三角形の頂点2の座標．
@param  x3,y3  三角形の頂点3の座標．
@param  cc     線の濃度．
@param  mode   ON なら三角形の内部の vp.zero〜cc-1 をccで塗りつぶす．

@par 使用例
@code
// (100,100),(100,200),(200,100)を頂点とした三角形(直角三角形)を描き，内部を塗りつぶす 
triangle(vp, 100, 100, 100, 200, 200, 100, 1000, 1);
@endcode

*/
template <typename T>  void  MSGraph_Triangle(MSGraph<T> vp, int x1, int y1, int x2, int y2, int x3, int y3, int cc, int mode=OFF) 
{
    MSGraph_Line(vp, x1, y1, x2, y2, cc);
    MSGraph_Line(vp, x2, y2, x3, y3, cc);
    MSGraph_Line(vp, x3, y3, x1, y1, cc);

    if (mode==ON) {
        MSGraph_Paint(vp, (x1+x2+x3)/3, (y1+y2+y3)/3, (int)vp.zero, cc-1, cc, 4); 
    }
    return;
}


/**
template <typename T> void  MSGraph_Box(MSGraph<T> vp, int x1, int y1, int x2, int y2, int cc, int mode=OFF) 

点(x1,y1)-(x2,y2)を対角とした四角形を描く．線の濃度は cc．@n
modeが 1以上なら，四角形の内部を塗りつぶす．

@param  vp              操作対象となるグラフィックデータ構造体．
@param (x1,y1),(x2,y2)  四角形の対角の座標．
@param cc               線の濃度．
@param mode             @b ON なら四角形の内部の vp.zero〜cc-1 をccで塗りつぶす．

@par 使用例
@code
box(vp, 100, 100, 200, 200, 1000, 0);  // 100,100),(200,200)を対角とした四角形を描く 
@endcode
*/
template <typename T> void  MSGraph_Box(MSGraph<T> vp, int x1, int y1, int x2, int y2, int cc, int mode=OFF) 
{
    MSGraph_Line(vp, x1, y1, x2, y1, cc);
    MSGraph_Line(vp, x2, y1, x2, y2, cc);
    MSGraph_Line(vp, x2, y2, x1, y2, cc);
    MSGraph_Line(vp, x1, y2, x1, y1, cc);

    if (mode==ON) {
        MSGraph_Paint(vp, (x1+x2)/2, (y1+y2)/2, (int)vp.zero, cc-1, cc, 4); 
    }
    return;
}


/**
template <typename T>  void  MSGraph_Line3D(MSGraph<T> vp, int x1, int y1, int z1, int x2, int y2, int z2, int cc)

3Dのラインの描画．点(x1,y1,z1)から点(x2,y2,z2)へ濃度 ccの線を引く．@n
座標の範囲チェックあり．

@param vp          操作対象となるグラフィックデータ構造体．
@param x1, y1, z1  線の始点の座標．
@param x2, y2, z2  線の終点の座標．
@param cc          線の濃度．
*/
template <typename T>  void  MSGraph_Line3D(MSGraph<T> vp, int x1, int y1, int z1, int x2, int y2, int z2, int cc)
{
    int   i;
    int   xx, yy, zz, dx, dy, dz;
    int   ux=1, uy=1, uz=1;
    int   sx=0, sy=0, sz=0;

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;

    if (dx<0) {
        dx = -dx;
        ux = -1;
    }
    if (dy<0) {
        dy = -dy;
        uy = -1;
    }
    if (dz<0) {
        dz = -dz;
        uz = -1;
    }

    xx = x1;
    yy = y1;
    zz = z1;

    if (xx>=0 && xx<vp.xs && yy>=0 && yy<vp.ys && zz>=0 && zz<vp.zs) {
        vp.point(xx, yy, zz) = (T)cc;
    }
    if (dx>=dy && dx>=dz) {
        for (i=1; i<=dx; i++) {
            xx = xx + ux;
            sy = sy + dy;
            sz = sz + dz;
            if (sy>dx) {
                sy = sy - dx;
                yy = yy + uy;
            }
            if (sz>dx) {
                sz = sz - dx;
                zz = zz + uz;
            }
            if (xx>=0 && xx<vp.xs && yy>=0 && yy<vp.ys && zz>=0 && zz<vp.zs) {
                vp.point(xx, yy, zz) = (T)cc;
            }
        }
    }
    else if (dy>dx && dy>=dz) {
        for (i=1; i<=dy; i++) {
            yy = yy + uy;
            sx = sx + dx;
            sz = sz + dz;
            if (sx>dy) {
                sx = sx - dy;
                xx = xx + ux;
            }
            if (sz>dy) {
                sz = sz - dy;
                zz = zz + uz;
            }
            if (xx>=0 && xx<vp.xs && yy>=0 && yy<vp.ys && zz>=0 && zz<vp.zs) {
                vp.point(xx, yy, zz) = (T)cc;
            }
        }
    }
    else {
        for (i=1; i<=dz; i++) {
            zz = zz + uz;
            sx = sx + dx;
            sy = sy + dy;
            if (sx>dz) {
                sx = sx - dz;
                xx = xx + ux;
            }
            if (sy>dz) {
                sy = sy - dz;
                yy = yy + uy;
            }
            if (xx>=0 && xx<vp.xs && yy>=0 && yy<vp.ys && zz>=0 && zz<vp.zs) {
                vp.point(xx, yy, zz) = (T)cc;
            }
        }
    }
}


/**
template <typename T>  void  MSGraph_Circle(MSGraph<T> vp, int x, int y, int r, int cc, int mode=OFF)

2Dの円の描画．点(x,y)を中心に半径rで濃度 ccの円を書く．

@param  vp    操作対象となるグラフィックデータ構造体．
@param  x, y  円の中心の座標．
@param  r     円の半径．
@param  cc    線の濃度．
@param  mode  @b ON なら円の内部の vp.zero〜cc-1 をccで塗りつぶす．
*/
template <typename T>  void  MSGraph_Circle(MSGraph<T> vp, int x, int y, int r, int cc, int mode=OFF)
{
    double yy, dy, dt;
    int i, nn, cx;
    int ix, iy, ux=1;
    int *px, *py;

    if (r<=0) return;

    px = (int*)malloc(sizeof(int)*(r+1));
    py = (int*)malloc(sizeof(int)*(r+1));
    if (px==NULL || py==NULL) {
        if (px!=NULL) free(px);
        if (py!=NULL) free(py);
        return;
    }
    memset(px, 0, sizeof(int)*(r+1));
    memset(py, 0, sizeof(int)*(r+1));

    ix = 0;
    iy = r;
    yy = (double)r;
    nn = 0;
    px[0] = ix;
    py[0] = iy;

    cx = (y+iy)*vp.xs + (x+ix);
    if (x+ix>=0 && x+ix<vp.xs && y+iy>=0 && y+iy<vp.ys) vp.gp[cx] = (T)cc;
    while(iy>=ix) {
        ix = ix + ux;
        dt = -ux/yy;
        dy = ix*dt;
        yy = yy + dy;
        iy = (int)yy;

        if (x+ix>=0 && x+ix<vp.xs && y+iy>=0 && y+iy<vp.ys) vp.point(x+ix, y+iy) = (T)cc;
        nn++;
        px[nn] = ix;
        py[nn] = iy;
    }

    for (i=0; i<=nn; i++) {
        ix = py[nn-i];
        iy = px[nn-i];
        if (x+ix>=0 && x+ix<vp.xs && y+iy>=0 && y+iy<vp.ys) vp.point(x+ix, y+iy) = (T)cc;
    }

    for (i=0; i<=nn; i++) {
        ix =  py[i];
        iy = -px[i];
        if (x+ix>=0 && x+ix<vp.xs && y+iy>=0 && y+iy<vp.ys) vp.point(x+ix, y+iy) = (T)cc;
    }

    for (i=0; i<=nn; i++) {
        ix =  px[nn-i];
        iy = -py[nn-i];
        if (x+ix>=0 && x+ix<vp.xs && y+iy>=0 && y+iy<vp.ys) vp.point(x+ix, y+iy) = (T)cc;
    }

    for (i=0; i<=nn; i++) {
        ix = -px[i];
        iy = -py[i];
        if (x+ix>=0 && x+ix<vp.xs && y+iy>=0 && y+iy<vp.ys) vp.point(x+ix, y+iy) = (T)cc;
    }

    for (i=0; i<=nn; i++) {
        ix = -py[nn-i];
        iy = -px[nn-i];
        if (x+ix>=0 && x+ix<vp.xs && y+iy>=0 && y+iy<vp.ys) vp.point(x+ix, y+iy) = (T)cc;
    }

    for (i=0; i<=nn; i++) {
        ix = -py[i];
        iy =  px[i];
        if (x+ix>=0 && x+ix<vp.xs && y+iy>=0 && y+iy<vp.ys) vp.point(x+ix, y+iy) = (T)cc;
    }

    for (i=0; i<=nn; i++) {
        ix = -px[nn-i];
        iy =  py[nn-i];
        if (x+ix>=0 && x+ix<vp.xs && y+iy>=0 && y+iy<vp.ys) vp.point(x+ix, y+iy) = (T)cc;
    }

    if (mode==ON) MSGraph_Paint(vp, x, y, (int)vp.zero, cc-1, cc, 4);

    free(px);
    free(py);
}


/**
template <typename T>
void  MSGraph_Circle3D(MSGraph<T> vp, Vector<> ox, Vector<> ex, int rr, int cc, int mode=OFF)

3D円の描画．
 
@param vp    操作対象となるグラフィックデータ構造体．
@param ox    円の中心の座標ベクトル．
@param ex    円の中心の法線ベクトル．
@param rr    円の半径．
@param cc    線の濃度．
@param mode  @b ON なら円の内部の vp.zero〜cc-1 をccで塗りつぶす．
*/
template <typename T>  void  MSGraph_Circle3D(MSGraph<T> vp, Vector<> ox, Vector<> ex, int rr, int cc, int mode=OFF)
{
    MSGraph<T>  xp(2*rr+3, 2*rr+3);
    if (xp.gp==NULL) return;

    Vector<> oz((xp.xs-1)/2., (xp.ys-1)/2., 0.0);

    MSGraph_Circle(xp, rr+1, rr+1, rr, cc, mode);
    Local2World(vp, xp, ox, oz, ex);
    xp.free();
}



/**
template <typename T>  void  MSGraph_Pool(MSGraph<T> vp, Vector<> a, Vector<> b, int rr, int cc)

3D円柱の描画．中身はccで塗りつぶされる．

@param vp  操作対象となるグラフィックデータ構造体．
@param a   円柱上面の円の中心の座標ベクトル．
@param b   円柱底面の円の中心の座標ベクトル．
@param rr  円柱の半径．
@param cc  線と塗りつぶしの濃度．
*/
template <typename T>  void  MSGraph_Pool(MSGraph<T> vp, Vector<> a, Vector<> b, int rr, int cc)
{
    int  i, cz;
    Vector<>   ox, oz;
    MSGraph<T> xp, pp;

    ox = a - b;
    ox.norm();
    xp.set(2*rr+3, 2*rr+3, (int)(ox.n + 0.5));
    if (xp.gp==NULL) return;

    pp = xp;
    for (i=0; i<xp.zs; i++) {
        cz = i*xp.xs*xp.ys;
        pp.gp = &(xp.gp[cz]);
        MSGraph_Circle(pp, rr+1, rr+1, rr, cc, ON);
    }
    
    oz.set((xp.xs-1)/2.0, (xp.ys-1)/2.0, (xp.zs-1)/2.0);
    Local2World(vp, xp, (a+b)/2.0, oz, ox);
    xp.free();
    return;
}


/**
template <typename T>  
void  MSGraph_Torus(MSGraph<T> vp, Vector<> ox, Vector<> ex, int rr, int ra, int cc)

3Dトーラスの描画．中身はccで塗りつぶされる．

@param vp  操作対象となるグラフィックデータ構造体．
@param ox  トーラスの中心の座標ベクトル．
@param ex  トーラスの中心の法線ベクトル．
@param rr  トーラスの半径(トーラスの中心から断面の円の中心まで)．
@param ra  トーラスの断面の円の半径
@param cc  線と塗りつぶしの濃度．
*/
template <typename T>  
    void  MSGraph_Torus(MSGraph<T> vp, Vector<> ox, Vector<> ex, int rr, int ra, int cc)
{
    int  i, nn;
    double  dt, th, xx, yy, zz, sn, cs;
    MSGraph<T> xp;
    Vector<>   ve, vo, vz;

    xp.set(2*(rr+ra)+3, 2*(rr+ra)+3, 2*ra+3);
    if (xp.gp==NULL) return;
    nn = (int)(2.0*PI*(rr+ra)*2 + 0.5);
    dt = 2.0*PI/nn;

    zz = (xp.zs-1)/2.0;
    for (i=0; i<nn; i++) {
        th = dt*i;
        sn = sin(th);
        cs = cos(th);
        xx = (xp.xs-1)/2.0 + rr*cs;
        yy = (xp.ys-1)/2.0 - rr*sn;
        vo.set(xx, yy, zz);
        ve.set(sn, cs, 0.0);
        MSGraph_Circle3D(xp, vo, ve, ra, cc, ON);
    }
    vz.set((xp.xs-1)/2., (xp.ys-1)/2., (xp.zs-1)/2.);

    Local2World(vp, xp, ox, vz, ex);
    xp.free();

    return;
}


/**
template <typename T>  void  MSGraph_Sphere(MSGraph<T> vp, Vector<> a, int r, int cc, int mode=1)

球の描画．境界チェックあり．

@param  vp    操作対象となるグラフィックデータ構造体．
@param  a     球の中心の座標ベクトル．
@param  r     球の半径．
@param  cc    線と塗りつぶしの濃度(mode=1のとき)
@param  mode  モード．@n
              @b  1: 円を重ねて球を作る．中身はccで塗りつぶされる．それぞれの円の中心が，領域内にないと塗りつぶしに失敗する．@n
              @b -1: 極座標で球を作る．vpとの境界に壁を作る．set_vPoint()を使用． @n
              @b それ以外: 極座標で球を作る．set_vPoint()を使用．
*/
template <typename T>  void  MSGraph_Sphere(MSGraph<T> vp, Vector<> a, int r, int cc, int mode=1)
{
    int i, j, k, rx, nn, s=1;   // s: 壁を作る場合の境界からの距離
    double th, fi, cs, sn, cf, sf, dt;
    double xx, yy, zz;
    MSGraph<T> xp;
 
    if (mode==1) {
        xp = vp;
        for (k=(int)(a.z-r+0.5); k<=(int)(a.z+r+0.5); k++) {
            if (k>=0 && k<vp.zs) {
                xp.gp = &vp.gp[k*vp.xs*vp.ys];
                rx = (int)(sqrt(r*r-(a.z-k)*(a.z-k))+0.5);
                MSGraph_Circle(xp, (int)(a.x+0.5), (int)(a.y+0.5), rx, cc, ON);
            }
        }   
    }
    else {
        nn = (int)(2*PI*r + 0.5)*2;
        dt = PI/nn;
        for (i=0; i<=nn; i++) {
            th = dt*i;
            sn = sin(th);
            cs = cos(th);
            zz = r*cs + a.z;
            if (mode==-1) {
                if (zz<s)        zz = s;
                if (zz>vp.zs-s-1) zz = vp.zs - s - 1;
            }
            for (j=0; j<=2*nn; j++) {
                fi = dt*j;
                cf = cos(fi);
                sf = sin(fi);
                xx = r*sn*cf + a.x;
                yy = r*sn*sf + a.y;
                if (mode==-1) {
                    if (xx<s)        xx = s;
                    if (yy<s)        yy = s;
                    if (xx>vp.xs-s-1) xx = vp.xs - s - 1;
                    if (yy>vp.ys-s-1) yy = vp.ys - s - 1;
                }
                vp.set_vPoint(xx, yy, zz, (T)cc, ON);
            }
        }
    }
    return;
}


/**
template <typename T>  MSGraph<T>  cut_object_MSGraph(MSGraph<T> vp, int mn, int mx, int blank=BOUNDARY_BLANK, bool ecnt=false)

グラフィックデータから mn〜mx の輝度値を持つ部分を抜き出す．

@param  vp     操作対象となるグラフィックデータ構造体．
@param  mn     抜き出す画像の輝度値の最小値．
@param  mx     抜き出す画像の輝度値の最大値．
@param  blank  境界の余白．あそび．
@param  ecnt   カウンタを使用するかどうか．デフォルトは false
 
@return 抜き出されたグラフィックデータ．rboundメンバに境界情報が入る．
*/
template <typename T>  MSGraph<T>  cut_object_MSGraph(MSGraph<T> vp, int mn, int mx, int blank=BOUNDARY_BLANK, bool ecnt=false)
{
    int  i, j, k, n;
    int  cx, cy, cz, cp;
    int  ax, ay, az, ap;
    MSGraph<T> xp;  // = new MSGraph<T>();

    xp.init();
    RBound<int> rb(vp.xs-1, 0, vp.ys-1, 0, vp.zs-1, 0);
    RBound<int> rx(0, vp.xs-1, 0, vp.ys-1, 0, vp.zs-1);
    CVCounter* counter = NULL;

    if (ecnt) counter = GetUsableGlobalCounter();
    if (counter!=NULL) {
        if (counter->isCanceled()) {
            xp.state = JBXL_GRAPH_CANCEL;
            return xp;
        }
        counter->SetMax(vp.zs*2);
    }

    ap = vp.xs*vp.ys;
    n  = 0;
    for (k=0; k<vp.zs; k++) {
        az = ap*k;
        for (j=0; j<vp.ys; j++) {
            ay = az + vp.xs*j;
            for (i=0; i<vp.xs; i++) {
                ax = ay + i;
                if (vp.gp[ax]>=(T)mn && vp.gp[ax]<=(T)mx) {
                    n++;
                    rb.fusion(i, j, k);
                }
            }
        }
    
        // Counter
        if (counter!=NULL) {    
            counter->StepIt();
            if (counter->isCanceled()) {
                xp.state = JBXL_GRAPH_CANCEL;
                return xp;
            }
        }
    }

    if (n==0) {
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }

    if (blank>0) rb.enlarge(blank);
    rb.commonarea(rx);

    xp.set(rb.xmax-rb.xmin+1, rb.ymax-rb.ymin+1, rb.zmax-rb.zmin+1, vp.zero, vp.base, vp.RZxy);
    if (xp.isNull()) {
        xp.state = JBXL_GRAPH_MEMORY_ERROR;
        return xp;
    }
    xp.rbound = rb;
    xp.min    = (T)mx;
    xp.max    = (T)mn;
    xp.color  = vp.color;

    cp = xp.xs*xp.ys;
    for (k=0; k<xp.zs; k++) {
        cz = cp*k;
        az = ap*(k+rb.zmin);
        for (j=0; j<xp.ys; j++) {
            cy = cz + xp.xs*j;
            ay = az + vp.xs*(j+rb.ymin);
            for (i=0; i<xp.xs; i++) {
                cx = cy + i;
                ax = ay + (i+rb.xmin);
                if (vp.gp[ax]>=(T)mn && vp.gp[ax]<=(T)mx) {
                    xp.gp[cx] = vp.gp[ax];
                    xp.max = Max(xp.max, xp.gp[cx]);
                    xp.min = Min(xp.min, xp.gp[cx]);
                }
            }
        }

        // Counter
        if (counter!=NULL) {
            counter->StepIt();
            if (counter->isCanceled()) {
                xp.state = JBXL_GRAPH_CANCEL;
                return xp;
            }
        }
    }

    xp.rbound.cutdown(vp.rbound);

    return xp;
}


/**
template <typename T>  MSGraph<T>  cut_object_MSGraph(MSGraph<T> vp, int mn, int mx, RBound<int> rbound, int blank=BOUNDARY_BLANK, bool ecnt=false)

グラフィックデータから rboundの範囲で mn〜mx の輝度値を持つ部分を抜き出す．

@param  vp      操作対象となるグラフィックデータ構造体．
@param  mn      抜き出す画像の輝度値の最小値．
@param  mx      抜き出す画像の輝度値の最大値．
@param  rbound  切り抜き対象の範囲
@param  blank   境界の余白．余裕．
@param  ecnt    カウンタを使用するかどうか．デフォルトは false
 
@return 抜き出されたグラフィックデータ．rboundメンバに境界情報が入る．
*/
template <typename T>  MSGraph<T>  cut_object_MSGraph(MSGraph<T> vp, int mn, int mx, RBound<int> rbound, int blank=BOUNDARY_BLANK, bool ecnt=false)
{
    int  i, j, k, n;
    int  cx, cy, cz, cp;
    int  ax, ay, az, ap;
    MSGraph<T> xp;  // = new MSGraph<T>();

    xp.init();
    RBound<int> rb(vp.xs-1, 0, vp.ys-1, 0, vp.zs-1, 0);
    RBound<int> rx(0, vp.xs-1, 0, vp.ys-1, 0, vp.zs-1);
    CVCounter* counter = NULL;

    if (ecnt) counter = GetUsableGlobalCounter();
    if (counter!=NULL) {
        if (counter->isCanceled()) {
            xp.state = JBXL_GRAPH_CANCEL;
            return xp;
        }
        counter->SetMax(vp.zs*2);
    }

    rbound.commonarea(rx);
    ap = vp.xs*vp.ys;
    n  = 0;
    for (k=rbound.zmin; k<=rbound.zmax; k++) {
        az = ap*k;
        for (j=rbound.ymin; j<=rbound.ymax; j++) {
            ay = az + vp.xs*j;
            for (i=rbound.xmin; i<=rbound.xmax; i++) {
                ax = ay + i;
                if (vp.gp[ax]>=(T)mn && vp.gp[ax]<=(T)mx) {
                    n++;
                    rb.fusion(i, j, k);
                }
            }
        }

        // Counter
        if (counter!=NULL) {    
            counter->StepIt();
            if (counter->isCanceled()) {
                xp.state = JBXL_GRAPH_CANCEL;
                return xp;
            }
        }
    }

    if (n==0) {
        xp.state = JBXL_GRAPH_NODATA_ERROR;
        return xp;
    }

    if (blank>0) rb.enlarge(blank);
    rb.commonarea(rbound);

    xp.set(rb.xmax-rb.xmin+1, rb.ymax-rb.ymin+1, rb.zmax-rb.zmin+1, vp.zero, vp.base, vp.RZxy);
    if (xp.isNull()) {
        xp.state = JBXL_GRAPH_MEMORY_ERROR;
        return xp;
    }
    xp.rbound = rb;
    xp.min    = (T)mx;
    xp.max    = (T)mn;
    xp.color  = vp.color;

    cp = xp.xs*xp.ys;
    for (k=0; k<xp.zs; k++) {
        cz = cp*k;
        az = ap*(k+rb.zmin);
        for (j=0; j<xp.ys; j++) {
            cy = cz + xp.xs*j;
            ay = az + vp.xs*(j+rb.ymin);
            for (i=0; i<xp.xs; i++) {
                cx = cy + i;
                ax = ay + i + rb.xmin;
                if (vp.gp[ax]>=(T)mn && vp.gp[ax]<=(T)mx) {
                    xp.gp[cx] = vp.gp[ax];
                    xp.max = Max(xp.max, xp.gp[cx]);
                    xp.min = Min(xp.min, xp.gp[cx]);
                }
            }
        }

        // Counter
        if (counter!=NULL) {
            counter->StepIt();
            if (counter->isCanceled()) {
                xp.state = JBXL_GRAPH_CANCEL;
                return xp;
            }
        }
    }
    xp.rbound.cutdown(vp.rbound);

    return xp;
}


/**
template <typename T>  MSGraph<T>  cut_object_MSGraph(MSGraph<T> vp, RBound<int> rb, bool ecnt=false)

グラフィックデータから rbound の範囲を抜き出す．

@param vp    操作対象となるグラフィックデータ構造体．
@param rb    切り抜き対象の範囲
@param ecnt  カウンタを使用するかどうか．デフォルトは false
 
@return 抜き出されたグラフィックデータ．rboundメンバに境界情報が入る．
*/
template <typename T>  MSGraph<T>  cut_object_MSGraph(MSGraph<T> vp, RBound<int> rb, bool ecnt=false)
{
    int  i, j, k;
    int  cx, cy, cz, cp;
    int  ax, ay, az, ap;
    MSGraph<T> xp;  // = new MSGraph<T>();

    xp.init();
    RBound<int> rx(0, vp.xs-1, 0, vp.ys-1, 0, vp.zs-1);
    CVCounter* counter = NULL;

    if (ecnt) counter = GetUsableGlobalCounter();
    if (counter!=NULL) {
        if (counter->isCanceled()) {
            xp.state = JBXL_GRAPH_CANCEL;
            return xp;
        }
        counter->SetMax(vp.zs*2);
    }

    rb.commonarea(rx);
    xp.set(rb.xmax-rb.xmin+1, rb.ymax-rb.ymin+1, rb.zmax-rb.zmin+1, vp.zero, vp.base, vp.RZxy);
    if (xp.isNull()) {
        xp.state = JBXL_GRAPH_MEMORY_ERROR;
        return xp;
    }
    xp.rbound = rb;
    xp.color  = vp.color;

    ap = vp.xs*vp.ys;
    cp = xp.xs*xp.ys;

    for (k=0; k<xp.zs; k++) {
        cz = cp*k;
        az = ap*(k+rb.zmin);
        for (j=0; j<xp.ys; j++) {
            cy = cz + xp.xs*j;
            ay = az + vp.xs*(j+rb.ymin);
            for (i=0; i<xp.xs; i++) {
                cx = cy + i;
                ax = ay + i + rb.xmin;
                xp.gp[cx] = vp.gp[ax];
                
                if (cx==0) xp.max = xp.min = xp.gp[cx];
                else {
                    xp.max = Max(xp.max, xp.gp[cx]);
                    xp.min = Min(xp.min, xp.gp[cx]);
                }
            }
        }

        // Counter
        if (counter!=NULL) {
            counter->StepIt();
            if (counter->isCanceled()) {
                xp.state = JBXL_GRAPH_CANCEL;
                return xp;
            }
        }
    }
    xp.rbound.cutdown(vp.rbound);

    return xp;
}


/**
template <typename T>  MSGraph<T>  zoom_MSGraph(MSGraph<T> vp, double zm, int mode=ON)

2Dグラフィックデータ拡大する．

@param  vp   -- 変換する Vector型単純グラフィックデータ．
@param  zm   -- 倍率．
@param  mode -- モード．@b ON: 線形補間．@b その他: 単純拡大

@return 拡大したグラフィックデータ．
*/
template <typename T>  MSGraph<T>  zoom_MSGraph(MSGraph<T> vp, double zm, int mode=ON)
{
    int  xss, yss, ps, pz, py;
    MSGraph<T> vx;

    vx.init();

    if (vp.gp==NULL) {
        vx.state = JBXL_GRAPH_NODATA_ERROR;
        return vx;
    }
    if (zm==0.0) {
        vx.state = JBXL_GRAPH_IVDARG_ERROR;
        return vx;
    }
    else if (zm<0.0) zm = -zm;

    xss = (int)(vp.xs*zm) + 1;
    yss = (int)(vp.ys*zm) + 1;

    vx.set(xss, yss, vp.zs);
    if (vx.isNull()) return vx;
    vx.color = vp.color;

    ps = xss*yss;

    if (mode==ON) {
        int    ii, jj, kk, ll;
        double xx, yy, al, bt;

        for(int k=0; k<vx.zs; k++) {
            pz = k*ps;
            for(int j=0; j<yss; j++) {
                py = pz + j*xss;
                for(int i=0; i<xss; i++) {
                    xx = i/zm;
                    yy = j/zm;
                    ii = (int)xx;
                    jj = (int)yy;
                    if (ii>=vp.xs) ii = vp.xs - 1;
                    if (jj>=vp.ys) jj = vp.ys - 1;

                    kk = ii + 1;
                    ll = jj + 1;
                    if (kk>=vp.xs) kk = vp.xs - 1;
                    if (ll>=vp.ys) ll = vp.ys - 1;

                    if (xx>=0.) al = xx - ii;
                    else        al = 0.;
                    if (yy>=0.) bt = yy - jj;
                    else        bt = 0.;

                    vx.gp[py+i] = (T)((1.-al)*(1.-bt)*vp.point(ii, jj, k) + al*(1.-bt)*vp.point(kk, jj, k) 
                                                        + (1.-al)*bt*vp.point(ii, ll, k) + al*bt*vp.point(kk, ll, k) + 0.5);
                }
            }
        }
    }

    else {
        int ii, jj;

        for(int k=0; k<vx.zs; k++) {
            pz = k*ps;
            for(int j=0; j<yss; j++) {
                py = pz + j*xss;
                for(int i=0; i<xss; i++) {
                    ii = (int)(i/zm);
                    jj = (int)(j/zm);
                    vx.gp[py+i] = vp.point(ii, jj, k);
                }
            }
        }
    }

    return vx;
}


/**
*/
template <typename T>  MSGraph<T>  reduce_MSGraph(MSGraph<T> vp, double rc, int mode=ON)
{
    int  xss, yss, ps, pz, py;
    MSGraph<T> vx;

    vx.init();

    if (vp.gp==NULL) {
        vx.state = JBXL_GRAPH_NODATA_ERROR;
        return vx;
    }
    if (rc==0.0) {
        vx.state = JBXL_GRAPH_IVDARG_ERROR;
        return vx;
    }
    else if (rc<0.0) rc = -rc;

    xss = (int)(vp.xs/rc);
    yss = (int)(vp.ys/rc);

    vx.set(xss, yss, vp.zs);
    if (vx.isNull()) return vx;
    vx.color = vp.color;

    ps = xss*yss;

    if (mode==ON) {
        int ii, jj, kk, ll, ss, nn;

        for(int z=0; z<vx.zs; z++) {
            pz = z*ps;
            for(int j=0; j<yss; j++) {
                py = pz + j*xss;
                for(int i=0; i<xss; i++) {
                    ii = (int)(i*rc);
                    jj = (int)(j*rc);
                    if (ii>=vp.xs) ii = vp.xs - 1;
                    if (jj>=vp.ys) jj = vp.ys - 1;

                    ss = nn = 0;
                    for(int l=jj; l<jj+(int)rc; l++) {
                        ll = l;
                        if (ll>=vp.ys) ll = vp.ys - 1;
                        for(int k=ii; k<ii+(int)rc; k++) {
                            kk = k;
                            if (kk>=vp.xs) kk = vp.xs - 1;
                            ss += vp.point(kk, ll, z);
                            nn++;
                        }
                    }
                    vx.gp[py+i] = ss/nn;
                }
            }
        }
    }

    else {
        int ii, jj;

        for(int k=0; k<vx.zs; k++) {
            pz = k*ps;
            for(int j=0; j<yss; j++) {
                py = pz + j*xss;
                for(int i=0; i<xss; i++) {
                    ii = (int)(i*rc);
                    jj = (int)(j*rc);
                    if (ii>=vp.xs) ii = vp.xs - 1;
                    if (jj>=vp.ys) jj = vp.ys - 1;
                    vx.gp[py+i] = vp.point(ii, jj, k);
                }
            }
        }
    }

    return vx;
}


/**
template <typename T>  MSGraph<T>  rotate_MSGraph(MSGraph<T> vp, int xs, int ys, double cst, double snt, int mode=ON)

反時計回りに画像を回転させる
*/
template <typename T>  MSGraph<T>  rotate_MSGraph(MSGraph<T> vp, int xs, int ys, double cst, double snt, int mode=ON)
{
    MSGraph<T> vs;
    T   pt;
    int  i, j, m, n;
    int  ps, px, pz;
    double u, t, x, y, a, b;

    vs.set(xs, ys, vp.zs, vp.zero, vp.base);
    if (vs.isNull()) return vs;
    vs.color = vp.color;

    ps = vs.xs*vs.ys;

    for (int k=0; k<vs.zs; k++) {
        pz = k*ps;
        for (int jj=0; jj<vs.ys; jj++) {
            px = pz + jj*vs.xs;
            for (int ii=0; ii<vs.xs; ii++) {
                u = ii - (vs.xs-1)/2.;
                t = (vs.ys-1)/2. - jj;
                x =   u*cst + t*snt;
                y = - u*snt + t*cst;
                a = x + (vp.xs-1)/2.;
                b = (vp.ys-1)/2. - y;
                i = (int)a;
                j = (int)b;
                
                if (i<0 || i>=vp.xs || j<0 || j>=vp.ys) {
                    pt = vs.zero;
                }
                else {
                    if (mode==ON) {
                        if (a>=0.) a = a - i;
                        else       a = 0.;
                        if (b>=0.) b = b - j;
                        else       b = 0.;

                        m = i + 1;
                        n = j + 1;
                        if (m>=vp.xs) m = vp.xs - 1;
                        if (n>=vp.ys) n = vp.ys - 1;
                        pt = (T)((1.-a)*(1.-b)*vp.point(i, j, k) + (1.-a)*b*vp.point(i, n, k) 
                                                                 + a*(1.-b)*vp.point(m, j, k) + a*b*vp.point(m, n, k) + 0.5);
                    }
                    else {
                        pt = vp.point(i, j, k);
                    }
                }
                vs.gp[px+ii] = pt;
            }
        }
    }
    return vs;
}


/**
*/
template <typename T>  MSGraph<T>  rotate_MSGraph(MSGraph<T> vp, double th, int mode=ON)
{
    MSGraph<T> vs;
    double cst = cos(th);
    double snt = cos(th);
    int   xys = (int)sqrt(vp.xs*vp.xs+vp.ys*vp.ys) + 1;

    vs = rotate_MSGraph<T>(vp, xys, xys, cst, snt, mode);
    return vs;
}


/**
template <typename T>  MSGraph<T>  x_reverse_MSGraph(MSGraph<T> vp, bool ecnt=false)

グラフィックデータの x軸を反転させる．

@param  vp    操作対象となるグラフィックデータ構造体．
@param  ecnt  仮想カウンタを使用するか?

@return 反転したグラフィックデータ．
*/
template <typename T>  MSGraph<T>  x_reverse_MSGraph(MSGraph<T> vp, bool ecnt=false)
{
    int  i, j, k, cy, cz;
    MSGraph<T> wp;
    CVCounter* counter = NULL;
    
    wp.init();

    if (ecnt) counter = GetUsableGlobalCounter();
    if (counter!=NULL) {
        if (counter->isCanceled()) {
            wp.state = JBXL_GRAPH_CANCEL;
            return wp;
        }
        counter->SetMax(vp.zs);
    }

    wp.set(vp.xs, vp.ys, vp.zs, vp.zero, vp.base, vp.RZxy);
    if (wp.isNull()) return wp;
    wp.color = vp.color;

    for (k=0; k<vp.zs; k++) {
        cz = k*vp.xs*vp.ys;
        for (j=0; j<vp.ys; j++) {
            cy = cz + j*vp.xs;
            for (i=0; i<vp.xs; i++) {
                wp.gp[cy + i] = vp.gp[cy + vp.xs - 1 - i];
            }
        }

        // Counter
        if (counter!=NULL) {
            counter->StepIt();
            if (counter->isCanceled()) {
                wp.state = JBXL_GRAPH_CANCEL;
                return wp;
            }
        }
    }
    return wp;
}


/**
template <typename T>  void  set_around_MSGraph(MSGraph<T> vp, int cc=0, int size=1)

2Dグラフィックデータの縁の部分の輝度値を ccにする．

@param  vp    操作対象となるグラフィックデータ構造体．
@param  cc    データの縁に設定する輝度値．
@param  size  縁のサイズ
*/
template <typename T>  void  set_around_MSGraph(MSGraph<T> vp, int cc=0, int size=1)
{
    int   i, j, px1, px2;
/*
    for (i=0; i<vp.xs; i++){
        px1 = i;
        px2 = (vp.ys-1)*vp.xs + i;
        vp.gp[px1] = (T)cc;
        vp.gp[px2] = (T)cc;
    }

    for (j=1; j<vp.ys-1; j++){
        px1 = j*vp.xs;
        px2 = (j+1)*vp.xs - 1;
        vp.gp[px1] = (T)cc;
        vp.gp[px2] = (T)cc;
    }
*/
    for(j=0; j<vp.ys; j++) {
        px1 = j*vp.xs; 
        px2 = (j+1)*vp.xs-size;
        for(i=0; i<size; i++) {
            vp.gp[i+px1] = (T)cc; 
            vp.gp[i+px2] = (T)cc;
        }
    }

    for(j=0; j<size; j++) { 
        px1 = j*vp.xs;
        px2 = (j+vp.ys-size)*vp.xs;
        for(i=0; i<vp.xs; i++) vp.gp[i+px1] = (T)cc;
        for(i=0; i<vp.xs; i++) vp.gp[i+px2] = (T)cc;
    }

}


/**
template <typename T>  MSGraph<T>  grab_MSGraph(MSGraph<T> vp, int x1, int y1, int x2, int y2, int zs=0, int ze=0)

MSGraph<T>型データvpの (x1,y1)-(x2,y2)の矩形部分だけを取り出す．@n
(x1,y1)-(x2,y2)が元のデータの範囲を越える場合は,元のデータに合わせられる．

@param  vp               操作対象グラフィックデータ
@param  (x1,y1),(x2,y2)  矩形の対角の座標．
@param  zs, ze           z軸の範囲

@return 取り出したグラフィックデータ．bound メンバに切り出した範囲が入る．
*/
template <typename T>  MSGraph<T>  grab_MSGraph(MSGraph<T> vp, int x1, int y1, int x2, int y2, int zs=0, int ze=0)
{
    int  i, j, k;
    int  xs, xe, ys, ye;
    int  xsize, ysize, zsize;
    MSGraph<T> xp;

    xp.init();
    xs = Min(x1, x2);
    xe = Max(x1, x2);
    xs = Max(xs, 0);
    xe = Min(xe, vp.xs-1);
    ys = Min(y1, y2);
    ye = Max(y1, y2);
    ys = Max(ys, 0);
    ye = Min(ye, vp.ys-1);
    zs = Max(zs, 0);
    ze = Min(ze, vp.zs-1);

    xsize = xe - xs + 1;
    ysize = ye - ys + 1;
    zsize = ze - zs + 1;

    xp.set(xsize, ysize, zsize, vp.zero, vp.base);
    if (xp.isNull()) return xp;
    xp.color = vp.color;

    xp.max = vp.point(xs, ys, zs);
    xp.min = xp.max;

    for (k=0; k<zsize; k++) {
        for (j=0; j<ysize; j++) {
            for (i=0; i<xsize; i++) {
                T point = vp.point(xs+i, ys+j, zs+k);
                xp.point(i, j, k) = point;
                if      (point>xp.max) xp.max = point;
                else if (point<xp.min) xp.min = point; 
            }
        }
    }
    
    xp.rbound.xmin = vp.rbound.xmin + xs;
    xp.rbound.xmax = vp.rbound.xmin + xe;
    xp.rbound.ymin = vp.rbound.ymin + ys;
    xp.rbound.ymax = vp.rbound.ymin + ye;
    xp.rbound.zmin = vp.rbound.zmin + zs;
    xp.rbound.zmax = vp.rbound.zmin + ze;
    return xp;
}


/**
template <typename T>  MSGraph<T>*  grab_MSGraph(MSGraph<T>* vp, int x1, int y1, int x2, int y2, int zs=0, int ze=0)

MSGraph<T>型データvpの (x1,y1)-(x2,y2)の矩形部分だけを取り出す．@n
(x1,y1)-(x2,y2)が元のデータの範囲を越える場合は,元のデータに合わせられる．@n
輝度値の最大(max)，最小(min) も設定される．

@param  vp               操作対象グラフィックデータ
@param  (x1,y1),(x2,y2)  矩形の対角の座標．
@param  zs, ze           z軸の範囲

@return 取り出したグラフィックデータ．bound メンバに切り出した範囲が入る．
*/
template <typename T>  MSGraph<T>* grab_MSGraph(MSGraph<T>* vp, int x1, int y1, int x2, int y2, int zs=0, int ze=0)
{
    int  i, j, k;
    int  xs, xe, ys, ye;
    int  xsize, ysize, zsize;
    MSGraph<T>* xp = new MSGraph<T>();

    xp->init();
    xs = Min(x1, x2);
    xe = Max(x1, x2);
    xs = Max(xs, 0);
    xe = Min(xe, vp->xs-1);
    ys = Min(y1, y2);
    ye = Max(y1, y2);
    ys = Max(ys, 0);
    ye = Min(ye, vp->ys-1);
    zs = Max(zs, 0);
    ze = Min(ze, vp->zs-1);

    xsize = xe - xs + 1;
    ysize = ye - ys + 1;
    zsize = ze - zs + 1;

    xp->set(xsize, ysize, zsize, vp->zero, vp->base);
    xp->color = vp->color;

    xp->max = vp->point(xs, ys, zs);
    xp->min = xp->max;

    for (k=0; k<zsize; k++) {
        for (j=0; j<ysize; j++) {
            for (i=0; i<xsize; i++) {
                T point = vp->point(xs+i, ys+j, zs+k);
                xp->point(i, j, k) = point;
                if      (point>xp->max) xp->max = point;
                else if (point<xp->min) xp->min = point; 
            }
        }
    }
    
    xp->rbound.xmin = vp->rbound.xmin + xs;
    xp->rbound.xmax = vp->rbound.xmin + xe;
    xp->rbound.ymin = vp->rbound.ymin + ys;
    xp->rbound.ymax = vp->rbound.ymin + ye;
    xp->rbound.zmin = vp->rbound.zmin + zs;
    xp->rbound.zmax = vp->rbound.zmin + ze;
    return xp;
}


/**
template <typename T> int addPaint_MSGraph(MSGraph<T> xp, int x, int y, int mn, int mx, int add, int mode=8)

追加式の 2Dペイント@n

xp の (x, y)座標から，輝度値 mn〜mx の範囲の点に addを加える．

@param  xp    グラフィックデータ構造体．
@param  x     塗りつぶしを始める点の X座標．
@param  y     塗りつぶしを始める点の Y座標．
@param  mn    塗りつぶされる輝度値の最小値
@param  mx    塗りつぶされる輝度値の最大値．
@param  add   加える輝度値．マイナスでも動作するが，作業領域に注意．
@param  mode  モード．8以上：平面上で 8近傍の塗りつぶし その他：平面上で 4近傍の塗りつぶし

@attention
輝度値に，addがプラスの場合はadd以上，または addがマイナスの場合は add+輝度値がプラスになるもの
があると誤作動する．作業領域として add〜 を使用する．
*/
template <typename T> int addPaint_MSGraph(MSGraph<T> xp, int x, int y, int mn, int mx, int add, int mode=8)
{
    int  i, k, cc, cx, st, ed, num=0;

    if (x<0 || x>=xp.xs || y<0 || y>=xp.ys) return 0;

    // 左端へ
    cc = (int)xp.point(x, y);
    while(x>0) {
        cx = (int)xp.point(x-1, y);
        if (cx>mx || cx<mn) break;
        cc = cx;
        x--; 
    }
    st = k = x;

    // 右方向へ塗りつぶし
    while(k<xp.xs) {
        cx = (int)xp.point(k, y);
        if (cx>mx || cx<mn) break;
        xp.point(k, y) += add;
        cc = cx;
        num++;
        k++;
    }
    ed = k - 1;

    // 上下ライン
    for (i=st; i<=ed; i++){ 
        cc = (int)xp.point(i, y) - add;
        
        // 1ライン上へ
        if (y-1>=0 && y-1<xp.ys){ 
            cx = (int)xp.point(i, y-1);
            if (cx<=mx && cx>=mn) {
                num += addPaint_MSGraph(xp, i, y-1, mn, mx, add, mode);
            }
            if (mode>=8) {   
                if (i-1>=0) {
                    cx = (int)xp.point(i-1, y-1);
                    if (cx<=mx && cx>=mn) {
                        num += addPaint_MSGraph(xp, i-1, y-1, mn, mx, add, mode);
                    }
                }
                if (i+1<xp.xs) {
                    cx = (int)xp.point(i+1, y-1);
                    if (cx<=mx && cx>=mn) {
                        num += addPaint_MSGraph(xp, i+1, y-1, mn, mx, add, mode);
                    }
                }
            }
        }

        // 1ライン下へ
        if (y+1>=0 && y+1<xp.ys){
            cx = (int)xp.point(i, y+1);
            if (cx<=mx && cx>=mn) {
                num += addPaint_MSGraph(xp, i, y+1, mn, mx, add, mode);
            }
            if (mode>=8) {  
                if (i-1>=0) {
                    cx = (int)xp.point(i-1, y+1);
                    if (cx<=mx && cx>=mn) {
                        num += addPaint_MSGraph(xp, i-1, y+1, mn, mx, add, mode);
                    }
                }
                if (i+1<xp.xs) {
                    cx = (int)xp.point(i+1, y+1);
                    if (cx<=mx && cx>=mn) {
                        num += addPaint_MSGraph(xp, i+1, y+1, mn, mx, add, mode);
                    }
                }
            }
        }
    }
    return num;
}


/**
template <typename T> MSGraph<T>   Density_Mask(MSGraph<T> vp, double rate, int mode=8, int work_color=0)

密度マスク

区画の中で縁に接して，かつ zeroでない画素の数を数え，全体の画素の
数に対して rate 以上の比率の場合，その画素パターンを返す．@n
rateに満たない場合は zero で埋められたパターンを返す．

@param  vp          画像データ
@param  rate        密度
@param  mode        モード． @b 8：8近傍の塗りつぶし． @b その他：4近傍の塗りつぶし 
@param  work_color  作業用輝度値

@return 処理結果の画像データ
*/
template <typename T> MSGraph<T>   Density_Mask(MSGraph<T> vp, double rate, int mode=8, int work_color=0)
{
    MSGraph<T> pp;
    int  ps = vp.xs*vp.ys;

    if (work_color==0) {
        if (vp.max==0 || vp.max==vp.zero) vp.get_minmax();
        work_color = vp.max + 1;
    }
    pp.mimicry(vp);

    if ((int)(ps*rate+0.5)==ps) {
        int  eflg = OFF;
        for (int i=0; i<ps; i++) {
            if (vp.gp[i]==vp.zero) {
                pp.state = 0;
                eflg = ON;
                break;
            }
        }
        if (eflg==OFF) {
            for (int i=0; i<ps; i++) pp.gp[i] = vp.gp[i];
            pp.state = ps;
        }
        return pp;
    }

    MSGraph<T> xp;
    xp.set(vp.xs+2, vp.ys+2, 1, vp.zero, vp.base, vp.RZxy);
    xp.color = vp.color;

    for (int j=0; j<vp.ys; j++) {
        for (int i=0; i<vp.xs; i++) {
            xp.gp[(j+1)*xp.xs+i+1] = vp.gp[j*vp.xs+i];
        }
    }
    set_around_MSGraph(xp, xp.zero+1);
    MSGraph_Paint(xp, 0, 0, xp.zero+1, work_color-1, work_color, mode);

    int nn = 0;
    for (int j=0; j<vp.ys; j++) {
        for (int i=0; i<vp.xs; i++) {
            if (xp.gp[(j+1)*xp.xs+i+1]==work_color) {
                pp.gp[j*pp.xs+i] = vp.gp[j*vp.xs+i];
                nn++;
            }
        }
    }
    pp.state = nn;

    if ((int)(ps*rate+0.5)>nn) {
        pp.state = 0;
        pp.clear();
    }

    xp.free();
    return pp;
}


/**
template <typename T> MSGraph<T>   Density_Filter(MSGraph<T> vp, int size, double rate, int mode=8, int work_color=0)

密度フィルター @n
画像データの密度マスクを適用して返す．

@param  vp          画像データ
@param  size        マスクのサイズ
@param  rate        密度の閾値
@param  mode        近傍(4 or 8)
@param  work_color  作業用輝度値

@return 処理結果の画像データ
*/
template <typename T> MSGraph<T>   Density_Filter(MSGraph<T> vp, int size, double rate, int mode=8, int work_color=0)
{
    MSGraph<T> pp, xp, wp;
    int hsize = size/2;

    if (work_color==0) {
        if (vp.max==0 || vp.max==vp.zero) vp.get_minmax();
        work_color = vp.max + 1;
    }

    if (hsize==0) hsize = 1;
    size = hsize*2 + 1;
    pp.set(size, size, 1, vp.zero, vp.base);
    wp.mimicry(vp);

    for (int j=hsize; j<vp.ys-hsize; j++) {
        for (int i=hsize; i<vp.xs-hsize; i++) {

            for (int m=-hsize; m<=hsize; m++) {
                int vx = (m+j)*vp.xs + i;
                int px = (m+hsize)*pp.xs + hsize;

                for (int n=-hsize; n<=hsize; n++) {
                    pp.gp[px+n] = vp.gp[vx+n];
                }
            }

            xp = Density_Mask<T>(pp, rate, mode, work_color);

            if (xp.state!=0) {
                for (int m=-hsize; m<=hsize; m++) {
                    int wx = (m+j)*wp.xs + i;
                    int xx = (m+hsize)*xp.xs + hsize;

                    for (int n=-hsize; n<=hsize; n++) {
                        if (xp.gp[xx+n]!=vp.zero) {
                            wp.gp[wx+n] = xp.gp[xx+n];
                        }
                    }
                }
            }
            xp.free();
        }
    }

    pp.free();
    return wp;
}


/**
template <typename T> void   delete_noise_MSGraph(MSGraph<T> vp, int size, int mode=8, int work_color=0)

背景（vp.zero）に浮かぶ孤立ノイズを除去する．@n
-work_color〜-1, work_color〜 を作業領域として使用するので，この区間にデータがある場合は誤作動する．
*/
template <typename T> void   delete_noise_MSGraph(MSGraph<T> vp, int size, int mode=8, int work_color=0)
{
    int  i, j, k;
    int  num, ps ,pp;

    ps = vp.xs*vp.ys;
    if (work_color==0) {
        if (vp.max==0 || vp.max==vp.zero) vp.get_minmax();
        work_color = vp.max + 1;
    }

    for(j=0; j<vp.ys; j++) {
        pp = j*vp.xs;         
        for(i=0; i<vp.xs; i++) {                  
            if (vp.gp[i+pp]>vp.zero && vp.gp[i+pp]<work_color) {
                num = addPaint_MSGraph(vp, i, j, 1, work_color-1, work_color, mode);
                if (num<=size) {                    
                    int n = 0;
                    for (k=i+pp; k<ps; k++) {
                        if (vp.gp[k]>=work_color) {
                            vp.gp[k] = vp.zero;
                            n++;
                            if (n>=num) break;
                        }
                    }
                }
                else {
                    int n = 0;
                    for (k=i+pp; k<ps; k++) {
                        if (vp.gp[k]>=work_color) {
                            vp.gp[k] = -vp.gp[k];
                            n++;
                            if (n>=num) break;
                        }
                    }
                }
            }
        }
    }
   
    for (i=0; i<ps; i++) {
        if (vp.gp[i]<vp.zero) vp.gp[i] = - vp.gp[i] - work_color;
    }
}


}       // namespace


#endif
