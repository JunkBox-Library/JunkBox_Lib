#include "txml.h"

int main()
{
/*
    tXML* xml   = init_xml_doc();
    tXML* top   = add_xml_node(xml, "COLLADA");
    tXML* asset = add_xml_node(top, "asset");
    tXML* libgm = add_xml_node(top, "library_geometries");
    tXML* libvs = add_xml_node(top, "library_visual_scenes");
    tXML* vssn  = add_xml_node(libvs, "visual_scene");

    add_xml_attr(top, "xmlns=\"http://www.collada.org/2005/11/COLLADASchema\"");
    add_xml_attr(top, "version=\"1.4.1\"");
*/


    tXML* xml = NULL;
    tXML* array = NULL;

    xml = xml_rpc_add_member(NULL, "host", "HOST", NULL);
    xml = xml_rpc_add_member(xml, "sesssion", NULL, "");
    xml = xml_rpc_add_member(xml, "cell_id", "abc162aa890", "string");
    xml = xml_rpc_end_member(xml);
    array = xml_rpc_add_array(NULL, xml);

    xml = xml_rpc_add_member(NULL, "host", "HOST", NULL);
    xml = xml_rpc_add_member(xml, "sesssion", "123", "");
    xml = xml_rpc_add_member(xml, "cell_id", "abc162aa890", "string");
    xml = xml_rpc_end_member(xml);
    array = xml_rpc_add_array(array, xml);

    xml = xml_rpc_add_member(NULL, "host", "HOST", NULL);
    xml = xml_rpc_add_member(xml, "sesssion", "123", "");
    xml = xml_rpc_add_member(xml, "cell_id", "abc162aa890", "string");
    xml = xml_rpc_end_member(xml);
    array = xml_rpc_add_array(array, xml);

    array = xml_rpc_end_array(array);
    xml = array;

    Buffer buf = xml_rpc_request_pack("mod_mdlds_write_nbdata", xml);


    //
    print_xml_tree(stdout, xml, "    ");
    printf("\n");
    print_xml(stdout, xml, 2);

    print_message("===============================\n%s\n", buf.buf);

    del_xml(&xml);
    return 0;
}

