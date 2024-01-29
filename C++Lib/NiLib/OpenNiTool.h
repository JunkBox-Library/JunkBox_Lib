#ifndef  __JBXL_OPENNI_TOOL_H_
#define  __JBXL_OPENNI_TOOL_H_

/**
@brief    OpenNI用 ツール ヘッダ
@file     OpenNiTool.h
@author   Fumi.Iseki (C)
*/


//
#ifdef ENABLE_OPENNI2

#include "OpenNi2Tool.h"

#ifdef ENABLE_OPENNI
    #undef ENABLE_OPENNI
#endif

#endif


#ifdef  ENABLE_OPENNI

#include  "OpenNiDevice.h"


//
namespace jbxl {


class  COpenNiTool
{
public:
    COpenNiTool(void);
    virtual ~COpenNiTool(void) { free();}

private:
    COpenNiDevice*  device;
    COpenNiDevice*  dev_backup;

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

    BOOL  initDevice(BOOL image) { if (device!=NULL) return device->init(image); else return FALSE;}
    BOOL  initDevice(char* fname, BOOL image) { if (device!=NULL) return device->init(fname, image); else return FALSE;}
    
    BOOL  resetDevice(BOOL image) { deleteDepth(); deleteImage(); deletePlayer(); return initDevice(image);}
    BOOL  resetDevice(char* fname, BOOL image) { deleteDepth(); deleteImage(); deletePlayer(); return initDevice(fname, image);}

    BOOL  hasBackupDev(void)     { if (dev_backup!=NULL) return TRUE; else return FALSE;}
    void  deleteBackupDev(void)  { if (dev_backup!=NULL) { delete(dev_backup); dev_backup=NULL;}}

    int   getDevState(void)      { if (device!=NULL) return device->m_state; else return NI_STATE_UNKNOWN;}
    void  setDevState(int state) { if (device!=NULL) device->m_state = state;}

    BOOL  hasContext(void)   { if (device!=NULL && device->context!=NULL) return TRUE; else return FALSE;}
    BOOL  hasImageGen(void)  { if (device!=NULL && device->image!=NULL)   return TRUE; else return FALSE;}
    BOOL  hasDepthGen(void)  { if (device!=NULL && device->depth!=NULL)   return TRUE; else return FALSE;}
    BOOL  hasPlayer(void)    { if (device!=NULL && device->player!=NULL)  return TRUE; else return FALSE;}

    BOOL  createContext(void){ if (device!=NULL) return device->create_Context();else return FALSE;}
    BOOL  createImage(void)  { if (device!=NULL) return device->create_Image();  else return FALSE;}
    BOOL  createDepth(void)  { if (device!=NULL) return device->create_Depth();  else return FALSE;}
    BOOL  createPlayer(void) { if (device!=NULL) return device->create_Player(); else return FALSE;}

    void  deleteContext(void){ if (device!=NULL) device->delete_Context();}
    void  deleteImage(void)  { if (device!=NULL) device->delete_Image();}
    void  deleteDepth(void)  { if (device!=NULL) device->delete_Depth();}
    void  deletePlayer(void) { if (device!=NULL) device->delete_Player();}

    void  waitStreamData(void) { if (device!=NULL && device->context!=NULL) device->context->WaitAndUpdateAll();}     

    //
    BOOL  setImageData(void)
    {
        if (device!=NULL && device->image!=NULL && device->imageMD!=NULL) {
            device->image->GetMetaData(*(device->imageMD));
            return TRUE;
        }
        return FALSE;
    }
        
    //
    BOOL  setDepthData(void)
    {
        if (device!=NULL && device->depth!=NULL && device->depthMD!=NULL) {
            device->depth->GetMetaData(*(device->depthMD));
            return TRUE;
        }
        return FALSE;
    }

    //
    BOOL  setSceneData(void)
    {
        if (device!=NULL && device->user!=NULL && device->sceneMD!=NULL) {
            device->user->GetUserPixels(0, *(device->sceneMD));
            return TRUE;
        }
        return FALSE;
    }

    //
    uByte* getImageData(void) 
    { 
        if (device!=NULL && device->imageMD!=NULL) return (uByte*)device->imageMD->RGB24Data(); 
        else return NULL;
    }

    //
    uByte* getDepthData(void) 
    { 
        if (device!=NULL && device->depthMD!=NULL) return (uByte*)device->depthMD->Data(); 
        else return NULL;
    }

    //
    uByte* getSceneData(void) 
    { 
        if (device!=NULL && device->sceneMD!=NULL) return (uByte*)device->sceneMD->Data(); 
        else return NULL;
    }

    //
    BOOL  isTracking(unsigned int user)   
    { 
        if (device!=NULL && device->skeleton!=NULL && device->skeleton->IsTracking((XnUserID)user)) return TRUE; 
        else return FALSE;
    }
    void  stopTracking(unsigned int user) 
    { 
        if (device!=NULL && device->skeleton!=NULL) device->skeleton->StopTracking((XnUserID)user);
    }
    
    //
    void  setGlobalMirror(BOOL mirror) 
    {
        if (device!=NULL && device->context!=NULL) device->context->SetGlobalMirror(mirror);
    }

    //
    int   getXSize(void)  { if (device!=NULL) return device->m_xsize; else return 0;}
    int   getYSize(void)  { if (device!=NULL) return device->m_ysize; else return 0;}
    int   getFPS(void)    { if (device!=NULL) return device->m_nfps;  else return 0;}
    
    //
    void  setViewPoint(void)
    {
        if (device!=NULL && device->depth!=NULL && device->image!=NULL) {
            device->depth->GetAlternativeViewPointCap().SetViewPoint(*(device->image));
        }
    }

    //
    void  convertDepth2Image(int dx, int dy, unsigned short dz, int* ix, int* iy) 
    {
        (*ix) = dx;
        (*iy) = dy;
    }

    //
    void  convert2Projective(unsigned int c, XnPoint3D* a, XnPoint3D* b) 
    { 
        if (device!=NULL && device->depth!=NULL) device->depth->ConvertRealWorldToProjective(c, a, b);
    }

    //
    BOOL  startRecorde(char* file_name, BOOL use_image) 
    { 
        if (device!=NULL) return device->start_Recorde(file_name, use_image); 
        else return FALSE;
    }

    //
    void  stopRecorde(void) { device->stop_Recorde();}
    //
    void  playerSetRepeat(BOOL rep) { if (device!=NULL && device->player!=NULL) device->player->SetRepeat(rep); }
    void  playerSetSeek(XnInt64 tm) { if (device!=NULL && device->player!=NULL) device->player->SeekToTimeStamp(tm, XN_PLAYER_SEEK_SET);}

    // USB
    BOOL  open_USBDevice(void)   { if (device!=NULL) return device->open_USB_Device(); else return FALSE;}
    void  close_USBDevice(void)  { if (device!=NULL) device->close_USB_Device();}
    void  set_LEDColor(int col)  { if (device!=NULL) device->set_LED_Color(col);}
    void  set_TiltMotor(int ang) { if (device!=NULL) device->set_Tilt_Motor(ang);}
};


}       // namespace



#endif

#endif  // __JBXL_OPENNI_TOOL_H_










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



