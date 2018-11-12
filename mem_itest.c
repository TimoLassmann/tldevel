

#include "mem.h"

#include "tldevel.h"

int main(int argc, char** argv)
{
        float* tmp = NULL;

        int i;

        tmp= galloc(tmp,10);

        tmp= galloc(tmp,3);

        tmp= galloc(tmp,39);
        for(i=0;i<39;i++){
                tmp[i] = (float)i *0.77;
        }

        gfree(tmp);

        double** dd = NULL;
        dd = malloc_2d_double(dd, 10, 20, 0.0);



        gfree(dd);
        return 0;
}

