#include <hdf5.h>
#include <string.h>

#include "tldevel.h"
#include "tlmisc.h"


#define TLHDF5WRAP_IMPORT
#include "tlhdf5wrap.h"


#include "tlhdf5wrap_types.h"

/* important intermediate functions */

/* open / close groups - these are called by all MACROS:

HDFWRAP_WRITE_DATA
HDFWRAP_READ_DATA
HDFWRAP_WRITE_ATTRIBUTE
HDFWRAP_READ_ATTRIBUTE

to resolve the path to the data / attribute

 */
static int hdf5wrap_open_group(struct hdf5_data* hdf5_data, char* groupname);
static int hdf5wrap_close_group(struct hdf5_data* hdf5_data);

/* Functions to read write attributes - called by:

HDFWRAP_WRITE_ATTRIBUTE
HDFWRAP_READ_ATTRIBUTE

 */


//static int hdf5_read_attributes(struct hdf5_data* hdf5_data, char* group);
//static int clear_hdf5_attribute(struct hdf5_attribute* h);

/* allocating the hdf5 data structure - this is really more of a file handler...  */
static int alloc_hdf5_data(struct hdf5_data** h);
static void free_hdf5_data(struct hdf5_data* hdf5_data);


/* to write an array / matrix in hdf5 I need to pass a reference to first start of the memory block.
   this is &x[0] for arrays and &x[0][0] for things allocated with galloc.
   The function below + the generic function allow this to happen.
 */

#define STARTOF_DATA(type)                                      \
        static int startof_galloc_ ##type(type x, void** ptr);  \
        static int startof_galloc_ ##type##_s(type* x, void** ptr);  \
        static int startof_galloc_ ##type##_ss(type** x, void** ptr);

STARTOF_DATA(char)
STARTOF_DATA(int8_t)
STARTOF_DATA(uint8_t)
STARTOF_DATA(int16_t)
STARTOF_DATA(uint16_t)
STARTOF_DATA(int32_t)
STARTOF_DATA(uint32_t)
STARTOF_DATA(int64_t)
STARTOF_DATA(uint64_t)
STARTOF_DATA(float)
STARTOF_DATA(double)

#undef STARTOF_DATA

static int startof_galloc_unknown(void* x, void** ptr);

#define HDFWRAP_START_GALLOC(P,T) _Generic((P),                         \
                                           char: startof_galloc_char,   \
                                           char*: startof_galloc_char_s, \
                                           char**: startof_galloc_char_ss, \
                                           int8_t: startof_galloc_int8_t, \
                                           int8_t*: startof_galloc_int8_t_s, \
                                           int8_t**: startof_galloc_int8_t_ss, \
                                           uint8_t: startof_galloc_uint8_t, \
                                           uint8_t*: startof_galloc_uint8_t_s, \
                                           uint8_t**: startof_galloc_uint8_t_ss, \
                                           int16_t: startof_galloc_int16_t, \
                                           int16_t*: startof_galloc_int16_t_s, \
                                           int16_t**: startof_galloc_int16_t_ss, \
                                           uint16_t: startof_galloc_uint16_t, \
                                           uint16_t*: startof_galloc_uint16_t_s, \
                                           uint16_t**: startof_galloc_uint16_t_ss, \
                                           int32_t: startof_galloc_int32_t, \
                                           int32_t*: startof_galloc_int32_t_s, \
                                           int32_t**: startof_galloc_int32_t_ss, \
                                           uint32_t: startof_galloc_uint32_t, \
                                           uint32_t*: startof_galloc_uint32_t_s, \
                                           uint32_t**: startof_galloc_uint32_t_ss, \
                                           int64_t: startof_galloc_int64_t, \
                                           int64_t*: startof_galloc_int64_t_s, \
                                           int64_t**: startof_galloc_int64_t_ss, \
                                           uint64_t: startof_galloc_uint64_t, \
                                           uint64_t*: startof_galloc_uint64_t_s, \
                                           uint64_t**: startof_galloc_uint64_t_ss, \
                                           float: startof_galloc_float, \
                                           float*: startof_galloc_float_s, \
                                           float**: startof_galloc_float_ss, \
                                           double: startof_galloc_double, \
                                           double*: startof_galloc_double_s, \
                                           double**: startof_galloc_double_ss, \
                                           default: startof_galloc_unknown \
                )(P,T)

/* These functions determine the native hdf5 data type from the C data type  */

/*
  H5T_NATIVE_INT    int
  H5T_NATIVE_UINT   unsigned
  H5T_NATIVE_LONG   long
  H5T_NATIVE_ULONG  unsigned long
H5T_NATIVE_LLONG    long long
H5T_NATIVE_ULLONG   unsigned long long




*/
#define SETTYPR(t)                                      \
        static int set_type_ ## t(hid_t* type);

SETTYPR(char)
SETTYPR(int8_t)
SETTYPR(uint8_t)
SETTYPR(int16_t)
SETTYPR(uint16_t)
SETTYPR(int32_t)
SETTYPR(uint32_t)
SETTYPR(int64_t)
SETTYPR(uint64_t)
SETTYPR(float)
SETTYPR(double)

#undef SETTYPR

static int set_type_unknown(hid_t* type);



