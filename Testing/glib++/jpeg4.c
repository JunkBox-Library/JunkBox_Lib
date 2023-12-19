


#include  "jpeg4.h"




main()
{
    JPEGImage jp;
    MSGraph<sWord> vp, gd;
    int   i, j, ps, px;

    tList* lp;
    tList* lt;


    // データ読み込み
    jp = read_jpeg_file("img.jpg");
    vp = JPEGImage2MSGraph<sWord>(jp);
    jp.free();
    ps = vp.xs*vp.ys;


    // グレイスケール化
    gd.set(vp.xs, vp.ys);
    gd.min = gd.max = vp.gp[0];
    for(i=0; i<gd.ys*gd.xs; i++) {
        gd.gp[i] = (vp.gp[i] + vp.gp[i+ps] + vp.gp[i+ps*2]);
        gd.max = Max(gd.max, gd.gp[i]);
        gd.min = Min(gd.min, gd.gp[i]);
    }
    vp.free();


    // エッジ抽出
    vp = Nabra<sWord>(gd);


    // 背景一次処理
    int th = background_threshold(vp, 10, 2, 3, 0.3, 4, WORK_COLOR);

    MSGraph_Paint(vp, 0, 0, 0, th, WORK_COLOR, 4);
    for(i=0; i<vp.ys*vp.xs; i++) {
        if (vp.gp[i]>=WORK_COLOR) gd.gp[i] = 0;
    }
    vp.free();


    // ノイズ除去
    set_around_MSGraph(gd, 0, EDGE_SIZE);
    delete_noise_MSGraph(gd, 500, 4, WORK_COLOR);
    delete_rough(gd, 20, 4, WORK_COLOR);


    // 領域分割
    lp = lt = get_object_region(gd, 8, WORK_COLOR);


    // 中間データ出力
    char fname[128];
    while(lt!=NULL) {
        MSGraph<sWord>* mp = (MSGraph<sWord>*)lt->ldat.ptr;
        for(i=0; i<mp->ys*mp->xs; i++) {
            if (mp->gp[i]>=WORK_COLOR) mp->gp[i] -= WORK_COLOR;
        }
        mp->get_minmax();
        if (mp->max!=mp->min) {
            for(i=0; i<mp->ys*mp->xs; i++) {
                mp->gp[i] = 255*(mp->gp[i] - mp->min)/(mp->max - mp->min);
            }
        }
        print_message("(%d, %d)-(%d, %d) %d %d\n", mp->rbound.xmin, mp->rbound.ymin, mp->rbound.xmax, mp->rbound.ymax, mp->max, mp->min);
        snprintf(fname, 128, "%d.im8", lt->ldat.id);
        writeRasFile(fname, *mp);
        lt = lt->next;
    }


    lt = lp;
    while(lt!=NULL) {
        MSGraph<sWord>* mp = (MSGraph<sWord>*)lt->ldat.ptr;

        PCoordinate<int> coord = detect_cross_point(mp, WORK_COLOR);
        while (coord.dim<0) {
            MSGraph<sWord> xp = grab_MSGraph(*mp, 1, 1, mp->xs-2, mp->ys-2);    
            mp->free();
            *mp = xp;
            coord = detect_cross_point(mp, WORK_COLOR);
        }

        int xs = mp->rbound.xmin;
        int ys = mp->rbound.ymin;
        for (int k=0; k<coord.dim; k++) {
            int l  = (k+1)%coord.dim;
            int sx = xs + (int)(coord.point[k]->x);
            int sy = ys + (int)(coord.point[k]->y);
            int ex = xs + (int)(coord.point[l]->x);
            int ey = ys + (int)(coord.point[l]->y);
            MSGraph_Line(gd, sx, sy, ex, ey, gd.max);
        }

        coord.free();
        lt = lt->next;
    }





    // メモリの解放
    lt = lp;
    while(lt!=NULL) {
        MSGraph<sWord>* mp = (MSGraph<sWord>*)lt->ldat.ptr;
        mp->free();
        delete(mp);
        lt->ldat.ptr = NULL;
        lt->ldat.sz  = 0;
        lt = lt->next;
    }
    del_tList(&lp);


    // コントラスト変換
    //gd.get_minmax();
    print_message("max = %d, min = %d\n", gd.max, gd.min);
    if (gd.max!=gd.min) {
        for(i=0; i<gd.ys*gd.xs; i++) {
            gd.gp[i] = 255*(gd.gp[i]-gd.min)/(gd.max-gd.min);
        }
    }

    // Sun Raster
    writeRasFile("X.im8", gd);

    // JPEG
    jp = MSGraph2JPEGImage(gd);
    gd.free();

    write_jpeg_file("X.jpg", jp, 0);
    jp.free();
}






