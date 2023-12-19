
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

    
    replace_sip_contact(lp, "AAA.BBB.CCC.DDD", 999);
    replace_sip_contact_dstipport(lp, "EEE.FFF.GGG.HHH", 111);


//int       replace_invite_sdp(tList*lp, tList* ls, char* ipaddr, unsigned short port);

    snd = restore_protocol_header(lp, ": ", ON, NULL);
    print_message("----------------------------------------\n");
    print_message("%s\n", snd.buf);

    return 0;
}


