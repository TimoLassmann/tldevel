#ifndef TLHDF5WRAP_TYPES_H
#define TLHDF5WRAP_TYPES_H

#include <hdf5.h>

#define HDF5GLUE_MAX_NAME_LEN 200
#define HDF5GLUE_MAX_DIM 5
#define HDF5GLUE_MAX_CONTENT_LEN 1024


#define HDF5GLUE_TYPE_UNKNOWN 0
#define HDF5GLUE_TYPE_CHAR 1
#define HDF5GLUE_TYPE_INT 2
#define HDF5GLUE_TYPE_USHORT_INT 3

#define HDF5GLUE_TYPE_FLOAT 4
#define HDF5GLUE_TYPE_DOUBLE 5


struct hdf5_attribute{
        char attr_name[HDF5GLUE_MAX_NAME_LEN];
        char string[HDF5GLUE_MAX_CONTENT_LEN];
        int int_val;
        double double_val;
        int type;
};

struct hdf5_group_names{
        char** names;
        int num_names;
        int name_length;
        int num_names_mem;
};


struct hdf5_data{
        char dataset_name[HDF5GLUE_MAX_NAME_LEN];
        char group_name[HDF5GLUE_MAX_NAME_LEN];
        char file_name[HDF5GLUE_MAX_NAME_LEN];
        char tmp_name[HDF5GLUE_MAX_NAME_LEN];

        hsize_t dim[HDF5GLUE_MAX_DIM];
        hsize_t chunk_dim[HDF5GLUE_MAX_DIM];
        struct hdf5_group_names* grp_names;
        struct hdf5_attribute** attr;
        void* data;
        int num_attr;
        int num_attr_mem;
        int rank;

        hid_t fapl;
        hid_t	file;
        hid_t group;

        hid_t plist;
        hid_t dataset;

        hid_t attribute_id;
        hid_t attr_dataspace_id;

        hid_t datatype;
        hid_t dataspace;
        hid_t native_type;
        herr_t status;
};


#endif
