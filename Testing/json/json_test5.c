/*  
    JSON TEST Program 
*/


#include "tjson.h"



int main()
{

    char pp[] = "{\"sjkj\", \"aaaa\":[2,3,4,[[333,444], 222],666], \"bbbb\": {\"cccc\" , \"sss\":dhdhash, \"ABC\"  :  {  \"XYZ\" : {  } },\"JJJ\", },\"ppp\":xyz}";

    tJson* json;

    json = json_parse(pp, 2);
    
    print_tTree(stdout, json);
    fprintf(stdout, "\n");
    print_json_opt(stdout, json, "\r\n", "    ");

    fprintf(stdout, "----------------------------\n");
    Buffer buf;
    buf = json_inverse_parse_opt(json, "\r\n", "    ");
    fprintf(stdout, "%s", buf.buf);
    free_Buffer(&buf);

    fprintf(stdout, "----------------------------\n");
    buf = json_inverse_parse_opt(json, "", "");
    fprintf(stdout, "%s", buf.buf);
    fprintf(stdout, "\n");
    free_Buffer(&buf);


    return 0;
}




