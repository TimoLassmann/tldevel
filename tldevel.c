#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "tldevel.h"

#include <math.h>
#include <ctype.h>

static char logfile[BUFFER_LEN] = "default.log";

//static  void message(const char *location, const char *format, ...);
// void message_tee(const char *location, const char *format, ...);

static void log_message( const char *format, ...);
static void log_message_tee( const char *format, ...);

static void message(const char *location, const char *format, ...);
static void message_tee(const char *location, const char *format, ...);

static void warning(const char *location, const char *format, ...);
static void warning_tee(const char *location, const char *format, ...);

static void error(const char *location, const char *format, ...);
static void error_tee(const char *location, const char *format, ...);

static void unformatted( const char *format, ...);
static void unformatted_tee( const char *format, ...);

static void vunformat(FILE* f_ptr,const char *format,  va_list argp);
static void vlog(FILE* f_ptr,const char *format,  va_list argp);
static void vwarning(FILE* f_ptr,const char *location, const char *format,  va_list argp);
static void vmessage(FILE* f_ptr,const char *location, const char *format,  va_list argp);
static void verror(FILE* f_ptr,const char *location, const char *format,  va_list argp);

static void echo_build_config (void);
static void echo_build_config_log (void);
static int set_random_seed(void);

static void print_program_description(char* const argv[],const char* description);
static void print_program_description_log(char* const argv[],const char* description);

int print_program_header(char* const argv[],const char* description)
{
	tlog.echo_build_config();
	tlog.print_program_description(argv,description); 
	return OK;
}

void print_program_description_log(char* const argv[],const char* description){
	int i;
	int newline = 0;
	FILE* file;
		
	if(!my_file_exists(logfile)){
		file = fopen(logfile, "w");
	}else{
		file = fopen(logfile, "a");
	}
  
	fprintf(file,"%-*s: %s\n" ,MESSAGE_MARGIN,"Running program", basename(argv[0]));
	if(description){
		newline = 1;
		fprintf(file,"%-*s: ",MESSAGE_MARGIN,"Description");
		for(i = 0;i < (int) strlen(description);i++){
			if(description[i] == '\n'){
				fprintf(file,"\n");
				fprintf(file,"%-*s: ",MESSAGE_MARGIN,"");
				newline = 1;
				
			}else{
				if(isspace(description[i]) &&newline){
					
				}else{
					fprintf(file,"%c",description[i]);
					newline = 0;
				}
			}
		}
		fprintf(file,"\n");
	}
	fflush(file);
	
	if(file){
		fclose(file);
	} 
	// print param here? ....
}


void print_program_description(char * const argv[],const char* description)
{
	int i;
	int newline = 0;
	fprintf(stdout,"%-*s: %s\n" ,MESSAGE_MARGIN,"Running program", basename(argv[0]));
	if(description){
		newline = 1;
		fprintf(stdout,"%-*s: ",MESSAGE_MARGIN,"Description");
		for(i = 0;i < (int) strlen(description);i++){
			if(description[i] == '\n'){
				fprintf(stdout,"\n");
				fprintf(stdout,"%-*s: ",MESSAGE_MARGIN,"");
				newline = 1;
				
			}else{
				if(isspace(description[i]) &&newline){
					
				}else{
					fprintf(stdout,"%c",description[i]);
					newline = 0;
				}
			}
		}
		fprintf(stdout,"\n");
	}
	fflush(stdout);
}




void echo_build_config (void)
{
	fprintf(stdout,"\n%s\n",build_config);
	fflush(stdout);
	
	set_random_seed();
}

void echo_build_config_log (void)
{
	//
	//echo_build_config();
	
	FILE *file;
	if(!my_file_exists(logfile)){
		file = fopen(logfile, "w");
	}else{
		file = fopen(logfile, "a");
	}
	DPRINTF3("Logfile: %s\n",logfile );
	fprintf(file,"\n%s\n",build_config);
	
	
	if(file){
		fclose(file);
	}
	set_random_seed();
}

int set_random_seed(void)
{
#ifdef HAVE_ARC4RANDOM_UNIFORM
	return OK;
#elif HAVE_ARC4RANDOM
	return OK;
#else
	srand((unsigned int) (time(NULL) * (42)));
	srand48((long int)  (time(NULL) * (42)));
	return OK;
#endif // HAVE_ARC4RANDOM

}



int set_logfile(char* logfilename)
{
	//ASSERT(logfilename != NULL,"No logfilename given.");

	if(logfilename != NULL){
		ASSERT(strlen(logfilename)  > 3, "log file name: %s is too short. ",logfilename );
		
		memset( logfile, 0, BUFFER_LEN );
		strcpy( logfile, logfilename);

		tlog.log_message = log_message_tee;
		tlog.warning = warning_tee;
		tlog.message = message_tee;
		tlog.error = error_tee;
		tlog.unformatted = unformatted_tee;
		tlog.echo_build_config = echo_build_config_log; // this switches print description to log file (nothing to stdout)
		tlog.print_program_description = print_program_description_log; // this switches print description to log file (nothing to stdout)
	}else{
		tlog.log_message = log_message;
		tlog.warning = warning;
		tlog.message = message;
		tlog.error = error;
		tlog.unformatted = unformatted;
		tlog.echo_build_config = echo_build_config;
		tlog.print_program_description = print_program_description;

	}
	
	
	
	return OK;
ERROR:
	return FAIL;
}

int set_checkpoint_file(struct checkpoint* chk,char* function,char* location)
{ 
	char buffer[BUFFER_LEN];
	FILE* f_ptr = NULL;
	
	struct tm *ptr;
	
	char time_string[200];
	
	time_t current = time(NULL);
	ptr = localtime(&current);
	
	if(!strftime(time_string, 200, "%F %H:%M:%S", ptr)){
		error(AT,"Write failed");
	}
	
	
	snprintf(buffer,BUFFER_LEN ,"%s/%s_%d.chk", chk->base_dir,chk->base_name,chk->test_num );
	RUNP(f_ptr = fopen(buffer , "w" ));
	
	fprintf(f_ptr,"%*s: %d\n",MESSAGE_MARGIN, "checkpoint ID", chk->test_num);
	fprintf(f_ptr,"%*s: %s\n",MESSAGE_MARGIN, "function", function);
	fprintf(f_ptr,"%*s: %s\n",MESSAGE_MARGIN, "called in", location);
	fprintf(f_ptr,"%*s: %s\n",MESSAGE_MARGIN, "at time", time_string);
	fclose(f_ptr);
	
	return OK;
ERROR:
	if(f_ptr){
		fclose(f_ptr);
	}
	return FAIL;
}

int test_for_checkpoint_file(struct checkpoint* chk)
{
	char buffer[BUFFER_LEN];
	static int8_t found = 0;
	
	snprintf(buffer,BUFFER_LEN ,"%s/%s_%d.chk", chk->base_dir,chk->base_name,chk->test_num );
	if(my_file_exists(buffer) && !found){
		return 1;
	}else{
		found = 1;
	}
	return 0;
}

struct checkpoint* init_checkpoint(char* base_name,char* target_dir)
{
	struct checkpoint* chk = NULL;
	size_t i = 0;
	int j;
	MMALLOC(chk, sizeof(struct checkpoint));
	
	chk->test_num = 0;
	chk->base_dir = NULL;
	chk->base_name = NULL;
	
	i = strlen(target_dir);
	MMALLOC(chk->base_dir, sizeof(char) * (i+1));
	
