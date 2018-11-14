#ifndef TL_LOG_H
#define TL_LOG_H

#include "tl_global.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>




#define MESSAGE_MARGIN 22
#define TYPE_MARGIN 8

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

#define UNFORMAT_MSG(...) do {                    \
                unformatted( __VA_ARGS__ );	\
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

extern int log_command_line(const int argc,char* const argv[]);
extern char* make_cmd_line(const int argc,char* const argv[]);

//extern void log_message( const char *format, ...);
//extern void message(const char *location, const char *format, ...);
//extern void warning(const char *location, const char *format, ...);
//extern void error(const char *location, const char *format, ...);


typedef struct {
        void (*log_message )(const char *format, ...);
        void (*message)(const char *location, const char *format, ...);
        void (*warning)(const char *location, const char *format, ...);
        void (*error)(const char *location, const char *format, ...);
        void (*unformatted) ( const char *format, ...);
        void (*echo_build_config) (void);
        void (*print_program_description) (char *const argv[],const char* description);
        int (*  set_logfile)(char* logfilename);
} tlog_namespace;

extern tlog_namespace  tlog;

extern char build_config[];

#endif
