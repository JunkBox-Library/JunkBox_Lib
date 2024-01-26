
#include "FacetBaseData.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////
// FacetTriIndex
//

void  FacetTriIndex::set(int w1, int w2, int w3, int m1, int m2, int m3, int u1, int u2, int u3)
{
     v1 = w1;  v2 = w2;  v3 = w3;
     n1 = m1;  n2 = m2;  n3 = m3;
    uv1 = u1; uv2 = u2, uv3 = u3;
}


void  FacetTriIndex::mlt_set(int d1, int d2, int d3)
{
     v1 = d1;  v2 = d2;  v3 = d3;
     n1 = d1;  n2 = d2;  n3 = d3;
    uv1 = d1; uv2 = d2, uv3 = d3;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// FacetTriData
//

void  FacetTriData::init(void)
{
    facetNum = 0;

    v1.init();
    v2.init();
    v3.init();
    n1.init();
    n2.init();
    n3.init();
    uv1.init();
    uv2.init();
    uv3.init();
}


void  FacetTriData::execScale(double x, double y, double z)
{
    v1.x *= x;
    v1.y *= y;
    v1.z *= z;
    v2.x *= x;
    v2.y *= y;
    v2.z *= z;
    v3.x *= x;
    v3.y *= y;
    v3.z *= z;
}


void  FacetTriData::execRotate(Quaternion<double> q)
{
    v1 = q.execRotation(v1);
    v2 = q.execRotation(v2);
    v3 = q.execRotation(v3);
    n1 = q.execRotation(n1);
    n2 = q.execRotation(n2);
    n3 = q.execRotation(n3);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Facet Base Data
//

void  FacetBaseData::init(int idx, int num)
{
    num_index = idx;
    num_data  = num;
    vcount    = 3;

    index     = NULL;
    vertex    = NULL;
    normal    = NULL;
    texcrd    = NULL;
}


void  FacetBaseData::free(void)
{
    freeNull(index);

    freeNull(vertex);
    freeNull(normal);
    freeNull(texcrd);

    init();
}


bool  FacetBaseData::getm(void)
{
    index = (int*)malloc(sizeof(int)*num_index);

    vertex = (Vector<double>*)malloc(sizeof(Vector<double>)*num_data);
    normal = (Vector<double>*)malloc(sizeof(Vector<double>)*num_data);
    texcrd = (UVMap <double>*)malloc(sizeof(UVMap <double>)*num_data);

    if (index==NULL || vertex==NULL || normal==NULL || texcrd==NULL) {
        this->free();
        return false;
    }

    return true;
}


void  FacetBaseData::dup(FacetBaseData a)
{
    num_index = a.num_index;
    num_data  = a.num_data;
    vcount    = a.vcount;

    if (getm()) {
        for (int i=0; i<num_index; i++) index[i] = a.index[i];
        for (int i=0; i<num_data;  i++) {
            vertex[i] = a.vertex[i];
            normal[i] = a.normal[i];
            texcrd[i] = a.texcrd[i];
        }
    }
    return;
}


void  FacetBaseData::execScale(Vector<double> scale)
{
    for (int i=0; i<num_data;  i++) {
        vertex[i].x *= scale.x;
        vertex[i].y *= scale.y;
        vertex[i].z *= scale.z;
    }
}


void  FacetBaseData::execShift(Vector<double> shift)
{
    for (int i=0; i<num_data; i++) {
        vertex[i].x += shift.x;
        vertex[i].y += shift.y;
        vertex[i].z += shift.z;
    }
}


void  FacetBaseData::execRotate(Quaternion<double> quat)
{
    for (int i=0; i<num_data; i++) {
        vertex[i] = VectorRotation(vertex[i], quat);
        normal[i] = VectorRotation(normal[i], quat);
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Triangle Polygon Data
//

void  TriPolyData::init(void)
{
    facetNum   = -1;
    has_normal = false;
    has_texcrd = false;
}


void  TriPolyData::dup(TriPolyData a)
{
    *this = a;
}


void  TriPolyData::execScale(Vector<double> scale)
{
    for (int i=0; i<3; i++) {
        vertex[i].x *= scale.x;
        vertex[i].y *= scale.y;
        vertex[i].z *= scale.z;
    }
}


void  TriPolyData::execShift(Vector<double> shift)
{
    for (int i=0; i<3; i++) {
        vertex[i].x += shift.x;
        vertex[i].y += shift.y;
        vertex[i].z += shift.z;
    }
}


void  TriPolyData::execRotate(Quaternion<double> quat)
{
    for (int i=0; i<3; i++) {
        vertex[i] = VectorRotation(vertex[i], quat);
        normal[i] = VectorRotation(normal[i], quat);
    }
}



//////////////////////////////////////////////////////////////////////////////////
//

TriPolyData* jbxl::dupTriPolyData(TriPolyData* data, int num)
{
    if (data==NULL) return NULL;

    TriPolyData* dup = (TriPolyData*)malloc(num*sizeof(TriPolyData));
    if (dup==NULL) return NULL;

    for (int i=0; i<num; i++) dup[i].dup(data[i]);
    return dup;
}


TriPolyData*  jbxl::joinTriPolyData(TriPolyData*& first, int num_f, TriPolyData*& next, int num_n)
{
    if (first==NULL) return next;
    if (next ==NULL) return first;

    TriPolyData* join = (TriPolyData*)malloc((num_f+num_n)*sizeof(TriPolyData));
    if (join==NULL) return NULL;
    
    int fnum = 0;
    for (int i=0; i<num_f; i++) {
        join[i].dup(first[i]);
        if (join[i].facetNum>fnum) fnum = join[i].facetNum;
    }
    fnum += 1;

    for (int i=0; i<num_n; i++) {
        join[num_f+i].dup(next[i]);
        join[num_f+i].facetNum += fnum;
    }

    freeTriPolyData(first, num_f);
    freeTriPolyData(next,  num_n);

    return join;
}


void  jbxl::freeTriPolyData(TriPolyData*& tridata, int n)
{
    if (tridata!=NULL) {
        for (int i=0; i<n; i++) {
            tridata[i].free();
        }
        ::free(tridata);
        tridata = NULL;
    }
}

