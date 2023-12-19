
#include "xtools.h"
#include "protocol.h"

#include "mt.h"



/*
//Newton
double x2(double t)
{
    return t*t - 4.0;
}



double dx2(double t)
{
    return 2*t;
}

*/


#include "http_tool.h"


int main()
{

    Buffer buf;

    unsigned short sport, vport;
    Buffer  srvfqdn, prtcl, srvurl;

    srvurl = init_Buffer();

    buf = make_Buffer_bystr("202.26.159.200");

    decomp_url(buf, &srvurl, &prtcl, &srvfqdn, &sport, NULL);

    printf("%s\n", srvurl.buf);
fflush(stdout);
    if (prtcl.buf!=NULL) printf("%s\n", prtcl.buf);
fflush(stdout);
    printf("%s\n", srvfqdn.buf);
fflush(stdout);
    printf("%d\n", sport);
fflush(stdout);



    //printf("%s\n", get_localtime('-', 'T', ':', 'Z'));
    //printf("%s\n", get_localtime_ts('-', 'T', ':', 'Z'));
    printf("%s\n", get_local_timestamp(time(0), "%Y-%m-%dT%H:%M:%SZ"));
    printf("%s\n", get_http_header_date(time(0)));

//    Buffer buf;

/*
    buf = make_Buffer(BUFSZ);
    read_lines_Buffer(&buf, stdin);

    tList* lp = get_protocol_header_list(buf, ':', TRUE, TRUE);
    print_tList(stderr, lp);
    print_message("-------------------------------------------------\n");


    replace_protocol_header_item(lp, "via", 1, ' ', 2, "192.168.27.1", "202.26.159.1");
    print_tList(stderr, lp);
*/

/*
    Buffer ipport = comp_hostport("202.26.159.111", 999);
    print_message("ipport = %s\n", ipport.buf);

    Buffer ip;
    unsigned short port;

    decomp_hostport(ipport, &ip, &port);
    print_message("ip = %s port = %d\n", ip.buf, port);
*/

/*
    double x = 100.0;
    int n = newton_method(&x2, &dx2, &x, EPS);
    printf("newton = %d %f\n", n, x);
*/


/*
    Buffer  buf = set_Buffer("abcfef\r\nssssss\r\n\r\nzzzz\nuuuu\rZZZZZ\0\1xxxx\2\0", 41);
    //Buffer  buf = set_Buffer("abcfef\r\nsssss\0xxxx", 20);
    Buffer* ret;

    ret = decompline_Buffer_dim(buf, OFF);

    int i;
    for (i=0; i<ret->state; i++) {
        printf("%d: %s", ret[i].vldsz, ret[i].buf);
    }
    printf("\n");
    
    del_Buffer_dim(&ret);
*/
}
