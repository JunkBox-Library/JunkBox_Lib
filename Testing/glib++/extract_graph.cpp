

#include  "extract_graph.h"




tList*  extract_graph(JPEGImage jp)
{
    MSGraph<sWord> gd = JPEGImage2MSGraph<sWord>(jp);

    tList* lv = vertex_graph(gd);       // PCoordinate
    tList* lc = cut_graph(lv, gd);      // MSGraph
    tList* lp = rotate_graph(lc, lv);   // MSGraph
    tList* lj = graph2jpeg(lp);         // JPEGImage

    del_tList_object<PCoordinate<int> >(&lv);
    del_tList_object<MSGraph<sWord> >(&lc);
    del_tList_object<MSGraph<sWord> >(&lp);
    gd.free();

    return lj;
}





////////////////////////////////////////////////////////////////////////////////////////////
// ROTATE_GRAPH

tList*  rotate_graph(tList* lc, tList* lv)
{
    int  xx[4], yy[4];
    tList* lt = NULL;
    tList* lj = NULL;

    while (lc!=NULL && lv!=NULL) {
        MSGraph<sWord>*   vp = (MSGraph<sWord>*)lc->ldat.ptr;
        PCoordinate<int>* pc = (PCoordinate<int>*)lv->ldat.ptr;

        // 回転
        int    xys = (int)sqrt(vp->xs*vp->xs + vp->ys*vp->ys) + 1;
        double cst = get_grad_graph(*pc);
        double snt = sqrt(1.0 - cst*cst);
        if (cst<0.0) {
            cst = -cst;
            snt = -snt;
        }
        MSGraph<sWord> xp = rotate_MSGraph<sWord>(*vp, xys, xys, cst, snt);

        // 境界線の描画
        for (int i=0; i<4; i++) {
            xx[i] = pc->point[i]->x - vp->rbound.xmin;
            yy[i] = pc->point[i]->y - vp->rbound.ymin;
            rotate_point(xx[i], yy[i], (vp->xs-1.)/2., (vp->ys-1.)/2., (xys-1.)/2., (xys-1.)/2., cst, snt);
        }
        for (int i=0; i<4; i++) {
            int j = (i+1)%4;
            MSGraph_Line(xp, xx[i], yy[i], xx[j], yy[j], WORK_COLOR+1);
        }
        MSGraph_Paint(xp, 0, 0, 0, WORK_COLOR-1, WORK_COLOR, 4);

        // 余分な部分を塗りつぶす
        int ps = xp.xs*xp.ys;
        for (int i=0; i<ps; i++) {
            if (xp.gp[i]==WORK_COLOR) {
                xp.gp[i] = xp.gp[i+ps] = xp.gp[i+ps*2] = 0;
            }
            else if (xp.gp[i]==WORK_COLOR+1) {
                xp.gp[i] = xp.gp[i+ps] = xp.gp[i+ps*2] = MAX_GRAY_SCALE;
            }
        }

        // 領域の切り出し
        MSGraph<sWord> vx = cut_object_MSGraph<sWord>(xp, 1, WORK_COLOR, 0);
        MSGraph_Paint3D(vx, 0, 0, 0, 0, 0, MAX_GRAY_SCALE, 4);
        MSGraph_Paint3D(vx, vx.xs-1, 0, 0, 0, 0, MAX_GRAY_SCALE, 4);
        MSGraph_Paint3D(vx, 0, vx.ys-1, 0, 0, 0, MAX_GRAY_SCALE, 4);
        MSGraph_Paint3D(vx, vx.xs-1, vx.ys-1, 0, 0, 0, MAX_GRAY_SCALE, 4);
        xp.free();

        //
        lt = add_tList_object<MSGraph<sWord> >(lt, vx);
        if (lj==NULL) lj = lt;

        lc = lc->next;
        lv = lv->next;
    }

    return lj;
}




