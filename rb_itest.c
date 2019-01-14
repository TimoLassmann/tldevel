
#include <stdio.h>
#include "tldevel.h"
#include "rb.h"

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

void* rb_malloc_tldevel (struct libavl_allocator *allocator, size_t size)
{
        void* p = NULL;
        ASSERT(allocator != NULL, "No allocator");

        MMALLOC(p,size);
        return p;
ERROR:
        return NULL;
}

/* Frees |block|. */
void rb_free_tldevel (struct libavl_allocator *allocator, void *block)
{
        MFREE(block);
}

/* Default memory allocator that uses |malloc()| and |free()|. */
struct libavl_allocator rb_allocator_tldevel =
{
        rb_malloc_tldevel,
        rb_free_tldevel
};

int compare_test_data_d (const void *pa, const void *pb, void *param)
{
        const struct test_data *a = pa;
        const struct test_data *b = pb;

        double a_d,b_d;
        a_d = a->d_key;
        b_d = b->d_key;

        if(a_d < b_d){
                return -1;
        }else if(a_d > b_d){
                return 1;
        }else{
                int a_i,b_i;

                a_i = a->i_key;
                b_i = b->i_key;
                if(a_i < b_i){
                        return -1;
                }else if(a_i > b_i){
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


        struct rb_table* table_double = NULL;
        struct test_data* data = NULL;
        int i;
        double r = 0;
        unsigned seed = time(0);


        fprintf(stdout,"Create table\n");
        RUNP(table_double = rb_create(compare_test_data_d,NULL,&rb_allocator_tldevel));
        fprintf(stdout,"DONE\n");


        for(i = 0; i < 1024;i++){
                r = random_float_zero_to_x_thread(1.0, &seed);
                //fprintf(stdout,"%f\n",r);
                data= NULL;
                MMALLOC(data,sizeof(struct test_data));
                data->c_key = NULL;
                data->i_key = i;
                data->d_key = r;

                data = rb_insert(table_double, data);
                if(data){
                        ERROR_MSG("Insert failed");
                }

        }
        //print_whole_tree (table_double, "Double tree");
        struct rb_traverser* tmp = NULL;

        fprintf(stdout,"Count: %ld\n", rb_count(table_double));

        MMALLOC(tmp, sizeof(struct rb_traverser));
        /* init traverser */
        rb_t_init (tmp , table_double);

        data = rb_t_first (tmp, table_double);
        if(data){
                fprintf(stdout,"FIRST: %f \n",data->d_key);
                while((data = rb_t_next(tmp))){



                        //fprintf(stdout,"next: %f \n",data->d_key);
                }

                data = rb_t_last(tmp, table_double);
                fprintf(stdout,"LAST: %f \n",data->d_key);
                while((data= rb_t_prev(tmp))){



                        //fprintf(stdout,"prev: %f \n",data->d_key);
                }


        }

        rb_destroy(table_double,free_test_data_node );

        MFREE(tmp);

        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;

}
