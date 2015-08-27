/* {{{
 * =============================================================================
 *      Filename    :   clist.c
 *      Description :
 *      Created     :   2014-08-25 15:08:30
 *      Author      :    Wu Hong
 * =============================================================================
 }}} */

#include "clist.h"

clist_node *clist_node_new(void *val)
{
    clist_node *self = NULL;

    self = (clist_node*)malloc(sizeof(clist_node));
    if(NULL == self) return NULL;

    self->val = val;
    self->next = self->prev = NULL;

    return self;
}

void clist_node_free(clist_node *node)
{
    cobj_free(node->val);
    free(node);
}

void* clist_iter_obj(clist_iter *iter)
{
    return iter->node ? iter->node->val : NULL;
}

void clist_iter_init(clist_iter *iter, clist *list,
                     clist_node *node, clist_iter_dir dir)
{
    iter->list = list;
    iter->node = node;
    iter->dir  = dir;
}

clist_iter clist_begin(clist *list)
{
    clist_iter iter;

    clist_iter_init(&iter, list, list->head, CLIST_ITER_DIR_FORWORD);

    return iter;
}

clist_iter clist_rbegin(clist *list)
{
    clist_iter iter;

    clist_iter_init(&iter, list, list->tail, CLIST_ITER_DIR_BACKWORD);

    return iter;
}

clist_iter clist_end(clist *list)
{
    clist_iter iter;

    clist_iter_init(&iter, list, NULL, CLIST_ITER_DIR_FORWORD);

    return iter;
}

void clist_iter_to_next(clist_iter *iter)
{
    if(iter->node) {
        if(CLIST_ITER_DIR_FORWORD == iter->dir){
            iter->node = iter->node->next;
        } else {
            iter->node = iter->node->prev;
        }
    }
}

void clist_iter_to_prev(clist_iter *iter)
{
    if(iter->node) {
        if(CLIST_ITER_DIR_FORWORD == iter->dir){
            iter->node = iter->node->prev;
        } else {
            iter->node = iter->node->next;
        }
    }
}

bool clist_iter_is_end(clist_iter *iter)
{
    return iter->node == NULL;
}

unsigned int  clist_len(const clist *list)
{
    return list ? list->len : 0;
}

unsigned int  clist_size(const clist *list)
{
    return clist_len(list);
}

unsigned int  clist_count(const clist *list)
{
    return clist_len(list);
}

bool clist_is_empty(const clist *list)
{
    return clist_len(list) == 0;
}

void* clist_begin_obj(clist *list)
{
    return list->head ? list->head->val : NULL;
}

void* clist_last_obj(clist *list)
{
    return list->tail ? list->tail->val : NULL;
}

/*
 * Allocate a new clist. NULL on failure.
 */
clist* clist_new(void)
{
    clist *list = (clist*)calloc(1, sizeof(clist));

#ifdef CLIST_ENABLE_SEM
    list->sem = cmutex_new();
#endif

    return list;
}

void clist_print(const clist* list)
{
    clist_iter iter = clist_begin((clist*)list);
    void *obj  = NULL;

    printf("[");
    clist_iter_foreach_obj(&iter, obj) {
        cobj_print(obj);
        /* if(clist_iter_is_last(&iter)) { */
            printf(", ");
        /* } */
    }
    printf("]");
}

#ifdef CLIST_ENABLE_SEM
void clist_lock(clist *list)
{
    csem_lock(list->sem);
}

int clist_lock_timed(clist *list, int ms)
{
    return csem_lock_timed(list->sem, ms);
}

void clist_unlock(clist *list)
{
    csem_unlock(list->sem);
}
#endif

