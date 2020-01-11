#ifndef TLDEVEL_H
#define TLDEVEL_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef TLDEVEL_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif

#define TLDEVEL_VERSION PACKAGE_VERSION

#define OK              0
#define FAIL            1

#define MESSAGE_MARGIN 22

#define MACRO_MIN(a,b)          (((a)<(b))?(a):(b))
#define MACRO_MAX(a,b)          (((a)>(b))?(a):(b))

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ " line " TOSTRING(__LINE__)


#define ERROR_MSG(...) do {                     \
                error(AT, __VA_ARGS__ );        \
                goto ERROR;                     \
        }while (0)

#define WARNING_MSG(...) do {                   \
                warning(AT, __VA_ARGS__ );      \
        }while (0)


#define LOG_MSG(...) do {                       \
                log_message( __VA_ARGS__ );     \
        }while (0)

#define ASSERT(TEST,...)  if(!(TEST)) {         \
                error(AT,#TEST );               \
                error(AT, __VA_ARGS__);         \
                goto ERROR;                     \
        }

#if (DEBUGLEVEL >= 1)
#define DASSERT(TEST,...) if(!(TEST)) {         \
                error(AT,#TEST );               \
                error(AT, __VA_ARGS__);         \
                goto ERROR;                     \
        }
#else
#define DASSERT(TEST,...)

#endif


#define ADDFAILED(x)  "Function \"" TOSTRING(x) "\" failed."

#define RUN(EXP) do {                               \
                if((EXP) != OK){                    \
                        ERROR_MSG(ADDFAILED(EXP));	\
                }                                   \
        }while (0)

#define RUNP(EXP) do {                              \
                if((EXP) == NULL){                  \
                        ERROR_MSG(ADDFAILED(EXP));	\
                }                                   \
        }while (0)


EXTERN int nearly_equal_float(float a, float b);
EXTERN int nearly_equal_double(double a, double b);


#define TLSAFE_EQ(X,Y) _Generic((X),                        \
                                float: nearly_equal_float,  \
                                double: nearly_equal_double \
                )(X,Y)




/* Functions to declare and use a timer */

#define DECLARE_TIMER(n) struct timespec ts1_##n; struct timespec ts2_##n;
#define START_TIMER(n) clock_gettime(CLOCK_MONOTONIC_RAW, &ts1_##n);
#define STOP_TIMER(n) clock_gettime(CLOCK_MONOTONIC_RAW, &ts2_##n);
#define GET_TIMING(n) (double)(ts2_##n.tv_sec - ts1_##n.tv_sec) + ((double)  ts2_##n.tv_nsec - ts1_##n.tv_nsec) / 1000000000.0

/* Memory functions  */

#define MFREE(p) do {                                           \
                if(p){                                          \
                        free(p);                                \
                        p = NULL;                               \
                }else{                                          \
                        WARNING_MSG("free on a null pointer");  \
                }                                               \
        } while (0)

#define MMALLOC(p,size) do {                                          \
                if (p != NULL){                                       \
                        ERROR_MSG( "malloc on a nun-null pointer");   \
                        goto ERROR;                                   \
                }                                                     \
                if(size == 0){                                        \
                        ERROR_MSG("malloc of size %d failed", size);	\
                        goto ERROR;                                   \
                }                                                     \
                if (((p) = malloc(size)) == NULL) {                   \
                        ERROR_MSG("malloc of size %d failed", size);	\
                        goto ERROR;                                   \
                }                                                     \
        } while (0)

#define MREALLOC(p, size) do {                                          \
                void *tmpp;                                             \
                if(size == 0){                                          \
                        ERROR_MSG("malloc of size %d failed", size);    \
                        goto ERROR;                                     \
                }                                                       \
                if ((p) == NULL) {                                      \
                        tmpp = malloc(size);                            \
                }else {                                                 \
                        tmpp = realloc((p), (size));                    \
                }                                                       \
                if (tmpp != NULL){                                      \
                        p = tmpp;                                       \
                }else {                                                 \
                        ERROR_MSG("realloc for size %d failed", size);  \
                        goto ERROR;                                     \
                }} while (0)

/* g memory functions */

EXTERN int get_dim1(void* ptr, int* d);
EXTERN int get_dim2(void* ptr, int* d);

/*
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,N,...) N

#define macro_dispatcher(func, ...) macro_dispatcher_(func, VA_NUM_ARGS(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func ## nargs
*/

