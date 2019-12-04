#include "hash_table.h"

#include <string.h>
#include <stdio.h>

#include "tldevel.h"


static uint32_t hash_uint32( uint32_t a);

/* print commands */
int print_int(const int a)
{
        fprintf(stdout,"%d ",a);
        return OK;
}

int print_int_star(int* a)
{

        int len;
        RUN(get_dim1(a,&len));
        int i;
        for(i = 0; i < len;i++){
                fprintf(stdout,"%d,",a[i]);
        }
        fprintf(stdout,"\n");
        return OK;
ERROR:
        return FAIL;
}

int print_double(const double a)
{
        fprintf(stdout,"%f ",a);
        return OK;
}

int print_string(const char* a)
{
        fprintf(stdout,"%s ",a);
        return OK;
}

int ht_compare_key_int(const int a, const int b)
{
        if(a > b){
                return -1;
        }
        if(a == b){
                return 0;
        }
        return 1;
}

int ht_compare_key_int_star(int* a, int* b)
{
        int len_a;
        int len_b;

        get_dim1(a, &len_a);
        get_dim1(b, &len_b);
        int min_l = MACRO_MIN(len_a, len_b);
        int i;
        for(i = 0.; i < min_l;i++){
                if(a[i] > b[i]){
                        return -1;
                }else if(a[i] < b[i]){
                        return 1;
                }
        }
        if(len_a > len_b){
                return -1;
        }

        return 0;
}

int ht_compare_key_strings(const char* a, const char* b)
{
        return strcmp(a,b);
}


int ht_compare_key_double(const double a, const double b)
{
        if(a > b){
                return -1;
        }

        if(a == b){
                return 0;
        }
        return 1;

}



/* Hash variable commands  */
uint32_t get_hash_value_double(const double x,const int table_size)
{
        ASSERT(table_size != 0, "Table size cannot be 0!");

        return (unsigned long long) x %table_size;
ERROR:
        return 0;
}

uint32_t get_hash_value_int(const int x,const int table_size)
{

        ASSERT(table_size != 0, "Table size cannot be 0!");
        return x % table_size;
ERROR:
        return 0;
}


uint32_t get_hash_value_string(const char* s,const int table_size)
{
        ASSERT(table_size != 0, "Table size cannot be 0!");
        uint32_t hash = 0;

        for(; *s; ++s)
        {
                // fprintf(stdout,"%c\n",*s);
                hash += *s;
                hash += (hash << 10);
                hash ^= (hash >> 6);
        }

        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);

        return hash % table_size;
ERROR:
        return 0;
}

uint32_t hash_uint32( uint32_t a)
{
        a = (a+0x7ed55d16) + (a<<12);
        a = (a^0xc761c23c) ^ (a>>19);
        a = (a+0x165667b1) + (a<<5);
        a = (a+0xd3a2646c) ^ (a<<9);
        a = (a+0xfd7046c5) + (a<<3);
        a = (a^0xb55a4f09) ^ (a>>16);
        return a;
}

uint32_t get_hash_value_int_array(int* x,const int table_size)
{
        ASSERT(table_size != 0, "Table size cannot be 0!");
        uint32_t hash = 0;
        int len;
        int i;
        get_dim1(x,&len);
        hash = hash ^  hash_uint32(x[0]);
        for(i = 1; i < len;i++){
                hash = (hash << 5) ^ ( hash >> (27));
                hash ^= hash_uint32(x[i]);
        }
        return hash % table_size;
ERROR:
        return 0;
}

#ifdef HTITEST
HT_GLOBAL_INIT(TEST, char*)
HT_GLOBAL_INIT(TESTINT, int*)
HT_GLOBAL_INIT(TEST_DOUBLE, double)
int double_cmp(const void *a, const void *b);

struct hash_table_node_TEST_DOUBLE_t;
int double_cmp(const void *a, const void *b)
{
        hash_table_node_TEST_DOUBLE_t* const *ia = a;
        hash_table_node_TEST_DOUBLE_t* const *ib = b;
        if((*ia)->key  > (*ib)->key){
                return 1;
        }
        if((*ia)->key  > (*ib)->key){
                return 0;
        }
        return -1;

/* integer comparison: returns negative if b > a
   and positive if a > b */
}



