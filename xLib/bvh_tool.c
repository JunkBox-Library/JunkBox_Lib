/**
@brief   BVHツールプログラム
@file    bvh_tool.c
@author  Fumi.Iseki (C)
*/

#include "xtools.h"
#include "bvh_tool.h"


///////////////////////////////////////////////////////////////////////////////////
// Basic Function of BVHData
//

BVHData*   new_BVHData(void)
{
    BVHData* bvh = NULL;

    bvh = (BVHData*)malloc(sizeof(BVHData));
    if (bvh==NULL) return NULL;
    
    init_BVHData(bvh);

    return bvh;
}


void  del_BVHData(BVHData** bvh)
{
    if (bvh==NULL || *bvh==NULL) return;

    clear_BVHData(*bvh);
    free(*bvh);

    *bvh = NULL;
    return;
}


void  init_BVHData(BVHData* bvh)
{
    if (bvh==NULL) return;

    memset(bvh, 0, sizeof(BVHData));
    bvh->state = JBXL_NORMAL;

    return;
}


void  clear_BVHData(BVHData* bvh)
{
    if (bvh==NULL) return;

    if (bvh->hierarchy!=NULL) {
        del_tTree(&bvh->hierarchy);
        bvh->hierarchy = NULL;
    }
    if (bvh->motion!=NULL) {
        free(bvh->motion);
        bvh->motion = NULL;
    }
    if (bvh->offset!=NULL) {
        free(bvh->offset);
        bvh->offset = NULL;
    }

    init_BVHData(bvh);

    return;
}



///////////////////////////////////////////////////////////////////////////////////
// Tiny Tools 
//

/**
tList*  bvh_get_seq_data(FILE* fp)

fp の一行分のデータに含まれる項目を空白を区切りにしてリストに格納する．

@param fp   読み出しファイルの識別子
@return     アイテムを格納したリストへのポインタ．失敗した場合は NULL
*/
tList*  bvh_get_seq_data(FILE* fp)
{
    char* pp;
    UNUSED(pp);

    if (fp==NULL) return NULL;

    char line[LBUF+1];
    line[LBUF] = '\0';

    pp = fgets(line, LBUF, fp);
    char*  pack = pack_char(line, ' ');
    if (pack==NULL) return NULL;
    //
    replace_str(pack, LBUF, " :", ":");
    //replace_str(pack, LBUF, " {", "\n{\n");
    //replace_str(pack, LBUF, " }", "\n}\n");

    tList* list = awk_tList(pack, ' ');
    free(pack);

    return list;
}


/**
tList*  bvh_find_line(FILE* fp, const char* key)

fp のファイルから keyで始まる行を探し出し，その行に含まれる項目を空白を区切りにしてリストに格納する．

@param fp   ファイルポインタ
@param key  検索する文字列．
@return     アイテムを格納したリストへのポインタ．失敗した場合は NULL
*/
tList*  bvh_find_line(FILE* fp, const char* key)
{
    if (fp==NULL || key==NULL) return NULL;

    tList* list = bvh_get_seq_data(fp);

    while (!feof(fp)) {
        if (!strcasecmp((char*)list->ldat.key.buf, key)) return list;

        del_tList(&list);
        list = bvh_get_seq_data(fp);
    }

    return NULL;
}


int  bvh_count_channels(tTree* pp)
{
    int cnt = 0;

    if (pp==NULL) return 0;
    while(pp->esis!=NULL) pp = pp->esis;

    do {
        cnt += pp->ldat.lv;
        if (pp->next!=NULL) cnt += bvh_count_channels(pp->next);
        pp = pp->ysis;
    } while(pp!=NULL);

    return cnt;
}


int  bvh_count_joints(tTree* pp)
{
    int cnt = 0;

    if (pp==NULL) return 0;
    while(pp->esis!=NULL) pp = pp->esis;

    do {
        cnt++;
        if (pp->next!=NULL) cnt += bvh_count_joints(pp->next);
        pp = pp->ysis;
    } while(pp!=NULL);

    return cnt;
}


