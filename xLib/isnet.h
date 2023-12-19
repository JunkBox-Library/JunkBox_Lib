
#ifndef  __JBXL_INFO_SYSTEM_NET_H_
#define  __JBXL_INFO_SYSTEM_NET_H_

/**
@brief   ISNETプロトコル用ライブラリ ヘッダ
@file    isnet.h
@author  Fumi.Iseki (C)

@par プロトコルコマンド
@b HELLO:   接続を確認
@b BYE:     クライアントからの切断
@b USERID:  サーバにユーザID を送る．
@b NOCHLNG: チャレンジキーを使用しない．
@b PASSWD:  サーバにパスワードを送る
@b KEYEX:   暗号化のためのキーをサーバと交換する RSAは未実装(ssl_tool.cの中で実装される)
@b CRYPT:   使用する暗号方式をサーバに通知する．現在はまだ，AES128CBC, 3DES3CBC のみサポート
*/


#include "password.h"
#include "cipher.h"
#include "xtools.h"

#include "dh_tool.h"
#include "ssl_tool.h"
#include "ldap_tool.h"


#ifdef  ENABLE_SSL


// サーバ側でのコマンド受信処理
Buffer get_command(Buffer);
Buffer get_operand(Buffer);
Buffer get_comment(Buffer);


#include "isnet_server.h"

#include "isnet_client.h"


#endif      // ENABLE_SSL
#endif      // __JBXL_INFO_SYSTEM_NET_H_

