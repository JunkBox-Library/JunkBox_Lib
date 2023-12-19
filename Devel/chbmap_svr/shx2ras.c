
#include  "shape_file.h"



main()
{
    FILE* fp = fopen("./inba/admin_bnd/admin_bnd.shx", "rb");
    if (fp==NULL) exit(1);

    ShapeIndex* idx = read_shape_index_file(fp);
    fclose(fp);

    if (idx==NULL) {
        print_message("File Open Error\n");
        exit(1);
    }


    int i;
    int num = idx->maxnum;
    ShapeIndex* sx = idx;

    for (i=0; i<num; i++) {
        fprintf(stdout, "num = %d, type = %d, max = %d, offset = %d, size = %d\n", sx->datanum, sx->type, sx->maxnum, sx->offset, sx->size);
        sx++;
    }

    fp = fopen("./inba/admin_bnd/admin_bnd.shp", "rb");
    if (fp==NULL) exit(1);

    vector org  = set_vector(0.0, 0.0, 0.0);
    vector ddx  = set_vector(0.001, 0.001, 1.0);
    BSGraph* mp = read_shape_main_file(idx, NULL, org, ddx, fp);
    fclose(fp);

    if (mp!=NULL) {
        WSGraph vp = B2WSGraph(*mp);
        write_ras_file("ZZZ.ras", vp);
        free_BSGraph(mp);
        free_WSGraph(&vp);
    }

    free(idx);
}

