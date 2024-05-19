#ifndef  __JBXL_CPP_QUATERNION_H_
#define  __JBXL_CPP_QUATERNION_H_

/**
@brief    回転・クォータニオン ライブラリ ヘッダ
@file     Rotation.h
@author   Fumi.Iseki (C)
*/

#include  "Matrix++.h"
#include  "Vector.h"


//
namespace  jbxl {


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// クォータニオン
//

template <typename T> class  DllExport Quaternion;

// Q -> E
template <typename T> Vector<T>  Quaternion2ExtEulerXYZ(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T> Vector<T>  Quaternion2ExtEulerZYX(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T> Vector<T>  Quaternion2ExtEulerXZY(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T> Vector<T>  Quaternion2ExtEulerYZX(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T> Vector<T>  Quaternion2ExtEulerYXZ(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T> Vector<T>  Quaternion2ExtEulerZXY(Quaternion<T> qut, Vector<T>* vct=NULL);



////////////////////////////////////////////////////////////////////////////////////////////////

/**
class  DllExport Quaternion

クォータニオンの定義

回転の合成：A*B => Bの回転 -> Aの回転
*/
template <typename T=double> class  DllExport Quaternion
{
public:
    T    s;     ///< cos(θ/2)
    T    x;     ///< x 成分
    T    y;     ///< y 成分
    T    z;     ///< z 成分

    T    n;     ///< ノルム
    T    c;     ///< 信頼度

public:
    Quaternion(void)  { init();}
    Quaternion(T S, T X, T Y, T Z, T N=(T)0.0, T C=(T)1.0) { set(S, X, Y, Z, N, C);}
    Quaternion(T S, Vector<T> v) { setRotation(S, v);}
    virtual ~Quaternion(void) {}

    T       norm(void)  { n = (T)sqrt(s*s+x*x+y*y+z*z); return n;}
    void    normalize(void);

    void    set(T S, T X, T Y, T Z, T N=(T)0.0, T C=(T)1.0);
    void    init(T C=(T)1.0)  { s=n=(T)1.0; x=y=z=(T)0.0; c=C;}

    Vector<T> getVector() { Vector<T> v(x, y, z, (T)0.0, c); return v;}
    T         getScalar() { return s;}
    T         getAngle()  { return (T)(2.0*acos(s));}    ///< 0〜2π
    T         getMathAngle() { T t=(T)(2.0*acos(s)); if(t>(T)PI)t-=(T)PI2; return t;} ///< -π〜π
    
    Matrix<T> getRotMatrix();

    void    setRotation(T e, Vector<T> v);
    void    setRotation(T e, T X, T Y, T Z, T N=(T)0.0) { Vector<T> v(X, Y, Z, N); setRotation(e, v);}

    void    setRotate(T e, Vector<T> v) { setRotation(e, v);}
    void    setRotate(T e, T X, T Y, T Z, T N=(T)0.0)   { Vector<T> v(X, Y, Z, N); setRotation(e, v);}

    // 注：eの成分は演算順
    void    setExtEulerXYZ(Vector<T> e);    //
    void    setExtEulerZYX(Vector<T> e);    //
    void    setExtEulerXZY(Vector<T> e);    // 
    void    setExtEulerYZX(Vector<T> e);    // 
    void    setExtEulerYXZ(Vector<T> e);    // 
    void    setExtEulerZXY(Vector<T> e);    // 

    Vector<T> getExtEulerXYZ(Vector<T>* vt=NULL) { return Quaternion2ExtEulerXYZ<T>(*this, vt);}
    Vector<T> getExtEulerZYX(Vector<T>* vt=NULL) { return Quaternion2ExtEulerZYX<T>(*this, vt);}
    Vector<T> getExtEulerXZY(Vector<T>* vt=NULL) { return Quaternion2ExtEulerXZY<T>(*this, vt);}
    Vector<T> getExtEulerYZX(Vector<T>* vt=NULL) { return Quaternion2ExtEulerYZX<T>(*this, vt);}
    Vector<T> getExtEulerYXZ(Vector<T>* vt=NULL) { return Quaternion2ExtEulerYXZ<T>(*this, vt);}
    Vector<T> getExtEulerZXY(Vector<T>* vt=NULL) { return Quaternion2ExtEulerZXY<T>(*this, vt);}

    Vector<T> execRotation(Vector<T> v);
    Vector<T> execInvRotation(Vector<T> v);
    Vector<T> execRotate(Vector<T> v)    { return execRotation(v);}
    Vector<T> execInvRotate(Vector<T> v) { return execInvRotation(v);}
};



/////////////////////////////////////////////////////////////////////////////////////
// Quaternion オペレータ

template <typename T> inline bool operator == (const Quaternion<T> q1, const Quaternion<T> q2)
{ return (q1.s==q2.s && q1.x==q2.x && q1.y==q2.y && q1.z==q2.z); }


template <typename T> inline bool operator != (const Quaternion<T> q1, const Quaternion<T> q2)
{ return (q1.s!=q2.s || q1.x!=q2.x || q1.y!=q2.y || q1.z!=q2.z); }


///< ~ 共役
template <typename T> inline Quaternion<T> operator ~ (const Quaternion<T> a)
{ return Quaternion<T>(a.s, -a.x, -a.y, -a.z, a.n, a.c); }


//
template <typename T> inline Quaternion<T> operator - (const Quaternion<T> a)
{ return Quaternion<T>(-a.s, -a.x, -a.y, -a.z, a.n, a.c); }


template <typename T> inline Quaternion<T> operator + (const Quaternion<T> a, const Quaternion<T> b)
{ return Quaternion<T>(a.s+b.s, a.x+b.x, a.y+b.y, a.z+b.z, (T)0.0, Min(a.c, b.c)); }


template <typename T> inline Quaternion<T> operator - (const Quaternion<T> a, const Quaternion<T> b)
{ return Quaternion<T>(a.s-b.s, a.x-b.x, a.y-b.y, a.z-b.z, (T)0.0, Min(a.c, b.c)); }


template <typename T, typename R> inline Quaternion<T> operator * (const R d, const Quaternion<T> a)
{ return Quaternion<T>((T)d*a.s, (T)d*a.x, (T)d*a.y, (T)d*a.z, (T)d*a.n, a.c); }


template <typename T, typename R> inline Quaternion<T> operator * (const Quaternion<T> a, const R d)
{ return Quaternion<T>(a.s*(T)d, a.x*(T)d, a.y*(T)d, a.z*(T)d, a.n*(T)d, a.c); }


template <typename T, typename R> inline Quaternion<T> operator / (const Quaternion<T> a, const R d)
{ return Quaternion<T>(a.s/(T)d, a.x/(T)d, a.y/(T)d, a.z/(T)d, a.n/(T)d, a.c); }


/**
演算順序 A*B = Bの回転 -> Aの回転
*/
template <typename T> inline Quaternion<T> operator * (const Quaternion<T> a, const Quaternion<T> b)
{
    Quaternion<T> quat;
    if (a.c<(T)0.0 || b.c<(T)0.0) quat.init(-(T)1.0);
    else  quat.set(a.s*b.s-a.x*b.x-a.y*b.y-a.z*b.z, a.s*b.x+a.x*b.s+a.y*b.z-a.z*b.y, 
                   a.s*b.y+a.y*b.s+a.z*b.x-a.x*b.z, a.s*b.z+a.z*b.s+a.x*b.y-a.y*b.x, (T)1.0, Min(a.c, b.c)); 
    return quat;
}


// *
template <typename T> inline Quaternion<T> operator * (const Quaternion<T> q, const Vector<T> v)
{ 
    Quaternion<T> quat;
    if (q.c<(T)0.0 || v.c<(T)0.0) quat.init(-(T)1.0);
    else quat.set(-q.x*v.x-q.y*v.y-q.z*v.z, q.s*v.x+q.y*v.z-q.z*v.y, 
                   q.s*v.y+q.z*v.x-q.x*v.z, q.s*v.z+q.x*v.y-q.y*v.x, v.n, Min(q.c, v.c));
    return quat;
}


// *
template <typename T> inline Quaternion<T> operator * (const Vector<T> v, const Quaternion<T> q)
{ 
    Quaternion<T> quat;
    if (q.c<(T)0.0 || v.c<(T)0.0) quat.init(-(T)1.0);
    else quat.set(-v.x*q.x-v.y*q.y-v.z*q.z, v.x*q.s+v.y*q.z-v.z*q.y, 
                   v.y*q.s+v.z*q.x-v.x*q.z, v.z*q.s+v.x*q.y-v.y*q.x, (T)v.n, (T)Min(v.c, q.c));
    return quat;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 回転行列，オイラー角，クォータニオン
//
// 注：オイラー角ベクトルの成分は演算順に指定する．
//        例 YZX回転なら e.x:Y回転，e.y:Z回転, e.z:X回転
//     オイラー角の回転は固定座標での計算（点，ベクトルの計算）
//

// E -> R
/*
template <typename T=double> Matrix<T>  ExtEulerXYZ2RotMatrix(Vector<T> eul);
template <typename T=double> Matrix<T>  ExtEulerZYX2RotMatrix(Vector<T> eul);
template <typename T=double> Matrix<T>  ExtEulerXZY2RotMatrix(Vector<T> eul);
template <typename T=double> Matrix<T>  ExtEulerYZX2RotMatrix(Vector<T> eul);
template <typename T=double> Matrix<T>  ExtEulerYXZ2RotMatrix(Vector<T> eul);
template <typename T=double> Matrix<T>  ExtEulerZXY2RotMatrix(Vector<T> eul);
*/


// R -> E
/*
template <typename T=double> Vector<T>  RotMatrix2ExtEulerXYZ(Matrix<T> mtx, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrix2ExtEulerZYX(Matrix<T> mtx, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrix2ExtEulerXZY(Matrix<T> mtx, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrix2ExtEulerYZX(Matrix<T> mtx, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrix2ExtEulerYXZ(Matrix<T> mtx, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrix2ExtEulerZXY(Matrix<T> mtx, Vector<T>* vct=NULL);

template <typename T=double> Vector<T>  RotMatrixElements2ExtEulerXYZ(T m11, T m12, T m13, T m21, T m31, T m32, T m33, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrixElements2ExtEulerZYX(T m11, T m12, T m13, T m21, T m23, T m31, T m33, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrixElements2ExtEulerXZY(T m11, T m12, T m13, T m21, T m22, T m23, T m31, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrixElements2ExtEulerYZX(T m11, T m12, T m13, T m21, T m22, T m31, T m32, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrixElements2ExtEulerYXZ(T m12, T m21, T m22, T m23, T m31, T m32, T m33, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  RotMatrixElements2ExtEulerZXY(T m12, T m13, T m21, T m22, T m23, T m32, T m33, Vector<T>* vct=NULL);
*/


// Q -> E
/*
template <typename T=double> Vector<T>  Quaternion2ExtEulerXYZ(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  Quaternion2ExtEulerZYX(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  Quaternion2ExtEulerXZY(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  Quaternion2ExtEulerYZX(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  Quaternion2ExtEulerYXZ(Quaternion<T> qut, Vector<T>* vct=NULL);
template <typename T=double> Vector<T>  Quaternion2ExtEulerZXY(Quaternion<T> qut, Vector<T>* vct=NULL);
*/


// R -> Q
//template <typename T=double> Quaternion<T>    RotMatrix2Quaternion<T>(Matrix<T> mtx);


// Q -> R
#define          Quaternion2RotMatrix(q)  (q).getRotMatrix()


// E -> Q   （注：eの成分は演算順）
template <typename T> inline Quaternion<T> ExtEulerXYZ2Quaternion(Vector<T> e) { Quaternion<T> q; q.setExtEulerXYZ(e); return q;}
template <typename T> inline Quaternion<T> ExtEulerZYX2Quaternion(Vector<T> e) { Quaternion<T> q; q.setExtEulerZYX(e); return q;}
template <typename T> inline Quaternion<T> ExtEulerXZY2Quaternion(Vector<T> e) { Quaternion<T> q; q.setExtEulerXZY(e); return q;}
template <typename T> inline Quaternion<T> ExtEulerYZX2Quaternion(Vector<T> e) { Quaternion<T> q; q.setExtEulerYZX(e); return q;}
template <typename T> inline Quaternion<T> ExtEulerYXZ2Quaternion(Vector<T> e) { Quaternion<T> q; q.setExtEulerYXZ(e); return q;}
template <typename T> inline Quaternion<T> ExtEulerZXY2Quaternion(Vector<T> e) { Quaternion<T> q; q.setExtEulerZXY(e); return q;}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ベクトルの回転，クォータニオン
//

//template <typename T=double>  Vector<T>    VectorRotation<T>(Vector<T> v, Quaternion<T> q);        // たぶん execRotation() よりは高速．
//template <typename T=double>  Vector<T>    VectorInvRotation<T>(Vector<T> v, Quaternion<T> q);
//template <typename T=double>  T*           VectorRotation<T>(T* v, Quaternion<T> q);
//template <typename T=double>  T*           VectorInvRotation<T>(T* v, Quaternion<T> q);
#define    VectorRotate(v, q)       VectorRotation((v),(q))
#define    VectorInvRotate(v, q)    VectorInvRotation((v), (q))

/*
template <typename T=double> Quaternion<T>    V2VQuaternion(Vector<T> a, Vector<T> b);

template <typename T=double> Quaternion<T>    PPPQuaternion(Vector<T> a, Vector<T> b, Vector<T> c);
template <typename T=double> Quaternion<T>    VPPQuaternion(Vector<T> a, Vector<T> b, Vector<T> c);
template <typename T=double> Quaternion<T>    PPVQuaternion(Vector<T> a, Vector<T> b, Vector<T> c);


template <typename T=double> Quaternion<T>    SlerpQuaternion(Quaternion<T> a, Quaternion<T> b, T t);
*/



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// クォータニオン クラス
//

template <typename T> void  Quaternion<T>::set(T S, T X, T Y, T Z, T N, T C)
{
    s  = S;
    x  = X;
    y  = Y;
    z  = Z;
    n  = N;
    c  = C;

    if (n<=0.0) {
        n = (T)sqrt(s*s + x*x + y*y + z*z);
    }
}


template <typename T> void  Quaternion<T>::normalize() 
{
    T nrm = (T)sqrt(s*s + x*x + y*y + z*z);
    
    if (nrm>=Zero_Eps) {
        s  = s/nrm;
        x  = x/nrm;
        y  = y/nrm;
        z  = z/nrm;
        n  = (T)1.0;
    }
    else {
        init();
    }
}


/**
回転を定義する

@param e 回転角．ラジアン単位．
@param v 回転軸
*/
template <typename T> void  Quaternion<T>::setRotation(T e, Vector<T> v)
{
    if (v.n!=(T)1.0) v.normalize();

    if (e>(T)PI)       e = e - (T)PI2;
    else if (e<-(T)PI) e = (T)PI2 + e;

    T s2 = e/(T)2.0;
    T sn =(T) sin(s2);
    s  = (T)cos(s2);
    x  = v.x*sn;
    y  = v.y*sn;
    z  = v.z*sn;
    n  = (T)v.n;
    c  = (T)1.0;
}


/**
template <typename T> void  Quaternion<T>::setExtEulerYZX(Vector<T> e)

Y->Z->X の順にオイラー角でベクトルを回転させる場合のクオータニオンを計算する．

@param e オイラー角の成分．作用させる順番に格納する．
         この場合, e.xはY軸回転，e.yはZ軸回転，e.z はX軸回転のそれぞれの回転角（ラジアン）を格納する．
*/

/// Y->Z->X
template <typename T> void  Quaternion<T>::setExtEulerYZX(Vector<T> e)
{
    Vector<T> ex((T)1.0, (T)0.0, (T)0.0, (T)1.0);  
    Vector<T> ey((T)0.0, (T)1.0, (T)0.0, (T)1.0);  
    Vector<T> ez((T)0.0, (T)0.0, (T)1.0, (T)1.0);

    Quaternion<T> qx, qy, qz;
    qy.setRotation(e.element1(), ey);
    qz.setRotation(e.element2(), ez);
    qx.setRotation(e.element3(), ex);

    (*this) = qx*qz*qy;
    if (s<(T)0.0) (*this) = - (*this);
}


/// X->Y->Z
template <typename T> void  Quaternion<T>::setExtEulerXYZ(Vector<T> e)
{
    Vector<T> ex((T)1.0, (T)0.0, (T)0.0, (T)1.0);  
    Vector<T> ey((T)0.0, (T)1.0, (T)0.0, (T)1.0);  
    Vector<T> ez((T)0.0, (T)0.0, (T)1.0, (T)1.0);

    Quaternion<T> qx, qy, qz;
    qx.setRotation(e.element1(), ex);
    qy.setRotation(e.element2(), ey);
    qz.setRotation(e.element3(), ez);

    (*this) = qz*qy*qx;
    if (s<(T)0.0) (*this) = - (*this);
}


/// Z->Y->X
template <typename T> void  Quaternion<T>::setExtEulerZYX(Vector<T> e)
{
    Vector<T> ex((T)1.0, (T)0.0, (T)0.0, (T)1.0);  
    Vector<T> ey((T)0.0, (T)1.0, (T)0.0, (T)1.0);  
    Vector<T> ez((T)0.0, (T)0.0, (T)1.0, (T)1.0);

    Quaternion<T> qx, qy, qz;
    qz.setRotation(e.element1(), ez);
    qy.setRotation(e.element2(), ey);
    qx.setRotation(e.element3(), ex);

    (*this) = qx*qy*qz;
    if (s<(T)0.0) (*this) = - (*this);
}


/// X->Z->Y
template <typename T> void  Quaternion<T>::setExtEulerXZY(Vector<T> e)
{
    Vector<T> ex((T)1.0, (T)0.0, (T)0.0, (T)1.0);  
    Vector<T> ey((T)0.0, (T)1.0, (T)0.0, (T)1.0);  
    Vector<T> ez((T)0.0, (T)0.0, (T)1.0, (T)1.0);

    Quaternion<T> qx, qy, qz;
    qx.setRotation(e.element1(), ex);
    qz.setRotation(e.element2(), ez);
    qy.setRotation(e.element3(), ey);

    (*this) = qy*qz*qx;
    if (s<(T)0.0) (*this) = - (*this);
}


/// Y->X->Z
template <typename T> void  Quaternion<T>::setExtEulerYXZ(Vector<T> e)
{
    Vector<T> ex((T)1.0, (T)0.0, (T)0.0, (T)1.0);  
    Vector<T> ey((T)0.0, (T)1.0, (T)0.0, (T)1.0);  
    Vector<T> ez((T)0.0, (T)0.0, (T)1.0, (T)1.0);

    Quaternion<T> qx, qy, qz;
    qy.setRotation(e.element1(), ey);
    qx.setRotation(e.element2(), ex);
    qz.setRotation(e.element3(), ez);

    (*this) = qz*qx*qy;
    if (s<(T)0.0) (*this) = - (*this);
}


/// Z->X->Y
template <typename T> void  Quaternion<T>::setExtEulerZXY(Vector<T> e)
{
    Vector<T> ex((T)1.0, (T)0.0, (T)0.0, (T)1.0);  
    Vector<T> ey((T)0.0, (T)1.0, (T)0.0, (T)1.0);  
    Vector<T> ez((T)0.0, (T)0.0, (T)1.0, (T)1.0);

    Quaternion<T> qx, qy, qz;
    qz.setRotation(e.element1(), ez);
    qx.setRotation(e.element2(), ex);
    qy.setRotation(e.element3(), ey);

    (*this) = qy*qx*qz;
    if (s<(T)0.0) (*this) = - (*this);
}


/**
ベクトルを回転させる場合の回転行列を求める．
座標軸の回転の場合は共役クォータニオンで計算する．

*/
template <typename T> Matrix<T>  Quaternion<T>::getRotMatrix()
{
    Matrix<T> mtx(2, 3, 3);    // 2次元マトリックス, 3x3

    if (n!=(T)1.0) normalize();

    mtx.element(1, 1) = (T)1.0 - (T)2.0*y*y - (T)2.0*z*z;
    mtx.element(1, 2) = (T)2.0*x*y - (T)2.0*s*z;
    mtx.element(1, 3) = (T)2.0*x*z + (T)2.0*s*y;
    mtx.element(2, 1) = (T)2.0*x*y + (T)2.0*s*z;
    mtx.element(2, 2) = (T)1.0 - (T)2.0*x*x - (T)2.0*z*z;
    mtx.element(2, 3) = (T)2.0*y*z - (T)2.0*s*x;
    mtx.element(3, 1) = (T)2.0*x*z - (T)2.0*s*y;
    mtx.element(3, 2) = (T)2.0*y*z + (T)2.0*s*x;
    mtx.element(3, 3) = (T)1.0 - (T)2.0*x*x - (T)2.0*y*y;

    return mtx;
}


/// Exec Rotation  qv(~q)
template <typename T> Vector<T>  Quaternion<T>::execRotation(Vector<T> v)
{
    if (c<(T)0.0) return v;

    Quaternion<T> inv = ~(*this);
    Quaternion<T> qut =  (*this)*(v*inv);
    Vector<T> vct = qut.getVector();
    vct.d = v.d;

    return vct;
}


/// Exec Inv Rotation  (~q)vq
template <typename T> Vector<T>  Quaternion<T>::execInvRotation(Vector<T> v)
{
    if (c<(T)0.0) return v;

    Quaternion<T> inv = ~(*this);
    Quaternion<T> qut =  inv*(v*(*this));
    Vector<T> vct = qut.getVector();
    vct.d = v.d;

    return vct;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 回転行列，オイラー角
//
//   オイラー角の角度は全て右手系の正の方向が基準
//   オイラー角ベクトルの成分は演算順
//   固定座標（ベクトルの計算）
//

// ExtEuler -> Rotation Matrix
//
template <typename T> Matrix<T>  ExtEulerXYZ2RotMatrix(Vector<T> eul)
{
    Matrix<T> mtx(2, 3, 3);

    T sinx = (T)sin(eul.element1());
    T cosx = (T)cos(eul.element1());
    T siny = (T)sin(eul.element2());
    T cosy = (T)cos(eul.element2());
    T sinz = (T)sin(eul.element3());
    T cosz = (T)cos(eul.element3());

    mtx.element(1, 1) =  cosy*cosz;
    mtx.element(1, 2) =  sinx*siny*cosz - cosx*sinz;
    mtx.element(1, 3) =  cosx*siny*cosz + sinx*sinz;
    mtx.element(2, 1) =  cosy*sinz;
    mtx.element(2, 2) =  sinx*siny*sinz + cosx*cosz;
    mtx.element(2, 3) =  cosx*siny*sinz - sinx*cosz;
    mtx.element(3, 1) = -siny;
    mtx.element(3, 2) =  sinx*cosy;
    mtx.element(3, 3) =  cosx*cosy;

    return mtx;
}


template <typename T> Matrix<T>  ExtEulerZYX2RotMatrix(Vector<T> eul)
{
    Matrix<T> mtx(2, 3, 3);

    T sinz = (T)sin(eul.element1());
    T cosz = (T)cos(eul.element1());
    T siny = (T)sin(eul.element2());
    T cosy = (T)cos(eul.element2());
    T sinx = (T)sin(eul.element3());
    T cosx = (T)cos(eul.element3());

    mtx.element(1, 1) =  cosy*cosz;
    mtx.element(1, 2) = -cosy*sinz;
    mtx.element(1, 3) =  siny;
    mtx.element(2, 1) =  sinx*siny*cosz + cosx*sinz;
    mtx.element(2, 2) = -sinx*siny*sinz + cosx*cosz;
    mtx.element(2, 3) = -sinx*cosy;
    mtx.element(3, 1) = -cosx*siny*cosz + sinx*sinz;
    mtx.element(3, 2) =  cosx*siny*sinz + sinx*cosz;
    mtx.element(3, 3) =  cosx*cosy;

    return mtx;
}


template <typename T> Matrix<T>  ExtEulerXZY2RotMatrix(Vector<T> eul)
{
    Matrix<T> mtx(2, 3, 3);

    T sinx = (T)sin(eul.element1());
    T cosx = (T)cos(eul.element1());
    T sinz = (T)sin(eul.element2());
    T cosz = (T)cos(eul.element2());
    T siny = (T)sin(eul.element3());
    T cosy = (T)cos(eul.element3());

    mtx.element(1, 1) =  cosy*cosz;
    mtx.element(1, 2) = -cosx*cosy*sinz + sinx*siny;
    mtx.element(1, 3) =  sinx*cosy*sinz + cosx*siny;
    mtx.element(2, 1) =  sinz;
    mtx.element(2, 2) =  cosx*cosz;
    mtx.element(2, 3) = -sinx*cosz;
    mtx.element(3, 1) = -siny*cosz;
    mtx.element(3, 2) =  cosx*siny*sinz + sinx*cosy;
    mtx.element(3, 3) = -sinx*siny*sinz + cosx*cosy;

    return mtx;
}


template <typename T> Matrix<T>  ExtEulerYZX2RotMatrix(Vector<T> eul)
{
    Matrix<T> mtx(2, 3, 3);

    T siny = (T)sin(eul.element1());
    T cosy = (T)cos(eul.element1());
    T sinz = (T)sin(eul.element2());
    T cosz = (T)cos(eul.element2());
    T sinx = (T)sin(eul.element3());
    T cosx = (T)cos(eul.element3());

    mtx.element(1, 1) =  cosy*cosz;
    mtx.element(1, 2) = -sinz;
    mtx.element(1, 3) =  siny*cosz;
    mtx.element(2, 1) =  cosx*cosy*sinz + sinx*siny;
    mtx.element(2, 2) =  cosx*cosz;
    mtx.element(2, 3) =  cosx*siny*sinz - sinx*cosy;
    mtx.element(3, 1) =  sinx*cosy*sinz - cosx*siny;
    mtx.element(3, 2) =  sinx*cosz;
    mtx.element(3, 3) =  sinx*siny*sinz + cosx*cosy;

    return mtx;
}


template <typename T> Matrix<T>  ExtEulerZXY2RotMatrix(Vector<T> eul)
{
    Matrix<T> mtx(2, 3, 3);

    T sinz = (T)sin(eul.element1());
    T cosz = (T)cos(eul.element1());
    T sinx = (T)sin(eul.element2());
    T cosx = (T)cos(eul.element2());
    T siny = (T)sin(eul.element3());
    T cosy = (T)cos(eul.element3());

    mtx.element(1, 1) =  sinx*siny*sinz + cosy*cosz;
    mtx.element(1, 2) =  sinx*siny*cosz - cosy*sinz;
    mtx.element(1, 3) =  cosx*siny;
    mtx.element(2, 1) =  cosx*sinz;
    mtx.element(2, 2) =  cosx*cosz;
    mtx.element(2, 3) = -sinx;
    mtx.element(3, 1) =  sinx*cosy*sinz - siny*cosz;
    mtx.element(3, 2) =  sinx*cosy*cosz + siny*sinz;
    mtx.element(3, 3) =  cosx*cosy;
    
    return mtx;
}


template <typename T> Matrix<T>  ExtEulerYXZ2RotMatrix(Vector<T> eul)
{
    Matrix<T> mtx(2, 3, 3);

    T siny = (T)sin(eul.element1());
    T cosy = (T)cos(eul.element1());
    T sinx = (T)sin(eul.element2());
    T cosx = (T)cos(eul.element2());
    T sinz = (T)sin(eul.element3());
    T cosz = (T)cos(eul.element3());

    mtx.element(1, 1) = -sinx*siny*sinz + cosy*cosz;
    mtx.element(1, 2) = -cosx*sinz;
    mtx.element(1, 3) =  sinx*cosy*sinz + siny*cosz;
    mtx.element(2, 1) =  sinx*siny*cosz + cosy*sinz;
    mtx.element(2, 2) =  cosx*cosz;
    mtx.element(2, 3) = -sinx*cosy*cosz + siny*sinz;
    mtx.element(3, 1) = -cosx*siny;
    mtx.element(3, 2) =  sinx;
    mtx.element(3, 3) =  cosx*cosy;

    return mtx;
}


// Rotation Matrix Elements -> ExtEuler
//
template <typename T> Vector<T>  RotMatrixElements2ExtEulerXYZ(T m11, T m12, T m13, T m21, T m31, T m32, T m33, Vector<T>* vct=NULL)
{
    Vector<T> eul((T)0.0, (T)0.0, (T)0.0, (T)0.0, -(T)1.0);
    Vector<T> ev[2];
    T arctang;

    if (m31<-(T)1.0 || m31>(T)1.0) return eul;

    if ((T)1.0-m31<(T)Zero_Eps || (T)1.0+m31<(T)Zero_Eps) {
        if ((T)1.0-m31<Zero_Eps) {              // m31==1.0
            ev[0].y = ev[1].y = -(T)PI_DIV2;
            ev[0].z = (T)0.0;
            ev[1].z = (T)PI_DIV2;
            arctang = (T)atan2(-m12, -m13);
            ev[0].x = arctang - ev[0].z;
            ev[1].x = arctang - ev[1].z;
        }
        else {                                  // m31==-1.0
            ev[0].y = ev[1].y = (T)PI_DIV2;
            ev[0].z = (T)0.0;
            ev[1].z = (T)PI_DIV2;
            arctang = (T)atan2(m12, m13);
            ev[0].x = arctang + ev[0].z;
            ev[1].x = arctang + ev[1].z;
        }
    }

    else {
        ev[0].y = -(T)asin(m31); 
        if (ev[0].y>=(T)0.0) ev[1].y =   (T)PI - ev[0].y;    // 別解
        else                   ev[1].y = - (T)PI - ev[0].y;

        T cy1 = (T)cos(ev[0].y);
        T cy2 = (T)cos(ev[1].y);
        if (Xabs(cy1)<(T)Zero_Eps || Xabs(cy2)<(T)Zero_Eps) return eul;

        ev[0].x = (T)atan2(m32/cy1, m33/cy1);
        ev[0].z = (T)atan2(m21/cy1, m11/cy1);

        if (ev[0].x>=(T)0.0) ev[1].x = ev[0].x - (T)PI;
        else                 ev[1].x = ev[0].x + (T)PI;
        if (ev[0].z>=(T)0.0) ev[1].z = ev[0].z - (T)PI;
        else                 ev[1].z = ev[0].z + (T)PI;
    }

    //
    // XYZ
    if (vct!=NULL) {
        vct[0].set(ev[0].x, ev[0].y, ev[0].z);
        vct[1].set(ev[1].x, ev[1].y, ev[1].z);
    }
    eul.set(ev[0].x, ev[0].y, ev[0].z);

    return eul;
}


template <typename T> Vector<T>  RotMatrixElements2ExtEulerZYX(T m11, T m12, T m13, T m21, T m23, T m31, T m33, Vector<T>* vct=NULL)
{
    Vector<T> eul((T)0.0, (T)0.0, (T)0.0, (T)0.0, -(T)1.0);
    Vector<T> ev[2];
    T arctang;

    if (m13<-(T)1.0 || m13>(T)1.0) return eul;

    if ((T)1.0-m13<(T)Zero_Eps || (T)1.0+m13<(T)Zero_Eps) {    
        if ((T)1.0-m13<(T)Zero_Eps) {           // m13==1.0
            ev[0].y = ev[1].y = (T)PI_DIV2;
            ev[0].z = (T)0.0;
            ev[1].z = (T)PI_DIV2;
            arctang = (T)atan2(m21, -m31);
            ev[0].x = (T)arctang - ev[0].z;
            ev[1].x = (T)arctang - ev[1].z;
        }
        else {                                  // m13==-1.0
            ev[0].y = ev[1].y = -(T)PI_DIV2;
            ev[0].z = (T)0.0;
            ev[1].z = (T)PI_DIV2;
            arctang = (T)atan2(-m21, m31);
            ev[0].x = (T)arctang + ev[0].z;
            ev[1].x = (T)arctang + ev[1].z;
        }
    }

    else {
        ev[0].y = (T)asin(m13); 
        if (ev[0].y>=0.0) ev[1].y =  (T)PI - ev[0].y;
        else              ev[1].y = -(T)PI - ev[0].y;

        T cy1 = (T)cos(ev[0].y);
        T cy2 = (T)cos(ev[1].y);
        if (Xabs(cy1)<Zero_Eps || Xabs(cy2)<Zero_Eps) return eul;

        ev[0].x = atan2(-m23/cy1, m33/cy1);
        ev[0].z = atan2(-m12/cy1, m11/cy1);

        if (ev[0].x>=(T)0.0) ev[1].x = ev[0].x - (T)PI;
        else                 ev[1].x = ev[0].x + (T)PI;
        if (ev[0].z>=(T)0.0) ev[1].z = ev[0].z - (T)PI;
        else                 ev[1].z = ev[0].z + (T)PI;
    }

    //
    // ZYX
    if (vct!=NULL) {
        vct[0].set(ev[0].z, ev[0].y, ev[0].x);
        vct[1].set(ev[1].z, ev[1].y, ev[1].x);
    }
    eul.set(ev[0].z, ev[0].y, ev[0].x);

    return eul;
}


template <typename T> Vector<T>  RotMatrixElements2ExtEulerXZY(T m11, T m12, T m13, T m21, T m22, T m23, T m31, Vector<T>* vct=NULL)
{
    Vector<T> eul((T)0.0, (T)0.0, (T)0.0, (T)0.0, -(T)1.0);
    Vector<T> ev[2];
    T arctang;

    if (m21<-(T)1.0 || m21>(T)1.0) return eul;

    if ((T)1.0-m21<(T)Zero_Eps || (T)1.0+m21<(T)Zero_Eps) {    
        if ((T)1.0-m21<(T)Zero_Eps) {           // m21==1.0
            ev[0].z = ev[1].z = (T)PI_DIV2;
            ev[0].y = (T)0.0;
            ev[1].y = (T)PI_DIV2;
            arctang = (T)atan2(m13, -m12);
            ev[0].x = arctang - ev[0].y;
            ev[1].x = arctang - ev[1].y;
        }
        else {                                  // m21==-1.0
            ev[0].z = ev[1].y = -(T)PI_DIV2;
            ev[0].y = (T)0.0;
            ev[1].y = (T)PI_DIV2;
            arctang = (T)atan2(-m13, m12);
            ev[0].x = arctang + ev[0].y;
            ev[1].x = arctang + ev[1].y;
        }
    }

    else {
        ev[0].z = (T)asin(m21); 
        if (ev[0].z>=(T)0.0) ev[1].z =  (T)PI - ev[0].z;
        else                 ev[1].z = -(T)PI - ev[0].z;

        T cz1 = (T)cos(ev[0].z);
        T cz2 = (T)cos(ev[1].z);
        if (Xabs(cz1)<(T)Zero_Eps || Xabs(cz2)<(T)Zero_Eps) return eul;

        ev[0].x = (T)atan2(-m23/cz1, m22/cz1);
        ev[0].y = (T)atan2(-m31/cz1, m11/cz1);

        if (ev[0].x>=(T)0.0) ev[1].x = ev[0].x - (T)PI;
        else                 ev[1].x = ev[0].x + (T)PI;
        if (ev[0].y>=(T)0.0) ev[1].y = ev[0].y - (T)PI;
        else                 ev[1].y = ev[0].y + (T)PI;
    }

    //
    // XZY
    if (vct!=NULL) {
        vct[0].set(ev[0].x, ev[0].z, ev[0].y);
        vct[1].set(ev[1].x, ev[1].z, ev[1].y);
    }
    eul.set(ev[0].x, ev[0].z, ev[0].y);

    return eul;
}


template <typename T> Vector<T>  RotMatrixElements2ExtEulerYZX(T m11, T m12, T m13, T m21, T m22, T m31, T m32, Vector<T>* vct=NULL)
{
    Vector<T> eul((T)0.0, (T)0.0, (T)0.0, (T)0.0, -(T)1.0);
    Vector<T> ev[2];
    T arctang;

    if (m12<-(T)1.0 || m12>(T)1.0) return eul;

    if ((T)1.0-m12<(T)Zero_Eps || (T)1.0+m12<(T)Zero_Eps) {    
        if (1.0-m12<Zero_Eps) {                 // m12==1.0
            ev[0].z = ev[1].z = -(T)PI_DIV2;
            ev[0].y = (T)0.0;
            ev[1].y = (T)PI_DIV2;
            arctang = (T)atan2(-m31, -m21);
            ev[0].x = arctang - ev[0].y;
            ev[1].x = arctang - ev[1].y;
        }
        else {                                  // m12==-1.0
            ev[0].z = ev[1].y = (T)PI_DIV2;
            ev[0].y = (T)0.0;
            ev[1].y = (T)PI_DIV2;
            arctang = (T)atan2(m31, m21);
            ev[0].x = arctang + ev[0].y;
            ev[1].x = arctang + ev[1].y;
        }
    }

    else {
        ev[0].z = -(T)asin(m12); 
        if (ev[0].z>=0.0) ev[1].z =  (T)PI - ev[0].z;
        else              ev[1].z = -(T)PI - ev[0].z;

        T cz1 = (T)cos(ev[0].z);
        T cz2 = (T)cos(ev[1].z);
        if (Xabs(cz1)<(T)Zero_Eps || Xabs(cz2)<(T)Zero_Eps) return eul;

        ev[0].x = (T)atan2(m32/cz1, m22/cz1);
        ev[0].y = (T)atan2(m13/cz1, m11/cz1);

        if (ev[0].x>=(T)0.0) ev[1].x = ev[0].x - (T)PI;
        else                 ev[1].x = ev[0].x + (T)PI;
        if (ev[0].y>=(T)0.0) ev[1].y = ev[0].y - (T)PI;
        else                 ev[1].y = ev[0].y + (T)PI;
    }

    //
    // YZX
    if (vct!=NULL) {
        vct[0].set(ev[0].y, ev[0].z, ev[0].x);
        vct[1].set(ev[1].y, ev[1].z, ev[1].x);
    }
    eul.set(ev[0].y, ev[0].z, ev[0].x);

    return eul;
}


template <typename T> Vector<T>  RotMatrixElements2ExtEulerYXZ(T m12, T m21, T m22, T m23, T m31, T m32, T m33, Vector<T>* vct=NULL)
{
    Vector<T> eul((T)0.0, (T)0.0, (T)0.0, (T)0.0, -(T)1.0);
    Vector<T> ev[2];
    T arctang;

    if (m32<-(T)1.0 || m32>(T)1.0) return eul;

    if ((T)1.0-m32<(T)Zero_Eps || (T)1.0+m32<(T)Zero_Eps) {    
        if ((T)1.0-m32<(T)Zero_Eps) {           // m32==1.0
            ev[0].x = ev[1].x = (T)PI_DIV2;
            ev[0].z = (T)0.0;
            ev[1].z = (T)PI_DIV2;
            arctang = (T)atan2(m21, -m23);
            ev[0].y = arctang - ev[0].z;
            ev[1].y = arctang - ev[1].z;
        }
        else {                                  // m32==-1.0
            ev[0].x = ev[1].x = -(T)PI_DIV2;
            ev[0].z = (T)0.0;
            ev[1].z = (T)PI_DIV2;
            arctang = (T)atan2(-m21, m23);
            ev[0].y = arctang + ev[0].z;
            ev[1].y = arctang + ev[1].z;
        }
    }

    else {
        ev[0].x = (T)asin(m32); 
        if (ev[0].x>=0.0) ev[1].x =  (T)PI - ev[0].x;
        else              ev[1].x = -(T)PI - ev[0].x;

        T cx1 = (T)cos(ev[0].x);
        T cx2 = (T)cos(ev[1].x);
        if (Xabs(cx1)<(T)Zero_Eps || Xabs(cx2)<(T)Zero_Eps) return eul;

        ev[0].y = (T)atan2(-m31/cx1, m33/cx1);
        ev[0].z = (T)atan2(-m12/cx1, m22/cx1);

        if (ev[0].y>=(T)0.0) ev[1].y = ev[0].y - (T)PI;
        else                 ev[1].y = ev[0].y + (T)PI;
        if (ev[0].z>=(T)0.0) ev[1].z = ev[0].z - (T)PI;
        else                 ev[1].z = ev[0].z + (T)PI;
    }

    //
    // YXZ
    if (vct!=NULL) {
        vct[0].set(ev[0].y, ev[0].x, ev[0].z);
        vct[1].set(ev[1].y, ev[1].x, ev[1].z);
    }
    eul.set(ev[0].y, ev[0].x, ev[0].z);

    return eul;
}


template <typename T> Vector<T>  RotMatrixElements2ExtEulerZXY(T m12, T m13, T m21, T m22, T m23, T m32, T m33, Vector<T>* vct=NULL)
{
    Vector<T> eul((T)0.0, (T)0.0, (T)0.0, (T)0.0, -(T)1.0);
    Vector<T> ev[2];
    T arctang;

    if (m23<-(T)1.0 || m23>(T)1.0) return eul;

    if ((T)1.0-m23<(T)Zero_Eps || (T)1.0+m23<(T)Zero_Eps) {    
        if ((T)1.0-m23<(T)Zero_Eps) {           // m23==1.0
            ev[0].x = ev[1].x = -(T)PI_DIV2;
            ev[0].z = (T)0.0;
            ev[1].z = (T)PI_DIV2;
            arctang = (T)atan2(-m12, -m32);
            ev[0].y = arctang - ev[0].z;
            ev[1].y = arctang - ev[1].z;
        }
        else {                                  // m23==-1.0
            ev[0].x = ev[1].x = (T)PI_DIV2;
            ev[0].z = (T)0.0;
            ev[1].z = (T)PI_DIV2;
            arctang = (T)atan2(m12, m32);
            ev[0].y = arctang + ev[0].z;
            ev[1].y = arctang + ev[1].z;
        }
    }

    else {
        ev[0].x = -(T)asin(m23); 
        if (ev[0].x>=(T)0.0) ev[1].x =  (T)PI - ev[0].x;
        else                 ev[1].x = -(T)PI - ev[0].x;

        T cx1 = (T)cos(ev[0].x);
        T cx2 = (T)cos(ev[1].x);
        if (Xabs(cx1)<(T)Zero_Eps || Xabs(cx2)<(T)Zero_Eps) return eul;

        ev[0].y = (T)atan2(m13/cx1, m33/cx1);
        ev[0].z = (T)atan2(m21/cx1, m22/cx1);

        if (ev[0].y>=(T)0.0) ev[1].y = ev[0].y - (T)PI;
        else                 ev[1].y = ev[0].y + (T)PI;
        if (ev[0].z>=(T)0.0) ev[1].z = ev[0].z - (T)PI;
        else                 ev[1].z = ev[0].z + (T)PI;
    }

    //
    // ZXY
    if (vct!=NULL) {
        vct[0].set(ev[0].z, ev[0].x, ev[0].y);
        vct[1].set(ev[1].z, ev[1].x, ev[1].y);
    }
    eul.set(ev[0].z, ev[0].x, ev[0].y);

    return eul;
}


// Rotation Matrix -> ExtEuler
//
template <typename T> Vector<T>  RotMatrix2ExtEulerXYZ(Matrix<T> mtx, Vector<T>* vct=NULL)
{
    T m11 = mtx.element(1, 1);
    T m12 = mtx.element(1, 2);
    T m13 = mtx.element(1, 3);
    T m21 = mtx.element(2, 1);
    T m31 = mtx.element(3, 1);
    T m32 = mtx.element(3, 2);
    T m33 = mtx.element(3, 3);

    Vector<T> eul = RotMatrixElements2ExtEulerXYZ<T>(m11, m12, m13, m21, m31, m32, m33, vct);

    return eul;
}


//template <typename T> Vector<T>  RotMatrix2ExtEulerZYX(Matrix<T> mtx, Vector<T>* vct=NULL)
template <typename T> Vector<T>  RotMatrix2ExtEulerZYX(Matrix<T> mtx, Vector<T>* vct)
{
    T m11 = mtx.element(1, 1);
    T m12 = mtx.element(1, 2);
    T m13 = mtx.element(1, 3);
    T m21 = mtx.element(2, 1);
    T m23 = mtx.element(2, 3);
    T m31 = mtx.element(3, 1);
    T m33 = mtx.element(3, 3);

    Vector<T> eul = RotMatrixElements2ExtEulerZYX<T>(m11, m12, m13, m21, m23, m31, m33, vct);

    return eul;
}


template <typename T> Vector<T>  RotMatrix2ExtEulerXZY(Matrix<T> mtx, Vector<T>* vct=NULL)
{
    T m11 = mtx.element(1, 1);
    T m12 = mtx.element(1, 2);
    T m13 = mtx.element(1, 3);
    T m21 = mtx.element(2, 1);
    T m22 = mtx.element(2, 2);
    T m23 = mtx.element(2, 3);
    T m31 = mtx.element(3, 1);

    Vector<T> eul = RotMatrixElements2ExtEulerXZY<T>(m11, m12, m13, m21, m22, m23, m31, vct);

    return eul;
}


template <typename T> Vector<T>  RotMatrix2ExtEulerYZX(Matrix<T> mtx, Vector<T>* vct=NULL)
{
    T m11 = mtx.element(1, 1);
    T m12 = mtx.element(1, 2);
    T m13 = mtx.element(1, 3);
    T m21 = mtx.element(2, 1);
    T m22 = mtx.element(2, 2);
    T m31 = mtx.element(3, 1);
    T m32 = mtx.element(3, 2);

    Vector<T> eul = RotMatrixElements2ExtEulerYZX<T>(m11, m12, m13, m21, m22, m31, m32, vct);

    return eul;
}


template <typename T> Vector<T>  RotMatrix2ExtEulerYXZ(Matrix<T> mtx, Vector<T>* vct=NULL)
{
    T m12 = mtx.element(1, 2);
    T m21 = mtx.element(2, 1);
    T m22 = mtx.element(2, 2);
    T m23 = mtx.element(2, 3);
    T m31 = mtx.element(3, 1);
    T m32 = mtx.element(3, 2);
    T m33 = mtx.element(3, 3);

    Vector<T> eul = RotMatrixElements2ExtEulerYXZ<T>(m12, m21, m22, m23, m31, m32, m33, vct);

    return eul;
}


template <typename T> Vector<T>  RotMatrix2ExtEulerZXY(Matrix<T> mtx, Vector<T>* vct=NULL)
{
    T m12 = mtx.element(1, 2);
    T m13 = mtx.element(1, 3);
    T m21 = mtx.element(2, 1);
    T m22 = mtx.element(2, 2);
    T m23 = mtx.element(2, 3);
    T m32 = mtx.element(3, 2);
    T m33 = mtx.element(3, 3);

    Vector<T> eul = RotMatrixElements2ExtEulerZXY<T>(m12, m13, m21, m22, m23, m32, m33, vct);

    return eul;
}


// Quaternion -> ExtEuler
//
//template <typename T> Vector<T>  Quaternion2ExtEulerXYZ(Quaternion<T> qut, Vector<T>* vct=NULL)
template <typename T> Vector<T>  Quaternion2ExtEulerXYZ(Quaternion<T> qut, Vector<T>* vct)
{
    Matrix<T> mtx = qut.getRotMatrix();
    Vector<T> eul = RotMatrix2ExtEulerXYZ<T>(mtx, vct);
    mtx.free();

    return eul;
}


//template <typename T> Vector<T>  Quaternion2ExtEulerZYX(Quaternion<T> qut, Vector<T>* vct=NULL)
template <typename T> Vector<T>  Quaternion2ExtEulerZYX(Quaternion<T> qut, Vector<T>* vct)
{
    Matrix<T> mtx = qut.getRotMatrix();
    Vector<T> eul = RotMatrix2ExtEulerZYX(mtx, vct);
    mtx.free();

    return eul;
}


//template <typename T> Vector<T>  Quaternion2ExtEulerXZY(Quaternion<T> qut, Vector<T>* vct=NULL)
template <typename T> Vector<T>  Quaternion2ExtEulerXZY(Quaternion<T> qut, Vector<T>* vct)
{
    Matrix<T> mtx = qut.getRotMatrix();
    Vector<T> eul = RotMatrix2ExtEulerXZY(mtx, vct);
    mtx.free();

    return eul;
}


//template <typename T> Vector<T>  Quaternion2ExtEulerYZX(Quaternion<T> qut, Vector<T>* vct=NULL)
template <typename T> Vector<T>  Quaternion2ExtEulerYZX(Quaternion<T> qut, Vector<T>* vct)
{
    Matrix<T> mtx = qut.getRotMatrix();
    Vector<T> eul = RotMatrix2ExtEulerYZX(mtx, vct);
    mtx.free();

    return eul;
}


//template <typename T> Vector<T>  Quaternion2ExtEulerYXZ(Quaternion<T> qut, Vector<T>* vct=NULL)
template <typename T> Vector<T>  Quaternion2ExtEulerYXZ(Quaternion<T> qut, Vector<T>* vct)
{
    Matrix<T> mtx = qut.getRotMatrix();
    Vector<T> eul = RotMatrix2ExtEulerYXZ(mtx, vct);
    mtx.free();

    return eul;
}


//template <typename T> Vector<T>  Quaternion2ExtEulerZXY(Quaternion<T> qut, Vector<T>* vct=NULL)
template <typename T> Vector<T>  Quaternion2ExtEulerZXY(Quaternion<T> qut, Vector<T>* vct)
{
    Matrix<T> mtx = qut.getRotMatrix();
    Vector<T> eul = RotMatrix2ExtEulerZXY(mtx, vct);
    mtx.free();

    return eul;
}


// Rotation Matrix -> Quaternion
//
template <typename T> Quaternion<T>  RotMatrix2Quaternion(Matrix<T> mtx)
{
    Quaternion<T> qut;
    Vector<T> xyz = RotMatrix2ExtEulerXYZ(mtx);
    qut.setExtEulerXYZ(xyz);

    return qut;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ベクトルの回転，クォータニオン
//

/**
ベクトル回転の高速計算

@sa http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/
*/
template <typename T> Vector<T>  VectorRotation(Vector<T> v, Quaternion<T> q)
{
    Vector<T> vect;

    vect.x = (q.s*q.s + q.x*q.x - q.y*q.y -q.z*q.z)*v.x +
             ((T)2.0*q.x*q.y - (T)2.0*q.s*q.z)*v.y +
             ((T)2.0*q.x*q.z + (T)2.0*q.s*q.y)*v.z;

    vect.y = ((T)2.0*q.x*q.y + (T)2.0*q.s*q.z)*v.x +
             (q.s*q.s - q.x*q.x + q.y*q.y - q.z*q.z)*v.y +
             ((T)2.0*q.y*q.z - (T)2.0*q.s*q.x)*v.z;

    vect.z = ((T)2.0*q.x*q.z - (T)2.0*q.s*q.y)*v.x +
             ((T)2.0*q.y*q.z + (T)2.0*q.s*q.x)*v.y +
             (q.s*q.s - q.x*q.x - q.y*q.y + q.z*q.z)*v.z;

    return vect;
}


template <typename T> Vector<T>  VectorInvRotation(Vector<T> v, Quaternion<T> q)
{
    Vector<T> vect;

    vect.x = (q.s*q.s + q.x*q.x - q.y*q.y -q.z*q.z)*v.x +
             ((T)2.0*q.x*q.y + (T)2.0*q.s*q.z)*v.y +
             ((T)2.0*q.x*q.z - (T)2.0*q.s*q.y)*v.z;

    vect.y = ((T)2.0*q.x*q.y - (T)2.0*q.s*q.z)*v.x +
             (q.s*q.s - q.x*q.x + q.y*q.y - q.z*q.z)*v.y +
             ((T)2.0*q.y*q.z + (T)2.0*q.s*q.x)*v.z;

    vect.z = ((T)2.0*q.x*q.z + (T)2.0*q.s*q.y)*v.x +
             ((T)2.0*q.y*q.z - (T)2.0*q.s*q.x)*v.y +
             (q.s*q.s - q.x*q.x - q.y*q.y + q.z*q.z)*v.z;

    return vect;
}


template <typename T> T*  VectorRotation(T* v, Quaternion<T> q)
{
    T x, y, z;

    x = (q.s*q.s + q.x*q.x - q.y*q.y -q.z*q.z)*v[0] +
        ((T)2.0*q.x*q.y - (T)2.0*q.s*q.z)*v[1] +
        ((T)2.0*q.x*q.z + (T)2.0*q.s*q.y)*v[2];

    y = ((T)2.0*q.x*q.y + (T)2.0*q.s*q.z)*v[0] +
        (q.s*q.s - q.x*q.x + q.y*q.y - q.z*q.z)*v[1] +
        ((T)2.0*q.y*q.z - (T)2.0*q.s*q.x)*v[2];

    z = ((T)2.0*q.x*q.z - (T)2.0*q.s*q.y)*v[0] +
        ((T)2.0*q.y*q.z + (T)2.0*q.s*q.x)*v[1] +
        (q.s*q.s - q.x*q.x - q.y*q.y + q.z*q.z)*v[2];

    v[0] = x;
    v[1] = y;
    v[2] = z;

    return v;
}


template <typename T> T*  VectorInvRotation(T* v, Quaternion<T> q)
{
    T x, y, z;

    x = (q.s*q.s + q.x*q.x - q.y*q.y -q.z*q.z)*v[0] +
        ((T)2.0*q.x*q.y + (T)2.0*q.s*q.z)*v[1] +
        ((T)2.0*q.x*q.z - (T)2.0*q.s*q.y)*v[2];

    y = ((T)2.0*q.x*q.y - (T)2.0*q.s*q.z)*v[0] +
        (q.s*q.s - q.x*q.x + q.y*q.y - q.z*q.z)*v[1] +
        ((T)2.0*q.y*q.z + (T)2.0*q.s*q.x)*v[2];

    z = ((T)2.0*q.x*q.z + (T)2.0*q.s*q.y)*v[0] +
        ((T)2.0*q.y*q.z - (T)2.0*q.s*q.x)*v[1] +
        (q.s*q.s - q.x*q.x - q.y*q.y + q.z*q.z)*v[2];

    v[0] = x;
    v[1] = y;
    v[2] = z;

    return v;
}


template <typename T> Quaternion<T>  V2VQuaternion(Vector<T> a, Vector<T> b)
{
    Quaternion<T> qut((T)1.0, (T)0.0, (T)0.0, (T)0.0, (T)1.0);

    a.normalize();
    b.normalize();
    if (a.n<(T)Zero_Eps || b.n<(T)Zero_Eps) return qut;
    
    Vector<T> nr = a^b;
    nr.normalize();

    T cs2 = (a*b)/(T)2.0;            // θ/2 = 0〜π/2
    
    T sn;
    if (cs2>(T)0.5) sn = (T)0.0;
    else            sn = (T)sqrt(0.5 - cs2);

    if (Xabs(sn-(T)1.0)<(T)Zero_Eps) {
        Vector<T> c = a + b;
        if (c.norm()<(T)1.0) {
            qut.setRotation(PI,  nr);
            if (nr.n<Zero_Eps) {
                qut.s = (T)0.0;
                qut.n = (T)0.0;
                qut.c = -(T)1.0;
            }
        }
        else {
            qut.setRotation((T)0.0, nr);
        }
        return qut;
    }

    T cs;
    if (cs2<-(T)0.5) cs = (T)0.0;
    else             cs = (T)sqrt(0.5 + cs2);

    //
    if (cs>(T)1.0) qut.set((T)1.0, (T)0.0, (T)0.0, (T)0.0, (T)1.0, -(T)1.0);
    else           qut.set(cs, nr.x*sn, nr.y*sn, nr.z*sn, (T)1.0, (T)Min(a.c, b.c));
    
    return qut;
}


template <typename T> Quaternion<T>  PPPQuaternion(Vector<T> a, Vector<T> b, Vector<T> c)
{
    Quaternion<T> qut((T)1.0, (T)0.0, (T)0.0, (T)0.0, (T)1.0);

    if (a.c<(T)0.0 || b.c<(T)0.0 || c.c<(T)0.0) return qut;

    qut = V2VQuaternion<T>(b-a, c-b);
    return qut;
}


template <typename T> Quaternion<T>  VPPQuaternion(Vector<T> a, Vector<T> b, Vector<T> c)
{
    Quaternion<T> qut((T)1.0, (T)0.0, (T)0.0, (T)0.0, (T)1.0);

    if (a.c<(T)0.0 || b.c<(T)0.0 || c.c<(T)0.0) return qut;

    qut = V2VQuaternion<T>(a, c-b);
    return qut;
}


template <typename T> Quaternion<T>  PPVQuaternion(Vector<T> a, Vector<T> b, Vector<T> c)
{
    Quaternion<T> qut((T)1.0, (T)0.0, (T)0.0, (T)0.0, (T)1.0);

    if (a.c<(T)0.0 || b.c<(T)0.0 || c.c<(T)0.0) return qut;

    qut = V2VQuaternion<T>(b-a, c);
    return qut;
}


/**
Quaternion<T>  SlerpQuaternion(Quaternion<T> qa, Quaternion<T> qb, T t)

クオータニオンの球面線形補間 (Sphercal Linear Interpolation)@n

回転軸が変化しない，または180度反転した状態での 180度以上の回転に対応．

@param   qa  始点のクオータニオン
@param   qb  終点のクオータニオン
@param   t   パラメータ 0〜1

@return  補間結果のクオータニオン

@par SLERP
q(t) = qa*sin((1-t)*th)/sin(th) + qb*sin(t*th)/sin(th)
*/
template <typename T> Quaternion<T>  SlerpQuaternion(Quaternion<T> qa, Quaternion<T> qb, T t)
{
    if (qa.n!=(T)1.0) qa.normalize();
    if (qb.n!=(T)1.0) qb.normalize();
    
    //
    T             dot;
    Quaternion<T> qc;

    ///////////////////////////////////////////
    //
    Vector<T> va = qa.getVector();
    Vector<T> vb = qb.getVector();
    va.normalize();
    vb.normalize();

    //
    dot = va*vb;
    if (dot>(T)1.0)       dot =  1.0;
    else if (dot<-(T)1.0) dot = -1.0;

    // 回転軸が変わらない場合
    if (dot>(T)1.0-Sin_Tolerance) {
        T anga = qa.getMathAngle();
        T angb = qb.getMathAngle();
        T angd = angb - anga;
        if (angd<-(T)PI)     angd += (T)PI2; 
        else if (angd>(T)PI) angd -= (T)PI2;

        T angc = anga + t*angd;
        qc.setRotation(angc, va);
        qc.normalize();
        return qc;
    }

    // 回転軸が反転する場合
//    else if (dot<-(T)1.0+(T)Sin_Tolerance) {
    else if (dot<-(T)0.99) {
        T anga = qa.getMathAngle();
        T angb = - qb.getMathAngle();
        T angd = angb - anga;
        if (angd<-(T)PI)     angd += (T)PI2; 
        else if (angd>(T)PI) angd -= (T)PI2;

        T angc = anga + t*angd;
        qc.setRotation(angc, va);
        qc.normalize();
        return qc;
    }

    // SLERP
    dot = qa.x*qb.x + qa.y*qb.y + qa.z*qb.z + qa.s*qb.s;
    if (dot>(T)1.0) dot = (T)1.0;
    else if (dot<(T)0.0) {
        if (t<=(T)0.5) return qa;
        else           return qb;
    }
    //if (dot<0.0) DEBUG_WARN("WARNING: SlerpQuaternion: dot = %f", dot);

    //
    T th = (T)acos(dot);
    T sn = (T)sin(th);
    if (Xabs(sn)<(T)Sin_Tolerance) return qa;

    //
    qc = qa*((T)sin(((T)1.0-t)*th)/sn) + qb*((T)sin(t*th)/sn);
    qc.normalize();

    return qc;
}


}        // namespace


#endif 

