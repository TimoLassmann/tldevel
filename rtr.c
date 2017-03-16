#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "tldevel.h"


#include "rtr.h"

struct rtree_interval
{
	int32_t* coordinates;//[NUMSIDES]; /* xmin,ymin,...,xmax,ymax,... */
	uint32_t count;
//	void* data;
};

struct rtree_branch
{
	struct rtree_interval* interval;
	struct rtree_node *child;
	int32_t identifier;
};

struct rtree_node{
	struct rtree_branch** branch;//[MAXCARD];
	int count;
	int level; /* 0 is leaf, others positive */
	
};



//search STUFF
int search_data_rtree(struct rtr_data* rtrd , struct rtree_interval* new,int* found);
int do_search_data(struct rtr_data* rtrd, struct rtree_node* node);
int do_check_and_update_counts(struct rtr_data* rtrd, struct rtree_node* node, int* identifier);

int get_overlapping_intervals(struct rtr_data** rtrd, struct rtree_search_results* sr, int num_tree);

//int do_get_overlapping(struct rtr_data* rtrd,struct rtree_node* node, struct rtree_search_results* sr);
int do_get_interval_id(struct rtr_data* rtrd, struct rtree_node* node,struct rtree_interval* query, int* identifier);
//INSERT STUFF

int do_insert_data(struct rtr_data* rtrd, struct rtree_node* node);
int do_add_branch(struct rtr_data* rtrd, struct rtree_node* node);

int pick_branch( struct rtr_data* rtrd, struct rtree_node *n);
int classify_nodes(struct rtr_data* rtrd, int target, int group);
int node_cover(struct rtr_data* rtrd, struct rtree_node *n,struct rtree_interval* r );

int node_overlap(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b);


int copy_branch(struct rtr_data* rtr_data,struct rtree_branch* target, struct rtree_branch* source);

int merge_interval(struct rtr_data* rtrd, struct rtree_branch* a, struct rtree_branch* b);

int perform_a_split(struct rtr_data* rtrd);
double intervalSphericalVolume(struct rtree_interval *r , int dim);
double sphere_volume(double dimension);

int collect_entries(struct rtr_data* rtr_data, struct rtree_node* n,int* count);

int sort_interval_based_on_count(const void * a, const void * b);

void q_sort_coordinates(int32_t* c, int32_t left, int32_t right, int dim);

// print stuff
struct rtree_interval*  random_interval(int dim);

int print_tikz_image(struct rtr_data* rtr_data, struct rtree_node* rtr_node, int maxlevel);

// INIT structures...

// RTREE DATA

// RTREE NODE
struct rtree_node* init_rtr_node(struct rtr_data* rtrd);
int reset_rtr_node(struct rtr_data* rtrd,	struct rtree_node* rtrn);
void free_rtr_node(struct rtr_data* rtrd,	struct rtree_node* rtrn);

//RTREE BRANCH
struct rtree_branch* init_rtr_branch(struct rtr_data* rtr_data);
int reset_rtr_branch(struct rtr_data* rtr_data,struct rtree_branch* rtrb);
void free_rtr_branch(struct rtree_branch* rtrb);

//RTREE INTERVAL
int reset_rtree_interval(struct rtr_data* rtr_data,struct rtree_interval* rtri);

int insert(struct rtr_data* rtrd , int64_t* val,int32_t identifier);

int insert(struct rtr_data* rtrd , int64_t* val,int32_t identifier)
{
	struct rtree_interval* tmp = NULL;
	int i;

	fprintf(stdout,"Got here\n");
	int dim = rtrd->dim;

	RUNP(tmp = init_rtree_interval(rtrd));
	
	
	/* change dimensions if necessary..  */
	fprintf(stdout,"Got here\n");
	/* copy values and insert...  */
	for(i = 0; i < rtrd->dim;i++){
		fprintf(stdout,"%d %d\m",tmp->coordinates[i],val[i]);
		tmp->coordinates[i] = val[i];
		tmp->coordinates[dim+i] = val[dim+i];
	}
	RUN(insert_data_rtree(rtrd , tmp,identifier ));

	free_rtree_interval(tmp);
	
	return OK;
ERROR:
	return FAIL;
}





#ifdef ITEST
int main (int argc,char * argv[])
{
	struct rtr_data** tree_list = NULL;
	struct rtr_data* rtrd = NULL;
	struct rtr_data* overlaptree = NULL;
	
	int num_intervals_for_testing = 100000;
	struct rtree_interval* tmp = NULL;
	int64_t test_val[4]; 
	int i,j;
	int num_tree = 1;
	
	MMALLOC(tree_list, sizeof(struct rtr_data*) * num_tree);
	DECLARE_TIMER(t);
	START_TIMER(t);
	for(i = 0; i < num_tree;i++){
		LOG_MSG("Doing tree %d.",i);
		RUNP(tree_list[i] = init_rtr_data(NULL,&node_identical,2, 5,num_intervals_for_testing));
		tree_list[i]->mode = MODE_RTREE_MERGE_IDENTICAL;
		rtrd =tree_list[i];
		j = 0;
		tmp = init_rtree_interval(rtrd);
		
		tmp->coordinates[0] = 86164185;
		tmp->coordinates[1] = 202465983;
		tmp->coordinates[2] = 86164188;
		tmp->coordinates[3] = 202465986;
		
		RUN(insert_data_rtree(rtrd , tmp,j+1));
		free_rtree_interval(tmp);


		test_val[0] = 86164185;
		test_val[1] = 202465983;
	        test_val[2] = 86164188;
		test_val[3] = 202465986;
		fprintf(stdout,"Got here\n");
		rtrd->insert(rtrd,test_val,77);
		  fprintf(stdout,"Got here\n");
		
		tmp = init_rtree_interval(rtrd);
		
		tmp->coordinates[0] = 86704840;
		tmp->coordinates[1] = 202779757;
		tmp->coordinates[2] = 86704842;
		tmp->coordinates[3] = 202779759;
		
		RUN(insert_data_rtree(rtrd , tmp,j+1));
		free_rtree_interval(tmp);
		
		tmp = init_rtree_interval(rtrd);
		
		tmp->coordinates[0] = 86856210  ;
		tmp->coordinates[1] = 202908320   ;
		tmp->coordinates[2] = 86856213  ;
		tmp->coordinates[3] = 202908322;
		
		RUN(insert_data_rtree(rtrd , tmp,j+1));
		free_rtree_interval(tmp);
		
		tmp = init_rtree_interval(rtrd);
		
		tmp->coordinates[0] = 86649879   ;
		tmp->coordinates[1] = 202954384     ;
		tmp->coordinates[2] = 86649882     ;
		tmp->coordinates[3] = 202954386 ;
		
		RUN(insert_data_rtree(rtrd , tmp,j+1));
		free_rtree_interval(tmp);
		
		tmp = init_rtree_interval(rtrd);
		
		tmp->coordinates[0] = 87169264     ;
		tmp->coordinates[1] = 202957295       ;
		tmp->coordinates[2] = 87169266      ;
		tmp->coordinates[3] = 202957297   ;
		
		RUN(insert_data_rtree(rtrd , tmp,j+1));
		free_rtree_interval(tmp);


		
		tmp = init_rtree_interval(rtrd);
		
		tmp->coordinates[0] = 86419825            ;
		tmp->coordinates[1] = 203349347       ;
		tmp->coordinates[2] = 86419827      ;
		tmp->coordinates[3] = 203349350   ;
		
		RUN(insert_data_rtree(rtrd , tmp,j+1));
		free_rtree_interval(tmp);

		
		
		
		
		LOG_MSG("Done %d tree. (used:%f)",i,   (double)rtrd->new_node_index /(double) rtrd->num_new_node_malloc );
	}
	STOP_TIMER(t);
	LOG_MSG("Processing %d intervals took: %f seconds.",num_intervals_for_testing,GET_TIMING(t));
	
	print_rtree_node(tree_list[0], tree_list[0]->root);

	exit(0);
	//MMALLOC(tree_list, sizeof(struct rtr_data*) * num_tree);
	//DECLARE_TIMER(t);
	START_TIMER(t);
	for(i = 0; i < num_tree;i++){
		LOG_MSG("Doing tree %d.",i);
		RUNP(tree_list[i] = init_rtr_data(NULL,&node_identical,1, 10,num_intervals_for_testing));
		tree_list[i]->mode = MODE_RTREE_MERGE_IDENTICAL;
		rtrd =tree_list[i];
		for(j	= 0 ;j < num_intervals_for_testing;j++){
			RUNP(tmp = random_interval(rtrd->dim));
			//RUN(rescale_interval(rtrd, tmp, 4),"rescale_interval failed");
			RUN(insert_data_rtree(rtrd , tmp,j+1));
			free_rtree_interval(tmp);
		}
		LOG_MSG("Done %d tree. (used:%f)",i,   (double)rtrd->new_node_index /(double) rtrd->num_new_node_malloc );
	}
	STOP_TIMER(t);
	LOG_MSG("Processing %d intervals took: %f seconds.",num_intervals_for_testing,GET_TIMING(t));
	//log_message("Intervals: %d.",rtrd->stats_num_interval);
	//log_message("Overlapping: %d.",rtrd->stats_overlapping);
	//log_message("Identical: %d.",rtrd->stats_duplicated);
	//rtrd->stats_num_interval = 0;
	
	RUNP(overlaptree = init_rtr_data(NULL,&node_identical,1, 10,num_intervals_for_testing*num_tree ));
	
	
	START_TIMER(t);
	overlaptree->r = 0.8;
	RUN(combine_trees(tree_list, overlaptree, num_tree));
	//RUN(condense_tree_intervals(tree_list, overlaptree, 8),"condense_intervals failed.");
	STOP_TIMER(t);
	LOG_MSG("Combine tree in: %f seconds.",GET_TIMING(t));
	int identifier = 0;
	for(i = 0; i < num_tree;i++){
		for(j	= 0;j < 10;j++){
			identifier = -1;
			RUN(get_interval_id(overlaptree, tree_list[i]->flat_interval[j], &identifier ) );
			print_rtree_interval(overlaptree, tree_list[i]->flat_interval[j]);
			fprintf(stdout,"ID:%d\n", identifier);
			print_rtree_interval(tree_list[i], tree_list[i]->flat_interval[j]);
			fprintf(stdout,"ID:%d\n", identifier);
		}
	}
	
	
	//print_rtree_node(overlaptree, overlaptree->root);
	
	
	START_TIMER(t);
	for(i = 0; i < num_tree;i++){
		LOG_MSG("Doing tree %d.",i);
		RUN(reset_rtr_data(tree_list[i]));
		
		tree_list[i]->mode = MODE_RTREE_MERGE_IDENTICAL;
		rtrd =tree_list[i];
		for(j	= 0 ;j < num_intervals_for_testing;j++){
			RUNP(tmp = random_interval(rtrd->dim));
			
			//print_rtree_interval(rtrd, tmp);
			//RUN(rescale_interval(rtrd, tmp, 4),"rescale_interval failed");
			RUN(insert_data_rtree(rtrd , tmp,j+1));
			free_rtree_interval(tmp);
		}
		LOG_MSG("Done %d tree. (used:%f)",i,   (double)rtrd->new_node_index /(double) rtrd->num_new_node_malloc );
	}
	
	
	STOP_TIMER(t);
	LOG_MSG("Processing %d intervals took: %f seconds.",num_intervals_for_testing,GET_TIMING(t));
	RUN(reset_rtr_data(overlaptree));
	START_TIMER(t);
	overlaptree->r = 0.8;
	RUN(condense_tree_intervals(tree_list, overlaptree, num_tree));
	//RUN(condense_tree_intervals(tree_list, overlaptree, 8),"condense_intervals failed.");
	STOP_TIMER(t);
	LOG_MSG("Condensed tree in: %f seconds.",GET_TIMING(t));
	
	START_TIMER(t);
	for(i = 0; i < num_tree;i++){
		LOG_MSG("Doing tree %d.",i);
		RUN(reset_rtr_data(tree_list[i]));
		
		tree_list[i]->mode = MODE_RTREE_MERGE_IDENTICAL;
		rtrd =tree_list[i];
		for(j	= 0 ;j < num_intervals_for_testing;j++){
			RUNP(tmp = random_interval(rtrd->dim));
			
			//print_rtree_interval(rtrd, tmp);
			//RUN(rescale_interval(rtrd, tmp, 4),"rescale_interval failed");
			RUN(insert_data_rtree(rtrd , tmp,j+1));
			free_rtree_interval(tmp);
		}
		LOG_MSG("Done %d tree. (used:%f)",i,   (double)rtrd->new_node_index /(double) rtrd->num_new_node_malloc );
	}
	
	
	STOP_TIMER(t);
	LOG_MSG("Processing %d intervals took: %f seconds.",num_intervals_for_testing,GET_TIMING(t));
	RUN(reset_rtr_data(overlaptree));
	START_TIMER(t);
	overlaptree->r = 1;
	
	RUN(merge_tree_intervals(tree_list, overlaptree, num_tree));
	//RUN(condense_tree_intervals(tree_list, overlaptree, 8),"condense_intervals failed.");
	STOP_TIMER(t);
	LOG_MSG("Merge tree in: %f seconds.",GET_TIMING(t));

	
	
	
	// Blow away tree(s);
	for(i = 0; i < num_tree;i++){
		free_rtr_data(tree_list[i]);
	}
	MFREE(tree_list);
	free_rtr_data(overlaptree);
	
	
	return EXIT_SUCCESS;
ERROR:
	free_rtr_data(rtrd);
	
	return EXIT_FAILURE;
}

