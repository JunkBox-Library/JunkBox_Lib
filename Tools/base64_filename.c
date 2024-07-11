
#include "Lib/tools.h"

int main()
{
    unsigned char enc[] = "AAAAAAAAAAAACwAAAAAAAAAAAAAAAABP";
                                     //                                 "f39-mQAAAAAACgAAAAAAAAAAAAAAAABP";
    int sz = 0;

    unsigned char* buf = decode_base64_filename(enc, &sz, '-');

    float r = 1.0f - (float)buf[0]/255.0f;
    float g = 1.0f - (float)buf[1]/255.0f;
    float b = 1.0f - (float)buf[2]/255.0f;
    float a = 1.0f - (float)buf[3]/255.0f;

    float c = (float)buf[4]/255.0f;
    float s = (float)buf[5]/255.0f;
    float w = (float)buf[6]/255.0f;
    float t = (float)buf[7]/255.0f;
    float l = (float)buf[8]/255.0f;
    int   m = (int)buf[9];
    char  k = (char)buf[23];


    printf("(R, G, B, A) = (%f, %f, %f, %f)\n", r, g, b, a);
    printf("cutout = %f\n", c);
    printf("shininess = %f\n", s);
    printf("glow   = %f\n", w);
    printf("bright = %f\n", t);
    printf("light  = %f\n", l);
    printf("mode   = %d, %d\n", m/10, m%10);
    printf("kind   = %c\n", k);

    return 0;
}
