
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "fdb.h"




DB* db;

void xyz()
{
    int x;
    Buffer key, dat;

    key = make_Buffer(12);
    dat = make_Buffer(64);


    copy_s2Buffer("XXXXXXXX", &key);
    copy_s2Buffer("JJJJJJJJ", &dat);
    x = set_fdb_data(db, key, dat);

    copy_s2Buffer("AAAAAAAA", &key);
    copy_s2Buffer("DDDDDDDD", &dat);
    x = set_fdb_data(db, key, dat);

    copy_s2Buffer("AAA", &key);
    copy_s2Buffer("DDDDD", &dat);
    x = set_fdb_data(db, key, dat);

    copy_s2Buffer("VVV", &key);
    copy_s2Buffer("GGGGGGGG", &dat);
    x = set_fdb_data(db, key, dat);

    copy_s2Buffer("X", &key);
    copy_s2Buffer("DDDD", &dat);
    x = set_fdb_data(db, key, dat);
}






main()
{
    db = open_fdb("DB", DB_CREATE);
    if (db!=NULL) xyz();
    close_fdb(&db); 
}
