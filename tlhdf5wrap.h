#ifndef TLHDF5WRAP_H
#define TLHDF5WRAP_H


#ifdef TLHDF5WRAP_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif


struct hdf5_data;

EXTERN int open_hdf5_file(struct hdf5_data** h, char* filename);
EXTERN int hdf5_close_file(struct hdf5_data** h);

int hdf5wrap_add_attribute_int(int x, struct hdf5_data* hdf5_data,char* name);
int hdf5wrap_add_attribute_double(double x, struct hdf5_data* hdf5_data,char* name);
int hdf5wrap_add_attribute_string(char* x, struct hdf5_data* hdf5_data,char* name);

#define HDFWRAP_ADD_ATTRIBUTE(X,Y,Z) _Generic((X),                      \
                                              int: hdf5wrap_add_attribute_int, \
                                              double: hdf5wrap_add_attribute_double, \
                                              char*: hdf5wrap_add_attribute_string \
                )(X,Y,Z)


#undef TLHDF5WRAP_IMPORT
#undef EXTERN


#endif
