
#ifndef  __JBXL_OPENNI2_DEV_H_
#define  __JBXL_OPENNI2_DEV_H_


#ifdef  ENABLE_OPENNI2


#ifdef WIN32
//#pragma warning(disable:4996)
#pragma warning(disable:4005)       // INT8_MIN etc マクロの再定義
#endif


#include  <OpenNI.h>
#include  <NiTE.h>

#include  "NiDevice.h"
#include  "common++.h"
#include  "tools++.h"
#include  "buffer.h"


//
namespace jbxl {


#define   OPENNI2_DEVICE_PS1080     "PS1080"
#define   OPENNI2_DEVICE_KINECT     "Kinect"
#define   OPENNI2_DEVICE_ONIFILE    "oni File"

//
#define   OPENNI2_STREAM_DATA_NUM   3

#define   OPENNI2_DEPTH_FLAG        0x01
#define   OPENNI2_IMAGE_FLAG        0x02
//#define   OPENNI2_AUDIO_FLAG      0x04

#define   OPENNI2_DEPTH_INDEX       0
#define   OPENNI2_IMAGE_INDEX       1
//#define   OPENNI2_AUDIO_INDEX     2



class  COpenNi2Device
{
public:
    COpenNi2Device(void);
    virtual ~COpenNi2Device(void) { free();}

    ONI_EnuDev  m_enu_dev;

    int      m_state;
    BOOL     m_has_camera;
    Buffer   m_err_mesg;
    Buffer   m_dev_name;

    int      m_xsize;
    int      m_ysize;
    int      m_nfps;

public:
    BOOL    init(BOOL use_image);
    BOOL    init(char* fname, BOOL use_image);
    void    free(void);

    // Generators
    BOOL    create_Context(char* fname=NULL);
    BOOL    create_Recorder(char* fname, BOOL use_image);

    BOOL    create_Image(void);
    BOOL    create_Depth(void);
    BOOL    create_User(void);

    void    delete_Context(void);
    void    delete_Recorder(void);

    void    delete_Image(void);
    void    delete_Depth(void);
    void    delete_User(void);

public:
    void    wait_StreamData(void);

    BOOL    set_ImageData(void); 
    BOOL    set_DepthData(void);
    BOOL    set_SceneData(void);

    uByte*  get_ImageData(void) { if (imageFrame.isValid()) return (uByte*)imageFrame.getData(); else return NULL;}
    uByte*  get_DepthData(void) { if (depthFrame.isValid()) return (uByte*)depthFrame.getData(); else return NULL;}
    uByte*  get_SceneData(void) { if (userFrame.isValid())  return (uByte*)userFrame.getUserMap().getPixels(); else return NULL;}

public:
    openni::Device*         context;

    openni::VideoStream*    image;
    openni::VideoStream*    depth;
//  openni::VideoStream*    audio;

    openni::VideoFrameRef   imageFrame;
    openni::VideoFrameRef   depthFrame;
//  openni::VideoFrameRef   audioFrame;

    openni::VideoMode       imageMode;
    openni::VideoMode       depthMode;

    openni::Recorder*       recorder;

    // NiTE
    nite::UserTracker*      user;
    nite::UserTrackerFrameRef userFrame;

private:
    unsigned char   m_stream_flg;
    int             m_stream_num;
    BOOL            m_has_image_data;
    BOOL            m_has_depth_data;
//  BOOL            m_has_audio_data;

    openni::VideoStream*    m_streams[OPENNI2_STREAM_DATA_NUM];

public:
    //
    BOOL     setup_Tracking(int profile, double smooth);

    // File
    BOOL     start_Recorde(char* file_name, BOOL use_image);
    void     stop_Recorde(void);

public:
    // USB
    BOOL    open_USB_Device (void)  { return FALSE;}
    void    close_USB_Device(void)  {}
    void    set_LED_Color (int col) {}
    void    set_Tilt_Motor(int ang) {}
};




////////////////////////////////////////////////////////////////////////////////////
//

Buffer  initialize_OpenNI2(void);
void    shutdown_OpenNI2(void);

Buffer  initialize_NiTE2(void);
void    shutdown_NiTE2(void);


}       // namespace


#endif  // ENABLE_OPENNI2

#endif

