#ifndef TLRNG_H
#define TLRNG_H

#include "stdint.h"
#ifdef HAVE_HDF5
#include "tlhdf5wrap.h"
#endif


#ifdef TLRNG_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif


typedef struct rng_state rng_state;
struct rng_dist;

EXTERN struct rng_state* init_rng(uint64_t seed);
EXTERN struct rng_state* init_rng_from_rng(struct rng_state* rng);


EXTERN int tl_random_sdist_init(struct rng_dist** rng_dist,double* w, int n,int seed);
EXTERN int tl_random_sdist_smpl(struct rng_dist* d);
EXTERN void tl_random_sdist_free(struct rng_dist* d);

EXTERN void free_rng(struct rng_state* rng);

EXTERN double tl_random_double(struct rng_state* rng);
EXTERN double tl_random_gaussian(struct rng_state* rng, double mu, double sigma);
EXTERN double tl_random_gamma(struct rng_state* rng, double shape, double scale);
EXTERN int tl_random_int(struct rng_state* rng,int a);

#ifdef HAVE_HDF5

EXTERN int tl_random_write_hdf5(struct rng_state* rng,struct hdf5_data* d, char* file, char* group);
EXTERN int tl_random_read_hdf5(struct rng_state** rng,struct hdf5_data* d, char* file, char* group);

#endif

#undef TLRNG_IMPORT
#undef EXTERN
#endif