/*
 combine_trees:
 1) throw all entried from N trees into result,.. 
 */

int combine_trees(struct rtr_data** org, struct rtr_data* result,int num_tree)
{
	int i,treeID;
	
	int new_node_id = 1;
	
	struct rtree_interval* tmp_interval = NULL;
	ASSERT(org != NULL,"No interval data structure(s).");
	ASSERT(result != NULL,"No datastructure for results! ");
	
	for(i = 0; i < num_tree;i++){
		ASSERT(org[i] != NULL,"Tree %d is empty",i);
		ASSERT(org[i]->dim == result->dim,"org and result tree have different dimensions");
		RUN(flatten_rtree(org[i]));
		DPRINTF1("TREE:%d count of largest node:%d",i, org[i]->flat_interval[0]->count );
	}
	for(treeID = 0; treeID  < num_tree;treeID++){
		for(i = 0; i < org[treeID]->stats_num_interval;i++){
			//RUN(insert_data_rtree(result ,  org[treeID]->flat_interval[i],j+1),"insert_data_rtree failed.");
			
			RUNP(tmp_interval = init_rtree_interval(result));
			RUN(copy_interval(result, tmp_interval, org[treeID]->flat_interval[i]));
			//tmp_interval->count = 1; // reset count...
			
			RUN(insert_data_rtree(result, tmp_interval, new_node_id));
			new_node_id++;

			free_rtree_interval(tmp_interval);
		}
	}
	return OK;
ERROR:
	return FAIL;
}

#endif

/*
 merge_intervals:
 1) grabs intervals from tree 1 (interval with largest count first... )
 2) searches all other trees for overlapping intervals
 3) compares the resulting intervals to make sure they actually overlap (not necessary unless we use a reciprogal overlapp.
 4) Coordinates of all intervals that pass 3 are merged. (and their count in the original trees set to zero (i.e. they will not be reported in subsequent searches .
 5) goto 2 until no hits are found...
  */
int merge_tree_intervals(struct rtr_data** org, struct rtr_data* result,int num_tree)
{
	int i,j,treeID;
	
	int new_node_id = 1;
	
	struct rtree_interval* tmp_interval = NULL;
	struct rtree_search_results* sr = NULL;
	int overlap_result;
	int positive_overlaps;
	ASSERT(org != NULL,"No interval data structure(s).");
	ASSERT(result != NULL,"No datastructure for results! ");
	ASSERT(result->r >=1,"Merge intervals is set to reciprogal overlap! ");
	
	for(i = 0; i < num_tree;i++){
		ASSERT(org[i] != NULL,"Tree %d is empty",i);
		ASSERT(org[i]->dim == result->dim,"org and result tree have different dimensions");
		RUN(flatten_rtree(org[i]));
		DPRINTF1("TREE:%d count of largest node:%d",i, org[i]->flat_interval[0]->count );
	}
	
	RUNP(sr = init_search_results());
	for(treeID = 0; treeID  < num_tree;treeID++){
		for(i = 0; i < org[treeID]->stats_num_interval;i++){
			if(org[treeID]->flat_interval[i]->count){
				sr->num_results = 1;
				sr->query = org[treeID]->flat_interval[i];
				
				org[treeID]->flat_interval[i]->count = 0;
				while(sr->num_results){
					reset_search_results(sr);
					RUN(get_overlapping_intervals(org, sr,num_tree));
					// reciproigal
					//fprintf(stdout,"Searching with:\n");
					//print_rtree_interval(rtrd,sr->query);
					
					positive_overlaps = 0;
					for(j = 0;j < sr->num_results;j++){
						//fprintf(stdout,"\t");
						//print_rtree_interval(rtrd,sr->nodes[j]);
						
						RUN(node_reciprogal_overlap(result, sr->query,sr->nodes[j], &overlap_result));
						if(overlap_result){
							//	fprintf(stdout,"HAS AN OVERLAP!!!!\n");
							RUN(combine_interval(result, sr->query,sr->query,sr->nodes[j]));
							sr->nodes[j]->count = 0;
							positive_overlaps++;
							//sr->query->count += sr->nodes[j]->count;
						}
					}
					sr->num_results = positive_overlaps;
					//break;
				}
				
				RUNP(tmp_interval = init_rtree_interval(result));
				RUN(copy_interval(result, tmp_interval, sr->query));
				tmp_interval->count = 1; // reset count...
				
				RUN(insert_data_rtree(result, tmp_interval, new_node_id));
				new_node_id++;
				
				free_rtree_interval(tmp_interval);
				
				//fprintf(stdout,"DONE with iterative merging...\n");
				//print_rtree_interval(result,sr->query);
				// here I should insert into result tree....
			}
		}
	}
	free_search_results(sr);
	
	
	
	return OK;
ERROR:
	if(sr){
		free_search_results(sr);
	}
	return FAIL;
}

/*
condense_intervals:
 1) grabs intervals from tree 1 (interval with largest count first... )
 2) searches all other trees for overlapping intervals
 3) compares the resulting intervals to make sure they actually overlap (not necessary unless we use a reciprogal overlap.
 4) Merge intervals and finish.
 */
int condense_tree_intervals(struct rtr_data** org, struct rtr_data* result,int num_tree)
{
	int i,j,treeID;
	
	int new_node_id = 1;
	
	struct rtree_interval* tmp_interval = NULL;
	struct rtree_search_results* sr = NULL;
	int overlap_result;

	ASSERT(org!=NULL,"No interval data structure(s).");
	ASSERT(result!=NULL,"No datastructure for results! ");
	ASSERT(result->r < 1,"Merge intervals is set to insect! ");
	
	for(i = 0; i < num_tree;i++){
		ASSERT(org[i] != NULL,"Tree %d is empty",i);
		ASSERT(org[i]->dim == result->dim,"org and result tree have different dimensions");
		RUN(flatten_rtree(org[i]));
		DPRINTF1("TREE:%d count of largest node:%d\n",i, org[i]->flat_interval[0]->count );
	}
	
	
	RUNP(sr = init_search_results());
	for(treeID = 0; treeID  < num_tree;treeID++){
		for(i = 0; i < org[treeID]->stats_num_interval;i++){
			if(org[treeID]->flat_interval[i]->count){
				
				sr->query = org[treeID]->flat_interval[i];
				
				org[treeID]->flat_interval[i]->count = 0;
				
				
				reset_search_results(sr);
				RUN(get_overlapping_intervals(org, sr,num_tree));
				for(j = 0;j < sr->num_results;j++){
					RUN(node_reciprogal_overlap(result, sr->query,sr->nodes[j], &overlap_result));
					if(overlap_result){
						RUN(combine_interval(result, sr->query,sr->query,sr->nodes[j]));
						sr->nodes[j]->count = 0;
						//sr->query->count += sr->nodes[j]->count;
					}
				}
				
				RUNP(tmp_interval = init_rtree_interval(result));
				RUN(copy_interval(result, tmp_interval, sr->query));
				tmp_interval->count = 1; // reset count...
				//fprintf(stdout,"INSERTING:\t");
				//print_rtree_interval(result, tmp_interval);

				RUN(insert_data_rtree(result, tmp_interval, new_node_id));
				new_node_id++;
				
				free_rtree_interval(tmp_interval);
				
				//fprintf(stdout,"DONE with iterative merging...\n");
				//print_rtree_interval(result,sr->query);
				// here I should insert into result tree....
			}
		}
	}
	free_search_results(sr);
	return OK;
ERROR:
	if(sr){
		free_search_results(sr);
	}
	return FAIL;
}



