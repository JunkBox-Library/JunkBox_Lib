
/**
@brief    X-Winow 表示用関数
@file     openX.c
@version  2.11
@author   Fumi.Iseki (C)
*/


#include "openX.h"


#ifndef DISABLE_X11


/**
openX  disp_image(WSGraph gd, int lc, int hc, int cflg) 

グラフィックデータ gdをディスプレイ上に表示する．

@param  gd    表示するグラフィックデータ．
@param  lc    コントラスト調整．これより小さい輝度値は表示しない．0より小さい（-1以下の）場合は自動調整する．
@param  hc    コントラスト調整．これより大きい輝度値は表示しない．0以下の場合は自動調整する．
@param  cflg  表示モード  @b MONOC: グレースケール表示．@b COLOR: カラー表示 (未サポート)

@return 表示したウィンドウの識別子．

@attention
正しく表示されない場合は，openX.h の COLOR_DEPTH を変更する．

@bug カラーデプスが8bitでは正しく表示されない．
     表示モードの COLORは完全にサポートされていないので表示がおかしくなる．
*/
openX  disp_image(WSGraph gd, int lc, int hc, int cflg) 
{
    int  i, j, sl, sh;
    char *image;
    int  nc = COLOR_NUM;
    int  width, height, dpth, dpb;
    int  tmp, col;
    unsigned short int* pnt;

    sWord *imagewk;
    XImage  *ximg;
    Visual  *visl;
    Pixmap  pmap;
    openX   xid;

    width   = gd.xs;
    height  = gd.ys;
    imagewk = gd.gp;

    xid = displayOpen(gd.xs, gd.ys, cflg);

    sl = sh = 1; 
    if (hc<=0) sh = -1;
    if (lc<0) {
        sl = -1;
        lc = SINTMAX;
    }
    if (sl<0 || sh<0) {
        for(i=0; i<width*height; i++){
            tmp = *(imagewk + i);
            if (sl<0) lc = Min(lc, tmp); 
            if (sh<0) hc = Max(hc, tmp); 
        }
    }
    hc = Max(hc, lc+2);
  
    visl = DefaultVisual(xid.display, xid.screen);
    dpth = DefaultDepth (xid.display, xid.screen);
    dpb  = dpth/8;
    if (dpb==3 && COLOR_DEPTH>=32) dpb = 4;

    image = (char*)malloc(width*height*dpb);
    for(i=0; i<width*height; i++){
        tmp = *(imagewk + i);
        if (tmp>hc) tmp = hc;  
        if (tmp<lc) tmp = lc;
        if (cflg==MONOC) tmp = (int)((double)(tmp-lc)*(nc-1)/(double)(hc-lc));
        else             tmp = (int)(nc-(double)(tmp-lc)*(nc-1)/(double)(hc-lc)-1);

        col = xid.color_index[tmp];
        if (dpb>=3) {
            for (j=0; j<dpb; j++) image[dpb*i+j] = col;     // 24,32bit
        }
        else {                                              // 16bit
            pnt = (unsigned short int*)&(image[dpb*i]);
            *pnt = col;
        }
    }

    pmap = XCreatePixmap(xid.display, xid.window, width, height, dpth);
    ximg = XCreateImage(xid.display,visl,dpth,ZPixmap,0,(char*)image,width,height,8,0);
    XPutImage(xid.display, pmap, xid.gc, ximg, 0, 0, 0, 0, width, height);
    XSetWindowBackgroundPixmap(xid.display, xid.window, pmap);
    XMapWindow  (xid.display, xid.window);
    XClearWindow(xid.display, xid.window);
    XFlush(xid.display);

    xid.id = 1;
    return xid;
}



/**
void  displayClose(openX xid)

表示したウィンドウを閉じる．

@param  xid  閉じたいウィンドウの識別子．
*/
void  displayClose(openX xid)
{
    if (xid.id >= 0) {
        free(xid.color_index);
        XFreeGC(xid.display, xid.gc);
        XDestroyWindow(xid.display, xid.window);
        XCloseDisplay(xid.display);
        xid.id = -1;
    }
}



openX  displayOpen(int xs, int ys, int cflg)
{
    Window     root_window;
    XSizeHints hint;
    XColor     color;
    Colormap   cmap;
    XSetWindowAttributes at;
    openX xid;

    int  ir, ig, ib;
    unsigned long foreground, background;
    unsigned short values[] = {0x0, 0x4444, 0x8888, 0xcccc, 0xffff};

    hint.x = 0;
    hint.y = 0;
    hint.width  = xs;
    hint.height = ys;
    hint.flags  = PPosition|PSize;

    xid.display = XOpenDisplay(NULL);
    root_window = DefaultRootWindow(xid.display);

    xid.screen  = DefaultScreen(xid.display);
    foreground  = WhitePixel(xid.display, xid.screen);
    background  = BlackPixel(xid.display, xid.screen);

    xid.window  = XCreateSimpleWindow(xid.display, root_window, hint.x, hint.y, 
              hint.width, hint.height, 5, foreground, background);
    XSetStandardProperties(xid.display,xid.window,"openX","openX",None,NULL,0,&hint);

    xid.gc = XCreateGC(xid.display, xid.window, 0, 0);
    XSetBackground(xid.display, xid.gc, background);
    XSetForeground(xid.display, xid.gc, foreground);

    at.backing_store = WhenMapped;
    at.bit_gravity = CenterGravity;
    XChangeWindowAttributes(xid.display, xid.window, CWBackingStore, &at);
    XChangeWindowAttributes(xid.display, xid.window, CWBitGravity,   &at);

    XSelectInput(xid.display, xid.window, ButtonPressMask|KeyPressMask|ExposureMask);
    XMapRaised(xid.display, xid.window);

    cmap = DefaultColormap(xid.display, xid.screen);
    xid.color_index = (long unsigned int*)malloc(COLOR_NUM*sizeof(long));
    for (ir = 0; ir < 5; ir++) {
        for (ig = 0; ig < 5; ig++) {
            for (ib = 0; ib < 5; ib++) {
                if (cflg==MONOC) {  // MONO 
                    color.red = (int)((ir+5.*ig+ib*25.)/(double)COLOR_NUM*65532.);
                    color.green = color.blue = color.red;
                }
                else {
                    color.red   = values[ir];
                    color.green = values[ig];
                    color.blue  = values[ib];
                }
                XAllocColor(xid.display, cmap, &color);
                xid.color_index[ir+5*ig+25*ib] = color.pixel;
            }
        }
    }
 
    xid.id = 0;
    set_color(xid, 0.0, 0.0, 0.0);
    return xid;
}



void  set_color(openX xid, double r, double g, double b)
{
    int ir, ig, ib;

    r = Max(0.0, Min(r, 1.0));
    g = Max(0.0, Min(g, 1.0));
    b = Max(0.0, Min(b, 1.0));

    ir = (int)(4*r + 0.5);
    ig = (int)(4*g + 0.5);
    ib = (int)(4*b + 0.5);

    XSetForeground(xid.display, xid.gc, xid.color_index[ir+5*ig+25*ib]);
}


#endif  // DISABLE_X11
