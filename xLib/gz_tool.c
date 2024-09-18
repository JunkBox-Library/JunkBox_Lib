/**
@brief   ZLIB圧縮用ライブラリ
@file    gz_tool.c
@author  Fumi.Iseki (C)

@attention
このプログラムは zlib を使用しています．@n
*/

#ifdef CPLUSPLUS
    #undef CPLUSPLUS
#endif

#include "gz_tool.h"
#include "jbxl_state.h"


#ifndef  DISABLE_ZLIB 


////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
Buffer  gz_decode_data(Buffer enc)

gzipデータ encを解凍して返す．データエラーが発生しても，解凍可能なところまでは解凍する．@n

@param  enc  解凍する gzipデータが格納された Buffer変数．
@return 解凍データが格納された Buffer変数．
*/
Buffer  gz_decode_data(Buffer enc)
{
    Buffer dec = init_Buffer();

    if (enc.buf==NULL) return dec;

    z_stream zstrm;
    zstrm.zalloc    = Z_NULL;
    zstrm.zfree     = Z_NULL;
    zstrm.opaque    = Z_NULL;
    zstrm.next_in   = Z_NULL;
    zstrm.avail_in  = 0;

    int ret = inflateInit2(&zstrm, 47);
    if (ret!=Z_OK) return dec;

    //
    dec = make_Buffer(enc.vldsz*5);
    Buffer wrk = make_Buffer(BUFSZ);

    zstrm.next_in  = enc.buf;
    zstrm.avail_in = (size_t)enc.vldsz;

    do {
        zstrm.next_out  = wrk.buf;
        zstrm.avail_out = (size_t)wrk.bufsz; 

        ret = inflate(&zstrm, Z_NO_FLUSH);
        if (ret!=Z_OK && ret!=Z_STREAM_END) {
            wrk.vldsz = wrk.bufsz - zstrm.avail_out;
            cat_Buffer(&wrk, &dec);
            inflateEnd(&zstrm);
            free_Buffer(&wrk);
            //
            DEBUG_MODE PRINT_MESG("ERROR: GZ_DECODE_DATA: inflate error = %d\n", ret);
            dec.state = JBXL_ERROR;
            return dec;
        }

        wrk.vldsz = wrk.bufsz - zstrm.avail_out; 
        cat_Buffer(&wrk, &dec);

        if (ret==Z_STREAM_END && zstrm.avail_in!=0) {
            ret = inflateReset(&zstrm);
        } 

    } while (ret!=Z_STREAM_END && zstrm.avail_in>0);
    //

    inflateEnd(&zstrm);
    free_Buffer(&wrk);

    return dec;
}


