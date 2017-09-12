#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tldevel.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#include "htslib/cram.h"
#include "htslib/sam.h"
#include "htslib/faidx.h"
#include "htslib/khash.h"


#include "htsglue.h"




typedef struct {
	int32_t line_len, line_blen;
	int64_t len;
	int64_t offset;
} faidx1_t;
KHASH_MAP_INIT_STR(s, faidx1_t)

struct __faidx_t {
	BGZF *bgzf;
	int n, m;
	char **name;
	khash_t(s) *hash;
};

static int get_alignment_length(char*p,bam_hdr_t *h, int* len);

static int rev_cmp(char* p,int len);

static int compare_headers(struct sam_bam_file* sb1_file,struct sam_bam_file* sb2_file);

static int make_si_info_in_sam_bam_file(struct sam_bam_file* sb_file);


int get_max_seq_len_from_sb_buffer(struct sam_bam_file* sb_file,int* max_len)
{
	int i;
	ASSERT(sb_file != NULL,"file handler is NULL.");
	ASSERT(sb_file->buffer != NULL,"sequence buffer is not allocated.");

	*max_len = 0;

	for (i = 0; i < sb_file->buffer_size; i++) {
		if(sb_file->buffer[i]->max_len > *max_len){
			*max_len = sb_file->buffer[i]->max_len;
		}
	}
	

	
	return OK;
ERROR:
	return FAIL;
}


int compare_multiple_SAM_BAM_headers(char** filenames, int num_files)
{
	int i,j;
	
	struct sam_bam_file* sb1_file = NULL;
	struct sam_bam_file* sb2_file = NULL;
	
	for(i = 0; i < num_files;i++){
		
		RUNP(sb1_file= open_SAMBAMfile(filenames[i],0,10,-1,-1));
		for(j = i+1; j <  num_files;j++){
			RUNP(sb2_file= open_SAMBAMfile(filenames[j],0,10,-1,-1));
			
			RUN(compare_headers(sb1_file,sb2_file));
			
			RUN(close_SAMBAMfile(sb2_file));
		}
		
		RUN(close_SAMBAMfile(sb1_file));
	}
	
	
	return OK;
ERROR:
	if(sb1_file){
		RUN(close_SAMBAMfile(sb1_file));
	}
	if(sb2_file){
		RUN(close_SAMBAMfile(sb2_file));
	}
	
	return FAIL;
}

int compare_headers(struct sam_bam_file* sb1_file,struct sam_bam_file* sb2_file)
{
	int i;
	if(sb1_file->header->n_targets != sb2_file->header->n_targets){
		ERROR_MSG("Different number of reference sequences.");
	}
	if(sb1_file->total_length != sb2_file->total_length){
		ERROR_MSG("Different length of references.");
	}
	
	
	for(i = 0; i < sb1_file->header->n_targets;i++){
		if(strcmp(sb1_file->header->target_name[i], sb2_file->header->target_name[i])){
			ERROR_MSG("Different  reference sequence %d: %s %s \n",i,sb1_file->header->target_name[i],sb2_file->header->target_name[i]);
		}
	}
	return OK;
ERROR:
	ERROR_MSG("in files:%s vs %s.",sb1_file->file_name ,sb2_file->file_name );
	return FAIL;
}


faidx_t* get_faidx(const char* fasta_name)
{
	char* index_name = NULL;
	faidx_t* index = NULL;
	
	MMALLOC(index_name, sizeof(char) *( strlen(fasta_name)+1));
	
	snprintf(index_name,  strlen(fasta_name)+1,"%s.fai",fasta_name );
	
	if(my_file_exists(index_name)){
		index = fai_load(fasta_name);
	}else{
		if(fai_build(fasta_name)){
			ERROR_MSG("building faidx for file:%s  failed",fasta_name);
		}
		index = fai_load(fasta_name);
	}
	MFREE(index_name);
	return index;
ERROR:
	if(index_name){
		MFREE(index_name);
	}
	if(index){
		fai_destroy(index);
	}
	return NULL;
}

char* get_sequence(faidx_t* index, struct genome_interval* g_int)//   char* chr, int start, int stop,int strand)
{
	char* sequence = NULL;
	int len = 0;
	static int warning_number = 10;
	
	khiter_t iter;
	faidx1_t val;
	//char *seq=NULL;
	
	// Adjust position
	iter = kh_get(s, index->hash, g_int->chromosome);
	if (iter == kh_end(index->hash))
	{
		ERROR_MSG("get_sequence failed - chr:%s not found in index.",g_int->chromosome);
	}
	val = kh_value(index->hash, iter);
	
	
	ASSERT(g_int->stop >= g_int->start,"Start coordinate bigger than end: %d -> %d.",g_int->start ,g_int->stop);
	
	if( g_int->start < 0){
		 g_int->start = 0;
	}else if(val.len <=  g_int->start){
		 g_int->start = val.len - 1;
	}
	if(g_int->stop < 0){
		g_int->stop = 0;
	}else if(val.len <= g_int->stop){
		g_int->stop = val.len ;
	}
		
	sequence = faidx_fetch_seq(index, g_int->chromosome , g_int->start,  g_int->stop-1, &len);
	
	if(len == -2){
		if(warning_number){
			WARNING_MSG("Your faidx index does not contain sequence: %s",g_int->chromosome);
			warning_number--;
		}
		
	}
	if(len == -1){
		ERROR_MSG("faidx_fetch_seq failed");
	}
	
	if(g_int->strand){
		rev_cmp(sequence, len);
	}
	
	return sequence;
ERROR:
	MFREE(sequence);
	return NULL;
}

int free_faidx(faidx_t*  index)
{
	if(index){
		fai_destroy(index);
	}
	return OK;
}



