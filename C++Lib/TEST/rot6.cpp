

#include "common++.h"
#include "Rotation.h"


int main()
{
    using namespace jbxl;
    printf("\n");


    // Rotation Matrix
    Vector<double> vct(0.0, 1.0, 1.0);
    double  th = PI_DIV2;

    Quaternion<> qut(th, vct);

    Matrix<double> mtx = qut.getRotMatrix();

    print_Matrix(stdout, mtx);
    printf("\n");

    Vector<double> pnt(1.0, 0.0, 0.0);
    Vector<double> ppt = mtx*pnt;

    printf(" (%lf,%lf,%lf) -> (%lf,%lf,%lf)\n", pnt.x, pnt.y, pnt.z, ppt.x, ppt.y, ppt.z);
    printf("----------------\n\n");


    //
    vct.set(1.0, 2.0, -3.0);
    qut.setRotation(PI_DIV4, vct);
    printf("QUAT = (%lf, %lf, %lf) %lf\n", qut.x, qut.y, qut.z, qut.s);
    printf("----------------\n");

    Vector<double> vt[2];
    Vector<double> eul = qut.getExtEulerZXY(vt);


    printf("----------------\n");
    printf("Euler1 = %lf %lf %lf\n", vt[0].x, vt[0].y, vt[0].z);
    printf("Euler2 = %lf %lf %lf\n", vt[1].x, vt[1].y, vt[1].z);
    printf("----------------\n");

    qut.setExtEulerZXY(vt[1]);
    printf("QUAT = (%lf, %lf, %lf) %lf\n", qut.x, qut.y, qut.z, qut.s);
    printf("----------------\n");







}
