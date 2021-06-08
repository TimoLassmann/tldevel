#include "tldevel.h"

#include "tlhdf5wrap.h"
#include "tlhdf5wrap_types.h"

#include <string.h>
/* generic print functions taken from:

http://www.robertgamble.net/2012/01/c11-generic-selections.html

and altered to use the standard int types.

 */
#define printf_dec_format(x) _Generic((x),              \
                                      char: "%c",       \
                                      int8_t: "%hhd",   \
                                      uint8_t: "%hhu",  \
                                      int16_t: "%hd",   \
                                      uint16_t: "%hu",  \
                                      int32_t: "%d",    \
                                      uint32_t: "%u",   \
                                      int64_t: "%ld",   \
                                      uint64_t: "%lu",  \
                                      float: "%f",      \
                                      double: "%f",     \
                                      char *: "%s",     \
                                      void *: "%p")

#define print(x) printf(printf_dec_format(x), x)
#define printnl(x) printf(printf_dec_format(x), x), printf("\n");

#define STR_VALUE(arg)      #arg
#define TYPE_NAME(name) STR_VALUE(name)


#define ATTR_TEST(type)                                                 \
        int attribute_test_ ##type(type x);                             \
        int attribute_test_ ##type(type x)                              \
        {                                                               \
                struct hdf5_data* d = NULL;                             \
                type r = 0;                                             \
                RUN(open_hdf5_file(&d,"hdf5testfile.h5"));              \
                RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/",TYPE_NAME(type),x));  \
                RUN(HDFWRAP_READ_ATTRIBUTE(d,"/",TYPE_NAME(type), &r)); \
                ASSERT(r == x,"Attribute read failed - read and written value differ."); \
                RUN(close_hdf5_file(&d));                               \
                d = NULL;                             \
                RUN(open_hdf5_file(&d,"hdf5testfile.h5"));              \
                RUN(HDFWRAP_READ_ATTRIBUTE(d,"/",TYPE_NAME(type),&r));  \
                ASSERT(r == x,"Attribute read failed after re-opening file - read and written value differ"); \
                RUN(close_hdf5_file(&d));                               \
                printnl(x);                                             \
                printnl(r);                                             \
                return OK;                                              \
        ERROR:                                                          \
                return FAIL;                                            \
        }


ATTR_TEST(int8_t)
ATTR_TEST(uint8_t)
ATTR_TEST(int16_t)
ATTR_TEST(uint16_t)
ATTR_TEST(int32_t)
ATTR_TEST(uint32_t)
ATTR_TEST(int64_t)
ATTR_TEST(uint64_t)
ATTR_TEST(float)
ATTR_TEST(double)

//#undef ATTR_TEST

static int unknown(void);

#define ATTRIBUTE_ITEST(x) _Generic((x),                                \
                                    int8_t: attribute_test_int8_t,      \
                                    uint8_t: attribute_test_uint8_t,    \
                                    int16_t: attribute_test_int16_t,    \
                                    uint16_t: attribute_test_uint16_t,  \
                                    int32_t: attribute_test_int32_t,    \
                                    uint32_t: attribute_test_uint32_t,  \
                                    int64_t: attribute_test_int64_t,    \
                                    uint64_t: attribute_test_uint64_t,  \
                                    float: attribute_test_float,        \
                                    double: attribute_test_double      \
                                    )(x)

int unknown(void)
{
        ERROR_MSG("unknown data type");
        return OK;
ERROR:
        return FAIL;
}

