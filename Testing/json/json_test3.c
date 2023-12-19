/*  
    JSON TEST Program 
*/


#include "tjson.h"



int main()
{

    char pp1[] = "{\"@@@\"}{\"sjkj\", \"aa";
    char pp2[] = "aa\":xxxx, \"bbbb\"";
    char pp3[] = " : {\"cccc\" , \"sss\":dhdhash, \"ABC\"  :  { \"";
    char pp4[] = "XYZ\" : {  } },\"JJJ\", }";
    char pp5[] = ",\"ppp\":xyz}";

    tJson* json = NULL;
    json = json_parse_seq(NULL, pp1, 2);
    json = json_parse_seq(json, pp2, 2);
    json = json_parse_seq(json, pp3, 2);
    json = json_parse_seq(json, pp4, 2);
    json = json_parse_seq(json, pp5, 2);
    
    print_tTree(stdout, json);
    //
    jbxl_fprint_state(stdout, json->state);

    Buffer buf;
    fprintf(stdout, "%s", "----------------------------\n");
    buf = json_inverse_parse_opt(json, "", "");
    fprintf(stdout, "%s", buf.buf);
    fprintf(stdout, "\n");
    free_Buffer(&buf);
}




