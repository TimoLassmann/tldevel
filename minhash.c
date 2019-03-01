#include "minhash.h"




static struct Boolean_matrix* init_random_Bmatrix(int columns,int rows,  double alpha,struct drand48_data *rd);
static int print_Boolean_matrix(struct Boolean_matrix* bm);
static int print_minhash_signatures(struct minhash* min_h);
static int shuffle_arr(int* arr,int n);

int main (int argc,char * const argv[])
{
        fprintf(stdout,"Hello world\n");

        struct Boolean_matrix* bm = NULL;
        struct minhash* min_h = NULL;
        double sim;
        double sim_min;
        int num_samples = 1000;
        int i,j;
        int trials = 100;
        double s1 = 0.0;
        double s2 = 0.0;
        double s1_p = 0.0;
        double s2_p= 0.0;

        double diff;
        double diff_p;
        double p_S_in_X;
        double alpha = 0.999;
        double min_avg_sig_diff;
        int num_hash_functions = 100;
        int iter;
        struct drand48_data randBuffer;
        int* index =NULL;
        int S_size = 30;

        MMALLOC(index, sizeof(int) * S_size);
        for(i = 0; i < S_size;i++){
                index[i] = i;
        }


        srand48_r(time(NULL), &randBuffer);

        for(iter = 0; iter < trials;iter++){
                /* S_size is just for simulation - this should be the number of variables... */
                RUNP(bm = init_random_Bmatrix(S_size,num_samples,alpha, &randBuffer));
                //RUN(print_Boolean_matrix(bm));
                min_h = create_min_hash(bm, num_hash_functions, &randBuffer);
                RUN(jaccard_sim(bm,index, S_size, &sim));
                jaccard_sim_min_multihash(min_h, index, S_size, &sim_min,&min_avg_sig_diff);


                p_S_in_X = sim_min* (((double)(num_samples+1) /(double) num_samples) *(  1.0 / min_avg_sig_diff) -(1.0/(double)(num_samples +1)));
                diff = fabs(sim-sim_min);
                s1 += diff;
                s2 += diff * diff;
                diff_p = fabs(p_S_in_X-  pow(alpha,(double)S_size));
                s1_p += diff_p;
                s2_p += diff_p * diff_p;
                fprintf(stdout,"%f %f delta: %f\t",sim,sim_min, diff);
                fprintf(stdout,"P seeing: %f (%f) delta: %f\n",p_S_in_X,   pow(alpha, (double)S_size),diff_p);


                //RUN(print_Boolean_matrix(bm));
                //RUN(print_minhash_signatures(min_h));
                /*for(i = 0; i < 2;i++){
                  for(j = i+1; j < 2;j++){
                  RUN(jaccard_sim(bm, i, j, &sim));
                  RUN(jaccard_sim_min_hash(min_h, i, j, &sim_min,&min_avg_sig_diff));
                  diff = fabs(sim-sim_min);
                  fprintf(stdout,"%d %d: %f %f delta: %f\n",i,j,sim,sim_min, diff);

                  // P(1 & 2 in data)
                  fprintf(stdout,"P seeing %d and %d: %f (%f)\n", i,j,sim_min* ((double)(num_samples+1) /(double) num_samples *(  1.0 / min_avg_sig_diff) -1.0/(double)(num_samples+1)), alpha * alpha);

                  s1 += diff;
                  s2 += diff * diff;
                  jaccard_sim_min_multihash(min_h, index, 2, &sim_min,&min_avg_sig_diff);
                  diff = fabs(sim-sim_min);
                  fprintf(stdout,"%d %d: %f %f delta: %f\n",i,j,sim,sim_min, diff);


                  }
                  }*/


                free_minhash(min_h);
                free_Boolean_matrix(bm);
        }

        s2 = sqrt(((double) trials * s2 - s1 * s1)/ ((double) trials * ((double) trials -1.0)));
        s1 = s1 / (double) trials;
        fprintf(stdout,"mean: %f stdev:%f\n", s1,s2);

s2_p = sqrt(((double) trials * s2_p - s1_p * s1_p)/ ((double) trials * ((double) trials -1.0)));
        s1_p = s1_p / (double) trials;
        fprintf(stdout,"mean: %f stdev:%f\n", s1_p,s2_p);

        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;
}



