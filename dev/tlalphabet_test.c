

#include "tldevel.h"

#include "tlalphabet.h"

#include <stdint.h>
#include <string.h>

int print_seq(uint8_t* seq, int len);

int main(int argc, char *argv[])
{
        struct alphabet* a = NULL;
        int len;
        int i;
        char dna[] = "ACGTACGTACGTNNNN";

        uint8_t* dna_seq = NULL;
        len = strlen(dna);
        MMALLOC(dna_seq,sizeof(uint8_t) * len);

        RUN(create_alphabet(&a,NULL,TLALPHABET_DEFAULT_DNA));
        for(i = 0; i < strlen(dna);i++){
                dna_seq[i] = (uint8_t) dna[i];
        }

        print_seq(dna_seq, len);

        RUN(convert_to_internal(a, dna_seq,len));
        //RUN(convert_to_external(a, dna_seq,len));
        print_seq(dna_seq, len);

        free_alphabet(a);

        RUN(create_alphabet(&a,NULL,TLALPHABET_NOAMBIGUOUS_DNA ));

        for(i = 0; i < strlen(dna);i++){
                dna_seq[i] = (uint8_t) dna[i];
        }

        print_seq(dna_seq, len);

        RUN(convert_to_internal(a, dna_seq,len));
        //RUN(convert_to_external(a, dna_seq,len));
        print_seq(dna_seq, len);

        free_alphabet(a);
        exit(0);
//convert_to_internal(dna_seq);
        //convert_to_


        RUN(create_alphabet(&a,NULL,TLALPHABET_DEFAULT_PROTEIN));

        //print_alphabet(a);
        free_alphabet(a);

        a = NULL;
        RUN(create_alphabet(&a,NULL,TLALPHABET_REDUCED_PROTEIN));

        //print_alphabet(a);
        free_alphabet(a);


        RUN(create_alphabet(&a,NULL,TLALPHABET_DEFAULT_DNA));

        //print_alphabet(a);
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
                fprintf(stdout,"%d", seq[i]);
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