	for(j = 0;j < i;j++){
		chk->base_dir[j] = target_dir[j];
	}
	chk->base_dir[i] = 0;
	
	i = strlen(base_name);
	MMALLOC(chk->base_name, sizeof(char) * (i+1));
	for(j = 0;j < i;j++){
		chk->base_name[j] = base_name[j];
	}
	chk->base_name[i] = 0;
	
	return chk;
ERROR:
	return NULL;
}

void free_checkpoint(struct checkpoint* chk)
{
	if(chk){
		MFREE(chk->base_dir);
		MFREE(chk->base_name);
		MFREE(chk);
		chk = NULL;
	}
}

uint32_t* make_bitvector(uint32_t num_elem)
{
	uint32_t* x = NULL;
	MMALLOC(x , sizeof(int) *((num_elem / 32) + 1));
	RUN(clear_bitvector(x, num_elem));
	return x;
ERROR:
	return NULL;
}

int clear_bitvector(uint32_t* array,uint32_t num_elem)
{
	memset(array, 0, sizeof(int) *((num_elem / 32) + 1));
	return OK;
}

void bit_set(uint32_t* array, uint32_t i)
{
	array[i >> 5] |= (1 << (i & 0x1F));
}

void bit_clr(uint32_t* array, uint32_t i)
{
	array[i >> 5] &= ~(1 << (i & 0x1F));
}

int bit_test(uint32_t* array, uint32_t i)
{
	return array[i >> 5] & (1 << (i & 0x1F));
}

int my_file_exists(char* name)
{
	struct stat buf;
	int ret,local_ret;
	ret = 0;
	local_ret= stat ( name, &buf );
	/* File found */
	if ( local_ret == 0 )
	{
		ret++;
	}
	return ret;
}

uint32_t adler(const void* buf, size_t len)
{
	const uint8_t* buffer = NULL;
	uint32_t s1 = 1;
	uint32_t s2 = 0;
	
	buffer = (const uint8_t*) buf; 

	for (size_t i = 0; i < len; i++) {
		s1 = (s1 + buffer[i]) % 65521;
		s2 = (s2 + s1) % 65521;
	}
	return (s2 << 16) | s1;
}



int log_command_line(const int argc,char* const argv[])
{
	char* buffer = NULL;

	RUNP(buffer = make_cmd_line(argc,argv));
	LOG_MSG("%s",buffer);
	MFREE(buffer);
	return OK;
ERROR:
	MFREE(buffer);
	return FAIL;
}

char* make_cmd_line(const int argc,char* const argv[])
{
	char* cmd = NULL;
	int i,j,c;
	
	MMALLOC(cmd, 16384);
	
	cmd[0] = 'c';
	cmd[1] = 'm';
	cmd[2] = 'd';
	cmd[3] = ':';
	cmd[4] = ' ';
	c = 5;
	for(i =0 ; i < argc;i++){
		for(j = 0; j < strlen(argv[i]);j++){
			if(c == 16384-1){
				break;
			}
			cmd[c] = argv[i][j];
			c++;
			
		}
		if(c == 16384-1){
			break;
		}
		cmd[c] = ' ';
		c++;
		
		
	}
	cmd[c] = 0;

	return cmd;
ERROR:
	return NULL;
}

void log_message( const char *format, ...)
{
	va_list argp;
	//struct tm *ptr;
	
	//char time_string[200];
	
	//if(get_time(time_string, 200) != OK){
	//	fprintf(stdout,"notime");
	//}
	
	//if(fprintf(stdout,"%s%s",time_string,new_message ) < 0) KSLIB_XEXCEPTION(kslEWRT, "write failed");
	//fprintf(stdout,"%*s: ",MESSAGE_MARGIN, time_string);
	
	va_start(argp, format);
	vlog(stdout,format, argp);
	
	//vfprintf(stdout, format, argp);
	va_end(argp);
	//fprintf(stdout,"\n");
	//fflush(stdout);

}


void log_message_tee( const char *format, ...)
{
	va_list argp;
	//struct tm *ptr;
	
	
	
	//time_t current = time(NULL);
	//ptr = localtime(&current);
	
	//if(!strftime(time_string, 200, "[%F %H:%M:%S]", ptr)){
	//	error(AT,"Write failed");
	//}
	
	//if(fprintf(stdout,"%s%s",time_string,new_message ) < 0) KSLIB_XEXCEPTION(kslEWRT, "write failed");
	/*fprintf(stdout,"%*s: ",MESSAGE_MARGIN, time_string);
	
	  va_start(argp, format);
	  vfprintf(stdout, format, argp);
	  va_end(argp);
	  fprintf(stdout,"\n");
	  fflush(stdout);*/
	va_start(argp, format);
	vlog(stdout,format, argp);
	va_end(argp);
	//fprintf(stderr,"Writing to file : %s\n",  logfile);
	FILE *file;
	if(!my_file_exists(logfile)){
		file = fopen(logfile, "w");
	}else{
		file = fopen(logfile, "a");
	}
	va_start(argp, format);
	vlog(file,format, argp);
	va_end(argp);
	if(file){
		fclose(file);
	}
}

int get_time(char* time_ptr, int size)
{
	struct tm *ptr;
	time_t current = time(NULL);
	ptr = localtime(&current);
	if(!strftime(time_ptr, size, "[%F %H:%M:%S] ", ptr))ERROR_MSG("write failed");
	return OK;
ERROR:
	return FAIL;
}

void vunformat(FILE* f_ptr,const char *format,  va_list argp)
{
	vfprintf(f_ptr, format, argp);
	//fprintf(f_ptr,"\n");
}

void vlog(FILE* f_ptr,const char *format,  va_list argp)
{

	char time_string[200];
	
	if(get_time(time_string, 200) != OK){
		fprintf(stderr,"notime");
	}
	fprintf(f_ptr,"%*s: ",MESSAGE_MARGIN,time_string);
	fprintf(f_ptr,"%*s: ",TYPE_MARGIN,"LOG ");
	vfprintf(f_ptr, format, argp);
	fprintf(f_ptr,"\n");
	fflush(f_ptr);
}

void vmessage(FILE* f_ptr,const char *location, const char *format,  va_list argp)
{
	char time_string[200];
	
	if(get_time(time_string, 200) != OK){
		fprintf(stderr,"notime");
	}
	fprintf(f_ptr,"%*s: ",MESSAGE_MARGIN,time_string);
	fprintf(f_ptr,"%*s: ",TYPE_MARGIN,"MESSAGE ");
	vfprintf(f_ptr, format, argp);
	fprintf(f_ptr," (%s)\n",location);
	fflush(f_ptr);
	//fprintf(stderr, "\n");
}

void vwarning(FILE* f_ptr,const char *location, const char *format,  va_list argp)
{
	char time_string[200];
	
	if(get_time(time_string, 200) != OK){
		fprintf(stderr,"notime");
	}
	fprintf(f_ptr,"%*s: ",MESSAGE_MARGIN,time_string);
	fprintf(f_ptr,"%*s: ",TYPE_MARGIN,"WARNING ");
	vfprintf(f_ptr, format, argp);
	fprintf(f_ptr," (%s)\n",location);
	fflush(f_ptr);
}
// IWWAS

