

#ifndef  __EXTRACT_GRAPH_H
#define  __EXTRACT_GRAPH_H



#include  "common++.h"
#include  "tools++.h"
#include  "xtools++.h"

#include  "JpegTool.h"
#include  "Gio.h"
#include  "Graph.h"
#include  "Gmt.h"



#define  WORK_COLOR     10000
#define  EDGE_SIZE      30
#define  MAX_GRAY_SCALE 255



tList*  extract_graph(JPEGImage jp);

tList*  cut_graph(tList* lv, MSGraph<sWord> gd);
tList*  graph2jpeg(tList* lp);

tList*  rotate_graph(tList* lc, tList* lv);
double  get_grad_graph(PCoordinate<int> pc);

tList*  vertex_graph(MSGraph<sWord> jp);
int     background_threshold(MSGraph<sWord> vp, int ith, int dth, int size, float rate, int mode, int work_color);
void    delete_rough(MSGraph<sWord> vp, int size, int mode, int work_color);
tList*  get_object_region(MSGraph<sWord> gd, int mode, int work_color);
tList*  get_region_vertex(tList* lt, int err, int sz);

PCoordinate<int>  detect_cross_point(MSGraph<sWord>* vp, int err, int work_color);
PCoordinate<int>  find_vertex(MSGraph<sWord> vp, PCoordinate<int> coord);
int   detect_cross_point_0(MSGraph<sWord>vp, int& i, int& j, int err, int work_color);
int   detect_cross_point_1(MSGraph<sWord>vp, int& i, int& j, int err, int work_color);
int   detect_cross_point_2(MSGraph<sWord>vp, int& i, int& j, int err, int work_color);
int   detect_cross_point_3(MSGraph<sWord>vp, int& i, int& j, int err, int work_color);




#endif
