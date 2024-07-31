#ifndef  __JBXL_CPP_TOOLSPP_H_
#define  __JBXL_CPP_TOOLSPP_H_

/**
@brief    ツールライブラリ ヘッダ for C++
@file     tools++.h
@author   Fumi.Iseki (C)
*/

#include "common++.h"
#include "tools.h"
#include "xtools.h"


//
namespace jbxl {


/////////////////////////////////////////////////////////////////////////////////////////////
// Classes
//


/**
 * 汎用配列パラメータ

*/
//template <typename T=double> class ArrayParam
template <typename T> class ArrayParam
{
private:
    int _size;
    T*  _value;

public:
    ArrayParam(int n = 0)     { init(n);}
    virtual ~ArrayParam(void) {}    // ディストラクタではなく，free() で解放する．

    void  init(int n = 0);
    void  free(void);
    void  free_ptr(void);           // _value[i] がポインタの場合，各ポイント先を開放

    int   get_size(void) { return _size;}
    T     get_value(int n);
    bool  set_value(int n, T val);

    void  dup(ArrayParam<T> a, bool del = true);
};


template <typename T> void  ArrayParam<T>::init(int n)
{
    if (n<0) n = 0;
    _size = n;
    if (_size==0) {
        _value = NULL;
        return;
    }
    if (n>0) _value = (T*)malloc(sizeof(T)*_size);
    else     _value = NULL;

    return;
}


template <typename T> void  ArrayParam<T>::free(void)
{
    if (_size>0) {
        _size = 0;
        //DEBUG_MODE PRINT_MESG("ArrayParam<T>::free: _value = %016x\n", _value);
        if (_value!=NULL) ::free(_value);
    }
    _value = NULL;
    return;
}


/**
_value[i] がポインタの場合．（実行注意！）@n
_value 自体は freeしない．
*/
template <typename T> void  ArrayParam<T>::free_ptr(void)
{
    if (_size<=0 || _value==NULL) return;

    for (int i=0; i<_size; i++) {
        if (_value[i]!=NULL){
            ::free(_value[i]);
            _value[i] = NULL;
        }
    }
    return;
}


template <typename T> T  ArrayParam<T>::get_value(int n)
{
    if (_size<=0 || _value==NULL) return (T)0;

    if (n<0) n = 0;
    else if (n>=_size) n = _size - 1;

    return _value[n];
}


/**

*/
template <typename T> bool  ArrayParam<T>::set_value(int n, T val)
{
    if (n>=_size || n<0) {
        PRINT_MESG("WARNING: ArrayParam<T>::set_value: size missmatch (%d !< %d)\n", n, _size);
    }

    if (_size<=0 || _value==NULL) return false;

    if (n<0) n = 0;
    else if (n>=_size) n = _size - 1;
    _value[n] = val;

    return true;
}


/**
template <typename T> void ArrayParam<T>::dup(ArrayParam<T> a, bool del)

ArrayParam<T> のコピーを作る．
既に何かデータが入っている場合は, del を trueにする．
メモリ確保直後（_value の値が不定）の場合に del を trueにすると，セグメンテーションエラーを起こす．

@param  param  コピーするデータ．
@param  del    最初にデータを free()するか？　true: 最初に free()する（デフォルト），false: free() しない．
*/
template <typename T> void ArrayParam<T>::dup(ArrayParam<T> a, bool del)
{
    if (del) this->free();
    this->init(a.get_size());

    for (int i=0; i<this->_size; i++) {
        this->_value[i] = a.get_value(i);
    }
}


/**
void  freeArrayParams(ArrayParam<T>* p, int num)

ArrayParam の配列を解放する．@n
この関数呼び出し後に，必ず p = NULL とすること．
*/
template <typename T> void  freeArrayParams(ArrayParam<T>* p, int num)
{
    if (p!=NULL) {
        for (int i=0; i<num; i++) {
            p[i].free();
        }
        ::free(p);
    }
}


/**
 * Class Base64
 *
 *unsigned char* を返す encode(), decode() は ::free() が必要
 */
class Base64
{
public:
    Base64(void) { init();}
    virtual ~Base64(void)  { free();}

private:
    void    init(void) { }
    void    free(void) { }

protected:

public:
    unsigned char* encode(unsigned char* str, int  sz) { return encode_base64(str, sz);}
    unsigned char* decode(unsigned char* str, int* sz) { return decode_base64(str, sz);}
    Buffer         encode(Buffer buf) { return encode_base64_Buffer(buf);}
    Buffer         decode(Buffer buf) { return decode_base64_Buffer(buf);}

    Buffer         encode_Buffer_bin(unsigned char* str, int  sz, int nopad=FALSE) { return encode_base64_Buffer_bin(str, sz, nopad);}
};



/////////////////////////////////////////////////////////////////////////////////////////////
//
//

/**
非推奨
use  get_local_timestamp(time(0), "%Y-%m-%dT%H:%M:%SZ"));   need free()

inline  char*  GetLocalTime(char deli1='-', char deli2='T', char deli3=':', char deli4='Z')        ///< not free()
{
    return get_localtime(deli1, deli2, deli3, deli4);
}
*/



/////////////////////////////////////////////////////////////////////////////////////////////
// by tList
//

template <typename T> tList* add_tList_object(tList* lt, T obj)
{
    T* pp = new T();
    *pp = obj;
    lt  = add_tList_node_bystr(lt, 0, 0, NULL, NULL, (void*)pp, sizeof(T));

    return lt;
}


template <typename T> void   del_tList_object(tList** lp)
{
    tList* lt = *lp;

    while(lt!=NULL) {
        T* pp = (T*)lt->ldat.ptr;
        pp->free();
        delete(pp);
        lt->ldat.ptr = NULL;
        lt->ldat.sz  = 0;
        lt = lt->next;
    }
    del_tList(lp);
}



/////////////////////////////////////////////////////////////////////////////////////////////
// for MS Windows

#ifdef WIN32

#ifndef va_start
    #include  <stdarg.h>
#endif

void  DisPatcher(int sno=0, ...);

#endif    // WIN32



}        // namespace


#endif
