#ifndef TLMISC_H
#define TLMISC_H



#ifdef TLMISC_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int my_file_exists(const char* name);
EXTERN int make_cmd_line(char** command, const int argc,char* const argv[]);
#undef TLMISC_IMPORT
#undef EXTERN

#endif
