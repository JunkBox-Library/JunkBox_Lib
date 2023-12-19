
#include "xtools.h"
#include "bvh_tool.h"





#define  BVH_FL "PSI_Pose.bvh"


int main()
{
    BVHData* bvh = bvh_read_file(BVH_FL);


    if (bvh!=NULL) {
        fprintf(stdout, "node  = %d channel = %d\n", bvh->joint_num, bvh->channels);
        fprintf(stdout, "frame = %d time    = %f\n", bvh->framepsec, bvh->frame_time);
        fprintf(stdout, "frame_num = %d\n", bvh->frame_num);


        if (bvh->hierarchy!=NULL) {
            print_tTree(stdout, bvh->hierarchy);    
        }


         fprintf(stdout, "\n\n\n");
         bvh_print_data(stdout, bvh);

    }

}
