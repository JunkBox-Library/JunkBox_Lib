/**

*/

#include  "OpenNi2Device.h"

#ifdef  ENABLE_OPENNI2


using namespace jbxl;


COpenNi2Device::COpenNi2Device(void)
{
    context      = NULL;
    recorder     = NULL;

    image        = NULL;
    depth        = NULL;
//  audio        = NULL;
    user         = NULL;

    m_enu_dev    = EnuDev_Unknown;
    m_has_camera = TRUE;
    m_err_mesg   = make_Buffer(LMESG);

    m_state      = NI_STATE_DETECT_STOPPED;
    m_xsize      = OPENNI_DEPTH_XSIZE;
    m_ysize      = OPENNI_DEPTH_YSIZE;
    m_nfps       = OPENNI_DEPTH_FPS;

    //
    m_has_image_data = TRUE;
    m_has_depth_data = TRUE;
//  m_has_audio_data = TRUE;

    // Video Mode
    imageMode.setFps(m_nfps);
    imageMode.setResolution(m_xsize, m_ysize);
    imageMode.setPixelFormat(openni::PIXEL_FORMAT_RGB888);

    depthMode.setFps(m_nfps);
    depthMode.setResolution(m_xsize, m_ysize);
    depthMode.setPixelFormat(openni::PIXEL_FORMAT_DEPTH_1_MM);
}


BOOL  COpenNi2Device::init(BOOL use_image)
{
    m_stream_flg = 0x00; 
    m_stream_num = 0;
    for (int s=0; s<OPENNI2_STREAM_DATA_NUM; s++) m_streams[s] = NULL;

    m_enu_dev = EnuDev_None;

    BOOL ret = create_Context();
    if (ret && use_image) ret = create_Image();
    if (ret) ret = create_Depth();
    if (ret) {
        const openni::DeviceInfo info = context->getDeviceInfo();
        DEBUG_INFO("INFO: COpenNi2Device::init(): Device Name = %s", info.getName());
        //
        if      (!strcasecmp(info.getName(), OPENNI2_DEVICE_PS1080))  m_enu_dev = EnuDev_PS1080;
        else if (!strcasecmp(info.getName(), OPENNI2_DEVICE_KINECT))  m_enu_dev = EnuDev_Kinect;
        else if (!strcasecmp(info.getName(), OPENNI2_DEVICE_ONIFILE)) m_enu_dev = EnuDev_OniFile;
    }
    return ret;
}


BOOL  COpenNi2Device::init(char* fname, BOOL use_image)
{
    m_enu_dev = EnuDev_None;

    if (fname==NULL) {
        copy_s2Buffer("ERROR: COpenNi2Device::init(): File name is NULL.", &m_err_mesg);
        return FALSE;
    }

    m_stream_flg = 0x00; 
    m_stream_num = 0;
    for (int s=0; s<OPENNI2_STREAM_DATA_NUM; s++) m_streams[s] = NULL;

    BOOL ret = create_Context(fname);
    if (ret && use_image) ret = create_Image();
    if (ret) ret = create_Depth();
    if (ret) m_enu_dev = EnuDev_OniFile;

    return ret;
}


