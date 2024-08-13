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

#define  JBXL_GLB_HEADER         "glTF"
#define  JBXL_GLB_VERSION        2.0
#define  JBXL_GLB_TYPE_JSON      "JSON"
#define  JBXL_GLB_TYPE_BIN       "BIN\0"


#define  JBXL_GLTF_BIN_AOS       1
#define  JBXL_GLTF_BIN_SOA       2


#define  JBXL_GLTF_COPYRIGHT     "From OpenSimulator"
#define  JBXL_GLTF_GENERATOR     "JBXL glTF Tool Library (C) 2024 v1.0 by Fumi.Iseki"
#define  JBXL_GLTF_VERSION       "2.0"

// accessors
#define  JBXL_GLB_ACCESSORS_PNG_IMAGE   "{\"bufferView\":%d,\"mimeType\":\"image/png\"}"
#define  JBXL_GLB_ACCESSORS_JPEG_IMAGE  "{\"bufferView\":%d,\"mimeType\":\"image/jpeg\"}"

#define  JBXL_GLTF_BUFFERS_BIN    "{\"uri\":\"%s\",\"byteLength\":%u}"
#define  JBXL_GLTF_BUFFERS_B64    "{\"uri\":\"data:application/octet-stream;base64,%s\",\"byteLength\":%u}"

#define  JBXL_GLTF_VIEWS          "{\"buffer\":%d,\"byteOffset\":%u,\"byteLength\":%u,\"byteStride\":%u,\"target\":34962}"
#define  JBXL_GLTF_VIEWS_ELEMENT  "{\"buffer\":%d,\"byteOffset\":%u,\"byteLength\":%u,\"target\":34963}"
//#define  JBXL_GLTF_VIEWS_IBM      "{\"buffer\":%d,\"byteOffset\":%u,\"byteLength\":%u,\"byteStride\":%u}"
#define  JBXL_GLTF_VIEWS_DATA     "{\"buffer\":%d,\"byteOffset\":%u,\"byteLength\":%u}"

#define  JBXL_GLTF_ACCESSORS      "{\"bufferView\":%d,\"byteOffset\":%u,\"componentType\":%d,\"count\":%d,\"type\":\"%s\"}"
#define  JBXL_GLTF_ACCESSORS_S    "{\"bufferView\":%d,\"byteOffset\":%u,\"componentType\":%d,\"count\":%d,\"type\":\"%s\",\"max\":[%d],\"min\":[%d]}"
#define  JBXL_GLTF_ACCESSORS_V2   "{\"bufferView\":%d,\"byteOffset\":%u,\"componentType\":%d,\"count\":%d,\"type\":\"%s\",\"max\":[%f,%f],\"min\":[%f,%f]}"
#define  JBXL_GLTF_ACCESSORS_V3   "{\"bufferView\":%d,\"byteOffset\":%u,\"componentType\":%d,\"count\":%d,\"type\":\"%s\",\"max\":[%f,%f,%f],\"min\":[%f,%f,%f]}"

#define  JBXL_GLTF_NODES_ROOT     "{\"name\":\"Root\"}"
#define  JBXL_GLTF_NODES_ARMATURE "{\"name\":\"Armature\"}"
#define  JBXL_GLTF_NODES_MESH     "{\"name\":\"%s\",\"mesh\":%d}"
#define  JBXL_GLTF_NODES_SKIN     "{\"skin\":%d}"
#define  JBXL_GLTF_NODES_SKLTN    "{\"name\":%s}"    // It doesn't have to be \"%s\" 

#define  JBXL_GLTF_MESHES_PRIM    "{\"indices\":%d,\"attributes\":{\"POSITION\":%d,\"NORMAL\":%d,\"TEXCOORD_0\":%d},\"material\":%d,\"mode\":4}"
#define  JBXL_GLTF_MESHES_PRIM_JW "{\"indices\":%d,\"attributes\":{\"POSITION\":%d,\"NORMAL\":%d,\"TEXCOORD_0\":%d,\"JOINTS_0\":%d,\"WEIGHTS_0\":%d},\"material\":%d,\"mode\":4}"
#define  JBXL_GLTF_SKINS          "{\"inverseBindMatrices\":%d,\"skeleton\":%d}"
//#define  JBXL_GLTF_MTLS           "{\"name\":\"%s\",\"pbrMetallicRoughness\":{\"baseColorFactor\":[%f,%f,%f,%f],\"baseColorTexture\":{\"index\":%d,\"texCoord\":0}}}"

#define  JBXL_GLTF_TEXTURES       "{\"source\":%d}"
//#define  JBXL_GLTF_TEXTURES       "{\"source\":%d, \"sampler\":%d}"
#define  JBXL_GLTF_IMAGES         "{\"uri\":\"%s\"}"

#define  JBXL_GLTF_MTLS_NAME_PBR  "{\"name\":\"%s\",\"pbrMetallicRoughness\":{\"baseColorTexture\":{\"index\":%d,\"texCoord\":0}}}"
#define  JBXL_GLTF_MTLS_BCOLORF   "{\"baseColorFactor\":[%f,%f,%f,%f]}"
#define  JBXL_GLTF_MTLS_METALF    "{\"metallicFactor\":%f}"
#define  JBXL_GLTF_MTLS_ROUGHF    "{\"roughnessFactor\":%f}"

