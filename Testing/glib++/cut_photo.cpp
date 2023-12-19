


#include  "extract_graph.h"




int main(int argc, char** argv)
{
    int  i;
    Buffer fname = make_Buffer(LNAME);

    for (i=1; i<argc; i++) {
        if      (!strcmp(argv[i],"-i")) {if (i!=argc-1) copy_s2Buffer(argv[i+1], &fname);}
        else if (!strcmp(argv[i],"-d")) DebugMode = ON;
    }

    if (fname.buf[0]=='\0') {
        print_message("Usage ... %s -i filename [-d]\n", argv[0]);
        exit(1);
    }


    // Input
    JPEGImage jp = read_jpeg_file((char*)fname.buf);
    if (jp.isNull()) {
        print_message("file %s open error!!\n", fname.buf);
        exit(1);
    }


    // Translation
    tList* lv = extract_graph(jp);
    if (lv==NULL) {
        print_message("fail extract graph!!\n");
        exit(1);
    }
    jp.free();


    // Output
    int  len, num = 0;
    char fext[LSNAME];

    for (i=fname.vldsz-1; i>=0; i--) {
        if (fname.buf[i]=='.') break;
    }
    if (i==-1) len = 0;
    else       len = i;

    tList* lt = lv;
    while (lt!=NULL) {
        JPEGImage* pp = (JPEGImage*)lt->ldat.ptr;
        fname.buf[len] = '\0';
        fname.vldsz    = len;
        snprintf(fext, LSNAME, "_%02d.jpg", num);
        cat_s2Buffer(fext, &fname);
        write_jpeg_file((char*)fname.buf, *pp, 100);
        num++;
        lt = lt->next;
    }

    del_tList_object<JPEGImage>(&lv);

    return 0;
}




