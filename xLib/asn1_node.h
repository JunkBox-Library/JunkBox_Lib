
#ifndef  __JBXL_JBXL_ASN1_NODE_H_
#define  __JBXL_JBXL_ASN1_NODE_H_

#include "buffer.h"
#include "asn1_tool.h"

//
// http://www5d.biglobe.ne.jp/stssk/asn1/index.html
//

#define  JBXL_ASN1_ANCHOR       0xff        ///< ANCHOR

#define  JBXL_ASN1_PRIMITIVE    0x00        ///< PRIMITIVE
#define  JBXL_ASN1_CNSTRCTD     0x20        ///< CONSTRUCTED  構造化フラグ

#define  JBXL_ASN1_UNIVERSAL    0x00        ///< UNIVERSAL
#define  JBXL_ASN1_APPLI        0x40        ///< APPLICATION
#define  JBXL_ASN1_CONTEXT      0x80        ///< CONTEXT-DEFINED
#define  JBXL_ASN1_PRIVATE      0xc0        ///< PRIVATE

#define  JBXL_ASN1_END          0x00        ///< DATA_END
#define  JBXL_ASN1_BOOL         0x01        ///< BOOLEAN
#define  JBXL_ASN1_INT          0x02        ///< INTEGER
#define  JBXL_ASN1_BIT          0x03        ///< BIT_STRING 
#define  JBXL_ASN1_OCT          0x04        ///< OCTET_STRING
#define  JBXL_ASN1_NULL         0x05        ///< NULL
#define  JBXL_ASN1_OBJ          0x06        ///< OBJECT_IDENTIFIER
#define  JBXL_ASN1_DSC          0x07        ///< OBJECT_DESCRITOR
#define  JBXL_ASN1_EXT          0x08        ///< EXTERNAL
#define  JBXL_ASN1_REAL         0x09        ///< REAL
#define  JBXL_ASN1_ENU          0x0a        ///< ENUMERATED
#define  JBXL_ASN1_EMB          0x0b        ///< EMBEDDED_PDV
#define  JBXL_ASN1_UTF8         0x0c        ///< UTF8_STRING
#define  JBXL_ASN1_OID          0x0d        ///< RELATIVE-OID
#define  JBXL_ASN1_SEQ          0x10        ///< SEQUENCE
#define  JBXL_ASN1_SET          0x11        ///< SET

#define  JBXL_ASN1_NMSTR        0x12        ///< NUMERIC_STRING
#define  JBXL_ASN1_PRSTR        0x13        ///< PRINTABLE_STRING
#define  JBXL_ASN1_TLSTR        0x14        ///< TELETEX_STRING
#define  JBXL_ASN1_VDSTR        0x15        ///< VIDEOTEX_STRING
#define  JBXL_ASN1_IA5          0x16        ///< IA5_STRING
#define  JBXL_ASN1_UTC          0x17        ///< UTC_TIME
#define  JBXL_ASN1_GTM          0x18        ///< GENERALIZED_TIME
#define  JBXL_ASN1_GRSTR        0x19        ///< GRAPHIC_STRING
#define  JBXL_ASN1_VSSTR        0x1a        ///< VISIBLE_STRING
#define  JBXL_ASN1_GNSTR        0x1b        ///< GENERAL_STRING
#define  JBXL_ASN1_UNSTR        0x1c        ///< UNIVERSAL_STRING
#define  JBXL_ASN1_CHSTR        0x1d        ///< CHARACTER_STRING
#define  JBXL_ASN1_BMSTR        0x1e        ///< BMP_STRING

#define  JBXL_ASN1_SEQ_CNSTRCTD 0x30        ///< SEQUENCE + CONSTRUCTED (構造化フラグ)
#define  JBXL_ASN1_SET_CNSTRCTD 0x31        ///< SET + CONSTRUCTED (構造化フラグ)


void  asn1_print_id(FILE* fp, int id);
void  asn1_id_type(int id, int* type, int* cnst, int* tag);
void  asn1_print_tag_value(FILE* fp, int tag, Buffer buf);



#endif        // __JBXL_JBXL_ASN1_TOOL_H_
