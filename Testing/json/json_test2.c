/*  
    JSON TEST Program 
*/


#include "tjson.h"



int main()
{

    //char pp1[] = "{\"@@@\"}{\"sjkj\", \"aaaa\":xxxx, \"bbbb\": { \"cccc\" , \"sss\":dhdhash, \"ABC\"  :  {  \"XYZ\" : {  } },\"JJJ\", },\"ppp\":xyz}";
    char pp1[] = "{\"@@@\"}{\"sj";
    char pp2[] = "kj\", \"aaaa\":xxxx, \"bbbb\": {";
    char pp3[] = " \"cccc\"";
    char pp4[] =" , \"sss\" ";
    char pp5[] = "  : ";
    char pp6[] = "dhdhash, ";
    char pp7[] = "\"ABC\"  : ";
    char pp8[] = " {  \"XYZ\" : {  } },\"J";
    char pp9[] = "JJ\", },\"ppp\":x";
    char pp10[] = "yz}";


    tJson* json = NULL;

    json = json_parse_seq(NULL, pp1, 2);
    json = json_parse_seq(json, pp2, 2);
    json = json_parse_seq(json, pp3, 2);
    json = json_parse_seq(json, pp4, 2);
    json = json_parse_seq(json, pp5, 2);
    json = json_parse_seq(json, pp6, 2);
    json = json_parse_seq(json, pp7, 2);
    json = json_parse_seq(json, pp8, 2);
    json = json_parse_seq(json, pp9, 2);
    json = json_parse_seq(json, pp10,2);
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



