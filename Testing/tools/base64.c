
#include "tools.h"


int main()
{
/*
    unsigned char xx[4] = {0x40, 0x9a, 0xdf, 0xa9};

    char* p = (char*)encode_base64(xx, 4);
    printf("XXX = %s\n", p);
*/

    int  s;
    char x[] = "Q049aXNla2ksT1U95pWZ5ZOhLE9VPeODpuODvOOCtuODvCxEQz1lZHV0dWlzLERDPWxvY2Fs";

    unsigned char *p = (unsigned char*)decode_base64((unsigned char*)x, &s);

    fdump(stdout, p, s);
    printf("%d: x = %s\n", s, p);

    return 0;
}
