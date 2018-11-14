#ifndef libtldevel_included

#define libtldevel_included


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <math.h>
#include <ctype.h>

/* Basic stuff */
#define OK              0
#define FAIL            1

#define BUFFER_LEN 500
#define LINE_LEN 10000

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ " line " TOSTRING(__LINE__)

#define MACRO_MIN(a,b)          (((a)<(b))?(a):(b))
#define MACRO_MAX(a,b)          (((a)>(b))?(a):(b))

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


/* END Basic stuff */




/* Start logging block */


#define MESSAGE_MARGIN 22
#define TYPE_MARGIN 8

extern void error(const char *location, const char *format, ...);
extern void warning(const char *location, const char *format, ...);
extern void log_message( const char *format, ...);
extern void message(const char *location, const char *format, ...);

extern int log_command_line(const int argc,char* const argv[]);
extern char* make_cmd_line(const int argc,char* const argv[]);

extern char build_config[];
extern int print_program_header(char* const argv[],const char* description);
//extern void echo_build_config (void);



#define ERROR_MSG(...) do {                     \
                error(AT, __VA_ARGS__ );   \
                goto ERROR;                     \
        }while (0)

#define WARNING_MSG(...) do {                   \
                warning(AT, __VA_ARGS__ );	\
        }while (0)


#define LOG_MSG(...) do {                         \
                log_message( __VA_ARGS__ );	\
        }while (0)

#define CODE_MSG(...) do {                       \
                message(AT, __VA_ARGS__ );      \
        }while (0)


#define ASSERT(TEST,...)  if(!(TEST)) {         \
                error(AT,#TEST );          \
                error(AT, ##__VA_ARGS__);  \
                goto ERROR;                     \
        }

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




/****************************/
/* End of logging functions */
/****************************/

/******************************/
/* Start of memory functions  */
/******************************/

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



#define ALLOC_1D_ARRAY_DEF(type)                                            \
        extern type *alloc_1D_array_size_ ##type (type *array, int dim1);

#define ALLOC_2D_ARRAY_DEF(type)                                            \
extern type **alloc_2D_array_size_ ##type (type **array, int dim1,int dim2);


#define FREE_1D_ARRAY_DEF(type)                          \
        extern void free_1d_array_ ##type(type *array);

#define FREE_2D_ARRAY_DEF(type)                         \
        extern void free_2d_array_ ##type(type **array);

FREE_1D_ARRAY_DEF(int)
FREE_1D_ARRAY_DEF(float)
FREE_1D_ARRAY_DEF(double)
FREE_1D_ARRAY_DEF(int_fast32_t)

FREE_2D_ARRAY_DEF(int)
FREE_2D_ARRAY_DEF(float)
FREE_2D_ARRAY_DEF(double)
FREE_2D_ARRAY_DEF(int_fast32_t)

ALLOC_1D_ARRAY_DEF(int)
ALLOC_1D_ARRAY_DEF(float)
ALLOC_1D_ARRAY_DEF(double)
ALLOC_1D_ARRAY_DEF(int_fast32_t)

ALLOC_2D_ARRAY_DEF(int)
ALLOC_2D_ARRAY_DEF(float)
ALLOC_2D_ARRAY_DEF(double)
ALLOC_2D_ARRAY_DEF(int_fast32_t)



//int  *alloc_1D_array_size_int (int *array, int dim1);

#define galloc(...) SELECTGALLOC(__VA_ARGS__)(__VA_ARGS__)

#define SELECTGALLOC(...) CONCAT(SELECTGALLOC_, NARG(__VA_ARGS__))(__VA_ARGS__)
#define CONCAT(X, Y) CONCAT_(X, Y)
#define CONCAT_(X, Y) X ## Y

#define SELECTGALLOC_0()

#define SELECTGALLOC_1(_1) _Generic ((_1),          \
                               default: galloc_void \
                )

#define SELECTGALLOC_2(_1, _2) _Generic((_1),                           \
                                  int*: alloc_1D_array_size_int,        \
                                  float*:  alloc_1D_array_size_float,   \
                                  double*:alloc_1D_array_size_double,   \
                                  int_fast32_t*: alloc_1D_array_size_int_fast32_t \
                )

#define SELECTGALLOC_3(_1, _2, _3) _Generic((_1),                       \
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