/**
void  bvh_get_offset(tTree* pp, vector** vect)

OFFSETの値を順に取り出して *vect[] に格納する．

@param pp   Hierarchy データへのポインタ．
@param vect OFFSETを格納する vect[]へのポインタ．vect[]は予め領域を確保しておく．
*/
void  bvh_get_offset(tTree* pp, vector** vect)
{
    if (pp==NULL) return;

    while(pp->esis!=NULL) pp = pp->esis;

    do {
        if (*vect!=NULL) {
            **vect = *(vector*)pp->ldat.ptr;
            (*vect)++;
        }

        if (pp->next!=NULL) bvh_get_offset(pp->next, vect);
        pp = pp->ysis;
    } while(pp!=NULL);

    return;
}


/**
void  bvh_get_parameter(BVHData* bvh)

hierarchy データから joint_num と channelsを設定する．
*/
void  bvh_get_parameter(BVHData* bvh)
{
    if (bvh==NULL || bvh->hierarchy==NULL) return;

    bvh->joint_num = 0;
    bvh->channels  = 0;

    _bvh_recsv_get_parameter(bvh->hierarchy, bvh);    
}


void  _bvh_recsv_get_parameter(tTree* pp, BVHData* bvh)
{
    if (pp==NULL) return;

    while(pp->esis!=NULL) pp = pp->esis;

    do {
        bvh->channels += pp->ldat.lv;
        bvh->joint_num++;

        if (pp->next!=NULL) _bvh_recsv_get_parameter(pp->next, bvh);
        pp = pp->ysis;
    } while(pp!=NULL);

    return;
}


void  _bvh_space_format(FILE* fp, int depth)
{
    int i;

    for (i=0; i<depth;i++) fprintf(fp, "    ");
}



///////////////////////////////////////////////////////////////////////////////////
// BVH FILE I/O
//

/**
BVHData*  bvh_read_file(const char* fn)

*/
BVHData*  bvh_read_file(const char* fn)
{
    BVHData* bvh = NULL;

    FILE* fp = fopen(fn, "r");
    if (fp==NULL) return NULL;

    bvh = bvh_parse_data(fp);
    fclose(fp);

    return bvh;
}


/**
int  bvh_write_file(const char* fn, BVHData* bvh)

@retval 0    正常終了
@retval JBXL_ARGS_ERROR    不正な入力データ
@retval JBXL_FILE_OPEN_ERROR    ファイルのオープン失敗
@retval JBXL_BVH_HIERARCHY_ERROR    HIERARCHYデータの書き込み失敗
@retval JBXL_BVH_MOTION_ERROR    MOTIONデータの書き込み失敗
*/
int  bvh_write_file(const char* fn, BVHData* bvh)
{
    if (fn==NULL || bvh==NULL) return JBXL_ARGS_ERROR;
    if (bvh->hierarchy==NULL || bvh->motion==NULL || bvh->state<0) return JBXL_ARGS_ERROR;

    FILE* fp = fopen(fn, "w");
    if (fp==NULL) return JBXL_FILE_OPEN_ERROR;

    int ret = bvh_print_data(fp, bvh);
    fclose(fp);

    return ret;
}


int  bvh_print_data(FILE* fp, BVHData* bvh)
{
    if (bvh==NULL || bvh->hierarchy==NULL || bvh->motion==NULL) return JBXL_ARGS_ERROR;
    if (fp==NULL) return JBXL_FILE_OPEN_ERROR;

    int ret = JBXL_BVH_HIERARCHY_ERROR;

    int hrt = bvh_print_hierarchy(fp, bvh);
    if (hrt>=0) {
        ret = JBXL_BVH_MOTION_ERROR;
        int mrt = bvh_print_motion(fp, bvh);
        if (mrt>=0) ret = 0;
    }

    return ret;
}


int  bvh_print_hierarchy(FILE* fp, BVHData* bvh)
{
    if (fp==NULL || bvh==NULL || bvh->hierarchy==NULL) return JBXL_ARGS_ERROR;

    fprintf(fp, "%s\n", BVH_STR_HIERARCHY);

    int ret = _bvh_recsv_print_hierarchy(fp, bvh->hierarchy);
    fflush(fp);

    return ret;
}


