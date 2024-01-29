#ifndef  __JBXL_CPP_COMMONPP_HEADER_
#define  __JBXL_CPP_COMMONPP_HEADER_

/**
@brief    Common Header for C++
@file     common++.h
@author   Fumi.Iseki (C)
*/

#ifndef CPLUSPLUS
    #define CPLUSPLUS
#endif

#include "common.h"
#include <string>

//
namespace jbxl {


//
// case of C, isNull() is ormal function, see tools.h
//
inline  bool  isNull(void* p) {
    if (p==NULL) return true;

#ifdef WIN32
    if (p==WIN_DD_NULL) return true;
#endif
    
    return false;
}

#ifdef freeNull  // for C
#undef freeNull
#endif
template <typename T> inline void   freeNull(T& p) { if (!jbxl::isNull(p)) ::free(p); p = (T)NULL;}

template <typename T> inline void deleteNull(T& p) { delete p; p = (T)NULL;}


#ifndef WIN32
    #ifndef BOOL
        #define BOOL int
    #endif
#endif


}       // namespace


#endif
