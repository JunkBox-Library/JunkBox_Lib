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
        fprintf(stderr, "Cannot get file size: %s\n", argv[1]);
        exit(1);
    }
    buf = (unsigned char*)malloc(sz);
    if (buf==NULL) {
        fprintf(stderr, "No more memoey!");
        exit(1);
    }
    fp = fopen(argv[1], "r");
    if (fp==NULL) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        exit(1);
    }
    fread(buf, sz, 1, fp);
    fclose(fp);

    ////////////////////////////////////////////////
    tXML* xml = NULL; 
    int hdsz  = llsd_bin_get_length(buf, sz);
    xml = llsd_bin_parse(buf, hdsz); 
    if (xml!=NULL) {
        fprintf(stdout, "=== Header (%d Byte) =========\n", hdsz);
        print_xml(stdout, xml, 2);
        del_xml(&xml);
    }

    // LOD
    tXML* lod = llsd_bin_get_block_data(buf, sz, "high_lod");
    if (lod==NULL) lod = llsd_bin_get_block_data(buf, sz, "medium_lod");
    if (lod==NULL) lod = llsd_bin_get_block_data(buf, sz, "low_lod");
    if (lod==NULL) {
        fprintf(stderr, "No LOD data\n");
        exit(1);
    }

/**
    fprintf(stdout, "\n=== LOD =========\n");
    print_tTree_tree(stdout, lod, "  ");
**/
    int n = 0;

    // INDEX
    tList* lpindex = get_xml_content_list_bystr(lod, "<map><key>TriangleList</key><binary>");
    tList* index = lpindex;
    fprintf(stdout, "\n== INDEXES =========\n");
    n = 0;
    while (index!=NULL) {
        if (index->altp!=NULL) {
            //fprintf(stdout, "%s\n", index->altp->ldat.key.buf);
            Buffer idx = decode_base64_Buffer(index->altp->ldat.key);
            fprintf(stdout, "====== INDEX (%d, %d) =========\n", n, idx.vldsz/2);
            if (n==0) {  // n>0 は省略
                uWord* dat = (uWord*)idx.buf;
                for (int i=0; i<idx.vldsz/2; i++) fprintf(stdout, "%d ", dat[i]);
                fprintf(stdout, "\n");
            }
            free_Buffer(&idx);
        }
        index = index->next;
        n++;
    }
    del_tList(&lpindex);

    // POSITION & WEIGHT
    tList* lppostn = get_xml_content_list_bystr(lod, "<map><key>Position</key><binary>");
    tList* lpweght = get_xml_content_list_bystr(lod, "<map><key>Weights</key><binary>");
    tList* postn = lppostn;
    tList* weght = lpweght;
    //
    fprintf(stdout, "\n== POSITION & WEIGHT =========\n");
    n = 0;
    while (postn!=NULL) {
        int vertex_num = 0;
        // POSITION
        if (postn->altp!=NULL) {
            //fprintf(stdout, "%s\n", postn->altp->ldat.key.buf);
            Buffer pos = decode_base64_Buffer(postn->altp->ldat.key);
            fprintf(stdout, "====== POSITION (%d, %d) =========\n", n, pos.vldsz/2);
            if (n==0) {  // n>0 は省略
                uWord* dat = (uWord*)pos.buf;
                for (int i=0; i<pos.vldsz/2; i++) fprintf(stdout, "%d ", dat[i]);
                fprintf(stdout, "\n");
            }
            vertex_num = pos.vldsz/6;
            free_Buffer(&pos);
        }

        // WEIGHT
        if (vertex_num>0 && weght!=NULL && weght->altp!=NULL) {
            //fprintf(stdout, "%s\n", weght->altp->ldat.key.buf);
            Buffer wgt = decode_base64_Buffer(weght->altp->ldat.key);
            uWord* weight = llsd_bin_get_skin_weight((uByte*)wgt.buf, wgt.vldsz, vertex_num);

            fprintf(stdout, "====== WEIGHT (%d) =========\n", n);
            if (n==0) {  // n>0 は省略
                for (int i=0; i<vertex_num; i++) {
                    for (int j=0; j<LLSD_JOINT_MAX_NUMBER; j++) {
                        int pos = i*LLSD_JOINT_MAX_NUMBER + j;
                        if (weight[pos]!=0) fprintf(stdout, "[%d,%2d = %d] ", i, j, weight[pos]);
                        //if (weight[pos]!=0) fprintf(stdout, "[%d,%2d] ", i, j);
                    }
                    fprintf(stdout, "\n");
                }
                fprintf(stdout, "\n");
            }
            free_Buffer(&wgt);
            freeNull(weight);
        }
        postn = postn->next;
        weght = weght->next;
        n++;
    }
    del_tList(&lppostn);
    del_tList(&lpweght);

    // NORMAL
    tList* lpnrmal = get_xml_content_list_bystr(lod, "<map><key>Normal</key><binary>");
    tList* nrmal = lpnrmal;
    fprintf(stdout, "\n== NORMAL =========\n");
    n = 0;
    while (nrmal!=NULL) {
        if (nrmal->altp!=NULL) {
            //fprintf(stdout, "%s\n", nrmal->altp->ldat.key.buf);
            Buffer nml = decode_base64_Buffer(nrmal->altp->ldat.key);
            fprintf(stdout, "====== NORMAL (%d, %d) =========\n", n, nml.vldsz/2);
            if (n==0) {  // n>0 は省略
                uWord* dat = (uWord*)nml.buf;
                for (int i=0; i<nml.vldsz/2; i++) fprintf(stdout, "%d ", dat[i]);
                fprintf(stdout, "\n");
            }
            free_Buffer(&nml);
        }
        nrmal = nrmal->next;
        n++;
    }
    del_tList(&lpnrmal);

    // UVMAP
    tList* lpuvmap = get_xml_content_list_bystr(lod, "<map><key>TexCoord0</key><binary>");
    tList* uvmap = lpuvmap;
    fprintf(stdout, "\n== UVMAP =========\n");
    n = 0;
    while (uvmap!=NULL) {
        if (uvmap->altp!=NULL) {
            //fprintf(stdout, "%s\n", uvmap->altp->ldat.key.buf);
            Buffer uvm = decode_base64_Buffer(uvmap->altp->ldat.key);
            fprintf(stdout, "====== UVMAP (%d, %d) =========\n", n, uvm.vldsz/2);
            if (n==0) {  // n>0 は省略
                uWord* dat = (uWord*)uvm.buf;
                for (int i=0; i<uvm.vldsz/2; i++) fprintf(stdout, "%d ", dat[i]);
                fprintf(stdout, "\n");
            }
            free_Buffer(&uvm);
        }
        uvmap = uvmap->next;
        n++;
    }
    del_tList(&lpuvmap);

    del_xml(&lod);


    ///////////////////////////////////////////////////////////////////
    if (key!=NULL) {
        fprintf(stdout, "\n== %s BLOCK =========\n", key);
        xml= llsd_bin_get_block_data(buf, sz, key);
        if (xml!=NULL) {
            fprintf(stdout, "\n");
            print_xml(stdout, xml, 2);
        }
    }
    del_xml(&lod);

    free(buf);
    return 0;
}

