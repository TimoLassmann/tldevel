
#ifndef TLALPHABET_H
#define TLALPHABET_H





#ifdef TLALPHABET_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif

#define TLALPHABET_DEFAULT_PROTEIN 1
#define TLALPHABET_DEFAULT_DNA 2
#define TLALPHABET_REDUCED_PROTEIN 3

#define TLALPHABET_NOAMBIGIOUS_PROTEIN 4
#define TLALPHABET_NOAMBIGUOUS_DNA 5


#include <stdint.h>

struct alphabet;

struct rng_state;

EXTERN int create_alphabet(struct alphabet** alphabet, struct rng_state* rng,int type);

EXTERN uint8_t tlalphabet_get_code (const struct alphabet* a,const char c);
EXTERN int convert_to_internal(struct alphabet* a, uint8_t* seq, int len);
//EXTERN int convert_to_external(struct alphabet* a, uint8_t* seq, int len);

EXTERN void free_alphabet(struct alphabet* a);

#undef TLALPHABET_IMPORT
#undef EXTERN
#endif
