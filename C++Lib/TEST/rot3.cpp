

#include "common++.h"
#include "Rotation.h"


int main()
{
    using namespace jbxl;
    printf("\n");


    Vector<double> eul1(1.0, 1.0, 0.5);
    printf ("vct1 = (%lf, %lf, %lf)\n", eul1.x, eul1.y, eul1.z);
    printf("\n");

    Matrix<double> mt1 = ExtEulerZYX2RotMatrix(eul1);

    Vector<double> vct[2];
    Vector<double> v = RotMatrix2ExtEulerZYX(mt1, vct);

    printf("--- ZYX ans -----------------------\n");
    printf ("vct1 = (%lf, %lf, %lf)\n", vct[0].x, vct[0].y, vct[0].z);
    printf ("vct1 = (%lf, %lf, %lf)\n", vct[1].x, vct[1].y, vct[1].z);
    printf ("vct1 = (%lf, %lf, %lf)\n", v.x, v.y, v.z);
    printf("\n");


    printf("--- ZYX mat1: Same mat2 and mat3 --\n");
    print_Matrix(stdout, mt1);
    printf("\n");

    printf("--- ZYX mat2: Same mat1 and mat3 --\n");
    Matrix<double> mt2 = ExtEulerZYX2RotMatrix(vct[0]);
    print_Matrix(stdout, mt2);
    printf("\n");

    printf("--- ZYX mat3: Same mat1 and mat2 --\n");
    Matrix<double> mt3 = ExtEulerZYX2RotMatrix(vct[1]);
    print_Matrix(stdout, mt3);
    printf("\n");
}
