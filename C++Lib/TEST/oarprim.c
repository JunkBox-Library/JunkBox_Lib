/**
OAR の objects / *.xml の処理テスト用

プリムデータに対して Colladaデータを出力する

*/

#include "PrimMesher.h"
#include "MeshMerizer.h"


using namespace jbxl;


int main(int argc, char** argv)
{
//  DebugMode = ON;

    if (argc<2) {
        fprintf(stderr, "Usage.... %s oar\n", argv[0]);
        exit(1);
    }

    int shno = 0;
    PrimBaseShape* shapes;

    tXML* sxml = xml_parse_file(argv[1]);
    if (sxml!=NULL) {
        shapes = CreatePrimBaseShapesFromXML(sxml, &shno);
        del_tTree(&sxml);
        if (shapes==NULL || shno<=0) {
            fprintf(stderr, "Not found shape data in %s\n", argv[1]);
            exit(1);
        }
    }
    else {
        fprintf(stderr, "XML File %s Read Error\n", argv[1]);
        exit(1);
    }

    //ColladaXML* dae = new ColladaXML(COLLADA_LEFT_HAND);
    ColladaXML* dae = new ColladaXML();
    dae->setBlankTexture(PRIM_OS_BLANK_TEXTURE);

    tList* resourceList = add_resource_list("./assets/", 36, NULL);      // 36 is length of GUID

    double xx, yy, zz;
    xx = yy = zz = 0.0;
    for (int s=0; s<shno; s++) {
        if (s==0) {
            xx = shapes[s].affineTrans.shift.x; 
            yy = shapes[s].affineTrans.shift.y; 
            zz = shapes[s].affineTrans.shift.z; 
        }
        //
        shapes[s].affineTrans.addShift(-xx, -yy, -zz);
        MeshObjectData* data = MeshObjectDataFromPrimShape(shapes[s], resourceList, "tga", false);
        if (data!=NULL) {
            dae->addObject(data);
            freeMeshObjectData(data);
        }
    }
    dae->print(stdout, XML_INDENT_FORMAT);

    freeColladaXML(dae);
    for (int s=0; s<shno; s++) shapes[s].free();
    ::free(shapes);

    return 0;
}



