
#include "common.h"
#include "tools.h"


int main(int argc, char** argv)
{
    long   sz, offset=0;
    char*  buf;
    FILE*  fp;
    size_t rs;
    UNUSED(rs);

    if (argc<2) {
        fprintf(stderr,"Usage %s dump_file [offset]\n", argv[0]);
        exit(1);
    }

    if (argc==3) {
        offset = (atol)(argv[2]);
    } 

    sz = file_size(argv[1]);
    if (sz<=0) exit(1);


    buf = (char*)malloc(sz);
    fp = fopen(argv[1], "r");
    if (fp==NULL) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(1);
    }

    rs = fread(buf, sz, 1, fp);
    fclose(fp);

    fdump(stdout, (unsigned char*)(buf+offset), sz);

    return 0;
}

