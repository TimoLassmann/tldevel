

#include "tldevel.h"

struct test{
        float** a;
        int num_items;
};

int main(int argc, char** argv)
{
        print_program_header(argv, "MEMORY TESTING PROGRAM");
        int* tmp = NULL;
        struct test* test = NULL;

        MMALLOC(test, sizeof(struct test));
        test->a = NULL;
        test->a = galloc(test->a,2,4,0.5f);
        gfree(test->a);
        MFREE(test);
        tmp= galloc(tmp,1);
        fprintf(stdout,"len: %d\n", DIM1(tmp));
        fprintf(stdout,"len: %d\n", DIM2(tmp));

        tmp= galloc(tmp,1);
        fprintf(stdout,"len: %d\n", DIM1(tmp));
        fprintf(stdout,"len: %d\n", DIM2(tmp));

        tmp= galloc(tmp,1);
        fprintf(stdout,"len: %d\n", DIM1(tmp));
        fprintf(stdout,"len: %d\n", DIM2(tmp));
        gfree(tmp);

        ulong** aligator = NULL;
        int i,j,c =0;

        fprintf(stdout,"%ld %ld\n", sizeof *aligator, sizeof **aligator);
        aligator = galloc(aligator, 4,4,0);
        fprintf(stdout,"len: %d\n", DIM1(aligator));
        fprintf(stdout,"len: %d\n", DIM2(aligator));
        for(i = 0; i < DIM1(aligator);i++){
                for(j = 0; j < DIM2(aligator);j++){
                        //aligator[i][j] = c;
                        fprintf(stdout,"%*ld ",3,aligator[i][j]);
                        c++;
                }
                fprintf(stdout,"\n");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"\n");
        aligator = galloc(aligator, 4,6,3);
        fprintf(stdout,"len: %d\n", DIM1(aligator));
        fprintf(stdout,"len: %d\n", DIM2(aligator));
        for(i = 0; i < DIM1(aligator);i++){
                for(j = 0; j < DIM2(aligator);j++){
                        fprintf(stdout,"%*ld ",3,aligator[i][j]);
                }
                fprintf(stdout,"\n");
        }
        fprintf(stdout,"\n");

        aligator = galloc(aligator, 6,6,8);
        fprintf(stdout,"len: %d\n", DIM1(aligator));
        fprintf(stdout,"len: %d\n", DIM2(aligator));
        for(i = 0; i < DIM1(aligator);i++){
                for(j = 0; j < DIM2(aligator);j++){
                        fprintf(stdout,"%*ld ",3,aligator[i][j]);
                }
                fprintf(stdout,"\n");
        }
        fprintf(stdout,"\n");

        aligator = galloc(aligator, 5,7,9);
        fprintf(stdout,"len: %d\n", DIM1(aligator));
        fprintf(stdout,"len: %d\n", DIM2(aligator));
        for(i = 0; i < DIM1(aligator);i++){
                for(j = 0; j < DIM2(aligator);j++){
                        fprintf(stdout,"%*ld ",3,aligator[i][j]);
                }
                fprintf(stdout,"\n");
        }
        fprintf(stdout,"\n");
        gfree(aligator);

        return 0;
ERROR:
        return 1;
}

//gcc -O3 -fdata-sections -ffunction-sections  -std=gnu11  mem_itest.c libtldevel.a  -lm  -Wl,--gc-sections
 
