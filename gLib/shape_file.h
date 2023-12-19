
#ifndef  __JBL_SHAPE_FILE_H_
#define  __JBL_SHAPE_FILE_H_

/**
@brief    SHAPE FILE TOOL Header
@file     shape_file.h
@author   Fumi.Iseki (C)
*/


#include  "gio.h"
#include  "graph.h"

//#include  "jpeg_tool.h"


#define SHAPE_TYPE_NULL_SHAPE   0 
#define SHAPE_TYPE_POINT        1 
#define SHAPE_TYPE_POLY_LINE    3 
#define SHAPE_TYPE_POLYGON      5
#define SHAPE_TYPE_MULTI_POINT  8
#define SHAPE_TYPE_POINTZ       11
#define SHAPE_TYPE_MULTI_PATCH  31


typedef struct {
    int  datanum;
    int  offset;
    int  size;
    int  type;
    int  maxnum;
} ShapeIndex;



////////////////////////////////////////////////////////////////////////////////////////////////////

ShapeIndex* read_shape_index_file(FILE* fp);
BSGraph*    read_shape_main_file(ShapeIndex* idx, BSGraph* mp, vector og, vector dd, FILE* fp);

void  draw_shape_polygon(ShapeIndex* idx, BSGraph* mp, vector og, vector dd, int cc, FILE* fp);



#endif


