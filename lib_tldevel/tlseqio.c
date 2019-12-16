


#include <string.h>


#include "tldevel.h"
#include "tlmisc.h"

#define TLSEQIO_IMPORT
#include "tlseqio.h"

int get_io_handler(struct file_handler** fh,const char* filename)
{
        struct file_handler* f_handle = NULL;
        char* ret = NULL;
        gzFile f_ptr = NULL;


        ASSERT(my_file_exists(filename), "File: %s not found.", filename);

        if(!*fh){
                LOG_MSG("FH not allocated");
                MMALLOC(f_handle, sizeof(struct file_handler));
                //        *fh = f_handle;
        }

        fprintf(stdout,"%p\n", f_handle);
        f_handle->f_ptr = NULL;
        f_handle->file_type = 0;
        f_handle->seq_index = NULL;

        ret = strstr(filename, "fasta");
        if(ret){
                f_handle->file_type += TLSEQIO_FASTA;
        }
        ret = strstr(filename, "fastq");
        if(ret){
                f_handle->file_type+= TLSEQIO_FASTQ;
        }
        ret = strstr(filename, "GHz");
        if(ret){
                f_handle->file_type |= TLSEQIO_GZIPPED;
        }

        RUNP(f_ptr = gzopen(filename, "r"));
        //RUNP(fp = gzopen(filename, file_mode));

        f_handle->f_ptr = f_ptr;

        *fh = f_handle;
        return OK;
ERROR:
        return FAIL;
}

void free_io_handler(struct file_handler* f)
{
        if(f){
                if(f->f_ptr){
                        gzclose(f->f_ptr);
                }
                MFREE(f);
        }
}

#define LENGTH  0x1000
int echo_file(struct file_handler* f)
{
        while (1) {
                int bytes_read;
                unsigned char buffer[LENGTH];
                bytes_read = gzread ( f->f_ptr, buffer, LENGTH - 1);
                buffer[bytes_read] = '\0';
                printf ("%s", buffer);
                if (bytes_read < LENGTH - 1) {
                        if (gzeof (f->f_ptr )) {
                                break;
                        }else{
                                ERROR_MSG("Something went wrong");
                        }
                }
        }
        return OK;
ERROR:
        return FAIL;
}