#define HDFWRAP_SET_TYPE(P,T) _Generic((P),                             \
                                       char: set_type_char,             \
                                       char*: set_type_char,            \
                                       char**: set_type_char,           \
                                       char***: set_type_char,          \
                                       int8_t: set_type_int8_t,         \
                                       int8_t*: set_type_int8_t,        \
                                       int8_t**: set_type_int8_t,       \
                                       int8_t***: set_type_int8_t,      \
                                       uint8_t: set_type_uint8_t,       \
                                       uint8_t*: set_type_uint8_t,      \
                                       uint8_t**: set_type_uint8_t,     \
                                       uint8_t***: set_type_uint8_t,    \
                                       int16_t: set_type_int16_t,       \
                                       int16_t*: set_type_int16_t,      \
                                       int16_t**: set_type_int16_t,     \
                                       int16_t***: set_type_int16_t,    \
                                       uint16_t: set_type_uint16_t,     \
                                       uint16_t*: set_type_uint16_t,    \
                                       uint16_t**: set_type_uint16_t,   \
                                       uint16_t***: set_type_uint16_t,  \
                                       int32_t: set_type_int32_t,       \
                                       int32_t*: set_type_int32_t,      \
                                       int32_t**: set_type_int32_t,     \
                                       int32_t***: set_type_int32_t,    \
                                       uint32_t: set_type_uint32_t,     \
                                       uint32_t*: set_type_uint32_t,    \
                                       uint32_t**: set_type_uint32_t,   \
                                       uint32_t***: set_type_uint32_t,  \
                                       int64_t: set_type_int64_t,       \
                                       int64_t*: set_type_int64_t,      \
                                       int64_t**: set_type_int64_t,     \
                                       int64_t***: set_type_int64_t,    \
                                       uint64_t: set_type_uint64_t,     \
                                       uint64_t*: set_type_uint64_t,    \
                                       uint64_t**: set_type_uint64_t,   \
                                       uint64_t***: set_type_uint64_t,  \
                                       float: set_type_float,           \
                                       float*: set_type_float,          \
                                       float**: set_type_float,         \
                                       float***: set_type_float,        \
                                       double: set_type_double,         \
                                       double*: set_type_double,        \
                                       double**: set_type_double,       \
                                       double***: set_type_double,      \
                                       default: set_type_unknown        \
                )(T)

#define HDF5WRAP_ADD_SINGLE_BODY do {                                   \
                int i;                                                  \
                                                                        \
                RUN(hdf5wrap_open_group(hdf5_data, group));             \
                                                                        \
                                                                        \
                hdf5_data->dim[0] = 1;                                  \
                hdf5_data->dim[1] = 0;                                  \
                hdf5_data->chunk_dim[0] = 1;                            \
                hdf5_data->chunk_dim[1] = 0;                            \
                                                                        \
                hdf5_data->rank = 1;                                    \
                                                                        \
                HDFWRAP_SET_TYPE(data,&hdf5_data->native_type);         \
                                                                        \
                for(i = 0; i< hdf5_data->rank;i++){                     \
                        if(hdf5_data->chunk_dim[i] >  hdf5_data->dim[i]){ \
                                ERROR_MSG("chunk dimenson exceeds dataset dimension:%d (rank) %d %d \n", i,hdf5_data->chunk_dim[i], hdf5_data->dim[i] ); \
                        }                                               \
                        if(hdf5_data->chunk_dim[i] == 0){               \
                                hdf5_data->chunk_dim[i] = hdf5_data->dim[i]; \
                        }                                               \
                }                                                       \
                                                                        \
                                                                        \
                snprintf(hdf5_data->dataset_name , HDF5GLUE_MAX_NAME_LEN,"%s",name); \
                hdf5_data->status = H5Lexists(hdf5_data->group, hdf5_data->dataset_name, H5P_DEFAULT); \
                                                                        \
                if(!hdf5_data->status){                                 \
                        snprintf(hdf5_data->dataset_name , HDF5GLUE_MAX_NAME_LEN,"%s",name); \
                        if((hdf5_data->dataspace = H5Screate_simple(hdf5_data->rank,  hdf5_data->dim , NULL)) < 0)ERROR_MSG("H5Screate_simple failed."); \
                                                                        \
                        if((hdf5_data->datatype = H5Tcopy(hdf5_data->native_type )) < 0) ERROR_MSG("H5Tcopy failed"); \
                                                                        \
                        if((hdf5_data->status = H5Tset_order(hdf5_data->datatype, H5T_ORDER_LE)) < 0) ERROR_MSG("H5Tset_order failed."); \
                                                                        \
                        if((hdf5_data->plist = H5Pcreate (H5P_DATASET_CREATE)) < 0) ERROR_MSG("H5Pcreate failed."); \
                                                                        \
                        if((hdf5_data->status = H5Pset_shuffle (hdf5_data->plist )) < 0 )ERROR_MSG("H5Pset_shuffle failed."); \
                                                                        \
                        if((hdf5_data->status = H5Pset_deflate (hdf5_data->plist, 2)) < 0 )ERROR_MSG("H5Pset_deflate failed."); \
                        if((hdf5_data->status = H5Pset_chunk (hdf5_data->plist, hdf5_data->rank,  hdf5_data->chunk_dim)) < 0 )ERROR_MSG("H5Pset_chunk failed."); \
                                                                        \
                                                                        \
                        if((hdf5_data->dataset = H5Dcreate(hdf5_data->group, \
                                                           hdf5_data->dataset_name, \
                                                           hdf5_data->datatype, \
                                                           hdf5_data->dataspace, \
                                                           H5P_DEFAULT, \
                                                           hdf5_data->plist, H5P_DEFAULT)) < 0 )ERROR_MSG("H5Dcreate failed"); \
                                                                        \
                        if((hdf5_data->status  = H5Dwrite(hdf5_data->dataset,hdf5_data->native_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void*)&data)) < 0) ERROR_MSG("H5Dwrite failed"); \
                                                                        \
                        /* closing stuff */                             \
                        if((hdf5_data->status = H5Pclose(hdf5_data->plist)) < 0) ERROR_MSG("H5Pclose failed"); \
                        if((hdf5_data->status = H5Tclose(hdf5_data->datatype)) < 0) ERROR_MSG("H5Tclose failed"); \
                        if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                        if((hdf5_data->status = H5Sclose(hdf5_data->dataspace)) < 0) ERROR_MSG("H5Sclose failed"); \
                }else{                                                  \
                        if((hdf5_data->dataset = H5Dopen(hdf5_data->group,hdf5_data->dataset_name ,H5P_DEFAULT)) == -1)ERROR_MSG("H5Dopen failed\n"); \
                                                                        \
                        hdf5_data->datatype  = H5Dget_type(hdf5_data->dataset );     /* datatype handle */ \
                                                                        \
                        if(!H5Tequal(hdf5_data->datatype, hdf5_data->native_type)){ \
                                WARNING_MSG("Writing into an existing dataset failed"); \
                                WARNING_MSG("Data type is different!"); \
                                if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                                ERROR_MSG("Overwriting failed");        \
                        }                                               \
                        hdf5_data->dataspace = H5Dget_space(hdf5_data->dataset); \
                        hdf5_data->rank      = H5Sget_simple_extent_ndims(hdf5_data->dataspace); \
                        hdf5_data->status  = H5Sget_simple_extent_dims(hdf5_data->dataspace,hdf5_data->dim , NULL); \
                                                                        \
                        if(1== (int) hdf5_data->dim[0] && 0 == (int)hdf5_data->dim[1]){ \
                                if((hdf5_data->status  = H5Dwrite(hdf5_data->dataset,hdf5_data->native_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void*) &data)) < 0) ERROR_MSG("H5Dwrite failed"); \
                                                                        \
                                                                        \
                        }else{                                          \
                                WARNING_MSG("Writing into an existing dataset failed"); \
                                WARNING_MSG("Dims in file are: %d %d",hdf5_data->dim[0],hdf5_data->dim[1]); \
                                WARNING_MSG("Dims in data are: %d %d",1,0); \
                                                                        \
                                if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                                ERROR_MSG("Overwriting failed");        \
                        }                                               \
                                                                        \
                        if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                }                                                       \
                                                                        \
                hdf5wrap_close_group(hdf5_data);                        \
                return OK;                                              \
        ERROR:                                                          \
                return FAIL;                                            \
        } while (0)


