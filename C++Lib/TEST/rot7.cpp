

#include "common++.h"
#include "Rotation.h"


int main()
{
    using namespace jbxl;
    printf("\n");

    AffineTrans<> affine;

    affine.setScale(1.0, 2.0, 3.0);
    affine.setShift(4.0, 5.0, 6.0);
    affine.setRotate(PI_DIV6, 1.0, 1.0, 1.0);
    affine.computeMatrix();

    PRINT_MESG("Affine 成分");
    PRINT_MESG("Scale = %lf %lf %lf\n", affine.scale.x, affine.scale.y, affine.scale.z);
    PRINT_MESG("Shift = %lf %lf %lf\n", affine.shift.x, affine.shift.y, affine.shift.z);
    PRINT_MESG("Rotat = %lf %lf %lf %lf\n", affine.rotate.s, affine.rotate.x, affine.rotate.y, affine.rotate.z);
    PRINT_MESG("\n");

    PRINT_MESG("MATRIX\n");
    PRINT_MESG("%lf %lf %lf %lf\n", affine.matrix.element(1,1), affine.matrix.element(1,2), affine.matrix.element(1,3),affine.matrix.element(1,4));
    PRINT_MESG("%lf %lf %lf %lf\n", affine.matrix.element(2,1), affine.matrix.element(2,2), affine.matrix.element(2,3),affine.matrix.element(2,4));
    PRINT_MESG("%lf %lf %lf %lf\n", affine.matrix.element(3,1), affine.matrix.element(3,2), affine.matrix.element(3,3),affine.matrix.element(3,4));
    PRINT_MESG("%lf %lf %lf %lf\n", affine.matrix.element(4,1), affine.matrix.element(4,2), affine.matrix.element(4,3),affine.matrix.element(4,4));
    PRINT_MESG("\n");

    Vector<double> vt(1.0, 1.0, 2.0);
    PRINT_MESG("Vector = %lf %lf %lf\n", vt.x, vt.y, vt.z);
    PRINT_MESG("\n");

    Vector<double> aa = affine.execTrans(vt);
    Vector<double> bb = affine.execMatrixTrans(vt);

    affine.computeComponents();
    Vector<double> xx = affine.execTrans(vt);

    PRINT_MESG("変換\n");
    PRINT_MESG("Comp   aa => %lf %lf %lf\n", aa.x, aa.y, aa.z);
    PRINT_MESG("Comp   xx => %lf %lf %lf\n", xx.x, xx.y, xx.z);
    PRINT_MESG("Matrix bb => %lf %lf %lf\n", bb.x, bb.y, bb.z);
    PRINT_MESG("\n");

    PRINT_MESG("再計算成分\n");
    PRINT_MESG("Scale = %lf %lf %lf\n", affine.scale.x, affine.scale.y, affine.scale.z);
    PRINT_MESG("Shift = %lf %lf %lf\n", affine.shift.x, affine.shift.y, affine.shift.z);
    PRINT_MESG("Rotat = %lf %lf %lf %lf\n", affine.rotate.s, affine.rotate.x, affine.rotate.y, affine.rotate.z);
    PRINT_MESG("\n");

    AffineTrans<> rev = affine.getInvAffine();

    PRINT_MESG("逆MATRIX\n");
    PRINT_MESG("%lf %lf %lf %lf\n", rev.matrix.element(1,1), rev.matrix.element(1,2), rev.matrix.element(1,3),rev.matrix.element(1,4));
    PRINT_MESG("%lf %lf %lf %lf\n", rev.matrix.element(2,1), rev.matrix.element(2,2), rev.matrix.element(2,3),rev.matrix.element(2,4));
    PRINT_MESG("%lf %lf %lf %lf\n", rev.matrix.element(3,1), rev.matrix.element(3,2), rev.matrix.element(3,3),rev.matrix.element(3,4));
    PRINT_MESG("%lf %lf %lf %lf\n", rev.matrix.element(4,1), rev.matrix.element(4,2), rev.matrix.element(4,3),rev.matrix.element(4,4));
    PRINT_MESG("\n");


    Vector<double> dd = rev.execMatrixTrans(aa);
    Vector<double> cc = rev.execTrans(aa);
    PRINT_MESG("逆変換\n");
    PRINT_MESG("Comp   cc => %lf %lf %lf\n", cc.x, cc.y, cc.z);
    PRINT_MESG("Matrix dd => %lf %lf %lf\n", dd.x, dd.y, dd.z);
    PRINT_MESG("\n");

    PRINT_MESG("再計算成分\n");
    PRINT_MESG("Scale = %lf %lf %lf\n", rev.scale.x, rev.scale.y, rev.scale.z);
    PRINT_MESG("Shift = %lf %lf %lf\n", rev.shift.x, rev.shift.y, rev.shift.z);
    PRINT_MESG("Rotat = %lf %lf %lf %lf\n", rev.rotate.s, rev.rotate.x, rev.rotate.y, rev.rotate.z);
    PRINT_MESG("\n");

    affine.free();
}
