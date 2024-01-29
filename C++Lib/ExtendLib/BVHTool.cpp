/**
@brief    BVH用 ツール
@file     BVHTool.cpp
@author   Fumi.Iseki (C)
*/

#include  "BVHTool.h"


using namespace jbxl;


/////////////////////////////////////////////////////////////////////////////////////

CBVHTool::~CBVHTool(void)
{ 
    DEBUG_INFO("DESTRUCTOR: CBVHTool");

    free_data();
}


void  CBVHTool::init()
{
    // BVHData
    joint_num   = 0;    // 関節の数（End Site を含まない）
    frame_num   = 0;    // フレームの数
    frame_time  = 0.0;  // フレーム間の時間 s
    framepsec   = 0;    // FPS
    channels    = 0;    // 総チャンネル数．チャンネル数の合計．モーションデータの数．
    state       = 0;    // データの状態
    
    hierarchy   = NULL; // 関節部のツリーデータへのポインタ．上記参照．
    motion      = NULL; // MOTION データへのポインタ joint_num*frame_num
    offset      = NULL; // 関節のOFFSET
    //
    channel_num = NULL;
    channel_idx = NULL;
    joint_name  = NULL;

    flex_joint  = init_Buffer();
}


void  CBVHTool::free_data(void)
{
    state       = 0;
    joint_num   = 0;
    channels    = 0;
    frame_num   = 0;
    frame_time  = 0.0;
    framepsec   = 0;

    if (hierarchy!=NULL) {
        del_tTree(&hierarchy);
        hierarchy = NULL;
    }
    if (motion!=NULL) {
        free(motion);
        motion = NULL;
    }
    if (offset!=NULL) {
        free(offset);
        offset = NULL;
    }

    clear_data();
}


void  CBVHTool::clear_data(void)
{
    if (joint_name!=NULL) {
        for (int j=0; j<joint_num; j++) {
            free_Buffer(&joint_name[j]);
        }
        free(joint_name);
    }
    joint_name = NULL;

    if (channel_num!=NULL) {
        free(channel_num);
        channel_num = NULL;
    }

    if (channel_idx!=NULL) {
        free(channel_idx);
        channel_idx = NULL;
    }

    free_Buffer(&flex_joint);
}


void  CBVHTool::copy_bvh_data(BVHData* bvh)
{
    if (bvh==NULL) return;

    joint_num   = bvh->joint_num;
    frame_num   = bvh->frame_num;
    frame_time  = bvh->frame_time;
    framepsec   = bvh->framepsec;
    channels    = bvh->channels;
    state       = bvh->state;
    
    hierarchy   = bvh->hierarchy;
    motion      = bvh->motion;
    offset      = bvh->offset;
}


BVHData  CBVHTool::setback_bvh_data(void)
{
    BVHData bvh;

    set_offset();

    bvh.joint_num   = joint_num;
    bvh.frame_num   = frame_num;
    bvh.frame_time  = frame_time;
    bvh.framepsec   = framepsec;
    bvh.channels    = channels;
    bvh.state       = state;
    
    bvh.hierarchy   = hierarchy;
    bvh.motion      = motion;
    bvh.offset      = offset;

    return bvh;
}



/////////////////////////////////////////////////////////////////////////////////////////
//

BOOL  CBVHTool::readFile(char* fname)
{
    state = BVH_ERR_INVLD_ARGS;
    if (fname==NULL || fname[0]=='\0') return FALSE;

    unsigned long int sz = file_size(fname);
    if (sz==0) return FALSE;

    state = BVH_ERR_FAIL_FOPEN;
    FILE* fp = fopen(fname, "r");
    if (fp==NULL) return FALSE;

    BOOL ret = readBVH(fp);

    return ret;
}

    
BOOL  CBVHTool::readBVH(FILE* fp)
{
    state = BVH_ERR_INVLD_ARGS;
    if (fp==NULL) return FALSE;

    state = BVH_ERR_FAIL_OP;
    BVHData* ptr = bvh_read_data(fp);
    if (ptr==NULL) return FALSE;
    if (ptr->state!=0) {
        del_BVHData(&ptr);
        return FALSE;
    }
    //
    copy_bvh_data(ptr);
    get_bvh_params();

    return TRUE;
}


BOOL  CBVHTool::writeFile(char* fname)
{
    state = BVH_ERR_INVLD_ARGS;
    if (fname==NULL || fname[0]=='\0') return FALSE;

    state = 0;
    BVHData bvh_data = setback_bvh_data();
    state = bvh_write_file(fname, &bvh_data);
    if (state!=0) return FALSE;

    return TRUE;
}


