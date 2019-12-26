
#include <stdio.h>
#include <stdarg.h>

#define p1(X) _Generic((X),                         \
                         double: p1_double,           \
                         int: p1_int,                 \
                         default: p1_int              \
                )(X)

#define p2(X,Y) _Generic((X),                          \
                         double: p2_double,           \
                         int: p2_int,                 \
                         default: p2_int              \
                )(X,Y)

#define p3(X,Y,Z) _Generic((X),                        \
                         double: p3_double,           \
                         int: p3_int,                 \
                           int***: p3_int_ss,          \
                         default: error              \
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


void p1_int(int a)
{
       printf("%s seeing a=%d \n", __func__, a);
}

void p1_double(double a)
{
       printf("%s seeing a=%f \n", __func__, a);
}


void p2_int(int a, int b)
{
        printf("%s seeing a=%d %d\n", __func__, a,b);
}

void p2_double(double a,int b)
{
        printf("%s seeing a=%f %d\n", __func__, a,b);
}

void p3_int(int a, int b,int c)
{
        printf("%s seeing a=%d %d %d\n", __func__, a,b,c);
}

void p3_double(double a,int b,int c)
{
        printf("%s seeing a=%f %d %d\n", __func__, a,b,c);
}

void p3_int_ss(int*** a,int b,int c)
{
        printf("%s seeing a=%p %d %d\n", __func__,(void*) a,b,c);
}


void error (void* p, ...)
{
        printf("%s  pointer: %p\n", __func__, (void*)p);
}

int main(int argc, char** argv)
{
        galloc(6);
        galloc(6.827463);
        galloc(6,2);

        galloc(6.53, 0);
        galloc(6, 0,7);
        galloc(6.5, 0,7);

        int** i = NULL;
        galloc(&i, 0,7);
        double** d = NULL;
        galloc(&d, 0,7);

        return 0;
}