int  _bvh_recsv_print_hierarchy(FILE* fp, tTree* tree)
{
    if (tree!=NULL) {
        while(tree->esis!=NULL) tree = tree->esis;
        do {
            int i;
            tList_data ld = tree->ldat;

            ///////////////
            if (ld.id==BVH_NODE_ROOT || ld.id==BVH_NODE_JOINT || ld.id==BVH_NODE_END) {
                _bvh_space_format(fp, tree->depth-1);

                if      (ld.id==BVH_NODE_ROOT)  fprintf(fp, "%s", BVH_STR_ROOT);
                else if (ld.id==BVH_NODE_JOINT) fprintf(fp, "%s", BVH_STR_JOINT);
                else                           fprintf(fp, "%s", BVH_STR_END);

                // NAME
                if (ld.key.buf!=NULL) fprintf(fp, " %s", ld.key.buf);
                fprintf(fp, "\n");
                _bvh_space_format(fp, tree->depth-1);
                fprintf(fp, "{\n");

                // OFFSET
                _bvh_space_format(fp, tree->depth);
                fprintf(fp, "%s", BVH_STR_OFFSET);
                vector* vect = (vector*)ld.ptr;
                if (vect!=NULL) {
                    fprintf(fp, " %f %f %f\n", vect->x, vect->y, vect->z);
                }
                else {
                    fprintf(fp, " %f %f %f\n", 0.0, 0.0, 0.0);
                }

                // CHANNELS
                if (ld.lv>0 && ld.val.buf!=NULL) {
                    _bvh_space_format(fp, tree->depth);
                    fprintf(fp, "%s %d", BVH_STR_CHANNELS, ld.lv);
                    for (i=0; i<ld.lv*2; i+=2) {
                        if (ld.val.buf[i]=='P') {
                            if      (ld.val.buf[i+1]=='X') fprintf(fp, " %s", BVH_STR_PX);
                            else if (ld.val.buf[i+1]=='Y') fprintf(fp, " %s", BVH_STR_PY);
                            else if (ld.val.buf[i+1]=='Z') fprintf(fp, " %s", BVH_STR_PZ);
                        }
                        else if (ld.val.buf[i]=='R') {
                            if      (ld.val.buf[i+1]=='X') fprintf(fp, " %s", BVH_STR_RX);
                            else if (ld.val.buf[i+1]=='Y') fprintf(fp, " %s", BVH_STR_RY);
                            else if (ld.val.buf[i+1]=='Z') fprintf(fp, " %s", BVH_STR_RZ);
                        }
                    }
                    fprintf(fp, "\n");
                }
            }

            //
            if (tree->next!=NULL) {
                _bvh_recsv_print_hierarchy(fp, tree->next);
            }

            _bvh_space_format(fp, tree->depth-1);
            fprintf(fp, "}\n");

            tree = tree->ysis;
        } while(tree!=NULL);
    }

    return 0;
}


int  bvh_print_motion(FILE* fp, BVHData* bvh)
{
    if (fp==NULL || bvh==NULL || bvh->motion==NULL) return JBXL_ARGS_ERROR;

    fprintf(fp, "%s\n",    BVH_STR_MOTION);
    fprintf(fp, "%s %d\n", BVH_STR_FRAMES,     bvh->frame_num);
    fprintf(fp, "%s %f\n", BVH_STR_FRAME_TIME, bvh->frame_time);

    int i, j;

    for (j=0; j<bvh->frame_num; j++) {
        int jpos = j*bvh->channels;
        for (i=0; i<bvh->channels; i++) {
            fprintf(fp, "%f ", bvh->motion[jpos+i]);
        }
        fprintf(fp, "\n");
    }

    fflush(fp);

    return 0;
}



///////////////////////////////////////////////////////////////////////////////////
// BVH Parser
//

