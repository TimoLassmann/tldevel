
#ifndef lib_rbtree_header
#define lib_rbtree_header


struct rbtree_node{
	struct rbtree_node* right;
	struct rbtree_node* left;
	void* data_node;
	int color;
	unsigned int num;
};


struct rbtree_root{
	struct rbtree_node* node;
	void** data_nodes;
	int cur_data_nodes;
	int num_entries;
	int identical_mode;
	void* (*key)(void* ptr);
	long int (*compare)(void* keyA, void* keyB);
	int (*resolve_same)(void* ptr_a,void* ptr_b);
	void (*fp_print)(void* ptr,FILE* out_ptr);
	void (*fp_free)(void* ptr);
};


extern struct rbtree_root* init_tree(void* (*key_function_pointer)(void* ptr), long int (*compare_function_pointer)(void* keyA, void* keyB),int (*resolve_same_pointer)(void* ptr_a,void* ptr_b),void (*fp_print)(void* ptr,FILE* out_ptr),void (*fp_free)(void* ptr));

//void tree_insert(struct rbtree_root* root, void* datanode);
extern int tree_insert(struct rbtree_root* root, void* datanode);
extern int tree_delete(struct rbtree_root* root, void* datanode);
extern void* tree_get_data(struct rbtree_root* root, void* key);
extern struct rbtree_node*  tree_get_node(struct rbtree_root* root, void* key);

extern void free_tree( struct rbtree_root* root);
extern void print_tree(struct rbtree_root* root,FILE* out_ptr);

extern int flatten_tree(struct rbtree_root* root);

#endif

