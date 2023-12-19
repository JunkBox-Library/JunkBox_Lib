
#include "tlist.h"
#include "xtools.h"


int main()
{
    tList* cc = cawk_tList("   jdjsajdla dsahashdak     hsdhjsa", ' ');

    print_tList(stdout, cc);

    //Buffer* xx = get_Buffer_dim_tList_value(cc);
    Buffer* xx = get_Buffer_dim_tList(cc);

    if (xx!=NULL) {
        printf("len = %d\n", xx->state);
        Buffer  zz = join_Buffer_dim(xx, ":");
        printf("str = %s\n", zz.buf);
        del_Buffer_dim(&xx);
    }
}