struct minhash* create_min_hash(struct Boolean_matrix* bm, int num_sig,struct drand48_data* rd)
{
        struct minhash* min_h = NULL;
        uint32_t* col = NULL;
        int* list = NULL;
        int i,j,c;
        int n,m;
        //int hash_val;
        long int r = 0;
        ASSERT(bm!= NULL, "No matrix");



        MMALLOC(min_h, sizeof(struct minhash));
        min_h->sig = NULL;
        min_h->a = NULL;
        min_h->b = NULL;


        min_h->n_signatures = num_sig;
        min_h->n_columns = bm->n_column;

        MMALLOC( min_h->sig, sizeof(uint32_t*) * bm->n_column);

        for(i = 0; i < min_h->n_columns;i++){
                min_h->sig[i] = NULL;
                MMALLOC(min_h->sig[i],sizeof(uint32_t) * min_h->n_signatures);
                for(j = 0; j < min_h->n_signatures;j++){
                        min_h->sig[i][j] = INT_MAX;
                }
        }
        MMALLOC(min_h->a, sizeof(uint32_t)* min_h->n_signatures);
        MMALLOC(min_h->b, sizeof(uint32_t)* min_h->n_signatures);



        m = bm->n_column;
        n = bm->n_row;

        //fprintf(stdout,"%d columns\n", m);
        MMALLOC(list,sizeof(int)* n);
        for(i = 0; i < n;i++){
                list[i] = i;
        }

        /*for(i = 0; i < min_h->n_signatures;i++){
               RUN(lrand48_r(rd, &r));



               min_h->a[i] = r ;// (uint32_t) lrand48() % (n-1);
                RUN(lrand48_r(rd, &r));
                min_h->b[i] = r;//(uint32_t) lrand48() % (n-1);
                }*/

        //for(i = 0; i < 10;i++){
        //        fprintf(stdout,"%d %d\n", min_h->a[i],min_h->b[i]);

        //}
        //exit(0);
        /* Apply has functions  */
        for(c = 0;c < min_h->n_signatures;c++){

                shuffle_arr(list, n);
                for(i = 0; i < m;i++){
                        col = bm->m[i];
                        for(j = 0; j < n;j++){
                                if(bit_test(col,list[j])){
                                        if(j < min_h->sig[i][c]){
                                                min_h->sig[i][c] = j+1;
                                        }
                                }
                        }
                }
        }
        MFREE(list);

        /*for(i =0; i < m;i++){
                col = bm->m[i];
                fprintf(stdout,"COLUMN:%d\n",i);
                for(j = 0; j < n;j++){

                        //c = 1;
                        //               hash_val = (min_h->a[c] * j + min_h->b[c]) % n ;
                        //         fprintf(stdout,"HASH:%d %d %d %d\n",i,j,c,hash_val);
                        //fprintf(stdout,"%d",bit_test(col, j));

                        fprintf(stdout,"item: %d ", bit_test(col, j));
                        for(c = 0; c < min_h->n_signatures;c++){
                                hash_val = (min_h->a[c] * (j+1) + min_h->b[c])  % n;//4294967311;
                                fprintf(stdout,"%d ",hash_val);
                                if(bit_test(col, j)){
                                        //fprintf(stdout,"comparing: %d %d\n",hash_val , min_h->sig[i][c]);
                                        if(hash_val <  min_h->sig[i][c]){
                                                min_h->sig[i][c] = hash_val;
                                        }
                                }
                        }
                        fprintf(stdout,"\n");

                }
                //fprintf(stdout,"\t");
                //for(i = 0; i < min_h->n_columns;i++){

                //        for(c = 0; c < min_h->n_signatures;c++){
                                //      fprintf(stdout,"%d ", min_h->sig[i][c]);
                //        }
                        // fprintf(stdout,"\t");
                //}
                //   fprintf(stdout,"\n");

                }*/

