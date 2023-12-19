
/*
*/



#include "read_srtm.h"




int main(int argc, char** argv)
{
    int   i, wsize, hsize, oclv;
    float lat, lng, rate;

    sWord* hgt = NULL;
    sWord* map_height;


    oclv = SRTM_OCLEVEL;
    lat = lng = 0.0;
    rate  = 1.0;
    wsize = 256;

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i],"-lati")) {if (i!=argc-1) lat   = atof(argv[i+1]);}
        if (!strcmp(argv[i],"-n"))    {if (i!=argc-1) lat   = atof(argv[i+1]);}
        if (!strcmp(argv[i],"-long")) {if (i!=argc-1) lng   = atof(argv[i+1]);}
        if (!strcmp(argv[i],"-e"))    {if (i!=argc-1) lng   = atof(argv[i+1]);}
        if (!strcmp(argv[i],"-size")) {if (i!=argc-1) wsize = atoi(argv[i+1]);}
        if (!strcmp(argv[i],"-s"))    {if (i!=argc-1) wsize = atoi(argv[i+1]);}
        if (!strcmp(argv[i],"-rate")) {if (i!=argc-1) rate  = atof(argv[i+1]);}
        if (!strcmp(argv[i],"-r"))    {if (i!=argc-1) rate  = atof(argv[i+1]);}
        if (!strcmp(argv[i],"-oclv")) {if (i!=argc-1) oclv  = atoi(argv[i+1]);}
        if (!strcmp(argv[i],"-o"))    {if (i!=argc-1) oclv  = atoi(argv[i+1]);}
    }
    if (wsize<=0.0) wsize = 256;
    hsize = (int)(wsize*rate) + 1;
    if (wsize>=SRTM_SIZE) wsize = SRTM_SIZE - 1;
    if (hsize>=SRTM_SIZE) hsize = SRTM_SIZE - 1;

    map_height = (sWord*)malloc(wsize*hsize*sizeof(sWord));
    if (map_height==NULL) {
        print_message("No more memory!! (%d)\n", wsize*hsize*sizeof(sWord));
        exit(1);
    }
    for (i=0; i<wsize*hsize; i++) map_height[i] = 0;


    set_height(map_height, wsize, hsize, lat, lng);

    int chk;
    do {
        map_height = median (map_height, wsize, hsize, &chk);
        //map_height = average(map_height, wsize, hsize, &chk);
    } while (chk==ON);

    print_height(map_height, wsize, hsize, oclv);

    return 0;
}



sWord*  median(sWord* map_height, int wsize, int hsize, int* chk)
{
    int i, j, mapj, hh, nn;
    int median[9];

    if (chk!=NULL) *chk = OFF;

    sWord* hgt = (sWord*)malloc(wsize*hsize*sizeof(sWord));
    if (hgt==NULL) return map_height;


    for (j=0; j<hsize; j++) {
        mapj = j*wsize;
        for (i=0; i<wsize; i++) {
            if (map_height[mapj+i]==SRTM_NODATA) {
                hh = nn = 0;
                if (i!=0) { 
                    int gg = map_height[mapj+i-1];  
                    if (gg!=SRTM_NODATA) {
                        median[nn] = gg;
                        nn++;
                    }
                }
                if (i!=wsize-1) { 
                    int gg = map_height[mapj+i+1];  
                    if (gg!=SRTM_NODATA) {
                        median[nn] = gg;
                        nn++;
                    }
                }
                if (j!=0) { 
                    int gg = map_height[mapj-wsize+i];  
                    if (gg!=SRTM_NODATA) {
                        median[nn] = gg;
                        nn++;
                    }
                }
                if (j!=hsize-1) { 
                    int gg = map_height[mapj+wsize+i];  
                    if (gg!=SRTM_NODATA) {
                        median[nn] = gg;
                        nn++;
                    }
                }

                if (i!=0 && j!=0) { 
                    int gg = map_height[mapj-wsize+i-1];    
                    if (gg!=SRTM_NODATA) {
                        median[nn] = gg;
                        nn++;
                    }
                }
                if (i!=0 && j!=hsize-1) { 
                    int gg = map_height[mapj+wsize+i-1];    
                    if (gg!=SRTM_NODATA) {
                        median[nn] = gg;
                        nn++;
                    }
                }
                if (i!=wsize-1 && j!=0) { 
                    int gg = map_height[mapj-wsize+i+1];    
                    if (gg!=SRTM_NODATA) {
                        median[nn] = gg;
                        nn++;
                    }
                }
                if (i!=wsize-1 && j!=hsize-1) { 
                    int gg = map_height[mapj+wsize+i+1];    
                    if (gg!=SRTM_NODATA) {
                        median[nn] = gg;
                        nn++;
                    }
                }

                if (nn!=0) {
                    int k, l;
                    for (k=0; k<nn-1; k++) {
                        for (l=k+1; l<nn; l++) {
                            if (median[k]>median[l]) {
                                int swp = median[k];
                                median[k] = median[l];
                                median[l] = swp;
                            }
                        }
                    }
                    hgt[mapj+i] = median[nn/2];
                }
                else {
                    hgt[mapj+i] = SRTM_NODATA;
                    if (chk!=NULL) *chk = ON;
                }
            }
            else {
                hgt[mapj+i] = map_height[mapj+i];
            }
        }
    }
    free(map_height);

    return hgt;
}



