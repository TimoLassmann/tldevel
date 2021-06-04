#include <stdio.h>
#include <stdlib.h>
#include "tldevel.h"
#include "stdint.h"

static int mem_report(void* g);

#define typename(X) _Generic((X),                       \
                             char: "char",              \
                             int8_t: "int8",            \
                             uint8_t: "uint8_t",        \
                             int16_t: "int16_t",        \
                             uint16_t: "uint16_t",      \
                             int32_t: "int32_t",        \
                             uint32_t: "uint32_t",      \
                             int64_t: "int64_t",        \
                             uint64_t: "uint64_t",      \
                             float: "float",            \
                             double: "double",          \
                             char*: "char*",            \
                             int8_t*: "int8*",          \
                             uint8_t*: "uint8_t*",      \
                             int16_t*: "int16_t*",      \
                             uint16_t*: "uint16_t*",    \
                             int32_t*: "int32_t*",      \
                             uint32_t*: "uint32_t*",    \
                             int64_t*: "int64_t*",      \
                             uint64_t*: "uint64_t*",    \
                             float*: "float*",          \
                             double*: "double*",        \
                             char**: "char**",          \
                             int8_t**: "int8**",        \
                             uint8_t**: "uint8_t**",    \
                             int16_t**: "int16_t**",    \
                             uint16_t**: "uint16_t**",  \
                             int32_t**: "int32_t**",    \
                             uint32_t**: "uint32_t**",  \
                             int64_t**: "int64_t**",    \
                             uint64_t**: "uint64_t**",  \
                             float**: "float**",        \
                             double**: "double**",      \
                             default: "other")

#define printf_dec_format(x) _Generic((x),                  \
                                      char: "%d ",          \
                                      int8_t: "%d ",        \
                                      uint8_t: "%ud ",      \
                                      int16_t: "%d ",       \
                                      uint16_t: "%ud ",     \
                                      int32_t: "%d ",       \
                                      uint32_t: "%ud ",     \
                                      int64_t: "%ld ",      \
                                      uint64_t: "%lud ",    \
                                      float: "%f ",         \
                                      double: "%f ",        \
                                      default: "%d")
/* more code deleted */

#define testprint(x) fprintf(stdout,printf_dec_format(x), x)

#define TEST_1D(type)                                               \
        int test_1D_ ##type (void);                                 \
        int test_1D_ ##type (void) {                                \
                type* x = NULL;                                     \
                LOG_MSG("----------------------------------");      \
                LOG_MSG("Testing: %s aligned ", typename(x) );      \
                fprintf(stdout,"\n");                               \
                int i;                                              \
                RUN(galloc_aligned(&x,11));                         \
                mem_report(x);                                      \
                for(i = 0; i < 11; i++){                            \
                        x[i] = i;                                   \
                }                                                   \
                fprintf(stdout,"\t");                               \
                for(i = 0; i < 11; i++){                            \
                        testprint(x[i]);                            \
                }                                                   \
                fprintf(stdout,"\n");                               \
                fprintf(stdout,"\t");                               \
                RUN(galloc_aligned(&x,121));                        \
                mem_report(x);                                      \
                for(i = 0; i < 121; i++){                           \
                        testprint(x[i]);                            \
                }                                                   \
                fprintf(stdout,"\n");                               \
                RUN(galloc_aligned(&x,2));                          \
                mem_report(x);                                      \
                fprintf(stdout,"\t");                               \
                for(i = 0; i < 2; i++){                             \
                        testprint(x[i]);                            \
                }                                                   \
                fprintf(stdout,"\n");                               \
                gfree(x);                                           \
                fprintf(stdout,"\n");                               \
                LOG_MSG("----------------------------------");      \
                x = NULL;                                           \
                LOG_MSG("Testing: %s unaligned ", typename(x) );    \
                fprintf(stdout,"\n");                               \
                RUN(galloc(&x,11));                                 \
                mem_report(x);                                      \
                for(i = 0; i < 11; i++){                            \
                        x[i] = i;                                   \
                }                                                   \
                fprintf(stdout,"\t");                               \
                for(i = 0; i < 11; i++){                            \
                        testprint(x[i]);                            \
                }                                                   \
                fprintf(stdout,"\n");                               \
                fprintf(stdout,"\t");                               \
                RUN(galloc(&x,121));                                \
                mem_report(x);                                      \
                for(i = 0; i < 121; i++){                           \
                        testprint(x[i]);                            \
                }                                                   \
                fprintf(stdout,"\n");                               \
                RUN(galloc(&x,2));                                  \
                mem_report(x);                                      \
                fprintf(stdout,"\t");                               \
                for(i = 0; i < 2; i++){                             \
                        testprint(x[i]);                            \
                }                                                   \
                fprintf(stdout,"\n");                               \
                gfree(x);                                           \
                fprintf(stdout,"\n");                               \
                LOG_MSG("----------------------------------");      \
                return OK;                                          \
        ERROR:                                                      \
                return FAIL;                                        \
        }