/**************************/
/* All the free functions */
/**************************/

#define FREE_1D_ARRAY(type)                          \
        void free_1d_array_ ##type(type *array){     \
                MFREE((void*)array - sizeof(mem_i));  \
        }


#define FREE_2D_ARRAY(type)                         \
        void free_2d_array_ ##type(type **array){   \
                 MFREE(array[0]);                     \
                 MFREE((void*)array- sizeof(mem_i));  \
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






/***************************************************************/
/* Mildely more clever versions for allocing 1D and 2d arrays. */
/***************************************************************/


#define ALLOC_1D_ARRAY(type)                                            \
        type *alloc_1D_array_size_ ##type (type *array, int dim1) {     \
                mem_i* h = NULL;                                        \
                void* tmp = NULL;                                       \
                if(array == NULL){                                      \
                        MMALLOC(tmp,(dim1  * sizeof *array + sizeof(mem_i))); \
                }else{                                                  \
                        tmp = array;                                    \
                        tmp = tmp - sizeof(mem_i);                      \
                        h = (mem_i*)(tmp);                              \
                        if(h->dim1 < dim1){                             \
                                MREALLOC(tmp,(dim1  * sizeof *array + sizeof(mem_i))); \
                        }else{                                          \
                                return tmp + sizeof(mem_i);             \
                        }                                               \
                }                                                       \
                h = (mem_i*)(tmp);                                      \
                h->dim1  = dim1;                                        \
                h->dim2  = 0;                                           \
                return (type*)  (tmp + sizeof(mem_i));                  \
        ERROR:                                                          \
                return NULL;                                            \
        }


#define ALLOC_2D_ARRAY(type)                                            \
        type **alloc_2D_array_size_ ##type (type **array, int dim1,int dim2) { \
                int i;                                                  \
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
                        ptr_t =(type**) (tmp + sizeof(mem_i));          \
                        for(i = 0;i< dim1;i++){                         \
                                ptr_t[i] = ptr_tt + i * dim2;           \
                        }                                               \
                        array = ptr_t;                                  \
                }else{                                                  \
                        ptr_tt = array[0];                              \
                        tmp = (void*)(array) -sizeof(mem_i) ;           \
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
        ERROR:                                                          \
                return NULL;                                            \
        }



/******************************/
/* End of memory functions  */
/******************************/


/* Logging DONE */
#define MCALLOC(p,count,type) do {                                      \
                if (p != NULL){                                         \
                        ERROR_MSG( "calloc on a nun-null pointer");     \
                        goto ERROR;                                     \
                }                                                       \
                if (((p) = calloc(count, sizeof(type))) == NULL) {      \
                        ERROR_MSG("calloc of n=%d of type %s failed", count, #type); \
                        goto ERROR;                                     \
                }                                                       \
        } while (0)

#if (DEBUGLEVEL >= 1)
#define DPRINTF1(...)  message(AT,##__VA_ARGS__);
#define DCHECK1(TEST,...) if(!(TEST)) {error(AT,#TEST );error(AT,##__VA_ARGS__);goto ERROR;}
#else
#define DPRINTF1(...)
#define DCHECK1(A,...)
#endif

#if (DEBUGLEVEL >= 2)
#define DPRINTF2(...)  message(AT,##__VA_ARGS__);
#define DCHECK2(TEST,...)  if(!(TEST)) {error(AT,#TEST );error(AT,##__VA_ARGS__);goto ERROR;}
#else
#define DPRINTF2(...)
#define DCHECK2(TEST,...)
#endif

#if (DEBUGLEVEL >= 3)
#define DPRINTF3(...)  message(AT,##__VA_ARGS__);
#define DCHECK3(TEST,...)  if(!(TEST)) {error(AT,#TEST );error(AT,##__VA_ARGS__);goto ERROR;}
#else
#define DPRINTF3(...)
#define DCHECK3(TEST,...)
#endif

#define LOGSUM_SIZE 1600000
#define SCALE 100000.0

#define DECLARE_TIMER(n) clock_t _start_##n; clock_t _stop_##n;
#define START_TIMER(n) _start_##n = clock();
#define STOP_TIMER(n) _stop_##n = clock();
#define GET_TIMING(n) (double)(_stop_##n - _start_##n)   / CLOCKS_PER_SEC

