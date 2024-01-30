#ifndef  __JBXL_MATH_LIBRARY_H_
#define  __JBXL_MATH_LIBRARY_H_

/**
@brief 数学関数ヘッダ     mt.h
@file    mt.h
@author  Fumi.Iseki (C)
*/ 

#include "common.h"
#include <math.h>

extern double EPS;

double   comb(int n, int i);
double   perm(int n, int i);
double   fact(int n); 
double   power(double x, double y);

typedef  double (*PTR_DFFUNC)(double);                                      ///< ニュートン法で解を求める場合の関数の型

int  newton_method(PTR_DFFUNC fn, PTR_DFFUNC df, double* t, double e);      ///< ニュートン法で解を求める


#endif
