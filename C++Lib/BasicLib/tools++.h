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
template <typename T=double> class ArrayParam
{
private:
    int _size;
    T*  _value;

public:
    ArrayParam(int n = 0)     { init(n);}
    virtual ~ArrayParam(void) { free();}

    void  init(int n = 0) { if (n<0) n = 0; _size = n; if (n>0) _value = (T*)malloc(sizeof(T)*_size); else _value = NULL;}
    void  free(void) { _size = 0; if (_value!=NULL) ::free(_value); _value = NULL;}

    // _value[i] がポインタの場合．（実行注意！） 
    void  free_ptr(void) { for (int i=0; i<_size; i++) if (_value[i]!=NULL){ ::free(_value[i]); _value[i] = NULL;}}

    int   get_size() { return _size;}
    int   get_value(int n) { if (n<0) n=0; else if (n>=_size) n = _size-1; return _value[n];}
    bool  set_value(int n, T val);

    void  dup(ArrayParam<T> a, bool reset = true);
};


/**

*/
template <typename T> bool  ArrayParam<T>::set_value(int n, T val)
{
    if (_size==0) return false; 

    if (n<0) n = 0;
    else if (n>=_size) n = _size - 1;  
    _value[n] = val;

    return true;
}


/**
template <typename T> void ArrayParam<T>::dup(ArrayParam<T> a)

ArrayParam<T> のコピーを作る．

@param  param  コピーするデータ．
@param  del    現在のデータを初期化するか？　true: 初期化する（デフォルト），false: 初期化しない．

*/
template <typename T> void ArrayParam<T>::dup(ArrayParam<T> a, bool del)
{
    DEBUG_MODE PRINT_MESG("ArrayParam<T>::dup(): start.\n");
    int size = a.get_size();
    if (del) this->free();
    this->init(size);

    for (int i=0; i<size; i++) {
        this->_value[i] = a.get_value(i);
    }
    DEBUG_MODE PRINT_MESG("ArrayParam<T>::dup(): end.\n");
}


/**
void  freeArrayParams(ArrayParam<T>* p, int num)

ArrayParam の配列を解放する．
*/
template <typename T> void  freeArrayParams(ArrayParam<T>* p, int num)
{
    DEBUG_MODE PRINT_MESG("freeArrayParams: start.\n");
    if (p!=NULL) {
        for (int i=0; i<num; i++) {
            p[i].free();
        }
        freeNull(p);
    }
    DEBUG_MODE PRINT_MESG("freeArrayParams: end.\n");
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
