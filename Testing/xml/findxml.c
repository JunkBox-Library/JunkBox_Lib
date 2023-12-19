
#include "txml.h"
#include "jbxl_state.h"


/**



*/

int main(int argc, char** argv)
{
    Buffer key, buf;
    tTree* tt;
    tTree* tx;

    if (argc<2) {
        print_message("Usage.... cat xmlfile | %s keyword\n", argv[0]);
        exit(1);
    }


    buf = make_Buffer(BUFSZ);
    read_lines_Buffer(&buf, stdin);

    tt = xml_parse((char*)buf.buf);
/*
    if (tt->state!=JBXL_XML_PARSED) {
        jbxl_print_state(tt->state);
    }
*/

    key = make_Buffer_bystr(argv[1]);
    tx = xml_parse((char*)key.buf);
    if (tx->state!=JBXL_XML_PARSED) {
        jbxl_print_state(tx->state);
    }


    tList* lp = get_xml_node(tt, tx);
    if (lp!=NULL) print_xml(stdout, lp, 3);

print_message("\n");

    
print_message("SSSSSSSSSS => %s \n", (char*)key.buf);
    lp = get_xml_content_list(tt, tx);
print_message("SSSSSSSSSS => %s \n", (char*)key.buf);
    while (lp!=NULL) {
        print_message("--> %s\n", lp->altp->ldat.key.buf);
        lp = lp->next;
    }

    del_xml(&tt);
    del_xml(&tx);
    del_tList(&lp);

    free_Buffer(&buf);
    free_Buffer(&key);
    return 0;
}


