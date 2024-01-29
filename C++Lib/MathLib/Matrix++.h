#ifndef  __JBXL_CPP_MATRIX_H_
#define  __JBXL_CPP_MATRIX_H_

/**
@brief    マトリックス ライブラリヘッダ for C++ 
@file     Matrix++.h
@author   Fumi.Iseki (C)
*/


#include "tools++.h"
#include <math.h>
#include "Vector.h"


//
namespace jbxl {


/**
template <typename T=double> class  Matrix

@par 多次元マトリックス型  Matrix 
- 注: Matrix型の引数は (行, 列, ....)となるので，直接メモリにアクセスする場合は注意が必要である．
- 2次元(3x3)の場合...., (1,1), (1,2), (1,3), (2,1), (2,2), (2,3), (3,1), ....
- 3次元(2x2x2)の場合... (1,1,1), (1,1,2), (1,2,1), (1,2,2), (2,1,1), ....
*/
template <typename T=double> class  Matrix
{
public:
    int  n;             ///< 次元数
    int  r;             ///< 全要素数 sz[0]*sz[1]*...*sz[n-1]
    int  d;             ///< 汎用
    int* sz;            ///< 各次元の要素数 sz[0] 〜 sz[n-1]
    T*   mx;            ///< 要素 mx[0] 〜 mx[r-1]
    T    err;           ///< エラー時にこれを参照用として返す．

public:
    Matrix()  { init();}
    Matrix(int nn, ...);
    virtual ~Matrix() {}    ///< 関数にコピー渡しした場合に，関数内でディストラクトされても良い様に free() は使用しない．

    void  init() { n = r = d = 0; err = (T)0; sz = NULL; mx = NULL;}
    void  init(int nn, ...);
    
    void  getm(int nn, int* size);
    T&    element(int i, ...);
    void  clear(T v=T(0)) { for(int i=0;i<r;i++) mx[i]=v;}
    void  dup(Matrix<T> a) { *this = dup_Matrix(a);}    

    /// free() は手動で呼び出す．
    void  free() { if(sz!=NULL) ::free(sz); if(mx!=NULL) ::free(mx); init();}   
};


/**
template <typename T>  Matrix<T>::Matrix(int nn, ...)

コンストラクタ 

@param  nn   マトリックスの次元数．
@param  ...  各次元の要素数．

@par 例
@code
    Matrix(1, 4)      --- 4元ベクトル
    Matrix(2, 3, 4)   --- 3x4 の2次元マトリックス
    Matrix(3, 2, 4, 3) -- 2x4x3 の3次元マトリックス
@endcode
*/
template <typename T>  Matrix<T>::Matrix(int nn, ...) 
{
    r   = 0;
    n   = nn;
    d   = 0;
    err = (T)0;
    mx  = NULL;
    sz  = (int*)malloc(n*sizeof(int));
    if (sz==NULL) return;

    va_list argsptr;

    va_start(argsptr, nn);
    r = 1;
    for (int i=0; i<n; i++) {
        sz[i] = (int)va_arg(argsptr, int);
        r = r*sz[i];
    }
    va_end(argsptr);

    mx = (T*)malloc(r*sizeof(T));
    if (mx==NULL) {
        ::free(sz);
        sz = NULL;
        return;
    }
    for (int i=0; i<r; i++) mx[i] = (T)0;
    
    return;
}


/**
template <typename T>  void  Matrix<T>::init(int nn, ...) 

現在のバッファ部をクリアして，任意(n)次元の実数マトリックスのバッファ部をつくり出す．@n
要素自体は (T)0に初期化される．現在のバッファ部をクリアする以外は，コンストラクタと同じ．

@param  nn   マトリックスの次元数．
@param  ...  各次元の要素数．
*/
template <typename T>  void  Matrix<T>::init(int nn, ...) 
{
    if (sz!=NULL) ::free(sz);
    if (mx!=NULL) ::free(mx);

    r   = 0;
    n   = nn;
    d   = 0;
    err = (T)0;
    mx  = NULL;
    sz  = (int*)malloc(n*sizeof(int));
    if (sz==NULL) return;

    va_list argsptr;

    va_start(argsptr, nn);
    r = 1;
    for (int i=0; i<n; i++) {
        sz[i] = (int)va_arg(argsptr, int);
        r = r*sz[i];
    }
    va_end(argsptr);

    mx = (T*)malloc(r*sizeof(T));
    if (mx==NULL) {
        ::free(sz);
        sz = NULL;
        return;
    }
    for (int i=0; i<r; i++) mx[i] = (T)0;
    
    return;
}


/**
template <typename T>  void  Matrix<T>::getm(int nn, int* size)
  
現在のバッファ部をクリアして，任意(n)次元の実数マトリックスのバッファ部をつくり出す．@n
要素自体は (T)0に初期化される．init() とは引数の形が違うだけ

@param  nn    マトリックスの次元数．
@param  size  size[0]〜size[nn-1]: 各次元の要素数．
*/
template <typename T>  void  Matrix<T>::getm(int nn, int* size)
{
    if (size==NULL) return;
    if (sz!=NULL) ::free(sz);
    if (mx!=NULL) ::free(mx);

    r   = 0;
    n   = nn;
    d   = 0;
    err = (T)0;
    mx  = NULL;
    sz  = (int*)malloc(n*sizeof(int));
    if (sz==NULL) return;

    r = 1;
    for (int i=0; i<n; i++) {
        sz[i] = size[i];
        r = r*sz[i];
    }

    mx = (T*)malloc(r*sizeof(T));
    if (mx==NULL) {
        ::free(sz);
        sz = NULL;
        return;
    }
    for (int i=0; i<r; i++) mx[i] = (T)0;

    return;
}


/**  
template <typename T>  T&  Matrix<T>::element(int i, ...) 

Matrix の要素を返す．次元数に制限はない．インデックスは1から数える（0からではない）．

@par 参考
1次元配列へのアクセスインデックス
@code
    1次元: element(i)                                                                    (i-1)
    2次元: element(i,j)                                        (j-1) +             sz[1]*(i-1)
    3次元: element(i,j,k)                  (k-1) +       sz[2]*(j-1) +       sz[1]*sz[2]*(i-1)
    4次元: element(i,j,k,l)  (l-1) + sz[3]*(k-1) + sz[2]*sz[3]*(j-1) + sz[1]*sz[2]*sz[3]*(i-1)
    ...................
@endcode
*/
template <typename T>  T&  Matrix<T>::element(int i, ...) 
{
    int*  args;
    va_list argsptr;
        
    args = (int*)malloc(n*sizeof(int));
    if (args==NULL) return err;

    va_start(argsptr, i);
    args[0] = i;
    for (int m=1; m<n; m++) {
        args[m] = (int)va_arg(argsptr, int);
    }
    va_end(argsptr);

    int dx = args[0] - 1;
    for (int d=1; d<n; d++) dx = dx*sz[d] + args[d] - 1;
    ::free(args);

    if (dx>=r || dx<0) return err;
    return mx[dx];
}


/**
template <typename T> void   print_Matrix(FILE* fp, Matrix<T> a)

実数マトリックスの要素を標準出力に書き出す．デバッグ用．

@param  fp  出力先のファイルディスクリプタ
@param  a   プリントするマトリックス
*/
template <typename T> void   print_Matrix(FILE* fp, Matrix<T> a)
{
    for (int i=0; i<a.r; i++) {
        fprintf(fp, "  %15lf", (double)a.mx[i]);
        if ((i+1)%a.sz[a.n-1]==0) fprintf(fp, "\n");
    }
}


/**
template <typename T>  Matrix<T>  dup_Matrix(Matrix<T> a)

同じ Matrixデータを作り出す．ただしメモリ部は共有しない
*/
template <typename T>  Matrix<T>  dup_Matrix(Matrix<T> a)
{
    Matrix<T> mtx;
    
    if (a.sz!=NULL) {
        mtx.getm(a.n, a.sz);
        if (a.mx!=NULL && mtx.r==a.r) {
            for (int i=0; i<a.r; i++) {
                mtx.mx[i] = a.mx[i];
            }
        }
    }
    
    mtx.d   = a.d;
    mtx.err = (T)(1);
    //
    return mtx;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// オペレータ

/**
template <typename T>  Matrix<T> operator * (const Matrix<T> a, const Matrix<T> b)

マトリックスの掛け算．@n
マトリックス a, bを掛け算して，結果のマトリックスを返す．

@param  a  掛けられるマトリックス．
@param  b  掛けるマトリックス．

@return 掛け算結果のマトリックス．掛け算不能，または失敗の場合は空のマトリックス．
*/
template <typename T>  Matrix<T> operator * (const Matrix<T> a, const Matrix<T> b)
{
    int  i, j, k, n, ii, aa, bb;
    int  *sz, *sa, *sb, *sc, *cx;
    T    st;
    Matrix<T> c;

    if (a.mx==NULL || b.mx==NULL) return c;
    if (a.sz[a.n-1]!=b.sz[0])     return c;

    n  = a.n + b.n - 2;
    sz = (int*)malloc(n*sizeof(int));
    if (sz==NULL) return c;
    sa = (int*)malloc(a.n*sizeof(int));
    if (sa==NULL) {free(sz); return c;}
    sb = (int*)malloc(b.n*sizeof(int));
    if (sb==NULL) {free(sz); free(sa); return c;}
    sc = (int*)malloc(n*sizeof(int));
    if (sc==NULL) {free(sz); free(sa); free(sb); return c;}
    cx = (int*)malloc(n*sizeof(int));
    if (cx==NULL) {free(sz); free(sa); free(sb); free(sc); return c;}

    for (i=0; i<a.n-1; i++) sz[i] = a.sz[i];
    for (i=1; i<b.n; i++)   sz[a.n-2+i] = b.sz[i];

    sa[a.n-1] = sb[b.n-1] = sc[n-1] = 1;
    for (i=a.n-2; i>=0; i--) sa[i] = sa[i+1]*a.sz[i+1];
    for (i=b.n-2; i>=0; i--) sb[i] = sb[i+1]*b.sz[i+1];
    for (i=n-2;   i>=0; i--) sc[i] = sc[i+1]*sz[i+1];

    c.getm(n, sz);

    if (c.sz!=NULL) {
       for (i=0; i<c.r; i++) {
            ii = i;
            for (j=0; j<c.n; j++) {
                cx[j] = ii / sc[j]; 
                ii = ii % sc[j];
            }
            aa = bb = 0;
            for (j=0; j<a.n-1; j++) aa = aa + sa[j]*cx[j]; 
            for (j=1; j<b.n;   j++) bb = bb + sb[j]*cx[j+a.n-2];

            st = (T)0;
            for (k=0; k<b.sz[0]; k++) st = st + a.mx[k+aa]*b.mx[bb+sb[0]*k];
            c.mx[i] = st;
        }
    }
        
    free(sz);
    free(sa);
    free(sb);
    free(sc);
    free(cx);

    return c;
}


/**
template <typename T>  Matrix<T> operator * (const Matrix<T> a, const Matrix<T> b)

マトリックスとベクトルの掛け算．@n
マトリックス a と 3Dベクトル vを掛け算して，結果のベクトルを返す．

@param  a  掛けられるマトリックス．
@param  v  掛けるベクトル．

@return 掛け算結果のベクトル．掛け算不能，または失敗の場合は 0ベクトル．
*/
template <typename T>  Vector<T> operator * (const Matrix<T> a, const Vector<T> v)
{
    Vector<T> vct(0.0, 0.0, 0.0, 0.0, -1.0);

    Matrix<T> b(1, 3);
    b.mx[0] = v.x;
    b.mx[1] = v.y;
    b.mx[2] = v.z;

    Matrix<T> c = a*b;
    b.free();

    if (c.mx==NULL) return vct;
    vct.x = c.mx[0];
    vct.y = c.mx[1];
    vct.z = c.mx[2];
    vct.c = v.c;
    c.free();

    return vct;
}


template <typename T> inline Matrix<T> operator - (const Matrix<T> a)
{   
    Matrix<T>  c(a.n, a.sz);
    if (c.mx!=NULL) for (int i=0; i<a.r; i++) c.mx[i] = -a.mx[i];
    return c;
}


template <typename T> inline Matrix<T> operator + (const Matrix<T> a, const Matrix<T> b)
{ 
    Matrix<T>  c;
    if (!isSameDimension(a, b)) return c;
    
    c.getm(a.n, a.sz);
    if (c.mx!=NULL) for (int i=0; i<a.r; i++) c.mx[i] = a.mx[i] + b.mx[i];
    return c;
}


template <typename T> inline Matrix<T> operator - (const Matrix<T> a, const Matrix<T> b)
{ 
    Matrix<T>  c;
    if (!isSameDimension(a, b)) return c;

    c.getm(a.n, a.sz);
    if (c.mx!=NULL) for (int i=0; i<a.r; i++) c.mx[i] = a.mx[i] - b.mx[i];
    return c;
}


//
template <typename T, typename R> inline Matrix<T> operator * (const R d, const Matrix<T> a)
{ 
    Matrix<T>  c(a.n, a.sz);
    if (c.mx!=NULL) for (int i=0; i<a.r; i++) c.mx[i] = (T)(d)*a.mx[i];
    return c;
}


//
template <typename T, typename R> inline Matrix<T> operator * (const Matrix<T> a, const R d)
{ 
    Matrix<T>  c(a.n, a.sz);
    if (c.mx!=NULL) for (int i=0; i<a.r; i++) c.mx[i] = a.mx[i]*(T)d;
    return c;
}


//
template <typename T, typename R> inline Matrix<T> operator / (const Matrix<T> a, const R d)
{
    Matrix<T>  c(a.n, a.sz);
    if (c.mx!=NULL) for (int i=0; i<a.r; i++) c.mx[i] = a.mx[i]/(T)d;
    return c;
}


template <typename T> inline bool operator == (const Matrix<T> v1, const Matrix<T> v2)
{
    if (!isSameDimension(v1, v2)) return false;
    for (int i=0; i<v1.r; i++) if (v1.mx[i]!=v2.mx[i]) return false;
    return true;
}


template <typename T> inline bool isSameDimension(const Matrix<T> v1, const Matrix<T> v2)
{
    if (v1.n!=v2.n || v1.r!=v2.r) return false;
    for (int i=0; i<v1.n; i++) if (v1.sz[i]!=v2.sz[i]) return false;
    return true;
}


}       // namespace

#endif 
