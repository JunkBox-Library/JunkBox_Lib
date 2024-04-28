
#include "ContourBaseData.h"


using namespace jbxl;


///////////////////////////////////////////////////////////////////////////////////////////////
// ContourTriIndex
//

void  ContourTriIndex::set(int w1, int w2, int w3, int m1, int m2, int m3, int u1, int u2, int u3)
{
     v1 = w1;  v2 = w2;  v3 = w3;
     n1 = m1;  n2 = m2;  n3 = m3;
    uv1 = u1; uv2 = u2, uv3 = u3;
}


void  ContourTriIndex::mlt_set(int d1, int d2, int d3)
{
     v1 = d1;  v2 = d2;  v3 = d3;
     n1 = d1;  n2 = d2;  n3 = d3;
    uv1 = d1; uv2 = d2, uv3 = d3;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ContourTriData
//

void  ContourTriData::init(void)
{
    contourNum = 0;

    v1.init();
    v2.init();
    v3.init();
    n1.init();
    n2.init();
    n3.init();
    uv1.init();
    uv2.init();
    uv3.init();

    memset(&w1, 0, sizeof(llsd_weight));
    memset(&w2, 0, sizeof(llsd_weight));
    memset(&w3, 0, sizeof(llsd_weight));
}


void  ContourTriData::execScale(double x, double y, double z)
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


void  ContourTriData::execRotate(Quaternion<double> q)
{
    v1 = q.execRotation(v1);
    v2 = q.execRotation(v2);
    v3 = q.execRotation(v3);
    n1 = q.execRotation(n1);
    n2 = q.execRotation(n2);
    n3 = q.execRotation(n3);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Contour Base Data
//

void  ContourBaseData::init(int idx, int num)
{
    num_index = idx;
    num_data  = num;
    vcount    = 3;

    index     = NULL;
    vertex    = NULL;
    normal    = NULL;
    texcrd    = NULL;
    weight    = NULL;
}


void  ContourBaseData::free(void)
{
    freeNull(index);

    freeNull(vertex);
    freeNull(normal);
    freeNull(texcrd);
    freeNull(weight);

    init();
}


bool  ContourBaseData::getm(void)
{
    index = (int*)malloc(sizeof(int)*num_index);

    vertex = (Vector<double>*)malloc(sizeof(Vector<double>)*num_data);
    normal = (Vector<double>*)malloc(sizeof(Vector<double>)*num_data);
    texcrd = (UVMap <double>*)malloc(sizeof(UVMap <double>)*num_data);
    weight = (llsd_weight*)   malloc(sizeof(llsd_weight)*num_data);

    if (index==NULL || vertex==NULL || normal==NULL || texcrd==NULL) {
        this->free();
        return false;
    }

    return true;
}


void  ContourBaseData::dup(ContourBaseData a)
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
            weight[i] = a.weight[i];
        }
    }
    return;
}


void  ContourBaseData::execScale(Vector<double> scale)
{
    for (int i=0; i<num_data;  i++) {
        vertex[i].x *= scale.x;
        vertex[i].y *= scale.y;
        vertex[i].z *= scale.z;
    }
}


void  ContourBaseData::execShift(Vector<double> shift)
{
    for (int i=0; i<num_data; i++) {
        vertex[i].x += shift.x;
        vertex[i].y += shift.y;
        vertex[i].z += shift.z;
    }
}


void  ContourBaseData::execRotate(Quaternion<double> quat)
{
    for (int i=0; i<num_data; i++) {
        vertex[i] = VectorRotation(vertex[i], quat);
        normal[i] = VectorRotation(normal[i], quat);
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Triangle Polygon Data
//

void  TriPolygonData::init(void)
{
    polygonNum = -1;
    has_normal = false;
    has_texcrd = false;
    has_weight = false;
    //
    for (int i=0; i<3; i++) {
        vertex[i].init();
        normal[i].init();
        texcrd[i].init();
        memset(&weight[i], 0, sizeof(llsd_weight));
    }
}


void  TriPolygonData::dup(TriPolygonData a)
{
    *this = a;
}


void  TriPolygonData::execScale(Vector<double> scale)
{
    for (int i=0; i<3; i++) {
        vertex[i].x *= scale.x;
        vertex[i].y *= scale.y;
        vertex[i].z *= scale.z;
    }
}


void  TriPolygonData::execShift(Vector<double> shift)
{
    for (int i=0; i<3; i++) {
        vertex[i].x += shift.x;
        vertex[i].y += shift.y;
        vertex[i].z += shift.z;
    }
}


void  TriPolygonData::execRotate(Quaternion<double> quat)
{
    for (int i=0; i<3; i++) {
        vertex[i] = VectorRotation(vertex[i], quat);
        normal[i] = VectorRotation(normal[i], quat);
    }
}



//////////////////////////////////////////////////////////////////////////////////
//

TriPolygonData* jbxl::dupTriPolygonData(TriPolygonData* data, int num)
{
    if (data==NULL) return NULL;

    TriPolygonData* dup = (TriPolygonData*)malloc(num*sizeof(TriPolygonData));
    if (dup==NULL) return NULL;

    for (int i=0; i<num; i++) dup[i].dup(data[i]);
    return dup;
}


TriPolygonData*  jbxl::joinTriPolygonData(TriPolygonData*& first, int num_p, TriPolygonData*& next, int num_n)
{
    if (first==NULL) return next;
    if (next ==NULL) return first;

    TriPolygonData* join = (TriPolygonData*)malloc((num_p+num_n)*sizeof(TriPolygonData));
    if (join==NULL) return NULL;
    
    int num = 0;
    for (int i=0; i<num_p; i++) {
        join[i].dup(first[i]);
        if (join[i].polygonNum>num) num = join[i].polygonNum;
    }
    num += 1;

    for (int i=0; i<num_n; i++) {
        join[num_p+i].dup(next[i]);
        join[num_p+i].polygonNum += num;
    }

    freeTriPolygonData(first, num_p);
    freeTriPolygonData(next,  num_n);

    return join;
}


void  jbxl::freeTriPolygonData(TriPolygonData*& tridata, int n)
{
    if (tridata!=NULL) {
        for (int i=0; i<n; i++) {
            tridata[i].free();
        }
        ::free(tridata);
        tridata = NULL;
    }
}

