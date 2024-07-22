#ifndef  __JBXL_JP2K_TOOL_H_
#define  __JBXL_JP2K_TOOL_H_

/**
@brief    JP2K TOOL HEADER

@file     jp2k_tool.h
@version  0.9
@date     2014 9/24
@author   Fumi.Iseki (C)

@attention
this software is based on OpenJPEG. http://www.openjpeg.org/
*/

#include "xtools.h"

#ifndef HAVE_OPENJPEG_H
#ifndef DISABLE_OPENJPEG
#define DISABLE_OPENJPEG
#endif
#endif

#ifdef  DISABLE_OPENJPEG
#undef  ENABLE_OPENJPEG
#endif


#ifdef  ENABLE_OPENJPEG

#ifndef OPENJPEG_VER
  #define OPENJPEG_VER  12
#endif

#define JP2K_VER_12     12
#define JP2K_VER_14     14
#define JP2K_VER_15     15
#define JP2K_VER_20     20
#define JP2K_VER_21     21


#include "gdata.h"


#if OPENJPEG_VER <= JP2K_VER_15
  #undef OFF
  #ifdef WIN32
    #ifdef _DEBUG
      #pragma  comment(lib, "LibOpenJPEGd.lib")
    #else
      #pragma  comment(lib, "LibOpenJPEG.lib")
    #endif
  #endif
#elif OPENJPEG_VER == JP2K_VER_21
  #ifdef WIN32
    #pragma  comment(lib, "openjp2.lib")
  #endif
#else       // default JP2K_VER_20
  #ifdef WIN32
    #pragma  comment(lib, "openjp2.lib")
  #endif
#endif

#include <openjpeg.h>


#define JP2K_MAGIC_RFC3745_JP2  "\x00\x00\x00\x0c\x6a\x50\x20\x20\x0d\x0a\x87\x0a"
#define JP2K_MAGIC_JP2          "\x0d\x0a\x87\x0a"
#define JP2K_MAGIC_J2K          "\xff\x4f\xff\x51"

#define JP2K_FMT_NONE           0
#define JP2K_FMT_JP2            1
#define JP2K_FMT_J2K            2
#define JP2K_FMT_JPT            3


typedef struct
{
    int             xs;
    int             ys;
    int             ws;
    int             hs;
    int             col;
    int             cmode;
    int             state;
    opj_image_t*    image;
    //
} JP2KImage;


//////////////////////////////////////////////////////////////////////////////////////////
void      init_jp2k(JP2KImage* jp);
void      free_jp2k(JP2KImage* jp);
void      setup_jp2k(JP2KImage* jp);
int       get_jp2k_format(uByte* header);

JP2KImage read_jp2k_file(const char* fname);

#if   OPENJPEG_VER <= JP2K_VER_15
JP2KImage read_jp2k_data(unsigned char* data, int len, int format);
#elif OPENJPEG_VER >= JP2K_VER_21
JP2KImage read_jp2k_data(const char* fname, int format);
#else               //JP2K_VER_20
JP2KImage read_jp2k_data(FILE* fp, int format);
#endif

//int     write_jp2k_file(const char* fname, JP2KImage* jp, int qulty);
//int     write_jp2k_mem(unsigned char** buf, unsigned long* len, JP2KImage* jp, int qulty);

BSGraph   jp2k_toBSGraph(JP2KImage jp);
//JP2KImage  BSGraph2_jp2k(BSGraph vp);
//JP2KImage  make_jp2k(int xs, int ys, int col);


#endif      // ENABLE_OPENJPEG
#endif      // __JBXL_JP2K_TOOL_H_

