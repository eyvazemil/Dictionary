#include <stdio.h>
#include <stdlib.h>

// AVL tree node 
typedef struct _node {
    void * key;
    struct _node * left;
    struct _node * right;
    int height;
} Node;

// AVL tree
typedef struct avl {
    Node * root;
    int (*compare_func)(const void *, const void *);
    void (*key_free)(void *);
} AVL;

// initialize an AVL tree
void _avl_init_(AVL *);
// assign custom functions for AVL tree
void assign_compare_func(AVL *, int (*my_comparator)(const void *, const void *));
void assign_free_func(AVL *, void (*my_free)(void *));
// get maximum of two integers
static int max(int, int);
// get the height of the tree
static int height(Node *);
static Node * create_node(void *);
// right rotate 
static Node * right_rotate(Node *);
// left rotate
static Node * left_rotate(Node *);
// get Balance factor of node 
static int get_balance(Node *);
// insert and balance the tree(if needed)
/* takes already allocated key and assigns it to the new created node */
/* frees that key, if it is already present in the tree */
/* return 1 if insertion was successful and 0, if such a key was already in the tree */
int insert(AVL *, void *);
static Node * insert_helper(AVL *, Node *, void *);
// find the node with the minimum value
static Node * min_value_node(Node *);
// delete a node from the tree
/* takes already allocated key, but, does not delete it */
/* the key should be deleted manually by calling find_node before deletion and freeing it after */
int delete(AVL *, void *);
static Node * delete_helper(AVL *, Node *, void *);
// find a node(if (exists) return 1; else return 0)
Node * find_node(AVL *, void *);
static Node * find_node_helper(AVL *, Node *, void *);
// free the tree
void free_tree(AVL *);
static void free_tree_helper(AVL *, Node *);
// get keys of the tree in-order
void in_order_get(AVL *, void *, void * (*my_get_callback)(void *, void *));
void in_order_get_func_para(AVL * avl_tree, void * (*func_para)(void *), void * (*my_get_callback)(void * (*func)(void *), void *));
static void in_order_get_helper(Node *, void *, void * (*my_get_callback)(void *, void *));
static void in_order_get_func_para_helper(Node * node, void * (*func_para)(void *), void * (*my_get_callback)(void * (*func)(void *), void *));
