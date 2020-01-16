
#include <hdf5.h>
#include <string.h>

#include "tldevel.h"

#define TLHDF5WRAP_IMPORT
#include "tlhdf5wrap.h"

#include "tlhdf5wrap_types.h"



static int alloc_hdf5_data(struct hdf5_data** h);
static void free_hdf5_data(struct hdf5_data* hdf5_data);

static int clear_hdf5_attribute(struct hdf5_attribute* h);

int hdf5wrap_write_dataset(struct hdf5_data* hdf5_data, int** data, char* dataset_name)
{
        ASSERT(hdf5_data != NULL,"No HDF5 file handler");

        return OK;
ERROR:
        return FAIL;

}

int hdf5wrap_add_attribute_int(int x, struct hdf5_data* hdf5_data,char* name)
{
        struct hdf5_attribute* h = NULL;
        int n = hdf5_data->num_attr;
        ASSERT(n != hdf5_data->num_attr_mem,"No space to add more attributes...");
        h = hdf5_data->attr[n];
        RUN(clear_hdf5_attribute(h));
        snprintf(h->attr_name, HDF5GLUE_MAX_NAME_LEN,"%s", name);
        h->type = HDF5GLUE_TYPE_INT;
        h->int_val = x;
        return OK;
ERROR:
        return FAIL;
}

int hdf5wrap_add_attribute_double(double x, struct hdf5_data* hdf5_data,char* name)
{

        struct hdf5_attribute* h = NULL;
        int n = hdf5_data->num_attr;
        ASSERT(n != hdf5_data->num_attr_mem,"No space to add more attributes...");
        h = hdf5_data->attr[n];
        RUN(clear_hdf5_attribute(h));
        snprintf(h->attr_name, HDF5GLUE_MAX_NAME_LEN,"%s", name);
        h->type = HDF5GLUE_TYPE_DOUBLE;
        h->double_val = x;
        return OK;
ERROR:
        return FAIL;
}


int hdf5wrap_add_attribute_string(char* x, struct hdf5_data* hdf5_data,char* name)
{
        struct hdf5_attribute* h = NULL;
        int n = hdf5_data->num_attr;
        ASSERT(n != hdf5_data->num_attr_mem,"No space to add more attributes...");
        h = hdf5_data->attr[n];
        RUN(clear_hdf5_attribute(h));
        snprintf(h->attr_name, HDF5GLUE_MAX_NAME_LEN,"%s", name);
        h->type = HDF5GLUE_TYPE_CHAR;
        snprintf(h->string, HDF5GLUE_MAX_CONTENT_LEN,"%s", x);
        return OK;
ERROR:
        return FAIL;
}

int hdf5_add_attribute(struct hdf5_data* hdf5_data,char* attr_name, char* string, int int_val, double double_val, int type)
{
        int n = hdf5_data->num_attr;

        ASSERT(n != hdf5_data->num_attr_mem,"No space to add more attributes...");
        //if(n == hdf5_data->num_attr_mem){
        //	KSLIB_EXCEPTION(kslFAIL,"No space to add more attributes...");
        //}

        snprintf(hdf5_data->attr[n]->attr_name, HDF5GLUE_MAX_NAME_LEN,"%s", attr_name);

        hdf5_data->attr[n]->type = type;

        if(string){
                snprintf(hdf5_data->attr[n]->string, HDF5GLUE_MAX_CONTENT_LEN,"%s", string);
        }
        hdf5_data->attr[n]->int_val = int_val;
        hdf5_data->attr[n]->double_val = double_val;


        hdf5_data->num_attr++;

        return OK;
ERROR:
        return FAIL;
}


