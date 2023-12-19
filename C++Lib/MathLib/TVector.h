
#ifndef  __JBXL_CPP_T_VECTOR_H_
#define  __JBXL_CPP_T_VECTOR_H_

/**
@brief    トレランス付き（信頼精度付き）ベクトル ライブラリ ヘッダ
@file     TVector.h
@author   Fumi.Iseki (C)
*/


#include "Vector.h"


//
namespace jbxl {


#define  TVECTOR  TVector


/**
template<typename T=double> class TVector

トレランス付き 3次元ベクトルの定義
 */
template<typename T=double> class DllExport TVector : public Vector<T>
{
public:
    T  t;           ///< トレランス

public:
    TVector(T X=0, T Y=0, T Z=0, T W=0, double N=0.0, double C=1.0, int D=0) { set(X, Y, Z, W, N, C, D);}
    virtual ~TVector() {}

    void    set(T X, T Y=0, T Z=0, T W=0, double N=0.0, double C=1.0, int D=0);
    void    init() { Vector<T>::init(); t = (T)0;}
};



/**
template <typename T> void TVector<T>::set(T X, T Y, T Z, T W, double N) 

3次元ベクトルに値をセット．ノルムの計算有り
*/
template <typename T> void TVector<T>::set(T X, T Y, T Z, T W, double N, double C, int D) 
{
    Vector<T>::set(X, Y, Z, N, C, D);
    t = W;

    if (N<=0.0) {
        Vector<T>::n = (T)sqrt(X*X + Y*Y + Z*Z);
    }
}



/**
template <typename T> double  ProportionVector(TVector<T> v1, TVector<T> v2, T& t)

ベクトルv1, v2が一直線上にあるかどうかをチェックする．@n
v1 = c*v2 の cを返す. tには誤差が入る．
*/
template <typename T> double  ProportionVector(TVector<T> v1, TVector<T> v2, T& t)
{
    double  cc = 0.0;
    T  tt = (T)(Max(v2.t, Zero_Eps));
    if (v2.n>=tt) cc = (v1*v2)/(double)(v2.n*v2.n);

    TVector<T> dif = v1 - cc*v2;
    T tolerance = (T)Max(dif.t, Zero_Eps);
    if (dif.n>=tolerance) {
        t = 0.0;
        return  -HUGE_VALF; 
    }
    t = (T)((v1.n*v2.t + v2.n*v1.t)/(v2.n*v2.n));   // 誤差
    return cc;
}



//////////////////////////////////////////////////////////////////////////////////////////
// オペレータ

template<typename T> inline TVector<T> operator - (const TVector<T> a)
{ return TVector<T>(-a.x, -a.y, -a.z, a.t, a.n); }


template<typename T> inline TVector<T> operator + (const TVector<T> a, const TVector<T> b)
{ 
    T  xx = a.x + b.x;
    T  yy = a.y + b.y;
    T  zz = a.z + b.z;
    T  tt = a.t + b.t;
    if (Xabs(xx)<tt) xx = (T)0.0;
    if (Xabs(yy)<tt) yy = (T)0.0;
    if (Xabs(zz)<tt) zz = (T)0.0;
    return TVector<T>(xx, yy, zz, tt);
}


template<typename T, typename R> inline TVector<T> operator + (const R d, const TVector<T> a)
{ return TVector<T>((T)d+a.x, (T)d+a.y, (T)d+a.z, a.t);}


template<typename T, typename R> inline TVector<T> operator + (const TVector<T> a, const R d)
{ return TVector<T>(a.x+(T)d, a.y+(T)d, a.z+(T)d, a.t);}


template<typename T> inline TVector<T> operator - (const TVector<T> a, const TVector<T> b)
{
    T  xx = a.x - b.x;
    T  yy = a.y - b.y;
    T  zz = a.z - b.z;
    T  tt = a.t + b.t;
    if (Xabs(xx)<tt) xx = (T)0.0;
    if (Xabs(yy)<tt) yy = (T)0.0;
    if (Xabs(zz)<tt) zz = (T)0.0;
    return TVector<T>(xx, yy, zz, tt);
}


template<typename T, typename R> inline TVector<T> operator - (const R d, const TVector<T> a)
{ return TVector<T>((T)d-a.x, (T)d-a.y, (T)d-a.z, a.t);}


template<typename T, typename R> inline TVector<T> operator - (const TVector<T> a, const R d)
{ return TVector<T>(a.x-(T)d, a.y-(T)d, a.z-(T)d, a.t);}


template<typename T, typename R> inline TVector<T> operator * (const R d, const TVector<T> a)
{ return TVector<T>(a.x*(T)d, a.y*(T)d, a.z*(T)d, a.t*Xabs((T)d));}


template<typename T, typename R> inline TVector<T> operator * (const TVector<T> a, const R d)
{ return TVector<T>(a.x*(T)d, a.y*(T)d, a.z*(T)d, a.t*Xabs((T)d));}


template<typename T, typename R> inline TVector<T> operator / (const TVector<T> a, const R d)
{ return TVector<T>(a.x/(T)d, a.y/(T)d, a.z/(T)d, a.t/Xabs((T)d));}


template<typename T, typename R> inline TVector<T> operator / (const R d, const TVector<T> a)
{ return TVector<T>((T)d/a.x, (T)d/a.y, (T)d/a.z, Xabs((T)d)*a.t/a.norm2());}


template <typename T> inline bool operator == (const TVector<T> v1, const TVector<T> v2)
{
    T dst = (v1.x-v2.x)*(v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y) + (v1.z-v2.z)*(v1.z-v2.z);
    T err = (v1.t+v2.t)*(v1.t+v2.t);
    if (dst<=err) return true;
    return false;
}


template <typename T> inline bool operator != (const TVector<T> v1, const TVector<T> v2)
{
    T dst = (v1.x-v2.x)*(v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y) + (v1.z-v2.z)*(v1.z-v2.z);
    T err = (v1.t+v2.t)*(v1.t+v2.t);
    if (dst>err) return true;
    return false;
}


/// Cross product  外積
template<typename T> inline TVector<T> operator ^ (const TVector<T> a, const TVector<T> b)
{
    T  xx = a.y*b.z - a.z*b.y;
    T  yy = a.z*b.x - a.x*b.z;
    T  zz = a.x*b.y - a.y*b.x;
    T  tt = (T)(a.n*b.t + a.t*b.n);
    if (Xabs(xx)<tt) xx = (T)0.0;
    if (Xabs(yy)<tt) yy = (T)0.0;
    if (Xabs(zz)<tt) zz = (T)0.0;
    return TVector<T>(xx, yy, zz, tt);
}


/// Dot product    内積
template<typename T> inline T operator * (const TVector<T> a, const TVector<T> b)
{ return  (T)(a.x*b.x + a.y*b.y + a.z*b.z);}


/// 内積の誤差
template<typename T> inline T TVectorMultiTolerance(TVector<T> a, TVector<T> b) 
{ return  (T)(a.n*b.t + a.t*b.n); }



}       // namespace


#endif

