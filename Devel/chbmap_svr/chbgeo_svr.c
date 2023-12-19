/*  

*/

#include "chbgeo_svr.h"


int     Vofd            = 0;
int     Wofd            = 0;
int     MaxIdleTime     = 300;

char*   MyIPaddr        = NULL;
char*   MyIPaddrNum     = NULL;
char*   ClIPaddr        = NULL;
char*   ClIPaddrNum     = NULL;

tList*  Allow_IPaddr    = NULL;

pid_t   RootPID         = 0;

char*   Hosts_Allow_File = "/usr/local/etc/chb_svr/hosts.allow";




int main(int argc, char** argv)
{
    int      i, j, cdlen;
    unsigned short  vport=0;
    struct sockaddr_in cl_addr;
    struct sigaction sa;

    Buffer conffile, imgfile, pidfile;

    // 引数処理
    conffile = make_Buffer(LNAME);
    imgfile  = make_Buffer(LNAME);
    pidfile  = make_Buffer(LNAME);

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i],"-f"))        {if (i!=argc-1) copy_s2Buffer(argv[i+1], &conffile);}
        else if (!strcmp(argv[i],"-i"))   {if (i!=argc-1) copy_s2Buffer(argv[i+1], &imgfile);}
        else if (!strcmp(argv[i],"-p"))   {if (i!=argc-1) vport = (unsigned short)atoi(argv[i+1]);}
        else if (!strcmp(argv[i],"-d"))   DebugMode = ON;
        else if (!strcmp(argv[i],"-pid")) {if (i!=argc-1) copy_s2Buffer(argv[i+1], &pidfile);}

    }

    if (vport==0) vport = SERVER_PORT;
    RootPID = getpid();

/*
    // 設定ファイルの読み込み
    //if (conffile.buf[0]=='\0') copy_s2Buffer(CONFIG_FILE, &conffile);
    //read_config_file((char*)conffile.buf);
    //free_Buffer(&conffile);
*/


