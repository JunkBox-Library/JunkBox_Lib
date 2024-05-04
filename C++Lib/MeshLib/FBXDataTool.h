#ifndef  __JBXL_CPP_FBX_TOOL_H_
#define  __JBXL_CPP_FBX_TOOL_H_

/**
  Wavefront FBX ファイル用ツール

*/

#include  "tools++.h"
#include  "txml.h"

#include  "TriBrep.h"
#include  "Rotation.h"
#include  "ContourBaseData.h"
#include  "MaterialParam.h"
#include  "MeshObjectData.h"


namespace jbxl {


//////////////////////////////////////////////////////////////////////////////////////////
//

/**
先頭のデータはアンカー．
アンカーでない場合は num_fbx == -1
*/
class  FBXData
{
public:
    FBXData() { this->init();}
    virtual ~FBXData(void);

public:
    Buffer  fbx_name;
    bool    phantom_out;

    bool    forUnity;
    bool    forUE;
    int     engine;

    AffineTrans<double>* affineTrans;

public:
    void    init(void); 
    void    free(void); 

    void    setUnity(bool b) { this->forUnity = b;}
    void    setUE(bool b)    { this->forUE = b;}
    void    setEngine(int);

    void    setAffineTrans (AffineTrans<double> a) { delAffineTrans(); affineTrans = new AffineTrans<double>(); affineTrans->dup(a);}
    void    delAffineTrans (void) { freeAffineTrans(this->affineTrans);}
    Vector<double> execAffineTrans(bool origin);

    void    addObject(MeshObjectData* meshdata, bool collider, SkinJointData* joints);

    void    outputFile(const char* fn, const char* out_path, const char* tex_dirn);
    void    output_fbx(FILE* fp);
};


inline void  freeFBXData(FBXData* fbx) { if(fbx!=NULL) { fbx->free(); delete fbx; fbx=NULL;} }


}

#endif
