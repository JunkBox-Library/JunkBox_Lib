
#include "txml.h"
#include "sl_relay_voice_io.h"




/**

    TEST for  transform_cap_xml()  function in sl_realy/sl_tcp_transformation.c

*/

int   Count = 0;
int   CountA = 0;

int   UseWhiteFilter  = OFF;
int   UseServerSSL = ON;

int   UseVoiceServer = OFF;
voice_param  VoiceParam;
int   VoiceServerPort = 0;
char* VoiceServerIP = NULL;
char* VoiceServerIPNum = NULL;

char* MyIPaddrNum  = NULL;
int   MaxUdpExPort = 9000;
int   MinUdpExPort = 10000;

//pid_t CrntPID = 0;




int main()
{
    int ret;
    Buffer buf;
    tXML*  sxml;
    struct sockaddr_in ut_addr;


    char*   svrip  = NULL;
    char*   hname  = NULL;
    unsigned short portn, hport;


    DebugMode = ON;
    UnitTestMode = ON;

    buf = make_Buffer(BUFSZ);
    //read_lines_Buffer(&buf, stdin);
    //sxml = xml_parse((char*)buf.buf);

    //buf = read_Buffer_file("PFrh5ub8fgZOrZor");
    //sxml = xml_parse(buf.buf);
    //free_Buffer(&buf);

    //sxml = xml_parse_file("PFrh5ub8fgZOrZor");
    sxml = xml_parse_file("uploader.xml");

    /////////////////////////////////////////////////////////////////////////////////////////
    ret = transform_cap_xml(sxml, NULL, "202.26.159.100", 1, ut_addr, 1, ut_addr, NULL);
    /////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////
    //ret = transform_login_xml(sxml, &svrip, &portn, 998, &hname, &hport, 999, "202.26.159.140");
    //print_message("LOGIN_RELAY: TCP IP address  transform %s \n", hname);
    //print_message("LOGIN_RELAY: TCP Port number transform %d \n", hport);
    //print_message("LOGIN_RELAY: UDP IP address  transform %s \n", svrip);
    //print_message("LOGIN_RELAY: UDP Port number transform %d \n", portn);
    /////////////////////////////////////////////////////////////////////////////////////////


    printf("ret = %d\n", ret);

    buf = xml_inverse_parse(sxml, 2);
    if (buf.buf!=NULL) fprintf(stdout, "%s", buf.buf);

    if (sxml->state!=XML_PARSED) {
        if    (sxml->state==XML_PARSE_ERROR)    fprintf(stdout, "XML Parse Error!! (%d)\n", sxml->state);
        else if (sxml->state==XML_MEMORY_ERROR) fprintf(stdout, "XML Memory Error!! (%d)\n", sxml->state);
        else                                    fprintf(stdout, "XML any Error!! (%d)\n", sxml->state);
    }


    del_tTree(&sxml);

    return 0;
}

