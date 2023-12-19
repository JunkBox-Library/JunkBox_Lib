#ifndef  __JBXL_OPENNI2_TOOL_H_
#define  __JBXL_OPENNI2_TOOL_H_

/**
@brief    OpenNI2用 ツール ヘッダ
@file     OpenNi2Tool.h
@author   Fumi.Iseki (C)
*/


//
#ifdef ENABLE_OPENNI2
#ifdef ENABLE_OPENNI
#undef ENABLE_OPENNI
#endif
#endif



#ifdef ENABLE_OPENNI2


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenNI2
//

#include  "OpenNi2Device.h"
#include  "NiDevice.h"



namespace jbxl {


////////////////////////////////////////////////////////
// compatible for OpenNI ver 1.x

typedef struct XnVector3D
{
    double X;
    double Y;
    double Z;
} XnVector3D;


//
typedef struct XnMatrix3X3
{
    double elements[9];
} XnMatrix3X3;


typedef XnVector3D XnPoint3D;


#define XN_SKEL_PROFILE_ALL     2
#define XN_SKEL_PROFILE_UPPER   3



///////////////////////////////////////////////////////////
//

class  COpenNiTool
{
public:
    COpenNiTool(void);
    virtual ~COpenNiTool(void) { free();}


private:
    COpenNi2Device* device;
    COpenNi2Device* dev_backup;

public:
    Buffer          m_err_mesg;

    unsigned int    tracking_user;
    unsigned int    tracking_deny;


public:
    BOOL        init(BOOL use_camera);
    void        free(void);
    void        delete_Device(void);
    
    void        clear_JointsData(void);
    void        clear_JointsPosData(void);
    void        clear_JointsRotData(void);

    nite::UserData* get_Avatar(unsigned int id);


public:
    // Joints
    void        get_JointsPositionData(unsigned int nId);
    void        get_JointsRotationData(unsigned int nId);

    XnVector3D  joint_PositionData(int joint);
    XnMatrix3X3 joint_RotationData(int joint);
    double      joint_PositionConfidence(int joint);
    double      joint_RotationConfidence(int joint);


public:
    // Detection
    BOOL         start_Detection(int profile, double smooth);
    BOOL         stop_Detection(void);

    unsigned int get_TrackingUser(void);
    void         set_DenyTrackingSearch(unsigned int user);


    // Device
    BOOL        backupDevice(void);
    BOOL        restoreDevice(void);

protected:
// Data
    XnVector3D  jointPosData[OPENNI_JOINT_NUM];         // no data at [0]
    XnMatrix3X3 jointRotData[OPENNI_JOINT_NUM];         // no data at [0]
    double      jointPosConfidence[OPENNI_JOINT_NUM];
    double      jointRotConfidence[OPENNI_JOINT_NUM];

// device Interface
public:
    ONI_EnuDev getEnuDevice(void) { if (device!=NULL) return device->m_enu_dev; else return EnuDev_Unknown;} 

    BOOL    initDevice(BOOL image) { if (device!=NULL) return device->init(image); else return FALSE;}
    BOOL    initDevice(char* fname, BOOL image) { if (device!=NULL) return device->init(fname, image); else return FALSE;}

    BOOL    resetDevice(BOOL image) { deleteDepth(); deleteImage(); return initDevice(image);}
    BOOL    resetDevice(char* fname, BOOL image) { deleteDepth(); deleteImage(); return initDevice(fname, image);}

    BOOL    hasBackupDev(void)     { if (dev_backup!=NULL) return TRUE; else return FALSE;}
    void    deleteBackupDev(void)  { if (dev_backup!=NULL) { delete(dev_backup); dev_backup=NULL;}}

    int     getDevState(void)      { if (device!=NULL) return device->m_state; else return NI_STATE_UNKNOWN;}
    void    setDevState(int state) { if (device!=NULL) device->m_state = state;}

    BOOL    hasContext(void)   { if (device!=NULL && device->context!=NULL) return TRUE; else return FALSE;}
    BOOL    hasImageGen(void)  { if (device!=NULL && device->image!=NULL)   return TRUE; else return FALSE;}
    BOOL    hasDepthGen(void)  { if (device!=NULL && device->depth!=NULL)   return TRUE; else return FALSE;}

    BOOL    createContext(void){ if (device!=NULL) return device->create_Context(); else return FALSE;}
    BOOL    createImage(void)  { if (device!=NULL) return device->create_Image();   else return FALSE;}
    BOOL    createDepth(void)  { if (device!=NULL) return device->create_Depth();   else return FALSE;}

    void    deleteContext(void){ if (device!=NULL) device->delete_Context();}
    void    deleteImage(void)  { if (device!=NULL) device->delete_Image();}
    void    deleteDepth(void)  { if (device!=NULL) device->delete_Depth();}

    //
    void    waitStreamData(void) { if (device!=NULL) device->wait_StreamData();}

    BOOL    setImageData(void) { if (device!=NULL) return device->set_ImageData(); else return FALSE;}
    BOOL    setDepthData(void) { if (device!=NULL) return device->set_DepthData(); else return FALSE;}
    BOOL    setSceneData(void) { if (device!=NULL) return device->set_SceneData(); else return FALSE;}
    //
    uByte*  getImageData(void) { if (device!=NULL) return device->get_ImageData(); else return NULL; }
    uByte*  getDepthData(void) { if (device!=NULL) return device->get_DepthData(); else return NULL; }
    uByte*  getSceneData(void) { if (device!=NULL) return device->get_SceneData(); else return NULL; } 

