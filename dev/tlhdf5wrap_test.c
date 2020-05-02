
#include "tldevel.h"

#include "tlhdf5wrap.h"
#include "tlhdf5wrap_types.h"

int main(int argc, char *argv[])
{
        struct hdf5_data* d = NULL;

        RUN(open_hdf5_file(&d,"hdf5testfile.h5"));

        RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/","fourty four",44));
        RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/","otto","otto"));
        RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/","pi",3.14));

        RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/group10/subsectionA","otto","otto"));

        RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/g1/g2/g4","pi",3.14));
        RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/g1/g2/g4","piINT",314));

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



        int** test = NULL;
        int i,j,c;
        RUN(galloc(&test,10,10));
        c = 0;
        for(i = 0;i < 10;i++){
                for(j = 0;j < 10;j++){
                        test[i][j]=c;
                        c++;
                }
        }
        //LOG_MSG("Got here");
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
        RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/floaty/here","FLT_ATTR", "This is a float matrix"));
        //RUN(add_dataset_int(d,"/","INT2D", g));

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


        char* string = NULL;
        RUN(HDFWRAP_READ_ATTRIBUTE(d,"/floaty/here","FLT_ATTR", &string));
        LOG_MSG("atrribute : %s",string);

        MFREE(string);
        string  = NULL;


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


        //int x_int;
        //double x_d;

        //RUN(HDFWRAP_WRITE_ATTRIBUTE(d,"/g1/g2/g4","pi",3.14));
        //hdf5_read_attributes_test(d,"/g1/g2/g4","pi",&x_int);
        //hdf5_read_attributes_test(d,"/g1/g2/g4","piINT",&x_int);
        //hdf5_read_attributes_test(d,"/g1/g2/g4","pi",&x_d);
        //hdf5_read_attributes_test(d,"/g1/g2/g4","piINT",&x_d);


        RUN(close_hdf5_file(&d));
        gfree(g);
        gfree(test);
        return EXIT_SUCCESS;
ERROR:
        gfree(test);
        close_hdf5_file(&d);
        return EXIT_FAILURE;
}