/**
deflateデータを gzipのデータ構造に変換する．

zlibの関数群は deflateを自動判別するようなので，zlibの関数を直接使用する場合はこの関数は必要ない．@n
ファイルとして保存する場合は，gzipの headerとtailerを付加する必要があるが，圧縮前のCRC32とISIZEは通常は計算できない．@n
従ってこの headerと tailerを付加したファイルは gunzuipコマンドでは解凍できない．zcat はエラーを出すが，解凍自体は可能．@n
headerと tailerを付加しても，zlibの関数群では使用可能@n

@param[out] def 変換する deflateのデータが保存された Buffer変数へのポインタ．
@sa http://www.ietf.org/rfc/rfc1952.txt
*/
void  deflate2gzip(Buffer* def)
{
    unsigned char gzip_header[] = { 0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff};
    unsigned char gzip_tailer[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};    // CRC32 4Byte, ISIZE 4Byte

    Buffer gzp = init_Buffer();

    if (def->buf[0]==GZIP_DEFLATE_ID1 && def->buf[1]==GZIP_DEFLATE_ID2) {
        gzp = make_Buffer(def->vldsz+16);
        cat_b2Buffer(gzip_header, &gzp, 10);
        cat_b2Buffer(def->buf+2, &gzp, def->vldsz-2);       
        cat_b2Buffer(gzip_tailer, &gzp, 8);
        free_Buffer(def);
        *def = gzp;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// FILE I/O
// 

/**
int  gz_decode_fp(FILE* infp, FILE* otfp)

gzipファイル infnを解凍して ファイル otfnへ保存する．データエラーが発生しても，解凍可能なところまでは解凍する．

@param  infp 解凍する gzipファイルのファイルポインタ．
@param  otfp 解凍されたファイルのファイルポインタ．
@return 書き込んだファイルのサイズ．
*/
int  gz_decode_fp(FILE* infp, FILE* otfp)
{
    if (infp==NULL || otfp==NULL) return 0;
    
    int sz = 0;
    unsigned char inbuf[LBUF];
    unsigned char otbuf[BUFSZ];

    //
    z_stream zstrm;
    zstrm.zalloc    = Z_NULL;
    zstrm.zfree     = Z_NULL;
    zstrm.opaque    = Z_NULL;
    zstrm.next_in   = Z_NULL;
    zstrm.avail_in  = 0;

    int ret = inflateInit2(&zstrm, 47);
    if (ret!=Z_OK) return JBXL_ERROR;

    do {
        zstrm.next_in  = inbuf;
        zstrm.avail_in = (int)fread(inbuf, 1, LBUF, infp);

        do {
            zstrm.next_out  = otbuf;
            zstrm.avail_out = BUFSZ; 

            ret = inflate(&zstrm, Z_NO_FLUSH);
            if (ret!=Z_OK && ret!=Z_STREAM_END) {
                fwrite(otbuf, BUFSZ-zstrm.avail_out, 1, otfp);          
                //
                inflateEnd(&zstrm);
                DEBUG_MODE PRINT_MESG("ERROR: GZ_DECODE_FP: inflate error = %d\n", ret);
                if (sz==0) sz = JBXL_ERROR;
                return sz;
            }

            int otsz = BUFSZ - zstrm.avail_out; 
            fwrite(otbuf, otsz, 1, otfp);           
            sz += otsz;

            if (ret==Z_STREAM_END && zstrm.avail_in!=0) {
                ret = inflateReset(&zstrm);
            } 

        } while (ret!=Z_STREAM_END && zstrm.avail_in>0);
        //
    } while(!feof(infp));

    inflateEnd(&zstrm);

    return sz;
}


/*
int  gz_encode_gzfp(FILE* fp, gzFile* gf)

ファイル fpを圧縮して gzipファイル gfへ保存する．fread() で読んで，gzwrite() で書き込む．

@param  fp 読み込み用の通常ファイルへのポインタ．
@param  gf 書き込み用のgzipファイルへのポインタ．
@return 読み込んだファイルのサイズ．
*/
int  gz_encode_gzfp(FILE* fp, gzFile* gf)
{
    size_t  cc, sz;
    unsigned char buf[RECVBUFSZ];

    if (*gf==NULL) return JBXL_ARGS_ERROR;

    memset(buf, 0, RECVBUFSZ);
    sz = cc = fread(buf, RECVBUFSZ, 1, fp);
    while(cc>0) {
        gzwrite(*gf, (voidp)buf, (unsigned int)cc);
        memset(buf, 0, cc);
        cc = fread(buf, RECVBUFSZ, 1, fp);
        sz += cc;
    }

    return (int)sz;
}


/**
int  gz_decode_gzfp(gzFile* gf, FILE* fp)

gzip ファイル gfを解凍して ファイル fpへ保存する． gzread()で読んで fwrite()で書き込む．

@param  gf 読み込み用の gzipファイルへのポインタ．
@param  fp 書き込み用の通常ファイルへのポインタ．
@return 書き込んだファイルのサイズ．
*/
int  gz_decode_gzfp(gzFile* gf, FILE* fp)
{
    int  cc, sz;
    unsigned char buf[RECVBUFSZ];

    if (*gf==NULL) return JBXL_ARGS_ERROR;

    memset(buf, 0, RECVBUFSZ);
    sz = cc = gzread(*gf, (voidp)buf, RECVBUFSZ);
    while(cc>0) {
        fwrite(buf, cc, 1, fp);
        memset(buf, 0, cc);
        cc = gzread(*gf, (voidp)buf, RECVBUFSZ);
        sz += cc;
    }

    return sz;
}


/**
int  gz_encode_file(const char* fmfn, const char* tofn)

ファイル fmfnを圧縮して gzipファイル tofnへ保存する．fread()で読んで，gzwrite()で書き込む．

@param  fmfn 圧縮するファイル名
@param  tofn 圧縮された gzip ファイル名
@return 読み込んだファイル(fmfn)のサイズ．
*/
int  gz_encode_file(const char* fmfn, const char* tofn)
{
    size_t cc, sz;
    unsigned char buf[RECVBUFSZ];
    gzFile gf;
    FILE*  fp;

    fp = fopen(fmfn, "rb");
    if (fp==NULL) return JBXL_FILE_OPEN_ERROR;

    gf = gzopen(tofn, "wb");
    if (gf==NULL) {
        fclose(fp);
        return JBXL_FILE_DESTOPEN_ERROR;
    }

    memset(buf, 0, RECVBUFSZ);
    sz = cc = fread(buf, RECVBUFSZ, 1, fp);
    while(cc>0) {
        gzwrite(gf, (voidp)buf, (unsigned int)cc);
        memset(buf, 0, cc);
        cc = fread(buf, RECVBUFSZ, 1, fp);
        sz += cc;
    }

    gzclose(gf);    
    fclose(fp);

    return (int)sz;
}


/**
int  gz_decode_file(const char* fmfn, const char* tofn)

gzipファイル fmfnを解凍して ファイル tofnへ保存する．

@param  fmfn 解凍する gzipファイル名
@param  tofn 解凍されたファイル名
@return 書き込んだファイル(tofn)のサイズ．
*/
int  gz_decode_file(const char* fmfn, const char* tofn)
{
    FILE* infp;
    FILE* otfp;

    infp = fopen(fmfn, "rb");
    if (infp==NULL) return JBXL_FILE_OPEN_ERROR;

    otfp = fopen(tofn, "wb");
    if (otfp==NULL) {
        fclose(infp);
        return JBXL_FILE_DESTOPEN_ERROR;
    }

    int sz = gz_decode_fp(infp, otfp);

    fclose(infp);
    fclose(otfp);

    return sz;

/*
    int  cc, sz;
    unsigned char buf[RECVBUFSZ];
    gzFile gf;
    FILE*  fp;

    gf = gzopen(fmfn, "rb");
    if (gf==NULL) return JBXL_FILE_OPEN_ERROR;

    fp = fopen(tofn, "wb");
    if (fp==NULL) {
        gzclose(gf);
        return JBXL_FILE_DESTOPEN_ERROR;
    }

    memset(buf, 0, RECVBUFSZ);
    sz = cc = gzread(gf, (voidp)buf, RECVBUFSZ);
    while(cc>0) {
        fwrite(buf, cc, 1, fp);
        memset(buf, 0, cc);
        cc = gzread(gf, (voidp)buf, RECVBUFSZ);
        sz += cc;
    }

    gzclose(gf);    
    fclose(fp);

    return sz;
*/
}


/**
int  gz_decode_file_replace(const char* fn, const char* tempdir)

gzipファイル fnを解凍して，通常のファイルで置き換える．

@param  fn 解凍する gzipファイル名
@param  tempdir 作業用ディレクトリ．NULLも可．
@return 書き込んだファイル(tofn)のサイズ．
*/
int  gz_decode_file_replace(const char* fn, const char* tempdir)
{
    int cc;
    char* tempfn;

    tempfn = temp_filename(tempdir, 16);
    cc = gz_decode_file(fn, tempfn);
    if (cc<=0) {
        unlink(tempfn);
        free(tempfn);
        return cc;
    }

    unlink(fn);
    rename(tempfn, fn);
    freeNull(tempfn);

    return cc;
} 


int  is_gz_data(Buffer enc)
{
    if (enc.vldsz<2) return FALSE;

    if (enc.buf[0]==0x1f && enc.buf[1]==0x8b) return TRUE;
    return FALSE;
}




////////////////////////////////////////////////////////////////////////////////////////////////
// tiny Tar

void  extract_tTar(Buffer tardata, Buffer prefix, mode_t mode)
{
    Tar_Header  tar_header;

    long unsigned int size = 0;
    long unsigned int datalen = (long unsigned int)(tardata.vldsz - 1024);

#ifdef WIN32
    if (prefix.buf[prefix.vldsz-1]!='\\') cat_s2Buffer("\\", &prefix);
#else
    if (prefix.buf[prefix.vldsz-1]!='/')  cat_s2Buffer("/", &prefix);
#endif
    //
    while (size < datalen) {
        memcpy(&tar_header, (char*)&tardata.buf[size], sizeof(Tar_Header));
        Buffer fname = make_Buffer_bystr(tar_header.name);
        canonical_filename_Buffer(&fname, FALSE);
        size += sizeof(Tar_Header);
        //
        Buffer path = dup_Buffer(prefix);
        cat_Buffer(&fname, &path);
        free_Buffer(&fname);
        //
        int ret = mkdirp((char*)path.buf, mode);
        if (ret<0) PRINT_MESG("extract_tTar: WARNING: Failed to create directory.\n");
        long unsigned int len = (long unsigned int)strtol(tar_header.size, NULL, 8);
        write_file((char*)path.buf, &tardata.buf[size], len); 
        free_Buffer(&path);
        //
        if (len%512>0) len = (len/512 + 1)*512;
        size += len;
    }
    return;
}


void  extract_tTar_file(const char* fn)
{
    UNUSED(fn);

    return;
}


#endif
