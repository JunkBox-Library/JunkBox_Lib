
/**
@brief    TIFF TOOL
@file     TiffTool.cpp
@version  0.9 
@date     2010/11/9 
@author   Fumi.Iseki (C)

@attention
C から C++へ．まだ書き換え中!!
*/


/////////////////////////////////////////////////////////////////////////////////////////
//
//

#include "TiffTool.h"


int  TIFF_Swap_Flag = FALSE;


/**
TIFF_ifd**  read_tiff_file(const char* fname)

TIFF形式の画像データを読みこむ．

@param  fname  読み込むファイル名
@return TIFFImage データ
*/
TIFF_ifd**  read_tiff_file(const char* fname)
{
    TIFF_ifd** ptr_ifd = NULL;
    unsigned long int size;
    int  i;

    ptr_ifd = (TIFF_ifd**)malloc((MAX_IFD_DEM_NUM+1)*sizeof(TIFF_ifd*));
    if (ptr_ifd==NULL) return NULL;
    for (i=0; i<MAX_IFD_DEM_NUM+1; i++) ptr_ifd[i] = NULL;
        
    unsigned char* buf = read_file(fname, &size);
    if (size<=0 || buf==NULL) {
        free(ptr_ifd);
        freeNull(buf);
        return NULL;
    }

    int num = 0;
    do {
        ptr_ifd[num] = get_tiff_ifd(buf, num+1);
        if (ptr_ifd[num]==NULL) break;
        num++;
    } while(num<MAX_IFD_DEM_NUM && ptr_ifd[num-1]->value!=0);

    //if (num==MAX_IFD_DEM_NUM && ptr_ifd[num-1]->value!=0) {
    //  PRINT_MESG("READ_TIFF_FILE: included image file number is over %d!!\n", MAX_IFD_DEM_NUM);
    //}

    for (i=0; i<num; i++) {
        proc_tiff_ifd(ptr_ifd[i], buf);
    }

    freeNull(buf);
    return ptr_ifd;
}


/**
TIFF_ifd*  get_tiff_ifd(unsigned char* buf, int num)

TIFF形式のデータから num番目の IFDデータを取り出す．

@param  buf  TIFFデータの格納された Buffer型変数
@param  num  何番目の IFDデータを取り出すか？

@return IFDデータ (ifd)
@retval ifd->tga==0 is header
@retval ifd->type  is seq number of this image (num)
@retval ifd->count is number of IFDs in this image
@retval ifd->value is next image IFD offset. if this is 0, next image does not exist
*/
TIFF_ifd*  get_tiff_ifd(unsigned char* buf, int num)
{
    TIFF_ifd* ifd = NULL;
    int  i, k;

    // Endian
    if (buf[0]=='I' && buf[1]=='I') {
        if (is_big_endian()) TIFF_Swap_Flag = TRUE;
    }
    else if (buf[0]=='M' && buf[1]=='M') {
        if (is_little_endian()) TIFF_Swap_Flag = TRUE;
    }
    else {
        return NULL;
    }

    unsigned char* ptr = buf + 2;
    short version = *((short*)ptr);
    if (TIFF_Swap_Flag) version = swaps(version);
    if (version!=42) {
        return NULL;
    }

    ptr += 2;
    unsigned int offset = *((unsigned int*)ptr);
    if (TIFF_Swap_Flag) offset = swapl(offset);

    if (num<0) num = 1;
    k = 0;
    while (k!=num && offset!=0) {
        k++;
        ptr = buf + offset;
        short nn = *((short*)ptr);
        if (TIFF_Swap_Flag) nn = swaps(nn);
        ptr += 2;

        if (k==num) {
            ifd = (TIFF_ifd*)malloc(sizeof(TIFF_ifd)*(nn+1));
            if (ifd==NULL) {
                return NULL;
            }

            memset(&ifd[0], 0, 12);
            ifd[0].type  = num;
            ifd[0].count = nn;

            for (i=1; i<=nn; i++) {
                if (k==num) {
                    memcpy(&ifd[i], ptr, 12);
                    if (TIFF_Swap_Flag) {
                        ifd[i].tag   = swaps(ifd[i].tag);
                        ifd[i].type  = swaps(ifd[i].type);
                        ifd[i].count = swapl(ifd[i].count);
                        ifd[i].value = swapl(ifd[i].value);
                    }
                    ifd[i].ex_value = NULL;
                }
                ptr += 12;
            }
        }
        else {
            for (i=0; i<nn; i++) ptr += 12;
        }

        offset = *((unsigned int*)ptr);
        if (TIFF_Swap_Flag) offset = swapl(offset);
    }

    if (ifd!=NULL) ifd[0].value = offset;
    return ifd;
}


