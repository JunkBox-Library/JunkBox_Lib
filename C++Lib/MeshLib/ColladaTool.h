
#ifndef  __JBXL_CPP_COLLADA_TOOL_H_
#define  __JBXL_CPP_COLLADA_TOOL_H_


#include  "tools++.h"
#include  "txml.h"

#include  "TriBrep.h"
#include  "Rotation.h"
#include  "FacetBaseData.h"
#include  "MaterialParam.h"
#include  "MeshObjectData.h"



namespace jbxl {


#define  COLLADA_STR_XMLNS      "http://www.collada.org/2005/11/COLLADASchema"
#define  COLLADA_STR_AUTHOR     "JBXL Collada Tools Library (C) 2014"
#define  COLLADA_STR_TOOL       "JunkBox_Lib++ (http://www.nsl.tuis.ac.jp/)"

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
    void    init(float  meter, int axis = COLLADA_Z_UP, const char* ver = NULL)  { init((double)meter, axis, ver); }
    void    clear(double meter=1.0, int axis=COLLADA_Z_UP, const char* ver=NULL);
    void    clear(float  meter, int axis = COLLADA_Z_UP, const char* ver = NULL) { clear((double)meter, axis, ver); }
    void    free (void);

    void    print(FILE* fp, int mode=XML_INDENT_FORMAT) { print_xml(fp, xml, mode);}
    void    print_tree(FILE* fp) { print_xml_tree(fp, xml, "    ");}

public:
    void    initCollada(double meter, int axis, const char* ver);
    void    initCollada(float  meter, int axis, const char* ver) { initCollada((double)meter, axis, ver); }
    void    addObject(MeshObjectData* meshdata, bool collider);

    char*   addGeometry(MeshObjectData* meshdata);
    void    addScene(const char* geometry_id, MeshObjectData* meshdata, bool collider);

    char*   addVertexSource(tXML* mesh, MeshObjectData* meshdata);
    char*   addNormalSource(tXML* mesh, MeshObjectData* meshdata);
    char*   addTexcrdSource(tXML* mesh, MeshObjectData* meshdata);
    char*   addVerticesPos (tXML* mesh, const char* position_id);

    void    addPosTechniqueAccessor(tXML* source, const char* source_array_id, int count);
    void    addMapTechniqueAccessor(tXML* source, const char* source_array_id, int count);

    void    addPolylists(tXML* mesh, MeshObjectData* meshdata, const char* vertex_id, const char* normal_id=NULL, const char* texcrd_id=NULL);
    char*   addImage(const char* filename);
    char*   addMaterial(const char* material);
    tXML*   addEffect(const char* material_url, const char* file_id, MaterialParam param);
    void    addExtraBumpmap(tXML* effect, const char* bump_id);

    bool    existSameID(tXML* top, const char* tag, const char* id);

    void    setBlankTexture(const char* name) { if(name!=NULL) blank_texture = make_Buffer_bystr(name);}
    bool    isBlankTexture (const char* name);

    void    outputFile(const char* fn, const char* path=NULL, int mode=XML_INDENT_FORMAT);

    // for Unity3D
    bool    forUnity5;
    bool    forUnity3D;
    void    addCenterObject(void);
    void    addCenterScene(void);

private:

public:
    tXML*   xml;
    tXML*   collada;

    tXML*   asset;
    tXML*   contributor;
    tXML*   author;
    tXML*   authoring_tool;
    tXML*   created;
    tXML*   modified;
    tXML*   unit;
    tXML*   up_axis;

    tXML*   library_images;
    tXML*   library_geometries;
    tXML*   library_effects;
    tXML*   library_materials;
    tXML*   library_visual_scenes;
    tXML*   library_physics_scenes;
    tXML*   library_physics_models;

    tXML*   instance_physics_model;

    tXML*   visual_scene;
    tXML*   physics_scene;
    tXML*   physics_model;

    tXML*   scene;
    tXML*   instance_visual_scene;
    tXML*   instance_physics_scene;

    Buffer  blank_texture;
};


inline void  freeColladaXML(ColladaXML*& xml) { if(xml!=NULL) { xml->free(); delete xml; xml=NULL;} }



}       // namespace


#endif

