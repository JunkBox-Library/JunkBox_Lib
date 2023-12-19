

#include "tlist.h"



int main(void)
{
    tList* pp;
    tList* xx;

    pp = add_tList_node_bystr(NULL, 1, 0, "AA", "aa", NULL, 0);
    xx = add_tList_node_bystr(pp,   2, 0, "BB", "bb", NULL, 0);

    add_tList_node_bystr(pp, 3, 0, "CC", "cc", NULL, 0);
    add_tList_node_bystr(xx, 4 ,0, "XX", "xx", NULL, 0);
    add_tList_node_bystr(pp, 5, 0, "ZZ", "zz", NULL, 0);
    add_tList_node_bystr(xx, 6, 0, "YY", "yy", NULL, 0);

    print_tList(stdout, pp);
    printf("-----------------------\n");

    print_tList(stdout, xx);
    printf("-----------------------\n");
    del_tList(&xx);
    print_tList(stdout, xx);
}