#define DECLARE_CHK(n,dir) struct checkpoint* chk_##n = NULL;  RUNP( chk_##n =  init_checkpoint(TOSTRING(n),dir));

#define RUN_CHECKPOINT(n,EXP,CMD) do {                                  \
                                   if(test_for_checkpoint_file(chk_##n,TOSTRING(EXP),AT,CMD) ==0 ){ \
                                           RUN(EXP);                    \
                                           RUN(set_checkpoint_file(chk_##n,TOSTRING(EXP),AT,CMD)); \
                                   }else{                               \
                                           log_message("Skipping over: %s (%s)",TOSTRING(EXP),AT); \
                                   }                                    \
                                   chk_##n->test_num += 1;              \
                           }while (0)

#ifndef MAXMEM
#define MAXMEM 4
#endif

#define MAX_MEMORY_LIMIT (int64_t) MAXMEM * 1073741824LL

#define DESTROY_CHK(n) if(chk_##n){free_checkpoint( chk_##n);};

                           struct checkpoint{
                                   char* base_dir;
                                   char* base_name;
                                   int test_num;
                           };

        /* typedef struct { */
        /*         void (*log_message )(const char *format, ...); */
        /*         void (*message)(const char *location, const char *format, ...); */
        /*         void (*warning)(const char *location, const char *format, ...); */
        /*         void (*error)(const char *location, const char *format, ...); */
        /*         void (*unformatted) ( const char *format, ...); */
        /*         void (*echo_build_config) (void); */
        /*         void (*print_program_description) (char *const argv[],const char* description); */
        /*         int (*  set_logfile)(char* logfilename); */
        /* } tlog_namespace; */

        /* extern tlog_namespace  tlog; */

        /* extern char build_config[]; */

        float logsum_lookup[LOGSUM_SIZE];


        extern int print_program_header(char * const argv[],const char* description);


        extern int get_time(char* time_ptr, int size);
        extern int my_file_exists(char* name);

        extern char** malloc_2d_char(char**m,int newdim1, int newdim2, char fill_value);
        extern int** malloc_2d_int(int**m,int newdim1, int newdim2, int fill_value);
        extern float** malloc_2d_float(float**m,int newdim1, int newdim2, float fill_value);
        extern float*** malloc_3d_float(int dim1, int dim2, int dim3, float fill_value);
        extern float**** malloc_4d_float(int dim1, int dim2, int dim3,int dim4, float fill_value);

        extern double** malloc_2d_double(double**m,int newdim1, int newdim2, double fill_value);

        extern void free_2d(void** m);
        extern void free_3d(void*** m);
        extern void free_4d(void**** m);

        extern uint32_t* make_bitvector(uint32_t num_elem);
        extern int clear_bitvector(uint32_t* array,uint32_t num_elem);
        extern void bit_set(uint32_t* array, uint32_t i);
        extern void bit_clr(uint32_t* array, uint32_t i);
        extern int bit_test(uint32_t* array, uint32_t i);

//Hash function

        uint32_t adler(const void* buf, size_t len);
        extern int ulltoa(uint64_t value, char *buf, int radix);

        extern struct checkpoint* init_checkpoint(char* base_name,char* target_dir);

        extern int test_for_checkpoint_file(struct checkpoint* chk,char* function,char* location, char* cmd);

        extern int set_checkpoint_file(struct checkpoint* chk,char* function,char* location,char* cmd);
        extern void free_checkpoint(struct checkpoint* chk);


        extern char* shorten_pathname(char* p);
        extern char* basename(const char* name);

        extern int replace_punctuation_with_underscore(char* p);

/* misc math functions */
        extern void init_logsum();


        extern uint16_t prob_to_uint16(float x);
        extern float uint16_to_prob(uint16_t a);

        extern uint32_t prob_to_uint32(float x);
        extern float uint32_to_prob(uint32_t a);



        extern float logsum(const float a,const float b);
        extern float prob2scaledprob(float p);
        extern float scaledprob2prob(float p);

        extern float random_float_zero_to_x(const float x);
        extern uint32_t random_int_zero_to_x(const uint32_t x);


        extern float random_float_zero_to_x_thread(const float x, unsigned int* seed);
        extern uint32_t random_int_zero_to_x_thread(const uint32_t x, unsigned int* seed);


#endif

