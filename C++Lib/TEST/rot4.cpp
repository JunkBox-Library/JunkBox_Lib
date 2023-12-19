

#include "common++.h"
#include "Rotation.h"


int main()
{
    using namespace jbxl;
    printf("\n");

    Quaternion<> quatX, quatY;
    Vector<double> vx(1.0, 0.0, 0.0);
    Vector<double> vy(0.0, 1.0, 0.0);
    Vector<double> vz(0.0, 0.0, 1.0);

    quatX.setRotation(PI/2., vx);
    quatY.setRotation(PI/2., vy);

    Quaternion<> quatXY, quatYX;

    quatXY = quatX*quatY;
    quatYX = quatY*quatX;
    
    Vector<double> vXY = quatXY.execRotate(vz);
    Vector<double> vYX = quatYX.execRotate(vz);

    printf ("XY = (%lf, %lf, %lf)\n", vXY.x, vXY.y, vXY.z);
    printf ("YX = (%lf, %lf, %lf)\n", vYX.x, vYX.y, vYX.z);

/*
    Vector<double> eul1(0.8, 1.1, 0.2);
    printf ("vct = (%lf, %lf, %lf)\n", eul1.x, eul1.y, eul1.z);
    printf("\n");

    Quaternion qut1, qut2;
    qut1.setEulerXYZ(eul1);

    printf("--- Quaternion ------------------------- \n");
    printf ("qut = (%lf, %lf, %lf, %lf)\n", qut1.s, qut1.x, qut1.y, qut1.z);
    printf("\n");


    printf("--- Rotation Matrix of qut ------------ \n");
    Matrix<double> m = qut1.getRotMatrix();
    print_Matrix(stdout, m);
    printf("\n");

    Vector<double> vct[2];
    Vector<double> v1 = Quaternion2EulerXYZ(qut1, vct);

    printf("--- Rotation Matrix of qut (Same vct) -- \n");
    printf ("vct = (%lf, %lf, %lf)\n", vct[0].x, vct[0].y, vct[0].z);
    printf ("vct = (%lf, %lf, %lf)\n", vct[1].x, vct[1].y, vct[1].z);
    printf ("vct = (%lf, %lf, %lf)\n", v1.x, v1.y, v1.z);
    printf("\n");
*/
}
