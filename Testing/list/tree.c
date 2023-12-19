

#include "ttree.h"



int main(void)
{
    tTree* pp;
    tTree* xx;
    tTree* yy;
    tTree* cc;
    tTree* tt;

    pp = add_tTree_node_bystr(NULL, 1, 0, "AA", "aa", NULL, 0);

    xx = add_tTree_node_bystr(pp, 2, 0, "BB", "bb", NULL, 0);
    cc = add_tTree_node_bystr(pp, 3, 0, "CC", "cc", NULL, 0);

    add_tTree_node_bystr(xx, 4, 0, "XX", "xx", NULL, 0);
    add_tTree_node_bystr(xx, 5, 0, "ZZ", "zz", NULL, 0);
    yy = add_tTree_node_bystr(xx, 6, 0, "YY", "yy", NULL, 0);
    add_tTree_node_bystr(yy, 7, 0, "PP", "pp", NULL, 0);

    add_tTree_node_bystr(cc, 8, 0, "FF", "ff", NULL, 0);
    add_tTree_node_bystr(cc, 9, 0, "QQ", "qq", NULL, 0);

    print_tTree(stderr, pp);
    fprintf(stderr, "------------------------\n");

    fprintf(stderr, "num = %d\n", pp->num);
    fprintf(stderr, "------------------------\n");

    tt = strncmp_tTree(pp, "PP", 0, 1);
    if (tt!=NULL) printf("%s\n", tt->ldat.key.buf);



}

