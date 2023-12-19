

#include "common++.h"
#include "Rotation.h"


int main()
{
    using namespace jbxl;
    printf("\n");

    Vector<double> eul1(1.0, 2.0, 1.0);
    Vector<double> eul2 = - eul1;

/*
    Vector4<double> x, y;

    y = x = eul1;

    x.t = 100;
    printf("x4 = %lf %lf %lf %lf\n", x.x, x.y, x.z, x.t);
    printf("y4 = %lf %lf %lf %lf\n", y.x, y.y, y.z, y.t);
    y.init();
    printf("y4 = %lf %lf %lf %lf\n", y.x, y.y, y.z, y.t);
    printf("\n");

    printf("vct1 = (%lf, %lf, %lf)\n", eul1.x, eul1.y, eul1.z);
    printf("vct2 = (%lf, %lf, %lf)\n", eul2.x, eul2.y, eul2.z);
    printf("\n");
*/


    Matrix<double> mt1 = ExtEulerXYZ2RotMatrix(eul1);
    Matrix<double> mt2 = ExtEulerZYX2RotMatrix(eul2);
    Matrix<double> mt3 = mt2*mt1;

    printf("--- Rotation by vct 1 ------------------\n");
    print_Matrix(stdout, mt1);
    printf("\n");
    printf("--- Rotation by vct 2 ------------------\n");
    print_Matrix(stdout, mt2);
    printf("\n");
    printf("--- Rotation by vct 1->2 Unit Matrix ---\n");
    print_Matrix(stdout, mt3);
    printf("\n");
}
