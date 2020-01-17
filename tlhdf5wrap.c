
#include <hdf5.h>
#include <string.h>

#include "tldevel.h"
#include "tlmisc.h"


#define TLHDF5WRAP_IMPORT
#include "tlhdf5wrap.h"


#include "tlhdf5wrap_types.h"


static int hdf5wrap_open_group(struct hdf5_data* hdf5_data, char* groupname);
static int hdf5wrap_close_group(struct hdf5_data* hdf5_data);


static int alloc_hdf5_data(struct hdf5_data** h);
static void free_hdf5_data(struct hdf5_data* hdf5_data);

static int clear_hdf5_attribute(struct hdf5_attribute* h);



static int set_type_char(hid_t* type);
static int set_type_int(hid_t* type);
static int set_type_float(hid_t* type);
static int set_type_double(hid_t* type);

#define HDFWRAP_SET_TYPE(P,T) _Generic((P),     \
                char: set_type_char,            \
                char*: set_type_char,           \
                char**: set_type_char,          \
                int: set_type_int,              \
                int*: set_type_int,             \
                int**: set_type_int,            \
                float: set_type_float,          \
                float*: set_type_float,         \
                float**: set_type_float,        \
                double: set_type_double,        \
                double*: set_type_double,       \
                double**: set_type_double,      \
                default: set_type_unknown       \
)(T)


int set_type_char(hid_t* type)
{
        *type = H5T_NATIVE_CHAR;
        return OK;
}

int set_type_int(hid_t* type)
{
        *type = H5T_NATIVE_INT;
        return OK;
}

int set_type_float(hid_t* type)
{
        *type = H5T_NATIVE_FLOAT;
        return OK;
}

int set_type_double(hid_t* type)
{
        *type = H5T_NATIVE_DOUBLE;
        return OK;
}


int set_type_unknown(hid_t* type)
{

        WARNING_MSG("Could not determine type!");
        return FAIL;
}

