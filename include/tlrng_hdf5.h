#ifndef TLRNG_HDF5_H
#define TLRNG_HDF5_H

#ifdef TLRNG_HDF5_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif

struct hdf5_data;
struct rng_state;
EXTERN int tl_random_write_hdf5(struct rng_state* rng,struct hdf5_data* d, char* file, char* group);
EXTERN int tl_random_read_hdf5(struct rng_state** rng,struct hdf5_data* d, char* file, char* group);


#undef TLRNG_HDF5_IMPORT
#undef EXTERN


#endif
