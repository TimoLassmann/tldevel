
#include "tldevel.h"
#include "tlseqio.h"

int main(int argc, char *argv[])
{
        struct file_handler* f = NULL;
        struct file_handler* f_out = NULL;
        struct file_handler* f_out_bam = NULL;
        struct tl_seq_buffer* sb = NULL;
        char* aux; 
        int i;
        if(argc > 1){
                RUN(open_fasta_fastq_file(&f, argv[1], TLSEQIO_READ));


                RUN(open_fasta_fastq_file(&f_out, "Dummy.fastq.gz", TLSEQIO_WRITE ));

//#ifdef HAVE_HTS
                RUN(open_sam_bam(&f_out_bam, "Dummy.bam", TLSEQIO_WRITE ));
//#endif
                //int total_r = 0;
                //int total_w = 0;
                while(1){
                        RUN(read_fasta_fastq_file(f, &sb, 1000));
                        detect_format(sb);
                        //total_r+= sb->num_seq;
                        LOG_MSG("Finished reading chunk: found %d ",sb->num_seq);
                        for(i = 0; i < sb->num_seq;i++){
                                aux = NULL;
                                MMALLOC(aux,sizeof(char) * 1024);
                                snprintf(aux,1024,"FP:Z:CACG BC:Z:ACAGTG FP:Z:GGGG");
                                sb->sequences[i]->data = aux;

                        }

                        if(sb->num_seq == 0){
                                break;
                        }
                        //total_w+= sb->num_seq;
                        //LOG_MSG("%d %d",total_r,total_w);
                        RUN(write_seq_buf(sb,f_out));
                            //RUN(write_fasta_fastq(sb, f_out));
                            //#ifdef HAVE_HTS
                        RUN(write_seq_buf(sb,f_out_bam));
                            //#endif
                        for(i = 0; i < sb->num_seq;i++){
                                aux = sb->sequences[i]->data;
                                sb->sequences[i]->data = NULL;
                                MFREE(aux);
                        }

                }

                free_tl_seq_buffer(sb);
                RUN(close_seq_file(&f));
                RUN(close_seq_file(&f_out));
//#ifdef HAVE_HTS
                RUN(close_seq_file(&f_out_bam));
//#endif
                //fprintf(stdout,"%p",f);
        }

        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;
}
