/**
*/


#ifndef  __CHBMAP_SVR_H_
#define  __CHBMAP_SVR_H_


#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif


#include "xtools.h"

#include "gdata.h"
#include "tiff_tool.h"
#include "jpeg_tool.h"


#define  SERVER_PORT  8080






//////////////////////////////////////////////////////////////////////////

extern int      Vofd, Wofd;
extern int      MaxIdleTime;

extern char*    Hosts_Allow_File;
extern tList*   Allow_IPaddr;

extern char*    MyIPaddr;
extern char*    MyIPaddrNum;
extern char*    ClIPaddr;
extern char*    ClIPaddrNum;






//////////////////////////////////////////////////////////////////////////
// Functions

// sig term
void    sigterm_process(int signal);
JPEGImage get_terrain_texture(TIFF_ifd* ifd, double lon, double lat, int xsz, int ysz, int center);


#endif


