
#include "sip_tool.h"



int main(int argc, char** argv)
{
    Buffer  buf, snd;
    tList*  lp;

    if (argc<2) exit(1);
    snd = init_Buffer();

    lp = get_protocol_header_list_file(argv[1], ':', TRUE, TRUE);
    buf = restore_protocol_header(lp, ": ", ON, NULL);
    print_message("\n--- Readed File -------------------------\n");
    print_message("%s\n", buf.buf);

    
    Buffer xxx = replace_sip_via(lp, "AAA.BBB.CCC.DDD", 100, 1);
/*
    if (xxx.buf!=NULL) {
        print_message("-----------------------------------------\n");
        print_message("IPPORT = %s\n", xxx.buf);
        print_tList(stderr, lp);
    }
*/
    
    insert_sip_via(lp, "202.202.202.202", 9999, "hehehehe", TRUE);
    insert_sip_via(lp, "202.202.202.202", 9999, "hahahahaha", TRUE);
    insert_sip_via(lp, "202.202.202.111", 10, NULL, TRUE);


    snd = restore_protocol_header(lp, ": ", ON, NULL);
    print_message("----------------------------------------\n");
    print_message("%s\n", snd.buf);

    del_sip_via(lp, "202.202.202.202", 9999);
    del_sip_record_route_all(lp);
    
    snd = restore_protocol_header(lp, ": ", ON, NULL);
    print_message("----------------------------------------\n");
    print_message("%s\n", snd.buf);

    return 0;
}


