
/**
@brief    SHAPE FILE TOOL
@file     shape_file.c
@author   Fumi.Iseki (C)
*/


#include  "shape_file.h"
#include  "jbxl_state.h"


ShapeIndex* read_shape_index_file(FILE* fp)
{
    int magic, type, rdint;
    size_t rs;
    UNUSED(rs);

    // Magic Number
    fseek(fp, 0L, SEEK_SET);
    rs = fread(&rdint, 4, 1, fp);
    magic = int_from_big_endian((void*)&rdint);
    if (magic!=9994) {
        DEBUG_MODE PRINT_MESG("READ_SHAPE_INDEX_FILE: But Mafic Number = %d\n", magic);
        return NULL;
    }
    
    // Shape Type
    fseek(fp, 32L, SEEK_SET);
    rs = fread(&rdint, 4, 1, fp);
    type = int_from_little_endian((void*)&rdint);

    //
    int i, count = 0;

    fseek(fp, 100L, SEEK_SET);
    rs = fread(&rdint, 4, 1, fp);
    rs = fread(&rdint, 4, 1, fp);
    while(!feof(fp)) {
        count++;
        rs = fread(&rdint, 4, 1, fp);    // next record
        rs = fread(&rdint, 4, 1, fp);    // next record
    }

    ShapeIndex* shpidx = (ShapeIndex*)malloc(count*sizeof(ShapeIndex));
    if (shpidx==NULL) return NULL;
    
    fseek(fp, 100L, SEEK_SET);
    for (i=0; i<count; i++) {
        shpidx[i].datanum = i;
        shpidx[i].type    = type;
        shpidx[i].maxnum  = count;
        rs = fread(&rdint, 4, 1, fp);
        shpidx[i].offset  = int_from_big_endian((void*)&rdint);
        rs = fread(&rdint, 4, 1, fp);
        shpidx[i].size    = int_from_big_endian((void*)&rdint);
    }
    
    return shpidx;
}



BSGraph*  read_shape_main_file(ShapeIndex* idx, BSGraph* mp, vector og, vector dd, FILE* fp)
{
    int magic, type, rdint;
    unsigned char rdbyte[8];
    double xmin, ymin, zmin, xmax, ymax, zmax;
    size_t rs;
    UNUSED(rs);

    // Magic Number
    fseek(fp, 0L, SEEK_SET);
    rs = fread(&rdint, 4, 1, fp);
    magic = int_from_big_endian((void*)&rdint);
    if (magic!=9994) {
        DEBUG_MODE PRINT_MESG("READ_SHAPE_MAIN_FILE: But Mafic Number = %d\n", magic);
        return NULL;
    }
    
    // Shape Type
    fseek(fp, 32L, SEEK_SET);
    rs = fread(&rdint, 4, 1, fp);
    type = int_from_little_endian((void*)&rdint);

    if (idx->type!=type) {
        DEBUG_MODE PRINT_MESG("READ_SHAPE_MAIN_FILE: Type mismatch with index file %d != %d\n", type, idx->type);
        return NULL;
    }

    if (idx->type!=SHAPE_TYPE_POLYGON) {
        DEBUG_MODE PRINT_MESG("READ_SHAPE_MAIN_FILE: Unsupported Shape Type = %d\n", idx->type);
        return NULL;
    }

    // Bounding Box
    fseek(fp, 36L, SEEK_SET);
    rs = fread(rdbyte, 8, 1, fp);
    xmin = double_from_little_endian((void*)rdbyte);
    rs = fread(rdbyte, 8, 1, fp);
    ymin = double_from_little_endian((void*)rdbyte);
    rs = fread(rdbyte, 8, 1, fp);
    xmax = double_from_little_endian((void*)rdbyte);
    rs = fread(rdbyte, 8, 1, fp);
    ymax = double_from_little_endian((void*)rdbyte);
    rs = fread(rdbyte, 8, 1, fp);
    zmin = double_from_little_endian((void*)rdbyte);
    rs = fread(rdbyte, 8, 1, fp);
    zmax = double_from_little_endian((void*)rdbyte);

    //
    if (mp==NULL) {
        mp = (BSGraph*)malloc(sizeof(BSGraph));
        memset(mp, 0, sizeof(BSGraph));
    }
    if (mp->gp==NULL) {
        if (dd.x<=0.0) dd.x = 1.0;
        if (dd.y<=0.0) dd.y = 1.0;
        if (dd.z<=0.0) dd.z = 1.0;
        int xsize = (int)((xmax - xmin)/dd.x) + 1;
        int ysize = (int)((ymax - ymin)/dd.y) + 1;
        int zsize = (int)((zmax - zmin)/dd.z) + 1;
        *mp = make_BSGraph(xsize, ysize, zsize);
        og  = set_vector(xmin, ymax, zmin);
    }

    //
    switch(idx->type) {

      case SHAPE_TYPE_POLYGON :
        draw_shape_polygon(idx, mp, og, dd, 255, fp);
        break;
    }

    mp->state = JBXL_NORMAL;
    return mp;
}



void  draw_shape_polygon(ShapeIndex* idx, BSGraph* mp, vector og, vector dd, int cc, FILE* fp)
{
    size_t rs;
    UNUSED(rs);

    int i, j, k, rdint;
    int kk = (int)(og.z + 0.5);     // color
    if (kk<0 || kk>mp->zs-1) kk = 0;

    ShapeIndex* sx = idx;

    for (i=0; i<idx->maxnum; i++) {
        fseek(fp, sx->offset*2+8, SEEK_SET);
        rs = fread(&rdint, 4, 1, fp);
        int type = int_from_little_endian((void*)&rdint);
        if (type!=SHAPE_TYPE_POLYGON) return;

        fseek(fp, 32L, SEEK_CUR);
        rs = fread(&rdint, 4, 1, fp);
        int parts  = int_from_little_endian((void*)&rdint);
        rs = fread(&rdint, 4, 1, fp);
        int points = int_from_little_endian((void*)&rdint);
    
        for (j=0; j<parts; j++) {
            fseek(fp, sx->offset*2+52+j*4, SEEK_SET);       // offset*2 + 8 + 44 + j*4
            rs = fread(&rdint, 4, 1, fp);
            int sttidx = int_from_little_endian((void*)&rdint);
            int endidx = points - 1;
    
            if (j<parts-1) {
                rs = fread(&rdint, 4, 1, fp);
                endidx = int_from_little_endian((void*)&rdint) - 1;
            }

            double xx, yy, ox, oy;
            fseek(fp, sx->offset*2+52+parts*4+sttidx*16, SEEK_SET); // offset*2 + 8 + 44 + parts*4 + sttidx*16

            for (k=sttidx; k<=endidx; k++) {
                rs = fread(&xx, 8, 1, fp);
                rs = fread(&yy, 8, 1, fp);
                xx = double_from_little_endian((void*)&xx);
                yy = double_from_little_endian((void*)&yy);

                if (k!=sttidx) {
                    int oi = (int)((ox-og.x)/dd.x + 0.5);
                    int oj = (int)((og.y-oy)/dd.y + 0.5);
                    int ii = (int)((xx-og.x)/dd.x + 0.5);
                    int jj = (int)((og.y-yy)/dd.y + 0.5);
                    bline3d(*mp, oi, oj, kk, ii, jj, kk, cc);
                }
                ox = xx;
                oy = yy;
            }
        }
        sx++;
    }
}



