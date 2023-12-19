
#include "xtools.h"
#include "protocol.h"

int main(int argc, char** argv)
{
    char    SIPuri[LBUF];
    Buffer  buf, snd, hx;
    Buffer* hd;
    tList*  lp;
    tList*  lh;

    if (argc<2) exit(1);
    snd = init_Buffer();

    lp = get_protocol_header_list_file(argv[1], ':', TRUE, TRUE);
    buf = restore_protocol_header(lp, ": ", ON, NULL);
    print_message("\n--- Readed File -------------------------\n");
    print_message("%s\n", buf.buf);
    print_message("-----------------------------------------\n");

    lh = strncmp_tList(lp, HDLIST_FIRST_LINE_KEY, 0, 1);
    if (lh==NULL) {
        del_all_tList(&lp);
        return 1;
    }
    hd = cawk_Buffer_dim(lh->ldat.val, ' ');
    if (hd==NULL || hd->state<2) {
        del_all_tList(&lp);
        return 1;
    }

    print_message("Method Header Decompose\n");
    print_message("size = %d\n", hd->state);
    print_message("1: %s\n", hd[0].buf);
    print_message("2: %s\n", hd[1].buf);
    print_message("3: %s\n\n", hd[2].buf);
    print_message("-----------------------------------------\n");


    memset(SIPuri, 0, LBUF);
    snprintf(SIPuri, LBUF, "sip:%s:%d", "VIVOX.com", 5060);
    print_message("URI = %s\n", SIPuri);

    if (!strcmp((char*)hd[0].buf, "REGISTER")) {
        if (hd->state>=3) {
            free_Buffer(&hd[1]);
            hd[1] = make_Buffer_bystr(SIPuri);
            hx = join_Buffer_dim(hd, " ");
            print_message("Method = %s\n", hx.buf);
            free_Buffer(&lh->ldat.val);
            lh->ldat.val = hx;
        }
    }                      
    else if (!strcmp((char*)hd[1].buf, "INVITE")) {
    
    }                                        

    del_Buffer_dim(&hd);
    snd = restore_protocol_header(lp, ": ", ON, NULL);
    del_all_tList(&lp);

    print_message("\n--- to Server ---------------------------\n");
    print_message("%s\n", snd.buf);

    return 0;
}

