
#ifndef  __JBXL_CPP_TOLERANCE_H_
#define  __JBXL_CPP_TOLERANCE_H_


/**
@brief    各種トレランス ヘッダ
@file     Tolerance.h
@author   Fumi.Iseki (C)
*/



//
namespace jbxl {



// 相対値
extern double   Vector_Tolerance;
extern double   Vertex_Tolerance;
extern double   Edge_Tolerance;
extern double   Facet_Tolerance;

// 絶対値
extern double   Abs_Vector_Tolerance;
extern double   Abs_Vertex_Tolerance;
extern double   Collision_Tolerance;        ///< 衝突判定用トレランス
extern double   Sin_Tolerance;              ///< sinθ==0 


// 1に対して 0とするトレランス
extern double   Zero_Eps;
extern double   Zero_Eps2;


/*
void SetZeroEPS(double eps);
void SetVectorTolerance(double tol);
void SetVertexTolerance(double tol);
void SetEdgeTolerance(double tol);
void SetFacetTolerance(double tol);
void SetCollisionTolerance(double tol);
void SetSINTolerance(double tol);
void SetAbsVectorTolerance(double tol);
void SetAbsVertexTolerance(double tol);
*/

inline void SetZeroEPS(double eps)           { Zero_Eps = eps; }
inline void SetVectorTolerance(double tol)   { Vector_Tolerance = tol; }
inline void SetVertexTolerance(double tol)   { Vertex_Tolerance = tol; }
inline void SetEdgeTolerance(double tol)     { Edge_Tolerance = tol; }
inline void SetFacetTolerance(double tol)    { Facet_Tolerance = tol; }
inline void SetCollisionTolerance(double tol){ Collision_Tolerance = tol; }
inline void SetSINTolerance(double tol)      { Sin_Tolerance = tol; }
inline void SetAbsVectorTolerance(double tol){ Abs_Vector_Tolerance = tol; }
inline void SetAbsVertexTolerance(double tol){ Abs_Vertex_Tolerance = tol; }



}       // namespace


#endif

