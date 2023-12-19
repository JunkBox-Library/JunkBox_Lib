
/*  
        
                by Fumi Iseki '06 3/24
*/

#include "ipaddr_tool.h"




 
int  main(int argc, char** argv)
{
    int i, pmode=OFF;
    char* fname=NULL;
    char* ipchar=NULL;
    unsigned char* ip=NULL;
    tList*  lt = NULL;


    for (i=1; i<argc; i++) {
        if      (!strcmp(argv[i],"-f")) {if (i!=argc-1) fname  = argv[i+1];}
        else if (!strcmp(argv[i],"-a")) {if (i!=argc-1) ipchar = argv[i+1];}
        else if (!strcmp(argv[i],"-p")) pmode = ON;
    }
    if (fname==NULL || (ipchar==NULL && pmode==OFF)) {
        print_message("Usage... %s -f allow_file -a ip-address [-p]\n",argv[0]);
        return -1;
    }

    lt = read_ipaddr_file(fname);
    if (lt==NULL) return -2;

    if (pmode==ON) {
        print_address_in_list(stdout, lt);
        if (ipchar==NULL) {
            del_all_tList(&lt);
            return 0;
        }
    }
    

    ip = to_address_num8_ipv4(ipchar, 1);
    if (ip==NULL) {
        printf("NG");
        del_all_tList(&lt);
        return -3;
    }
    if (is_ipaddr_num_in_list(lt, ip)) printf("OK");
    else                               printf("NG");

    del_all_tList(&lt);

    return 0;
}


