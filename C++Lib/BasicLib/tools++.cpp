
#include  "tools++.h"


/**
@brief    ツールライブラリ for C++
@file     tools++.cpp
@author   Fumi.Iseki (C)
*/


using namespace jbxl;


//
//


#ifdef WIN32

/**
Windows用 ディスパッチャー

void  WinDisPatcher(int sno=0, unsigned int signal1, unsigned int signal2, ...);
  
  機能：シグナル（メッセージ）キューに溜まったシグナル（メッセージ）をディスパッチする．
        引数に，無視する（ディスパッチしない）シグナル（メッセージ）を複数（可変）個指定可能．
        プログラム自体を終了させるシグナル（メッセージ）がキューにあった場合は，プログラムを
        強制終了させる．Windowsの場合は WM_QUIT (0x0111)

        Windows 用のみ実装．Linux の処理の仕方は知らない.......（不勉強です）
  
  引数：無視するシグナル（メッセージ）の数，無視するシグナル１，無視するシグナル２，.........
        シグナル（メッセージ）自体は unsigned int で指定する．
        無視するシグナル（メッセージ）を指定しない場合は，引数無しで呼び出し可能
  
  例  ：DisPatcher(1, WM_NCLBUTTONDOWN);
        DisPatcher();    DisPatcher(0) と同じ  
*/

void  jbxl::DisPatcher(int signalno, ...)
{
    int   i;
    unsigned int*  ign_signal = NULL;

    va_list args;    
    va_start(args, signalno);

    if (signalno>0) ign_signal = (unsigned int*)malloc(signalno*sizeof(int));
    if (ign_signal==NULL) signalno = 0;
    for (i=0; i<signalno; i++) {
        ign_signal[i] = va_arg(args, unsigned int);
    }

    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
        TranslateMessage(&msg);
        if (msg.message==WM_QUIT) exit(1);            //    強制終了
        for (i=0; i<signalno; i++) { 
            if (msg.message==ign_signal[i]) {
                msg.message = WM_NULL;
                break;
            }
        }
        DispatchMessage(&msg);
    }

    free(ign_signal);
    va_end(args);
    return;
}


#endif
