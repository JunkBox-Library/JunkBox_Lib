
#ifndef  __JBXL_CPP_GRAPHIC_DATA_H_
#define  __JBXL_CPP_GRAPHIC_DATA_H_


/**
@brief    グラフィックデータ定義用ヘッダ  
@file     Gdata.h
@author   Fumi.Iseki (C)
*/


#include "ClassBox.h"
#include "gheader.h"

#include "Vector.h"


//
namespace jbxl {


extern int  ZeroBase;
extern int  TempBase;


///////////////////////////////////////////////////////////////////////////////////////
//
//
void    free_CmnHead(CmnHead* hd);      //< 共通ヘッダのメモリ領域を開放する
void    init_CmnHead(CmnHead* hd);

#define freeCmnHead  free_CmnHead
#define initCmnHead  init_CmnHead

CmnHead getinfo_CmnHead(CmnHead hd);    ///< ヘッダ情報のみをコピーする

/// 共通ヘッダCmnHeadから座標を指定して，画像データを取り出す
inline  uByte& CmnHeadBytePoint(CmnHead hd, int i=0, int j=0, int k=0) 
{
    return hd.grptr[(int)i + (int)j*hd.xsize + (int)k*hd.xsize*hd.ysize]; 
}

//template <typename T>  MSGraph<T>  copyCmnHead2MSGraph(CmnHead hd,     unsigned int mode=CH2MG_NORMAL);
//template <typename T>  CmnHead     copyMSGraph2CmnHead(MSGraph<T> mgr, unsigned int mode=MG2CH_NORMAL);
//template <typename T>  MSGraph<T>  scalingMSGraph2D(MSGraph<T> vp, double scale)


////////////////////////////////////////////////////////////////////////////////////////////

/**
@file  Gio.h

グラフィックデータの構造 (C++)
---
@par メモリ内での形式．
グラフィックデータはファイルから読み込まれた後，メモリ内で, MSGraph<>型のデータとなる．@n
ただし，ヘッダを解析できない場合や，他のインターフェイスにデータを渡す場合は，
Common Head 形式に変換される場合もある．

@par ファイル中での形式．
ファイルとして扱うことのできるデータは，基本的に Common Head 形式である．@n
ただし，いくつかの形式（Gio.h 参照）は直接読み出すことができる．
*/
template <typename T> class MSGraph;
template <typename T> class ExMSGraph;


/**
template <typename T=uWord> class MSGraph

シンプルグラフィックデータ．@n
メモリ中での画像データのデフォルト．

@attention
ディストラクタはメモリを解放しないので，必ず手動で解放すること．
*/
template <typename T=sWord> class MSGraph
{
public:
    int xs;                 ///< xサイズ. 4Byte.
    int ys;                 ///< yサイズ. 4Byte.
    int zs;                 ///< zサイズ. 4Byte. 2Dの場合は 1.
    T*  gp;                 ///< グラフィックデータへのポインタ.
    T   zero;               ///< 画素値のゼロ位．
    T   base;               ///< 画措置の底上げの値．

// 以下は必要に応じて設定．
public:
    T   max;                ///< 画素値の最大値
    T   min;                ///< 画素値の最小値
    int color;              ///< データのカラータイプ
    int state;              ///< エラー制御
    RBound<int>  rbound;    ///< 画像の境界情報
    double  RZxy;           ///< Z軸の歪．Z軸の間隔を 1とした XY軸の間隔．(X or Y)/Z
    double  norm;           ///< 規格化定数．フィルタのときに使用．

public:
    double  wRateX;         ///< ワールド座標系の x成分1ドットに対するスクリーン座標系のドット数．
    double  wRateY;         ///< ワールド座標系の y成分1ドットに対するスクリーン座標系のドット数．
    double  wRateZ;         ///< ワールド座標系の z成分1ドットに対するスクリーン座標系のドット数．
    double  wZeroX;         ///< スクリーン座標系の原点に対するワールド座標系の x成分．
    double  wZeroY;         ///< スクリーン座標系の原点に対するワールド座標系の y成分．
    double  wZeroZ;         ///< スクリーン座標系の原点に対するワールド座標系の z成分．
    double  wNowX;          ///< ペンの現地点の x成分(ワールド座標系)
    double  wNowY;          ///< ペンの現地点の y成分(ワールド座標系)
    double  wNowZ;          ///< ペンの現地点の z成分(ワールド座標系)

//
public:
    MSGraph(int x, int y=1, int z=1, T v=(T)0, T b=(T)0, double rz=1.0) { set(x, y, z, v, b, rz);}
    MSGraph(RBound<int> rb, T v=(T)0, T b=(T)0, double rz=1.0) { set(rb, v, b, rz);}
    MSGraph(void) { init();}

    virtual ~MSGraph(void) {}

    T&     point(int x, int y=0, int z=0) { return gp[x+xs*y+xs*ys*z];} ///< 座標(x,y,z)の画素値の参照
    T      get_vPoint(double x, double y=0., double z=0., int mode=OFF);
    void   set_vPoint(double x, double y=0., double z=0., T cc=(T)0, int mode=OFF);
    void   put_vPoint(double x, double y=0., double z=0., T cc=(T)0, int mode=OFF);

    void   set(RBound<int> rb, T v=(T)0, T b=(T)0, double rz=1.0);
    void   set(int x, int y=1, int z=1, T v=(T)0, T b=(T)0, double rz=1.0);

    void   init(void);                                      ///< グラフィックデータは解放しない
    bool   isNull(void);                                    ///< グラフィックデータを持っていないか？
    void   fill(T v=(T)0);                                  ///< 全空間を画素値 v にする
    void   clear(T v);                                      ///< 全空間を画素値 v にする
    void   clear(void);                                     ///< 全空間を画素値 zero にする
    void   getm(int x, int y, int z=1, T v=(T)0);           ///< グラフィックメモリを獲得する
    void   getm(T v=(T)0);                                  ///< グラフィックメモリを獲得する
    void   free(void);                                      ///< グラフィックデータを開放する
    void   get_minmax(void);                                ///< min, max を獲得
    void   set_array(T* ary, int sz=0);                     ///< ary の内容を gpにコピー．
    void   contrast(T low, T high);                         ///< コントラストの調整

    template <typename R> void  mimicry(MSGraph<R> s) {
        set(s.xs, s.ys, s.zs, (T)s.zero, (T)s.base, s.RZxy); 
        rbound = s.rbound;
        color  = s.color;
        state  = s.state;
        max    = s.zero;
        min    = s.zero;
        norm   = s.norm;
    };

    template <typename R> void dup(MSGraph<R> s) {
        *this = s;
        getm();
        memcpy(gp, s.gp, xs*ys*zs*sizeof(T));
    };

    // for viewport
    void   setWindow(double x1, double y1, double x2, double y2);
    void   setWindow3D(double x1, double y1, double z1, double x2, double y2, double z2);
    
    void   wMove(double x, double y, double z=0.0) {wNowX=x; wNowY=y; wNowZ=z;}
    void   wMove_rel(double x, double y, double z=0.0) {wNowX+=x; wNowY+=y; wNowZ+=z;}


    MSGraph<T> operator= (ExMSGraph<T>& s);
};


/**
template <typename T>  void  MSGraph<T>::init(void) 

メンバ変数を初期化する．画像データの開放は行わない
*/
template <typename T>  void  MSGraph<T>::init(void) 
{
    xs = ys = zs = 0; 
    gp    = NULL; 
    RZxy  = 1.0;
    zero  = (T)0;
    base  = (T)0;
    min   = (T)0;
    max   = (T)0;
    norm  = 1.0;
    color = GRAPH_COLOR_UNKNOWN;
    state = STATE_GRAPH_NOERR;
    rbound.init();

    wRateX = wRateY = 1.0;
    wZeroX = wZeroY = 1.0;
    wNowX  = wNowY  = 0.0;

    return;
}


/**
template <typename T>  bool  MSGraph<T>::isNull() 

グラフィックデータを持いないか？
*/
template <typename T>  bool  MSGraph<T>::isNull() 
{
    if (gp==NULL) return true; 

#ifdef WIN32
    if (gp==(T*)WIN_DD_NULL) return true; 
#endif

    return false;
}


//
template <typename T>  void  MSGraph<T>::set(RBound<int> rb, T v, T b, double rz) 
{
    set(rb.xmax-rb.xmin+1, rb.ymax-rb.ymin+1, rb.zmax-rb.zmin+1, v, b, rz);
    rbound = rb;
    return;
}


//
template <typename T>  void  MSGraph<T>::set(int x, int y, int z, T v, T b, double rz) 
{
    if (z<=0) z = 1;

    getm(x, y, z, v); 
    if (gp==NULL) return;

    base = b;
    if (rz<=0.0) RZxy = 1.0;
    else         RZxy = rz;
    rbound.set(0, x-1, 0, y-1, 0, z-1);
    //
    color = GRAPH_COLOR_UNKNOWN;
    if (z==1) color = GRAPH_COLOR_MONO;

    norm  = 1.0;
    state = STATE_GRAPH_NOERR;
    return;
}


/**
template <typename T> void  MSGraph<T>::getm(int x, int y, int z, T v)

typename T 型単純グラフィックデータ(MSGraph)のメモリ領域を確保する．@n
グラフィックデータ部分は v (デフォルトは0)で初期化される．

@param  x  確保するグラフィックデータの x方向のサイズ．
@param  y  確保するグラフィックデータの y方向のサイズ．
@param  z  確保するグラフィックデータの z方向のサイズ．
@param  v  グラフィックデータ部分の初期化値（デフォルトは0）
 
メモリ領域を獲得できなかった場合,メモリ領域へのポインタは NULLとなる．
*/
template <typename T> void  MSGraph<T>::getm(int x, int y, int z, T v)
{
    xs = x;
    ys = y;
    zs = z;
    gp = NULL;
    zero = max = min = v;

    if (xs>0 && ys>0 && zs>0) {
        gp = (T*)malloc(xs*ys*zs*sizeof(T));
        if (gp==NULL) {
            init();
            state = JBXL_GRAPH_MEMORY_ERROR;
            return;
        }
        for (int i=0; i<xs*ys*zs; i++) gp[i] = (T)v;
    }
}


/**
template <typename T> void  MSGraph<T>::getm(T v)

既に指定されているサイズに従って，typename T 型単純グラフィックデータ(MSGraph)
のメモリ領域を確保する．@n
グラフィックデータ部分は v (デフォルトは0)で初期化される．

@param  v  グラフィックデータ部分の初期化値（デフォルトは0）
 
メモリ領域を獲得できなかった場合,メモリ領域へのポインタは NULLとなる．
*/
template <typename T> void  MSGraph<T>::getm(T v)
{
    gp = NULL;
    zero = max = min = v;

    if (xs>0 && ys>0 && zs>0) {
        gp = (T*)malloc(xs*ys*zs*sizeof(T));
        if (gp==NULL) {
            init();
            state = JBXL_GRAPH_MEMORY_ERROR;
            return;
        }
        for (int i=0; i<xs*ys*zs; i++) gp[i] = (T)v;
    }
}


template <typename T> void  MSGraph<T>::get_minmax(void)
{
    int i;

    min = max = gp[0];

    for (i=1; i<xs*ys; i++) {
        min = Min(min, gp[i]);
        max = Max(max, gp[i]);
    }
}


template <typename T> void  MSGraph<T>::set_array(T* ary, int sz)
{
    int size = xs*ys*zs;

    if (sz>0) size = Min(size, sz);
    for (int i=0; i<size; i++) gp[i] = ary[i];
}


template <typename T> void  MSGraph<T>::contrast(T low, T high)
{
    if (max==min) return;

    T dif = (high-low)/(max-min);

    for (int i=0; i<xs*ys*zs; i++) {
        gp[i] = (gp[i]-min)*dif + low;
    }

    return;
}


/**
template <typename T>  void  MSGraph<T>::fill(T v) 

全空間を画素値 v にする
*/
template <typename T>  void  MSGraph<T>::fill(T v) 
{       
    for (int i=0; i<xs*ys*zs; i++) gp[i] = v;
    min = max = v;
    return;
}


/**
template <typename T>  void  MSGraph<T>::clear(T v) 

全空間を画素値 v にする．v を zeroにする．
*/
template <typename T>  void  MSGraph<T>::clear(T v) 
{       
    for (int i=0; i<xs*ys*zs; i++) gp[i] = v;
    zero = min = max = v;
    return;
}


/**
template <typename T>  void  MSGraph<T>::clear(void) 

全空間を画素値 zero にする
*/
template <typename T>  void  MSGraph<T>::clear(void) 
{       
    for (int i=0; i<xs*ys*zs; i++) gp[i] = zero; 
    min = max = zero;
    return;
}


/**
template <typename T>  void  MSGraph<T>::free(void) 

グラフィックデータを開放する
*/
template <typename T>  void  MSGraph<T>::free(void) 
{  
    if (gp!=NULL) ::free(gp); 
    init();
    return;
}


/**
template <typename T> T  MSGraph<T>::get_vPoint(double x, double y, double z, int mode);

グラフィックデータ構造体 vpの任意の 1voxelの値(濃度)を返す．@n
座標の範囲チェックあり．Z軸の歪，境界補整あり．

@param  x, y, z  値を設定する xyz座標．
@param  mode     モード： @b OFF なら最も近い点の濃度を返す．それ以外なら周囲の平均．

@return (x,y,z)点の濃度(輝度値)．(x,y,z)が座標の範囲外の場合は zeroを返す．
*/  
template <typename T> T  MSGraph<T>::get_vPoint(double x, double y, double z, int mode)
{
    int   ix, iy, iz;
    int   xx, yy, zz;
    T     ret;

    ret = zero;

    z  = z*RZxy;  
    if (mode==OFF) {
        ix = (int)(x+0.5) - rbound.xmin;
        iy = (int)(y+0.5) - rbound.ymin;
        iz = (int)(z+0.5) - rbound.zmin;
        if (ix>=0 && ix<xs &&iy>=0 && iy<ys && iz>=0 && iz<zs){
            ret = gp[iz*xs*ys + iy*xs + ix];
        }
    }
    else {
        int n = 0;
        T cc = (T)0;

        xx = (int)x - rbound.xmin;
        yy = (int)y - rbound.ymin;
        zz = (int)z - rbound.zmin;
        for (ix=xx; ix<=xx+1; ix++)
        for (iy=yy; iy<=yy+1; iy++)
        for (iz=zz; iz<=zz+1; iz++) {
            if (ix>=0 && ix<xs && iy>=0 && iy<ys && iz>=0 && iz<zs){
                cc = cc + gp[iz*xs*ys + iy*xs + ix];
                n++;
            }
        }
        if (n!=0) ret = cc/n;
    }

    return ret;
}


/**
template <typename T>  void  MSGraph<T>::set_vPoint(double x, double y, double z, T cc, int mode)

グラフィックデータ構造体 gdの任意の 1voxelに値を設定する．@n
座標の範囲チェックあり．Z軸の歪，境界補整あり．

@param  x,y,z  値を設定する xyz座標．
@param  cc     点(x,y,z)に設定する点の値(濃度)．
@param  mode   @b OFFの場合，(x,y,z)に一番近い点に ccを設定する．
               @b ON の場合，(x,y,z)の周りの格子点全てに ccを設定する．
*/
template <typename T>  void  MSGraph<T>::set_vPoint(double x, double y, double z, T cc, int mode)
{
    int   ix, iy, iz;
    int   xx, yy, zz;

    z = z*RZxy;  
    if (mode==OFF) {
        ix = (int)(x+0.5) - rbound.xmin;
        iy = (int)(y+0.5) - rbound.ymin;
        iz = (int)(z+0.5) - rbound.zmin;
        if (ix>=0 && ix<xs && iy>=0 && iy<ys && iz>=0 && iz<zs){
            gp[iz*xs*ys + iy*xs + ix] = cc;
        }
    }
    else {
        xx = (int)x - rbound.xmin;
        yy = (int)y - rbound.ymin;
        zz = (int)z - rbound.zmin;
        for (ix=xx; ix<=xx+1; ix++)
        for (iy=yy; iy<=yy+1; iy++)
        for (iz=zz; iz<=zz+1; iz++) {
            if (ix>=0 && ix<xs && iy>=0 && iy<ys && iz>=0 && iz<zs){
                gp[iz*xs*ys + iy*xs + ix] = cc;
            }
        }
    }
    return;
}


/**
template <typename T>  void  MSGraph<T>::put_vPoint(double x, double y, double z, T cc, int mode)

グラフィックデータ構造体 gdの任意の 1voxelに値を設定する．

座標の範囲チェックあり．Z軸の歪，境界補整あり．@n
ただし，set_vPoint()は強制的に値を設定するが，put_vPoint()は既に
値が設定されている場合(zeroでない場合)は，指定された値を設定しない．

@param x,y,z  値を設定する xyz座標．
@param cc     点(x,y,z)に設定する点の値(濃度)．
@param mode   @b OFFの場合，(x,y,z)に一番近い点に ccを設定する．
              @b ON の場合，(x,y,z)の周りの格子点全てに ccを設定する．
*/
template <typename T>  void  MSGraph<T>::put_vPoint(double x, double y, double z, T cc, int mode)
{
    int   ix, iy, iz;
    int   xx, yy, zz;

    z = z*RZxy;  
    if (mode==OFF) {
        ix = (int)(x+0.5) - rbound.xmin;
        iy = (int)(y+0.5) - rbound.ymin;
        iz = (int)(z+0.5) - rbound.zmin;
        if (ix>=0 && ix<xs && iy>=0 && iy<ys && iz>=0 && iz<zs){
            if (gp[iz*xs*ys + iy*xs + ix]==zero) gp[iz*xs*ys + iy*xs + ix] = cc;
        }
    }
    else {
        xx = (int)x - rbound.xmin;
        yy = (int)y - rbound.ymin;
        zz = (int)z - rbound.zmin;
        for (ix=xx; ix<=xx+1; ix++)
        for (iy=yy; iy<=yy+1; iy++)
        for (iz=zz; iz<=zz+1; iz++) {
            if (ix>=0 && ix<xs && iy>=0 && iy<ys && iz>=0 && iz<zs){
                if (gp[iz*xs*ys + iy*xs + ix]==zero) gp[iz*xs*ys + iy*xs + ix] = cc;
            }
        }
   }

    return;
}


template <typename T> void  MSGraph<T>::setWindow(double x1, double y1, double x2, double y2)
{
    if (x1==x2 || y1==y2) return;

    wRateX = (xs-1.0)/Xabs(x2-x1);
    wRateY = (ys-1.0)/Xabs(y2-y1);
    wZeroX = Min(x1, x2);
    wZeroY = Max(y1, y2);
}


/**
3D空間にビューポートを設定する．

@param  x1  ビューポートの任意の頂点の x座標
@param  y1  ビューポートの任意の頂点の y座標
@param  z1  ビューポートの任意の頂点の z座標
@param  x2  ビューポートの対角の頂点の x座標
@param  y2  ビューポートの対角の頂点の y座標
@param  z2  ビューポートの対角の頂点の z座標

@par 座標系
@code
                Z
                |
                |
                |/
    X ----------+-
               /|
              /
             / Y
@endcode
*/
template <typename T> void  MSGraph<T>::setWindow3D(double x1, double y1, double z1, double x2, double y2, double z2)
{
    if (x1==x2 || y1==y2 || z1==z2) return;

    wRateX = (xs-1.0)/Xabs(x2-x1);
    wRateY = (ys-1.0)/Xabs(y2-y1);
    wRateZ = (zs-1.0)/Xabs(z2-z1);
    wZeroX = Max(x1, x2);
    wZeroY = Min(y1, y2);
    wZeroZ = Max(z1, z2);
}


// オペレータ
//
template <typename T>  MSGraph<T> MSGraph<T>::operator= (ExMSGraph<T>& s)
{
    xs     = s.xs;
    ys     = s.ys;
    zs     = s.zs;
    gp     = s.gp;
    zero   = s.zero;
    base   = s.base;

    max    = s.max;
    min    = s.min;
    color  = s.color;
    state  = s.state;
    rbound = s.rbound;
    RZxy   = s.RZxy;
    norm   = s.norm;

    wRateX = s.wRateX;
    wRateY = s.wRateY;
    wRateZ = s.wRateZ;
    wZeroX = s.wZeroX;
    wZeroY = s.wZeroY;
    wZeroZ = s.wZeroZ;
    wNowX  = s.wNowX;
    wNowY  = s.wNowY;
    wNowZ  = s.wNowZ;

    return *this;
}



////////////////////////////////////////////////////////////////////////////////////////////
// 簡易表示インターフェイス
//
template <typename T=uWord> class MSGraphDisp : public CVDisplay
{
public:

public:
    MSGraphDisp()  {}
    virtual ~MSGraphDisp() {}

    virtual void  displayNew (MSGraph<T> vp) {}
    virtual void  displayNext(MSGraph<T> vp) {}
};



////////////////////////////////////////////////////////////////////////////////////////////////
// データ変換
//

// ビット和を執れるように設定する．
#define   CH2MG_NORMAL   0   //< ノーマルコピー
#define   CH2MG_NOPARM   1   ///< パラメータ処理（Z軸の歪, rboundの処理）をしない．

#define   MG2CH_NORMAL   0   ///< ノーマルコピー
#define   MG2CH_NOPARM   1   ///< パラメータ処理（Z軸の歪, rboundの処理）をしない．
#define   MG2CH_CONTRAST 2   ///< コントラスト調整を行う
#define   MG2CH_OCTET    4   ///< 8bitへ変換

//template <typename T>  MSGraph<T>  copyCmnHead2MSGraph(CmnHead hd,     unsigned int mode=CH2MG_NORMAL);
//template <typename T>  CmnHead     copyMSGraph2CmnHead(MSGraph<T> mgr, unsigned int mode=MG2CH_NORMAL);

/**
template <typename T>  MSGraph<T> copyCmnHead2MSGraph(CmnHead hd, unsigned int mode=CH2MG_NORMAL, bool cnt=false)

CmnHead から MSGraph<T>のデータを作り出す．@n
MSGraph<T>.gp のメモリ領域は別に作られる．グローバルカウンタが使用可能．

@param  hd    共通ヘッダ
@param  mode  @b CH2MG_NORMAL (0) ノーマルコピー（デフォルト）@n
              @b CH2MG_NORZXY (1)  Z軸の歪を処理しない．
@param  cnt   カウンタを使用するか（デフォルト：使用しない false）

@return 画像データ．エラーの場合は，@b stateメンバに値が設定される．
@retval JBXL_GRAPH_HEADER_ERROR @b state ヘッダエラー
@retval JBXL_GRAPH_MEMORY_ERROR @b state メモリ確保エラー 
@retval JBXL_GRAPH_CANCEL @b state ユーザによる中断
*/
template <typename T>  MSGraph<T> copyCmnHead2MSGraph(CmnHead hd, unsigned int mode=CH2MG_NORMAL, bool cnt=false)
{
    MSGraph<T> vp;
    int kind = hd.kind & 0x00ff;

    vp.init();  // vp.color = GRAPH_COLOR_UNKNOWN;

    if (kind==UN_KNOWN_DATA || hd.kind==HEADER_NONE) {
        vp.state = JBXL_GRAPH_HEADER_ERROR;
        return vp;
    }

    vp.set(hd.xsize, hd.ysize, hd.zsize);
    if (vp.gp==NULL) { vp.state = JBXL_GRAPH_MEMORY_ERROR; return vp;}

    // カラータイプ
    if (kind==JPEG_RGB_DATA || kind==JPEG_MONO_DATA) {
        //if (hd.zsize==3) vp.color = GRAPH_COLOR_PRGB;
        if (hd.zsize==3) vp.color = GRAPH_COLOR_RGB;
    }
    else if (hd.depth==16) {
        vp.color = GRAPH_COLOR_MONO16;
    }

    // カウンタ．ここでは，delete禁止
    CVCounter* counter = NULL;
    if (vp.zs>=10 && cnt) {
        counter = GetUsableGlobalCounter();
        if (counter!=NULL) counter->SetMax(vp.zs/10);
    }

    int ks, js, ln;
    ln = vp.xs*vp.ys;

    if (hd.depth==16){
        sWord* bp = (sWord*)hd.grptr;
        vp.max = vp.min = bp[0];
        for (int k=0; k<vp.zs; k++) {
            ks = k*ln;
            for (int j=0; j<vp.xs*vp.ys; j++) {
                js = j+ ks;
                vp.gp[js] = bp[js];
                vp.max = Max(vp.max, vp.gp[js]);
                vp.min = Min(vp.min, vp.gp[js]);
            }
            
            if (counter!=NULL && k%10==0) {
                counter->StepIt();
                if (counter->isCanceled()) {        //  キャンセル
                    vp.free();
                    vp.state = JBXL_GRAPH_CANCEL;
                    return vp;
                }
            }

        }
    }

    else if (hd.depth<16){ // ==8
        //int uint = (unsigned char)hd.grptr[0];
        vp.max = vp.min = (T)((unsigned char)hd.grptr[0]);
        for (int k=0; k<vp.zs; k++) {
            ks = k*ln;
            for (int j=0; j<vp.xs*vp.ys; j++) {
                js = j + ks;
                vp.gp[js] = (T)((unsigned char)hd.grptr[js]); 
                vp.max = Max(vp.max, vp.gp[js]);
                vp.min = Min(vp.min, vp.gp[js]);
            }

            if (counter!=NULL && k%10==0) {
                counter->StepIt();
                if (counter->isCanceled()) {        //  キャンセル
                    vp.free();
                    vp.state = JBXL_GRAPH_CANCEL;
                    return vp;
                }
            }
        }
    }

    else {
        vp.max = vp.min = (T)((uByte)hd.grptr[0]>>(hd.depth-15));
        for (int k=0; k<vp.zs; k++) {
            ks = k*ln;
            for (int j=0; j<vp.xs*vp.ys; j++) {
                js = j + ks;
                vp.gp[js] = (T)((uByte)hd.grptr[js]>>(hd.depth-15));
                vp.max = Max(vp.max, vp.gp[js]);
                vp.min = Min(vp.min, vp.gp[js]);
            }

            if (counter!=NULL && k%10==0) {
                counter->StepIt();
                if (counter->isCanceled()) {        //  キャンセル
                    vp.free();
                    vp.state = JBXL_GRAPH_CANCEL;
                    return vp;
                }
            }
        }
    }

    // もし存在するなら，ヘッダ部分から Z方向の歪率, rboundを取り出す．
    if (!checkBit(mode, CH2MG_NOPARM)) {
        if (hd.bsize>0 && (kind==CT_DATA || kind==CT_3DM || kind==CT_3D_VOL)) {
            sWord* rz = (sWord*)hd.buf;
            if (rz[9]==RZXY_RATE || checkBit(hd.kind, HAS_RZXY)) {
                double rzm = rz[8]/(double)rz[9];
                if (rzm<5.0 && rzm>0.) vp.RZxy = rzm;
            }

            if (rz[10]!=0 && checkBit(hd.kind, HAS_BASE)) {
                vp.base = (T)rz[10];
            }

            if (checkBit(hd.kind, HAS_RBOUND)) {
                vp.rbound.xmin = rz[6];
                vp.rbound.xmax = rz[7];
                vp.rbound.ymin = rz[4];
                vp.rbound.ymax = rz[5];
                vp.rbound.zmin = rz[2];
                vp.rbound.zmax = rz[3];
            }
        }
    }

    if (counter!=NULL) counter->PutFill();

    return  vp;
}


/**
template <typename T>  CmnHead  copyMSGraph2CmnHead(MSGraph<T>& vp, unsigned int mode=MG2CH_NORMAL, bool cnt=false)

MSGraph<T> から CmnHeadのデータを作り出す．

CmnHead.grptr のメモリ領域は別に作られる．vpに関しては vp.max, vp.minが設定される．@n
グローバルカウンタが使用可能．

@param  vp   コピー元のグラフィックデータ
@param  mode @b MG2CH_NORMAL   (0)  ノーマルコピー（デフォルト）@n
             @b MG2CH_NORZXY   (1)  Z軸の歪を処理しない．@n
             @b MG2CH_CONTRAST (2)  コントラスト調整 @n
             @b MG2CH_OCTET    (4)  8bitへ変換 @n
@param  cnt  カウンタを使用するか（デフォルト：使用しない false）

@return 共通ヘッダ．エラーの場合は @b kind が @b HEADER_NONE になり，@b xsizeに値が設定される．
@retval HEADER_NONE @b kind  エラー
@retval JBXL_GRAPH_HEADER_ERROR @b xsize ヘッダエラー
@retval JBXL_GRAPH_MEMORY_ERROR @b xsize メモリ確保エラー 
@retval JBXL_GRAPH_CANCEL @b xsize ユーザによる中断
*/
template <typename T>  CmnHead  copyMSGraph2CmnHead(MSGraph<T>& vp, unsigned int mode=MG2CH_NORMAL, bool cnt=false)
{
    CmnHead  hd;
    CTHead   ct;

    memset(&ct, 0, sizeof(CTHead));
    memset(&hd, 0, sizeof(CmnHead));

    if (vp.gp==NULL) {
        hd.xsize = JBXL_GRAPH_HEADER_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }

    //
    hd.kind  = 0;   // MSGraphからのコピー時には，ヘッダ情報は既に消失している．
    hd.xsize = ct.xsize = ct.cutright= vp.xs;
    hd.ysize = ct.ysize = ct.cutdown = vp.ys;
    hd.zsize = vp.zs;

    if (checkBit(mode, MG2CH_OCTET)) hd.depth = 8;
    else                             hd.depth = sizeof(T)*8;
    hd.lsize = hd.xsize*hd.ysize*hd.zsize*(hd.depth/8);
    hd.bsize = sizeof(CTHead);

    hd.grptr = (uByte*)malloc(hd.lsize*sizeof(uByte));
    if (hd.grptr==NULL) {
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        hd.kind  = HEADER_NONE;
        return hd;
    }
    hd.buf = (uByte*)malloc(hd.bsize*sizeof(uByte));
    if (hd.buf==NULL) {
        free_CmnHead(&hd);
        hd.xsize = JBXL_GRAPH_MEMORY_ERROR;
        return hd;
    }

    // カウンタ．ここでは，delete禁止
    CVCounter* counter = NULL;

    if (hd.zsize>=5 && cnt) {
        counter = GetUsableGlobalCounter();
        if (counter!=NULL) counter->SetMax(hd.zsize*2/10);
    }

    // 最大値，最小値を求める
    int j, k, ks, js, ln;
    T   max, min;
    max = min = vp.gp[0];

    ln = vp.xs*vp.ys;
    for (k=0; k<vp.zs; k++) {
        ks = k*ln;
        for (j=0; j<vp.ys*vp.xs; j++) {
            js = j + ks;
            min = Min(vp.gp[js], min);
            max = Max(vp.gp[js], max);
        }

        // カウンタ
        if (counter!=NULL && k%10==0) {
            counter->StepIt();
            if (counter->isCanceled()) {        //  キャンセル
                free_CmnHead(&hd);
                hd.xsize = JBXL_GRAPH_CANCEL;
                return hd;
            }
        }
    }
    vp.max = max;
    vp.min = min;

    // ノーマルコピー
    if (mode==MG2CH_NORMAL) {
        memcpy(hd.grptr, vp.gp, hd.lsize);
    }

    // 8bitへ変換
    else if (checkBit(mode, MG2CH_OCTET)) {
        // 255以下はコントラスト調整しない
        if (!checkBit(mode, MG2CH_CONTRAST) && vp.max<=255 && vp.min>=0) {
            max = 255;
            min = 0;
        }

        for (k=0; k<hd.zsize; k++) {
            ks = k*ln;
            for (j=0; j<hd.xsize*hd.ysize; j++) {
                js = j + ks;
                hd.grptr[js] = (uByte)(((vp.gp[js]-min)/(double)(max-min))*255.);
            }
            
            // カウンタ
            if (counter!=NULL && k%10==0) {
                counter->StepIt();
                if (counter->isCanceled()) {        // キャンセル
                    free_CmnHead(&hd);
                    hd.xsize = JBXL_GRAPH_CANCEL;
                    return hd;
                }
            }
        }
    }
    if (counter!=NULL) counter->PutFill();

    memcpy(hd.buf, &ct, hd.bsize);

    // Z方向の歪率, rboundの設定．
    if (!checkBit(mode, MG2CH_NOPARM)) {
        sWord* rz  = (sWord*)hd.buf;
        if (vp.RZxy!=1.0) {     
            hd.kind = hd.kind | HAS_RZXY;
            rz[9] = (sWord)RZXY_RATE;
            rz[8] = (sWord)(vp.RZxy*RZXY_RATE);
        }
        if (vp.base!=0) {
            rz[10] = (sWord)vp.base;
            hd.kind |= HAS_BASE;
        }
        rz[2] = (sWord)vp.rbound.zmin;
        rz[3] = (sWord)vp.rbound.zmax;
        rz[4] = (sWord)vp.rbound.ymin;
        rz[5] = (sWord)vp.rbound.ymax;
        rz[6] = (sWord)vp.rbound.xmin;
        rz[7] = (sWord)vp.rbound.xmax;
        hd.kind |= HAS_RBOUND;
    }

    return hd;
}


// 2D画像の拡大・縮小
//
template <typename T>  MSGraph<T>  scalingMSGraph2D(MSGraph<T> vp, double scale)
{
    MSGraph<T> xp;

    if (scale==0.0) return xp;

    int xs = (int)(vp.xs*scale);
    int ys = (int)(vp.ys*scale);
    int zs = vp.zs;
    int ps = xs*ys;
    if (vp.xs*scale-xs>0.0) xs++;
    if (vp.ys*scale-ys>0.0) ys++;

    xp.set(xs, ys, zs);
    xp.getm();
    xp.color = vp.color;

    for (int kk=0; kk<zs; kk++) {
        int posk = kk*ps;
        //
        for (int jj=0; jj<ys; jj++) {
            int posj = jj*xs + posk;
            double y = jj/scale;
            int j = (int)y;
            int n = j + 1;
            if (j>vp.ys-1) j = vp.ys-1;
            if (n>vp.ys-1) n = vp.ys-1;
            double bt = y - j;
            //
            for (int ii=0; ii<xs; ii++) {
                double x = ii/scale;
                int i = (int)x;
                int m = i + 1;
                if (j>vp.xs-1) j = vp.xs-1;
                if (n>vp.xs-1) n = vp.xs-1;
                double al = x - i;
                
                T a = vp.point(i, j, kk);
                T b = vp.point(m, j, kk);
                T c = vp.point(i, n, kk);
                T d = vp.point(m, n, kk);
                T p = (T)(a*(1.0-al)*(1.0-bt) + b*al*(1.0-bt) + c*(1.0-al)*bt + d*al*bt);

                xp.gp[ii+posj] = p;
            }
        }
    }
        
    return xp;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tools
//

inline  unsigned int  ARGB2Int(unsigned int a, unsigned int r, unsigned int g, unsigned int b)
{
    if (isBigEndian) {      // A-R-G-B
        a = (unsigned int)((a&0x000000ff)<<24);
        r = (unsigned int)((r&0x000000ff)<<16);
        g = (unsigned int)((g&0x000000ff)<<8);
        b = (unsigned int) (b&0x000000ff);
    }
    else {                  // B-G-R-A
        b = (unsigned int)((b&0x000000ff)<<24);
        g = (unsigned int)((g&0x000000ff)<<16);
        r = (unsigned int)((r&0x000000ff)<<8);
        a = (unsigned int) (a&0x000000ff);
    }

    unsigned int c = (unsigned int)(a + r + g + b);
    return  c;
}


#define  RGBA2Int(r, g, b, a)  ARGB2Int((r), (g), (b), (a))
#define  ABGR2Int(a, b, g, r)  ARGB2Int((a), (b), (g), (r))
#define  BGRA2Int(b, g, r, a)  ARGB2Int((b), (g), (r), (a))


inline  uWord  ARGB2Word(uWord a, uWord r, uWord g, uWord b)
{
    if (isBigEndian) {  // A-R-G-B
        a = (int)(((a&0x00ff)>>4)<<12);
        r = (int)(((r&0x00ff)>>4)<<8);
        g = (int)(((g&0x00ff)>>4)<<4);
        b = (int) ((b&0x00ff)>>4);
    }
    else {              // G-B A-R
        g = (int)(((g&0x00ff)>>4)<<12);
        b = (int)(((b&0x00ff)>>4)<<8);
        a = (int)(((a&0x00ff)>>4)<<4);
        r = (int) ((r&0x00ff)>>4);
    }

    uWord c = (uWord)(a + r + g + b);
    return  c;
}


#define  RGBA2Word(r, g, b, a)  ARGB2Word((r), (g), (b), (a))
#define  ABGR2Word(a, b, g, r)  ARGB2Word((a), (b), (g), (r))
#define  BGRA2Word(b, g, r, a)  ARGB2Word((b), (g), (r), (a))


inline  uWord  RGB2Word(uWord r, uWord g, uWord b)
{
    r = (int)(((r&0x00ff)>>3)<<11);
    g = (int)(((g&0x00ff)>>2)<<5);
    b = (int) ((b&0x00ff)>>3);
    uWord c = (uWord)(r + g + b);

    if (isLittleEndian) swap_byte(&c, 2, 2);
    return  c;
}


/**
@file  Gdata.h

@par カラー関数
- inline D3DCOLOR  Dx9Word2RGBA()
- inline D3DCOLOR  Dx9Word2ARGB()
- inline D3DCOLOR  Dx9Word2RGB ()

@see Dx9.h
*/


}       // namespace


#endif

