
#include "tldevel.h"
#include "tlseqio.h"



int main(int argc, char *argv[])
{
        struct file_handler* f = NULL;
        struct tl_seq_buffer* sb = NULL;
        int i,j;
        int emergency;
        if(argc > 1){
                RUN(open_fasta_fastq_file(&f, argv[1]));
                //free_io_handler(f);
                emergency = 0;
                while(1){

                        RUN(read_fasta_fastq_file(f, &sb, 10));
                        LOG_MSG("Finished reading chunk: found %d ",sb->num_seq);
                        for(i = 0; i < sb->num_seq;i++){

                                fprintf(stdout,"%s\n", sb->sequences[i]->name);
                                for(j = 0; j < sb->sequences[i]->len;j++){
                                        fprintf(stdout,"%c",(char)sb->sequences[i]->seq[j]);
                                }
                                fprintf(stdout,"\n");
                                /*
                                for(j = 0; j < sb->sequences[i]->len;j++){
                                        fprintf(stdout,"%c",sb->sequences[i]->qual[j]);
                                }
                                fprintf(stdout,"\n");
                                */
                        }
                        if(sb->num_seq == 0){
                                break;
                        }
                        emergency++;
                        if(emergency == 2){
                                break;
                        }
                }

                free_tl_seq_buffer(sb);
                RUN(close_fasta_fastq_file(&f));
                //fprintf(stdout,"%p",f);
        }

        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;
}