#define ADD_ARRAY(type)                                                 \
        int hdf5wrap_add_0D_dataset_ ##type(struct hdf5_data* hdf5_data, char* group, char* name, type data) \
        {                                                               \
                HDF5WRAP_ADD_SINGLE_BODY;                                      \
        }

ADD_ARRAY(char)
ADD_ARRAY(int8_t)
ADD_ARRAY(uint8_t)
ADD_ARRAY(int16_t)
ADD_ARRAY(uint16_t)
ADD_ARRAY(int32_t)
ADD_ARRAY(uint32_t)
ADD_ARRAY(int64_t)
ADD_ARRAY(uint64_t)
ADD_ARRAY(float)
ADD_ARRAY(double)



#undef ADD_ARRAY

/* This is the body of the ADD data functions */
/* I separated this out because it is shared among all 'add' functions (which I should really rename to read) */
#define HDF5WRAP_ADD_BODY do {                                          \
        int i;                                                          \
        int d1,d2;                                                      \
        void* ptr = NULL;                                               \
                                                                        \
        RUN(hdf5wrap_open_group(hdf5_data, group));                     \
                                                                        \
        RUN(get_dim1(data, &d1));                                       \
        RUN(get_dim2(data, &d2));                                       \
                                                                        \
        hdf5_data->dim[0] = d1;                                         \
        hdf5_data->dim[1] = d2;                                         \
        hdf5_data->chunk_dim[0] = d1;                                   \
        hdf5_data->chunk_dim[1] = d2;                                   \
                                                                        \
        hdf5_data->rank = 1;                                            \
        if(d2){                                                         \
                hdf5_data->rank = 2;                                    \
        }                                                               \
                                                                        \
        HDFWRAP_SET_TYPE(data,&hdf5_data->native_type);                 \
                                                                        \
        for(i = 0; i< hdf5_data->rank;i++){                             \
                if(hdf5_data->chunk_dim[i] >  hdf5_data->dim[i]){       \
                        ERROR_MSG("chunk dimenson exceeds dataset dimension:%d (rank) %d %d \n", i,hdf5_data->chunk_dim[i], hdf5_data->dim[i] ); \
                }                                                       \
                if(hdf5_data->chunk_dim[i] == 0){                       \
                        hdf5_data->chunk_dim[i] = hdf5_data->dim[i];    \
                }                                                       \
        }                                                               \
                                                                        \
                                                                        \
        snprintf(hdf5_data->dataset_name , HDF5GLUE_MAX_NAME_LEN,"%s",name); \
        hdf5_data->status = H5Lexists(hdf5_data->group, hdf5_data->dataset_name, H5P_DEFAULT); \
                                                                        \
        if(!hdf5_data->status){                                         \
        snprintf(hdf5_data->dataset_name , HDF5GLUE_MAX_NAME_LEN,"%s",name); \
        if((hdf5_data->dataspace = H5Screate_simple(hdf5_data->rank,  hdf5_data->dim , NULL)) < 0)ERROR_MSG("H5Screate_simple failed."); \
                                                                        \
        if((hdf5_data->datatype = H5Tcopy(hdf5_data->native_type )) < 0) ERROR_MSG("H5Tcopy failed"); \
                                                                        \
        if((hdf5_data->status = H5Tset_order(hdf5_data->datatype, H5T_ORDER_LE)) < 0) ERROR_MSG("H5Tset_order failed."); \
                                                                        \
        if((hdf5_data->plist = H5Pcreate (H5P_DATASET_CREATE)) < 0) ERROR_MSG("H5Pcreate failed."); \
                                                                        \
        if((hdf5_data->status = H5Pset_shuffle (hdf5_data->plist )) < 0 )ERROR_MSG("H5Pset_shuffle failed."); \
                                                                        \
        if((hdf5_data->status = H5Pset_deflate (hdf5_data->plist, 2)) < 0 )ERROR_MSG("H5Pset_deflate failed."); \
        if((hdf5_data->status = H5Pset_chunk (hdf5_data->plist, hdf5_data->rank,  hdf5_data->chunk_dim)) < 0 )ERROR_MSG("H5Pset_chunk failed."); \
                                                                        \
                                                                        \
        if((hdf5_data->dataset = H5Dcreate(hdf5_data->group,            \
                                           hdf5_data->dataset_name,     \
                                           hdf5_data->datatype,         \
                                           hdf5_data->dataspace,        \
                                           H5P_DEFAULT,                 \
                                           hdf5_data->plist, H5P_DEFAULT)) < 0 )ERROR_MSG("H5Dcreate failed"); \
                                                                        \
        HDFWRAP_START_GALLOC(data,&ptr);                                \
        if((hdf5_data->status  = H5Dwrite(hdf5_data->dataset,hdf5_data->native_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, ptr)) < 0) ERROR_MSG("H5Dwrite failed"); \
                                                                        \
        /* closing stuff */                                             \
        if((hdf5_data->status = H5Sclose(hdf5_data->dataspace)) < 0) ERROR_MSG("H5Sclose failed"); \
        if((hdf5_data->status = H5Pclose(hdf5_data->plist)) < 0) ERROR_MSG("H5Pclose failed"); \
        if((hdf5_data->status = H5Tclose(hdf5_data->datatype)) < 0) ERROR_MSG("H5Tclose failed"); \
        if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
        }else{                                                          \
                 if((hdf5_data->dataset = H5Dopen(hdf5_data->group,hdf5_data->dataset_name ,H5P_DEFAULT)) == -1)ERROR_MSG("H5Dopen failed\n"); \
                                                                        \
                 hdf5_data->datatype  = H5Dget_type(hdf5_data->dataset );     /* datatype handle */ \
                                                                        \
                 if(!H5Tequal(hdf5_data->datatype, hdf5_data->native_type)){ \
                         WARNING_MSG("Writing into an existing dataset failed"); \
                         WARNING_MSG("Data type is different!");        \
                         if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                         ERROR_MSG("Overwriting failed");               \
                 }                                                      \
                 hdf5_data->dataspace = H5Dget_space(hdf5_data->dataset); \
                 hdf5_data->rank      = H5Sget_simple_extent_ndims(hdf5_data->dataspace); \
                 hdf5_data->status  = H5Sget_simple_extent_dims(hdf5_data->dataspace,hdf5_data->dim , NULL); \
                                                                        \
                 HDFWRAP_START_GALLOC(data,&ptr);                       \
                 if(d1 == (int) hdf5_data->dim[0] && d2 == (int)hdf5_data->dim[1]){ \
                         if((hdf5_data->status  = H5Dwrite(hdf5_data->dataset,hdf5_data->native_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, ptr)) < 0) ERROR_MSG("H5Dwrite failed"); \
                                                                        \
                                                                        \
                 }else{                                                 \
                         WARNING_MSG("Writing into an existing dataset failed"); \
                         WARNING_MSG("Dims in file are: %d %d",hdf5_data->dim[0],hdf5_data->dim[1]); \
                         WARNING_MSG("Dims in data are: %d %d",d1,d2);  \
                                                                        \
                         if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                         ERROR_MSG("Overwriting failed");               \
                 }                                                      \
                                                                        \
                 if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
         }                                                              \
                                                                        \
        hdf5wrap_close_group(hdf5_data);                                \
        return OK;                                                      \
        ERROR:                                                          \
        return FAIL;                                                    \
        } while (0)

