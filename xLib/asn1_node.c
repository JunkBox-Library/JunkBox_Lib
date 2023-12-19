/** 
@brief   ASN1 NODE用ライブラリ
@file    asn1_node.c
@author  Fumi.Iseki (C)
*/

#include "asn1_node.h"


const char* _ASN1_NODE_CLASS[] = 
{
    "UNIVERSAL",            // 0x00
    "APPLICATION",          // 0x40
    "CONTEXT-DEFINED",      // 0x80
    "PRIVATE"               // 0xc0
};


const char* _ASN1_NODE_CONST[] = 
{
    "PRIMITIVE",            // 0x00
    "CONSTRUCTED"           // 0x20
};


const char* _ASN1_NODE_TAG[] = 
{
    "DATA_END",             // 0x00
    "BOOLEAN",              // 0x01
    "INTEGER",              // 0x02
    "BIT_STRING",           // 0x03
    "OCTET_STRING",         // 0x04
    "NULL",                 // 0x05
    "OBJECT_IDENTIFIER",    // 0x06
    "OBJECT_DESCRITOR",     // 0x07
    "EXTERNAL",             // 0x08
    "REAL",                 // 0x09
    "ENUMERATED",           // 0x0a
    "EMBEDDED PDV",         // 0x0b
    "UTF8_STRING",          // 0x0c
    "RELATIVE-OID",         // 0x0d
    "?",                    // 0x0e
    "?",                    // 0x0f
    "SEQUENCE",             // 0x10
    "SET",                  // 0x11
    "NUMERIC_STRING",       // 0x12
    "PRINTABLE_STRING",     // 0x13
    "TELETEX_STRING",       // 0x14
    "VIDEOTEX_STRING",      // 0x15
    "IA5_STRING",           // 0x16
    "UTC_TIME",             // 0x17
    "GENERALIZED_TIME",     // 0x18
    "GRAPHIC_STRING",       // 0x19
    "VISIBLE_STRING",       // 0x1a
    "GENERAL_STRING",       // 0x1b
    "UNIVERSAL_STRING",     // 0x1c
    "CHARACTER_STRING",     // 0x1d
    "BMP_STRING",           // 0x1e
    "?",                    // 0x1f
};



void  asn1_id_type(int id, int* type, int* cnst, int* tag)
{
    if (cnst!=NULL) {
        if (id & JBXL_ASN1_CNSTRCTD) *cnst = 1;   // Constructed
        else *cnst = 0;
    }
    if (type!=NULL) *type = (id & 0xc0) >> 6;     // Class Type
    if (tag!=NULL)  *tag  = id & 0x1f;            // Tag

    return;
}


void  asn1_print_id(FILE* fp, int id)
{
    int cnst, type, tag;

    asn1_id_type(id, &type, &cnst, &tag);

    fprintf(fp, "[%02x] ", id);                                 // Identifier data
    fprintf(fp, "%s ", _ASN1_NODE_CLASS[type]);                 // Class
    fprintf(fp, "%s ", _ASN1_NODE_CONST[cnst]);                 // Primitive/Constructed
    if (type==0x00) fprintf(fp, "%s ", _ASN1_NODE_TAG[tag]);    // Tag: if Class is UNIVERSAL
    else fprintf(fp, "(%02x) ", tag);                          // 

    return;
}
    


void  asn1_print_tag_value(FILE* fp, int id, Buffer buf)
{
    if (buf.vldsz<=0) return;

    fprintf(fp, ": ");

    int tag = id & 0x1f;
    if (tag==JBXL_ASN1_INT) {
        long int n = bin2int_DER(buf);
        fprintf(fp, "INT = %ld ", n);
    }
    else if (tag==JBXL_ASN1_OCT) {
        if (buf.buf!=NULL) fprintf(fp, "\"%s\" ", buf.buf);
        else               fprintf(fp, "\"\" ");
    }
    else if (buf.buf!=NULL) {               // その他
        fprintf(fp, "....... ");
    }

    return;
}