int read_SAMBAM_chunk(struct sam_bam_file* sb_file,int all, int window)
{
	
	char chr[FIELD_BUFFER_LEN];// = NULL;
	char pos_str[FIELD_BUFFER_LEN];// = NULL;
	char cigar[FIELD_BUFFER_LEN];//= NULL;
	int i;
	int r = 0;
	int num_read = 0;
	int read_flag = 0;
	
	read_flag = sb_file->read_flag;
	sb_file->num_read = 0;
	sb_file->total_entries_in_file = 0;
	bam1_t *b = sb_file->b;
	bam_hdr_t *h =sb_file->header;
	
	
	while ((r = sam_read1(sb_file->in, h, b)) >= 0){
		sb_file->total_entries_in_file++;
		if(!(read_flag & b->core.flag) && b->core.qual >= sb_file->read_Q_threshold){
			struct sam_bam_entry* sb_ptr = sb_file->buffer[sb_file->num_read];
			
                        /* Whether mapped or not get the name , sequence and qual....   */
			uint8_t * seq =    bam_get_seq(b);
			uint8_t* qual_ptr = bam_get_qual(b);
			
			snprintf(sb_ptr->name, MAX_SEQ_NAME_LEN,"%s",bam_get_qname(b));				

			sb_ptr->num_hits = 0;
			sb_ptr->qual = b->core.qual;

			/* read in len of sequence */
				
			sb_ptr->len =b->core.l_qseq;
				
			/* read in the sequence... */
				
			if(sb_ptr->len+1  >= sb_ptr->max_len){
				while(sb_ptr->len+1 >=sb_ptr->max_len){
					sb_ptr->max_len = sb_ptr->max_len + 10;
				}
					
				MREALLOC(sb_ptr->sequence, sizeof(char) * sb_file->buffer[num_read]->max_len );
				MREALLOC(sb_ptr->base_qual,sizeof(uint8_t) * sb_file->buffer[num_read]->max_len );
					
			}
				
			for (i = 0; i < sb_ptr->len; ++i){
				sb_ptr->sequence[i] ="=ACMGRSVTWYHKDBN"[bam_seqi(seq, i)];
			}
			sb_ptr->sequence[sb_ptr->len ] = 0;



			if(qual_ptr[0] == 0xFF){
				sb_ptr->base_qual[0] = '*';
				sb_ptr->base_qual[1] = 0;			 
			}else{
				for (i = 0; i < sb_ptr->len; ++i){
					sb_ptr->base_qual[i] = qual_ptr[i] + 33;
				}
				sb_ptr->base_qual[sb_ptr->len ] = 0;
			}

			
			if(! (BAM_FUNMAP & b->core.flag)){
				uint8_t * s =  bam_get_aux(b);
				int id = b->core.tid;

				if(labs(b->core.pos)- window >= 1 && labs(bam_endpos(b))+window <= sb_file->si->len[id]){
					//do stuff;
					sb_ptr->start[sb_ptr->num_hits] = labs(b->core.pos)+sb_file->cum_chr_len[id]-window;//stored 0... but sam/bam/ucsc output is 1 based...

					sb_ptr->stop[sb_ptr->num_hits] = labs(bam_endpos(b)) +sb_file->cum_chr_len[id]+window;
					
					if(bam_is_rev(b)){
					
						sb_ptr->start[sb_ptr->num_hits]  += sb_file->total_length + STRAND_BUFFER;
						sb_ptr->stop[sb_ptr->num_hits]  += sb_file->total_length + STRAND_BUFFER;
					
						RUN(rev_cmp(sb_ptr->sequence,sb_ptr->len));
					
						DPRINTF3("%s",sb_file->buffer[num_read]->sequence);
					
					}

					sb_ptr->num_hits++;	   
				}else{
					WARNING_MSG("Alignment of read %s overlaps chromosome boundaries.", sb_ptr->name);
				}

				if(b->core.qual <= sb_file->multimap_Q_threshold ){
					
					while (s+4 <= b->data + b->l_data) {
						DPRINTF3("HERE:%s",s);
						uint8_t type, key[2];
						key[0] = s[0]; key[1] = s[1];
						s += 2; type = *s++;
						
						if(key[0]  == 'X' && key[1] == 'A'){
							
							int errors;
							int pos;
							int n;
							int aln_len = 0;
							n = 0;
							while(s[n]){
								
								if(s[n] == ',' || s[n] == ';'){
									s[n] = ' ';
								}
								n++;
							}
							
							while (s < b->data + b->l_data && *s){
								n = 0;
								chr[0] = 0;
								
								sscanf((char*)s, "%"xstr(127)"s %"xstr(127)"s %"xstr(127)"s %d %n",chr,pos_str,cigar,&errors,&n);
								RUN(get_alignment_length(cigar, h, &aln_len));
								
								pos = atoi(pos_str);
								id = bam_name2id(h, chr);
								DPRINTF3("CHRID:%d length = %" PRId64 "\n",id,sb_file->cum_chr_len[id]);

								if(labs(pos)- window >= 1 && labs(pos)+aln_len <= sb_file->si->len[id]){
									//do stuff;
									sb_ptr->start[sb_ptr->num_hits] = labs(pos)+sb_file->cum_chr_len[id]-window;//stored 0... but sam/bam/ucsc output is 1 based...

									sb_ptr->stop[sb_ptr->num_hits] = labs(pos)+ aln_len  +sb_file->cum_chr_len[id]+window;
					
									if(pos < 0){
					
										sb_ptr->start[sb_ptr->num_hits]  += sb_file->total_length + STRAND_BUFFER;
										sb_ptr->stop[sb_ptr->num_hits]  += sb_file->total_length + STRAND_BUFFER;				       
					
									}

									sb_ptr->num_hits++;	   
								}else{
									WARNING_MSG("Alignment of read %s overlaps chromosome boundaries:",sb_ptr->name);
									WARNING_MSG("%s %s %s %d",chr,pos_str,cigar,errors);
								}
								
								
								s += n;
								
								if(sb_ptr->num_hits >= sb_ptr->max_num_hits){
									goto exit_alignments; 
								}
							}
							if (s >= b->data + b->l_data){
								ERROR_MSG("S is too big"); 
							}
							++s;
						}else if (type == 'A') {
							++s;
						} else if (type == 'C') {
							if( key[0] == 'X' && key[1] == '0'){
							}
							++s;
						} else if (type == 'c') {
							++s;
						} else if (type == 'S') {
							if (s+2 <= b->data + b->l_data) {
			        				s += 2;
							} else ERROR_MSG("S is too big"); ;
						} else if (type == 's') {
							if (s+2 <= b->data + b->l_data) {
								s += 2;
							} else ERROR_MSG("S is too big"); ;
						} else if (type == 'I') {
							if (s+4 <= b->data + b->l_data) {
								s += 4;
							} else ERROR_MSG("S is too big"); ;
						} else if (type == 'i') {
							if (s+4 <= b->data + b->l_data) {
								s += 4;
							} else ERROR_MSG("S is too big"); ;
						} else if (type == 'f') {
							if (s+4 <= b->data + b->l_data) {
								s += 4;
							} else ERROR_MSG("S is too big"); ;
							
						} else if (type == 'd') {
							if (s+8 <= b->data + b->l_data) {
								s += 8;
							} else ERROR_MSG("S is too big"); ;
						} else if (type == 'Z' || type == 'H') {
							while (s < b->data + b->l_data && *s){
								s++;
							}
							if (s >= b->data + b->l_data)
								ERROR_MSG("S is too big"); 
							++s;
						} else if (type == 'B') {
							uint8_t sub_type = *(s++);
							int32_t n;
							memcpy(&n, s, 4);
							s += 4; // no point to the start of the array
							if (s + n >= b->data + b->l_data)
								ERROR_MSG("S is too big"); 
							for (i = 0; i < n; ++i) { 
								if ('c' == sub_type)      { ++s; }
								else if ('C' == sub_type) { ++s; }
								else if ('s' == sub_type) { s += 2; }
								else if ('S' == sub_type) {  s += 2; }
								else if ('i' == sub_type) { s += 4; }
								else if ('I' == sub_type) {  s += 4; }
								else if ('f' == sub_type) {  s += 4; }
							}
						}
					}
				
				
					
				}
			}
			
		exit_alignments:
#if (DEBUGLEVEL >= 3)
				
			for (i = 0; i < b->core.l_qseq; ++i) fprintf(stdout,"%c","=ACMGRSVTWYHKDBN"[bam_seqi(seq, i)]);
			fprintf(stdout,"\n");
#endif
				
			DPRINTF3("%s",sb_ptr->sequence);
			DPRINTF3("%s",sb_ptr->base_qual);
				
			if(all){
				sb_file->num_read++;
			}else{
				if(sb_ptr->num_hits > 1){
					sb_file->num_read++;
				}
			}
				
				
			if(sb_file->num_read == sb_file->buffer_size){
				return OK;
			}
			
		}
	}	
	return OK;
ERROR:
	return FAIL;
}

