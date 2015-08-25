#ifndef CCLIST_H_201408251408
#define CCLIST_H_201408251408
#ifdef __cplusplus
extern "C" {
#endif

/* {{{
 * =============================================================================
 *      Filename    :   clist.h
 *      Description :
 *      Created     :   2014-08-25 14:08:22
 *      Author      :    Wu Hong
 * =============================================================================
 }}} */

// #define CLIST_ENABLE_SEM

#include <stdlib.h>
#include <stdbool.h>
#include "cobj.h"
#ifdef CLIST_ENABLE_SEM
#include "csem.h"
#endif

typedef struct clist_node {
    void *val;

    struct clist_node *prev;
    struct clist_node *next;
}clist_node;

typedef struct clist {
#ifdef CLIST_ENABLE_SEM
    csem   *sem;
#endif
    unsigned int len;

    clist_node *head;
    clist_node *tail;
}clist;

typedef enum clist_iter_dir {
    CLIST_ITER_DIR_FORWORD = 0,
    CLIST_ITER_DIR_BACKWORD
} clist_iter_dir;

typedef struct clist_iter {
    clist_iter_dir  dir;
    clist           *list;
    clist_node      *node;
}clist_iter;

#define clist_foreach(list, node)                           \
    for(node = list->head; node; node = node->next)
#define clist_foreach_tail(list, node)                      \
    for(node = list->tail; node; node = node->prev)

#define clist_foreach_val(list, node, val_ptr)              \
    for(node = list->head;                                  \
        (val_ptr = NULL, node) && (val_ptr = node->val, 1); \
        node = node->next)
#define clist_foreach_val_tail(list, node, val_ptr)         \
    for(node = list->tail;                                  \
        (val_ptr = NULL, node) && (val_ptr = node->val, 1); \
        node = node->prev)

#define clist_iter_foreach(iter_ptr) \
    for(; !clist_iter_is_end(iter_ptr); clist_iter_to_next(iter_ptr))
#define clist_iter_foreach_obj(iter_ptr, obj_ptr)           \
    for(;    !clist_iter_is_end(iter_ptr) \
          && (obj_ptr = clist_iter_obj(iter_ptr), 1); \
        clist_iter_to_next(iter_ptr))

/*
 * clist iterator interface
 */
void* clist_iter_obj(clist_iter *iter);
void  clist_iter_to_next(clist_iter *iter);
void  clist_iter_to_prev(clist_iter *iter);
bool  clist_iter_is_end(clist_iter *iter);

/* ==========================================================================
 *        clist interface
 * ========================================================================== */
clist* clist_new(void);
void clist_print(const clist* list);

bool clist_is_empty(const clist *list);
unsigned int clist_size(const clist *list);
unsigned int clist_count(const clist *list);
unsigned int clist_len(const clist *list);

#ifdef CLIST_ENABLE_SEM
void clist_lock(clist *list);
int  clist_lock_timed(clist *list, int ms);
void clist_unlock(clist *list);
#endif

void clist_append(clist *list, void *obj);
void clist_prepend(clist *list, void *obj);
void* clist_pop_back(clist *list);
void* clist_pop_front(clist *list);
void* clist_pop(const clist_iter *iter);

clist_iter clist_find(clist *list, const void *obj);
void* clist_find_obj(clist *list, const void *obj);
bool clist_find_then_remove(clist *list, const void *obj);
#if 0
clist_iter clist_find_with_cb(clist *list, cobj_cb_is_match cb, void *arg);
void* clist_find_obj_with_cb(clist *list, cobj_cb_is_match cb, void *arg);
bool clist_find_then_remove_with_cb(clist *list, cobj_cb_is_match cb, void *arg);
#endif

clist_iter clist_at(clist *list, int index);
clist_iter clist_begin(clist *list);
clist_iter clist_rbegin(clist *list);
clist_iter clist_end(clist *list);
void* clist_at_obj(clist *list, int index);
void* clist_begin_obj(clist *list);
void* clist_last_obj(clist *list);

void clist_move(clist *list, int from, int to);
void clist_swap(clist *list, int i, int j);
void clist_sort_asc(clist *list);   /* sort list by order asc */
void clist_sort_desc(clist *list);

void clist_remove(clist_iter *iter);
void clist_remove_at(clist *list, int index);
void clist_remove_first(clist *list);
void clist_remove_last(clist *list);
void clist_free(clist *list);
void clist_clear(clist *list);

#ifdef __cplusplus
}
#endif
#endif  /* Cclist_H_201408251408 */
