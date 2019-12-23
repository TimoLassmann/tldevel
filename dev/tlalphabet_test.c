

#include <stdint.h>
#include <string.h>


#include "tldevel.h"

#include "tlalphabet.h"


int print_seq(uint8_t* seq, int len);

int main(int argc, char *argv[])
{
        struct alphabet* a = NULL;
        int len = 16;
        int i;
        char dna[] = "ACGTACGTACGTNNNN";

        uint8_t* dna_seq = NULL;

        MMALLOC(dna_seq,sizeof(uint8_t) * len);

        RUN(create_alphabet(&a,NULL,TLALPHABET_DEFAULT_DNA));
        for(i = 0; i < len;i++){
                dna_seq[i] = (uint8_t) dna[i];
        }

        print_seq(dna_seq, len);

        RUN(convert_to_internal(a, dna_seq,len));
        //RUN(convert_to_external(a, dna_seq,len));
        print_seq(dna_seq, len);

        free_alphabet(a);

        RUN(create_alphabet(&a,NULL,TLALPHABET_NOAMBIGUOUS_DNA ));

        for(i = 0; i < len;i++){
                dna_seq[i] = (uint8_t) dna[i];
        }

        print_seq(dna_seq, len);

        RUN(convert_to_internal(a, dna_seq,len));
        //RUN(convert_to_external(a, dna_seq,len));
        print_seq(dna_seq, len);

        free_alphabet(a);
        MFREE(dna_seq);
        return EXIT_SUCCESS;
ERROR:
        return EXIT_FAILURE;
}


int print_seq(uint8_t* seq, int len)
{
        int i;

        for(i = 0; i < len;i++){
                fprintf(stdout,"%d %d\n",i, seq[i]);
        }
        fprintf(stdout,"\n");
        return OK;
}
/*
int print_alphabet(struct alphabet* a)
{
        //LOG_MSG("Type: %d", a->type);
        //fprintf(stdout,"LEN: %d\n",a->L);
        int i;
        for(i = 64;i < 96;i++){
                if(a->to_internal[i] != -1){
                        fprintf(stdout,"%c\t%d\n",  (char)i, a->to_internal[i]);
                }
        }
        return OK;
}
*/