struct sam_bam_file* open_SAMBAMfile(char* name,int buffer_size,int max_num_hits, int read_Q_threshold, int multimap_Q_threshold)
{
	struct sam_bam_file* sb_file = NULL;
	
	int max_len = 120;
	int i;
	
	MMALLOC(sb_file,sizeof(struct sam_bam_file));
	sb_file->buffer = NULL;
	sb_file->cum_chr_len = NULL;
	sb_file->header = NULL;
	sb_file->in = NULL;
	sb_file->b = NULL;
	sb_file->multimap_Q_threshold = multimap_Q_threshold;
	sb_file->read_Q_threshold = read_Q_threshold;
	
	sb_file->max_num_hits = max_num_hits;
	sb_file->si = NULL;
	sb_file->total_entries_in_file = 0;
	sb_file->read_flag = BAM_FQCFAIL | BAM_FSECONDARY;
	sb_file->buffer_size = buffer_size;
	sb_file->file_name = strdup(name);
	RUNP(sb_file->in = sam_open(name, "r"));
	

	
	RUNP(sb_file->header = sam_hdr_read(sb_file->in));
	
	if (sb_file->header->cigar_tab == NULL) {
		DPRINTF3("need to make header cigartab\n");
		MMALLOC(sb_file->header->cigar_tab , sizeof(int8_t) * 128);
		
		for (i = 0; i < 128; ++i){
			sb_file->header->cigar_tab[i] = -1;
		}
		for (i = 0; BAM_CIGAR_STR[i]; ++i){
			sb_file->header->cigar_tab[(int)BAM_CIGAR_STR[i]] = i;
		}
	}
	
	sb_file->header->ignore_sam_err = 0;
	
	MMALLOC(sb_file->cum_chr_len, sizeof(int64_t) *sb_file->header->n_targets );
	sb_file->total_length = (int64_t) sb_file->header->target_len[0];
	sb_file->cum_chr_len[0] = 0;
	for(i = 1; i <sb_file->header->n_targets;i++){
		sb_file->cum_chr_len[i]  = sb_file->cum_chr_len[i-1]  + (int64_t) sb_file->header->target_len[i-1];
		sb_file->total_length +=(int64_t) sb_file->header->target_len[i];
	}
	
	if(buffer_size){
		MCALLOC(sb_file->buffer, buffer_size,struct sam_bam_entry*);
		for(i = 0; i < buffer_size;i++ ){
		
			MCALLOC(sb_file->buffer[i],1, struct sam_bam_entry);
		
		
			MMALLOC(sb_file->buffer[i]->sequence, sizeof(char)* max_len);
			MMALLOC(sb_file->buffer[i]->base_qual,sizeof(uint8_t)* max_len);
			MMALLOC(sb_file->buffer[i]->start, sizeof(int64_t) * max_num_hits);
			MMALLOC(sb_file->buffer[i]->stop, sizeof(int64_t) * max_num_hits);
			MMALLOC(sb_file->buffer[i]->name, sizeof(char) * MAX_SEQ_NAME_LEN);
			sb_file->buffer[i]->max_len = max_len;
			sb_file->buffer[i]->max_num_hits = max_num_hits;
			sb_file->buffer[i]->num_hits = 0;
			sb_file->buffer[i]->len = 0;
			sb_file->buffer[i]->qual = 0;
		}

	}
	
	RUN(make_si_info_in_sam_bam_file(sb_file));
	
	sb_file->b = bam_init1();
	
	
	return sb_file;
ERROR:

	if(sb_file){
		if(sb_file->si){
			free_sequence_info(sb_file->si);
		}
		if(sb_file->b){
			bam_destroy1(sb_file->b);
		}
		if(sb_file->header){
			bam_hdr_destroy(sb_file->header);
		}
		if(sb_file->in){
			i = sam_close(sb_file->in);
			if(i < 0) WARNING_MSG("Error closing input: %s.\n",sb_file->file_name);
		}
		
		if(sb_file->buffer){
			for(i = 0; i < buffer_size;i++ ){
				if(sb_file->buffer[i]){
					MFREE(sb_file->buffer[i]->sequence);
					MFREE(sb_file->buffer[i]->start);
					MFREE(sb_file->buffer[i]->stop);
				}
				MFREE(sb_file->buffer[i]);
			}
			MFREE(sb_file->buffer);
		}
		
		MFREE(sb_file->cum_chr_len);
		MFREE(sb_file->file_name);
		MFREE(sb_file);
	}
	
	return NULL;
}

int make_si_info_in_sam_bam_file(struct sam_bam_file* sb_file)
{
	struct seq_info* si = NULL;
	int i;

	ASSERT(sb_file->si == NULL,"si is no NULL.");
	
	MMALLOC(si, sizeof(struct seq_info));
	
	si->cum_chr_len = NULL;
	si->len = NULL;
	si->names = NULL;
	si->sn_len = NULL;
	si->total_len = sb_file->total_length;
	si->num_seq = sb_file->header->n_targets;
	
	MMALLOC(si->cum_chr_len , sizeof(int64_t) *(si->num_seq+1)  );
	MMALLOC(si->len , sizeof(unsigned int) *si->num_seq  );
	MMALLOC(si->names , sizeof(char*) *si->num_seq  );
	MMALLOC(si->sn_len, sizeof(int) * si->num_seq );
	
	for(i = 0; i < si->num_seq ;i++){
		si->cum_chr_len[i] = sb_file->cum_chr_len[i];
		si->names[i] = NULL;
		si->sn_len[i] = (int)strlen(sb_file->header->target_name[i]) +1;
		MMALLOC(si->names[i], sizeof(char) *si->sn_len[i] );
		snprintf(si->names[i], si->sn_len[i] , "%s",sb_file->header->target_name[i] );
		si->len[i] = sb_file->header->target_len[i];
	}
	si->cum_chr_len[si->num_seq] = si->total_len;
	
	sb_file->si  = si;
	return OK;
ERROR:
	if(si){
		free_sequence_info(si);
	}
	return FAIL;
}