struct rtree_interval*  random_interval(int dim)
{
	struct rtree_interval *r = NULL;
	int i;

	MMALLOC(r, sizeof(struct rtree_interval ));
	r->coordinates = NULL;
	//r->data = NULL;
	r->count = 1;
	register int64_t width;
	int64_t tmp_coordinated[20];
	
	
	MMALLOC(r->coordinates, sizeof(int32_t ) * dim*2);
	for (i = 0; i < dim; i++)	{
		width = drand48() * (100) + 1;
		tmp_coordinated[i] =drand48() * (1000000000-width); /* low side */
		tmp_coordinated[i+dim] =tmp_coordinated[i] + width;
		//r->coordinates[i] = drand48() * (1000000000-width); /* low side */
		//r->coordinates[i + dim] = r->coordinates[i] + width; /* high side */
		
	}
	RUN(make_rtree_coordinates(r, tmp_coordinated, dim,4));
	
	sort_coordinates(r->coordinates, dim);
	
	return r;
ERROR:
	if(r){
		if(r->coordinates){
			MFREE(r->coordinates);
		}
		MFREE(r);
	}
	return NULL;
}


//#endif

int get_overlapping_intervals(struct rtr_data** rtrd, struct rtree_search_results* sr, int num_tree)
{
	ASSERT(sr->num_results == 0,"Search struct is not empty.");
	int i;
	
	for(i = 0; i < num_tree;i++){
		RUN(do_get_overlapping(rtrd[i], rtrd[i]->root, sr));
	}
	return OK;
ERROR:
	return FAIL;
}


int do_get_overlapping(struct rtr_data* rtrd,struct rtree_node* node, struct rtree_search_results* sr)
{
	int i;
	if(node->level > 0){
		for(i = 0 ;i < node->count;i++){
		
			if(node->branch[i]->child &&  node_overlap(rtrd, sr->query ,node->branch[i]->interval )){
				RUN(do_get_overlapping(rtrd, node->branch[i]->child,sr));
			}
		}
	}else{
		for(i = 0 ;i <  node->count;i++){
			if(node->branch[i]->interval->count){
				if(node->branch[i]->identifier &&  node_overlap(rtrd, sr->query,node->branch[i]->interval )){
					
					if(sr->num_results == sr->num_results_malloced){
						sr->num_results_malloced = sr->num_results_malloced << 1;
						MREALLOC(sr->nodes, sizeof(struct rtree_interval*) * sr->num_results_malloced);
						
					}
					sr->nodes[sr->num_results] =node->branch[i]->interval;
					sr->num_results++;
				}
			}
		}
	}
	return OK;
ERROR:
	return FAIL;
}

int check_and_update_counts(struct rtr_data* rtrd , struct rtree_interval* query,int* identifier)
{
	struct rtree_interval* interval_ptr = NULL;
	*identifier = 0;
	rtrd->new_branch->child = NULL;
	
	interval_ptr =rtrd->new_branch->interval;
	
	RUN(copy_interval(rtrd,interval_ptr, query));
	
	RUN(do_check_and_update_counts(rtrd, rtrd->root, identifier));
	
	return OK;
ERROR:
	return FAIL;

}

int do_check_and_update_counts(struct rtr_data* rtrd, struct rtree_node* node, int* identifier)
{
	int i;
	if(node->level > 0){
		//fprintf(stdout,"Comparing: at level%d\n", node->level);
		for(i = 0 ;i < node->count;i++){
			//fprintf(stdout,"Comparing: at level%d\n", node->level);
			//print_rtree_interval(rtrd, rtrd->new_branch->interval);
			//print_rtree_interval(rtrd,node->branch[i]->interval);
			if(node->branch[i]->child &&  node_overlap(rtrd, rtrd->new_branch->interval,node->branch[i]->interval )){
				
				
				
				RUN(do_check_and_update_counts(rtrd, node->branch[i]->child,identifier));
			}
		}
	}else{
		for(i = 0 ;i <  node->count;i++){
			
			//	c = root->compare(root->key(n->data_node),root->key(datanode));
			//
			/*if( node_identical(rtrd, rtrd->new_branch->interval,node->branch[i]->interval ,rtrd->compare_cutoff) != rtrd->compare_function(rtrd, rtrd->new_branch->interval,node->branch[i]->interval ,rtrd->compare_cutoff)){
				DPRINTF1("Comparing: %d and %d\n",node_identical(rtrd, rtrd->new_branch->interval,node->branch[i]->interval ,rtrd->compare_cutoff),rtrd->compare_function(rtrd, rtrd->new_branch->interval,node->branch[i]->interval ,rtrd->compare_cutoff));
				ERROR_MSG("Pointer func and normal have different results.");
			}*/
			if(node->branch[i]->identifier &&   rtrd->compare_function(rtrd, rtrd->new_branch->interval,node->branch[i]->interval ,rtrd->compare_cutoff)){

			//if(node->branch[i]->identifier &&  node_identical(rtrd, rtrd->new_branch->interval,node->branch[i]->interval ,rtrd->compare_cutoff)){
				*identifier = node->branch[i]->identifier;
			//	fprintf(stdout,"FOUND NODE!\n");
				node->branch[i]->interval->count += rtrd->new_branch->interval->count;
				return OK;
			}
		}
	}
	return OK;
ERROR:
	return FAIL;
}



int get_interval_id(struct rtr_data* rtrd , struct rtree_interval* query,int* identifier)
{
	//struct rtree_interval* interval_ptr = NULL;
	/**identifier = 0;
	rtrd->new_branch->child = NULL;
	
	interval_ptr =rtrd->new_branch->interval;
	
	RUN(copy_interval(rtrd,interval_ptr, query), "copy_interval failed.");
	*/
	RUN(do_get_interval_id(rtrd, rtrd->root, query,identifier));
	
	return OK;
ERROR:
	return FAIL;
}

int do_get_interval_id(struct rtr_data* rtrd, struct rtree_node* node,struct rtree_interval* query, int* identifier)
{
	int i;
	if(node->level > 0){
		for(i = 0 ;i < node->count;i++){
			if(node->branch[i]->child &&  node_overlap(rtrd, query,node->branch[i]->interval )){
				RUN(do_get_interval_id(rtrd, node->branch[i]->child,query,identifier));
			}
		}
	}else{
		for(i = 0 ;i <  node->count;i++){
			if(node->branch[i]->identifier &&   rtrd->compare_function(rtrd, query,node->branch[i]->interval , rtrd->compare_cutoff)){
			//if(node->branch[i]->identifier &&  node_identical(rtrd, query,node->branch[i]->interval , rtrd->compare_cutoff)){
				*identifier = node->branch[i]->identifier;
//				/node->branch[i]->interval->count++; // increment counter...
				return OK;
			}
		}
	}
	return OK;
ERROR:
	return FAIL;
}

int search_data_rtree(struct rtr_data* rtrd , struct rtree_interval* new, int* found)
{
	struct rtree_interval* interval_ptr = NULL;
	

	
	*found = 0;
	
	rtrd->level = 0;

	
	rtrd->new_branch->child = NULL;

	interval_ptr =rtrd->new_branch->interval;
	
	RUN(copy_interval(rtrd,interval_ptr, new));
	
	
	
	*found = do_search_data(rtrd,rtrd->root);
	
	return OK;
ERROR:
	return FAIL;
}

int do_search_data(struct rtr_data* rtrd, struct rtree_node* node)

{
	
	int hitcount = 0;
	int i;
	if(node->level > 0){
		for(i = 0 ;i < node->count;i++){
			if(node->branch[i]->child &&  node_overlap(rtrd, rtrd->new_branch->interval,node->branch[i]->interval )){
				hitcount += do_search_data(rtrd, node->branch[i]->child);
			}
		}
	}else{
		for(i = 0 ;i <  node->count;i++){
			
			
			if(node->branch[i]->identifier &&   rtrd->compare_function(rtrd, rtrd->new_branch->interval,node->branch[i]->interval , rtrd->compare_cutoff)){
			//if(node->branch[i]->identifier &&  node_identical(rtrd, rtrd->new_branch->interval,node->branch[i]->interval , rtrd->compare_cutoff)){
				hitcount++;
				//node_cover(<#struct rtr_data *rtrd#>, <#struct rtree_node *n#>, <#struct rtree_interval *r#>)
			}
		}
	}
	return hitcount;
}


int flatten_rtree(struct rtr_data* rtrd)
{
	int num_entries = 0 ;
	int i = 0;
	RUN(count_leaf_entries(rtrd,rtrd->root,&num_entries));

	rtrd->stats_num_interval = num_entries;
	
	if(num_entries > rtrd->num_flat_interval_malloc){
		MREALLOC(rtrd->flat_interval, sizeof(struct rtree_interval*) *num_entries);
		rtrd->num_flat_interval_malloc =num_entries;
	}
	for(i = 0 ;i < rtrd->num_flat_interval_malloc ;i++){
		rtrd->flat_interval[i] = NULL;
	}
	num_entries = 0 ;
	RUN(collect_entries(rtrd,rtrd->root,&num_entries));
	
	qsort(rtrd->flat_interval,rtrd->stats_num_interval,sizeof(struct rtree_interval*),sort_interval_based_on_count);
	
	return OK;
ERROR:
	if(rtrd->flat_interval){
		MFREE(rtrd->flat_interval);
	}
	return FAIL;
}

