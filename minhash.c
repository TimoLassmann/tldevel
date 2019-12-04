

#include "minhash.h"

#include "tlrng.h"
int shuffle_arr_minhash(int* arr,int n, struct rng_state* rng);

struct minhash* create_min_hash(struct Boolean_matrix* bm, int num_sig,long int seed)
{
        struct minhash* min_h = NULL;
        uint32_t* col = NULL;
        int* list = NULL;
        int i,j,c;
        int n,m;
        //struct drand48_data randBuffer;

        struct rng_state* rng = NULL;
        ASSERT(bm!= NULL, "No matrix");

        RUNP(rng = init_rng(seed));

        /*if(seed){
                srand48_r(seed, &randBuffer);
        }else{
                srand48_r(time(NULL), &randBuffer);
                }*/


        MMALLOC(min_h, sizeof(struct minhash));
        min_h->sig = NULL;
        //min_h->a = NULL;
        //min_h->b = NULL;


        min_h->n_signatures = num_sig;
        min_h->n_columns = bm->n_column;
        min_h->n_samples = bm->n_row;

        MMALLOC( min_h->sig, sizeof(uint32_t*) * bm->n_column);

        for(i = 0; i < min_h->n_columns;i++){
                min_h->sig[i] = NULL;
                MMALLOC(min_h->sig[i],sizeof(uint32_t) * min_h->n_signatures);
                for(j = 0; j < min_h->n_signatures;j++){
                        min_h->sig[i][j] = INT_MAX;
                }
        }
        //MMALLOC(min_h->a, sizeof(uint32_t)* min_h->n_signatures);
        //MMALLOC(min_h->b, sizeof(uint32_t)* min_h->n_signatures);



        m = bm->n_column;
        n = bm->n_row;

        //fprintf(stdout,"%d columns\n", m);
        MMALLOC(list,sizeof(int)* n);
        for(i = 0; i < n;i++){
                list[i] = i;
        }

        /*for(i = 0; i < min_h->n_signatures;i++){

                c = 1;
                while(c){
                        c = 0;
                        RUN(lrand48_r(&randBuffer, &r));

                        min_h->a[i] = (r  %  (PRIME_P-1u)) +1u ;
                }


        }
        c = 1;
        while (c == 1){
                c = 0;
                for(i = 0;i < min_h->n_signatures-1;i++){
                        for(j = i+1;j < min_h->n_signatures;j++){
                                if(min_h->a[i] == min_h->a[j]){
                                        RUN(lrand48_r(&randBuffer, &r));

                                        min_h->a[i] = (r  %  (PRIME_P-1u)) +1u ;

                                        c = 1;
                                        break;
                                }
                        }
                }
        }
        for(i = 0; i < min_h->n_signatures;i++){
                RUN(lrand48_r(&randBuffer, &r));
                min_h->b[i] = (r  %  (PRIME_P-1u));//r % max_item_id;//(MAXITEMID + 1000);
        }
        c = 1;
        while (c == 1){
                c = 0;
                for(i = 0;i < min_h->n_signatures-1;i++){
                        for(j = i+1;j < min_h->n_signatures;j++){
                                if(min_h->b[i] == min_h->b[j]){
                                        RUN(lrand48_r(&randBuffer, &r));
                                        min_h->b[j] =(r  %  (PRIME_P-1u));//r  % max_item_id;
                                        c = 1;
                                        break;
                                }
                        }
                }
                }*/


        //for(i = 0; i < min_h->n_signatures;i++){
                //       fprintf(stdout,"%d %d %d %d\n",i, min_h->a[i],min_h->b[i], prime);
        //}
        //exit(0);
        /* Apply has functions  */
        for(c = 0;c < min_h->n_signatures;c++){

                shuffle_arr_minhash(list, n ,rng);
                for(i = 0; i < m;i++){
                        col = bm->m[i];
                        for(j = 0; j < n;j++){
                                if(bit_test(col,j)){
                                        if(list[j]+1 < min_h->sig[i][c]){
                                                min_h->sig[i][c] = list[j]+1;
                                        }
                                }
                        }
                }
        }
        MFREE(list);
        //fprintf(stdout,"PRIME: %d",prime);
        /*for(i =0; i < m;i++){
                col = bm->m[i];
                //fprintf(stdout,"COLUMN:%d\n",i);
                for(c = 0; c < min_h->n_signatures;c++){
                        if(!i){
                                  fprintf(stdout,"%d\t",c);
                        }
                        min_h->sig[i][c] = PRIME_P + 1;
                        for(j = 0; j < n;j++){
                                if(!i){
                                                       fprintf(stdout,"%u\t",( (min_h->a[c] * (j) + min_h->b[c]) % PRIME_P ) );
                                }
                                if(bit_test(col, j)){
                                        hash_val = ( (min_h->a[c] * (unsigned int)(j+1) + min_h->b[c]) % PRIME_P ) ;
                                        if(hash_val < min_h->sig[i][c]){
                                                min_h->sig[i][c] = hash_val;
                                        }
                                }
                        }
                        if(!i){
                                      fprintf(stdout,"\n");
                        }
                }
                }
        // exit(0);
//(size_t) (a*x+b) >> (w-M)
//(size_t) (a*x+b) >> (w-M)*/
        /*for(i = 0; i < min_h->n_columns;i++){

                for(j = 0; j < min_h->n_signatures;j++){
                               fprintf(stdout,"%d ", min_h->sig[i][j]);
                }
                fprintf(stdout,"\n");
                }*/
        MFREE(rng);
        return min_h;
ERROR:
        return NULL;
}



