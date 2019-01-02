#include "hash_table.h"
/* print commands */
int print_int(int a)
{
        fprintf(stdout,"%d ",a);
        return OK;
}

int print_int_star(int* a)
{
        fprintf(stdout,"%d ",*a);
        return OK;
}

int print_double(double a)
{
        fprintf(stdout,"%f ",a);
        return OK;
}

int print_string(char* a)
{
        fprintf(stdout,"%s ",a);
        return OK;
}


/* compare commands */

int ht_compare_key_int(int a, int b)
{
        if(a == b){
                return 1;
        }
        return 0;
}

int ht_compare_key_strings(char* a, char* b)
{

        if(!strcmp(a,b)){
                return 1;
        }
        return 0;
}

int ht_compare_key_int_star(int* a, int* b)
{
        if(*a == *b){
                return 1;
        }
        return 0;
}

int ht_compare_key_double(double a, double b)
{
        if(a == b){
                return 1;
        }
        return 0;

}



/* Hash variable commands  */
uint32_t get_hash_value_double(double x,int table_size)
{
        ASSERT(table_size != 0, "Table size cannot be 0!");

        return (unsigned long long) x %table_size;
ERROR:
        return 0;
}

uint32_t get_hash_value_int(int x, int table_size)
{

        ASSERT(table_size != 0, "Table size cannot be 0!");
        return x % table_size;
ERROR:
        return 0;
}

uint32_t get_hash_value_int_star(int* x, int table_size)
{

        ASSERT(table_size != 0, "Table size cannot be 0!");
        return *x % table_size;
ERROR:
        return 0;
}

uint32_t get_hash_value_string(char* s, int table_size)
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

#ifdef HTITEST
HT_GLOBAL_INIT(TEST, char*);

HT_GLOBAL_INIT(TEST_DOUBLE, double);
int main (int argc,char * const argv[])
{

        int i;
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
        HT_PRINT(TEST,my_ht);
        HT_FREE(TEST,my_ht);


        HT_TYPE(TEST_DOUBLE )* my_htt = NULL;

        my_htt = HT_INIT(TEST_DOUBLE,123);
        for(i = 0; i < 2000;i++){
                RUN(HT_INSERT(TEST_DOUBLE,my_htt,(double)i,NULL));

        }
        HT_PRINT(TEST_DOUBLE,my_htt);
        HT_FREE(TEST_DOUBLE,my_htt);

        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;

}
#endif
