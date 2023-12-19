
#include "txml.h"

/*
            tp                          tr
            A --> B --> Z               C --> M --> Y
                    --> M
              --> C --> M --> Y  (KK)       
                          --> X  (XX)     
                          --> YY (YY)
                          (MM)
                    --> C --> X
                          --> M --> X
                                --> Y
            X --> M --> Y
   
*/

int main()
{
    tTree* tt;
    tTree* tx;
    tTree* ll;

/*
    tt = xml_parse("userid=xDl7DbGxzTR219yvIKqoKjQ==&pwd=y0S0CpW0CZ0PRLU0P0L0T0V");


    print_tTree(stderr, tt, "   ");

    Buffer buf = xml_inverse_parse(tt, 0);
    print_message("== %s\n", buf.buf);
*/

    tt = xml_parse("<A><B><Z /><M>MM</M></B><C><M><Y>KK</Y><X>XX</X><YY yyy=\"888\">YY</YY>MM</M><C bbb=\"777\" xxx=\"000i\" xxxx=\"000\" ><X></X><M><X></X>YY<Y>ooooooooooooooooooo</Y></M></C></C></A><X><M><Y>MM</Y></M></X>");
    print_xml_tree(stderr, tt, "    ");
    print_message("----------------------------------------------\n\n");


    tx = xml_parse("<C><M><Y>");
    print_xml_tree(stderr, tx, "    ");
    print_message("----------------------------------------------\n\n");


    ll = get_xml_node_list(tt, tx);

    while (ll!=NULL) {
        //if (ll->altp!=NULL) print_message("%s\n", ll->altp->ldat.key.buf);
        if (ll->altp!=NULL) print_xml_node(stderr, ll->altp);
        ll = ll->next;
    }
    print_message("\n");

    return 0;
}

