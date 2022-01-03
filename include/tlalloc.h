#ifndef TLALLOC_H
#define TLALLOC_H
#include <string.h>
#include <stdint.h>

#ifdef TLALLOC_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif

#define MFREE(p) do {                                           \
                if(p){                                          \
                        free(p);                                \
                        p = NULL;                               \
                }else{                                          \
                        WARNING_MSG("free on a null pointer");  \
                }                                               \
        } while (0)

#define MMALLOC(p,size) do {                                            \
                if (p != NULL){                                         \
                        ERROR_MSG( "malloc on a nun-null pointer");     \
                        goto ERROR;                                     \
                }                                                       \
                if(size == 0){                                          \
                        ERROR_MSG("malloc of size %d failed", size);	\
                        goto ERROR;                                     \
                }                                                       \
                if (((p) = malloc(size)) == NULL) {                     \
                        ERROR_MSG("malloc of size %d failed", size);	\
                        goto ERROR;                                     \
                }                                                       \
                memset(p, 0, size);                                     \
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

#define FUNC_DEF(type)                                                  \
        EXTERN int alloc_1D_array_size_ ##type (type **array, int dim1); \
        EXTERN int alloc_2D_array_size_ ##type (type ***array, int dim1,int dim2); \
        EXTERN int alloc_aligned_1D_array_size_ ##type (type **array, int dim1); \
        EXTERN int alloc_aligned_2D_array_size_ ##type (type ***array, int dim1,int dim2); \
        EXTERN void gfree_void_ ##type(type *a);                        \
        EXTERN void free_1d_array_ ##type(type **array);                \
        EXTERN void free_2d_array_ ##type(type ***array);

FUNC_DEF(char)
FUNC_DEF(int8_t)
FUNC_DEF(uint8_t)
FUNC_DEF(int16_t)
FUNC_DEF(uint16_t)
FUNC_DEF(int32_t)
FUNC_DEF(uint32_t)
FUNC_DEF(int64_t)
FUNC_DEF(uint64_t)
FUNC_DEF(float)
FUNC_DEF(double)

#undef FUNC_DEF

EXTERN int galloc_unknown_type_error (void* p, ...);
EXTERN int galloc_too_few_arg_error (void* p);

#define p1_alloc(X) _Generic((X),                                 \
                       default: galloc_too_few_arg_error    \
                )(X)

#define p2_alloc(X,Y) _Generic((X),                                       \
                         char**: alloc_1D_array_size_char,          \
                         int8_t**: alloc_1D_array_size_int8_t,      \
                         uint8_t**: alloc_1D_array_size_uint8_t,    \
                         int16_t**: alloc_1D_array_size_int16_t,    \
                         uint16_t**: alloc_1D_array_size_uint16_t,  \
                         int32_t**: alloc_1D_array_size_int32_t,    \
                         uint32_t**: alloc_1D_array_size_uint32_t,  \
                         int64_t**: alloc_1D_array_size_int64_t,    \
                         uint64_t**: alloc_1D_array_size_uint64_t,  \
                         float**: alloc_1D_array_size_float,        \
                         double**: alloc_1D_array_size_double,      \
                         default: galloc_unknown_type_error         \
                )(X,Y)

#define p3_alloc(X,Y,Z) _Generic((X),                                         \
                           char***: alloc_2D_array_size_char,           \
                           int8_t***: alloc_2D_array_size_int8_t,       \
                           uint8_t***: alloc_2D_array_size_uint8_t,     \
                           int16_t***: alloc_2D_array_size_int16_t,     \
                           uint16_t***: alloc_2D_array_size_uint16_t,   \
                           int32_t***: alloc_2D_array_size_int32_t,     \
                           uint32_t***: alloc_2D_array_size_uint32_t,   \
                           int64_t***: alloc_2D_array_size_int64_t,     \
                           uint64_t***: alloc_2D_array_size_uint64_t,   \
                           float***: alloc_2D_array_size_float,         \
                           double***: alloc_2D_array_size_double,       \
                           default: galloc_unknown_type_error           \
                )(X,Y,Z)

#define p1_alloc_aligned(X) _Generic((X),                                 \
                       default: galloc_too_few_arg_error    \
                )(X)

#define p2_alloc_aligned(X,Y) _Generic((X),                                       \
                         char**: alloc_aligned_1D_array_size_char,          \
                         int8_t**: alloc_aligned_1D_array_size_int8_t,      \
                         uint8_t**: alloc_aligned_1D_array_size_uint8_t,    \
                         int16_t**: alloc_aligned_1D_array_size_int16_t,    \
                         uint16_t**: alloc_aligned_1D_array_size_uint16_t,  \
                         int32_t**: alloc_aligned_1D_array_size_int32_t,    \
                         uint32_t**: alloc_aligned_1D_array_size_uint32_t,  \
                         int64_t**: alloc_aligned_1D_array_size_int64_t,    \
                         uint64_t**: alloc_aligned_1D_array_size_uint64_t,  \
                         float**: alloc_aligned_1D_array_size_float,        \
                         double**: alloc_aligned_1D_array_size_double,      \
                         default: galloc_unknown_type_error         \
                )(X,Y)

