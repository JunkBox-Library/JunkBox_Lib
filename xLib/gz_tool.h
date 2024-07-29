#ifndef  __JBXL_GZPACKING_TOOL_H_
#define  __JBXL_GZPACKING_TOOL_H_

/**
@brief   gzツールプログラム ヘッダ  (-lz)
@file    gz_tool.h
@author  Fumi.Iseki (C)
*/

#include "tools.h"
#include "buffer.h"

#include <zlib.h>

/*
#ifdef WIN32
  #ifdef _DEBUG 
    #pragma  comment(lib, "zlibd.lib")
  #else 
    #pragma  comment(lib, "zlib.lib")
  #endif
#endif
*/

#define  GZIP_DEFLATE_ID1   0x78
#define  GZIP_DEFLATE_ID2   0xda


// gzipデータ
Buffer  gz_decode_data(Buffer enc);                             ///< 圧縮データ encを解凍する．
void    deflate2gzip(Buffer* def);                              ///< deflateデータを gzipのデータ構造に変換する．


// FILE I/O
int  gz_decode_fp(FILE* infp, FILE* otfp);                      ///< ファイルポインタによるファイルの解凍

int  gz_encode_gzfp(FILE* fp, gzFile* gf);                      ///< GZIPのファイルポインタを用いた ファイルの圧縮
int  gz_decode_gzfp(gzFile* gf, FILE* fp);                      ///< GZIPのファイルポインタを用いた ファイルの解凍

int  gz_encode_file(const char* ffn, const char* tfn);          ///< ファイル名による ファイルの圧縮
int  gz_decode_file(const char* ffn, const char* tfn);          ///< ファイル名による ファイルの解凍
int  gz_decode_file_replace(const char* fn, const char* dir);   ///< ファイル名による ファイルの解凍．ファイルを置き換える．


#endif 
