

#include "BVHTool.h"


using namespace jbxl;



int main()
{
    CBVHTool* bvh = new CBVHTool();

    bvh->readFile("PSI_Pose.bvh");

//  bvh->printBVH(stdout);


    for (int i=0; i<bvh->joint_num; i++) {
        printf("%2d ", bvh->channel_num[i]);
    }
    printf("\n");

    for (int i=0; i<bvh->joint_num; i++) {
        printf("%2d ", bvh->channel_idx[i]);
    }
    printf("\n");

/*
    printf("\n==> %s\n", bvh->flex.buf);
*/
    printf("joints ==> %d\n", bvh->joint_num);

    printf("\n");
    Vector<>* vect = bvh->getPosOffset();
    for (int i=0; i<bvh->joint_num; i++) {
        printf("%d  (%f, %f, %f)\n", i, vect[i].x, vect[i].y, vect[i].z);
    }

    printf("\n");
    vect = bvh->getPosData(29); 
    for (int i=0; i<bvh->joint_num; i++) {
        printf("%d  (%f, %f, %f)\n", i, vect[i].x, vect[i].y, vect[i].z);
    }


    printf("\n");
    Quaternion* quat = bvh->getQuaternion(1);
    for (int j=0; j<bvh->joint_num; j++) {
        printf("%2d: (%f, %f, %f) %f \n", j, quat[j].x, quat[j].y, quat[j].z, quat[j].s);
    }

    printf("\n");
    for (int j=0; j<bvh->joint_num; j++) {
        printf("%2d: %s\n", j, bvh->joint_name[j].buf);
    }


    delete(bvh);
}
