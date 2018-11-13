#ifndef MEM_HEADER_INC

#define MEM_HEADER_INC

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tldevel.h"

#define galloc(...) SELECT(__VA_ARGS__)(__VA_ARGS__)

#define SELECT(...) CONCAT(SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)
#define CONCAT(X, Y) CONCAT_(X, Y)
#define CONCAT_(X, Y) X ## Y

#define SELECT_0()
#define SELECT_1(_1) _Generic ((_1),                 \
                default: galloc_void \
                )
/* #define SELECT_2(_1, _2) _Generic((_1),              \ */
/*                                   default: galloc_void  \ */
/*                 ) */




#define SELECT_3(_1, _2, _3) _Generic((_1),                             \
                                      int*: _Generic((_2),              \
                                                     int: alloc_1D_array_size_int \
                                              ),                        \
                                      float*: _Generic((_2),            \
                                                       int: alloc_1D_array_size_float \
                                              ),                        \
                                      double*: _Generic((_2),           \
                                                        int: alloc_1D_array_size_double \
                                              ),                        \
                                      int_fast32_t*:  _Generic((_2),    \
                                                               int: alloc_1D_array_size_int_fast32_t \
                                              )                         \
                )


#define SELECT_4(_1, _2, _3, _4) _Generic((_1),                         \
                                          int**: _Generic((_2),          \
                                                         int: alloc_2D_array_size_int \
                                                  ),                    \
                                          float**: _Generic((_2),        \
                                                           int: alloc_2D_array_size_float \
                                                  ),                    \
                                          double**: _Generic((_2),       \
                                                            int: alloc_2D_array_size_double \
                                                  ),                    \
                                          int_fast32_t**:  _Generic((_2), \
                                                                   int: alloc_2D_array_size_int_fast32_t \
                                                  )                     \
                )




#define ARGN(...) ARGN_(__VA_ARGS__)
#define ARGN_(_0, _1, _2, _3 , N, ...) N

#define NARG(...) ARGN(__VA_ARGS__ COMMA(__VA_ARGS__) 4, 3, 2, 1, 0)
#define HAS_COMMA(...) ARGN(__VA_ARGS__, 1, 1, 0)

#define SET_COMMA(...) ,

#define COMMA(...) SELECT_COMMA                     \
        (                                           \
                HAS_COMMA(__VA_ARGS__),             \
                HAS_COMMA(__VA_ARGS__ ()),          \
                HAS_COMMA(SET_COMMA __VA_ARGS__),   \
                HAS_COMMA(SET_COMMA __VA_ARGS__),   \
                HAS_COMMA(SET_COMMA __VA_ARGS__ ()) \
                )

#define SELECT_COMMA(_0, _1, _2, _3, _4) SELECT_COMMA_(_0, _1, _2, _3, _4)
#define SELECT_COMMA_(_0, _1, _2, _3, _4) COMMA_ ## _0 ## _1 ## _2 ## _3 ## _4

#define COMMA_00000 ,
#define COMMA_00001
#define COMMA_00010 ,
#define COMMA_00011 ,
#define COMMA_00100 ,
#define COMMA_00101 ,
#define COMMA_00110 ,
#define COMMA_00111 ,
#define COMMA_01000 ,
#define COMMA_01001 ,
#define COMMA_01010 ,
#define COMMA_01011 ,
#define COMMA_01100 ,
#define COMMA_01101 ,
#define COMMA_01110 ,
#define COMMA_01111 ,
#define COMMA_10000 ,
#define COMMA_10001 ,
#define COMMA_10010 ,
#define COMMA_10011 ,
#define COMMA_10100 ,
#define COMMA_10101 ,
#define COMMA_10110 ,
#define COMMA_10111 ,
#define COMMA_11000 ,
#define COMMA_11001 ,
#define COMMA_11010 ,
#define COMMA_11011 ,
#define COMMA_11100 ,
#define COMMA_11101 ,
#define COMMA_11110 ,
#define COMMA_11111 ,




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
                          int_fast32_t**: free_2d_array_int_fast32_t   \
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



        FREE_1D_ARRAY(int)
        FREE_1D_ARRAY(float)
        FREE_1D_ARRAY(double)
        FREE_1D_ARRAY(int_fast32_t)

        FREE_2D_ARRAY(int)
        FREE_2D_ARRAY(float)
        FREE_2D_ARRAY(double)
        FREE_2D_ARRAY(int_fast32_t)


