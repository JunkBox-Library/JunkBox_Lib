/**
プリムのTree生成のテスト
*/


#include "MeshMerizer.h"
#include "ColladaTool.h"

using namespace jbxl;


int main(int argc, char** argv)
{
    DebugMode = ON;

    if (argc<2) {
        PRINT_MESG("Usage... %s texture_data\n", argv[0]);
        exit(1);
    }

    int prim_type = PRIM_TYPE_BOX;

    PrimBaseShape shape;
    PrimMeshParam param;

    shape.GenerateParam(prim_type);
    param.GetParamFromShape(shape);
    shape.free();

    int trino;      // 三角ポリゴンの数
    int facetno;    // 面の数

    PrimMesh primMesh = GeneratePrimMesh(param);
    TriPolyData* tridata = TriPolyDataFromPrimMesh(primMesh, &facetno, &trino);
    primMesh.free();
    param.free();

    for (int i=0; i<trino; i++) {
        tridata[i].execScale(Vector<double>(2.0, 0.1, 2.0));
    }
    TriPolyData* orgdata = dupTriPolyData(tridata, trino);

    int N = 4;
    double th = PI/N;
    for (int n=1; n<N; n++) {
        TriPolyData* dupdata = dupTriPolyData(orgdata, trino);
        for (int i=0; i<trino; i++) {
            dupdata[i].execRotate(Quaternion(th*n, Vector<double>(0.0, 0.0, 1.0)));
        }
        tridata = joinTriPolyData(tridata, trino*n, dupdata, trino);
    }
    trino   *= N;
    facetno *= N;

/*
    MeshObjectData* data[4];
    data[0] = new MeshObjectData("Tree1");
    data[1] = new MeshObjectData("Tree2");
    data[2] = new MeshObjectData("Tree3");
    data[3] = new MeshObjectData("Tree4");

    AffineTrans affine;
    affine.setScale(0.5, 0.01, 0.5);
*/
    
    MeshObjectData* data = new MeshObjectData("Tree");

    MaterialParam mparam1;
    MaterialParam mparam2;
    MaterialParam mparam3;
    mparam1.setTexture("a.png");
    mparam2.setTexture(argv[1]);
    mparam2.shiftU =  0.25;
    mparam2.shiftV = -0.25;
    mparam2.scaleU =  0.45;
    mparam2.scaleV =  0.45;
    mparam3.dup(mparam2);
    mparam3.flipU  =  true;

/*
    for (int i=0; i<4; i++) {
        affine.setRotate(PI_DIV4*i, 0.0, 0.0, 1.0);
        data[i]->setAffineTrans(affine);
        for (int f=0; f<facetno; f++) {
            data[i]->addData(tridata, trino, f);
        }

        data[i]->setMaterial(mparam1);
        data[i]->setMaterial(mparam2);
        data[i]->setMaterial(mparam1);
        data[i]->setMaterial(mparam3);
        data[i]->setMaterial(mparam1);
        data[i]->setMaterial(mparam1);
    }
*/

    for (int f=0; f<facetno; f++) {
        data->addData(tridata, trino, f);
    }
    
    for (int i=0; i<N; i++) { 
        data->setMaterial(mparam1);
        data->setMaterial(mparam2);
        data->setMaterial(mparam1);
        data->setMaterial(mparam3);
        data->setMaterial(mparam1);
        data->setMaterial(mparam1);
    }

    //
    ColladaXML* dae = new ColladaXML();
//  for (int i=0; i<4; i++) dae->addObject(data[i]);
    dae->addObject(data);
    dae->print(stdout, XML_INDENT_FORMAT);

    //
    freeTriPolyData(tridata, trino);
//  affine.free();
    mparam1.free();
    mparam2.free();
    mparam3.free();
//  for (int i=0; i<4; i++) freeMeshObjectData(data[i]);
    freeMeshObjectData(data);
    freeColladaXML(dae);
    
    return 0;
}