/* The add functions - the input parameters will have different types but the rest is the same  */

#define ADD_ARRAY(type)                                                 \
        int hdf5wrap_add_1D_dataset_ ##type(struct hdf5_data* hdf5_data, char* group, char* name, type* data) \
        {                                                               \
                HDF5WRAP_ADD_BODY;                                      \
        }\
        int hdf5wrap_add_2D_dataset_ ##type(struct hdf5_data* hdf5_data, char* group, char* name, type** data) \
        {                                                               \
                HDF5WRAP_ADD_BODY;                                      \
        }


ADD_ARRAY(char)
ADD_ARRAY(int8_t)
ADD_ARRAY(uint8_t)
ADD_ARRAY(int16_t)
ADD_ARRAY(uint16_t)
ADD_ARRAY(int32_t)
ADD_ARRAY(uint32_t)
ADD_ARRAY(int64_t)
ADD_ARRAY(uint64_t)
ADD_ARRAY(float)
ADD_ARRAY(double)

#undef ADD_ARRAY


/* Read function */

#define READ_ARRAY(type)                                                \
        int hdf5wrap_read_0D_dataset_ ##type(struct hdf5_data* hdf5_data, char* group, char* name, type* data) \
        {                                                               \
                RUN(hdf5wrap_open_group(hdf5_data, group));             \
                if((hdf5_data->dataset = H5Dopen(hdf5_data->group,name,H5P_DEFAULT)) == -1)ERROR_MSG("H5Dopen failed\n"); \
                hdf5_data->datatype  = H5Dget_type(hdf5_data->dataset ); \
                HDFWRAP_SET_TYPE(data,&hdf5_data->native_type);         \
                if(!H5Tequal(hdf5_data->datatype, hdf5_data->native_type)){ \
                        WARNING_MSG("The type of the data in the file doesn't match the type of the pointer"); \
                        if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                        ERROR_MSG("Reading failed");                    \
                }                                                       \
                                                                        \
                hdf5_data->dataspace = H5Dget_space(hdf5_data->dataset); \
                hdf5_data->rank      = H5Sget_simple_extent_ndims(hdf5_data->dataspace); \
                hdf5_data->status  = H5Sget_simple_extent_dims(hdf5_data->dataspace,hdf5_data->dim , NULL); \
                                                                        \
                hdf5_data->data = NULL;                                 \
                                                                        \
                hdf5_data->status = H5Dread(hdf5_data->dataset, hdf5_data->datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT,data); \
                                                                        \
                if((hdf5_data->status = H5Tclose(hdf5_data->datatype)) < 0) ERROR_MSG("H5Tclose failed"); \
                if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                                                                        \
                hdf5wrap_close_group(hdf5_data);                        \
                                                                        \
                return OK;                                              \
        ERROR:                                                          \
                return FAIL;                                            \
        }

