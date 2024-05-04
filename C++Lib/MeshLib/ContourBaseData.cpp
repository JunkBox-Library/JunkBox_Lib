
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
    //w1.init();
    //w2.init();
    //w3.init();
}


void  ContourTriData::free(void)
{
    contourNum = 0;

    //w1.free();
    //w2.free();
    //w3.free();
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
    num_index  = idx;
    num_data   = num;
    vcount     = 3;

    index      = NULL;
    vertex     = NULL;
    normal     = NULL;
    texcrd     = NULL;
    weight     = NULL;
}


void  ContourBaseData::free(void)
{
    freeNull(index);
    freeNull(vertex);
    freeNull(normal);
    freeNull(texcrd);
    freeArrayParams<double>(weight, num_data);

    init(0, 0);
}


bool  ContourBaseData::getm(void)
{
    index  = (int*)malloc(sizeof(int)*num_index);
    vertex = (Vector<double>*)malloc(sizeof(Vector<double>)*num_data);
    normal = (Vector<double>*)malloc(sizeof(Vector<double>)*num_data);
    texcrd = (UVMap <double>*)malloc(sizeof(UVMap<double>) *num_data);
    weight = (ArrayParam<double>*)malloc(sizeof(ArrayParam<double>)*num_data);

    if (index==NULL || vertex==NULL || normal==NULL || texcrd==NULL) {
        this->free();
        return false;
    }

    memset(index,  0, sizeof(int)*num_data);
    memset(vertex, 0, sizeof(Vector<double>)*num_data);
    memset(normal, 0, sizeof(Vector<double>)*num_data);
    memset(texcrd, 0, sizeof(UVMap<double>) *num_data);
    memset(weight, 0, sizeof(ArrayParam<double>)*num_data);

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
            weight[i].dup(a.weight[i]);
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
        weight[i].init();
    }
}


void  TriPolygonData::free(void)
{
    for (int i=0; i<3; i++) {
        if (!weight[i].get_size()) weight[i].free();
    }
    init();
}


void  TriPolygonData::dup(TriPolygonData a)
{
    polygonNum = a.polygonNum;     ///< ポリゴン番号
    has_normal = a.has_normal;     ///< 配列データの場合，一番最初のデータが値を持っていれば十分である．
    has_texcrd = a.has_texcrd;     ///< 配列データの場合，一番最初のデータが値を持っていれば十分である．
    has_weight = a.has_weight;     ///< 配列データの場合，一番最初のデータが値を持っていれば十分である．
    //
    for (int i=0; i<3; i++) {
        vertex[i] = a.vertex[i];
        normal[i] = a.normal[i];
        texcrd[i] = a.texcrd[i];
        weight[i].dup(a.weight[i]);
    }
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
    //DEBUG_MODE PRINT_MESG("JBXL::freeTriPolygonData(): start.\n");
    if (n<=0) return;

    if (tridata!=NULL) {
        for (int i=0; i<n; i++) {
            tridata[i].free();
        }
        ::free(tridata);
        tridata = NULL;
    }
    //DEBUG_MODE PRINT_MESG("JBXL::freeTriPolygonData(): end.\n");
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// SkinJointData
//

void  SkinJointData::init(int n)
{
    joint_num = 0;
    pelvis_offset = 0.0;

    inverse_bind = NULL;
    alt_inverse_bind = NULL;
    bind_shape.init();
    joint_names.init();

    if (n>0) {
        joint_num = n;
        inverse_bind     = (Matrix<double>*)malloc(sizeof(Matrix<double>)*joint_num);
        alt_inverse_bind = (Matrix<double>*)malloc(sizeof(Matrix<double>)*joint_num);
        //
        for (int i=0; i<joint_num; i++) {
            inverse_bind[i]     = Matrix<double>(2, 4, 4);
            alt_inverse_bind[i] = Matrix<double>(2, 4, 4);
        }
        bind_shape = Matrix<double>(2, 4, 4);
        //
        joint_names.init(joint_num);
    }
}


void  SkinJointData::free()
{
    for (int i=0; i<joint_num; i++) {
        inverse_bind[i].free();
        alt_inverse_bind[i].free();
    }
    freeNull(inverse_bind);
    freeNull(alt_inverse_bind);
    bind_shape.free();
    //
    joint_names.free_ptr();
    joint_names.free();

    init();
}

