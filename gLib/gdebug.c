
/**
@brief    グラフィック環境でのデバック用ライブラリ
@file     gdebug.c
@author   Fumi.Iseki (C) 
*/


#include "gdebug.h"


#ifndef  DISABLE_X11

void  disp_img(WSGraph vp, char* mesg)
{
    openX  xd;

    if (DebugMode==OFF) return;

    fprintf(stderr, "%s", mesg);
    fflush(stderr);
    xd = disp_image(vp, 0, 0, MONOC);
    getchar();
    displayClose(xd);
}

#endif



void  write_img(char* fn, WSGraph vp, char* mesg)
{
    if (DebugMode==OFF) return;

    fprintf(stderr, "%s", mesg);
    fflush(stderr);
    write_wsg_file(fn, vp);
}