#define  JBXL_GLTF_MTLS_CUTOFF    "{\"alphaCutoff\":%f}"
#define  JBXL_GLTF_MTLS_EMISSIVE  "{\"emissiveFactor\":[%f,%f,%f]}"



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
    unsigned int    shell_indexes;  // この SHELL での vi の総数
    unsigned int    shell_vertexes; // この SHELL での vv の総数 (vn, vu に対しても同じ数)

    bool            collider;

    unsigned int    num_facets;     // この SHELL での FACET の総数   
    unsigned int*   facet_index;    // 各 FACET での vi の数を格納した配列
    unsigned int*   facet_vertex;   // 各 FACET での vv (vn, vu) の数を格納した配列

    unsigned int*   vi;             // インデックス (shell_indexes  個)
    Vector<float>*  vv;             // 頂点座標     (shell_vertexes 個)
    Vector<float>*  vn;             // 法線ベクトル (shell_vertexes 個)
    UVMap<float>*   vu;             // UVマップ     (shell_vertexes 個)
    Vector4<short unsigned>* vj;    // 重み付けされたジョイント (shell_vertexes 個)
    Vector4<float>*          vw;    // ジョイントの重み (shell_vertexes 個)
    float*          vm;             // Inverse Bind Matrix (num_joints x 16 個)

    GLTFShellNode* next;            // 次の SHELL ノードへのポインタ

public:
    void   init(void);
    void   free(void);
    void   delete_next(void);       // 次の SHELLノードを再帰的に呼び出して削除する
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
    bool    no_offset;

    int     bin_mode;       // JBXL_GLTF_BIN_AOS or JBXL_GLTF_BIN_SOA
    bool    bin_seq;        // binデータを逐次的に作成．false の場合，createShellGeoData() で一度に binデータを生成．

    bool    glb_out;        // glb ファイルを出力．

    bool    forUnity;
    bool    forUE;
    int     engine;

    bool    has_joints;
    tList*  joints_list;

    tList*  image_list;
    tList*  material_list;

    Vector<double>       center;
    AffineTrans<double>  affineRoot;

    Buffer  bin_buffer;
    unsigned int bin_offset;
    //
    //Buffer  matrix_buffer;
    //unsigned int matrix_offset;

    // counter
    unsigned int  shell_no;                 // shell の通し番号（addShellが呼ばれた回数）
    unsigned int  node_no;                  // nodes の要素（node）の通し番号
    unsigned int  mesh_no;                  // meshes の要素（mesh）の通し番号
    unsigned int  mesh_prim_no;             // meshe の primitive 要素の通し番号
    unsigned int  skin_no;                  // skins の要素（skin）の通し番号
    unsigned int  view_no;                  // bufferViews の要素（bufferView）の通し番号
    unsigned int  accessor_no;              // accessors の要素（accessor）の通し番号
    unsigned int  material_no;              // materials の要素（material）の通し番号
    unsigned int  image_no;                 // images の要素（image）の通し番号． material.index -> texture.source -> image

    unsigned int  num_joints;
    unsigned int  node_offset;

    tJson*  json_data;
    tJson*  scenes;
    tJson*  scenes_name;
    tJson*  scenes_nodes;
    tJson*  nodes;
    tJson*  meshes;
    tJson*  skins;
    tJson*  buffers;
    tJson*  buffviews;
    tJson*  accessors;
    tJson*  materials;
    tJson*  textures;
    tJson*  images;

    tJson*  nodes_children;

    GLTFShellNode* shellNode;   // 

public:
    void    init(void); 
    void    free(void); 
    void    initGLTF(void);

    void    setUnity(bool b) { this->forUnity = b;}
    void    setUE(bool b)    { this->forUE    = b;}
    void    setEngine(int);

    void    addShell(MeshObjectData* meshdata, bool collider, SkinJointData* skin_joint=NULL, tList* joints_info=NULL);

    void    addScenes(void);
    void    addRootNode(AffineTrans<double>* affine);
    void    addSkeletonNodes(SkinJointData* skin_joint, AffineTrans<double>* affin);
    void    addNodes(AffineTrans<double>* affine);

    void    addTextures(MeshFacetNode* facet);
    void    addMeshes(MeshFacetNode* facet);
    void    addSkins(void);
    void    addMaterials(MeshFacetNode* facet);
    void    addMaterialParameters(tJson* pbr, MeshFacetNode* facet);

    void    closeSolid(void);

    // Affine Transfer
    AffineTrans<double> getAffineBaseTrans4Engine(void);
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
    void    createShellGeometryData(MeshFacetNode* facet, int shell_indexes, int shell_vertexes, SkinJointData* skin_joint=NULL);
    void    createBinDataAoS(void);
    void    createBinDataSoA(void);

    // IBM
    void    addBufferViewsIBM(void);
    void    addAccessorsIBM(void);
    void    createInverseBindMatrix(SkinJointData* skin_joint);

    // output
    void    outputFile (const char* fn, const char* out_dirn, const char* ptm_dirn, const char* tex_dirn, const char* bin_dirn);
    void    output_gltf(char* fn, char* out_dirn, char* ptm_dirn, char* tex_dirn, char* bin_dirn);
    void    output_glb (char* fn, char* out_dirn, char* ptm_dirn, char* tex_dirn, char* bin_dirn);
    void    convertJson_TexturePath(char* tex_dirn);

    gltfFacetMinMax getFacetMinMax(MeshFacetNode* facet);

    // GLB
    uDWord  convertJson_gltf2glb(glbTextureInfo* tex_info);
    glbTextureInfo*  getGLBTextureInfo(const char* tex_dirn);
    void    freeGLBTextureInfo(glbTextureInfo* tex_info);
};


inline void  freeGLTFData(GLTFData* gltf) { if(gltf!=NULL) { gltf->free(); delete gltf; gltf=NULL;} }


}

#endif
