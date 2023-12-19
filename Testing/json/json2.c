/*  
    JSON TEST Program 
*/


#include "tjson.h"


void  check_json(tJson* json)
{
    tJson* stts = NULL;
    tJson* cell = NULL;
    tJson* user = NULL;
    tJson* date = NULL;
    tJson* mssn = NULL;
    tJson* pssn = NULL;
    tJson* tags = NULL;
    tJson* mgid = NULL;

    stts = find_double_key_json(json, "metadata", "status");
    if (stts==NULL) {
        cell = find_double_key_json(json, "metadata", "cellId");
    }

    // S->C
    if (stts!=NULL) {
        user = find_double_key_json(json, "header", "username");
        if (user!=NULL) {
            date = find_key_sister_json(user, "date");
            mssn = find_key_sister_json(user, "session");
            pssn = find_double_key_json(json, "parent_header", "session");
            mgid = find_key_sister_json(pssn, "msg_id");
        }
    }

    // C->S
    if (cell!=NULL) {
        if (cell!=NULL) {
            tags = find_key_sister_json(cell, "tags");
            date = find_double_key_json(json, "header", "date");
            pssn = find_key_sister_json(date, "session");
            mgid = find_key_sister_json(date, "msg_id");
        }
    }

    if (mssn!=NULL) print_message("SESSION   = %s\n", mssn->ldat.val.buf);
    if (pssn!=NULL) print_message("PSESSION  = %s\n", pssn->ldat.val.buf);
    if (mgid!=NULL) print_message("MESSAGEID = %s\n", mgid->ldat.val.buf);
    if (user!=NULL) print_message("USERNAMNE = %s\n", user->ldat.val.buf);
    if (cell!=NULL) print_message("CELLID    = %s\n", cell->ldat.val.buf);
    if (tags!=NULL) print_message("TAGS      = %s\n", tags->ldat.val.buf);
    if (stts!=NULL) print_message("STATUS    = %s\n", stts->ldat.val.buf);
    if (date!=NULL) print_message("DATE      = %s\n", date->ldat.val.buf);

    if (stts!=NULL || cell!=NULL) print_message("------------------------------------\n");

    return ;
}



int main(int argc, char** argv)
{
    if (argc<2) {
        fprintf(stderr, "usage ... %s filename\n", argv[0]);
        exit(1);
    }

    tJson* json = json_parse_file(argv[1], 0);
    if (json==NULL) exit(1);


    tJson* pp = search_top_bracket_json(json, 5);
    print_tTree(stdout, pp->next);
    




    /*
    print_tTree(stdout, json);

    json = json->next;
    if (json==NULL) return 1;

    while (json->esis!=NULL) {
        json = json->esis;
    }

    int n = 0;
    while (json!=NULL) {
        n++;
        check_json(json);
        json = json->ysis;
    }   
    */

    //printf("N= %d\n",n);

    return 0;
}



