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

#ifndef PNG_SIGNATURE_SIZE
#define  PNG_SIGNATURE_SIZE  8
#endif


typedef struct
{
    int      xs;
    int      ys;
    int      col;
    int      state;
    uByte    type;
    uByte*   gp;
    //
} PNGImage;



//////////////////////////////////////////////////////////////////////////////////////////

PNGImage   read_png_file (const char* fname);
int        write_png_file(const char* fname, PNGImage* png);

WSGraph    PNGImage2WSGraph(PNGImage png);
BSGraph    PNGImage2BSGraph(PNGImage png);
PNGImage   WSGraph2PNGImage(WSGraph vp);
PNGImage   BSGraph2PNGImage(BSGraph vp);

void       free_PNGImage(PNGImage* png);


#endif      // DISABLE_PNG
#endif      // __JBXL_PNG_TOOL_H_