void clist_clear(clist *list)
{
    unsigned int len = list->len;
    clist_node *next = NULL;
    clist_node *node = list->head;

    while (len--) {
        next = node->next;

        clist_node_free(node);

        node = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->len  = 0;
}

/*
 * Free the list.
 */
void clist_free(clist *list)
{
    if(list){
        clist_clear(list);
#ifdef CLIST_ENABLE_SEM
        csem_free(list->sem);
#endif
        free(list);
    }
}

/*
 * Prepend the given node to the list
 * and return the node, NULL on failure.
 */

static void clist_push_front(clist *list, clist_node *node)
{
    if (!node) return;

    if (list->len) {
        node->next       = list->head;
        node->prev       = NULL;
        list->head->prev = node;
        list->head       = node;
    } else {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    }

    ++list->len;
}

/*
 * Append the given node to the list
 * and return the node, NULL on failure.
 */
static void clist_push_back(clist *list, clist_node *node)
{
    if(!node) return ;

    if(list->len) {
        node->prev       = list->tail;
        node->next       = NULL;
        list->tail->next = node;
        list->tail       = node;
    } else {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    }

    ++list->len;
}

static void* clist_detach_node(clist *list, clist_node *node)
{
    void *obj = NULL;

    if(node){
        node->prev ? (node->prev->next = node->next) : (list->head = node->next);
        node->next ? (node->next->prev = node->prev) : (list->tail = node->prev);

        --list->len;

        obj = node->val;
        free(node);
    }

    return obj;
}

void* clist_pop_back(clist *list)
{
    if(clist_is_empty(list)) return NULL;

    return clist_detach_node(list, list->tail);
}

void* clist_pop_front(clist *list)
{
    if(clist_is_empty(list)) return NULL;
    return clist_detach_node(list, list->head);
}

void* clist_pop(const clist_iter *iter)
{
    if(clist_is_empty(iter->list)) return NULL;
    return clist_detach_node(iter->list, iter->node);
}

void clist_append(clist *list, void *obj)
{
    clist_push_back(list, clist_node_new(obj));
}

void clist_prepend(clist *list, void *obj)
{
    clist_push_front(list, clist_node_new(obj));
}

/*
 * Return the node associated to val or NULL.
 */

clist_iter clist_find(clist *list, const void *obj)
{
    clist_iter iter     = clist_begin(list);
    void       *obj_cmp = NULL;

    clist_iter_foreach_obj(&iter, obj_cmp) {
        if(cobj_equal(obj, obj_cmp)){
            break;
        }
    }

    return iter;
}

void* clist_find_obj(clist *list, const void *obj)
{
    clist_iter iter = clist_find(list, obj);

    return clist_iter_obj(&iter);
}

bool clist_find_then_remove(clist *list, const void *obj)
{
    clist_iter iter = clist_find(list, obj);

    if(!clist_iter_is_end(&iter)) {
        clist_remove(&iter);
        return true;
    } else {
        return false;
    }
}

#if 0
clist_iter clist_find_with_cb(clist *list, cobj_cb_is_match cb, void *arg)
{
    clist_iter iter = clist_begin(list);
    void       *obj = NULL;

    clist_iter_foreach_obj(&iter, obj) {
        if(cb(obj, arg)){
            break;
        }
    }

    return iter;
}

void* clist_find_obj_with_cb(clist *list, cobj_cb_is_match cb, void *arg)
{
    clist_iter iter = clist_find_with_cb(list, cb, arg);

    return clist_iter_obj(&iter);
}
bool clist_find_then_remove_with_cb(clist *list, cobj_cb_is_match cb, void *arg)
{
    clist_iter iter = clist_find_with_cb(list, cb, arg);

    if(!clist_iter_is_end(&iter)) {
        clist_remove(&iter);
        return true;
    } else {
        return false;
    }
}
#endif

/*
 * Return the node at the given index or NULL.
 */

clist_iter clist_at(clist *list, int index)
{
    clist_iter iter;
    clist_node *node = list->head;
    int        idx   = index;

    if(index < 0) {
        idx = clist_size(list) + index;
    }

    if(idx >= 0 && idx < (int)list->len) {
        while(idx) { node = node->next; --idx; }
    } else {
        node = NULL;
    }

    clist_iter_init(&iter, list, node, CLIST_ITER_DIR_FORWORD);

    return iter;
}

void* clist_at_obj(clist *list, int index)
{
    clist_iter iter = clist_at(list, index);
    return clist_iter_obj(&iter);
}

/*
 * Remove the given node from the list, freeing it and it's value.
 */

void clist_remove(clist_iter *iter)
{
    clist *list = iter->list;
    clist_node *node = iter->node;

    if(!list || !node) return;

    clist_detach_node(list, node);

    clist_node_free(node);
}

void clist_remove_at(clist *list, int index)
{
    clist_iter iter = clist_at(list, index);

    clist_remove(&iter);
}

void clist_remove_first(clist *list)
{
    clist_remove_at(list, 0);
}

void clist_remove_last(clist *list)
{
    clist_remove_at(list, -1);
}
