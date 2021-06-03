#include <stdlib.h>
#include <stdio.h>

#define ALIGNEDVEC_ALLOC_IMPORT
#include "alignedvec_alloc.h"



#define ALIGNMENT 128

static inline int ceil64(int a){
        int tmp_ceil_64 = (a >> 6) << 6;
        return a > tmp_ceil_64? tmp_ceil_64 + 64 : tmp_ceil_64;
}

#define AVALLOC_1D_ARRAY(type)                                          \
        int avalloc_1D_size_##type (type **array, int dim1) {           \
                size_t size;                                            \
                type* tmp = NULL;                                       \
                int i;                                                  \
                dim1 = ceil64(dim1);                                    \
                size = dim1 * sizeof(**array);                          \
                if(size == 0){                                          \
                        goto ERROR;                                     \
                }                                                       \
                if (((tmp) = aligned_alloc(ALIGNMENT, size)) == NULL) { \
                        goto ERROR;                                     \
                }                                                       \
                /* Zero out */                                          \
                for(i = 0; i < dim1;i++){                               \
                        tmp[i] = 0;                                     \
                }                                                       \
                *array = tmp;                                           \
                return 0;                                               \
        ERROR:                                                          \
                avafree(*array);                                        \
                return 1;                                               \
        }

AVALLOC_1D_ARRAY(char)
        AVALLOC_1D_ARRAY(int8_t)
        AVALLOC_1D_ARRAY(uint8_t)
        AVALLOC_1D_ARRAY(int16_t)
        AVALLOC_1D_ARRAY(uint16_t)
        AVALLOC_1D_ARRAY(int32_t)
        AVALLOC_1D_ARRAY(uint32_t)
        AVALLOC_1D_ARRAY(int64_t)
        AVALLOC_1D_ARRAY(uint64_t)
        AVALLOC_1D_ARRAY(float)
        AVALLOC_1D_ARRAY(double)

#undef AVALLOC_1D_ARRAY


#define AVALLOC_2D_ARRAY(type)                                          \
        int avalloc_2D_size_##type (type ***array, int dim1,int dim2) { \
        int i;                                                          \
        int j;                                                          \
        size_t size;                                                    \
        type** ptr_t = NULL;                                            \
        type* ptr_tt = NULL;                                            \
        dim1 = ceil64(dim1);                                            \
        dim2 = ceil64(dim2);                                            \
        size = dim1 * sizeof(**array);                                  \
        if(size == 0){                                                  \
                goto ERROR;                                             \
        }                                                               \
        if (((ptr_t) = aligned_alloc(ALIGNMENT, size)) == NULL) {       \
                goto ERROR;                                             \
        }                                                               \
        size = (dim1 * dim2) * sizeof(***array);                        \
        if(size == 0){                                                  \
                goto ERROR;                                             \
        }                                                               \
        if ((( ptr_tt) = aligned_alloc(ALIGNMENT, size)) == NULL) {     \
                goto ERROR;                                             \
        }                                                               \
        j = dim1 * dim2;                                                \
        for(i = 0;i < dim1;i++){                                        \
                ptr_t[i] = ptr_tt + i * dim2;                           \
                for(j = 0; j < dim2; j++){                              \
                        ptr_t[i][j] =0;                                 \
                }                                                       \
        }                                                               \
        *array = ptr_t;                                                 \
        return 0;                                                       \
ERROR:                                                                  \
avafree(ptr_t);                                                         \
avafree(ptr_tt);                                                        \
return 1;                                                               \
}

AVALLOC_2D_ARRAY(char)
AVALLOC_2D_ARRAY(int8_t)
AVALLOC_2D_ARRAY(uint8_t)
AVALLOC_2D_ARRAY(int16_t)
AVALLOC_2D_ARRAY(uint16_t)
AVALLOC_2D_ARRAY(int32_t)
AVALLOC_2D_ARRAY(uint32_t)
AVALLOC_2D_ARRAY(int64_t)
AVALLOC_2D_ARRAY(uint64_t)
AVALLOC_2D_ARRAY(float)
AVALLOC_2D_ARRAY(double)

