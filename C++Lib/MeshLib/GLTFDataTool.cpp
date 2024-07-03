/**
@brief    GLTF用 ツール
@file     GLTFDataTool.cpp
@author   Fumi.Iseki (C)
*/

#include  "GLTFDataTool.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////
//

GLTFData::~GLTFData(void)
{
    DEBUG_MODE PRINT_MESG("GLTFData::DESTRUCTOR:\n");
    this->free();
}


void  GLTFData::init(int n)
{
    initGLTF();

    this->gltf_name   = init_Buffer();
    this->num_gltf    = n;
    this->phantom_out = true;
    this->no_offset   = false;

    this->forUnity    = true;
    this->forUE       = false;

    this->engine      = JBXL_3D_ENGINE_UE;
    this->next        = NULL;
    this->affineTrans = NULL;
    this->skeleton.init();
}


void  GLTFData::free(void)
{
    free_Buffer(&(this->gltf_name));
    del_all_json(&(this->json_data));
    this->skeleton.free();

    this->delAffineTrans();
    this->affineTrans = NULL;

    this->delete_next();
}


void  GLTFData::setEngine(int e)
{
    this->setUnity(false);
    this->setUE(false);
    //
    this->engine = e;
    if (e==JBXL_3D_ENGINE_UNITY)   this->setUnity(true);
    else if (e==JBXL_3D_ENGINE_UE) this->setUE(true);

    return;
}


void  GLTFData::delete_next(void)
{
    if (this->next==NULL) return;

    GLTFData* _next = this->next;
    while (_next!=NULL) {
        GLTFData* _curr_node = _next;
        _next = _next->next;
        _curr_node->next = NULL;
        delete(_curr_node);
    }
    this->next = NULL;
}


void  GLTFData::initGLTF(void)
{
    char asset[] = "{asset:{\"copyright\": , \"generator\": , \"version\": }";
    this->json_data = json_parse(asset, 99);
}


void  GLTFData::outputFile(const char* fname, const char* out_path, const char* tex_dirn)
{
    FILE* fp = NULL;
    char* packname = pack_head_tail_char(get_file_name(fname), ' ');
    Buffer file_name = make_Buffer_bystr(packname);
    ::free(packname);

    canonical_filename_Buffer(&file_name);
    if (file_name.buf[0]=='.') file_name.buf[0] = '_';
    //
    Buffer gltf_path;    // 出力パス
    if (out_path==NULL) gltf_path = make_Buffer_bystr("./");
    else                gltf_path = make_Buffer_bystr(out_path);
    //
    Buffer rel_tex;     //  Texture 相対パス
    if (tex_dirn==NULL) rel_tex = make_Buffer_bystr("");
    else                rel_tex = make_Buffer_bystr(tex_dirn);

    cat_Buffer(&file_name, &gltf_path);
    change_file_extension_Buffer(&gltf_path, ".gltf");

    fp = fopen((char*)gltf_path.buf, "wb");
    if (fp!=NULL) {
        this->output_gltf(fp);
        fclose(fp);
    }
    //
    free_Buffer(&gltf_path);
    free_Buffer(&rel_tex);
    //
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////

void  GLTFData::addObject(MeshObjectData* meshdata, bool collider, SkinJointData* joints)
{
    if (meshdata==NULL) return;

    if (collider) {
    }

    if (joints!=NULL) {
    }

    return;
}


/**
Vector<double>  GLTFData::execAffineTrans(void)

GLTFデータの Affine変換を行う．
no_offset が trueの場合，データの中心を原点に戻し，実際の位置をオフセットで返す．
*/
Vector<double>  GLTFData::execAffineTrans(void)
{
    Vector<double> center(0.0, 0.0, 0.0);

    GLTFData* gltf = this->next;  // Top はアンカー
    if (gltf!=NULL && this->no_offset) center = gltf->affineTrans->shift;

    return center;
}


void  GLTFData::output_gltf(FILE* fp)
{
    if (fp==NULL) return;
    
    return;
}

