/**
@brief    COLLADA用 ツール
@file     ColladaTool.cpp
@author   Fumi.Iseki (C)
*/

#include  "ColladaTool.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////////
// ColladaXML
// 

ColladaXML::~ColladaXML(void)
{ 
    DEBUG_MODE PRINT_MESG("ColladaXML::DESTRUCTOR:\n");
}


void  ColladaXML::init(double meter, int axis, const char* ver)
{
    initCollada(meter, axis, ver);
    //
    joints_template_tag = NULL;
    has_joints          = false;
    no_offset           = false;

    blank_texture = init_Buffer();
    phantom_out   = true;
    forUnity5     = false;
    forUnity      = true;
     
    affineTrans   = NULL;;
    skeleton.init();
}


void  ColladaXML::free(void)
{
    free_Buffer(&blank_texture);
    del_all_xml(&xml_tag);
    skeleton.free();

    if (affineTrans!=NULL) affineTrans->free();
    affineTrans = NULL;
}


void  ColladaXML::clear(double meter, int axis, const char* ver)
{
    del_all_xml(&xml_tag);
    init(meter, axis, ver);
}


bool  ColladaXML::isBlankTexture(const char* name)
{
    if (blank_texture.buf==NULL) return false;
    if (strncasecmp(_tochar(blank_texture.buf), name, strlen(_tochar(blank_texture.buf)))) return false;

    return true;
}



////////////////////////////////////////////////////////////////////////////////////////
//

void  ColladaXML::initCollada(double meter, int axis, const char* ver)
{
    Buffer buf;
    if (ver!=NULL) buf = make_Buffer_str(ver);
    else           buf = make_Buffer_str(COLLADA_STR_VER);

    // COLLADA
    xml_tag     = init_xml_doc();
    collada_tag = add_xml_node(xml_tag, "COLLADA");
    //
    add_xml_attr_str(collada_tag, "xmlns",   COLLADA_STR_XMLNS);
    add_xml_attr_str(collada_tag, "version", _tochar(buf.buf));

    // asset
    asset_tag          = add_xml_node(collada_tag, "asset");
    contributor_tag    = add_xml_node(asset_tag, "contributor");
    author_tag         = add_xml_node(contributor_tag, "author");
    authoring_tool_tag = add_xml_node(contributor_tag, "authoring_tool");
    created_tag        = add_xml_node(asset_tag, "created");
    modified_tag       = add_xml_node(asset_tag, "modified");
    unit_tag           = add_xml_node(asset_tag, "unit");
    up_axis_tag        = add_xml_node(asset_tag, "up_axis");
    
    //char* ltime = get_localtime('-', 'T', ':', 'Z');
    char* ltime = get_local_timestamp(time(0), "%Y-%m-%dT%H:%M:%SZ");
    //
    add_xml_content_node(author_tag, COLLADA_STR_AUTHOR);
    add_xml_content_node(authoring_tool_tag, COLLADA_STR_TOOL);
    add_xml_content_node(created_tag,  ltime);
    add_xml_content_node(modified_tag, ltime);
    add_xml_attr_str(unit_tag, "name", "meter");
    add_xml_attr_float(unit_tag, "meter", (float)meter);
    ::free(ltime);

    if      (axis==COLLADA_X_UP) add_xml_content_node(up_axis_tag, "X_UP");
    else if (axis==COLLADA_Y_UP) add_xml_content_node(up_axis_tag, "Y_UP");
    else                         add_xml_content_node(up_axis_tag, "Z_UP");

    library_images_tag      = add_xml_node(collada_tag, "library_images");
    library_effects_tag     = add_xml_node(collada_tag, "library_effects");
    library_materials_tag   = add_xml_node(collada_tag, "library_materials");
    library_geometries_tag  = add_xml_node(collada_tag, "library_geometries");
    library_controllers_tag = add_xml_node(collada_tag, "library_controllers");

    // library_physics_models
    library_physics_models_tag = add_xml_node(collada_tag, "library_physics_models");
    physics_model_tag          = add_xml_node(library_physics_models_tag, "physics_model");
    add_xml_attr_str(physics_model_tag, "id",   "Physics_Model");
    add_xml_attr_str(physics_model_tag, "name", "Physics_Model");

    // library_physics_scenes
    library_physics_scenes_tag = add_xml_node(collada_tag, "library_physics_scenes");
    physics_scene_tag          = add_xml_node(library_physics_scenes_tag, "physics_scene");
    add_xml_attr_str(physics_scene_tag, "id",   "Physics_Scene");
    add_xml_attr_str(physics_scene_tag, "name", "Physics_Scene");
    instance_physics_model_tag = add_xml_node(physics_scene_tag, "instance_physics_model");
    add_xml_attr_str(instance_physics_model_tag, "url", "#Physics_Model");
    //tXML* technique_tag = add_xml_node(physics_scene_tag, "technique_common");
    //tXML* gravity_tag = add_xml_node(technique_tag, "gravity");
    //add_xml_content(gravity_tag, "0.000000 -9.810000 0.000000");

    // library_visual_scenes
    library_visual_scenes_tag = add_xml_node(collada_tag, "library_visual_scenes");
    visual_scene_tag          = add_xml_node(library_visual_scenes_tag, "visual_scene");
    add_xml_attr_str(visual_scene_tag, "id",   "Scene");
    add_xml_attr_str(visual_scene_tag, "name", "Scene");

    // scence
    scene_tag = add_xml_node(collada_tag, "scene");
    instance_physics_scene_tag = add_xml_node(scene_tag, "instance_physics_scene");
    add_xml_attr_str(instance_physics_scene_tag, "url", "#Physics_Scene");
    instance_visual_scene_tag = add_xml_node(scene_tag, "instance_visual_scene");
    add_xml_attr_str(instance_visual_scene_tag, "url", "#Scene");
    //

    free_Buffer(&buf);
}


void  ColladaXML::addObject(MeshObjectData* meshdata, bool collider, SkinJointData* joints, tXML* joints_template)
{
    if (meshdata==NULL) return;

    if (joints!=NULL && joints_template!=NULL) {
        if (joints_template_tag==NULL) {
            has_joints = true;
            joints_template_tag = joints_template;
        }
        else {
            del_all_xml(&joints_template);
        }
    }
    //
    char* geom_id = addGeometry(meshdata);                          // 幾何情報を配置
    char* ctrl_id = addController(geom_id, meshdata, joints);       // Joints 情報を配置
    addScene(geom_id, ctrl_id, meshdata, collider, joints);         // Scene への配置（位置，サイズ，回転，コライダー, Joints）
    
    if (geom_id!=NULL) ::free(geom_id);
    if (ctrl_id!=NULL) ::free(ctrl_id);
    return;
}