void   delete_rough(MSGraph<sWord> vp, int size, int mode, int work_color)
{
    int  i, j, k;
    int  px, sp;

    for(j=0; j<vp.ys; j++) {
        px = j*vp.xs;
        sp = -1;
        for(i=0; i<vp.xs; i++) {
            if      (sp==-1 && vp.gp[px+i]!=0) sp = i;
            else if (sp>=0  && vp.gp[px+i]==0) {
                if (i-sp<size) {
                    for (k=sp; k<i; k++) vp.gp[px+k] = 0;
                }
                sp = -1;
            }
        }
    }

    for(i=0; i<vp.xs; i++) {
        sp = -1;
        for(j=0; j<vp.ys; j++) {
            if      (sp==-1 && vp.gp[i+j*vp.xs]!=0) sp = j;
            else if (sp>=0  && vp.gp[i+j*vp.xs]==0) {
                if (j-sp<size) {
                    for (k=sp; k<j; k++) vp.gp[i+k*vp.xs] = 0;
                }
                sp = -1;
            }
        }
    }
}





/**

    ∇の画像に対して，ith から dth刻みで背景領域を探す．



*/
int    background_threshold(MSGraph<sWord> vp, int ith, int dth, int size, float rate, int mode, int work_color)
{
    int   i,  j;
    int   th, sn, pp, ps, hz;
    float sx, sy;


    ps = vp.xs*vp.ys;
    th = ith - dth;
    sx = sy = sn = 0;
    hz = size/2;


    do {
        th += dth;
        sx = sy = sn = 0;

        MSGraph_Paint(vp, 0, 0, 0, th, work_color, mode);
        for(j=0; j<vp.ys; j++) {
            pp = j*vp.xs;
            for(i=0; i<vp.xs; i++) {
                if (vp.gp[i+pp]>=work_color) {
                    sx += i;
                    sy += j;
                    sn++;
                    vp.gp[i+pp] = 0;
                }
            }
        }
        if (sn!=0) {
            sx = sx/sn;
            sy = sy/sn;
        }

    } while (sx<(vp.xs*hz)/size || sx>(vp.xs*(hz+1))/size || 
             sy<(vp.ys*hz)/size || sy>(vp.ys*(hz+1))/size || sn<ps*rate);


    return th;
}







tList*  get_object_region(MSGraph<sWord> gd, int mode, int work_color)
{
    MSGraph<sWord>  vp;
    MSGraph<sWord>* pp;
    int  i, j, num, px;
    tList* lt = NULL;
    tList* lp = NULL;

    num = 0;
    vp = dup_MSGraph(gd);

    for (j=0; j<vp.ys; j++) {
        px = j*vp.xs;
        for (i=0; i<vp.xs; i++) {
            if (vp.gp[i+px]>0 && vp.gp[i+px]<work_color) {
                addPaint_MSGraph(vp, i, j, 1, work_color-1, work_color, mode);
                pp  = new MSGraph<sWord>();
                *pp = cut_object_MSGraph(vp, work_color, work_color*2, 0);
                MSGraph_Paint(vp, i, j, work_color, work_color*2, 0, mode);
                lt = add_tList_node_bystr(lt, ++num, 0, NULL, NULL, (void*)pp, sizeof(MSGraph<sWord>));
                if (lp==NULL) lp = lt;
            }
        }
    }
    vp.free();

    return lp;
}




