/**
*/



#include "tools.h"
#include "buffer.h"


#define  SRTM_SIZE          1201
#define  SRTM_SIZE_SQR      1442401     // 1201*1201
#define  SRTM_NODATA        -32768

#define  SRTM_OCLEVEL       0

#define  SRTM_DATA_DIR      "/home/apache/gsigo/SRTM3/"
#define  SRTM_FNAME_FORMAT  "%sN%02dE%03d.hgt"


sWord* read_hgt_file(int nlat, int elng);
sWord* average(sWord* map_height, int wsize, int hsize, int* chk);
sWord* median (sWord* map_height, int wsize, int hsize, int* chk);
void   set_height(sWord* map_height, int wsize, int hsize, float lat, float lng);
void   print_height(sWord* map_height, int wsize, int hsize, int nfl);

