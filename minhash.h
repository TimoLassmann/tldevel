#ifndef MINHASH_H
#define MINHASH_H



#include "tldevel.h"
#include <limits.h>

#include "tlbitvec.h"
#include <stdint.h>

struct Boolean_matrix{

        struct bitvec** m;
        int n_row;
        int n_column;
};


struct minhash{
        unsigned int** sig;
        //unsigned int* a;
        //unsigned int* b;
        int n_signatures;
        int n_columns;
        int n_samples;

};

/* allocate and free Boolean matrix */
extern struct Boolean_matrix* init_Bmatrix( int columns,int rows);
extern void free_Boolean_matrix(struct Boolean_matrix* bm);

/* Create minhash from Boolean matrix and free */
struct minhash* create_min_hash(struct Boolean_matrix* bm, int num_sig, long int seed);
void free_minhash(struct minhash* min_h);

/* calculate jaccard index for set S in based on the minhash and the probability of finding set S in n samples  */

int jaccard_sim_min_multihash(struct minhash* min_h , int* S, int n,double* jac_sim, double *p_S_in_X);
#endif
