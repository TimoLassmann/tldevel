#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <stdint.h>
#include "tldevel.h"


#include "pwrite.h"


struct pwrite_buffer{
	char* buffer;
	int len;
	int pos;
};

struct thread_control_variables{
	pthread_mutex_t mutex; // needed to add/remove data from the buffer
	pthread_cond_t can_produce; // signaled when items are removed
	pthread_cond_t can_consume; // signaled when items are added
	uint8_t run;
	uint8_t writer_ready;
};


/* init conditional variables to orchastrate buffer switches..  */
int init_thread_control_variables(struct pwrite_main* pw);
/* allocate space for buffers */
int init_write_buffers(struct pwrite_main* pw);

/* parallel write */
int pwrite(struct pwrite_main* pw,const int id,const char *format,...);

/* flush buffer i and re-set run - necessary in user thread functions..  */
int cleanup_p_write(struct pwrite_main* pw,const int id);
int flush_pwrite(struct pwrite_main* pw,const int id);


/* Free's parallel write structure...  */
void free_pw_main(struct pwrite_main* pw);

/* this functionwill write data to disk  */
void* write_thread_function(void *threadarg);
/* This function will block starting threads until the writer thread is ready...  */
int wait_for_writer(struct pwrite_main* pw);



struct pwrite_main* init_pwrite_main(char* outname, int num_threads,int buffer_size)
{
	struct pwrite_main* pw = NULL;

	ASSERT(outname != NULL, "no filename give.");
	ASSERT(num_threads < 256,"too many write threads (%d).",num_threads);
	ASSERT(buffer_size > 1,"too little memory for each write buffer. (%d).",buffer_size);

	MMALLOC(pw, sizeof(struct pwrite_main));
	/* initialization */
	pw->tcv = NULL;
	pw->disk = NULL;
	pw->memory = NULL;
	pw->shared_buffer = NULL;
	pw->out_ptr = NULL;

	if(my_file_exists(outname)){
		WARNING_MSG("File \%s\" will be overwritten.",outname);
	}

	RUNP(pw->out_ptr = fopen(outname, "w"));
	
	pw->buffer_len = buffer_size;
	pw->num_threads = num_threads;

	pw->write_thread_function = write_thread_function;
	pw->write_wait = wait_for_writer;
	pw->free = free_pw_main;
	pw->write = pwrite;
	pw->flush = cleanup_p_write;
	/* allocations etc.. */
	RUN(init_thread_control_variables(pw));
	RUN(init_write_buffers(pw));
	
	return pw;
ERROR:
	free_pw_main(pw);
	return NULL;
}

void*  write_thread_function(void *threadarg)
{
	struct pwrite_main* pw = NULL;
	struct pwrite_buffer* tmp = NULL;
	
	pw = (struct pwrite_main*)threadarg;
	
	while(1) {
		pthread_mutex_lock(&pw->tcv->mutex);
		pw->tcv->writer_ready = 1;
		pthread_cond_signal(&pw->tcv->can_produce);
	
		while(pw->shared_buffer->pos == 0 && pw->tcv->run) { // empty			
			pthread_cond_wait(&pw->tcv->can_consume, &pw->tcv->mutex);
		}
		if(pw->shared_buffer->pos == 0 && !pw->tcv->run ){
			pthread_mutex_unlock(&pw->tcv->mutex);
			break;
		}
		
	        tmp = pw->disk;
		pw->disk = pw->shared_buffer;
		pw->shared_buffer = tmp;

		pw->shared_buffer->pos = 0;//pw->shared_buffer->len;
		
		/* maybe check if pw->shared is empty (i.e. contents were written)  */
	        		
		pthread_cond_signal(&pw->tcv->can_produce);
		pthread_mutex_unlock(&pw->tcv->mutex);
		/* the actual write.. */
		fprintf(pw->out_ptr,"%s",pw->disk->buffer);
		pw->disk->pos = 0;
		
	}
	return NULL;//	pthread_exit(0);
}

int wait_for_writer(struct pwrite_main* pw)
{
	pthread_mutex_lock(&pw->tcv->mutex);
	LOG_MSG("Waiting for writer thread to enter loop...");
	while(pw->tcv->writer_ready != 1 ) { //shared buffer is not empty...
		pthread_cond_wait(&pw->tcv->can_produce, &pw->tcv->mutex);
	}
	LOG_MSG("Done waiting.");
	//fprintf(stdout,"%'lld\tunlocking\n", (long long)threadID1);
	pthread_mutex_unlock(&pw->tcv->mutex);
	return OK;
}


