
#include "tools.h"


void x(int sig)
{
    print_message("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    exit(sig);


}



void main()
{
    int  i;
    int  z[10];    

    set_sigseg_handler(x);

    int n = 100000;

    for (i=0; i<n; i++) {
        print_message("i = %d  %d\n", i, *(z+i));
    }

    return; 
}


