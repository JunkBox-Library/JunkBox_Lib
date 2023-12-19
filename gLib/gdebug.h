
#ifndef  __JBXL_GRAPHIC_DEBUG_H_
#define  __JBXL_GRAPHIC_DEBUG_H_


/**
@brief    グラフィック環境でのデバック用ヘッダ
@file     gdebug.h
@author   Fumi.Iseki (C) 
*/


#include "gio.h"
#include "openX.h"


extern int  DebugMode;


#ifndef DISABLE_X11
    void  disp_img(WSGraph vp, char* mesg);
#endif

void  write_img(char* fn, WSGraph vp, char* mesg);


#ifdef EBUG
    #ifndef DISABLE_X11
        #define DEBUG_DISP_IMG(vp, mesg)   disp_img(vp, mesg)
    #endif
    #define DEBUG_WRITE_IMG(fn, vp, mesg)  write_img(fn, vp, mesg)
#else
    #ifndef DISABLE_X11
        #define DEBUG_DISP_IMG(vp, mesg)   {}
    #endif
    #define DEBUG_WRITE_IMG(fn, vp, mesg)  {}
#endif

#endif      // __JBXL_GRAPHIC_DEBUG_H_

