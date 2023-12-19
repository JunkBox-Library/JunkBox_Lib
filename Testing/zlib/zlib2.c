
#include "gz_tool.h"


char fmfn[] = "ZZ";
char tofn[] = "output.txt";

int main()
{
    FILE* infp = fopen(fmfn, "rb");
    if (infp==NULL) {
        return -1;
    }

    FILE* otfp = fopen(tofn, "wb");
    if (otfp==NULL) {
        return -1;
    }

    int sz = gz_decode_fp(infp, otfp);
    print_message("SZ = %d\n", sz);

    fclose(infp);
    fclose(otfp);
    //

}
