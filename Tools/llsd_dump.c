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
        fprintf(stderr,"Usage %s llmesh_file key\n", argv[0]);
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
        fprintf(stdout, "\n");
        del_xml(&xml);
    }
    if (key!=NULL) {
        xml= llsd_bin_get_blockdata(buf, sz, key);
        if (xml!=NULL) {
            print_xml(stdout, xml, 2);

    tList* lpindex = get_xml_content_list_bystr(xml, "<map><key>TriangleList</key><binary>");
    tList* lppostn = get_xml_content_list_bystr(xml, "<map><key>Position</key><binary>");
Buffer idx = decode_base64_Buffer(lpindex->altp->ldat.key);
Buffer vrt = decode_base64_Buffer(lppostn->altp->ldat.key);

printf("===============================\n");
printf("index num = %d, pos num = %d\n", idx.vldsz, vrt.vldsz);




            //
            tList* lpweght = get_xml_content_list_bystr(xml, "<map><key>Weights</key><binary>");
            if (lpweght->altp!=NULL) {
                Buffer dec = decode_base64_Buffer(lpweght->altp->ldat.key);

printf("%s\n %d, %d\n", lpweght->altp->ldat.key.buf, lpweght->altp->ldat.key.vldsz, dec.vldsz);
                int invrtx = 0;
                int vertex = 0;
                int ptr    = 0;

                uByte* pweight = (uByte*)dec.buf;
                while (ptr < dec.vldsz) {
                    uByte joint = *(pweight + ptr);
printf("%3d  %02x ", vertex, joint);
                    ptr++;

                    if (joint==0xff) {
                        invrtx = 0;
                        vertex++;
printf("\n");
                    }
                    else {
                        invrtx++;
                        uWord weight = *(uWord*)(pweight + ptr);
                        printf("%04x, ", weight);                     
                        ptr += 2;

                        if (invrtx%4==0) {
                            invrtx = 0;
                            vertex++;
printf("\n");
                        }
                    }
                }



/*
                ptr = (uByte*)dec.buf;
                fprintf(stdout, "===========================\n");
                for (int i=0; i<dec.vldsz; i++) {
                    fprintf(stdout, "%3d: 0x%02x\n", i, ptr[i]);
                }
*/
            }

            del_xml(&xml);
        }
    }
    ////////////////////////////////////////////////

    free(buf);
    return 0;
}

