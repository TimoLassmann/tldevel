#include <stdint.h>
#include <string.h>

#include <ctype.h>

#include "tldevel.h"


#define str(x)          # x
#define xstr(x)         str(x)


#define TLSEQBUFFER_IMPORT
#include "tlseqbuffer.h"

/* detect format  */
int detect_format(struct tl_seq_buffer* sb)
{
        /* logic:  */
        int min;
        int i,j,c;

        char instrument_R1[256];
        int run_id_R1 = 0;
        char flowcell_R1[256];
        int flowcell_lane_R1= 0;
        int tile_number_R1= 0;
        int x_coordinate_R1= 0;
        int y_coordinate_R1= 0;

        char* seq;
        char* name;
        char* qual;
        int len;

        int number_of_values_found = 0;

        uint8_t DNA[256];
        uint8_t protein[256];
        uint8_t illumina15[256];
        uint8_t illumina18[256];

        uint8_t query[256];
        int diff[4];
        char DNA_letters[]= "ACGTUN";
        char protein_letters[] = "ACDEFGHIKLMNPQRSTVWY";
        char Illumina15[] = "BCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghi";
        char Illumina18[] = "!\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJ";



        struct results{
                int illumina18_name;
                int illumina15_name;
                int dna_line;
                int protein_line;
                int illumina_18_line;
                int illumina_15_line;
        } res;


        if(sb->num_seq == 0){
                return OK;
        }
        /* init */
        res.illumina15_name = 0;
        res.illumina18_name = 0;
        res.dna_line = 0;
        res.protein_line = 0;
        res.illumina_15_line = 0;
        res.illumina_18_line = 0;

        //ASSERT(sb != NULL, "No sequence buffer.");

        for(i = 0; i <256;i++){
                DNA[i] = 0;
                protein[i] = 0;
                query[i] = 0;
                illumina15[i] = 0;
                illumina18[i] = 0;
        }

        for(i = 0 ; i < (int) strlen(DNA_letters);i++){
                DNA[(int) DNA_letters[i]] = 1;
        }

        for(i = 0 ; i < (int)strlen(protein_letters);i++){
                protein[(int) protein_letters[i]] = 1;
        }
        for(i = 0; i < (int)strlen(Illumina15);i++){
                illumina15[(int) Illumina15[i]] = 1;
        }
        for(i = 0; i < (int)strlen(Illumina18);i++){
                illumina18[(int) Illumina18[i]] = 1;
        }


        for(i = 0; i < sb->num_seq;i++){
                seq = sb->sequences[i]->seq;
                name= sb->sequences[i]->name;
                qual= sb->sequences[i]->qual;
                len= sb->sequences[i]->len;

                /* analyze names  */
                number_of_values_found =sscanf(name  ,"%"xstr(256)"[^:]:%d:%"xstr(256)"[^:]:%d:%d:%d:%d ", instrument_R1,&run_id_R1,flowcell_R1,&flowcell_lane_R1,&tile_number_R1,&x_coordinate_R1,&y_coordinate_R1 );
                if(number_of_values_found == 7){
                        res.illumina18_name++;
                        //      LOG_MSG("Detected casava 1.8 format.\n");
                }


                /* is this an old(er) illumina readname?  */
                number_of_values_found =sscanf(name,"%"xstr(256)"[^:]:%d:%d:%d:%d", instrument_R1,&flowcell_lane_R1,&tile_number_R1,&x_coordinate_R1,&y_coordinate_R1);

                if(number_of_values_found == 5){
                        res.illumina15_name++;
                        //LOG_MSG("Detected casava <1.7 format.\n");
                        //param->messages = append_message(param->messages, param->buffer);
                }

                /* analyze sequences  */
                diff[0] = 0;
                diff[1] = 0;
                diff[2] = 0;
                diff[3] = 0;


                for(j = 0 ; j < 256;j++){
                        query[j] = 0;
                }
                for(j = 0 ; j < len;j++){
                        query[toupper((int) seq[j])] = 1;

                }
                //for(j = 0 ; j < 256;j++){
                //fprintf(stdout,"%d (%d %d)\n", query[j], DNA[j], protein[j]);
                //}
                for(j = 0; j < 256;j++){
                        //if(query[j]){
                                if(query[j] != DNA[j]){
                                        diff[0]++;
                                }
                                if(query[j] != protein[j]){
                                        diff[1]++;
                                }
                                //}
                }
                //fprintf(stdout,"\n%d %d\n", diff[0],diff[1]);
                //exit(0);
                c = -1;
                min = INT32_MAX;
                for(j = 0; j < 2;j++){
                        if(diff[j] < min){
                                min = diff[j];

                        }
                }
                for(j = 0; j < 2;j++){
                        if(diff[j] == min){
                                c = j;
                                switch (c) {
                                case 0:
                                        res.dna_line++;
                                        break;
                                case 1:
                                        res.protein_line++;
                                        break;
                                default:
                                        break;
                                }

                        }

                }


                for(j = 0 ; j < 256;j++){
                        query[j] = 0;
                }
                for(j = 0 ; j < len;j++){
                        query[(int)qual[j]] = 1;
                }
                for(j = 0; j < 256;j++){
                        if(query[j]){
                                if(query[j] != illumina15[j]){
                                        diff[2]++;
                                }
                                if(query[j] != illumina18[j]){
                                        diff[3]++;
                                }
                        }
                }
                c = -1;
                min = INT32_MAX;
                for(j = 2; j < 4;j++){
                        if(diff[j] < min){
                                min = diff[j];

                        }
                }
                for(j = 2; j < 4;j++){
                        if(diff[j] == min){
                                c = j;
                                switch (c) {
                                case 2:
                                        res.illumina_15_line++;
                                        break;
                                case 3:
                                        res.illumina_18_line++;
                                        break;
                                default:
                                        break;
                                }

                        }

                }


        }

        if(res.dna_line > res.protein_line){

                sb->L = TL_SEQ_BUFFER_DNA;
        }else{
                sb->L = TL_SEQ_BUFFER_PROTEIN;
        }
        if(res.illumina18_name > res.illumina15_name){
                if(res.illumina_18_line > res.illumina_15_line){
                        sb->base_quality_offset = 33;
                }else{
                        sb->base_quality_offset = 0;
                }
        }
        if(res.illumina15_name > res.illumina18_name){
                if(res.illumina_15_line > res.illumina_18_line){
                        sb->base_quality_offset = 64;
                }else{
                        sb->base_quality_offset = 0;
                }
        }

        if(sb->base_quality_offset == 0){

                WARNING_MSG("Could not detect file format.");
                WARNING_MSG("This is the file info:");
                WARNING_MSG("Among %d sequences there are:", sb->num_seq);
                WARNING_MSG("%d\tIllumina 1.8 read names", res.illumina18_name);
                WARNING_MSG("%d\tIllumina 1.5 read names", res.illumina15_name);
                WARNING_MSG("%d\tDNA lines", res.dna_line);
                WARNING_MSG("%d\tProtein lines", res.protein_line);
                WARNING_MSG("%d\tIllumina 1.8 base quality lines.", res.illumina_18_line);
                WARNING_MSG("%d\tIllumina 1.5 base quality lines.", res.illumina_15_line);

                WARNING_MSG("");
                if(res.illumina18_name || res.illumina15_name){
                        if(res.illumina18_name > res.illumina15_name){
                                WARNING_MSG("Will assume the base qualities are in standard Sanger format (Phred + 33)");
                                sb->base_quality_offset = 33;

                        }else{
                                WARNING_MSG("Will assume the base qualities are in older Illumina format (Phred + 64)");
                                sb->base_quality_offset = 64;
                        }

                }else{
                        WARNING_MSG("Will assume the base qualities are in standard Sanger format (Phred + 33)");
                        sb->base_quality_offset = 33;
                }

                //WARNING_MSG("Will assume the base qualities are in standard Sanger format (Phred + 33)");


        }
        return OK;
}

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
        sb->base_quality_offset = 0;
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
        int i;
        MMALLOC(s, sizeof(struct tl_seq));
        s->malloc_len = 128;
        s->len = 0;
        s->seq = NULL;
        s->qual = NULL;
        s->name = NULL;
        s->aux = NULL;
        MMALLOC(s->seq, sizeof(char)* s->malloc_len);
        MMALLOC(s->qual, sizeof(char) * s->malloc_len);
        MMALLOC(s->name, sizeof(char) * TL_SEQ_MAX_NAME_LEN);
        for(i = 0; i < s->malloc_len;i++){
                s->qual[i] = 'J';
        }
        *sequence = s;
        return OK;
ERROR:
        free_tl_seq(s);
        return FAIL;
}

int resize_tl_seq(struct tl_seq* s)
{
        int i,o;
        ASSERT(s != NULL, "No sequence");
        o = s->malloc_len;
        s->malloc_len = s->malloc_len + s->malloc_len / 2;
        //LOG_MSG("New len: %d", s->malloc_len);
        MREALLOC(s->seq, sizeof(char)* s->malloc_len);
        MREALLOC(s->qual, sizeof(char) * s->malloc_len);
        for(i = o; i < s->malloc_len;i++){
                s->qual[i] = 'J';
        }
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
