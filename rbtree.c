#include "rbtree.h"

sv_rb_node_t *sv_rb_node_new(sv_rb_tree_t *p_tree, void *p_data) {
    sv_rb_node_t *rn = (sv_rb_node_t *) malloc(sizeof(sv_rb_node_t));
    if (rn == NULL) {
        fprintf(stderr, "sv_rb_node_new: failed to do malloc!\n");
        exit(-1);
    }
    rn->red = 1;
    rn->data = p_tree->on_clone(p_data);
    rn->link[0] = rn->link[1] = NULL;
    return rn;
}

sv_rb_tree_t *sv_rb_new(sv_rb_cmp_cb p_on_cmp, sv_rb_clone_cb p_on_clone, sv_rb_free_cb p_on_free) {
    sv_rb_tree_t *rt = (sv_rb_tree_t *) malloc(sizeof(sv_rb_tree_t));
    if (rt == NULL) {
        fprintf(stderr, "sv_rb_new: failed to do malloc!\n");
        exit(-1);
    }
    rt->root = NULL;
    rt->on_cmp = p_on_cmp;
    rt->on_clone = p_on_clone;
    rt->on_free = p_on_free;
    rt->size = 0;
    return rt;
}

void sv_rb_free(sv_rb_tree_t *p_tree) {
    sv_rb_node_t *it = p_tree->root;
    sv_rb_node_t *save;
    while (it != NULL) {
        if (it->link[0] == NULL) {
            save = it->link[1];
            p_tree->on_free(it->data);
            free(it);
        } else {
            save = it->link[0];
            it->link[0] = save->link[1];
            save->link[1] = it;
        }
        it = save;
    }
    free(p_tree);
}

int sv_rb_is_red(sv_rb_node_t *p_root) {
    return p_root != NULL && p_root->red == 1;
}

sv_rb_node_t *sv_rb_single_rotate(sv_rb_node_t *p_root, int p_dir) {
    sv_rb_node_t *save = p_root->link[!p_dir];
    p_root->link[!p_dir] = save->link[p_dir];
    save->link[p_dir] = p_root;
    p_root->red = 1;
    save->red = 0;
    return save;
}

sv_rb_node_t *sv_rb_double_rotate(sv_rb_node_t *p_root, int p_dir) {
    p_root->link[!p_dir] = sv_rb_single_rotate(p_root->link[!p_dir], !p_dir);
    return sv_rb_single_rotate(p_root, p_dir);
}

void *sv_rb_find(sv_rb_tree_t *tree, void *data) {
    sv_rb_node_t *it = tree->root;
    while (it != NULL) {
        int cmp = tree->on_cmp(it->data, data);
        if (cmp == 0) break;
        it = it->link[cmp < 0];
    }
    return it == NULL ? NULL : it->data;
}

void sv_rb_insert(sv_rb_tree_t *p_tree, void *p_data) {
    if (p_tree->root == NULL) {
        p_tree->root = sv_rb_node_new(p_tree, p_data);
    } else {
        sv_rb_node_t head = {0}, *g, *t, *p, *q;
        int dir = 0, last = 0;
        t = &head;
        g = p = NULL;
        q = t->link[1] = p_tree->root;
        while (1) {
            if (q == NULL) {
                p->link[dir] = q = sv_rb_node_new(p_tree, p_data);
            } else if (sv_rb_is_red(q->link[0]) && sv_rb_is_red(q->link[1])) {
                q->red = 1;
                q->link[0]->red = 0;
                q->link[1]->red = 0;
            }
            if (sv_rb_is_red(q) && sv_rb_is_red(p)) {
                int dir2 = t->link[1] == g;
                if (q == p->link[last]) t->link[dir2] = sv_rb_single_rotate(g, !last);
                else t->link[dir2] = sv_rb_double_rotate(g, !last);
            }
            if (p_tree->on_cmp(q->data, p_data) == 0) break;
            last = dir;
            dir = p_tree->on_cmp(q->data, p_data) < 0;
            if (g != NULL) t = g;
            g = p, p = q;
            q = q->link[dir];
        }
        p_tree->root = head.link[1];
    }
    p_tree->root->red = 0;
    p_tree->size++;
}

void sv_rb_erase(sv_rb_tree_t *p_tree, void *p_data) {
    if (p_tree->root != NULL) {
        sv_rb_node_t head = {0}, *q, *p, *g, *f = NULL;
        int dir = 1;
        q = &head;
        g = p = NULL;
        q->link[1] = p_tree->root;
        while (q->link[dir] != NULL) {
            int last = dir;
            g = p, p = q;
            q = q->link[dir];
            dir = p_tree->on_cmp(q->data, p_data) < 0;
            if (p_tree->on_cmp(q->data, p_data) == 0) f = q;
            if (!sv_rb_is_red(q) && !sv_rb_is_red(q->link[dir])) {
                if (sv_rb_is_red(q->link[!dir])) p = p->link[last] = sv_rb_single_rotate(q, dir);
                else if (!sv_rb_is_red(q->link[!dir])) {
                    sv_rb_node_t *s = p->link[!last];
                    if (s != NULL) {
                        if (!sv_rb_is_red(s->link[!last]) && !sv_rb_is_red(s->link[last])) {
                            p->red = 0;
                            s->red = 1;
                            q->red = 1;
                        } else {
                            int dir2 = g->link[1] == p;
                            if (sv_rb_is_red(s->link[last])) g->link[dir2] = sv_rb_double_rotate(p, last);
                            else if (sv_rb_is_red(s->link[!last])) g->link[dir2] = sv_rb_single_rotate(p, last);
                            q->red = g->link[dir2]->red = 1;
                            g->link[dir2]->link[0]->red = 0;
                            g->link[dir2]->link[1]->red = 0;
                        }
                    }
                }
            }
        }
        if (f != NULL) {
            p_tree->on_free(f->data);
            f->data = q->data;
            p->link[p->link[1] == q] = q->link[q->link[0] == NULL];
            free(q);
        }
        p_tree->root = head.link[1];
        if (p_tree->root != NULL) p_tree->root->red = 0;
        p_tree->size--;
    }
}
