/**



*/


#include  "OpenNiDevice.h"
#include  "tools++.h"


using namespace jbxl;



#ifdef  ENABLE_OPENNI



COpenNiDevice::COpenNiDevice(void)
{
    context      = NULL;
    image        = NULL;
    user         = NULL;
    depth        = NULL;
    player       = NULL;

    imageMD      = NULL;
    depthMD      = NULL;
    sceneMD      = NULL;

    m_enu_dev    = EnuDev_Unknown;
    m_has_camera = TRUE;
    m_err_mesg   = make_Buffer(LMESG);

    m_state      = NI_STATE_DETECT_STOPPED;
    m_xsize      = OPENNI_DEPTH_XSIZE;
    m_ysize      = OPENNI_DEPTH_YSIZE;
    m_nfps       = OPENNI_DEPTH_FPS;

    //
    recorder     = NULL;
    skeleton     = NULL;
//  pose         = NULL;

    usb_dev      = NULL;
    devPath      = NULL;

    userCallbacks   = NULL;
    poseCallbacks   = NULL;
    calibCallbacks  = NULL;

    memset(dUsers, 0, sizeof(XnUserID)*OPENNI_USERS_NUM);
}



BOOL  COpenNiDevice::init(BOOL use_image)
{
    outputMode.nXRes = OPENNI_DEPTH_XSIZE; 
    outputMode.nYRes = OPENNI_DEPTH_YSIZE; 
    outputMode.nFPS  = OPENNI_DEPTH_FPS; 

    m_enu_dev = EnuDev_None;

    BOOL ret = create_Context();
    if (ret && use_image) create_Image();
    if (ret) ret = create_Depth();
    if (ret) m_enu_dev = EnuDev_PS1080;

    return ret;
}



BOOL  COpenNiDevice::init(char* fname, BOOL use_image)
{
    m_enu_dev = EnuDev_None;

    if (fname==NULL) {
        copy_s2Buffer("COpenNiDevice::init ERROR: File name is NULL", &m_err_mesg);
        return FALSE;
    }

    //
    BOOL ret = create_Context(fname);
    if (ret) ret = create_Player(TRUE);
    if (!ret) {
        copy_s2Buffer("COpenNiDevice::init ERROR: create error!!", &m_err_mesg);
        return FALSE;
    }
    
    //
    xn::NodeInfoList nodeList;
    player->EnumerateNodes(nodeList);

    for (xn::NodeInfoList::Iterator node=nodeList.Begin(); node!=nodeList.End(); node++) {
        if ((*node).GetDescription().Type==XN_NODE_TYPE_IMAGE) {
            if (use_image) {
                ret = create_Image(TRUE);
                if (!ret) break;
            }
        }
        if ((*node).GetDescription().Type==XN_NODE_TYPE_DEPTH) {
            ret = create_Depth(TRUE);
            if (!ret) break;
        }
    }

    //
    if (image!=NULL && use_image) {
        image->GetMapOutputMode(outputMode);
    }
    else if (depth!=NULL) {
        depth->GetMapOutputMode(outputMode);
    }
    else {
        copy_s2Buffer("COpenNiDevice::init ERROR: No Generators", &m_err_mesg);
        return FALSE;
    }

    m_enu_dev = EnuDev_OniFile;
    return TRUE;
}