void  ColladaXML::closeSolid(void)
{
    if (has_joints) {
        tXML* pelvis_tag = get_xml_attr_node(joints_template_tag, "name","\"mPelvis\"");
        if (pelvis_tag!=NULL) {
            deleteNousedJoints(pelvis_tag);
        }
        else {
            PRINT_MESG("WARNING: ColladaXML::closeObject: not found mPelvis in Joints template file.\n");
        }
    }

    return;
}


char*  ColladaXML::addController(const char* geometry_id, MeshObjectData* meshdata, SkinJointData* joints)
{
    if (geometry_id==NULL || meshdata==NULL || joints==NULL) return NULL;

    Buffer geometry_name = dup_Buffer(meshdata->data_name);
    if (geometry_name.buf==NULL) geometry_name = make_Buffer_str(geometry_id+1);

    Buffer randomstr = make_Buffer_randomstr(8);

    Buffer controller_id = make_Buffer_str("#AVATAR_");
    cat_Buffer(&randomstr, &controller_id);

    tXML* controller_tag = add_xml_node(library_controllers_tag, "controller");
    add_xml_attr_str(controller_tag, "id", _tochar(controller_id.buf + 1));
    add_xml_attr_str(controller_tag, "name",  "avater");

    tXML* skin_tag = add_xml_node(controller_tag, "skin");
    add_xml_attr_str(skin_tag, "source", _tochar(geometry_id));

    // bind_shape_matrix
    tXML* bind_shape_tag = add_xml_node(skin_tag, "bind_shape_matrix");
    for (int i=1; i<=4; i++) {
        for (int j=1; j<=4; j++) {
            append_xml_content_node(bind_shape_tag, dtostr(joints->bind_shape.matrix.element(i, j)));
        }
    }

    // source JOINT
    Buffer joint_id = make_Buffer_str("#SOURCE_JOINT_");
    cat_Buffer(&randomstr, &joint_id);
    Buffer joint_name_id = make_Buffer_str("#SOURCE_JOINT_ARRAY_");
    cat_Buffer(&randomstr, &joint_name_id);

    tXML* joint_tag = add_xml_node(skin_tag, "source");
    add_xml_attr_str(joint_tag, "id", _tochar(joint_id.buf + 1));

    int joints_num = joints->joint_names.get_size();
    tXML* joint_name_tag = add_xml_node(joint_tag, "Name_array");
    add_xml_attr_str(joint_name_tag, "id", _tochar(joint_name_id.buf + 1));
    add_xml_attr_int(joint_name_tag, "count", joints_num);

    for (int jnt=0; jnt<joints_num; jnt++) {
        const char* joint_name = (const char*)joints->joint_names.get_value(jnt);
        append_xml_content_node(joint_name_tag, joint_name);
    }
    addSimpleTechniqueAccessor(joint_tag, _tochar(joint_name_id.buf), joints_num, 1, "JOINT", "name");

    // source INVERSE_BIND_MATRIX
    Buffer invbind_id = make_Buffer_str("#SOURCE_INVBIND_");
    cat_Buffer(&randomstr, &invbind_id);
    Buffer invbind_float_id = make_Buffer_str("#SOURCE_INVBIND_ARRAY_");
    cat_Buffer(&randomstr, &invbind_float_id);

    tXML* invbind_tag = add_xml_node(skin_tag, "source");
    add_xml_attr_str(invbind_tag, "id", _tochar(invbind_id.buf + 1));

    tXML* invbind_float_tag = add_xml_node(invbind_tag, "float_array");
    add_xml_attr_str(invbind_float_tag, "id", _tochar(invbind_float_id.buf + 1));
    add_xml_attr_int(invbind_float_tag, "count", joints_num*16);

    for (int jnt=0; jnt<joints_num; jnt++) {
        for (int i=1; i<=4; i++) {
            for (int j=1; j<=4; j++) {
                append_xml_content_node(invbind_float_tag, dtostr(joints->inverse_bind[jnt].matrix.element(i, j)));
            }
        }
    }
    addSimpleTechniqueAccessor(invbind_tag, _tochar(invbind_float_id.buf), joints_num, 16, "TRANSFORM", "float4x4");

    // source WEIGHT
    int vec_len = sizeof(Vector<int>)*meshdata->ttl_vertex*joints_num;
    Vector<int>* weight_index = (Vector<int>*)malloc(vec_len);
    if (weight_index==NULL) return NULL;
    memset(weight_index, 0, vec_len);
    char* weight_id = addWeightSource(skin_tag, meshdata, weight_index, joints_num);

    // joints
    tXML* joints_tag = add_xml_node(skin_tag, "joints");
    tXML* input_joint_tag = add_xml_node(joints_tag, "input");
    add_xml_attr_str(input_joint_tag, "semantic", "JOINT");
    add_xml_attr_str(input_joint_tag, "source", _tochar(joint_id.buf));

    tXML* input_invbind_tag = add_xml_node(joints_tag, "input");
    add_xml_attr_str(input_invbind_tag, "semantic", "INV_BIND_MATRIX");
    add_xml_attr_str(input_invbind_tag, "source", _tochar(invbind_id.buf));

    // vertex_weights
    tXML* vertex_weights_tag = add_xml_node(skin_tag, "vertex_weights");
    tXML* input_joint2_tag = add_xml_node(vertex_weights_tag, "input");
    add_xml_attr_str(input_joint2_tag, "semantic", "JOINT");
    add_xml_attr_str(input_joint2_tag, "source", _tochar(joint_id.buf));
    add_xml_attr_int(input_joint2_tag, "offset", 0);

    tXML* input_weight_tag = add_xml_node(vertex_weights_tag, "input");
    add_xml_attr_str(input_weight_tag, "semantic", "WEIGHT");
    add_xml_attr_str(input_weight_tag, "source", weight_id);
    add_xml_attr_int(input_weight_tag, "offset", 1);

    tXML* vcount_tag = add_xml_node(vertex_weights_tag, "vcount");
    tXML* vindex_tag = add_xml_node(vertex_weights_tag, "v");

    int snum = 0;
    int wnum = 0;
    int vnum = 0;
    int prev_vertex = -1;
    //
    for (int i=0; i<meshdata->ttl_vertex*joints_num; i++) {
        if (weight_index[i].z==TRUE) {
            if (prev_vertex==-1) prev_vertex = weight_index[i].x;               // vertex number
            append_xml_content_node(vindex_tag, itostr(weight_index[i].y));     // joint number
            append_xml_content_node(vindex_tag, itostr(wnum));
            //            
            if (prev_vertex!=weight_index[i].x) {
                append_xml_content_node(vcount_tag, itostr(vnum));
                prev_vertex = weight_index[i].x;
                snum++;
                vnum = 1;
            }
            else vnum++;
            wnum++;
        }
    }
    append_xml_content_node(vcount_tag, itostr(vnum));
    snum++;
    
    if (snum!=meshdata->ttl_vertex) {
        PRINT_MESG("WARNING: ColladaXML::addController: Vcount and Vertex number missmatched (%d != %d), Valid data num = %d\n", snum, meshdata->ttl_vertex, wnum);
    }
    add_xml_attr_str(vertex_weights_tag, "count", itostr(snum));

    free_Buffer(&geometry_name);
    free_Buffer(&randomstr);
    free_Buffer(&joint_id);
    free_Buffer(&joint_name_id);
    free_Buffer(&invbind_id);
    free_Buffer(&invbind_float_id);
    if (weight_id!=NULL) ::free(weight_id);

    return _tochar(controller_id.buf);
}


