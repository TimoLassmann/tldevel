

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
        int i,j;
        double f = 3.12435;
        int g = 10;
        MMALLOC(test, sizeof(struct test));
        test->a = NULL;
        test->a = galloc(test->a,2,4);
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

        int** aligator = NULL;

        fprintf(stdout,"%d %d\n", sizeof *aligator, sizeof **aligator);
        aligator = galloc(aligator, 4,4);
        fprintf(stdout,"len: %d\n", DIM1(aligator));
        fprintf(stdout,"len: %d\n", DIM2(aligator));
        aligator = galloc(aligator, 3,40);
        fprintf(stdout,"len: %d\n", DIM1(aligator));
        fprintf(stdout,"len: %d\n", DIM2(aligator));
        
        gfree(aligator);
        return 0;
ERROR:
        return 1;
}

//gcc -O3 -fdata-sections -ffunction-sections  -std=gnu11  mem_itest.c libtldevel.a  -lm  -Wl,--gc-sections
 
