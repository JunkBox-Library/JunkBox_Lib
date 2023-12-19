/*  
    JSON TEST Program 
*/


#include "tjson.h"



int main()
{
    tJson* json;
    json = json_parse_file("xxx", 2);
    
	if (json!=NULL) {
    	print_tTree(stderr, json);
    	print_message("%s", "=============================================\n");

    	Buffer buf;
    	buf = json_inverse_parse_opt(json, "\r\n", "    ");
    	fprintf(stdout, "%s", buf.buf);
    	free_Buffer(&buf);
	}

    return 0;
}




