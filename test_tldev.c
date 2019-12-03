
#include "tldev.h"


int main(int argc, char *argv[])
{
        double** ga = NULL;
        int* blah = NULL;
        int d;

        galloc(&blah,-1);
        LOG_MSG("%p",blah);
        RUN(get_dim1(blah,&d));
        LOG_MSG("d1: %d", d);
        RUN(get_dim2(blah,&d));
        LOG_MSG("d2: %d", d);

        gfree(blah);
        LOG_MSG("%p",blah);
        galloc(&blah,2765);
        RUN(get_dim1(blah,&d));
        LOG_MSG("d1: %d", d);
        RUN(get_dim2(blah,&d));
        LOG_MSG("d2: %d", d);

        LOG_MSG("%p",blah);
        gfree(blah);


        LOG_MSG("%p",blah);
        blah = NULL;


        RUN(galloc(&ga,19, 10));
        RUN(get_dim1(ga,&d));
        LOG_MSG("d1: %d", d);
        RUN(get_dim2(ga,&d));
        LOG_MSG("d2: %d", d);

        RUN(galloc(&ga,190, -100));
        RUN(get_dim1(ga,&d));
        LOG_MSG("d1: %d", d);
        RUN(get_dim2(ga,&d));
        LOG_MSG("d2: %d", d);
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
