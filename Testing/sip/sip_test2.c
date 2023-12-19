
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

    Buffer x = get_sip_via_address(lp, 5);
    print_message("XXX = > %s\n", x.buf);

    del_sip_via(lp, "202.202.202.202", 9999);
    
    replace_sip_contact(lp, "XXX.YYY.ZZZ.XXXX", 999);
/*
Buffer  replace_sip_via(tList* lp, char* ipaddr, unsigned short port);
int     insert_sip_via(tList* lp, char* ipaddr, unsigned short port, char* branch);
void    del_sip_via(tList* lp, char* ipaddr, unsigned short port);

void    del_sip_record_route(tList* lp);

int     replace_sip_contact(tList* lp, char* ipaddr, unsigned short port);
int     replace_sip_contact_dstipport(tList* lp, char* ipaddr, unsigned short port);


int     replace_invite_sdp(tList*lp, tList* ls, char* ipaddr, unsigned short port);

*/

    snd = restore_protocol_header(lp, ": ", ON, NULL);
    print_message("----------------------------------------\n");
    print_message("%s\n", snd.buf);

    return 0;
}


