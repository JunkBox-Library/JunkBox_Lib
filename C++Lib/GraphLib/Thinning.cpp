
/**
@brief    細線化関数
@file     Thinning.cpp
@author   Fumi.Iseki (C)

@see 信学論D-II, Vol.J79-D-II, pp.1675-1685, 1996.
*/


#include  "Thinning.h"


using namespace jbxl;


/**
int  jbxl::connectNumber(int* w, int c, int d)

連結数の計算

@param  w  w[] 近傍の連結数
@param  c  近傍数
@param  d  次元数

@return 連結数
*/
int  jbxl::connectNumber(int* w, int c, int d)
{
    int  i;
    int  cn, n0, n1, n2;
    int  v[27];

    if (d<3) {
        for(i=0; i<9; i++) v[i] = w[i];
        if (c==8) {
            v[4] = 1 - v[4];
            for (i=0; i<9; i++) v[i] = 1 - v[i];
        }
        if (c==4 || c==8) {
            cn =  v[1] - v[1]*v[0]*v[3]; 
            cn += v[3] - v[3]*v[6]*v[7]; 
            cn += v[5] - v[5]*v[2]*v[1]; 
            cn += v[7] - v[7]*v[8]*v[5]; 
        }
        else {
            cn = -1;
        }
    }
    else {
        for (i=0; i<27; i++) v[i] = w[i];
        if (c==26) {
            v[13] = 1 - v[13];
            for(i=0; i<27; i++) v[i] = 1 - v[i];
        }
        if (c==6 || c==26) {
        n0 = v[13]*v[14] + v[13]*v[12] +
             v[13]*v[16] + v[13]*v[10] +
             v[13]*v[22] + v[13]*v[ 4];

        n1 = v[13]*v[14]*v[10]*v[11] + v[13]*v[14]*v[16]*v[17] +
             v[13]*v[14]*v[ 4]*v[ 5] + v[13]*v[14]*v[22]*v[23] +
             v[13]*v[12]*v[10]*v[ 9] + v[13]*v[12]*v[16]*v[15] +
             v[13]*v[12]*v[ 4]*v[ 3] + v[13]*v[12]*v[22]*v[21] +
             v[13]*v[10]*v[ 4]*v[ 1] + v[13]*v[10]*v[22]*v[19] +
             v[13]*v[16]*v[ 4]*v[ 7] + v[13]*v[16]*v[22]*v[25];

        n2 = v[13]*v[14]*v[10]*v[11]*v[ 4]*v[ 5]*v[ 1]*v[ 2] +
             v[13]*v[14]*v[10]*v[11]*v[22]*v[23]*v[19]*v[20] +
             v[13]*v[14]*v[16]*v[17]*v[ 4]*v[ 5]*v[ 7]*v[ 8] +
             v[13]*v[14]*v[16]*v[17]*v[22]*v[23]*v[25]*v[26] +
             v[13]*v[12]*v[10]*v[ 9]*v[ 4]*v[ 3]*v[ 1]*v[ 0] +
             v[13]*v[12]*v[10]*v[ 9]*v[22]*v[21]*v[19]*v[18] +
             v[13]*v[12]*v[16]*v[15]*v[ 4]*v[ 3]*v[ 7]*v[ 6] +
             v[13]*v[12]*v[16]*v[15]*v[22]*v[21]*v[25]*v[24];

            cn = n0 - n1 + n2;
            if (c==26) cn = 2 - cn; 
        }
        else {
            cn = -1;
        }
    }

    return  cn;
}


bool  jbxl::deletable_s(int* v)
{
    int  s;

    s = (1-v[ 6])*v[ 3]*v[ 7]*v[ 4]*v[12]*v[15]*v[16] +
        (1-v[ 8])*v[ 7]*v[ 5]*v[ 4]*v[16]*v[17]*v[14] +
        (1-v[ 2])*v[ 5]*v[ 1]*v[ 4]*v[14]*v[11]*v[10] +
        (1-v[ 0])*v[ 1]*v[ 3]*v[ 4]*v[10]*v[ 9]*v[12] +
        (1-v[24])*v[21]*v[25]*v[22]*v[12]*v[15]*v[16] +
        (1-v[26])*v[25]*v[23]*v[22]*v[16]*v[17]*v[14] +
        (1-v[20])*v[23]*v[19]*v[22]*v[14]*v[11]*v[10] +
        (1-v[18])*v[19]*v[21]*v[22]*v[10]*v[ 9]*v[12];

    if (s==1) return false;
    else      return true;
}


