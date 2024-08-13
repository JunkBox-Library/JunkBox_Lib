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
private:
    Matrix<T>        matrix;        // Matrix(2, 4, 4); 2次元 4x4行列 

    Vector<T>        shift;
    Vector<T>        scale;
    Quaternion<T>    rotate;

private:
    bool             _changed_matrix;
    bool             _changed_components;

public:
    AffineTrans(void) { init();}
    virtual ~AffineTrans(void) {}

    void   init(void) { initComponents(); matrix = Matrix<T>(2, 4, 4); _changed_matrix = false; computeMatrix();}
    void   setup(void){ init();}
    void   initComponents(void) { initScale(); initRotation(); initShift();}

    void   set(Vector<T> s, Quaternion<T> q, Vector<T> t) { scale = s; shift = t, rotate = q; computeMatrix();}
    void   free(void) { initComponents(); matrix.free();  _changed_matrix = false; computeMatrix();}
    void   clear(void){ initComponents(); matrix.clear(); _changed_matrix = false; computeMatrix();}
    void   dup(AffineTrans a);
    AffineTrans<T>  dup(void);

    void   clean_matrix(void)           { _changed_matrix = false;}
    void   clean_components(void)       { _changed_components = false;}
    void   changed_matrix(void)         { _changed_matrix = true;}
    void   changed_components(void)     { _changed_components = true;}
    bool   is_changed_matrix(void)      { return _changed_matrix;}        // true なら computeComponents() を実行する（推奨）
    bool   is_changed_components(void)  { return _changed_components;}    // true なら computeMatrix() を実行する（推奨）
    bool   need_compute_matrix(void)    { return ( _changed_components && !_changed_matrix);}
    bool   need_compute_components(void){ return (!_changed_components &&  _changed_matrix);}

    void   computeMatrix(bool with_scale = true);
    void   computeComponents(void);

    void   initShift(void) { shift.init(); _changed_components = true;}
    void   initScale(void) { scale.set((T)1.0, (T)1.0, (T)1.0); _changed_components = true;}
    void   initRotation(void) { rotate.init(); _changed_components = true;}

    void   setShift(T x, T y, T z) { shift.set(x, y, z); _changed_components = true;}
    void   setScale(T x, T y, T z) { scale.set(x, y, z); _changed_components = true;}
    void   setRotation(T s, T x, T y, T z) { rotate.set(s, x, y, z); _changed_components = true;}

    void   setShift(Vector<T> v) { shift = v; _changed_components = true;}
    void   setScale(Vector<T> v) { scale = v; _changed_components = true;}
    void   setRotation(Quaternion<T> q) { rotate = q; _changed_components = true;}

    void   addShift(T x, T y, T z) { shift.x += x; shift.y += y; shift.z += z; _changed_components = true;}
    void   addScale(T x, T y, T z) { scale.x *= x; scale.y *= y; scale.z *= z; _changed_components = true;}
    void   addRotation(T s, T x, T y, T z) { Quaternion<T> q(s, x, y, z); rotate = q*rotate; _changed_components = true;}

    void   addShift(Vector<T> v) { shift = shift + v; _changed_components = true;}
    void   addScale(Vector<T> v) { scale = shift + v; _changed_components = true;}
    void   addRotation(Quaternion<T> q) { rotate = q*rotate; _changed_components = true;}

    void   setMatrix(int i, int j, T v) { matrix.element(i, j) = v; _changed_matrix = true;}
    T      getMatrix(int i, int j) { return matrix.element(i, j);}
    void   element(int i, int j, T v) { matrix.element(i, j) = v; _changed_matrix = true;}
    T      element(int i, int j) { return matrix.element(i, j);}

    T      getShiftX(void)   { return shift.x;}
    T      getShiftY(void)   { return shift.y;}
    T      getShiftZ(void)   { return shift.z;}
    T      getScaleX(void)   { return scale.x;}
    T      getScaleY(void)   { return scale.y;}
    T      getScaleZ(void)   { return scale.z;}
    T      getRotationS(void){ return rotate.s;}
    T      getRotationX(void){ return rotate.x;}
    T      getRotationY(void){ return rotate.y;}
    T      getRotationZ(void){ return rotate.z;}

    Vector<T>       getScale (void)   { return scale;}
    Quaternion<T>   getRotation(void) { return rotate;}
    Vector<T>       getShift (void)   { return shift;}
    Matrix<T>       getMatrix(void)   { return matrix;}
    Matrix<T>       getRotationMatrix(void);
    AffineTrans<T>  getInverseAffine(void);

    // operator * は コンポーネントが計算されていることが条件．
    // 下記関数は コンポ―テントが計算されていなくても良い．
    void   affineMatrixAfter (AffineTrans<T> a);  // *this = (*this) * a を Matrix のままで計算する．
    void   affineMatrixBefore(AffineTrans<T> a);  // *this = a * (*this) を Matrix のままで計算する．

    bool   isSetComponents(void) { if(isSetShift() || isSetScale() || isSetRotation()) return true; else return false;}
    bool   isSetShift(void){ return (shift !=Vector<T>());}
    bool   isSetScale(void){ return (scale !=Vector<T>((T)1.0, (T)1.0, (T)1.0));}
    bool   isSetRotation(void) { return (rotate!=Quaternion<T>());}
    bool   isNormal(void){ if(scale.x>=JBXL_EPS && scale.y>=JBXL_EPS && scale.z>=JBXL_EPS) return true; else return false;}

    Vector<T> execMatrixTrans(Vector<T> v);

    Vector<T> execTrans(Vector<T> v)    { return execShift(execRotation(execScale(v)));}
    Vector<T> execInvTrans(Vector<T> v) { return execInvScale(execInvRotation(execInvShift(v)));}

    Vector<T> execRotationScale(Vector<T> v)    { return execRotation(execScale(v));} 
    Vector<T> execInvRotationScale(Vector<T> v) { return execInvScale(execInvRotation(v));}
    //
    Vector<T> execShift(Vector<T> v)      { return Vector<T>(v.x+shift.x, v.y+shift.y, v.z+shift.z, (T)0.0, Min(v.c, shift.c));}
    Vector<T> execInvShift(Vector<T> v)   { return Vector<T>(v.x-shift.x, v.y-shift.y, v.z-shift.z, (T)0.0, Min(v.c, shift.c));}
    Vector<T> execScale(Vector<T> v)      { return Vector<T>(v.x*scale.x, v.y*scale.y, v.z*scale.z, (T)0.0, Min(v.c, scale.c));}
    Vector<T> execInvScale(Vector<T> v)   { return Vector<T>(v.x/scale.x, v.y/scale.y, v.z/scale.z, (T)0.0, Min(v.c, scale.c));}
    Vector<T> execRotation(Vector<T> v)   { return VectorRotation(v, rotate);}
    Vector<T> execInvRotation(Vector<T> v){ return VectorInvRotation(v, rotate);}

    // for debug
    void   printMatrix(void);
    void   printComponents(void);
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

