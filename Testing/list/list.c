
#include "tlist.h"
#include "jbxl_state.h"

int main(void)
{
    tList* tx;
    tList* tt = NULL;

    tt = tx = add_tList_node_bystr(NULL, 1, 2, "AAA", "BBB", NULL, 0);
    tt = add_tList_node_bystr(tt,   2, 4, "CCC", "RDD", NULL, 0);
    tt = add_tList_node_bystr(tt,   3, 5, "XXX", "YYY", NULL, 0);
    tt = add_tList_node_bystr(tt,   4, 6, "ZZZ", "MMM", NULL, 0);

    print_tList(stderr, tx);
    print_message("---------------------\n");

    tt = dup_tList(tx);
    print_tList(stderr, tt);

    del_tList(&tx);
    del_tList(&tt);


    fprintf(stderr, "\n");
    jbxl_print_state(JBXL_MALLOC_ERROR);
    jbxl_print_state(JBXL_XML_ERROR);
    jbxl_print_state(JBXL_NET_RECV_ERROR);
    jbxl_print_state(JBXL_GRAPH_ERROR);
    fprintf(stderr, "\n");
}