char*  ColladaXML::addGeometry(MeshObjectData* meshdata)
{
    if (meshdata==NULL) return NULL;

    // ID String
    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer geometry_id = make_Buffer_str("#GEOMETRY_");
    cat_Buffer(&randomstr, &geometry_id);

    Buffer geometry_name = dup_Buffer(meshdata->data_name);
    if (geometry_name.buf==NULL) geometry_name = make_Buffer_str(geometry_id.buf + 1);

    // library_geometries
    tXML* geomrtry_tag = add_xml_node(library_geometries_tag, "geometry");
    add_xml_attr_str(geomrtry_tag, "id",   _tochar(geometry_id.buf + 1));
    add_xml_attr_str(geomrtry_tag, "name", _tochar(geometry_name.buf));
    tXML* mesh_tag = add_xml_node(geomrtry_tag, "mesh");

    // Source & Vertices
    char* psitin_id = addVertexSource(mesh_tag, meshdata);
    char* normal_id = addNormalSource(mesh_tag, meshdata);
    char* texcrd_id = addTexcrdSource(mesh_tag, meshdata);
    char* vertex_id = addVerticesPos (mesh_tag, psitin_id);

    // Polylist
    addPolylists(mesh_tag, meshdata, vertex_id, normal_id, texcrd_id);

    freeNull(psitin_id);
    freeNull(normal_id);
    freeNull(vertex_id);
    freeNull(texcrd_id);

    free_Buffer(&randomstr);
    free_Buffer(&geometry_name);

    return _tochar(geometry_id.buf);
}


char*  ColladaXML::addVertexSource(tXML* tag, MeshObjectData* meshdata)
{
    if (tag==NULL || meshdata==NULL) return NULL;

    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer source_id = make_Buffer_str("#SOURCE_POS_");
    Buffer source_array_id = make_Buffer_str("#SOURCE_POS_ARRAY_");
    cat_Buffer(&randomstr, &source_id);
    cat_Buffer(&randomstr, &source_array_id);

    int vnum = meshdata->ttl_vertex;
    tXML* source_tag = add_xml_node(tag, "source");
    add_xml_attr_str(source_tag, "id", _tochar(source_id.buf + 1));
    tXML* source_array_tag = add_xml_node(source_tag, "float_array");
    add_xml_attr_str(source_array_tag, "id", _tochar(source_array_id.buf + 1));
    add_xml_attr_int(source_array_tag, "count", vnum*3);                            // 3 -> X, Y, Z の次元数
    //
    if (add_xml_content_area(source_array_tag, meshdata->ttl_vertex*10)) {
        MeshFacetNode* facet = meshdata->facet;
        while (facet!=NULL) {
            Vector<double>* vect = facet->vertex_value;
            for (int i=0; i<facet->num_vertex; i++) {
                append_xml_content_node(source_array_tag, dtostr(vect[i].x));
                append_xml_content_node(source_array_tag, dtostr(vect[i].y));
                append_xml_content_node(source_array_tag, dtostr(vect[i].z));
            }
            facet = facet->next;
        }   
    }
    addPosTechniqueAccessor(source_tag, _tochar(source_array_id.buf), vnum);
    
    free_Buffer(&randomstr);
    free_Buffer(&source_array_id);

    return _tochar(source_id.buf);
}


char*  ColladaXML::addNormalSource(tXML* tag, MeshObjectData* meshdata)
{
    if (tag==NULL || meshdata==NULL) return NULL;

    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer source_id = make_Buffer_str("#SOURCE_NORMAL_");
    Buffer source_array_id = make_Buffer_str("#SOURCE_NORMAL_ARRAY_");
    cat_Buffer(&randomstr, &source_id);
    cat_Buffer(&randomstr, &source_array_id);

    int vnum = meshdata->ttl_vertex;
    tXML* source_tag = add_xml_node(tag, "source");
    add_xml_attr_str(source_tag, "id", _tochar(source_id.buf + 1));
    tXML* source_array_tag = add_xml_node(source_tag, "float_array");
    add_xml_attr_str(source_array_tag, "id", _tochar(source_array_id.buf + 1));
    add_xml_attr_int(source_array_tag, "count", vnum*3);                            // 3 -> X, Y, Z の次元数
    //
    if (add_xml_content_area(source_array_tag, meshdata->ttl_vertex*10)) {
        MeshFacetNode* facet = meshdata->facet;
        while (facet!=NULL) {
            Vector<double>* vect = facet->normal_value;
            for (int i=0; i<facet->num_vertex; i++) {
                append_xml_content_node(source_array_tag, dtostr(vect[i].x));
                append_xml_content_node(source_array_tag, dtostr(vect[i].y));
                append_xml_content_node(source_array_tag, dtostr(vect[i].z));
            }
            facet = facet->next;
        }
    }
    addPosTechniqueAccessor(source_tag, _tochar(source_array_id.buf), vnum);
    
    free_Buffer(&randomstr);
    free_Buffer(&source_array_id);

    return _tochar(source_id.buf);
}


