#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>



#define TLDEVEL_IMPORT
#include "tldevel.h"


#define TYPE_MARGIN 8

static void verror(FILE* f_ptr, const char *location, const char *format,  va_list argp);
static void vwarning(FILE* f_ptr,const char *location, const char *format,  va_list argp);
static void vlog(FILE* f_ptr,const char *format,  va_list argp);
static int get_time(char* time_ptr, int size);


const char* tldevel_version(void)
{
        return TLDEVEL_VERSION;
}

void error(const char *location, const char *format, ...)
{
        va_list argp;
        va_start(argp, format);
        verror(stderr,location,format,argp);
        va_end(argp);
}

void warning(const char *location, const char *format, ...)
{
        va_list argp;
        va_start(argp, format);
        vwarning(stdout,location, format, argp);
        va_end(argp);
}

void log_message( const char *format, ...)
{
        va_list argp;
        va_start(argp, format);
        vlog(stdout,format, argp);
        va_end(argp);
}

int get_time(char* time_ptr, int size)
{
        /* struct tm *ptr; */
        struct tm local_time;
        time_t current = time(NULL);
        //ptr = localtime(&current);
        //if(!strftime(time_ptr, size, "[%F %H:%M:%S] ", ptr))ERROR_MSG("write failed");

        if((localtime(&current,&local_time)) == NULL){
                ERROR_MSG("could not get local time");
        }

        if(!strftime(time_ptr, size, "[%F %H:%M:%S] ", &local_time))ERROR_MSG("write failed");
        return OK;
ERROR:
        return FAIL;
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


int nearly_equal_float(float a, float b)
{
        float absa = fabsf(a);
        float absb = fabsf(b);
        float d = fabsf(a-b);
        if(a == b){
                return 1;
        }else if (a == 0.0f || b == 0 || (absa + absb < FLT_MIN)){
                return d < (FLT_EPSILON * FLT_MIN);
        }else{
                return d / MACRO_MIN((absa+absb), FLT_MIN) < FLT_EPSILON;
        }
}

int nearly_equal_double(double a, double b)
{
        double absa = fabs(a);
        double absb = fabs(b);
        double d = fabs(a-b);
        if(a == b){
                return 1;
        }else if (a == 0.0f || b == 0 || (absa + absb < DBL_MIN)){
                return d < (DBL_EPSILON * DBL_MIN);
        }else{
                return d / MACRO_MIN((absa+absb), DBL_MIN) < DBL_EPSILON;
        }
}