#define ONE_RW_TEST(type)                                               \
        int one_d_rw_test_ ##type(struct hdf5_data* d)                  \
        {                                                               \
                char* group_name = NULL;                                \
                type* x = NULL;                                         \
                int dim1 = 0;                                           \
                int dim2 = 0;                                           \
                MMALLOC(group_name, sizeof(char) * BUFSIZ);             \
                snprintf(group_name, BUFSIZ,"%s_1D", TYPE_NAME(type));  \
                LOG_MSG("%s", group_name);                              \
                dim1 = 6;                                               \
                RUN(galloc(&x,dim1));                                   \
                RUN(HDFWRAP_WRITE_DATA(d,"/1D_rw_test",group_name, x)); \
                gfree(x);                                               \
                x = NULL;                                               \
                RUN(HDFWRAP_READ_DATA(d,"/1D_rw_test",group_name, &x)); \
                gfree(x);                                               \
                                                                        \
                x = NULL;                                               \
                dim1 = 6;                                               \
                RUN(galloc_aligned(&x,dim1));                                   \
                RUN(HDFWRAP_WRITE_DATA(d,"/1D_rw_test_aligned",group_name, x)); \
                gfree(x);                                               \
                x = NULL;                                               \
                RUN(HDFWRAP_READ_DATA(d,"/1D_rw_test_aligned",group_name, &x)); \
                gfree(x);                                               \
                                                                        \
                MFREE(group_name);                                      \
                return OK;                                              \
        ERROR:                                                          \
                return FAIL;                                            \
        }

ONE_RW_TEST(int8_t)
ONE_RW_TEST(uint8_t)
ONE_RW_TEST(int16_t)
ONE_RW_TEST(uint16_t)
ONE_RW_TEST(int32_t)
ONE_RW_TEST(uint32_t)
ONE_RW_TEST(int64_t)
ONE_RW_TEST(uint64_t)
ONE_RW_TEST(float)
ONE_RW_TEST(double)

#define TESTFILE "hdf5testfile.h5"
static int rw_test(void);

int rw_test(void)
{
        struct hdf5_data* d = NULL;
        RUN(open_hdf5_file(&d,TESTFILE));
        one_d_rw_test_uint8_t(d);
        one_d_rw_test_int8_t(d);
        one_d_rw_test_uint16_t(d);
        one_d_rw_test_int16_t(d);
        one_d_rw_test_uint32_t(d);
        one_d_rw_test_int32_t(d);
        one_d_rw_test_uint64_t(d);
        one_d_rw_test_int64_t(d);
        one_d_rw_test_float(d);
        one_d_rw_test_double(d);
        RUN(close_hdf5_file(&d));
        return OK;
ERROR:
        return FAIL;
}

