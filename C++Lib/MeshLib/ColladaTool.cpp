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
    //DEBUG_INFO("DESTRUCTOR: ColladaXML");
}


void  ColladaXML::init(double meter, int axis, const char* ver)
{
    initCollada(meter, axis, ver);
    blank_texture = init_Buffer();
    phantom_out = true;

    forUnity5 = false;
    forUnity  = true;
}


void  ColladaXML::free(void)
{
    free_Buffer(&blank_texture);
    del_xml(&xml);
}


void  ColladaXML::clear(double meter, int axis, const char* ver)
{
    del_xml(&xml);
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
    xml     = init_xml_doc();
    collada = add_xml_node(xml, "COLLADA");
    //
    add_xml_attr_str(collada, "xmlns",   COLLADA_STR_XMLNS);
    add_xml_attr_str(collada, "version", _tochar(buf.buf));

    // asset
    asset          = add_xml_node(collada, "asset");
    contributor    = add_xml_node(asset, "contributor");
    author         = add_xml_node(contributor, "author");
    authoring_tool = add_xml_node(contributor, "authoring_tool");
    created        = add_xml_node(asset, "created");
    modified       = add_xml_node(asset, "modified");
    unit           = add_xml_node(asset, "unit");
    up_axis        = add_xml_node(asset, "up_axis");
    
    //char* ltime = get_localtime('-', 'T', ':', 'Z');
    char* ltime = get_local_timestamp(time(0), "%Y-%m-%dT%H:%M:%SZ");
    //
    add_xml_content(author, COLLADA_STR_AUTHOR);
    add_xml_content(authoring_tool, COLLADA_STR_TOOL);
    add_xml_content(created,  ltime);
    add_xml_content(modified, ltime);
    add_xml_attr_str(unit, "name", "meter");
    add_xml_attr_float(unit, "meter", (float)meter);
    ::free(ltime);

    if      (axis==COLLADA_X_UP) add_xml_content(up_axis, "X_UP");
    else if (axis==COLLADA_Y_UP) add_xml_content(up_axis, "Y_UP");
    else                         add_xml_content(up_axis, "Z_UP");

    // library_images
    library_images = add_xml_node(collada, "library_images");

    // library_materials
    library_materials = add_xml_node(collada, "library_materials");

    // library_geometries
    library_geometries = add_xml_node(collada, "library_geometries");

    // library_effects
    library_effects = add_xml_node(collada, "library_effects");

    // library_materials
    //library_materials = add_xml_node(collada, "library_materials");

    // library_physics_models
    library_physics_models = add_xml_node(collada, "library_physics_models");
    physics_model          = add_xml_node(library_physics_models, "physics_model");
    add_xml_attr_str(physics_model, "id",   "Physics_Model");
    add_xml_attr_str(physics_model, "name", "Physics_Model");

    // library_physics_scenes
    library_physics_scenes = add_xml_node(collada, "library_physics_scenes");
    physics_scene          = add_xml_node(library_physics_scenes, "physics_scene");
    add_xml_attr_str(physics_scene, "id",   "Physics_Scene");
    add_xml_attr_str(physics_scene, "name", "Physics_Scene");
    instance_physics_model = add_xml_node(physics_scene, "instance_physics_model");
    add_xml_attr_str(instance_physics_model, "url", "#Physics_Model");
    //tXML* technique = add_xml_node(physics_scene, "technique_common");
    //tXML* gravity = add_xml_node(technique, "gravity");
    //add_xml_content(gravity, "0.000000 -9.810000 0.000000");

    // library_visual_scenes
    library_visual_scenes = add_xml_node(collada, "library_visual_scenes");
    visual_scene          = add_xml_node(library_visual_scenes, "visual_scene");
    add_xml_attr_str(visual_scene, "id",   "Scene");
    add_xml_attr_str(visual_scene, "name", "Scene");

    // scence
    scene = add_xml_node(collada, "scene");
    instance_physics_scene = add_xml_node(scene, "instance_physics_scene");
    add_xml_attr_str(instance_physics_scene, "url", "#Physics_Scene");
    instance_visual_scene = add_xml_node(scene, "instance_visual_scene");
    add_xml_attr_str(instance_visual_scene, "url", "#Scene");

    //
    free_Buffer(&buf);
}


