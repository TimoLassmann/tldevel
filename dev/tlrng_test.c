
#include <stdio.h>

#include "tldevel.h"
#include "tlrng.h"


void printHistogram(double* values, int n);

int main(int argc, char *argv[])
{
        struct rng_state* rng = NULL;
        struct rng_state* rng_second = NULL;
        double* values = NULL;
        int num_val = 1000000;
        int i;
        RUNP(rng = init_rng(0));
        RUNP(rng_second = init_rng_from_rng(rng));
        for(i = 0; i < 10;i++){
                fprintf(stdout,"%f\t%d\n", tl_random_double(rng), tl_random_int(rng,10));
                fprintf(stdout,"%f\t%d\n", tl_random_double(rng_second), tl_random_int(rng_second,10));
        }
        double mean = 0.0;
        double stdev = 1.0;


        MMALLOC(values,sizeof(double) * num_val);

        LOG_MSG("Gaussian");
        for(i = 0; i < num_val;i++){
                values[i] = tl_random_gaussian(rng,mean,stdev);
                //fprintf(stdout,"%f\t%d\n", tl_random_double(rng_second), tl_random_int(rng_second,10));
        }

        printHistogram(values,num_val);
        MFREE(values);

        free_rng(rng);
        free_rng(rng_second);
        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;
}

/* from RosettaCode */
// https://rosettacode.org/wiki/Statistics/Normal_distribution#C

void printHistogram(double* values, int n)
{
        const int width = 50;
        int max = 0;

        const double low   = -3.05;
        const double high  =  3.05;
        const double delta =  0.1;

        int i,j,k;
        int nbins = (int)((high - low) / delta);
        int* bins = (int*)calloc(nbins,sizeof(int));
        if ( bins != NULL )
        {
                for ( i = 0; i < n; i++ )
                {
                        int j = (int)( (values[i] - low) / delta );
                        if ( 0 <= j  &&  j < nbins )
                                bins[j]++;
                }

                for ( j = 0; j < nbins; j++ )
                        if ( max < bins[j] )
                                max = bins[j];

                for ( j = 0; j < nbins; j++ )
                {
                        printf("(%5.2f, %5.2f) |", low + j * delta, low + (j + 1) * delta );
                        k = (int)( (double)width * (double)bins[j] / (double)max );
                        while(k-- > 0) putchar('*');
                        printf("  %-.1f%%", bins[j] * 100.0 / (double)n);
                        putchar('\n');
                }

                free(bins);
        }
}
