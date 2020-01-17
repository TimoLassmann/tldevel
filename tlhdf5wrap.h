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
