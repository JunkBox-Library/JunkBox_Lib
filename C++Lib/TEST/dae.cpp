

#include <dae.h>
#include <dae/daeUtils.h>
#include <dom/domCOLLADA.h>


#include "tools++.h"
#include "buffer.h"
#include "TriBrep.h"


using namespace ColladaDOM141;
using namespace jbxl;


void  Brep2Dae(BREP_SOLID* solid, daeElement* root, char* name)
{
    BREP_VERTEX** vertexp;
    long int  num;

    vertexp = GetOctreeVertices(solid->octree, &num);
    

/*
    for (int i=0; i<num; i++) {
        BREP_VERTEX* vertex = vertexp[i];
        printf("%d   (%f, %f, %f)\n", vertex->index, vertex->point.x, vertex->point.y, vertex->point.z);
    }
*/












    Buffer randstr   = make_Buffer_randomstr(8);
    Buffer vtxsource = make_Buffer_str("#VTXSOURCE_");
    Buffer nrmsource = make_Buffer_str("#NRMSOURCE_");
    Buffer txtsource = make_Buffer_str("#TXTSOURCE_");
    Buffer vertexid  = make_Buffer_str("#VERTEXID_");

    cat_Buffer(&randstr, &vtxsource);
    cat_Buffer(&randstr, &nrmsource);
    cat_Buffer(&randstr, &txtsource);
    cat_Buffer(&randstr, &vertexid);

    //
    daeElement* geometry = root->add("library_geometries geometry");
    geometry->setAttribute("id", (char*)randstr.buf);
    geometry->setAttribute("name", name);
    //
    daeElement* mesh = geometry->add("mesh");

    // Vertex Source
    daeElement* vsurc = mesh->add("source");
    vsurc->setAttribute("id", (char*)(vtxsource.buf+1));
    //
    daeElement* varray = vsurc->add("float_array");
    varray->setAttribute("count", itostr(num*3));
    varray->setAttribute("digits", "float");
    for (int i=0; i<num; i++) {
        VECTOR<double> vect = vertexp[i]->point;
        ((domFloat_array*)varray)->getValue().append3((float)vect.x, (float)vect.y, (float)vect.z);
    }



/*
    // Normal Source
    daeElement* nsurc = mesh->add("source");
    nsurc->setAttribute("id", (char*)(nrmsource.buf+1));
    daeElement* nfary = nsurc->add("float_array");
*/

/*
    // Texture Sorce
    daeElement* tsurc = mesh->add("source");
    tsurc->setAttribute("id", (char*)(txtsource.buf+1));
*/


    // Vertex
    daeElement* vertx = mesh->add("vertices");
    vertx->setAttribute("id", (char*)(vertexid.buf+1));
    //
    daeElement* vinput = vertx->add("input");
    vinput->setAttribute("semantic", "POSITION");
    vinput->setAttribute("source", (char*)vtxsource.buf);


    // Triangles
    domElement* triangle = mesh->add("triangles");
    triangle->setAttribute("count", itostr(solid->facetno));
    //
    daeElement* inputv = triangle->add("input");
    inputv->setAttribute("offset", "0");
    inputv->setAttribute("semantic", "VERTEX");
    inputv->setAttribute("source", (char*)vertexid.buf);

    domP* pindex = (domP*)triangle->add("p");
    BREP_CONTOUR_LIST::iterator icon;
    for (icon=solid->contours.begin(); icon!=solid->contours.end(); icon++){
        BREP_WING* wing = (*icon)->wing;
        for (int i=0; i<3; i++) {
            BREP_VERTEX* vect = wing->vertex;
            pindex->getValue().append(vect->index);
            wing = wing->next;
        }
    }

//  daeElement* inputn = triangle->add("input");
//  inputn->setAttribute("offset", "1");
//  inputn->setAttribute("semantic", "NORMAL");
//  inputn->setAttribute("source", (char*)nrmsource.buf);

//  daeElement* inputt = triangle->add("input");
//  inputt->setAttribute("offset", "2");
//  inputt->setAttribute("semantic", "TEXCOORD");
//  inputt->setAttribute("source", (char*)txtsource.buf);


    //
    daeElement* vir_scene = root->add("library_visual_scenes visual_scene");

    free_Buffer(&randstr);
    free_Buffer(&vtxsource);
    free_Buffer(&nrmsource);
    free_Buffer(&txtsource);
    free_Buffer(&vertexid);
}





int main()
{
    long int fno;

    //STLData* stldata = ReadSTLFile("Normal.stl", &fno);
    STLData* stldata = ReadSTLFile("tri.stl", &fno);

    //
    BREP_SOLID* solid  = new BREP_SOLID();
    int vfno = CreateSolidFromSTL(stldata, fno, solid);

    printf("%d %d\n", fno, vfno);

    BREP_CONTOUR_LIST::iterator icon;
    for (icon=solid->contours.begin(); icon!=solid->contours.end(); icon++){
        println_FacetAsciiSTL(*icon);
        printf("\n");
    }


    //
    DAE dae;
    
    daeElement* root  = dae.add("sample.dae");
    daeElement* asset = root->add("asset");

    daeElement* contrib  = asset->add("contributor");
    daeElement* created  = asset->add("created");
    daeElement* modified = asset->add("modified");

    const char* date = "2014-07-24";
    created->setCharData(date);
    modified->setCharData(date);


    Brep2Dae(solid, root, "TEST");


    //
    dae.writeAll();
    //
    freeSTL(stldata);
    delete solid;

    return 0;
}
