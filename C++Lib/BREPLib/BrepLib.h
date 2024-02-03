#ifndef  __JBXL_BREPLIB_H_
#define  __JBXL_BREPLIB_H_

/**
@brief    BREP (Boundary REPresentation model) Library
@file     BrepLib.h
@version  2.1.0
@author   Fumi.Iseki
@date     2019 11/29

@attention
注：Octreeのポリゴンの衝突検出は高速化が不十分．
*/

#include  "TriBrep.h"
#include  "MeshObjectData.h"


#define  BREPLIB_VERSION    "v2.1.0   '20 11/29" 
#define  BREPLIB_NAME       "BREPLib" 
#define  BREPLIB_COPYRIGHT  "by Fumi.Iseki 2003" 


namespace jbxl {


class  BrepSolidList
{
public:
    BrepSolidList(void) { init();}
    virtual ~BrepSolidList(void);

public:
    BREP_SOLID_LIST solid_list;

public:
    void    init (void);
    void    clear(void);
    void    free (void);

public:
    int     addObject(MeshObjectData* mesh) { return addSolid(mesh);}   // 有効なファセットの数を返す
    int     addSolid (MeshObjectData* mesh);                            // 有効なファセットの数を返す
    void    outputFile(const char* fn, const char* path=NULL, bool asciifile=true);

    BREP_SOLID*  getMerge(CVCounter* counter=NULL);
};


inline void  freeBrepSolidList(BrepSolidList*& solids) { if(solids!=NULL){ solids->free(); delete solids; solids=NULL;} }



}       // namespace


#endif