void verror(FILE* f_ptr, const char *location, const char *format,  va_list argp)
{
	char time_string[200];
	
	if(get_time(time_string, 200) != OK){
		fprintf(stderr,"notime");
	}
	fprintf(f_ptr,"%*s: ",MESSAGE_MARGIN,time_string);
	fprintf(f_ptr,"%*s: ",TYPE_MARGIN,"ERROR ");
	vfprintf(f_ptr, format, argp);
	fprintf(f_ptr," (%s)\n",location);
	fflush(f_ptr);
	//fprintf(stderr, "\n");
}


void unformatted( const char *format, ...)
{
	va_list argp;
	//fprintf(stdout,"%-25s %8s ", location,"MESSAGE:");
	//fprintf(stdout,"%*s: ",MESSAGE_MARGIN,"MESSAGE ");
	va_start(argp, format);
	
	vunformat(stdout,format,argp);
	//	vfprintf(stdout, format, argp);
	va_end(argp);

}

void unformatted_tee( const char *format, ...)
{
	va_list argp;
	//fprintf(stdout,"%-25s %8s ", location,"MESSAGE:");
	//fprintf(stdout,"%*s: ",MESSAGE_MARGIN,"MESSAGE ");
	va_start(argp, format);
	
	vunformat(stdout,format,argp);
	//	vfprintf(stdout, format, argp);
	va_end(argp);
	
	FILE *file;
	if(!my_file_exists(logfile)){
		file = fopen(logfile, "w");
	}else{
		file = fopen(logfile, "a");
	}
	va_start(argp, format);
	
	vunformat(file,format,argp);
	va_end(argp);
	//fprintf(file,"%*s: ",MESSAGE_MARGIN, "MESSAGE ");
	//va_start(argp, format);
	//vfprintf(file, format, argp);
	//va_end(argp);
	//fprintf(file," (%s)\n",location);
	//fflush(file);
	
	if(file){
		fclose(file);
	}
	
	
}



void message(const char *location, const char *format, ...)
{
	va_list argp;
	//fprintf(stdout,"%-25s %8s ", location,"MESSAGE:");
	//fprintf(stdout,"%*s: ",MESSAGE_MARGIN,"MESSAGE ");
	va_start(argp, format);
	
	vmessage(stdout,location,format,argp);
	//	vfprintf(stdout, format, argp);
	va_end(argp);
	//	fprintf(stdout," (%s)\n",location);
	//fprintf(stdout,"\n");
	//	fflush(stdout);
	
}

void message_tee(const char *location, const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	
	vmessage(stdout,location,format,argp);
	va_end(argp);

	
	
	
	FILE *file;
	if(!my_file_exists(logfile)){
		file = fopen(logfile, "w");
	}else{
		file = fopen(logfile, "a");
	}
	va_start(argp, format);

	vmessage(file,location,format,argp);
	va_end(argp);
	//fprintf(file,"%*s: ",MESSAGE_MARGIN, "MESSAGE ");
	//va_start(argp, format);
	//vfprintf(file, format, argp);
	//va_end(argp);
	//fprintf(file," (%s)\n",location);
	//fflush(file);
	
	if(file){
		fclose(file);
	}
}




void warning(const char *location, const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	vwarning(stdout,location, format, argp);
	va_end(argp);
}

void warning_tee(const char *location, const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	vwarning(stdout,location, format, argp);
	va_end(argp);
  
	FILE *file;
	if(!my_file_exists(logfile)){
		file = fopen(logfile, "w");
	}else{
		file = fopen(logfile, "a");
	}
	va_start(argp, format);
	vwarning(file,location, format, argp);
	va_end(argp);
  
	
	if(file){
		fclose(file);
	}
 
}


void error(const char *location, const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	verror(stderr,location,format,argp);
	va_end(argp);
}

void error_tee(const char *location, const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	verror(stderr,location,format,argp);
	va_end(argp);
	
	
	FILE *file;
	if(!my_file_exists(logfile)){
		file = fopen(logfile, "w");
	}else{
		file = fopen(logfile, "a");
	}
	va_start(argp, format);
	verror(file,location,format,argp);
	va_end(argp);
	
	if(file){
		fclose(file);
	}
}



