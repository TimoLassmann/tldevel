
#include <string.h>


#include "tldevel.h"
#include "tlmisc.h"

#define TLSEQIO_IMPORT
#include "tlseqio.h"

#define TL_SEQ_MAX_NAME_LEN 128

//#define TL_READ_BUFF_LEN 8388608
#define TL_READ_BUFF_LEN 300

#define str(x)          # x
#define xstr(x)         str(x)

#define FILE_TYPE_UNDEFINED 3
#define FILE_TYPE_FASTA 1
#define FILE_TYPE_FASTQ 2

#define RS_UNDEFINED 0
#define RS_NAME 1
#define RS_SEQ 2
#define RS_QUAL 3

static int detect_fasta_fastq(const char* b, int len, int* type);

static int read_sequences(struct file_handler*fh, struct tl_seq_buffer* sb, int num);

static int parse_buf_fasta(struct file_handler* fh, struct tl_seq_buffer* sb,int num);
static int parse_buf_fastq(struct file_handler* fh, struct tl_seq_buffer* sb,int num);

static int read_file_contents(struct file_handler* fh);

static  int get_io_handler(struct file_handler** fh,const char* filename);
static  void free_io_handler(struct file_handler* f);

static  int alloc_tl_seq_buffer(struct tl_seq_buffer** seq_buf, int size);
static  int resize_tl_seq_buffer(struct tl_seq_buffer* sb);
static  int reset_tl_seq_buffer(struct tl_seq_buffer* sb);



static  int resize_tl_seq(struct tl_seq* s);



static int alloc_tl_seq(struct tl_seq** sequence);
static void free_tl_seq(struct tl_seq* sequence);



int open_fasta_fastq_file(struct file_handler** fh,char* filename)
{
        struct file_handler* f = NULL;
        int type = 0;
        int status;
        ASSERT(my_file_exists(filename),"File: %s does not exists",filename);
        RUN(get_io_handler(&f, filename));

        RUN(read_file_contents(f));

        /* detect file type  */

        RUN(detect_fasta_fastq(f->read_buffer , f->bytes_read, &type));

        status = gzrewind(f->f_ptr);
        if(status){
                ERROR_MSG("gzrewind failed");
        }
        switch (type) {
        case FILE_TYPE_FASTA:
                LOG_MSG("Found fasta");
                break;
        case FILE_TYPE_FASTQ:
                LOG_MSG("Found fastq");
                break;
        case FILE_TYPE_UNDEFINED:
                ERROR_MSG("Could not determine type of file: %s",filename);
                break;

        default:
                break;
        }
        f->file_type = type;
        *fh = f;
        return OK;
ERROR:
        free_io_handler(f);
        return FAIL;
}

int read_fasta_fastq_file(struct file_handler* fh, struct tl_seq_buffer** seq_buf, int num)
{
        struct tl_seq_buffer* sb = NULL;
        ASSERT(fh!= NULL, "No file handler");
        ASSERT(num > 0, "Need to read more than %d sequences",num);


        sb = *seq_buf;
        if(sb == NULL){
                LOG_MSG("Allocating seqbuffer");
                RUN(alloc_tl_seq_buffer(&sb, num));
        }else{
                while(num > sb->malloc_num){
                        RUN(resize_tl_seq_buffer(sb));
                }
        }

        RUN(reset_tl_seq_buffer(sb));

        /* reading  */

        RUN(read_sequences(fh,sb,num));
        //if(fh->file_type == FILE_TYPE_FASTA){
        //RUN(read_fasta(fh,sb,num));
        //}

        *seq_buf = sb;
        return OK;
ERROR:
        return FAIL;
}

int read_sequences(struct file_handler*fh, struct tl_seq_buffer* sb, int num)
{
        while(1){
                if (gzeof (fh->f_ptr)){
                        if(fh->file_type == FILE_TYPE_FASTA){
                                sb->num_seq++;
                        }
                        break;
                }

                if(fh->pos){
                        //LOG_MSG("Finishing buffer");
                        if(fh->file_type == FILE_TYPE_FASTA){
                                RUN(parse_buf_fasta(fh,sb,num));
                        }else if(fh->file_type == FILE_TYPE_FASTQ){
                                RUN(parse_buf_fastq(fh,sb,num));
                        }else{
                                ERROR_MSG("Unknown file type");
                        }

                        if(sb->num_seq == num){
                                break;
                        }
                }else{
                        //LOG_MSG("read new content");
                        RUN(read_file_contents(fh));
                        if(!fh->bytes_read){
                                break;
                        }
                        if(fh->file_type == FILE_TYPE_FASTA){
                                RUN(parse_buf_fasta(fh,sb,num));
                        }else if(fh->file_type == FILE_TYPE_FASTQ){
                                RUN(parse_buf_fastq(fh,sb,num));
                        }else{
                                ERROR_MSG("Unknown file type");
                        }
                        if(sb->num_seq == num){
                                break;
                        }
                }

        }
        return OK;
ERROR:
        return FAIL;
}

