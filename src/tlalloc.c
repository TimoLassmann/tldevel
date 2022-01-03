#include "tldevel.h"

#include <string.h>

#include <stdlib.h>

#define TLALLOC_IMPORT
#include "tlalloc.h"


typedef struct {
        uint32_t aligned : 1;
        uint32_t dim1 : 31;
        uint32_t dim2;
} mem_i;

#define ALIGNMENT 64

static int get_aligned(void* ptr,int* d);

static inline int ceil64(int a){
        int tmp_ceil_64 = (a >> 6) << 6;
        return a > tmp_ceil_64? tmp_ceil_64 + 64 : tmp_ceil_64;
}

static inline void* tl_aligned_alloc(size_t size){
        void* p = NULL;
        size = ceil64(size);

        if(size == 0){
                ERROR_MSG("aligned malloc of size %d failed", size);
                goto ERROR;
        }
        if (((p) = aligned_alloc(ALIGNMENT,size)) == NULL) {
                ERROR_MSG("aligned malloc of size %d failed", size);
                goto ERROR;
        }

        memset(p, 0, size);
        return p;
 ERROR:
        return NULL;
}

int galloc_unknown_type_error (void* p, ...)
{
        error(AT, "galloc was called with pointer of unknown type: %p", p);
        return FAIL;
}

int galloc_too_few_arg_error (void* p)
{
        error(AT,"galloc was called with only one argument: %p",p);
        return FAIL;
}

int get_aligned(void* ptr,int* d)
{
        if(ptr){
                *d = ((mem_i*)((void*) ((uint8_t*)ptr - sizeof(mem_i))))->aligned;
                return OK;
        }
        return FAIL;
}

int get_dim1(void* ptr,int* d)
{
        if(ptr){
                *d = ((mem_i*)((void*) ((uint8_t*)ptr - sizeof(mem_i))))->dim1;
                return OK;
        }
        return FAIL;
}

int get_dim2(void* ptr,int* d)
{
        if(ptr){
                *d = ((mem_i*)((void*) ((char*)ptr - sizeof(mem_i))))->dim2;
                return OK;
        }
        return FAIL;
}


#define ALLOC_1D_ARRAY(type)                                            \
        int alloc_1D_array_size_ ##type (type **array, int dim1) {      \
                mem_i* h = NULL;                                        \
                void* tmp = NULL;                                       \
                ASSERT(dim1 >= 1,"DIM1 is too small: %d",dim1);         \
                if(*array == NULL){                                     \
                        MMALLOC(tmp,(dim1  * sizeof **array + sizeof(mem_i))); \
                }else{                                                  \
                        MMALLOC(tmp,(dim1  * sizeof **array +  sizeof(mem_i))); \
                        int odim1;                                      \
                        RUN(get_dim1(*array,&odim1));                   \
                        LOG_MSG("DIM: %d", odim1);                      \
                        memcpy((uint8_t*)tmp+  sizeof(mem_i),*array, MACRO_MIN(odim1,dim1) * sizeof **array ); \
                        gfree(*array);                                  \
                }                                                       \
                h = (mem_i*)(tmp);                                      \
                h->aligned = 0u;                                        \
                h->dim1  = dim1;                                        \
                h->dim2  = 0u;                                          \
                *array= (type*)  ((uint8_t*)tmp + sizeof(mem_i));       \
                return OK;                                              \
        ERROR:                                                          \
                gfree(*array);                                          \
                return FAIL;                                            \
        }                                                               \
                                                                        \
        int alloc_aligned_1D_array_size_ ##type (type **array, int dim1) { \
                mem_i* h = NULL;                                        \
                void* tmp = NULL;                                       \
                ASSERT(dim1 >= 1,"DIM1 is too small: %d",dim1);         \
                dim1 = ceil64(dim1);                                    \
                if(*array == NULL){                                     \
                        tmp = tl_aligned_alloc(dim1  * sizeof **array + ALIGNMENT); \
                }else{                                                  \
                        tmp = tl_aligned_alloc(dim1  * sizeof **array + ALIGNMENT); \
                        int odim1;                                      \
                        RUN(get_dim1(*array,&odim1));                   \
                        LOG_MSG("DIM: %d", odim1);                      \
                        memcpy((uint8_t*)tmp+ ALIGNMENT,*array, MACRO_MIN(odim1,dim1) * sizeof **array ); \
                        gfree(*array);                                  \
                }                                                       \
                h = (mem_i*)((uint8_t*)tmp + ALIGNMENT - sizeof(mem_i)); \
                h->aligned = 1u;                                        \
                h->dim1  = dim1;                                        \
                h->dim2  = 0u;                                          \
                *array= (type*)  ((uint8_t*)tmp + ALIGNMENT);           \
                return OK;                                              \
        ERROR:                                                          \
                gfree(*array);                                          \
                return FAIL;                                            \
        }


