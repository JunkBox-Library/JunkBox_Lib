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

#define  JBXL_GLTF_ELEMENT_VIEW     "{\"buffer\":%d, \"byteOffset\":%lu, \"byteLength\":%lu, \"target\": 34963}"
#define  JBXL_GLTF_VIEW             "{\"buffer\":%d, \"byteOffset\":%lu, \"byteLength\":%lu, \"byteStride\":%d, \"target\": 34962}"
#define  JBXL_GLTF_ACCESSOR         "{\"bufferView\":%d, \"byteOffset\":%lu, \"componentType\":%d, \"count\":%d, \"type\":\"%s\"}"
#define  JBXL_GLTF_MESH             "{\"name\":\"%s\",\"mesh\":%d}" 
#define  JBXL_GLTF_MESH_PRIMITIVE   "{\"indices\":%d,\"attributes\":{\"POSITION\":%d,\"NORMAL\":%d,\"TEXCOORD_0\":%d},\"material\":%d,\"mode\":4}" 
#define  JBXL_GLTF_MATERIAL         "{\"name\":\"%s\",\"pbrMetallicRoughness\":{\"baseColorFactor\":[%f,%f,%f,%f],\"baseColorTexture\":{\"index\":%d,\"texCoord\":0}}}" 
//#define  JBXL_GLTF_TEXTURE          "{\"source\":%d, \"sampler\":%d}" 
#define  JBXL_GLTF_TEXTURE          "{\"source\":%d}" 
#define  JBXL_GLTF_IMAGE            "{\"uri\": \"%s\"}" 



//////////////////////////////////////////////////////////////////////////////////////////
//

class  GLTFShellGeoNode
{
public:
    GLTFShellGeoNode() { this->init();}
    virtual ~GLTFShellGeoNode(void) { this->free();}

public:
    int     shell_indexes;          // この SHELL での data_index の総数
    int     shell_vertexes;         // この SHELL での vv の総数 (vn, vt に対しても同じ数)

    bool    collider;

    int     num_facet;              // この SHELL での FACET の数   
    int*    facet_index;            // 各 FACET での data_index の数
    int*    facet_vertex;           // 各 FACET での vv の数

    int*    data_index;
    Vector<double>* vv;
    Vector<double>* vn;
    UVMap<double>*  vt;

    AffineTrans<double>* uvmap_trans;

    GLTFShellGeoNode* next;         // 次の SHELL ノードへのポインタ

public:
    void    init(void);
    void    free(void);
    void    delete_next(void);      // 次の SHELLノードを再帰的に呼び出して削除する
};



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
    Buffer  alt_name;
    bool    phantom_out;
    bool    has_joints;
    bool    no_offset;

    bool    forUnity;
    bool    forUE;
    int     engine;

    GLTFShellGeoNode*    shellNode;

    AffineTrans<double>* affineTrans;
    AffineTrans<double>  skeleton;

    Buffer  bin_buffer;
    long unsigned int bin_offset;

    int     node_num;
    int     view_num;
    int     mesh_num;
    int     material_num;
    int     texture_num;

    tJson*  json_data;
    tJson*  scenes;
    tJson*  scenes_name;
    tJson*  scenes_nodes;
    tJson*  nodes;
    tJson*  meshes;
    tJson*  buffers;
    tJson*  buffviews;
    tJson*  accessors;
    tJson*  materials;
    tJson*  textures;
    tJson*  images;

public:
    void    init(void); 
    void    free(void); 

    void    setUnity(bool b) { this->forUnity = b;}
    void    setUE(bool b)    { this->forUE    = b;}
    void    setEngine(int);

    void    setAffineTrans (AffineTrans<double> a) { delAffineTrans(); affineTrans = new AffineTrans<double>(); affineTrans->dup(a);}
    void    delAffineTrans (void) { freeAffineTrans(this->affineTrans);}

    Vector<double> execDegeneracy(void);
    Vector<double> execAffineTrans(void);

    void    initGLTF(void);

    void    addShell(MeshObjectData* meshdata, bool collider, SkinJointData* joints=NULL);
    void    addScenesNodes(MeshFacetNode* facet, AffineTrans<double>* affine);

    void    addMeshes(MeshFacetNode* facet);
    void    addMaterials(MeshFacetNode* facet);
    void    addTextures(MeshFacetNode* facet, AffineTrans<double>* affine);
    void    makeShellGeometory(MeshFacetNode* facet, int shell_indexes, int shell_vertexes);

    // AoS
    void    addBufferViewsAoS(MeshFacetNode* facet);
    void    addAccessorsAoS(MeshFacetNode* facet);
    void    makeBinDataAoS(void);

    // SoA
    void    addBufferViewsSoA(MeshFacetNode* facet);
    void    addAccessorsSoA(MeshFacetNode* facet);
    void    makeBinDataSoA(void);


    void    closeSolid(void) {}

    void    outputFile(const char* fn, const char* out_path, const char* tex_dirn, const char* bin_dirn);
    void    output_gltf(char* json_file, char* bin_file);
};


inline void  freeGLTFData(GLTFData* gltf) { if(gltf!=NULL) { gltf->free(); delete gltf; gltf=NULL;} }



}

#endif
