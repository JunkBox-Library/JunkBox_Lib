
/**
@brief    メモリ管理機能付グラフィックデータ定義ライブラリ 
@file     ExGdata.cpp
@author   Fumi.Iseki
*/


#include  "ExGdata.h"


using namespace jbxl;


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExCmnHead 
//

//
void  ExCmnHead::init(void)
{   
    init_CmnHead(this);
    dbyte = 0;
    state = 0;
    refno = NULL;
}


//
void  ExCmnHead::set(RBound<int> rb, int d, int* rn) {
    set(rb.xmax-rb.xmin+1, rb.ymax-rb.ymin+1, rb.zmax-rb.zmin+1, d, rn);
    return;
}


//
void  ExCmnHead::set(int x, int y, int z, int d, int* rn) 
{ 
    init_CmnHead(this);
    state = 0;
    getm(x, y, z, d); 

    if (rn!=NULL) setRefer(rn);
    else if (refno!=NULL) {
        if (grptr==NULL) *refno = -1;
        else             *refno =  1;
    }

    return;
}


//
void  ExCmnHead::getm(int x, int y, int z, int d)
{
    if (x<=0) x = 1;
    if (y<=0) y = 1;
    if (z<=0) z = 1;
    if (d<=0) d = 8;

    xsize = x;
    ysize = y;
    zsize = z;
    depth = d;
    state = 0;

    dbyte = (d+7)/8;
    lsize = x*y*z*dbyte;

    grptr = (uByte*)malloc(lsize*sizeof(uByte));
    if (grptr==NULL) {
        init_CmnHead(this);
        state = JBXL_GRAPH_MEMORY_ERROR;
        return;
    }
    memset(grptr, 0, lsize);
}


//
void  ExCmnHead::setRefer(int* rn)
{
    refno = rn;
    if (refno!=NULL) {
        if (grptr==NULL) *refno = -1;
        else             *refno =  1;
    }
}


//
bool  ExCmnHead::isNull(void)
{
    if (grptr==NULL) return true;

#ifdef WIN32
    if (grptr==(uByte*)WIN_DD_NULL) return true;
#endif

    if (refno!=NULL) {
        if (*refno<0) return true;
        else          return false;
    }
    return false;
}


/**
void  ExCmnHead::mfree(bool azero)

グラフィックデータを開放する

メモリ管理をしている場合は，参照数を減らす．
メモリ管理をしていない場合は，何もしない．
*/
void  ExCmnHead::mfree(bool azero)
{
    if (refno!=NULL) {
        (*refno)--;
        if (!azero && *refno==0) {
            free_CmnHead(this);
            *refno = -1;
        }
        else if (*refno==-1) {
            DEBUG_MODE PRINT_MESG("ExCmnHead::free(): [警告] 通常ありえない参照数 0\n");
        }
        else if (*refno<-1) {
             *refno = -1;
        }
    }

    return;
}


/**
void  ExCmnHead::free(void)

グラフィックデータを開放する

メモリ管理をしている場合は，参照数を減らす．
メモリ管理をしていない場合は，強制的に開放する．
*/
void  ExCmnHead::free(void)
{
    if (refno==NULL) {
        if (!isNull()) {
            free_CmnHead(this);
        }
    }
    else {
        mfree();
    }
    return;
}


/**
void  ExCmnHead::ffree(void)

グラフィックデータを強制的に開放する
*/
void  ExCmnHead::ffree(void)
{  
    if (refno==NULL) {
        if (!isNull()) {
            free_CmnHead(this);
        }
    }
    else {
        if (*refno>=0) {
            free_CmnHead(this);
            *refno = -1;
        }
    }

    return;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExCmnHead オペレータ
//

ExCmnHead  ExCmnHead::operator= (ExCmnHead& s)
{ 
    ExCmnHead dm = *this;
    
    kind  = s.kind;
    xsize = s.xsize;
    ysize = s.ysize;
    zsize = s.zsize;
    dbyte = s.dbyte;
    depth = s.depth;
    bsize = s.bsize;
    lsize = s.lsize;
    buf   = s.buf;
    grptr = s.grptr;
    state = s.state;

    if (!s.isNull()) {                          // コピー元はデータを持っている．
        if (s.refno!=NULL) {                        // コピー元は参照変数を持っている．
            if (*s.refno>=0) {                      // 相手の参照変数を使う．
                refno = s.refno;
                (*refno)++;
            }
            else {
                DEBUG_MODE PRINT_MESG("ExCmnHead::= (ExCmnHead): コピー元参照変数の不正!!\n");
            }
        }
        else {                                      // コピー元は参照変数を持っていない．
            if (refno!=NULL) {                          // 自分の参照変数を持っている．
                if (*refno<=-1) *refno = 1;             // 自分の参照変数が未使用ならそれを使う．
                else  refno = NULL;                         // 参照変数は誰かが使っているので，自分は参照変数無しになる．
            }   
        }
    }

    else {                                      // コピー元はデータを持っていない
        grptr = NULL;
        if (s.refno!=NULL) {                        // コピー元は参照変数を持っている．
            refno = s.refno;                            // 相手の参照変数を使う．
            *refno = -1;                                // データは無いので -1 に．
        }                                       
        else {                                      // コピー元は参照変数を持っていない．
            if (refno!=NULL) {                          // 自分の参照変数を持っている．
                if (*refno<=-1) *refno = -1;            // 自分の参照変数が未使用ならそれを使う．
                else refno = NULL;                          // 参照変数は誰かが使っているので，自分は参照変数無しになる．
            }
        }
    }

    if (!dm.isNull()) dm.free();
    return *this;
}


//
// 
ExCmnHead  ExCmnHead::operator= (CmnHead& s)
{ 
    if (!isNull()) free();

    kind  = s.kind;
    xsize = s.xsize;
    ysize = s.ysize;
    zsize = s.zsize;
    depth = s.depth;
    bsize = s.bsize;
    lsize = s.lsize;
    buf   = s.buf;
    grptr = s.grptr;
    state = 0;
    dbyte = (depth+7)/8;

    if (s.grptr!=NULL) {                        // コピー元はデータを持っている．
        if (refno!=NULL) {                          // 自分の参照変数を持っている．
            if (*refno<=-1) *refno =  1;            // 自分の参照変数が未使用ならそれを使う．
            else  refno = NULL;                         // 参照変数は誰かが使っているので，自分は参照変数無しになる．
        }   
    }

    else {                                      // コピー元はデータを持っていない
        grptr = NULL;
        if (refno!=NULL) {                          // 自分の参照変数を持っている．
            if (*refno<=-1) *refno = -1;            // 自分の参照変数が未使用ならそれを使う．
            else refno = NULL;                          // 参照変数は誰かが使っているので，自分は参照変数無しになる．
        }
    }

    return *this;
}

