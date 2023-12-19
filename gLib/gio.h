#ifndef  __JBXL_GRAPHIC_IO_H_
#define  __JBXL_GRAPHIC_IO_H_


/**
@brief    グラフィック用ファイル入出力関数ヘッダ
@file     gio.h
@version  3.1
@date     2007 6/27
@author   Fumi.Iseki (C)
*/


#include "gdata.h"



extern  IRBound ExRBound;


int   write_wsg_file(const char* fn, WSGraph gr); 
int   write_file_rb (const char* fn, WSGraph gr, IRBound rb);
int   write_ras_file(const char* fn, WSGraph gr); 

WSGraph read_wsg_file   (const char* fn);
WSGraph read_wsg_file_rb(const char* fn, IRBound* rb);
WSGraph read_ras_file   (const char* fn);


int   write_cmn_file(const char* fn, CmnHead* hd);
int   write_ct_file (const char* fn, CmnHead* hd); 
int   write_ras_file_obit(const char* fn, CmnHead* hd, int obit);

CmnHead  read_xxx_file  (const char* fn);
CmnHead  read_cmn_file  (const char* fn);
CmnHead  read_cmn_header(const char* fn);

CmnHead  read_ras_data (FILE* fp);                              ///< readRasData()      for C++
CmnHead  read_user_data(FILE* fp, CmnHead* hd);                 ///< readUserSetData()  for C++
CmnHead  read_ct_data  (FILE* fp);                              ///< readCTData()       for C++

int      dicom_header(FILE* fp, int fsize, int* dsize, int* xsize, int* ysize, int* depth, double* rzxy);
WSGraph  read_dicom_file(const char* fn);


#endif




