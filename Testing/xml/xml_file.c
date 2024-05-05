
#include "txml.h"




/**

    TEST for  transform_cap_xml()  function in sl_realy/sl_tcp_transformation.c

*/


int main()
{
    DebugMode = ON;

    tXML*  sxml = xml_parse_file("joint_template.xml");

    tXML*  rxml = get_xml_attr_node(sxml, "name", "\"mEyeRight\"");
    tXML*  lxml = get_xml_attr_node(sxml, "name", "\"mEyeLeft\"");

print_message("1111111111111\n");
set_xml_content_node(rxml->next, "1111111111111");
print_message("2222222222222\n");
set_xml_content_node(lxml->next, "1111111111111");
print_message("3333333333333\n");

print_xml_tree(stdout, sxml, "  ");
print_xml(stdout, sxml, 2);

    return 0;
}

