#ifndef  __JBXL_CPP_COLLADA_TOOL_H_
#define  __JBXL_CPP_COLLADA_TOOL_H_

#include  "tools++.h"
#include  "txml.h"

#include  "TriBrep.h"
#include  "Rotation.h"
#include  "ContourBaseData.h"
#include  "MaterialParam.h"
#include  "MeshObjectData.h"


namespace jbxl {


#define  COLLADA_STR_AUTHOR     "JBXL Collada Tool Library (C) 2014 by Fumi.Iseki"
#define  COLLADA_STR_XMLNS      "http://www.collada.org/2005/11/COLLADASchema"
#define  COLLADA_STR_TOOL       "JunkBox_Lib++ (https://github.com/JunkBox-Library)"
#define  COLLADA_STR_VER        "1.5.0"

//
#define  COLLADA_X_UP           0
#define  COLLADA_Y_UP           1
#define  COLLADA_Z_UP           2


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ColladaXML.  XML for Collada Data
//

/**

*/
class  ColladaXML
{
public:
    ColladaXML(double meter=1.0, int axis=COLLADA_Z_UP, const char* ver=NULL) { init(meter, axis, ver);}
    ColladaXML(float  meter,     int axis=COLLADA_Z_UP, const char* ver=NULL) { init(meter, axis, ver);}
    virtual ~ColladaXML(void);

public:
    void    init (double meter=1.0, int axis=COLLADA_Z_UP, const char* ver=NULL);
    void    init (float  meter,     int axis=COLLADA_Z_UP, const char* ver=NULL) { init((double)meter, axis, ver); }
    void    clear(double meter=1.0, int axis=COLLADA_Z_UP, const char* ver=NULL);
    void    clear(float  meter,     int axis=COLLADA_Z_UP, const char* ver=NULL) { clear((double)meter, axis, ver); }
    void    free (void);

    void    outputFile(const char* fn, const char* path=NULL, int mode=XML_SPACE_FORMAT);
    void    output_dae (FILE* fp, int mode=XML_SPACE_FORMAT) { print_xml(fp, xml_tag, mode);}
    void    output_tree(FILE* fp) { print_xml_tree(fp, xml_tag, "    ");}

public:
    void    initCollada(double meter, int axis, const char* ver);
    void    initCollada(float  meter, int axis, const char* ver) { initCollada((double)meter, axis, ver); }
    void    addShell(MeshObjectData* shelldata, bool collider, SkinJointData* skin_joint=NULL, tXML* joints_template=NULL);

    char*   addGeometry(MeshObjectData* shelldata);
    char*   addController(const char* geometry_id, MeshObjectData* shelldata, SkinJointData* skin_joint);
    void    addScene(const char* geometry_id, char* controll_id, MeshObjectData* shelldata, bool collider, SkinJointData* skin_joint);

    char*   addVertexSource(tXML* tag, MeshObjectData* shelldata);
    char*   addNormalSource(tXML* tag, MeshObjectData* shelldata);
    char*   addTexcrdSource(tXML* tag, MeshObjectData* shelldata);
    char*   addWeightSource(tXML* tag, MeshObjectData* shelldata, Vector<int>* weight_index, int num_joints);

    char*   addVerticesPos (tXML* mesh_tag, const char* position_id);

    void    addSimpleTechniqueAccessor(tXML* source_tag, const char* source_array_id, int count, int stride, const char* name, const char* type);
    void    addPosTechniqueAccessor(tXML* source_tag, const char* source_array_id, int count);
    void    addMapTechniqueAccessor(tXML* source_tag, const char* source_array_id, int count);

    void    addPolylists(tXML* mesh_tag, MeshObjectData* shelldata, const char* vertex_id, const char* normal_id=NULL, const char* texcrd_id=NULL);
    char*   addImage(const char* filename);
    char*   addMaterial(const char* material);
    tXML*   addEffect(const char* material_url, const char* file_id, MaterialParam param);
    void    addExtraBumpmap(tXML* effect_tag, const char* bump_id);

    void    closeSolid(void);
    void    deleteJoint(tXML* delete_tag);

    bool    existSameID(tXML* top, const char* tag, const char* id);

    void    setBlankTexture(const char* name) { if(name!=NULL) blank_texture = make_Buffer_bystr(name);}
    bool    isBlankTexture (const char* name);

    void    setJointLocationMatrix(void);
    void    deleteNousedJoints(tXML* delete_tag);
    void    deleteListJoints(tXML* top_tag, tList* joints_name);

private:

public:
    tXML*   xml_tag;
    //
    tXML*   collada_tag;
    tXML*   asset_tag;
    tXML*   contributor_tag;
    tXML*   author_tag;
    tXML*   authoring_tool_tag;
    tXML*   created_tag;
    tXML*   modified_tag;
    tXML*   unit_tag;
    tXML*   up_axis_tag;

    tXML*   library_images_tag;
    tXML*   library_effects_tag;
    tXML*   library_materials_tag;
    tXML*   library_geometries_tag;
    tXML*   library_controllers_tag;
    tXML*   library_physics_scenes_tag;
    tXML*   library_physics_models_tag;
    tXML*   library_visual_scenes_tag;

    tXML*   instance_physics_model_tag;

    tXML*   visual_scene_tag;
    tXML*   physics_scene_tag;
    tXML*   physics_model_tag;

    tXML*   scene_tag;
    tXML*   instance_visual_scene_tag;
    tXML*   instance_physics_scene_tag;

public:
    AffineTrans<double>* affineTrans;
    AffineTrans<double>  affineSkeleton;

    tXML*   joints_template_tag;
    bool    added_joints_xml;
    bool    has_joints;
    bool    no_offset;
    bool    phantom_out;
    Buffer  blank_texture;

// for Unity
public:
    bool    forUnity5;
    bool    forUnity;
    void    addCenterObject(void);
    void    addCenterScene(void);
};


inline void  freeColladaXML(ColladaXML*& dae) { if(dae!=NULL) { dae->free(); delete dae; dae=NULL;} }


}       // namespace

#endif