int jaccard_sim_min_multihash(struct minhash* min_h , int* S, int n, double* jac_sim, double *p_S_in_X)
{
        int i,j;
        unsigned int** m = NULL;
        double set_intersection = 0.0;
        double c,min;
        int num_samples;

        double min_stuff = 0.0;
        ASSERT(min_h != NULL, "No minhash");
        m = min_h->sig;
        num_samples = min_h->n_samples;

        for(i = 0; i < min_h->n_signatures;i++){
                min =  m[S[0]][i];
                c = 1.0;
                for(j = 1; j < n;j++){
                        min = MACRO_MIN(min, m[S[j]][i]);
                        if(m[S[j]][i] != m[S[0]][i]){
                                c =0.0;
                                //break;
                        }
                }

                set_intersection += c;

                //fprintf(stdout,"%d %d : %f\n",col_a[i],col_b[i],(double)MACRO_MIN(col_a[i],col_b[i]));
                min_stuff += min;
        }
        //fprintf(stdout,"%f\n",min_stuff);
        min_stuff = min_stuff / (double)  min_h->n_signatures;
        *jac_sim = set_intersection / (double) (min_h->n_signatures);
        //fprintf(stdout,"%f\n",min_stuff);

        *p_S_in_X = *jac_sim *  (((double)(num_samples+1) /(double) num_samples) *(  1.0 /min_stuff) -(1.0/(double)(num_samples +1)));
        //fprintf(stdout,"%d %d %f %f\n",a,b,set_intersection,  *avg_min_sig_diff);
        //LOG_MSG("samples:%d %d", num_samples,min_h->n_samples);

        return OK;
ERROR:
        return FAIL;
}


int jaccard_sim_min_hash(struct minhash* min_h , int a, int b, double* jac_sim, double *avg_min_sig_diff)
{
        int i;
        double set_intersection = 0.0;


        unsigned int* col_a = NULL;
        unsigned int* col_b = NULL;
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
                //MFREE(min_h->a);
                //MFREE(min_h->b);
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


int shuffle_arr_minhash(int* arr,int n, struct rng_state* rng)
{
        int i,j;
        int tmp;
        long int r;


        for (i = 0; i < n - 1; i++) {

                r = tl_random_int(rng, n-i);
                //RUN(lrand48_r(randBuffer,&r));
                //int lrand48_r(struct drand48_data *buffer, long int *result);
                j = i + r;// ((int) r % (int) (n-i));
                tmp = arr[j];
                arr[j] = arr[i];
                arr[i] = tmp;
        }
        return OK;
}


#ifdef MINITEST
int jaccard_sim(struct Boolean_matrix* bm, int*S , int n, double* jac_sim);
int jaccard_sim_min_hash(struct minhash* min_h , int a, int b, double* jac_sim, double *avg_min_sig_diff);

static struct Boolean_matrix* init_random_Bmatrix(int columns,int rows,  double alpha,struct drand48_data *rd);
int print_Boolean_matrix(struct Boolean_matrix* bm);
int print_minhash_signatures(struct minhash* min_h);


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

int main (int argc,char * const argv[])
{
        fprintf(stdout,"Hello world\n");

        struct Boolean_matrix* bm = NULL;
        struct minhash* min_h = NULL;
        double sim;
        double sim_min = 0.0;
        int num_samples = 1000;
        int i;
        int trials = 10;
        double s1 = 0.0;
        double s2 = 0.0;
        double s1_p = 0.0;
        double s2_p= 0.0;

        double diff;
        double diff_p;
        double p_S_in_X = 0.0;
        double alpha = 0.999;

        int num_hash_functions = 200;
        int iter;
        struct drand48_data randBuffer;
        int* index =NULL;
        int S_size = 6;

        MMALLOC(index, sizeof(int) * S_size);
        for(i = 0; i < S_size;i++){
                index[i] = i;
        }


        srand48_r(42, &randBuffer);

        for(iter = 0; iter < trials;iter++){
                /* S_size is just for simulation - this should be the number of variables... */
                RUNP(bm = init_random_Bmatrix(S_size,num_samples,alpha, &randBuffer));
                //RUN(print_Boolean_matrix(bm));
                min_h = create_min_hash(bm, num_hash_functions, 0);
                RUN(jaccard_sim(bm,index, S_size, &sim));
                jaccard_sim_min_multihash(min_h, index, S_size, &sim_min,&p_S_in_X);

                diff = fabs(sim-sim_min);
                s1 += diff;
                s2 += diff * diff;
                diff_p = fabs(p_S_in_X-  pow(alpha,(double)S_size));
                s1_p += diff_p;
                s2_p += diff_p * diff_p;
                fprintf(stdout,"%f %f delta: %f\t",sim,sim_min, diff);
                fprintf(stdout,"P seeing: %f (%f) delta: %f\n",p_S_in_X,   pow(alpha, (double)S_size),diff_p);
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


#endif