int hdf5_read_attributes(struct hdf5_data* hdf5_data,hid_t target)
{
        H5O_info_t oinfo;
        hid_t atype,atype_mem;
        H5T_class_t type_class;
        int i;
        hdf5_data->status = H5Oget_info(target, &oinfo);
        hdf5_data->num_attr = 0;
        for(i = 0; i < (unsigned)oinfo.num_attrs; i++) {

                if(i == hdf5_data->num_attr_mem){
                        break;
                }

                hdf5_data->attribute_id = H5Aopen_by_idx(target, ".", H5_INDEX_CRT_ORDER, H5_ITER_INC, (hsize_t)i, H5P_DEFAULT, H5P_DEFAULT);
                atype = H5Aget_type(hdf5_data->attribute_id);
                type_class = H5Tget_class(atype);
                H5Aget_name(hdf5_data->attribute_id , HDF5GLUE_MAX_NAME_LEN  , hdf5_data->attr[i]->attr_name);

                if (type_class == H5T_STRING) {
                        hdf5_data->attr[i]->type = HDF5GLUE_TYPE_CHAR;
                        atype_mem = H5Tget_native_type(atype, H5T_DIR_ASCEND);
                        hdf5_data->status   = H5Aread(hdf5_data->attribute_id, atype_mem, hdf5_data->attr[i]->string  );
                        hdf5_data->status   = H5Tclose(atype_mem);
                }

                if(type_class == H5T_INTEGER){
                        hdf5_data->attr[i]->type = HDF5GLUE_TYPE_INT;
                        hdf5_data->status = H5Aread(hdf5_data->attribute_id, H5T_NATIVE_INT, &hdf5_data->attr[i]->int_val);
                }
                if(type_class == H5T_FLOAT){
                        hdf5_data->attr[i]->type = HDF5GLUE_TYPE_DOUBLE;
                        hdf5_data->status = H5Aread(hdf5_data->attribute_id, H5T_NATIVE_DOUBLE, &hdf5_data->attr[i]->double_val);
                }

                hdf5_data->num_attr++;


                hdf5_data->status   = H5Aclose(hdf5_data->attribute_id);
                hdf5_data->status   = H5Tclose(atype);
        }
        return OK;
}

int hdf5_write_attributes(struct hdf5_data* hdf5_data,hid_t target)
{
        int i;
        hid_t aid;
        hid_t atype;
        hid_t attr;


        int len;

        for(i = 0;i < hdf5_data->num_attr;i++){
                switch (hdf5_data->attr[i]->type) {
                case HDF5GLUE_TYPE_CHAR:

                        len = (int) strlen(hdf5_data->attr[i]->string) + 1;

                        aid  = H5Screate(H5S_SCALAR);
                        atype = H5Tcopy(H5T_C_S1);
                        H5Tset_size(atype, len);
                        H5Tset_strpad(atype,H5T_STR_NULLTERM);
                        attr = H5Acreate2(target,hdf5_data->attr[i]->attr_name, atype, aid, H5P_DEFAULT, H5P_DEFAULT);


                        hdf5_data->status = H5Awrite(attr, atype, hdf5_data->attr[i]->string);

                        hdf5_data->status = H5Sclose(aid);
                        hdf5_data->status = H5Tclose(atype);
                        hdf5_data->status = H5Aclose(attr);
                        break;
                case HDF5GLUE_TYPE_INT:

                        aid  = H5Screate(H5S_SCALAR);
                        attr = H5Acreate2(target,hdf5_data->attr[i]->attr_name, H5T_NATIVE_INT, aid,  H5P_DEFAULT, H5P_DEFAULT);

                        hdf5_data->status = H5Awrite(attr, H5T_NATIVE_INT, &hdf5_data->attr[i]->int_val);
                        hdf5_data->status = H5Sclose(aid);
                        hdf5_data->status = H5Aclose(attr);

                        break;
                case HDF5GLUE_TYPE_DOUBLE:
                        aid  = H5Screate(H5S_SCALAR);
                        attr = H5Acreate2(target,hdf5_data->attr[i]->attr_name, H5T_NATIVE_DOUBLE, aid,  H5P_DEFAULT, H5P_DEFAULT);

                        hdf5_data->status = H5Awrite(attr, H5T_NATIVE_DOUBLE, &hdf5_data->attr[i]->double_val);
                        hdf5_data->status = H5Sclose(aid);
                        hdf5_data->status = H5Aclose(attr);
                        break;
                default:
                        break;
                }
        }
        hdf5_data->num_attr = 0;
        return OK;
}