void  COpenNiDevice::free(void)
{
    clear_Tracking();
    clear_CallBacks();
    clear_Skeleton();

    delete_User();
    delete_Depth();
    delete_Image();
    delete_Player();
    delete_Context();

    free_Buffer(&m_err_mesg);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Delete Generators

BOOL  COpenNiDevice::create_Context(char* fname)
{
    if (context==NULL) {
        context = new xn::Context();
        if (context==NULL) {
            copy_s2Buffer("COpenNiDevice::create_Context ERROR: context is NULL", &m_err_mesg);
            return FALSE;
        }
    }

    XnStatus rc = context->Init();
    if (rc==XN_STATUS_OK && fname!=NULL) rc = context->OpenFileRecording(fname);

    if (rc!=XN_STATUS_OK) {
        copy_s2Buffer("COpenNiDevice::create_Context ERROR: ", &m_err_mesg);
        cat_s2Buffer (::xnGetStatusString(rc), &m_err_mesg);
        return FALSE;
    }

    return TRUE;
}



BOOL  COpenNiDevice::create_Image(BOOL easy)
{
    if (!m_has_camera) return FALSE;

    if (context==NULL) {
        copy_s2Buffer("COpenNiDevice::create_Image ERROR: context is NULL", &m_err_mesg);
        return FALSE;
    }

    if (image==NULL) {
        image = new xn::ImageGenerator();
        if (image==NULL) {
            copy_s2Buffer("COpenNiDevice::create_Image ERROR: fail to create image generator", &m_err_mesg);
            m_has_camera = FALSE;
            return FALSE;
        }
    }

    XnStatus rc = XN_STATUS_ERROR;
    if (easy) {
        rc = context->FindExistingNode(XN_NODE_TYPE_IMAGE, *image);
    }
    else {
        rc = image->Create(*context);
        if (rc==XN_STATUS_OK) rc = image->StartGenerating();
        if (rc==XN_STATUS_OK) rc = image->SetMapOutputMode(outputMode); 
    }

    if (rc!=XN_STATUS_OK) {
        copy_s2Buffer("COpenNiDevice::create_Image ERROR: ", &m_err_mesg);
        cat_s2Buffer (::xnGetStatusString(rc), &m_err_mesg);
        delete_Image();
        return FALSE;
    }

    imageMD = new xn::ImageMetaData();
    //imageData = (uByte*)malloc(outputMode.nXRes*outputMode.nYRes*3);

    return TRUE;
}



BOOL  COpenNiDevice::create_Depth(BOOL easy)
{
    if (context==NULL) {
        copy_s2Buffer("COpenNiDevice::create_Depth ERROR: context is NULL", &m_err_mesg);
        return FALSE;
    }

    if (depth==NULL) {
        depth = new xn::DepthGenerator();
        if (depth==NULL) {
            copy_s2Buffer("COpenNiDevice::create_Depth ERROR: fail to create depth generator", &m_err_mesg);
            return FALSE;
        }
    }

    XnStatus rc = XN_STATUS_ERROR;
    if (easy) {
        rc = context->FindExistingNode(XN_NODE_TYPE_DEPTH, *depth);
    }
    else {
        rc = depth->Create(*context);
        if (rc==XN_STATUS_OK) rc = depth->StartGenerating();
        if (rc==XN_STATUS_OK) rc = depth->SetMapOutputMode(outputMode);
        if (rc==XN_STATUS_OK && image!=NULL) {
            rc = depth->GetAlternativeViewPointCap().SetViewPoint(*image);
            /*if (rc==XN_STATUS_OK && depth->IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC)) {
                if (depth->GetFrameSyncCap().CanFrameSyncWith(*image)) {
                    rc = depth->GetFrameSyncCap().FrameSyncWith(*image);
                }
            }*/
        }
    }
    
    if (rc!=XN_STATUS_OK) {
        copy_s2Buffer("COpenNiDevice::create_Depth ERROR: ", &m_err_mesg);
        cat_s2Buffer (::xnGetStatusString(rc), &m_err_mesg);
        delete_Depth();
        return FALSE;
    }

    depthMD = new xn::DepthMetaData();
    //depthData = (XnLabel*)malloc(outputMode.nXRes*outputMode.nYRes*sizeof(XnLabel));

    return TRUE;
}



BOOL  COpenNiDevice::create_User(BOOL easy)
{
    if (context==NULL) {
        copy_s2Buffer("COpenNiDevice::create_User ERROR: context is NULL", &m_err_mesg);
        return FALSE;
    }

    if (user==NULL) {
        user = new xn::UserGenerator();
        if (user==NULL) {
            copy_s2Buffer("COpenNiDevice::create_User ERROR: fail to create user generator", &m_err_mesg);
            return FALSE;
        }
    }

    XnStatus rc = XN_STATUS_ERROR;
    if (easy) {
        rc = context->FindExistingNode(XN_NODE_TYPE_USER, *user);
    }
    else {
        rc = user->Create(*context);
        if (rc==XN_STATUS_OK) rc = user->StartGenerating();
    }

    if (rc!=XN_STATUS_OK) {
        copy_s2Buffer("COpenNiDevice::create_User ERROR: ", &m_err_mesg);
        cat_s2Buffer (::xnGetStatusString(rc), &m_err_mesg);
        delete_User();
        return FALSE;
    }

    sceneMD = new xn::SceneMetaData();
    return TRUE;
}



BOOL  COpenNiDevice::create_Player(BOOL easy)
{
    if (context==NULL) {
        copy_s2Buffer("COpenNiDevice::create_Player ERROR: context is NULL", &m_err_mesg);
        return FALSE;
    }   

    if (player==NULL) {
        player = new xn::Player();
        if (player==NULL) {
            copy_s2Buffer("COpenNiDevice::create_Player ERROR: fail to create player generator", &m_err_mesg);
            return FALSE;
        }
    }

    XnStatus rc = XN_STATUS_ERROR;
    if (easy) {
        rc = context->FindExistingNode(XN_NODE_TYPE_PLAYER, *player);
    }
    else {
        rc = player->Create(*context, "oni");
    }
    
    if (rc!=XN_STATUS_OK) {
        copy_s2Buffer("COpenNiDevice::create_Player EROR: ", &m_err_mesg);
        cat_s2Buffer (::xnGetStatusString(rc), &m_err_mesg);
        delete_Player();
        return FALSE;
    }

    return TRUE;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Delete Generators

void  COpenNiDevice::delete_Context(void)
{
    if (context!=NULL) {
        context->Release();
        delete(context);
    }
    context = NULL;
}



void  COpenNiDevice::delete_Image(void)
{
    if (image!=NULL) {
        if (image->IsValid()) {
            if (image->IsGenerating()) image->StopGenerating();
        }
        image->Release();
        delete(image);
    }
    if (imageMD!=NULL) delete(imageMD);

    image   = NULL;
    imageMD = NULL;
}



void  COpenNiDevice::delete_Depth(void)
{
    if (depth!=NULL) {
        if (depth->IsValid()) {
            if (depth->IsGenerating()) depth->StopGenerating();
        }
        depth->Release();
        delete(depth);
    }
    if (depthMD!=NULL) delete(depthMD);

    depth   = NULL;
    depthMD = NULL;
}



void  COpenNiDevice::delete_User(void)
{
    if (user!=NULL) {
        if (user->IsValid()) {
            if (user->IsGenerating()) user->StopGenerating();
        }
        user->Release();
        delete(user);
    }
    if (sceneMD!=NULL) delete(sceneMD);

    user    = NULL;
    sceneMD = NULL;
}



void   COpenNiDevice::delete_Player(void)
{
    if (player!=NULL) {
        player->Release();
        delete(player);
        player = NULL;  
    }   
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create Capabilities

BOOL  COpenNiDevice::setup_Tracking(int profile, double smooth)
{
    if (user==NULL) {
        copy_s2Buffer("COpenNiDevice::setup_Tracking ERROR: user is NULL", &m_err_mesg);
        return FALSE;
    }
    if (!user->IsCapabilitySupported(XN_CAPABILITY_SKELETON)) {
        copy_s2Buffer("COpenNiDevice::setup_Tracking ERROR: not support skeleton", &m_err_mesg);
        return FALSE;
    }

    if (skeleton==NULL) {
        skeleton = new xn::SkeletonCapability(*user);
        if (skeleton==NULL) {
            copy_s2Buffer("COpenNiDevice::setup_Tracking ERROR: fail to make skeleton capability", &m_err_mesg);
            return FALSE;
        }
    }

    //
    skeleton->SetSkeletonProfile((XnSkeletonProfile)profile);
    skeleton->SetSmoothing((XnFloat)smooth);
    //
    return TRUE;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Clear Capabilities

//
void  COpenNiDevice::clear_Skeleton(void)
{
    if (skeleton!=NULL) delete(skeleton);
    skeleton = NULL;
}



void  COpenNiDevice::clear_Tracking(void)
{
    if (user!=NULL) {
        nUsers = OPENNI_USERS_NUM;
        user->GetUsers(dUsers, nUsers);
        for (int i=0; i<nUsers; i++) {
            if (skeleton!=NULL && skeleton->IsTracking(dUsers[i])) {
                skeleton->StopTracking(dUsers[i]);
            }
        }
    }
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
// File

BOOL  COpenNiDevice::create_Recorder(char* file_name, BOOL use_image)
{
    if (context==NULL) {
        copy_s2Buffer("COpenNiDevice::create_Recorder ERROR: context is NULL", &m_err_mesg);
        return FALSE;
    }   
    if (depth==NULL) {
        copy_s2Buffer("COpenNiDevice::create_Recorder ERROR: depth is NULL", &m_err_mesg);
        return FALSE;
    }

    recorder = new xn::Recorder();
    rc = recorder->Create(*context);
    if (rc==XN_STATUS_OK) rc = recorder->SetDestination(XN_RECORD_MEDIUM_FILE, file_name);
    if (rc==XN_STATUS_OK && use_image && image!=NULL) rc = recorder->AddNodeToRecording(*image, XN_CODEC_JPEG);
    if (rc==XN_STATUS_OK) rc = recorder->AddNodeToRecording(*depth, XN_CODEC_16Z);  
    
    if (rc!=XN_STATUS_OK) {
        copy_s2Buffer("COpenNiDevice::create_Recorder ERROR: ", &m_err_mesg);
        cat_s2Buffer (::xnGetStatusString(rc), &m_err_mesg);
        return FALSE;
    }

    return TRUE;
}



void   COpenNiDevice::delete_Recorder(void)
{
    if (recorder!=NULL) {
        int prev_state = m_state;
        m_state = NI_STATE_SAVE_WORKING;
        {
            Sleep(NI_WORKING_WAIT_TIME);
            if (depth!=NULL) recorder->RemoveNodeFromRecording(*depth);
            if (image!=NULL) recorder->RemoveNodeFromRecording(*image);
            //if (use_camera && image!=NULL) recorder->RemoveNodeFromRecording(*image);
            recorder->Release();
            delete(recorder);
        }
        m_state = prev_state;
    }   
    recorder = NULL;
}



BOOL  COpenNiDevice::start_Recorde(char* file_name, BOOL use_image)
{
    BOOL ret = FALSE;
    unlink(file_name);

    int prev_state = m_state;
    m_state = NI_STATE_SAVE_WORKING;
    {
        Sleep(NI_WORKING_WAIT_TIME);
        ret = create_Recorder(file_name, use_image);
        if (ret) {
            rc = recorder->Record();
            if (rc!=XN_STATUS_OK) {
                copy_s2Buffer("COpenNiDevice::start_Recorde ERROR: ", &m_err_mesg);
                cat_s2Buffer (::xnGetStatusString(rc), &m_err_mesg);
                ret = FALSE;
            }
        }
    }
    m_state = prev_state;

    if (!ret) {
        delete_Recorder();
        unlink(file_name);
    }
    return ret;
}



void  COpenNiDevice::stop_Recorde(void)
{
    delete_Recorder();
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
// USB

BOOL  COpenNiDevice::open_USB_Device(void)
{
    rc = ::xnUSBInit();
    if (rc!=XN_STATUS_OK && rc!=XN_STATUS_USB_ALREADY_INIT) return FALSE;
    
#ifdef WIN32

    XnUInt32 count;
    rc = ::xnUSBEnumerateDevices(NI_VID_MICROSOFT, NI_PID_NUI_MOTOR, (const XnUSBConnectionString**)&devPath, &count);
    if (rc!=XN_STATUS_OK) {
        ::xnUSBShutdown();
        return FALSE;
    }

    rc = ::xnUSBOpenDeviceByPath(*devPath, &usb_dev);
    if (rc!=XN_STATUS_OK) {
        ::xnUSBFreeDevicesList(devPath);
        return FALSE;
    }

#else

    rc = ::xnUSBOpenDevice(OPENNI_VID_MICROSOFT, OPENNI_PID_NUI_MOTOR, NULL, NULL, &usb_dev);
    if (rc!=XN_STATUS_OK) {
        ::xnUSBShutdown();
        return FALSE;
    }

#endif

    return TRUE;
}



void  COpenNiDevice::close_USB_Device(void)
{
    if (usb_dev!=NULL) {
        ::xnUSBCloseDevice(usb_dev);
        usb_dev = NULL;
    }

#ifdef WIN32

    if (devPath!=NULL) {
        ::xnUSBFreeDevicesList(devPath);
        devPath = NULL;
    }

#endif

    ::xnUSBShutdown();
}



void  COpenNiDevice::set_LED_Color(int col)
{
    if (usb_dev==NULL) return;

    ::xnUSBSendControl(usb_dev, XN_USB_CONTROL_TYPE_VENDOR, 0x06, (XnUInt16)col, 0, NULL, 0, 0);

    return;
}



void  COpenNiDevice::set_Tilt_Motor(int ang)
{
    if (usb_dev==NULL) return;
    //if (ang>30 || ang<-30) return;

    ::xnUSBSendControl(usb_dev, XN_USB_CONTROL_TYPE_VENDOR, 0x31, (XnUInt16)(ang*2), 0, NULL, 0, 0);

    
    Sleep(1000);
    return;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// コールバック

/// コールバック関数の登録
BOOL  COpenNiDevice::setup_CallBacks()
{
    BOOL     ret = set_UserCallbacks (&userDetect, &userLost, this);
    if (ret) ret = set_CalibCallbacks(&calibStart, &calibEnd, this);
    if (!ret) return FALSE;

    return TRUE;
}



void  COpenNiDevice::clear_CallBacks()
{
    unset_UserCallbacks();
    unset_CalibCallbacks();
}



BOOL  COpenNiDevice::set_UserCallbacks(xn::UserGenerator::UserHandler newUser, xn::UserGenerator::UserHandler lostUser, void* cookie)
{
    if (user==NULL) {
        copy_s2Buffer("COpenNiDevice::set_UserCallbacks ERROR: user is NULL", &m_err_mesg);
        return FALSE;
    }

    rc = user->RegisterUserCallbacks(newUser, lostUser, cookie, userCallbacks);
    if (rc!=XN_STATUS_OK) {
        copy_s2Buffer("COpenNiDevice::set_UserCallbacks ERROR: ", &m_err_mesg);
        cat_s2Buffer (::xnGetStatusString(rc), &m_err_mesg);
        unset_UserCallbacks();
        return FALSE;
    }
    return TRUE;
}



BOOL  COpenNiDevice::set_CalibCallbacks(xn::SkeletonCapability::CalibrationStart calibStart, xn::SkeletonCapability::CalibrationEnd calibEnd, void* cookie)
{
    if (skeleton==NULL) {
        copy_s2Buffer("COpenNiDevice::set_CalibCallbacks ERROR: skeleton is NULL", &m_err_mesg);
        return FALSE;
    }

    rc = skeleton->RegisterCalibrationCallbacks(calibStart, calibEnd, cookie, calibCallbacks);
    if (rc!=XN_STATUS_OK) {
        copy_s2Buffer("COpenNiDevice::set_CalibCallbacks ERROR: ", &m_err_mesg);
        cat_s2Buffer (::xnGetStatusString(rc), &m_err_mesg);
        unset_CalibCallbacks();
        return FALSE;
    }
    return TRUE;
}



void  COpenNiDevice::unset_UserCallbacks()
{
    if (user!=NULL && userCallbacks!=NULL) {
        user->UnregisterUserCallbacks(userCallbacks);
        userCallbacks = NULL;
    }
    return;
}



void  COpenNiDevice::unset_CalibCallbacks()
{
    if (skeleton!=NULL && calibCallbacks!=NULL) {
        skeleton->UnregisterCalibrationCallbacks(calibCallbacks);
        calibCallbacks = NULL;
    }
    return;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
// External Callback Functions
//

void  XN_CALLBACK_TYPE jbxl::userDetect(xn::UserGenerator& user, unsigned int nId, void* cookie)
{
    DEBUG_INFO("jbxl::userDetect(): User is detected. (%d)", nId);
    if (cookie==NULL) return;
    
    user.GetSkeletonCap().RequestCalibration(nId, TRUE);
    
    DEBUG_INFO("jbxl::userDetect(): Start Pose Detection. (%d)", nId);
}



void  XN_CALLBACK_TYPE jbxl::userLost(xn::UserGenerator& user, unsigned int nId, void* cookie)
{
    DEBUG_INFO("jbxl::userLost(): Lost user. (%d)", nId);

    if (user.GetSkeletonCap().IsValid()) {
        if (user.GetSkeletonCap().IsTracking(nId)) {
            user.GetSkeletonCap().StopTracking(nId);
            DEBUG_INFO("jbxl::userLost(): Stop Tracking. (%d)", nId);
        }
    }
}



// Calibration

void  XN_CALLBACK_TYPE jbxl::calibStart(xn::SkeletonCapability& skeleton, unsigned int nId, void* cookie)
{
    DEBUG_INFO("jbxl::calibStart(): Start Calibration. (%d)", nId);
}



void  XN_CALLBACK_TYPE jbxl::calibEnd(xn::SkeletonCapability& skeleton, unsigned int nId, XnBool success, void* cookie)
{
    DEBUG_INFO("jbxl::calibEnd(): End Calibration. (%d)", nId);
    if (cookie==NULL) return;

    //
    xn::UserGenerator* user = ((COpenNiDevice*)cookie)->user;
    if (user!=NULL) {
        if (success) {
            if (user!=NULL) {
                user->GetSkeletonCap().StartTracking(nId);
                DEBUG_INFO("jbxl::calibEnd(): Start Tracking. (%d)", nId);
            }
        }
    }
}


#endif      // ENABLE_OPENNI