        //for(i = 0; i < min_h->n_columns;i++){

        //        for(j = 0; j < min_h->n_signatures;j++){
                        //       fprintf(stdout,"%d ", min_h->sig[i][j]);
        //        }
                //fprintf(stdout,"\n");
        //}

        return min_h;
ERROR:
        return NULL;
}



int jaccard_sim_min_multihash(struct minhash* min_h , int* S, int n, double* jac_sim, double *avg_min_sig_diff)
{
        int i,j;
        int** m = NULL;
        double set_intersection = 0.0;
        double c,min;


        double min_stuff = 0.0;
        ASSERT(min_h != NULL, "No minhash");
        m = min_h->sig;

        for(i = 0; i < min_h->n_signatures;i++){
                min =  m[S[0]][i];
                c = 1.0;
                for(j = 1; j < n;j++){
                        min = MACRO_MIN(min, m[S[j]][i]);
                        if(m[S[j]][i] != m[S[0]][i]){
                                c =0.0;
                                break;
                        }
                }

                set_intersection += c;

                //fprintf(stdout,"%d %d : %f\n",col_a[i],col_b[i],(double)MACRO_MIN(col_a[i],col_b[i]));
                min_stuff += min;
        }
        *avg_min_sig_diff =        (min_stuff /= (double)  min_h->n_signatures);
        //fprintf(stdout,"%d %d %f %f\n",a,b,set_intersection,  *avg_min_sig_diff);

        *jac_sim = set_intersection / (double) (min_h->n_signatures);

        return OK;
ERROR:
        return FAIL;
}


int jaccard_sim_min_hash(struct minhash* min_h , int a, int b, double* jac_sim, double *avg_min_sig_diff)
{
        int i;
        double set_intersection = 0.0;


        int* col_a = NULL;
        int* col_b = NULL;
        double min_stuff = 0.0;
        ASSERT(min_h != NULL, "No minhash");
        col_a = min_h->sig[a];
        col_b = min_h->sig[b];
        for(i = 0; i < min_h->n_signatures;i++){
                if(col_a[i] == col_b[i]){
                        set_intersection += 1.0;
                }
                //fprintf(stdout,"%d %d : %f\n",col_a[i],col_b[i],(double)MACRO_MIN(col_a[i],col_b[i]));
                min_stuff += (double)MACRO_MIN(col_a[i],col_b[i]);
        }
        *avg_min_sig_diff =        (min_stuff /= (double)  min_h->n_signatures);
        //fprintf(stdout,"%d %d %f %f\n",a,b,set_intersection,  *avg_min_sig_diff);

        *jac_sim = set_intersection / (double) (min_h->n_signatures);

        return OK;
ERROR:
        return FAIL;
}



int jaccard_sim(struct Boolean_matrix* bm, int*S , int n, double* jac_sim)
{
        int i,j,c;
        int n_row;
        double set_intersection = 0.0;
        double set_union = 0.0;

        ASSERT(bm!= NULL, "No matrix");
        n_row = bm->n_row;

        for(i = 0; i < n_row;i++){
                c = 0;

                for(j = 0; j < n;j++){
                        c+= bit_test(bm->m[S[j]],i);
                }
                if(c == n){
                        set_intersection += 1.0;
                }
                if(c){
                        set_union += 1.0;
                }
        }
        *jac_sim = 0.0;

        if(set_union){
                *jac_sim = set_intersection / set_union;
        }
        return OK;
ERROR:
        return FAIL;
}

struct Boolean_matrix* init_Bmatrix( int columns,int rows)
{
        struct Boolean_matrix* bm = NULL;
        int i;

        //long int seed = 0;