// UVマップの出力
char*  ColladaXML::addTexcrdSource(tXML* tag, MeshObjectData* meshdata)
{
    if (tag==NULL || meshdata==NULL) return NULL;

    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer source_id = make_Buffer_str("#SOURCE_MAP_");
    Buffer source_array_id = make_Buffer_str("#SOURCE_MAP_ARRAY_");
    cat_Buffer(&randomstr, &source_id);
    cat_Buffer(&randomstr, &source_array_id);

    int vnum = meshdata->ttl_vertex;
    tXML* source_tag = add_xml_node(tag, "source");
    add_xml_attr_str(source_tag, "id", _tochar(source_id.buf + 1));
    tXML* source_array_tag = add_xml_node(source_tag, "float_array");
    add_xml_attr_str(source_array_tag, "id", _tochar(source_array_id.buf + 1));
    add_xml_attr_int(source_array_tag, "count", vnum*2);                            // 2 -> S,T (U, V) の次元数
    //
    if (add_xml_content_area(source_array_tag, meshdata->ttl_texcrd*10)) {
        MeshFacetNode* facet = meshdata->facet;
        while (facet!=NULL) {
            size_t len = facet->num_texcrd*sizeof(UVMap<double>);
            UVMap<double>* uvmap = (UVMap<double>*)malloc(len);
            if (uvmap!=NULL) {
                memcpy(uvmap, facet->texcrd_value, len);
                // PLANAR Texture
                if (facet->material_param.mapping==MATERIAL_MAPPING_PLANAR) {
                    Vector<double> scale(1.0, 1.0, 1.0);
                    if (meshdata->affineTrans!=NULL) scale = meshdata->affineTrans->scale;
                    facet->generatePlanarUVMap(scale, uvmap);
                }
                facet->execAffineTransUVMap(uvmap, facet->num_texcrd);

                for (int i=0; i<facet->num_texcrd; i++) {
                    append_xml_content_node(source_array_tag, dtostr(uvmap[i].u));
                    append_xml_content_node(source_array_tag, dtostr(uvmap[i].v));
                }
                ::free(uvmap);
            }
            facet = facet->next;
        }
    }
    addMapTechniqueAccessor(source_tag, _tochar(source_array_id.buf), vnum);
    
    free_Buffer(&randomstr);
    free_Buffer(&source_array_id);

    return _tochar(source_id.buf);
}


/**
char*  ColladaXML::addWeightSource(tXML* tag, MeshObjectData* meshdata, Vector<int>* weight_index, int joints_num)

@retval  weight_index[].x: vertex No.
@retval  weight_index[].y: joint No.
@retval  weight_index[].z: FALSE->データなし, TRUE->データあり
@return  source_id
*/
char*  ColladaXML::addWeightSource(tXML* tag, MeshObjectData* meshdata, Vector<int>* weight_index, int joints_num)
{
    if (tag==NULL || meshdata==NULL || weight_index==NULL) return NULL;

    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer source_id = make_Buffer_str("#SOURCE_WEIGHT_");
    Buffer source_array_id = make_Buffer_str("#SOURCE_WEIGHT_ARRAY_");
    cat_Buffer(&randomstr, &source_id);
    cat_Buffer(&randomstr, &source_array_id);

    tXML* source_tag = add_xml_node(tag, "source");
    add_xml_attr_str(source_tag, "id", _tochar(source_id.buf + 1));
    tXML* source_array_tag = add_xml_node(source_tag, "float_array");
    add_xml_attr_str(source_array_tag, "id", _tochar(source_array_id.buf + 1));
    //
    int count = 0;
    int vnum  = 0;
    if (add_xml_content_area(source_array_tag, meshdata->ttl_vertex*10)) {          // 1データ 10桁の領域．予め確保した方が早い．
        MeshFacetNode* facet = meshdata->facet;
        while (facet!=NULL) {
            ArrayParam<int>* weight = facet->weight_value;
            for (int i=0; i<facet->num_vertex; i++) {
                int total = 0;
                int jnum = weight[i].get_size();        // その頂点で有効なジョイントの最大値
                for (int j=0; j<jnum; j++) {
                    total += weight[i].get_value(j);
                }

                if (total!=0) {
                    for (int j=0; j<joints_num; j++) {
                        weight_index[joints_num*vnum + j].x = i;
                        weight_index[joints_num*vnum + j].y = j;
                        weight_index[joints_num*vnum + j].z = FALSE;

                        int weight_value = 0;
                        if (j<jnum) weight_value = weight[i].get_value(j);

                        if (weight_value!=0) {
                            double value = weight_value/(double)total;
                            append_xml_content_node(source_array_tag, dtostr(value));
                            weight_index[vnum*joints_num + j].z = TRUE;
                            count++;
                        }
                    }
                }
                else {
                    weight_index[vnum*joints_num].x = i;
                    weight_index[vnum*joints_num].y = 0;
                    weight_index[vnum*joints_num].z = TRUE;
                    append_xml_content_node(source_array_tag, "0.000000");
                    count++;
                }
                vnum++;
            }
            facet = facet->next;
        }
    }
    add_xml_attr_int(source_array_tag, "count", count);

    if (vnum!=meshdata->ttl_vertex) {
        PRINT_MESG("WARNING: ColladaXML::addWeightSource: no match vertex num (%d != %d)\n", vnum, meshdata->ttl_vertex);
    }
    addSimpleTechniqueAccessor(source_tag, _tochar(source_array_id.buf), count, 1, "WEIGHT", "float");

    free_Buffer(&randomstr);
    free_Buffer(&source_array_id);
    return _tochar(source_id.buf);
}


char*  ColladaXML::addVerticesPos(tXML* tag, const char* position_id)
{
    if (tag==NULL || position_id==NULL) return NULL;

    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer vertex_id = make_Buffer_str("#VERTEX_");
    cat_Buffer(&randomstr, &vertex_id);

    // Vertices
    tXML* vertices_tag = add_xml_node(tag, "vertices");
    add_xml_attr_str(vertices_tag, "id", _tochar(vertex_id.buf + 1));
    tXML* input_position_tag = add_xml_node(vertices_tag, "input");
    add_xml_attr_str(input_position_tag, "semantic", "POSITION");
    add_xml_attr_str(input_position_tag, "source", position_id);

    free_Buffer(&randomstr);

    return _tochar(vertex_id.buf);
}


void   ColladaXML::addSimpleTechniqueAccessor(tXML* source_tag, const char* source_array_id, int count, int stride, const char* name, const char* type)
{
    if (source_tag==NULL || source_array_id==NULL) return;

    tXML* technique_common_tag = add_xml_node(source_tag, "technique_common");
    tXML* accessor_tag  = add_xml_node(technique_common_tag, "accessor");
    add_xml_attr_str(accessor_tag, "source", source_array_id);
    add_xml_attr_int(accessor_tag, "count", count);
    add_xml_attr_int(accessor_tag, "stride", stride);
    tXML* param_tag = add_xml_node(accessor_tag, "param");
    add_xml_attr_str(param_tag, "name", name);
    add_xml_attr_str(param_tag, "type", type);

    return;
}


void   ColladaXML::addPosTechniqueAccessor(tXML* source_tag, const char* source_array_id, int count)
{
    if (source_tag==NULL || source_array_id==NULL) return;

    tXML* technique_common_tag = add_xml_node(source_tag, "technique_common");
    tXML* accessor_tag = add_xml_node(technique_common_tag, "accessor");
    add_xml_attr_str(accessor_tag, "source", source_array_id);
    add_xml_attr_int(accessor_tag, "count", count);
    add_xml_attr_int(accessor_tag, "stride", 3);

    tXML* param_x_tag = add_xml_node(accessor_tag, "param");
    tXML* param_y_tag = add_xml_node(accessor_tag, "param");
    tXML* param_z_tag = add_xml_node(accessor_tag, "param");
    add_xml_attr_str(param_x_tag, "name", "X");
    add_xml_attr_str(param_y_tag, "name", "Y");
    add_xml_attr_str(param_z_tag, "name", "Z");
    add_xml_attr_str(param_x_tag, "type", "float");
    add_xml_attr_str(param_y_tag, "type", "float");
    add_xml_attr_str(param_z_tag, "type", "float");

    return;
}


