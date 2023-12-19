#include "txml.h"


/**

    XMLパース&表示プログラム

*/

int main(int argc, char** argv)
{
    int  i;
    Buffer buf;
    tTree* tt;

    int  mode = 2, dflg = OFF;
    
    for (i=1; i<argc; i++) {
        if    (!strcmp(argv[i],"-0"))  mode = 0;
        else if (!strcmp(argv[i],"-1"))  mode = 1;
        else if (!strcmp(argv[i],"-2"))  mode = 2;
        else if (!strcmp(argv[i],"-d"))  dflg = ON;
        else if (!strcmp(argv[i],"-h"))  {fprintf(stdout, "Usage ... %s [-0] [-1] [-2] [-d] [-h]\n", argv[0]); exit(0);}
    }

    if (dflg) print_message("reading data from stdin.....\n");
    buf = make_Buffer(BUFSZ);
    read_lines_Buffer(&buf, stdin);

    if (dflg) print_message("parsing read data.....\n");
    tt = xml_parse((char*)buf.buf);
    free_Buffer(&buf);

    print_xml(stdout, tt, mode);
    
    del_xml(&tt);

    return 0;
}
