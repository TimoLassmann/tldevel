#ifndef KSA_H
#define KSA_H

#include <stdint.h>

typedef int64_t saint_t;
#define SAINT_MAX INT64_MAX
#define SAIS_CORE ksa_core64
#define SAIS_BWT  ksa_bwt64
#define SAIS_MAIN ksa_sa64

#ifdef KSA_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif



EXTERN int SAIS_MAIN(const unsigned char *T, saint_t *SA, saint_t n, int k);

#endif
