
#ifndef  __JBXL_CPP_GRAPHIC_LIBRARY_H_
#define  __JBXL_CPP_GRAPHIC_LIBRARY_H_


/**
@brief    グラフィックライブラリ
@file     GraphLib.h
@version  5.0
@date     2008 8/11
@author   Fumi.Iseki (C)

@par 変更履歴 
'05 12/11   Gio.cpp, Gio.h  CTLibとの機能切り分けのため，構成を変更．
'05  1/22   Gdata.h         MSGraph<T>::state を追加，CT_3D_VOLを定義．
'05  1/20   Gdata.h         MSGraph<T>::put_vPoint() を追加
'04 11/22   Gio.h           writeGraphicFile() の機能拡張
'04 11/15   ExGdata.h       メモリ管理機能付き ExMSGraph の説明文（コメント）を訂正
'04 11/15   Gio.cpp         writeCTFile() の修正
'04  9/12   Gio.cpp         書き込み関数 writeGraphicFile(), writeCTFile() 追加．
                            readGraphicSlices() 関数のCT_3DM読み込み修正．
'04  9/11   ExGdata.cpp     メモリ管理機能付き ExMSGraph, ExCmnHead を定義．
'04  9/10   Gio.cpp         オペレータ（ユーザ）定義ヘッダファイル読み込み関数 readUserSetData()作成
*/


#include  "MathLib.h"

#include  "ExGData.h"
#include  "Graph.h"
#include  "Gio.h"
#include  "Morph.h"
#include  "Thinning.h"

#define  JBXL_GRAPHLIB_VERSION      "v5.0 '08 8/11" 
#define  JBXL_GRAPHLIB_NAME         "GraphLib"
#define  JBXL_GRAPHLIB_COPYRIGHT    "by Fumi.Iseki (C)  2000"


#endif
 
