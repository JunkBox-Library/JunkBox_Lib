#include "txml.h"
#include "jbxl_state.h"

/*
    tp                          tx
    A --> B --> Z               C --> M --> Y
            --> M
      --> C --> M --> X        
                  --> YY       
            --> C --> X
                  --> M --> X
                        --> Y
    X --> M --> Y
*/

int main()
{
    tTree* tt;
    tTree* tx;
    tTree* rt;

    tt = xml_parse("<A aaa=\"YYYY\"><B><Z zzz=\"99 9\"/><M>MM</M></B><C><M><X>XX</X><YY yyy=\"888\">YY</YY>MM</M><C bbb=\"777\" xxx=\"000i\" xxxx=\"000\" ><X></X><M><X></X>YY<Y>ooooooooooooooooooo</Y></M></C></C></A><X><M><Y>MM</Y></M></X>");
    jbxl_print_state(tt->state);
    print_xml_tree(stderr, tt, "      ");
    print_message("-------------------------------------\n\n");


    tx = xml_parse("<C><M><Y>");
    jbxl_print_state(tx->state);
    print_xml_tree(stderr, tx, "      ");
    print_message("-------------------------------------\n\n");


    rt = get_xml_node(tt, tx);
    jbxl_print_state(rt->state);
    print_xml_tree(stderr, rt, "      ");
    print_message("-------------------------------------\n");

    print_xml_node(stderr, rt);
    print_message("\n");

    return 0;
}