void   ColladaXML::addMapTechniqueAccessor(tXML* source_tag, const char* source_array_id, int count)
{
    if (source_tag==NULL || source_array_id==NULL) return;

    tXML* technique_common_tag = add_xml_node(source_tag, "technique_common");
    tXML* accessor_tag = add_xml_node(technique_common_tag, "accessor");
    add_xml_attr_str(accessor_tag, "source", source_array_id);
    add_xml_attr_int(accessor_tag, "count", count);
    add_xml_attr_int(accessor_tag, "stride", 2);

    tXML* param_s_tag = add_xml_node(accessor_tag, "param");
    tXML* param_t_tag = add_xml_node(accessor_tag, "param");
    add_xml_attr_str(param_s_tag, "name", "S");
    add_xml_attr_str(param_t_tag, "name", "T");
    add_xml_attr_str(param_s_tag, "type", "float");
    add_xml_attr_str(param_t_tag, "type", "float");

    return;
}


// Polylists
void   ColladaXML::addPolylists(tXML* mesh_tag, MeshObjectData* meshdata, const char* vertex_id, const char* normal_id, const char* texcrd_id)
{
    if (mesh_tag==NULL || vertex_id==NULL || meshdata==NULL) return;

    int pnum = 0;
    MeshFacetNode* facet = meshdata->facet;

    while (facet!=NULL) {
        //
        tXML* polylist_tag = add_xml_node(mesh_tag, "polylist");
        add_xml_attr_int(polylist_tag, "count", facet->num_polygon);

        // Material
        if (facet->material_param.enable && facet->material_id.buf!=NULL) {
            //
            add_xml_attr_str(polylist_tag, "material", _tochar(facet->material_id.buf + 1));
            if (!facet->same_material) {
                char* material = _tochar(facet->material_id.buf + 1);
                bool exist_same_material = existSameID(library_materials_tag, "<library_materials><material>", material);
                if (!exist_same_material) { // 一番最初
                    char* material_url = addMaterial(_tochar(facet->material_id.buf + 1));
                    char* file_id = addImage(facet->material_param.getTextureName());
                    tXML* profile_tag = addEffect(material_url, file_id, facet->material_param);
                    ::free(material_url);
                    ::free(file_id);
                    // Bumpmap
                    char* bump_id = addImage(facet->material_param.getBumpMapName());
                    if (bump_id!=NULL) {
                        addExtraBumpmap(profile_tag, bump_id);
                        ::free(bump_id);
                    }
                }
            }
        }

        //
        tXML* input_vertex_tag = add_xml_node(polylist_tag, "input");
        add_xml_attr_str(input_vertex_tag, "semantic", "VERTEX");
        add_xml_attr_str(input_vertex_tag, "source", vertex_id);
        add_xml_attr_int(input_vertex_tag, "offset", 0);

        if (normal_id!=NULL) {
            tXML* input_normal_tag = add_xml_node(polylist_tag, "input");
            add_xml_attr_str(input_normal_tag, "semantic", "NORMAL");
            add_xml_attr_str(input_normal_tag, "source", normal_id);
            add_xml_attr_int(input_normal_tag, "offset", 0);
        }
    
        if (texcrd_id!=NULL) {
            tXML* input_uvmap_tag = add_xml_node(polylist_tag, "input");
            add_xml_attr_str(input_uvmap_tag, "semantic", "TEXCOORD");
            add_xml_attr_str(input_uvmap_tag, "source", texcrd_id);
            add_xml_attr_int(input_uvmap_tag, "offset", 0);
        }
    
        tXML* vcount_tag = add_xml_node(polylist_tag, "vcount");
        if (add_xml_content_area(vcount_tag, facet->num_polygon*10)) {          // 1データ 10桁の領域．予め確保した方が早い．
            for (int i=0; i<facet->num_polygon; i++) {
                append_xml_content_node(vcount_tag, itostr(meshdata->num_vcount));
            }
        }

        tXML* p_data_tag = add_xml_node(polylist_tag, "p");
        if (add_xml_content_area(p_data_tag, meshdata->num_vcount*10)) {        // 1データ 10桁の領域．予め確保した方が早い．
            for (int i=0; i<facet->num_polygon; i++) {
                int n = i*meshdata->num_vcount;
                for (int j=0; j<meshdata->num_vcount; j++) {
                    append_xml_content_node(p_data_tag, itostr(facet->data_index[n+j] + pnum));
                }
                append_xml_content_node(p_data_tag, "");
            }
        }

        pnum += facet->num_vertex;
        facet = facet->next;
    }
    return;
}


char*  ColladaXML::addImage(const char* fn)
{
    if (fn==NULL) return NULL;

    Buffer filename = make_Buffer_str(fn);
    Buffer temp_id  = replace_sBuffer_str(filename, ".", "_");
    Buffer file_id  = replace_sBuffer_str(temp_id , " ", "_");
    free_Buffer(&temp_id);

    bool exist_same_image = existSameID(library_images_tag, "<library_images><image>", _tochar(file_id.buf));
    if (!exist_same_image) {
        tXML* image_tag = add_xml_node(library_images_tag, "image");
        add_xml_attr_str(image_tag, "id",   _tochar(file_id.buf));
        add_xml_attr_str(image_tag, "name", _tochar(file_id.buf));
        add_xml_attr_int(image_tag, "height", 0);
        add_xml_attr_int(image_tag, "width",  0);

        tXML* init_from_tag = add_xml_node(image_tag, "init_from");
        append_xml_content_node(init_from_tag, _tochar(filename.buf));
    }

    free_Buffer(&filename);

    return _tochar(file_id.buf);
}


char*  ColladaXML::addMaterial(const char* material_id)
{
    if (material_id==NULL) return NULL;

    Buffer material_url = make_Buffer_str("#");
    cat_s2Buffer(material_id, &material_url);
    cat_s2Buffer("_URL", &material_url);

    tXML* material_tag = add_xml_node(library_materials_tag, "material");
    add_xml_attr_str(material_tag, "id", material_id);
    tXML* instance_effect_tag = add_xml_node(material_tag, "instance_effect");
    add_xml_attr_str(instance_effect_tag, "url", _tochar(material_url.buf));

    return _tochar(material_url.buf);
}


