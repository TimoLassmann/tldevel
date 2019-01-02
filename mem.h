 #ifndef MEM_HEADER_INC

#define MEM_HEADER_INC


#include "tl_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



typedef struct {
        int dim1;
        int dim2;
} mem_i;

#define MFREE(p) do {                                           \
                if(p){                                          \
                        free(p);                                \
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
                if(size == 0){                                       \
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
                        ERROR_MSG("realloc for size %d failed", size); \
                        goto ERROR;                                     \
                }} while (0)


#define DIM1(X) ((mem_i*)((void*)X - sizeof(mem_i)))->dim1
#define DIM2(X) ((mem_i*)((void*)X - sizeof(mem_i)))->dim2


#define galloc(...) SELECT(__VA_ARGS__)(__VA_ARGS__)

#define SELECT(...) CONCAT(SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)
#define CONCAT(X, Y) CONCAT_(X, Y)
#define CONCAT_(X, Y) X ## Y

#define SELECT_0()
#define SELECT_1(_1) _Generic ((_1),                \
                               default: galloc_void \
                )
#define SELECT_2(_1, _2) _Generic((_1),                                 \
                                  int*: alloc_1D_array_size_int,        \
                                  float*:  alloc_1D_array_size_float,   \
                                  double*:alloc_1D_array_size_double,   \
                                  int_fast32_t*: alloc_1D_array_size_int_fast32_t \
                )




#define SELECT_3(_1, _2, _3) _Generic((_1),                             \
                                      int**: _Generic((_2),              \
                                                     int: alloc_2D_array_size_int \
                                              ),                        \
                                      float**: _Generic((_2),            \
                                                       int: alloc_2D_array_size_float \
                                              ),                        \
                                      double**: _Generic((_2),           \
                                                        int: alloc_2D_array_size_double \
                                              ),                        \
                                      int_fast32_t**:  _Generic((_2),    \
                                                               int: alloc_2D_array_size_int_fast32_t \
                                              )                         \
                )


/* #define SELECT_4(_1, _2, _3, _4) _Generic((_1),                         \ */
/*                                           int**: _Generic((_2),         \ */
/*                                                           int: alloc_2D_array_size_int \ */
/*                                                   ),                    \ */
/*                                           float**: _Generic((_2),       \ */
/*                                                             int: alloc_2D_array_size_float \ */
/*                                                   ),                    \ */
/*                                           double**: _Generic((_2),      \ */
/*                                                              int: alloc_2D_array_size_double \ */
/*                                                   ),                    \ */
/*                                           int_fast32_t**:  _Generic((_2), \ */
/*                                                                     int: alloc_2D_array_size_int_fast32_t \ */
/*                                                   )                     \ */
/*                 ) */




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

void gfree_void(type a)
{
        fprintf(stdout,"void placeholder called");
}



#define gfree(X) _Generic((X),                                        \
                          int*: free_1d_array_int,                    \
                          float*: free_1d_array_float,                \
                          double*: free_1d_array_double,              \
                          int_fast32_t*: free_1d_array_int_fast32_t,  \
                          int**: free_2d_array_int,                   \
                          float**: free_2d_array_float,               \
                          double**: free_2d_array_double,             \
                          int_fast32_t**: free_2d_array_int_fast32_t, \
                          int: gfree_void,                            \
                          double: gfree_void,                         \
                          char*: gfree_void                           \
                )(X)

#define FREE_1D_ARRAY(type)                           \
        void free_1d_array_ ##type(type *array){      \
                MFREE((void*)array - sizeof(mem_i));  \
        }


#define FREE_2D_ARRAY(type)                         \
        void free_2d_array_ ##type(type **array){   \
                 MFREE(array[0]);                     \
                 MFREE((void*)array- sizeof(mem_i));  \
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
                type *alloc_1D_array_size_ ##type (type *array, int dim1) { \
                        mem_i* h = NULL;                                \
                        void* tmp = NULL;                               \
                        if(array == NULL){                              \
                                MMALLOC(tmp,(dim1  * sizeof *array + sizeof(mem_i))); \
                                LOG_MSG("Allocated:%d",(dim1  * sizeof *array + sizeof(mem_i))); \
                        }else{                                          \
                                LOG_MSG("%p BERFORE", array);           \
                                tmp = array;                            \
                                tmp = tmp - sizeof(mem_i);              \
                                LOG_MSG("%p after", array);             \
                                h = (mem_i*)(tmp);                      \
                                if(h->dim1 < dim1){                     \
                                        MREALLOC(tmp,(dim1  * sizeof *array + sizeof(mem_i))); \
                                        LOG_MSG("%p after realloc", array); \
                                }else{                                  \
                                        return tmp + sizeof(mem_i);     \
                                }                                       \
                        }                                               \
                        h->dim1  = dim1;                                \
                        h->dim2  = 0;                                   \
                        return (type*)  (tmp + sizeof(mem_i));          \
ERROR:                                                                  \
return NULL;                                                            \
                }


#define ALLOC_2D_ARRAY(type)                                            \
        type **alloc_2D_array_size_ ##type (type **array, int dim1,int dim2) { \
                int i;                                                \
                mem_i* h = NULL;                                        \
                type** ptr_t = NULL;                                    \
                type* ptr_tt = NULL;                                    \
                void* tmp = NULL;                                       \
                int max1, max2;                                         \
                ASSERT((dim1 > 0), "Malloc 2D double failed: dim1:%d\n",dim1); \
                ASSERT((dim2 > 0), "Malloc 2D double failed: dim2:%d\n",dim2); \
                if(array == NULL){                                      \
                        MMALLOC(tmp,(dim1  * sizeof *array+ sizeof(mem_i))); \
                        MMALLOC(ptr_tt,((dim1 * dim2)  * sizeof **array)); \
                        h = (mem_i*)tmp;                                \
                        h->dim1  = dim1;                                \
                        h->dim2  = dim2;                                \
                        LOG_MSG("%p",tmp);                              \
                        ptr_t =(type**) (tmp + sizeof(mem_i));          \
                        for(i = 0;i< dim1;i++){                         \
                                ptr_t[i] = ptr_tt + i * dim2;           \
                        }                                               \
                        array = ptr_t;                                  \
                }else{                                                  \
                        ptr_tt = array[0];                              \
                        tmp = (void*)(array) -sizeof(mem_i) ;           \
                        LOG_MSG("%p - b real",tmp);                     \
                        h = (mem_i*)tmp;                                \
                        max1 = MACRO_MAX(dim1,h->dim1);                 \
                        max2 = MACRO_MAX(dim2,h->dim2);                 \
                        if(dim1 > h->dim1){                             \
                                MREALLOC(tmp,(dim1  * sizeof *array+ sizeof(mem_i))); \
                                MREALLOC(ptr_tt,((dim1* max2)  * sizeof **array )); \
                        }else if(dim2 > h->dim2){                       \
                                MREALLOC(ptr_tt,((max1 * dim2) * sizeof **array )); \
                        }else{                                          \
                                return array;                           \
                        }                                               \
                        LOG_MSG("%p - realloced",tmp);                              \
                        h = (mem_i*)tmp;                                \
                        h->dim1 = max1;                                 \
                        h->dim2 = max2;                                 \
                        ptr_t = (type**) (tmp + sizeof(mem_i));         \
                        for(i = 0; i < max1;i++){                       \
                                ptr_t[i] = ptr_tt + i * max2;           \
                        }                                               \
                        array = ptr_t;                                  \
                }                                                       \
                return array;                                           \
ERROR:                                                                  \
return NULL;                                                            \
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
