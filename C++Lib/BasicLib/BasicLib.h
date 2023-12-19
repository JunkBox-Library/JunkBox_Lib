
#ifndef  __JBXL_CPP_BASICLIB_H_
#define  __JBXL_CPP_BASICLIB_H_


/**
@brief    Basic Library for C++
@file     BasicLib.h
@author   Fumi.Iseki (C)
@version  5.0
@date     2008 8/10

@par Change Log
'08  8/10  Linux and VC++ 統合 @n
'05  1/22  magdebug.cpp メッセージ出力関数の改行の取り扱いを変更 @n
'05  1/19  一部コメント，ファイル名を修正 @n
'04  9/13  decode_Base64() 文字長修正（前から分っていたヤツ）@n
'04  9/13  buffer.c, tlist.c を追加 @n
'04  9/13  tools.h :　 ntoh_st(), hton_st(), ntoh_ar(), hton_ar() の解釈ミスを修正 @n
*/


#include  "tools++.h"

#include  "mt.h"
#include  "matrix.h"
#include  "txml.h"
#include  "xtools.h"

#include  "ClassBox.h"



#define  JBXL_BASICLIB_VERSION      "v5.0 stable  '08 8/10" 
#define  JBXL_BASICLIB_NAME         "BasicLib" 
#define  JBXL_BASICLIB_COPYRIGHT    "by Fumi.Iseki (C) 2000" 


#endif
