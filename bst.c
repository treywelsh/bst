#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stack.h>
#include <err.h>

#include "bst.h"

/* Internal free list management */
static void
bst_freelist_init(bst_t * b) {
    uint32_t i;

    assert(b != NULL);

    b->head_free = BST_FIRST;

    /* initialize free list */
    for (i = 0 ; i <= b->nodes_max ; i++) {
        (b->nodes)[i].right = i + 1;
    }
}

static void
bst_freelist_add(bst_t * b, uint32_t i) {

    assert(b != NULL);

    (b->nodes)[i].right = b->head_free;
    b->head_free = i;

    /* decrement used nodes count*/
    (b->nodes_count)--;
}

static uint32_t
bst_freelist_get(bst_t * b) {
    uint32_t nd;

    assert(b != NULL);

    nd = b->head_free;
    b->head_free = (b->nodes)[b->head_free].right;

    /* increment used nodes count*/
    (b->nodes_count)++;

    return nd;
}


int
bst_init(struct bst* b, uint32_t max_len) {

    assert(b != NULL);
    assert(max_len >= 3);

    b->nodes = malloc(sizeof(*(b->nodes)) * (max_len + 1));
    if (b->nodes == NULL) {
        return 1;
    }
    memset(b->nodes, 0, sizeof(*(b->nodes)) * (max_len + 1));

    b->nodes_max = max_len;
    b->nodes_count = 0;

    bst_freelist_init(b);

    return 0;
}

void
bst_clean(struct bst* b) {
    assert(b != NULL);
    free(b->nodes);
}

/* internal return codes for __bst_find */
enum {
    NODE_NOT_FOUND = 0,
    NODE_FOUND = 1,
};
static int
__bst_find(const struct bst * b, int elm, uint32_t * index, uint32_t * parent_index) {
    uint32_t i, i_prev;
    struct bst_node * node;

    assert(b != NULL);
    assert(b->nodes != NULL);
    assert(index != NULL);
    assert(parent_index != NULL);

    *index = BST_NULL;
    *parent_index = BST_NULL;

    /* if bst is empty */
    if (bst_is_empty(b)) {
        err_print("empty, cannot find any node\n");
        return 1;
    }

    i_prev = BST_NULL;
    i = BST_FIRST;

    while (bst_get_node(b, i)->val != elm) {
        node = bst_get_node(b, i);
        i_prev = i;

        if (elm > node->val && node->right != BST_NULL) {
            i = node->right;
            continue;
        } else if (elm < node->val && node->left != BST_NULL) {
            i = node->left;
            continue;
        }

        /* There is neither left son nor right son,
         * and current node does not contains the right value
         */
        return NODE_NOT_FOUND;
    }

    *parent_index = i_prev;
    *index = i;

    return NODE_FOUND;
}

int
bst_insert(struct bst* b, int elm) {
    uint32_t i;
    uint32_t free_index;
    struct bst_node * node = NULL;

    assert(b != NULL);
    assert(b->nodes != NULL);

    /* insert first node */
    if (bst_is_empty(b)) {
        free_index = bst_freelist_get(b);
        assert(free_index == BST_FIRST);

        bst_node_init(&(b->nodes)[free_index], elm);
        return 0;
    }

    /* XXX can realloc array of nodes here for a bigger tree */
    if (bst_is_full(b)) {
        err_print("full, cannot insert\n");
        return 1;
    }

    /* Look for insertion point in tree */
   for (i = BST_FIRST ; i != BST_NULL ; ) {
        node = &(b->nodes)[i];

        if (elm > node->val) {
            i = node->right;
        } else {
            i = node->left;
        }
    }

    free_index = bst_freelist_get(b);

    /* link to parent node*/
    if (elm > node->val) {
        node->right = free_index;
    } else {
        node->left = free_index;
    }

    /* Copy node at end of array */
    bst_node_init(&((b->nodes)[free_index]), elm);

    return 0;
}

