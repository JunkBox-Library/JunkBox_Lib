#ifndef  __JBXL_MATERIAL_PARAM_H_
#define  __JBXL_MATERIAL_PARAM_H_

/**
 Materialパラメータ
*/

#include "Vector.h"
#include "Rotation.h"
#include "buffer.h"

// アルファチャンネル モード
#define  MATERIAL_ALPHA_NONE            0
#define  MATERIAL_ALPHA_BLENDING        1
#define  MATERIAL_ALPHA_MASKING         2
#define  MATERIAL_ALPHA_EMISSIVE        3     // 未実装

// テクスチャのマッピング方法
#define  MATERIAL_MAPPING_DEFAULT       0
#define  MATERIAL_MAPPING_PLANAR        2
#define  MATERIAL_MAPPING_SPHERICAL     4     // 未実装
#define  MATERIAL_MAPPING_CYLINDRICAL   6     // 未実装

//
#define  MATERIAL_ATTR_LEN              24     // Base64 string len = 32

#define  MATERIAL_ATTR_COLOR_RED        0
#define  MATERIAL_ATTR_COLOR_GREEN      1
#define  MATERIAL_ATTR_COLOR_BLUE       2
#define  MATERIAL_ATTR_TRANSPARENT      3
#define  MATERIAL_ATTR_ALPHACUTOFF      4
#define  MATERIAL_ATTR_SHININESS        5
#define  MATERIAL_ATTR_GLOW             6
#define  MATERIAL_ATTR_BRIGHT           7
#define  MATERIAL_ATTR_LIGHT            8
// 9-12 予備
#define  MATERIAL_ATTR_SHIFT_U          13     // short 2Byte
#define  MATERIAL_ATTR_SHIFT_V          15     // short 2Byte
#define  MATERIAL_ATTR_SCALE_U          17     // short 2Byte
#define  MATERIAL_ATTR_SCALE_V          19     // short 2Byte
#define  MATERIAL_ATTR_ROTATE           21     // short 2Byte
#define  MATERIAL_ATTR_OBJECT           23     //


namespace  jbxl {


//////////////////////////////////////////////////////////////////////////////////////
// TextureParam  テクスチャ用パラメータ
//

class  TextureParam
{
private:
    Buffer   name;          ///< テクスチャ名
    double   color[4];      ///< RGBA

    bool     alphaChannel;  ///< テクスチャデータがアルファチャンネルを持っているかどうか．
    int      alphaMode;     ///< アルファチャンネルのモード
    double   alphaCutoff;   ///< アルファチャンネルの Cutoff値．alphaMode==MATERIAL_ALPHA_MASKING の場合に有効

    double   shiftU;        ///< UVマップのシフト  （U方向）
    double   shiftV;        ///< UVマップのシフト   (V方向)
    double   scaleU;        ///< UVマップのスケール (U方向)
    double   scaleV;        ///< UVマップのスケール (V方向)
    double   rotate;        ///< UVマップの回転

    bool     flipU;         ///< UVマップで U方向の反転を行うか
    bool     flipV;         ///< UVマップで V方向の反転を行うか

public:
    TextureParam(void) { init();}
    virtual ~TextureParam(void) {}

    void    init (void);
    void    free (void);
    void    clear(void) { free(); init();}

    void    dup(TextureParam m);

    bool    isSetTexture(void) { return (name.buf!=NULL);}
    bool    isSetColor  (void) { return (color[0]!=1.0 || color[1]!=1.0 || color[2]!=1.0 || color[3]!=1.0);}
    bool    isSetAlpha  (void) { return (alphaChannel && (alphaMode==MATERIAL_ALPHA_BLENDING || alphaMode==MATERIAL_ALPHA_MASKING));}
    bool    isSetParams (void) { return (isSetColor() || isSetAlpha());}

    bool    isSetShift  (void) { return (shiftU!=0.0 || shiftV!=0.0);}
    bool    isSetScale  (void) { return (scaleU!=1.0 || scaleV!=1.0);}
    bool    isSetRotate (void) { return (rotate!=0.0);}

    void    setName(const char* nm) { free_Buffer(&name); if(nm!=NULL) name = make_Buffer_bystr(nm);}
    char*   getName(void) { return (char*)name.buf;}
    void    addName(const char* nm) { if(nm!=NULL) cat_s2Buffer(nm, &name);}

    void    setColor(double r, double g, double b, double a=1.0) { color[0] = r; color[1] = g; color[2] = b; color[3] = a;}
    void    setColor(double v, int c) { if(c<0) c = 0; else if(c>3) c = 3; color[c] = v;}
    void    setShiftU(double u) { shiftU = u;}
    void    setShiftV(double v) { shiftV = v;}
    void    setScaleU(double u) { scaleU = u;}
    void    setScaleV(double v) { scaleV = v;}
    void    setRotate(double r) { rotate = r;}
    void    setFlipU (bool f)   { flipU  = f;}
    void    setFlipV (bool f)   { flipV  = f;}
    void    setShift (double u, double v) { shiftU = u; shiftV = v;}
    void    setScale (double u, double v) { scaleU = u; scaleV = v;}

    void    setAlphaChannel(bool h)  { alphaChannel = h;}
    void    setAlphaMode(int m)      { alphaMode    = m;}
    void    setAlphaCutoff(double m) { alphaCutoff  = m;}

    Vector<double> getColor(void) { return Vector<double>(color[0], color[1], color[2]);}
    double  getColor(int c) { if(c<0) c = 0; else if(c>3) c = 3; return color[c];}
    double  getAlpha(void)  { return color[3];}
    double  getShiftU(void) { return shiftU;}
    double  getShiftV(void) { return shiftV;}
    double  getScaleU(void) { return scaleU;}
    double  getScaleV(void) { return scaleV;}
    double  getRotate(void) { return rotate;}