int collect_entries(struct rtr_data* rtr_data, struct rtree_node* n,int* count)
{
	
	int i;
	if(!n->level){
		for(i = 0; i < n->count;i++){
			rtr_data->flat_interval[*count] = n->branch[i]->interval;
			*count = *count  + 1;
		}
	}else{
		for(i = 0; i < n->count;i++){
			if(n->branch[i]->child){
				RUN(collect_entries(rtr_data,n->branch[i]->child,count ));
			}
		}
	}
	return OK;
ERROR:
	return FAIL;
}


int insert_data_rtree(struct rtr_data* rtrd , struct rtree_interval* new,int identifier)
{
	struct rtree_interval* interval_ptr = NULL;
	
	struct rtree_node* new_root = NULL;

	ASSERT(identifier>0, "Identifier of new interval has to be bigger than one,");
	
	rtrd->level = 0;	
	rtrd->new_branch->child = NULL;
	rtrd->new_branch->identifier = identifier;
	interval_ptr =rtrd->new_branch->interval;
	
	RUN(copy_interval(rtrd,interval_ptr, new));
	interval_ptr->count = new->count;
	
	RUN(do_insert_data(rtrd,rtrd->root));
	if(rtrd->new_node){
		//fprintf(stdout,"NEW ROOT??? for ID:%d\n",identifier  );
		
		new_root = rtrd->new_node_store[rtrd->new_node_index];
		//rtrd->new_node_store[rtrd->new_node_index] = NULL;
		rtrd->new_node_index++;
		if(rtrd->new_node_index ==rtrd->num_new_node_malloc){
			RUN(extend_num_nodes_in_rtr_data(rtrd, 10000));
		}
		ASSERT(rtrd->new_node_index != rtrd->num_new_node_malloc,"Running out of nodes... ");
		
		//RUNP(new_root = init_rtr_node(rtrd),"init_rtr_node failed.");
		new_root->level =rtrd->root->level + 1;
		
		//RUNP(new_branch = init_rtr_branch(rtrd),"init_rtr_branch failed.");
		RUN(node_cover(rtrd, rtrd->root,rtrd->new_branch->interval));
		rtrd->new_branch->child =  rtrd->root;
		RUN(do_add_branch(rtrd, new_root));
		
		
		RUN(node_cover(rtrd, rtrd->new_node,rtrd->new_branch->interval));
		rtrd->new_branch->child =  rtrd->new_node;
		RUN(do_add_branch(rtrd, new_root));

		rtrd->root =new_root;
		rtrd->new_node = NULL;
	}
	
	
	
	
	return OK;
ERROR:
	return FAIL;
}

int do_insert_data(struct rtr_data* rtrd, struct rtree_node* node)
{
	//int i,j;
	//struct rtree_interval* interval_ptr = NULL;
	int i;
	//int j;
	
	
	if (node->level >  rtrd->level){
		i = pick_branch(rtrd,  node);
		
		/*fprintf(stdout,"picked branch %d:  (LEVEL:%d)\n",i, node->level);
		print_rtree_interval(rtrd,node->branch[i]->interval);
		fprintf(stdout,"containing: \n");
		
		for(j = 0 ; j < node->branch[i]->child->count;j++){
			print_rtree_interval(rtrd,  node->branch[i]->child->branch[j]->interval);
		}
		fprintf(stdout,"\n\n");*/
		
		RUN(do_insert_data(rtrd, node->branch[i]->child));
		
		
		
		if(rtrd->new_node){// a split has happened...
			
			
			
			/*fprintf(stdout,"A new node was created at level: %d\n",node->level);
			
			fprintf(stdout,"leftover:\n");
			for(j = 0 ; j < node->branch[i]->child->count;j++){
				print_rtree_interval(rtrd,  node->branch[i]->child->branch[j]->interval);
			}
			fprintf(stdout,"\n");
			
			
			
			fprintf(stdout,"new:\n");
			for(j = 0 ; j <  rtrd->new_node->count;j++){
				print_rtree_interval(rtrd,  rtrd->new_node->branch[j]->interval);
			}
			fprintf(stdout,"\n");*/
			//fprintf(stdout,"\n");
			
			
			//RUN(node_cover(rtrd, node->branch[i]->child, rtrd->new_branch->interval),"node_cover failed. " );
			//update rectangle of old node..
			RUN(node_cover(rtrd, node->branch[i]->child, node->branch[i]->interval));
			
			rtrd->new_branch->child =  rtrd->new_node;
			
			
			//print_rtree_interval(rtrd, node->branch[i]->interval);
			
			
			RUN(node_cover(rtrd, rtrd->new_branch->child,rtrd->new_branch->interval));
			
			//print_rtree_interval(rtrd, rtrd->new_branch->interval);
			
			rtrd->new_node = NULL;
			
			//fprintf(stdout,"before inserting: ");
			
			RUN(do_add_branch(rtrd,node));
			
			return OK;
		}else{
			//fprintf(stdout,"A new node was not created at level %d\n",node->level);
			//
			//fprintf(stdout,"old coordinates::\n");
			///
			//print_rtree_interval(rtrd, node->branch[i]->interval);
			//
			//Combine?? ot cover?
			
			RUN(node_cover(rtrd,  node->branch[i]->child ,node->branch[i]->interval));
			
			//RUN(combine_interval(rtrd, node->branch[i]->interval, node->branch[i]->interval, rtrd->new_branch->interval),"combine_interval failed.");
			//fprintf(stdout,"this interval should be added:\n");
			//print_rtree_interval(rtrd,rtrd->new_branch->interval);
			//fprintf(stdout,"new coordinates::\n");

			//print_rtree_interval(rtrd, node->branch[i]->interval);
		}
		
	}else{
		RUN(do_add_branch(rtrd,node));
	}
	return OK;
ERROR:
	return FAIL;
}

int do_add_branch(struct rtr_data* rtrd, struct rtree_node* node)
{
	int i;
	int level;
	

	//fprintf(stdout,"Adding branch to:\n");
	
	/*for(i = 0; i < rtrd->num_branches;i++){
		if(node->branch[i]->identifier != 0 || node->branch[i]->child != NULL){
		print_rtree_interval(rtrd,  node->branch[i]->interval);
		}
	}
	fprintf(stdout,"\n\n");*/
	
	
	/*if(node->level == 0){
		for(i = 0; i < rtrd->num_branches;i++){
		switch (rtrd->mode ) {
			case MODE_RTREE_MERGE_OVERLAPPING:
				if(node_overlap(rtrd, node->branch[i]->interval , rtrd->new_branch->interval)){
					RUN(merge_interval(rtrd, node->branch[i], rtrd->new_branch),"merge_interval failed.");
					rtrd->stats_overlapping++;
					return OK;
				}
				break;
			case MODE_RTREE_MERGE_IDENTICAL:
				if(node_identical(rtrd, node->branch[i]->interval , rtrd->new_branch->interval)){
					RUN(merge_interval(rtrd, node->branch[i], rtrd->new_branch),"merge_interval failed.");
					
					
					rtrd->stats_duplicated++;
					return OK;
				}
				
				break;
			default:
				break;
		}
		}
	}*/
	
	if(node->count < rtrd->num_branches){ // no split necessary!!
		for(i = 0; i < rtrd->num_branches;i++){
			// got to the end - will just insert
			if(node->branch[i]->identifier == 0 && node->branch[i]->child == NULL){
				node->branch[i]->child = rtrd->new_branch->child;
				
				if(node->level == 0){
					rtrd->stats_num_interval++;
					node->branch[i]->interval->count = rtrd->new_branch->interval->count;
					node->branch[i]->identifier = rtrd->new_branch->identifier;
				//	fprintf(stdout,"\t at level:%d	add id:%d \n",node->level,rtrd->new_branch->identifier );
				}else{
				//	fprintf(stdout,"\t at level:%d\n",node->level);
					node->branch[i]->identifier = 0;
				}
				
				RUN(copy_interval(rtrd,node->branch[i]->interval , rtrd->new_branch->interval));
				//print_rtree_interval(rtrd, node->branch[i]->interval);
				node->count++;
				break;
			}
		}
		
		/*fprintf(stdout,"Now looks like this:\n");
		
		for(i = 0; i < rtrd->num_branches;i++){
			if(node->branch[i]->identifier != 0 || node->branch[i]->child != NULL){
				print_rtree_interval(rtrd,  node->branch[i]->interval);
			}
		}
		
		
		fprintf(stdout,"\n\n");*/
		
		//RUN(node_cover(rtrd, node, node-> struct rtree_interval* r )
		
		
		//node->branch[node->count]->interval->coordinates[i];
		//fprintf(stdout,"SPACE? : %d\n", space);
		return OK;
		
	}else{ // spit !./
		
		//because intervals can be merged check is split is actually necessary or if one of the intervals stored in the node overlap / identical to new node...
		/*if(rtrd->mode && node->level == 0){
			if(rtrd->mode == MODE_RTREE_MERGE_OVERLAPPING){
				for(i = 0; i < node->count;i++){
					if(node_overlap(rtrd, node->branch[i]->interval , rtrd->new_branch->interval)){
						RUN(merge_interval(rtrd, node->branch[i], rtrd->new_branch),"merge_interval failed.");
						rtrd->stats_overlapping++;
						return OK;
					}
				}
			}
			
			if(rtrd->mode == MODE_RTREE_MERGE_IDENTICAL){
				for(i = 0; i < node->count;i++){
					if(node_identical(rtrd, node->branch[i]->interval , rtrd->new_branch->interval)){
						RUN(merge_interval(rtrd, node->branch[i], rtrd->new_branch),"merge_interval failed.");
						rtrd->stats_duplicated++;
						return OK;
					}
				}
			}
		}*/
		// split node...
		// Step 1 copy all branches into buffer in main datastructure
		for(i = 0; i < node->count;i++){
		//	fprintf(stdout,"Splitting at level: %d\n", node->level);
		//	ASSERT(node->branch[i]->child ==NULL,"Child is not null");
			
			RUN(copy_branch(rtrd,rtrd->branch_buffer[i], node->branch[i]));
			
			
		}
		
		
		if(node->level == 0){
			rtrd->stats_num_interval -= node->count;
		}
		// copy new branch into buffer.
		rtrd->branch_buffer[node->count]->identifier =  rtrd->new_branch->identifier;
		rtrd->branch_buffer[node->count]->child =  rtrd->new_branch->child;
		
		RUN(copy_interval(rtrd,rtrd->branch_buffer[node->count]->interval , rtrd->new_branch->interval));

				// Step 2 select seeds for groups. ....
		// do splitting...
		RUN(perform_a_split(rtrd));
		
		// Step 3: delete contents of node...
		level = node->level;
		RUN(reset_rtr_node(rtrd, node));
		
		// Step 4 : create new node...
		
		rtrd->new_node  = rtrd->new_node_store[rtrd->new_node_index];
		//rtrd->new_node_store[rtrd->new_node_index] = NULL;
		rtrd->new_node_index++;
		if(rtrd->new_node_index ==rtrd->num_new_node_malloc){
			RUN(extend_num_nodes_in_rtr_data(rtrd, 10000));
		}
		ASSERT(rtrd->new_node_index != rtrd->num_new_node_malloc,"Running out of nodes... ");
		//RUNP(rtrd->new_node = init_rtr_node(rtrd)," init_rtr_node failed.");
		rtrd->new_node->level = level;
		node->level = level;
		// Step 5: assign nodes on buffer to eirther newnode (new) or node (old)//
		for(i =0 ; i < rtrd->num_branches+1; i++){// Don't forget the buffer contains nodes + the new one...
			rtrd->new_branch->identifier =rtrd->branch_buffer[i]->identifier;
			rtrd->new_branch->child =rtrd->branch_buffer[i]->child;
			RUN(copy_interval(rtrd,rtrd->new_branch->interval ,rtrd->branch_buffer[i]->interval));
			if(rtrd->group[i] == 0){
				RUN(do_add_branch(rtrd, node));
			}else{
				RUN(do_add_branch(rtrd, rtrd->new_node ));
			}
		}
		//fprintf(stdout,"ORIGINGAL NODE:\n ");
		//print_rtree_node(rtrd, node);
		//fprintf(stdout,"NEW NODE:\n ");
		//print_rtree_node(rtrd,  rtrd->new_node );
		/*for (i=0; i < p->total; i++){
			assert(p->partition[i] == 0 || p->partition[i] == 1);
			if (p->partition[i] == 0){
				RTreeAddBranch(BranchBuf[i], n, NULL,split,dim);
			}else if (p->partition[i] == 1){
				RTreeAddBranch(BranchBuf[i], q, NULL,split,dim);
			}
		}
		*/
		
		//free_rtr_node(rtrd, rtrd->new_node ); // JUST FOR TESTING !~!!! !@ !
		
		//RUN(copy_branch(rtrd, node->branch[i], rtrd->branch_buffer[i]),"copy_branch failed.");
		
		//rtrd->branch_buffer
		
		
		return OK;
	}
	return OK;
ERROR:
	return FAIL;
}


