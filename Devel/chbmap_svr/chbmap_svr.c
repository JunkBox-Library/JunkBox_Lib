/*  

*/


#include "chbmap_svr.h"
#include "shape_file.h"


int     Vofd            = 0;
int     Wofd            = 0;
int     MaxIdleTime     = 300;

char*   MyIPaddr        = NULL;
char*   MyIPaddrNum     = NULL;
char*   ClIPaddr        = NULL;
char*   ClIPaddrNum     = NULL;

tList*  Allow_IPaddr    = NULL;


char*   Hosts_Allow_File = "/usr/local/etc/chb_svr/hosts.allow";




int main(int argc, char** argv)
{
    int      i, cdlen;
    unsigned short  vport=0;
    struct sockaddr_in cl_addr;
    struct sigaction sa;

    Buffer conffile, imgfile, pidfile, shpfile;

    // 引数処理
    conffile = make_Buffer(LNAME);
    imgfile  = make_Buffer(LNAME);
    pidfile  = make_Buffer(LNAME);
    shpfile  = make_Buffer(LNAME);

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i],"-f"))        {if (i!=argc-1) copy_s2Buffer(argv[i+1], &conffile);}
        else if (!strcmp(argv[i],"-i"))   {if (i!=argc-1) copy_s2Buffer(argv[i+1], &imgfile);}
        else if (!strcmp(argv[i],"-s"))   {if (i!=argc-1) copy_s2Buffer(argv[i+1], &shpfile);}
        else if (!strcmp(argv[i],"-p"))   {if (i!=argc-1) vport = (unsigned short)atoi(argv[i+1]);}
        else if (!strcmp(argv[i],"-d"))   DebugMode = ON;
        else if (!strcmp(argv[i],"-pid")) {if (i!=argc-1) copy_s2Buffer(argv[i+1], &pidfile);}

    }

    if (vport==0) vport = SERVER_PORT;


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
    
    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_GEO_PXL_SCALE);
    double ddx = get_tiff_double_field(ifd, 0);
    double ddy = get_tiff_double_field(ifd, 1);
    double ddz = get_tiff_double_field(ifd, 2);
    vector dd  = set_vector(ddx, ddy, ddz);

    ifd = find_tiff_ifd(*ptr_ifd, TIFF_TAG_GEO_TIEPOINT);
    double ogx = get_tiff_double_field(ifd, 3);
    double ogy = get_tiff_double_field(ifd, 4);
    double ogz = get_tiff_double_field(ifd, 5);
    vector og  = set_vector(ogx, ogy, ogz);


    //
    if (depth==32 && dbyte==1) {
        data_type = 1;
    }
    else if (depth=8 && dbyte==3) {
        data_type = 2;
        MSGraph* mp = (MSGraph*)((*ptr_ifd)->ex_value);
        BSGraph* vp = (BSGraph*)malloc(sizeof(BSGraph));
        *vp = make_BSGraph(mp->xs, mp->ys, 3);
        
        if (vp->gp!=NULL) {
            int j, ii, jj, xx, ps = vp->xs*vp->ys;
            for (j=0; j<vp->ys; j++) {
                jj = j*vp->xs;
                for (i=0; i<vp->xs; i++) {
                    ii = jj + i;
                    xx = ii*3;
                    vp->gp[ii]      = mp->gp[xx];
                    vp->gp[ii+ps]   = mp->gp[xx+1];
                    vp->gp[ii+ps*2] = mp->gp[xx+2];
                }
            }
            free_MSGraph(mp);
            (*ptr_ifd)->ex_value = (void*)vp;

            //
            // Shape File
            //
            if (shpfile.buf!=NULL) {
                Buffer filenm = make_Buffer(LNAME);
                copy_Buffer(&shpfile, &filenm);
                cat_s2Buffer(".shx", &filenm);
                FILE* fp = fopen(filenm.buf, "rb");


                if (fp!=NULL) {
                    print_message("Reading Shape Index File...\n");
                    ShapeIndex* idx = read_shape_index_file(fp);
                    fclose(fp);

                    if (idx!=NULL) {
                        copy_Buffer(&shpfile, &filenm);
                        cat_s2Buffer(".shp", &filenm);
                        fp = fopen(filenm.buf, "rb");
                        if (fp!=NULL) {
                            print_message("Reading Shape Main File...");
                            vp = read_shape_main_file(idx, vp, og, dd, fp);
                            print_message(" done. %d data readed.\n", idx->maxnum);
                            fclose(fp);

                            DEBUG_MODE {
                                ShapeIndex* sx = idx;
                                for (i=0; i<idx->maxnum; i++) {
                                    print_message("num = %3d, type = %2d, max = %3d, offset = %5d, size = %4d\n", 
                                                            sx->datanum, sx->type, sx->maxnum, sx->offset, sx->size);
                                    sx++;
                                }
                            }
                            print_message("\n");
                        }
                        free(idx);
                    }
                }
            }
        }
        else {
            print_message("ERROR: no more memory.\n");
            free_TIFF_ifd_dem(ptr_ifd);
            exit(1);
        }
    }
    else {
        print_message("ERROR: unknown data type.\n");
        free_TIFF_ifd_dem(ptr_ifd);
        exit(1);
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (data_type!=2) {
        print_message("ERROR: unsupported data type. type = %d\n", data_type);
        free_TIFF_ifd_dem(ptr_ifd);
        exit(1);
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // メインループ
    //
    DEBUG_MODE {
        print_message("CHB_SVR: start CHB Server Main Loop for TCP connection. Port = %d\n", vport);
        print_message("CHB_SVR: wait... GET lon lat width height center\n");
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
        if (rcv!=NULL && rcv->state>=3) {
            if (!strcasecmp("GET", rcv[0].buf)) {
                double lon = atof(rcv[1].buf);
                double lat = atof(rcv[2].buf);

                int xsz = 256;
                int ysz = 256;
                int cnt = FALSE;

                if (rcv->state>=5) {
                   xsz = atoi(rcv[3].buf);
                   ysz = atoi(rcv[4].buf);
                }
                if (rcv->state>=6) {
                   cnt = atoi(rcv[5].buf);
                }

                JPEGImage jpg = get_terrain_texture(*ptr_ifd, lon, lat, xsz, ysz, cnt);
                if (jpg.state==0) {
                    unsigned char  msg[21];
                    unsigned char* buf = NULL;
                    unsigned long  len = 0;

                    int ret = write_jpeg_mem(&buf, &len, jpg, 100);
                    if (ret==0 && len!=0) {
                        snprintf(msg, 20, "%x", len);
                        tcp_send_mesgln(Wofd, msg);
                        tcp_send(Wofd, buf, len);
                    }
                    freeNull(buf);
                }
                else {
                    tcp_send_mesgln(Wofd, "ERROR");
                }
                free_JPEGImage(&jpg);
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




JPEGImage get_terrain_texture(TIFF_ifd* ifd, double lon, double lat, int xsz, int ysz, int center)
{
    TIFF_ifd* ptr = NULL;
    JPEGImage jpg;
    BSGraph*  mp = (BSGraph*)(ifd->ex_value);

    memset(&jpg, 0, sizeof(JPEGImage));
    jpg.state = -1;

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
    double lons = get_tiff_double_field(ptr, 3);
    double lats = get_tiff_double_field(ptr, 4);

    int mpxs, mpys, mpxe, mpye;

    if (center==TRUE) {
        mpxs = (int)( (lon - lons)/dlon + mpxo - xsz/2);
        mpys = (int)(-(lat - lats)/dlat + mpyo - ysz/2);
    }
    else {
        mpxs = (int)( (lon - lons)/dlon + mpxo);
        mpys = (int)(-(lat - lats)/dlat + mpyo);
    }   
    mpxe = mpxs + xsz;
    mpye = mpys + ysz;

    if (mpxs>=0 && mpxe<width && mpys>=0 && mpye<height) {
        int xs = mpxe - mpxs + 1;
        int ys = mpye - mpys + 1;
        if (xs>0 && ys>0) {
            BSGraph vp = make_BSGraph(xs, ys, 3);
            int ps = xs*ys, ls = mp->xs*mp->ys;
            int i, j;
            for (j=0; j<ys; j++) {
                int jj = j*xs;
                int yy = (mpys+j)*mp->xs;
                for (i=0; i<xs; i++) {
                    int ii = jj + i;
                    int xx = yy + mpxs + i;
                    vp.gp[ii]      = mp->gp[xx];
                    vp.gp[ii+ps]   = mp->gp[xx+ls];
                    vp.gp[ii+ps*2] = mp->gp[xx+ls*2];
                }
            }

            jpg = BSGraph2JPEGImage(vp);
            free_BSGraph(&vp);
        }
    }
    
    return jpg;
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