READ_ARRAY(char)
READ_ARRAY(int8_t)
READ_ARRAY(uint8_t)
READ_ARRAY(int16_t)
READ_ARRAY(uint16_t)
READ_ARRAY(int32_t)
READ_ARRAY(uint32_t)
READ_ARRAY(int64_t)
READ_ARRAY(uint64_t)
READ_ARRAY(float)
READ_ARRAY(double)

#undef READ_ARRAY



#define READ_ARRAY(type)                                                \
        int hdf5wrap_read_1D_dataset_ ##type(struct hdf5_data* hdf5_data, char* group, char* name, type** data) \
        {                                                               \
                void* ptr = NULL;                                       \
                type* p = NULL;                                          \
                RUN(hdf5wrap_open_group(hdf5_data, group));             \
                if((hdf5_data->dataset = H5Dopen(hdf5_data->group,name,H5P_DEFAULT)) == -1)ERROR_MSG("H5Dopen failed\n"); \
                hdf5_data->datatype  = H5Dget_type(hdf5_data->dataset ); \
                HDFWRAP_SET_TYPE(data,&hdf5_data->native_type);         \
                if(!H5Tequal(hdf5_data->datatype, hdf5_data->native_type)){ \
                        WARNING_MSG("The type of the data in the file doesn't match the type of the pointer"); \
                        if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                        ERROR_MSG("Reading failed");                    \
                }                                                       \
                                                                        \
                hdf5_data->dataspace = H5Dget_space(hdf5_data->dataset); \
                hdf5_data->rank      = H5Sget_simple_extent_ndims(hdf5_data->dataspace); \
                hdf5_data->status  = H5Sget_simple_extent_dims(hdf5_data->dataspace,hdf5_data->dim , NULL); \
                                                                        \
                hdf5_data->data = NULL;                                 \
                                                                        \
                RUN(galloc(&p, hdf5_data->dim[0]));                      \
                HDFWRAP_START_GALLOC(p,&ptr);                           \
                hdf5_data->status = H5Dread(hdf5_data->dataset, hdf5_data->datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT,ptr); \
                                                                        \
                if((hdf5_data->status = H5Tclose(hdf5_data->datatype)) < 0) ERROR_MSG("H5Tclose failed"); \
                if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                                                                        \
                hdf5wrap_close_group(hdf5_data);                        \
                                                                        \
                *data = p;                                              \
                                                                        \
                return OK;                                              \
        ERROR:                                                          \
                return FAIL;                                            \
        }

READ_ARRAY(char)
READ_ARRAY(int8_t)
READ_ARRAY(uint8_t)
READ_ARRAY(int16_t)
READ_ARRAY(uint16_t)
READ_ARRAY(int32_t)
READ_ARRAY(uint32_t)
READ_ARRAY(int64_t)
READ_ARRAY(uint64_t)
READ_ARRAY(float)
READ_ARRAY(double)

#undef READ_ARRAY

#define READ_ARRAY(type)                                                \
        int hdf5wrap_read_2D_dataset_ ##type(struct hdf5_data* hdf5_data, char* group, char* name, type*** data) \
        {                                                               \
                void* ptr = NULL;                                       \
                type** p = NULL;                                          \
                RUN(hdf5wrap_open_group(hdf5_data, group));             \
                if((hdf5_data->dataset = H5Dopen(hdf5_data->group,name,H5P_DEFAULT)) == -1)ERROR_MSG("H5Dopen failed\n"); \
                hdf5_data->datatype  = H5Dget_type(hdf5_data->dataset ); \
                HDFWRAP_SET_TYPE(data,&hdf5_data->native_type);         \
                if(!H5Tequal(hdf5_data->datatype, hdf5_data->native_type)){ \
                        WARNING_MSG("The type of the data in the file doesn't match the type of the pointer"); \
                        if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                        ERROR_MSG("Reading failed");                    \
                }                                                       \
                                                                        \
                hdf5_data->dataspace = H5Dget_space(hdf5_data->dataset); \
                hdf5_data->rank      = H5Sget_simple_extent_ndims(hdf5_data->dataspace); \
                hdf5_data->status  = H5Sget_simple_extent_dims(hdf5_data->dataspace,hdf5_data->dim , NULL); \
                                                                        \
                hdf5_data->data = NULL;                                 \
                                                                        \
                RUN(galloc(&p, hdf5_data->dim[0],hdf5_data->dim[1]));    \
                HDFWRAP_START_GALLOC(p,&ptr);                           \
                hdf5_data->status = H5Dread(hdf5_data->dataset, hdf5_data->datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT,ptr); \
                                                                        \
                if((hdf5_data->status = H5Tclose(hdf5_data->datatype)) < 0) ERROR_MSG("H5Tclose failed"); \
                if((hdf5_data->status = H5Dclose(hdf5_data->dataset)) < 0) ERROR_MSG("H5Dclose failed"); \
                                                                        \
                hdf5wrap_close_group(hdf5_data);                        \
                                                                        \
                *data = p;                                              \
                                                                        \
                return OK;                                              \
        ERROR:                                                          \
                return FAIL;                                            \
        }