tXML*  ColladaXML::addEffect(const char* material_url, const char* file_id, MaterialParam mparam)
{
    if (material_url==NULL || file_id==NULL) return NULL;

    Buffer fid = make_Buffer_str(file_id);
    Buffer srf = make_Buffer_str(file_id);
    Buffer smp = make_Buffer_str(file_id);

    Buffer randomstr = make_Buffer_randomstr(5);
    cat_s2Buffer("-", &srf);
    cat_s2Buffer("-", &smp);
    cat_s2Buffer(randomstr.buf, &srf);
    cat_s2Buffer(randomstr.buf, &smp);
    cat_s2Buffer("-surface", &srf);
    cat_s2Buffer("-sampler", &smp);
    free_Buffer(&randomstr);

    tXML* effect_tag = add_xml_node(library_effects_tag, "effect");
    add_xml_attr_str(effect_tag, "id", material_url+1);
    tXML* profile_tag = add_xml_node(effect_tag, "profile_COMMON");

    if (!isBlankTexture(mparam.getTextureName())) {
        tXML* newparam_tag = add_xml_node(profile_tag, "newparam");
        add_xml_attr_str(newparam_tag, "sid", _tochar(srf.buf));
        tXML* surface_tag = add_xml_node(newparam_tag, "surface");
        add_xml_attr_str(surface_tag, "type", "2D");
        tXML* init_from_tag = add_xml_node(surface_tag, "init_from");
        append_xml_content_node(init_from_tag, _tochar(fid.buf));

        newparam_tag = add_xml_node(profile_tag, "newparam");
        add_xml_attr_str(newparam_tag, "sid", _tochar(smp.buf));
        tXML* sample_tag = add_xml_node(newparam_tag, "sampler2D");
        tXML* source_tag = add_xml_node(sample_tag, "source");
        append_xml_content_node(source_tag, _tochar(srf.buf));
    }

    tXML* technique_tag = add_xml_node(profile_tag, "technique");
    add_xml_attr_str(technique_tag, "sid", "common");
    tXML* phong_tag = add_xml_node(technique_tag, "phong");

    // emission (glow)
    if (mparam.isSetGlow()) {
        tXML* emission_tag = add_xml_node(phong_tag, "emission");
        tXML* color_tag = add_xml_node(emission_tag, "color");
        for (int i=0; i<3; i++) append_xml_content_node(color_tag, dtostr(mparam.getGlow()));
        append_xml_content_node(color_tag, "1.0");   // alpha
    }

    // ambient (bright)
    if (mparam.isSetBright()) {
        tXML* ambient_tag = add_xml_node(phong_tag, "ambient");
        tXML* color_tag = add_xml_node(ambient_tag, "color");
        for (int i=0; i<3; i++) append_xml_content_node(color_tag, dtostr(mparam.getBright()));
        append_xml_content_node(color_tag, "1.0");   // alpha
    }

    // diffuse
    tXML* diffuse_tag = NULL;
    if (!isBlankTexture(mparam.getTextureName())) {
        diffuse_tag = add_xml_node(phong_tag, "diffuse");
        tXML* texture_tag = add_xml_node(diffuse_tag, "texture");
        add_xml_attr_str(texture_tag, "texture",  _tochar(smp.buf));
        add_xml_attr_str(texture_tag, "texcoord", _tochar(fid.buf));
    }

    // color
    if (mparam.texture.isSetColor()) {
        bool no_texture = true;
        if (diffuse_tag!=NULL) no_texture = false;
        //
        if (no_texture || forUnity5) {
            // テクスチャがない場合 または Unity5 の場合は <diffuse>を新しく作成．
            // テクスチャが存在 かつ Unity5 の場合は カラーを読むために必要．
            // ただし，Unity2022, Unity2023 はカラーを読まない（たぶん）．
            diffuse_tag = add_xml_node(phong_tag, "diffuse");
        }
        //
        tXML* color_tag = add_xml_node(diffuse_tag, "color");
        //add_xml_attr_str(color_tag, "sid", "diffuse");
        for (int i=0; i<4; i++) {
            double col = mparam.texture.getColor(i);
            if (forUnity && col==0.0) col = 0.0001;
            append_xml_content_node(color_tag, dtostr(col));
        }
    }

    // transparency (alpha channel) 
    if (mparam.isTransparency()) {
        double alpha = Min(mparam.getTransparent(), mparam.texture.getColor(3));
        if (forUnity && alpha<0.01) alpha = 0.01;
        tXML* transparency_tag = add_xml_node(phong_tag, "transparency");
        tXML* transfloat_tag   = add_xml_node(transparency_tag, "float");
        append_xml_content_node(transfloat_tag, dtostr(alpha));
    }

    // specular map & shininess 
    if (mparam.isSetShininess()) {
        tXML* specular_tag = add_xml_node(phong_tag, "specular");
        tXML* color_tag = add_xml_node(specular_tag, "color");
        for (int i=0; i<4; i++) append_xml_content_node(color_tag, dtostr(mparam.specmap.getColor(i)));
        //
        tXML* shininess_tag = add_xml_node(phong_tag, "shininess");
        tXML* shinfloat_tag = add_xml_node(shininess_tag, "float");
        append_xml_content_node(shinfloat_tag, dtostr(mparam.getShininess()));
    }

    free_Buffer(&fid);
    free_Buffer(&srf);
    free_Buffer(&smp);

    return profile_tag;
}


void  ColladaXML::addExtraBumpmap(tXML* profile_tag, const char* bump_id)
{
    if (profile_tag==NULL || bump_id==NULL) return;

    Buffer fid = make_Buffer_str(bump_id);
    Buffer srf = make_Buffer_str(bump_id);
    Buffer smp = make_Buffer_str(bump_id);

    Buffer randomstr = make_Buffer_randomstr(5);
    cat_s2Buffer("-", &srf);
    cat_s2Buffer("-", &smp);
    cat_s2Buffer(randomstr.buf, &srf);
    cat_s2Buffer(randomstr.buf, &smp);
    cat_s2Buffer("-surface", &srf);
    cat_s2Buffer("-sampler", &smp);
    free_Buffer(&randomstr);

    tXML* newparam_tag = insert_xml_node(profile_tag, "newparam");
    add_xml_attr_str(newparam_tag, "sid", _tochar(smp.buf));
    tXML* sample_tag = add_xml_node(newparam_tag, "sampler2D");
    tXML* source_tag = add_xml_node(sample_tag, "source");
    append_xml_content_node(source_tag, _tochar(srf.buf));

    newparam_tag = insert_xml_node(profile_tag, "newparam");
    add_xml_attr_str(newparam_tag, "sid", _tochar(srf.buf));
    tXML* surface_tag = add_xml_node(newparam_tag, "surface");
    add_xml_attr_str(surface_tag, "type", "2D");
    tXML* init_from_tag = add_xml_node(surface_tag, "init_from");
    append_xml_content_node(init_from_tag, _tochar(fid.buf));

    tXML* technique_tag = get_xml_node_bystr(profile_tag, "<technique>");
    tXML* extra_tag = add_xml_node(technique_tag, "extra");
    tXML* techbump_tag = add_xml_node(extra_tag, "technique");
    add_xml_attr_str(techbump_tag, "profile", "BumpMap");
    tXML* bump_tag = add_xml_node(techbump_tag, "bump");

    tXML* texture_tag = add_xml_node(bump_tag, "texture");
    add_xml_attr_str(texture_tag, "texture",  _tochar(smp.buf));
    add_xml_attr_str(texture_tag, "texcoord", _tochar(fid.buf));

    free_Buffer(&fid);
    free_Buffer(&srf);
    free_Buffer(&smp);

    return;
}


