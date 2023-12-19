
#ifndef  __JBXL_CPP_NIDEVICE_H_
#define  __JBXL_CPP_NIDEVICE_H_



#ifdef ENABLE_OPENNI2
    #define   OPENNI2_JOINT_NUM     16                  // NiteCEnums.h (15+1) [+1はPELVIS] 
    #define   OPENNI_JOINT_NUM      OPENNI2_JOINT_NUM
#elif defined(ENABLE_OPENNI)
    #define   OPENNI_JOINT_NUM      25                  // XnTypes.h (24+1)    [+1はPELVIS] 
#endif


#define   OPENNI_USERS_NUM          6
#define   OPENNI_JOINT_CONFIDENCE   0.5


//
enum  ONI_EnuDev
{
    EnuDev_Unknown,
    EnuDev_None,
    EnuDev_PS1080,
    EnuDev_Kinect,
    EnuDev_OniFile
};



//#define   OPENNI_XGA_MODE

#ifdef  OPENNI_XGA_MODE
    #define OPENNI_DEPTH_XSIZE  1024    // XN_XGA_X_RES
    #define OPENNI_DEPTH_YSIZE  768     // XN_XGA_Y_RES
    #define OPENNI_DEPTH_FPS    15
#else
    #define OPENNI_DEPTH_XSIZE  640     // XN_VGA_X_RES
    #define OPENNI_DEPTH_YSIZE  480     // XN_VGA_Y_RES
    #define OPENNI_DEPTH_FPS    30
#endif



//

#define   NI_STATE_UNKNOWN          -1  
#define   NI_STATE_DETECT_STOPPED    0
#define   NI_STATE_DETECT_STARTING   1
#define   NI_STATE_DETECT_EXEC       2
#define   NI_STATE_DETECT_STOPPING   3
#define   NI_STATE_SAVE_WORKING      4

#define   NI_WORKING_WAIT_TIME      200     // ms
#define   NI_STOP_WAIT_TIME         500     // ms

#define   NI_FPS_MAX                30

// USB
#define   NI_VID_MICROSOFT          0x045e
#define   NI_PID_NUI_MOTOR          0x02b0

#define   NI_LED_OFF                0
#define   NI_LED_GREEN              1
#define   NI_LED_RED                2
#define   NI_LED_ORANGE             3
#define   NI_LED_BLINK_GREEN        4
#define   NI_LED_BLINK_RED          5
#define   NI_LED_BLINK_ORANGE       6


namespace jbxl {




}


#endif

