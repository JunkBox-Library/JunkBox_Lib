/** 
@brief   暗号用ライブラリ
@file    cipher.c
@author  Fumi.Iseki (C)

CentOS8 では encrypt(), setkey() が存在しない？
*/

#include "cipher.h"


/**
void  setkey_byBase64(Buffer key)

Base64によってエンコードされた文字列をデコードし,それを encrypt関数の暗号化キーとして設定する．@n
キー長は通常 8Byte(64bit)だが,有効なものは最初の7Byte(56bit)である．

@param  key     暗号化のキー
*/
void  setkey_byBase64(Buffer key)
{
    Buffer tmpkey, deskey; 

    tmpkey = decode_base64_Buffer(key);
    deskey = to_bin64(tmpkey); 

    setkey((const char*)deskey.buf);
    free_Buffer(&tmpkey);
    free_Buffer(&deskey);
    return;
}


/**
void  setkey_byBuffer(Buffer key)

アスキー文字(8Byte)を用いて,encrypt関数の暗号化キーを設定する．
ただし,キーとして有もなものは最初の7Byteである．

@param  key  暗号化のキー
*/
void  setkey_byBuffer(Buffer key)
{
    Buffer deskey; 

    deskey = to_bin64(key); 
    setkey((const char*)deskey.buf);
    free_Buffer(&deskey);

    return;
}


/**
Buffer  encrypt_Buffer(Buffer buf, int mode)

バイナリデータの暗号化または復号を行う．
この関数を呼ぶ前に, setkey_XXXX()関数で暗号化キーを設定しなければならない．

@param  buf   暗号化または復号を行うデータ．
@param  mode  @b 0: 暗号化．@b 1: 復号．

@return  暗号化の場合は 暗号化されたデータ．
@return  復号の場合は 復号されたデータ．
*/
Buffer  encrypt_Buffer(Buffer buf, int mode)
{
    Buffer  str, cry;

    str = to_bin64(buf);
    encrypt((char*)str.buf, mode);
    cry = from_bin64(str);
    free_Buffer(&str);
    return  cry;
}


/**
Buffer  encrypt_Base64(Buffer buf, int mode)

文字列の暗号化または復号を行う

暗号化または復号される文字列はBase64でエンコードされている．
内部では一旦 Base64からデコードされて処理がおこなわれ，再びエンコードされる．

この関数を呼ぶ前に, setkey_XXXX()関数で暗号化キーを設定しなければならない．

@param  buf   暗号化または復号を行う Base64文字列．
@param  mode  @b 0: 暗号化．@b 1: 復号．

@return 暗号化または復号された文字列．Base64でエンコードされる．
*/
Buffer  encrypt_Base64(Buffer buf, int mode)
{
    Buffer dec, str, enc;

    dec = decode_base64_Buffer(buf);
    str = encrypt_Buffer(dec, mode);
    free_Buffer(&dec);

    enc = encode_base64_Buffer(str);
    free_Buffer(&str);
    return enc;
}


/**
Buffer  to_bin64(Buffer str)

Bit列を 0,1のバイト列(64Byte)に変換する (bin64形式に件関する)．@n
例えば、整数の 2は 0,0,0,0,0,.....,0,1,0 のバイト列(64Byte)になる．

@param  str  バイト列に変換するビット列．
@return      変換されたバイト列．1バイトの 0,1の列．bin64形式．
*/
Buffer  to_bin64(Buffer str)
{
    int    i, mi;
    Buffer ret;

    ret = make_Buffer(64);
    if (ret.buf==NULL) return ret;

    mi = Min(64, str.vldsz*8);
    for (i=0; i<mi; i++) ret.buf[i] = (uByte)getBit(str.buf, i);
    ret.vldsz = mi;
    return ret;
}


/**
Buffer  from_bin64(Buffer str)

0,1のバイト列(64Byte, bin64形式)を 8byteのビット列に変換する．

@param  str  ビット列に変換するバイト列．64Byte. bin64形式．
@return      変換されたビット列．8Byte.  64bit.
*/
Buffer  from_bin64(Buffer str)
{
    int    i;
    Buffer ret;
    
    ret = init_Buffer();
    if (str.vldsz!=64) return ret;

    ret = make_Buffer(8);
    if (ret.buf==NULL) return ret;

    for (i=0; i<64; i++) setBit(ret.buf, i, (int)str.buf[i]);
    ret.vldsz = 8;
    return  ret;
}


/**
void  dump_bin64(char* format, Buffer str)

bin64形式を標準エラー出力に表示する．

@param  format  bin64の内容を表示する前に表示するタイトル．
@param  str     表示する bin64形式のバイト列
*/
void  dump_bin64(char* format, Buffer str)
{
    int  i;

    fprintf(stderr, format);
    for (i=0; i<str.vldsz; i++) fprintf(stderr, "%1d", str.buf[i]);
    fprintf(stderr, "\n");
}

