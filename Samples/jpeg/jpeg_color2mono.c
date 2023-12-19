
#include  "jpeg_tool.h"
#include  "gio.h"

int main(void)
{
    JPEGImage jp;
    WSGraph   vp;
    int  i, j;

    jp = read_jpeg_file("XX.jpg");
    if (jp.gp==NULL) {
        print_message("fail read XX.jpg!!\n");
        exit(1);
    }
    vp = JPEGImage2WSGraph(jp);
    free_JPEGImage(&jp);

    WSGraph gp = make_WSGraph(vp.xs, vp.ys, 1);

    int max = 0;
    int min = vp.gp[0];
    for(i=0; i<vp.ys*vp.xs; i++) {
        gp.gp[i] = (vp.gp[i] + vp.gp[i+vp.xs*vp.ys] + vp.gp[i+vp.xs*vp.ys*2])/3;
        max = Max(max, gp.gp[i]);
        min = Min(min, gp.gp[i]);
    }
    free_WSGraph(&vp);

    print_message("max = %d, min = %d\n", max, min);
    for(i=0; i<vp.ys*vp.xs; i++) {
        gp.gp[i] = 255*(gp.gp[i]-min)/(max-min);
    }

    write_ras_file("X.im8", gp);

    jp = WSGraph2JPEGImage(gp);
    free_WSGraph(&gp);

    write_jpeg_file("X.jpg", jp, 0);
    free_JPEGImage(&jp);

    return 0;
}
