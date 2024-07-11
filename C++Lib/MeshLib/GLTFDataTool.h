﻿#ifndef  __JBXL_CPP_GLTF_TOOL_H_
#define  __JBXL_CPP_GLTF_TOOL_H_

/**
  Wavefront GLTF ファイル用ツール

*/

#include  "tools++.h"
#include  "txml.h"
#include  "tjson.h"

#include  "TriBrep.h"
#include  "Rotation.h"
#include  "ContourBaseData.h"
#include  "MaterialParam.h"
#include  "MeshObjectData.h"


namespace jbxl {


#define  JBXL_GLTF_COPYRIGHT    "from OpenSimulator"
#define  JBXL_GLTF_GENERATOR    "JBXL glTF Tool Library (C) 2024 v1.0 by Fumi.Iseki"
#define  JBXL_GLTF_VERSION      "2.0"

//#define  JBXL_GLTF_COPYRIGHT    "\"from OpenSimulator\""
//#define  JBXL_GLTF_GENERATOR    "\"JBXL glTF Tool Library (C) 2024 v1.0 by Fumi.Iseki\""
//#define  JBXL_GLTF_VERSION      "\"2.0\""



#define  JBXL_GLTF_ELEMENT_ARRAY_BUFFER "{\"buffer\":%d, \"byteLength\":%d, \"byteOffset\":%d, \"target\": 34963}"
#define  JBXL_GLTF_ARRAY_BUFFER         "{\"buffer\":%d, \"byteLength\":%d, \"byteOffset\":%d, \"byteStride\":%d, \"target\": 34962}"


//////////////////////////////////////////////////////////////////////////////////////////
//

/**
先頭のデータはアンカー．
アンカーでない場合は num_gltf == -1
*/
class  GLTFData
{
public:
    GLTFData(void) { this->init();}
    virtual ~GLTFData(void);

public:
    Buffer  gltf_name;
    bool    phantom_out;
    bool    has_joints;
    bool    no_offset;

    bool    forUnity;
    bool    forUE;
    int     engine;

    AffineTrans<double>* affineTrans;
    AffineTrans<double>  skeleton;

    Buffer  index_buffer;
    Buffer  vertex_buffer;
    Buffer  normal_buffer;
    Buffer  texcrd_buffer;

    tJson*  json_data;
    tJson*  buffers;
    tJson*  buffviews;
    tJson*  accessors;

public:
    void    init(void); 
    void    free(void); 

    void    setUnity(bool b) { this->forUnity = b; this->forUE = !b;}
    void    setUE(bool b)    { this->forUE = b; this->forUnity = !b;}
    void    setEngine(int);

    void    setAffineTrans (AffineTrans<double> a) { delAffineTrans(); affineTrans = new AffineTrans<double>(); affineTrans->dup(a);}
    void    delAffineTrans (void) { freeAffineTrans(this->affineTrans);}
    Vector<double> execAffineTrans(void);

    void    initGLTF(void);

    void    addObject(MeshObjectData* meshdata, bool collider, SkinJointData* joints=NULL);
    void    closeSolid(void) {}

    void    outputFile(const char* fn, const char* out_path, const char* tex_dirn);
    void    output_gltf(FILE* fp);
};


inline void  freeGLTFData(GLTFData* gltf) { if(gltf!=NULL) { gltf->free(); delete gltf; gltf=NULL;} }


}

#endif