TEST_1D(char)
TEST_1D(int8_t)
TEST_1D(uint8_t)
TEST_1D(int16_t)
TEST_1D(uint16_t)
TEST_1D(int32_t)
TEST_1D(uint32_t)
TEST_1D(int64_t)
TEST_1D(uint64_t)
TEST_1D(float)
TEST_1D(double)


#define TEST_2D(type)                                               \
        int test_2D_ ##type (void);                                 \
        int test_2D_ ##type (void) {                                \
                type** x = NULL;                                    \
                LOG_MSG("----------------------------------");      \
                LOG_MSG("Testing: %s aligned ", typename(x) );      \
                fprintf(stdout,"\n");                               \
                int i;                                              \
                int j;                                              \
                RUN(galloc_aligned(&x,5,5));                        \
                mem_report(x);                                      \
                RUN(galloc_aligned(&x,71,75));                      \
                mem_report(x);                                      \
                RUN(galloc_aligned(&x,2,2));                        \
                mem_report(x);                                      \
                gfree(x);                                           \
                fprintf(stdout,"\n");                               \
                LOG_MSG("----------------------------------");      \
                x = NULL;                                           \
                RUN(galloc(&x,5,5));                                \
                mem_report(x);                                      \
                RUN(galloc(&x,71,75));                              \
                mem_report(x);                                      \
                RUN(galloc(&x,2,2));                                \
                mem_report(x);                                      \
                gfree(x);                                           \
                LOG_MSG("Testing: %s unaligned ", typename(x) );    \
                LOG_MSG("----------------------------------");      \
                return OK;                                          \
        ERROR:                                                      \
                return FAIL;                                        \
        }

TEST_2D(char)
TEST_2D(int8_t)
TEST_2D(uint8_t)
TEST_2D(int16_t)
TEST_2D(uint16_t)
TEST_2D(int32_t)
TEST_2D(uint32_t)
TEST_2D(int64_t)
TEST_2D(uint64_t)
TEST_2D(float)
TEST_2D(double)


int main(void)
{

        double** g = NULL;
        int i;

        uint16_t* x = NULL;



        test_1D_char();
        test_1D_int8_t();
        test_1D_uint8_t();
        test_1D_int16_t();
        test_1D_uint16_t();
        test_1D_int32_t();
        test_1D_uint32_t();
        test_1D_int64_t();
        test_1D_uint64_t();
        test_1D_float();
        test_1D_double();

        test_2D_char();
        test_2D_int8_t();
        test_2D_uint8_t();
        test_2D_int16_t();
        test_2D_uint16_t();
        test_2D_int32_t();
        test_2D_uint32_t();
        test_2D_int64_t();
        test_2D_uint64_t();
        test_2D_float();
        test_2D_double();

        return EXIT_SUCCESS;
}

int mem_report(void* g)
{
        int d1;
        int d2;

        get_dim1(g, &d1);

        get_dim2(g, &d2);

        fprintf(stdout,"\tSize: %d %d Alignment: %p %d\n", d1,d2,(void*) g, (int) ( (uint64_t)g % 64));


        return OK;
}
