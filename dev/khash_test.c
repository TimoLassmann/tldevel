
#include <stdio.h>

#include "tldevel.h"

#include "khash.h"

typedef struct test_struct{
        int a;
        int b;
} test;



KHASH_MAP_INIT_INT(32, int)
KHASH_MAP_INIT_INT(gg, test)

int main() {
        int ret, is_missing;
        int r;
        int i;

        khiter_t k;

        LOG_MSG("Trying simple int insert....");
        khash_t(32) *h = kh_init(32);

        for(i = 0; i < 1000000;i++){

                r = rand() % 1000;
                k = kh_put(32, h, r, &ret);
                //is_missing = (k == kh_end(h));
                //fprintf(stdout,"put: %d   ret:%d\n",k,ret);
                if (!ret){
                        //k = kh_get(32, h, r);
                        //fprintf(stdout,"Exists - increment\n");
                        kh_value(h, k)++;
                }else{
                        kh_value(h, k) = 1;
                        //fprintf(stdout,"set to 1 \n");
                }
                //fprintf(stdout, "%d %d\n",k,kh_value(h, k));
                        //}else{
                        //
                        //}
        }


        i = 0;
        //kh_del(32, h, k);
        for (k = kh_begin(h); k != kh_end(h); ++k){
                if (kh_exist(h, k)){
                        fprintf(stdout, "%d %d\n",k,kh_value(h, k));
                        i += kh_value(h, k);
                        //kh_value(h, k) = 1;
                }
        }

        fprintf(stdout," hash size: %d, count: %d\n", kh_size(h),i);
        kh_destroy(32, h);

        LOG_MSG("Done.");

        LOG_MSG("Slightly more interesting: a hash filled with structs");



        khash_t(gg) *h2 = kh_init(gg);

        for(i = 0; i < 1000000;i++){

                r = rand() % 1000;
                //k = kh_put(32, h, r, &ret);
                k = kh_put(gg, h2, r, &ret);
                //is_missing = (k == kh_end(h));
                //fprintf(stdout,"put: %d   ret:%d\n",k,ret);
                if (!ret){
                        //k = kh_get(32, h, r);
                        //fprintf(stdout,"Exists - increment\n");
                        kh_value(h2, k).a++;
                        kh_value(h2, k).b++;
                }else{
                        kh_value(h2, k).a = 1;
                        kh_value(h2, k).b = 0;

                        //fprintf(stdout,"set to 1 \n");
                }
        }

        for (k = kh_begin(h2); k != kh_end(h2); ++k){
                if (kh_exist(h2, k)){
                        fprintf(stdout, "%d a:%d\n",k,kh_value(h2, k).a );
                        fprintf(stdout, "%d b:%d\n",k,kh_value(h2, k).b );

                }
        }


        kh_destroy(gg, h2);
        return 0;

}
