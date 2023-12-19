
/**
@brief    OpenNI用 ツール
@file     OpenNiTool.cpp
@author   Fumi.Iseki (C)
*/


#include  "OpenNiTool.h"
#include  "tools++.h"


using namespace jbxl;


#ifdef  ENABLE_OPENNI


COpenNiTool::COpenNiTool(void)
{
    device     = NULL;
    dev_backup = NULL;

    tracking_user = 0;
    tracking_deny = 0;

    m_err_mesg = make_Buffer(LMESG);

    clear_JointsData();
}



BOOL  COpenNiTool::init(BOOL use_image)
{
    device = new COpenNiDevice();
    BOOL ret = device->init(use_image);
    if (ret) {
        // default is mirror mode
        if (device->context!=NULL) device->context->SetGlobalMirror(TRUE);
    }
    else {
        copy_Buffer(&device->m_err_mesg, &m_err_mesg);
    }

    //
    device->m_state = NI_STATE_DETECT_STOPPED;

    return ret;
}



void  COpenNiTool::free(void)
{
    delete_Device();

    free_Buffer(&m_err_mesg);
}



void  COpenNiTool::delete_Device(void)
{
    if (device!=NULL) {
        delete(device);
        device = NULL;
    }
    return;
}



void  COpenNiTool::clear_JointsData(void)
{
    clear_JointsPosData();
    clear_JointsRotData();
}



void  COpenNiTool::clear_JointsPosData(void)
{
    memset(jointPosData, 0, sizeof(XnVector3D) *OPENNI_JOINT_NUM);
    memset(jointPosConfidence, 0, sizeof(double)*OPENNI_JOINT_NUM);
}



void  COpenNiTool::clear_JointsRotData(void)
{
    memset(jointRotData, 0, sizeof(XnMatrix3X3)*OPENNI_JOINT_NUM);
    memset(jointRotConfidence, 0, sizeof(double)*OPENNI_JOINT_NUM);
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
// Joint Data

void  COpenNiTool::get_JointsPositionData(unsigned int nId)
{
    XnSkeletonJointPosition  joint;
    //memset(jointPosData, 0, sizeof(XnVector3D)*OPENNI_JOINT_NUM);

    if (device->skeleton!=NULL) {
        for (int j=1; j<OPENNI_JOINT_NUM; j++) {
            device->skeleton->GetSkeletonJointPosition(nId, (XnSkeletonJoint)j, joint);
            jointPosData[j] = joint.position;
            jointPosConfidence[j] = joint.fConfidence;
        }
    }
}



void  COpenNiTool::get_JointsRotationData(unsigned int nId)
{
    XnSkeletonJointOrientation  joint;
    //memset(jointRotData, 0, sizeof(XnMatrix3X3)*OPENNI_JOINT_NUM);

    if (device->skeleton!=NULL) {
        for (int j=1; j<OPENNI_JOINT_NUM; j++) {
            device->skeleton->GetSkeletonJointOrientation(nId, (XnSkeletonJoint)j, joint);
            jointRotData[j] = joint.orientation;
            jointRotConfidence[j] = joint.fConfidence;
        }
    }
}



XnVector3D  COpenNiTool::joint_PositionData(int j)
{ 
    XnVector3D vect;

    if (j>=0 && j<OPENNI_JOINT_NUM) {
        vect = jointPosData[j];
    }
    else {
        memset(&vect, 0, sizeof(XnVector3D));
    }

    return vect;
}



XnMatrix3X3  COpenNiTool::joint_RotationData(int j)
{
    XnMatrix3X3 mtrx;

    if (j>=0 && j<OPENNI_JOINT_NUM) {
        mtrx = jointRotData[j];
    }
    else {
        memset(&mtrx, 0, sizeof(XnMatrix3X3));
    }

    return mtrx;
}



double  COpenNiTool::joint_PositionConfidence(int j)
{ 
    double cnfd = 0.0;

    if (j>=0 && j<OPENNI_JOINT_NUM) {
        cnfd = (double)jointPosConfidence[j];
    }

    return cnfd;
}



double  COpenNiTool::joint_RotationConfidence(int j)
{ 
    double cnfd = 0.0;

    if (j>=0 && j<OPENNI_JOINT_NUM) {
        cnfd = (double)jointRotConfidence[j];
    }

    return cnfd;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Start and Stop Detection (Reset)

BOOL  COpenNiTool::start_Detection(int profile, double smooth)
{
    if (device->m_state==NI_STATE_DETECT_EXEC) {
        copy_s2Buffer("COpenNiTool:start_Detection WARNING: detection is already executed", &m_err_mesg);
        return FALSE;
    }

    device->m_state = NI_STATE_DETECT_STARTING;
    clear_JointsData();
    tracking_user = 0;
    tracking_deny = 0;

    BOOL     ret = device->create_User();
    if (ret) ret = device->setup_Tracking(profile, smooth);
    if (ret) ret = device->setup_CallBacks();

    if (!ret) {
        device->m_state = NI_STATE_DETECT_STOPPING;
        device->clear_Skeleton();
        device->delete_User();
        device->m_state = NI_STATE_DETECT_STOPPED;
        copy_s2Buffer("COpenNiTool:start_Detection ERROR: Myabe NITE is not installed!!", &m_err_mesg);
        return FALSE;
    }

    device->m_state = NI_STATE_DETECT_EXEC;

    return TRUE;
}



BOOL  COpenNiTool::stop_Detection(void)
{
    if (device->m_state==NI_STATE_DETECT_STOPPED) {
        copy_s2Buffer("COpenNiTool:stop_Detection WARNING: detection is already stopped", &m_err_mesg);
        return FALSE;
    }

    device->m_state = NI_STATE_DETECT_STOPPING;
    Sleep(NI_STOP_WAIT_TIME);

    device->clear_Tracking();
    device->clear_CallBacks();
    device->clear_Skeleton();
    device->delete_User();

    tracking_user = (unsigned int)0;
    tracking_deny = (unsigned int)0;

    device->m_state = NI_STATE_DETECT_STOPPED;
    
    return TRUE;
}



unsigned int  COpenNiTool::get_TrackingUser(void)
{
    XnUserID id = (unsigned int)0;

    if (device->user!=NULL && device->skeleton!=NULL) {
        device->nUsers = OPENNI_USERS_NUM;
        device->user->GetUsers(device->dUsers, device->nUsers);

        int start = 0;
        if (tracking_deny!=0) {
            for (int i=0; i<device->nUsers; i++) {
                if (tracking_deny==device->dUsers[i]) {
                    start = i + 1;
                    break;
                }
            }
        }
        //
        for (int i=0; i<device->nUsers; i++) {
            int idx = (start + i) % device->nUsers;
            if (device->skeleton->IsTracking(device->dUsers[idx])) {
                id = device->dUsers[idx];
                break;
            }
        }
    }

    return id;
}



void  COpenNiTool::set_DenyTrackingSearch(unsigned int user)
{
    if ((int)user>0) tracking_deny = user;
    else             tracking_deny = 0;

    return;
}




///////////////////////////////////////////////////////////////////////

BOOL  COpenNiTool::backupDevice(void)
{
    dev_backup = device;
    device = new COpenNiDevice();
    
    if (device==NULL) {
        device = dev_backup;
        dev_backup = NULL;
        return FALSE;
    }
    return TRUE;
}



BOOL  COpenNiTool::restoreDevice(void)
{
    if (dev_backup==NULL) return FALSE;
    
    delete(device);
    device = dev_backup;
    dev_backup = NULL;
    return TRUE;
}



#endif      // ifdef ENABLE_OPENNI
