/**
 JPEG 2000 のテスト用

*/

#include "Jpeg2KTool.h"


using namespace jbxl;



int main(int argc, char** argv)
{
    DebugMode = ON;

    if (argc<2) exit(1);

    JPEG2KImage jp2 = readJPEG2KFile(argv[1]);

    if (jp2.isNull()) printf("Null\n");
    else printf("col = %d, num = %d, mode = %d, spec = %d\n", jp2.col, (int)jp2.image->numcomps, jp2.cmode, jp2.image->color_space);

    printf("state = %d\n", jp2.state);
    return 0;
}