/**
double   get_grad_graph(PCoordinate<int> pc) 

    正なら左（反時計回り）に回転，負なら右（時計回り）に回転させる

*/
double   get_grad_graph(PCoordinate<int> pc) 
{
    double csr = 0.0;
    double csl = 0.0;
    Vector<int> vt, vu;


    vu = *pc.point[1] - *pc.point[0];
    vt.set(0, -1);
    csr += (vu*vt)/vu.n;

    vu = *pc.point[2] - *pc.point[1];
    vt.set(1, 0);
    csr += (vu*vt)/vu.n;

    vu = *pc.point[3] - *pc.point[2];
    vt.set(0, 1);
    csr += (vu*vt)/vu.n;

    vu = *pc.point[0] - *pc.point[3];
    vt.set(-1, 0);
    csr += (vu*vt)/vu.n;

    
    vu = *pc.point[0] - *pc.point[1];
    vt.set(-1, 0);
    csl += (vu*vt)/vu.n;

    vu = *pc.point[1] - *pc.point[2];
    vt.set(0, -1);
    csl += (vu*vt)/vu.n;

    vu = *pc.point[2] - *pc.point[3];
    vt.set(1, 0);
    csl += (vu*vt)/vu.n;

    vu = *pc.point[3] - *pc.point[0];
    vt.set(0, 1);
    csl += (vu*vt)/vu.n;


    DEBUG_MODE print_message("rotation : csl = %f, csr = %f\n", csl/4., csr/4.);

    if (Xabs(csl)<Xabs(csr)) return csr/4.;
    else return csl/4.;
}





////////////////////////////////////////////////////////////////////////////////////////////
// CUT_GRAPH 

tList*  cut_graph(tList* lv, MSGraph<sWord> gd)
{
    int  sx, sy, ex, ey;
    tList* lt = NULL;
    tList* lp = NULL;
    

    while (lv!=NULL) {
        PCoordinate<int>* pc = (PCoordinate<int>*)lv->ldat.ptr;
        sx = ex = pc->point[0]->x;
        sy = ey = pc->point[0]->y;
        for (int i=1; i<pc->dim; i++) {
            sx = Min(sx, pc->point[i]->x) - 1;
            ex = Max(ex, pc->point[i]->x) + 1;
            sy = Min(sy, pc->point[i]->y) - 1;
            ey = Max(ey, pc->point[i]->y) + 1;
        }

        MSGraph<sWord> vp = grab_MSGraph(gd, sx, sy, ex, ey, 0, 2);
        lt = add_tList_object<MSGraph<sWord> >(lt, vp);
        if (lp==NULL) lp = lt;

        lv = lv->next;
    }

    return lp;
}





////////////////////////////////////////////////////////////////////////////////////////////
// GRAPH2JPEG

tList*  graph2jpeg(tList* lc)
{
    tList* lp = NULL;
    tList* lj = NULL;

    while (lc!=NULL) {
        MSGraph<sWord>* vp = (MSGraph<sWord>*)lc->ldat.ptr;

        JPEGImage jp = MSGraph2JPEGImage(*vp);
        lp = add_tList_object<JPEGImage>(lp, jp);
        if (lj==NULL) lj = lp;

        lc = lc->next;
    }

    return lj;
}







////////////////////////////////////////////////////////////////////////////////////////////
// VERTEX_GRAPH