void  ColladaXML::addObject(MeshObjectData* meshdata, bool collider)
{
    if (meshdata==NULL) return;

    char* geom_id = addGeometry(meshdata);      // 幾何情報を配置
    if (geom_id==NULL) return;

    addScene(geom_id, meshdata, collider);      // Scene への配置（位置，サイズ，回転，コライダー）

    ::free(geom_id);
    return;
}


char*  ColladaXML::addGeometry(MeshObjectData* meshdata)
{
    if (meshdata==NULL) return NULL;

    // ID String
    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer geometry_id = make_Buffer_str("#GEOMETRY_");
    cat_Buffer(&randomstr, &geometry_id);

    Buffer geometry_name = dup_Buffer(meshdata->data_name);
    if (geometry_name.buf==NULL) geometry_name = make_Buffer_str(geometry_id.buf+1);

    // library_geometries
    tXML* geomrtry = add_xml_node(library_geometries, "geometry");
    add_xml_attr_str(geomrtry, "id",   _tochar(geometry_id.buf+1));
    add_xml_attr_str(geomrtry, "name", _tochar(geometry_name.buf));
    tXML* mesh = add_xml_node(geomrtry, "mesh");

    // Source & Vertices
    char* psitin_id = addVertexSource(mesh, meshdata);
    char* normal_id = addNormalSource(mesh, meshdata);
    char* texcrd_id = addTexcrdSource(mesh, meshdata);
    char* vertex_id = addVerticesPos (mesh, psitin_id);

    // Polylist
    addPolylists(mesh, meshdata, vertex_id, normal_id, texcrd_id);

    freeNull(psitin_id);
    freeNull(normal_id);
    freeNull(vertex_id);
    freeNull(texcrd_id);

    free_Buffer(&randomstr);
    free_Buffer(&geometry_name);

    return _tochar(geometry_id.buf);
}


char*  ColladaXML::addVertexSource(tXML* mesh, MeshObjectData* meshdata)
{
    if (mesh==NULL || meshdata==NULL) return NULL;

    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer source_id = make_Buffer_str("#SOURCE_POS_");
    Buffer source_array_id = make_Buffer_str("#SOURCE_POS_ARRAY_");
    cat_Buffer(&randomstr, &source_id);
    cat_Buffer(&randomstr, &source_array_id);

    int vnum = meshdata->ttl_vertex;
    tXML* source = add_xml_node(mesh, "source");
    add_xml_attr_str(source, "id", _tochar(source_id.buf+1));
    tXML* source_array = add_xml_node(source, "float_array");
    add_xml_attr_str(source_array, "id", _tochar(source_array_id.buf+1));
    add_xml_attr_int(source_array, "count", vnum*3);                            // 3 -> X, Y, Z の次元数
    //
    if (add_xml_content_area(source_array, meshdata->ttl_vertex*10)) {
        MeshFacetNode* facet = meshdata->facet;
        while (facet!=NULL) {
            Vector<double>* vect = facet->vertex_value;
            for (int i=0; i<facet->num_vertex; i++) {
                append_xml_content(source_array, dtostr(vect[i].x));
                append_xml_content(source_array, dtostr(vect[i].y));
                append_xml_content(source_array, dtostr(vect[i].z));
            }
            facet = facet->next;
        }   
    }
    addPosTechniqueAccessor(source, _tochar(source_array_id.buf), vnum);
    
    free_Buffer(&randomstr);
    free_Buffer(&source_array_id);

    return _tochar(source_id.buf);
}


