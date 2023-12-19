
#include "fdb.h"



int main()
{
    int ret;
    u_int32_t flag;
    DB_ENV* envp=NULL;

    DB*  dp=NULL;
    Buffer key, data;
    Buffer dbdata;


    ret = db_env_create(&envp, 0);
    if (ret!=0) {
        printf("db_env_create ret = %d\n", ret);
        exit(1);
    }

   flag =
      DB_CREATE     |
      DB_INIT_LOCK  |  /* Initialize the locking subsystem */
      DB_INIT_TXN   |  /* Initialize the transactional subsystem. This * also turns on logging. */
      DB_INIT_LOG   |
      DB_INIT_MPOOL;

    ret = envp->set_lk_detect(envp, DB_LOCK_MINWRITE);

    ret = envp->open(envp, "./", flag, 0);
    if (ret!=0) {
        printf("open\n");
    }


    ret = db_create(&dp, envp, 0);
    if (ret==0) {
        ret = dp->open(dp, NULL, "DB", NULL, DB_HASH, DB_CREATE, 0600);
        if (ret!=0) printf("OPEN2\n");
    }
    else printf("OPEN1\n");

    DebugMode = ON;
    key = make_Buffer(50);
    copy_s2Buffer("X", &key);
    data = get_fdb_data(dp, key);
    printf("dat = %s\n", data.buf);

}