tList*  vertex_graph(MSGraph<sWord> jp)
{
    MSGraph<sWord> gd;
    int ps = jp.xs*jp.ys;


    // グレイスケール化
    gd.set(jp.xs, jp.ys);
    gd.min = gd.max = jp.gp[0];
    for(int i=0; i<gd.ys*gd.xs; i++) {
        gd.gp[i] = (jp.gp[i] + jp.gp[i+ps] + jp.gp[i+ps*2]);
        gd.max = Max(gd.max, gd.gp[i]);
        gd.min = Min(gd.min, gd.gp[i]);
    }


    // エッジ抽出
    MSGraph<sWord> vp = Nabra<sWord>(gd);

    // 背景一次処理  10, +2, 1/3, 0.3
    int th = background_threshold(vp, 10, 2, 3, 0.3, 4, WORK_COLOR);

    MSGraph_Paint(vp, 0, 0, 0, th, WORK_COLOR, 4);
    for(int i=0; i<vp.ys*vp.xs; i++) {
        if (vp.gp[i]>=WORK_COLOR) gd.gp[i] = 0;
    }
    vp.free();


    // ノイズ除去  500, 200
    set_around_MSGraph(gd, 0, EDGE_SIZE);
    delete_noise_MSGraph(gd, 500, 4, WORK_COLOR);
    delete_rough(gd, 20, 4, WORK_COLOR);

    //JPEGImage jg = MSGraph2JPEGImage<sWord>(gd);
    //write_jpeg_file("XX.jpg", jg);
    //jg.free();


    // 領域分割
    tList* lp = get_object_region(gd, 8, WORK_COLOR);   // MSGraph<sWord>


    DEBUG_MODE {
        // 中間データ出力
        //char fname[128];
        tList* lt = lp;
        while(lt!=NULL) {
            MSGraph<sWord>* mp = (MSGraph<sWord>*)lt->ldat.ptr;
            print_message("VERTEX_GRAPH: (%d, %d)-(%d, %d)\n", mp->rbound.xmin, mp->rbound.ymin, mp->rbound.xmax, mp->rbound.ymax);
            //snprintf(fname, 128, "%d.im8", lt->ldat.id);
            //writeRasFile(fname, *mp);
            lt = lt->next;
        }
        print_message("\n");
    }


    tList* lv = get_region_vertex(lp, 10, 2);           // PCoordinate<int>
    del_tList_object<MSGraph<sWord> >(&lp);

    return lv;
}