void  COpenNi2Device::free(void)
{
    DEBUG_LOG("LOG: COpenNi2Device::free(): deleting Recorder ...");
    delete_Recorder();
    //
    DEBUG_LOG("LOG: COpenNi2Device::free(): deleting Image ...");
    delete_Image();
    DEBUG_LOG("LOG: COpenNi2Device::free(): deleting Depth ...");
    delete_Depth();
    DEBUG_LOG("LOG: COpenNi2Device::free(): deleting Context ...");
    delete_Context();
    DEBUG_LOG("LOG: COpenNi2Device::free(): deleted Generators.");
    //
    free_Buffer(&m_err_mesg);

    DEBUG_LOG("LOG: COpenNi2Device::free(): END");
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Delete Generators

BOOL  COpenNi2Device::create_Context(char* fname)
{
    if (context==NULL) {
        context = new openni::Device();
        if (context==NULL) {
            DEBUG_ERR("ERROR: COpenNi2Device::create_Context(): context is NULL!!");
            return FALSE;
        }
    }
    else context->close();

    const char* uri = fname;
    if (uri==NULL) uri = openni::ANY_DEVICE;

    openni::Status ret = context->open(uri);
    if (ret!=openni::STATUS_OK) {
        DEBUG_ERR("ERROR: COpenNi2Device::create_Context(): Device Open Error!! (%d)", ret);
        deleteNull(context);
        return FALSE;
    }
    //
    //context->setDepthColorSyncEnabled(true);

    DEBUG_INFO("INFO: COpenNi2Device::create_Context(): created Context.");
    return TRUE;
}


BOOL  COpenNi2Device::create_Recorder(char* fname, BOOL use_image)
{
    if (fname==NULL) {
        copy_s2Buffer("ERROR: COpenNi2Device::create_Recorder(): File name is NULL.", &m_err_mesg);
        return FALSE;
    }
    if (depth==NULL || (use_image && image==NULL)) {
        copy_s2Buffer("ERROR: COpenNi2Device::create_Recorder(): Image or Depth Stream is NULL.", &m_err_mesg);
        return FALSE;
    }

    if (recorder==NULL) {
        recorder = new openni::Recorder();
        if (recorder==NULL) {
            DEBUG_ERR("ERROR: COpenNi2Device::create_Recorder(): recorder is NULL!!");
            return FALSE;
        }
    }
    else recorder->stop();
    
    openni::Status ret = recorder->create(fname);
    if (ret!=openni::STATUS_OK) {
        deleteNull(recorder);
        DEBUG_ERR("ERROR: COpenNi2Device::create_Recorder: File Open Error.");
        copy_s2Buffer("ERROR: COpenNi2Device::create_Recorder: File Open Error.", &m_err_mesg);
        return FALSE;
    }   
    
    ret = recorder->attach(*depth, true);
    if (ret==openni::STATUS_OK && use_image) ret = recorder->attach(*image, true);
    if (ret!=openni::STATUS_OK) {
        deleteNull(recorder);
        DEBUG_ERR("ERROR: COpenNi2Device::create_Recorder: Stream Attach Error.");
        copy_s2Buffer("ERROR: COpenNi2Device::create_Recorder: Stream Attach Error.", &m_err_mesg);
        return FALSE;
    }

    DEBUG_INFO("INFO: COpenNi2Device::create_Recorder(): created Recorder.");
    return TRUE;
}


BOOL  COpenNi2Device::create_Image(void)
{
    if (!m_has_camera) return FALSE;

    if (context==NULL) {
        copy_s2Buffer("ERROR: COpenNi2Device::create_Image(): context is NULL.", &m_err_mesg);
        return FALSE;
    }

    image = new openni::VideoStream();
    if (image==NULL) return FALSE;

    openni::Status ret = image->create(*context, openni::SENSOR_COLOR);
    if (ret!=openni::STATUS_OK) {
        deleteNull(image);
        copy_s2Buffer("ERROR: COpenNi2Device::create_Image(): Craete Error.", &m_err_mesg);
        return FALSE;
    }

    ret = image->start();
    if (ret!=openni::STATUS_OK) {
        deleteNull(image);
        copy_s2Buffer("ERROR: COpenNi2Device::create_Image(): Start Error.", &m_err_mesg);
        return FALSE;
    }

    image->setVideoMode(imageMode);

    m_streams[OPENNI2_IMAGE_INDEX] = image;
    m_stream_flg |= OPENNI2_IMAGE_FLAG;
    m_stream_num++;

    DEBUG_INFO("INFO: COpenNi2Device::create_Image(): created Image Stream.");
    return TRUE;
}


BOOL  COpenNi2Device::create_Depth(void)
{
    if (context==NULL) {
        copy_s2Buffer("ERROR: COpenNi2Device::create_Depth(): context is NULL.", &m_err_mesg);
        return FALSE;
    }

    depth = new openni::VideoStream();
    if (depth==NULL) return FALSE;

    openni::Status ret = depth->create(*context, openni::SENSOR_DEPTH);
    if (ret!=openni::STATUS_OK) {
        deleteNull(depth);
        copy_s2Buffer("ERROR: COpenNi2Device::create_Depth(): Craete Error.", &m_err_mesg);
        return FALSE;
    }

    ret = depth->start();
    if (ret!=openni::STATUS_OK) {
        deleteNull(depth);
        copy_s2Buffer("ERROR: COpenNi2Device::create_Depth(): Start Error.", &m_err_mesg);
        return FALSE;
    }

    depth->setVideoMode(depthMode);

    m_streams[OPENNI2_DEPTH_INDEX] = depth;
    m_stream_flg |= OPENNI2_DEPTH_FLAG;
    m_stream_num++;

    DEBUG_INFO("INFO: COpenNi2Device::create_Depth(): created Depth Stream.");
    return TRUE;
}


BOOL  COpenNi2Device::create_User(void)
{
    user = new nite::UserTracker();
    if (user==NULL) return FALSE;

    nite::Status ret = user->create(context);   // context==NULL is OK
    if (ret!=nite::STATUS_OK) {
        deleteNull(user);
        copy_s2Buffer("ERROR: COpenNi2Device::create_Depth(): Create Error", &m_err_mesg);
        return FALSE;
    }

    DEBUG_INFO("INFO: COpenNi2Device::create_User(): created User Tracker.");
    return TRUE;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Delete Generators

void  COpenNi2Device::delete_Context(void)
{
    if (context==NULL || !context->isValid()) return;
    //
    context->close();
    delete(context);
    context = NULL;

    return;
}


void    COpenNi2Device::delete_Recorder(void)
{
    if (recorder==NULL || !recorder->isValid()) return;
    //
    int prev_state = m_state;
    m_state = NI_STATE_SAVE_WORKING;
    {
        recorder->stop();
        DEBUG_INFO("INFO: COpenNi2Device::delete_Recorder(): Recorder Stopped.");
        delete(recorder);
        recorder = NULL;
    }
    m_state = prev_state;

    return;
}


void  COpenNi2Device::delete_Image(void)
{
    m_stream_num--;
    m_streams[OPENNI2_IMAGE_INDEX] = NULL;
    m_stream_flg &= ~OPENNI2_IMAGE_FLAG;

    if (image==NULL || !image->isValid()) return;

    //
    image->stop();

    if (imageFrame.isValid()) imageFrame.release();
    delete(image);
    image = NULL;
    return;
}


void  COpenNi2Device::delete_Depth(void)
{
    m_stream_num--;
    m_streams[OPENNI2_DEPTH_INDEX] = NULL;
    m_stream_flg &= ~OPENNI2_DEPTH_FLAG;
    
    if (depth==NULL || !depth->isValid()) return;

    //
    depth->stop();

    if (depthFrame.isValid()) depthFrame.release();
    delete(depth);
    depth = NULL;
    return;
}


void  COpenNi2Device::delete_User(void)
{
    if (user==NULL || !user->isValid()) return;

    //
    if (userFrame.isValid()) userFrame.release();
    delete(user);
    user = NULL;

    return;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// File

BOOL  COpenNi2Device::start_Recorde(char* file_name, BOOL use_image)
{
    DEBUG_INFO("INFO: COpenNi2Device::delete_Recorder(): Recorder Starting ...");

    BOOL ret = FALSE;
    unlink(file_name);
    
    int prev_state = m_state;
    m_state = NI_STATE_SAVE_WORKING;
    {
        Sleep(NI_WORKING_WAIT_TIME);
        ret = create_Recorder(file_name, use_image);
        if (ret) {
            openni::Status nc = recorder->start();
            if (nc!=openni::STATUS_OK) {
                copy_s2Buffer("ERROR: COpenNi2Device::start_Recorde: Start Error.", &m_err_mesg);
                ret = FALSE;
            }
        }
    }
    m_state = prev_state;

    if (ret) {
        DEBUG_INFO("INFO: COpenNi2Device::delete_Recorder(): Recorder Started.");
    }
    else {
        delete_Recorder();
        unlink(file_name);
        DEBUG_INFO("INFO: COpenNi2Device::delete_Recorder(): Recorder can not Start.");
    }
    
    return ret;
}


void  COpenNi2Device::stop_Recorde(void)
{
    DEBUG_INFO("INFO: COpenNi2Device::delete_Recorder(): Recorder Stopping ...");
    if (recorder!=NULL && recorder->isValid()) {
        delete_Recorder();
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

BOOL  COpenNi2Device::setup_Tracking(int profile, double smooth)
{
    if (user!=NULL) {
        nite::Status nc = user->setSkeletonSmoothingFactor(smooth);
        if (nc==nite::STATUS_OK) return TRUE;
    }

    return FALSE;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set Data

void   COpenNi2Device::wait_StreamData(void) 
{
//  return;

    openni::Status rc;
    unsigned char flag = 0x00;
    int stream_idx;
    
    m_has_image_data = FALSE;
    m_has_depth_data = FALSE;
//  m_has_audio_data = FALSE;

    Loop {
        rc = openni::OpenNI::waitForAnyStream(m_streams, m_stream_num, &stream_idx, 0);
        if (rc==openni::STATUS_OK) {
            if (stream_idx==OPENNI2_IMAGE_INDEX) {
                if (image!=NULL) {
                    image->readFrame(&imageFrame);
                    flag |= OPENNI2_IMAGE_FLAG;
                    m_has_image_data = TRUE;
                }
            }
            else if (stream_idx==OPENNI2_DEPTH_INDEX) {
                if (depth!=NULL) {
                    depth->readFrame(&depthFrame);
                    flag |= OPENNI2_DEPTH_FLAG;
                    m_has_depth_data = TRUE;
                }
            }
            /*
            else if (stream_idx==OPENNI2_AUDIO_INDEX) {
                if (audio!=NULL) {
                    audio->readFrame(&audioFrame);
                    flag |= OPENNI2_AUDIO_FLAG;
                    m_has_audio_data = TRUE;
                }
            }*/

            if ((flag & m_stream_flg)==m_stream_flg) break;
        }
    }
}


BOOL  COpenNi2Device::set_ImageData(void) 
{
    if (image!=NULL) {
//      image->readFrame(&imageFrame);
        if (imageFrame.isValid()) return TRUE;
    }
    return FALSE;
}


BOOL  COpenNi2Device::set_DepthData(void)
{
    if (depth!=NULL) {
        //depth->readFrame(&depthFrame);
        if (depthFrame.isValid()) return TRUE;      
    }
    return FALSE;
}


BOOL  COpenNi2Device::set_SceneData(void) 
{
    if (user!=NULL) {
        user->readFrame(&userFrame);
        if (userFrame.isValid()) return TRUE;
    }

    return FALSE;
}



/////////////////////////////////////////////////////////////////////////////////////
// External Support Functions
//

// need ./OpenNI2/Drivers/*.dll
//
Buffer  jbxl::initialize_OpenNI2(void)
{
    Buffer mesg = init_Buffer();

    openni::Status rc = openni::OpenNI::initialize();
    if (rc!=openni::STATUS_OK) {
        mesg = make_Buffer(LMESG);
        copy_s2Buffer("ERROR: jbxl::initializeOpenNI2(): Initialize OpenNI Error!!\n", &mesg);
        cat_s2Buffer (openni::OpenNI::getExtendedError(), &mesg);
        DEBUG_WARN("WARNING: %s", mesg.buf);
    }
    else DEBUG_INFO("INFO: jbxl::initialize_OpenNI2(): initialize OpenNI2");

    return mesg;
}


void  jbxl::shutdown_OpenNI2(void)
{ 
    openni::OpenNI::shutdown();
    DEBUG_INFO("INFO: jbxl::shutdown_OpenNI2(): shutdown OpenNI2");
}


//
Buffer  jbxl::initialize_NiTE2(void)
{
    Buffer mesg = init_Buffer();
    
    nite::Status nc = nite::NiTE::initialize();
    if (nc!=nite::STATUS_OK) {
        mesg = make_Buffer(LMESG);
        copy_s2Buffer("ERROR: jbxl::initializeNiTE2(): Initialize NiTE Error!!\n", &mesg);
        DEBUG_WARN("WARNING: %s", mesg.buf);
    }
    else DEBUG_INFO("INFO: jbxl::initialize_NiTE2(): initialize NiTE2");

    return mesg;
}


void  jbxl::shutdown_NiTE2(void)
{ 
    nite::NiTE::shutdown();
    DEBUG_INFO("INFO: jbxl::shutdown_NiTE2(): shutdown NiTE2");
}

#endif      // ifdef ENABLE_OPENNI2x
