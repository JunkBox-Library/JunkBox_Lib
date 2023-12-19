/*  
    JSON TEST Program 
*/


#include "tjson.h"



int main()
{
    char pp[] = "\"color\":[ \"red\", \"green\", \"blue\" ]";

    tJson* json;

    json = json_array_parse(NULL, pp, 2);
    
    print_tTree(stdout, json);
    fprintf(stdout, "\n");
    print_json_opt(stdout, json, "\r\n", "    ");

    fprintf(stdout, "----------------------------\n");
    Buffer buf;
    buf = json_inverse_parse_opt(json, "\r\n", "    ");
    if (buf.buf!=NULL) {
        fprintf(stdout, "%s", buf.buf);
        free_Buffer(&buf);
    }

    fprintf(stdout, "----------------------------\n");
    buf = json_inverse_parse_opt(json, "", "");
    if (buf.buf!=NULL) {
        fprintf(stdout, "%s", buf.buf);
        fprintf(stdout, "\n");
        free_Buffer(&buf);
    }

    return 0;
}