*/
template <typename T> inline AffineTrans<T> operator * (AffineTrans<T> a, AffineTrans<T> b)
{ 
    a.computeMatrix();
    b.computeMatrix();

    AffineTrans<T> affine;
    for (int i=1; i<=4; i++) {
        for (int j=1; j<=4; j++) {
            T sum = (T)0.0;
            for (int k=1; k<=4; k++) {
                sum += a.element(i, k) * b.element(k, j);
            }
            affine.element(i, j, sum);
        }
    }
    affine.computeComponents();

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
// Matrix が計算されていること．
template <typename T> Matrix<T>  AffineTrans<T>::getRotationMatrix(void)
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
template <typename T> AffineTrans<T>  AffineTrans<T>::getInverseAffine(void)
{
    AffineTrans<T> affine;
    if (!isNormal()) return affine;
    computeComponents();

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
    affine.computeComponents();

    rst.free();

    return affine;
}


/**
template <typename T> void  AffineTrans<T>::affineMatrixAfter(AffineTrans<T> a)

*this = (*this) * a を Matrix のままで計算する．
*/
template <typename T> void  AffineTrans<T>::affineMatrixAfter(AffineTrans<T> a)
{
    AffineTrans<T> affine;
    for (int i=1; i<=4; i++) {
        for (int j=1; j<=4; j++) {
            T sum = (T)0;
            for (int k=1; k<=4; k++) {
                sum += matrix.element(i, k) * a.element(k, j);
            }
            affine.element(i, j, sum);
        }
    }
    for (int i=1; i<=4; i++) {
        for (int j=1; j<=4; j++) {
            matrix.element(i, j) = affine.element(i, j);
        }
    }
    affine.free();
    computeComponents();
    return;
}


/**
template <typename T> void  AffineTrans<T>::affineMatrixBefore(AffineTrans<T> a)

*this = a * (*this) を Matrix のままで計算する．
*/
template <typename T> void  AffineTrans<T>::affineMatrixBefore(AffineTrans<T> a)
{
    AffineTrans<T> affine;
    for (int i=1; i<=4; i++) {
        for (int j=1; j<=4; j++) {
            T sum = (T)0;
            for (int k=1; k<=4; k++) {
                sum += a.element(i, k) * matrix.element(k, j);
            }
            affine.element(i, j, sum);
        }
    }
    for (int i=1; i<=4; i++) {
        for (int j=1; j<=4; j++) {
            matrix.element(i, j) = affine.element(i, j);
        }
    }
    affine.free();
    computeComponents();
    return;
}


// 各コンポーネントから，変換行列を計算する．
template <typename T> void   AffineTrans<T>::computeMatrix(bool with_scale)
{
    if (_changed_matrix) {
        PRINT_MESG("AffineTrans<T>::computeMatrix: ERROR: may destroy Matrix!\n");
    }

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

    _changed_matrix     = false;
    _changed_components = false;
    return;
}


// 変換行列から，各コンポーネントを再計算する．
template <typename T> void   AffineTrans<T>::computeComponents(void)
{
    if (_changed_components) {
        PRINT_MESG("AffineTrans<T>::computeComponents: ERROR: may destroy Components!\n");
    }

    T sx, sy, sz; 
    sx = sy = sz = (T)0.0;
    for (int i=1; i<=3; i++) {
        sx += matrix.element(i, 1)*matrix.element(i, 1);
        sy += matrix.element(i, 2)*matrix.element(i, 2);
        sz += matrix.element(i, 3)*matrix.element(i, 3);
    }
    sx = (T)sqrt(sx); 
    sy = (T)sqrt(sy); 
    sz = (T)sqrt(sz); 
    if (!isNormal()) return;
    scale.set(sx, sy, sz);

    //
    Matrix<T> mt = getRotationMatrix();

    for (int i=1; i<=3; i++) {
        mt.element(i, 1) /= sx;
        mt.element(i, 2) /= sy;
        mt.element(i, 3) /= sz;
    }
    rotate = RotMatrix2Quaternion<T>(mt);    
    mt.free();
    //
    shift.set(matrix.element(1,4), matrix.element(2,4), matrix.element(3,4));

    _changed_matrix     = false;
    _changed_components = false;
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

    Matrix<T> mt = matrix * mv;
    Vector<T> vct;
    vct.x = mt.mx[0];
    vct.y = mt.mx[1];
    vct.z = mt.mx[2];

    mt.free();
    mv.free();
    
    return vct;
}


template <typename T> void  AffineTrans<T>::printMatrix(void)
{
    for(int j=1; j<=4; j++) {
        for(int i=1; i<=4; i++) {
            T element = matrix.element(i, j);
            print_message(" %g ", element);
        }
        print_message("\n");
    }
    return;
}


template <typename T> void  AffineTrans<T>::printComponents(void)
{
    print_message("shift = (%g, %g, %g)\n", shift.x, shift.y, shift.z);
    print_message("scale = (%g, %g, %g)\n", scale.x, scale.y, scale.z);
    print_message("rot   = (%g, %g, %g, %g)\n", rotate.s, rotate.x, rotate.y, rotate.z);
}



}        // namespace


#endif 

