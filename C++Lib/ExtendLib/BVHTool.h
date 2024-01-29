#ifndef  __JBXL_CPP_BVH_TOOL_H_
#define  __JBXL_CPP_BVH_TOOL_H_

#include  "tools++.h"
#include  "bvh_tool.h"
#include  "Vector.h"
#include  "Rotation.h"

//
#define  BVH_ERR_FAIL_WRT_MOTION    -1        // モーションデータの書き込み失敗
#define  BVH_ERR_FAIL_WRT_HIRCHY    -2        // ジョイントデータの書き込み失敗
#define  BVH_ERR_FAIL_FOPEN         -3        // ファイルのオープンに失敗
#define  BVH_ERR_INVLD_ARGS         -4        // 不正な引数（入力値）
#define  BVH_ERR_FAIL_OP            -5        // 処理失敗


namespace jbxl {


class  CBVHTool : public BVHData
{
public:
    CBVHTool(void)  { init();}
    virtual ~CBVHTool(void);

public:
//
//    int           joint_num;        // ジョイントの数
//    int           frame_num;        // フレームの数
//    double        frame_time;       // フレーム間の時間 sec
//    int           framepsec;        // FPS = (int)(1.0/frame_time)
//    int           channels;         // 総チャンネル数．チャンネル数の合計．モーションデータの数．
//    int           state;            // データの状態
    
//    tTree*        hierarchy;        // ジョイント部のツリーデータへのポインタ．上記参照．
//    double*       otion;            // MOTION データへのポインタ joint_num*frame_num
//    vector*       offset;           // ジョイントのOFFSET

//    Buffer*       joint_name;       // ジョイントの名前の配列

public:
    //
    void            init(void);
    void            free_data (void);
    void            clear_data(void);

    void            copy_bvh_data(BVHData* bvh);
    BVHData         setback_bvh_data(void);

    BOOL            readFile (char* fname);
    BOOL            writeFile(char* fname);
    BOOL            writeMultiFile(char* fname, int sec, int space);
    
    BOOL            readBVH (FILE* fp);
    BOOL            printBVH(FILE* fp);

    void            setHierarchy(tTree* hrchy);

    Vector<double>*     getPosOffset(void);
    Vector<double>*     getPosData(int frame);
    Quaternion<double>* getQuaternion(int frame);

//    Matrix<double>*   getRotMatrix(int frame);

private:

public:
    //
    Buffer*         joint_name;     // ジョイントの名前の配列
    int*            channel_num;    // 各ジョイントのチャンネル数
    int*            channel_idx;    // 各ジョイントのチャンネルの位置（累計）
    Buffer          flex_joint;     // 各ジョイントのチャンネル種別  ex) "PXPYPZRXRYRZRXRYRZ...."

private:
    //
    void            set_offset(void);
    void            _set_offset(tTree* tree, int* jnum);

    void            get_bvh_params(void);
    void            _get_bvh_params(tTree* tree, int** ch, Buffer** jn);
};



}        // namespace

#endif
