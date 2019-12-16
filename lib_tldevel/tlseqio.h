#ifndef TLSEQIO_H
#define TLSEQIO_H

#include <stdint.h>
#include <stdio.h>

#ifdef TLSEQIO_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif


#include <zlib.h>


#define TLSEQIO_FASTA 1
#define TLSEQIO_FASTQ 2
#define TLSEQIO_GZIPPED 4
struct file_handler{
        gzFile f_ptr;
        uint32_t* seq_index;
        uint8_t file_type;
};

EXTERN int get_io_handler(struct file_handler** fh,const char* filename);
EXTERN void free_io_handler(struct file_handler* f);
EXTERN int echo_file(struct file_handler* f);
#undef TLSEQIO_IMPORT
#undef EXTERN

#endif