ALLOC_1D_ARRAY(char)
ALLOC_1D_ARRAY(int8_t)
ALLOC_1D_ARRAY(uint8_t)
ALLOC_1D_ARRAY(int16_t)
ALLOC_1D_ARRAY(uint16_t)
ALLOC_1D_ARRAY(int32_t)
ALLOC_1D_ARRAY(uint32_t)
ALLOC_1D_ARRAY(int64_t)
ALLOC_1D_ARRAY(uint64_t)
ALLOC_1D_ARRAY(float)
ALLOC_1D_ARRAY(double)

#undef ALLOC_1D_ARRAY

#define ALLOC_2D_ARRAY(type)                                            \
        int alloc_2D_array_size_ ##type (type ***array, int dim1,int dim2) { \
                int i,j;                                                \
                mem_i* h = NULL;                                        \
                type** ptr_t = NULL;                                    \
                type* ptr_tt = NULL;                                    \
                void* tmp = NULL;                                       \
                int o1, o2;                                             \
                ASSERT(dim1 >= 1,"DIM1 is too small: %d",dim1);         \
                ASSERT(dim2 >= 1,"DIM1 is too small: %d",dim2);         \
                if(*array == NULL){                                     \
                        MMALLOC(tmp,(dim1  * sizeof **array+ sizeof(mem_i))); \
                        MMALLOC(ptr_tt,((dim1 * dim2)  * sizeof ***array)); \
                        h = (mem_i*)tmp;                                \
                        h->aligned = 0u;                                \
                        h->dim1  = dim1;                                \
                        h->dim2  = dim2;                                \
                        ptr_t =(type**) ((uint8_t*)tmp + sizeof(mem_i)); \
                        for(i = 0;i< dim1;i++){                         \
                                ptr_t[i] = ptr_tt + i * dim2;           \
                        }                                               \
                        *array = ptr_t;                                 \
                }else{                                                  \
                        MMALLOC(tmp,(dim1  * sizeof **array+ sizeof(mem_i))); \
                        MMALLOC(ptr_tt,((dim1 * dim2)  * sizeof ***array)); \
                        h = (mem_i*)tmp;                                \
                        h->aligned = 0u;                                \
                        h->dim1  = dim1;                                \
                        h->dim2  = dim2;                                \
                        ptr_t =(type**) ((uint8_t*)tmp + sizeof(mem_i)); \
                        for(i = 0;i< dim1;i++){                         \
                                ptr_t[i] = ptr_tt + i * dim2;           \
                        }                                               \
                        get_dim1(*array,&o1);                           \
                        get_dim2(*array,&o2);                           \
                        type** a = *array;                              \
                        for(i = 0; i < MACRO_MIN(dim1,o1); i++){        \
                                for (j = 0; j < MACRO_MIN(dim2,o2);j++){ \
                                        ptr_t[i][j] = a[i][j];          \
                                }                                       \
                        }                                               \
                        gfree(*array);                                  \
                        *array = ptr_t;                                 \
                }                                                       \
                return OK;                                              \
        ERROR:                                                          \
                gfree(*array);                                          \
                return FAIL;                                            \
        }                                                               \
                                                                        \
        int alloc_aligned_2D_array_size_ ##type (type ***array, int dim1,int dim2) { \
                int i,j;                                                \
                mem_i* h = NULL;                                        \
                type** ptr_t = NULL;                                    \
                type* ptr_tt = NULL;                                    \
                void* tmp = NULL;                                       \
                int o1, o2;                                             \
                ASSERT(dim1 >= 1,"DIM1 is too small: %d",dim1);         \
                ASSERT(dim2 >= 1,"DIM1 is too small: %d",dim2);         \
                dim1 = ceil64(dim1);                                    \
                dim2 = ceil64(dim2);                                    \
                tmp = tl_aligned_alloc(dim1  * sizeof **array+ ALIGNMENT); \
                ptr_tt = tl_aligned_alloc((dim1 * dim2)  * sizeof ***array); \
                h = (mem_i*)((uint8_t*)tmp + ALIGNMENT - sizeof(mem_i)); \
                h->aligned = 1u;                                        \
                h->dim1  = dim1;                                        \
                h->dim2  = dim2;                                        \
                ptr_t =(type**) ((uint8_t*)tmp + ALIGNMENT);            \
                for(i = 0;i < dim1;i++){                                \
                        ptr_t[i] = ptr_tt + i * dim2;                   \
                }                                                       \
                if(*array != NULL){                                     \
                        get_dim1(*array,&o1);                           \
                        get_dim2(*array,&o2);                           \
                        type** a = *array;                              \
                        for(i = 0; i < MACRO_MIN(dim1,o1); i++){        \
                                for (j = 0; j < MACRO_MIN(dim2,o2);j++){ \
                                        ptr_t[i][j] = a[i][j];          \
                                }                                       \
                        }                                               \
                        gfree(*array);                                  \
                }                                                       \
                *array = ptr_t;                                         \
                return OK;                                              \
        ERROR:                                                          \
                gfree(*array);                                          \
                return FAIL;                                            \
        }

     ALLOC_2D_ARRAY(char)
     ALLOC_2D_ARRAY(int8_t)
