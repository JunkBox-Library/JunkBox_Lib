
#include "mysql_tool.h"


int main()
{
    MYSQL_RES* result;
    MYSQL_ROW  row;

    MYSQL* mysql = sql_open("202.26.159.196", "opensim_nsl", "opensim_user", "OpenSimSimPass", 1);
    if (mysql==NULL) {
        printf("Fail Connect\n");
        exit(1);
    }


    int query_res = mysql_query(mysql, "select * from regions");
    if (query_res==0) {
        result = mysql_store_result(mysql);
        if (result!=NULL) {
            int num_rows = mysql_num_rows(result);
            int num_flds = mysql_num_fields(result);

            while(row = mysql_fetch_row(result)) {
                int i;
                for (i=0; i< num_flds; i++) {
                    printf("%s\t", row[i]);
                }
                printf("\n");
            }
            mysql_free_result(result);
        }
    }


    sql_close(mysql);

    return 0;
}