int main (int argc,char * const argv[])
{

        int i,j;
        fprintf(stdout,"Hello world\n");
        char* f = NULL;


        RUNP(f = galloc(f,10));

        f[0] = 'a';
        f[1] = 'c';
        f[2] = 'c';
        f[3] = 'g';
        f[4] = 't';
        f[5] = 0;

        int test_int = 32;

        get_hash_value(test_int,10);
        fprintf(stdout,"HASH = %d\n",get_hash_value(test_int,100));
        fprintf(stdout,"HASH = %d\n",get_hash_value(f,100));
        // fprintf(stdout,"%d\n",get_hash_value(test_int));
        gfree(f);


        /*struct hash_table* ht = NULL;

        RUNP(ht = init_hash_table(345));

        for(i = 0; i < 2000;i++){
                insert(ht,i,NULL);
        }

        print_hash_table(ht);
        free_hash_table(ht);*/

        HT_TYPE(TEST)* my_ht = NULL;

        my_ht = HT_INIT(TEST,123);
        for(i = 0; i < 2000;i++){
                char* test = NULL;
                //MMALLOC(test, sizeof(char)*100);
                test = galloc(test,100);
                int j,c;
                c = i;
                for(j = 0; j < 10;j++){
                        test[j] = (c & 0x3) +65;
                        c = c >> 2;
                }
                test[10] = 0;

                RUN(HT_INSERT(TEST,my_ht,test,NULL));

        }

        for(i = 0; i < 2000;i++){
                char* test = NULL;
                //MMALLOC(test, sizeof(char)*100);
                test = galloc(test,100);
                int j,c;
                c = i;
                for(j = 0; j < 10;j++){
                        test[j] = (c & 0x3) +65;
                        c = c >> 2;
                }
                test[10] = 0;

                RUN(HT_INSERT(TEST,my_ht,test,NULL));

        }
        HT_PRINT(TEST,my_ht);
        HT_FREE(TEST,my_ht);


        HT_TYPE(TEST_DOUBLE )* my_htt = NULL;

        my_htt = HT_INIT(TEST_DOUBLE,123);
        for(i = 0; i < 2000;i++){
                RUN(HT_INSERT(TEST_DOUBLE,my_htt,(double)i,NULL));

        }
        for(i = 0; i < 2000;i++){
                RUN(HT_INSERT(TEST_DOUBLE,my_htt,(double)i,NULL));

        }
        hash_table_node_TEST_DOUBLE_t* hashnode = NULL;



        HT_PRINT(TEST_DOUBLE,my_htt);
        HT_FLATTEN(TEST_DOUBLE,my_htt);

        qsort(my_htt->flat,my_htt->num_items, sizeof(hash_table_node_TEST_DOUBLE_t*), double_cmp);
        for(i = 0; i < my_htt->num_items;i++){
                fprintf(stdout,"%d %f %d \n", i, my_htt->flat[i]->key,my_htt->flat[i]->count);
        }
        fprintf(stdout,"Hash table has %d entries, %d nodes and %d item counts\n", my_htt->table_size, my_htt->num_items, my_htt->total_count);

        LOG_MSG("searching for %f", (double) 42);

        hashnode = HT_SEARCH(TEST_DOUBLE, my_htt, 42);
        if(hashnode){
                LOG_MSG("Found:%f %d %p\n", hashnode->key,hashnode->count, hashnode->data);
        }
        HT_FREE(TEST_DOUBLE,my_htt);

        HT_TYPE(TESTINT )* int_array_table = NULL;
        int_array_table = HT_INIT(TESTINT,123);
        int* tmp = NULL;
        for(i = 0; i < 200;i++){
                tmp = NULL;
                tmp= galloc(tmp,10);
                for(j = 0 ; j < 10;j++){
                        tmp[j] = rand() % 151;
                }

                RUN(HT_INSERT(TESTINT,int_array_table,tmp,NULL));

        }
        HT_PRINT(TESTINT,int_array_table);

        HT_FREE(TESTINT,int_array_table);
        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;

}


#endif