int merge_interval(struct rtr_data* rtrd, struct rtree_branch* a, struct rtree_branch* b)
{
	a->identifier = MACRO_MIN(a->identifier ,b->identifier);
	
	b->identifier = a->identifier ;
	
	RUN(combine_interval(rtrd, a->interval, a->interval, b->interval));
	// fix interval in new node (will be up-propagated - needed to fix bounding boxes in the tree above..
	RUN(combine_interval(rtrd, b->interval,a->interval,b->interval));
	
	a->interval->count = a->interval->count + b->interval->count;
	if(rtrd->merge_action){
	//	RUN(rtrd->merge_action(  a->interval->data,    b->interval->data ),"merge_action failed.");
	}
	return OK;
ERROR:
	return FAIL;
}


int perform_a_split(struct rtr_data* rtrd)
{
	struct rtree_interval* cover = NULL;
	struct rtree_interval* tmp_interval = NULL;

	
	int num_elements = rtrd->num_branches+1;
	int min_elements = num_elements /2;
	int i,j,c;
	double area;
	
	double biggestDiff;
	double worst;
	double waste;
	int seed1;
	int seed2;
	
	
	cover = rtrd->tmp_cover;
	
	//RUNP(cover = init_rtree_interval(rtrd),"init_rtree_interval failed.");

	
	//RUNP(tmp_interval = init_rtree_interval(rtrd),"init_rtree_interval failed.");
	tmp_interval = rtrd->tmp_interval;
	
	// init rtrd variables...
	rtrd->count[0] = 0;
	rtrd->count[1] = 0;
	
	for(c = 0;c < rtrd->dim << 1;c++){
		cover->coordinates[c] = rtrd->branch_buffer[0]->interval->coordinates[c];
	}
	
	for(i = 0 ;i <num_elements;i++){
		rtrd->group[i] = 0;
		rtrd->taken[i] = 0;
		
		RUN(combine_interval(rtrd, cover,cover, rtrd->branch_buffer[i]->interval));
		
		rtrd->area[i] = intervalSphericalVolume(  rtrd->branch_buffer[i]->interval,rtrd->dim);
		
	}
	area = intervalSphericalVolume(cover,rtrd->dim);
	//print_rtree_node(rtrd,rtrd->root);
	//fprintf(stdout," Area: %f\n",area );
	
	//for(i = 0 ;i <num_elements;i++){
	//	fprintf(stdout,"local Area: %E	 (%E)\n",rtrd->area[i], area -rtrd->area[i] );
	//}
	worst = -area - 1;
	//fprintf(stdout," worst: %f\n",worst );
	
	seed1 = -1;
	seed2 = -1;
	
	for(i = 0 ; i < num_elements-1;i++){
		for(j = i+1; j < num_elements;j++){
			// combin 2 rectangles...
			
			RUN(combine_interval(rtrd, cover,rtrd->branch_buffer[i]->interval, rtrd->branch_buffer[j]->interval));
			waste = intervalSphericalVolume(cover,rtrd->dim) - rtrd->area[i] -rtrd->area[j];
			//fprintf(stdout,"%d %d %f	",i,j, waste);
			if(waste > worst){
			//	fprintf(stdout," SET ");
				worst = waste;
				seed1 = i;
				seed2 = j;
			}
		//	fprintf(stdout,"\n");
			
		}
	}
	ASSERT(seed1 != -1,"Weird - no seed set");
	ASSERT(seed2 != -1,"Weird - no seed set");
	RUN(classify_nodes(rtrd,seed1, 0));
	RUN(classify_nodes(rtrd,seed2, 1));
	
	
	biggestDiff = (double) -1;

	//fprintf(stdout,"%f\n",biggestDiff );
	// OK set seeds....
	int chosen;
	int group;
	int betterGroup;
	
	while(rtrd->count[0] + rtrd->count[1]  < num_elements && rtrd->count[0]  < num_elements-min_elements &&  rtrd->count[1]  < num_elements-min_elements){
		
		biggestDiff =-1.0;
		//biggestDiff = (RectReal)-1.;
		chosen = -1;
		betterGroup = -1;
		for (i=0; i<num_elements; i++)	{
			if (!rtrd->taken[i]){
				//struct Rect *r, rect_0, rect_1;
				double growth0, growth1, diff;
				
				//r = &BranchBuf[i].rect;
				
				RUN(combine_interval(rtrd,tmp_interval , rtrd->branch_buffer[i]->interval, rtrd->split_interval[0]));
				growth0 = intervalSphericalVolume(tmp_interval, rtrd->dim) - rtrd->split_area[0];
				
				RUN(combine_interval(rtrd,tmp_interval , rtrd->branch_buffer[i]->interval, rtrd->split_interval[1]));
				growth1 = intervalSphericalVolume(tmp_interval, rtrd->dim) - rtrd->split_area[1];
				
				diff = growth1 - growth0;
				if (diff >= 0){
					group = 0;
				}else	{
					group = 1;
					diff = -diff;
				}
				
				if (diff > biggestDiff){
					biggestDiff = diff;
					chosen = i;
					betterGroup = group;
				}else if (diff==biggestDiff &&  rtrd->count[group]<  rtrd->count[betterGroup])	{
					chosen = i;
					betterGroup = group;
				}
			}
		}
		ASSERT(betterGroup != -1,"betterGroup should not be -1");
		ASSERT(chosen != -1,"chosen should not be -1");
		
		RUN(classify_nodes(rtrd,chosen ,betterGroup));

	}
	
	if (rtrd->count[0] + rtrd->count[1] < num_elements){
		if (rtrd->count[0] >= num_elements - min_elements ){
			group = 1;
		}else{
			group = 0;
		}
		for (i=0; i< num_elements; i++){
			if (!rtrd->taken[i]){
				RUN(classify_nodes(rtrd,i ,group));
			}
		}
	}
	
	ASSERT(rtrd->count[0] + rtrd->count[1] == num_elements," Weird some nodes were not assigned to a group?");
	ASSERT(rtrd->count[0] >= min_elements && rtrd->count[1] >= min_elements," more weirdness");
	
	
	
	//for(i = 0 ; i < num_elements;i++){
	//	fprintf(stdout," %d %d\n", i, rtrd->group[i]);
	//}
	//RTreeClassify(seed1, 1, p);
	//free_rtree_interval(tmp_interval);
	//free_rtree_interval(cover);

	return OK;
ERROR:
	//free_rtree_interval(cover);

	return FAIL;
}