struct seq_info* make_si_info_from_fai(const faidx_t *fai)
{
	
	struct seq_info* si = NULL;
	
	
	khint_t k;
	int i;
	
	int64_t prev = 0;
	
	ASSERT(fai != NULL,"no faidx index passed to make_si_info_from_fai.");
	
	MMALLOC(si, sizeof(struct seq_info));
	
	si->cum_chr_len = NULL;
	si->len = NULL;
	si->names = NULL;
	si->sn_len = NULL;
	si->total_len = 0;
	si->num_seq = fai->n;
	
	MMALLOC(si->cum_chr_len , sizeof(int64_t) * (si->num_seq+1)  );
	MMALLOC(si->len , sizeof(unsigned int) *si->num_seq  );
	MMALLOC(si->names , sizeof(char*) *si->num_seq  );
	MMALLOC(si->sn_len, sizeof(int) * si->num_seq );
	
	for(i = 0; i < si->num_seq ;i++){
		faidx1_t x;
		k = kh_get(s, fai->hash, fai->name[i]);
		x = kh_value(fai->hash, k);
		
		si->names[i] = NULL;
		si->sn_len[i] = (int)strlen(fai->name[i]) +1;
		MMALLOC(si->names[i], sizeof(char) *si->sn_len[i] );
		snprintf(si->names[i], si->sn_len[i] , "%s",fai->name[i] );
		si->len[i] = x.len;
		
		si->cum_chr_len[i] = prev;
		
		si->total_len += si->len[i];
		prev += si->len[i] ;
		
	}

	si->cum_chr_len[si->num_seq] = si->total_len;
	return si;
ERROR:
	free_sequence_info(si);
	return NULL;
}


int compare_sequence_info(struct seq_info* sa,struct seq_info* sb)
{
	ASSERT(sa->num_seq == sb->num_seq,"different number of chromosomes!");
	
	int i;
	int c;
	
	for(i = 0; i < sa->num_seq;i++){
		
		c = strcmp(sa->names[i], sb->names[i]);
		if(c){
			ERROR_MSG("Name of chromosome %d is different: %s %s.",i, sa->names[i], sb->names[i]);
		}
		if(sa->len[i] !=sb->len[i]){
			ERROR_MSG("Length of chromosome %d is different: %d %d.",i, sa->len[i], sb->len[i]);
		}
		if(sa->cum_chr_len[i] !=sb->cum_chr_len[i]){
			ERROR_MSG("Cumulative length is different at chromosome %d : %lu %lu.",i, sa->cum_chr_len[i], sb->cum_chr_len[i]);
		}
		
		fprintf(stdout,"%s %d %" PRId64 "\n", sa->names[i], sa->len[i],sa->cum_chr_len[i]);
		fprintf(stdout,"%s %d %" PRId64 "\n", sb->names[i], sb->len[i],sb->cum_chr_len[i]);
		
	}
	
	return OK;
ERROR:
	return FAIL;
}

int write_seq_info(struct seq_info* si, char* filename)
{
	FILE* file = NULL;
	int i;
	if(filename){
		RUNP(file = fopen(filename, "w" ));
	}else{
		file = stdout;
	}
	
	fprintf(file,"%d\tNumber of chromosomes\n",si->num_seq);
	fprintf(file,"%" PRId64 "\tTotal length\n",si->total_len);
	
	for(i = 0;i < si->num_seq;i++){
		fprintf(file,"%s\t%d\t%" PRId64 "\n", si->names[i], si->len[i],si->cum_chr_len[i]);
	}
	if(filename){
		fclose(file);
	}
	return OK;
ERROR:
	if(filename){
		fclose(file);
	}
	return FAIL;
}

struct seq_info* read_seq_info(char* filename)
{
	struct seq_info* si = NULL;
	FILE* file = NULL;
	int numseq = 0;
	int i;
	
	ASSERT(filename != NULL,"No filename.");
	
	if(filename){
		RUNP(file = fopen(filename, "r" ));
	}
	
	if(fscanf(file,"%d\tNumber of chromosomes\n",&numseq) != 1){
		ERROR_MSG("Could not scan number of sequences...");
	}
	ASSERT(numseq != 0,"No sequences found");
	
	MMALLOC(si, sizeof(struct seq_info));
	si->cum_chr_len = NULL;
	si->len = NULL;
	si->names = NULL;
	si->sn_len = NULL;
	si->total_len = 0;
	si->num_seq = numseq;
	
	
	if(fscanf(file,"%" PRId64 "\tTotal length\n",&si->total_len) != 1){
		ERROR_MSG("Could not scan total length");
	}

	
	MMALLOC(si->cum_chr_len , sizeof(int64_t) * (si->num_seq +1) );
	MMALLOC(si->len , sizeof(unsigned int) *si->num_seq  );
	MMALLOC(si->names , sizeof(char*) *si->num_seq  );
	MMALLOC(si->sn_len, sizeof(int) * si->num_seq );
	for(i = 0;i < numseq;i++){
		si->names[i] = NULL;
		MMALLOC(si->names[i],sizeof(char) *FIELD_BUFFER_LEN );

		if(fscanf(file,"%"xstr(FIELD_BUFFER_LEN)"s\t%d\t%" PRId64 "\n",si->names[i],&si->len[i],&si->cum_chr_len[i]) != 3){
			ERROR_MSG("Could not scan info on chromosome number %d.",i);
		}
	}
	si->cum_chr_len[si->num_seq] = si->total_len;
	
	fclose(file);
	return si;
ERROR:
	if(file){
		fclose(file);
	}
	free_sequence_info(si);
	return NULL;
	
}





void free_sequence_info(struct seq_info* si)
{
	int i;
	if(si){
		for(i = 0; i < si->num_seq ;i++){
			MFREE(si->names[i]);
		}
		MFREE(si->cum_chr_len);// , sizeof(uint64_t) *si->num_seq  );
		MFREE(si->len);// , sizeof(unsigned int) *si->num_seq  );
		MFREE(si->names);// , sizeof(char*) *si->num_seq  );
		MFREE(si->sn_len);//, sizeof(int) * si->num_seq );

		MFREE(si);
	}
}