/*
    // PIDファイルの作成
    if (pidfile.buf[0]!='\0') {
        FILE* fp = fopen((char*)pidfile.buf, "w");
        if (fp!=NULL) {
            fprintf(fp, "%d", (int)RootPID);
            fclose(fp);
        }
    }
    free_Buffer(&pidfile);

    // 接続許可・禁止ファイルの読み込み
    Allow_IPaddr = read_ipaddr_file(Hosts_Allow_File);
    if (Allow_IPaddr!=NULL) {
        DEBUG_MODE {
            print_message("[%d] CHB_SVR: readed access control list.\n", RootPID);
            print_address_in_list(stderr, Allow_IPaddr);
        }
        if (UseIntWebProxy) add_tList_node_str(Allow_IPaddr, "127.0.0.1", NULL);
    }
    else {
        DEBUG_MODE print_message("CHB_SVR: cannot read access contorol list. no access control.\n");
    }
*/

    // シグナルハンドリング
    sa.sa_handler = sigterm_process;
    sa.sa_flags   = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGHUP,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    DEBUG_MODE set_sigseg_handler(NULL);    // Segmentation Falt check


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // read TIFF file
    //
    TIFF_ifd*  ifd = NULL;
    TIFF_ifd** ptr_ifd = NULL;
    int data_type = 0;

    if (imgfile.buf==NULL) {
        print_message("ERROR: Image file is not specified!!\n");
        exit(1);
    }

    ptr_ifd = read_tiff_file(imgfile.buf);
    if (ptr_ifd==NULL) {
        print_message("ERROR: Is %s TIFF File?\n", imgfile.buf);
        exit(1);
    }

    int nn = 0;
    TIFF_ifd** ptr = ptr_ifd;
    while((*ptr)!=NULL) {
        ptr++;
        nn++;
    }

    if (nn!=1) {
        print_message("ERROR: not support this image number = %d\n", nn);
        free_TIFF_ifd_dem(ptr_ifd);
        exit(1);
    }

    print_tiff_ifd(stderr, *ptr_ifd, 20);
    print_message("\n");


    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_DEPTH);
    int depth = get_tiff_uint_field(ifd, 0);
    
    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_PIXEL);
    int dbyte = get_tiff_uint_field(ifd, 0);
    

    if (depth==32 && dbyte==1) {
        data_type = 1;
    }
    else if (depth=8 && dbyte==3) {
        data_type = 2;
    }
    else {
        print_message("ERROR: unknown data type.\n");
        free_TIFF_ifd_dem(ptr_ifd);
        exit(1);
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (data_type!=1) {
        print_message("ERROR: unsupported data type. type = %d\n", data_type);
        free_TIFF_ifd_dem(ptr_ifd);
        exit(1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // メインループ
    //
    DEBUG_MODE {
        print_message("CHB_SVR: start CHB Server Main Loop for TCP connection. Port = %d\n", vport);
        print_message("CHB_SVR: wait... GET lon lat width height\n");
    }

    Buffer buf = make_Buffer(RECVBUFSZ);
    Vofd = tcp_server_socket((int)vport);

    Loop {
        cdlen = sizeof(cl_addr);
        Wofd = accept_intr(Vofd, (struct sockaddr*)&cl_addr, (socklen_t*)&cdlen);
        if (Wofd<0) {
            //DEBUG_MODE print_message("[%d] CHB_SVR: ERROR: accept() error.\n", CrntPID);
            print_message("CHB_SVR: ERROR: accept() error [%s].\n", strerror(errno));
            exit(1);
        }

        ClIPaddr    = get_ipaddr(cl_addr.sin_addr);
        ClIPaddrNum = (char*)to_address_num4(ClIPaddr, 0);

        // IP Address check
        if (Allow_IPaddr!=NULL && !is_host_in_list(Allow_IPaddr, (unsigned char*)ClIPaddrNum, NULL)) {
            print_message("CHB_SVR: ERROR: not allowed access from [%s]\n", ClIPaddr);
            freeNull(ClIPaddr);
            freeNull(ClIPaddrNum);
            close(Wofd);
            Wofd = 0;
            continue;
        }

        int cc = tcp_recv_Buffer(Wofd, &buf);
        Buffer* rcv = cawk_Buffer_dim(buf, ' ');
        if (rcv!=NULL && rcv->state>=5) {
            if (!strcasecmp("GET", rcv[0].buf)) {
                double lon  = atof(rcv[1].buf);
                double lat  = atof(rcv[2].buf);
                int   ysize = atoi(rcv[3].buf);
                int   xsize = atoi(rcv[4].buf);

                DEBUG_MODE print_message("CHB_SVR: (lon, lat) = (%f, %f), (width, height) = (%d, %d)\n", lon, lat, xsize, ysize);
                FSGraph alt = get_terrain_altitude(*ptr_ifd, lon, lat, xsize, ysize);
                if (alt.state==0) {
                    unsigned long  len = alt.xs*alt.ys*4;
                    tcp_send(Wofd, (char*)alt.gp, len);
                }
                else {
                    tcp_send_mesgln(Wofd, "ERROR");
                }
                free_FSGraph(&alt);
            }
            else {
                tcp_send_mesgln(Wofd, "ERROR");
            }
        }
        else {
            tcp_send_mesgln(Wofd, "ERROR");
        }

        free_Buffer(rcv);


        freeNull(ClIPaddr);
        freeNull(ClIPaddrNum);
        close(Wofd);
        Wofd = 0;
    }


    // not reachable
    socket_close(Vofd);
    Vofd = 0;
    freeNull(MyIPaddr); 

    exit(0);
}




FSGraph get_terrain_altitude(TIFF_ifd* ifd, double lon, double lat, int xsize, int ysize)
{
    TIFF_ifd* ptr = NULL;
    FSGraph   alt;
    MSGraph*  mp = (MSGraph*)(ifd->ex_value);

    memset(&alt, 0, sizeof(FSGraph));
    alt.state = -1;

    ptr= find_tiff_ifd(ifd, TIFF_TAG_WIDTH);
    int width = get_tiff_uint_field(ptr, 0);

    ptr = find_tiff_ifd(ifd, TIFF_TAG_HEIGHT);
    int height = get_tiff_uint_field(ptr, 0);

    ptr= find_tiff_ifd(ifd, TIFF_TAG_GEO_PXL_SCALE);
    double dlon = get_tiff_double_field(ptr, 0);
    double dlat = get_tiff_double_field(ptr, 1);

    ptr = find_tiff_ifd(ifd, TIFF_TAG_GEO_TIEPOINT);
    double mpxo = get_tiff_double_field(ptr, 0);
    double mpyo = get_tiff_double_field(ptr, 1);
    double lono = get_tiff_double_field(ptr, 3);
    double lato = get_tiff_double_field(ptr, 4);

    int mpxs = (int)( (lon - lono)/dlon + mpxo);
    int mpys = (int)(-(lat - lato)/dlat + mpyo);
    int mpxe = mpxs + xsize - 1;
    int mpye = mpys + ysize - 1;

    if (mpxs>=0 && mpxe<width && mpys>=0 && mpye<height) {

        alt = make_FSGraph(xsize, ysize, 1);

        int ps = xsize*ysize;
        int i, j;
        for (j=0; j<ysize; j++) {
            int jj = j*xsize;
            int yy = (mpys+j)*mp->xs;
            for (i=0; i<xsize; i++) {
                int ii = jj + i;
                int xx = yy + mpxs + i;
                if (TIFF_Swap_Flag) {
                    alt.gp[ii]  = swapl(((float*)mp->gp)[xx]);
                }
                else {
                    alt.gp[ii]  = ((float*)mp->gp)[xx];
                }
            }
        }
    }
    
    return alt;
}










//////////////////////////////////////////////////////////////////////////
// プログラムの終了
//
void  sigterm_process(int sig)
{  
    DEBUG_MODE print_message("SIGTERM_PROCESS: sigterm_process is called.\n");

    // Socket
    if (Wofd>0) socket_close(Wofd);
    if (Vofd>0) socket_close(Vofd);
    Wofd = 0;
    Vofd = 0;

    exit(sig);
}




