/*  
    JSON TEST Program 
*/


#include "tjson.h"


void  check_json(tJson* json)
{
    tJson* user = NULL;
    tJson* cell = NULL;
    tJson* date = NULL;
    tJson* mssn = NULL;
    tJson* pssn = NULL;
    tJson* tags = NULL;
    tJson* stts = NULL;

    tJson* meta = search_key_json_obj(json, "metadata", 1);
    if (meta!=NULL) {
        stts = search_key_json(meta, "status", FALSE, 1);
        if (stts!=NULL) {   // S->C
            user = search_key_json(json, "username", TRUE, 1);
            if (user!=NULL) {
                date = search_key_sister_json(user, "date", TRUE);
                mssn = search_key_sister_json(user, "session", TRUE);
                pssn = search_key_json(json, "session", TRUE, 2);
            }
        }
        else {              // C->S
            cell = search_key_json(json, "cellId", TRUE, 1);
            if (cell!=NULL) {
                tags = search_key_sister_json(cell, "tags", TRUE);
                date = search_key_json(json, "date", TRUE, 1);
                mssn = search_key_json(json, "session", TRUE, 1);
            }
        }
    }

    if (mssn!=NULL) print_message("SESSION   = %s\n", mssn->ldat.val.buf);
    if (date!=NULL) print_message("DATE      = %s\n", date->ldat.val.buf);
    if (user!=NULL) print_message("USERNAMNE = %s\n", user->ldat.val.buf);
    if (cell!=NULL) print_message("CELLID    = %s\n", cell->ldat.val.buf);
    if (tags!=NULL) print_message("TAGS      = %s\n", tags->ldat.val.buf);
    if (pssn!=NULL) print_message("PSESSION  = %s\n", pssn->ldat.val.buf);
    if (stts!=NULL) print_message("STATUS    = %s\n", stts->ldat.val.buf);

    if (stts!=NULL || cell!=NULL) print_message("------------------------------------\n");

    return ;
}



int main(int argc, char** argv)
{
    if (argc<2) {
        fprintf(stderr, "usage ... %s filename\n", argv[0]);
        exit(1);
    }

    tJson* json = json_parse_file(argv[1], 2);
    if (json==NULL) exit(1);

    //print_tTree(stdout, json);

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

    //printf("N= %d\n",n);

    return 0;
}