char*  ColladaXML::addNormalSource(tXML* mesh, MeshObjectData* meshdata)
{
    if (mesh==NULL || meshdata==NULL) return NULL;

    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer source_id = make_Buffer_str("#SOURCE_NORMAL_");
    Buffer source_array_id = make_Buffer_str("#SOURCE_NORMAL_ARRAY_");
    cat_Buffer(&randomstr, &source_id);
    cat_Buffer(&randomstr, &source_array_id);

    int vnum = meshdata->ttl_vertex;
    tXML* source = add_xml_node(mesh, "source");
    add_xml_attr_str(source, "id", _tochar(source_id.buf+1));
    tXML* source_array = add_xml_node(source, "float_array");
    add_xml_attr_str(source_array, "id", _tochar(source_array_id.buf+1));
    add_xml_attr_int(source_array, "count", vnum*3);                            // 3 -> X, Y, Z の次元数
    //
    if (add_xml_content_area(source_array, meshdata->ttl_vertex*10)) {
        MeshFacetNode* facet = meshdata->facet;
        while (facet!=NULL) {
            Vector<double>* vect = facet->normal_value;
            for (int i=0; i<facet->num_vertex; i++) {
                append_xml_content(source_array, dtostr(vect[i].x));
                append_xml_content(source_array, dtostr(vect[i].y));
                append_xml_content(source_array, dtostr(vect[i].z));
            }
            facet = facet->next;
        }
    }
    addPosTechniqueAccessor(source, _tochar(source_array_id.buf), vnum);
    
    free_Buffer(&randomstr);
    free_Buffer(&source_array_id);

    return _tochar(source_id.buf);
}


// UVマップの出力
//
char*  ColladaXML::addTexcrdSource(tXML* mesh, MeshObjectData* meshdata)
{
    if (mesh==NULL || meshdata==NULL) return NULL;

    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer source_id = make_Buffer_str("#SOURCE_MAP_");
    Buffer source_array_id = make_Buffer_str("#SOURCE_MAP_ARRAY_");
    cat_Buffer(&randomstr, &source_id);
    cat_Buffer(&randomstr, &source_array_id);

    int vnum = meshdata->ttl_vertex;
    tXML* source = add_xml_node(mesh, "source");
    add_xml_attr_str(source, "id", _tochar(source_id.buf+1));
    tXML* source_array = add_xml_node(source, "float_array");
    add_xml_attr_str(source_array, "id", _tochar(source_array_id.buf+1));
    add_xml_attr_int(source_array, "count", vnum*2);                            // 2 -> S,T (U, V) の次元数
    //
    if (add_xml_content_area(source_array, meshdata->ttl_texcrd*10)) {
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
                    append_xml_content(source_array, dtostr(uvmap[i].u));
                    append_xml_content(source_array, dtostr(uvmap[i].v));
                }
                ::free(uvmap);
            }
            facet = facet->next;
        }
    }
    addMapTechniqueAccessor(source, _tochar(source_array_id.buf), vnum);
    
    free_Buffer(&randomstr);
    free_Buffer(&source_array_id);

    return _tochar(source_id.buf);
}


char*  ColladaXML::addVerticesPos(tXML* mesh, const char* position_id)
{
    if (mesh==NULL || position_id==NULL) return NULL;

    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer vertex_id = make_Buffer_str("#VERTEX_");
    cat_Buffer(&randomstr, &vertex_id);

    // Vertices
    tXML* vertices = add_xml_node(mesh, "vertices");
    add_xml_attr_str(vertices, "id", _tochar(vertex_id.buf+1));
    tXML* position_input = add_xml_node(vertices, "input");
    add_xml_attr_str(position_input, "semantic", "POSITION");
    add_xml_attr_str(position_input, "source", position_id);

    free_Buffer(&randomstr);

    return _tochar(vertex_id.buf);
}


void   ColladaXML::addPosTechniqueAccessor(tXML* source, const char* source_array_id, int count)
{
    if (source==NULL || source_array_id==NULL) return;

    tXML* technique_common = add_xml_node(source, "technique_common");
    tXML* accessor = add_xml_node(technique_common, "accessor");
    add_xml_attr_str(accessor, "source", source_array_id);
    add_xml_attr_int(accessor, "count", count);
    add_xml_attr_int(accessor, "stride", 3);

    tXML* param_x = add_xml_node(accessor, "param");
    tXML* param_y = add_xml_node(accessor, "param");
    tXML* param_z = add_xml_node(accessor, "param");
    add_xml_attr_str(param_x, "name", "X");
    add_xml_attr_str(param_y, "name", "Y");
    add_xml_attr_str(param_z, "name", "Z");
    add_xml_attr_str(param_x, "type", "float");
    add_xml_attr_str(param_y, "type", "float");
    add_xml_attr_str(param_z, "type", "float");

    return;
}


