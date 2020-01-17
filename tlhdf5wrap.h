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



#define ADD_DATA_DEF(type)                                              \
        EXTERN int hdf5wrap_add_1D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type* data)

ADD_DATA_DEF(char);
ADD_DATA_DEF(int);
ADD_DATA_DEF(float);
ADD_DATA_DEF(double);

#undef ADD_DATA_DEF

#define ADD_DATA_DEF(type)                                              \
        EXTERN int hdf5wrap_add_2D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type** data)

ADD_DATA_DEF(char);
ADD_DATA_DEF(int);
ADD_DATA_DEF(float);
ADD_DATA_DEF(double);

#undef ADD_DATA_DEF

#define HDFWRAP_WRITE_DATA(F,G,N,V) _Generic((V),                         \
                                           char*: hdf5wrap_add_1D_dataset_char, \
                                           int*: hdf5wrap_add_1D_dataset_int, \
                                           float*:   hdf5wrap_add_1D_dataset_float, \
                                           double*:  hdf5wrap_add_1D_dataset_double, \
                                           char**: hdf5wrap_add_2D_dataset_char, \
                                           int**: hdf5wrap_add_2D_dataset_int, \
                                           float**:   hdf5wrap_add_2D_dataset_float, \
                                           double**:  hdf5wrap_add_2D_dataset_double \
                )(F,G,N,V)


#define ADD_DATA_DEF(type)                                              \
        EXTERN int hdf5wrap_read_1D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type** data)

ADD_DATA_DEF(char);
ADD_DATA_DEF(int);
ADD_DATA_DEF(float);
ADD_DATA_DEF(double);

#undef ADD_DATA_DEF

#define ADD_DATA_DEF(type)                                              \
        EXTERN int hdf5wrap_read_2D_dataset_ ##type (struct hdf5_data* hdf5_data, char* group, char* name, type*** data)

ADD_DATA_DEF(char);
ADD_DATA_DEF(int);
ADD_DATA_DEF(float);
ADD_DATA_DEF(double);

#undef ADD_DATA_DEF


#define HDFWRAP_READ_DATA(F,G,N,V) _Generic((V),                         \
                                           char**: hdf5wrap_read_1D_dataset_char, \
                                           int**: hdf5wrap_read_1D_dataset_int, \
                                           float**:   hdf5wrap_read_1D_dataset_float, \
                                           double**:  hdf5wrap_read_1D_dataset_double, \
                                           char***: hdf5wrap_read_2D_dataset_char, \
                                           int***: hdf5wrap_read_2D_dataset_int, \
                                           float***:   hdf5wrap_read_2D_dataset_float, \
                                           double***:  hdf5wrap_read_2D_dataset_double \
                )(F,G,N,V)



EXTERN int hdf5wrap_add_attribute_int(struct hdf5_data* hdf5_data,char* group, char* name,int x);
EXTERN int hdf5wrap_add_attribute_double(struct hdf5_data* hdf5_data,char* group, char* name,double x);
EXTERN int hdf5wrap_add_attribute_string(struct hdf5_data* hdf5_data,char* group, char* name,char* x);

#define HDFWRAP_WRITE_ATTRIBUTE(F,G,N,V) _Generic((V),                     \
                                              int: hdf5wrap_add_attribute_int, \
                                              double: hdf5wrap_add_attribute_double, \
                                              char*: hdf5wrap_add_attribute_string \
                )(F,G,N,V)


EXTERN int hdf5wrap_read_attribute_int(struct hdf5_data* hdf5_data, char* group, char* name, int* x);
EXTERN int hdf5wrap_read_attribute_double(struct hdf5_data* hdf5_data, char* group, char* name, double* x);
EXTERN int hdf5wrap_read_attribute_string(struct hdf5_data* hdf5_data, char* group, char* name, char** x);

#define HDFWRAP_READ_ATTRIBUTE(F,G,N,V) _Generic((V),                     \
                                              int*: hdf5wrap_read_attribute_int, \
                                              double*: hdf5wrap_read_attribute_double, \
                                              char**: hdf5wrap_read_attribute_string \
                )(F,G,N,V)






#undef TLHDF5WRAP_IMPORT
#undef EXTERN


#endif