int init_write_buffers(struct pwrite_main* pw)
{
	int num_buffers = pw->num_threads;
	int i; 
	MMALLOC(pw->memory, sizeof(struct pwrite_buffer*) * num_buffers);

	for (i = 0; i < num_buffers; i++) {
		pw->memory[i] = NULL;
		MMALLOC(pw->memory[i],sizeof(struct pwrite_buffer));
		pw->memory[i]->pos = 0;
		
		pw->memory[i]->len = pw->buffer_len;
		pw->memory[i]->buffer = NULL;
		MMALLOC(pw->memory[i]->buffer,sizeof(char) * 	pw->memory[i]->len);
				
	}
	pw->disk = NULL;
	MMALLOC(pw->disk,sizeof(struct pwrite_buffer));
	pw->disk->pos = 0;		
	pw->disk->len = pw->buffer_len;
	pw->disk->buffer = NULL;
	MMALLOC(pw->disk->buffer,sizeof(char) * pw->disk->len);

	pw->shared_buffer = NULL;
	MMALLOC(pw->shared_buffer,sizeof(struct pwrite_buffer));
	pw->shared_buffer->pos = 0;		
	pw->shared_buffer->len = pw->buffer_len;
	pw->shared_buffer->buffer = NULL;
	MMALLOC(pw->shared_buffer->buffer,sizeof(char) * pw->shared_buffer->len);	
	return OK;
ERROR:
	free_pw_main(pw);
	return FAIL;
}


int init_thread_control_variables(struct pwrite_main* pw)
{
	struct thread_control_variables* tcv = NULL;

	MMALLOC(tcv, sizeof(struct thread_control_variables));
	tcv->run = pw->num_threads;
	tcv->writer_ready = 0;
	RUN(pthread_cond_init(&tcv->can_consume, NULL));
	RUN(pthread_cond_init(&tcv->can_produce, NULL));
	RUN(pthread_mutex_init(&tcv->mutex, NULL));

	pw->tcv = tcv;
	
	return OK;
ERROR:
	return FAIL;
}

void free_pw_main(struct pwrite_main* pw)
{
	int i; 
	if(pw){
		if(pw->tcv){
			/* shuts down writer thread   */
			pthread_mutex_lock(&pw->tcv->mutex);
			pw->tcv->run = 0;
			pthread_cond_signal(&pw->tcv->can_consume);
			pthread_mutex_unlock(&pw->tcv->mutex);
		
			pthread_cond_destroy(&pw->tcv->can_consume);
			pthread_cond_destroy(&pw->tcv->can_produce);
			pthread_mutex_destroy(&pw->tcv->mutex);
			MFREE(pw->tcv);
		}
	

		/* closes outfile... */
		if(pw->out_ptr){
			fclose(pw->out_ptr);
		}
		if(pw->memory){
			for (i = 0; i < pw->num_threads; i++) { /* has to be +2 to free disk and shared buffer  buffer as well  */
				if(pw->memory[i]->pos){
					LOG_MSG("WARNING: parallel write buffer %d is not empty.",i);
					LOG_MSG("%s",pw->memory[i]->buffer);
				}
				MFREE(pw->memory[i]->buffer);
				MFREE(pw->memory[i]);
			}
			MFREE(pw->memory);
		}
		if(pw->disk){
			MFREE(pw->disk->buffer);
			MFREE(pw->disk);
		}
		if(pw->shared_buffer){
			MFREE(pw->shared_buffer->buffer);
			MFREE(pw->shared_buffer);
		}	
		MFREE(pw);
	}
}


int cleanup_p_write(struct pwrite_main* pw,const int id)
{
	RUN(flush_pwrite(pw,id));

	fprintf(stdout,"got lock cleanup write... %d\n",pw->shared_buffer->pos);
	pthread_mutex_lock(&pw->tcv->mutex);

	pw->tcv->run = pw->tcv->run -1;
	fprintf(stdout,"got lock2 cleanup write... %d\n",pw->shared_buffer->pos);
	pthread_cond_signal(&pw->tcv->can_consume);
	
	pthread_mutex_unlock(&pw->tcv->mutex);
	return OK;
ERROR:
	return FAIL;
}

