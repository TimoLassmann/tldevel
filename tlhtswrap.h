#ifndef TLHTSWRAP_H
#define TLHTSWRAP_H

#include "tlseqbuffer.h"

#ifdef TLHTSWRAP_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif


#define TLSEQIO_READ 0

#define TLSEQIO_WRITE 1
#define TLSEQIO_WRITE_GZIPPED  TLSEQIO_WRITE


typedef struct file_handler file_handler;



EXTERN int open_fasta_fastq_file(struct file_handler** fh,char* filename, int mode);
EXTERN int open_sam_bam_file(struct file_handler** fh, char* filename, int mode);
//EXTERN int read_fasta_fastq_file(struct file_handler* fh, struct tl_seq_buffer** seq_buf, int num);
//EXTERN int close_fasta_fastq_file(struct file_handler** fh);
EXTERN int close_seq_file(struct file_handler** fh);


#undef TLHTSWRAP_IMPORT
#undef EXTERN

#endif
