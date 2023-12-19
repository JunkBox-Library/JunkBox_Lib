
#include  "jpeg.h"

using namespace jbxl;

int main()
{
    MSGraph<sWord> vp, xp;

    vp = readRasFile<sWord>("a.ras");

    xp = rotate_MSGraph<sWord>(vp, PI/6.);
    if (xp.gp!=NULL) writeRasFile("X.im8", xp);

    xp = zoom_MSGraph(vp, 3.0);
    if (xp.gp!=NULL) writeRasFile("Y.im8", xp);

    xp = reduce_MSGraph(vp, 2.0);
    if (xp.gp!=NULL) writeRasFile("Z.im8", xp);
}




