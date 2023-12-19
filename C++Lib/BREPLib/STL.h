#ifndef __JBXL_STL_H_
#define __JBXL_STL_H_


/**
@brief    STL ファイル入出力用ライブラリ ヘッダ
@file     STL.h
@author   Fumi.Iseki (C)

@note
エンディアン無変換
*/


#include  "Brep.h"


namespace jbxl {


/// データ読み込み用一時データ
typedef struct {
    unsigned char tmp[50]; 
} tmpSTLData;   // 50Byte



/// 三角形データ用
///   v[0] 〜 v[2] : 法線ベクトル
///   v[3] 〜 v[5] : 第１の頂点の X,Y,Z座標   
///   v[6] 〜 v[8] : 第２の頂点の X,Y,Z座標   
///   v[9] 〜 v[11]: 第３の頂点の X,Y,Z座標
///   STL Data は 32bit単位 なので float を使用
typedef struct {
    float vect[12];
    unsigned short pad; 
} STLData;      // 52Byte


DllExport STLData* ReadSTLFile (char* fname,  long int* fno);
DllExport int      WriteSTLFile(char* fname,  BREP_SOLID* solid, bool ascii=false);
DllExport int      WriteSTLFile(char* fname,  BREP_SOLID_LIST solid_list, bool ascii=false);

DllExport STLData* ReadSTLFileA (char* fname, long int* fno);
DllExport STLData* ReadSTLFileB (char* fname, long int* fno);
DllExport int      WriteSTLFileA(char* fname, BREP_SOLID* solid);
DllExport int      WriteSTLFileB(char* fname, BREP_SOLID* solid);
DllExport int      WriteSTLFileA(char* fname, BREP_SOLID_LIST solid_list);
DllExport int      WriteSTLFileB(char* fname, BREP_SOLID_LIST solid_list);

DllExport void     println_FacetAsciiSTL(BREP_CONTOUR* contour);

DllExport void     freeSTL(STLData* stldata);


}       // namespace

#endif