int parse_buf_fasta(struct file_handler* fh, struct tl_seq_buffer* sb,int num)
{
        char* buf = NULL;
        uint8_t* seq = NULL;
        int state;
        int pos;
        int len;
        int i;
        int buf_len;
        ASSERT(fh!= NULL, "No file handler");
        ASSERT(sb!= NULL, "No sequence buffer");
        buf = fh->read_buffer;
        pos = fh->pos;
        buf_len = fh->bytes_read;
        state = fh->read_state;
        //fprintf(stdout,"%s", buf);
        for(i = pos;i < buf_len;i++){
                if(buf[i] == '>'){
                        //LOG_MSG("state:%d seq: %d", state, sb->num_seq);
                        if(state == RS_SEQ || state == RS_QUAL){
                                sb->num_seq++;
                        }
                        if(sb->num_seq == num){
                                fh->pos = i;
                                fh->read_state = state;
                                return OK;
                        }

                        /* copy name */
                        i++;
                        len =0;
                        while(1){
                                if(buf[i] == '\n' || buf[i] == 0){
                                        sb->sequences[sb->num_seq]->name[len] = 0;
                                        break;
                                }

                                sb->sequences[sb->num_seq]->name[len] = buf[i];
                                len++;
                                if(len+1 == TL_SEQ_MAX_NAME_LEN){
                                        sb->sequences[sb->num_seq]->name[len] = 0;
                                        break;
                                }
                                i++;
                        }
                        state = RS_SEQ;
                }else if(buf[i] == '+'){
                        state = RS_QUAL;
                }else{
                        if(state == RS_SEQ){
                                len = sb->sequences[sb->num_seq]->len;
                                seq  =sb->sequences[sb->num_seq]->seq;
                                while(1){
                                        if(buf[i] == '\n' || buf[i] == 0){
                                                break;
                                        }
                                        seq[len] = buf[i];
                                        len++;
                                        //LOG_MSG("%d",len);
                                        if(len == sb->sequences[sb->num_seq]->malloc_len){
                                                //LOG_MSG("Resize from: %d", sb->sequences[sb->num_seq]->malloc_len);
                                                RUN(resize_tl_seq(sb->sequences[sb->num_seq]));
                                                //LOG_MSG("Resize   to: %d", sb->sequences[sb->num_seq]->malloc_len);
                                                seq  =sb->sequences[sb->num_seq]->seq;
                                        }
                                        i++;
                                }
                                sb->sequences[sb->num_seq]->len = len;

                        }
                }
        }

        /* propagate read state  */
        fh->read_state = state;
        fh->pos = 0;            /* am at end of buffer */

        //LOG_MSG("END state: %d", fh->read_state);
        return OK;
ERROR:
        return FAIL;
}

