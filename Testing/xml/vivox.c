
#include "txml.h"

/**

    TEST for  transform_cap_xml()  function in sl_realy/sl_tcp_transformation.c

*/


int  UseWhiteFilter  = OFF;
int  UseServerSSL = ON;
pid_t CrntPID = 0;


int main()
{
    Buffer buf;
    tXML*  tt;
    struct sockaddr_in ut_addr;

    DebugMode = ON;

    buf = make_Buffer(BUFSZ);
    read_lines_Buffer(&buf, stdin);
    tt = xml_parse((char*)buf.buf);
    free_Buffer(&buf);


    /////////////////////////////////////////////////////////////////////////////////////////
    transform_cap_xml(tt, NULL, "202.26.159.100", 0, ut_addr, 0, ut_addr, NULL);
    /////////////////////////////////////////////////////////////////////////////////////////

    tXML* tx = xml_parse("<VCConfiguration><DefaultSIPProxy>202.26.159.140:5060");
    //set_xml_tag(tt, tx);


    int n = set_xml_next_tag_list(tt, tx, "202.26.159.140:5060", NULL);

    printf("n = %d\n", n);

/*
    while (pl!=NULL) {
        printf("%s\n", pl->altp->ldat.key.buf);
        free_Buffer(&(pl->altp->ldat.key));
        pl->altp->ldat.key = make_Buffer_bystr("202.26.159.140:5060");
        pl = pl->next;
    }
*/

    //replace_all_tag_string(tt, "proxy", "202.26.159.140:5060xxxxxxxxxxxxxx", -1);




    buf = xml_inverse_parse(tt, 2);
    if (buf.buf!=NULL) fprintf(stdout, "%s", buf.buf);

    if (tt->state!=XML_PARSED) {
        if      (tt->state==XML_PARSE_ERROR)  fprintf(stdout, "XML Parse Error!! (%d)\n", tt->state);
        else if (tt->state==XML_MEMORY_ERROR) fprintf(stdout, "XML Memory Error!! (%d)\n", tt->state);
        else                                  fprintf(stdout, "XML any Error!! (%d)\n", tt->state);
    }


    del_tTree(&tt);

    return 0;
}