int close_SAMBAMfile(struct sam_bam_file* sb_file)
{
	int status;
	int i;
	bam_destroy1(sb_file->b);
	bam_hdr_destroy(sb_file->header);
	
	
	status = sam_close(sb_file->in);
	if (status < 0) {
		ERROR_MSG("Error closing input: %s.\n",sb_file->file_name);
	}
	if(sb_file->si){
		free_sequence_info(sb_file->si);
	}
	if(sb_file->buffer_size){
		for(i = 0; i < sb_file->buffer_size;i++ ){
			if(sb_file->buffer[i]){
				MFREE(sb_file->buffer[i]->sequence);
				MFREE(sb_file->buffer[i]->base_qual);
				MFREE(sb_file->buffer[i]->name);
				MFREE(sb_file->buffer[i]->start);
				MFREE(sb_file->buffer[i]->stop);
			}
			MFREE(sb_file->buffer[i]);
		}
		MFREE(sb_file->buffer);
	}
	MFREE(sb_file->file_name);
	MFREE(sb_file->cum_chr_len);
	MFREE(sb_file);
	
	return OK;
ERROR:
	if(sb_file){
		if(sb_file->buffer_size){
			for(i = 0; i < sb_file->buffer_size;i++ ){
				if(sb_file->buffer[i]){
					MFREE(sb_file->buffer[i]->sequence);
					MFREE(sb_file->buffer[i]->base_qual);
					MFREE(sb_file->buffer[i]->name);
					MFREE(sb_file->buffer[i]->start);
					MFREE(sb_file->buffer[i]->stop);
				}
				MFREE(sb_file->buffer[i]);
			}
			MFREE(sb_file->buffer);
		}
		MFREE(sb_file->cum_chr_len);
		MFREE(sb_file->file_name);
		MFREE(sb_file);
	}
	return FAIL;
}

int rev_cmp(char* p,int len)
{
	char c;
	int i,j;
	for (i = 0, j = len - 1; i < j; i++, j--)
	{
		c = p[i];
		p[i] = p[j];
		p[j] = c;
	}
	for (i = 0; i <  len ; i++){
		switch (p[i]) {
			case 'A':
			case 'a':
				p[i] = 'T';
				break;
			case 'C':
			case 'c':
				p[i] = 'G';
				break;
			case 'G':
			case 'g':
				p[i] = 'C';
				break;
			case 'T':
			case 't':
				p[i] = 'A';
				break;
			default:
				break;
		}
	}
	return OK;
}


int echo_header(struct sam_bam_file* sb_file)
{
	int i;
	fprintf(stdout,"%d sequences	(%" PRId64 " total length)\n",sb_file->header->n_targets,sb_file->total_length);
	for(i = 0; i < sb_file->header->n_targets;i++){
		fprintf(stdout,"%s %d %" PRId64 "\n", sb_file->header->target_name[i],(int)sb_file->header->target_len[i], sb_file->cum_chr_len[i]  );
	}
	fflush(stdout);
	return OK;
}

int get_alignment_length(char*p,bam_hdr_t *h, int* len)
{
	char  *q;
	int i;
	
	uint32_t *cigar = NULL;
	size_t n_cigar = 0;
	for (q = p; *p && *p != 0; ++p){
		if (!isdigit(*p)) ++n_cigar;
	}
	if (*p++ != 0) {
		if(n_cigar == 0){
			ERROR_MSG("no CIGAR operations");
		}
		if(n_cigar >= 65536){
			ERROR_MSG("too many CIGAR operations");
		}
		ERROR_MSG("Something went wrong");
	}
	
	MMALLOC(cigar, sizeof(uint32_t) *n_cigar);
	for (i = 0; i < n_cigar; ++i, ++q) {
		int op;
		cigar[i] = strtol(q, &q, 10)<<BAM_CIGAR_SHIFT;
		op = (uint8_t)*q >= 128? -1 : h->cigar_tab[(int)*q];
		ASSERT(op>=0,"unrecognized CIGAR operator:%d",op);
		cigar[i] |= op;
	}
	*len = bam_cigar2rlen(n_cigar, cigar);
	MFREE(cigar);
	
	return OK;
ERROR:
	
	WARNING_MSG("get_alignment_length failed: %s",p);
	MFREE(cigar);
	return FAIL;
}

int internal_to_chr_start_stop_strand(struct seq_info* si,struct genome_interval* g_int)
{
	int c;
	int found = 0;
	int64_t tmp_start, tmp_stop;
	ASSERT(g_int != NULL,"genome_interval not allocated!");
	g_int->strand = 0;
	tmp_start = g_int->g_start;
	tmp_stop = g_int->g_stop;
	if(g_int->g_start >=  si->total_len){
		g_int->strand = 1;
		tmp_start -= (si->total_len + STRAND_BUFFER);
		tmp_stop -= (si->total_len + STRAND_BUFFER);
	}
	for(c = 0;c <= si->num_seq;c++){
		
		if(tmp_start < si->cum_chr_len[c]){
			snprintf( g_int->chromosome, FIELD_BUFFER_LEN,"%s",si->names[c-1]);//   sb_file->header->target_name[c-1]);
			tmp_start -= si->cum_chr_len[c-1];  //sb_file->cum_chr_len[c-1];
			tmp_stop -= si->cum_chr_len[c-1]; //sb_file->cum_chr_len[c-1];
			found = 1;
			
			break;
		}
	}
	
	ASSERT(found == 1,"Chromosome not found.");
	
	g_int->start = (int)tmp_start;
	g_int->stop = (int)tmp_stop;
	return OK;
ERROR:
	return FAIL;
}

int chr_start_stop_strand_to_internal_tome(struct seq_info* si,struct genome_interval* g_int)
{
	int c;
	ASSERT(g_int != NULL,"genome_interval not allocated!");
	ASSERT(g_int->chromosome != NULL,"chromosome not defined.");
	
	int64_t offset = 0;
	int found = 0;
	for(c = 0;c < si->num_seq;c++){
		if(!strcmp(    g_int->chromosome  ,si->names[c])){
			offset = si->cum_chr_len[c];
			found = 1;
			break;
		}
	}
	
	ASSERT(found == 1,"Chromosome name: %s not found in index", g_int->chromosome );
	
	g_int->g_start = offset;
	if(!g_int->strand){
		g_int->g_start += g_int->start;
	}else{
		g_int->g_start += g_int->stop -1;
	}
	
	return OK;
ERROR:
	return FAIL;
}