ALLOC_2D_ARRAY(uint8_t)
ALLOC_2D_ARRAY(int16_t)
ALLOC_2D_ARRAY(uint16_t)
ALLOC_2D_ARRAY(int32_t)
ALLOC_2D_ARRAY(uint32_t)
ALLOC_2D_ARRAY(int64_t)
ALLOC_2D_ARRAY(uint64_t)
ALLOC_2D_ARRAY(float)
ALLOC_2D_ARRAY(double)


#define FREE_VOID(type)                                                 \
     void gfree_void_ ##type(type *a){                                  \
             error(AT, "free was called on wrong type (%p)",(void*)a);  \
     }

FREE_VOID(char)
FREE_VOID(int8_t)
FREE_VOID(uint8_t)
FREE_VOID(int16_t)
FREE_VOID(uint16_t)
FREE_VOID(int32_t)
FREE_VOID(uint32_t)
FREE_VOID(int64_t)
FREE_VOID(uint64_t)
FREE_VOID(float)
FREE_VOID(double)

#undef FREE_VOID


#define FREE_1D_ARRAY(type)                                             \
     void free_1d_array_ ##type(type **array){                          \
             if(*array){                                                \
                     int a;                                             \
                     void* ptr = NULL;                                  \
                     get_aligned(*array, &a);                           \
                     if(a){                                             \
                             ptr = (void*)((uint8_t*)*array- ALIGNMENT); \
                     }else{                                             \
                             ptr = (void*)((uint8_t*)*array- sizeof(mem_i)); \
                     }                                                  \
                     MFREE(ptr);                                        \
                     *array = NULL;                                     \
             }                                                          \
     }


FREE_1D_ARRAY(char)
FREE_1D_ARRAY(int8_t)
FREE_1D_ARRAY(uint8_t)
FREE_1D_ARRAY(int16_t)
FREE_1D_ARRAY(uint16_t)
FREE_1D_ARRAY(int32_t)
FREE_1D_ARRAY(uint32_t)
FREE_1D_ARRAY(int64_t)
FREE_1D_ARRAY(uint64_t)
FREE_1D_ARRAY(float)
FREE_1D_ARRAY(double)

#undef FREE_1D_ARRAY


#define FREE_2D_ARRAY(type)                                             \
        void free_2d_array_ ##type(type ***array){                      \
                type** x = *array;                                      \
                if(x){                                                  \
                        if(x[0]){                                       \
                                MFREE(x[0]);                            \
                        }                                               \
                        int a;                                          \
                        get_aligned(x, &a);                             \
                        void* ptr = NULL;                               \
                        if(a){                                          \
                                ptr = (void*)((uint8_t*)*array- ALIGNMENT); \
                        }else{                                          \
                                ptr = (void*)((uint8_t*)*array- sizeof(mem_i)); \
                        }                                               \
                        MFREE(ptr);                                     \
                        *array = NULL;                                  \
                }                                                       \
        }

FREE_2D_ARRAY(char)
FREE_2D_ARRAY(int8_t)
FREE_2D_ARRAY(uint8_t)
FREE_2D_ARRAY(int16_t)
FREE_2D_ARRAY(uint16_t)
FREE_2D_ARRAY(int32_t)
FREE_2D_ARRAY(uint32_t)
FREE_2D_ARRAY(int64_t)
FREE_2D_ARRAY(uint64_t)
FREE_2D_ARRAY(float)
FREE_2D_ARRAY(double)

#undef FREE_2D_ARRAY
