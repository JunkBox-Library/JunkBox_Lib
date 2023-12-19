
#ifndef  __JBXL_CPP_VECTOR_H_
#define  __JBXL_CPP_VECTOR_H_


/**
@brief    ベクトルライブラリ for C++
@file     Vector.h
@author   Fumi.Iseki (C)
*/



/*
template <typename T=double> class DllExport Vector      // ３次元ベクトルの定義
template <typename T=double> class DllExport Vector2D    // ２次元ベクトルの定義, パラメトリック曲面用
template <typename T=int> class DllExport PCoordinate   // 多角形の座標の値を格納するクラス

template <typename T=int> class DllExport RBound        // 境界構造体
template <typename T> inline bool disJunctBounds(RBound<T> b1, RBound<T> b2);

template <typename T> inline double VectorAngle(Vector<T> a, Vector<T> b)
template <typename T> inline Vector<T> NewellMethod(Vector<T> v1, Vector<T> v2, Vector<T> v3)
template <typename T> inline Vector<T> BSplineInterp4(Vector<T> p0, Vector<T> p1, double t)
*/



#include "tools++.h"
#include <math.h>
#include "Tolerance.h"



//
namespace  jbxl {



#define  VECTOR     Vector
#define  UVMAP      UVMap


#define  BOUNDARY_BLANK 5       ///< 境界 RBound<T> の余白


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector
//

/**
template <typename T=double> class Vector

    ３次元ベクトルの定義

*/
template <typename T=double> class DllExport Vector
{
public:
    T x;
    T y;
    T z;

    double n;       ///< ノルム
    double c;       ///< 信頼度
    int    d;       ///< 汎用

public:
    Vector(T X=0, T Y=0, T Z=0, double N=0.0, double C=1.0, int D=0) { set(X, Y, Z, N, C, D);}
    virtual ~Vector(void) {}

    T     norm2(void) { return (x*x + y*y + z*z);}
    double norm(void) { n = (double)sqrt(x*x + y*y + z*z); return n;}
    Vector<T> normalize(void);

    void   init(double C=1.0) { x = y = z = (T)0; n = 0.0; c = C; d = 0;}
    void   set(T X, T Y=0, T Z=0, double N=0.0, double C=1.0, int D=0);

    // 別名
    T      element1(void) { return x;}
    T      element2(void) { return y;}
    T      element3(void) { return z;}
    T      element(int i) { if(i==1) return x; else if(i==2) return y; else if(i==3) return z; else return (T)0;} 

    template <typename R> Vector<T>& operator = (const Vector<R> a) { x=(T)a.x; y=(T)a.y; z=(T)a.z; n=a.n; c=a.c; d=a.d; return *this;}
};



template <typename T> Vector<T> Vector<T>::normalize(void)
{
    double nrm = (double)sqrt(x*x+y*y+z*z);

    if (nrm>=Zero_Eps) {
        x  = (T)(x/nrm);
        y  = (T)(y/nrm);
        z  = (T)(z/nrm);
        n  = 1.0;
    }
    else {
        init();
    }

    return *this;
}



/**
template <typename T> void Vector<T>::set(T X, T Y, T Z, double N) 

3次元ベクトルに値をセット．
*/
template <typename T> inline void Vector<T>::set(T X, T Y, T Z, double N, double C, int D) 
{
    x  = X;
    y  = Y;
    z  = Z;
    n  = N;
    c  = C;
    d  = D;

    if (n<=0.0) {
        n = (double)sqrt(x*x + y*y + z*z);
    }
}



///////////////////////////////////////////////////////////////////////////
// Vector オペレータ

template <typename T> inline Vector<T> operator - (const Vector<T> a)
{ return Vector<T>(-a.x, -a.y, -a.z, a.n, a.c, a.d); }


template <typename T> inline Vector<T> operator + (const Vector<T> a, const Vector<T> b)
{ return Vector<T>(a.x+b.x, a.y+b.y, a.z+b.z, 0.0, Min(a.c, b.c), a.d); }


template <typename T, typename R> inline Vector<T> operator + (const Vector<T> a, R c)
{ return Vector<T>(a.x+(T)c, a.y+(T)c, a.z+(T)c, 0.0, a.c, a.d); }


template <typename T, typename R> inline Vector<T> operator + (const R c, Vector<T> a)
{ return Vector<T>((T)c+a.x, (T)c+a.y, (T)c+a.z, 0.0, a.c, a.d); }


template <typename T> inline Vector<T> operator - (const Vector<T> a, const Vector<T> b)
{ return Vector<T>(a.x-b.x, a.y-b.y, a.z-b.z, 0.0, Min(a.c, b.c), a.d); }


template <typename T, typename R> inline Vector<T> operator - (const Vector<T> a, R c)
{ return Vector<T>(a.x-(T)c, a.y-(T)c, a.z-(T)c, 0.0, a.c, a.d); }


template <typename T, typename R> inline Vector<T> operator - (R c, const Vector<T> a)
{ return Vector<T>((T)c-a.x, (T)c-a.y, (T)c-a.z, 0.0, a.c, a.d); }


template <typename T, typename R> inline Vector<T> operator * (const R d, const Vector<T> a)
{ return Vector<T>((T)d*a.x, (T)d*a.y, (T)d*a.z, (T)d*a.n, a.c, a.d); }


template <typename T, typename R> inline Vector<T> operator * (const Vector<T> a, const R d)
{ return Vector<T>(a.x*(T)d, a.y*(T)d, a.z*(T)d, a.n*(T)d, a.c, a.d); }


template <typename T, typename R> inline Vector<T> operator / (const Vector<T> a, const R d)
{ return Vector<T>(a.x/(T)d, a.y/(T)d, a.z/(T)d, a.n/(T)d, a.c, a.d); }


template <typename T, typename R> inline Vector<T> operator / (const R d, const Vector<T> a)
{
    Vector<T> v((T)d/a.x, (T)d/a.y, (T)d/a.z, 0.0, a.c, a.d);
    v.norm();
    return v;
}


/// Cross product  外積
template <typename T> inline Vector<T> operator ^ (const Vector<T> a, const Vector<T> b)
{ return Vector<T>(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x, 0.0, Min(a.c, b.c)); }


/// Dot product    内積
template <typename T> inline T operator * (const Vector<T> a, const Vector<T> b)
{ return (a.x*b.x + a.y*b.y + a.z*b.z); }


template <typename T> inline bool operator == (const Vector<T> v1, const Vector<T> v2)
{ return (v1.x==v2.x && v1.y==v2.y && v1.z==v2.z); }


template <typename T> inline bool operator != (const Vector<T> v1, const Vector<T> v2)
{ return (v1.x!=v2.x || v1.y!=v2.y || v1.z!=v2.z); }

//
template <typename T> inline Vector<T> MidPoint(const Vector<T> a, const Vector<T> b)
{ return 0.5*(a+b); }


//
/// 点a と b の距離　（a,b は位置ベクトル）
template <typename T> inline double VectorDist(const Vector<T> a, const Vector<T> b)
{
    //return (b-a).n;
    return (b-a).norm();
}



template <typename T> inline bool operator < (const Vector<T> v1, const Vector<T> v2)
{
    if (v1.x != v2.x) return v1.x < v2.x;
    if (v1.y != v2.y) return v1.y < v2.y;
    if (v1.z != v2.z) return v1.z < v2.z;
    return false;
}



///////////////////////////////////////////////////////////////////////////

/**
v1, v2 が同じ点かチェックする．信頼度は考慮しない．

*/
template <typename T> inline bool same_vector(Vector<T> v1, Vector<T> v2)
{
    T dx = v1.x - v2.x;
    T dy = v1.y - v2.y;
    T dz = v1.z - v2.z;
    T d2 = dx*dx + dy*dy + dz*dz;
    double t2 = Vector_Tolerance*Vector_Tolerance;

    if ((double)d2>t2) return false;
    return true;
}



template <typename T> inline Vector<T>* dupVector(Vector<T>* a, int n)
{
    Vector<T>* v = (Vector<T>*)malloc(sizeof(Vector<T>)*n);
    if (v==NULL) return NULL;

    for (int i=0; i<n; i++) v[i] = a[i];

    return v;
}


// angle between a and b
template <typename T> inline double VectorAngle(Vector<T> a, Vector<T> b)
{
    a.normalize();
    b.normalize();
    if (a.n<Zero_Eps || b.n<Zero_Eps) return 0.0;

    double cs = a*b;
    if      (cs>=1.0)  return 0.0;
    else if (cs<=-1.0) return PI;
    
    return acos(a*b);
}


template <typename T> inline double VectorAngle(Vector<T> a, Vector<T> b,  Vector<T> c)
{
    return VectorAngle(b-a, c-b);
}


//
/// Normal Vector of 3 Vectors with Newell Mothod
template <typename T> inline Vector<T> NewellMethod(Vector<T> v1, Vector<T> v2, Vector<T> v3)
{
    Vector<T> vect;

    vect.x = (v1.y-v2.y)*(v1.z+v2.z) + (v2.y-v3.y)*(v2.z+v3.z) + (v3.y-v1.y)*(v3.z+v1.z);
    vect.y = (v1.z-v2.z)*(v1.x+v2.x) + (v2.z-v3.z)*(v2.x+v3.x) + (v3.z-v1.z)*(v3.x+v1.x);
    vect.z = (v1.x-v2.x)*(v1.y+v2.y) + (v2.x-v3.x)*(v2.y+v3.y) + (v3.x-v1.x)*(v3.y+v1.y);
    vect.n = 0.0;
    vect.c = Min(v1.c, v2.c);
    vect.c = Min(v3.c, vect.c);

    return vect;
}


template <typename T> inline Vector<T> NewellMethod3(Vector<T> v1, Vector<T> v2, Vector<T> v3)
{
    return NewellMethod(v1, v2, v3);
}


template <typename T> inline Vector<T> NewellMethod4(Vector<T> v1, Vector<T> v2, Vector<T> v3, Vector<T> v4)
{
    Vector<T> vect;

    vect.x = (v1.y-v2.y)*(v1.z+v2.z) + (v2.y-v3.y)*(v2.z+v3.z) + (v3.y-v4.y)*(v3.z+v4.z) + (v4.y-v1.y)*(v4.z+v1.z);
    vect.y = (v1.z-v2.z)*(v1.x+v2.x) + (v2.z-v3.z)*(v2.x+v3.x) + (v3.z-v4.z)*(v3.x+v4.x) + (v4.z-v1.z)*(v4.x+v1.x);
    vect.z = (v1.x-v2.x)*(v1.y+v2.y) + (v2.x-v3.x)*(v2.y+v3.y) + (v3.x-v4.x)*(v3.y+v4.y) + (v4.x-v1.x)*(v4.y+v1.y);
    vect.n = 0.0;
    vect.c = Min(v1.c, v2.c);
    vect.c = Min(v3.c, vect.c);
    vect.c = Min(v4.c, vect.c);
    return vect;
}



/**
4階（3次）Bスプライン曲線補間．
2次の導関数が両端で 0の条件有り．

P(t) = 1/6*(1-t)^3*Q0 + (1/2*t^3 - t^2 + 2/3)*Q1 + (-1/2*t^3 + 1/2*t^2 + 1/2*t + 1/6)*Q2 + 1/6*t^3*Q3
    Q0 = 2*P(0) - P(1);
    Q1 = P(0)
    Q2 = P(1)
    Q3 = 2*P(1) - P(0)
*/
template <typename T> inline Vector<T> BSplineInterp4(Vector<T> p0, Vector<T> p1, double t)
{
    Vector<T> q0 = 2*p0 - p1;
//  Vector<T> q1 = p0;
//  Vector<T> q2 = p1;
    Vector<T> q3 = 2*p1 - p0;

    double t2 = t*t;
    double t3 = t2*t;
    
    double c0 = (t2 - t)*0.5 + (1.0 - t3)*0.1666666666666667;   // 1/6*(1-t)^3
    double c1 = t3*0.5 - t2 + 0.6666666666666667;               // 1/2*t3 - t2 + 2/3
    double c2 = (t + t2 - t3)*0.5 + 0.1666666666666667;         // -1/2*t3 + 1/2*t2 + 1/2*t + 1/6
    double c3 = t3*0.1666666666666667;                          // 1/6*t3

    Vector<T> vect = c0*q0 + c1*p0 + c2*p1 + c3*q3;

    return vect;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3D多角形座標
//

/**
template <typename T=int> class DllExport  PCoordinate

多角形の座標の値を格納するクラス

*/
template <typename T=int> class DllExport  PCoordinate
{
public:
    int  dim;               // 角数
    Vector<T>** point;      // データ

public:
    
    PCoordinate(void)  { dim = 0; point = NULL;}
    PCoordinate(int n) { init(n);}

    virtual ~PCoordinate(void) {}

    //
    void  init(int n) {
        dim   = 0;
        point = NULL;

        if (n>0) {
            point = (Vector<T>**)malloc(sizeof(Vector<T>*)*n);
            if (point!=NULL) {
                dim = n;
                for (int i=0; i<dim; i++) point[i] = new Vector<T>();
            }
        }
    }

    void  set(int m, T x, T y=0, T z=0, double n=0.0) {
        if (m>=0 && m<dim && point!=NULL) point[m]->set(x, y, z, n); 
    }       
    
    void  clear(void) {
        if (point!=NULL) {
            for (int i=0; i<dim; i++) point[i]->set((T)0);
        }
    }

    void  free(void) {
        if (point!=NULL) {
            for (int i=0; i<dim; i++) delete(point[i]);
            ::free(point);
            point = NULL;
        }
        dim = 0;
    }

};




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 境界構造体
//

/**
template <typename T=int> class RBound 

3Dデータの境界情報を保存する型． 
*/
template <typename T=int> class DllExport RBound 
{
public:
    T   xmin;       ///< x軸境界の最小値．
    T   xmax;       ///< x軸境界の最大値．
    T   ymin;       ///< y軸境界の最小値．
    T   ymax;       ///< y軸境界の最大値．
    T   zmin;       ///< z軸境界の最小値．
    T   zmax;       ///< z軸境界の最大値．
    T   tmin;       ///< 汎用．
    T   tmax;       ///< 汎用．

public:

    RBound(T XMin=(T)0, T XMax=(T)0, T YMin=(T)0, T YMax=(T)0, T ZMin=(T)0, T ZMax=(T)0, T TMin=(T)0, T TMax=(T)0) {
        set(XMin, XMax, YMin, YMax, ZMin, ZMax, TMin, TMax);
    }

    virtual ~RBound() {}


    //
    void  set(T XMin=(T)0, T XMax=(T)0, T YMin=(T)0, T YMax=(T)0, T ZMin=(T)0, T ZMax=(T)0, T TMin=(T)0, T TMax=(T)0){
        xmin = XMin;
        ymin = YMin;
        zmin = ZMin;
        xmax = XMax;
        ymax = YMax;
        zmax = ZMax;
        tmin = TMin;
        tmax = TMax;
    }       

    void init() { xmin = xmax = ymin = ymax = zmin = zmax = tmin = tmax = (T)0;}

    ///////////////////////////////////////////
    // 拡大

    /// 境界構造体 bound を広げる
    void  enlarge(T f) {
        xmin -= f;
        ymin -= f;
        zmin -= f;
        xmax += f;
        ymax += f;
        zmax += f;
    }

    void  multiple(T f) {
        xmin *= f;
        ymin *= f;
        zmin *= f;
        xmax *= f;
        ymax *= f;
        zmax *= f;
    }

    ///////////////////////////////////////////
    // 融合

    /// 境界構造体 bound と融合させる
    void  fusion(RBound<T> bound) {
        xmin = Min(xmin, bound.xmin);
        ymin = Min(ymin, bound.ymin);
        zmin = Min(zmin, bound.zmin);
        xmax = Max(xmax, bound.xmax);
        ymax = Max(ymax, bound.ymax);
        zmax = Max(zmax, bound.zmax);
    }

    /// ベクトル vect と融合させる  
    void  fusion(Vector<T> vect){
        xmin = Min(xmin, vect.x);
        ymin = Min(ymin, vect.y);
        zmin = Min(zmin, vect.z);
        xmax = Max(xmax, vect.x);
        ymax = Max(ymax, vect.y);
        zmax = Max(zmax, vect.z);
    }

    /// ポイントと融合させる  
    void  fusion(T x, T y, T z){
        xmin = Min(xmin, x);
        ymin = Min(ymin, y);
        zmin = Min(zmin, z);
        xmax = Max(xmax, x);
        ymax = Max(ymax, y);
        zmax = Max(zmax, z);
    }

    ///////////////////////////////////////////
    // 共通領域

    /// 境界構造体 bound との共通領域  共通領域がない場合は，min>max になる
    void  commonarea(RBound<T> bound) {
        xmin = Max(xmin, bound.xmin);
        ymin = Max(ymin, bound.ymin);
        zmin = Max(zmin, bound.zmin);
        xmax = Min(xmax, bound.xmax);
        ymax = Min(ymax, bound.ymax);
        zmax = Min(zmax, bound.zmax);
    }

    ///////////////////////////////////////////
    /// 切り出した場合の境界
    void  cutdown(RBound<T> bound) {
        xmin += bound.xmin;
        ymin += bound.ymin;
        zmin += bound.zmin;
        xmax += bound.xmin;
        ymax += bound.ymin;
        zmax += bound.zmin;
    }

    //   
    void  cutdown(Vector<T> vect){
        xmin += vect.x;
        ymin += vect.y;
        zmin += vect.z;
        xmax += vect.x;
        ymax += vect.y;
        zmax += vect.z;
    }

    //  
    void  cutdown(T x, T y, T z){
        xmin += x;
        ymin += y;
        zmin += z;
        xmax += x;
        ymax += y;
        zmax += z;
    }

    /// ベクトルが位置ベクトルの場合，その点は境界外か？ 境界外：true，境界内：false
    bool  outofBounds(Vector<T> vect) {
        return  vect.x < xmin || vect.x > xmax ||
                vect.y < ymin || vect.y > ymax ||
                vect.z < zmin || vect.z > zmax;
    }

    /// その点は境界外か？ 境界外：true，境界内：false
    bool  outofBounds(T x, T y, T z) {
        return  x < xmin || x > xmax ||
                y < ymin || y > ymax ||
                z < zmin || z > zmax;
    }
};



/**
template <typename T> inline bool disJunctBounds(RBound<T> b1, RBound<T> b2)
    
境界領域b1とb2が重なっているかどうかをチェック
   
@retval true   重なっていない
@retval false  重なっている 
*/
template <typename T> inline bool disJunctBounds(RBound<T> b1, RBound<T> b2)
{
    return  (b1.xmin >= b2.xmax) || (b2.xmin >= b1.xmax) || 
            (b1.ymin >= b2.ymax) || (b2.ymin >= b1.ymax) || 
            (b1.zmin >= b2.zmax) || (b2.zmin >= b1.zmax);
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UVMap
//

/**
template <typename T=double> class UVMap

２次元ベクトルの定義@n
パラメトリック曲面用
*/
template <typename T=double> class DllExport UVMap
{
public:
    T   u;
    T   v;
    int d;      ///< 汎用

public:
    UVMap(T U=0, T V=0, int d=0) { set(U, V, d);}
    virtual ~UVMap(void) {}

    void   init(void) { u = v = (T)0; d = 0;}
    void   set(T U, T V=0, int D=0) { u = U, v = V; d = D;}

    UVMap flip(void)  { u = (T)(1.0-u); v = (T)(1.0-v); return *this;}
    UVMap flipV(void) { v = (T)(1.0-v); return *this;}
    UVMap flipU(void) { u = (T)(1.0-u); return *this;}

    template <typename R> UVMap<T>& operator = (const UVMap<R> a) { u=(T)a.u; v=(T)a.v; d=a.d; return *this;}
};




///////////////////////////////////////////////////////////////////////////
// UVMap オペレータ

template <typename T> inline UVMap<T> operator - (const UVMap<T> a)
{ return UVMap<T>(-a.u, -a.v, a.d); }


template <typename T> inline UVMap<T> operator + (const UVMap<T> a, const UVMap<T> b)
{ return UVMap<T>(a.u+b.u, a.v+b.v, a.d); }


template <typename T, typename R> inline UVMap<T> operator + (const UVMap<T> a, R c)
{ return UVMap<T>(a.u+(T)c, a.v+(T)c, a.d); }


template <typename T, typename R> inline UVMap<T> operator + (const R c, UVMap<T> a)
{ return UVMap<T>((T)c+a.u, (T)c+a.v, a.d); }


template <typename T> inline UVMap<T> operator - (const UVMap<T> a, const UVMap<T> b)
{ return UVMap<T>(a.u-b.u, a.v-b.v, a.d); }


template <typename T, typename R> inline UVMap<T> operator - (const UVMap<T> a, R c)
{ return UVMap<T>(a.u-(T)c, a.v-(T)c, a.d); }


template <typename T, typename R> inline UVMap<T> operator - (const R c, UVMap<T>a)
{ return UVMap<T>((T)c-a.u, (T)c-a.v, a.d); }


template <typename T, typename R> inline UVMap<T> operator * (const R d, const UVMap<T> a)
{ return UVMap<T>((T)d*a.u, (T)d*a.v, a.d); }


template <typename T, typename R> inline UVMap<T> operator * (const UVMap<T> a, const R d)
{ return UVMap<T>(a.u*(T)d, a.v*(T)d, a.d); }


template <typename T, typename R> inline UVMap<T> operator / (const UVMap<T> a, const R d)
{ return UVMap<T>(a.u/(T)d, a.v/(T)d, a.d); }


template <typename T, typename R> inline UVMap<T> operator / (const R d, const UVMap<T> a)
{ return UVMap<T>((T)d/a.u, (T)d/a.v, a.d); }


template <typename T> inline bool operator == (const UVMap<T> a, const UVMap<T> b)
{ return (a.u==b.u && a.v==b.v); }


template <typename T> inline bool operator != (const UVMap<T> a, const UVMap<T> b)
{ return (a.u!=b.u || a.v!=b.v); }



}       // namespace

#endif 


