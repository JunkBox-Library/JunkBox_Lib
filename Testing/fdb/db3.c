
#include "fdb.h"


main()
{
    tList* lt = get_all_fdb_data_file("DB");

    print_tList(stderr, lt);
    del_tList(&lt);
}