/*
 Below are init, reset and free functions for the main datastructures...
 
 
*/
struct rtr_data* init_rtr_data(int (*merge_action_funtion_pointer)(void* dataA, void* dataB),int(* compare_function_pointer)(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff), int dim, int num_branches,int expected_num_items)
{
	struct rtr_data* rtrd = NULL;
	int i;
	MMALLOC(rtrd, sizeof(struct rtr_data));
	
	
	
	rtrd->si = NULL;
	rtrd->branch_buffer = NULL;
	rtrd->group = NULL;
	rtrd->taken = NULL;
	rtrd->area = NULL;
	rtrd->flat_interval = NULL;
	
	rtrd->dim = dim;
	rtrd->num_branches = num_branches;
	rtrd->root = NULL;
	rtrd->split_interval = NULL;
	rtrd->new_node = NULL;
	rtrd->new_branch = NULL;
	rtrd->merge_action = merge_action_funtion_pointer;
	rtrd->compare_function = compare_function_pointer;
	
	rtrd->stats_duplicated = 0;
	rtrd->stats_overlapping = 0;
	rtrd->stats_num_interval = 0;
	rtrd->r = 0.5;
	rtrd->compare_cutoff = 0.0;
	rtrd->mode = MODE_RTREE_MERGE_NOPE;
	rtrd->insert = insert; 
	MMALLOC(rtrd->branch_buffer, sizeof(struct rtree_branch*) * (num_branches+1));
	MMALLOC(rtrd->group, sizeof(uint8_t) * (num_branches+1));
	MMALLOC(rtrd->taken, sizeof(uint8_t ) * (num_branches+1));
	MMALLOC(rtrd->area, sizeof(double ) * (num_branches+1));
	
	
	// for optimization. .v
	
	rtrd->new_node_store = NULL;
	rtrd->new_node_index = 0;
	rtrd->num_new_node_malloc = expected_num_items  / (num_branches /2) + 1;
	
	
	rtrd->num_flat_interval_malloc = expected_num_items;
	
	MMALLOC(rtrd->flat_interval, sizeof(struct rtree_interval*) *rtrd->num_flat_interval_malloc );
	for(i = 0 ;i < rtrd->num_flat_interval_malloc;i++){
		rtrd->flat_interval[i] = NULL;
	}
	
	
	MMALLOC(rtrd->new_node_store, sizeof(struct rtree_node*)  * rtrd->num_new_node_malloc );
	
	for(i = 0; i <  rtrd->num_new_node_malloc;i++){
		rtrd->new_node_store[i]  = NULL;
		RUNP(rtrd->new_node_store[i] = init_rtr_node(rtrd));
	}
	
	rtrd->tmp_interval = NULL;
	rtrd->tmp_cover = NULL;
	
	RUNP(rtrd->tmp_interval = init_rtree_interval(rtrd));
	RUNP(rtrd->tmp_cover = init_rtree_interval(rtrd));
	
	
	
	MMALLOC(rtrd->split_interval,sizeof(struct rtree_interval*) * 2);
	rtrd->split_interval[0] = NULL;
	rtrd->split_interval[1] = NULL;
	RUNP(rtrd->split_interval[0]  = init_rtree_interval(rtrd));
	RUNP(rtrd->split_interval[1]  = init_rtree_interval(rtrd));
	
	
	RUNP(rtrd->new_branch = init_rtr_branch(rtrd));

	
	for(i = 0; i < num_branches+1;i++){
		rtrd->branch_buffer[i] = NULL;
		
		RUNP(rtrd->branch_buffer[i] = init_rtr_branch(rtrd));
	}
	
	rtrd->root = rtrd->new_node_store[0];
	rtrd->new_node_index++;
	
	//RUNP(rtrd->root = init_rtr_node(rtrd)," init_rtr_node failed.");
	
	return rtrd;
ERROR:
	free_rtr_data(rtrd);
	return NULL;
}


int extend_num_nodes_in_rtr_data(struct rtr_data* rtrd, int num_add_elements)
{
	int i;
	if(num_add_elements + rtrd->new_node_index > rtrd->num_new_node_malloc){
		
		rtrd->num_new_node_malloc =  rtrd->new_node_index + num_add_elements ;
		MREALLOC(rtrd->new_node_store, sizeof(struct rtree_node*)  * rtrd->num_new_node_malloc );
		for(i = rtrd->new_node_index ; i <  rtrd->num_new_node_malloc;i++){
			rtrd->new_node_store[i]  = NULL;
			RUNP(rtrd->new_node_store[i] = init_rtr_node(rtrd));
		}
	}
	
	
	return OK;
ERROR:
	return FAIL;
}

int reset_rtr_data(struct rtr_data* rtrd)
{
	int i;
	ASSERT(rtrd != NULL, "No data!");
	for(i = 0; i < rtrd->num_new_node_malloc;i++){
		reset_rtr_node(rtrd,  rtrd->new_node_store[i]);
	}
	RUN(reset_rtree_interval(rtrd,rtrd->tmp_cover));
	RUN(reset_rtree_interval(rtrd,rtrd->tmp_interval));
	
	RUN(reset_rtree_interval(rtrd,rtrd->split_interval[0]));
	RUN(reset_rtree_interval(rtrd,rtrd->split_interval[1]));
	
	for(i = 0; i < rtrd->num_branches+1;i++){
		RUN(reset_rtr_branch(rtrd, rtrd->branch_buffer[i]));
	}
	for(i = 0 ;i < rtrd->num_flat_interval_malloc ;i++){
		rtrd->flat_interval[i] = NULL;
	}
	
	rtrd->new_node_index = 0;
	rtrd->root = rtrd->new_node_store[0];
	rtrd->new_node_index++;
	
	rtrd->stats_duplicated = 0;
	rtrd->stats_overlapping = 0;
	rtrd->stats_num_interval = 0;
	
	return OK;
ERROR:
	return FAIL;
}

void free_rtr_data(struct rtr_data* rtrd)
{
	int i;
	if(rtrd){
		
		if(rtrd->branch_buffer){
			for(i = 0; i <  rtrd->num_branches+1;i++){
				free_rtr_branch(rtrd->branch_buffer[i]);
			}
			MFREE(rtrd->branch_buffer);
		}
		if(rtrd->group){
			MFREE(rtrd->group);// sizeof(uint8_t) * (num_branches+1));
		}
		if(rtrd->taken){
			MFREE(rtrd->taken);//, sizeof(uint8_t ) * (num_branches+1));
		}
		if(rtrd->area){
			MFREE(rtrd->area);
		}
		if(rtrd->split_interval){
			free_rtree_interval(rtrd->split_interval[0]);
			free_rtree_interval(rtrd->split_interval[1]);
			MFREE(rtrd->split_interval);
		}
		if(rtrd->new_branch){
			free_rtr_branch(rtrd->new_branch);
		}
		
		if(rtrd->root){
			//free_rtr_node(rtrd, rtrd->root);
		}
		if(rtrd->flat_interval){
			MFREE(rtrd->flat_interval);
		}
		if(rtrd->new_node_store){
			for(i =  0; i < rtrd->num_new_node_malloc;i++){
				if(rtrd->new_node_store[i]){
					free_rtr_node(rtrd, rtrd->new_node_store[i]);
				}
			}
			MFREE(rtrd->new_node_store);
		}
		if(rtrd->tmp_interval){
			free_rtree_interval(rtrd->tmp_interval);
		}
		if(rtrd->tmp_cover){
			free_rtree_interval(rtrd->tmp_cover);
		}
		MFREE(rtrd);
	}
}

struct rtree_node* init_rtr_node(struct rtr_data* rtrd)
{
	struct rtree_node* rtrn = NULL;
	int num_branches = rtrd->num_branches;
	int i;
	MMALLOC(rtrn, sizeof(struct rtree_node));
	
	rtrn->branch = NULL;
	
	
	MMALLOC(rtrn->branch, sizeof(struct rtree_branch*) * num_branches);
	
	for(i = 0; i < num_branches;i++){
		RUNP(rtrn->branch[i] = init_rtr_branch(rtrd));
	}
	
	rtrn->count = 0;
	rtrn->level = 0;
	
	
	
	return rtrn;
ERROR:
	return NULL;
}


int reset_rtr_node(struct rtr_data* rtrd,	struct rtree_node* rtrn)
{
	int i;
	
	for(i = 0; i < rtrd->num_branches;i++){
		RUN(reset_rtr_branch(rtrd, rtrn->branch[i] ));
	}
	
	rtrn->count = 0;
	rtrn->level = 0;
	return OK;
ERROR:
	return FAIL;
}

void free_rtr_node(struct rtr_data* rtrd,	struct rtree_node* rtrn)
{
	if(rtrn){
		int num_branches = rtrd->num_branches;
		int i;
		if(rtrn->branch){
			for(i = 0; i < num_branches;i++){
				
				if(rtrn->branch[i]->child){
				//	free_rtr_node(rtrd,rtrn->branch[i]->child);
				}
				
				free_rtr_branch(rtrn->branch[i]);
				//RUNP(rtrn->branch[i] = init_rtr_branch(rtrd),"init_rtr_branch failed.");
			}
			MFREE(rtrn->branch);
		}
		MFREE(rtrn);
	}
}




struct rtree_branch* init_rtr_branch(struct rtr_data* rtr_data)
{
	//int dim =rtr_data->dim;
	
	struct rtree_branch* rtrb = NULL;
	MMALLOC(rtrb, sizeof(struct rtree_branch));
	rtrb->child = NULL;
	rtrb->interval = NULL;
	rtrb->identifier = 0;
	RUNP(rtrb->interval  = init_rtree_interval(rtr_data));
	
	return rtrb;
ERROR:
	free_rtr_branch(rtrb);
	return NULL;
}

int reset_rtr_branch(struct rtr_data* rtr_data,struct rtree_branch* rtrb )
{
	//int dim = rtr_data->dim;
	RUN(reset_rtree_interval(rtr_data,rtrb->interval));
	rtrb->child = NULL;
	rtrb->identifier = 0;
	return OK;
ERROR:
	return FAIL;
}