int add_dataset_int(struct hdf5_data* hdf5_data, char* group, char* name, int** data)
{
        int i;
        int d1,d2;
        //LOG_MSG("Looking for %s", group);
        RUN(hdf5wrap_open_group(hdf5_data, group));

        //LOG_MSG("All good Looking for %s", group);

        RUN(get_dim1(data, &d1));
        RUN(get_dim2(data, &d2));

        hdf5_data->dim[0] = d1;
        hdf5_data->dim[1] = d2;
        hdf5_data->chunk_dim[0] = d1;
        hdf5_data->chunk_dim[1] = d2;

        hdf5_data->rank = 1;
        if(d2){
                hdf5_data->rank = 2;
        }

        HDFWRAP_SET_TYPE(data,&hdf5_data->native_type);

        for(i = 0; i< hdf5_data->rank;i++){
                if(hdf5_data->chunk_dim[i] >  hdf5_data->dim[i]){
                        ERROR_MSG("chunk dimenson exceeds dataset dimension:%d (rank) %d %d \n", i,hdf5_data->chunk_dim[i], hdf5_data->dim[i] );
                }
                if(hdf5_data->chunk_dim[i] == 0){
                        hdf5_data->chunk_dim[i] = hdf5_data->dim[i];
                }
        }


        snprintf(hdf5_data->dataset_name , HDF5GLUE_MAX_NAME_LEN,"%s",name);
        hdf5_data->status = H5Lexists(hdf5_data->group, hdf5_data->dataset_name, H5P_DEFAULT);
        //LOG_MSG("exist: %d", hdf5_data->status);
        if(!hdf5_data->status){
                snprintf(hdf5_data->dataset_name , HDF5GLUE_MAX_NAME_LEN,"%s",name);
                if((hdf5_data->dataspace = H5Screate_simple(hdf5_data->rank,  hdf5_data->dim , NULL)) < 0)ERROR_MSG("H5Screate_simple failed.");

                if((hdf5_data->datatype = H5Tcopy(hdf5_data->native_type )) < 0) ERROR_MSG("H5Tcopy failed");

                if((hdf5_data->status = H5Tset_order(hdf5_data->datatype, H5T_ORDER_LE)) < 0) ERROR_MSG("H5Tset_order failed.");

                if((hdf5_data->plist = H5Pcreate (H5P_DATASET_CREATE)) < 0) ERROR_MSG("H5Pcreate failed.");

                if((hdf5_data->status = H5Pset_shuffle (hdf5_data->plist )) < 0 )ERROR_MSG("H5Pset_shuffle failed.");

                if((hdf5_data->status = H5Pset_deflate (hdf5_data->plist, 2)) < 0 )ERROR_MSG("H5Pset_deflate failed.");
                if((hdf5_data->status = H5Pset_chunk (hdf5_data->plist, hdf5_data->rank,  hdf5_data->chunk_dim)) < 0 )ERROR_MSG("H5Pset_chunk failed.");


                if((hdf5_data->dataset = H5Dcreate(hdf5_data->group, hdf5_data->dataset_name, hdf5_data->datatype, hdf5_data->dataspace,    H5P_DEFAULT, hdf5_data->plist , H5P_DEFAULT)) < 0 )ERROR_MSG("H5Dcreate failed");
                //hdf5_data->plist
                if((hdf5_data->status  = H5Dwrite(hdf5_data->dataset,hdf5_data->native_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data[0][0])) < 0) ERROR_MSG("H5Dwrite failed");

                /* closing stuff */
                if((hdf5_data->status = H5Sclose(hdf5_data->dataspace)) < 0) ERROR_MSG("H5Sclose failed");
                if((hdf5_data->status = H5Pclose(hdf5_data->plist)) < 0) ERROR_MSG("H5Pclose failed");
                if((hdf5_data->status = H5Tclose(hdf5_data->datatype)) < 0) ERROR_MSG("H5Tclose failed");
                if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed");
        }else{
                if((hdf5_data->dataset = H5Dopen(hdf5_data->group,hdf5_data->dataset_name ,H5P_DEFAULT)) == -1)ERROR_MSG("H5Dopen failed\n");

                hdf5_data->datatype  = H5Dget_type(hdf5_data->dataset );     /* datatype handle */

                if(!H5Tequal(hdf5_data->datatype, hdf5_data->native_type)){
                        WARNING_MSG("Writing into an existing dataset failed");
                        WARNING_MSG("Data type is different!");
                        if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed");
                        ERROR_MSG("Overwriting failed");
                }
                LOG_MSG("%d %d", hdf5_data->datatype, hdf5_data->native_type);
                hdf5_data->dataspace = H5Dget_space(hdf5_data->dataset);
                hdf5_data->rank      = H5Sget_simple_extent_ndims(hdf5_data->dataspace);
                hdf5_data->status  = H5Sget_simple_extent_dims(hdf5_data->dataspace,hdf5_data->dim , NULL);

                if(d1 == hdf5_data->dim[0] && d2 == hdf5_data->dim[1]){
                        if((hdf5_data->status  = H5Dwrite(hdf5_data->dataset,hdf5_data->native_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data[0][0])) < 0) ERROR_MSG("H5Dwrite failed");

                }else{
                        WARNING_MSG("Writing into an existing dataset failed");
                        WARNING_MSG("Dims in file are: %d %d",hdf5_data->dim[0],hdf5_data->dim[1]);
                        WARNING_MSG("Dims in data are: %d %d",d1,d2);

                        if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed");
                        ERROR_MSG("Overwriting failed");
                }

                if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed");
        }

        hdf5wrap_close_group(hdf5_data);
        //LOG_MSG("End");
        return OK;
ERROR:
        return FAIL;
}


