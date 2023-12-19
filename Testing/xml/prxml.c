#include "txml.h"
#include "jbxl_state.h"

/**

    XMLパース&表示プログラム

*/

int main(int argc, char** argv)
{
    int    i;
    Buffer buf;
    tTree* tt;
    time_t t1, t2;

    int  mode = 2, dflg = OFF;
    
    for (i=1; i<argc; i++) {
        if      (!strcmp(argv[i],"-0"))  mode = 0;
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

    /*
    if (dflg) {
        print_xml(stdout, tt, "    ");
        printf("\n---------------------------------------------------------------------------------------\n\n");
    }
    */
    
    if (dflg) print_message("inverse parsing xml data.....\n");
    t1 = time(NULL);
    buf = xml_inverse_parse(tt, mode);
    t2 = time(NULL);

    if (dflg) print_message("printing xml data.....\n");
    //if (buf.buf!=NULL) fprintf(stdout, "%s", buf.buf);
    if (mode==0) fprintf(stdout, "\n");

    if (tt->state!=JBXL_XML_PARSED) {
        jbxl_print_state(tt->state);
    }

    if (dflg) print_message("inverse parsing time is %ds, num = %d\n", t2-t1, tt->num);
    free_Buffer(&buf);
    fflush(stdout);


    print_tTree(stdout, tt);
    jbxl_print_state(tt->state);

    del_xml(&tt);

    return 0;
}
