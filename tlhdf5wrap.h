#ifndef TLHDF5WRAP_H
#define TLHDF5WRAP_H

#include <hdf5.h>


#ifdef TLHDF5WRAP_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif


struct hdf5_data;

EXTERN int open_hdf5_file(struct hdf5_data** h, char* filename);
EXTERN int close_hdf5_file(struct hdf5_data** h);


EXTERN int hdf5_write_attributes(struct hdf5_data* hdf5_data, char* target);
EXTERN int hdf5_read_attributes(struct hdf5_data* hdf5_data, char* target);

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


#define HDFWRAP_ADD_DATA(F,G,N,V) _Generic((V),                         \
                                           char*: hdf5wrap_add_1D_dataset_char, \
                                           int*: hdf5wrap_add_1D_dataset_int, \
                                           float*:   hdf5wrap_add_1D_dataset_float, \
                                           double*:  hdf5wrap_add_1D_dataset_double, \
                                           char**: hdf5wrap_add_2D_dataset_char, \
                                           int**: hdf5wrap_add_2D_dataset_int, \
                                           float**:   hdf5wrap_add_2D_dataset_float, \
                                           double**:  hdf5wrap_add_2D_dataset_double \
                )(F,G,N,V)





int add_dataset_int(struct hdf5_data* hdf5_data, char* group, char* name, int** data);

int hdf5wrap_add_attribute_int(struct hdf5_data* hdf5_data,char* group, char* name,int x);
int hdf5wrap_add_attribute_double(struct hdf5_data* hdf5_data,char* group, char* name,double x);
int hdf5wrap_add_attribute_string(struct hdf5_data* hdf5_data,char* group, char* name,char* x);

#define HDFWRAP_ADD_ATTRIBUTE(F,G,N,V) _Generic((V),                     \
                                              int: hdf5wrap_add_attribute_int, \
                                              double: hdf5wrap_add_attribute_double, \
                                              char*: hdf5wrap_add_attribute_string \
                )(F,G,N,V)


int hdf5wrap_add_dataset_int(struct hdf5_data* hdf5_data,char* group, char* name,int x);
int hdf5wrap_add_dataset_double(struct hdf5_data* hdf5_data,char* group, char* name,double x);
int hdf5wrap_add_dataset_string(struct hdf5_data* hdf5_data,char* group, char* name,char* x);

#define HDFWRAP_ADD_DATASET(F,G,N,V) _Generic((V),                     \
                                              int: hdf5wrap_add_dataset_int, \
                                              double: hdf5wrap_add_dataset_double, \
                                              char*: hdf5wrap_add_dataset_string \
                )(F,G,N,V)



#undef TLHDF5WRAP_IMPORT
#undef EXTERN


#endif
