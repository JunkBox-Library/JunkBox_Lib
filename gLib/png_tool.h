#ifndef  __JBXL_PNG_TOOL_H_
#define  __JBXL_PNG_TOOL_H_

#include "xtools.h"

#ifndef HAVE_PNG_H
#ifndef DISABLE_PNG
#define DISABLE_PNG
#endif
#endif

#ifdef  DISABLE_PNG
#undef  ENABLE_PNG
#endif


#ifdef  ENABLE_PNG

/**
@brief    PNG TOOL HEADER
@file     png_tool.h
@version  0.9
@date     2024 7/19
@author   Fumi.Iseki (C)

@attention
*/


#include "gdata.h"
#include <png.h>


typedef struct
{
    int      xs;
    int      ys;
    int      col;
    int      state;
    void*    gp;
    void*    img;
    //
} PNGImage;



//////////////////////////////////////////////////////////////////////////////////////////

PNGImage   read_png_file (const char* fname);
int        write_png_file(const char* fname, PNGImage jp, int qulty);
int        write_png_mem(unsigned char** buf, unsigned long* len, PNGImage jp, int qulty);

WSGraph    PNGImage2WSGraph(PNGImage jp);
BSGraph    PNGImage2BSGraph(PNGImage jp);
PNGImage   WSGraph2PNGImage(WSGraph vp);
PNGImage   BSGraph2PNGImage(BSGraph vp);

PNGImage   make_PNGImage(int xs, int ys, int col);
void       free_PNGImage(PNGImage* jp);


#endif      // DISABLE_PNG
#endif      // __JBXL_PNG_TOOL_H_

