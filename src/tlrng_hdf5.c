
#include "tldevel.h"
#include "tlrng_struct.h"
#include "tlrng.h"
#include "tlhdf5wrap.h"
int tl_random_write_hdf5(struct rng_state* rng,struct hdf5_data* d, char* file, char* group)
{

        ASSERT(rng != NULL, "No rng_state");


        char buf[BUFSIZ];
        if(d && file){
                ERROR_MSG("Both hdf5 data and file present - don't know which one to use.");
        }else if(d != NULL && file == NULL){
                /* writing into a file previously opened  */

                snprintf(buf, BUFSIZ,"/%s/RngState", group);
                RUN(HDFWRAP_WRITE_DATA(d, buf, "s1", rng->s[0]));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "s2", rng->s[1]));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "s3", rng->s[2]));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "s4", rng->s[3]));

                RUN(HDFWRAP_WRITE_DATA(d, buf, "gen", rng->gen));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "z1",  rng->z1));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "has_gauss", rng->has_gauss));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "gauss", rng->gauss));

        }else if(d == NULL && file != NULL){
                struct hdf5_data* d = NULL;
                RUN(open_hdf5_file(&d, file));
                snprintf(buf, BUFSIZ,"/%s/RngState", group);

                RUN(HDFWRAP_WRITE_DATA(d, buf, "s1", rng->s[0]));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "s2", rng->s[1]));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "s3", rng->s[2]));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "s4", rng->s[3]));

                RUN(HDFWRAP_WRITE_DATA(d, buf, "gen", rng->gen));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "z1",  rng->z1));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "has_gauss", rng->has_gauss));
                RUN(HDFWRAP_WRITE_DATA(d, buf, "gauss", rng->gauss));
                RUN(close_hdf5_file(&d));
        }else{
                ERROR_MSG("Neither hdf5 data or file present - don't know where to write to. ");
        }

        return OK;
ERROR:
        return FAIL;
}

int tl_random_read_hdf5(struct rng_state** rng,struct hdf5_data* d, char* file, char* group)
{

        struct rng_state* r = NULL;
        if(*rng != NULL){
                r = *rng;
        }else{
                RUNP(r = init_rng(0));
        }
        ASSERT(r != NULL, "No rng_state");
        char buf[BUFSIZ];
        if(d && file){
                ERROR_MSG("Both hdf5 data and file present - don't know which one to use.");
        }else if(d != NULL && file == NULL){
                /* writing into a file previously opened  */

                snprintf(buf, BUFSIZ,"/%s/RngState", group);

                RUN(HDFWRAP_READ_DATA(d, buf, "s1", &r->s[0]));
                RUN(HDFWRAP_READ_DATA(d, buf, "s2", &r->s[1]));
                RUN(HDFWRAP_READ_DATA(d, buf, "s3", &r->s[2]));
                RUN(HDFWRAP_READ_DATA(d, buf, "s4", &r->s[3]));

                RUN(HDFWRAP_READ_DATA(d, buf, "gen", &r->gen));
                RUN(HDFWRAP_READ_DATA(d, buf, "z1",  &r->z1));
                RUN(HDFWRAP_READ_DATA(d, buf, "has_gauss", &r->has_gauss));
                RUN(HDFWRAP_READ_DATA(d, buf, "gauss", &r->gauss));

        }else if(d == NULL && file != NULL){
                struct hdf5_data* d = NULL;
                RUN(open_hdf5_file(&d, file));
                snprintf(buf, BUFSIZ,"/%s/RngState", group);
                RUN(HDFWRAP_READ_DATA(d, buf, "s1", &r->s[0]));
                RUN(HDFWRAP_READ_DATA(d, buf, "s2", &r->s[1]));
                RUN(HDFWRAP_READ_DATA(d, buf, "s3", &r->s[2]));
                RUN(HDFWRAP_READ_DATA(d, buf, "s4", &r->s[3]));

                RUN(HDFWRAP_READ_DATA(d, buf, "gen", &r->gen));
                RUN(HDFWRAP_READ_DATA(d, buf, "z1",  &r->z1));
                RUN(HDFWRAP_READ_DATA(d, buf, "has_gauss", &r->has_gauss));
                RUN(HDFWRAP_READ_DATA(d, buf, "gauss", &r->gauss));

                RUN(close_hdf5_file(&d));
        }else{
                ERROR_MSG("Neither hdf5 data or file present - don't know where to write to. ");
        }

        *rng = r;

        return OK;
ERROR:
        return FAIL;
}
