#ifndef TLHDF5WRAP_H
#define TLHDF5WRAP_H

#include <hdf5.h>


#ifdef TLHDF5WRAP_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif


/* This is an attempt to make reading to / from hdf5 files simpler */

/*
   Read / writing is done via these MACROS:

HDFWRAP_WRITE_DATA(file handler, path, name, data)
HDFWRAP_READ_DATA(file handler, path, name, pointer to where the data should go)
HDFWRAP_WRITE_ATTRIBUTE(file handler, path, name, attribute)
HDFWRAP_READ_ATTRIBUTE(file handler, path, name, pointer to where the attribute value should go)

In all these examples the 'path' should start with '/' and have no trailing '/'. For example:

/
/group1
/group1/subdir

The data type to be used in the hdf5 file is determined via C11 generic functions

   to open a file:

   open_hdf5_file(file hander, <filename>)

   close:
   close_hdf5_file(file hander,)

 */

struct hdf5_data;

EXTERN int open_hdf5_file(struct hdf5_data** h, char* filename);
EXTERN int close_hdf5_file(struct hdf5_data** h);

//EXTERN int search(struct hdf5_data* hdf5_data);
EXTERN int hdf5wrap_search(struct hdf5_data* hdf5_data,char* target, char** location);


#define ADD_DATA_DEF(type)                                              \
        EXTERN int hdf5wrap_add_0D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type data); \
EXTERN int hdf5wrap_add_1D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type* data); \
EXTERN int hdf5wrap_add_2D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type** data);

ADD_DATA_DEF(char)
ADD_DATA_DEF(int8_t)
ADD_DATA_DEF(uint8_t)
ADD_DATA_DEF(int16_t)
ADD_DATA_DEF(uint16_t)
ADD_DATA_DEF(int32_t)
ADD_DATA_DEF(uint32_t)
ADD_DATA_DEF(int64_t)
ADD_DATA_DEF(uint64_t)
ADD_DATA_DEF(float)
ADD_DATA_DEF(double)

#undef ADD_DATA_DEF


#define HDFWRAP_WRITE_DATA(F,G,N,V) _Generic((V),                       \
                char: hdf5wrap_add_0D_dataset_char,                     \
                char*: hdf5wrap_add_1D_dataset_char,                    \
                char**: hdf5wrap_add_2D_dataset_char,                   \
                int8_t: hdf5wrap_add_0D_dataset_int8_t,                       \
                int8_t*: hdf5wrap_add_1D_dataset_int8_t,                      \
                int8_t**: hdf5wrap_add_2D_dataset_int8_t,                     \
                uint8_t: hdf5wrap_add_0D_dataset_uint8_t,                       \
                uint8_t*: hdf5wrap_add_1D_dataset_uint8_t,                      \
                uint8_t**: hdf5wrap_add_2D_dataset_uint8_t,                     \
                int16_t: hdf5wrap_add_0D_dataset_int16_t,                       \
                int16_t*: hdf5wrap_add_1D_dataset_int16_t,                      \
                int16_t**: hdf5wrap_add_2D_dataset_int16_t,                     \
                uint16_t: hdf5wrap_add_0D_dataset_uint16_t,                       \
                uint16_t*: hdf5wrap_add_1D_dataset_uint16_t,                      \
                uint16_t**: hdf5wrap_add_2D_dataset_uint16_t,                     \
                int32_t: hdf5wrap_add_0D_dataset_int32_t,                       \
                int32_t*: hdf5wrap_add_1D_dataset_int32_t,                      \
                int32_t**: hdf5wrap_add_2D_dataset_int32_t,                     \
                uint32_t: hdf5wrap_add_0D_dataset_uint32_t,                       \
                uint32_t*: hdf5wrap_add_1D_dataset_uint32_t,                      \
                uint32_t**: hdf5wrap_add_2D_dataset_uint32_t,                     \
                int64_t: hdf5wrap_add_0D_dataset_int64_t,                       \
                int64_t*: hdf5wrap_add_1D_dataset_int64_t,                      \
                int64_t**: hdf5wrap_add_2D_dataset_int64_t,                     \
                uint64_t: hdf5wrap_add_0D_dataset_uint64_t,                       \
                uint64_t*: hdf5wrap_add_1D_dataset_uint64_t,                      \
                uint64_t**: hdf5wrap_add_2D_dataset_uint64_t,                     \
                float:   hdf5wrap_add_0D_dataset_float,                 \
                float*:   hdf5wrap_add_1D_dataset_float,                \
                float**:   hdf5wrap_add_2D_dataset_float,               \
                double:  hdf5wrap_add_0D_dataset_double,                \
                double*:  hdf5wrap_add_1D_dataset_double,               \
                double**:  hdf5wrap_add_2D_dataset_double               \
                )(F,G,N,V)


#define READ_ARRAY(type)                                              \
        EXTERN int hdf5wrap_read_0D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type* data); \
        EXTERN int hdf5wrap_read_1D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type** data); \
EXTERN int hdf5wrap_read_2D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type*** data);


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