PCoordinate<int>  find_vertex(MSGraph<sWord> vp, PCoordinate<int> coord)
{

    float a1, a2, b1, b2;
    PCoordinate<int> vertex;
    
    vertex.init(4);


    // for 0
    if (coord.point[0]->x==0) {
        vertex.set(0, 0, 0);
    }
    else {
        if (coord.point[1]->x==vp.xs-1) {
            if (coord.point[0]->x > coord.point[0]->y) vertex.set(0, 0, coord.point[0]->y);
            else                                       vertex.set(0, coord.point[0]->x, 0);
        }
        else {
            a1 = -(float)coord.point[0]->y/coord.point[0]->x;
            b1 =  (float)coord.point[0]->y;
            a2 =  (float)coord.point[1]->y/(vp.xs-1-coord.point[1]->x);
            b2 = -(float)coord.point[1]->x*coord.point[1]->y/(vp.xs-1-coord.point[1]->x);
            if (Xabs(a1-a2)<ZERO_EPS) {
                vertex.free();
                vertex.dim = -1;
                return vertex;
            }
print_message("x = %d, y = %d\n",coord.point[0]->x, coord.point[0]->y);
print_message("a1 = %f, a2 = %f, b1 = %f, b2 = %f\n", a1, a2, b1, b2);
            vertex.set(0, -(int)((b1-b2)/(a1-a2)), (int)((b2*a1-b1*a2)/(a1-a2)));
        }
    }
            

    // for 1
    if (coord.point[1]->x==vp.xs-1) {
        vertex.set(1, vp.xs-1, 0);
    }
    else {
        if (coord.point[2]->x==vp.xs-1) {
            if (vp.xs-1-coord.point[1]->x > coord.point[1]->y) vertex.set(0, vp.xs-1, coord.point[1]->y);
            else                                               vertex.set(0, coord.point[1]->x, 0);
        }
        else {
            a1 =  (float)coord.point[1]->y/(vp.xs-1-coord.point[1]->x);
            b1 = -(float)coord.point[1]->x*coord.point[1]->y/(vp.xs-1-coord.point[1]->x);
            a2 = -(float)(vp.ys-1-coord.point[2]->y)/(vp.xs-1-coord.point[2]->x);
            b2 =  (float)((vp.xs-1)*(vp.ys-1)-coord.point[2]->x*coord.point[2]->y)/(vp.xs-1-coord.point[2]->x);
            if (Xabs(a1-a2)<ZERO_EPS) {
                vertex.free();
                vertex.dim = -1;
                return vertex;
            }
            vertex.set(1, -(int)((b1-b2)/(a1-a2)), (int)((b2*a1-b1*a2)/(a1-a2)));
        }
    }
            

    // for 2
    if (coord.point[2]->x==vp.xs-1) {
        vertex.set(2, vp.xs-1, vp.ys-1);
    }
    else {
        if (coord.point[3]->x==0) {
            if (vp.xs-1-coord.point[2]->x > vp.ys-1-coord.point[2]->y) vertex.set(0, vp.xs-1, coord.point[2]->y);
            else                                                       vertex.set(0, coord.point[2]->x, vp.ys-1);
        }
        else {
            a1 = -(float)(vp.ys-1-coord.point[2]->y)/(vp.xs-1-coord.point[2]->x);
            b1 =  (float)((vp.xs-1)*(vp.ys-1)-coord.point[2]->x*coord.point[2]->y)/(vp.xs-1-coord.point[2]->x);
            a2 =  (float)(vp.ys-1-coord.point[3]->y)/coord.point[3]->x;
            b2 =  (float)coord.point[3]->y;
            if (Xabs(a1-a2)<ZERO_EPS) {
                vertex.free();
                vertex.dim = -1;
                return vertex;
            }
            vertex.set(2, -(int)((b1-b2)/(a1-a2)), (int)((b2*a1-b1*a2)/(a1-a2)));
        }
    }
            

    // for 3
    if (coord.point[3]->x==0) {
        vertex.set(3, 0, vp.ys-1);
    }
    else {
        if (coord.point[0]->x==0) {
            if (coord.point[3]->x > vp.ys-1-coord.point[3]->y) vertex.set(0, 0, coord.point[3]->y);
            else                                               vertex.set(0, coord.point[3]->x, vp.ys-1);
        }
        else {
            a1 =  (float)(vp.ys-1-coord.point[3]->y)/coord.point[3]->x;
            b1 =  (float)coord.point[3]->y;
            a2 = -(float)coord.point[0]->y/coord.point[0]->x;
            b2 =  (float)coord.point[0]->y;
            if (Xabs(a1-a2)<ZERO_EPS) {
                vertex.free();
                vertex.dim = -1;
                return vertex;
            }
            vertex.set(3, -(int)((b1-b2)/(a1-a2)), (int)((b2*a1-b1*a2)/(a1-a2)));
        }
    }


    for(int i=0; i<vertex.dim; i++) {
        print_message("x = %d, y = %d\n", vertex.point[i]->x, vertex.point[i]->y);
    }
    print_message("\n");

            
    return vertex;
}












