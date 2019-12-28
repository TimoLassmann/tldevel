#ifndef TLHTSWRAP_H
#define TLHTSWRAP_H

#ifdef TLHTSWRAP_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif


typedef struct file_handler file_handler;

struct tl_seq{
        char* seq;
        char* name;
        char* qual;
        int malloc_len;
        int len;
};

struct tl_seq_buffer{
        struct tl_seq** sequences;
        int malloc_num;
        int offset;
        int num_seq;
        int max_len;
        int is_fastq;
        int L;
};


EXTERN int hts_wrap_open_fasta_fastq(struct file_handler** fh, char* filename, int mode);
EXTERN int hts_wrap_open_sam_bam(struct file_handler** fh, char* filename, int mode);
EXTERN int hts_wrap_close_file(struct file_handler** fh);

#undef TLHTSWRAP_IMPORT
#undef EXTERN

#endif
