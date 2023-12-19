#include "tools.h"


int main ()
{
    unsigned char a[] = "aaff5349-a04a-f0bf-600f-d18c8912dd85";
    unsigned char* uuid;
    unsigned char* guid;

    uuid = guid2uuid(a);
    guid = uuid2guid(uuid);

    printf("%s\n", a);
    printf("%s\n", guid);

    return 0;
}