#define p3_alloc_aligned(X,Y,Z) _Generic((X),                                         \
                           char***: alloc_aligned_2D_array_size_char,           \
                           int8_t***: alloc_aligned_2D_array_size_int8_t,       \
                           uint8_t***: alloc_aligned_2D_array_size_uint8_t,     \
                           int16_t***: alloc_aligned_2D_array_size_int16_t,     \
                           uint16_t***: alloc_aligned_2D_array_size_uint16_t,   \
                           int32_t***: alloc_aligned_2D_array_size_int32_t,     \
                           uint32_t***: alloc_aligned_2D_array_size_uint32_t,   \
                           int64_t***: alloc_aligned_2D_array_size_int64_t,     \
                           uint64_t***: alloc_aligned_2D_array_size_uint64_t,   \
                           float***: alloc_aligned_2D_array_size_float,         \
                           double***: alloc_aligned_2D_array_size_double,       \
                           default: galloc_unknown_type_error           \
                )(X,Y,Z)


#define _ARG3(_0, _1, _2, _3, ...) _3
#define NARG3(...) _ARG3(__VA_ARGS__,3, 2, 1, 0)

#define _GALLOC_ARGS_1( a) p1_alloc(a)
#define _GALLOC_ARGS_2( a, b) p2_alloc(a,b)
#define _GALLOC_ARGS_3( a, b, c ) p3_alloc(a,b,c)

#define __GALLOC_ARGS( N, ...) _GALLOC_ARGS_ ## N ( __VA_ARGS__)
#define _GALLOC_ARGS( N, ...) __GALLOC_ARGS( N, __VA_ARGS__)
#define GALLOC_ARGS( ...) _GALLOC_ARGS( NARG3(__VA_ARGS__), __VA_ARGS__)
#define galloc(...) GALLOC_ARGS( __VA_ARGS__)


#define _GALLOC_ALIGNED_ARGS_1( a) p1_alloc_aligned(a)
#define _GALLOC_ALIGNED_ARGS_2( a, b) p2_alloc_aligned(a,b)
#define _GALLOC_ALIGNED_ARGS_3( a, b, c ) p3_alloc_aligned(a,b,c)

#define __GALLOC_ALIGNED_ARGS( N, ...) _GALLOC_ALIGNED_ARGS_ ## N ( __VA_ARGS__)
#define _GALLOC_ALIGNED_ARGS( N, ...) __GALLOC_ALIGNED_ARGS( N, __VA_ARGS__)
#define GALLOC_ALIGNED_ARGS( ...) _GALLOC_ALIGNED_ARGS( NARG3(__VA_ARGS__), __VA_ARGS__)
#define galloc_aligned(...) GALLOC_ALIGNED_ARGS( __VA_ARGS__)




#define gfree(X) _Generic((&X),                                 \
                          char*: gfree_void_char,               \
                          int8_t*: gfree_void_int8_t,           \
                          uint8_t*: gfree_void_uint8_t,         \
                          int16_t*: gfree_void_int16_t,         \
                          uint16_t*: gfree_void_uint16_t,       \
                          int32_t*: gfree_void_int32_t,         \
                          uint32_t*: gfree_void_uint32_t,       \
                          int64_t*: gfree_void_int64_t,         \
                          uint64_t*: gfree_void_uint64_t,       \
                          float*: gfree_void_float,             \
                          double*: gfree_void_double,           \
                          char**: free_1d_array_char,           \
                          int8_t**: free_1d_array_int8_t,       \
                          uint8_t**: free_1d_array_uint8_t,     \
                          int16_t**: free_1d_array_int16_t,     \
                          uint16_t**: free_1d_array_uint16_t,   \
                          int32_t**: free_1d_array_int32_t,     \
                          uint32_t**: free_1d_array_uint32_t,   \
                          int64_t**: free_1d_array_int64_t,     \
                          uint64_t**: free_1d_array_uint64_t,   \
                          float**: free_1d_array_float,         \
                          double**: free_1d_array_double,       \
                          char***: free_2d_array_char,          \
                          int8_t***: free_2d_array_int8_t,      \
                          uint8_t***: free_2d_array_uint8_t,    \
                          int16_t***: free_2d_array_int16_t,    \
                          uint16_t***: free_2d_array_uint16_t,  \
                          int32_t***: free_2d_array_int32_t,    \
                          uint32_t***: free_2d_array_uint32_t,  \
                          int64_t***: free_2d_array_int64_t,    \
                          uint64_t***: free_2d_array_uint64_t,  \
                          float***: free_2d_array_float,        \
                          double***: free_2d_array_double       \
                )(&X)

#undef TLALLOC_IMPORT
#undef EXTERN

#endif