int chr_start_stop_strand_to_internal(struct seq_info* si,struct genome_interval* g_int)
{
	int c;
	ASSERT(g_int != NULL,"genome_interval not allocated!");
	ASSERT(g_int->chromosome != NULL,"chromosome not defined.");

	int64_t offset = 0;
	int found = 0;
	g_int->g_start = 0;
	g_int->g_stop = 0;
	for(c = 0;c < si->num_seq;c++){
		if(!strcmp(    g_int->chromosome  ,si->names[c])){
			offset = si->cum_chr_len[c];
			found = 1;
			break;
		}
	}
	ASSERT(found == 1,"Chromosome name: %s not found in index", g_int->chromosome );
	
	if(g_int->strand){
		g_int->g_start = si->total_len + STRAND_BUFFER;
		g_int->g_stop = si->total_len + STRAND_BUFFER;
	}
	g_int->g_start += offset;
	g_int->g_stop += offset;
	
	g_int->g_start += g_int->start;
	g_int->g_stop += g_int->stop;
	
	return OK;
ERROR:
	return FAIL;
}

int get_chr_start_stop(struct seq_info* si,struct genome_interval* g_int, int64_t start, int64_t stop)
{
	int c;
	int found = 0;
	int64_t tmp_start, tmp_stop;
	ASSERT(g_int != NULL,"genome_interval not allocated!");
	g_int->strand = 0;
	
	g_int->g_start = start ;
	g_int->g_stop = stop;
	
	tmp_start = start;
	tmp_stop = stop;
	if(start >=  si->total_len+ STRAND_BUFFER){
		g_int->strand = 1;
		tmp_start -= (si->total_len + STRAND_BUFFER);
		tmp_stop -= (si->total_len + STRAND_BUFFER);
	}
	for(c = 0;c <= si->num_seq;c++){
		if(tmp_start < si->cum_chr_len[c]){
			snprintf( g_int->chromosome   ,FIELD_BUFFER_LEN,"%s",si->names[c-1]);//   sb_file->header->target_name[c-1]);
			tmp_start -= si->cum_chr_len[c-1];  //sb_file->cum_chr_len[c-1];
			tmp_stop -= si->cum_chr_len[c-1]; //sb_file->cum_chr_len[c-1];
			found = 1;
			break;
		}
	}
	
	g_int->start = (int)tmp_start-1;
	g_int->stop = (int)tmp_stop-2;
	ASSERT(found == 1,"Chromosome not found.");
	
	return OK;
ERROR:
	return FAIL;
}

struct genome_interval* init_genome_interval(void*(*init_data)(void) ,int (*data_resize) (void * data, va_list args),void (*data_free)(void* ptr))
{
	struct genome_interval* g_int = NULL;
	MMALLOC(g_int, sizeof(struct genome_interval));
	g_int->chromosome = NULL;
	
	MMALLOC(g_int->chromosome,sizeof(char) * FIELD_BUFFER_LEN);
	g_int->chromosome[0] = 0;
	g_int->g_start = 0;
	g_int->g_stop = 0;
	g_int->start = 0;
	g_int->stop = 0;
	g_int->strand = 0;
	g_int->data = NULL;
	g_int->data_free = data_free;
	g_int->data_resize =data_resize;
	
	if(init_data){
		RUNP(g_int->data  = init_data());
	}
	return g_int;
ERROR:
	free_genome_interval(g_int);
	return NULL;
}
int resize_genome_interval(struct genome_interval* g_int,...)
{
	va_list args;
	va_start(args, g_int);
	RUN(g_int->data_resize(g_int->data,args ));
	va_end(args);
	return OK;
ERROR:
	return FAIL;
}

void* init_gi_bed_data(void)
{
	struct gi_bed_data* tmp = NULL;
	MMALLOC(tmp, sizeof(struct gi_bed_data));
	tmp->annotation = NULL;
	MMALLOC(tmp->annotation, sizeof(char) *FIELD_BUFFER_LEN);
	tmp->value = 0.0;
	return tmp;
ERROR:
	free_gi_bed_data(tmp);
	return NULL;
}

int resize_gi_bed_data(void* ptr,va_list args)
{
	struct gi_bed_data* tmp = (struct gi_bed_data*)  ptr;
	
	int new_size = va_arg (args, int);
	MREALLOC(tmp->annotation, sizeof(char) * new_size);
	return OK;
ERROR:
	return FAIL;
}

void free_gi_bed_data(void* ptr)
{
	struct gi_bed_data* tmp = (struct gi_bed_data*)  ptr;

	if(tmp){
		if(tmp->annotation){
			MFREE(tmp->annotation);
		}
		MFREE(tmp);
	}
}



void free_genome_interval(struct genome_interval*  g_int)
{
	if(g_int){
		if(g_int->data_free){
			g_int->data_free(g_int->data);
		}
		if(g_int->chromosome){
			MFREE(g_int->chromosome);
		}
		MFREE(g_int);
	}
}




struct rbtree_root*  init_rb_tree_for_bed_file( struct rbtree_root* root)
{
	
	void*  (*fp_get)(void* ptr) = NULL;
	long int (*fp_cmp)(void* keyA, void* keyB)= NULL;
	int (*fp_cmp_same)(void* ptr_a,void* ptr_b);
	void (*fp_print)(void* ptr,FILE* out_ptr) = NULL;
	void (*fp_free)(void* ptr) = NULL;
	
	
	
	fp_get = &get_genome_interval_id;
	fp_cmp_same = &resolve_same_genome_interval_ID;
	
	
	fp_cmp = &compare_genome_interval_ID;
	fp_print = &print_genome_interval_tree_entry;
	fp_free = &free_genome_interval_tree_entry;
	
	root = init_tree(fp_get,fp_cmp,fp_cmp_same,fp_print,fp_free);
	return root;
	
}

void* get_genome_interval_id(void* ptr)
{
	struct genome_interval* tmp = (struct genome_interval*)  ptr;
	return tmp;
}

int resolve_same_genome_interval_ID(void* ptr_a,void* ptr_b)
{
	return 1;
}

long int compare_genome_interval_ID(void* keyA, void* keyB)
{
	struct genome_interval* num1 = (struct genome_interval*)keyA;
	struct genome_interval* num2 = (struct genome_interval*)keyB;
	
	int c;
	
	
	c = num1->strand - num2->strand;
	
	if(c < 0){
		return 1;
	}
	if(c > 0){
		return -1;
	}
	
	c = strcmp(num1->chromosome, num2->chromosome);
	
	if(c > 0){
		return -1;		
	}
	if(c < 0){
	//	DPRINTF3("1");
		return 1;
	}
	if(c == 0){
		if(num1->start > num2->start){
	//		DPRINTF3("-1");
			return -1;
		}
		if(num1->start < num2->start){
	//		DPRINTF3("1");
			return 1;
		}
		
		if(num1->start == num2->start){
	//		DPRINTF3("0");
			return 0;
		}
	}
	return 0;
}


