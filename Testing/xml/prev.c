
#include "txml.h"
#include "jbxl_state.h"


/**



*/

int main(int argc, char** argv)
{
    Buffer key, buf;
    tTree* tt;
    tTree* tx;


    buf = make_Buffer(BUFSZ);
    read_lines_Buffer(&buf, stdin);
    tt = xml_parse((char*)buf.buf);

    key = make_Buffer_bystr(argv[1]);
    tx = xml_parse((char*)key.buf);

    //tx = xml_parse("<k>X</k><z>");
    //tx = xml_parse("<k>X</k><k>Z</k><b>");
    //tx = xml_parse("<k>Y</k><m><k>Z</k><b>");
    //tx = xml_parse("<key>SimulatorInfo</key><array><map><key>IP</key><binary>");

    print_xml(stdout, tt, 2);
    print_message("-------------------------------\n\n");
    print_xml(stdout, tx, 2);
    print_message("-------------------------------\n\n");

    //tList* lst = get_xml_node_list_bystr(tt, "<k>Y</k><m><k>Z</k><b>");
    //tList* lst = get_xml_node_list_bystr(tt, "<k>X</k><k>Z</k><b>");
    tList* lst = get_xml_node_list(tt, tx);

    if (lst==NULL) print_message("lst is NULL\n");
    if (lst!=NULL && lst->ldat.key.buf!=NULL) printf("GET END %s\n", lst->ldat.key.buf);
    fflush(stdout);

    tList* lp = lst;
    while (lp!=NULL && lp->altp!=NULL) {
        printf("%s\n", lp->altp->ldat.key.buf);
        lp = lp->next;
    }

    fflush(stdout);

    jbxl_print_state(tt->state);

    del_xml(&tt);
    del_tList(&lst);

    return 0;
}