int main(int argc, char *argv[])
{
        LOG_MSG("Testing RW");
        rw_test();
        LOG_MSG("Done");


        struct hdf5_data* d = NULL;
        int** test = NULL;

        LOG_MSG("Running attribute tests.");

        int8_t i8= -7;
        uint8_t ui8= 7;
        int16_t i16= 15;
        uint16_t ui16 = 15;
        int32_t i32 = -31;
        uint32_t ui32= 31;
        int64_t i64= -63 ;
        uint64_t ui64= 63;
        float flt = 0.123f;
        double dbl = 0.1234;
        char* string = "otto";
        char* string_read = NULL;
        LOG_MSG("writing int8_t attribute.");
        ATTRIBUTE_ITEST(i8);
        LOG_MSG("writing uint8_t attribute.");
        ATTRIBUTE_ITEST(ui8);
        LOG_MSG("writing int16_t attribute.");
        ATTRIBUTE_ITEST(i16);
        LOG_MSG("writing uint16_t attribute.");
        ATTRIBUTE_ITEST(ui16);
        LOG_MSG("writing int32_t attribute.");
        ATTRIBUTE_ITEST(i32);
        LOG_MSG("writing uint32_t attribute.");
        ATTRIBUTE_ITEST(ui32);
        LOG_MSG("writing int64_t attribute.");
        ATTRIBUTE_ITEST(i64);
        LOG_MSG("writing uint64_t attribute.");
        ATTRIBUTE_ITEST(ui64);
        LOG_MSG("writing float attribute.");
        ATTRIBUTE_ITEST(flt);
        LOG_MSG("writing double attribute.");
        ATTRIBUTE_ITEST(dbl);

        LOG_MSG("writing string attribute.");

        RUN(open_hdf5_file(&d,"hdf5testfile.h5"));

        RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/",TYPE_NAME(type),string));
        RUN(HDFWRAP_READ_ATTRIBUTE(d,"/",TYPE_NAME(type), &string_read));
        if(strcmp(string,string_read)){
                printnl(string);
                printnl(string_read);
                ERROR_MSG("Strings differ!");
        }
        RUN(close_hdf5_file(&d));
        MFREE(string_read);
        string_read = NULL;
        RUN(open_hdf5_file(&d,"hdf5testfile.h5"));
        RUN(HDFWRAP_READ_ATTRIBUTE(d,"/",TYPE_NAME(type),&string_read));
        if(strcmp(string,string_read
                  )){
                printnl(string);
                printnl(string_read);
                ERROR_MSG("Strings differ!");
        }
        RUN(close_hdf5_file(&d));
        printnl(string);
        printnl(string_read);
        MFREE(string_read);


        RUN(open_hdf5_file(&d,"hdf5testfile.h5"));


        //RUN(hdf5wrap)
        int16_t sint = 23;
        short int sint2 = 22;

        RUN(HDFWRAP_WRITE_DATA(d,"/","INT0", 22));

        RUN(HDFWRAP_WRITE_DATA(d,"/","sINT0", sint));
        RUN(HDFWRAP_WRITE_DATA(d,"/","sINT1", sint2));

        char test_char = 'A';
        uint8_t test_char2 = 65;

        RUN(HDFWRAP_WRITE_DATA(d,"/","chartype", test_char));
        RUN(HDFWRAP_WRITE_DATA(d,"/","uint8_t", test_char2));




        int i,j,c;

        RUN(galloc(&test,10,10));
        c = 0;
        for(i = 0;i < 10;i++){
                for(j = 0;j < 10;j++){
                        test[i][j]=c;
                        c++;
                }
        }

        RUN(HDFWRAP_WRITE_DATA(d,"/","INT2D", test));

        c =1000;
        for(i = 0;i < 10;i++){
                for(j = 0;j < 10;j++){
                        test[i][j]=c;
                        c--;
                }
        }
//LOG_MSG("Got here");
        RUN(HDFWRAP_WRITE_DATA(d,"/","INT2D", test));

        gfree(test);

        test = NULL;
        float** g = NULL;
        RUN(galloc(&g,10,10));
        c = 0;
        for(i = 0;i < 10;i++){
                for(j = 0;j < 10;j++){
                        g[i][j]= (float) c / 0.9957f;
                        c++;
                }
        }
        RUN(HDFWRAP_WRITE_DATA(d,"/floaty/here","FLOAT2D", g));
        RUN(HDFWRAP_WRITE_DATA(d,"/floaty/there","FLOAT2D", g));

        int** donkey = NULL;

        RUN(HDFWRAP_READ_DATA(d,"/","INT2D", &donkey));

        int d1, d2;
        RUN(get_dim1(donkey,&d1));
        RUN(get_dim2(donkey,&d2));
        for(i = 0;i < d1;i++){
                for(j = 0;j < d2;j++){
                        fprintf(stdout,"%d ", donkey[i][j]);
                }
                fprintf(stdout,"\n");
        }




        gfree(donkey);

        H5Fflush(d->file,H5F_SCOPE_GLOBAL);

        RUN(hdf5wrap_search(d, "FLOAT2D",&string));

        if(string){
                LOG_MSG("found at %s",string);
                MFREE(string);
        }
        RUN(close_hdf5_file(&d));


        d = NULL;
        RUN(open_hdf5_file(&d,"hdf5testfile.h5"));

        sint = 0;
        sint2 = 0;



        RUN(HDFWRAP_READ_DATA(d,"/","sINT0", &sint));
        RUN(HDFWRAP_READ_DATA(d,"/","sINT1", &sint2));
        LOG_MSG("Read %d %d", sint, sint2);
        test_char = 0;
        test_char2 = 0;


        RUN(HDFWRAP_READ_DATA(d,"/","chartype", &test_char));
        RUN(HDFWRAP_READ_DATA(d,"/","uint8_t", &test_char2));
        LOG_MSG("Read %c %d", test_char, test_char2);

        LOG_MSG("Hmm got here ");


        RUN(close_hdf5_file(&d));
        LOG_MSG("Hmm got here ");
        LOG_MSG("%p %p", g, test);
        gfree(g);
        LOG_MSG("%p %p", g, test);
        gfree(test);
        return EXIT_SUCCESS;
ERROR:
        if(test){
                gfree(test);
        }
        close_hdf5_file(&d);
        return EXIT_FAILURE;
}
