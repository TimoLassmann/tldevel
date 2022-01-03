#ifndef TLSEQBUFFER_H
#define TLSEQBUFFER_H

#include <inttypes.h>

#ifdef TLSEQBUFFER_IMPORT
#define EXTERN
#else
#define EXTERN extern
#endif

struct tl_seq{
        uint8_t * seq;
        char* name;
        char* qual;
        void* data;
        int malloc_len;
        int len;
};

struct tl_seq_buffer{
        struct tl_seq** sequences;
        void* data;
        int malloc_num;
        int offset;
        int base_quality_offset;
        int num_seq;
        int max_len;
        int is_fastq;
        int L;
};


#define TL_SEQ_MAX_NAME_LEN 128
#define TL_SEQ_BUFFER_PROTEIN 20
#define TL_SEQ_BUFFER_DNA 4

EXTERN int detect_format(struct tl_seq_buffer* sb);

EXTERN int alloc_tl_seq_buffer(struct tl_seq_buffer** seq_buf, int size);
EXTERN int resize_tl_seq_buffer(struct tl_seq_buffer* sb);
EXTERN int reset_tl_seq_buffer(struct tl_seq_buffer* sb);
EXTERN void free_tl_seq_buffer(struct tl_seq_buffer* sb);

EXTERN int alloc_tl_seq(struct tl_seq** sequence);
EXTERN int resize_tl_seq(struct tl_seq* s);
EXTERN void free_tl_seq(struct tl_seq* sequence);


#undef TLSEQBUFFER_IMPORT
#undef EXTERN

#endif
