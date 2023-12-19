
#ifndef  __JBXL_OPENNI_DEV_H_
#define  __JBXL_OPENNI_DEV_H_


#ifdef  ENABLE_OPENNI2
#ifdef  ENABLE_OPENNI
#undef  ENABLE_OPENNI
#endif
#endif


#ifdef  ENABLE_OPENNI


#ifdef WIN32
#pragma warning(disable:4996)
#endif


#include  <XnCppWrapper.h>
#include  <XnUSB.h>

#include  "NiDevice.h"
#include  "common++.h"
#include  "tools++.h"
#include  "buffer.h"


//
namespace jbxl {



class  COpenNiDevice
{
public:
    COpenNiDevice(void);
    virtual ~COpenNiDevice(void) { free();}

    ONI_EnuDev  m_enu_dev;

    int      m_state;
    BOOL     m_has_camera;
    Buffer   m_err_mesg;

    int      m_xsize;
    int      m_ysize;
    int      m_nfps;

    XnStatus rc;

public:
    BOOL    init(BOOL use_image);
    BOOL    init(char* fname, BOOL use_image);
    void    free(void);

    // Generators
    BOOL    create_Context(char* fname=NULL);
    BOOL    create_Image (BOOL easy=FALSE);
    BOOL    create_Depth (BOOL easy=FALSE);
    BOOL    create_User  (BOOL easy=FALSE);
    BOOL    create_Player(BOOL easy=FALSE);

    void    delete_Context(void);
    void    delete_Image(void);
    void    delete_Depth(void);
    void    delete_User(void);
    void    delete_Player(void);

public:
    XnMapOutputMode         outputMode;

    xn::Context*            context;
    xn::ImageGenerator*     image;
    xn::DepthGenerator*     depth;
    xn::UserGenerator*      user;
    xn::Player*             player;

    xn::ImageMetaData*      imageMD;
    xn::DepthMetaData*      depthMD;
    xn::SceneMetaData*      sceneMD;    // for User

    xn::Recorder*           recorder;
    xn::SkeletonCapability* skeleton;

public:
    XnUserID                dUsers[OPENNI_USERS_NUM];
    XnUInt16                nUsers;


public:
    // Capabilities
    BOOL     setup_Tracking(int profile, double smooth);
    void     clear_Skeleton(void);
    void     clear_Tracking(void);

    // File
    BOOL     create_Recorder(char* file_name, BOOL use_image);
    void     delete_Recorder(void);

    BOOL     start_Recorde(char* file_name, BOOL use_image);
    void     stop_Recorde(void);

public:
    // USB
    XN_USB_DEV_HANDLE       usb_dev;
    XnUSBConnectionString*  devPath;

public:
    // USB
    BOOL    open_USB_Device (void);
    void    close_USB_Device(void);
    void    set_LED_Color (int col);
    void    set_Tilt_Motor(int ang);

public:
    XnCallbackHandle        userCallbacks;
    XnCallbackHandle        poseCallbacks;
    XnCallbackHandle        calibCallbacks;

public:
    // Call Back
    BOOL    setup_CallBacks(void);
    void    clear_CallBacks(void);

    BOOL    set_UserCallbacks(xn::UserGenerator::UserHandler newUser, xn::UserGenerator::UserHandler lostUser, void* cookie);
    BOOL    set_CalibCallbacks(xn::SkeletonCapability::CalibrationStart calibStart, xn::SkeletonCapability::CalibrationEnd calibEnd, void* cookie);

    void    unset_UserCallbacks(void);
    void    unset_CalibCallbacks(void);
};




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback Functions

void  XN_CALLBACK_TYPE userDetect(xn::UserGenerator& user, XnUserID nId, void* cookie);
void  XN_CALLBACK_TYPE userLost  (xn::UserGenerator& user, XnUserID nId, void* cookie);
void  XN_CALLBACK_TYPE calibStart(xn::SkeletonCapability& skeleton, XnUserID nId, void* cookie);
void  XN_CALLBACK_TYPE calibEnd  (xn::SkeletonCapability& skeleton, XnUserID nId, XnBool success, void* cookie);




}       // namespace


#endif
#endif

