
#ifndef  __JBXL_IP_ADDRESS_TOOL_H_
#define  __JBXL_IP_ADDRESS_TOOL_H_


/**  
@brief   IP Address Tool Header
@file    ipaddr_tool.h
@author  Fumi Iseki 
@date    2005 09/21
*/


#include "network.h"
#include "tlist.h"


tList*  read_ipaddr_file(char* fn);
tList*  read_ipaddr_fp(FILE* fp);
tList*  to_ipaddress_list(tList* lp);

int     is_host_in_list(tList* lp, unsigned char*, char* host);
int     is_ipaddr_num_in_list(tList* lp, unsigned char* ipnum);

void    print_address_in_list(FILE* fp, tList* lp);


#endif

