
/**
llmesh データのヘッダテスト用


*/


#include "common.h"
#include "tools.h"
#include "llsd_tool.h"
#include "gz_tool.h"


int main(int argc, char** argv)
{
    DebugMode = ON;

    long   sz;
    unsigned char*  buf;
    FILE*  fp;

    if (argc<2) {
        fprintf(stderr,"Usage %s dump_file\n", argv[0]);
        exit(1);
    }

    sz = file_size(argv[1]);
    if (sz<=0) exit(1);

    buf = (unsigned char*)malloc(sz);
    fp = fopen(argv[1], "r");
    if (fp==NULL) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(1);
    }

    fread(buf, sz, 1, fp);
    fclose(fp);


    ////////////////////////////////////////////////

    int hdsz  = llsd_bin_get_length(buf, sz);
    tXML* xml = llsd_bin_parse(buf, hdsz); 
    print_xml(stderr, xml, 2);

    int ofst = -1, size = -1;

    if (llsd_xml_contain_key(xml, "high_lod")){
        ofst = llsd_xml_get_content_int(xml, "high_lod", "offset");
        size = llsd_xml_get_content_int(xml, "high_lod", "size");
    }

    printf("offset = %d\n", ofst);
    printf("size   = %d\n", size);

    del_xml(&xml);


    ////////////////////////////////////////////////
    ::free(buf);

    return 0;
}