int parse_buf_fastq(struct file_handler* fh, struct tl_seq_buffer* sb,int num)
{
        char* buf = NULL;
        uint8_t* seq = NULL;
        char* qual;
        int state;
        int pos;
        int len;
        int i;
        int buf_len;
        ASSERT(fh!= NULL, "No file handler");
        ASSERT(sb!= NULL, "No sequence buffer");
        buf = fh->read_buffer;
        pos = fh->pos;
        buf_len = fh->bytes_read;
        state = fh->read_state;
        //fprintf(stdout,"state: %d\n", state);
        //fprintf(stdout,"BUFF:\n%s\n", buf);
        for(i = pos;i < buf_len;i++){
                if(buf[i] == '@' && (state == RS_NAME || state == RS_UNDEFINED)){
                        /* works because when we start reading state will be undefined */
                        /* copy name */
                        i++;
                        len =0;
                        while(1){
                                if(buf[i] == '\n' || buf[i] == 0){
                                        sb->sequences[sb->num_seq]->name[len] = 0;
                                        break;
                                }

                                sb->sequences[sb->num_seq]->name[len] = buf[i];
                                len++;
                                if(len+1 == TL_SEQ_MAX_NAME_LEN){
                                        sb->sequences[sb->num_seq]->name[len] = 0;
                                        break;
                                }
                                i++;
                        }
                        //fprintf(stdout, "NAME: %s\n",sb->sequences[sb->num_seq]->name);
                        state = RS_SEQ;
                }else if(buf[i] == '+' && state == RS_QUAL){
                        state = RS_QUAL;
                        while(1){
                                if(buf[i] == '\n' || buf[i] == 0){
                                        break;
                                }
                                i++;
                        }
                }else{
                        if(state == RS_SEQ){
                                len = sb->sequences[sb->num_seq]->len;
                                seq  =sb->sequences[sb->num_seq]->seq;
                                while(1){
                                        if(buf[i] == '\n' || buf[i] == 0){
                                                break;
                                        }
                                        seq[len] = buf[i];
                                        len++;
                                        //LOG_MSG("%d",len);
                                        if(len == sb->sequences[sb->num_seq]->malloc_len){
                                                //LOG_MSG("Resize from: %d", sb->sequences[sb->num_seq]->malloc_len);
                                                RUN(resize_tl_seq(sb->sequences[sb->num_seq]));
                                                //LOG_MSG("Resize   to: %d", sb->sequences[sb->num_seq]->malloc_len);
                                                seq  =sb->sequences[sb->num_seq]->seq;
                                        }
                                        i++;
                                }
                                sb->sequences[sb->num_seq]->len = len;
                                state = RS_QUAL;
                        }else if(state == RS_QUAL){
                                len = 0;
                                qual = sb->sequences[sb->num_seq]->qual;
                                while(1){
                                        ///fprintf(stdout,"<<<%c>>>",buf[i]);
                                        if(buf[i] == '\n' || buf[i] == 0){
                                                break;
                                        }
                                        qual[len] = buf[i];
                                        len++;
                                        //LOG_MSG("%d",len);
                                        if(len == sb->sequences[sb->num_seq]->malloc_len){
                                                ERROR_MSG("Quality string is longer than sequence");
                                        }
                                        i++;
                                }
                                sb->sequences[sb->num_seq]->qual = qual;
                                ASSERT(len == sb->sequences[sb->num_seq]->len,"seq and qual have different lengths: %d %d",len, sb->sequences[sb->num_seq]->len);
                                /* if I read everyting until here and seqlen == qual len I am done */
                                sb->num_seq++;
                                state = RS_NAME;



                        }
                        if(sb->num_seq == num){
                                fh->pos = i;
                                fh->read_state = state;
                                //LOG_MSG("Internal stop:state: %d seq: %d pos: %d", fh->read_state, sb->num_seq,fh->pos);
                                //fprintf(stdout,"leftover:\n%s\n", buf+fh->pos);
                                return OK;
                        }

                }
        }

        /* checkif last entry is complete */

        /* propagate read state  */
        fh->read_state = state;
        fh->pos = 0;            /* am at end of buffer */

        //LOG_MSG("END state: %d", fh->read_state);
        return OK;
ERROR:
        return FAIL;
}

int close_fasta_fastq_file(struct file_handler** fh)
{
        if(*fh){
                free_io_handler(*fh);
                *fh = NULL;
        }
        return OK;
}

