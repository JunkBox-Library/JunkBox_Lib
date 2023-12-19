/** 
@brief   エラーメッセージ用ライブラリ
@file    jbxl_state_func.c
@author  Fumi.Iseki (C)
*/

#include "tlist.h"
#include "jbxl_state.h"


tList* _jbxl_state_list    = NULL;
tList* _jbxl_state_pointer = NULL;



/**
void  jbxl_fprint_state(FILE* fp, int no)

JBXLライブラリの state の情報を表示する．
デバッグ用．

@param  fp    出力先のファイル識別子
@param  no    JBXLライブラリの state番号
*/
void  jbxl_fprint_state(FILE* fp, int no)
{
    if (fp==NULL) fp = stderr;
    if (_jbxl_state_list==NULL) jbxl_init_state_info();

    tList* lt = search_id_tList(_jbxl_state_list, no, 1);
    if (lt==NULL) {
        fprintf(fp, "JBXL_STATE: %d: Error Message is not exist!\n", no);
    }
    else {
        fprintf(fp, "JBXL_STATE: %s: %d\n", lt->ldat.key.buf, no);
    }
}



/**
void  jbxl_fprint_state_jp(FILE* fp, int no)

JBXLライブラリの state の詳細情報（日本語）を表示する．
デバッグ用．

@param  fp    出力先のファイル識別子
@param  no    JBXLライブラリの state番号
*/
void  jbxl_fprint_state_jp(FILE* fp, int no)
{
    if (fp==NULL) fp = stderr;
    if (_jbxl_state_list==NULL) jbxl_init_state_info();

    tList* lt = search_id_tList(_jbxl_state_list, no, 1);
    if (lt==NULL) {
        fprintf(fp, "JBXL_STATE: %d: Error Message is not exist!\n", no);
    }
    else {
        fprintf(fp, "JBXL_STATE: %s: %d: %s\n", lt->ldat.key.buf, no, lt->ldat.val.buf);
    }
}



//////////////////////////////////////////////////////

void  jbxl_init_state_info(void)
{
    if (_jbxl_state_list!=NULL) {
        del_tList(&_jbxl_state_list);
        _jbxl_state_pointer = NULL;
    }

    jbxl_add_state(JBXL_STATE_ANCHOR, "JBXL_STATE_ANCHOR", "アンカー");
    _jbxl_state_list = _jbxl_state_pointer;
    jbxl_add_all_states();
}


void  jbxl_add_state(int id, const char* smb, const char* str)
{
    _jbxl_state_pointer = add_tList_node_bystr(_jbxl_state_pointer, id, 0, smb, str, NULL, 0);
}



