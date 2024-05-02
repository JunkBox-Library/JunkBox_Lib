/**
llmesh データ処理用

*/

#include "common.h"
#include "tools.h"
#include "llsd_tool.h"
#include "gz_tool.h"


int main(int argc, char** argv)
{
    //DebugMode = ON;

    long   sz;
    unsigned char*  buf;
    char*  key = NULL;
    FILE*  fp;

    if (argc<2) {
        fprintf(stderr,"Usage %s llmesh_file [key]\n", argv[0]);
        exit(1);
    }
    if (argc==3) key = argv[2];

    sz = file_size(argv[1]);
    if (sz<=0) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(1);
    }
    buf = (unsigned char*)malloc(sz);
    if (buf==NULL) {
        fprintf(stderr, "No more memoey!");
        exit(1);
    }
    fp = fopen(argv[1], "r");
    if (fp==NULL) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(1);
    }
    fread(buf, sz, 1, fp);
    fclose(fp);

    ////////////////////////////////////////////////
    tXML* xml = NULL; 
    int hdsz  = llsd_bin_get_length(buf, sz);
    xml = llsd_bin_parse(buf, hdsz); 
    if (xml!=NULL) {
        print_xml(stdout, xml, 2);
        del_xml(&xml);
    }

    // Skin
    tXML* skn = llsd_bin_get_block_data(buf, sz, "skin");

    // LOD
    tXML* lod = llsd_bin_get_block_data(buf, sz, "high_lod");
    tList* lpindex = get_xml_content_list_bystr(lod, "<map><key>TriangleList</key><binary>");
    tList* lppostn = get_xml_content_list_bystr(lod, "<map><key>Position</key><binary>");
    Buffer idx = decode_base64_Buffer(lpindex->altp->ldat.key);
    Buffer vrt = decode_base64_Buffer(lppostn->altp->ldat.key);

    tList* lpweght = NULL;
    Buffer wgt = init_Buffer();
    if (skn!=NULL) {
        lpweght = get_xml_content_list_bystr(lod, "<map><key>Weights</key><binary>");
        if (lpweght!=NULL && lpweght->altp!=NULL) {
            wgt = decode_base64_Buffer(lpweght->altp->ldat.key);
        }
    }

    if (key!=NULL) {
        xml= llsd_bin_get_block_data(buf, sz, key);
        if (xml!=NULL) {
            fprintf(stdout, "\n");
            print_xml(stdout, xml, 2);
        }
    }

    // Weight
    uWord* weight = llsd_bin_get_skin_weight((uByte*)wgt.buf, wgt.vldsz, vrt.vldsz/6);
    if (weight!=NULL) {
        fprintf(stdout, "\n");
        for (int i=0; i<vrt.vldsz/6; i++) {
            for (int j=0; j<LLSD_JOINT_MAX_NUMBER; j++) {
                int pos = i*LLSD_JOINT_MAX_NUMBER + j;
                if (weight[pos]!=0x0000) printf("%3d[%2d] = 0x%04x\n", i, j, weight[pos]);
            }
        }
    }
    ////////////////////////////////////////////////

    del_xml(&xml);
    del_xml(&skn);
    del_xml(&lod);
    del_tList(&lpindex);
    del_tList(&lppostn);
    del_tList(&lpweght);

    free_Buffer(&idx);
    free_Buffer(&vrt);
    free_Buffer(&wgt);

    if (weight!=NULL) free(weight);

    free(buf);
    return 0;
}