void free_rtr_branch(struct rtree_branch* rtrb)
{
	if(rtrb){
		if(rtrb->interval){
			free_rtree_interval(rtrb->interval);
		}
		MFREE(rtrb);
	}
}


struct rtree_interval* init_rtree_interval(struct rtr_data* rtr_data)
{
	int dim = rtr_data->dim;
	struct rtree_interval* rtri = NULL;
	MMALLOC(rtri, sizeof(struct rtree_interval));
	rtri->coordinates = NULL;
	//rtri->data = NULL;
	rtri->count = 0;
	
	
	MMALLOC(rtri->coordinates, sizeof(int32_t) * (dim << 1));
	RUN(reset_rtree_interval(rtr_data,rtri));
	
	return rtri;
ERROR:
	free_rtree_interval(rtri);
	return NULL;
}

int reset_rtree_interval(struct rtr_data* rtr_data,struct rtree_interval* rtri)
{
	int dim = rtr_data->dim;
	ASSERT(rtri != NULL,"tree interval is not allocated! ");
	int i;
	for(i = 0; i < (dim << 1);i++){
		rtri->coordinates[i] = 0;
	}
	//rtri->data = NULL;
	rtri->count = 0;
	return OK;
ERROR:
	return FAIL;
}

void free_rtree_interval(struct rtree_interval* rtri)
{
	if(rtri){
		if(rtri->coordinates){
			MFREE(rtri->coordinates);
		}
		MFREE(rtri);
	}
}


struct rtree_search_results* init_search_results(void)
{
	struct rtree_search_results* sr = NULL;
	int i;
	MMALLOC(sr, sizeof(struct rtree_search_results));
	sr->nodes = NULL;
	sr->query = NULL;
	sr->num_results = 0;
	sr->num_results_malloced = 16;

	MMALLOC(sr->nodes, sizeof(struct rtree_interval*) * sr->num_results_malloced);
	for(i = 0; i < sr->num_results_malloced;i++){
		sr->nodes[i] = NULL;
	}
	
	return sr;
ERROR:
	return NULL;
}

int reset_search_results(struct rtree_search_results* sr)
{
	int i;
	for(i = 0; i< sr->num_results;i++){
		
		sr->nodes[i] = 0;
	}
	sr->num_results = 0;
	return OK;
}

void free_search_results(struct rtree_search_results* sr)
{
	if(sr){
		MFREE(sr->nodes);
		MFREE(sr);
	}
}




int print_rtree_interval(struct rtr_data* rtr_data,struct rtree_interval* rtri)
{
	int i;
	
	
	for(i = 0; i < rtr_data->dim;i++){
		fprintf(stdout,"%10lld - %10lld ",(long long int) rtri->coordinates[i], (long long int) rtri->coordinates[i+ rtr_data->dim]);
	}
	
	fprintf(stdout,"\t COUNT: %d\n", rtri->count);
	return OK;
}

int count_leaf_entries(struct rtr_data* rtr_data, struct rtree_node* n,int* count)
{
	int i;
	if(!n->level){
		
		*count += n->count;
	}else{
		for(i = 0; i < n->count;i++){
			if(n->branch[i]->child){
				RUN(count_leaf_entries(rtr_data,n->branch[i]->child,count ));
			}
		}
	}
	return OK;
ERROR:
	return FAIL;
}

int re_label_tree_nodes(struct rtr_data* rtr_data, struct rtree_node* n,int* identifier)
{
	int i;
	if(!n->level){
		for(i = 0; i < n->count;i++){
			
			n->branch[i]->identifier = *identifier;
			*identifier = *identifier + 1;
		}
	}else{
		for(i = 0; i < n->count;i++){
			if(n->branch[i]->child){
				RUN(re_label_tree_nodes(rtr_data,n->branch[i]->child,identifier ));
			}
		}
	}
	return OK;
ERROR:
	return FAIL;
}


int print_rtree_node(struct rtr_data* rtr_data, struct rtree_node* rtr_node)
{
	int i;
	struct rtree_interval* interval = NULL;
	
	//if(!rtr_node->level){
		fprintf(stdout,"LEVEL%d\n",rtr_node->level);
		
		for(i = 0; i < rtr_node->count;i++){
			fprintf(stdout,"node:%d\t(ID:%d)\n",i,rtr_node->branch[i]->identifier );
			interval = rtr_node->branch[i]->interval;
			fprintf(stdout,"\t");
			RUN(print_rtree_interval(rtr_data, interval));
		}
	//}
	for(i = 0; i < rtr_node->count;i++){
		if(rtr_node->branch[i]->child){
			print_rtree_node(rtr_data,rtr_node->branch[i]->child);
		}
	}
	return OK;
ERROR:
	return FAIL;
}

int print_tikz_image(struct rtr_data* rtr_data, struct rtree_node* rtr_node,int maxlevel)
{
	ASSERT(rtr_data->dim == 2,"Not 2 dimensional");
	int i;
	struct rtree_interval* interval = NULL;
	float thickness = 0.1 * (float)(rtr_node->level+1) ;
	
	int color = (int)  ( (float) (rtr_node->level+1) / (float) (maxlevel+1)* 100.0);
	
	//fprintf(stdout,"LEVEL%d\n",rtr_node->level);
	
	
	
	for(i = 0; i < rtr_node->count;i++){
		if(rtr_node->branch[i]->child){
			RUN(print_tikz_image(rtr_data,rtr_node->branch[i]->child,maxlevel));
		}
	}
	for(i = 0; i < rtr_node->count;i++){
		//fprintf(stdout,"node:%d (ID:%d)\n",i,rtr_node->branch[i]->identifier );
		interval = rtr_node->branch[i]->interval;
		//fprintf(stdout,"\t");
		
		//RUN(print_rtree_interval(rtr_data, interval),"print_rtree_interval failed.");
		fprintf(stdout,"\\draw [rounded corners,draw = black!%d!white, line width=%fem]   (%f,%f) rectangle (%f,%f);\n",color,thickness,(float)interval->coordinates[0]/100.0,(float)interval->coordinates[1]/100.0,(float)interval->coordinates[2]/100.0,(float)interval->coordinates[3]/100.0);
		
	}
	
	
	return OK;
ERROR:
	return FAIL;
}




int pick_branch( struct rtr_data* rtrd, struct rtree_node *n)
{
	register struct rtree_interval *r = rtrd->new_branch->interval;
	register int i, first_time=1;
	
	double increase,bestIncr,area, bestArea;
	
	bestIncr = -1.0;
	
	
	int best = -1;
	struct rtree_interval* tmp_interval;
	
	//RUNP(tmp_interval = init_rtree_interval(rtrd),"init_rtree_interval failed.");
	
	tmp_interval = rtrd->tmp_interval;
	
	
	for(i = 0; i < rtrd->num_branches;i++){
		if(n->branch[i]->child){
			area = intervalSphericalVolume(n->branch[i]->interval , rtrd->dim);
			RUN(combine_interval(rtrd, tmp_interval, r, n->branch[i]->interval));
			increase = intervalSphericalVolume(tmp_interval,  rtrd->dim) - area;
			if (increase < bestIncr || first_time){
				best = i;
				bestArea = area;
				bestIncr = increase;
				first_time = 0;
			}else if (increase == bestIncr && area < bestArea)	{
				best = i;
				bestArea = area;
				bestIncr = increase;
			}
		}
	}
	//free_rtree_interval(tmp_interval);
	
	return best;
ERROR:
	return best;
}



/**
 * Find the smallest rectangle that includes all rectangles in
 * branches of a node.
 */
int node_cover(struct rtr_data* rtrd, struct rtree_node *n,struct rtree_interval* r )
{
	
	register int i, first_time=1;
	for(i = 0; i < rtrd->num_branches;i++){
		if(n->branch[i]->identifier || n->branch[i]->child){
			if(first_time){
				RUN(copy_interval(rtrd, r, n->branch[i]->interval));
				first_time = 0;
			}else{
				RUN(combine_interval(rtrd, r, r, n->branch[i]->interval));
			}
		}
	}
	return OK;
ERROR:
	return FAIL;
}



double intervalSphericalVolume(struct rtree_interval *r , int dim)
{
	
	const double UnitSphereVolumes[] = {
		0.000000,  /* dimension   0 */
		2.000000,  /* dimension   1 */
		3.141593,  /* dimension   2 */
		4.188790,  /* dimension   3 */
		4.934802,  /* dimension   4 */
		5.263789,  /* dimension   5 */
		5.167713,  /* dimension   6 */
		4.724766,  /* dimension   7 */
		4.058712,  /* dimension   8 */
		3.298509,  /* dimension   9 */
		2.550164,  /* dimension  10 */
		1.884104,  /* dimension  11 */
		1.335263,  /* dimension  12 */
		0.910629,  /* dimension  13 */
		0.599265,  /* dimension  14 */
		0.381443,  /* dimension  15 */
		0.235331,  /* dimension  16 */
		0.140981,  /* dimension  17 */
		0.082146,  /* dimension  18 */
		0.046622,  /* dimension  19 */
		0.025807  /* dimension  20 */
	};
	
	register int i;
	register double sum_of_squares=0.0;
	double radius = 0.0;
	//static double UnitSphereVolume = 0.0;
	
	//if(!UnitSphereVolume){
	//	UnitSphereVolume = sphere_volume((double)dim);
	//}

	for (i=0; i < dim; i++) {
		double half_extent = (double) (r->coordinates[i+dim] - r->coordinates[i]) / 2.0;
		sum_of_squares += half_extent * half_extent;
	}
	radius = sqrt(sum_of_squares);
	
	radius =pow(radius, dim) * UnitSphereVolumes[dim];
	return radius;
}

double sphere_volume(double dimension)
{
	static double log_pi = (-1);
	double log_gamma, log_volume;
	
	if (log_pi < 0) {
		log_pi = log(M_PI);
	}
	
	log_gamma = lgamma(dimension/2.0 + 1);
	log_volume = dimension/2.0 * log_pi - log_gamma;
	return exp(log_volume);
}


