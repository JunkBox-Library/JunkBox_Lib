#ifndef  __JBXL_CPP_EXTEND_GRAPHIC_DATA_H_
#define  __JBXL_CPP_EXTEND_GRAPHIC_DATA_H_

/**
@brief    メモリ管理機能付グラフィックデータ定義用ヘッダ  
@file     ExGdata.h
@author   Fumi.Iseki
*/

#include  "Gdata.h"


//
namespace jbxl {


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メモリ管理機能付 ExMSGraph, ExCmnHead
//

/**
@file     ExGdata.h

メモリ管理機能
---

@par メモリ管理を付ける事の利点：
色々な関数（クラス）から同時に使用（兼用）できるので，メモリの節約になる．メモリが沢山（無尽蔵に）
あるのなら，その都度コピーして渡した方がアルゴリズムは簡単．

@par 手法：
メモリ（データ）に対して，ペアとなるint型の参照数変数を用意し，メモリへの参照数を参照数変数に格納する．
そのため operator= をオーバーロードする．@n
メモリ管理機能の無い変数から代入される場合は，予め参照数変数を結合しておかないと，メモリ管理
機能無しの状態になる．@n
例えば，msgにメモリ管理機能を持たせるには，以下のようにする．
@code
    int  ref;
    ExMSGraph<> msg;
    msg.setRefer(&ref);
    msg = vp;   // vp: 管理機能無しの変数
@endcode
    また，以下の場合 xpがメモリ（データ）を持つ時は注意．
@code
    ExMSGraph<> msg;
    msg = xp;   // xp: 管理機能ありの変数
    msg = vp;   // vp: 管理機能無しの変数
@endcode
msgは xpの参照数変数を引き継ぎ，メモリ（データ）管理機能付変数となる．@n
msgのメモリ部（データ）が空の場合，msg = vp; では，引き続き参照数変数を使用して，
vpのメモリ（データ）を管理する．@n
msgにメモリ（データ）が存在する場合，msg = vp; でmsgはそのメモリ（データ）への参照を止め，vp をコピーする．@n
すなわち，この場合はメモリ管理機能なしになる．@n
つまり，xpがメモリ（データ）を持たなければ，xp, msgの参照数変数は 1になり，msgは vpのメモリを管理する．@n
xpがメモリを持っていれば，msgはメモリ管理機能なしになる．@n

@par 参照数変数
- 1以上 : メモリ（グラフィックデータ）が存在し，参照されている．
- 0     : メモリ（グラフィックデータ）は存在するが，どこからも参照されていない．
          通常この状態は一時的であり，次のステップで 1 か -1になる．
- -1 : メモリ（グラフィックデータ）は存在しない．
- -2以下 : 通常この状態は -1 に戻される．
  　    
@attention
ディストラクタはメモリを解放しないので，必ず手動で解放すること．
*/


/**
template <typename T=uWord> class ExMSGraph : public MSGraph<T>

    必ず参照数変数(refnoの実体)を指定すること．無くても動くかも知れないが，保証は無い．

    関数の中からディストラクトされるメモリ（のコピー）をリターンする場合は，returnの前に 
    mfree()を実行させること．
*/
template <typename T=uWord> class ExMSGraph : public MSGraph<T>
{
public:
//protected:
    int*    refno;  // メモリ領域への参照オブジェクト数

//
public:
    ExMSGraph(int x, int y=1, int z=1, T v=(T)0, T b=(T)0, double rz=1.0, int* rn=NULL) { set(x, y, z, v, b, rz, rn);}
    ExMSGraph(RBound<int> rb, T v=(T)0, T b=(T)0, double rz=1.0, int* rn=NULL){ set(rb, v, b, rz, rn);}
    ExMSGraph(int* rn=NULL)  { MSGraph<T>::init(); setRefer(rn);}
    virtual ~ExMSGraph(void) {}

    void   set(int x, int y=1, int z=1, T v=(T)0, T b=(T)0, double rz=1.0, int*rn=NULL);
    void   set(RBound<int> rb, T v=(T)0, T b=(T)0, double rz=1.0, int*rn=NULL);
    void   mimicry(ExMSGraph<T> s) { set(s.xs, s.ys, s.zs, s.zero, s.base, s.RZxy, NULL);}

    bool   isNull(void);                                ///< グラフィックデータを持いないか？
    void   free(void);                                  ///< グラフィックデータを開放する（メモリ管理なし兼用）
    void   mfree(bool azero=false);                     ///< グラフィックデータを開放する
    void   ffree(void);                                 ///< グラフィックデータを強制開放する
    void   setRefer(int* rn);

    ExMSGraph<T> operator= (ExMSGraph<T>& s);
    ExMSGraph<T> operator= (MSGraph<T>& s);
};


//
template <typename T>  void  ExMSGraph<T>::set(RBound<int> rb, T v, T b, double rz, int* rn) {
    set(rb.xmax-rb.xmin+1, rb.ymax-rb.ymin+1, rb.zmax-rb.zmin+1, v, b, rz, rn);
    MSGraph<T>::rbound = rb;
    return;
}


//
template <typename T>  void  ExMSGraph<T>::set(int x, int y, int z, T v, T b, double rz, int* rn) 
{ 
    if (z<=0) z = 1;

    MSGraph<T>::getm(x, y, z, v); 
    if (MSGraph<T>::gp==NULL) return;

    MSGraph<T>::base = b;
    if (rz==0.0) MSGraph<T>::RZxy = 1.0;
    else         MSGraph<T>::RZxy = rz;
    MSGraph<T>::rbound.set(0, x-1, 0, y-1, 0, z-1);
    
    if (rn!=NULL) setRefer(rn);
    else if (refno!=NULL) {
        if (MSGraph<T>::gp==NULL) *refno = -1;
        else                      *refno =  1;
    }
    return;
}


//
template <typename T>  void  ExMSGraph<T>::setRefer(int* rn)
{
    refno = rn;
    if (refno!=NULL) {
        if (MSGraph<T>::gp==NULL) *refno = -1;
        else                      *refno =  1;
    }
}


// グラフィックデータを持いないか？
template <typename T>  bool  ExMSGraph<T>::isNull(void) 
{   
    if (MSGraph<T>::gp==NULL) return true;

#ifdef WIN32    
    if (MSGraph<T>::gp==(T*)WIN_DD_NULL) return true;
#endif

    if (refno!=NULL) {
        if (*refno<0) return true;
        else          return false;
    }
    return false;
}


/**
template <typename T>  void  ExMSGraph<T>::free(void) 

グラフィックデータを開放する

メモリ管理をしている場合は，参照数を減らす．
メモリ管理をしていない場合は，強制的に開放する．
*/
template <typename T>  void  ExMSGraph<T>::free(void) 
{  
    if (refno==NULL) {
        if (!isNull()) {
            ::free(MSGraph<T>::gp);
            MSGraph<T>::init();         // gp = NULL;
        }
    }
    else {
        mfree();
    }
    return;
}


/**
template <typename T>  void  ExMSGraph<T>::mfree(bool azero) 

グラフィックデータを開放する

メモリ管理をしている場合は，参照数を減らす．
メモリ管理をしていない場合は，何もしない．

@param  azero  @b true: 参照数 0 の状態を許す．
*/
template <typename T>  void  ExMSGraph<T>::mfree(bool azero) 
{  
    if (refno!=NULL) {
        (*refno)--;
        if (!azero && *refno==0) {
            ::free(MSGraph<T>::gp);
            *refno = -1; 
            MSGraph<T>::init();             // gp = NULL;
        }
        else if (*refno==-1) {
            DEBUG_MODE PRINT_MESG("ExMSGraph<T>::mfree: [警告] 通常ありえない参照数 0\n");
        }
        else if (*refno<-1) {
            *refno = -1;
        }
    }
    return;
}


/**
template <typename T>  void  ExMSGraph<T>::ffree() 

グラフィックデータを強制的に開放する
*/
template <typename T>  void  ExMSGraph<T>::ffree() 
{  
    if (refno==NULL) {
        if (!isNull()) {
            ::free(MSGraph<T>::gp);
        }
    }
    else {
        if (*refno>=0) {
            ::free(MSGraph<T>::gp);
            *refno = -1;
        }
    }
    MSGraph<T>::init();     // gp = NULL;
    return;
}


// オペレータ
//
template <typename T>  ExMSGraph<T> ExMSGraph<T>::operator= (ExMSGraph<T>& s)
{
    ExMSGraph<T> dm = *this;

    MSGraph<T>::xs = s.xs;
    MSGraph<T>::ys = s.ys;
    MSGraph<T>::zs = s.zs;
    MSGraph<T>::gp = s.gp;
    MSGraph<T>::zero = s.zero;
    MSGraph<T>::base = s.base;

    MSGraph<T>::max = s.max;
    MSGraph<T>::min = s.min;
    MSGraph<T>::color = s.color;
    MSGraph<T>::state = s.state;
    MSGraph<T>::rbound = s.rbound;
    MSGraph<T>::RZxy = s.RZxy;
    MSGraph<T>::norm = s.norm;

    MSGraph<T>::wRateX = s.wRateX;
    MSGraph<T>::wRateY = s.wRateY;
    MSGraph<T>::wRateZ = s.wRateZ;
    MSGraph<T>::wZeroX = s.wZeroX;
    MSGraph<T>::wZeroY = s.wZeroY;
    MSGraph<T>::wZeroZ = s.wZeroZ;
    MSGraph<T>::wNowX  = s.wNowX;
    MSGraph<T>::wNowY  = s.wNowY;
    MSGraph<T>::wNowZ  = s.wNowZ;

    if (!s.isNull()) {                          // コピー元はデータを持っている．
        if (s.refno!=NULL) {                        // コピー元は参照数変数を持っている．
            if (*s.refno>=0) {                      // 相手の参照数変数を使う．
                refno = s.refno;
                (*refno)++;
            }
            else {
                DEBUG_MODE PRINT_MESG("ExMSGraph<T>::= (ExMSGraph): コピー元参照数変数の不正!!\n");
            }
        }
        else {                                      // コピー元は参照数変数を持っていない．
            if (refno!=NULL) {                          // 自分の参照数変数を持っている．
                if (*refno<=-1) *refno = 1;             // 自分の参照数変数が未使用ならそれを使う．
                else  refno = NULL;                         // 参照数変数は誰かが使っているので，自分は参照数変数無しになる．
            }   
        }
    }

    else {                                      // コピー元はデータを持っていない
        MSGraph<T>::gp = NULL;
        if (s.refno!=NULL) {                        // コピー元は参照数変数を持っている．
            refno = s.refno;                            // 相手の参照数変数を使う．
            *refno = -1;                                // データは無いので -1 に．
        }                                       
        else {                                      // コピー元は参照数変数を持っていない．
            if (refno!=NULL) {                          // 自分の参照数変数を持っている．
                if (*refno<=-1) *refno = -1;            // 自分の参照数変数が未使用ならそれを使う．
                else refno = NULL;                          // 参照数変数は誰かが使っているので，自分は参照数変数無しになる．
            }
        }
    }

    if (!dm.isNull()) dm.mfree();
    return *this;
}


// オペレータ 
//
template <typename T>  ExMSGraph<T> ExMSGraph<T>::operator= (MSGraph<T>& s)
{ 
    if (!isNull()) mfree();

    MSGraph<T>::xs = s.xs;
    MSGraph<T>::ys = s.ys;
    MSGraph<T>::zs = s.zs;
    MSGraph<T>::gp = s.gp;
    MSGraph<T>::zero = s.zero;
    MSGraph<T>::base = s.base;

    MSGraph<T>::max  = s.max;
    MSGraph<T>::min  = s.min;
    MSGraph<T>::color = s.color;
    MSGraph<T>::state = s.state;
    MSGraph<T>::rbound = s.rbound;
    MSGraph<T>::RZxy = s.RZxy;
    MSGraph<T>::norm = s.norm;

    MSGraph<T>::wRateX = s.wRateX;
    MSGraph<T>::wRateY = s.wRateY;
    MSGraph<T>::wRateZ = s.wRateZ;
    MSGraph<T>::wZeroX = s.wZeroX;
    MSGraph<T>::wZeroY = s.wZeroY;
    MSGraph<T>::wZeroZ = s.wZeroZ;
    MSGraph<T>::wNowX  = s.wNowX;
    MSGraph<T>::wNowY  = s.wNowY;

    if (!s.isNull()) {                          // コピー元はデータを持っている．
        if (refno!=NULL) {                          // 自分の参照数変数を持っている．
            if (*refno<=-1) *refno =  1;            // 自分の参照数変数が未使用ならそれを使う．
            else  refno = NULL;                         // 参照数変数は誰かが使っているので，自分は参照数変数無しになる．
        }   
    }

    else {                                      // コピー元はデータを持っていない
        MSGraph<T>::gp = NULL;
        if (refno!=NULL) {                          // 自分の参照数変数を持っている．
            if (*refno<=-1) *refno = -1;            // 自分の参照数変数が未使用ならそれを使う．
            else refno = NULL;                          // 参照数変数は誰かが使っているので，自分は参照数変数無しになる．
        }
    }

    return *this;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メモリ管理付 CmnHead
//

/**
class  ExCmnHead : public CmnHead 

必ず参照数変数(refnoの実体)を指定すること．無くても動くかも知れないが，保証は無い．

関数の中からディストラクトされるメモリ（のコピー）をリターンする場合は，returnの前に 
mfree()を実行させること．
*/
class  ExCmnHead : public CmnHead 
{
public:
    int     state;
    int     dbyte;

//protected:
    int*    refno;

public:
    ExCmnHead(int x, int y=1, int z=1, int d=8, int* rn=NULL) { set(x, y, z, d, rn);}
    ExCmnHead(RBound<int> rb, int d=8, int* rn=NULL) { set(rb, d, rn);}
    ExCmnHead(int* rn=NULL)  { init(); setRefer(rn);}
    ExCmnHead(ExCmnHead& cmhd) = default; 
    virtual ~ExCmnHead(void) {}

    void   init(void);

    void   set (RBound<int> rb, int d=8, int* rn=NULL);
    void   set (int x, int y=1, int z=1, int d=8, int* rn=NULL);
    void   getm(int x, int y=1, int z=1, int d=8);  ///< グラフィックメモリを獲得する．

    void   clear() {memset(grptr, 0, lsize);}
    uByte& point(int x, int y=0, int z=0) { return grptr[dbyte*(x+xsize*y+xsize*ysize*z)];} ///< 座標(x,y,z)の画素値の参照

    bool   isNull(void);                            ///< グラフィックデータを持いないか？
    void   free(void);                              ///< グラフィックデータを開放する（メモリ管理なし兼用）
    void   mfree(bool azero=false);                 ///< グラフィックデータを開放する
    void   ffree(void);                             ///< グラフィックデータを強制開放する
    void   setRefer(int* rn);
//  void   decRefer() { (*refno)--;}                ///< 次のステップで強制的に +1される場合，予め実行する．

    ExCmnHead operator= (ExCmnHead& m);
    ExCmnHead operator= (CmnHead& m);
};


}       // namespace



#endif  //__JBXL_CPP_EXTEND_GRAPHIC_DATA_H_
