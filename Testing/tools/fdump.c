
#include "common.h"
#include "tools.h"
#include "buffer.h"


int main(int argc, char** argv)
{
    long  sz;
    char* buf;
    FILE* fp;

    if (argc<2) {
        fprintf(stderr,"Usage %s dump_file \n", argv[0]);
        exit(1);
    }

    sz = file_size(argv[1]);
    if (sz<=0) exit(1);


    buf = (char*)malloc(sz);
    fp = fopen(argv[1], "r");
    if (fp==NULL) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(1);
    }

    fread(buf, sz, 1, fp);
    fclose(fp);

    fdump(stdout, (unsigned char*)buf, sz);


    printf("--------------------------------------\n");
    Buffer xxx = set_Buffer(buf, sz);
    Buffer str = dump_Buffer(xxx);
    printf("%s", str.buf);

    return 0;
}

