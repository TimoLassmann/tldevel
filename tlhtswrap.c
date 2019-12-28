

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#include "htslib/kseq.h"
#include "htslib/kstring.h"
#include "htslib/hfile.h"
#include "htslib/sam.h"
#include "htslib/bgzf.h"
#include "htslib/kseq.h"
#include "htslib/hts.h"

#include "tldevel.h"
#include "tlmisc.h"




#define TLSEQIO_IMPORT
#include "tlseqio.h"


KSEQ_INIT(gzFile, gzread)

#define bam1_seq_seti(s, i, c) ( (s)[(i)>>1] = ((s)[(i)>>1] & 0xf<<(((i)&1)<<2)) | (c)<<((~(i)&1)<<2) )


struct file_handler{
        kseq_t *kfasta;
        gzFile fp_fa;
        samFile *fp_out;
        sam_hdr_t* header;
        int mode;
};


int open_fasta_fastq_file(struct file_handler** fh, char* filename, int mode)
{
        struct file_handler* f_hand = NULL;
        if(*fh){
                ERROR_MSG("File handle is not NULL");
        }
        MMALLOC(f_hand, sizeof(struct file_handler));
        f_hand->mode = mode;
        f_hand->fp_fa = NULL;
        f_hand->fp_out = NULL;
        f_hand->header = NULL;
        f_hand->kfasta = NULL;

        switch (mode) {
        case TLSEQIO_READ: {
                ASSERT(my_file_exists(filename) == 1,"File %s does not exist");
                RUNP(f_hand->fp_fa = gzopen(filename, "r"));
                RUNP(f_hand->kfasta = kseq_init(f_hand->fp_fa));
                break;
        }
        case TLSEQIO_WRITE: {
                RUNP(f_hand->fp_out = hts_open(filename, "wb"));
                break;
        }
        default:
                ERROR_MSG("Unknown mode");
                break;
        }

        *fh = f_hand;

        return OK;
ERROR:
        MFREE(f_hand);
        return FAIL;
}


int open_sam_bam(struct file_handler** fh, char* filename, int mode)
{
        struct file_handler* f_hand = NULL;
        char wmode[3] = {'w', 'b', 0};
        int status;
        if(*fh){
                ERROR_MSG("File handle is not NULL");
        }
        MMALLOC(f_hand, sizeof(struct file_handler));
        f_hand->mode = mode;
        f_hand->fp_fa = NULL;
        f_hand->fp_out = NULL;
        f_hand->header = NULL;
        f_hand->kfasta = NULL;

        switch (mode) {
        case TLSEQIO_READ: {
                ERROR_MSG("Reading from sam/ bam is currently not supported");
                break;
        }
        case TLSEQIO_WRITE: {
                status = sam_open_mode(wmode+1, filename,NULL);
                ASSERT(status != -1, "File extension not recognised");
                RUNP(f_hand->fp_out = sam_open(filename, wmode));
                RUNP(f_hand->header = sam_hdr_init());


                status  = sam_hdr_add_line(f_hand->header, "HD", "VN",SAM_FORMAT_VERSION, "SO", "unknown",NULL);
                ASSERT(status != -1, "sam_hdr_add_line failed.");
                status  = sam_hdr_write(f_hand->fp_out, f_hand->header);
                ASSERT(status != -1, "sam_hdr_write failed.");

                break;
        }
        default:
                ERROR_MSG("Unknown mode");
                break;
        }

        *fh = f_hand;

        return OK;
ERROR:
        MFREE(f_hand);
        return FAIL;
}