char** malloc_2d_char(char**m,int newdim1, int newdim2, char fill_value)
{
	int i,j;
	
	char** ptr_t = NULL;
	char* ptr_tt = NULL;
	
	int* int_ptr = NULL;
	
	int olddim1,olddim2;
	int max1, max2;
	
	
	ASSERT((newdim1 > 0), "Malloc 2D char failed: dim1:%d\n",newdim1);
	ASSERT((newdim2 > 0), "Malloc 2D char failed: dim2:%d\n",newdim2);
	
	
	if(m == NULL){
		MMALLOC(ptr_t, sizeof(char*) * newdim1);
		MMALLOC(ptr_tt, sizeof(char) * (newdim1*newdim2) + 3*sizeof(int) );
		
		int_ptr = (int*)ptr_tt;
		int_ptr[0] = 2;
		int_ptr[1] = newdim1;
		int_ptr[2] = newdim2;
		
		ptr_tt = (char*)(int_ptr + 3);
		
		for(i = 0;i< newdim1;i++){
			ptr_t[i] = ptr_tt + i * newdim2;
			for(j = 0; j < newdim2;j++){
				ptr_t[i][j] = fill_value;
			}
		}
		m = ptr_t;
		
	}else{
		ptr_t = m;
		int_ptr = (int*) m[0];
		int_ptr  = int_ptr -3;
		
		ptr_tt = (char* )int_ptr;
		
		olddim1 = *(int_ptr+1);
		olddim2 = *(int_ptr+2);
		
		DPRINTF3("%d-%d new: %d-%d", olddim1,olddim2, newdim1,newdim2);
		
		/* in case we want a smaller matrix don't realloc but zero out "free mem"*/
		if(olddim1 >newdim1 || olddim2 > newdim2){
			max1 = (olddim1 > newdim1) ? olddim1:newdim1;
			max2 = (olddim2 > newdim2) ? olddim2:newdim2;
			for(i = 0; i <max1;i++){
				for(j = 0; j < max2;j++){
					if(i >= newdim1 || j >= newdim2){
						m[i][j] = fill_value;
					}
				}
			}
			if(olddim1 > newdim1){
				newdim1 = olddim1;
			}
			if(olddim1 >newdim1){
				newdim2 = olddim2;
			}
			
		}
		
		/* case 0: old == new*/
		if(olddim1 == newdim1 && olddim2 == newdim2){
			return m;
		}
		
		/*case 1 : both dimensions increase... */
		
		
		if(olddim1 < newdim1 && olddim2 < newdim2){
			MREALLOC(ptr_t,  sizeof(char*) * newdim1);
			MREALLOC(ptr_tt,  sizeof(char) * (newdim1*newdim2) + 3*sizeof(int) );
			int_ptr = (int*)ptr_tt;
			ptr_tt = (char*) (int_ptr + 3);
			for(i = olddim1-1; i >= 0;i-- ){
				for(j = olddim2-1;j >=0;j--){
					
					*(ptr_tt + i* newdim2 + j) =*(ptr_tt + i*olddim2 + j);
				}
				for(j = newdim2-1;j >= olddim2;j--){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
				
			}
			
			for(i = olddim1; i < newdim1;i++){
				for(j = 0; j < newdim2;j++){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
			}
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			
			
			int_ptr[0] = 2;
			int_ptr[1] = newdim1;
			int_ptr[2] = newdim2;
			m = ptr_t;
		}else if(olddim1 == newdim1 && olddim2 < newdim2){
			MREALLOC(ptr_t,  sizeof(char*) * newdim1);
			MREALLOC(ptr_tt,  sizeof(char) * (newdim1*newdim2) + 3*sizeof(int) );
			int_ptr = (int*)ptr_tt;
			ptr_tt = (char*) (int_ptr + 3);
			for(i = olddim1-1; i >= 0;i-- ){
				for(j = olddim2-1;j >=0;j--){
					
					*(ptr_tt + i* newdim2 + j) =*(ptr_tt + i*olddim2 + j);
				}
				for(j = newdim2-1;j >= olddim2;j--){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
				
			}
			
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			
			
			int_ptr[0] = 2;
			int_ptr[1] = newdim1;
			int_ptr[2] = newdim2;
			m = ptr_t;
		}else if(olddim1 < newdim1 && olddim2 == newdim2){
			MREALLOC(ptr_t,  sizeof(char*) * newdim1);
			MREALLOC(ptr_tt,  sizeof(char) * (newdim1*newdim2) + 3*sizeof(int) );
			int_ptr = (int*)ptr_tt;
			ptr_tt = (char*) (int_ptr + 3);
			for(i = olddim1; i < newdim1;i++){
				for(j = 0; j < olddim2;j++){
					*(ptr_tt + i* olddim2 + j) = fill_value;
				}
			}
			for(i = olddim1;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * olddim2;
			}
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			int_ptr[0] = 2;
			int_ptr[1] = newdim1;
			int_ptr[2] = newdim2;
			m = ptr_t;
			
		}
	}
	return m;
ERROR:
	MFREE(ptr_t );
	MFREE(ptr_tt);
	return NULL;
}


int** malloc_2d_int(int**m,int newdim1, int newdim2, int fill_value)
{
	int i,j;
	int** ptr_t = NULL;
	int* ptr_tt = NULL;
	
	int olddim1,olddim2;
	int max1, max2;
	
	ASSERT((newdim1 > 0), "Malloc 2D int failed: dim1:%d\n",newdim1);
	ASSERT((newdim2 > 0), "Malloc 2D int failed: dim2:%d\n",newdim2);
	
	
	
	if(m == NULL){
		MMALLOC(ptr_t, sizeof(int*) * newdim1);
		MMALLOC(ptr_tt, sizeof(int) * (newdim1*newdim2) + 3*sizeof(int));
		ptr_tt[0] = 2;
		ptr_tt[1] = newdim1;
		ptr_tt[2] = newdim2;
		
		ptr_tt = ptr_tt + 3;
		
		for(i = 0;i< newdim1;i++){
			ptr_t[i] = ptr_tt + i * newdim2;
			for(j = 0; j < newdim2;j++){
				ptr_t[i][j] = fill_value;
			}
		}
		m = ptr_t;
		
	}else{
		ptr_t = m;
		ptr_tt  =m[0]-3;
		olddim1 = *(ptr_tt+1);
		olddim2 = *(ptr_tt+2);
		DPRINTF3("%d-%d new: %d-%d", olddim1,olddim2, newdim1,newdim2 );
		
		/* in case we want a smaller matrix don't realloc but zero out "free mem"*/
		if(olddim1 >newdim1 || olddim2 > newdim2){
			max1 = (olddim1 > newdim1) ? olddim1:newdim1;
			max2 = (olddim2 > newdim2) ? olddim2:newdim2;
			for(i = 0; i <max1;i++){
				for(j = 0; j < max2;j++){
					if(i >= newdim1 || j >= newdim2){
						m[i][j] = fill_value;
					}
				}
			}
			if(olddim1 > newdim1){
				newdim1 = olddim1;
			}
			if(olddim1 >newdim1){
				newdim2 = olddim2;
			}
			
		}
		
		/* case 0: old == new*/
		if(olddim1 == newdim1 && olddim2 == newdim2){
			return m;
		}
		
		/*case 1 : both dimensions increase... */
		
		
		if(olddim1 < newdim1 && olddim2 < newdim2){
			MREALLOC(ptr_t,  sizeof(int*) * newdim1);
			MREALLOC(ptr_tt, sizeof(int) * (newdim1*newdim2) + 3*sizeof(int));
			ptr_tt = ptr_tt + 3;
			for(i = olddim1-1; i >= 0;i-- ){
				for(j = olddim2-1;j >=0;j--){
					
					*(ptr_tt + i* newdim2 + j) =*(ptr_tt + i*olddim2 + j);
				}
				for(j = newdim2-1;j >= olddim2;j--){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
				
			}
			
			for(i = olddim1; i < newdim1;i++){
				for(j = 0; j < newdim2;j++){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
			}
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			
			
			ptr_tt = ptr_tt - 3;
			ptr_tt[0] = 2;
			ptr_tt[1] = newdim1;
			ptr_tt[2] = newdim2;
			m = ptr_t;
		}else if(olddim1 == newdim1 && olddim2 < newdim2){
			MREALLOC(ptr_t,  sizeof(int*) * newdim1);
			MREALLOC(ptr_tt, sizeof(int) * (newdim1*newdim2) + 3*sizeof(int));
			ptr_tt = ptr_tt + 3;
			for(i = olddim1-1; i >= 0;i-- ){
				for(j = olddim2-1;j >=0;j--){
					
					*(ptr_tt + i* newdim2 + j) =*(ptr_tt + i*olddim2 + j);
				}
				for(j = newdim2-1;j >= olddim2;j--){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
				
			}
			
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			
			ptr_tt = ptr_tt - 3;
			ptr_tt[0] = 2;
			ptr_tt[1] = olddim1;
			ptr_tt[2] = newdim2;
			m = ptr_t;
		}else if(olddim1 < newdim1 && olddim2 == newdim2){
			MREALLOC(ptr_t,  sizeof(int*) * newdim1);
			MREALLOC(ptr_tt, sizeof(int) * (newdim1*newdim2) + 3*sizeof(int));
			ptr_tt = ptr_tt + 3;
			for(i = olddim1; i < newdim1;i++){
				for(j = 0; j < olddim2;j++){
					*(ptr_tt + i* olddim2 + j) = fill_value;
				}
			}
			for(i = olddim1;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * olddim2;
			}
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			
			
			ptr_tt = ptr_tt - 3;
			ptr_tt[0] = 2;
			ptr_tt[1] = newdim1;
			ptr_tt[2] = olddim2;
			m = ptr_t;
			
		}
	}
	return m;
ERROR:
	MFREE(ptr_t );
	MFREE(ptr_tt);
	
	return NULL;
}

float** malloc_2d_float(float**m,int newdim1, int newdim2,float fill_value)
{
	int i,j;
	float** ptr_t = NULL;
	float* ptr_tt = NULL;
	
	int* int_ptr = NULL;
	
	int olddim1,olddim2;
	int max1, max2;
	
	ASSERT((newdim1 > 0), "Malloc 2D float failed: dim1:%d\n",newdim1);
	ASSERT((newdim2 > 0), "Malloc 2D float failed: dim2:%d\n",newdim2);
	
	
	if(m == NULL){
		MMALLOC(ptr_t, sizeof(float*) * newdim1);
		MMALLOC(ptr_tt, sizeof(float) * (newdim1*newdim2) + 3*sizeof(int) );
		
		int_ptr = (int*)ptr_tt;
		int_ptr[0] = 2;
		int_ptr[1] = newdim1;
		int_ptr[2] = newdim2;
		
		ptr_tt = (float*)(int_ptr + 3);
		
		for(i = 0;i< newdim1;i++){
			ptr_t[i] = ptr_tt + i * newdim2;
			for(j = 0; j < newdim2;j++){
				ptr_t[i][j] = fill_value;
			}
		}
		m = ptr_t;
		
	}else{
		ptr_t = m;
		int_ptr = (int*) m[0];
		int_ptr  = int_ptr -3;
		
		ptr_tt = (float* )int_ptr;
		
		olddim1 = *(int_ptr+1);
		olddim2 = *(int_ptr+2);
		DPRINTF3("%d-%d new: %d-%d", olddim1,olddim2, newdim1,newdim2 );
		
		/* in case we want a smaller matrix don't realloc but zero out "free mem"*/
		if(olddim1 >newdim1 || olddim2 > newdim2){
			max1 = (olddim1 > newdim1) ? olddim1:newdim1;
			max2 = (olddim2 > newdim2) ? olddim2:newdim2;
			for(i = 0; i <max1;i++){
				for(j = 0; j < max2;j++){
					if(i >= newdim1 || j >= newdim2){
						m[i][j] = fill_value;
					}
				}
			}
			if(olddim1 > newdim1){
				newdim1 = olddim1;
			}
			if(olddim1 >newdim1){
				newdim2 = olddim2;
			}
			
		}
		
		/* case 0: old == new*/
		if(olddim1 == newdim1 && olddim2 == newdim2){
			return m;
		}
		
		/*case 1 : both dimensions increase... */
		
		if(olddim1 < newdim1 && olddim2 < newdim2){
			MREALLOC(ptr_t,  sizeof(float*) * newdim1);
			MREALLOC(ptr_tt,  sizeof(float) * (newdim1*newdim2) + 3*sizeof(int) );
			int_ptr = (int*)ptr_tt;
			ptr_tt = (float*) (int_ptr + 3);
			for(i = olddim1-1; i >= 0;i-- ){
				for(j = olddim2-1;j >=0;j--){
					
					*(ptr_tt + i* newdim2 + j) =*(ptr_tt + i*olddim2 + j);
				}
				for(j = newdim2-1;j >= olddim2;j--){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
				
			}
			
			for(i = olddim1; i < newdim1;i++){
				for(j = 0; j < newdim2;j++){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
			}
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			
			
			int_ptr[0] = 2;
			int_ptr[1] = newdim1;
			int_ptr[2] = newdim2;
			m = ptr_t;
		}else if(olddim1 == newdim1 && olddim2 < newdim2){
			MREALLOC(ptr_t,  sizeof(float*) * newdim1);
			MREALLOC(ptr_tt,  sizeof(float) * (newdim1*newdim2) + 3*sizeof(int) );
			int_ptr = (int*)ptr_tt;
			ptr_tt = (float*) (int_ptr + 3);
			for(i = olddim1-1; i >= 0;i-- ){
				for(j = olddim2-1;j >=0;j--){
					
					*(ptr_tt + i* newdim2 + j) =*(ptr_tt + i*olddim2 + j);
				}
				for(j = newdim2-1;j >= olddim2;j--){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
				
			}
			
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			
			
			int_ptr[0] = 2;
			int_ptr[1] = newdim1;
			int_ptr[2] = newdim2;
			m = ptr_t;
		}else if(olddim1 < newdim1 && olddim2 == newdim2){
			MREALLOC(ptr_t,  sizeof(float*) * newdim1);
			MREALLOC(ptr_tt,  sizeof(float) * (newdim1*newdim2) + 3*sizeof(int) );
			int_ptr = (int*)ptr_tt;
			ptr_tt = (float*) (int_ptr + 3);
			for(i = olddim1; i < newdim1;i++){
				for(j = 0; j < olddim2;j++){
					*(ptr_tt + i* olddim2 + j) = fill_value;
				}
			}
			for(i = olddim1;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * olddim2;
			}
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			int_ptr[0] = 2;
			int_ptr[1] = newdim1;
			int_ptr[2] = newdim2;
			m = ptr_t;
			
		}
	}
	return m;
	
ERROR:
	MFREE(ptr_t );
	MFREE(ptr_tt);
	return NULL;
}


float*** malloc_3d_float(int dim1, int dim2, int dim3, float fill_value)
{
	int i,j,c;
	float*** ptr_t = NULL;
	float** ptr_tt = NULL;
	float* ptr_ttt = NULL;
	
	int* int_ptr = NULL;
	
	ASSERT((dim1 > 0), "Malloc 3D float failed: dim1:%d\n",dim1);
	ASSERT((dim2 > 0), "Malloc 3D float failed: dim2:%d\n",dim2);
	ASSERT((dim3 > 0), "Malloc 3D float failed: dim3:%d\n",dim3);
	
	
	
	MMALLOC(ptr_t, sizeof(float**) * dim1);
	MMALLOC(ptr_tt, sizeof(float*) * (dim1*dim2));
	MMALLOC(ptr_ttt, sizeof(float) * (dim1 * dim2 * dim3) + 4*sizeof(int) );
	
	int_ptr = (int*)ptr_ttt;
	int_ptr[0] = 3;
	int_ptr[1] = dim1;
	int_ptr[2] = dim2;
	int_ptr[3] = dim3;
	
	ptr_ttt = (float*)(int_ptr + 4);
	
	for (i = 0; i < dim1; i++){
		ptr_t[i] = ptr_tt + dim2*i;
		for (j = 0; j < dim2; j++){
			ptr_t[i][j] = ptr_ttt + dim2*dim3*i + dim3*j;
			for(c =0 ; c < dim3;c++){
				ptr_t[i][j][c] = fill_value;
			}
		}
	}
	
	return ptr_t;
ERROR:
	MFREE(ptr_t );
	MFREE(ptr_tt);
	MFREE(ptr_ttt);
	return NULL;

}

float**** malloc_4d_float(int dim1, int dim2, int dim3,int dim4, float fill_value)
{
	int i,j,c,f;
	float**** ptr_t = NULL;
	float*** ptr_tt = NULL;
	float** ptr_ttt = NULL;
	float* ptr_tttt = NULL;
	
	int* int_ptr = NULL;
	
	
	ASSERT((dim1 > 0), "Malloc 3D float failed: dim1:%d\n",dim1);
	ASSERT((dim2 > 0), "Malloc 3D float failed: dim2:%d\n",dim2);
	ASSERT((dim3 > 0), "Malloc 3D float failed: dim3:%d\n",dim3);
	ASSERT((dim4 > 0), "Malloc 3D float failed: dim4:%d\n",dim4);
	
	
	//dim3 = -1;
	
	
	MMALLOC(ptr_t, sizeof(float***) * (long int)dim1);
	MMALLOC(ptr_tt, sizeof(float**) * ((long int)dim1*(long int)dim2));
	MMALLOC(ptr_ttt, sizeof(float*) * ((long int)dim1 * (long int)dim2 * (long int)dim3));
	MMALLOC(ptr_tttt, sizeof(float) * ((long int)dim1 * (long int)dim2 * (long int)dim3 *(long int)dim4) + 5*sizeof(int) );
	
	int_ptr = (int*)ptr_tttt;
	int_ptr[0] = 4;
	int_ptr[1] = dim1;
	int_ptr[2] = dim2;
	int_ptr[3] = dim3;
	int_ptr[4] = dim4;
	ptr_tttt = (float*)(int_ptr + 5);
	
	for (i = 0; i < dim1; i++){
		ptr_t[i] = ptr_tt + dim2*i;
		for (j = 0; j < dim2; j++){
			ptr_t[i][j] = ptr_ttt + dim2*dim3*i + dim3*j;
			for(c =0 ; c < dim3;c++){
				ptr_t[i][j][c] =  ptr_tttt + dim2*dim3*dim4 * i + dim3*dim4*j  + dim4 *c;
				for(f =0 ; f < dim4;f++){
					ptr_t[i][j][c][f] = fill_value;
					fprintf(stdout,"%d %d %d %d: %f\n",i,j,c,f,fill_value );
				}
			}
		}
	}
	
	return ptr_t;
ERROR:
	MFREE(ptr_t );
	MFREE(ptr_tt);
	MFREE(ptr_ttt);
	MFREE(ptr_tttt);
	return NULL;
}



double** malloc_2d_double(double**m,int newdim1, int newdim2, double fill_value)
{
	int i,j;
	double** ptr_t = NULL;
	double* ptr_tt = NULL;
	
	int* int_ptr = NULL;
	int max1, max2;
	int olddim1,olddim2;
	
	ASSERT((newdim1 > 0), "Malloc 2D double failed: dim1:%d\n",newdim1);
	ASSERT((newdim2 > 0), "Malloc 2D double failed: dim2:%d\n",newdim2);
	
	
	

	
	if(m == NULL){
		MMALLOC(ptr_t, sizeof(double*) * newdim1);
		MMALLOC(ptr_tt, sizeof(double) * (newdim1*newdim2) + 3*sizeof(int) );
		
		int_ptr = (int*)ptr_tt;
		int_ptr[0] = 2;
		int_ptr[1] = newdim1;
		int_ptr[2] = newdim2;
		
		ptr_tt = (double*)(int_ptr + 3);
		
		for(i = 0;i< newdim1;i++){
			ptr_t[i] = ptr_tt + i * newdim2;
			for(j = 0; j < newdim2;j++){
				ptr_t[i][j] = fill_value;
			}
		}
		m = ptr_t;
		
	}else{
		ptr_t = m;
		int_ptr = (int*) m[0];
		int_ptr  = int_ptr -3;
		
		ptr_tt = (double* )int_ptr;
		
		olddim1 = *(int_ptr+1);
		olddim2 = *(int_ptr+2);
		//	DPRINTF3("%d-%d new: %d-%d", olddim1,olddim2, newdim1,newdim2 );
		
		
		
		
		/* in case we want a smaller matrix don't realloc but zero out "free mem"*/
		if(olddim1 >newdim1 || olddim2 > newdim2){
			max1 = (olddim1 > newdim1) ? olddim1:newdim1;
			max2 = (olddim2 > newdim2) ? olddim2:newdim2;
			for(i = 0; i <max1;i++){
				for(j = 0; j < max2;j++){
					if(i >= newdim1 || j >= newdim2){
						m[i][j] = fill_value;
					}
				}
			}
			if(olddim1 > newdim1){
				newdim1 = olddim1;
			}
			if(olddim1 >newdim1){
				newdim2 = olddim2;
			}
			
		}
		
		
		/* case 0: old == new*/
		if(olddim1 == newdim1 && olddim2 == newdim2){
			return m;
		}
		
		/*case 1 : both dimensions increase... */
		
		
		if(olddim1 < newdim1 && olddim2 < newdim2){
			MREALLOC(ptr_t,  sizeof(double*) * newdim1);
			MREALLOC(ptr_tt,  sizeof(double) * (newdim1*newdim2) + 3*sizeof(int) );
			int_ptr = (int*)ptr_tt;
			ptr_tt = (double*) (int_ptr + 3);
			for(i = olddim1-1; i >= 0;i-- ){
				for(j = olddim2-1;j >=0;j--){
					
					*(ptr_tt + i* newdim2 + j) =*(ptr_tt + i*olddim2 + j);
				}
				for(j = newdim2-1;j >= olddim2;j--){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
				
			}
			
			for(i = olddim1; i < newdim1;i++){
				for(j = 0; j < newdim2;j++){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
			}
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			
			
			int_ptr[0] = 2;
			int_ptr[1] = newdim1;
			int_ptr[2] = newdim2;
			m = ptr_t;
		}else if(olddim1 == newdim1 && olddim2 < newdim2){
			MREALLOC(ptr_t,  sizeof(double*) * newdim1);
			MREALLOC(ptr_tt,  sizeof(double) * (newdim1*newdim2) + 3*sizeof(int) );
			int_ptr = (int*)ptr_tt;
			ptr_tt = (double*) (int_ptr + 3);
			for(i = olddim1-1; i >= 0;i-- ){
				for(j = olddim2-1;j >=0;j--){
					
					*(ptr_tt + i* newdim2 + j) =*(ptr_tt + i*olddim2 + j);
				}
				for(j = newdim2-1;j >= olddim2;j--){
					*(ptr_tt + i* newdim2 + j) = fill_value;
				}
				
			}
			
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			
			
			int_ptr[0] = 2;
			int_ptr[1] = newdim1;
			int_ptr[2] = newdim2;
			m = ptr_t;
		}else if(olddim1 < newdim1 && olddim2 == newdim2){
			MREALLOC(ptr_t,  sizeof(double*) * newdim1);
			MREALLOC(ptr_tt,  sizeof(double) * (newdim1*newdim2) + 3*sizeof(int) );
			int_ptr = (int*)ptr_tt;
			ptr_tt = (double*) (int_ptr + 3);
			for(i = olddim1; i < newdim1;i++){
				for(j = 0; j < olddim2;j++){
					*(ptr_tt + i* olddim2 + j) = fill_value;
				}
			}
			for(i = olddim1;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * olddim2;
			}
			for(i = 0;i< newdim1;i++){
				ptr_t[i] = ptr_tt + i * newdim2;
			}
			int_ptr[0] = 2;
			int_ptr[1] = newdim1;
			int_ptr[2] = newdim2;
			m = ptr_t;
			
		}
	}
	return m;
ERROR:
	MFREE(ptr_t );
	MFREE(ptr_tt);
	
	return NULL;
}



void free_2d(void** m)
{
	int* ptr_tt  = (int*)m[0];
	ptr_tt = ptr_tt -3;
	//DPRINTF3("%d %d %d",*(ptr_tt+0),*(ptr_tt+1),*(ptr_tt+2));
	MFREE(ptr_tt);
	MFREE(m);
}

void free_3d(void*** m)
{
	int* ptr_ttt  = (int*)m[0][0];
	ptr_ttt = ptr_ttt -4;
	//DPRINTF3("%d %d %d %d",*(ptr_ttt+0),*(ptr_ttt+1),*(ptr_ttt+2),*(ptr_ttt+3));
	MFREE(ptr_ttt);
	MFREE(m[0]);
	MFREE(m);
}

void free_4d(void**** m)
{
	int* ptr_tttt  = (int*)m[0][0][0];
	ptr_tttt = ptr_tttt -5;
	//DPRINTF3("%d %d %d %d	%d",*(ptr_tttt+0),*(ptr_tttt+1),*(ptr_tttt+2),*(ptr_tttt+3),*(ptr_tttt+4));
	MFREE(ptr_tttt);
	MFREE(m[0][0]);
	MFREE(m[0]);
	MFREE(m);
}



uint16_t prob_to_uint16(float x)
{
	uint16_t a;
	
	x = scaledprob2prob(x);
	if(x > 1.0){
		x = 1.0;
	}
	if(x < 0.0){
		x = 0.0;
	}
	
	a = (uint16_t) roundf(x *UINT16_MAX);
	
	return a;
}

float uint16_to_prob(uint16_t a)
{
	float x;
	
	x = prob2scaledprob( (double)a  / UINT16_MAX);
	return x;
}



uint32_t prob_to_uint32(float x)
{
	uint32_t a;
	
	x = scaledprob2prob(x);
	if(x > 1.0){
		x = 1.0;
	}
	if(x < 0.0){
		x = 0.0;
	}
	
	a = (uint32_t) roundf(x *UINT32_MAX);
	
	return a;
}

float uint32_to_prob(uint32_t a)
{
	float x;
	
	x = prob2scaledprob( (double)a  / UINT32_MAX);
	return x;
}






void init_logsum()
{
	static int called = 0;
	int i;
	if(!called){
		called = 1;
		for(i = 0; i < LOGSUM_SIZE;i++){
			logsum_lookup[i] = log(1.0 +exp((double) -i / SCALE));
			//logsub_lookup[i]  = log(1.0 - exp((double) -i / SCALE));
		}
	}
}

float logsum(const float a,const float b)
{
	register const float max = MACRO_MAX(a, b);
	register const float min = MACRO_MIN(a, b);
	
	if(min == -INFINITY){
		return max;
	}
	if( (max-min) >= 15.7f){
		return max;
	}
	return  max+ logsum_lookup[(int)((max-min)*SCALE)];
}

float prob2scaledprob(float p)
{
	if(p == 0.0){
		return -INFINITY;
	}else{
		return  log(p);
	}
}


float scaledprob2prob(float p)
{
	if(p == -INFINITY){
		return 0.0;
	}else{
		return exp(p);
	}
}

float random_float_zero_to_x_thread(const float x, unsigned int* seed)
{
#ifdef HAVE_ARC4RANDOM
	return (float) arc4random() / (float) 0xFFFFFFFF *x; 
#else
	return (float) rand_r(seed) / (float) RAND_MAX *x; 
//ERROR:
//	WARNING_MSG("could not generate random number wirh random_r");
//	return FAIL;
#endif
}

uint32_t random_int_zero_to_x_thread(const uint32_t x, unsigned int* seed)
{

#ifdef HAVE_ARC4RANDOM_UNIFORM
	return arc4random_uniform(x+1);
#elif HAVE_ARC4RANDOM
	return arc4random() % ( x+1); 
#else
	return rand_r(seed) % (x+1); 
//ERROR:
//	WARNING_MSG("could not generate random number wirh random_r");
//	return FAIL;
#endif
}


float random_float_zero_to_x(const float x)
{
#ifdef HAVE_ARC4RANDOM
	return (float) arc4random() / (float) 0xFFFFFFFF *x; 
#else
	return (float) drand48();
#endif // HAVE_ARC4RANDOM
		
}

uint32_t random_int_zero_to_x(const uint32_t x)
{
#ifdef HAVE_ARC4RANDOM_UNIFORM
	return arc4random_uniform(x+1);
#elif HAVE_ARC4RANDOM
	return arc4random() % ( x+1); 
#else
	return rand() % (x+1);
#endif // HAVE_ARC4RANDOM
}


char* shorten_pathname(char* p)
{
	int i;
	char* tmp = p;
	int len = 0;
	len = (int) strlen(p);
	if(len){
		for(i = 0; i< len;i++){
			if(p[i] == '/'){
				tmp = p+i +1;
			}
		}
	}
	return tmp;
}

char* basename(const char* name)
{
	int i= 0;
	int c = 0;
	
	while(1){
		if(name[i] == '/'){
			c = i+1;
		}
		if(!name[i]){
			break;
		}
		i++;
		//c++;
	}
	return (char*)(name +c);
}



int replace_punctuation_with_underscore(char* p)
{
	int i;
	int c;
	int len = 0;
	len = (int) strlen(p);
	if(len){
		for(i = 0; i < len;i++){
			c = (int) p[i];
			if(ispunct(c)){
				p[i] = '_';
			}
		}
	}
	return OK;
}


#ifdef ITEST

int dummy_broken_func(int i);

int char_test(void);
int int_test(void);
int float_test(void);
int double_test(void);


int float_3d_test(void);

int float_4d_test(void);

int float_4d_test(void)
{
	float**** m = NULL;
	int i,j,c,f;
	int dim1,dim2,dim3,dim4;
	dim1 = 2;
	
	dim2 = 3;
	dim3 = 5;
	dim4 = 5;
	
	RUNP(m = malloc_4d_float(dim1,dim2,dim3,dim4,0.0f));
	//m = malloc_4d_float(dim1,dim2,dim3,dim4,0.0f);
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			for(c = 0; c < dim3;c++){
				for(f = 0; f < dim4;f++){
					m[i][j][c][f] =(float)i/1000.0 + (float)j/10.0+  (float)c +  (float)f;
				}
			}
		}
	}
	
	for(i =0;i < dim1;i++){
		fprintf(stdout,"LEVEL: %d\n",i);
		
		for(j = 0; j < dim2;j++){
			fprintf(stdout,"SUBLEVEL: %d\n",j);
			for(c = 0; c < dim3;c++){
				for(f = 0; f < dim4;f++){
					fprintf(stdout," %0.1f",m[i][j][c][f]);
				}
				fprintf(stdout,"\n");
			}
			fprintf(stdout,"\n");
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	
	free_4d((void****)m);
	return OK;
ERROR:
	return FAIL;
}


int float_3d_test(void)
{
	float*** m = NULL;
	int i,j,c;
	int dim1,dim2,dim3;
	dim1 = 3;
	
	dim2 = 5;
	dim3 = 5;
	
	RUNP(m = malloc_3d_float(dim1,dim2,dim3,0.0f));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			for(c = 0; c < dim3;c++){
				m[i][j][c] = (float)i/10.0+  (float)j +  (float)c;
			}
		}
	}
	
	for(i =0;i < dim1;i++){
		fprintf(stdout,"LEVEL: %d\n",i);
		
		for(j = 0; j < dim2;j++){
			for(c = 0; c < dim3;c++){
				fprintf(stdout," %0.1f",m[i][j][c]);
			}
			fprintf(stdout,"\n");
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	
	free_3d((void***)m);
	return OK;
ERROR:
	return FAIL;
}

int char_test(void)
{
	int dim1,dim2,i,j;
	char** m = NULL;
	
	DPRINTF1("Testing char 2D");
	
	dim1 = 5;
	dim2 = 5;
	
	RUNP(m = malloc_2d_char(m,dim1,dim2,0));
	//DPRINTF1("H2 char 2D\n");

	if(!m){
		ERROR_MSG("malloc_2d_char failed");
	}
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			m[i][j] = i+j + 65;
		}
	}
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			if(m[i][j] == 0){
				fprintf(stdout," %d",m[i][j]);
			}else{
				fprintf(stdout," %c",m[i][j]);
			}
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	dim1 += 1;
	dim2 += 1;
	RUNP(m = malloc_2d_char(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			if(m[i][j] == 0){
				fprintf(stdout," %d",m[i][j]);
			}else{
				fprintf(stdout," %c",m[i][j]);
			}
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	dim1 += 1;
	dim2 += 0;
	RUNP(m = malloc_2d_char(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			if(m[i][j] == 0){
				fprintf(stdout," %d",m[i][j]);
			}else{
				fprintf(stdout," %c",m[i][j]);
			}
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 += 0;
	dim2 += 1;
	RUNP(m = malloc_2d_char(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			if(m[i][j] == 0){
				fprintf(stdout," %d",m[i][j]);
			}else{
				fprintf(stdout," %c",m[i][j]);
			}
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 = 3;
	dim2 = 3;
	RUNP(m = malloc_2d_char(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			if(m[i][j] == 0){
				fprintf(stdout," %d",m[i][j]);
			}else{
				fprintf(stdout," %c",m[i][j]);
			}
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	
	dim1 = 10;
	dim2 = 10;
	RUNP(m = malloc_2d_char(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			if(m[i][j] == 0){
				fprintf(stdout," %d",m[i][j]);
			}else{
				fprintf(stdout," %c",m[i][j]);
			}
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	free_2d((void**) m);
	return OK;
ERROR:
	return FAIL;
}


int int_test(void)
{
	int dim1,dim2,i,j;
	int** m = NULL;
	
	fprintf(stdout,"Testing int 2D\n");
	
	dim1 = 5;
	dim2 = 5;
	RUNP(m = malloc_2d_int(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			m[i][j] = i+j;
		}
	}
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %d",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	dim1 += 1;
	dim2 += 1;
	RUNP(m = malloc_2d_int(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %d",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	dim1 += 1;
	dim2 += 0;
	RUNP(m = malloc_2d_int(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %d",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 += 0;
	dim2 += 1;
	RUNP(m = malloc_2d_int(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %d",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	
	dim1 = 3;
	dim2 = 3;
	RUNP(m = malloc_2d_int(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %d",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 = 10;
	dim2 = 10;
	RUNP(m = malloc_2d_int(m,dim1,dim2,0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %d",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	free_2d((void**) m);
	return OK;
ERROR:
	return FAIL;
}


int float_test(void)
{
	int dim1,dim2,i,j;
	float** m = NULL;
	
	DPRINTF1("Testing float 2D");
	
	dim1 = 5;
	dim2 = 5;
	RUNP(m = malloc_2d_float(m,dim1,dim2,0.0f));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			m[i][j] = i+j + 0.1;
		}
	}
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			
			fprintf(stdout," %0.1f",m[i][j]);
			
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	dim1 += 1;
	dim2 += 1;
	RUNP(m = malloc_2d_float(m,dim1,dim2,0.0f));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	dim1 += 1;
	dim2 += 0;
	RUNP(m = malloc_2d_float(m,dim1,dim2,0.0f));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 += 0;
	dim2 += 1;
	RUNP(m = malloc_2d_float(m,dim1,dim2,0.0f));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 = 3;
	dim2 = 3;
	RUNP(m = malloc_2d_float(m,dim1,dim2,0.0f));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 = 10;
	dim2 = 10;
	RUNP(m = malloc_2d_float(m,dim1,dim2,0.0f));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	free_2d((void**) m);
	return OK;
ERROR:
	return FAIL;
}


int double_test(void)
{
	int dim1,dim2,i,j;
	double** m = NULL;
	
	DPRINTF1("Testing double 2D");
	
	dim1 = 5;
	dim2 = 5;
	
	RUNP(m = malloc_2d_double(m,dim1,dim2,0.0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			m[i][j] = i+j + 0.1;
		}
	}
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			
			fprintf(stdout," %0.1f",m[i][j]);
			
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	dim1 += 1;
	dim2 += 1;
	RUNP(m = malloc_2d_double(m,dim1,dim2,0.0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	dim1 += 1;
	dim2 += 0;
	RUNP(m = malloc_2d_double(m,dim1,dim2,0.0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 += 0;
	dim2 += 1;
	RUNP(m = malloc_2d_double(m,dim1,dim2,0.0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 = 3;
	dim2 = 3;
	RUNP(m = malloc_2d_double(m,dim1,dim2,0.0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	dim1 = 10;
	dim2 = 10;
	RUNP(m = malloc_2d_double(m,dim1,dim2,0.0));
	for(i =0;i < dim1;i++){
		for(j = 0; j < dim2;j++){
			fprintf(stdout," %0.1f",m[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"\n");
	
	
	free_2d((void**) m);
	return OK;
ERROR:
	return FAIL;
}

struct char_struct{
	int len;
	char* seq;
};


int main (int argc,char * const argv[])
{
	int* p = NULL;
	MFREE(p);
	MFREE(p);
	char* my_str_seq  = NULL;
	struct char_struct* my_str = NULL;
	
	tlog.echo_build_config();
	
	fprintf(stdout,"Testing logging functions\n");
	LOG_MSG("Testing %s","the here and now.");
	tlog.log_message("TESTING");
	tlog.error(AT,"test error %d in stderr %s", 1,"this should go to stderr" );
	tlog.message(AT, "Here is a message %d", 100);
	tlog.warning(AT, "Oh dear I have a warning: %s","big warning to stdout");
	
	tlog.set_logfile("testlog.txt");
	tlog.echo_build_config() ;
	tlog.set_logfile("00");
	
	tlog.message(AT, "Here is a message %d", 666);
	tlog.log_message("TESTING TEE");

	tlog.error(AT,"test error %d in stderr %s", 2,"this should be tee'd" );
	//tlog.error(AT,"test TEE ERROR " );
	
	
	tlog.warning(AT, "Oh dear I have a warning: %s","big warning to be tee'd");
	
	tlog.set_logfile(NULL);
	
	tlog.warning(AT, "Oh dear I have a warning (not to be written to log...) : %s","big warning to be tee'd");
	ASSERT(1 == 1,"Of dear 1 in NOT equal to %d", 1);
	
	//exit(0);
	
	fprintf(stdout,"%'lld\n",MAX_MEMORY_LIMIT);
	fprintf(stdout,"Running libks sanity tests\n");
	
	
	//ASSERT((1== 1),"1 == 1");
	//ASSERT((1== 0),"one is not equal to 0");
	
	//RUN(dummy_broken_func(10));
	RUN(char_test() );
	char_test();
	int_test();
	float_test();
	double_test();
	
	float_3d_test();
	float_4d_test();
	
	p= NULL;
	
	MCALLOC(p, 10,int);
	

	
	//MMALLOC(p, sizeof(int) * 10);
	MFREE(p);
	
	
	MCALLOC(my_str, 1,struct char_struct);

	fprintf(stderr,"%d %s\n",	my_str->len,my_str->seq);
	
	MMALLOC(my_str_seq, sizeof(char) * 10);
	my_str->seq =my_str_seq;
	
	log_message("All is good");
	MFREE(my_str->seq);
	MFREE(my_str);

	log_message("Yes %s is ","it");
	int i;
	for(i = 0; i < 100;i++){
		fprintf(stdout,"%d %d %f %f\n", random_int_zero_to_x(10), random_int_zero_to_x(10),random_float_zero_to_x(1.0), random_float_zero_to_x(1.0));
	}

	
	return EXIT_SUCCESS;
ERROR:
	MFREE(my_str_seq);
	MFREE(my_str);
	MFREE(p);
	return EXIT_FAILURE;
}

int dummy_broken_func(int i)
{
	return FAIL;
}

#endif

tlog_namespace  tlog = { log_message,message,warning,error,unformatted,echo_build_config,print_program_description,set_logfile };

