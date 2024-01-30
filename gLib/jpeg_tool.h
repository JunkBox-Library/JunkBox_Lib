#ifndef  __JBXL_JPEG_TOOL_H_
#define  __JBXL_JPEG_TOOL_H_

#include "xtools.h"

#ifndef HAVE_JPEGLIB_H
#ifndef DISABLE_JPEGLIB
#define DISABLE_JPEGLIB
#endif
#endif

#ifdef  DISABLE_JPEGLIB
#undef  ENABLE_JPEGLIB
#endif


#ifdef  ENABLE_JPEGLIB

/**
@brief    JPEG TOOL HEADER
@file     jpeg_tool.h
@version  0.9
@date     2009 3/8
@author   Fumi.Iseki (C)

@attention
this software is based in part on the work of the Independent JPEG Group. http://www.ijg.org/ \n
need libjpeg-turbo-devel
*/


#include "gdata.h"
#include <jpeglib.h>


typedef struct
{
    int         xs;
    int         ys;
    int         col;
    int         state;
    JSAMPLE*    gp;
    JSAMPARRAY  img;
    //
} JPEGImage;


#if JPEG_LIB_VERSION < 80

typedef struct {
  struct jpeg_destination_mgr pub;  // public fields 

  unsigned char** outbuffer;        // target buffer 
  unsigned long*  outsize;
  unsigned char*  newbuffer;        // newly allocated buffer 
  JOCTET * buffer;                  // start of buffer 
  size_t bufsize;
} my_mem_destination_mgr;

typedef my_mem_destination_mgr * my_mem_dest_ptr;

#endif


//////////////////////////////////////////////////////////////////////////////////////////

JPEGImage  read_jpeg_file (const char* fname);
int        write_jpeg_file(const char* fname, JPEGImage jp, int qulty);
int        write_jpeg_mem(unsigned char** buf, unsigned long* len, JPEGImage jp, int qulty);

WSGraph    JPEGImage2WSGraph(JPEGImage jp);
BSGraph    JPEGImage2BSGraph(JPEGImage jp);
JPEGImage  WSGraph2JPEGImage(WSGraph vp);
JPEGImage  BSGraph2JPEGImage(BSGraph vp);

JPEGImage  make_JPEGImage(int xs, int ys, int col);
void       free_JPEGImage(JPEGImage* jp);

//
#if JPEG_LIB_VERSION < 80

METHODDEF(void)     mem_init_destination(j_compress_ptr cinfo);
METHODDEF(boolean)  mem_empty_output_buffer(j_compress_ptr cinfo);
METHODDEF(void)     mem_term_destination(j_compress_ptr cinfo);
GLOBAL(void)        jpeg_mem_dest(j_compress_ptr cinfo, unsigned char** buf, unsigned long* len);

#endif


#endif      // DISABLE_JPEGLIB
#endif      // __JBXL_JPEG_TOOL_H_


