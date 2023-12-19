
#include "tools.h"


int main(int argc, char** argv)
{
    int   i, flg;
    char  buf[BUFSZ];
    FILE* fp;
    char* pp;
    UNUSED(pp);

    if (argc<2) {
        fprintf(stderr,"Usage %s c_source_file \n", argv[0]);
        exit(1);
    }

    fp = fopen(argv[1], "r");
    if (fp==NULL) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(1);
    }

    flg = OFF;
    pp = fgets(buf, BUFSZ, fp);
    replace_str(buf, BUFSZ, "@\\", "\\");
    replace_str(buf, BUFSZ, "@b ", "");

    while (!feof(fp)) {
        if (flg==ON) {
              i = 0;
            while (buf[i]!='\0' && flg==ON) {
                if (!strncmp(&(buf[i]), "*/", 2)) {
                    flg = OFF;
                    fprintf(stdout, "\n\n\n\n\n\n");
                }
                i++;
            }
            if (flg==ON) {
                // for @n
                int pos = strlen(buf);
                if (pos>0 && buf[pos-1]==CHAR_LF) {
                    if (pos>1 && buf[pos-2]==CHAR_CR) pos--;
                    if (pos>2 && buf[pos-2]=='n' && buf[pos-3]=='@') {
                        buf[pos-2] = '\0';    
                        buf[pos-3] = '\n';    
                    }
                }
                //
                fprintf(stdout, "%s", buf);
            }
        }
        else {
            //
            i = 0;
            while (i<BUFSZ-3 && buf[i]!='\0' && flg==OFF) {
                if (!strncmp(&(buf[i]), "/**", 3)) {
                    flg = ON;
                    fprintf(stdout, "---------------------------------\n");

                    if (i<BUFSZ-4 && (buf[i+3]==' ' || buf[i+3]==CHAR_TAB)) {
                        char cc = buf[i+4]; 
                        if (cc!='\0' && cc!=CHAR_CR && cc!=CHAR_LF) {
                            char* prptr = &(buf[i+4]);
                            while(*prptr==' ' || *prptr==CHAR_TAB) prptr++;
                            fprintf(stdout, "%s", prptr);
                        }
                    }
                }
                i++;
            }
        }
        pp = fgets(buf, BUFSZ, fp);

        replace_str(buf, BUFSZ, "@\\", "\\");
        replace_str(buf, BUFSZ, "@b ", "");
    }

    fclose(fp);

    return 0;
}

