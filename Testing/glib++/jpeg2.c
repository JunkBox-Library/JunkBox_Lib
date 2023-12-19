

#include  "JpegTool.h"
#include  "Gio.h"
#include  "Graph.h"
#include  "Gmt.h"


main()
{
    JPEGImage jp;
    MSGraph<sWord> vp, gd;
    int  i, j;


    // データ読み込み
    jp = read_jpeg_file("img.jpg");
    vp = JPEGImage2MSGraph<sWord>(jp);
    jp.free();


    // グレイスケール化
    gd.set(vp.xs, vp.ys);

    for(i=0; i<gd.ys*gd.xs; i++) {
        //gd.gp[i] = (vp.gp[i] + vp.gp[i+vp.xs*vp.ys] + vp.gp[i+vp.xs*vp.ys*2])/3;
        gd.gp[i] = (vp.gp[i] + vp.gp[i+vp.xs*vp.ys] + vp.gp[i+vp.xs*vp.ys*2]);
        gd.max = Max(gd.max, gd.gp[i]);
        gd.min = Min(gd.min, gd.gp[i]);
    }


/*
    // コントラスト変換
    print_message("max = %d, min = %d\n", gd.max, gd.min);
    if (gd.max!=gd.min) {
        for(i=0; i<gd.ys*gd.xs; i++) {
            gd.gp[i] = 255*(gd.gp[i]-gd.min)/(gd.max-gd.min);
        }
    }
*/

    //vp.free();
    //copy_data_MSGraph(gd, vp);


    //vp = zoom_MSGraph(gd, 0.5, 1);
    //gd.free();



/*
    // ラプラシアン
    gd.free();
    gd = Laplacian<sWord, sWord>(vp);
    for(i=0; i<gd.ys*gd.xs; i++) {
        gd.gp[i] = vp.gp[i] - gd.gp[i];
        //if (gd.gp[i]<0) gd.gp[i] = 0;
        //if (gd.gp[i]>255) gd.gp[i] = 255;
        gd.max = Max(gd.max, gd.gp[i]);
        gd.min = Min(gd.min, gd.gp[i]);
    }
*/


/*
    print_message("max = %d, min = %d\n", gd.max, gd.min);
    if (gd.max!=gd.min) {
        for(i=0; i<gd.ys*gd.xs; i++) {
            gd.gp[i] = 255*(gd.gp[i]-gd.min)/(gd.max-gd.min);
        }
    }
*/

    vp.free();
    vp = Nabra<sWord>(gd);

    gd.free();

    gd = vp;
    //gd = medianFilter<sWord>(vp, 3);

    //print_message("X = %d\n", gd.point(0,0));
    //MSGraph_Paint(gd, 0, 0, 0, 20, 255, 4);


    writeRasFile("X.im8", gd);



    // gdを JPEGファイルへ
    jp = MSGraph2JPEGImage(gd);
    gd.free();

    write_jpeg_file("X.jpg", jp, 0);
    jp.free();
}
