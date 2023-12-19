
#include "common.h"
#include "tools.h"
//#include "buffer.h"

#include "gdata.h"
#include "tiff_tool.h"
#include "jpeg_tool.h"




int main(int argc, char** argv)
{
    int  nn = 0;
    TIFF_ifd** ptr_ifd = NULL;
    TIFF_ifd*  ifd = NULL;

    if (argc<2) {
        fprintf(stderr,"Usage %s tiff_file \n", argv[0]);
        exit(1);
    }

    
    ptr_ifd = read_tiff_file(argv[1]);
    if (ptr_ifd==NULL) {
        print_message("EXIT!! Is %s TIFF File?\n", argv[1]);
        exit(1);
    }

    TIFF_ifd** ptr = ptr_ifd;
    while((*ptr)!=NULL) {
        ptr++;
        nn++;
    }
    //print_message("Image header found. %d\n", nn);
    //if (nn>0 && ptr_ifd[nn-1]->value!=0) {
    //  print_message("All image header was not read!! Images > %d\n", nn);
    //}

    if (nn!=1) {
        print_message("not support this image number = %d\n", nn);
        free_TIFF_ifd_dem(ptr_ifd);
        exit(1);
    }

    print_tiff_ifd(stdout, *ptr_ifd, 20);

    //
    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_WIDTH);
    int width = get_tiff_uint_field(ifd, 0);

    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_HEIGHT);
    int height = get_tiff_uint_field(ifd, 0);

    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_DEPTH);
    int depth = get_tiff_uint_field(ifd, 0);
    
    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_PIXEL);
    int dbyte = get_tiff_uint_field(ifd, 0);
    
    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_GEO_PXL_SCALE);
    double dlng = get_tiff_double_field(ifd, 0);
    double dlat = get_tiff_double_field(ifd, 1);

    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_GEO_TIEPOINT);
    double mpx0 = get_tiff_double_field(ifd, 0);
    double mpy0 = get_tiff_double_field(ifd, 1);
    double lng0 = get_tiff_double_field(ifd, 3);
    double lat0 = get_tiff_double_field(ifd, 4);


    double lngs = 140.1;
    double lats = 36.1;
    double lnge = 140.15;
    double late = 36.05;




    
    if (depth==32 && dbyte==1) {
        // Mono
    }
    else if (depth=8 && dbyte==3) {
        // RGB
        MSGraph* mp = (MSGraph*)((*ptr_ifd)->ex_value);

        int mpxs = (int)( (lngs - lng0)/dlng + mpx0);
        int mpys = (int)(-(lats - lat0)/dlat + mpy0);
        int mpxe = (int)( (lnge - lng0)/dlng + mpx0);
        int mpye = (int)(-(late - lat0)/dlat + mpy0);
        

mpxs = width/2;
mpys = height/2;
mpxe = mpxs + 1024;
mpye = mpys + 1024;


        // Tiff -> BSGraph
        if (mpxs>=0 && mpxe<width && mpys>=0 && mpye<height) {
            int xs = mpxe - mpxs + 1;
            int ys = mpye - mpys + 1;
            if (xs>0 && ys>0) {
                BSGraph bp = make_BSGraph(xs, ys, 3);
                int ps = xs*ys;
                int i, j;
                for (j=0; j<ys; j++) {
                    int jj = j*xs;
                    int yy = (mpys+j)*mp->xs;
                    for (i=0; i<xs; i++) {
                        int ii = jj + i;
                        int xx = (yy + mpxs + i)*3;
                        bp.gp[ii]      = mp->gp[xx];
                        bp.gp[ii+ps]   = mp->gp[xx+1];
                        bp.gp[ii+ps*2] = mp->gp[xx+2];
                    }
                }

                JPEGImage jpg = BSGraph2JPEGImage(bp);
                free_BSGraph(&bp);

                write_jpeg_file("out_sample.jpg", jpg, 100);
                free_JPEGImage(&jpg);
            }
        }
        print_message("%d %d %d %d %04x\n", mp->xs, mp->ys, mp->zs, mp->depth, mp->gp);
    }
    else {
        print_message("not support color mode. %d:%d\n", depth, dbyte);
        exit(1);
    }

//printf("\n%d %d\n", depth, dbyte);
//printf("%f %f, %f %f, %f %f\n", dx, dy, mx, my, fx, fy);

    free_TIFF_ifd_dem(ptr_ifd);

    return 0;
}

