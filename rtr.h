
#ifndef rtr_header

#define rtr_header

#define MODE_RTREE_MERGE_NOPE  0
#define MODE_RTREE_MERGE_IDENTICAL 1
#define MODE_RTREE_MERGE_OVERLAPPING 2


struct rtree_interval;
struct rtree_branch;
struct rtree_node;


// This will be used to propagate information up and down the tree...
struct rtr_data{
	
	struct seq_info* si;
	struct rtree_interval** flat_interval;
	struct rtree_node* root;
	struct rtree_node* new_node;
	struct rtree_branch* new_branch;
	
	
	// optimization... - no mallocs...
	
	
	
	struct rtree_node** new_node_store;
	int num_new_node_malloc;
	int new_node_index; 
	
	struct rtree_interval* tmp_interval;
	struct rtree_interval* tmp_cover;
	int num_flat_interval_malloc;
	
	// stuff for splitting...
	struct rtree_branch** branch_buffer;
	
	double* area; 
	uint8_t* group;
	uint8_t* taken;
	
	float r; // reciprogal overlap....
	
	int count[2];
	int split_area[2];
	struct rtree_interval** split_interval;
	
	int (*merge_action)(void* dataA, void* dataB);
	
	int(* compare_function)(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff);
	double compare_cutoff;
	int stats_duplicated;
	int stats_overlapping;
	int stats_num_interval;
	int mode;
	int dim;
	int num_branches;
	int level;
	int (*insert) (struct rtr_data* rtrd , int64_t* val,int32_t identifier);

	
};

struct rtree_search_results{
	struct rtree_interval* query;
	struct rtree_interval** nodes;
	int num_results;
	int num_results_malloced;
};

extern struct rtr_data* init_rtr_data(int (*merge_action_funtion_pointer)(void* dataA, void* dataB),int(* compare_function_pointer)(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff),int dim, int num_branches,int expected_num_items);
extern void free_rtr_data(struct rtr_data* rtrd);


extern int combine_interval(struct rtr_data* rtr_data, struct rtree_interval* target,struct rtree_interval* sourceA,struct rtree_interval* sourceB);
extern int sort_interval_based_on_count_and_coordinates(const void * a, const void * b);


extern int do_get_overlapping(struct rtr_data* rtrd,struct rtree_node* node, struct rtree_search_results* sr);
extern int node_reciprogal_overlap(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b,int* result);
extern int node_identical(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff);
extern int node_contained(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff);
extern int node_similar_euclidian_distance(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff);

extern int extend_num_nodes_in_rtr_data(struct rtr_data* rtrd, int num_add_elements);
extern int get_interval_id(struct rtr_data* rtrd , struct rtree_interval* query,int* identifier);
extern int reset_rtr_data(struct rtr_data* rtrd);

extern int combine_trees(struct rtr_data** org, struct rtr_data* result,int num_tree);
extern int merge_tree_intervals(struct rtr_data** org, struct rtr_data* result,int num_tree);
extern int condense_tree_intervals(struct rtr_data** org, struct rtr_data* result,int num_tree);
extern int insert_data_rtree(struct rtr_data* rtrd , struct rtree_interval* new,int identifier);

extern int flatten_rtree(struct rtr_data* rtrd);

extern int rescale_interval(struct rtr_data* rtrd,struct rtree_interval *a, int32_t dim);
extern int make_rtree_coordinates(struct rtree_interval *a, int64_t* in_coordinates,int32_t dim, int64_t shift);

extern int re_label_tree_nodes(struct rtr_data* rtr_data, struct rtree_node* n,int* identifier);
extern struct rtree_search_results* init_search_results(void);
extern int reset_search_results(struct rtree_search_results* sr);
extern void free_search_results(struct rtree_search_results* sr);

extern int count_leaf_entries(struct rtr_data* rtr_data, struct rtree_node* n,int* count);

extern struct rtree_interval* init_rtree_interval(struct rtr_data* rtr_data);
extern int copy_interval(struct rtr_data* rtr_data, struct rtree_interval* target,struct rtree_interval* source);
extern void free_rtree_interval(struct rtree_interval* rtri);
extern void sort_coordinates(int32_t* c , int dim);

extern int print_rtree_node(struct rtr_data* rtr_data, struct rtree_node* rtr_node);
extern int print_rtree_interval(struct rtr_data* rtr_data,struct rtree_interval* rtri);

extern int check_and_update_counts(struct rtr_data* rtrd , struct rtree_interval* query,int* identifier);


#endif
