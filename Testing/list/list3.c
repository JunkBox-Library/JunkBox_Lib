
#include "tlist.h"

int main(void)
{
    tList* tx;
    tList* tt;
    tList* ta;
    tList* t2;

    tt = tx = add_tList_node_bystr(NULL, 1, 2, "AAA", "BBB", NULL, 0);
    tt = t2 = add_tList_node_bystr(tt, 2, 4, "CCC", "RDD", NULL, 0);
    tt = add_tList_node_bystr(tt, 4, 6, "AAA", "MMM", NULL, 0);

    ta = add_tList_node_bystr(NULL, 3, 5, "XXX", "YYY", NULL, 0);

    print_tList(stderr, tx);
    print_message("---------------------\n");

    // ta を t2 の後ろに追加

    t2->next->prev = ta;
    ta->prev = t2;
    ta->next = t2->next;
    t2->next = ta;


    print_tList(stderr, tx);


    print_message("---------------------\n");
    del_tList_key(&tx, "AAA", 0);
    print_tList(stderr, tx);

    del_tList(&tx);
}