#undef AVALLOC_2D_ARRAY


#define AVFREE_VOID(type)                         \
        void  avallov_free_ ##type(type *a){\
                fprintf(stderr, "free was called on wrong type (%p)",(void*)a); \
        }

AVFREE_VOID(char)
AVFREE_VOID(int8_t)
AVFREE_VOID(uint8_t)
AVFREE_VOID(int16_t)
AVFREE_VOID(uint16_t)
AVFREE_VOID(int32_t)
AVFREE_VOID(uint32_t)
AVFREE_VOID(int64_t)
AVFREE_VOID(uint64_t)
AVFREE_VOID(float)
AVFREE_VOID(double)

#undef AVFREE_VOID


#define AVFREE_1D_ARRAY(type)                       \
        void avalloc_1D_free_ ##type(type **array){ \
                if(*array){                         \
                        free(*array);               \
                }                                   \
        }


AVFREE_1D_ARRAY(char)
AVFREE_1D_ARRAY(int8_t)
AVFREE_1D_ARRAY(uint8_t)
AVFREE_1D_ARRAY(int16_t)
AVFREE_1D_ARRAY(uint16_t)
AVFREE_1D_ARRAY(int32_t)
AVFREE_1D_ARRAY(uint32_t)
AVFREE_1D_ARRAY(int64_t)
AVFREE_1D_ARRAY(uint64_t)
AVFREE_1D_ARRAY(float)
AVFREE_1D_ARRAY(double)

#undef AVFREE_1D_ARRAY


#define AVFREE_2D_ARRAY(type)                         \
        void avalloc_2D_free_ ##type(type ***array){  \
                if(*array){                           \
                        if(*array[0]){                \
                                free(*array[0]);      \
                        }                             \
                        free(*array);                 \
                }                                     \
        }

AVFREE_2D_ARRAY(char)
AVFREE_2D_ARRAY(int8_t)
AVFREE_2D_ARRAY(uint8_t)
AVFREE_2D_ARRAY(int16_t)
AVFREE_2D_ARRAY(uint16_t)
AVFREE_2D_ARRAY(int32_t)
AVFREE_2D_ARRAY(uint32_t)
AVFREE_2D_ARRAY(int64_t)
AVFREE_2D_ARRAY(uint64_t)
AVFREE_2D_ARRAY(float)
AVFREE_2D_ARRAY(double)

#undef AVFREE_2D_ARRAY


#ifdef AVATEST
int main(void)
{
        double** g = NULL;
        int i;


        avalloc(&g,10,10);

        /* for(i = 0;i < 64;i++){ */
        /*         for(j = 0; j < 64;j++){ */
        /*                 fprintf(stdout,"%f ", g[i][j]); */
        /*         } */
        /*         fprintf(stdout,"\n"); */
        /* } */
        fprintf(stdout,"Aligned? %p %d (this is the ava alignment )\n", (void*) g, (int) ( (uint64_t)g %128));
        double*f = NULL;
        for(i = 0;i < 10;i++){
                f = malloc(sizeof(double) * 1000);
                fprintf(stdout,"Aligned? %p %d (default malloc)\n", (void*) f, (int) ( (uint64_t)f  %128));
                fprintf(stdout,"DONKEY");
                free(f);
        }

        fprintf(stdout,"DONKEY  b free\n");
        avafree(g);
        fprintf(stdout,"DONKEY a free\n");

        f = NULL;
        /* double* f = NULL; */
        fprintf(stdout,"f ptr: %p\n",(void*) f);
        avalloc(&f,10);

        fprintf(stdout,"f ptr: %p\n",(void*) f);
        for(i = 0;i < 10;i++){
                fprintf(stdout,"%f ", f[i]);
        }


        avafree(f);
        return 0;
}

#endif
