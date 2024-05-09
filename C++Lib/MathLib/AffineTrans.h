#ifndef  __JBXL_CPP_AFFINETRANS_H_
#define  __JBXL_CPP_AFFINETRANS_H_

/**
@brief    Affine変換 ライブラリ ヘッダ
@file     AffineTrans.h
@author   Fumi.Iseki (C)
*/

#include  "Rotation.h"


//
namespace  jbxl {


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Affine Transfer
//

/**
アフィン変換
    拡大縮小，回転，平行移動（のみ）

変換の合成：A*B => Bの変換 -> Aの変換
*/
template <typename T=double> class  DllExport AffineTrans
{
public:
    Matrix<T>        matrix;        // Matrix(2, 4, 4); 2次元 4x4行列 

    Vector<T>        shift;
    Vector<T>        scale;
    Quaternion<T>    rotate;

    bool             isInverse;

public:
    AffineTrans(void) { init();}
    virtual ~AffineTrans(void) {}

    void   init(void) { initComponents(); matrix = Matrix<T>(2, 4, 4);}
    void   setup(void){ init();}
    void   initComponents(void) { initScale(); initRotate(); initShift(); isInverse = false;}

    void   set(Vector<T> s, Quaternion<T> q, Vector<T> t, bool inv=false) { scale=s; shift=t, rotate=q; isInverse=inv; computeMatrix(true);}
    void   free(void) { initComponents(); matrix.free();}
    void   clear(void){ initComponents(); matrix.clear();}
    void   dup(AffineTrans a);
    AffineTrans<T>  dup(void);

    void   computeMatrix(bool with_scale=true);
    void   computeComponents(void);

    void   initShift (void) { shift.init();}
    void   initScale (void) { scale.set((T)1.0, (T)1.0, (T)1.0);}
    void   initRotate(void) { rotate.init();}

    void   setShift (T x, T y, T z) { shift.set(x, y, z);}
    void   setScale (T x, T y, T z) { scale.set(x, y, z);}
    void   setRotate(T s, T x, T y, T z) { rotate.setRotation(s, x, y, z);}

    void   setShift (Vector<T> v) { shift = v;}
    void   setScale (Vector<T> v) { scale = v;}
    void   setRotate(Quaternion<T> q) { rotate = q;}

    void   addShift (T x, T y, T z) { shift.x += x; shift.y += y; shift.z += z;}
    void   addScale (T x, T y, T z) { scale.x *= x; scale.y *= y; scale.z *= z;}
    void   addRotate(T s, T x, T y, T z) { Quaternion<T> q(s, x, y, z); rotate = q*rotate;}

    void   addShift (Vector<T> v) { shift = shift + v;}
    void   addScale (Vector<T> v) { scale = shift + v;}
    void   addRotate(Quaternion<T> q) { rotate = q*rotate;}

    Vector<T>        getShift (void) { return shift;}
    Vector<T>        getScale (void) { return scale;}
    Quaternion<T>    getRotate(void) { return rotate;}
    Matrix<T>        getMatrix(void) { return matrix;}
    Matrix<T>        getRotMatrix(void);
    AffineTrans<T>   getInvAffine(void);

    bool   isSetComponents(void) { if(isSetShift() || isSetScale() || isSetRotate()) return true; else return false;}
    bool   isSetShift(void)  { return (shift !=Vector<T>());}
    bool   isSetScale(void)  { return (scale !=Vector<T>((T)1.0, (T)1.0, (T)1.0));}
    bool   isSetRotate(void) { return (rotate!=Quaternion<T>());}
    bool   isNormal(void)    { if(scale.x>=JBXL_EPS && scale.y>=JBXL_EPS && scale.z>=JBXL_EPS) return true; else return false;}

    Vector<T> execMatrixTrans(Vector<T> v);

    Vector<T> execTrans(Vector<T> v)    { if(!isInverse) return execShift(execRotate(execScale(v))); 
                                          else           return execScale(execRotate(execShift(v)));}
    Vector<T> execInvTrans(Vector<T> v) { if(!isInverse) return execInvScale(execInvRotate(execInvShift(v)));
                                          else           return execInvShift(execInvRotate(execInvScale(v)));}

    Vector<T> execRotateScale(Vector<T> v)    { if(!isInverse) return execRotate(execScale(v)); 
                                                else           return execScale(execRotate(v));}
    Vector<T> execInvRotateScale(Vector<T> v) { if(!isInverse) return execInvScale(execInvRotate(v));
                                                else           return execInvRotate(execInvScale(v));}
    //
    Vector<T> execShift(Vector<T> v)    { return Vector<T>(v.x+shift.x, v.y+shift.y, v.z+shift.z, (T)0.0, Min(v.c, shift.c));}
    Vector<T> execInvShift(Vector<T> v) { return Vector<T>(v.x-shift.x, v.y-shift.y, v.z-shift.z, (T)0.0, Min(v.c, shift.c));}
    Vector<T> execScale(Vector<T> v)    { return Vector<T>(v.x*scale.x, v.y*scale.y, v.z*scale.z, (T)0.0, Min(v.c, scale.c));}
    Vector<T> execInvScale(Vector<T> v) { return Vector<T>(v.x/scale.x, v.y/scale.y, v.z/scale.z, (T)0.0, Min(v.c, scale.c));}
    Vector<T> execRotate(Vector<T> v)   { return VectorRotation(v, rotate);}
    Vector<T> execInvRotate(Vector<T> v){ return VectorInvRotation(v, rotate);}

    //
    T* execTrans(T* v)    { if(!isInverse) return execShift(execRotate(execScale(v))); 
                            else           return execScale(execRotate(execShift(v)));}
    T* execInvTrans(T* v) { if(!isInverse) return execInvScale(execInvRotate(execInvShift(v)));
                            else           return execInvShift(execInvRotate(execInvScale(v)));}

    T* execRotateScale(T* v)    { if(!isInverse) return execRotate(execScale(v)); 
                                  else           return execScale(execRotate(v));}
    T* execInvRotateScale(T* v) { if(!isInverse) return execInvScale(execInvRotate(v));
                                  else           return execInvRotate(execInvScale(v));}

    T*  execShift(T* v)    { v[0]+=shift.x; v[1]+=shift.y; v[2]+=shift.z; return v;}
    T*  execInvShift(T* v) { v[0]-=shift.x; v[1]-=shift.y; v[2]-=shift.z; return v;}
    T*  execScale(T* v)    { v[0]*=scale.x; v[1]*=scale.y; v[2]*=scale.z; return v;}
    T*  execInvScale(T* v) { v[0]/=scale.x; v[1]/=scale.y; v[2]/=scale.z; return v;}
    T*  execRotate(T* v)   { return VectorRotation(v, rotate);}
    T*  execInvRotate(T* v){ return VectorInvRotation(v, rotate);}
};


template <typename T> inline void  freeAffineTrans(AffineTrans<T>*& affine)
{
    if (affine!=NULL){
        affine->free();
        delete(affine);
        affine = NULL;
    }
}


template <typename T> inline AffineTrans<T>*  newAffineTrans(AffineTrans<T> p)
{
    AffineTrans<T>* a = new AffineTrans<T>();
    a->dup(p);
    return a;
}


/**
変換の合成：A*B => Bの変換 -> Aの変換

a, b ともにコンポーネントが計算されている事が条件．
*/
template <typename T> inline AffineTrans<T> operator * (const AffineTrans<T> a, const AffineTrans<T> b)
{ 
    Quaternion<T> rotate = a.rotate * b.rotate;
    Vector<T> shift = a.shift + b.shift;
    //
    Vector<T> scale;
    scale.set(a.scale.x*b.scale.x, a.scale.y*b.scale.y, a.scale.z*b.scale.z, (T)0.0, Min(a.scale.c, b.scale.c));
    
    AffineTrans<T> affine;
    affine.set(scale, rotate, shift, a.isInverse);
    affine.computeMatrix(true);

    return affine;
}



////////////////////////////////////////////////////////////////////////////////
// Affine Transfer
//

// AffineTrans のコピーを作る．マトリックスデータのメモリ部は共有しない．
template <typename T> void  AffineTrans<T>::dup(AffineTrans<T> a)
{
    *this = a;
    matrix.dup(a.matrix);

    return;
}


// 自分自身の コピーを作って返す．
template <typename T>  AffineTrans<T> AffineTrans<T>::dup(void)
{
    AffineTrans<T> affine;
    affine.dup(*this);

    return affine;
}


// 3x3 の回転行列を得る
template <typename T> Matrix<T>  AffineTrans<T>::getRotMatrix(void)
{
    Matrix<T> mt;

    if (matrix.element(4, 4)==(T)1.0) {
        mt.init(2, 3, 3);
        for (int j=1; j<=3; j++) { 
            for (int i=1; i<=3; i++) { 
                mt.element(i, j) = matrix.element(i, j);
            }
        }
    }
    else {
        mt = rotate.getRotMatrix();
    }
    return mt;
}


// 各コンポーテントの成分から，逆変換を定義する．
template <typename T> AffineTrans<T>  AffineTrans<T>::getInvAffine(void)
{
    AffineTrans<T> affine;
    if (!isNormal()) return affine;

    Matrix<T> rsz(2, 3, 3);
    rsz.element(1, 1) = (T)1.0/scale.x;
    rsz.element(2, 2) = (T)1.0/scale.y;
    rsz.element(3, 3) = (T)1.0/scale.z;

    Matrix<T> rmt = rsz*(~rotate).getRotMatrix();
    
    affine.matrix.element(4, 4) = (T)1.0;
    for (int j=1; j<=3; j++) { 
        for (int i=1; i<=3; i++) { 
            affine.matrix.element(i, j) = rmt.element(i, j);
        }
    }
    rsz.free();
    rmt.free();

    Matrix<T> rst(2, 4, 4);
    rst.element(1, 1) = rst.element(2,2) = rst.element(3,3) = rst.element(4,4) = (T)1.0;
    rst.element(1, 4) = -shift.x;
    rst.element(2, 4) = -shift.y;
    rst.element(3, 4) = -shift.z;

    affine.matrix = affine.matrix*rst;
    affine.isInverse = !isInverse;
    affine.computeComponents();

    rst.free();

    return affine;
}


// 各コンポーネントから，変換行列を計算する．
template <typename T> void   AffineTrans<T>::computeMatrix(bool with_scale)
{
    Matrix<T> sz(2, 3, 3);
    if (with_scale) {
        sz.element(1, 1) = scale.x;
        sz.element(2, 2) = scale.y;
        sz.element(3, 3) = scale.z;
    }
    else {
        sz.element(1, 1) = (T)1.0;
        sz.element(2, 2) = (T)1.0;
        sz.element(3, 3) = (T)1.0;
    }
    
    matrix.clear();
    Matrix<T> mt = rotate.getRotMatrix()*sz;
    for (int j=1; j<=3; j++) { 
        for (int i=1; i<=3; i++) { 
            matrix.element(i, j) = mt.element(i, j);
        }
    }
    sz.free();
    mt.free();

    matrix.element(1, 4) = shift.x;
    matrix.element(2, 4) = shift.y;
    matrix.element(3, 4) = shift.z;
    matrix.element(4, 4) = (T)1.0;
    matrix.element(4, 1) = (T)0.0;
    matrix.element(4, 2) = (T)0.0;
    matrix.element(4, 3) = (T)0.0;

    return;
}


// 変換行列から，各コンポーネントを再計算する．
template <typename T> void   AffineTrans<T>::computeComponents(void)
{
    T sx, sy, sz; 
    sx = sy = sz = (T)0.0;
    if (!isInverse) {
        for (int i=1; i<=3; i++) {
            sx += matrix.element(i, 1)*matrix.element(i, 1);
            sy += matrix.element(i, 2)*matrix.element(i, 2);
            sz += matrix.element(i, 3)*matrix.element(i, 3);
        }
    }
    else {
        for (int i=1; i<=3; i++) {
            sx += matrix.element(1, i)*matrix.element(1, i);
            sy += matrix.element(2, i)*matrix.element(2, i);
            sz += matrix.element(3, i)*matrix.element(3, i);
        }
    }
    sx = (T)sqrt(sx); 
    sy = (T)sqrt(sy); 
    sz = (T)sqrt(sz); 
    if (!isNormal()) return;

    scale.set(sx, sy, sz);

    //
    Matrix<T> mt = getRotMatrix();
    if (!isInverse) {
        for (int i=1; i<=3; i++) {
            mt.element(i, 1) /= sx;
            mt.element(i, 2) /= sy;
            mt.element(i, 3) /= sz;
        }
    }
    else {
        for (int i=1; i<=3; i++) {
            mt.element(1, i) /= sx;
            mt.element(2, i) /= sy;
            mt.element(3, i) /= sz;
        }
    }
    rotate = RotMatrix2Quaternion<T>(mt);    
    mt.free();

    //
    if (!isInverse) {
        shift.set(matrix.element(1,4), matrix.element(2,4), matrix.element(3,4));
    }
    else {
        Vector<T> sh(matrix.element(1,4)/sx, matrix.element(2,4)/sy, matrix.element(3,4)/sz);
        shift = rotate.execInvRotate(sh);
    }

    return;
}


// 変換行列を用いて，ベクトルを変換する．
template <typename T> Vector<T>  AffineTrans<T>::execMatrixTrans(Vector<T> v)
{
    //if (matrix.element(4,4)!=(T)1.0) computeMatrix(true);
    computeMatrix(true);

    Matrix<T> mv(1, 4);
    mv.mx[0] = v.x;
    mv.mx[1] = v.y;
    mv.mx[2] = v.z;
    mv.mx[3] = (T)1.0;

    Matrix<T> mt = matrix*mv;
    Vector<T> vct;
    vct.x = mt.mx[0];
    vct.y = mt.mx[1];
    vct.z = mt.mx[2];

    mt.free();
    mv.free();
    
    return vct;
}


}        // namespace


#endif 

