
#include <stdio.h>
#include "tldevel.h"
#include "tavl.h"

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

void* tavl_malloc_tldevel (struct libavl_allocator *allocator, size_t size)
{
        void* p = NULL;
        ASSERT(allocator != NULL, "No allocator");

        MMALLOC(p,size);
        return p;
ERROR:
        return NULL;
}

/* Frees |block|. */
void tavl_free_tldevel (struct libavl_allocator *allocator, void *block)
{
        MFREE(block);
}

/* Default memory allocator that uses |malloc()| and |free()|. */
struct libavl_allocator tavl_allocator_tldevel =
{
        tavl_malloc_tldevel,
        tavl_free_tldevel
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

void
print_tree_structure (struct tavl_node *node, int level)
{
  int i;

  /* You can set the maximum level as high as you like.
     Most of the time, you'll want to debug code using small trees,
     so that a large |level| indicates a ``loop'', which is a bug. */
  if (level > 16)
    {
      printf ("[...]");
      return;
    }

  if (node == NULL)
    {
      printf ("<nil>");
      return;
    }

  printf ("%d(", node->tavl_data ? *(int *) node->tavl_data : -1);

  for (i = 0; i <= 1; i++)
    {
      if (node->tavl_tag[i] == TAVL_CHILD)
        {
          if (node->tavl_link[i] == node)
            printf ("loop");
          else
            print_tree_structure (node->tavl_link[i], level + 1);
        }
      else if (node->tavl_link[i] != NULL)
        printf (">%d",
                (node->tavl_link[i]->tavl_data
                ? *(int *) node->tavl_link[i]->tavl_data : -1));
      else
        printf (">>");

      if (i == 0)
        fputs (", ", stdout);
    }

  putchar (')');
}

/* Prints the entire structure of |tree| with the given |title|. */
void
print_whole_tree (const struct tavl_table *tree, const char *title)
{
  printf ("%s: ", title);
  print_tree_structure (tree->tavl_root, 0);
  putchar ('\n');
}

int main (int argc,char * argv[])
{
        fprintf(stdout,"Hello world\n");


        struct tavl_table* table_double = NULL;
        struct test_data* data = NULL;
        int i;
        double r = 0;
        unsigned seed = time(0);


        fprintf(stdout,"Create table\n");
        RUNP(table_double = tavl_create(compare_test_data_d,NULL,&tavl_allocator_tldevel));
        fprintf(stdout,"DONE\n");

        fprintf(stdout,"root:%p\n",table_double->tavl_root);


        fprintf(stdout,"MAX:height%d\n", TAVL_MAX_HEIGHT);
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

                data = tavl_insert(table_double, data);

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
        struct tavl_traverser* tmp = NULL;

        fprintf(stdout,"Count: %ld\n",tavl_count(table_double));

        MMALLOC(tmp, sizeof(struct tavl_traverser));
        /* init traverser */
        tavl_t_init (tmp , table_double);

        data = tavl_t_first (tmp, table_double);
        if(data){
                fprintf(stdout,"FIRST: %f \n",data->d_key);
                while((data = tavl_t_next(tmp))){



                        fprintf(stdout,"next: %f \n",data->d_key);
                }

                data = tavl_t_last(tmp, table_double);
                fprintf(stdout,"LAST: %f \n",data->d_key);
                while((data= tavl_t_prev(tmp))){



                        fprintf(stdout,"prev: %f \n",data->d_key);
                }


        }

        tavl_destroy(table_double,free_test_data_node );

        MFREE(tmp);

        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;

}
