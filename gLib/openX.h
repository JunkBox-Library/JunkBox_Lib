#ifndef  __JBXL_OPEN_X11_H_ 
#define  __JBXL_OPEN_X11_H_


#include "gdata.h"


#ifdef X_DISPLAY_MISSING
#ifndef DISABLE_X11
#define DISABLE_X11
#endif
#endif

#ifdef  DISABLE_X11
#undef  ENABLE_X11
#endif


#ifndef DISABLE_X11

/**
@brief    X-Winow 表示用関数ヘッダ
@file     openX.h
@version  2.0
@author   Fumi.Iseki (C)
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>


// 画像が正しく表示されない場合は 32に変更する．
//#define  COLOR_DEPTH  24
#define  COLOR_DEPTH  32

typedef struct {
    int      id;
    Display* display;
    Window   window;
    GC       gc;
    int      screen;
    unsigned long* color_index;
} openX;



#define  MONOC        0
#define  COLOR        1
#define  COLOR_NUM  125


openX  disp_image(WSGraph gd, int lc, int hc, int cflg);
void   displayClose(openX xid);

void   set_color(openX xid, double r, double g, double b);
openX  displayOpen(int xs, int ys, int cflg);

#define disp_close(xd)  displayClose(xd)
#define check_image(vp) {openX x;x=disp_image((vp),0,0,0);getchar();displayClose(x);}



#endif  // DISABLE_X11
#endif  // __JBXL_OPEN_X11_H_ 