PCoordinate<int>  detect_cross_point(MSGraph<sWord>* mp, int work_color)
{
    MSGraph<sWord> vp;
    int  i, j, ii, jj, ss, os, nn;
    PCoordinate<int> coord;
    float rt, rate=0.1;
    int itnum = 5;

    coord.init(4);

    vp = dup_MSGraph(*mp);
    nn = 0;
    ss = detect_cross_point_0(vp, i, j, work_color);
    do {
        os = ss;
        ii = i;
        jj = j;
        ss = detect_cross_point_0(vp, i, j, work_color);
        rt = (float)(os-ss)/ss;
        nn++;
    } while (rt>rate && nn<itnum && ss>=0);
    vp.free();

    if (ss<0) {
        coord.free();
        coord.dim = -1;
        return coord;
    }
    coord.set(0, ii, jj);


    vp = dup_MSGraph(*mp);
    nn = 0;
    ss = detect_cross_point_2(vp, i, j, work_color);
    do {
        os = ss;
        ii = i;
        jj = j;
        ss = detect_cross_point_2(vp, i, j, work_color);
        rt = (float)(os-ss)/ss;
        nn++;
    } while (rt>rate && nn<itnum && ss>=0);
    vp.free();

    if (ss<0) {
        vp.free();
        coord.free();
        coord.dim = -1;
        return coord;
    }
    coord.set(2, ii, jj);


    vp = dup_MSGraph(*mp);
    nn = 0;
    ss = detect_cross_point_1(vp, i, j, work_color);
    do {
        os = ss;
        ii = i;
        jj = j;
        ss = detect_cross_point_1(vp, i, j, work_color);
        rt = (float)(ss-os)/ss;
        nn++;
    } while (rt>rate && nn<itnum && ss>=0);
    vp.free();
    coord.set(1, ii, jj);


    vp = dup_MSGraph(*mp);
    nn = 0;
    ss = detect_cross_point_3(vp, i, j, work_color);
    do {
        ii = i;
        jj = j;
        os = ss;
        ss = detect_cross_point_3(vp, i, j, work_color);
        rt = (float)(ss-os)/ss;
        nn++;
    } while (rt>rate && nn<itnum && ss>=0);
    vp.free();
    coord.set(3, ii, jj);


    vp = dup_MSGraph(*mp);
    PCoordinate<int> vertex = find_vertex(vp, coord);
    vp.free();
    coord.free();

    return vertex;
}