/**
BOOL  CBVHTool::writeMultiFile(char* fname, int sec, int space)

sec秒区切りで．BVHファイルを出力する．のりしろ(フレーム数)付き．

@param fname  出力ファイル名のベース．fname_00 〜 fname_99 のファイルが出力される．
@param sec    1ファイルの最大再生時間（秒）．
@param space  のりしろのフレーム数．
@retval TRUE  成功
@retval FALSE 失敗
*/
BOOL  CBVHTool::writeMultiFile(char* fname, int sec, int space)
{
    state = BVH_ERR_INVLD_ARGS;
    if (fname==NULL || fname[0]=='\0') return FALSE;

    int echfrm = (int)(sec/frame_time);         // 1ファイルのフレーム数
    int num = (frame_num + echfrm - 1)/echfrm;  // 出力ファイルの数
    if (num==0 || num>99) return FALSE;

    //
    char* fn = dup_str(fname);
    char* et = get_file_extension(fn);
    if (et!=NULL) et[-1] = '\0';

    Buffer format = make_Buffer_bystr(fn);
    cat_s2Buffer("_%02d", &format);
    if (et!=NULL) {
        cat_s2Buffer(".", &format);
        cat_s2Buffer(et,  &format);
    }
    ::free(fn);

    //
    state = 0;
    BVHData bvh_data = setback_bvh_data();

    for (int i=0; i<num; i++) {
        //
        int outfrm = i*echfrm;                          // 今まで出力した有効フレーム数

        if (i==num-1) {
            bvh_data.frame_num = frame_num - outfrm;    // 残りのフレーム数．のりしろは使用しない．
        }
        else if (i==num-2) {
            if ((num-1)*echfrm + space > frame_num) {   // のりしろが大きすぎる
                space = frame_num - (num-1)*echfrm;
            }
            bvh_data.frame_num = echfrm + space;
        }
        else {
            bvh_data.frame_num = echfrm + space;
        }

        if (i!=0 && space>0) {
            bvh_data.frame_num++;                               //
            bvh_data.motion = &(motion[(outfrm-1)*channels]);   // -1: のりしろ
            if (channel_num[0]==6) {                            // ROOT のチャンネル数
                bvh_data.motion[0] = motion[0];
                bvh_data.motion[1] = motion[1];
                bvh_data.motion[2] = motion[2];
                bvh_data.motion[3] = 0.0;
                bvh_data.motion[4] = 0.0;
                bvh_data.motion[5] = 0.0;
            }
        }
        else {
            bvh_data.motion = &(motion[outfrm*channels]);
        }
        
        //
        char* fn = numbering_name((char*)format.buf, i);
        state = bvh_write_file(fn, &bvh_data);
        free(fn);
        if (state!=0) break;
    }
    
    free_Buffer(&format);
    
    if (state!=0) return FALSE;
    return TRUE;
}


BOOL  CBVHTool::printBVH(FILE* fp)
{
    state = BVH_ERR_INVLD_ARGS;
    if (fp==NULL) return FALSE;

    state = 0;
    BVHData bvh_data = setback_bvh_data();
    state = bvh_print_data(fp, &bvh_data);
    if (state!=0) return FALSE;

    return TRUE;
}


void  CBVHTool::setHierarchy(tTree* hrchy)
{
    free_data();

    hierarchy = hrchy;
    joint_num = bvh_count_joints(hierarchy);
    channels  = bvh_count_channels(hierarchy);

    get_bvh_params();
}


void  CBVHTool::set_offset(void)
{
    if (offset==NULL) return;

    int jnum = 0;
    _set_offset(hierarchy, &jnum);
}


void  CBVHTool::_set_offset(tTree* tree, int* jnum)
{
   if (tree!=NULL) {
        while(tree->esis!=NULL) tree = tree->esis;
  
        do {
            tList_data* ld = &(tree->ldat);

            /////////////////////////////////////////
            if (ld->ptr==NULL) {
                ld->sz  = sizeof(vector);
                ld->ptr = (void*)malloc(ld->sz);
            }
            *((vector*)ld->ptr) = offset[*jnum];
            (*jnum)++;
            /////////////////////////////////////////

            if (tree->next!=NULL) _set_offset(tree->next, jnum);
            tree = tree->ysis;
            //
        } while(tree!=NULL);
    }
    return;
}



/////////////////////////////////////////////////////////////////////////////////////////
//

