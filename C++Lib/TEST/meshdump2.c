
/**
llmeshデータの処理テスト

*/


#include "MeshMerizer.h"
#include "Vector.h"

#include "common.h"
#include "tools.h"
#include "llsd_tool.h"
#include "gz_tool.h"

using namespace jbxl;

int main(int argc, char** argv)
{
    DebugMode = ON;

    if (argc<2) {
        fprintf(stderr,"Usage %s dump_file\n", argv[0]);
        exit(1);
    }

    int facet_num, tri_num;
    TriPolyData* tridata = TriPolyDataFromLLMeshFile(argv[1], &facet_num, &tri_num);
    if (tridata==NULL) {
        PRINT_MESG("FAIL.\n");
        exit(1);
    }

    MeshObjectData* data = new MeshObjectData("AAAAAAAAAA");
    for (int i=0; i<facet_num; i++) {
        data->addData(tridata, tri_num, i, false);
    }

    ColladaXML* dae = new ColladaXML();
    dae->setBlankTexture(PRIM_OS_BLANK_TEXTURE);
    dae->addObject(data);
    dae->print(stdout, 2);

    freeColladaXML(dae);
    freeMeshObjectData(data);
    freeTriPolyData(tridata, tri_num);

    return 0;
}

