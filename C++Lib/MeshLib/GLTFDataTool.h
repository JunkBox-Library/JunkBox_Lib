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

#define  JBXL_GLB_HEADER       "glTF"
#define  JBXL_GLB_VERSION      2.0
#define  JBXL_GLB_TYPE_JSON    "JSON"
#define  JBXL_GLB_TYPE_BIN     "BIN\0"


#define  JBXL_GLTF_BIN_AOS      1
#define  JBXL_GLTF_BIN_SOA      2


#define  JBXL_GLTF_COPYRIGHT    "From OpenSimulator"
#define  JBXL_GLTF_GENERATOR    "JBXL glTF Tool Library (C) 2024 v1.0 by Fumi.Iseki"
#define  JBXL_GLTF_VERSION      "2.0"

//#define  JBXL_GLTF_BUFFER       "{\"uri\":%s,\"byteLength\":%u}"
#define  JBXL_GLTF_VIEW         "{\"buffer\":%d,\"byteOffset\":%u,\"byteLength\":%u,\"byteStride\":%d,\"target\":34962}"
#define  JBXL_GLTF_ELEMENT_VIEW "{\"buffer\":%d,\"byteOffset\":%u,\"byteLength\":%u,\"target\":34963}"
#define  JBXL_GLTF_TEXTURE_VIEW "{\"buffer\":%d,\"byteOffset\":%u,\"byteLength\":%u}"
#define  JBXL_GLTF_ACCESSOR     "{\"bufferView\":%d,\"byteOffset\":%u,\"componentType\":%d,\"count\":%d,\"type\":\"%s\"}"
#define  JBXL_GLTF_ACCESSOR_S   "{\"bufferView\":%d,\"byteOffset\":%u,\"componentType\":%d,\"count\":%d,\"type\":\"%s\",\"max\":[%d],\"min\":[%d]}"
#define  JBXL_GLTF_ACCESSOR_V2  "{\"bufferView\":%d,\"byteOffset\":%u,\"componentType\":%d,\"count\":%d,\"type\":\"%s\",\"max\":[%lf,%lf],\"min\":[%lf,%lf]}"
#define  JBXL_GLTF_ACCESSOR_V3  "{\"bufferView\":%d,\"byteOffset\":%u,\"componentType\":%d,\"count\":%d,\"type\":\"%s\",\"max\":[%lf,%lf,%lf],\"min\":[%lf,%lf,%lf]}"
#define  JBXL_GLTF_MESH         "{\"name\":\"%s\",\"mesh\":%d}"
#define  JBXL_GLTF_MESH_PRIM    "{\"indices\":%d,\"attributes\":{\"POSITION\":%d,\"NORMAL\":%d,\"TEXCOORD_0\":%d},\"material\":%d,\"mode\":4}"
#define  JBXL_GLTF_MATERIAL     "{\"name\":\"%s\",\"pbrMetallicRoughness\":{\"baseColorFactor\":[%f,%f,%f,%f],\"baseColorTexture\":{\"index\":%d,\"texCoord\":0}}}"
#define  JBXL_GLTF_TEXTURE      "{\"source\":%d}"
//#define  JBXL_GLTF_TEXTURE      "{\"source\":%d, \"sampler\":%d}"
#define  JBXL_GLTF_IMAGE        "{\"uri\":\"%s\"}"

#define  JBXL_GLB_PNG_IMAGE     "{\"bufferView\":%d,\"mimeType\":\"image/png\"}"
#define  JBXL_GLB_JPEG_IMAGE    "{\"bufferView\":%d,\"mimeType\":\"image/jpeg\"}"



typedef struct _gltf_facet_min_max {
    int    index_max;
    int    index_min;
    float  vertex_x_max;
    float  vertex_x_min;
    float  vertex_y_max;
    float  vertex_y_min;
    float  vertex_z_max;
    float  vertex_z_min;
    float  normal_x_max;
    float  normal_x_min;
    float  normal_y_max;
    float  normal_y_min;
    float  normal_z_max;
    float  normal_z_min;
    float  texcrd_u_max;
    float  texcrd_u_min;
    float  texcrd_v_max;
    float  texcrd_v_min;
}  gltfFacetMinMax;


typedef struct _glb_file_header {
    uDWord    magic;
    uDWord    version;
    uDWord    length;
}  glbFileHeader;
     

typedef struct _glb_data_chunk {
    uDWord    length;   // データサイズ + PAD
    uDWord    type;
    uDWord    pad;      // 4Byte 境界のパッド
    uByte*    data;
}  glbDataChunk;


typedef struct _glb_texture_info {
    uDWord    length;   // 画像データのサイズ
    uDWord    pad;      // 4Byte 境界のパッド
    Buffer*   fname;
    tJson*    json;
    struct _glb_texture_info* next; 
}  glbTextureInfo;