/**
CBVHTool::get_bvh_params()

既存の hierarchyデータから以下のデータを得る．
    channel_num, channel_idx, flex_joint, joint_name
*/
void  CBVHTool::get_bvh_params()
{
    size_t  len = sizeof(int)*joint_num;
    channel_num = (int*)malloc(len);
    channel_idx = (int*)malloc(len);
    //
    if (channel_num==NULL || channel_idx==NULL) {
        if (channel_num!=NULL) free(channel_num);
        if (channel_idx!=NULL) free(channel_idx);
        channel_num = NULL;
        channel_idx = NULL;
        return;
    }
    memset(channel_num, 0, sizeof((int)len));
    memset(channel_idx, 0, sizeof((int)len));

    //
    flex_joint = make_Buffer(joint_num*6);
    joint_name = (Buffer*)malloc(sizeof(Buffer)*joint_num);

    int* ch = channel_num;
    Buffer* jn = joint_name;

    _get_bvh_params(hierarchy, &ch, &jn);

    channel_idx[0] = 0;
    for (int j=1; j<joint_num; j++) {
        channel_idx[j] = channel_idx[j-1] + channel_num[j-1];   
    }
    return;
}


void  CBVHTool::_get_bvh_params(tTree* tree, int** ch, Buffer** jn)
{
    if (tree!=NULL) {
        while(tree->esis!=NULL) tree = tree->esis;
  
        do {
            tList_data ld = tree->ldat;

            /////////////////////////////////////////
            if ((*ch)!=NULL) {
                **ch = ld.lv;
                (*ch)++;
            }
            cat_Buffer(&ld.val, &flex_joint);

            **jn = dup_Buffer(ld.key);
            (*jn)++;
            /////////////////////////////////////////

            if (tree->next!=NULL) {
                _get_bvh_params(tree->next, ch, jn);
            }
            tree = tree->ysis;
            //
        } while(tree!=NULL);
    }
    return;
}



/////////////////////////////////////////////////////////////////////////////////////////
//

Vector<double>*  CBVHTool::getPosData(int frame)
{
    if (frame<0 || frame>=frame_num) return NULL;

    size_t len = sizeof(Vector<double>)*joint_num;
    Vector<double>* vect = (Vector<double>*)malloc(len);
    if (vect==NULL) return NULL;
    memset(vect, 0, len);

    int frm = frame*channels;

    for (int j=0; j<joint_num; j++) {
        //
        BOOL  exstf = FALSE;
        int   chnum = channel_num[j];
        int   index = channel_idx[j];
        char* tpchr = (char*)&flex_joint.buf[index*2];

        for (int i=0; i<chnum; i++) {
            if (tpchr[i*2]=='P') {
                exstf = TRUE;
                index = frm + index + i;
                tpchr+= i*2;
                break;
            }
        }
        //
        vect[j].set(0.0, 0.0, 0.0);

        if (exstf) {
            for (int k=0; k<3; k++) {
                int k2 = k*2;
                if (tpchr[k2]=='P') {
                    if      (tpchr[k2+1]=='X') vect[j].x = motion[index+k];
                    else if (tpchr[k2+1]=='Y') vect[j].y = motion[index+k];
                    else if (tpchr[k2+1]=='Z') vect[j].z = motion[index+k];
                }
            }
        }
    }
    return vect;
}


Quaternion<double>*  CBVHTool::getQuaternion(int frame)
{
    if (frame<0 || frame>=frame_num) return NULL;

    size_t len = sizeof(Quaternion<double>)*joint_num;
    Quaternion<double>* quat = (Quaternion<double>*)malloc(len);
    if (quat==NULL) return NULL;

    Vector<double> ex(1.0, 0.0, 0.0, 1.0);  
    Vector<double> ey(0.0, 1.0, 0.0, 1.0);  
    Vector<double> ez(0.0, 0.0, 1.0, 1.0);
    
    double d2r = PI/180.0;
    int frm = frame*channels;
    //
    for (int j=0; j<joint_num; j++) {
        //
        BOOL  exstf = FALSE;
        int   chnum = channel_num[j];
        int   index = channel_idx[j];
        char* tpchr = (char*)&flex_joint.buf[index*2];

        quat[j].set(1.0, 0.0, 0.0, 0.0, 1.0);
        //
        int chfwd = 0;
        for (int i=0; i<chnum; i++) {
            if (tpchr[i*2]=='R') {
                exstf = TRUE;               // 回転用データの存在
                index = frm + index + i;    // MOTION データの位置
                tpchr+= i*2;                // チャンネルインデックスの位置
                chfwd = i;                  // 処理済チャンネル数
                break;
            }
        }
        //
        if (exstf) {
            Quaternion<double> q1(1.0, 0.0, 0.0, 0.0, 1.0);
            Quaternion<double> q2(1.0, 0.0, 0.0, 0.0, 1.0);
            Quaternion<double> q3(1.0, 0.0, 0.0, 0.0, 1.0);
            
            // X->Y, Y->Z, Z->X
            double th = motion[index]*d2r;
            if      (tpchr[1]=='X') q3.setRotation(th, ey); 
            else if (tpchr[1]=='Y') q3.setRotation(th, ez);
            else if (tpchr[1]=='Z') q3.setRotation(th, ex);

            chnum = chnum - chfwd - 1;  // 残りの回転用チャンネル数
            if (chnum>0 && tpchr[2]=='R') {
                th = motion[index+1]*d2r;
                if      (tpchr[3]=='X') q2.setRotation(th, ey);
                else if (tpchr[3]=='Y') q2.setRotation(th, ez);
                else if (tpchr[3]=='Z') q2.setRotation(th, ex);

                chnum--;
                if (chnum>0 && tpchr[4]=='R') {
                    th = motion[index+2]*d2r;
                    if      (tpchr[5]=='X') q1.setRotation(th, ey);
                    else if (tpchr[5]=='Y') q1.setRotation(th, ez);
                    else if (tpchr[5]=='Z') q1.setRotation(th, ex);
                }
            }
            quat[j] = q3*q2*q1;
        }
    }
    return quat;
}