/**
 Scene への配置（位置，サイズ，回転，コライダー, Joints）
*/
void  ColladaXML::addScene(const char* geometry_id, char* controller_id, MeshObjectData* meshdata, bool collider, SkinJointData* joints)
{
    if ((geometry_id==NULL && controller_id==NULL) || meshdata==NULL) return;

    bool local_affine = true;
    AffineTrans<double> affine;
    if (meshdata->affineTrans!=NULL) {
        local_affine = false;
        affine.free();
        affine = *(meshdata->affineTrans);
        //
        if (affineTrans==NULL) {
            affineTrans = new AffineTrans<double>();
            affineTrans->setShift(affine.shift);
        }
    }

    Vector<double> pelvis = Vector<double>(0.0, 0.0, 1.067);
    // joints
    if (has_joints && visual_scene_tag!=NULL) {
        char buf[LNAME];
        memset(buf, 0, LNAME);
        buf[0] = '"';

        int pelvis_num = -1;
        int joints_num = joints->joint_names.get_size();
        for (int jnt=0; jnt<joints_num; jnt++) {
            const char* joint_name = (const char*)joints->joint_names.get_value(jnt);
            if (joint_name!=NULL) {
                int len = (int)strlen(joint_name);
                memcpy(buf + 1, joint_name, len);
                buf[len + 1] = '"';
                buf[len + 2] = '\0';

                tXML* node_tag = get_xml_attr_node(joints_template_tag, "name", (const char*)buf);
                if (node_tag!=NULL) {
                    tXML* matrix_tag = node_tag->next;
                    for (int i=1; i<=4; i++) {
                        for (int j=1; j<=4; j++) {
                            double element = joints->alt_inverse_bind[jnt].matrix.element(i, j);
                            if (i==1 && j==1) set_xml_content_node(matrix_tag, dtostr(element));
                            else           append_xml_content_node(matrix_tag, dtostr(element));
                        }
                    }
                }
                else {
                    PRINT_MESG("ERROR: ColladaXML::addScene: Joint (%s) is not found in Joints template file!\n", joint_name);
                }
                // Pelvis の座標
                if (!strcasecmp(joint_name, "mPelvis")) {
                    pelvis.x = joints->alt_inverse_bind[jnt].matrix.element(1, 4);
                    pelvis.y = joints->alt_inverse_bind[jnt].matrix.element(2, 4);
                    pelvis.z = joints->alt_inverse_bind[jnt].matrix.element(3, 4);
                    pelvis_num = jnt;
                }
            }
        }

        // joints_template の結合
        if (visual_scene_tag->next==NULL) {
            join_xml(visual_scene_tag, joints_template_tag);
        }

        //**********************************************************************************
        // jointの位置合わせ用変換行列を計算
        if (pelvis_num >= 0) {
            AffineTrans<double> joint_space = joints->inverse_bind[pelvis_num] * joints->bind_shape;
            AffineTrans<double> joint_trans = joint_space.getInvAffine();

            Vector<double> shift = joint_trans.execRotateScale(pelvis);
            joint_trans.shift = joint_trans.shift - shift;
            skeleton = affine*joint_trans;      // Joint -> Real の Affine変換
            setJointLocationMatrix();
            //
            joint_space.free();
            joint_trans.free();
        }
    }

    Buffer geometry_name = dup_Buffer(meshdata->data_name);
    if (geometry_name.buf==NULL) geometry_name = make_Buffer_str(geometry_id + 1);
    //
    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer node_id = make_Buffer_str("#NODE_");
    cat_Buffer(&randomstr, &node_id);

    tXML* node_tag = add_xml_node(visual_scene_tag, "node");
    add_xml_attr_str(node_tag, "id",   _tochar(node_id.buf + 1));
    add_xml_attr_str(node_tag, "name", _tochar(geometry_name.buf));
    add_xml_attr_str(node_tag, "type", "NODE");

    // Collider
    tXML* rigid_body_tag = NULL;
    tXML* instance_rigid_body_tag = NULL;
    if (collider) {
        rigid_body_tag = add_xml_node(physics_model_tag, "rigid_body");
        add_xml_attr_str(rigid_body_tag, "sid",  _tochar(node_id.buf + 1));
        add_xml_attr_str(rigid_body_tag, "name", _tochar(geometry_name.buf));
        //
        instance_rigid_body_tag = add_xml_node(instance_physics_model_tag, "instance_rigid_body");
        add_xml_attr_str(instance_rigid_body_tag, "body",   _tochar(node_id.buf + 1));
        add_xml_attr_str(instance_rigid_body_tag, "target", _tochar(node_id.buf));
    }
    free_Buffer(&randomstr);
    free_Buffer(&node_id);

    // 位置
    if (no_offset) affine.shift = affine.shift - affineTrans->shift;
    affine.computeMatrix();
    tXML* matrix_tag = add_xml_node(node_tag, "matrix");
    for (int i = 1; i <= 4; i++) {
        for (int j = 1; j <= 4; j++) {
            append_xml_content_node(matrix_tag, dtostr(affine.matrix.element(i, j)));
        }
    }

    // Controller
    tXML* instance_tag = NULL;
    if (controller_id!=NULL) {
        instance_tag = add_xml_node(node_tag, "instance_controller");
        add_xml_attr_str(instance_tag, "url", controller_id);
        //
        tXML* skeleton_tag = add_xml_node(instance_tag, "skeleton");
        set_xml_content_node(skeleton_tag, "mPelvis");
    }
    else {
        instance_tag = add_xml_node(node_tag, "instance_geometry");
        add_xml_attr_str(instance_tag, "url", geometry_id);
    }

    tXML* bind_material_tag = add_xml_node(instance_tag, "bind_material");
    tXML* technique_common_tag = add_xml_node(bind_material_tag, "technique_common");
    
    tXML* instance_material_tag;
    MeshFacetNode* facet = meshdata->facet;
    while(facet!=NULL) {
        if (!facet->same_material) {
            instance_material_tag = add_xml_node(technique_common_tag, "instance_material");
            add_xml_attr_str(instance_material_tag, "symbol", _tochar(facet->material_id.buf + 1));
            add_xml_attr_str(instance_material_tag, "target", _tochar(facet->material_id.buf));
        }
        facet = facet->next;
    }

    // Collider: physics_model
    if (collider) {
        technique_common_tag = add_xml_node(rigid_body_tag, "technique_common");
        tXML* dynamic_tag = add_xml_node(technique_common_tag, "dynamic");
        tXML* mass_tag    = add_xml_node(technique_common_tag, "mass");
        add_xml_content_node(dynamic_tag, "false");
        add_xml_content_node(mass_tag, "0");
        tXML* shape_tag   = add_xml_node(technique_common_tag, "shape");
        //
        if (controller_id!=NULL) {
            instance_tag = add_xml_node(shape_tag, "instance_controller");
            add_xml_attr_str(instance_tag, "url", controller_id);
        }
        else {
            instance_tag = add_xml_node(shape_tag, "instance_geometry");
            add_xml_attr_str(instance_tag, "url", geometry_id);
        }
    }

    //
    free_Buffer(&geometry_name);
    if (local_affine) affine.free();

    return;
}