void  proc_tiff_ifd(TIFF_ifd* ptr, unsigned char* buf)
{
    int i, j;
    int cnt = ptr->count;
    int width=0, height=0, depth=1, bdepth=1, comp=1, color=0;
    int tilew=0, tileh=0;

    MSGraph vp;
    TIFF_ifd* ifd = ptr + 1;
    TIFF_ifd* strip_ifd = NULL;

    memset(&vp, 0, sizeof(MSGraph));

    //
    for (i=1; i<=cnt; i++) {

        int offsize = get_tiff_type_length(ifd->type)*ifd->count;
        if (offsize>4) {
            //PRINT_MESG("Malloc Ex_Value. Tag = %d, Length = %d\n", ifd->tag, offsize);
            ifd->ex_value = (void*)malloc(offsize);
            memcpy(ifd->ex_value, buf + ifd->value, offsize);
        }

        // Tag
        switch(ifd->tag) {

          case TIFF_TAG_WIDTH:
            width  = get_tiff_uint_field(ifd, 0);
            break;

          case TIFF_TAG_HEIGHT:
            height = get_tiff_uint_field(ifd, 0);
            break;

          case TIFF_TAG_DEPTH:
            if (ifd->count==1) depth = ifd->value;
            else if (ifd->count>=2) {
                for (j=0, depth=0; j<(int)ifd->count; j++) {
                    depth += get_tiff_uint_field(ifd, j);
                }
            }
            bdepth = (depth+7)/8;
            break;

          case TIFF_TAG_COMP:
            comp = get_tiff_uint_field(ifd, 0);
            if (comp!=1) {
                //PRINT_MESG("PROC_TIFF_IFD: In this version, I can not processes compressed TIFF imgae!!\n");
                ptr->type = -1;
            }
            break;

          case TIFF_TAG_COLOR:
            color = get_tiff_uint_field(ifd, 0);
            if (color==3) {
                //PRINT_MESG("PROC_TIFF_IFD: In this version, I can not processes Color Map!!\n");
                ptr->type = -1;
            }
            break;


          // STRIP
          case TIFF_TAG_STRIP:
            vp = make_MSGraph(width, height, 1, depth);
            if (vp.gp!=NULL) {
                if (ifd->count==1) {
                    memcpy(vp.gp, buf+ifd->value, width*height*bdepth);
                }
                else {
                    strip_ifd = ifd;
                }
            }
            else {
                ptr->type = -2;
            }
            break;

          case TIFF_TAG_STRIP_CNT:
            if (ifd->count>1) {
                unsigned char* img = (unsigned char*)vp.gp;
                for (j=0; j<(int)ifd->count; j++) {
                    int size    = get_tiff_uint_field(ifd, j);
                    int img_ptr = get_tiff_uint_field(strip_ifd, j);
                    memcpy(img, buf+img_ptr, size);
                    img += size;
                }
            }
            break;

          // TILE
          case TIFF_TAG_TILE_WIDTH:
            tilew = get_tiff_uint_field(ifd, 0);
            break;

          case TIFF_TAG_TILE_HEIGHT:
            tileh = get_tiff_uint_field(ifd, 0);
            break;

          case TIFF_TAG_TILE_OFFSET:
            if (tilew>0 && tileh>0) {
                int xnum = (width +tilew-1)/tilew;
                int ynum = (height+tileh-1)/tileh;
                if (vp.gp!=NULL) free_MSGraph(&vp);

                vp = make_MSGraph(tilew*xnum, tileh*ynum, 1, depth);
                if (vp.gp!=NULL) {
                    MSGraph mp = make_MSGraph(tilew, tileh, 1, depth);
                    int size = tilew*tileh*bdepth;

                    for (j=0; j<(int)ifd->count; j++) {             // Block
                        int img_ptr = get_tiff_uint_field(ifd, j);
                        memcpy(mp.gp, buf+img_ptr, size);

                        int k, l, m;
                        int ii = j%xnum;
                        int jj = j/xnum;
                        for (k=0; k<tileh; k++) {
                            int kk = k*tilew;
                            int yy = (jj*tileh + k)*vp.xs;
                            for (l=0; l<tilew; l++) {
                                int ll = (kk + l)*bdepth;
                                int xx = (yy + ii*tilew + l)*bdepth;
                                for (m=0; m<bdepth; m++) {
                                    vp.gp[xx+m] = mp.gp[ll+m];
                                }
                            }
                        }
                    }
                    free_MSGraph(&mp);
                }
                else {
                    ptr->type = -2;
                }
            }
            break;

          case TIFF_TAG_TILE_BYTE:
            {
                int block = tilew*tileh*bdepth;
                for (j=0; j<(int)ifd->count; j++) {
                    int size = get_tiff_uint_field(ifd, j);
                    if (block!=size) {
                        PRINT_MESG("PROC_TIFF_IFD: Not match Tile Size %d != %d\n", block, size);
                        ptr->type = -1;
                    }
                }
            }
            break;
        }

        ifd++;
    }

    if (ptr->type<0) {
        free_MSGraph(&vp);
    }
    else {
        if (vp.gp!=NULL) {
            ptr->ex_value = (void*)malloc(sizeof(MSGraph));
            memcpy(ptr->ex_value, &vp, sizeof(MSGraph));
        }
    }
    return;
}


