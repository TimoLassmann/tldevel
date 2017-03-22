
#ifndef hts_glue_header

#define hts_glue_header


#define FIELD_BUFFER_LEN 128
#define MAX_SEQ_NAME_LEN 128

#define str(x)          # x
#define xstr(x)         str(x)

#if (DEBUGLEVEL >= 1)

#define SAMBAMBUFFERSIZE 1

#else

#endif

#include "rbtree.h"

#include "htslib/cram.h"

#include "htslib/sam.h"
#include "htslib/faidx.h"

// This is a buffer between the plus strand:
// 0 -> total_len
// buffer
// and the negative strand
// total_len+ buffer -> totallen*2+buffer..

#define STRAND_BUFFER 65536LL // this is a buffer between the plus strand


struct seq_info{
	char** names;
	unsigned int* len;
	int64_t* cum_chr_len;
	int* sn_len;
	int num_seq;
	int64_t total_len;
};

struct genome_interval{
	int64_t g_start;
	int64_t g_stop;
	void* data;
	char* chromosome;
	int start; // 0 based ,
	int stop; // include this base  i.e. get sequence for i = start ; i <= end....
	int strand;
	void (*data_free)(void* ptr);
	int (*data_resize) (void * data, va_list args);
};

struct gi_bed_data{
	char* annotation;
	float value;
};


struct sam_bam_entry{
	int64_t* start;
	int64_t* stop;
	char* sequence;
	char* name;
	int qual;
	int len;
	int max_len;
	int num_hits;
	int max_num_hits;
};

struct sam_bam_file{
	samFile *in;
	bam_hdr_t *header;
	bam1_t *b;
	struct seq_info* si;
	char* file_name;
	struct sam_bam_entry** buffer;
	int buffer_size;
	int num_read;
	int total_entries_in_file;
	
	int read_Q_threshold;
	int multimap_Q_threshold;
	int max_num_hits;
	
	int64_t* cum_chr_len;
	int64_t total_length;
	int read_mode;
};


extern int read_SAMBAM_chunk(struct sam_bam_file* sb_file,int all,int window);
//extern struct sam_bam_file* open_SAMBAMfile(char* name,int build_buffer);
extern struct sam_bam_file* open_SAMBAMfile(char* name,int buffer_size, int read_Q_threshold, int multimap_Q_threshold);
extern int close_SAMBAMfile(struct sam_bam_file* sb_file);

extern int echo_header(struct sam_bam_file* sb_file);
extern int compare_multiple_SAM_BAM_headers(char** filenames, int num_files);

//extern int get_chr_start_stop(struct sam_bam_file* sb_file,int read, int hit,char* chr,int* start,int*stop,int* strand);
//extern int get_chr_start_stop(struct sam_bam_file* sb_file,int read,int hit, struct genome_interval* g_int);
extern int get_chr_start_stop(struct seq_info* si,struct genome_interval* g_int, int64_t start, int64_t stop);


extern int chr_start_stop_strand_to_internal_tome(struct seq_info* si,struct genome_interval* g_int);
extern int chr_start_stop_strand_to_internal(struct seq_info* si,struct genome_interval* g_int);
extern int internal_to_chr_start_stop_strand(struct seq_info* si,struct genome_interval* g_int);

/*faidx */
extern faidx_t* get_faidx(const char* fasta_name);
extern char* get_sequence(faidx_t* index, struct genome_interval* g_int);
extern int free_faidx(faidx_t*  index);


extern struct genome_interval* init_genome_interval(void*(*init_data)(void) ,int (*data_resize) (void * data, va_list args),void (*data_free)(void* ptr));
extern int resize_genome_interval(struct genome_interval* g_int,...);

extern void* init_gi_bed_data(void);
extern int resize_gi_bed_data(void* ptr,va_list args);
extern void free_gi_bed_data(void* ptr);

extern void free_genome_interval(struct genome_interval*  g_int);

struct seq_info* make_si_info_from_fai(const faidx_t *fai);
int write_seq_info(struct seq_info* si, char* filename);
struct seq_info* read_seq_info(char* filename);
extern void free_sequence_info(struct seq_info* si);

extern int compare_sequence_info(struct seq_info* sa,struct seq_info* sb);

// bed tree
extern struct rbtree_root*  init_rb_tree_for_bed_file( struct rbtree_root* root);
extern void* get_genome_interval_id(void* ptr);
extern int resolve_same_genome_interval_ID(void* ptr_a,void* ptr_b);
extern long int compare_genome_interval_ID(void* keyA, void* keyB);
extern void print_genome_interval_tree_entry(void* ptr,FILE* out_ptr);
extern void free_genome_interval_tree_entry(void* ptr);


#endif
