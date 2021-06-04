#ifndef ALIGNEDVEC_ALLOC_H
#define ALIGNEDVEC_ALLOC_H


#include <stdint.h>

#ifdef ALIGNEDVEC_ALLOC_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif

#define AVALLOC_FUNC_DEF(type)                                          \
        EXTERN int avalloc_1D_size_ ##type(type **array, int dim1);     \
        EXTERN int avalloc_2D_size_ ##type(type ***array, int dim1,int dim2); \
        EXTERN void avalloc_free_void_ ##type(type *a);                 \
        EXTERN void avalloc_1D_free_ ##type(type **array);              \
        EXTERN void avalloc_2D_free_ ##type(type ***array);

AVALLOC_FUNC_DEF(char)
AVALLOC_FUNC_DEF(int8_t)
AVALLOC_FUNC_DEF(uint8_t)
AVALLOC_FUNC_DEF(int16_t)
AVALLOC_FUNC_DEF(uint16_t)
AVALLOC_FUNC_DEF(int32_t)
AVALLOC_FUNC_DEF(uint32_t)
AVALLOC_FUNC_DEF(int64_t)
AVALLOC_FUNC_DEF(uint64_t)
AVALLOC_FUNC_DEF(float)
AVALLOC_FUNC_DEF(double)

#undef AVALLOC_FUNC_DEF

EXTERN int avalloc_unknown_type_error (void* p, ...);
EXTERN int avalloc_too_few_arg_error (void* p);

#define AVA_p1(X) _Generic((X),                               \
                       default: avalloc_too_few_arg_error \
                )(X)
#define AVA_p2(X,Y) _Generic((X),                                     \
                         char**        : avalloc_1D_size_char,       \
                         int8_t**      : avalloc_1D_size_int8_t,     \
                         uint8_t**     : avalloc_1D_size_uint8_t,    \
                         int16_t**     : avalloc_1D_size_int16_t,    \
                         uint16_t**    : avalloc_1D_size_uint16_t,   \
                         int32_t**     : avalloc_1D_size_int32_t,    \
                         uint32_t**    : avalloc_1D_size_uint32_t,   \
                         int64_t**     : avalloc_1D_size_int64_t,    \
                         uint64_t**    : avalloc_1D_size_uint64_t,   \
                         float**       : avalloc_1D_size_float,      \
                         double**      : avalloc_1D_size_double,     \
                         default       : avalloc_unknown_type_error  \
                )(X,Y)

#define AVA_p3(X,Y,Z) _Generic((X),                                     \
                           char***     : avalloc_2D_size_char,      \
                           int8_t***   : avalloc_2D_size_int8_t,    \
                           uint8_t***  : avalloc_2D_size_uint8_t,   \
                           int16_t***  : avalloc_2D_size_int16_t,   \
                           uint16_t*** : avalloc_2D_size_uint16_t,  \
                           int32_t***  : avalloc_2D_size_int32_t,   \
                           uint32_t*** : avalloc_2D_size_uint32_t,  \
                           int64_t***  : avalloc_2D_size_int64_t,   \
                           uint64_t*** : avalloc_2D_size_uint64_t,  \
                           float***    : avalloc_2D_size_float,     \
                           double***   : avalloc_2D_size_double,    \
                           default     : avalloc_unknown_type_error \
                )(X,Y,Z)

#define _ARG3(_0, _1, _2, _3, ...) _3
#define NARG3(...) _ARG3(__VA_ARGS__,3, 2, 1, 0)

#define _AVALLOC_ARGS_1( a) AVA_p1(a)
#define _AVALLOC_ARGS_2( a, b) AVA_p2(a,b)
#define _AVALLOC_ARGS_3( a, b, c ) AVA_p3(a,b,c)

#define __AVALLOC_ARGS( N, ...) _AVALLOC_ARGS_ ## N ( __VA_ARGS__)
#define _AVALLOC_ARGS( N, ...) __AVALLOC_ARGS( N, __VA_ARGS__)
#define AVALLOC_ARGS( ...) _AVALLOC_ARGS( NARG3(__VA_ARGS__), __VA_ARGS__)
#define avalloc(...) AVALLOC_ARGS( __VA_ARGS__)


#define avafree(X) _Generic((&X),                                   \
                            char*         : avalloc_free_void_char,      \
                            int8_t*       : avalloc_free_void_int8_t,    \
                            uint8_t*      : avalloc_free_void_uint8_t,   \
                            int16_t*      : avalloc_free_void_int16_t,   \
                            uint16_t*     : avalloc_free_void_uint16_t,  \
                            int32_t*      : avalloc_free_void_int32_t,   \
                            uint32_t*     : avalloc_free_void_uint32_t,  \
                            int64_t*      : avalloc_free_void_int64_t,   \
                            uint64_t*     : avalloc_free_void_uint64_t,  \
                            float*        : avalloc_free_void_float,     \
                            double*       : avalloc_free_void_double,    \
                            char**        : avalloc_1D_free_char,     \
                            int8_t**      : avalloc_1D_free_int8_t,   \
                            uint8_t**     : avalloc_1D_free_uint8_t,  \
                            int16_t**     : avalloc_1D_free_int16_t,  \
                            uint16_t**    : avalloc_1D_free_uint16_t, \
                            int32_t**     : avalloc_1D_free_int32_t,  \
                            uint32_t**    : avalloc_1D_free_uint32_t, \
                            int64_t**     : avalloc_1D_free_int64_t,  \
                            uint64_t**    : avalloc_1D_free_uint64_t, \
                            float**       : avalloc_1D_free_float,    \
                            double**      : avalloc_1D_free_double,   \
                            char***       : avalloc_2D_free_char,     \
                            int8_t***     : avalloc_2D_free_int8_t,   \
                            uint8_t***    : avalloc_2D_free_uint8_t,  \
                            int16_t***    : avalloc_2D_free_int16_t,  \
                            uint16_t***   : avalloc_2D_free_uint16_t, \
                            int32_t***    : avalloc_2D_free_int32_t,  \
                            uint32_t***   : avalloc_2D_free_uint32_t, \
                            int64_t***    : avalloc_2D_free_int64_t,  \
                            uint64_t***   : avalloc_2D_free_uint64_t, \
                            float***      : avalloc_2D_free_float,    \
                            double***     : avalloc_2D_free_double    \
                )(&X)

/* functions  */

#undef ALIGNEDVEC_ALLOC_IMPORT
#undef EXTERN


#endif