void  print_tiff_ifd(FILE* fp, TIFF_ifd* ifd, int max_values)
{
    int  i;
    if (max_values<0) max_values = 0;

    if (ifd->tag==0) {
        int num = ifd->count;
        ifd++;
        for (i=1; i<=num; i++) {
            print_tiff_ifd_indiv(fp, ifd, max_values);
            ifd++;
        }
    }
    else {
        print_tiff_ifd_indiv(fp, ifd, max_values);
    }

}


void  print_tiff_ifd_indiv(FILE* fp, TIFF_ifd* ifd, int max_values)
{
    int  i;
    if (max_values<0) max_values = 0;

    fprintf(fp, "%5d %2d %d -> ", ifd->tag, ifd->type, ifd->count);

    if (ifd->ex_value!=NULL) {
        int count = Min((int)ifd->count, max_values);

        if (ifd->type==TIFF_TYPE_ASCII) {
            fprintf(fp, " %s", get_tiff_ascii_field(ifd, 0));
        }
        else if (ifd->type==TIFF_TYPE_SOHRT || ifd->type==TIFF_TYPE_SSHORT) {
            for (i=0; i<count; i++) fprintf(fp, " %d", get_tiff_uint_field(ifd, i));
        }
        else if (ifd->type==TIFF_TYPE_LONG || ifd->type==TIFF_TYPE_SLONG) {
            for (i=0; i<count; i++) fprintf(fp, " %d", get_tiff_uint_field(ifd, i));
        }
        else if (ifd->type==TIFF_TYPE_RATIONAL || ifd->type==TIFF_TYPE_SRATIONAL) {
            for (i=0; i<count; i++) fprintf(fp, " %d/%d", get_tiff_uint_field(ifd, 2*i), get_tiff_uint_field(ifd, 2*i+1));
        }
        else if (ifd->type==TIFF_TYPE_FLOAT || ifd->type==TIFF_TYPE_DOUBLE) {
            for (i=0; i<count; i++) fprintf(fp, " %f", get_tiff_double_field(ifd, i));
        }
        else if (count>0) fprintf(fp, " PRINT_TIFF_IFD_INDIV: not supported IFD type => %d", ifd->type);

        if (ifd->type!=TIFF_TYPE_ASCII && count<(int)ifd->count && count!=0) fprintf(fp, " .......");
    }
    else fprintf(fp, " %d", ifd->value);

    fprintf(fp, "\n");
    fflush(fp);
}