bool  ColladaXML::existSameID(tXML* top, const char* tag, const char* id)
{
    bool ret = false;

    tXML* xml = xml_parse((char*)tag);
    tXML* lst = get_xml_node_list(top, xml);

    Buffer cmpid = make_Buffer_str("\"");
    cat_s2Buffer(id, &cmpid);
    cat_s2Buffer("\"", &cmpid);

    while (lst!=NULL) {
        char* attr_id = xml_get_node_attr(lst->altp, "id");
        if (!strcmp(attr_id, _tochar(cmpid.buf))) {
            ret = true;
            break;
        }
        lst = lst->next;
    }

    del_all_xml(&xml);
    del_all_xml(&lst);
    free_Buffer(&cmpid);

    return ret;
}


void  ColladaXML::outputFile(const char* fname, const char* path, int mode)
{
    char* packname = pack_head_tail_char(get_file_name(fname), ' ');
    Buffer file_name = make_Buffer_bystr(packname);
    ::free(packname);

    canonical_filename_Buffer(&file_name);
    if (file_name.buf[0]=='.') file_name.buf[0] = '_';
    //
    Buffer out_path;
    if (path==NULL) out_path = make_Buffer_bystr("./");
    else            out_path = make_Buffer_bystr(path);
    cat_Buffer(&file_name, &out_path);
    change_file_extension_Buffer(&out_path, ".dae");
    //
    FILE* fp = fopen((char*)out_path.buf, "wb");
    if (fp!=NULL) {
        output_dae(fp, mode);
        fclose(fp);
    }
    free_Buffer(&file_name);
    free_Buffer(&out_path);

    return;
}


/**
void  ColladaXML::setJointLocationMatrix(void)
*/
void  ColladaXML::setJointLocationMatrix(void)
{
    if (!has_joints) return;

    tXML* avatar_tag = get_xml_node_str(collada_tag, "<library_visual_scenes><visual_scene><node><matrix>");
    if (avatar_tag!=NULL) {
        if (no_offset) skeleton.shift = skeleton.shift - affineTrans->shift;
        skeleton.computeMatrix();
        //
        for (int i=1; i<=4; i++) {
            for (int j=1; j<=4; j++) {
                double element = skeleton.matrix.element(i, j);
                if (i==1 && j==1) set_xml_content_node(avatar_tag, dtostr(element));
                else           append_xml_content_node(avatar_tag, dtostr(element));
            }
        }
    }
    return;
}


void  ColladaXML::deleteJoint(tXML* delete_tag)
{
    if (delete_tag==NULL) return;

    if (delete_tag->next!=NULL) {
        tXML* next = delete_tag->next;
        while (next->esis!=NULL) next = next->esis;

        if (next->ysis!=NULL) {
            if (!strcasecmp((char*)next->ysis->ldat.key.buf, "extra")) {
                tXML* ysis = next->ysis;
                del_xml(&ysis);
            }
        }
        if (!strcasecmp((char*)next->ldat.key.buf, "matrix")) {
            del_xml(&next);
        }
    } 
    del_xml_node(&delete_tag);
}


// <matrix> のデータが空の Joint を削除する．
void  ColladaXML::deleteNousedJoints(tXML* delete_tag)
{
    if (delete_tag==NULL) return;

    if (!strcasecmp((char*)delete_tag->ldat.key.buf, "matrix")) {
        if (delete_tag->next==NULL) {
            if (delete_tag->prev!=NULL) delete_tag->prev->ctrl = TREE_DELETE_NODE;
        }
    }

    if (delete_tag->next!=NULL) {
        deleteNousedJoints(delete_tag->next);
    }
    if (delete_tag->ysis!=NULL) {
        deleteNousedJoints(delete_tag->ysis);
    }

    if (delete_tag->ctrl == TREE_DELETE_NODE) {
        deleteJoint(delete_tag);
    }
}


// joints_name にあるジョイントを削除する．
// 未使用
void  ColladaXML::deleteListJoints(tXML* top_tag, tList* joints_name)
{
    if (top_tag==NULL) return;

    char buf[LNAME];
    memset(buf, 0, LNAME);
    buf[0] = '"';

    tList* lp = joints_name;
    while (lp!=NULL) {
        char* jname = (char*)lp->ldat.key.buf;
        int len = (int)strlen(jname);
        memcpy(buf + 1, jname, len);
        buf[len + 1] = '"';
        buf[len + 2] = '\0';
        tXML* del_tag = get_xml_attr_node(top_tag, "name", (const char*)buf);
        //
        deleteJoint(del_tag);
        //
        lp = lp->next;
    }
    return;
}
    


////////////////////////////////////////////////////////////////////////////////
// for UNITY4

void  ColladaXML::addCenterObject(void)
{
    addCenterScene();
    return;
}


//
void  ColladaXML::addCenterScene(void)
{
    tXML* node_tag = add_xml_node(visual_scene_tag, "node");
    add_xml_attr_str(node_tag, "id",   "#NODE_DUMMY");
    add_xml_attr_str(node_tag, "name", "center_origin");
    add_xml_attr_str(node_tag, "type", "NODE");

    // 回転行列
    AffineTrans<double> affine;
    affine.computeMatrix();
    tXML* matrix_tag = add_xml_node(node_tag, "matrix");
    for (int i=1; i<=4; i++) {
        for (int j=1; j<=4; j++) {
            append_xml_content_node(matrix_tag, dtostr(affine.matrix.element(i, j)));
        }
    }
    affine.free();

    return;
}

