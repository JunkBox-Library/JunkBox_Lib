
#include "gz_tool.h"


char fmfn[] = "ZZ";
char tofn[] = "output.txt";

int main()
{
    uLongf insz = (uLongf)file_size((char*)fmfn);
    uLongf otsz = insz*1000;

    FILE* infp = fopen(fmfn, "rb");
    if (infp==NULL) return -1;

    unsigned char* inbuf = (unsigned char*)malloc(insz);
    unsigned char* otbuf = (unsigned char*)malloc(otsz);
    if (inbuf==NULL || otbuf==NULL) {
        if (inbuf!=NULL) free(inbuf);
        if (otbuf!=NULL) free(otbuf);
        return -1;
    }


    //
    z_stream zstrm;

    zstrm.zalloc    = Z_NULL;
    zstrm.zfree     = Z_NULL;
    zstrm.opaque    = Z_NULL;
    zstrm.next_in   = Z_NULL;
    zstrm.avail_in  = 0;

    int ret = inflateInit2(&zstrm, 47);
    if (ret!=Z_OK) {
        print_message("XXXX: inflateInit2() Error: %d\n", ret);
        return -1;
    }

    zstrm.avail_in = fread(inbuf, 1, insz, infp);

    while(!feof(infp)) {

        zstrm.next_in   = inbuf;

        do {
            //
            zstrm.next_out  = otbuf;
            zstrm.avail_out = otsz; 

            ret = inflate(&zstrm, Z_NO_FLUSH);

            print_message("-> %d\n", zstrm.avail_out);
            print_message("==> %s\n", otbuf);

            if (ret==Z_STREAM_END && (zstrm.avail_in!=0 || !feof(infp))) {
                ret = inflateReset(&zstrm);
                zstrm.avail_out = 0;    // use as end flag
            } 

            zstrm.avail_in = fread(inbuf, 1, insz, infp);

        } while (zstrm.avail_out==0 && ret!=Z_STREAM_END);


    }

    inflateEnd(&zstrm);

}
