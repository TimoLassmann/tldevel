#include "tl_log.h"


static char logfile[BUFFER_LEN] = "default.log";

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


int set_logfile(char* logfilename)
{
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

        c = 0;
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
        va_start(argp, format);
        vlog(stdout,format, argp);
        va_end(argp);
}


void log_message_tee( const char *format, ...)
{
        va_list argp;
        va_start(argp, format);
        vlog(stdout,format, argp);
        va_end(argp);
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
}


void unformatted( const char *format, ...)
{
        va_list argp;
        va_start(argp, format);
        vunformat(stdout,format,argp);
        va_end(argp);

}

void unformatted_tee( const char *format, ...)
{
        va_list argp;

        va_start(argp, format);
        vunformat(stdout,format,argp);
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
        if(file){
                fclose(file);
        }
}

void message(const char *location, const char *format, ...)
{
        va_list argp;

        va_start(argp, format);
        vmessage(stdout,location,format,argp);
        va_end(argp);
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

tlog_namespace  tlog = { log_message,message,warning,error,unformatted,echo_build_config,print_program_description,set_logfile };