READ_ARRAY(char)
READ_ARRAY(int8_t)
READ_ARRAY(uint8_t)
READ_ARRAY(int16_t)
READ_ARRAY(uint16_t)
READ_ARRAY(int32_t)
READ_ARRAY(uint32_t)
READ_ARRAY(int64_t)
READ_ARRAY(uint64_t)
READ_ARRAY(float)
READ_ARRAY(double)

#undef READ_ARRAY





#define ADD_ATTR(type)                                                  \
        int hdf5wrap_add_attribute_ ##type(struct hdf5_data* hdf5_data, char* group, char* name,type x) \
        {                                                               \
                hid_t aid;                                              \
                hid_t attr;                                             \
                RUN(hdf5wrap_open_group(hdf5_data, group));             \
                HDFWRAP_SET_TYPE(x,&hdf5_data->native_type);            \
                if( H5Aexists(hdf5_data->group, name)){                 \
                        LOG_MSG("EXISTS");                              \
                        attr = H5Aopen(hdf5_data->group,name, H5P_DEFAULT); \
                        hdf5_data->status = H5Awrite(attr,hdf5_data->native_type, &x); \
                        hdf5_data->status = H5Aclose(attr);             \
                }else{                                                  \
                        LOG_MSG("NEW");                                 \
                        aid  = H5Screate(H5S_SCALAR);                   \
                        if(aid < 0){                                    \
                                ERROR_MSG("H5Screate failed %d", aid);  \
                        }                                               \
                        attr = H5Acreate(hdf5_data->group,name, hdf5_data->native_type, aid,  H5P_DEFAULT, H5P_DEFAULT); \
                        if(attr < 0){                                   \
                                ERROR_MSG("H5Acreate failed %d", attr); \
                        }                                               \
                        hdf5_data->status = H5Awrite(attr,hdf5_data->native_type, &x); \
                        if(hdf5_data->status < 0){                      \
                                ERROR_MSG("H5Awrite failed %d", hdf5_data->status); \
                        }                                               \
                        hdf5_data->status = H5Sclose(aid);              \
                        hdf5_data->status = H5Aclose(attr);             \
                }                                                       \
                hdf5wrap_close_group(hdf5_data);                        \
                return OK;                                              \
        ERROR:                                                          \
                return FAIL;                                            \
        }

ADD_ATTR(int8_t)
ADD_ATTR(uint8_t)
ADD_ATTR(int16_t)
ADD_ATTR(uint16_t)
ADD_ATTR(int32_t)
ADD_ATTR(uint32_t)
ADD_ATTR(int64_t)
ADD_ATTR(uint64_t)
ADD_ATTR(float)
ADD_ATTR(double)

#undef ADD_ATTR



int hdf5wrap_add_attribute_string(struct hdf5_data* hdf5_data,char* group, char* name,char* x)
{
        hid_t aid;
        hid_t atype;
        hid_t attr;
        int len;
        RUN(hdf5wrap_open_group(hdf5_data, group));

        if( H5Aexists(hdf5_data->group, name)){
                H5Adelete(hdf5_data->group,name);
        }
        len = strnlen(x, HDF5GLUE_MAX_CONTENT_LEN) + 1;


        aid  = H5Screate(H5S_SCALAR);
        atype = H5Tcopy(H5T_C_S1);
        H5Tset_size(atype, len);
        H5Tset_strpad(atype,H5T_STR_NULLTERM);
        attr = H5Acreate(hdf5_data->group  ,name, atype, aid, H5P_DEFAULT, H5P_DEFAULT);
        hdf5_data->status = H5Awrite(attr, atype, x);
        hdf5_data->status = H5Sclose(aid);
        hdf5_data->status = H5Tclose(atype);
        hdf5_data->status = H5Aclose(attr);
        hdf5wrap_close_group(hdf5_data);
        return OK;
ERROR:
        return FAIL;
}

#define READ_ATTR(type)                                                 \
        int hdf5wrap_read_attribute_ ##type (struct hdf5_data* hdf5_data, char* group,char* name, type* x) \
        {                                                               \
                H5O_info_t oinfo;                                       \
                hid_t atype;                                            \
                int i;                                                  \
                char attr_name[HDF5GLUE_MAX_NAME_LEN];                  \
                RUN(hdf5wrap_open_group(hdf5_data, group));             \
                hdf5_data->status = H5Oget_info(hdf5_data->group , &oinfo,H5O_INFO_NUM_ATTRS); \
                for(i = 0; i < (int)oinfo.num_attrs; i++) {             \
                        hdf5_data->attribute_id = H5Aopen_by_idx(hdf5_data->group, ".", H5_INDEX_CRT_ORDER, H5_ITER_INC, (hsize_t)i, H5P_DEFAULT, H5P_DEFAULT); \
                        if(hdf5_data->attribute_id < 1){                \
                                ERROR_MSG("H5Aopen_by_idx failed with %d",hdf5_data->attribute_id); \
                        }                                               \
                        atype = H5Aget_type(hdf5_data->attribute_id);   \
                        H5Aget_name(hdf5_data->attribute_id ,HDF5GLUE_MAX_NAME_LEN,attr_name); \
                        if(!strncmp(name,attr_name, HDF5GLUE_MAX_NAME_LEN)){ \
                                HDFWRAP_SET_TYPE(x,&hdf5_data->native_type); \
                                if(H5Tequal(hdf5_data->native_type, atype)){ \
                                        hdf5_data->status = H5Aread(hdf5_data->attribute_id, hdf5_data->native_type, x); \
                                }else{                                  \
                                        ERROR_MSG("Type mismatch");     \
                                }                                       \
                        }                                               \
                        hdf5_data->status   = H5Aclose(hdf5_data->attribute_id); \
                        hdf5_data->status   = H5Tclose(atype);          \
                }                                                       \
                return OK;                                              \
        ERROR:                                                          \
                return FAIL;                                            \
        }