//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 SHELLノード
*/
class  GLTFShellNode
{
public:
    GLTFShellNode() { this->init();}
    virtual ~GLTFShellNode(void) { this->free();}

public:
    int    shell_indexes;       // この SHELL での data_index の総数
    int    shell_vertexes;      // この SHELL での vv の総数 (vn, vt に対しても同じ数)

    bool   collider;

    int    num_facet;           // この SHELL での FACET の数   
    int*   facet_index;         // 各 FACET での data_index の数を格納した配列
    int*   facet_vertex;        // 各 FACET での vv (vn, vt) の数を格納した配列

    int*            data_index;
    Vector<double>* vv;
    Vector<double>* vn;
    UVMap<double>*  vt;

    //AffineTrans<double>* uvmap_trans;

    GLTFShellNode* next;        // 次の SHELL ノードへのポインタ

public:
    void   init(void);
    void   free(void);
    void   delete_next(void);   // 次の SHELLノードを再帰的に呼び出して削除する
};



//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 GLTFデータ
 GLTF のデータを格納するクラス 
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

    int     bin_mode;       // JBXL_GLTF_BIN_AOS or JBXL_GLTF_BIN_SOA
    bool    bin_seq;        // binデータを逐次的に作成．false の場合，createShellGeoData() で一度に binデータを生成．

    bool    glb_out;        // glb ファイルを出力．

    bool    forUnity;
    bool    forUE;
    int     engine;

    tList*  image_list;
    tList*  material_list;

    Vector<double>       center;
    AffineTrans<double>* affineTrans;   // SOLID のアフィン変換
    AffineTrans<double>  skeleton;

    Buffer  bin_buffer;
    unsigned int bin_offset;

    // counter
    int     shell_no;
    int     node_no;
    int     image_no;
    int     material_no;
    int     mesh_no;
    int     view_no;

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

    GLTFShellNode* shellNode;

public:
    void    init(void); 
    void    free(void); 

    void    setUnity(bool b) { this->forUnity = b;}
    void    setUE(bool b)    { this->forUE    = b;}
    void    setEngine(int);

    void    setAffineTrans(AffineTrans<double> a) { delAffineTrans(); affineTrans = new AffineTrans<double>(); affineTrans->dup(a);}
    void    delAffineTrans(void) { freeAffineTrans(this->affineTrans);}

    AffineTrans<double> getAffineTrans4Engine(AffineTrans<double> affine);
    gltfFacetMinMax getFacetMinMax(MeshFacetNode* facet);

    void    initGLTF(void);

    void    addShell(MeshObjectData* meshdata, bool collider, SkinJointData* joints=NULL);
    void    addScenesNodes(MeshFacetNode* facet, AffineTrans<double>* affine);

    void    addTextures(MeshFacetNode* facet);
    void    addMeshes(MeshFacetNode* facet);
    void    addMaterials(MeshFacetNode* facet);

    void    execAffineUVMap(MeshFacetNode* facet, AffineTrans<double>* affine);

    // AoS
    void    addBufferViewsAoS(MeshFacetNode* facet);
    void    addAccessorsAoS(MeshFacetNode* facet);
    void    createBinDataSeqAoS(MeshFacetNode* facet, int shell_indexes, int shell_vertexes);

    // SoA
    void    addBufferViewsSoA(MeshFacetNode* facet);
    void    addAccessorsSoA(MeshFacetNode* facet);
    void    createBinDataSeqSoA(MeshFacetNode* facet, int shell_indexes, int shell_vertexes);

    // create bin data at onece
    void    createShellGeoData(MeshFacetNode* facet, int shell_indexes, int shell_vertexes);
    void    createBinDataAoS(void);
    void    createBinDataSoA(void);

    void    outputFile (const char* fn, const char* out_dirn, const char* ptm_dirn, const char* tex_dirn, const char* bin_dirn);
    void    output_gltf(char* fn, char* out_dirn, char* ptm_dirn, char* tex_dirn, char* bin_dirn);
    void    output_glb (char* fn, char* out_dirn, char* ptm_dirn, char* tex_dirn, char* bin_dirn);

    void    convertJson_TexturePath(char* tex_dirn);
    uDWord  convertJson_gltf2glb(glbTextureInfo* tex_info);

    glbTextureInfo*  getGLBTextureInfo(const char* tex_dirn);
    void    freeGLBTextureInfo(glbTextureInfo* tex_info);

    void    closeSolid(void) {}
};


inline void  freeGLTFData(GLTFData* gltf) { if(gltf!=NULL) { gltf->free(); delete gltf; gltf=NULL;} }


}

#endif
