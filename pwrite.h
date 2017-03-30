#ifndef pwrite_header

#define pwrite_header


#if (DEBUGLEVEL >= 1)

#define BUFFER_P_READ_SIZE 1000
#define BUFFER_P_WRITE_SIZE 1000

#else

#define BUFFER_P_READ_SIZE  1000000
#define BUFFER_P_WRITE_SIZE 500000

#endif

struct thread_control_variables;
struct pwrite_buffer;

struct pwrite_main{
	void* (*write_thread_function)(void *threadarg);
	int (*write)(struct pwrite_main* pw,const int id,const char *format,...);
	int (*flush)(struct pwrite_main* pw,const int id);
	void(*free) (struct pwrite_main* pm);
	struct thread_control_variables* tcv;
	struct pwrite_buffer** memory; 
	struct pwrite_buffer* disk;
	struct pwrite_buffer* shared_buffer;
	FILE* out_ptr; 
	int num_threads;
	int buffer_len;
};

extern struct pwrite_main* init_pwrite_main(char* outname, int num_threads,int buffer_size);



#endif



