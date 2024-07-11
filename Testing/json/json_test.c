﻿/*  
    JSON TEST Program 
*/


#include "tjson.h"

#define  GLTF_STR_COPYRIGHT    "\"from OpenSimulator\""
#define  GLTF_STR_GENERATOR    "\"JBXL glTF Tool Library (C) 2024 v1.0 by Fumi.Iseki\""
#define  GLTF_STR_VERSION      "\"2.0\""


int main()
{
    char asset[] = "{\"asset\": {\"copyright\", \"generator\", \"version\"}}";

    tJson* temp;
    tJson* json;
    json = json_parse(asset, 99);


    temp = search_double_key_json(json, "asset", "copyright", FALSE);
    if (temp!=NULL) json_set_str_val(temp, GLTF_STR_COPYRIGHT);
    temp = search_double_key_json(json, "asset", "generator", FALSE);
    if (temp!=NULL) json_set_str_val(temp, GLTF_STR_GENERATOR);
    temp = search_double_key_json(json, "asset", "version", FALSE);
    if (temp!=NULL) json_set_str_val(temp, GLTF_STR_VERSION);


    tJson* exused    = json_append_array_bykey(json, "\"extensionsUsed\"");
    tJson* scene     = json_append_array_bykey(json, "\"scene\"");
    tJson* scenes    = json_append_array_bykey(json, "\"scenes\"");
    tJson* nodes     = json_append_array_bykey(json, "\"nodes\"");
    tJson* materials = json_append_array_bykey(json, "\"materials\"");
    tJson* meshes    = json_append_array_bykey(json, "\"meshes\"");
    tJson* textures  = json_append_array_bykey(json, "\"textures\"");
    tJson* images    = json_append_array_bykey(json, "\"images\"");
    tJson* accessors = json_append_array_bykey(json, "\"accessors\"");
    tJson* bufferv   = json_append_array_bykey(json, "\"bufferViews\"");
    tJson* samplers  = json_append_array_bykey(json, "\"samplers\"");
    tJson* buffers   = json_append_array_bykey(json, "\"buffers\"");
    tJson* xxx = json_append_nodes_bystr(json, "{\"buffers\":{}}");


	print_tTree(stderr, json);
    print_json(stderr, json, JSON_INDENT_FORMAT);

    return 0;
}




