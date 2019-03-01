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
        int** sig;
        int* a;
        int* b;
        int n_signatures;
        int n_columns;
};

extern struct Boolean_matrix* init_Bmatrix( int columns,int rows);
struct minhash* create_min_hash(struct Boolean_matrix* bm, int num_sig,struct drand48_data* rd);
void free_minhash(struct minhash* min_h);


void free_Boolean_matrix(struct Boolean_matrix* bm);


int jaccard_sim(struct Boolean_matrix* bm, int*S , int n, double* jac_sim);
int jaccard_sim_min_hash(struct minhash* min_h , int a, int b, double* jac_sim, double *avg_min_sig_diff);
int jaccard_sim_min_multihash(struct minhash* min_h , int* S, int n, double* jac_sim, double *avg_min_sig_diff);
#endif
