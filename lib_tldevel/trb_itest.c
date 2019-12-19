
#include <stdio.h>
#include "tldevel.h"
#include "trb.h"

/* I want to test:
   1) how to provide my own malloc wrapper; OK
   2) how to work with structs as data items; OK
   3) sorting functions OK
   4) traversal
*/


struct test_data{
        char* c_key;
        double d_key;
        int  i_key;
};

void* trb_malloc_tldevel (struct libavl_allocator *allocator, size_t size)
{
        void* p = NULL;
        ASSERT(allocator != NULL, "No allocator");

        MMALLOC(p,size);
        return p;
ERROR:
        return NULL;
}

/* Frees |block|. */
void trb_free_tldevel (struct libavl_allocator *allocator, void *block)
{
        MFREE(block);
}

/* Default memory allocator that uses |malloc()| and |free()|. */
struct libavl_allocator trb_allocator_tldevel =
{
        trb_malloc_tldevel,
        trb_free_tldevel
};

int compare_test_data_d (const void *pa, const void *pb, void *param)
{
        const struct test_data *a = pa;
        const struct test_data *b = pb;

        fprintf(stdout,"%p %p\n", a,b);

        double a_d,b_d;
        a_d = a->d_key;
        b_d = b->d_key;
        fprintf(stdout,"%f\n", a_d);
        fprintf(stdout,"%f\n", b_d);

        if(a_d < b_d){
                return -1;
        }else if(a_d > b_d){
                return 1;
        }else{
                int a_i,b_i;
                LOG_MSG("Oh Oh duplicated item?");
                LOG_MSG("%f %f", a_d,b_d);

                a_i = a->i_key;
                b_i = b->i_key;
                LOG_MSG("%d %d", a_i,b_i);
                if(a_i < b_i){
                        LOG_MSG("a < b");
                        return -1;
                }else if(a_i > b_i){
                        LOG_MSG("a > b");
                        return 1;
                }else{
                        LOG_MSG("a == b");
                        return 0;
                }
        }
}

void free_test_data_node(void* item,void *param)
{
        MFREE( item);
}


int main (int argc,char * argv[])
{
        fprintf(stdout,"Hello world\n");


        struct trb_table* table_double = NULL;
        struct test_data* data = NULL;
        int i;
        double r = 0;
        unsigned seed = time(0);


        fprintf(stdout,"Create table\n");
        RUNP(table_double = trb_create(compare_test_data_d,NULL,&trb_allocator_tldevel));
        fprintf(stdout,"DONE\n");

        fprintf(stdout,"root:%p\n",table_double->trb_root);



        for(i = 0; i < 5;i++){
                fprintf(stdout,"ITER:%d\n",i);
                r = random_float_zero_to_x_thread(1.0, &seed);
                fprintf(stdout,"%f\n",r);
                data= NULL;
                MMALLOC(data,sizeof(struct test_data));
                data->c_key = NULL;
                data->i_key = i;
                data->d_key = r;
                fprintf(stdout,"KEY: %f %d\n",data->d_key,data->i_key);


                fprintf(stdout,"data pointer: %p\n", data);

                data = trb_insert(table_double, data);

                 fprintf(stdout,"data pointer: %p\n", data);

                //if(data != NULL){

                //        ERROR_MSG("Insert failed");

                //}
                /*MMALLOC(data,sizeof(struct test_data));
                data->c_key = NULL;
                data->i_key = i+100;
                data->d_key = r;
                fprintf(stdout,"%f %d ",data->d_key,data->i_key);
                data = tavl_insert(table_double, (void*) data);
                if(data != NULL){
                        ERROR_MSG("Insert failed");


                }*/

        }
        //print_whole_tree (table_double, "Double tree");
        struct trb_traverser* tmp = NULL;

        fprintf(stdout,"Count: %ld\n", trb_count(table_double));

        MMALLOC(tmp, sizeof(struct trb_traverser));
        /* init traverser */
        trb_t_init (tmp , table_double);

        data = trb_t_first (tmp, table_double);
        if(data){
                fprintf(stdout,"FIRST: %f \n",data->d_key);
                while((data = trb_t_next(tmp))){



                        fprintf(stdout,"next: %f \n",data->d_key);
                }

                data = trb_t_last(tmp, table_double);
                fprintf(stdout,"LAST: %f \n",data->d_key);
                while((data= trb_t_prev(tmp))){



                        fprintf(stdout,"prev: %f \n",data->d_key);
                }


        }

        trb_destroy(table_double,free_test_data_node );

        MFREE(tmp);

        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;

}
