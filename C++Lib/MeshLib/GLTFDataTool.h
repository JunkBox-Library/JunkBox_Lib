#ifndef  __JBXL_CPP_GLTF_TOOL_H_
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


#define  JBXL_GLTF_COPYRIGHT    "From OpenSimulator"
#define  JBXL_GLTF_GENERATOR    "JBXL glTF Tool Library (C) 2024 v1.0 by Fumi.Iseki"
#define  JBXL_GLTF_VERSION      "2.0"

#define  JBXL_GLTF_ELEMENT_BUFFER   "{\"buffer\":%d, \"byteOffset\":%lu, \"byteLength\":%lu, \"target\": 34963}"
#define  JBXL_GLTF_BUFFER           "{\"buffer\":%d, \"byteOffset\":%lu, \"byteLength\":%lu, \"byteStride\":%d, \"target\": 34962}"
#define  JBXL_GLTF_ACCESSOR         "{\"bufferView\":%d, \"byteOffset\":%lu, \"componentType\":%d, \"count\":%d, \"type\":\"%s\"}"
#define  JBXL_GLTF_MESH             "{\"name\":\"%s\",\"mesh\":%d}" 
#define  JBXL_GLTF_MESH_PRIMITIVE   "{\"indices\":%d,\"attributes\":{\"POSITION\":%d,\"NORMAL\":%d,\"TEXCOORD_0\":%d},\"mode\":4}" 

/*

  "meshes" : [
    {
      "primitives" : [ { "attributes" : { "POSITION" : 1 }, "indices" : 0 } ]
    }
  ],
*/



/*
      "componentType" : 5123,  // gl.UNSIGNED_SHORT
      "type" : "SCALAR",

      "componentType" : 5126,  // gl.FLOAT
      "count" : 3,
      "type" : "VEC3",
*/

//////////////////////////////////////////////////////////////////////////////////////////
//

/**
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

    Buffer  bin_buffer;
    long unsigned int bin_offset;

    int     node_num;
    int     view_num;
    int     access_num;

    tJson*  json_data;
    tJson*  scenes;
    tJson*  nodes;
    tJson*  meshes;
    //tJson*  primitives;
    tJson*  buffers;
    tJson*  buffviews;
    tJson*  accessors;

public:
    void    init(void); 
    void    free(void); 

    void    setUnity(bool b) { this->forUnity = b;}
    void    setUE(bool b)    { this->forUE    = b;}
    void    setEngine(int);

    void    setAffineTrans (AffineTrans<double> a) { delAffineTrans(); affineTrans = new AffineTrans<double>(); affineTrans->dup(a);}
    void    delAffineTrans (void) { freeAffineTrans(this->affineTrans);}
    Vector<double> execAffineTrans(void);

    void    initGLTF(void);

    void    addObject(MeshObjectData* meshdata, bool collider, SkinJointData* joints=NULL);
    void    closeSolid(void) {}

    //void    outputFile(const char* fn, const char* out_path, const char* tex_dirn);
    void    outputFile(const char* fn, const char* out_path);
    void    output_gltf(char* json_file, char* bin_file);
};


inline void  freeGLTFData(GLTFData* gltf) { if(gltf!=NULL) { gltf->free(); delete gltf; gltf=NULL;} }


}

#endif
