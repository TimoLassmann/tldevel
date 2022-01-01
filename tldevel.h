#ifndef TLDEVEL_H
#define TLDEVEL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <float.h>


#include "tlalloc.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#include "CmakeConfig.h"
#endif



#ifdef TLDEVEL_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif

#define TLDEVEL_VERSION PACKAGE_VERSION

#define OK              0
#define FAIL            1

#define MESSAGE_MARGIN 22

#define MACRO_MIN(a,b)          (((a)<(b))?(a):(b))
#define MACRO_MAX(a,b)          (((a)>(b))?(a):(b))

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ " line " TOSTRING(__LINE__)

#define ERROR_MSG(...) do {                     \
                error(AT, __VA_ARGS__ );        \
                goto ERROR;                     \
        }while (0)

#define WARNING_MSG(...) do {                   \
                warning(AT, __VA_ARGS__ );      \
        }while (0)


#define LOG_MSG(...) do {                       \
                log_message( __VA_ARGS__ );     \
        }while (0)

#define ASSERT(TEST,...)  if(!(TEST)) {         \
                error(AT,#TEST );               \
                error(AT, __VA_ARGS__);         \
                goto ERROR;                     \
        }

#if (DEBUGLEVEL >= 1)
#define DASSERT(TEST,...) if(!(TEST)) {         \
                error(AT,#TEST );               \
                error(AT, __VA_ARGS__);         \
                goto ERROR;                     \
        }
#else
#define DASSERT(TEST,...)

#endif


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


EXTERN int nearly_equal_float(float a, float b);
EXTERN int nearly_equal_double(double a, double b);


#define TLSAFE_EQ(X,Y) _Generic((X),                        \
                                float: nearly_equal_float,  \
                                double: nearly_equal_double \
                )(X,Y)

EXTERN void error(const char *location, const char *format, ...);
EXTERN void warning(const char *location, const char *format, ...);
EXTERN void log_message( const char *format, ...);



EXTERN const char* tldevel_version(void);

#undef TLDEVEL_IMPORT
#undef EXTERN
#endif
