#ifndef LIBSV_RBTREE_H
#define LIBSV_RBTREE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct sv_rb_node_s {
    uint8_t red;
    void *data;
    struct sv_rb_node_s *link[2];
} sv_rb_node_t;

typedef int (*sv_rb_cmp_cb)(const void *p_1, const void *p_2);

typedef void *(*sv_rb_clone_cb)(void *p_data);

typedef void (*sv_rb_free_cb)(void *p_data);

typedef struct sv_rb_tree_s {
    sv_rb_node_t *root;
    sv_rb_cmp_cb on_cmp;
    sv_rb_clone_cb on_clone;
    sv_rb_free_cb on_free;
    size_t size;
} sv_rb_tree_t;

sv_rb_tree_t *sv_rb_new(sv_rb_cmp_cb p_on_cmp, sv_rb_clone_cb p_on_clone, sv_rb_free_cb p_on_free);

void sv_rb_free(sv_rb_tree_t *p_tree);

void *sv_rb_find(sv_rb_tree_t *p_tree, void *p_data);

void sv_rb_insert(sv_rb_tree_t *p_tree, void *p_data);

void sv_rb_erase(sv_rb_tree_t *p_tree, void *p_data);

#endif // LIBSV_RBTREE_H
