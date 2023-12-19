
#include "txml.h"

/*
            tp                          tr
            A --> B --> M               C --> M --> Y
              --> C --> M --> X        
                          --> Y       
                    --> C --> X
                          --> M --> X
                                --> Y
   
*/

int main()
{
    tTree* tt;
    tTree* tx;
    tTree* rt;



    tt = xml_parse("<A>a</A><B>b</B><A>a</A><B>b</B><A>a</A>");
    print_xml(stderr, tt, 2);
    print_message("-------------------------------------\n\n");


    tx = xml_parse("<A>a</A><B>");
    print_xml(stderr, tx, 2);
    print_message("-------------------------------------\n\n");
    close_xml(tx);


    if (tt->ldat.id==0) tt = tt->next;
    if (tx->ldat.id==0) tx = tx->next;
    rt = cmp_sisters_xml(tt, tx);


    print_message("-------------------------------------\n\n");
    int n = 1;


print_tTree(stdout, tx);

    print_message("%s\n", tx->ldat.key.buf);
    print_message("%s\n", tx->next->ldat.key.buf);
//    print_message("%s\n", tx->ysis->ldat.key.buf);

//    print_message("%s\n", tx->altp->ldat.key.buf);
//  print_message("%s\n", tx->next->altp->ldat.key.buf);
//    print_message("%s\n", tx->ysis->altp->ldat.key.buf);


/*
    while (tx!=NULL) {
        print_message("%d: ", n++);
        if (tx->altp!=NULL) print_message("%s", tx->altp->ldat.key.buf);
        print_message("\n");
        tx = tx->next;
    }
*/





    return 0;
}

