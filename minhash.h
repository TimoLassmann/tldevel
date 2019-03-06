#ifndef MINHASH_H
#define MINHASH_H



#include "tldevel.h"
#include <limits.h>

struct Boolean_matrix{
        uint32_t** m;
        int n_row;
        int n_column;
};


struct minhash{
        unsigned int** sig;
        unsigned int* a;
        unsigned int* b;
        int n_signatures;
        int n_columns;
};

/* allocate and free Boolean matrix */
extern struct Boolean_matrix* init_Bmatrix( int columns,int rows);
extern void free_Boolean_matrix(struct Boolean_matrix* bm);

/* Create minhash from Boolean matrix and free */
struct minhash* create_min_hash(struct Boolean_matrix* bm, int num_sig, long int seed);
void free_minhash(struct minhash* min_h);

/* calculate jaccard index for set S in based on the minhash and the probability of finding set S in n samples  */

int jaccard_sim_min_multihash(struct minhash* min_h , int* S, int n,int num_samples, double* jac_sim, double *p_S_in_X);
#endif