/**
int    background_threshold(MSGraph<sWord> vp, int ith, int dth, int size, float rate, int mode, int work_color)

    |∇| の画像に対して，ith から dth刻みで背景領域を探す．

    パラメータ  ith     閾値の初期値
                dth     閾値の刻み幅
                size    背景の重心の範囲 (1/size)．背景は均等にばらついているものとする．奇数にする．
                rate    全領域に対する背景の割合の最小値
*/
int    background_threshold(MSGraph<sWord> vp, int ith, int dth, int size, float rate, int mode, int work_color)
{
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
        for(int j=0; j<vp.ys; j++) {
            pp = j*vp.xs;
            for(int i=0; i<vp.xs; i++) {
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





/**
void   delete_rough(MSGraph<sWord> vp, int size, int mode, int work_color)

    縁の出っ張りを削除する．
    
    パラメータ size  削除する出っ張りのサイズ（長さ）

*/
void   delete_rough(MSGraph<sWord> vp, int size, int mode, int work_color)
{
    int  px, sp;

    for(int j=0; j<vp.ys; j++) {
        px = j*vp.xs;
        sp = -1;
        for(int i=0; i<vp.xs; i++) {
            if      (sp==-1 && vp.gp[px+i]!=0) sp = i;
            else if (sp>=0  && vp.gp[px+i]==0) {
                if (i-sp<size) {
                    for (int k=sp; k<i; k++) vp.gp[px+k] = 0;
                }
                sp = -1;
            }
        }
    }

    for(int i=0; i<vp.xs; i++) {
        sp = -1;
        for(int j=0; j<vp.ys; j++) {
            if      (sp==-1 && vp.gp[i+j*vp.xs]!=0) sp = j;
            else if (sp>=0  && vp.gp[i+j*vp.xs]==0) {
                if (j-sp<size) {
                    for (int k=sp; k<j; k++) vp.gp[i+k*vp.xs] = 0;
                }
                sp = -1;
            }
        }
    }
}





/**
tList*  get_object_region(MSGraph<sWord> gd, int mode, int work_color)

    画像全体から領域（四角形）を抜き出しリストにする．

*/
tList*  get_object_region(MSGraph<sWord> gd, int mode, int work_color)
{
    int  px;
    tList* lt = NULL;
    tList* lp = NULL;

    MSGraph<sWord> vp = dup_MSGraph(gd);

    for (int j=0; j<vp.ys; j++) {
        px = j*vp.xs;
        for (int i=0; i<vp.xs; i++) {
            if (vp.gp[i+px]>0 && vp.gp[i+px]<work_color) {
                addPaint_MSGraph(vp, i, j, 1, work_color-1, work_color, mode);

                MSGraph<sWord> vs = cut_object_MSGraph(vp, work_color, work_color*2, 0);
                lt = add_tList_object<MSGraph<sWord> >(lt, vs);
                if (lp==NULL) lp = lt;
                MSGraph_Paint(vp, i, j, work_color, work_color*2, 0, mode);
            }
        }
    }
    vp.free();

    return lp;
}





/**
tList*  get_region_vertex(tList* lt, int err, int sz)

    パラメータ  err  領域の角で誤差と見なすドット数
                sz   エラー時の画像の縮退のドット数

*/
tList*  get_region_vertex(tList* lt, int err, int sz)
{
    int  num = 0;
    tList* lp = NULL;
    tList* lv = NULL;


    while(lt!=NULL) {
        MSGraph<sWord>*  mp = (MSGraph<sWord>*)lt->ldat.ptr;
        PCoordinate<int> coord = detect_cross_point(mp, err, WORK_COLOR);
        while (coord.dim<0) {
            MSGraph<sWord> xp = grab_MSGraph(*mp, sz, sz, mp->xs-sz-1, mp->ys-sz-1);    
            mp->free();
            *mp = xp;
            coord = detect_cross_point(mp, err, WORK_COLOR);
        }

        for (int k=0; k<coord.dim; k++) {
            coord.point[k]->x += mp->rbound.xmin;
            coord.point[k]->y += mp->rbound.ymin;
        }

        lp = add_tList_object<PCoordinate<int> >(lp, coord);
        if (lv==NULL) lv = lp;

        lt = lt->next;
    }

    return lv;
}





/**
PCoordinate<int>  detect_cross_point(MSGraph<sWord>* mp, int err, int work_color)

    領域の頂点を求める．

    パラメータ err 誤差と見なすドット数

*/
PCoordinate<int>  detect_cross_point(MSGraph<sWord>* mp, int err, int work_color)
{
    int  i, j, ss;
    PCoordinate<int> coord;

    MSGraph<sWord> vp = dup_MSGraph(*mp);
    coord.init(4);

    ss = detect_cross_point_0(vp, i, j, err, work_color);
    if (ss<0) {
        vp.free();
        coord.free();
        coord.dim = -1;
        return coord;
    }
    coord.set(0, i, j);

    ss = detect_cross_point_1(vp, i, j, err, work_color);
    if (ss<0) {
        vp.free();
        coord.free();
        coord.dim = -1;
        return coord;
    }
    coord.set(1, i, j);

    ss = detect_cross_point_2(vp, i, j, err, work_color);
    if (ss<0) {
        vp.free();
        coord.free();
        coord.dim = -1;
        return coord;
    }
    coord.set(2, i, j);

    ss = detect_cross_point_3(vp, i, j, err, work_color);
    if (ss<0) {
        vp.free();
        coord.free();
        coord.dim = -1;
        return coord;
    }
    coord.set(3, i, j);


    DEBUG_MODE {
        for (i=0; i<coord.dim; i++) {
            print_message("DETECT_CROSS_POINT: %d: (%d, %d)\n", i, coord.point[i]->x, coord.point[i]->y);
        }
        print_message("\n");
    }


    PCoordinate<int> vertex = find_vertex(vp, coord);

    vp.free();
    coord.free();

    return vertex;
}




int   detect_cross_point_0(MSGraph<sWord>vp, int& i, int& j, int err, int work_color)
{
    int  ss;

    ss = MSGraph_Paint(vp, 0, 0, 0, 0, work_color, 8);
    if (ss!=0 && ss<=err*err/2) return -1;

    for (i=0; i<vp.xs; i++) if (vp.gp[i]!=work_color) break;
    for (j=0; j<vp.ys; j++) if (vp.gp[j*vp.xs]!=work_color) break;
    if (i==vp.xs-1 || j==vp.ys-1) return -1;

    ss = MSGraph_Paint(vp, 0, 0, work_color, work_color, 0, 8);

    if (i>=j && i!=0) j = Min(j, 2*ss/i);
    else if (i<j)     i = Min(i, 2*ss/j);
    
    return ss;
}




int   detect_cross_point_1(MSGraph<sWord>vp, int& i, int& j, int err, int work_color)
{
    int  ss;

    ss = MSGraph_Paint(vp, vp.xs-1, 0, 0, 0, work_color, 8);
    if (ss!=0 && ss<=err*err/2) return -1;

    for (i=vp.xs-1; i>=0; i--) if (vp.gp[i]!=work_color) break;
    for (j=0; j<vp.ys; j++)    if (vp.gp[(j+1)*vp.xs-1]!=work_color) break;

    ss = MSGraph_Paint(vp, vp.xs-1, 0, work_color, work_color, 0, 8);

    if (vp.xs-1-i>=j && vp.xs-1-i!=0) j = Min(j, 2*ss/(vp.xs-1-i));
    else if (vp.xs-1-i<j)             i = Max(i, vp.xs-1-2*ss/j);
    
    return ss;
}




int   detect_cross_point_2(MSGraph<sWord>vp, int& i, int& j, int err, int work_color)
{
    int  ss;

    ss = MSGraph_Paint(vp, vp.xs-1, vp.ys-1, 0, 0, work_color, 8);
    if (ss!=0 && ss<=err*err/2) return -1;
    
    for (i=vp.xs-1; i>=0; i--) if (vp.gp[i+(vp.ys-1)*vp.xs]!=work_color) break;
    for (j=vp.ys-1; j>=0; j--) if (vp.gp[(j+1)*(vp.xs)-1]!=work_color) break;
    if (i==0 || j==0) return -1;

    ss = MSGraph_Paint(vp, vp.xs-1, vp.ys-1, work_color, work_color, 0, 8);

    if (vp.xs-1-i>=vp.ys-1-j && vp.xs-1-i!=0) j = Max(j, vp.ys-1-2*ss/(vp.xs-1-i));
    else if (vp.xs-1-i<vp.ys-1-j)             i = Max(i, vp.xs-1-2*ss/(vp.ys-1-j));
    
    return ss;
}




int   detect_cross_point_3(MSGraph<sWord>vp, int& i, int& j, int err, int work_color)
{
    int  ss;

    ss = MSGraph_Paint(vp, 0, vp.ys-1, 0, 0, work_color, 8);
    if (ss!=0 && ss<=err*err/2) return -1;

    for (i=0; i<vp.xs; i++)    if (vp.gp[i+(vp.ys-1)*vp.xs]!=work_color) break;
    for (j=vp.ys-1; j>=0; j--) if (vp.gp[j*vp.xs]!=work_color) break;

    ss = MSGraph_Paint(vp, 0, vp.ys-1, work_color, work_color, 0, 8);

    if (i>=vp.ys-1-j && i!=0) j = Max(j, vp.ys-1-2*ss/i);
    else if (i<vp.ys-1-j)     i = Min(i, 2*ss/(vp.ys-1-j));
    
    return ss;
}





/**
PCoordinate<int>  find_vertex(MSGraph<sWord> vp, PCoordinate<int> coord)

    領域と範囲（背景）との交点 coordから，矩形領域の頂点を計算する．

    パラメータなし
*/
PCoordinate<int>  find_vertex(MSGraph<sWord> vp, PCoordinate<int> coord)
{
    float a1, a2, b1, b2;
    PCoordinate<int> vertex;
    
    vertex.init(4);


    // for 0->1->2
    if (coord.point[0]->x==0 || coord.point[0]->y==0) {                 // 0->1 は横線
        if (coord.point[1]->x==vp.xs-1 || coord.point[1]->y==0) {           // 1->2 は縦線
            vertex.set(0, vp.xs-1, 0);
        }
        else {                                                              // 1->2 は斜線
            a2 =  (float)coord.point[1]->y/(vp.xs-1-coord.point[1]->x);
            b2 = -(float)coord.point[1]->x*coord.point[1]->y/(vp.xs-1-coord.point[1]->x);
            vertex.set(0, -(int)(b2/a2), 0);
        }
    }
    else {                                                              // 0->1 は斜線
        a1 = -(float)coord.point[0]->y/coord.point[0]->x;
        b1 =  (float)coord.point[0]->y;

        if (coord.point[1]->x==vp.xs-1 || coord.point[1]->y==0) {           // 1->2 は縦線
            vertex.set(0, vp.xs-1, (int)(a1*(vp.xs-1)+b1));
        }
        else {                                                              // 1->2 は斜線
            a2 =  (float)coord.point[1]->y/(vp.xs-1-coord.point[1]->x); 
            b2 = -(float)coord.point[1]->x*coord.point[1]->y/(vp.xs-1-coord.point[1]->x);
            if (Xabs(a1-a2)<ZERO_EPS) {
                vertex.free();
                vertex.dim = -1;
                return vertex;
            }
            vertex.set(0, -(int)((b1-b2)/(a1-a2)), (int)((b2*a1-b1*a2)/(a1-a2)));
        }
    }
            

    // for 1->2->3
    if (coord.point[1]->x==vp.xs-1 || coord.point[1]->y==0) {           // 1->2 は縦線
        if (coord.point[2]->x==vp.xs-1 || coord.point[2]->y==vp.ys-1) {     // 2->3 は横線
            vertex.set(1, vp.xs-1, vp.ys-1);
        }
        else {                                                              // 2->3 は斜線
            a2 = -(float)(vp.ys-1-coord.point[2]->y)/(vp.xs-1-coord.point[2]->x);
            b2 =  (float)((vp.xs-1)*(vp.ys-1)-coord.point[2]->x*coord.point[2]->y)/(vp.xs-1-coord.point[2]->x);
            vertex.set(1, vp.xs-1, (int)(a2*(vp.xs-1)+b2));
        }
    }
    else {                                                              // 1->2 は斜線
        a1 =  (float)coord.point[1]->y/(vp.xs-1-coord.point[1]->x);
        b1 = -(float)coord.point[1]->x*coord.point[1]->y/(vp.xs-1-coord.point[1]->x);

        if (coord.point[2]->x==vp.xs-1 || coord.point[2]->y==vp.ys-1) {     // 2->3 は横線
            vertex.set(1, (int)((vp.ys-1-b1)/a1), vp.ys-1);
        }
        else {                                                              // 2->3 は斜線
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
            

    // for 2->3->0
    if (coord.point[2]->x==vp.xs-1 || coord.point[2]->y==vp.ys-1) {     // 2->3 は横線
        if (coord.point[3]->x==0 || coord.point[3]->y==vp.ys-1) {           // 3->0 は縦線
            vertex.set(2, 0, vp.ys-1);
        }
        else {                                                              // 3->0 は斜線
            a2 =  (float)(vp.ys-1-coord.point[3]->y)/coord.point[3]->x;
            b2 =  (float)coord.point[3]->y;
            vertex.set(2, (int)((vp.ys-1-b2)/a2), vp.ys-1);
        }
    }
    else {                                                              // 2->3 は斜線
        a1 = -(float)(vp.ys-1-coord.point[2]->y)/(vp.xs-1-coord.point[2]->x);
        b1 =  (float)((vp.xs-1)*(vp.ys-1)-coord.point[2]->x*coord.point[2]->y)/(vp.xs-1-coord.point[2]->x);

        if (coord.point[3]->x==0 || coord.point[3]->y==vp.ys-1) {           // 3->0 は縦線
            vertex.set(2, 0, (int)b1);
        }
        else {                                                              // 3->0 は斜線
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
            

    // for 3->0->1
    if (coord.point[3]->x==0 || coord.point[3]->y==vp.ys-1) {           // 3->0 は縦線
        if (coord.point[0]->x==0 || coord.point[0]->y==0) {                 // 0->1 は横線
            vertex.set(3, 0, 0);
        }
        else {                                                              // 0->1 は斜線
            a2 = -(float)coord.point[0]->y/coord.point[0]->x;
            b2 =  (float)coord.point[0]->y;
            vertex.set(3, 0, (int)b2);
        }
    }
    else {                                                              // 3->0 は斜線
        a1 =  (float)(vp.ys-1-coord.point[3]->y)/coord.point[3]->x;
        b1 =  (float)coord.point[3]->y;

        if (coord.point[0]->x==0 || coord.point[0]->y==0) {                 // 0->1 は横線
            vertex.set(3, 0, -(int)(b1/a1));
        }
        else {                                                              // 0->1 は斜線
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


    DEBUG_MODE {
        for(int i=0; i<vertex.dim; i++) {
            print_message("FIND_VERTEX       : %d: (%d, %d)\n", i, vertex.point[i]->x, vertex.point[i]->y);
        }
        print_message("\n");
    }

    return vertex;
}