READ_ATTR(int8_t)
READ_ATTR(uint8_t)
READ_ATTR(int16_t)
READ_ATTR(uint16_t)
READ_ATTR(int32_t)
READ_ATTR(uint32_t)
READ_ATTR(int64_t)
READ_ATTR(uint64_t)
READ_ATTR(float)
READ_ATTR(double)

#undef READ_ATTR


int hdf5wrap_read_attribute_string(struct hdf5_data* hdf5_data, char* group,char* name, char** x)
{
        H5O_info_t oinfo;
        hid_t atype,atype_mem;
        //H5T_class_t type_class;
        int i;
        char attr_name[HDF5GLUE_MAX_NAME_LEN];

        RUN(hdf5wrap_open_group(hdf5_data, group));
#if defined(H5Oget_info_vers) && H5Oget_info_vers == 3
    
        hdf5_data->status = H5Oget_info(hdf5_data->group , &oinfo,H5O_INFO_NUM_ATTRS);
        //if(H5Oget_info3(getId(), &oinfo, H5O_INFO_NUM_ATTRS) < 0)
#else
        //H5O_info_t oinfo;    /* Object info */
        hdf5_data->status = H5Oget_info(hdf5_data->group , &oinfo);
#endif
        
        //hdf5_data->num_attr = 0;
        for(i = 0; i < (int)oinfo.num_attrs; i++) {
                hdf5_data->attribute_id = H5Aopen_by_idx(hdf5_data->group, ".", H5_INDEX_CRT_ORDER, H5_ITER_INC, (hsize_t)i, H5P_DEFAULT, H5P_DEFAULT);
                if(hdf5_data->attribute_id < 1){
                        ERROR_MSG("H5Aopen_by_idx failed with %d",hdf5_data->attribute_id);
                }
                atype = H5Aget_type(hdf5_data->attribute_id);
                H5Aget_name(hdf5_data->attribute_id ,HDF5GLUE_MAX_NAME_LEN,attr_name);

                if(!strncmp(name,attr_name, HDF5GLUE_MAX_NAME_LEN)){
                        char buffer[HDF5GLUE_MAX_NAME_LEN];
                        atype_mem = H5Tget_native_type(atype, H5T_DIR_ASCEND);
                        hdf5_data->status   = H5Aread(hdf5_data->attribute_id, atype_mem, buffer);

                        hdf5_data->status   = H5Tclose(atype_mem);
                        int l = strnlen(buffer, HDF5GLUE_MAX_CONTENT_LEN);
                        char* tmp = NULL;
                        MMALLOC(tmp, sizeof(char) *(l+1));
                        memcpy(tmp, buffer, l);
                        tmp[l] = 0;
                        *x = tmp;
                }
                hdf5_data->status   = H5Aclose(hdf5_data->attribute_id);
                hdf5_data->status   = H5Tclose(atype);
        }
        return OK;
ERROR:
        return FAIL;
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
                        /* fprintf(stdout,"Buffer: %s\n",buffer); */

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
        /* LOG_MSG("Got here"); */
        /* hdf5_data->status = H5Eset_auto(hdf5_data->status ,NULL, NULL); */
        /* hdf5_data->status = H5Gget_objinfo (hdf5_data->file, groupname, 0, NULL); */
        hdf5_data->status = H5Lexists(hdf5_data->file, groupname , H5P_DEFAULT);
        if (hdf5_data->status == 0){
                snprintf(hdf5_data->group_name , HDF5GLUE_MAX_NAME_LEN,"%s",groupname);
                if((hdf5_data->group = H5Gcreate (hdf5_data->file , hdf5_data->group_name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)ERROR_MSG("H5Gcreate failed\n");

                RUN(hdf5wrap_close_group(hdf5_data));
        }
        /* if (hdf5_data->status == 0){ */
                /* printf ("The group %s exists.\n",groupname ); */
        if((hdf5_data->group = H5Gopen(hdf5_data->file,  groupname , H5P_DEFAULT)) == -1)ERROR_MSG("H5Gopen2 failed\n");
        /* }else{ */

        /*         printf ("The group %s either does NOT exist\n or some other error occurred.\n",groupname); */
        /*         snprintf(hdf5_data->group_name , HDF5GLUE_MAX_NAME_LEN,"%s",groupname); */
        /*         if((hdf5_data->group = H5Gcreate (hdf5_data->file , hdf5_data->group_name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)ERROR_MSG("H5Gcreate failed\n"); */
        /* } */
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

        if(hdf5_data){
                /* if(H5Fflush(hdf5_data->file, H5F_SCOPE_LOCAL) < 0) ERROR_MSG("Flushl failed"); */
                if(H5Fclose(hdf5_data->file) < 0) ERROR_MSG("Close failed");
                /* H5garbage_collect(); */
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

        //int i;
        if(hdf5_data){
                if(hdf5_data->grp_names){
                        gfree(hdf5_data->grp_names->names);
                        MFREE(hdf5_data->grp_names);
                }
                MFREE(hdf5_data);
        }
}


#define STARTOF_DATA(type)                                    \
        int startof_galloc_ ##type(type x, void** ptr)        \
        {                                                     \
                *ptr = &x;                                    \
                return OK;                                    \
        }                                                     \
        int startof_galloc_ ##type ## _s(type* x, void** ptr)  \
        {                                                     \
                *ptr = &x[0];                                 \
                return OK;                                    \
        }                                                     \
        int startof_galloc_ ##type ## _ss(type** x, void** ptr) \
        {                                                     \
                *ptr = &x[0][0];                                 \
                return OK;                                    \
        }

STARTOF_DATA(char)
STARTOF_DATA(int8_t)
STARTOF_DATA(uint8_t)
STARTOF_DATA(int16_t)
STARTOF_DATA(uint16_t)
STARTOF_DATA(int32_t)
STARTOF_DATA(uint32_t)
STARTOF_DATA(int64_t)
STARTOF_DATA(uint64_t)
STARTOF_DATA(float)
STARTOF_DATA(double)
#undef STARTOF_DATA


int startof_galloc_unknown(void* x, void** ptr)
{
        ERROR_MSG("unknown data type: %p %p", x,*ptr);
        return OK;
ERROR:
        return FAIL;
}


int set_type_char(hid_t* type)
{
        *type = H5T_NATIVE_CHAR;
        return OK;
}

int set_type_int8_t(hid_t* type)
{
        *type = H5T_NATIVE_INT8;
        return OK;
}

int set_type_uint8_t(hid_t* type)
{
        *type = H5T_NATIVE_UINT8;
        return OK;
}

int set_type_int16_t(hid_t* type)
{
        *type = H5T_NATIVE_INT16;
        return OK;
}

int set_type_uint16_t(hid_t* type)
{
        *type = H5T_NATIVE_UINT16;
        return OK;
}

int set_type_int32_t(hid_t* type)
{
        *type = H5T_NATIVE_INT32;
        return OK;
}

int set_type_uint32_t(hid_t* type)
{
        *type = H5T_NATIVE_UINT32;
        return OK;
}
int set_type_int64_t(hid_t* type)
{
        *type = H5T_NATIVE_INT64;
        return OK;
}

int set_type_uint64_t(hid_t* type)
{
        *type = H5T_NATIVE_UINT64;
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

        WARNING_MSG("Could not determine type! (%d)",type);
        return FAIL;
}



static herr_t op_func (hid_t loc_id, const char *name, const H5L_info_t *info, void *operator_data);

int hdf5wrap_search(struct hdf5_data* hdf5_data,char* target, char** location)
{
        ERROR_MSG("This function no longer works.");
        char* look[2];
        look[0] = target;
        look[1] = NULL;

        hdf5wrap_open_group(hdf5_data, "/");

        if((hdf5_data->status = H5Lvisit (hdf5_data->group, H5_INDEX_NAME, H5_ITER_NATIVE,  op_func, &look)) < 0) ERROR_MSG("H5Literate failed");


        if(look[1]){
                *location = look[1];
        }else{
                *location = NULL;
        }
        return OK;

ERROR:
        return FAIL;

}

#define UNUSED(expr) do { (void)(expr); } while (0)
herr_t op_func (hid_t loc_id, const char *name, const H5L_info_t *info, void *operator_data)
{
        herr_t status;
        H5O_info_t infobuf;
        char** look = (char**)operator_data;

        int len_t,len_q;
        UNUSED(info);
#if defined(H5Oget_info_vers) && H5Oget_info_vers == 3
        LOG_MSG("Version 3");
        status = H5Oget_info_by_name (loc_id, name, &infobuf, H5O_INFO_BASIC,H5O_INFO_NUM_ATTRS);
#else
        LOG_MSG("Version other");
        status = H5Oget_info_by_name (loc_id, name, &infobuf, H5P_DEFAULT);
#endif 
        if(status <0){
                ERROR_MSG(" H5Oget_info_by_name failed. %d" , status);
        }

        switch (infobuf.type){
        case H5O_TYPE_GROUP:
                len_t = strnlen(name, HDF5GLUE_MAX_NAME_LEN);
                len_q = strnlen( look[0], HDF5GLUE_MAX_NAME_LEN);
                if(len_t >= len_q){ /* match is possible */
                        if(!strncmp(name + (len_t - len_q), look[0], HDF5GLUE_MAX_NAME_LEN)){
                                if(look[1]){
                                        WARNING_MSG("Multiple matches found:");
                                        WARNING_MSG(" - %s", look[1]);
                                        WARNING_MSG(" - %s", name);
                                }else{
                                        MMALLOC(look[1], sizeof(char) * (len_t+1));
                                        memcpy(look[1], name, len_t);
                                        look[1][len_t] =0;
                                }
                        }
                }
                break;
        case H5O_TYPE_DATASET:
                len_t = strnlen(name, HDF5GLUE_MAX_NAME_LEN);
                len_q = strnlen( look[0], HDF5GLUE_MAX_NAME_LEN);
                if(len_t >= len_q){ /* match is possible */
                        if(!strncmp(name + (len_t - len_q), look[0], HDF5GLUE_MAX_NAME_LEN)){
                                if(look[1]){
                                        WARNING_MSG("Multiple matches found:");
                                        WARNING_MSG(" - %s", look[1]);
                                        WARNING_MSG(" - %s", name);
                                }else{
                                        MMALLOC(look[1], sizeof(char) * (len_t+1));
                                        memcpy(look[1], name, len_t);
                                        look[1][len_t] =0;
                                }
                        }
                }
                break;
        case H5O_TYPE_NAMED_DATATYPE:
                break;
        case H5O_TYPE_UNKNOWN:
        case H5O_TYPE_NTYPES:
                break;
        default:
                break;
        }

        return OK;
ERROR:
        return FAIL;
}
#undef UNUSED
