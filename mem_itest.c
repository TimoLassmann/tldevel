

#include "mem.h"

#include "tldevel.h"

int main(int argc, char** argv)
{
        int* tmp = NULL;

        int i,j;
        double f = 3.12435;
        int g = 10;
        tmp= galloc(tmp,10,f);

        tmp= galloc(tmp,3,17.0f);

        tmp= galloc(tmp,39,g);

        //tmp= galloc(tmp,39,10,10.0);
        for(i=0;i<39;i++){
                fprintf(stdout,"%d\n",tmp[i]);
        }

        gfree(tmp);

        double** dd = NULL;
        dd = malloc_2d_double(dd, 10, 20, 0.0);

        int** aligator = NULL;

        fprintf(stdout,"%d %d\n", sizeof *aligator, sizeof **aligator);
        aligator = galloc(aligator, 4,4 , 7 );
        for(i =0; i < 3;i++){
                for(j = 0; j < 3;j++){

                        fprintf(stdout,"%d ", aligator[i][j]);
                }
                fprintf(stdout,"\n");
        }
        fprintf(stdout,"\n");
        aligator = galloc(aligator, 3,4, 4 );

        for(i =0; i < 3;i++){
                for(j = 0; j < 4;j++){

                        fprintf(stdout,"%d ", aligator[i][j]);
                }
                fprintf(stdout,"\n");
        }
        fprintf(stdout,"\n");
        gfree(aligator);
        gfree(dd);
        return 0;
}

