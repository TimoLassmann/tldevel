#ifndef TLBITVEC_H
#define TLBITVEC_H


#ifdef TLBITVEC_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif

struct bitvec;


EXTERN int make_bitvector(struct bitvec** bv, int num_elem);
EXTERN int clear_bitvector(struct bitvec* bv);
EXTERN int bit_set(struct bitvec* bv, int i);
EXTERN int bit_clr(struct bitvec* bv, int i);
EXTERN int bit_test(struct bitvec* bv, int i, int* ret);
EXTERN int free_bitvector(struct bitvec** bv);

#undef TLBITVEC_IMPORT
#undef EXTERN


#endif