int
bst_remove(struct bst* b, int elm) {
    uint32_t i, i_prev;
    uint32_t j;
    uint32_t j_min;
    uint32_t j_min_prev;

    assert(b != NULL);
    assert(b->nodes != NULL);

    if (bst_is_empty(b)) {
        err_print("empty, cannot remove\n");
        return 1;
    }

    if (!__bst_find(b, elm, &i, &i_prev)) {
        err_print("Can't find node\n");
        return 1;
    }

    /* http://www.algolist.net/Data_structures/Binary_search_tree/Removal */

    /* left and right cannot be equal, except if both are null */
    assert(((b->nodes)[i].left != (b->nodes)[i].right)
            || ((b->nodes)[i].left == BST_NULL
                && (b->nodes)[i].right == BST_NULL));

    if (((b->nodes)[i].left ^ (b->nodes)[i].right) == 0) {
        /* CASE 1 : No child */
        dbg_print("remove leaf\n");

        /* Set link parent to NULL */
        if (i == (b->nodes)[i_prev].left) {
            (b->nodes[i_prev]).left = BST_NULL;
        } else {
            (b->nodes[i_prev]).right = BST_NULL;
        }
        bst_freelist_add(b, i);

    } else if ((b->nodes)[i].left != BST_NULL
            && (b->nodes)[i].right != BST_NULL) {
        /* CASE 2 : Two children */
        dbg_print("remove node with two children\n");

        /* Look for smallest child, the leftmost child of right subtree */
        j = (b->nodes)[i].right;
        j_min = BST_NULL;
        j_min_prev = BST_NULL;

        while(j != BST_NULL) {
            j_min_prev = j_min;
            j_min = j;
            j = ((b->nodes)[j].left);
        }

        /* copy value */
        (b->nodes)[i].val  = (b->nodes)[j_min].val;

        /* remove selected subtree leave */
        if (j_min_prev == BST_NULL) {
            (b->nodes)[i].right = BST_NULL;
        } else {
            (b->nodes)[j_min_prev].left = BST_NULL;
        }
        bst_freelist_add(b, j_min);

    } else {
        /* CASE 3 : One child */

        dbg_print("remove node with one child\n");
        /* XXX replace test with
         * subtree = (b->nodes[subtree].left | b->nodes[subtree].left) ^ BST_NULL;
         */

        /* child go up */
        if ((b->nodes)[i].left != BST_NULL) {
            (b->nodes)[i_prev].left = (b->nodes)[i].left;
        } else {
            (b->nodes)[i_prev].right = (b->nodes)[i].right;
        }
        bst_freelist_add(b, i);

    }

    return 0;
}

int
bst_find(const struct bst* b, int elm, uint32_t *ret) {
    uint32_t i, i_prev;

    assert(b != NULL);
    assert(b->nodes != NULL);
    assert(ret != NULL);

    i = BST_NULL;
    i_prev = BST_NULL;
    *ret = BST_NULL;

    if (!__bst_find(b, elm, &i, &i_prev)) {
        return 0;
    }
    *ret = i;

    return 1;
}

#define STACK_SIZE 1024
/*
 * struct bst_inorder_iter {
 *  struct bst* b;
 *  struct stack stck;
 *  uint32_t node_i;
 * }
 *
 * int
 * bst_inorder_iter_init(struct bst* b, struct bst_inorder_iter * it) {
 *    if (bst_is_empty(b)) {
 *        return 1;
 *    }
 *
 *    stack_init(&stck, STACK_SIZE);
 *    node_i = BST_FIRST;
 *
 *  return 0;
 * }
 *
 *
 * int
 * bst_inorder_iter_next(struct bst_inorder_iter * it) {
 *
 *  return 0;
 * }
 *
 * int
 * bst_inorder_iter_clean(struct bst_inorder_iter * it) {
 *
 *      stack_clean(&it->stck);
 *      return 0;
 * }
 */
int
bst_inorder(const struct bst* b) {
    uint32_t node_i;
    struct stack stck;

    if (bst_is_empty(b)) {
        return 1;
    }

    stack_init(&stck, STACK_SIZE);

    //prev_node_i = BST_NULL;
    node_i = BST_FIRST;
    do {
        if (node_i != BST_NULL) {
            assert(!stack_is_full(&stck));
            stack_push(&stck, node_i);
            node_i = ((b->nodes)[node_i]).left;
            continue;
        }
        if (!stack_is_empty(&stck)) {
            node_i = stack_pop(&stck);
            //TODO return array or make an iterator ?
            printf("%d ", ((b->nodes)[node_i]).val);
            //prev_node_i = node;
        }
        node_i = ((b->nodes)[node_i]).right;
    } while (node_i != BST_NULL || !stack_is_empty(&stck));
    //TODO
    printf("\n");

    stack_clean(&stck);

    return 0;
}

