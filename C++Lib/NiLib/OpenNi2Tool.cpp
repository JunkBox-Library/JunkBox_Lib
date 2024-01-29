/**
@brief    OpenNI2用 ツール
@file     OpenNi2Tool.cpp
@author   Fumi.Iseki (C)
*/

#include  "OpenNi2Tool.h"
#include  "tools++.h"


#ifdef  ENABLE_OPENNI2


using namespace jbxl;


COpenNiTool::COpenNiTool(void)
{
    device        = NULL;
    dev_backup    = NULL; 

    tracking_user = 0;
    tracking_deny = 0;

    m_err_mesg    = make_Buffer(LMESG);

    clear_JointsData();
}


BOOL  COpenNiTool::init(BOOL use_image)
{
    device = new COpenNi2Device();
    BOOL ret = device->init(use_image);
    if (ret) {
        // default is mirror mode
        setGlobalMirror(TRUE);
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


nite::UserData* COpenNiTool::get_Avatar(unsigned int id)
{
    static nite::UserData avatar;

    if (device!=NULL && device->user!=NULL) {
        const nite::Array<nite::UserData>& avatars = device->userFrame.getUsers();
        for (int i=0; i<avatars.getSize(); i++) {
            avatar = avatars[i];
            if (id==(unsigned int)avatar.getId()) {
                return &avatar;
                break;
            }
        }
    }

    return NULL;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// Joint Data

void  COpenNiTool::get_JointsPositionData(unsigned int nId)
{
    nite::UserData* avatar = get_Avatar(nId);
    if (avatar==NULL) {
        clear_JointsPosData();
        return;
    }

    const nite::Skeleton& skeelton = avatar->getSkeleton();

    if (skeelton.getState()==nite::SKELETON_TRACKED) {
        for (int j=1; j<OPENNI_JOINT_NUM; j++) {
            const nite::SkeletonJoint& joint = skeelton.getJoint((nite::JointType)(j-1));
            const nite::Point3f& position = joint.getPosition();
            jointPosData[j].X = position.x;
            jointPosData[j].Y = position.y;
            jointPosData[j].Z = position.z;
            jointPosConfidence[j] = joint.getPositionConfidence();
        }
    }
}


void  COpenNiTool::get_JointsRotationData(unsigned int nId)
{
    memset(jointRotData, 0, sizeof(XnMatrix3X3)*OPENNI_JOINT_NUM);
    memset(jointRotConfidence, 0, sizeof(double)*OPENNI_JOINT_NUM);

    /*
    XnSkeletonJointOrientation  joint;
    //memset(jointRotData, 0, sizeof(XnMatrix3X3)*OPENNI_JOINT_NUM);

    if (device->skeleton!=NULL) {
        for (int j=1; j<OPENNI_JOINT_NUM; j++) {
            device->skeleton->GetSkeletonJointOrientation(nId, (XnSkeletonJoint)j, joint);
            jointRotData[j] = joint.orientation;
            jointRotConfidence[j] = joint.fConfidence;
        }
    }
    */
}


XnVector3D   COpenNiTool::joint_PositionData(int j)
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

    if (!ret) {
        device->m_state = NI_STATE_DETECT_STOPPING;
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
    device->delete_User();

    tracking_user = 0;
    tracking_deny = 0;

    device->m_state = NI_STATE_DETECT_STOPPED;
    
    return TRUE;
}


unsigned int  COpenNiTool::get_TrackingUser(void)
{
    unsigned int id = 0;

    if (device!=NULL && device->user!=NULL) {
        const nite::Array<nite::UserData>& avatars = device->userFrame.getUsers();
            
        int start = 0;
        if (tracking_deny!=0) {
            for (int i=0; i<avatars.getSize(); i++) {
                const nite::UserData& avatar = avatars[i];
                if (tracking_deny==(unsigned int)avatar.getId()) {
                    start = i + 1;
                    break;
                }
            }
        }
        //
        for (int i=0; i<avatars.getSize(); i++) {
            int idx = (start + i) % avatars.getSize();
            const nite::UserData& avatar = avatars[idx];
            if (avatar.isNew()) {
                nite::Status nc = device->user->startSkeletonTracking(avatar.getId());
                if (nc==nite::STATUS_OK) id = (unsigned int)avatar.getId();
                break;
            }
            else if (!avatar.isLost()) {
                id = (unsigned int)avatar.getId();
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
    device = new COpenNi2Device();
    
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


#endif      // ifdef ENABLE_OPENNI2
