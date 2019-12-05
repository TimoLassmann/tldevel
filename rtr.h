
#ifndef rtr_header

#define rtr_header

#define MODE_RTREE_MERGE_NOPE  0
#define MODE_RTREE_MERGE_IDENTICAL 1
#define MODE_RTREE_MERGE_OVERLAPPING 2

#include "stdint.h"

/* Needs to be declares here so I can access elments vis flatten_tree...  */
struct rtree_interval
{
        int32_t* coordinates;//[NUMSIDES]; /* xmin,ymin,...,xmax,ymax,... */
        int32_t count;
        void* data;
};


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
        struct rtree_branch** new_branch_store;
        void** data_store;

        int num_new_node_malloc;
        int new_node_index;
        int num_new_branch_malloc;
        int new_branch_index;
        int num_data_malloc;
        int data_index;

        struct rtree_interval* tmp_interval;
        struct rtree_interval* tmp_cover;
        int num_flat_interval_malloc;

        // stuff for splitting...
        struct rtree_branch** branch_buffer;

        double* area;
        uint8_t* group;
        uint8_t* taken;

        float r; // reciprogal overlap....

        int64_t shift;
        int count[2];
        int split_area[2];
        struct rtree_interval** split_interval;

        int(* compare_function)(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff);
        double compare_cutoff;
        int stats_duplicated;
        int stats_overlapping;
        int stats_num_interval;
        int mode;
        int dim;
        int num_branches;
        int level;
        int (*insert) (struct rtr_data* rtrd , int64_t* val,void* data,int32_t identifier,int32_t count, uint8_t keep_rep);
        //int (*query)(struct rtr_data* rtrd , int64_t* val,int32_t* identifier,int32_t* count);
        void* (*query)(struct rtr_data* rtrd , int64_t* val);
        int (*resolve_dup)(void* org, void* new);
        void (*free) (struct rtr_data* rtrd);
        int (*flatten_rtree)(struct rtr_data* rtrd);
        int (*re_label_tree_nodes)(struct rtr_data* rtr_data, struct rtree_node* n,int* identifier);
        int (*print_rtree)(struct rtr_data* rtr_data, struct rtree_node* rtr_node);

};

struct rtree_search_results{
        struct rtree_interval* query;
        struct rtree_interval** nodes;
        int num_results;
        int num_results_malloced;
};

extern struct rtr_data* init_rtr_data(int dim, int num_branches,int expected_num_items);
extern int do_get_overlapping(struct rtr_data* rtrd,struct rtree_node* node, struct rtree_search_results* sr);
extern int node_reciprogal_overlap(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b,int* result);
extern int node_contained(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff);
extern int node_similar_euclidian_distance(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff);

extern int reset_rtr_data(struct rtr_data* rtrd);

extern int combine_trees(struct rtr_data** org, struct rtr_data* result,int num_tree);
extern int merge_tree_intervals(struct rtr_data** org, struct rtr_data* result,int num_tree);
extern int condense_tree_intervals(struct rtr_data** org, struct rtr_data* result,int num_tree);



extern struct rtree_search_results* init_search_results(void);
extern int reset_search_results(struct rtree_search_results* sr);
extern void free_search_results(struct rtree_search_results* sr);


extern struct rtree_interval* init_rtree_interval(struct rtr_data* rtr_data);
extern int copy_interval(struct rtr_data* rtr_data, struct rtree_interval* target,struct rtree_interval* source);

extern void sort_coordinates(int32_t* c , int dim);

#endif