Vector<double>*  CBVHTool::getPosOffset(void)
{
    size_t len = sizeof(Vector<double>)*joint_num;
    Vector<double>* vect = (Vector<double>*)malloc(len);

    if (vect==NULL) return NULL;
    memset(vect, 0, len);

    for (int j=0; j<joint_num; j++) {
        vector vt = offset[j];
        vect[j].x = vt.x;
        vect[j].y = vt.y;
        vect[j].z = vt.z;
        vect[j].n = vt.n;
    }
    return vect;
}



////////////////////////////////////////////////////////////////////////////
// Pending

/*
Matrix<double>*  CBVHTool::getRotMatrix(int frame)
{
    if (frame<0 || frame>=frame_num) return NULL;

    size_t len = sizeof(Matrix<double>)*joint_num;
    Matrix<double>* mtrx = (Matrix<double>*)malloc(len);
    if (mtrx==NULL) return NULL;

    double d2r = PI/180.0;
    int frm = frame*bvh_data->channels;

    for (int j=0; j<joint_num; j++) {
        //
        BOOL  exstf = FALSE;
        int   chnum = channel_num[j];
        int   index = channel_idx[j];
        char* tpchr = (char*)&flex_joint.buf[index*2];

        for (int i=0; i<chnum; i++) {
            if (tpchr[i*2]=='R') {
                exstf = TRUE;
                index = frm + index + i;
                tpchr+= i*2;
                break;
            }
        }
        if (exstf) {
            if (tpchr[2]!='R' || tpchr[4]!='R') {
                exstf = FALSE;  // not supported
            }
        }

        if (exstf) {
            //
            Vector<double> e;

            if (tpchr[1]=='X' && tpchr[3]=='Y' && tpchr[5]=='Z') {
                // ZYX -> XZY
                PRINT_MESG("WARNING: CBVHTool::getRotMatrix: Not Implemeted Function is called. XYZ\n");
                mtrx[j].init(2, 3, 3);
            }

            else if (tpchr[1]=='X' && tpchr[3]=='Z' && tpchr[5]=='Y') {
                // YZX -> ZXY
                PRINT_MESG("WARNING: CBVHTool::getRotMatrix: Not Implemeted Function is called. XZY\n");
                mtrx[j].init(2, 3, 3);
            }

            else if (tpchr[1]=='Y' && tpchr[3]=='X' && tpchr[5]=='Z') {
                // ZXY -> XYZ
                e.z = bvh_data->motion[index]  *d2r;
                e.y = bvh_data->motion[index+1]*d2r;
                e.x = bvh_data->motion[index+2]*d2r;
                mtrx[j] = EulerXYZ2RotMatrix(e);
            }

            else if (tpchr[1]=='Y' && tpchr[3]=='Z' && tpchr[5]=='X') {
                // XZY -> YXZ 
                PRINT_MESG("WARNING: CBVHTool::getRotMatrix: Not Implemeted Function is called. YZX\n");
                mtrx[j].init(2, 3, 3);
            }

            else if (tpchr[1]=='Z' && tpchr[3]=='X' && tpchr[5]=='Y') {
                // YXZ -> ZYX 
                e.x = bvh_data->motion[index]  *d2r;
                e.y = bvh_data->motion[index+1]*d2r;
                e.z = bvh_data->motion[index+2]*d2r;
                mtrx[j] = EulerZYX2RotMatrix(e);
            }

            else if (tpchr[1]=='Z' && tpchr[3]=='Y' && tpchr[5]=='X') {
                // XYZ -> YZX
                PRINT_MESG("WARNING: CBVHTool::getRotMatrix: Not Implemeted Function is called. ZYX\n");
                mtrx[j].init(2, 3, 3);
            }

            else {
                mtrx[j].init(2, 3, 3);      // 3x3行列
            }
        }
        else {
            mtrx[j].init(2, 3, 3);          // 3x3行列
        }
    }
    return mtrx;
}

*/

