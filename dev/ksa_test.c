#include <stdio.h>
#include "string.h"
#include "tldevel.h"
#include "tlrng.h"

#include "ksa.h"

int searchSA(saint_t* SA, unsigned char* s,int n,char* p,int p_len, int *start,int*stop);

int main(int argc, char *argv[])
{
        struct rng_state* rng = NULL;
        unsigned char* t = NULL;
        saint_t* SA = NULL;
        int len = 1000000;
        int i;
        RUNP(rng = init_rng(42));

        MMALLOC(t,sizeof(unsigned char) * len);
        for(i = 0; i < len;i++){

                t[i] = (char) tl_random_int(rng, 4)+65;
                if(i && (i % 150) == 0){
                        t[i] = 0;
                        //fprintf(stdout,"\n");
                }else{
                        //fprintf(stdout,"%c",t[i]);
                }

        }
        t[len-1] = 0;
        MMALLOC(SA,sizeof(saint_t) * len);
        SAIS_MAIN(t,SA, len, 4+65);

        //for(i = 0; i < len;i++){
        //fprintf(stdout,"%d: %d %s\n",i, SA[i],  t+SA[i]);
        //}

        int start;
        int stop;

        searchSA(SA, t, len, "AADD",4, &start , &stop);
        LOG_MSG("%d->%d", start,stop);

        free_rng(rng);
        MFREE(SA);
        MFREE(t);

        return EXIT_SUCCESS;
ERROR:
        return FAIL;
}

int searchSA(saint_t* SA, unsigned char* s,int n,char* p,int p_len, int *start,int*stop)
{
        int l = 0;
        int r = n +1;
        int mid;
        while (l < r){
                mid = (l + r) / 2;
                if ( strncmp( s+SA[mid], p,p_len) < 0){
                        l = mid + 1;
                }else{
                        r = mid;
                }
        }
        *start = l;
        //s = l;
        r = n+1;
        while (l < r){
                mid = (l + r) / 2;
                if( strncmp(s+SA[mid], p,p_len)){
                        r = mid;
                }else{
                        l = mid + 1;
                }
        }
        //*start = s;
        *stop = r;
        return OK;

}

