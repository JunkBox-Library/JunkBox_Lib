


#include "protocol.h"
#include "smtp_tool.h"
#include "http_tool.h"


int main()
{
    tList* lp=NULL;
    char*  pp;
    Buffer buf;

/*
    buf = make_Buffer(BUFSZ);
    FILE* fp = fopen("222", "r");
    do {
        memset(buf.buf, 0, BUFSZ);
        fread(buf.buf, BUFSZ-1, 1, fp);
        buf.vldsz = strlen((char*)buf.buf);
        lp = get_protocol_header_list(lp, buf, TRUE, TRUE);
    } while (!feof(fp)); 
    fclose(fp);
    lp = find_tList_top(lp);

    buf = get_protocol_header(lp, ON);
*/


/*
    pp = get_mime_boundary(lp);
    lp = get_mime_filenameffn("2005", pp);
*/
    

    lp = get_protocol_header_list_file("sip.dat", ':', TRUE, TRUE);
    
    buf = search_protocol_header(lp, HDLIST_FIRST_LINE_KEY, 1);

    tList* lt = cawk_tList(buf.buf, ' ');


    print_tList(stdout, lt);

    return 0;
}