sWord*  average(sWord* map_height, int wsize, int hsize, int* chk)
{
    int i, j, mapj, hh, nn;

    if (chk!=NULL) *chk = OFF;

    sWord* hgt = (sWord*)malloc(wsize*hsize*sizeof(sWord));
    if (hgt==NULL) return map_height;

    for (j=0; j<hsize; j++) {
        mapj = j*wsize;
        for (i=0; i<wsize; i++) {
            if (map_height[mapj+i]==SRTM_NODATA) {
                hh = nn = 0;
                if (i!=0) { 
                    int gg = map_height[mapj+i-1];  
                    if (gg!=SRTM_NODATA) {
                        hh += 2*gg;
                        nn += 2;
                    }
                }
                if (i!=wsize-1) { 
                    int gg = map_height[mapj+i+1];  
                    if (gg!=SRTM_NODATA) {
                        hh += 2*gg;
                        nn += 2;
                    }
                }
                if (j!=0) { 
                    int gg = map_height[mapj-wsize+i];  
                    if (gg!=SRTM_NODATA) {
                        hh += 2*gg;
                        nn += 2;
                    }
                }
                if (j!=hsize-1) { 
                    int gg = map_height[mapj+wsize+i];  
                    if (gg!=SRTM_NODATA) {
                        hh += 2*gg;
                        nn += 2;
                    }
                }

                if (i!=0 && j!=0) { 
                    int gg = map_height[mapj-wsize+i-1];    
                    if (gg!=SRTM_NODATA) {
                        hh += gg;
                        nn++;
                    }
                }
                if (i!=0 && j!=hsize-1) { 
                    int gg = map_height[mapj+wsize+i-1];    
                    if (gg!=SRTM_NODATA) {
                        hh += gg;
                        nn++;
                    }
                }
                if (i!=wsize-1 && j!=0) { 
                    int gg = map_height[mapj-wsize+i+1];    
                    if (gg!=SRTM_NODATA) {
                        hh += gg;
                        nn++;
                    }
                }
                if (i!=wsize-1 && j!=hsize-1) { 
                    int gg = map_height[mapj+wsize+i+1];    
                    if (gg!=SRTM_NODATA) {
                        hh += gg;
                        nn++;
                    }
                }

                if (nn!=0) hgt[mapj+i] = (int)((float)hh/nn+0.5);
                else {
                    hgt[mapj+i] = SRTM_NODATA;
                    if (chk!=NULL) *chk = ON;
                }
            }
            else {
                hgt[mapj+i] = map_height[mapj+i];
            }
        }
    }
    free(map_height);

    return hgt;
}




