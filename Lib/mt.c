
/** 
@brief   数学用ライブラリ
@file    mt.c
@author  Fumi.Iseki (C)
*/



#include "mt.h"



double   EPS = 1.0e-6;



/**
double  power(double x, double y)

xの y乗 x^yを計算する．

*/
double  power(double x, double y)
{
    if (y==0.0)      return 1.0;
    else if (x==0.0) return 0.0;
    else return exp((y)*log(x));
}



/**
double  fact(int n)

nの階乗 n! を計算する．n>0 でない場合は 1.0 を返す． 

*/
double  fact(int n)
{
    int    i;
    double ret = 1.0;

    if (n>0) for (i=1; i<=n; i++)  ret = ret*(double)i;
    return ret;
}
    


/**
double  perm(int n, int m)

パームテーション nPm を計算する．

*/
double  perm(int n, int m)
{
    int    i;
    double p;

    if (m>n || n<=0 || m<=0) return 0.;

    p = 1.0;
    for (i=n-m+1; i<=n; i++) p = p*(double)i;

    return p;
}



/**
double  comb(int n, int m)

コンビネーション nCm を計算する． 

*/
double  comb(int n, int m)
{
    int    i;
    double c, p;

    if (m>n || n<=0 || m<=0) return 0.;

    p = c = 1.0;
    for (i=n-m+1; i<=n; i++) p = p*(double)i;
    for (i=1;     i<=m; i++) c = c*(double)i;

    return p/c;
}



#define  MAX_ITRTN_NEWTON_METHOD  30    ///< ニュートン法で解を求める際の，最大繰り返し回数．

/**    
int    newton_method(PTR_DFFUNC func, PTR_DFFUNC dfunc, double* t0, double eps)

ニュートン法で func()==0 の点を求める．@n
関数の形は double func(double)

@param         func   double func(double) 型の関数へのポインタ
@param         dfunc  func()の導関数へのポインタ．
@param[in,out] t0     in: 探索開始点（xの初期値）, out: 解．
@param         eps    誤差．1に対して 1.0e-8 など．

@return        解を求めるまでの繰り返し回数．0 の場合は失敗．
@see PTR_DFFUNC, MAX_ITRTN_NEWTON_METHOD
*/
int    newton_method(PTR_DFFUNC func, PTR_DFFUNC dfunc, double* t0, double eps)
{
    double tt  = *t0;
    double ff  = (*func)(tt);

    int n = 1;
    while (n<MAX_ITRTN_NEWTON_METHOD && Xabs(ff)>eps) {
        //
        double df = (*dfunc)(tt);
        if (Xabs(df)<eps) return 0;

        tt = tt - ff/df;
        ff = (*func)(tt);
        n++;
    }

    if (n==MAX_ITRTN_NEWTON_METHOD) return 0;
    *t0 = tt;
    return n;
}

