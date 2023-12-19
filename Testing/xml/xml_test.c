#include "txml.h"
#include "jbxl_state.h"

/*
 tt                                tx
    A --> Z                           C --> M --> Y
      --> B --> Z
            --> M
      --> C --> M
            --> C --> X
                  --> M --> X
                        --> Y
    X --> M --> Y
*/

int main()
{
    Buffer buf, chg, ret;
    tTree* tt;
    tTree* tx;
    tTree* rt;

    buf = make_Buffer_bystr("<A aaa=\"YYYY\"><Z></Z><B><Z zzz=\"99 9\"/><!sssss!><M>MM</M></B><C><M><X>XX</X><YY yyy=\"888\">YY</YY>MM</M><C bbb=\"777\" xxx=\"000i\" xxxx=\"000\" ><X></X><M><X></X>YY<Y>oooo</Y></M></C></C></A><X><M><Y>MM</Y></M></X>");
    //buf = make_Buffer_bystr("<a t=\"x\"><b>aaaa<d>bbbb</d>cccc<xxxi/>ddddd</b><c><e></e></c></a>");
    tt = xml_parse((char*)buf.buf);
    jbxl_print_state(tt->state);    
    print_xml_tree(stderr, tt, "    ");
    print_message("\n");
    print_message("N = %d %d\n", tt->state, tt->num);

    print_message("-----------------------\n");
    chg = make_Buffer_bystr("<C><M><Y>xxxx");
    tx = xml_parse((char*)chg.buf);
    jbxl_print_state(tx->state);    
    print_xml_tree(stderr, tx, "    ");
    print_message("\n");
    print_message("N = %d %d %d\n", tx->ldat.id, tx->state, tx->num);

    print_message("-----------------------\n");
    dup_merge_xml(tt->next, tx);
    jbxl_print_state(tt->state);    
    //int n = replace_all_tag_value(tt, "M", "MM", "VV");

    //ret = get_value_xml(tt, tx);
    //ret = get_attr_xml(tt, tx);
    //ret = get_value_tTree(tt, tx);
    //set_attr_xml_bystr(tt, tx, "XXXXXXXXXXXX");
    //set_xml_tag(tt, tx);

    //print_message(" --> ans = %s\n", ret.buf);

    print_xml_tree(stderr, tt, "    ");
    print_message("N = %d %d\n", tt->state, tt->num);
    //print_message("N = %d\n", n);

    print_message("-----------------------\n");
    ret = xml_inverse_parse(tt, XML_INDENT_FORMAT);
    print_message("%s\n", buf.buf);
    print_message("\n");
    print_message("%s\n", ret.buf);

    print_message("-----------------------\n");
    print_xml(stdout, tt, 2);
    return 0;
}