BVHData*  bvh_parse_data(FILE* fp)
{
    BVHData* bvh = NULL;
    tList*  list = NULL;

    if (fp==NULL) return NULL;

    // Search HIERARCHY
    list = bvh_find_line(fp, BVH_STR_HIERARCHY);
    if (list==NULL) return bvh;
    del_tList(&list);
    
    // HIERARCHY
    bvh = new_BVHData();
    if (bvh==NULL) return NULL;
    bvh->state = JBXL_ERROR;

    bvh->hierarchy = bvh_parse_hierarchy(NULL, fp);    
    if (bvh->hierarchy==NULL) {
        del_BVHData(&bvh);
        return NULL;
    }

    //bvh->joint_num = bvh_count_joints(bvh->hierarchy);
    //bvh->channels = bvh_count_channels(bvh->hierarchy);
    bvh_get_parameter(bvh);

    // OFFSET
    bvh->offset = (vector*)malloc(sizeof(vector)*bvh->joint_num);
    if (bvh->offset!=NULL) {
        memset(bvh->offset, 0, sizeof(vector)*bvh->joint_num);
        vector* vect = bvh->offset;
        bvh_get_offset(bvh->hierarchy, &vect);
    }

    // search MOTION
    list = bvh_find_line(fp, BVH_STR_MOTION);
    if (list==NULL) return bvh;
    del_tList(&list);

    list = bvh_find_line(fp, BVH_STR_FRAMES);
    if (list==NULL || list->next==NULL) return bvh;
    bvh->frame_num = atoi((char*)list->next->ldat.key.buf);
    del_tList(&list);

    list = bvh_find_line(fp, BVH_STR_FRAME);
    if (list==NULL || list->next==NULL || list->next->next==NULL)  return bvh;
    if (strcasecmp(BVH_STR_TIME, (char*)list->next->ldat.key.buf)) return bvh;
    bvh->frame_time = atof((char*)list->next->next->ldat.key.buf);
    if (bvh->frame_time!=0.0) bvh->framepsec = (int)(1./bvh->frame_time);
    else                      bvh->framepsec = 30;
    del_tList(&list);

    // MOTION
    bvh_parse_motion(bvh, fp);
    if (bvh->frame_num>0) bvh->state = JBXL_NORMAL;

    return bvh;
}


//
void  bvh_parse_motion(BVHData* bvh, FILE* fp)
{
    if (bvh==NULL || fp==NULL) return;
    if (bvh->channels<=0 || bvh->frame_num<=0) return;

    unsigned long int len = bvh->channels*bvh->frame_num*sizeof(double);
    bvh->motion = (double*)malloc(len);
    if (bvh->motion==NULL) return;
    memset(bvh->motion, 0, len);

    //
    tList* list = bvh_get_seq_data(fp);

    int j = 0;
    while (!feof(fp) && list!=NULL) {
        if (list->ldat.key.buf==NULL) {
            del_tList(&list);
            break;
        }

        int jptr  = j*bvh->channels;
        tList* lt = list;

        int i = 0;
        while (i<bvh->channels && lt!=NULL) {
            bvh->motion[jptr+i] = atof((char*)lt->ldat.key.buf);
            lt = lt->next;    
            i++;
        }
        j++;

        del_tList(&list);
        list = bvh_get_seq_data(fp);
    }

    bvh->frame_num = j;

    return;
}


