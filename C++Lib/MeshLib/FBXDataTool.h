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
*/
class  FBXData
{
public:
    FBXData(void) { this->init();}
    virtual ~FBXData(void);

public:
    Buffer  fbx_name;
    bool    phantom_out;
    bool    no_offset;

    bool    forUnity;
    bool    forUE;
    int     engine;

    bool    has_joints;
    tList*  joints_list;

    AffineTrans<double>* affineTrans;
    AffineTrans<double>  skeleton;

public:
    void    init(void); 
    void    free(void); 

    void    setUnity(bool b) { this->forUnity = b;}
    void    setUE(bool b)    { this->forUE    = b;}
    void    setEngine(int);

    void    setAffineTrans (AffineTrans<double> a) { delAffineTrans(); affineTrans = new AffineTrans<double>(); affineTrans->dup(a);}
    void    delAffineTrans (void) { freeAffineTrans(this->affineTrans);}
    Vector<double> execAffineTrans(void);

    void    addShell(MeshObjectData* meshdata, bool collider, SkinJointData* joints_data=NULL, tList* joints_template=NULL);
    void    closeSolid(void) {}

    void    outputFile(const char* fn, const char* out_dirn, const char* ptm_dirn, const char* tex_dirn, const char* bin_dirn);
    void    output_fbx(FILE* fp);
};


inline void  freeFBXData(FBXData* fbx) { if(fbx!=NULL) { fbx->free(); delete fbx; fbx=NULL;} }


}

#endif
