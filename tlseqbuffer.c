#include "tldevel.h"

#define TLSEQBUFFER_IMPORT
#include "tlseqbuffer.h"





/* Memory functions  */

int alloc_tl_seq_buffer(struct tl_seq_buffer** seq_buf, int size)
{
        struct tl_seq_buffer* sb = NULL;
        int i;
        ASSERT(size > 0, "Size of sequence buffer is %d",size);
        MMALLOC(sb,sizeof(struct tl_seq_buffer));
        sb->malloc_num = size;
        sb->max_len = 0;
        sb->num_seq = 0;
        sb->L = 0;
        sb->is_fastq = 0;
        sb->offset = 0;
        sb->sequences = NULL;

        MMALLOC(sb->sequences, sizeof(struct tl_seq*) * sb->malloc_num);
        for(i = 0; i < sb->malloc_num;i++){
                sb->sequences[i] = NULL;
                RUN(alloc_tl_seq(&sb->sequences[i]));
        }
        *seq_buf = sb;
        return OK;
ERROR:
        return FAIL;

}

int resize_tl_seq_buffer(struct tl_seq_buffer* sb)
{
        int old =0;
        int i;
        ASSERT(sb != NULL, "No sequence buffer");
        old = sb->malloc_num;
        sb->malloc_num = sb->malloc_num + sb->malloc_num/2;

        MREALLOC(sb->sequences, sizeof(struct tl_seq*) * sb->malloc_num);
        for(i = old; i < sb->malloc_num;i++){
                sb->sequences[i] = NULL;
                RUN(alloc_tl_seq(&sb->sequences[i]));
        }
        return OK;
ERROR:
        return FAIL;
}

int reset_tl_seq_buffer(struct tl_seq_buffer* sb)
{
        int i;
        ASSERT(sb != NULL, "No sequence buffer");
        for(i = 0; i < sb->malloc_num ;i++){
                sb->sequences[i]->len = 0;
        }
        sb->num_seq = 0;       /* horrible hack! as soon as the first seq name is encountered this is incremented to 0...  */
        return OK;
ERROR:
        return FAIL;
}

void free_tl_seq_buffer(struct tl_seq_buffer* sb)
{
        int i;
        if(sb){
                for(i =0; i < sb->malloc_num;i++){
                        free_tl_seq(sb->sequences[i]);
                }
                MFREE(sb->sequences);
                MFREE(sb);
        }
}


int alloc_tl_seq(struct tl_seq** sequence)
{

        struct tl_seq* s = NULL;

        MMALLOC(s, sizeof(struct tl_seq));
        s->malloc_len = 128;
        s->len = 0;
        s->seq = NULL;
        s->qual = NULL;
        s->name = NULL;
        MMALLOC(s->seq, sizeof(char)* s->malloc_len);
        MMALLOC(s->qual, sizeof(char) * s->malloc_len);
        MMALLOC(s->name, sizeof(char) * TL_SEQ_MAX_NAME_LEN);

        *sequence = s;
        return OK;
ERROR:
        free_tl_seq(s);
        return FAIL;
}

int resize_tl_seq(struct tl_seq* s)
{

        ASSERT(s != NULL, "No sequence");
        s->malloc_len = s->malloc_len + s->malloc_len / 2;
        //LOG_MSG("New len: %d", s->malloc_len);
        MREALLOC(s->seq, sizeof(char)* s->malloc_len);
        MREALLOC(s->qual, sizeof(char) * s->malloc_len);
        return OK;
ERROR:
        free_tl_seq(s);
        return FAIL;
}



void free_tl_seq(struct tl_seq* sequence)
{
        if(sequence){
                if(sequence->seq){
                        MFREE(sequence->seq);
                }
                if(sequence->name){
                        MFREE(sequence->name);
                }
                if(sequence->qual){
                        MFREE(sequence->qual);
                }
                MFREE(sequence);
        }
}
