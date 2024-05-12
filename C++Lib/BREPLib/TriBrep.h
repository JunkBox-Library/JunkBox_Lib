#ifndef  __JBXL_TRI_BREP_H_
#define  __JBXL_TRI_BREP_H_

/**
@brief    三角Contour(Facet)用ライブラリ ヘッダ for BREP
@file     TriBrep.h
@author   Fumi.Iseki (C)

@attention
このファイルの関数は Contour(=Facet)が三角形であることを前提とする．
*/


#include "STL.h"


namespace jbxl {


DllExport BREP_CONTOUR* CreateContourByVector(BREP_FACET* facet, Vector<double>* v, Vector<double>* n=NULL, UVMap<double>* uv=NULL, ArrayParam<int>* w=NULL, bool dupli=false);
DllExport BREP_CONTOUR* CreateContourByVertex(BREP_FACET* facet, BREP_VERTEX** vtx);

DllExport void   CreateContoursList(BREP_SOLID* solid);
DllExport void   CreateWingsList(BREP_SOLID* solid);
DllExport void   CreateSurplusContoursList(BREP_SOLID* solid);
DllExport void   CreateShortageWingsList(BREP_SOLID* solid);

DllExport void   DeleteSurplusContours(BREP_SOLID* solid);
DllExport void   DeleteStraightEdges(BREP_SOLID* solid);
DllExport void   DeleteShortageWings(BREP_SOLID* solid) ;

DllExport void   FillShortageWings(BREP_SOLID* solid, int method, bool mode);
DllExport int    FillShortageWings_Next(BREP_SOLID* solid, bool mode);
DllExport int    FillShortageWings_Near(BREP_SOLID* solid, bool mode);
DllExport bool   PatchupContour(BREP_SHELL* shell, BREP_VERTEX** vert, bool mode);

DllExport void   JoinShortageWings(BREP_SOLID* solid);

DllExport void   SetMinVertex(BREP_VERTEX_LIST* list, BREP_VERTEX* vrtx);
DllExport bool   IsIncludeCompleteEdge(BREP_WING* wing);
DllExport BREP_VERTEX*  FindConnectEdgeVertex(BREP_VERTEX* vert);

DllExport int    DupEdgeNumber(BREP_CONTOUR* contour);
DllExport void   ReverseContours(BREP_SOLID* solid);
DllExport void   SetDeletableContoursByEdge(BREP_EDGE* edge);
DllExport void   FastDeleteFacet(BREP_FACET* facet);

DllExport int    IsAtLine(BREP_VERTEX** v);
DllExport bool   IsForbiddenEdge(BREP_VERTEX** vert);

DllExport bool   IsCollisionContours(BREP_SOLID* solid, BREP_CONTOUR* contour, BREP_CONTOUR** collision);
DllExport bool   CollisionTriContour3D(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2);
DllExport bool   CollisionTriContour2D(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2);
DllExport bool   IsInTriangle(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2);

DllExport int    CommonVertex(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2);
DllExport bool   SamePlaneContour(BREP_CONTOUR* contour1, BREP_CONTOUR* contour2, int& lineno);

DllExport int    CreateTriSolidFromSTL(BREP_SOLID* solid, STLData* stldata, int fno, bool check=true);
DllExport int    CreateTriSolidFromVector(BREP_SOLID* solid, int vno, Vector<double>* v, Vector<double>* n=NULL, UVMap<double>* uv=NULL, ArrayParam<int>* w=NULL, bool dupli=false, bool check=true);

DllExport void   AddVector2TriSolid(BREP_SOLID* solid, BREP_SHELL* shell, Vector<double>* v, Vector<double>* n=NULL, UVMap<double>* uv=NULL, ArrayParam<int>* w=NULL, bool dupli=false);
DllExport int    CloseTriSolid(BREP_SOLID* solid, bool check=true, CVCounter* counter=NULL);

DllExport bool   IsConnectEdges(BREP_WING* wing1, BREP_WING* wing2);


}       // namespace


#endif