void print_genome_interval_tree_entry(void* ptr,FILE* out_ptr)
{
	//int i;
	struct genome_interval* tmp = (struct genome_interval*)  ptr;
	struct gi_bed_data* bed_data = NULL;
	bed_data = (struct gi_bed_data*) tmp->data;
	

	
	fprintf(out_ptr,"%s\t%d\t%d\t%s\t%0.2f\t%c\n", tmp->chromosome,tmp->start,tmp->stop,bed_data->annotation,bed_data->value,"+-"[tmp->strand]);
}

void free_genome_interval_tree_entry(void* ptr)
{
	struct genome_interval* tmp = (struct genome_interval*)  ptr;
	
	free_genome_interval(tmp);
}





#ifdef ITEST


int get_seq_test(void);

int get_seq_test(void)
{
	struct genome_interval* g_int = NULL;
	struct seq_info* si = NULL;
	faidx_t*  index = NULL;

	char fasta_name[] = "libhts_test.fa";
	char chr_name[] = "dummy_chr";
	char* seq = NULL;
	FILE* f_ptr = NULL;
	int i = 0;
	
	RUNP(f_ptr = fopen(fasta_name, "w"));
	fprintf(f_ptr,">%s\n", chr_name);
	for (i = 0; i < 10; i++) {
		fprintf(f_ptr,"A");
	
	}
	for (i = 0; i < 10; i++) {
		fprintf(f_ptr,"C");
	}
	for (i = 0; i < 10; i++) {
		fprintf(f_ptr,"G");
	}
	for (i = 0; i < 10; i++) {
		fprintf(f_ptr,"T");
	}
	fprintf(f_ptr,"\n");
		
	fclose(f_ptr);

	/* build index  */

	RUNP(index = get_faidx(fasta_name));
	RUNP(si =make_si_info_from_fai(index));

	
	RUNP(g_int =init_genome_interval(NULL,NULL,NULL));
	snprintf(g_int->chromosome ,FIELD_BUFFER_LEN ,"%s",chr_name);
	
	g_int->strand = 0;
	/* test 1: simple retrieve  */
	
	LOG_MSG("Retrieve seq 0-10.");
	g_int->start = 0;
	g_int->stop = 10;
	DPRINTF3("%s:%d-%d\n",g_int->chromosome,g_int->start,g_int->stop);
	RUNP(seq = get_sequence(index,g_int));
	LOG_MSG("%s:%d-%d;%c;\n%s\n",g_int->chromosome,g_int->start,g_int->stop,"+-"[g_int->strand],seq);
	MFREE(seq);
	

	LOG_MSG("Retrieve seq 5-15.");
	g_int->start = 5;
	g_int->stop = 15;
	RUNP(seq = get_sequence(index,g_int));
	LOG_MSG("%s:%d-%d;%c;\n%s\n",g_int->chromosome,g_int->start,g_int->stop,"+-"[g_int->strand],seq);
	MFREE(seq);

	LOG_MSG("Retrieve seq 5-15 (using internal coordinates first; then internal to chr... ");
	g_int->g_start = 5;
	g_int->g_stop = 15;
	RUN(internal_to_chr_start_stop_strand(si,g_int));
	RUNP(seq = get_sequence(index,g_int));
	LOG_MSG("%s:%d-%d;%c;\n%s\n",g_int->chromosome,g_int->start,g_int->stop,"+-"[g_int->strand],seq);
	MFREE(seq);

	LOG_MSG("Retrieve seq 5-15, negative strand.");
	g_int->start = 5;
	g_int->stop = 15;
	g_int->strand = 1;
	RUNP(seq = get_sequence(index,g_int));
	LOG_MSG("%s:%d-%d;%c;\n%s\n",g_int->chromosome,g_int->start,g_int->stop,"+-"[g_int->strand],seq);
	MFREE(seq);

	LOG_MSG("Retrieve seq 5-15.(to internal, back then retrieve");
	g_int->start = 5;
	g_int->stop = 15;
	g_int->strand = 0;
	RUN(chr_start_stop_strand_to_internal(si,g_int));
	RUN(internal_to_chr_start_stop_strand(si,g_int));
	RUNP(seq = get_sequence(index,g_int));
	LOG_MSG("%s:%d-%d;%c;\n%s\n",g_int->chromosome,g_int->start,g_int->stop,"+-"[g_int->strand],seq);
	MFREE(seq);

	LOG_MSG("Retrieve seq 5-15. negative strand(to internal, back then retrieve");
	g_int->start = 5;
	g_int->stop = 15;
	g_int->strand = 1;
	RUN(chr_start_stop_strand_to_internal(si,g_int));
	RUN(internal_to_chr_start_stop_strand(si,g_int));
	RUNP(seq = get_sequence(index,g_int));
	LOG_MSG("%s:%d-%d;%c;\n%s\n",g_int->chromosome,g_int->start,g_int->stop,"+-"[g_int->strand],seq);
	MFREE(seq);


	free_faidx(index);
	free_genome_interval(g_int);
	free_sequence_info(si);

	return OK;
ERROR:
	if(si){
		free_sequence_info(si);
	}
	if(index){
		free_faidx(index);
	}
	if(g_int){
		free_genome_interval(g_int);
	}
	return FAIL;
}



