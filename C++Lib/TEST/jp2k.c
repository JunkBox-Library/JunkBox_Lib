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

    if (jp2.isNull()) printf("SSSSSSSSSSSSSSSSSS\n");

    printf(" %d - %d - %d\n", jp2.col, jp2.state, (int)jp2.image->numcomps);

    return 0;
}