    bool    getAlphaChannel(void) { return alphaChannel;}
    int     getAlphaMode(void)    { if (color[3]<0.99) return MATERIAL_ALPHA_BLENDING; else return alphaMode;}
    double  getAlphaCutoff(void)  { return alphaCutoff;}

    void    execTrans (UVMap<double>* uv, int n);    ///< Rotate -> Scale -> Shift
    void    execShift (UVMap<double>* uv, int n);
    void    execScale (UVMap<double>* uv, int n);
    void    execRotate(UVMap<double>* uv, int n);

    void    execInvTrans (UVMap<double>* uv, int n); ///< Shift -> Scale -> Rotate
    void    execInvShift (UVMap<double>* uv, int n);
    void    execInvScale (UVMap<double>* uv, int n);
    void    execInvRotate(UVMap<double>* uv, int n);

    void    execFlipU(UVMap<double>* uv, int n) { for(int i=0; i<n; i++) uv[i].u = 1.0 - uv[i].u;}
    void    execFlipV(UVMap<double>* uv, int n) { for(int i=0; i<n; i++) uv[i].v = 1.0 - uv[i].v;}

    void    printParam(FILE* fp);
};


bool  isSameTexture(TextureParam a, TextureParam b);     ///< compare texture ma,e and color



//////////////////////////////////////////////////////////////////////////////////////
// MaterialParam  マテリアル用パラメータ
//

class  MaterialParam
{
private:
    Buffer  paramstr;       ///< パラメータ文字列 (Base64文字列)

    double  transparent;    ///< テクスチャのアルファチャンネルの係数．
    double  shininess;      ///< 輝き
    double  glow;           ///< 発光
    double  bright;         ///< 明るさ
    double  glossiness;     ///< 光沢
    double  environment;    ///< 環境光
    double  light;          ///< 周りを照らすライト

public:
    bool    enable;

    int     mapping;        ///< マッピング方法
    int     mflags;         ///< メディアフラグ
    int     others;         ///< その他のフラグ

    TextureParam  texture;  ///< テクスチャ
    TextureParam  bumpmap;  ///< Bumpmap テクスチャ
    TextureParam  specmap;  ///< Specular マップ テクスチャ

public:
    MaterialParam(void) { init();}
    virtual ~MaterialParam(void) {}

public:
    void    init (void);
    void    free (void);
    void    clear(void) { free(); init();}

    void    dup(MaterialParam m);

    bool    isSetTexture(void) { return texture.isSetTexture();}
    bool    isSetBumpMap(void) { return bumpmap.isSetTexture();}
    bool    isSetSpecMap(void) { return specmap.isSetTexture();}

    bool    isSetTextureParams(void) { return (texture.isSetParams() || bumpmap.isSetParams() || specmap.isSetParams());}
    bool    isSetParams (void) { return (isSetTransparent() || isSetGlow() || isSetShininess() || isSetBright());}

    bool    isSetGlow   (void) { return (glow!=0.0);}
    bool    isSetBright (void) { return (bright!=0.0);}
    bool    isSetShininess  (void) { return (shininess!=0.0);}
    bool    isSetTransparent(void) { return (transparent<0.99);}

    bool    isTransparency(void) { return (texture.isSetAlpha() || isSetTransparent() || texture.getColor(3)<0.99);}

    void    setTextureName(const char* name) { texture.setName(name);}
    void    setBumpMapName(const char* name) { bumpmap.setName(name);}
    void    setSpecMapName(const char* name) { specmap.setName(name);}

    char*   getTextureName(void) { return texture.getName();}       // 禁 free
    char*   getBumpMapName(void) { return bumpmap.getName();}       // 禁 free
    char*   getSpecMapName(void) { return specmap.getName();}       // 禁 free
    char*   getName(void);                                          // 禁 free

    void    setFullName(const char* extname);
    void    setParamString(const char* param) { if(param!=NULL) copy_s2Buffer(param, &paramstr);}
    void    addParamString(const char* param) { if(param!=NULL) cat_s2Buffer (param, &paramstr);}
    char*   getParamString(void) { return (char*)paramstr.buf;}     // 禁 free

    void    setTransparent(double a) { if(a>1.0) a = 1.0; else if(a<0.0) a = 0.0; transparent = a;}
    void    setShininess(double s)   { if(s>1.0) s = 1.0; else if(s<0.0) s = 0.0; shininess = s;}
    void    setGlow(double g)        { if(g>1.0) g = 1.0; else if(g<0.0) g = 0.0; glow = g;}
    void    setBright(double b)      { if(b>1.0) b = 1.0; else if(b<0.0) b = 0.0; bright = b;}
    void    setGlossiness (double g) { if(g>1.0) g = 1.0; else if(g<0.0) g = 0.0; glossiness = g;}
    void    setEnvironment(double e) { if(e>1.0) e = 1.0; else if(e<0.0) e = 0.0; environment = e;}
    void    setLight(double l)       { if(l>1.0) l = 1.0; else if(l<0.0) l = 0.0; light = l;}

    double  getTransparent(void);
    double  getShininess(void)   { return shininess;}
    double  getGlow(void)        { return glow;}
    double  getBright(void)      { return bright;}
    double  getGlossiness (void) { return glossiness;}
    double  getEnvironment(void) { return environment;}
    double  getLight(void)       { return light;}

    void    printParam(FILE* fp);
    char*   getBase64Params(unsigned char obj='\0', unsigned char cc='-');
};


inline MaterialParam*  newMaterialParam(MaterialParam p) { MaterialParam* m = new MaterialParam(); m->dup(p); return m;}

bool   isSameMaterial(MaterialParam a, MaterialParam b);        ///< compare each texture names and colors


}       // namespace

#endif

