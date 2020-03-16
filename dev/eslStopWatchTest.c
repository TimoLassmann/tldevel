#include "tldevel.h"

#include "esl_stopwatch.h"
int main(void)
{
        ESL_STOPWATCH *w = NULL;
        double         t = 0.;

        w = esl_stopwatch_Create();

        /* This tests the minimum *practical* resolution of the clock,
         * inclusive of overhead of calling the stopwatch functions.
         * It gives me ~0.1 usec (12 Jan 2016).
         */
        esl_stopwatch_Start(w);
        while (t == 0.)
        {
                esl_stopwatch_Stop(w);
                t = esl_stopwatch_GetElapsed(w);
        }

        printf("Elapsed time clock has practical resolution of around: %g sec\n", t);

        esl_stopwatch_Display(stdout, w, "CPU Time: ");
        esl_stopwatch_Destroy(w);


        LOG_MSG("macro tests");


        DECLARE_TIMER(n);
        int i;
        double x = 2.0;
        START_TIMER(n);
        for(i = 0; i < 1000;i++){
                x = x* x / 2.24;
        }
        STOP_TIMER(n);
        GET_TIMING(n);

        DESTROY_TIMER(n);


        return OK;
}
