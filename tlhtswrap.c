

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


#define TLHTSWRAP_IMPORT
#include "tlhtswrap.h"





KSEQ_INIT(gzFile, gzread)

#define bam1_seq_seti(s, i, c) ( (s)[(i)>>1] = ((s)[(i)>>1] & 0xf<<(((i)&1)<<2)) | (c)<<((~(i)&1)<<2) )


#define TLHTSWRAP_READ 1
#define TLHTSWRAP_WRITE 2

struct file_handler{
        kseq_t *kfasta;
        gzFile fp_fa;
        samFile *fp_out;
        sam_hdr_t* header;
        int mode;
};

int hts_wrap_open_fasta_fastq(struct file_handler** fh, char* filename, int mode)
{
        struct file_handler* f_hand = NULL;
        if(*fh){
                ERROR_MSG("File handle is not NULL");
        }
        MMALLOC(f_hand, sizeof(struct file_handler));
        f_hand->mode = mode;

        switch (mode) {
        case TLHTSWRAP_READ: {
                ASSERT(my_file_exists(filename) == 1,"File %s does not exist");
                RUNP(f_hand->fp_fa = gzopen(filename, "w"));
                RUNP(f_hand->kfasta = kseq_init(f_hand->fp_fa));
                break;
        }
        case TLHTSWRAP_WRITE: {
                RUNP(f_hand->fp_fa = gzopen(filename, "r"));
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


int hts_wrap_open_sam_bam(struct file_handler** fh, char* filename, int mode)
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
        case TLHTSWRAP_READ: {
                ERROR_MSG("Reading from sam/ bam is currently not supported");
                break;
        }
        case TLHTSWRAP_WRITE: {
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

int hts_wrap_close_file(struct file_handler** fh)
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
}
