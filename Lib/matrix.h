
#ifndef  __JBXL_MATRIX_LIBRARY_H_
#define  __JBXL_MATRIX_LIBRARY_H_


/** 
@brief   マトリックス＆ベクトルライブラリ ヘッダ
@file    matrix.h
@author  Fumi.Iseki (C)
*/


#include "common.h"
#include "mt.h"

#include <math.h>
#include <float.h>



#ifndef va_start
    #include <stdarg.h>
#endif


typedef struct _ivector {
    int x;                  ///< x方向成分
    int y;                  ///< y方向成分
    int z;                  ///<  z方向成分
    double n;               ///< ベクトルの大きさ
} ivector;


typedef struct _vector {
    double x;               ///< x方向成分
    double y;               ///< y方向成分
    double z;               ///< z方向成分
    double n;               ///< ベクトルの大きさ
} vector;


typedef struct _quaternion {
    double s;               ///< s
    double x;               ///< x成分
    double y;               ///< y成分
    double z;               ///< z成分
    double n;               ///< ベクトルの大きさ
} quaternion;



/**
多次元整数マトリックス型  imatrix

imatrix型の引数は (行, 列, ....)となり,実際には後ろの方から
順にメモリ格納される(グラフィック等とは逆方向 gdata.c)ので
直接メモリにアクセスする場合は注意が必要である．
*/
typedef struct _imatrix {
    int n;                  ///< 次元数
    int r;                  ///< 全要素数 sz[0]+sz[1]+...+sz[n-1]
    int *sz;                ///< 各次元の要素数 sz[0]?sz[n-1]
    int *mx;                ///< 要素 mx[0]?mx[r-1] 
} imatrix;


/**
多次元実数マトリックス型  matrix

matrix型の引数は (行, 列, ....)となり,実際には後ろの方から
順にメモリ格納される(グラフィック等とは逆方向 gdata.c)ので
直接メモリにアクセスする場合は注意が必要である．
 */
typedef struct _matrix {
    int n;                  ///< 次元数
    int r;                  ///< 全要素数 sz[0]+sz[1]+...+sz[n-1] 
    int *sz;                ///< 各次元の要素数 sz[0]?sz[n-1]
    double *mx;             ///< 要素 mx[0]?mx[r-1] 
} matrix;


#define  in_vector(a, b)   ((a).x*(b).x+(a).y*(b).y+(a).z*(b).z)                              ///< ベクトル a, bの内積をとる．
#define  add_vector(a, b)  set_vector((a).x+(b).x,(a).y+(b).y,(a).z+(b).z)                    ///< ベクトル a, bを足し算して, 結果を実数ベクトルで返す．
#define  sub_vector(a, b)  set_vector((a).x-(b).x,(a).y-(b).y,(a).z-(b).z)                    ///< ベクトル a, bを引き算して, 結果を実数ベクトルで返す．
#define  add_ivector(a, b) set_ivector((a).x+(b).x,(a).y+(b).y,(a).z+(b).z)                   ///< ベクトル a, bを足し算して, 結果を整数ベクトルで返す．
#define  sub_ivector(a, b) set_ivector((a).x-(b).x,(a).y-(b).y,(a).z-(b).z)                   ///< ベクトル a, bを引き算して, 結果を整数ベクトルで返す．
#define  normal_vector(a)  (a).r = sqrt((double)((a).x*(a).x)+(a).y*(a).y+(a).z*(a).z);       ///< ベクトル aの大きさを計算する．


#define Vt(m, i)      ((m).mx[(i)-1])                                                         ///< 1次元マトリックスの i番目の要素を返す．1から数える．
#define Mx(m, i, j)    ((m).mx[(j)-1+(m).sz[1]*((i)-1)])                                      ///< 2次元マトリックスの(i,j)要素を返す．   1から数える．

#define Mx1(m, i)                ((m).mx[(i)-1])                                              ///< 1次元マトリックスの i番目の要素を返す．1から数える．
#define Mx2(m, i, j)       ((m).mx[(j)-1+(m).sz[1]*((i)-1)])                                  ///< 2次元マトリックスの(i,j)要素を返す．   1から数える．
#define Mx3(m, i, j, k)    ((m).mx[(k)-1+(m).sz[2]*((j)-1)+(m).sz[1]*(m).sz[2]*((i)-1)])      ///< 3次元マトリックスの(i,j,k)要素を返す． 1から数える．
#define Mx4(m, i, j, k, l) ((m).mx[(l)-1+(m).sz[3]*((k)-1)+(m).sz[2]*(m).sz[3]*((j)-1) +\
                            (m).sz[1]*(m).sz[2]*(m).sz[3]*((i)-1)])                           ///< 4次元マトリックスの(i,j,k,l)要素を返す．1から数える．


vector   unit_vector(vector a);
vector   unit_ivector(ivector a);
vector   set_vector(double x, double y, double z);
ivector  set_ivector(int x, int y, int z);
vector   ex_vector(vector a, vector b);

ivector  f2ivector(vector a);
vector   i2vector(ivector a);

matrix   make_matrix1(int n);
matrix   make_matrix2(int n, int m);
matrix   make_matrix (int n, int* sz);

imatrix  make_imatrix1(int n);
imatrix  make_imatrix2(int n, int m);
imatrix  make_imatrix (int n, int* sz);


double*  get_matrix(matrix a, ...);
matrix   add_matrix(matrix a, matrix b);
matrix   sub_matrix(matrix a, matrix b);
matrix   mlt_matrix(matrix a, matrix b);
void     copy_matrix(matrix a, matrix b);
void     free_matrix(matrix* a);
void     clear_matrix(matrix* a);
void     print_matrix(FILE* fp, matrix a);

int*     get_imatrix(imatrix a, ...);
imatrix  add_imatrix(imatrix a, imatrix b);
imatrix  sub_imatrix(imatrix a, imatrix b);
imatrix  mlt_imatrix(imatrix a, imatrix b);
void     copy_imatrix(imatrix a, imatrix b);
void     free_imatrix(imatrix* a);
void     clear_imatrix(imatrix* a);
void     print_imatrix(FILE* fp, imatrix a);

//
matrix   decompQR(matrix xx, imatrix col);
matrix   trans_matrix(matrix a);
matrix   minimum2(matrix a, matrix x);
matrix   invrU_matrix(matrix x);


#endif
