
#include "buffer.h"
#include "fdb.h"





void xyz(DB* db)
{
    Buffer key, dat;

    key = make_Buffer(20);

    copy_s2Buffer("AAAAAAAA", &key);
    dat = get_fdb_data(db, key);
    printf("dat = %s\n", dat.buf);
    free_Buffer(&key);
    free_Buffer(&dat);

    copy_s2Buffer("XXXXXXXX", &key);
    dat = get_fdb_data(db, key);
    printf("dat = %s\n", dat.buf);
    free_Buffer(&key);
    free_Buffer(&dat);

    copy_s2Buffer("AAA", &key);
    del_fdb_data(db, key);
    free_Buffer(&key);

    copy_s2Buffer("X", &key);
    dat = get_fdb_data(db, key);
    printf("dat = %s\n", dat.buf);
    free_Buffer(&key);
    free_Buffer(&dat);

    tList* lp = get_all_fdb_data(db);
    print_tList(stderr, lp);
    del_tList(&lp);

}






main()
{
    DB* db = open_fdb("DB", 0);
    if (db!=NULL) xyz(db);
    close_fdb(&db); 
}

