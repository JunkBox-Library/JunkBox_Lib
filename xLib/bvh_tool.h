#ifndef  __JBXL_BVH_TOOL_H_
#define  __JBXL_BVH_TOOL_H_

/**
@brief   BVHツールプログラム ヘッダ
@file    bvh_tool.h
@author  Fumi.Iseki (C)
*/

#include "ttree.h"
#include "matrix.h"

//
#define  BVH_NODE_HIERARCHY 0
#define  BVH_NODE_ROOT      1
#define  BVH_NODE_JOINT     2
#define  BVH_NODE_END       3

#define  BVH_STR_HIERARCHY  "HIERARCHY"
#define  BVH_STR_MOTION     "MOTION"

#define  BVH_STR_ROOT       "ROOT"
#define  BVH_STR_JOINT      "JOINT"
#define  BVH_STR_END        "End"

#define  BVH_STR_OFFSET     "OFFSET"
#define  BVH_STR_CHANNELS   "CHANNELS"

#define  BVH_STR_FRAMES     "Frames:"
#define  BVH_STR_FRAME      "Frame"
#define  BVH_STR_TIME       "Time:"

#define  BVH_STR_FRAME_TIME "Frame Time:"
#define  BVH_STR_PX         "Xposition"
#define  BVH_STR_PY         "Yposition"
#define  BVH_STR_PZ         "Zposition"
#define  BVH_STR_RX         "Xrotation"
#define  BVH_STR_RY         "Yrotation"
#define  BVH_STR_RZ         "Zrotation"


/*
hierarchy データ
    int     ldat.id     ノードの種別．BVH_NODE_HIERARCHY, BHV_NODE_ROOT, BVH_NODE_JOINT, BVH_NODE_END
    int     ldat.lv     CHANNELS 6/3
    int     ldat.sz     sizeof(vector)
    Buffer  ldat.key    関節名
    Buffer  ldat.val    自由度 "P#P#P#R#R#R#" の形式
    void*   ldat.ptr    OFFSET の vector へのポインタ
    tTree*  ldat.lst    NULL

    tTree*  next        子ノード（長女）へのポインタ
    tTree*  prev        親ノードへのポインタ
    tTree*  altp        その他へのポインタ（汎用） 
    tTree*  back        子ノード（末っ子）へのポインタ 
    tTree*  esis        前の姉妹（姉）ノードへのポインタ
    tTree*  ysis        次の姉妹（妹）ノードへのポインタ

    int     depth       深さ                    
    int     num         子ノードの数（全ての子ノードの数）  
    int     ctrl        制御用
    int     state       ノードの状態 
*/


struct _bvh_data
{
    int     joint_num;       ///< 関節の数
    int     frame_num;       ///< フレームの数
    double  frame_time;      ///< フレーム間の時間 sec
    int     framepsec;       ///< FPS = (int)(1.0/frame_time)
    int     channels;        ///< 総チャンネル数．チャンネル数の合計．モーションデータの数．
    int     state;           ///< データの状態
    
    tTree*  hierarchy;       ///< 関節部のツリーデータへのポインタ．上記参照．
    double* motion;          ///< MOTION データへのポインタ joint_num*frame_num
    vector* offset;          ///< 関節のOFFSET
};

typedef struct _bvh_data  BVHData;


///////////////////////////////////////////////////////////////////////////
// Basic

BVHData*    new_BVHData(void);
void        del_BVHData(BVHData** bvh);

void        init_BVHData (BVHData* bvh);
void        clear_BVHData(BVHData* bvh);

#define     free_BVHData(t)     del_BVHData(t)

///////////////////////////////////////////////////////////////////////////
// Tiny Tool

tList*      bvh_get_seq_data(FILE* fp);
tList*      bvh_find_line(FILE* fp, const char* key);

int         bvh_count_joints(tTree* tree);
int         bvh_count_channels(tTree* tree);
void        bvh_get_offset(tTree* tree, vector** vect);
void        bvh_get_parameter(BVHData* bvh);

void        _bvh_recsv_get_parameter(tTree* tree, BVHData* bvh);
void        _bvh_space_format(FILE* fp, int depth);

///////////////////////////////////////////////////////////////////////////
// File I/O

BVHData*    bvh_read_file (const char* fn);
int         bvh_write_file(const char* fn, BVHData* bvh);

#define     bvh_read_data(f)    bvh_parse_data((f))

int         bvh_print_data(FILE* fp, BVHData* bvh);
int         bvh_print_hierarchy(FILE* fp, BVHData* bvh);
int         bvh_print_motion(FILE* fp, BVHData* bvh);

int         _bvh_recsv_print_hierarchy(FILE* fp, tTree* tree);

///////////////////////////////////////////////////////////////////////////
// Parser

BVHData*    bvh_parse_data(FILE* fp);
tTree*      bvh_parse_hierarchy(tTree* tree, FILE* fp);
void        bvh_parse_motion(BVHData* bvh, FILE* fp);


#endif
