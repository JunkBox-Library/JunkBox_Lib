

#ifndef  __EXTRACT_PHOTO_H
#define  __EXTRACT_PHOTO_H



#include  "common++.h"
#include  "tools++.h"

#include  "JpegTool.h"
#include  "Gio.h"
#include  "Graph.h"
#include  "Gmt.h"



#define  WORK_COLOR  10000
#define  EDGE_SIZE   30



tList*  extract_graph(JPEGImage jp);
tList*  vertex_graph(MSGraph<sWord> jp);
tList*  cut_graph(tList* lv, MSGraph<sWord> gd);
tList*  rotate_graph(tList* lc, tList* lv);
tList*  graph2jpeg(tList* lp);


double  get_grad_graph(PCoordinate<int> pc);
MSGraph<sWord> rotate_MSGraph(MSGraph<sWord> vp, double cst, PCoordinate<int>* pc=NULL);



void    delete_rough(MSGraph<sWord> vp, int size, int mode, int work_color);
int     background_threshold(MSGraph<sWord> vp, int ith, int dth, int size, float rate, int mode, int work_color);
tList*  get_object_region(MSGraph<sWord> gd, int mode, int work_color);
tList*  get_region_vertex(tList* lt, int err, int sz);


PCoordinate<int>  detect_cross_point(MSGraph<sWord>* vp, int work_color);
int   detect_cross_point_0(MSGraph<sWord>vp, int& i, int& j, int work_color);
int   detect_cross_point_1(MSGraph<sWord>vp, int& i, int& j, int work_color);
int   detect_cross_point_2(MSGraph<sWord>vp, int& i, int& j, int work_color);
int   detect_cross_point_3(MSGraph<sWord>vp, int& i, int& j, int work_color);

PCoordinate<int>  find_vertex(MSGraph<sWord> vp, PCoordinate<int> coord);


void  del_tList_MSGraph(tList** lp);
void  del_tList_PCoordinate(tList** lp);
void  del_tList_JPEGImage(tList** lp);



#endif