/*

 Example: Use the following steps to verify the existence of the link datasetD in the group group1/group2/softlink_to_group3/, where group1 is a member of the group specified by loc_id:

    First use H5Lexists to verify that group1 exists.
    If group1 exists, use H5Lexists again, this time with name set to group1/group2, to verify thatgroup2 exists.
    If group2 exists, use H5Lexists with name set to group1/group2/softlink_to_group3 to verify that softlink_to_group3 exists.

    If softlink_to_group3 exists, you can now safely use H5Lexists with name set to group1/group2/softlink_to_group3/datasetD to verify that the target link, datasetD, exists

*/
int hdf5wrap_exits(struct hdf5_data* hdf5_data, char*name)
{
        H5Lexists(hdf5_data->file, name, H5P_DEFAULT);
}


int hdf5wrap_open_group(struct hdf5_data* hdf5_data, char* groupname)
{

        char buffer[HDF5GLUE_MAX_NAME_LEN];

        int i;
        int l;

        ASSERT(hdf5_data!= NULL, "No hdf5 data");


        l = strnlen(groupname, HDF5GLUE_MAX_NAME_LEN);
        ASSERT(l != 0, "No groupname");
        //LOG_MSG("l:%d %s",l, groupname);
        ASSERT(groupname[0] == '/', "Groupname %s doesn't start with /", groupname);
        if(l > 1){
                ASSERT(groupname[l-1] != '/', "Groupname %s ends in /", groupname);
        }
        for(i = 1; i < l;i++){
                if(groupname[i] == '/'){
                        memcpy(buffer, groupname, i);
                        buffer[i] =0;
                        //fprintf(stdout,"Buffer: %s\n",buffer);

                        //hdf5_data->status = H5Eset_auto(hdf5_data->status ,NULL, NULL);
                        //hdf5_data->status = H5Gget_objinfo (hdf5_data->file, buffer, 0, NULL);
                        hdf5_data->status = H5Lexists(hdf5_data->file, buffer, H5P_DEFAULT);
                        if (hdf5_data->status == 0){
                                snprintf(hdf5_data->group_name , HDF5GLUE_MAX_NAME_LEN,"%s",buffer);
                                if((hdf5_data->group = H5Gcreate (hdf5_data->file , hdf5_data->group_name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)ERROR_MSG("H5Gcreate failed\n");

                                RUN(hdf5wrap_close_group(hdf5_data));
                        }
                }
        }
        hdf5_data->status = H5Eset_auto(hdf5_data->status ,NULL, NULL);
        hdf5_data->status = H5Gget_objinfo (hdf5_data->file, groupname, 0, NULL);

        if (hdf5_data->status == 0){
                //printf ("The group %s exists.\n",groupname );
                if((hdf5_data->group = H5Gopen(hdf5_data->file,  groupname , H5P_DEFAULT)) == -1)ERROR_MSG("H5Gopen2 failed\n");
        }else{

                //printf ("The group %s either does NOT exist\n or some other error occurred.\n",groupname);
                snprintf(hdf5_data->group_name , HDF5GLUE_MAX_NAME_LEN,"%s",groupname);
                if((hdf5_data->group = H5Gcreate (hdf5_data->file , hdf5_data->group_name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)ERROR_MSG("H5Gcreate failed\n");
        }
        return OK;
ERROR:
        return FAIL;
}

int hdf5wrap_close_group(struct hdf5_data* hdf5_data)
{
        if((hdf5_data->status = H5Gclose(hdf5_data->group)) < 0) ERROR_MSG("H5Gclose failed");

        return OK;
ERROR:
        return FAIL;
}


int hdf5wrap_add_attribute_int(struct hdf5_data* hdf5_data, char* group, char* name, int x)
{
        struct hdf5_attribute* h = NULL;
        int n = hdf5_data->num_attr;
        ASSERT(n != hdf5_data->num_attr_mem,"No space to add more attributes...");
        h = hdf5_data->attr[n];
        RUN(clear_hdf5_attribute(h));
        snprintf(h->attr_name, HDF5GLUE_MAX_NAME_LEN,"%s", name);
        h->type = HDF5GLUE_TYPE_INT;
        h->int_val = x;
        hdf5_data->num_attr++;

        RUN(hdf5_write_attributes(hdf5_data,group));
        return OK;
ERROR:
        return FAIL;
}

int hdf5wrap_add_attribute_double(struct hdf5_data* hdf5_data, char* group, char* name,double x)
{

        struct hdf5_attribute* h = NULL;
        int n = hdf5_data->num_attr;
        ASSERT(n != hdf5_data->num_attr_mem,"No space to add more attributes...");
        h = hdf5_data->attr[n];
        RUN(clear_hdf5_attribute(h));
        snprintf(h->attr_name, HDF5GLUE_MAX_NAME_LEN,"%s", name);
        h->type = HDF5GLUE_TYPE_DOUBLE;
        h->double_val = x;
        hdf5_data->num_attr++;
        RUN(hdf5_write_attributes(hdf5_data,group));
        return OK;
ERROR:
        return FAIL;
}


int hdf5wrap_add_attribute_string(struct hdf5_data* hdf5_data,char* group, char* name,char* x)
{
        struct hdf5_attribute* h = NULL;
        int n = hdf5_data->num_attr;
        ASSERT(n != hdf5_data->num_attr_mem,"No space to add more attributes...");
        h = hdf5_data->attr[n];
        RUN(clear_hdf5_attribute(h));
        snprintf(h->attr_name, HDF5GLUE_MAX_NAME_LEN,"%s", name);
        h->type = HDF5GLUE_TYPE_CHAR;
        snprintf(h->string, HDF5GLUE_MAX_CONTENT_LEN,"%s", x);
        hdf5_data->num_attr++;
        RUN(hdf5_write_attributes(hdf5_data,group));
        return OK;
ERROR:
        return FAIL;
}

int hdf5_read_attributes(struct hdf5_data* hdf5_data, char* target)
{
        H5O_info_t oinfo;
        hid_t atype,atype_mem;
        H5T_class_t type_class;
        int i;

        RUN(hdf5wrap_open_group(hdf5_data, target));


        hdf5_data->status = H5Oget_info(hdf5_data->group , &oinfo);
        hdf5_data->num_attr = 0;
        for(i = 0; i < (int)oinfo.num_attrs; i++) {

                if(i == hdf5_data->num_attr_mem){
                        break;
                }

                hdf5_data->attribute_id = H5Aopen_by_idx(hdf5_data->group, ".", H5_INDEX_CRT_ORDER, H5_ITER_INC, (hsize_t)i, H5P_DEFAULT, H5P_DEFAULT);
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
ERROR:
        return FAIL;
}

int hdf5_write_attributes(struct hdf5_data* hdf5_data, char* target)
{
        int i;
        hid_t aid;
        hid_t atype;
        hid_t attr;


        int len;

        RUN(hdf5wrap_open_group(hdf5_data, target));

        //LOG_MSG("writing: %d", hdf5_data->num_attr);
        for(i = 0;i < hdf5_data->num_attr;i++){
                switch (hdf5_data->attr[i]->type) {
                case HDF5GLUE_TYPE_CHAR:
                        if( H5Aexists(hdf5_data->group, hdf5_data->attr[i]->attr_name)){
                                H5Adelete(hdf5_data->group,hdf5_data->attr[i]->attr_name);
                        }
                        len = strnlen(hdf5_data->attr[i]->string, HDF5GLUE_MAX_CONTENT_LEN) + 1;
                        atype = H5Tcopy(H5T_C_S1);


                        aid  = H5Screate(H5S_SCALAR);
                        atype = H5Tcopy(H5T_C_S1);
                        H5Tset_size(atype, len);
                        H5Tset_strpad(atype,H5T_STR_NULLTERM);
                        attr = H5Acreate(hdf5_data->group  ,hdf5_data->attr[i]->attr_name, atype, aid, H5P_DEFAULT, H5P_DEFAULT);
                        hdf5_data->status = H5Awrite(attr, atype, hdf5_data->attr[i]->string);
                        hdf5_data->status = H5Sclose(aid);
                        hdf5_data->status = H5Tclose(atype);
                        hdf5_data->status = H5Aclose(attr);

                        break;
                case HDF5GLUE_TYPE_INT:
                        //LOG_MSG("witre: %d" ,hdf5_data->attr[i]->int_val);
                        if( H5Aexists(hdf5_data->group, hdf5_data->attr[i]->attr_name)){
                                attr = H5Aopen(hdf5_data->group,hdf5_data->attr[i]->attr_name, H5P_DEFAULT);
                                hdf5_data->status = H5Awrite(attr, H5T_NATIVE_INT, &hdf5_data->attr[i]->int_val);
                                hdf5_data->status = H5Aclose(attr);
                        }else{
                                aid  = H5Screate(H5S_SCALAR);
                                attr = H5Acreate(hdf5_data->group,hdf5_data->attr[i]->attr_name, H5T_NATIVE_INT, aid,  H5P_DEFAULT, H5P_DEFAULT);
                                if(attr < 0){
                                        ERROR_MSG("Create attribute %s failed - perhaps it exists already?");
                                }
                                hdf5_data->status = H5Awrite(attr, H5T_NATIVE_INT, &hdf5_data->attr[i]->int_val);
                                hdf5_data->status = H5Sclose(aid);
                                hdf5_data->status = H5Aclose(attr);

                        }

                        break;
                case HDF5GLUE_TYPE_DOUBLE:
                        if( H5Aexists(hdf5_data->group, hdf5_data->attr[i]->attr_name)){
                                attr = H5Aopen(hdf5_data->group,hdf5_data->attr[i]->attr_name, H5P_DEFAULT);
                                hdf5_data->status = H5Awrite(attr, H5T_NATIVE_DOUBLE, &hdf5_data->attr[i]->double_val);
                                hdf5_data->status = H5Aclose(attr);
                        }else{

                                aid  = H5Screate(H5S_SCALAR);
                                attr = H5Acreate(hdf5_data->group,hdf5_data->attr[i]->attr_name, H5T_NATIVE_DOUBLE, aid,  H5P_DEFAULT, H5P_DEFAULT);
                                hdf5_data->status = H5Awrite(attr, H5T_NATIVE_DOUBLE, &hdf5_data->attr[i]->double_val);
                                hdf5_data->status = H5Sclose(aid);
                                hdf5_data->status = H5Aclose(attr);
                        }
                        break;
                default:
                        break;
                }
        }
        hdf5_data->num_attr = 0;
        hdf5wrap_close_group(hdf5_data);
        return OK;
ERROR:
        return FAIL;
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
        if(my_file_exists(filename)){
                if((hdf5_data->file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT)) == -1)ERROR_MSG("H5Fopen failed");
        }else{
                snprintf(hdf5_data->file_name , HDF5GLUE_MAX_NAME_LEN,"%s",filename);

                if((hdf5_data->file = H5Fcreate(hdf5_data->file_name, H5F_ACC_TRUNC, H5P_DEFAULT,H5P_DEFAULT)) < 0)  ERROR_MSG("H5Fcreate failed: %s\n",hdf5_data->file_name);
        }
        *h = hdf5_data;
        return OK;
ERROR:
        return FAIL;
}

int close_hdf5_file(struct hdf5_data** h)
{
        struct hdf5_data* hdf5_data = NULL;

        hdf5_data  = *h;


        //LOG_MSG("data: %p", (void*) hdf5_data);
        //LOG_MSG("close");
        /* close everything that can be closed  */
        //if(hdf5_data->fapl){
        //if((hdf5_data->status = H5Pclose(hdf5_data->fapl)) < 0) ERROR_MSG("H5Pclose failed");
        //}
        //if(hdf5_data->file){
        //if((hdf5_data->status = H5Fclose(hdf5_data->file)) < 0) ERROR_MSG("H5Fclose failed");
        //}
        if(hdf5_data){
                free_hdf5_data(hdf5_data);
        }
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