int   detect_cross_point_0(MSGraph<sWord>vp, int& x, int& y, int work_color)
{
    int  i, j, ss;

    ss = MSGraph_Paint(vp, 0, 0, 0, 0, work_color, 8);
    if (i==vp.xs-1 || j==vp.ys-1) return -1;

    for (i=0; i<vp.xs; i++) if (vp.gp[i]!=work_color) break;
    for (j=0; j<vp.ys; j++) if (vp.gp[j*vp.xs]!=work_color) break;
    MSGraph_Paint(vp, 0, 0, work_color, work_color, 0, 8);

    if (i>=j && i!=0) j = Min(j, 2*ss/i);
    else if (i<j)     i = Min(i, 2*ss/j);

    MSGraph_Line(vp, i, 0, 0, j, work_color-1);
    
    x = i;
    y = j;
    return ss;
}




int   detect_cross_point_1(MSGraph<sWord>vp, int& x, int& y, int work_color)
{
    int  i, j, ss;

    ss = MSGraph_Paint(vp, vp.xs-1, 0, 0, 0, work_color, 8);

    for (i=vp.xs-1; i>=0; i--) if (vp.gp[i]!=work_color) break;
    for (j=0; j<vp.ys; j++)    if (vp.gp[(j+1)*vp.xs-1]!=work_color) break;
    MSGraph_Paint(vp, vp.xs-1, 0, work_color, work_color, 0, 8);

    if (vp.xs-1-i>=j && vp.xs-1-i!=0) j = Min(j, 2*ss/(vp.xs-1-i));
    else if (vp.xs-1-i<j)             i = Min(i, vp.xs-1-2*ss/j);

    MSGraph_Line(vp, i, 0, vp.xs-1, j, work_color-1);
    
    x = i;
    y = j;
    return ss;
}




int   detect_cross_point_2(MSGraph<sWord>vp, int& x, int& y, int work_color)
{
    int  i, j, ss;

    ss = MSGraph_Paint(vp, vp.xs-1, vp.ys-1, 0, 0, work_color, 8);
    if (i==0 || j==0) return -1;
    
    for (i=vp.xs-1; i>=0; i--) if (vp.gp[i+(vp.ys-1)*vp.xs]!=work_color) break;
    for (j=vp.ys-1; j>=0; j--) if (vp.gp[(j+1)*(vp.xs)-1]!=work_color) break;
    MSGraph_Paint(vp, vp.xs-1, vp.ys-1, work_color, work_color, 0, 8);

    if (vp.xs-1-i>=vp.ys-1-j && vp.xs-1-i!=0) j = Min(j, vp.ys-1-2*ss/(vp.xs-1-i));
    else if (vp.xs-1-i<vp.ys-1-j)             i = Min(i, vp.xs-1-2*ss/(vp.ys-1-j));

    MSGraph_Line(vp, i, vp.ys-1, vp.xs-1, j, work_color-1);
    
    x = i;
    y = j;
    return ss;
}



int   detect_cross_point_3(MSGraph<sWord>vp, int& x, int& y, int work_color)
{
    int  i, j, ss;

    ss = MSGraph_Paint(vp, 0, vp.ys-1, 0, 0, work_color, 8);

    for (i=0; i<vp.xs; i++)    if (vp.gp[i+(vp.ys-1)*vp.xs]!=work_color) break;
    for (j=vp.ys-1; j>=0; j--) if (vp.gp[j*vp.xs]!=work_color) break;
    MSGraph_Paint(vp, 0, vp.ys-1, work_color, work_color, 0, 8);

    if (i>=vp.ys-1-j && i!=0) j = Min(j, vp.ys-1-2*ss/i);
    else if (i<vp.ys-1-j)     i = Min(i, 2*ss/(vp.ys-1-j));

    MSGraph_Line(vp, i, vp.ys-1, 0, j, work_color-1);
    
    x = i;
    y = j;
    return ss;
}





