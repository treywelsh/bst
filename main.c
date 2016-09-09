#include <stdio.h>

#include "bst.h"
#include <err.h>

INIT_ERR();

int
main (int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    SET_ERR(stdout);

    struct bst b;
    int i, ret, a;
    unsigned int idx;

    bst_init(&b, 1000);

    for (i = 9 ; i < 15 ; i++) {
        bst_insert(&b, i);
    }
    for (i = 8 ; i > 0 ; i--) {
        bst_insert(&b, i);
    }
    for (i = 7 ; i < 12 ; i++) {
        bst_insert(&b, i);
    }
    bst_insert(&b, 12);

    a = 12;
    ret = bst_find(&b, a, &idx);
    if (ret) {
        printf("%d found at %d\n", a, idx);
    }

    //printf("inorder traversal : \n");
    bst_inorder(&b);

    /* Removing nodes */
    printf("remove node %d\n", 9);
    bst_remove(&b, 9);
    bst_inorder(&b);

    printf("remove node %d\n", 9);
    bst_remove(&b, 9);
    bst_inorder(&b);

    printf("remove node %d\n", 9);
    bst_remove(&b, 9);
    bst_inorder(&b);

    printf("remove node %d\n", 13);
    bst_remove(&b, 13);
    bst_inorder(&b);

    printf("remove first instance of node %d\n", 12);
    bst_remove(&b, 12);
    bst_inorder(&b);

    printf("remove leaf %d\n", 12);
    bst_remove(&b, 12);
    bst_inorder(&b);

    bst_clean(&b);
    return 0;
}
