/**
llmesh データのヘッダテスト用

*/

#include "common.h"
#include "tools.h"
#include "llsd_tool.h"
#include "gz_tool.h"
#include "xLib/llsd_tool.h"


int main(int argc, char** argv)
{
    DebugMode = ON;

    long   sz;
    unsigned char*  buf;
    char*  key = NULL;
    FILE*  fp;

    if (argc<2) {
        fprintf(stderr,"Usage %s dump_file key\n", argv[0]);
        exit(1);
    }
    if (argc==3) key = argv[2];

    sz = file_size(argv[1]);
    if (sz<=0) exit(1);

    buf = (unsigned char*)malloc(sz);
    fp = fopen(argv[1], "r");
    if (fp==NULL) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(1);
    }
    memset(buf, 0, sz);

    fread(buf, sz, 1, fp);
    fclose(fp);

    ////////////////////////////////////////////////
    int hdsz  = llsd_bin_get_length(buf, sz);
    tXML* xml = llsd_bin_parse(buf, hdsz); 
    if (xml!=NULL) {
        //print_xml(stdout, xml, 2);
        del_xml(&xml);
    }

    if (key!=NULL) {
        xml= llsd_bin_get_block_data(buf, sz, key);
        if (xml!=NULL) {
            print_xml(stdout, xml, 2);
            del_xml(&xml);
        }
    }
    ////////////////////////////////////////////////

    ::free(buf);
    return 0;
}

