#ifndef BST_H_
#define BST_H_

#include <stdint.h>

/* Single node with int values */
struct bst_node {
    int val;
    uint32_t left;
    uint32_t right;
};
#define BST_NULL 0
#define BST_FIRST 1
#define bst_has_child(n) ((n)->left | (n)->right)

#define bst_node_init(n, value) do{ \
    (n)->val = (value); \
    (n)->left = BST_NULL; \
    (n)->right = BST_NULL; \
}while(0)

/* BST */
struct bst {
    struct bst_node * nodes;
    uint32_t nodes_max;
    uint32_t nodes_count;
    uint32_t head_free; /* next node of free list is stored in right child
                               of each tree node. Not obvious,
                               but no extra space required. */
};
typedef struct bst bst_t;

#define bst_is_full(b) ((b)->nodes_count == (b)->nodes_max)
#define bst_is_empty(b) ((b)->nodes_count == 0)
#define bst_node_count(b) ((b)->nodes_count)
#define bst_get_node(b, i) (&((b)->nodes)[(i)])

int bst_init(struct bst* b, uint32_t max_len);
void bst_clean(struct bst* b);

int bst_insert(struct bst* b, int elm);
int bst_remove(struct bst* b, int elm);
int bst_find(const struct bst* b, int elm, uint32_t *ret);
int bst_inorder(const struct bst* b);

#endif /* BST_H_ */