int detect_fasta_fastq(const char* b, int len, int* type)
{
        /* logic:  */
        char* local_b = NULL;
        int min;
        int i,c;
        char delim[2];
        char instrument_R1[256];
        int run_id_R1 = 0;
        char flowcell_R1[256];
        int flowcell_lane_R1= 0;
        int tile_number_R1= 0;
        int x_coordinate_R1= 0;
        int y_coordinate_R1= 0;

        int number_of_values_found = 0;

        uint8_t DNA[256];
        uint8_t protein[256];
        uint8_t illumina15[256];
        uint8_t illumina18[256];

        uint8_t query[256];
        int diff[4];
        char DNA_letters[]= "acgtACGTnN";
        char protein_letters[] = "ACDEFGHIKLMNPQRSTVWY";
        char Illumina15[] = "BCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghi";
        char Illumina18[] = "!\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJ";

        char* token = NULL;

        struct results{
                int illumina18_name;
                int illumina15_name;
                int dna_line;
                int protein_line;
                int illumina_18_line;
                int illumina_15_line;
                int at_lines;
                int gt_lines;
                int plus_lines;
        } res;

        ASSERT(b != NULL, "No buffer");

        MMALLOC(local_b, sizeof(char) * (len+1));

        RUNP(strncpy(local_b, b, len));
        local_b[len] = 0;
        delim[0] = '\n';
        delim[1] = 0;
        /* init */
        res.illumina15_name = 0;
        res.illumina18_name = 0;
        res.dna_line = 0;
        res.protein_line = 0;
        res.illumina_15_line = 0;
        res.illumina_18_line = 0;
        res.at_lines = 0;
        res.plus_lines = 0;
        res.gt_lines = 0;

        //ASSERT(sb != NULL, "No sequence buffer.");

        for(i = 0; i <256;i++){
                DNA[i] = 0;
                protein[i] = 0;
                query[i] = 0;
                illumina15[i] = 0;
                illumina18[i] = 0;
        }

        for(i = 0 ; i < strlen(DNA_letters);i++){
                DNA[(int) DNA_letters[i]] = 1;
        }

        for(i = 0 ; i < strlen(protein_letters);i++){
                protein[(int) protein_letters[i]] = 1;
        }
        for(i = 0; i < strlen(Illumina15);i++){
                illumina15[(int) Illumina15[i]] = 1;
        }
        for(i = 0; i < strlen(Illumina18);i++){
                illumina18[(int) Illumina18[i]] = 1;
        }

        //fprintf(stdout,"BUFFER:\n%s",b);
        token= strtok(local_b, delim);

        while(token != NULL){
                c = strnlen(token, TL_SEQ_MAX_NAME_LEN );

                /* tests  */
                /* is this an illumina 1.8 readname?  */
                number_of_values_found =sscanf(token,"%"xstr(256)"[^:]:%d:%"xstr(256)"[^:]:%d:%d:%d:%d ", instrument_R1,&run_id_R1,flowcell_R1,&flowcell_lane_R1,&tile_number_R1,&x_coordinate_R1,&y_coordinate_R1 );
                if(number_of_values_found == 7){
                        res.illumina18_name++;
                        //      LOG_MSG("Detected casava 1.8 format.\n");
                }


                /* is this an old(er) illumina readname?  */
                number_of_values_found =sscanf(token,"%"xstr(256)"[^:]:%d:%d:%d:%d", instrument_R1,&flowcell_lane_R1,&tile_number_R1,&x_coordinate_R1,&y_coordinate_R1);

                if(number_of_values_found == 5){
                        res.illumina15_name++;
                        //LOG_MSG("Detected casava <1.7 format.\n");
                        //param->messages = append_message(param->messages, param->buffer);
                }

                if(token[0] == '@'){
                        res.at_lines++;
                }

                if(token[0] == '+'){
                        res.plus_lines++;
                }
                if(token[0] == '>'){
                        res.gt_lines++;
                }

                for(i = 0 ; i < 256;i++){
                        query[i] = 0;
                }
                for(i = 0 ; i < c;i++){
                        query[(int) token[i]] = 1;
                }

                diff[0] = 0;
                diff[1] = 0;
                diff[2] = 0;
                diff[3] = 0;
                for(i = 0; i < 256;i++){
                        if(query[i]){
                                if(query[i] != DNA[i]){
                                        diff[0]++;
                                }
                                if(query[i] != protein[i]){
                                        diff[1]++;
                                }
                                if(query[i] != illumina15[i]){
                                        diff[2]++;
                                }
                                if(query[i] != illumina18[i]){
                                        diff[3]++;
                                }
                        }
                }

                c = -1;
                min = INT32_MAX;
                for(i = 0; i < 4;i++){
                        if(diff[i] < min){
                                min = diff[i];

                        }
                }
                for(i = 0; i < 4;i++){
                        if(diff[i] == min){
                                c = i;
                                switch (c) {
                                case 0:
                                        res.dna_line++;
                                        break;
                                case 1:
                                        res.protein_line++;
                                        break;
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
                //fprintf(stdout,"%d\t%s\n",c, token);


                token = strtok(NULL, delim);

        }

        MFREE(local_b);


        /* LOGIC */
        *type = FILE_TYPE_UNDEFINED;
        if(res.gt_lines && (!res.at_lines ||  !res.plus_lines)){
                *type = FILE_TYPE_FASTA;
        }

        if(res.illumina18_name && res.at_lines && res.plus_lines){
                *type = FILE_TYPE_FASTQ;
        }

        if(res.illumina15_name && res.at_lines && res.plus_lines){
                *type = FILE_TYPE_FASTQ;

        }
        if(*type == FILE_TYPE_UNDEFINED){
                WARNING_MSG("Could not detect file format.");
                WARNING_MSG("This is the file info:");
                WARNING_MSG("Summary:");
                WARNING_MSG("%d\tIllumina 1.8 read names", res.illumina18_name);
                WARNING_MSG("%d\tIllumina 1.5 read names", res.illumina15_name);
                WARNING_MSG("%d\tDNA lines", res.dna_line);
                WARNING_MSG("%d\tProtein lines", res.protein_line);
                WARNING_MSG("%d\tIllumina 1.8 base quality lines.", res.illumina_18_line);
                WARNING_MSG("%d\tIllumina 1.5 base quality lines.", res.illumina_15_line);


                WARNING_MSG("%d\t @ lines.", res.at_lines);
                WARNING_MSG("%d\t > lines.", res.gt_lines);
                WARNING_MSG("%d\t + lines.", res.plus_lines);

        }
        return OK;
ERROR:
        if(local_b){
                MFREE(local_b);
        }
        return FAIL;
}


int get_io_handler(struct file_handler** fh,const char* filename)
{
        struct file_handler* f_handle = NULL;
        char* ret = NULL;
        gzFile f_ptr = NULL;


        ASSERT(my_file_exists(filename), "File: %s not found.", filename);

        /* This is a bit ugly - refine in future. */
        if(!*fh){
                MMALLOC(f_handle, sizeof(struct file_handler));
        }

        f_handle->f_ptr = NULL;
        f_handle->file_type = 0;
        f_handle->seq_index = NULL;
        f_handle->read_buffer = NULL;
        f_handle->bytes_read = 0;

        MMALLOC(f_handle->read_buffer, sizeof(char)*  TL_READ_BUFF_LEN + TL_READ_BUFF_LEN);

        f_handle->pos = 0;
        f_handle->read_state = RS_UNDEFINED;
        ret = strstr(filename, "fasta");
        if(ret){
                f_handle->file_type += TLSEQIO_FASTA;
        }
        ret = strstr(filename, "fastq");
        if(ret){
                f_handle->file_type+= TLSEQIO_FASTQ;
        }
        ret = strstr(filename, "gz");
        if(ret){
                f_handle->file_type |= TLSEQIO_GZIPPED;
        }

        RUNP(f_ptr = gzopen(filename, "r"));

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
                if(f->read_buffer){
                        MFREE(f->read_buffer);
                }
                MFREE(f);
        }
}


int echo_file(struct file_handler* f)
{
        char* buf = NULL;

        buf = f->read_buffer;
        while (1) {
                int bytes_read;
                bytes_read = gzread ( f->f_ptr, buf,  TL_READ_BUFF_LEN - 1);
                buf[bytes_read] = '\0';
                //printf ("%s", buf);
                LOG_MSG("Read: %d", bytes_read);
                if (bytes_read < TL_READ_BUFF_LEN - 1) {
                        LOG_MSG("Read: %d", bytes_read);
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
        for(i = 0; i < sb->num_seq;i++){
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
        MMALLOC(s->seq, sizeof(uint8_t)* s->malloc_len);
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
        MREALLOC(s->seq, sizeof(uint8_t)* s->malloc_len);
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


int read_file_contents(struct file_handler* fh)
{

        int i;

        char c;
        ASSERT(fh!=NULL, "No file handler");



        fh->bytes_read = gzread(fh->f_ptr, fh->read_buffer, TL_READ_BUFF_LEN -1);
        fh->read_buffer[fh->bytes_read] = 0;
        fh->pos = 0;
        fprintf(stdout,"%s\n\n", fh->read_buffer);

        //i = fh->bytes_read-1;
        if(fh->read_buffer[fh->bytes_read-1] != '\n'){

                while(1){
                        c = gzgetc(fh->f_ptr);
                        fh->read_buffer[fh->bytes_read] = c;
                        fh->bytes_read++;
                        if(c == '\n'){
                                break;
                        }
                        if(c == -1){
                                break;
                        }
                }
        }
        fh->read_buffer[fh->bytes_read] = 0;

        fprintf(stdout,"%s", fh->read_buffer);
        return OK;
ERROR:
        return FAIL;
}