bool  jbxl::deletable_4(int* v)
{
    int  i, w[6], u[6];

    bool ret = deletable_s(v);
    if (!ret) return false;

    u[0] = v[ 4];
    u[1] = v[10];
    u[2] = v[12];
    u[3] = v[14];
    u[4] = v[16];
    u[5] = v[22];

    for(i=0; i<6; i++)   w[i] = 0;

    if (v[ 1]==1) w[0] = w[1] = 1;
    if (v[ 3]==1) w[0] = w[2] = 1;
    if (v[ 5]==1) w[0] = w[3] = 1;
    if (v[ 7]==1) w[0] = w[4] = 1;
    if (v[ 9]==1) w[1] = w[2] = 1;
    if (v[11]==1) w[1] = w[3] = 1;
    if (v[15]==1) w[2] = w[4] = 1;
    if (v[17]==1) w[3] = w[4] = 1;
    if (v[19]==1) w[1] = w[5] = 1;
    if (v[21]==1) w[2] = w[5] = 1;
    if (v[23]==1) w[3] = w[5] = 1;
    if (v[25]==1) w[4] = w[5] = 1;

    ret = true;
    for(i=0; i<6; i++) if (w[i]==0 && u[i]==1) ret = false;
    
    return  ret;
}


bool  jbxl::deletable_5(int* v)
{
    int  i, j, k, m, s;
    int  mz[6][6], mx[6][6], mm[6][6];

    mm[0][1] = mm[1][0] = v[ 3];
    mm[0][2] = mm[2][0] = v[ 7];
    mm[0][3] = mm[3][0] = v[ 1];
    mm[0][4] = mm[4][0] = v[ 5];
    mm[1][2] = mm[2][1] = v[15];
    mm[1][3] = mm[3][1] = v[ 9];
    mm[1][5] = mm[5][1] = v[21];
    mm[2][4] = mm[4][2] = v[17];
    mm[2][5] = mm[5][2] = v[25];
    mm[3][4] = mm[4][3] = v[11];
    mm[3][5] = mm[5][3] = v[19];
    mm[4][5] = mm[5][4] = v[23];

    for(i=0; i<6; i++) {            // I+M 
        for(j=0; j<6; j++) {
            mx[i][j] = mm[i][j];
            if (i==j) mx[i][i]++;
        }
    }

    for(i=0; i<6; i++) {            // M(I+M) 
        for(j=0; j<6; j++) {
            m = 0;
            for(k=0; k<6; k++) {
                m = m + mm[i][k]*mx[k][j];
            }
            mz[i][j] = m; 
        }
    }
        
    for(i=0; i<6; i++) {            // I+M(I+M) 
        mz[i][i] = 1 + mz[i][i];
    }

    for(i=0; i<6; i++) {            // M(I+M(I+M)) 
        for(j=0; j<6; j++) {
            m = 0;
            for(k=0; k<6; k++) {
                m = m + mm[i][k]*mz[k][j];
            }
            mx[i][j] = m; 
        }
    }

    for(i=0; i<6; i++) {            // I+M(I+M(I+M)) 
        mx[i][i] = 1 + mx[i][i];
    }

    for(i=0; i<6; i++) {            // M(I+M(I+M(I+M))) 
        for(j=0; j<6; j++) {
            m = 0;
            for(k=0; k<6; k++) {
                m = m + mm[i][k]*mx[k][j];
            }
            mz[i][j] = m; 
        }
    }

    for(i=0; i<6; i++) {            // I+M(I+M(I+M(I+M))) 
        mz[i][i] = 1 + mz[i][i];
    }

    s = 1;
    for(i=0; i<6; i++) {            // M(I+M(I+M(I+M(I+M)))) 
        for(j=0; j<6; j++) {
            m = 0;
            for(k=0; k<6; k++) {
                m = m + mm[i][k]*mz[k][j];
            }
            s = s*m; 
        }
    }

    if (s==0)  return  false;
    else       return  true;
}
    



