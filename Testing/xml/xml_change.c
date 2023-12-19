
#include "txml.h"
#include "jbxl_state.h"



int main()
{
    Buffer buf, chg;
    tTree* tt;
    tTree* tx;


    buf = make_Buffer(BUFSZ);
    read_lines_Buffer(&buf, stdin);

    tt = xml_parse((char*)buf.buf);
    print_xml_tree(stderr, tt, "  ");
    print_message("-----------------------------\n");
    if (tt->state!=JBXL_XML_PARSED) {
        jbxl_print_state(tt->state);
    }

    //chg = make_Buffer_bystr("<member><name>version</name><value><string xxx=\"SSS\">Second Life Release 2.0.0.2");
    chg = make_Buffer_bystr("<xxx><yyy><zzz>");

    tx = xml_parse((char*)chg.buf);
    print_xml_tree(stderr, tx, "  ");
    print_message("-----------------------------\n");
    if (tx->state!=JBXL_XML_PARSED) {
        jbxl_print_state(tt->state);
    }

/*
    tList* lp = get_xml_node_list(tt, tx);
    while (lp!=NULL) {
        print_message("--> %s\n", lp->altp->ldat.key.buf);
        lp = lp->next;
    }
*/


/*
    int ret = replace_xml(tt, tx);
    print_xml_tree(tt, "  ");
*/


    int n = set_xml_content_list(tt, tx, "HUHUHUHU");
    print_message("xxxxxxxxxxxxxx %d\n", n);
    print_xml_tree(stderr, tt, "  ");

    return 0;
}