void  free_TIFF_ifd(TIFF_ifd* ifd)
{
    if (ifd==NULL) return;
    freeNull(ifd->ex_value);
    free(ifd);
}


void  free_TIFF_ifd_dem(TIFF_ifd** ptr_ifd)
{
    if (ptr_ifd==NULL) return;

    TIFF_ifd** ptr = ptr_ifd;
    while ((*ptr)!=NULL) {
        if ((*ptr)->tag==0) {
            if ((*ptr)->ex_value!=NULL) {
                free_MSGraph((MSGraph*)(*ptr)->ex_value);
            }
        }

        free_TIFF_ifd(*ptr);
        ptr++;
    }

    free(ptr_ifd);
}


char*  get_tiff_ascii_field(TIFF_ifd* ifd, int offset)
{
    if (ifd==NULL) return NULL;
    if (ifd->type!=TIFF_TYPE_ASCII) return NULL;
    if (offset<0) offset = 0;

    int len = get_tiff_type_length(ifd->type);
    unsigned char* ptr = NULL;

    if (len*ifd->count>4 && ifd->ex_value!=NULL) ptr = (unsigned char*)(ifd->ex_value);
    else ptr = (unsigned char*)&(ifd->value);

    return (char*)(ptr + offset);
}


unsigned int  get_tiff_uint_field(TIFF_ifd* ifd, int offset)
{
    if (ifd==NULL) return 0;
    if (ifd->type!=TIFF_TYPE_SOHRT    && ifd->type!=TIFF_TYPE_SSHORT &&
        ifd->type!=TIFF_TYPE_LONG     && ifd->type!=TIFF_TYPE_SLONG  &&
        ifd->type!=TIFF_TYPE_RATIONAL && ifd->type!=TIFF_TYPE_SRATIONAL) return 0;
    if (offset<0) offset = 0;

    unsigned int ret = 0;
    int len = get_tiff_type_length(ifd->type);
    unsigned char* ptr = NULL;

    if (len*ifd->count>4 && ifd->ex_value!=NULL) ptr = (unsigned char*)(ifd->ex_value);
    else ptr = (unsigned char*)&(ifd->value);

    if (ifd->type==TIFF_TYPE_RATIONAL || ifd->type==TIFF_TYPE_SRATIONAL) len /= 2;

    if (len==2) {
        if (TIFF_Swap_Flag) ret = swaps(*(unsigned short*)(ptr + offset*2));
        else                ret = *(unsigned short*)(ptr + offset*2);
    }
    else if (len==4) {
        if (TIFF_Swap_Flag) ret = swapl(*(unsigned int*)(ptr + offset*4));
        else                ret = *(unsigned int*)(ptr + offset*4);
    }

    return ret;
}


double  get_tiff_double_field(TIFF_ifd* ifd, int offset)
{
    if (ifd==NULL) return 0.0;
    if (ifd->type!=TIFF_TYPE_FLOAT && ifd->type!=TIFF_TYPE_DOUBLE) return 0.0;
    if (offset<0) offset = 0;

    double ret = 0.0;
    int    len = get_tiff_type_length(ifd->type);
    unsigned char* ptr = NULL;

    if (len*ifd->count>4 && ifd->ex_value!=NULL) ptr = (unsigned char*)(ifd->ex_value);
    else ptr = (unsigned char*)&(ifd->value);

    if (len==4) {
        ret = *(float*)(ptr + offset*4);
    }
    else if (len==8) {
        ret = *(double*)(ptr + offset*8);
    }

    return ret;
}


int  get_tiff_type_length(short type)
{
    int  byte_num[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};

    if (type>0 && type<13) return byte_num[type];
    return 0;
}


TIFF_ifd*  find_tiff_ifd(TIFF_ifd* ifd, unsigned short tag)
{
    if (ifd==NULL && tag<=0) return NULL;

    int i;
    int count = ifd->count;

    if (ifd->tag!=0) {
        if (ifd->tag==tag) return ifd;
        else return NULL;
    }

    ifd++;
    for (i=0; i<count; i++) {
        if (ifd->tag==tag) return ifd;
        ifd++;
    }
    return NULL;    
}


