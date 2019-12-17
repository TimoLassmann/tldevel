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
        char* read_buffer;
        int bytes_read;
        int pos;
        int read_state;
};


struct tl_seq{
        uint8_t* seq;
        char* name;
        char* qual;
        int malloc_len;
        int len;
};

struct tl_seq_buffer{
        struct tl_seq** sequences;
        int malloc_num;
        int num_seq;
        int max_len;
        int L;
};

EXTERN int open_fasta_fastq_file(struct file_handler** fh,char* filename);
EXTERN int read_fasta_fastq_file(struct file_handler* fh, struct tl_seq_buffer** seq_buf, int num);
EXTERN int close_fasta_fastq_file(struct file_handler** fh);

EXTERN void free_tl_seq_buffer(struct tl_seq_buffer* sb);


EXTERN int echo_file(struct file_handler* f);
#undef TLSEQIO_IMPORT
#undef EXTERN

#endif