        ASSERT(rows > 0, "No rows.");
        ASSERT(columns > 0, "No columns.");


        //seed  =  (long int) (time(NULL) * (42));

        //srand48(seed);

        MMALLOC(bm, sizeof(struct Boolean_matrix));
        bm->m = NULL;
        bm->n_column = columns;
        bm->n_row = rows;

        MMALLOC(bm->m, sizeof(uint32_t*) * bm->n_column);

        for(i = 0; i < bm->n_column;i++){
                bm->m[i] = NULL;
                RUNP(bm->m[i] = make_bitvector(bm->n_row));
        }
        return bm;
ERROR:
        return NULL;
}


struct Boolean_matrix* init_random_Bmatrix( int columns,int rows, double alpha,struct drand48_data* rd)
{
        struct Boolean_matrix* bm = NULL;
        double  r;
        int i,j;

        //long int seed = 0;

        ASSERT(rows > 0, "No rows.");
        ASSERT(columns > 0, "No columns.");
        ASSERT(alpha > 0.0, "No alpha - the matrix will be empty.");


        //seed  =  (long int) (time(NULL) * (42));

        //srand48(seed);

        MMALLOC(bm, sizeof(struct Boolean_matrix));
        bm->m = NULL;
        bm->n_column = columns;
        bm->n_row = rows;

        MMALLOC(bm->m, sizeof(uint32_t*) * bm->n_column);

        for(i = 0; i < bm->n_column;i++){
                bm->m[i] = NULL;
                RUNP(bm->m[i] = make_bitvector(bm->n_row));
        }


        for(i = 0; i < bm->n_column;i++){
                for(j = 0; j < bm->n_row;j++){
                        //r = drand48();
                        RUN(drand48_r(rd, &r));
                        if(r <= alpha){
                                bit_set(bm->m[i], j);
                        }
                }
        }

        return bm;
ERROR:
        return NULL;
}

int print_Boolean_matrix(struct Boolean_matrix* bm)
{
        int i,j;
        for(i = 0; i < bm->n_column;i++){
                for(j = 0; j < bm->n_row;j++){
                        fprintf(stdout,"%d",bit_test(bm->m[i], j));
                }
                fprintf(stdout,"\n");
        }
        fprintf(stdout,"\n");
        return OK;
}

int print_minhash_signatures(struct minhash* min_h)
{
        int i,j;
        fprintf(stdout,"\t");
        for(i = 0; i < min_h->n_columns  ;i++){
                fprintf(stdout,"Col:%d\t",i);
        }
        fprintf(stdout,"\n");
        for(j = 0; j < min_h->n_signatures;j++){
                fprintf(stdout,"h%d\t",j);
                for(i = 0; i < min_h->n_columns  ;i++){
                        fprintf(stdout,"%d ",  min_h->sig[i][j]);
                }
                fprintf(stdout,"\n");
        }
        fprintf(stdout,"\n");
        return OK;
}

void free_minhash(struct minhash* min_h)
{
        int i;
        if(min_h){
                if(min_h->sig){
                        for(i = 0; i < min_h->n_columns;i++){
                                MFREE(min_h->sig[i]);
                        }
                        MFREE(min_h->sig);
                }
                MFREE(min_h->a);
                MFREE(min_h->b);
                MFREE(min_h);
        }
}

void free_Boolean_matrix(struct Boolean_matrix* bm)
{
        int i;
        if(bm){

                if(bm->m){
                        for(i = 0; i < bm->n_column;i++){
                                MFREE(bm->m[i]);
                        }
                        MFREE(bm->m);
                }
                MFREE(bm);
        }
}


int shuffle_arr(int* arr,int n)
{
        int i,j;
        int tmp;
        for (i = 0; i < n - 1; i++) {
                j = i +  (int) (lrand48() % (int) (n-i));
                tmp = arr[j];
                arr[j] = arr[i];
                arr[i] = tmp;
        }
        return OK;
}
