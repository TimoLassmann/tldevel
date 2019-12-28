#ifndef TLSEQIO_H
#define TLSEQIO_H

#include <stdint.h>
#include <stdio.h>

#include "tlseqbuffer.h"

#ifdef TLSEQIO_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif



#define TLSEQIO_READ 0

#define TLSEQIO_WRITE 1
#define TLSEQIO_WRITE_GZIPPED 2

#define TLSEQIO_APPEND 3
#define TLSEQIO_APPEND_GZIPPED 4


typedef struct file_handler file_handler;


EXTERN int open_fasta_fastq_file(struct file_handler** fh,char* filename, int mode);
EXTERN int open_sam_bam(struct file_handler** fh, char* filename, int mode);
EXTERN int read_fasta_fastq_file(struct file_handler* fh, struct tl_seq_buffer** seq_buf, int num);
EXTERN int close_seq_file(struct file_handler** fh);

EXTERN int write_fasta_fastq(struct tl_seq_buffer* sb, struct file_handler* fh);




EXTERN int echo_file(struct file_handler* f);
#undef TLSEQIO_IMPORT
#undef EXTERN

#endif
