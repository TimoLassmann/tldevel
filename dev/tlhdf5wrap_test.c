
#include "tldevel.h"

#include "tlhdf5wrap.h"
#include "tlhdf5wrap_types.h"

int main(int argc, char *argv[])
{
        struct hdf5_data* d = NULL;

        RUN(open_hdf5_file(&d,"hdf5testfile.h5"));

        RUN(HDFWRAP_ADD_ATTRIBUTE(d,"/","fourty four",44));
        RUN(HDFWRAP_ADD_ATTRIBUTE(d,"/","otto","otto"));

            RUN(HDFWRAP_ADD_ATTRIBUTE(d,"/group10/subsectionA","otto","otto"));

            RUN(HDFWRAP_ADD_ATTRIBUTE(d,"/g1/g2/g4","pi",3.14));

        //RUN(hdf5wrap)
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
        RUN(add_dataset_int(d,"/","INT2D", test));

        c =1000;
        for(i = 0;i < 10;i++){
                for(j = 0;j < 10;j++){
                        test[i][j]=c;
                        c--;
                }
        }
        //LOG_MSG("Got here");
        RUN(add_dataset_int(d,"/","INT2D", test));

        gfree(test);

        test = NULL;
        float** g = NULL;
        RUN(galloc(&g,10,10));
        c = 0;
        for(i = 0;i < 10;i++){
                for(j = 0;j < 10;j++){
                        g[i][j]=c;
                        c++;
                }
        }
        //RUN(add_dataset_int(d,"/","INT2D", g));
        close_hdf5_file(&d);
        gfree(g);
        gfree(test);
        return EXIT_SUCCESS;
ERROR:
        gfree(test);
        close_hdf5_file(&d);
        return EXIT_FAILURE;

}