#define HDFWRAP_READ_DATA(F,G,N,V) _Generic((V),                        \
                                            char*: hdf5wrap_read_0D_dataset_char, \
                                            char**: hdf5wrap_read_1D_dataset_char, \
                                            char***: hdf5wrap_read_2D_dataset_char, \
                                            int8_t*: hdf5wrap_read_0D_dataset_int8_t, \
                                            int8_t**: hdf5wrap_read_1D_dataset_int8_t, \
                                            int8_t***: hdf5wrap_read_2D_dataset_int8_t, \
                                            uint8_t*: hdf5wrap_read_0D_dataset_uint8_t, \
                                            uint8_t**: hdf5wrap_read_1D_dataset_uint8_t, \
                                            uint8_t***: hdf5wrap_read_2D_dataset_uint8_t, \
                                            int16_t*: hdf5wrap_read_0D_dataset_int16_t, \
                                            int16_t**: hdf5wrap_read_1D_dataset_int16_t, \
                                            int16_t***: hdf5wrap_read_2D_dataset_int16_t, \
                                            uint16_t*: hdf5wrap_read_0D_dataset_uint16_t, \
                                            uint16_t**: hdf5wrap_read_1D_dataset_uint16_t, \
                                            uint16_t***: hdf5wrap_read_2D_dataset_uint16_t, \
                                            int32_t*: hdf5wrap_read_0D_dataset_int32_t, \
                                            int32_t**: hdf5wrap_read_1D_dataset_int32_t, \
                                            int32_t***: hdf5wrap_read_2D_dataset_int32_t, \
                                            uint32_t*: hdf5wrap_read_0D_dataset_uint32_t, \
                                            uint32_t**: hdf5wrap_read_1D_dataset_uint32_t, \
                                            uint32_t***: hdf5wrap_read_2D_dataset_uint32_t, \
                                            int64_t*: hdf5wrap_read_0D_dataset_int64_t, \
                                            int64_t**: hdf5wrap_read_1D_dataset_int64_t, \
                                            int64_t***: hdf5wrap_read_2D_dataset_int64_t, \
                                            uint64_t*: hdf5wrap_read_0D_dataset_uint64_t, \
                                            uint64_t**: hdf5wrap_read_1D_dataset_uint64_t, \
                                            uint64_t***: hdf5wrap_read_2D_dataset_uint64_t, \
                                            float*:   hdf5wrap_read_0D_dataset_float, \
                                            float**:   hdf5wrap_read_1D_dataset_float, \
                                            float***:   hdf5wrap_read_2D_dataset_float, \
                                            double*:  hdf5wrap_read_0D_dataset_double, \
                                            double**:  hdf5wrap_read_1D_dataset_double, \
                                            double***:  hdf5wrap_read_2D_dataset_double \
                )(F,G,N,V)


#define ADD_ATTR(type)                                                  \
        EXTERN int hdf5wrap_add_attribute_ ##type(struct hdf5_data* hdf5_data, char* group, char* name,type x);

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

EXTERN int hdf5wrap_add_attribute_string(struct hdf5_data* hdf5_data,char* group, char* name,char* x);

#define HDFWRAP_WRITE_ATTRIBUTE(F,G,N,V) _Generic((V),      \
                int8_t: hdf5wrap_add_attribute_int8_t,      \
                uint8_t: hdf5wrap_add_attribute_uint8_t,    \
                int16_t: hdf5wrap_add_attribute_int16_t,    \
                uint16_t: hdf5wrap_add_attribute_uint16_t,  \
                int32_t: hdf5wrap_add_attribute_int32_t,    \
                uint32_t: hdf5wrap_add_attribute_uint32_t,  \
                int64_t: hdf5wrap_add_attribute_int64_t,    \
                uint64_t: hdf5wrap_add_attribute_uint64_t,  \
                float: hdf5wrap_add_attribute_float,        \
                double: hdf5wrap_add_attribute_double,      \
                char*: hdf5wrap_add_attribute_string        \
                )(F,G,N,V)


#define READ_ATTR(type)                                                  \
        EXTERN int hdf5wrap_read_attribute_ ##type(struct hdf5_data* hdf5_data, char* group, char* name,type x);

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


EXTERN int hdf5wrap_read_attribute_string(struct hdf5_data* hdf5_data, char* group,char* name, char** x);

#define HDFWRAP_READ_ATTRIBUTE(F,G,N,V) _Generic((V),                     \
                int8_t: hdf5wrap_read_attribute_int8_t,                  \
                uint8_t: hdf5wrap_read_attribute_uint8_t,    \
                int16_t: hdf5wrap_read_attribute_int16_t,    \
                uint16_t: hdf5wrap_read_attribute_uint16_t,  \
                int32_t: hdf5wrap_read_attribute_int32_t,    \
                uint32_t: hdf5wrap_read_attribute_uint32_t,  \
                int64_t: hdf5wrap_read_attribute_int64_t,    \
                uint64_t: hdf5wrap_read_attribute_uint64_t,  \
                float: hdf5wrap_read_attribute_float,        \
                double: hdf5wrap_read_attribute_double,      \
                char**: hdf5wrap_read_attribute_string        \
                )(F,G,N,V)



#undef TLHDF5WRAP_IMPORT
#undef EXTERN


#endif
