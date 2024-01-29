/**
@brief    各種トレランス
@file     Tolerance.cpp
@author   Fumi.Iseki (C)
*/

#include "Tolerance.h"


using namespace jbxl;


// 相対値
double  jbxl::Vector_Tolerance     = 1.0e-6;
double  jbxl::Vertex_Tolerance     = 1.0e-6;
double  jbxl::Edge_Tolerance       = 1.0e-12;
double  jbxl::Facet_Tolerance      = 1.0e-12;

// 絶対値
double  jbxl::Abs_Vector_Tolerance = 1.0e-4;
double  jbxl::Abs_Vertex_Tolerance = 1.0e-4;
double  jbxl::Collision_Tolerance  = 1.0e-2;        ///< 衝突判定用トレランス
double  jbxl::Sin_Tolerance        = 1.0e-6;        ///< sinθ==0 

// 1に対して
double  jbxl::Zero_Eps             = 1.0e-6;        ///< 1に対して 0とするトレランス
double  jbxl::Zero_Eps2            = Zero_Eps*Zero_Eps;


/*
void jbxl::SetZeroEPS(double eps)           { Zero_Eps = eps; }
void jbxl::SetVectorTolerance(double tol)   { Vector_Tolerance = tol; }
void jbxl::SetVertexTolerance(double tol)   { Vertex_Tolerance = tol; }
void jbxl::SetEdgeTolerance(double tol)     { Edge_Tolerance = tol; }
void jbxl::SetFacetTolerance(double tol)    { Facet_Tolerance = tol; } 
void jbxl::SetCollisionTolerance(double tol){ Collision_Tolerance = tol; }
void jbxl::SetSINTolerance(double tol)      { Sin_Tolerance = tol; }
void jbxl::SetAbsVectorTolerance(double tol){ Abs_Vector_Tolerance = tol; }
void jbxl::SetAbsVertexTolerance(double tol){ Abs_Vertex_Tolerance = tol; }
*/