//
tTree*  bvh_parse_hierarchy(tTree* tree, FILE* fp)
{
    int    endf = FALSE;
    tTree* next = NULL;
    //
    tList* list = bvh_get_seq_data(fp);

    while (list!=NULL) {
        //
        char* cmp = (char*)list->ldat.key.buf;
        if (cmp!=NULL) {
            //
            // ROOT
            if (!strcasecmp(cmp, BVH_STR_ROOT)) {
                endf = TRUE;
                if (list->next==NULL) {
                    next = add_tTree_node_bystr(tree, BVH_NODE_ROOT, 0, "", NULL, NULL, 0);
                    if (tree==NULL) tree = next;
                }
                else {
                    next = add_tTree_node_bystr(tree, BVH_NODE_ROOT, 0, (char*)list->next->ldat.key.buf, NULL, NULL, 0);
                    if (tree==NULL) tree = next;
                }
            }

            //
            // JOINT
            else if (!strcasecmp(cmp, BVH_STR_JOINT)) {
                if (list->next==NULL) {
                    next = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 0, "", NULL, NULL, 0);
                    if (tree==NULL) tree = next;
                }
                else {
                    next = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 0, (char*)list->next->ldat.key.buf, NULL, NULL, 0);
                    if (tree==NULL) tree = next;
                }
            }

            //
            // END
            else if (!strcasecmp(cmp, BVH_STR_END)) {
                if (list->next==NULL) {
                    next = add_tTree_node_bystr(tree, BVH_NODE_END, 0, "", NULL, NULL, 0);
                }
                else {
                    next = add_tTree_node_bystr(tree, BVH_NODE_END, 0, (char*)list->next->ldat.key.buf, NULL, NULL, 0);
                }
            }

            //
            // OFFSET
            else if (!strcasecmp(cmp, BVH_STR_OFFSET)) {
                //
                tList* lt = list;

                vector* vect = (vector*)malloc(sizeof(vector));
                memset(vect, 0, sizeof(vector));
                //
                if (lt->next!=NULL) {
                    lt = lt->next;
                    vect->x = atof((char*)lt->ldat.key.buf);
                }
                if (lt->next!=NULL) {
                    lt = lt->next;
                    vect->y = atof((char*)lt->ldat.key.buf);
                }
                if (lt->next!=NULL) {
                    lt = lt->next;
                    vect->z = atof((char*)lt->ldat.key.buf);
                }

                if (tree!=NULL) {
                    tree->ldat.ptr = (void*)vect;
                    tree->ldat.sz  = sizeof(vector);
                }
            }

            //
            // CHANNELS
            else if (!strcasecmp(cmp, BVH_STR_CHANNELS)) {
                //
                int i, num = 0;
                tList*  lt = list;

                if (lt->next!=NULL) {
                    lt = lt->next;
                    num = atoi((char*)lt->ldat.key.buf);
                }
                if (tree!=NULL) tree->ldat.lv = num;
                
                //
                if (num>0) {
                    Buffer channel = make_Buffer(13);    // strlen("PXPYPZRXRYRZ") + 1

                    for (i=0; i<num; i++) {
                        if (lt->next!=NULL) {
                            lt = lt->next;
                            if      (!strcasecmp((char*)lt->ldat.key.buf, BVH_STR_PX)) {
                                cat_s2Buffer("PX", &channel);
                            }
                            else if (!strcasecmp((char*)lt->ldat.key.buf, BVH_STR_PY)) {
                                cat_s2Buffer("PY", &channel);
                            }
                            else if (!strcasecmp((char*)lt->ldat.key.buf, BVH_STR_PZ)) {
                                cat_s2Buffer("PZ", &channel);
                            }
                            else if (!strcasecmp((char*)lt->ldat.key.buf, BVH_STR_RX)) {
                                cat_s2Buffer("RX", &channel);
                            }
                            else if (!strcasecmp((char*)lt->ldat.key.buf, BVH_STR_RY)) {
                                cat_s2Buffer("RY", &channel);
                            }
                            else if (!strcasecmp((char*)lt->ldat.key.buf, BVH_STR_RZ)) {
                                cat_s2Buffer("RZ", &channel);
                            }
                        }
                    }
                
                    if (tree!=NULL) tree->ldat.val = channel;
                }
            }
    
            //
            else if (!strcmp(cmp, "{")) {
                if (next!=NULL) {
                    bvh_parse_hierarchy(next, fp);
                    if (endf) {
                        del_tList(&list);
                        return tree;
                    }
                }
            }
    
            //
            else if (!strcmp(cmp, "}")) {
                del_tList(&list);
                return tree;
            }
        }

        del_tList(&list);
        list = bvh_get_seq_data(fp);
    }

    return tree;
}