int open_hdf5_file(struct hdf5_data** h, char* filename)
{
        struct hdf5_data* hdf5_data = NULL;

        hdf5_data  = *h;
        if(hdf5_data){
                /* I should close all groups / file  */
        }else{
                RUN(alloc_hdf5_data(&hdf5_data));
        }
        if((hdf5_data->file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT)) == -1)ERROR_MSG("H5Fopen failed");
        return OK;
ERROR:
        return FAIL;
}

int hdf5_close_file(struct hdf5_data** h)
{
        struct hdf5_data* hdf5_data = NULL;

        hdf5_data  = *h;
        //LOG_MSG("close");
        /* close everything that can be closed  */
        if(hdf5_data->fapl){
                if((hdf5_data->status = H5Pclose(hdf5_data->fapl)) < 0) ERROR_MSG("H5Pclose failed");
        }
        if(hdf5_data->file){
                if((hdf5_data->status = H5Fclose(hdf5_data->file)) < 0) ERROR_MSG("H5Fclose failed");
        }

        free_hdf5_data(hdf5_data);
        *h = NULL;
        return OK;
ERROR:
        return FAIL;
}


int alloc_hdf5_data(struct hdf5_data** h)
{
        int i;
        struct hdf5_data* hdf5_data = NULL;


        MMALLOC(hdf5_data, sizeof(struct hdf5_data));

        hdf5_data->attr = NULL;
        hdf5_data->num_attr = 0;
        hdf5_data->num_attr_mem = 128;

        MMALLOC(hdf5_data->attr , sizeof(struct hdf5_attribute*) * hdf5_data->num_attr_mem);


        for(i = 0; i < hdf5_data->num_attr_mem;i++){
                hdf5_data->attr[i] = NULL;
                MMALLOC(hdf5_data->attr[i],  sizeof(struct hdf5_attribute));
        }

        for(i = 0; i < HDF5GLUE_MAX_DIM;i++){
                hdf5_data->dim[i] = 0;
                hdf5_data->chunk_dim[i] = 0;
        }

        hdf5_data->grp_names = NULL;
        hdf5_data->data = 0;

        hdf5_data->fapl = 0;
        hdf5_data->file = 0;
        hdf5_data->group = 0;

        hdf5_data->plist = 0;
        hdf5_data->dataset = 0;

        hdf5_data->attribute_id = 0;
        hdf5_data->attr_dataspace_id = 0;


        hdf5_data->datatype = 0;
        hdf5_data->dataspace = 0;

        hdf5_data->status = 0;

        hdf5_data->rank = 0;
        *h = hdf5_data;

        return OK;
ERROR:
        free_hdf5_data(hdf5_data);
        return FAIL;
}

void free_hdf5_data(struct hdf5_data* hdf5_data)
{

        int i;
        if(hdf5_data){
                for(i = 0; i < hdf5_data->num_attr_mem;i++){
                        //hdf5_data->attr = NULL;
                        MFREE(hdf5_data->attr[i]);//, sizeof(struct hdf5_attribute));

                }
                MFREE(hdf5_data->attr);// , sizeof(struct hdf5_attribute*) * hdf5_data->num_attr_mem);

                if(hdf5_data->grp_names){
                        gfree(hdf5_data->grp_names->names);
                        MFREE(hdf5_data->grp_names);
                }

                MFREE(hdf5_data);
        }
}


int clear_hdf5_attribute(struct hdf5_attribute* h)
{
        ASSERT(h != NULL, "No attribute");
        h->int_val = 0;
        h->double_val = 0.0;
        h->type = HDF5GLUE_TYPE_UNKNOWN;
        return OK;
ERROR:
        return FAIL;
}
