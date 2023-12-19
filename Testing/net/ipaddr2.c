
#include "network.h"


int main()
{
    unsigned char* pp = to_address_num8_ipv4("202.26.159.0/12", 1 );
    unsigned char* px = to_address_num32_ipv6("www.google.co.jp/64", 1);

    if (pp!=NULL) {
        for (int i=0; i<8; i++) {
            print_message("%2d => %02d\n", i, pp[i]);
        }
    }
    print_message("----------------------\n");
    if (px!=NULL) {
        for (int i=0; i<32; i++) {
            print_message("%2d => %02x\n", i, px[i]);
        }
    }


    char* cc;
    cc = to_address_char8_ipv4(pp);
    print_message("=====> %s\n", cc);

    cc = to_address_char32_ipv6(px);
    print_message("=====> %s\n", cc);

    free(pp);
    free(px);

    pp = get_localip_ipv4();
    px = get_localip_ipv6();
    print_message("LOCAL =====> %s\n", pp);
    print_message("LOCAL =====> %s\n", px);

    pp = get_mynetaddr_ipv6();
    print_message("=====> %s\n", pp);

    pp = get_mynetaddr_ipv4();
    print_message("=====> %s\n", pp);

}