int flush_pwrite(struct pwrite_main* pw,const int id)
{

	struct pwrite_buffer* tmp = NULL;
	pthread_mutex_lock(&pw->tcv->mutex);
	//fprintf(stdout,"got lock flusd write %d\n",pw->shared_buffer->pos);
	while(pw->shared_buffer->pos != 0 ) { //shared buffer is not empty...
		pthread_cond_wait(&pw->tcv->can_produce, &pw->tcv->mutex);
	}
	//LOG_MSG("flipping in DATA threads");
	//LOG_MSG("flipping memory %d to shared %d.",pw->memory[id]->pos,pw->shared_buffer->pos);
	tmp = pw->shared_buffer;
	pw->shared_buffer = pw->memory[id];
	pw->memory[id] = tmp;
	//pw->shared_buffer->pos = pw->shared_buffer->len;
		
	pthread_cond_signal(&pw->tcv->can_consume);
	//fprintf(stdout,"%'lld\tunlocking\n", (long long)threadID1);
	pthread_mutex_unlock(&pw->tcv->mutex);	
	return OK;
}

int pwrite(struct pwrite_main* pw,const int id,const char *format,...)
{
        va_list ap; 
	int w = 0;
	
	char* buffer = pw->memory[id]->buffer;
	int pos = pw->memory[id]->pos;
	int len = pw->memory[id]->len;
	
	va_start(ap, format);	
	w += vsnprintf(buffer+pos, len -pos, format, ap);
	va_end(ap);

	if(w <= -1){
		ERROR_MSG("write error");
	}	
	if(w + pos  >= len){
		buffer[pos] = 0;
		
		/* switch buffers and write.  */
	        RUN(flush_pwrite(pw,id));
			
	        /* re-establish connection  */
		buffer = pw->memory[id]->buffer;
		pos = pw->memory[id]->pos;
		len = pw->memory[id]->len;

		w = 0;	
		va_start(ap, format);
		w += vsnprintf(buffer+pos, len -pos, format, ap);	
		va_end(ap);
		
		if(w + pos  >= len){
			ERROR_MSG("No space ever for single write!!!");
		}
	}
	pos += w;


	pw->memory[id]->pos = pos; 
	
	return OK;
ERROR:
	return FAIL;
}



#ifdef ITEST

#define NUMTHREADS 65

#include "thr_pool.h"

struct thread_data{
	struct pwrite_main* pw;
	int num_threads;
	int thread_id;
};
	

void*  test_write_stuff(void *threadarg);

void*  test_write_stuff(void *threadarg)
{
	struct thread_data* td = (struct thread_data*) threadarg;
	struct pwrite_main* pw = NULL;
	int i;
	//int c = td->thread_id*10000000;
	LOG_MSG("thread %d says hello...",td->thread_id);


	pw = td->pw;
	for(i = 0; i < 10000;i++){
		if(i % td->num_threads == td->thread_id){
			RUN(pw->write(pw,td->thread_id,"%d\n",i));
		}
	}


	pw->flush(pw,td->thread_id);

	return NULL;
ERROR:
	
	pw->flush(pw,td->thread_id);
	return NULL;
}



int main (int argc,char * argv[])
{
	struct pwrite_main* pw = NULL;
	char* filename = NULL;

	struct thread_data** td = NULL;
	struct thr_pool* pool = NULL;

	int num_threads = 10;
	int t;
	int i;
	int status;

	MMALLOC(filename,sizeof(char)*100);
	
	for (i = 1; i < 16; i++) {
				
		num_threads = i;
		td = NULL;
		pool = NULL;
		pw = NULL;
		filename[0] = 0;

		snprintf(filename, 100,"pwtest_t%d.txt",i);
		
		RUNP(pool = thr_pool_create(num_threads+1,num_threads+1, 0, 0));

		RUNP(pw = init_pwrite_main(filename,num_threads,1000));

	
		if((status = thr_pool_queue(pool,  pw->write_thread_function, pw)) == -1) fprintf(stderr,"Adding job to queue failed.");	

		RUN(wait_for_writer(pw));

		MMALLOC(td, sizeof(struct thread_data*)* num_threads );

		for(t = 0; t < num_threads;t++){
			td[t] = NULL;
			MMALLOC(td[t] , sizeof(struct thread_data));
			td[t]->thread_id = t;
			td[t]->num_threads = num_threads;
			td[t]->pw = pw;
			if((status = thr_pool_queue(pool, test_write_stuff, (void *)td[t])) == -1) fprintf(stderr,"Adding job to queue failed.");
		
		}

		thr_pool_wait(pool);
		thr_pool_destroy(pool);
		for(t = 0; t < num_threads;t++){
			MFREE(td[t]);
		}
		MFREE(td);

		pw->free(pw);
	}
	MFREE(filename);
	return EXIT_SUCCESS;
ERROR:
	if(filename){
		MFREE(filename);
	}
	thr_pool_destroy(pool);
	return EXIT_FAILURE;
}
#endif
