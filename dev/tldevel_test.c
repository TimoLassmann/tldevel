
#include "tldevel.h"

#include <stdint.h>

int main(int argc, char *argv[])
{
        double** ga = NULL;
        int* blah = NULL;
        int d;



        LOG_MSG("%p",blah);

        galloc(&blah,1);
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

        RUN(galloc(&ga,190,400));
        RUN(get_dim1(ga,&d));
        LOG_MSG("d1: %d", d);
        RUN(get_dim2(ga,&d));
        LOG_MSG("d2: %d", d);
        gfree(ga);

        float* f;
        int status = 0;
        status = galloc(&f);
        if(status == OK){
                ERROR_MSG("Should have crashed");
        }else{
                LOG_MSG("galloc correctly reported error");
        }

        uint64_t** otto = NULL;

        status = galloc(&otto, 19,10);
        if(status == OK){
                ERROR_MSG("Should have crashed");
        }else{
                LOG_MSG("galloc correctly reported error");
        }

        return EXIT_SUCCESS;
ERROR:
        gfree(ga);
        gfree(blah);

        return EXIT_FAILURE;

}