#define ALLOC_1D_ARRAY(type)                                            \
                type *alloc_1D_array_size_ ##type (type *array, int dim1, type value) { \
                        type *tmp = NULL;                               \
                        int* int_ptr = NULL;                            \
                        int old_dim;                                    \
                        int i;                                          \
                        if(array == NULL){                              \
                                MMALLOC(tmp,(dim1  * sizeof *array + 2*sizeof(int))); \
                                int_ptr = (int*)tmp;                    \
                                int_ptr[0] = 1;                         \
                                int_ptr[1] = dim1;                      \
                                array = (type*)( int_ptr +2);           \
                                for(i = 0; i < dim1;i++){               \
                                        array[i] = value;               \
                                }                                       \
                                return array;                           \
                        }else{                                          \
                                int_ptr = (int*) array;                 \
                                int_ptr = int_ptr - 2;                  \
                                old_dim = *(int_ptr+1);                 \
                                if(old_dim < dim1){                     \
                                        MREALLOC(int_ptr,(dim1  * sizeof *array + 2*sizeof(int))); \
                                        int_ptr[0] = 1;                 \
                                        int_ptr[1] = dim1;              \
                                        array = (type*)( int_ptr + 2);  \
                                        for(i = old_dim; i < dim1;i++){ \
                                                array[i] = value;       \
                                        }                               \
                                }                                       \
                        }                                               \
                        return array;                                   \
                ERROR:                                                  \
                        return NULL;                                    \
                }


#define ALLOC_2D_ARRAY(type)                                            \
        type **alloc_2D_array_size_ ##type (type **array, int dim1,int dim2, type fill_value) { \
        int i,j;                                                        \
        type** ptr_t = NULL;                                            \
        type* ptr_tt = NULL;                                            \
        int* int_ptr = NULL;                                            \
        int max1, max2;                                                 \
        int olddim1,olddim2;                                            \
        ASSERT((dim1 > 0), "Malloc 2D double failed: dim1:%d\n",dim1);  \
        ASSERT((dim2 > 0), "Malloc 2D double failed: dim2:%d\n",dim2);  \
        if(array == NULL){                                              \
                MMALLOC(ptr_t,(dim1  * sizeof *array));                 \
                MMALLOC(int_ptr,((dim1 * dim2)  * sizeof **array + 3 * sizeof(int))); \
                int_ptr[0] = 2;                                         \
                int_ptr[1] = dim1;                                      \
                int_ptr[2] = dim2;                                      \
                ptr_tt = (type*)(int_ptr + 3);                          \
                for(i = 0;i< dim1;i++){                                 \
                        ptr_t[i] = ptr_tt + i * dim2;                   \
                        for(j = 0; j < dim2;j++){                       \
                                ptr_t[i][j] = fill_value;               \
                        }                                               \
                }                                                       \
                array = ptr_t;                                          \
        }else{                                                          \
                ptr_t = array;                                          \
                int_ptr = (int*) array[0];                              \
                int_ptr = int_ptr - 3;                                  \
                ptr_tt  = (type* )int_ptr;                              \
                olddim1 = int_ptr[1];                                   \
                olddim2 = int_ptr[2];                                   \
                max1 = MACRO_MAX(dim1,olddim1);                         \
                max2 = MACRO_MAX(dim2,olddim2);                         \
                if(dim1 > olddim1){                                     \
                        MREALLOC(ptr_t,(dim1  * sizeof *array));        \
                        MREALLOC(ptr_tt,((dim1* max2)  * sizeof **array + 3 * sizeof(int))); \
                }else if(dim2 > olddim2){                               \
                        MREALLOC(ptr_tt,((max1 * dim2)  * sizeof **array + 3 * sizeof(int))); \
                }else{                                                  \
                        LOG_MSG("Did nothing");                         \
                        return array;                                   \
                }                                                       \
                int_ptr = (int*)ptr_tt;                                 \
                ptr_tt = (type*) (int_ptr + 3);                         \
                for(i = 0; i < max1;i++){                               \
                        ptr_t[i] = ptr_tt + i * max2;                   \
                        for(j = 0; j < max2;j++){                       \
                                ptr_t[i][j] = fill_value;               \
                        }                                               \
                }                                                       \
                int_ptr[0] = 2;                                         \
                int_ptr[1] = max1;                                      \
                int_ptr[2] = max2;                                      \
                array = ptr_t;                                          \
        }                                                               \
                return array;                                           \
        ERROR:                                                          \
                return NULL;                                            \
        }


ALLOC_1D_ARRAY(int)
        ALLOC_1D_ARRAY(float)
        ALLOC_1D_ARRAY(double)
        ALLOC_1D_ARRAY(int_fast32_t)

        ALLOC_2D_ARRAY(int)
        ALLOC_2D_ARRAY(float)
        ALLOC_2D_ARRAY(double)
        ALLOC_2D_ARRAY(int_fast32_t)

#endif
