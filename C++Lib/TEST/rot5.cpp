

#include "common++.h"
#include "Rotation.h"


int main()
{
    using namespace jbxl;
    printf("\n");

    Vector<double> vct1(0.5, 1.1, 0.2);
    //Vector<double> vct2(0.9, 2.0, 1.0);
    Vector<double> vct2(1.0, 2.2, 0.4);

    printf ("vct1 = (%lf, %lf, %lf)\n", vct1.x, vct1.y, vct1.z);
    printf ("vct2 = (%lf, %lf, %lf)\n", vct2.x, vct2.y, vct2.z);
    printf("\n");

    Quaternion<> qut = V2VQuaternion(vct1, vct2);

    printf("--- Quaternion ------------------------- \n");
    printf ("qut = (%lf, (%lf, %lf, %lf), n=%lf)\n", qut.s, qut.x, qut.y, qut.z, qut.n);
    printf("\n");

    Vector<double> vct3 = qut.execRotation(vct1);

    printf("--- Rotation of vct1 ------------ \n");
    printf ("vct3 = (%lf, %lf, %lf)\n", vct3.x, vct3.y, vct3.z);

    vct2 = vct2/vct2.n*vct1.n;
    printf ("vct2 = (%lf, %lf, %lf)\n", vct2.x, vct2.y, vct2.z);
    printf("\n");
}