int read_fasta_fastq_file(struct file_handler* fh, struct tl_seq_buffer** seq_buf, int num)
{
        struct tl_seq_buffer* sb = NULL;
        kseq_t *ks;
        int l;
        int i;
        int j;
        int c;
        char* tmp;
        ASSERT(fh!= NULL, "No file handler");
        ASSERT(num > 0, "Need to read more than %d sequences",num);


        sb = *seq_buf;
        if(sb == NULL){
                //LOG_MSG("Allocating seq buffer");
                RUN(alloc_tl_seq_buffer(&sb, num));
        }else{
                while(num > sb->malloc_num){
                        RUN(resize_tl_seq_buffer(sb));
                }
        }


        sb->is_fastq = 0;

        RUN(reset_tl_seq_buffer(sb));

        ks = fh->kfasta;
        while ((l = kseq_read(ks)) >= 0) {
                //printf("%s %s\n",ks->name.s, ks->comment.s);
                tmp = sb->sequences[sb->num_seq]->name;
                if(ks->comment.l){
                        snprintf(tmp, TL_SEQ_MAX_NAME_LEN, "%s %s", ks->name.s,ks->comment.s);
                }else{
                        snprintf(tmp, TL_SEQ_MAX_NAME_LEN, "%s", ks->name.s);
                }

                while(ks->seq.m > sb->sequences[sb->num_seq]->malloc_len){
                        RUN(resize_tl_seq(sb->sequences[sb->num_seq]));
                }
                tmp = sb->sequences[sb->num_seq]->seq;

                for(i = 0; i <ks->seq.l;i++){
                         tmp[i] = ks->seq.s[i];
                }
                tmp[ks->seq.l] =0;

                if(ks->qual.l){
                        sb->is_fastq = 1;
                        tmp = sb->sequences[sb->num_seq]->qual;


                        for(i = 0; i <ks->qual.l;i++){
                                tmp[i] = ks->qual.s[i];
                        }
                        tmp[ks->qual.l] =0;


                }
                //fprintf(stdout,"%s\n",sb->sequences[sb->num_seq]->name);
                //fprintf(stdout,"%s\n",sb->sequences[sb->num_seq]->seq);
                //fprintf(stdout,"%s\n",sb->sequences[sb->num_seq]->qual);
                sb->num_seq++;
                if(sb->num_seq == num){
                        break;
                }
        }

        *seq_buf = sb;
        return OK;
ERROR:
        return FAIL;
}


int write_fasta_fastq(struct tl_seq_buffer* sb, struct file_handler* fh)
{

        int i,r;
        htsFile* fp_out;
        ssize_t bytes;

        ASSERT(sb != NULL, "No sequence buffer");

        ASSERT(fh->fp_out != NULL, "file handle is NULL");
        fp_out = fh->fp_out;
        r = 0;
        for(i = 0; i < sb->num_seq;i++){

                fp_out->line.l = 0;
                r |= kputc('@', &fp_out->line) < 0;
                r |= kputs(sb->sequences[i]->name, &fp_out->line) < 0;
                r |= kputc('\n', &fp_out->line) < 0;
                r |= kputs(sb->sequences[i]->seq, &fp_out->line) < 0;
                r |= kputc('\n', &fp_out->line) < 0;
                if(sb->is_fastq){
                        r |= kputc('+', &fp_out->line) < 0;
                        r |= kputc('\n', &fp_out->line) < 0;
                        r |= kputs(sb->sequences[i]->qual, &fp_out->line) < 0;
                        r |= kputc('\n', &fp_out->line) < 0;
                }
                //printf("%s %d\n",fp_out->line.s, fp_out->line.l);

                //r |= kputsn("\tLN:", 4, &fp->line) < 0;
                //r |= kputw(h->target_len[i], &fp->line) < 0;
                //r |= kputc('\n', &fp->line) < 0;
                if (r != 0){
                        return -1;
                }

                if (fp_out->format.compression == bgzf) {
                        bytes = bgzf_write(fp_out->fp.bgzf, fp_out->line.s, fp_out->line.l);
                } else {
                        bytes = hwrite(fp_out->fp.hfile, fp_out->line.s, fp_out->line.l);
                }
                if (bytes != fp_out->line.l){
                        return -1;
                }
        }

        return OK;
ERROR:
        return FAIL;
}

int close_seq_file(struct file_handler** fh)
{
        struct file_handler* f_hand = NULL;

        f_hand = *fh;
        if(!f_hand){
                ERROR_MSG("file handle is empty!");
        }
        if(f_hand->fp_fa){
                gzclose(f_hand->fp_fa);
        }
        if(f_hand->fp_out){
                hts_close(f_hand->fp_out);
        }
        if(f_hand->header){

        }
        if(f_hand->kfasta){
                kseq_destroy(f_hand->kfasta);
        }
        MFREE(f_hand);
        f_hand = NULL;

        *fh = f_hand;
        return OK;
ERROR:
        return FAIL;
}