    //
    BOOL    isTracking(unsigned int user)
    {
        nite::UserData* avatar = get_Avatar(user);
        if (avatar==NULL || avatar->isLost()) return FALSE;
        return TRUE;
    }

    //
    void    stopTracking(unsigned int user) 
    { 
        if (device!=NULL && device->user!=NULL) {
            device->user->stopSkeletonTracking((nite::UserId)user);
        }
    }
    
    //
    void    setGlobalMirror(BOOL mirror) 
    {
        if (device!=NULL) {
            bool mirr = false;
            if (mirror) mirr = true;
            if (device->image!=NULL) device->image->setMirroringEnabled(mirr);
            if (device->depth!=NULL) device->depth->setMirroringEnabled(mirr);
        }
    }

    //
    int     getXSize(void)  { if (device!=NULL) return device->m_xsize; else return 0;}
    int     getYSize(void)  { if (device!=NULL) return device->m_ysize; else return 0;}
    int     getFPS(void)    { if (device!=NULL) return device->m_nfps;  else return 0;}
    
    //
    void    setViewPoint(void) {}

    //
    void    convertDepth2Image(int dx, int dy, unsigned short dz, int* ix, int* iy)
    {
        if (device!=NULL && device->image!=NULL && device->depth!=NULL) {
            openni::CoordinateConverter::convertDepthToColor(*(device->depth), *(device->image), dx, dy, (openni::DepthPixel)dz, ix, iy);
        }
    }

    //
    void    convert2Projective(unsigned int c, XnPoint3D* a, XnPoint3D* b) 
    {
        if (device==NULL && device->user!=NULL) {
            memset(b, 0, c*sizeof(XnPoint3D));
            return;
        }
        //
        for (unsigned int i=0; i<c; i++) {
            device->user->convertJointCoordinatesToDepth(a->X, a->Y, a->Z, &(b->X), &(b->Y));
            a++;
            b++;
        }
        //
        // 本当はさらに Image座標系に変換しないといけない．
    }

    //
    BOOL    startRecorde(char* file_name, BOOL use_image) { 
        if (device!=NULL) return device->start_Recorde(file_name, use_image); 
        else return FALSE;
    }
    //
    void    stopRecorde(void) { device->stop_Recorde();}
    
    //
    void    playerSetRepeat(BOOL rep) {}
    void    playerSetSeek(int tm) {}

    // USB
    BOOL    open_USBDevice(void)   { if (device!=NULL) return device->open_USB_Device(); else return FALSE;}
    void    close_USBDevice(void)  { if (device!=NULL) device->close_USB_Device();}
    void    set_LEDColor(int col)  { if (device!=NULL) device->set_LED_Color(col);}
    void    set_TiltMotor(int ang) { if (device!=NULL) device->set_Tilt_Motor(ang);}

};



}       // namespace




#endif



#endif  // __JBXL_OPENNI2_TOOL_H_



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Information

/*
XnTypes.h

typedef enum XnSkeletonJoint
{
    XN_SKEL_HEAD            = 1,
    XN_SKEL_NECK            = 2,
    XN_SKEL_TORSO           = 3,
    XN_SKEL_WAIST           = 4,

    XN_SKEL_LEFT_COLLAR     = 5,
    XN_SKEL_LEFT_SHOULDER   = 6,
    XN_SKEL_LEFT_ELBOW      = 7,
    XN_SKEL_LEFT_WRIST      = 8,
    XN_SKEL_LEFT_HAND       = 9,
    XN_SKEL_LEFT_FINGERTIP  =10,

    XN_SKEL_RIGHT_COLLAR    =11,
    XN_SKEL_RIGHT_SHOULDER  =12,
    XN_SKEL_RIGHT_ELBOW     =13,
    XN_SKEL_RIGHT_WRIST     =14,
    XN_SKEL_RIGHT_HAND      =15,
    XN_SKEL_RIGHT_FINGERTIP =16,

    XN_SKEL_LEFT_HIP        =17,
    XN_SKEL_LEFT_KNEE       =18,
    XN_SKEL_LEFT_ANKLE      =19,
    XN_SKEL_LEFT_FOOT       =20,

    XN_SKEL_RIGHT_HIP       =21,
    XN_SKEL_RIGHT_KNEE      =22,
    XN_SKEL_RIGHT_ANKLE     =23,
    XN_SKEL_RIGHT_FOOT      =24 
} XnSkeletonJoint;
*/


/*
NiteCEnums.h

typedef enum
{
    NITE_JOINT_HEAD,
    NITE_JOINT_NECK,

    NITE_JOINT_LEFT_SHOULDER,
    NITE_JOINT_RIGHT_SHOULDER,
    NITE_JOINT_LEFT_ELBOW,
    NITE_JOINT_RIGHT_ELBOW,
    NITE_JOINT_LEFT_HAND,
    NITE_JOINT_RIGHT_HAND,

    NITE_JOINT_TORSO,

    NITE_JOINT_LEFT_HIP,
    NITE_JOINT_RIGHT_HIP,
    NITE_JOINT_LEFT_KNEE,
    NITE_JOINT_RIGHT_KNEE,
    NITE_JOINT_LEFT_FOOT,
    NITE_JOINT_RIGHT_FOOT,
} NiteJointType;
*/
