
#include "network.h"


int main()
{
    unsigned char* pp = get_ipaddr_byname_num("202.26.159.210", AF_INET);
    unsigned char* px = get_ipaddr_byname_num("www.google.co.jp", AF_INET6);

    unsigned char ppp[256];
    printf("%s\n", inet_ntop(AF_INET,  pp, ppp, 100));
    printf("%s\n", inet_ntop(AF_INET6, px, ppp, 100));

    free(pp);
    free(px);

    //
    pp = get_hostname("www.google.co.jp");
    printf("=> %s\n",pp);
    free(pp);

    pp = get_hostname("202.26.159.210");
    printf("=> %s\n",pp);
    free(pp);

    pp = get_hostname("2404:6800:4004:81b::2003");
    printf("=> %s\n",pp);
    free(pp);


    pp = get_ipaddr_byname("www.google.co.jp", AF_INET6);
    if (pp!=NULL) printf("6>> %s\n",pp);
    
    pp = get_ipaddr_byname("www.google.co.jp", AF_INET);
    if (pp!=NULL) printf("4>> %s\n",pp);
    
    pp = get_ipaddr_byname("202.26.159.210", AF_INET);
    if (pp!=NULL) printf("4>> %s\n",pp);

    pp = get_ipaddr_byname("sirius.nsl.tuis.ac.jp", AF_INET);
    if (pp!=NULL) printf("4>> %s\n",pp);

    //
    pp = get_myipaddr_num_ipv4();
    if (pp!=NULL) {
        for (int i=0; i<8; i++) {
            printf("%02d ==> %02x \n", i, pp[i]);
        }
    }

    printf("--------------\n");
    pp = get_myipaddr_num(AF_INET6);
    if (pp!=NULL) {
        for (int i=0; i<16; i++) {
            printf("%02d ==> %02x \n", i, pp[i]);
        }
    }

    pp = get_myipaddr_ipv4();
    printf("NAME = %s\n", pp);

    pp = get_myipaddr(AF_INET6);
    printf("NAME = %s\n", pp);

    printf("--------------\n");
    pp = get_myipaddr_num(AF_INET);
    if (pp!=NULL) {
        for (int i=0; i<8; i++) {
            printf("%02d ==> %02x \n", i, pp[i]);
        }
    }

    printf("--------------\n");
    pp = get_myipaddr_num(AF_INET6);
    if (pp!=NULL) {
        for (int i=0; i<32; i++) {
            printf("%02d ==> %02x \n", i, pp[i]);
        }
    }


    printf("--------------\n");
    pp = get_localip_bydest_ipv6("www.google.co.jp");
    if (pp!=NULL) {
        printf(" ==> %s \n", pp);
    }

    printf("--------------\n");
    pp = get_localip_ipv4();
    if (pp!=NULL) {
        printf(" ==> %s \n", pp);
    }

}
