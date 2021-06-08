#include <stdio.h>

#include "tldevel.h"
#include "tlrng.h"

#define HDF5_TEST_FILE "tlrng_hdf5.h5"

void printHistogram(double* values, int n);

static int test_rng_write(void);
int test_rng_write(void)
{
        struct rng_state* rng = NULL;
        int i;
        double* results = NULL;

        RUN(galloc(&results, 10));
        RUNP(rng = init_rng(1));
        RUN(tl_random_write_hdf5(rng,NULL,HDF5_TEST_FILE, "MainGroup"));

        for(i = 0; i < 10;i++){
                results[i] = tl_random_double(rng);
                /* fprintf(stdout,"%d ::: %f\n",i, results[i]); */
        }
        free_rng(rng);
        rng = NULL;

        RUN(tl_random_read_hdf5(&rng,NULL,HDF5_TEST_FILE, "MainGroup"));

        for(i = 0; i < 10;i++){
                fprintf(stdout,"%d ::: %f  (before: %f)\n",i, tl_random_double(rng), results[i]);
        }
        free_rng(rng);

        gfree(results);

        return OK;
ERROR:
        return FAIL;
}


int main(void)
{
        LOG_MSG("Write test");

        RUN(test_rng_write());

        LOG_MSG("Write test done");



        struct rng_state* rng = NULL;
        struct rng_state* rng_second = NULL;
        double* values = NULL;
        int num_val = 1000000;
        int i,j;
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
        LOG_MSG("Gamma");

        for(i = 0; i < 100;i+=5){
                mean = 0.0;
                for(j = 0; j < 10;j++){
                        values[j] = tl_random_gamma(rng, (double) i + 0.01,1.0);
                        mean+=values[j];
                }
                for(j = 0; j < 10;j++){
                        values[j] /= mean;
                }
                for(j = 0; j < 10;j++){
                        fprintf(stdout,"%0.2f ",values[j]);
                }
                fprintf(stdout,"\n");
        }


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