int main (int argc,char * argv[])
{
	
	struct sam_bam_file* sb_file = NULL;
	int i,j;
	struct genome_interval* g_int = NULL;
	
	struct seq_info* si = NULL;
	struct seq_info* si2 = NULL;

	
	faidx_t*  index = NULL;
	
	char* genomic_sequence = NULL;

	RUN(get_seq_test());
	


	
	if(!argv[1]){
		fprintf(stdout,"run:  ./test <sam/bam/cram file> <corresponding genome file>\n");
		return EXIT_SUCCESS;
	}

	if(argv[1] && !argv[2]){
		RUNP(sb_file= open_SAMBAMfile(argv[1],100,10,-1,-1));

		while(1){
			RUN(read_SAMBAM_chunk(sb_file,1,0));
			DPRINTF1("read %d entries\n",sb_file->num_read);
			
			if(!sb_file->num_read){
				break;
			}

			for(i =0; i < sb_file->num_read;i++){
				struct sam_bam_entry* sb_ptr = sb_file->buffer[i];
				fprintf(stdout,"%s %s (%d)  hits: %d\n", sb_ptr->sequence ,sb_ptr->base_qual,   sb_ptr->len , sb_ptr->num_hits);   
			}
		}
		RUN(close_SAMBAMfile(sb_file));
		sb_file = NULL;

		RUNP(sb_file= open_SAMBAMfile(argv[1],100,10,-1,-1));
		sb_file->read_flag = 0;//BAM_FUNMAP | BAM_FQCFAIL | BAM_FSECONDARY;
		while(1){
			RUN(read_SAMBAM_chunk(sb_file,1,0));
			DPRINTF1("read %d entries\n",sb_file->num_read);
			if(!sb_file->num_read){
				break;
			}
			for(i =0; i < sb_file->num_read;i++){
				struct sam_bam_entry* sb_ptr = sb_file->buffer[i];
				fprintf(stdout,"%s %s (%d)  hits: %d\n", sb_ptr->sequence ,sb_ptr->base_qual,   sb_ptr->len , sb_ptr->num_hits);     
				for(j = 0; j < sb_ptr->num_hits;j++){
					fprintf(stdout,"\t hit:%d\t%" PRId64 "\t%" PRId64 "\n",j,sb_ptr->start[j],sb_ptr->stop[j]);
				}
			}
			
		}
		RUN(close_SAMBAMfile(sb_file));
		sb_file = NULL;
		
	}

	
	if(argv[2]){
		
		RUNP(index = get_faidx(argv[2]));
		DPRINTF3("fai: %p\n",index);
	}
	RUNP(g_int =init_genome_interval(NULL,NULL,NULL));
	
	if(argv[1] && argv[2]){
		//RUNP(sb_file = open_SAMBAMfile(param->bedfile,buffer_size,10,0,0));
	
		RUNP(sb_file= open_SAMBAMfile(argv[1],100,10,0,0));
		//echo_header(sb_file);
		
		//test seq retrieval;
		
		j = 0;
		
		snprintf(g_int->chromosome ,FIELD_BUFFER_LEN ,"%s",sb_file->header->target_name[j]  );
		g_int->start = 0;
		g_int->stop = 6;
		g_int->strand = 0;
		
		genomic_sequence = get_sequence(index, g_int);
		if(genomic_sequence){
			
			fprintf(stdout,"%s:%d-%d\n", sb_file->header->target_name[j], 0, 6);
			
			fprintf(stdout,"%s\n",genomic_sequence);
			
			MFREE(genomic_sequence);
		}
		
		snprintf(g_int->chromosome ,FIELD_BUFFER_LEN ,"%s",sb_file->header->target_name[j] );
		g_int->start = sb_file->header->target_len[j]+1-5;
		g_int->stop =  sb_file->header->target_len[j]+5;
		g_int->strand = 0;
		
		genomic_sequence = get_sequence(index, g_int);
		if(genomic_sequence){
			fprintf(stdout,"%s:%d-%d\n", sb_file->header->target_name[j], sb_file->header->target_len[j]+1-5, sb_file->header->target_len[j]+5);

			fprintf(stdout,"%s\n",genomic_sequence);
			
			MFREE(genomic_sequence);
		}
		
		while(1){
			RUN(read_SAMBAM_chunk(sb_file,1,0));
			DPRINTF3("read %d entries\n",sb_file->num_read);
			for(i =0; i < sb_file->num_read;i++){
				struct sam_bam_entry* sb_ptr = sb_file->buffer[i];
				fprintf(stdout,"%s %s (%d)  hits:\n", sb_ptr->sequence ,sb_ptr->base_qual,   sb_ptr->len);
				for(j = 0; j < sb_ptr->num_hits;j++){
					
					//get_chr_start_stop(sb_file,i,j, g_int);
					
					g_int->g_start = sb_file->buffer[i]->start[j];
					g_int->g_stop =  sb_file->buffer[i]->stop[j];
					RUN(internal_to_chr_start_stop_strand(sb_file->si ,g_int));
					//get_chr_start_stop(sb_file->si ,g_int,sb_file->buffer[i]->start[j], sb_file->buffer[i]->stop[j]);
					
					RUNP(genomic_sequence = get_sequence(index, g_int));
					fprintf(stdout,"%s\n",genomic_sequence);
					if(!g_int->strand){
						fprintf(stdout,"   + %s:%d-%d\n",g_int->chromosome,g_int->start,g_int->stop);
					}else{
						fprintf(stdout,"   - %s:%d-%d\n",g_int->chromosome,g_int->start,g_int->stop);
					}
					
					MFREE(genomic_sequence);
					
				}
				fprintf(stdout,"\n");
			}
			
			if(!sb_file->num_read){
				break;
			}
		}
		
		
		RUNP(si = make_si_info_from_fai(index));
	        
		//fprintf(stdout,"Number of chromosomes: %d %d\n",sb_file->si->num_seq, si->num_seq );
		RUN(compare_sequence_info(sb_file->si,si));
		
		RUN(write_seq_info(sb_file->si, "testA.si"));
		RUN(write_seq_info(si, "testB.si"));
		
		free_sequence_info(si);
		
		RUN(close_SAMBAMfile(sb_file));
		sb_file = NULL;
		
	}
        
	if(argv[2]){
		DPRINTF3("fai: %p\n",index);
		free_faidx(index);
		index = NULL;
	}
	
	
	free_genome_interval(g_int);
	g_int = NULL;
	RUNP(g_int =init_genome_interval(init_gi_bed_data,resize_gi_bed_data, free_gi_bed_data));
	
	RUN(resize_genome_interval(g_int, 1000));
	
	free_genome_interval(g_int);
	g_int = NULL;
		
	si = NULL;
	si2 = NULL;
	
	RUNP(si = read_seq_info("testB.si"));
	RUNP(si2 = read_seq_info("testA.si"));
	
	RUN(compare_sequence_info(si,si2));
	
	RUNP(g_int =init_genome_interval(NULL,NULL, NULL));
	g_int->g_start = 2000000000;
	g_int->g_stop = 2000000100;
	fprintf(stdout,"%" PRId64 " %" PRId64 "\n",g_int->g_start,g_int->g_stop);
	RUN(internal_to_chr_start_stop_strand(si,g_int));
	
	g_int->g_start = 0;
	g_int->g_stop = 0;
	RUN(chr_start_stop_strand_to_internal(si,g_int));
	fprintf(stdout,"%" PRId64 " %" PRId64 "\n",g_int->g_start,g_int->g_stop);
	free_sequence_info(si);
	free_sequence_info(si2);
	free_genome_interval(g_int);
	return EXIT_SUCCESS;
ERROR:
	if(sb_file){
		close_SAMBAMfile(sb_file);//,"close SAM/BAM failed.");
	}
	free_faidx(index);
	free_genome_interval(g_int);
	return EXIT_FAILURE;
}

#endif

