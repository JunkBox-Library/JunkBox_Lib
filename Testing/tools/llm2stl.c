
#include <stdio.h>

#include "xtools.h"



int main(int argc, char** argv)
{
    char buf[LNAME];
    int  i, j, vnum, fnum;

    unsigned char   u8;
    unsigned short u16;
    short     int   s16;
    unsigned int   u32;
    int            s32;

    int hasWeights, hasDetailTexCoords;

    float* vertex = NULL;
    short* face   = NULL;
    float* weight = NULL;


    if (argc<3) {
        print_message("Usage ... %s -f llm_file [-m morph_name]\n", argv[0]);
        exit(0);
    }


    // 引数処理
    Buffer fname = make_Buffer(LNAME);
    Buffer morph = make_Buffer_str("Normal");



    for (i=1; i<argc; i++) {
        if      (!strcmp(argv[i],"-f")) {if (i!=argc-1) copy_s2Buffer(argv[i+1], &fname);}
        else if (!strcmp(argv[i],"-m")) {if (i!=argc-1) copy_s2Buffer(argv[i+1], &morph);}
    }


    //
    FILE* fp = fopen(fname.buf, "rb");
    if (fp==NULL) {
        print_message("Canot open file : %s\n", fname.buf);
        free_Buffer(&fname);
        exit(1);
    }

    //
    fread(buf, 24, 1, fp);
    if (strcmp(buf, "Linden Binary Mesh 1.0")) {
        print_message("Header of %s is unmatched!!\n", fname.buf);
        free_Buffer(&fname);
        exit(1);
    }
    free_Buffer(&fname);

    //
    fread(&u8, 1, 1, fp);
    hasWeights = (int)u8;
    fread(&u8, 1, 1, fp);
    hasDetailTexCoords  = (int)u8;
    
    fseek(fp, 37, SEEK_CUR);


    // VERTEX
    fread(&u16, 2, 1, fp);
    vnum = (int)u16;
    printf("VRTX NUM = %d\n", vnum);

    vertex = (float*)malloc(12*vnum);
    if (vertex==NULL) {
        print_message("No More Momory of Vertex.\n");
        exit(1);
    }
    fread(vertex, 12, vnum, fp);

    fseek(fp, 32*vnum, SEEK_CUR);

    if (hasDetailTexCoords) fseek(fp, 8*vnum, SEEK_CUR);
    

    // Weight
    if (hasWeights) {
        weight = (float*)malloc(8*vnum);
        if (weight==NULL) {
            free(vertex);
            print_message("No More Momory of Weight.\n");
            exit(1);
        }

        fread(weight, 4, vnum, fp);

        for (i=0; i<vnum; i++) printf("%d => %f\n", i, weight[i]);
        //fseek(fp, 4*vnum, SEEK_CUR);
    }


    // FACE
    fread(&u16, 2, 1, fp);
    fnum = (int)u16;
    printf("FACE NUM = %d\n", fnum);

    face = (short*)malloc(6*fnum);
    if (face==NULL) {
        free(vertex);
        free(weight);
        print_message("No More Momory of Face.\n");
        exit(1);
    }

    fread(face, 6, fnum, fp);


    // SKIN
    if (hasWeights) {
        fread(&u16, 2, 1, fp);
        int wnum = (int)u16;
        //fseek(fp, 64*wnum, SEEK_CUR);
        printf("SKIN NUM = %d\n", wnum);
        for (i=0; i<wnum; i++) {
            fread(buf, 64, 1, fp);
            printf("%3d: SKIN NAME = %s\n", i, buf);
        }
    }


    // Morph
    fread(buf, 64, 1, fp);
    while (strcmp(buf, "End Morphs")) {
        fread(&s32, 4, 1, fp);
        int mnum = (int)s32;
    
        if (!strcmp(buf, morph.buf)) {
            //
            printf("%s %d\n", buf, mnum);

            float x, y, z;

            for (i=0; i<mnum; i++) {
                fread(&u32, 4, 1, fp);
                fread(&x,   4, 1, fp);
                fread(&y,   4, 1, fp);
                fread(&z,   4, 1, fp);

                int index = 3*u32;
                vertex[index]   += x;
                vertex[index+1] += y;
                vertex[index+2] += z;

                printf("INDEX = %d  JOINT = %f\n", u32, weight[u32]);
                fseek(fp, 32, SEEK_CUR);
            }
        }
        else {
            fseek(fp, 48*mnum, SEEK_CUR);
        }
    
        fread(buf, 64, 1, fp);
    }


    // ReMap
    fread(&s32, 4, 1, fp);
    if (!feof(fp)) {
        int rnum = (int)s32;
        fseek(fp, 8*rnum, SEEK_CUR);
    }


    fclose(fp);


    // output STL
    Buffer stlfn = dup_Buffer(morph);
    cat_s2Buffer(".stl", &stlfn);

    FILE* wf = fopen(stlfn.buf, "w");
    fprintf(wf, "solid %s\n", morph.buf);

    for (i=0; i<fnum; i++) {
        int joint = (int)weight[face[3*i]];

        if (joint==14) {
            //
            fprintf(wf, "facet normal 0 0 0\n");
            fprintf(wf, "outer loop\n");

            for (j=0; j<3; j++) {
                int index = 3*face[3*i + j];
                if (index+2>=3*vnum) {
                    print_message("Algorithm Error!! %d>%d\n", index+2, vnum);
                    fclose(wf);
                    exit(1);
                }

                float xx = vertex[index];
                float yy = vertex[index+1];
                float zz = vertex[index+2];

                fprintf(wf, "    vertex %f %f %f\n", xx, yy, zz);
            }

            fprintf(wf, "endloop\n");
            fprintf(wf, "endfacet\n");
        }
    }

    fprintf(wf, "endsolid %s\n", morph.buf);
    fclose(wf);

    free_Buffer(&morph);
    free_Buffer(&stlfn);

    free(vertex);
    free(weight);
    free(face);

    return 0;
}

