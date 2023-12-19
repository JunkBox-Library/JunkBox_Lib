/**
プリムの基本形生成のテスト
*/


#include "MeshMerizer.h"
#include "ColladaTool.h"

using namespace jbxl;


int main(int argc, char** argv)
{
    DebugMode = ON;

    int prim_type;

    if (argc<2) {
        fprintf(stderr, "Usage  %s prim_type\n", argv[0]);
        exit(1);
    }
    else {
        prim_type = atoi(argv[1]);
    }

    PrimBaseShape shape;
    PrimMeshParam param;

    shape.GenerateParam(prim_type);
    param.GetParamFromShape(shape);

    int trino;      // 三角ポリゴンの数
    int facetno;    // 面の数

    PrimMesh primMesh = GeneratePrimMesh(param);
    TriPolyData* tridata = TriPolyDataFromPrimMesh(primMesh, &facetno, &trino);
    primMesh.free();

    MeshObjectData* data = new MeshObjectData("TEST");
    AffineTrans affine;
    affine.setRotate(0.0, 0.0, 0.0, 0.0);
    data->setAffineTrans(affine);
    
    fprintf(stderr, "numPrimFacets = %d\n", facetno);

    for (int i=0; i<facetno; i++) {
        fprintf(stderr, "Adding Facet %d .... ", i);
        bool ret = data->addData(tridata, trino, i);
        if (ret) fprintf(stderr, "done\n");
        else     fprintf(stderr, "failture\n");
    }
    fprintf(stderr,"\n");

    MaterialParam mparam;
    mparam.setTexture("970efb3c-9a48-4b7d-b20d-9ddef1d7adbd_texture.jpg");
    data->setMaterial(mparam);
    data->setMaterial(mparam);
    data->setMaterial(mparam);
    data->setMaterial(mparam);
    data->setMaterial(mparam);
    data->setMaterial(mparam);

    //
    ColladaXML* dae = new ColladaXML();
    dae->addObject(data);
    dae->print(stdout, XML_INDENT_FORMAT);

    freeTriPolyData(tridata, trino);
    shape.free();
    param.free();
    affine.free();
    mparam.free();
    freeMeshObjectData(data);
    freeColladaXML(dae);

    return 0;
}



