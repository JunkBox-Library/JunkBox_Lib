
#include "buffer.h"
#include "tlist.h"
#include "xtools.h"
#include "http_tool.h"

int main(void)
{
    tList* pp;
    tList* ps;
    Buffer xx;

    pp = ps = create_protocol_header(NULL, "FIRST_LINE", "POST /cgi-bin/login.cgi HTTP/1.1");
    //pp = ps = create_protocol_header(NULL, "FIRST_LINE", "HTTP/1.0 200 OK");
    pp = create_protocol_header(pp, "content-length", "122");
    pp = create_protocol_header(pp, "HEADER_END", "");

    print_tList(stderr, ps);
    print_message("---------------------\n");

    float ver = get_http_version_num(ps);

    print_message("====> %f\n", ver);

}
