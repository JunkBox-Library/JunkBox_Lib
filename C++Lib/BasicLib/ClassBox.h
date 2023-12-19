
#ifndef  __JBXL_CPP_CLASS_BOX_H_
#define  __JBXL_CPP_CLASS_BOX_H_


/**
@brief    C++ ToolBox of Class 
@file     ClassBox.h
@author   Fumi.Iseki (C)

@par 内容
便利なクラスや関数，構造体集
*/


#include  "common++.h"
#include  "tools++.h"



//
namespace jbxl {



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 仮想フレームクラス　Virtual Frame 
class   CVFrame;

extern  CVFrame*  GLFrame;        ///< グローバルディスプレイインターフェイス

/**
 * 上位のフレームクラスを使用する際のアクセスポイントとなる，仮想的なクラス．@n
 * このクラスのインスタンスを直接使用することは殆ど無い．
 */
class  CVFrame
{
public:
    CVFrame() {}
    virtual ~CVFrame() {}
};


/// グローバルドキュメントのセット
inline void  SetGlobalFrame(CVFrame* frm) { GLFrame = frm;}

/// グローバルドキュメントのクリア
inline void  ClearGlobalFrame() { GLFrame = NULL;}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 仮想表示クラス　Virtual Display 
class   CVDisplay;

extern  CVDisplay*  GLDisplay;        ///< グローバルディスプレイインターフェイス

/**
 * 上位の表示クラスを使用する際のアクセスポイントとなる，仮想的なクラス．@n
 * このクラスのインスタンスを直接使用することは殆ど無い．
*/
class  CVDisplay
{
public:
    const char* title;

public:
    CVDisplay() { title = NULL;}
    virtual ~CVDisplay() {}
};


/// グローバルドキュメントのセット
inline void  SetGlobalDisplay(CVDisplay* disp) { GLDisplay = disp;}

/// グローバルドキュメントのクリア
inline void  ClearGlobalDisplay() { GLDisplay = NULL;}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 仮想テキストドキュメントクラス　Virtual Text Document
class   CVTextDocument;

extern  CVTextDocument*  GLTextDocument;        ///< グローバル テキストドキュメント インターフェイス

/**
 * 上位のテキストドキュメントクラスを使用する際のアクセスポイントとなる，仮想的なクラス．@n
 * このクラスのインスタンスを直接使用することは殆ど無い．
*/
class  CVTextDocument
{
public:

public:
    CVTextDocument() { m_locked = FALSE;}
    virtual ~CVTextDocument() {}

protected:
    BOOL    m_locked;

public:
    virtual void  lprintBuffer(Buffer, int) {}           // ロック付き. int mode
    virtual void  lprintString(char*,  int) {}
    virtual void  lprintFormat(char*, ...)  {}
    virtual void  lfprintFormat(int, char*, ...) {}

    virtual void  printBuffer(Buffer, int) {}            // ロックなし
    virtual void  printString(char*,  int) {}
    virtual void  printFormat(char*, ...)  {}
    virtual void  fprintFormat(int, char*, ...) {}

    virtual void  lock(void)   { m_locked = TRUE;}
    virtual void  unlock(void) { m_locked = FALSE;}
};


/// グローバルドキュメントのセット
inline void  SetGlobalTextDocument(CVTextDocument* doc) { GLTextDocument = doc;}

/// グローバルドキュメントのクリア
inline void  ClearGlobalTextDocument() { GLTextDocument = NULL;}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 仮想計量クラス　Virtual Counter 
class   CVCounter;

extern  CVCounter*  GLCounter;        ///< グローバルカウンタ

/**
 * 上位のカウンタークラスを使用する際のアクセスポイントとなる，仮想的なクラス．
 * このクラスのインスタンスを直接使用することは殆ど無い．
 * 例えば，上位クラスとしては CProgressBarDLG を参照せよ．
*/
class  CVCounter
{
public:
    int        max;       ///< カウンタの最大値（0〜）
    int        pos;       ///< カウンタの値
    int        fill;      ///< ローカルターゲット（取り敢えず到達すべき目標）
    bool    cancel;       ///< カウンターがキャンセルされた．
    bool    start;        ///< カウンタはスタートしている
    const char* title;

public:
    CVCounter() {
        max    = 0;
        pos    = 0;
        fill   = 0;
        title  = NULL;
        cancel = false;
        start  = false;
    }

    virtual ~CVCounter() {}

    virtual void  Init()        {max=pos=fill=0; title=""; cancel=false;};
    virtual void  Clear()        {Init();}
    virtual void  SetMax(int m) {max = fill = m;}                ///< カウンタの最大値（最終目標）を設定
    virtual void  SetFill(int m){fill = m;}                      ///< 取り敢えずの目標（最短目標）を設定
    virtual void  SetPos(int p) {pos = p;}                       ///< カウンタの現在位置を設定    
    virtual void  PutFill()     {pos = fill;}                    ///< 取り敢えずの目標（最短目標）までカウンタを進める．
    virtual int   GetMax()      {return max;}                    ///< カウンタの最大値（最終目標）を得る
    virtual int   GetFill()     {return fill;}                   ///< 取り敢えずの目標（最短目標）を得る．
    virtual int   GetPos()      {return pos;}                    ///< カウンタの現在位置を得る
    
    virtual void  Start(int m=100, char* tn=NULL) {SetMax(m); title=tn; start=true;}    ///< カウンタのスタート & タイトル定義
    virtual void  Stop() {max=pos=fill=0; title=NULL; start=false;}    ///< カウンタのストップ
    virtual void  StepIt(int n=1) {pos+=n;}                      ///< カウンタのメモリを増やす

    virtual bool  isCanceled()    {return cancel;}               ///< カウンタがオペレータにより，キャンセルされたか
    virtual void  ResetRate(int n, int m) {fill=m; pos+=m-n;}    ///< nの目盛り幅を mで読み替える．ここでの定義はあまり意味は無い．
    virtual void  SetTitle(char* tn)    {title=tn;}              ///< カウンタのタイトルを設定

    virtual CVCounter* GetUsableCounter() {return this;}         ///< 現在使用可能な目盛りの有効領域を確保
    virtual CVCounter* MakeChildCounter(int n) {ResetRate(n,n); SetMax(n); return this;}    ///< 子カウンタの作成（有効領域を再定義）
    virtual void  DeleteChildCounter() {}                        ///< 子カウンタの削除（有効領域の無効化）

};


/// 現在有効なグローバルカウンタを得る．（子カウンタを得るかもしれない）
inline CVCounter*  GetUsableGlobalCounter() 
{ 
    if (GLCounter!=NULL) return GLCounter->GetUsableCounter(); 
    else return NULL;
}

/// グローバルカウンタのセット
inline void  SetGlobalCounter(CVCounter* counter) { GLCounter = counter;}

/// グローバルカウンタのクリア
inline void  ClearGlobalCounter() { GLCounter = NULL;}




}        // namespace



#endif
