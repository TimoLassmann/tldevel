
#include "tldev.h"


int main(int argc, char *argv[])
{
        double** ga = NULL;
        int* blah = NULL;

        galloc(&blah,-1);
        LOG_MSG("%p",blah);
        gfree(blah);


        LOG_MSG("%p",blah);
        blah = NULL;


        RUN(galloc(&ga,19, 10));
        LOG_MSG("d1: %d", get_dim1(ga));
        LOG_MSG("d2: %d", get_dim2(ga));
        RUN(galloc(&ga,190, -100));
        LOG_MSG("d1: %d", get_dim1(ga));
        LOG_MSG("d2: %d", get_dim2(ga));
        gfree(ga);
        init_logsum();

        float a,b;
        a = prob2scaledprob(0.3f);
        b = prob2scaledprob(0.3f);

        LOG_MSG("SUM: %f", scaledprob2prob(logsum(a, b)));
        return EXIT_SUCCESS;
ERROR:
        gfree(ga);
        gfree(blah);

        return EXIT_FAILURE;

}
