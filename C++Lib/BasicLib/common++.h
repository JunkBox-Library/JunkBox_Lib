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


// for 3D
#define   JBXL_3D_FORMAT_NONE       0
#define   JBXL_3D_FORMAT_DAE        1
#define   JBXL_3D_FORMAT_OBJ        2
#define   JBXL_3D_FORMAT_STL_A      3
#define   JBXL_3D_FORMAT_STL_B      4

#define   JBXL_3D_ENGINE_NONE       0
#define   JBXL_3D_ENGINE_UNITY      1
#define   JBXL_3D_ENGINE_UE         2

}       // namespace


#endif
