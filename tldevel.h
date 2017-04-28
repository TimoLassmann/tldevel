#ifndef libtldevel_included

#define libtldevel_included

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <math.h>



#define MESSAGE_MARGIN 22
#define TYPE_MARGIN 8 


#define OK              0
#define FAIL            1

#define BUFFER_LEN 500
#define LINE_LEN 10000

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ " line " TOSTRING(__LINE__) 

#define MCALLOC(p,count,type) do {					\
		if (p != NULL){						\
			ERROR_MSG( "calloc on a nun-null pointer");	\
			goto ERROR;					\
		}							\
		if (((p) = calloc(count, sizeof(type))) == NULL) {	\
			ERROR_MSG("calloc of n=%d of type %s failed", count, #type); \
			goto ERROR;					\
		}							\
	} while (0)

#define MMALLOC(p,size) do {						\
		if (p != NULL){						\
			ERROR_MSG( "malloc on a nun-null pointer");	\
			goto ERROR;					\
		}							\
		if (((p) = malloc(size)) == NULL && (size)) {		\
			ERROR_MSG("malloc of size %d failed", size);	\
			goto ERROR;					\
		}							\
	} while (0)

#define MREALLOC(p, newsize) do {					\
		void *tmpp;						\
		if ((p) == NULL) {					\
			tmpp = malloc(newsize);				\
		}else {							\
			tmpp = realloc((p), (newsize));			\
		}							\
		if (tmpp != NULL){					\
			p = tmpp;					\
		}else {							\
			ERROR_MSG("realloc for size %d failed", newsize); \
			goto ERROR;					\
		}} while (0)

#define ADDFAILED(x)  "Function \"" TOSTRING(x) "\" failed."

#define RUN(EXP) do {					\
		if((EXP) != OK){			\
			ERROR_MSG(ADDFAILED(EXP));	\
		}					\
	}while (0)

#define RUNP(EXP) do {					\
		if((EXP) == NULL){			\
			ERROR_MSG(ADDFAILED(EXP));	\
		}					\
	}while (0)

#define ERROR_MSG(...) do {			\
		tlog.error(AT, __VA_ARGS__ );	\
		goto ERROR;			\
	}while (0)

#define WARNING_MSG(...) do {			\
		tlog.warning(AT, __VA_ARGS__ );	\
	}while (0)


#define LOG_MSG(...) do {				\
		tlog.log_message( __VA_ARGS__ );	\
	}while (0)

#define UNFORMAT_MSG(...) do {				\
		tlog.unformatted( __VA_ARGS__ );	\
	}while (0)

#if (DEBUGLEVEL >= 1)
#define DPRINTF1(...)  tlog.message(AT,##__VA_ARGS__);
#define DCHECK1(TEST,...) if(!(TEST)) {tlog.error(AT,#TEST );tlog.error(AT,##__VA_ARGS__);goto ERROR;}

#define MFREE(p) do {						\
		if(p){						\
			free(p);				\
			p = NULL;				\
		}else{						\
			WARNING_MSG("free on a null pointer");	\
		}						\
	} while (0)

#else
#define DPRINTF1(...)
#define DCHECK1(A,...)

#define MFREE(p) do {				\
		free(p);			\
		p = NULL;			\
	} while (0)
#endif

#if (DEBUGLEVEL >= 2)
#define DPRINTF2(...)  tlog.message(AT,##__VA_ARGS__);
#define DCHECK2(TEST,...)  if(!(TEST)) {tlog.error(AT,#TEST );tlog.error(AT,##__VA_ARGS__);goto ERROR;}
#else
#define DPRINTF2(...)
#define DCHECK2(TEST,...)
#endif
#if (DEBUGLEVEL >= 3)
#define DPRINTF3(...)  tlog.message(AT,##__VA_ARGS__);
#define DCHECK3(TEST,...)  if(!(TEST)) {tlog.error(AT,#TEST );tlog.error(AT,##__VA_ARGS__);goto ERROR;}
#else
#define DPRINTF3(...)
#define DCHECK3(TEST,...)
#endif

#define ASSERT(TEST,...)  if(!(TEST)) {  tlog.error(AT,#TEST );tlog.error(AT, ##__VA_ARGS__);goto ERROR;}





#define MACRO_MIN(a,b)          (((a)<(b))?(a):(b))
#define MACRO_MAX(a,b)          (((a)>(b))?(a):(b))

#define LOGSUM_SIZE 1600000


#define SCALE 100000.0

#define DECLARE_TIMER(n) clock_t _start_##n; clock_t _stop_##n;
#define START_TIMER(n) _start_##n = clock();
#define STOP_TIMER(n) _stop_##n = clock();
#define GET_TIMING(n) (double)(_stop_##n - _start_##n)   / CLOCKS_PER_SEC

#define DECLARE_CHK(n,dir) struct checkpoint* chk_##n = NULL;  RUNP( chk_##n =  init_checkpoint(TOSTRING(n),dir));

#define RUN_CHECKPOINT(n,EXP,...) do {					\
		if(test_for_checkpoint_file(chk_##n) ==0 ){		\
			RUN(EXP);					\
			RUN(set_checkpoint_file(chk_##n,TOSTRING(EXP),AT)); \
		}else{							\
			tlog.log_message("Skipping over: %s (%s)",TOSTRING(EXP),AT); \
		}							\
		chk_##n->test_num += 1;					\
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

typedef struct {
	void (*log_message )(const char *format, ...);
	void (*message)(const char *location, const char *format, ...);
	void (*warning)(const char *location, const char *format, ...);
	void (*error)(const char *location, const char *format, ...);
	void (*unformatted) ( const char *format, ...);
	void (*echo_build_config) (void);
	void (*print_program_description) (const char **argv,const char* description);
	int (*  set_logfile)(char* logfilename);
} tlog_namespace;

extern tlog_namespace  tlog;

extern char build_config[];

float logsum_lookup[LOGSUM_SIZE];


extern int print_program_header(const char **argv,const char* description);
extern int log_command_line(const int argc,const char * argv[]);



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

extern struct checkpoint* init_checkpoint(char* base_name,char* target_dir);
extern int test_for_checkpoint_file(struct checkpoint* chk);

extern int set_checkpoint_file(struct checkpoint* chk,char* function,char* location);
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
//float logsub(const float a,const float b);
extern float prob2scaledprob(float p);
extern float scaledprob2prob(float p);




#endif

