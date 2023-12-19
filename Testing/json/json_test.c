/*  
    JSON TEST Program 
*/


#include "tjson.h"



int main()
{

    //char pp[] = "{\"@@@\"}{\"sjkj\", \"aaaa\":xxxx, 'bbbb': {\"cccc\" , \"sss\":dhdhash, 'ABC'  :  {  \"XYZ\" : {  } },\"JJJ\", },\"ppp\":xyz}";
    //char pp[] = "{'key':123, 'mession':['AAA', 'BBB', {'NNN','GGG':'ZZZ', 'AAA':[445, {'LLL':[iSS]}, 112]}], 'SSS':['SSS', 'ZZZ']}";
    //char pp[] = "{\"user\": \"iseki\", \"server_name\": \"\", \"target\": \"http://127.0.0.1:49112\", \"jupyterhub\": true}";
    char pp[] = "{\"/user/iseki\":{\"user\":\"iseki\",\"server_name\":\"\",\"target\":\"http://127.0.0.1:49162\",\"jupyterhub\":true,\"last_activity\":\"2022-01-17T09:23:04.774Z\"}}";
    //char pp[] = "{'key':123, 'mession':['AAA', 'BBB', {'ZZZ':'111', 'AAA':[111]}]}";
    //char pp[] = "{'key':123, 'mession':[]}";
    //char pp[] = "{\"A\":{\"B\":{\"C\":ABC, \"DEFG\"}, \"E\":X}}";

    tJson* json;
    json = json_parse_prop(NULL, "{\"zzz\":{ \"abc\":\"xxx\"}}", 2);
    //json = json_parse_prop(NULL, pp, 2);

    //tJson* xx = json_parse_prop(NULL, "{\"hub\":true,\"target\":\"http://202.26.150.55:8081\",\"jupyterhub\":true,\"last_activity\":\"2022-01-17T09:18:22.222Z\"}", 2);
    //tJson* yy = json_parse_prop(NULL, pp, 99);

    tJson* xxxx = json_parse_prop(NULL, pp, JSON_INDENT_FORMAT);


	print_tTree(stderr, json);
    print_json(stderr, json, JSON_INDENT_FORMAT);
	print_message("-------------------\n");
    print_json(stderr, xxxx, JSON_INDENT_FORMAT);
	print_message("-------------------\n");

    insert_json_nodes(json, xxxx); 
    print_json(stderr, json, JSON_INDENT_FORMAT);
	print_message("-------------------\n");

/*
    //json = json_parse(pp, 2);
    json = json_parse_prop(NULL, pp, 2);
    
    print_tTree(stdout, json);
    fprintf(stdout, "\n");
    //print_json_opt(stdout, json, "\n", "    ");


    Buffer user = get_key_json_val(json, "user", 1);
    if (user.buf!=NULL) {
        printf("%s\n", user.buf);
    }
*/  



    ////////////////////////////////////////////////////////////
/*
    fprintf(stdout, "----------------------------\n");
    Buffer buf;
    buf = json_inverse_parse_opt(json, "\n", "    ");
    fprintf(stdout, "%s", buf.buf);
    free_Buffer(&buf);

    fprintf(stdout, "----------------------------\n");
    buf = json_inverse_parse_opt(json, "", "");
    fprintf(stdout, "%s", buf.buf);
    fprintf(stdout, "\n");
    free_Buffer(&buf);
*/

    return 0;
}




