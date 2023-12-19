
#include "txml.h"



/**
    TEST for  transform_cap_xml()  function in sl_realy/sl_tcp_transformation.c
*/




int main()
{
    DebugMode = ON;

    tXML* sxml = xml_parse_file("data/login_s2v.xml");
    if (sxml!=NULL) {
        print_xml(stdout, sxml, 1);

        tXML* lp = get_xml_content_bystr(sxml, "<member><name>sim_ip</name><value><string>");

        if (lp!=NULL) {
            printf("ret = %s\n", (char*)(lp->ldat.key.buf));
        }
        else {
            printf("lp is NULL\n");
        }

        del_tTree(&sxml);
    }

    return 0;
}

