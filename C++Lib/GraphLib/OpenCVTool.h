#ifndef  __JBXL_CPP_OPENCV_TOOl_H_
#define  __JBXL_CPP_OPENCV_TOOl_H_

#ifndef  ENABLE_OPENCV
#error "OpenCVTool.h is called, but ENABLE_OPENCV is not defined."
#endif

/**
@brief    OpenCV用ヘッダ  
@file     OpenCVTool.h
@version  0.9
@date     2012 10/3
@author   Fumi.Iseki (C)

@attention
this software uses OpenCV
*/

#include "common++.h"
#include "opencv2/opencv.hpp"

#include "Gdata.h"
#include "xtools.h"


#ifdef  WIN32
#ifdef  _DEBUG
#pragma  comment(lib, "opencv_core242d.lib")
#pragma  comment(lib, "opencv_imgproc242d.lib")
#pragma  comment(lib, "opencv_objdetect242d.lib")
#else 
#pragma  comment(lib, "opencv_core242.lib")
#pragma  comment(lib, "opencv_imgproc242.lib")
#pragma  comment(lib, "opencv_objdetect242.lib")
#endif
#endif


//
namespace jbxl {

// template <typename T>  rectangle*  cvDetectObjects(cv::CascadeClassifier cascade, MSGraph<T>* vp, int& num, int sz=0, double scale=1.0)

// template <typename R, typename T>  cv::Mat  copyMSGraph2CvMat(MSGraph<T>* vp)
// template <typename T>  MSGraph<T>* getMSGraphFromCvMat(cv::Mat mat)

// 補助関数
// template <typename T, typename R>  MSGraph<T>* _getMSGraph_CvMat_C1(cv::Mat mat)
// template <typename T, typename R>  MSGraph<T>* _getMSGraph_CvMat_C3(cv::Mat mat)
// template <typename R, typename T>  int         _linecopy_MAT2MSG_C3(R* src, T* dist, int len, int sz)

/**
@param  cascade  識別エンジン
@param  vp     1plane(channel) MSGraphデータ（つまりモノクロ）へのポインタ
@param[out]    num 要素の数
@param  sz     検出する最小のサイズ
@param  scale  縮小スケール
*/
template <typename T>  rectangle*  cvDetectObjects(cv::CascadeClassifier cascade, MSGraph<T>* vp, int& num, int sz=0, double scale=1.0)
{
    num = 0;

    cv::Mat imag = copyMSGraph2CvMat<uByte>(vp);
    cv::equalizeHist(imag, imag);

    cv::Mat simg = imag;
    if (scale>1.0) {
        simg = cv::Mat_<uByte>((int)(imag.rows/scale), (int)(imag.cols/scale));
        cv::resize(imag, simg, simg.size(), 0, 0, cv::INTER_LINEAR);
    }
    else scale = 1.0;
    
    std::vector<cv::Rect> faces;
    if (sz>1) cascade.detectMultiScale(simg, faces, 1.1, 3, 0, cv::Size(sz, sz));
    else      cascade.detectMultiScale(simg, faces);

    num = (int)faces.size();
    if (num==0) return NULL;

    int len = sizeof(rectangle)*num;
    rectangle* data = (rectangle*)malloc(len);
    if (data==NULL) return NULL;
    memset(data, 0, len);

    int n = 0;
    std::vector<cv::Rect>::const_iterator r;    
    for (r=faces.begin(); r!=faces.end(); r++) {
        data[n].x     = (int)(r->x*scale);
        data[n].y     = (int)(r->y*scale);
        data[n].xsize = (int)(r->width *scale); 
        data[n].ysize = (int)(r->height*scale); 
        n++;
    }
    return data;
}


/**
template <typename R, typename T>  cv::Mat  copyMSGraph2CvMat(MSGraph<T>* vp)

MSGraph<T> vp から OpenCV用の Matデータ（型R）を作り出す．@n
型 T, R の整合性は呼び出し側の責任．
現在は T->R で単純にキャスト可能な場合にしか対応していない．

vp.color による Matの構成は今後....

@param  vp   元のグラフィックデータ
@return OpenCV のMatデータ

@code
    cv::Mat imag = copyMSGraph2CvMat<uByte>(*vp);
@endcode
*/
template <typename R, typename T>  cv::Mat  copyMSGraph2CvMat(MSGraph<T>* vp)
{
    cv::Mat mat;

    if (vp->zs<=1) {
        vp->zs = 1;
        mat = cv::Mat_<R>(vp->ys, vp->xs);

        // 通常は連続しているはず
        if (mat.isContinuous()) {
            R* dst = mat.ptr<R>(0);
            for (int i=0; i<vp->xs*vp->ys; i++) {
                dst[i] = (R)vp->gp[i];
            }
        }
        else {
            for (int j=0; j<vp->ys; j++) {
                R* dst = mat.ptr<R>(j);
                T* src = &(vp->gp[j*vp->xs]);
                for (int i=0; i<vp->xs; i++) {
                    dst[i] = (R)src[i];
                }
            }
        }
    }

    //
    else {
        int size[3];
        size[0] = vp->zs;
        size[1] = vp->ys;
        size[2] = vp->xs;
        mat = cv::Mat_<R>(3, size);

        // 通常は連続しているはず
        if (mat.isContinuous()) {
            R* dst = (R*)mat.data;
            for (int i=0; i<vp->xs*vp->ys*vp->zs; i++) {
                dst[i] = (R)vp->gp[i]; 
            }
        }
        else {
            for (int k=0; k<vp->zs; k++) {
                int kk = k*vp->ys*vp->xs;
                for (int j=0; j<vp->ys; j++) {
                    R* dst = mat.ptr<R>(k, j);
                    T* src = &(vp->gp[j*vp->xs + kk]);
                    for (int i=0; i<vp->xs; i++) {
                        dst[i] = (R)src[i];
                    }
                }
            }
        }
    }
    return mat;
}


/**
template <typename T>  MSGraph<T>*  getMSGraphFromCvMat(cv::Mat mat)

MSGraph<T> から OpenCV用の Matデータを作り出す．

T を cv::Mat の型と合わせるのは，呼び出し側の責任．
cv::Matの型と（長さなどが）合わない型Tを指定した場合，戻り値のデータ内容は保障されない．

Mat のチャンネル数は 1と 3のみをサポート. 浮動小数点の depth（CV_32F, CV_64F）は変換できない．@n
T は uByte, sByte, uWord, sWord または unsigned int を指定する．

@param  mat    元のMatデータ
@return MSGraph<>データ．失敗した場合は NULL

@attention
cv::imread()関数の戻り値のチャンネル数は通常3(RGB)となるので，モノクロデータを imread()関数で
読み込で，uWordのデータに変換するには場合は以下のようにする．
@code
    cv::Mat src_img = cv::imread("suba.ras");
    MSGraph<uByte>* zp = getMSGraphFromCvMat<uByte>(src_img); // <uWord> にすると精度が落ちる
    xp = new MSGraph<sWord>();
    copy_MSGraph(*zp, *xp);
    delete(zp);

または

    cv::Mat gray, src_img = cv::imread("suba.ras");
    cv::cvtColor(src_img, gray, CV_BGR2GRAY);
    xp = getMSGraphFromCvMat<sWord>(gray);
@endcode
*/
template <typename T>  MSGraph<T>*  getMSGraphFromCvMat(cv::Mat mat)
{
    MSGraph<T>* vp = NULL;

    if (mat.channels()==1) {
        if      (mat.depth()==CV_8U)  vp = _getMSGraph_CvMat_C1<T, uByte>(mat);
        else if (mat.depth()==CV_8S)  vp = _getMSGraph_CvMat_C1<T, sByte>(mat);
        else if (mat.depth()==CV_16U) vp = _getMSGraph_CvMat_C1<T, uWord>(mat);
        else if (mat.depth()==CV_16S) vp = _getMSGraph_CvMat_C1<T, sWord>(mat);
        else if (mat.depth()==CV_32S) vp = _getMSGraph_CvMat_C1<T, int>(mat);
        //else if (mat.depth()==CV_32F) vp = _getMSGraph_CvMat_C1<T, double>(mat);
        //else if (mat.depth()==CV_64F) vp = _getMSGraph_CvMat_C1<T, double>(mat);
    }

    else if (mat.channels()==3) {
        if      (mat.depth()==CV_8U)  vp = _getMSGraph_CvMat_C3<T, uByte>(mat);
        else if (mat.depth()==CV_8S)  vp = _getMSGraph_CvMat_C3<T, sByte>(mat);
        else if (mat.depth()==CV_16U) vp = _getMSGraph_CvMat_C3<T, uWord>(mat);
        else if (mat.depth()==CV_16S) vp = _getMSGraph_CvMat_C3<T, sWord>(mat);
        else if (mat.depth()==CV_32S) vp = _getMSGraph_CvMat_C3<T, int>(mat);
        //else if (mat.depth()==CV_32F) vp = getMSGraph_CvMatC3<T, double>(mat);
        //else if (mat.depth()==CV_64F) vp = getMSGraph_CvMatC3<T, double>(mat);
    }
    return vp;
}


// getMSGraphFromCvMat() の補助関数．
// チャンネル数１用
//
template <typename T, typename R>  MSGraph<T>*  _getMSGraph_CvMat_C1(cv::Mat mat)
{
    MSGraph<T>* vp = NULL;

    if (mat.channels()!=1) return NULL;

    if (mat.dims==2) {
        vp = new MSGraph<T>(mat.cols, mat.rows);
        if (vp==NULL || vp->gp==NULL) return vp;

        // 通常は連続しているはず
        if (mat.isContinuous()) {
            R* ptr = (R*)mat.data;
            T* dst = vp->gp;
            for (int i=0; i<vp->xs*vp->ys; i++) {
                dst[i] = (T)ptr[i];
            }
        }
        else {
            for (int j=0; j<vp->ys; j++) {
                R* ptr = mat.ptr<R>(j);
                T* dst = &(vp->gp[j*vp->xs]);
                for (int i=0; i<vp->xs; i++) {
                    dst[i] = (T)ptr[i];
                }
            }
        }
        //
        vp->color = GRAPH_COLOR_MONO;
    }

    // 3次元
    else if (mat.dims==3) {
        vp = new MSGraph<T>((int)mat.size[2], (int)mat.size[1], (int)mat.size[0]);
        if (vp==NULL || vp->gp==NULL) return vp;
        
        // 通常は連続しているはず
        if (mat.isContinuous()) {
            R* ptr = (R*)mat.data;
            T* dst = vp->gp;
            for (int i=0; i<vp->xs*vp->ys*vp->zs; i++) {
                dst[i] = (T)ptr[i]; 
            }
        }
        else {
            for (int k=0; k<vp->zs; k++) {
                int kk = k*vp->ys*vp->xs;
                for (int j=0; j<vp->ys; j++) {
                    R* ptr = mat.ptr<R>(k, j);
                    T* dst = &(vp->gp[j*vp->xs + kk]);
                    for (int i=0; i<vp->xs; i++) {
                        dst[i] = (T)ptr[i];
                    }
                }
            }
        }
        //
        vp->color = GRAPH_COLOR_MONO;
    }
    return vp;
}


// getMSGraphFromCvMat() の補助関数．
// チャンネル数 3用
//
template <typename T, typename R>  MSGraph<T>*  _getMSGraph_CvMat_C3(cv::Mat mat)
{
    MSGraph<T>* vp = NULL;

    if (mat.channels()!=3) return NULL;
    int tsz = sizeof(T);
    
    if (mat.dims==2) {
        vp = new MSGraph<T>(mat.cols, mat.rows);
        if (vp==NULL || vp->gp==NULL) return vp;

        // 通常は連続しているはず
        if (mat.isContinuous()) {
            R* src = (R*)mat.data;
            T* dst = vp->gp;
            vp->color = _linecopy_MAT2MSG_C3(src, dst, vp->xs*vp->ys, tsz);
        }

        else {
            for (int j=0; j<vp->ys; j++) {
                R* src = mat.ptr<R>(j);
                T* dst = &(vp->gp[j*vp->xs]);
                _linecopy_MAT2MSG_C3(src, dst, vp->xs, tsz);
            }
            vp->color = _linecopy_MAT2MSG_C3((R*)NULL, (T*)NULL, 0, tsz);       // return color only
        }
    }

    // 3次元
    else if (mat.dims==3) {
        vp = new MSGraph<T>((int)mat.size[2], (int)mat.size[1], (int)mat.size[0]);
        if (vp==NULL || vp->gp==NULL) return vp;
        
        // 通常は連続しているはず
        if (mat.isContinuous()) {
            R* src = (R*)mat.data;
            T* dst = vp->gp;
            vp->color = _linecopy_MAT2MSG_C3(src, dst, vp->xs*vp->ys*vp->zs, tsz);
        }
        else {
            for (int k=0; k<vp->zs; k++) {
                int kk = k*vp->ys*vp->xs;
                for (int j=0; j<vp->ys; j++) {
                    R* src = mat.ptr<R>(k, j);
                    T* dst = &(vp->gp[j*vp->xs + kk]);
                    for (int i=0; i<vp->xs; i++) {
                        _linecopy_MAT2MSG_C3(src, dst, vp->xs, tsz);
                    }
                }
            }
            vp->color = _linecopy_MAT2MSG_C3((R*)NULL, (T*)NULL, 0, tsz);       // return color only
        }
    }
    return vp;
}


// getMSGraphFromCvMat() の補助関数．
//
template <typename R, typename T>  int  _linecopy_MAT2MSG_C3(R* src, T* dst, int len, int sz)
{
    int i3 = 0;
    int color = GRAPH_COLOR_MONO;

    if (sz==1) {
        for (int i=0; i<len; i++) {
            dst[i] = (T)(((unsigned int)src[i3] + (unsigned int)src[i3+1] + (unsigned int)src[i3+2])/3);
            i3 += 3;
        }
    }

    else if (sz==2) {
        for (int i=0; i<len; i++) {
            dst[i] = (T)RGB2Word((unsigned int)src[i3+2], (unsigned int)src[i3+1], (unsigned int)src[i3]);
            i3 += 3;
        }
        color = GRAPH_COLOR_RGB16;
    }

    else {
        for (int i=0; i<len; i++) {
            dst[i] = (T)ABGR2Int(0, (unsigned int)src[i3], (unsigned int)src[i3+1], (unsigned int)src[i3+2]);
            i3 += 3;
        }
        color = GRAPH_COLOR_ABGR;
    }

    return color;
}


}       // namespace


#endif
 
