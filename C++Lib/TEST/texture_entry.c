/*
OAR の objects/*.xml内の TextureEntry タグの処理テスト

*/


#include "PrimMesher.h"

using namespace jbxl;


int main(int argc, char** argv)
{
    if (argc<2) {
        fprintf(stderr, "Usage %s file\n", argv[0]);
        exit(1);
    }

    DebugMode = ON;

    tXML* sxml = xml_parse_file(argv[1]);
    if (sxml!=NULL) {
        //print_xml(stdout, sxml, XML_INDENT_FORMAT);
        //fprintf(stdout,"-----------------------------------------------------------------------------\n\n");

        PrimBaseShape shape;
        shape.GetParamFromXML(sxml, NULL);
        //shape.PrintParam();
        //fprintf(stdout,"-----------------------------------------------------------------------------\n\n");

        //PrimMeshParam param;
        //param.GetParamFromShape(shape);
        //param.PrintParam();
        //fprintf(stdout,"-----------------------------------------------------------------------------\n\n");

        tXML* tag = get_xml_content_bystr(sxml, "<Shape><TextureEntry>");
        if (tag!=NULL) {
            int n;
            unsigned char* text = decode_base64(tag->ldat.key.buf, &n);
            fdump(stdout, text, n);
            free(text);
        }
        fprintf(stdout, "\n");

        for (int i=0; i<PRIM_MATERIAL_NUM; i++) {
            fprintf(stdout, "[%d]\n", i);
            shape.materialParam[i].PrintParam(stdout);  
            fprintf(stdout, "\n");
        }

        tag = get_xml_content_bystr(sxml, "<Shape><ExtraParams>");
        if (tag!=NULL) {
            int n;
            unsigned char* text = decode_base64(tag->ldat.key.buf, &n);
            fdump(stdout, text, n);
            free(text);
        }


        shape.free();
        del_tTree(&sxml);
    }

    return 0;
}

