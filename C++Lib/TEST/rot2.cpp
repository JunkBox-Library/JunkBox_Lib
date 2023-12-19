

#include "common++.h"
#include "Rotation.h"


int main()
{
    printf("\n");

    using namespace jbxl;

    Vector<double> eul1(1.0, 2.0, 0.5);
    Vector<double> eul2(-2.141593, 1.141593, -2.641593);    // other ans.

    Matrix<double> mt1 = ExtEulerXYZ2RotMatrix(eul1);
    Matrix<double> mt2 = ExtEulerXYZ2RotMatrix(eul2);
    mt2 = mt2 - mt1;

    printf("--- Zero Matrix ---------------------\n");
    print_Matrix(stdout, mt2);
    printf("\n\n\n");


    Vector<double> eul3(1.0, -PI/2., 0.5);
    printf("--- PI/2 Vector ---------------------\n");
    printf ("vct3 = (%lf, %lf, %lf)\n", eul3.x, eul3.y, eul3.z);
    printf("\n");

    Matrix<double> mt3 = ExtEulerXYZ2RotMatrix(eul3);
    Vector<double> vct[2];
    Vector<double> v = RotMatrix2ExtEulerXYZ(mt3, vct);

    printf("--- PI/2 Vector Answers -------------\n");
    printf ("vct3 = (%lf, %lf, %lf)\n", vct[0].x, vct[0].y, vct[0].z);
    printf ("vct3 = (%lf, %lf, %lf)\n", vct[1].x, vct[1].y, vct[1].z);
    printf ("vct3 = (%lf, %lf, %lf)\n", v.x, v.y, v.z);
    printf("\n");

    Matrix<double> mt4 = ExtEulerXYZ2RotMatrix(vct[0]);
    Matrix<double> mt5 = ExtEulerXYZ2RotMatrix(vct[1]);

    printf("--- mt3: Same mt4 and mt5 -----------\n");
    print_Matrix(stdout, mt3);
    printf("\n");

    printf("--- mt4: Same mt3 and mt5 -----------\n");
    print_Matrix(stdout, mt4);
    printf("\n");

    printf("--- mt5: Same mt3 and mt4 -----------\n");
    print_Matrix(stdout, mt5);
    printf("\n");

}