#define ALLOC_1D_ARRAY_DEF(type)                                  \
        EXTERN int alloc_1D_array_size_ ##type (type **array, int dim1)

ALLOC_1D_ARRAY_DEF(char);
ALLOC_1D_ARRAY_DEF(int);
ALLOC_1D_ARRAY_DEF(float);
ALLOC_1D_ARRAY_DEF(double);

#define ALLOC_2D_ARRAY_DEF(type)                                        \
        EXTERN int alloc_2D_array_size_ ##type (type ***array, int dim1,int dim2)

ALLOC_2D_ARRAY_DEF(char);
ALLOC_2D_ARRAY_DEF(int);
ALLOC_2D_ARRAY_DEF(float);
ALLOC_2D_ARRAY_DEF(double);


#define FREE_VOID_DEF(type)                     \
        EXTERN void gfree_void_ ##type(type *a)

#define FREE_1D_ARRAY_DEF(type)                 \
        EXTERN void free_1d_array_ ##type(type **array)

#define FREE_2D_ARRAY_DEF(type)                   \
        EXTERN void free_2d_array_ ##type(type ***array)


FREE_VOID_DEF(char);
FREE_VOID_DEF(int);
FREE_VOID_DEF(float);
FREE_VOID_DEF(double);

FREE_1D_ARRAY_DEF(char);
FREE_1D_ARRAY_DEF(int);
FREE_1D_ARRAY_DEF(float);
FREE_1D_ARRAY_DEF(double);

FREE_2D_ARRAY_DEF(char);
FREE_2D_ARRAY_DEF(int);
FREE_2D_ARRAY_DEF(float);
FREE_2D_ARRAY_DEF(double);

EXTERN int galloc_unknown_type_error (void* p, ...);
EXTERN int galloc_too_few_arg_error (void* p);

#define p1(X) _Generic((X),                               \
                       default: galloc_too_few_arg_error  \
                )(X)

#define p2(X,Y) _Generic((X),                                 \
                         char**: alloc_1D_array_size_char,    \
                         int**: alloc_1D_array_size_int,      \
                         float**:  alloc_1D_array_size_float, \
                         double**:alloc_1D_array_size_double, \
                         default: galloc_unknown_type_error  \
                )(X,Y)

#define p3(X,Y,Z) _Generic((X),                                 \
                           char***: alloc_2D_array_size_char,    \
                           int***: alloc_2D_array_size_int,      \
                           float***:  alloc_2D_array_size_float, \
                           double***:alloc_2D_array_size_double, \
                           default: galloc_unknown_type_error   \
                )(X,Y,Z)


#define _ARG3(_0, _1, _2, _3, ...) _3
#define NARG3(...) _ARG3(__VA_ARGS__,3, 2, 1, 0)

#define _GALLOC_ARGS_1( a) p1(a)
#define _GALLOC_ARGS_2( a, b) p2(a,b)
#define _GALLOC_ARGS_3( a, b, c ) p3(a,b,c)

#define __GALLOC_ARGS( N, ...) _GALLOC_ARGS_ ## N ( __VA_ARGS__)
#define _GALLOC_ARGS( N, ...) __GALLOC_ARGS( N, __VA_ARGS__)
#define GALLOC_ARGS( ...) _GALLOC_ARGS( NARG3(__VA_ARGS__), __VA_ARGS__)
#define galloc(...) GALLOC_ARGS( __VA_ARGS__)



#define gfree(X) _Generic((&X),                            \
                          char*: gfree_void_char,          \
                          int*: gfree_void_int,            \
                          float*: gfree_void_float,        \
                          double*: gfree_void_double,      \
                          char**: free_1d_array_char,      \
                          int**: free_1d_array_int,        \
                          float**: free_1d_array_float,    \
                          double**: free_1d_array_double,  \
                          char***: free_2d_array_char,     \
                          int***: free_2d_array_int,       \
                          float***: free_2d_array_float,   \
                          double***: free_2d_array_double  \
                )(&X)

/* functions  */

EXTERN void error(const char *location, const char *format, ...);
EXTERN void warning(const char *location, const char *format, ...);
EXTERN void log_message( const char *format, ...);



EXTERN const char* tldevel_version(void);

#undef TLDEVEL_IMPORT
#undef EXTERN
#endif
