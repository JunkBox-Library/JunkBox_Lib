
/**
@brief   ISNETプロトコル用ライブラリ
@file    isnet.c
@author  Fumi.Iseki (C)

@attention
make_Buffer() の確保チェックを行っていない．
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif


#include "isnet.h"


#ifdef  ENABLE_SSL


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// コマンド受信処理
//

/**
Buffer  get_command(Buffer msg)

Buffer (命令)からコマンド部を取り出す．小文字は大文字に変換する．

@param  msg  命令を格納した Buffer．改行コードは入れない．
@return コマンド部を格納した Buffer

@attention
注: 命令は comannd_operand_comment (_は複数の空白) の形をしている．comment自体は空白を含んでも良い．
*/
Buffer  get_command(Buffer msg)
{
    unsigned int i;
    Buffer com;

    com = cawk_Buffer(msg, ' ', 1);
    if (com.buf==NULL) {
        com = make_Buffer(1);
        return com;
    }

    for (i=0; i<strlen((const char*)com.buf); i++) {
        if (com.buf[i]>='a' && com.buf[i]<='z') com.buf[i] += - 'a' + 'A';
    }
    return com;
}



/**
Buffer  get_operand(Buffer msg)

Buffer（命令）からオペランド部を取り出す．小文字は大文字に変換する．

@param  msg  命令を格納した Buffer．改行コードは入れない．
@return オペランド部を格納した Buffer

@attention
注: 命令は comannd_operand_comment (_は複数の空白) の形をしている．comment自体は空白を含んでも良い．
*/
Buffer  get_operand(Buffer msg)
{
    Buffer opr;

    opr = cawk_Buffer(msg, ' ', 2);
    if (opr.buf==NULL) opr = make_Buffer(1);

    return opr;
}



/**
Buffer  get_comment(Buffer msg)

Buffer（命令）からコメント部を取り出す．空白も含む．

@param  msg  命令を格納した Buffer．改行コードは入れない．
@return コメント部を格納した Buffer

@attention
注: 命令は comannd_operand_comment (_は複数の空白) の形をしている．comment自体は空白を含んでも良い．
*/
Buffer  get_comment(Buffer msg)
{
    int i, j, pos, cnt;
    char*  buf;
    Buffer com;

    buf = (char*)msg.buf;
    for(i=0, j=0; j<2; j++) {
        while (buf[i]!='\0' && buf[i]!=' ') i++;
        while (buf[i]!='\0' && buf[i]==' ') i++;
    }

    pos = i;
    while (buf[i]!='\0') i++;
    cnt = i - pos;
    com = make_Buffer(cnt+1);

    if (com.buf==NULL) com = make_Buffer(1);
    else {
        for (i=0; i<cnt; i++) com.buf[i] = buf[pos+i];
        com.vldsz = cnt;
    }

    return com;
}



#include "isnet_server.c"

#include "isnet_client.c"


#endif      // ENABLE_SSL
