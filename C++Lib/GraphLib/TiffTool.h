
#ifndef __JBXLPP_TIFF_TOOL_H_
#define __JBXLPP_TIFF_TOOL_H_


/**
@brief    TIFF TOOL Header
@file     TiffTool.h
@version  0.9 
@date     2010/11/9 
@author   Fumi.Iseki (C)

@attention
C から C++へ．まだ書き換え中!!
*/


/////////////////////////////////////////////////////////////////////////////////////////
//

#include "xtools.h"
#include "gdata.h"
#include "Gdata.h"


typedef struct {
    uWord  tag;
    sWord  type;
    uDWord count;
    uDWord value;
    void*  ex_value;
} TIFF_ifd;


typedef struct {
    int       nn;
    TIFF_ifd* ifd;
    unsigned char* img;
} TIFFImage;


extern int  TIFF_Swap_Flag;


#define  MAX_IFD_DEM_NUM        20

#define  TIFF_TYPE_BYTE         1
#define  TIFF_TYPE_ASCII        2
#define  TIFF_TYPE_SOHRT        3
#define  TIFF_TYPE_LONG         4
#define  TIFF_TYPE_RATIONAL     5
#define  TIFF_TYPE_SBYTE        6
#define  TIFF_TYPE_UNDEFINED    7
#define  TIFF_TYPE_SSHORT       8
#define  TIFF_TYPE_SLONG        9
#define  TIFF_TYPE_SRATIONAL    10
#define  TIFF_TYPE_FLOAT        11
#define  TIFF_TYPE_DOUBLE       12

#define  TIFF_TAG_WIDTH         0x0100  // 256
#define  TIFF_TAG_HEIGHT        0x0101  // 257 
#define  TIFF_TAG_DEPTH         0x0102  // 258
#define  TIFF_TAG_COMP          0x0103  // 259
#define  TIFF_TAG_COLOR         0x0106  // 262
#define  TIFF_TAG_STRIP         0x0111  // 273
#define  TIFF_TAG_STRIP_RAW     0x0116  // 278
#define  TIFF_TAG_STRIP_CNT     0x0117  // 279
#define  TIFF_TAG_XRES          0x011a  // 282
#define  TIFF_TAG_YRES          0x011b  // 283
#define  TIFF_TAG_RES_UNIT      0x0128  // 296
#define  TIFF_TAG_COLORMAP      0x0140  // 320

#define  TIFF_TAG_PIXEL         0x0115  // 277
#define  TIFF_TAG_PLANAR        0x011c  // 284
#define  TIFF_TAG_SOFTWARE      0x0131  // 305

#define  TIFF_TAG_TILE_WIDTH    0x0142  // 322
#define  TIFF_TAG_TILE_HEIGHT   0x0143  // 323
#define  TIFF_TAG_TILE_OFFSET   0x0144  // 324
#define  TIFF_TAG_TILE_BYTE     0x0145  // 325
#define  TIFF_TAG_SMPL_FORMAT   0x0153  // 339

#define  TIFF_TAG_COPYRIGHT     0x8298  // 33432

// GeoTIFF
#define  TIFF_TAG_GEO_PXL_SCALE 0x830e  // 33550
#define  TIFF_TAG_GEO_TIEPOINT  0x8482  // 33922
#define  TIFF_TAG_GEO_TRANS     0x85d8  // 34264
#define  TIFF_TAG_GEO_DIR       0x87af  // 34735
#define  TIFF_TAG_GEO_DOUBLE_PM 0x87b0  // 34736
#define  TIFF_TAG_GEO_ASCII_PM  0x87b1  // 34737


//////////////////////////////////////////////////////////////////////////////////////////

TIFF_ifd** read_tiff_file(const char* fname);
TIFF_ifd*  get_tiff_ifd(unsigned char* buf, int num);
TIFF_ifd*  find_tiff_ifd(TIFF_ifd* ifd, unsigned short tag);
void       proc_tiff_ifd(TIFF_ifd* ifd, unsigned char* buf);

void  free_TIFF_ifd(TIFF_ifd* ifd);
void  free_TIFF_ifd_dem(TIFF_ifd** ptr_ifd);

void  print_tiff_ifd(FILE* fp, TIFF_ifd* ifd, int max_values);
void  print_tiff_ifd_indiv(FILE* fp, TIFF_ifd* ifd, int max_values);

char*        get_tiff_ascii_field(TIFF_ifd* ifd, int offset);
unsigned int get_tiff_uint_field(TIFF_ifd* ifd, int offset);
double       get_tiff_double_field(TIFF_ifd* ifd, int offset);

int   get_tiff_type_length(short type);



#endif



