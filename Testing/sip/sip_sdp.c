
#include "sip_tool.h"



int main(int argc, char** argv)
{
    Buffer  buf, cnt, sdp, snd;
    tList*  lp;
    tList*  ls;

    if (argc<2) exit(1);
    snd = init_Buffer();

    lp = get_protocol_header_list_file(argv[1], ':', TRUE, TRUE);

    buf = restore_protocol_header(lp, ": ", ON, NULL);
    print_message("\n--- Readed File -------------------------\n");
    print_message("%s", buf.buf);

    
    ls  = get_sdp_body_list(lp);
    replace_sdp_invite_addr(lp, ls, "AAA.BBB.CCC.DDD", 999, TRUE);

    sdp = restore_sdp_body(ls);
    set_sip_contents(lp, sdp);

    snd = restore_sip_header(lp);
    print_message("----------------------------------------\n");
    print_message("%s", snd.buf);

    print_message("========================================\n");
    lp = get_protocol_header_list_file(argv[1], ':', TRUE, TRUE);
    sdp = restore_sip_contents(lp);
print_message("%s", sdp.buf);
    set_sip_contents(lp, sdp);
    snd = restore_sip_header(lp);
    print_message("----------------------------------------\n");
    print_message("%s", snd.buf);

    return 0;
}
