
/**
JPEG 2000のヘッダからファイルの種類を返す．@n
ただし，ヘッダから JP2K_FMT_JPTであることは判別できないので，注意する．

@param buf ヘッダ情報の入った Buffer変数．最低12Byte必要．
@return    ファイル種別 (JP2K_FMT_NONE, JP2K_FMT_JP2, JP2K_FMT_J2K)
*/
int  jbxl::isJPEG2KHeader(Buffer buf)
{
    int format = JP2K_FMT_NONE;

    if (!memcmp(buf.buf, JP2K_MAGIC_RFC3745_JP2, 12) || !memcmp(buf.buf, JP2K_MAGIC_JP2, 4)) {
        format = JP2K_FMT_JP2;
    }
    else if (!memcmp(buf.buf, JP2K_MAGIC_J2K, 4)) {
        format = JP2K_FMT_J2K;
    }
    return format;
}


/**
破損した JPEG 2000のファイルを修復して，writef の名前で保存．
とは言っても修復すのは Title Part の Lost と EOC のみ．
*/
int  jbxl::repairJPEG2KFile(const char* fname, const char* writef)
{
    if (fname==NULL || writef==NULL) return JBXL_GRAPH_IVDARG_ERROR;
    int c_flag = FALSE;

    // read file
    Buffer buf = read_Buffer_file(fname);
    if (buf.vldsz<=0) return JBXL_GRAPH_RDFILE_ERROR;

    // EOC (ffd9)
    unsigned char eoc[] = {0xff, 0xd9};
    if (buf.buf[buf.vldsz-2]!=eoc[0] || buf.buf[buf.vldsz-1]!=eoc[1]) {
        cat_b2Buffer(eoc, &buf, 2);
        c_flag = TRUE;
    }

    // Title Part (ff90)
    for (int i=0; i<buf.vldsz-1; i++) {
        if (buf.buf[i]==0xff && buf.buf[i+1]==0x90) {
            if (i+9<buf.vldsz) {
                buf.buf[i+6] = 0x00;                    // Lost: 0 でOK
                buf.buf[i+7] = 0x00;
                buf.buf[i+8] = 0x00;
                buf.buf[i+9] = 0x00;
                c_flag = TRUE;
                break;
            }
        }
    }
 
    // write file
    int ret = JBXL_GRAPH_IVDDATA_ERROR;
    if (c_flag) {
        ret = JBXL_NORMAL;
        int sbf = save_Buffer_file(buf, (char*)writef);
        if (!sbf) ret = JBXL_GRAPH_WRFILE_ERROR;
    }
    free_Buffer(&buf);

    return ret;
}

