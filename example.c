#include "rbtree.h"

#include <string.h>

int my_cmp(const void *p_data_1, const void *p_data_2) {
    int d1 = *((int *) p_data_1);
    int d2 = *((int *) p_data_2);
    if (d1 < d2) return -1;
    else if (d1 > d2) return 1;
    else return 0;
}

void *my_clone(void *p_data) {
    void *ptr = malloc(sizeof(int));
    memcpy(ptr, p_data, sizeof(int));
    return ptr;
}

void my_free(void *p_data) {
    free(p_data);
}

void my_visit(sv_rb_node_t *p_node) {
    printf("%d ", *((int *) p_node->data));
}

void my_traverse(sv_rb_node_t *p_node) {
    if (p_node == NULL) return;
    my_visit(p_node);
    my_traverse(p_node->link[0]);
    my_traverse(p_node->link[1]);
}

int main() {
    sv_rb_tree_t *tree = sv_rb_new(my_cmp, my_clone, my_free);

    int i;
    for (i = 0; i < 10; i++) {
        sv_rb_insert(tree, (void *) &i);
    }

    // insert existing value should be safe.
    i = 6;
    sv_rb_insert(tree, &i);

    printf("should print:\n3 1 0 2 5 4 7 6 8 9\n");
    printf("real print:\n");
    my_traverse(tree->root);
    printf("\n");

    sv_rb_erase(tree, &i);
    // erase none-existing value should be safe.
    sv_rb_erase(tree, &i);

    printf("should print:\n3 1 0 2 5 4 8 7 9\n");
    printf("real print:\n");
    my_traverse(tree->root);
    printf("\n");

    sv_rb_free(tree);
}