void   ColladaXML::addMapTechniqueAccessor(tXML* source, const char* source_array_id, int count)
{
    if (source==NULL || source_array_id==NULL) return;

    tXML* technique_common = add_xml_node(source, "technique_common");
    tXML* accessor = add_xml_node(technique_common, "accessor");
    add_xml_attr_str(accessor, "source", source_array_id);
    add_xml_attr_int(accessor, "count", count);
    add_xml_attr_int(accessor, "stride", 2);

    tXML* param_s = add_xml_node(accessor, "param");
    tXML* param_t = add_xml_node(accessor, "param");
    add_xml_attr_str(param_s, "name", "S");
    add_xml_attr_str(param_t, "name", "T");
    add_xml_attr_str(param_s, "type", "float");
    add_xml_attr_str(param_t, "type", "float");

    return;
}


// Polylists
void   ColladaXML::addPolylists(tXML* mesh, MeshObjectData* meshdata, const char* vertex_id, const char* normal_id, const char* texcrd_id)
{
    if (vertex_id==NULL || meshdata==NULL) return;

    int pnum = 0;
    MeshFacetNode* facet = meshdata->facet;

    while (facet!=NULL) {
        //
        tXML* polylist = add_xml_node(mesh, "polylist");
        add_xml_attr_int(polylist, "count", facet->num_polygon);

        // Material
        if (facet->material_param.enable && facet->material_id.buf!=NULL) {
            //
            add_xml_attr_str(polylist, "material", _tochar(facet->material_id.buf+1));
            if (!facet->same_material) {
                char* material = _tochar(facet->material_id.buf+1);
                bool exist_same_material = existSameID(library_materials, "<library_materials><material>", material);
                if (!exist_same_material) { // 一番最初
                    char* material_url = addMaterial(_tochar(facet->material_id.buf+1));
                    char* file_id = addImage(facet->material_param.getTextureName());
                    tXML* profile = addEffect(material_url, file_id, facet->material_param);
                    ::free(material_url);
                    ::free(file_id);
                    // Bumpmap
                    char* bump_id = addImage(facet->material_param.getBumpMapName());
                    if (bump_id!=NULL) {
                        addExtraBumpmap(profile, bump_id);
                        ::free(bump_id);
                    }
                }
            }
        }

        //
        tXML* vertex_input = add_xml_node(polylist, "input");
        add_xml_attr_str(vertex_input, "semantic", "VERTEX");
        add_xml_attr_str(vertex_input, "source", vertex_id);
        add_xml_attr_int(vertex_input, "offset", 0);

        if (normal_id!=NULL) {
            tXML* normal_input = add_xml_node(polylist, "input");
            add_xml_attr_str(normal_input, "semantic", "NORMAL");
            add_xml_attr_str(normal_input, "source", normal_id);
            add_xml_attr_int(normal_input, "offset", 0);
        }
    
        if (texcrd_id!=NULL) {
            tXML* normal_input = add_xml_node(polylist, "input");
            add_xml_attr_str(normal_input, "semantic", "TEXCOORD");
            add_xml_attr_str(normal_input, "source", texcrd_id);
            add_xml_attr_int(normal_input, "offset", 0);
        }
    
        tXML* vcount = add_xml_node(polylist, "vcount");
        if (add_xml_content_area(vcount, facet->num_polygon*10)) {
            for (int i=0; i<facet->num_polygon; i++) {
                append_xml_content(vcount, itostr(meshdata->num_vcount));
            }
        }

        tXML* p_data = add_xml_node(polylist, "p");
        if (add_xml_content_area(p_data, meshdata->num_vcount*10)) {
            for (int i=0; i<facet->num_polygon; i++) {
                int n = i*meshdata->num_vcount;
                for (int j=0; j<meshdata->num_vcount; j++) {
                    append_xml_content(p_data, itostr(facet->data_index[n+j] + pnum));
                }
                append_xml_content(p_data, "");
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

    bool exist_same_image = existSameID(library_images, "<library_images><image>", _tochar(file_id.buf));
    if (!exist_same_image) {
        tXML* image = add_xml_node(library_images, "image");
        add_xml_attr_str(image, "id",   _tochar(file_id.buf));
        add_xml_attr_str(image, "name", _tochar(file_id.buf));
        add_xml_attr_int(image, "height", 0);
        add_xml_attr_int(image, "width",  0);

        tXML* init_from = add_xml_node(image, "init_from");
        append_xml_content(init_from, _tochar(filename.buf));
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

    tXML* material = add_xml_node(library_materials, "material");
    add_xml_attr_str(material, "id", material_id);
    tXML* instance_effect = add_xml_node(material, "instance_effect");
    add_xml_attr_str(instance_effect, "url", _tochar(material_url.buf));

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

    tXML* effect = add_xml_node(library_effects, "effect");
    add_xml_attr_str(effect, "id", material_url+1);
    tXML* profile = add_xml_node(effect, "profile_COMMON");

    if (!isBlankTexture(mparam.getTextureName())) {
        tXML* newparam;
        newparam = add_xml_node(profile, "newparam");
        add_xml_attr_str(newparam, "sid", _tochar(srf.buf));
        tXML* surface = add_xml_node(newparam, "surface");
        add_xml_attr_str(surface, "type", "2D");
        tXML* init_from = add_xml_node(surface, "init_from");
        append_xml_content(init_from, _tochar(fid.buf));

        newparam = add_xml_node(profile, "newparam");
        add_xml_attr_str(newparam, "sid", _tochar(smp.buf));
        tXML* sample = add_xml_node(newparam, "sampler2D");
        tXML* source = add_xml_node(sample, "source");
        append_xml_content(source, _tochar(srf.buf));
    }

    tXML* technique = add_xml_node(profile, "technique");
    add_xml_attr_str(technique, "sid", "common");
    tXML* phong = add_xml_node(technique, "phong");

    // emission (glow)
    if (mparam.isSetGlow()) {
        tXML* emission = add_xml_node(phong, "emission");
        tXML* color = add_xml_node(emission, "color");
        for (int i=0; i<3; i++) append_xml_content(color, dtostr(mparam.getGlow()));
        append_xml_content(color, "1.0");   // alpha
    }

    // ambient (bright)
    if (mparam.isSetBright()) {
        tXML* ambient = add_xml_node(phong, "ambient");
        tXML* color = add_xml_node(ambient, "color");
        for (int i=0; i<3; i++) append_xml_content(color, dtostr(mparam.getBright()));
        append_xml_content(color, "1.0");   // alpha
    }

    // diffuse
    tXML* diffuse = NULL;
    if (!isBlankTexture(mparam.getTextureName())) {
        diffuse = add_xml_node(phong, "diffuse");
        tXML* texture = add_xml_node(diffuse, "texture");
        add_xml_attr_str(texture, "texture",  _tochar(smp.buf));
        add_xml_attr_str(texture, "texcoord", _tochar(fid.buf));
    }

    // color
    if (mparam.texture.isSetColor()) {
        bool no_texture = true;
        if (diffuse!=NULL) no_texture = false;
        //
        if (no_texture || forUnity5) {
            // テクスチャがない場合 または Unity5 の場合は <diffuse>を新しく作成．
            // テクスチャが存在 かつ Unity5 の場合は Colladaの規格に違反しているが，カラーを読むために必要．
            // また，Unity2022, Unity2023 はカラーを読まない（たぶん）．
            diffuse = add_xml_node(phong, "diffuse");
        }
        //
        tXML* color = add_xml_node(diffuse, "color");
        //add_xml_attr_str(color, "sid", "diffuse");
        for (int i=0; i<4; i++) {
            double col = mparam.texture.getColor(i);
            if (forUnity && col==0.0) col = 0.0001;
            append_xml_content(color, dtostr(col));
        }
    }

    // transparency (alpha channel) 
    if (mparam.isTransparency()) {
        double alpha = Min(mparam.getTransparent(), mparam.texture.getColor(3));
        if (forUnity && alpha<0.01) alpha = 0.01;
        tXML* transparency = add_xml_node(phong, "transparency");
        tXML* transfloat   = add_xml_node(transparency, "float");
        append_xml_content(transfloat, dtostr(alpha));
    }

    // specular map & shininess 
    if (mparam.isSetShininess()) {
        tXML* specular = add_xml_node(phong, "specular");
        tXML* color = add_xml_node(specular, "color");
        for (int i=0; i<4; i++) append_xml_content(color, dtostr(mparam.specmap.getColor(i)));
        //
        tXML* shininess = add_xml_node(phong, "shininess");
        tXML* shinfloat = add_xml_node(shininess, "float");
        append_xml_content(shinfloat, dtostr(mparam.getShininess()));
    }

    free_Buffer(&fid);
    free_Buffer(&srf);
    free_Buffer(&smp);

    return profile;
}


void  ColladaXML::addExtraBumpmap(tXML* profile, const char* bump_id)
{
    if (profile==NULL || bump_id==NULL) return;

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

    tXML* newparam;
    newparam = insert_xml_node(profile, "newparam");
    add_xml_attr_str(newparam, "sid", _tochar(smp.buf));
    tXML* sample = add_xml_node(newparam, "sampler2D");
    tXML* source = add_xml_node(sample, "source");
    append_xml_content(source, _tochar(srf.buf));

    newparam = insert_xml_node(profile, "newparam");
    add_xml_attr_str(newparam, "sid", _tochar(srf.buf));
    tXML* surface = add_xml_node(newparam, "surface");
    add_xml_attr_str(surface, "type", "2D");
    tXML* init_from = add_xml_node(surface, "init_from");
    append_xml_content(init_from, _tochar(fid.buf));

    tXML* technique = get_xml_node_bystr(profile, "<technique>");
    tXML* extra = add_xml_node(technique, "extra");
    tXML* techbump = add_xml_node(extra, "technique");
    add_xml_attr_str(techbump, "profile", "BumpMap");
    tXML* bump = add_xml_node(techbump, "bump");

    tXML* texture = add_xml_node(bump, "texture");
    add_xml_attr_str(texture, "texture",  _tochar(smp.buf));
    add_xml_attr_str(texture, "texcoord", _tochar(fid.buf));

    free_Buffer(&fid);
    free_Buffer(&srf);
    free_Buffer(&smp);

    return;
}


/**
 Scene への配置（位置，サイズ，回転，コライダー）
*/
void  ColladaXML::addScene(const char* geometry_id, MeshObjectData* meshdata, bool collider)
{
    if (geometry_id==NULL || meshdata==NULL) return;

    bool local_affine = true;
    AffineTrans<double> affine;
    if (meshdata->affineTrans!=NULL) {
        local_affine = false;
        affine.free();
        affine = *(meshdata->affineTrans);
    }

    Buffer geometry_name = dup_Buffer(meshdata->data_name);
    if (geometry_name.buf==NULL) geometry_name = make_Buffer_str(geometry_id+1);

    //
    Buffer randomstr = make_Buffer_randomstr(8);
    Buffer node_id = make_Buffer_str("#NODE_");
    cat_Buffer(&randomstr, &node_id);

    tXML* nodetag = add_xml_node(visual_scene, "node");
    //add_xml_attr_str(nodetag, "id",   _tochar(node_id.buf));
    add_xml_attr_str(nodetag, "id",   _tochar(node_id.buf+1));
    add_xml_attr_str(nodetag, "name", _tochar(geometry_name.buf));
    add_xml_attr_str(nodetag, "type", "NODE");

    // Collider
    tXML* rigid_body = NULL;
    tXML* instance_rigid_body = NULL;
    if (collider) {
        rigid_body = add_xml_node(physics_model, "rigid_body");
        add_xml_attr_str(rigid_body, "sid",  _tochar(node_id.buf+1));
        add_xml_attr_str(rigid_body, "name", _tochar(geometry_name.buf));
        //
        instance_rigid_body = add_xml_node(instance_physics_model, "instance_rigid_body");
        add_xml_attr_str(instance_rigid_body, "body",   _tochar(node_id.buf+1));
        add_xml_attr_str(instance_rigid_body, "target", _tochar(node_id.buf));
    }

    free_Buffer(&randomstr);
    free_Buffer(&node_id);

    // 回転行列
    affine.computeMatrix(false);
    tXML* matrix = add_xml_node(nodetag, "matrix");
    for (int i=1; i<=4; i++) {
        for (int j=1; j<=4; j++) {
            append_xml_content(matrix, dtostr(affine.matrix.element(i,j)));
        }
    }

    // オイラー角
/*
    tXML* translate = add_xml_node(nodetag, "translate");
    add_xml_attr_str(translate, "sid", "location");
    append_xml_content(translate, dtostr(affine.shift.x));
    append_xml_content(translate, dtostr(affine.shift.y));
    append_xml_content(translate, dtostr(affine.shift.z));
    //
    Vector<double> euler = affine.rotate.getEulerZYX();
    tXML* rotate_z = add_xml_node(nodetag, "rotate");
    add_xml_attr_str(rotate_z, "sid", "rotationZ");
    append_xml_content(rotate_z, "0 0 1");
    append_xml_content(rotate_z, dtostr(euler.x));

    tXML* rotate_y = add_xml_node(nodetag, "rotate");
    add_xml_attr_str(rotate_y, "sid", "rotationY");
    append_xml_content(rotate_y, "0 1 0");
    append_xml_content(rotate_y, dtostr(euler.y));

    tXML* rotate_x = add_xml_node(nodetag, "rotate");
    add_xml_attr_str(rotate_x, "sid", "rotationX");
    append_xml_content(rotate_x, "1 0 0");
    append_xml_content(rotate_x, dtostr(euler.z));
*/
    
    tXML* scale = add_xml_node(nodetag, "scale");
    add_xml_attr_str(scale, "sid", "scale");
    append_xml_content(scale, dtostr(affine.scale.x));
    append_xml_content(scale, dtostr(affine.scale.y));
    append_xml_content(scale, dtostr(affine.scale.z));

    tXML* instance = add_xml_node(nodetag, "instance_geometry");
    add_xml_attr_str(instance, "url", geometry_id);

    tXML* bind_material = add_xml_node(instance, "bind_material");
    tXML* technique_common = add_xml_node(bind_material, "technique_common");
    
    tXML* instance_material;
    MeshFacetNode* facet = meshdata->facet;
    while(facet!=NULL) {
        if (!facet->same_material) {
            instance_material = add_xml_node(technique_common, "instance_material");
            add_xml_attr_str(instance_material, "symbol", _tochar(facet->material_id.buf+1));
            add_xml_attr_str(instance_material, "target", _tochar(facet->material_id.buf));
        }
        facet = facet->next;
    }

    // Collider: physics_model
    if (collider) {
        technique_common = add_xml_node(rigid_body, "technique_common");
        tXML* dynamic = add_xml_node(technique_common, "dynamic");
        tXML* mass    = add_xml_node(technique_common, "mass");
        add_xml_content(dynamic, "false");
        add_xml_content(mass, "0");
        tXML* shape   = add_xml_node(technique_common, "shape");
        instance      = add_xml_node(shape, "instance_geometry");
        add_xml_attr_str(instance, "url", geometry_id);
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

    del_xml(&xml);
    del_xml(&lst);
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
    tXML* nodetag = add_xml_node(visual_scene, "node");
    add_xml_attr_str(nodetag, "id",   "#NODE_DUMMY");
    add_xml_attr_str(nodetag, "name", "center_origin");
    add_xml_attr_str(nodetag, "type", "NODE");

    // 回転行列
    AffineTrans<double> affine;
    affine.computeMatrix(false);
    tXML* matrix = add_xml_node(nodetag, "matrix");
    for (int i=1; i<=4; i++) {
        for (int j=1; j<=4; j++) {
            append_xml_content(matrix, dtostr(affine.matrix.element(i,j)));
        }
    }
    affine.free();

    return;
}