void  set_height(sWord* map_height, int wsize, int hsize, float lat, float lng)
{
    int i, j;
    sWord* hgt;

    int ndg = (int)lat;
    int edg = (int)lng;

    lat = lat - ndg;
    lng = lng - edg;
    int sx = (int)(lng*(SRTM_SIZE-1)  + 0.5);                   // 0 - 1200
    int sy = (SRTM_SIZE-1) - (int)(lat*(SRTM_SIZE-1) + 0.5);    // 0 - 1200
    int ex = sx + wsize;
    int ey = sy + hsize;
    int mx = Min(ex, SRTM_SIZE-1);
    int my = Min(ey, SRTM_SIZE-1);


    hgt = read_hgt_file(ndg, edg);
    if (hgt!=NULL) {
        for (j=sy; j<my; j++) {
            int mapj = (j-sy)*wsize;
            int hgtj = j*SRTM_SIZE;
            for (i=sx; i<mx; i++) {
                map_height[mapj + i - sx] = hgt[hgtj + i];
            }
        }
        free(hgt);
    }

    if (ex>=SRTM_SIZE) {
        hgt = read_hgt_file(ndg, edg+1);
        if (hgt!=NULL) {
            for (j=sy; j<my; j++) {
                int mapj = (j-sy)*wsize;
                int hgtj = j*SRTM_SIZE;
                for (i=0; i<ex-SRTM_SIZE+1; i++) {
                    map_height[mapj + i + mx - sx] = hgt[hgtj + i];
                }
            }
            free(hgt);
        }
    }

    if (ey>=SRTM_SIZE) {
        hgt = read_hgt_file(ndg-1, edg);
        if (hgt!=NULL) {
            for (j=0; j<ey-SRTM_SIZE+1; j++) {
                int mapj = (j+my-sy)*wsize;
                int hgtj = j*SRTM_SIZE;
                for (i=sx; i<mx; i++) {
                    map_height[mapj + i - sx] = hgt[hgtj + i];
                }
            }
            free(hgt);
        }
    }

    if (ex>=SRTM_SIZE && ey>=SRTM_SIZE) {
        hgt = read_hgt_file(ndg-1, edg+1);
        if (hgt!=NULL) {
            for (j=0; j<ey-SRTM_SIZE+1; j++) {
                int mapj = (j+my-sy)*wsize;
                int hgtj = j*SRTM_SIZE;
                for (i=0; i<ex-SRTM_SIZE+1; i++) {
                    map_height[mapj + i + mx - sx] = hgt[hgtj + i];
                }
            }
            free(hgt);
        }
    }
}





void  print_height(sWord* map_height, int wsize, int hsize, int oclv)
{
    int  i, j;

    for (j=0; j<hsize; j++) {
        int mapj = j*wsize;
        for (i=0; i<wsize; i++) {
            int height = map_height[mapj + i];
            if (height<=0) height += oclv;
            printf("%06d ", height);
        }
        printf("\n");
    }
}







/**
  北緯 nlat, 東経 elngのデータを読み出す．

*/
sWord* read_hgt_file(int nlat, int elng)
{
    int  i;
    char fname[256];

    int dtsz = sizeof(sWord);

    sWord* hgt = (sWord*)malloc(SRTM_SIZE_SQR*dtsz);
    if (hgt==NULL) return NULL;
    for (i=0; i<SRTM_SIZE_SQR; i++) hgt[i] = 0;

    sprintf(fname, SRTM_FNAME_FORMAT, SRTM_DATA_DIR, nlat, elng);
    FILE* fp = fopen(fname, "rb");
    if (fp==NULL) return hgt;
    fread((void*)hgt, SRTM_SIZE_SQR*dtsz, 1, fp);
    fclose(fp);

    ntoh_ar(hgt, SRTM_SIZE_SQR*dtsz);
    return hgt;
}