int combine_interval(struct rtr_data* rtr_data, struct rtree_interval* target,struct rtree_interval* sourceA,struct rtree_interval* sourceB)
{
	int dim = rtr_data->dim;
	int i;
	int j;
	
	for(i = 0;i < dim;i++){
		target->coordinates[i] = MACRO_MIN(sourceA->coordinates[i], sourceB->coordinates[i]);
		j = i + dim;
		target->coordinates[j] = MACRO_MAX(sourceA->coordinates[j],  sourceB->coordinates[j]);
	}
	
	return OK;
}

int copy_interval(struct rtr_data* rtr_data, struct rtree_interval* target,struct rtree_interval* source)
{
	int dim = rtr_data->dim;
	int i;
	target->count = source->count;
	for(i = 0; i < dim;i++){
		target->coordinates[i] = 	source->coordinates[i];
		target->coordinates[i+dim] = 	source->coordinates[i+dim];
		
		
		
	}
	return OK;
}

int copy_branch(struct rtr_data* rtr_data,struct rtree_branch* target, struct rtree_branch* source)
{
	target->child = source->child;
	target->identifier = source->identifier;
	RUN(copy_interval(rtr_data,  target->interval,source->interval));
	return OK;
ERROR:
	return FAIL;
}

int classify_nodes(struct rtr_data* rtrd, int target, int group)
{
	//assert(p);
	//assert(!p->taken[i]);
	rtrd->group[target] = group;
	rtrd->taken[target] = 1;
	
	if(rtrd->count[group] ==0){
		RUN(copy_interval(rtrd,rtrd->split_interval[group], rtrd->branch_buffer[target]->interval));
	}else{
		RUN(combine_interval(rtrd, rtrd->split_interval[group],rtrd->split_interval[group], rtrd->branch_buffer[target]->interval));
	}
	rtrd->split_area[group] = intervalSphericalVolume( rtrd->split_interval[group], rtrd->dim);
	rtrd->count[group]++;
	//p->area[group] = RTreeRectSphericalVolume(&p->cover[group]);
	//p->count[group]++;
	return OK;
ERROR:
	return FAIL;
}

// genome is 3234830000 nt long... times two is :
// 6,469,660,000 - need to divide by 4 (ie. right shift by 2 AT LEAST!)
int make_rtree_coordinates(struct rtree_interval *a, int64_t* in_coordinates, int32_t dim, int64_t shift)
{
	register int32_t i,j;
	
	ASSERT(shift >= 2,"warning: the smallest bucket size for coordinated in rtree is 4 nt... ");
	if(!shift){
		shift = 2;
	}
	for(i = 0; i < dim;i++){
		a->coordinates[i] = (int32_t) (in_coordinates[i] >> shift);
		
		j = i + dim;
		a->coordinates[j] = (int32_t) ((in_coordinates[j] >> shift) +1);
	}
	return OK;
ERROR:
	return FAIL;
}

int rescale_interval(struct rtr_data* rtrd,struct rtree_interval *a, int32_t dim)
{
	register int i,j;
	for(i = 0; i < rtrd->dim;i++){
		a->coordinates[i] = a->coordinates[i] >> dim;
	
		j = i + rtrd->dim;
		a->coordinates[j] = (a->coordinates[j] >> dim) +1;
	}
	
	return OK;
}

/**
 * Decide whether two rectangles overlap.
 */
int node_overlap(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b)
{

	register int i, j;

	for(i = 0; i < rtrd->dim;i++){
		j = i + rtrd->dim;
		if (a->coordinates[i] > b->coordinates[j] ||    b->coordinates[i] > a->coordinates[j]){
			return 0;
		}
	}
	return 1;
}

int node_reciprogal_overlap(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b,int* result)
{
	float r = rtrd->r;
	float overlap = 0.0;
	int i = 0;
	int j = 0;
	int overlap_dim = 0;
	*result = 0;
	int32_t sa,eb,sb,ea;
	for(i = 0; i < rtrd->dim;i++){
		j = i + rtrd->dim;
		
		sa = a->coordinates[i];
		ea = a->coordinates[j];
		sb = b->coordinates[i];
		eb = b->coordinates[j];
		
		
		if(sa > eb){// a starts after b ends - no poverlap
			
			return OK;
		}
		if(sb > ea){ // b starts a ends - no overlap possible...
		
			return OK;
		}
		
		overlap =  MACRO_MIN(ea , eb) -  MACRO_MAX(sa, sb);
		//DPRINTF1("%ld -%ld vs %ld - %ld", sa,ea,sb,eb);
		//DPRINTF1("%f overlap",overlap);
		if(r < 1.0){
			
			//	DPRINTF1("%f overlap a",overlap / (float) (ea - sa) );
			//	DPRINTF1("%f overlap b",overlap / (float) (eb - sb) );
			
			
			if( (overlap / (float) (ea - sa) >= r) && (overlap  / (float)(eb-sb) >=r)){
				overlap_dim++;
		//		DPRINTF1("OK");
			}else{
				return OK;
			}
		}else{
			if(overlap >= r){
				overlap_dim++;
			}else{
				return OK;
			}
		}
	}
	if(overlap_dim == rtrd->dim){
		*result = 1;
	}
	return OK;
}



int node_similar_euclidian_distance(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff)
{
	register int i,j;
	double ds = 0.0;
	double de = 0.0;
	
	double w = 0;
	for(i = 0; i < rtrd->dim;i++){
		w = (double) (a->coordinates[i]  - b->coordinates[i]);
		ds += w*w;
		j = i + rtrd->dim;
		w = (double) (a->coordinates[j]  - b->coordinates[j]);
		de += w*w;
		
	}
	ds = sqrt(ds);
	de = sqrt(de);
	if(ds <= cutoff || de <= cutoff){
		return 1;
	}
	return 0;
}

int node_identical(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff)
{
	register int i, j;

	for(i = 0; i < rtrd->dim;i++){
		if(a->coordinates[i] != b->coordinates[i]){
			return 0;
		}
		j = i + rtrd->dim;
		if(a->coordinates[j] != b->coordinates[j]){
			return 0;
		}
	}
	
	return 1;
}


int node_contained(struct rtr_data* rtrd,struct rtree_interval *a, struct rtree_interval *b, double cutoff)
{
	// a should be contained in b !
	// or b in a//
	
	
	/*
	 1)
	 sa > sb && ea < eb
	 2)
	 sb > sa && eb < ea
	
	 if(sa < sb 
	 
	 // max s 
	 // max e 
	 
	 if(sa <  sb && ea < eb)
	 
	 if(sb < sa && eb < ea)
	 
	 */
	
	register int i, j;
	
	for(i = 0; i < rtrd->dim;i++){
		j = i + rtrd->dim;
		
		// this should capture:
		// --
		//     ---
		//and
		// --
		//  ---
		if(a->coordinates[i]  < b->coordinates[i] && a->coordinates[j] < b->coordinates[j]){
			return 0;
		}
		// this should capture:
		//     ---
		// --
		//and
		//  ---
		// --
		if(b->coordinates[i]  < a->coordinates[i] && b->coordinates[j] < a->coordinates[j]){
			return 0;
		}
		
		/*
		if(a->coordinates[i]  < b->coordinates[i]){
			return 0;
		}
		
		if(a->coordinates[j] > b->coordinates[j]){
			return 0;
		}*/
	}
	
	return 1;
}


int sort_interval_based_on_count(const void * a, const void * b)
{
	int64_t c;
	struct rtree_interval* const *one = a;
	struct rtree_interval* const *two = b;
	
	c = (int64_t)((*one)->count) - (int64_t)((*two)->count);
	
	
	if(c > 0){
		return -1;
	}else if(c < 0){
		return 1;
	}else{
		return 0;
	}
}


int sort_interval_based_on_count_and_coordinates(const void * a, const void * b)
{
	int64_t c;
	int i;
	int dim = 0;
	struct rtree_interval* const *one = a;
	struct rtree_interval* const *two = b;
	
	
	int64_t c_a, c_b;
	
	dim =(*one)->count >> 28;
	
	c_a = (int64_t)(((*one)->count) & 0xFFFFFFF);
	c_b = (int64_t)(((*two)->count) & 0xFFFFFFF);
	
	c = c_a -  c_b;
	
	
	if(c > 0){
		return -1;
	}else if(c < 0){
		return 1;
	}else{
		for(i = 0; i < dim;i++){
			c = (int64_t)((*one)->coordinates[i]) - (int64_t)((*two)->coordinates[i]);
			if(c > 0){
				return -1;
			}else if(c < 0){
				return 1;
			}
			
			c = (int64_t)((*one)->coordinates[i+dim]) - (int64_t)((*two)->coordinates[i+dim]);
			if(c > 0){
				return -1;
			}else if(c < 0){
				return 1;
			}

			
		}
		
		return 0;
		
	}
}






void sort_coordinates(int32_t* c , int dim)
{
	q_sort_coordinates(c, 0, dim - 1, dim);
}

void q_sort_coordinates(int32_t* c, int32_t left, int32_t right,int dim)
{
	int32_t pivot,pivot2,l_hold, r_hold;
	l_hold = left;
	r_hold = right;
	pivot = c[left];
	pivot2 = c[left + dim];
	while (left < right){
		while (( c[right] >= pivot) && (left < right)){
			right--;
		}
		if (left != right){
			c[left] = c[right];
			c[left+dim] = c[right+dim];
			left++;
		}
		while (( c[left]<= pivot) && (left < right)){
			left++;
		}
		if (left != right){
			c[right] = c[left];
			c[right+dim] = c[left+dim];
			right--;
		}
	}
	c[left] = pivot;
	c[left+dim] = pivot2;
	
	pivot = left;
	left = l_hold;
	right = r_hold;
	if (left < pivot){
		q_sort_coordinates(c, left, pivot-1,dim);
	}
	if (right > pivot){
		q_sort_coordinates(c, pivot+1, right,dim);
	}
}



