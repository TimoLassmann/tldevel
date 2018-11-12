#ifndef MEM_HEADER_INC

#define MEM_HEADER_INC

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define GMREALLOC(p, newsize) do {                                       \
                void *tmpp;                                             \
                if ((p) == NULL) {                                      \
                        tmpp = malloc(newsize);                         \
                }else {                                                 \
                        tmpp = realloc((p), (newsize));                 \
                }                                                       \
                if (tmpp != NULL){                                      \
                        p = tmpp;                                       \
                }else {                                                 \
                        goto ERROR;                                     \
                }} while (0)

#define galloc(...) SELECT(__VA_ARGS__)(__VA_ARGS__)

#define SELECT(...) CONCAT(SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)
#define CONCAT(X, Y) CONCAT_(X, Y)
#define CONCAT_(X, Y) X ## Y

#define SELECT_0()
#define SELECT_1(_1) _Generic ((_1),                 \
                default: galloc_void \
                )
#define SELECT_2(_1, _2) _Generic((_1),                                 \
                                  int*: _Generic((_2),                  \
                                                 int: alloc_1D_array_size_int \
                                          ),                            \
                                  float*: _Generic((_2),                \
                                                   int: alloc_1D_array_size_float \
                                          ),                            \
                                  double*: _Generic((_2),               \
                                                    int: alloc_1D_array_size_double \
                                          ),                            \
                                  int_fast32_t*:  _Generic((_2),        \
                                                           int: alloc_1D_array_size_int_fast32_t \
                                          )                             \
                )



#define SELECT_3(_1, _2, _3) _Generic((_1),                             \
                                      double: _Generic((_2),            \
                                                       int: _Generic((_3), \
                                                                     int: foo_double_int_int \
                                                               )        \
                                              )                         \
                )                                                       \



#define ARGN(...) ARGN_(__VA_ARGS__)
#define ARGN_(_0, _1, _2, N, ...) N

#define NARG(...) ARGN(__VA_ARGS__ COMMA(__VA_ARGS__) 3, 2, 1, 0)
#define HAS_COMMA(...) ARGN(__VA_ARGS__, 1, 1, 0)

#define SET_COMMA(...) ,

#define COMMA(...) SELECT_COMMA                     \
        (                                           \
                HAS_COMMA(__VA_ARGS__),             \
                HAS_COMMA(__VA_ARGS__ ()),          \
                HAS_COMMA(SET_COMMA __VA_ARGS__),   \
                HAS_COMMA(SET_COMMA __VA_ARGS__ ()) \
                )

#define SELECT_COMMA(_0, _1, _2, _3) SELECT_COMMA_(_0, _1, _2, _3)
#define SELECT_COMMA_(_0, _1, _2, _3) COMMA_ ## _0 ## _1 ## _2 ## _3

#define COMMA_0000 ,
#define COMMA_0001
#define COMMA_0010 ,
#define COMMA_0011 ,
#define COMMA_0100 ,
#define COMMA_0101 ,
#define COMMA_0110 ,
#define COMMA_0111 ,
#define COMMA_1000 ,
#define COMMA_1001 ,
#define COMMA_1010 ,
#define COMMA_1011 ,
#define COMMA_1100 ,
#define COMMA_1101 ,
#define COMMA_1110 ,
#define COMMA_1111 ,


void galloc_void(void)
{
        fprintf(stdout,"void placeholder called");
}


#define gfree(X) _Generic((X),                                          \
                          int*: free_1d_array_int,                      \
                          float*: free_1d_array_float,                  \
                          double*: free_1d_array_double,                \
                          int_fast32_t*: free_1d_array_int_fast32_t,    \
                          int**: free_2d_array_int,                     \
                          float**: free_2d_array_float,                 \
                          double**: free_2d_array_double,               \
                          int_fast32_t**: free_2d_array_int_fast32_t,   \
                          int***: free_3d_array_int,                    \
                          float***: free_3d_array_float,                \
                          double***: free_3d_array_double,              \
                          int_fast32_t***: free_3d_array_int_fast32_t   \
                )(X)

#define FREE_1D_ARRAY(type)                          \
        void free_1d_array_ ##type(type *array){     \
                int* tmp = (int*)array;              \
                tmp = tmp -2;                        \
                free(tmp);                           \
        }


#define FREE_2D_ARRAY(type)                          \
        void free_2d_array_ ##type(type **array){    \
                int* tmp = (int*)array[0];           \
                tmp = tmp -3;                        \
                free(tmp);                           \
                free(array);                         \
        }

#define FREE_3D_ARRAY(type)                          \
        void free_3d_array_ ##type(type ***array){   \
                int* tmp = (int*)array[0][0];        \
                tmp = tmp -4;                        \
                free(tmp);                           \
                free(array[0]);                      \
                free(array);                         \
        }


        FREE_1D_ARRAY(int)
        FREE_1D_ARRAY(float)
        FREE_1D_ARRAY(double)
        FREE_1D_ARRAY(int_fast32_t)

        FREE_2D_ARRAY(int)
        FREE_2D_ARRAY(float)
        FREE_2D_ARRAY(double)
        FREE_2D_ARRAY(int_fast32_t)

        FREE_3D_ARRAY(int)
        FREE_3D_ARRAY(float)
        FREE_3D_ARRAY(double)
        FREE_3D_ARRAY(int_fast32_t)


#define ALLOC_1D_ARRAY(type)                                            \
        type *alloc_1D_array_size_ ##type (type *array, int size) {     \
                type *tmp = NULL;                                       \
                int* int_ptr = NULL;                                    \
                int old_dim;                                            \
                if(array == NULL){                                      \
                GMREALLOC(tmp,(size  * sizeof *array + 2*sizeof(int))); \
                        if(tmp == NULL) {                               \
                                return NULL;                            \
                        }                                               \
                        int_ptr = (int*)tmp;                            \
                        int_ptr[0] = 1;                                 \
                        int_ptr[1] = size;                              \
                        array = (type*)( int_ptr +2);                   \
                        return array;                                   \
                }else{                                                  \
                        int_ptr = (int*) array;                         \
                        int_ptr = int_ptr - 2;                          \
                        old_dim = *(int_ptr+1);                         \
                        if(old_dim < size){                             \
                                GMREALLOC(int_ptr,(size  * sizeof *array + 2*sizeof(int))); \
                                int_ptr[0] = 1;                         \
                                int_ptr[1] = size;                      \
                                array = (type*)( int_ptr + 2);          \
                        }                                               \
                }                                                       \
                return array;                                           \
ERROR:                                                                  \
gfree(array);                                                           \
return NULL;                                                            \
        }

        ALLOC_1D_ARRAY(int)
        ALLOC_1D_ARRAY(float)
        ALLOC_1D_ARRAY(double)
        ALLOC_1D_ARRAY(int_fast32_t)


#endif
