/* {{{
 * =============================================================================
 *      Filename    :   cvector.c
 *      Description :
 *      Created     :   2015-05-15 11:05:08
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <stdlib.h>
#include <string.h>
#include "cvector.h"

#define CVECTOR_CHECK_IDX_FULL(v, i, ret) \
    do {    \
        if((i) < 0 || (i) >= ((v)->size_offset)){ \
            printf("[CVECTOR]index(%d) out of range(%d)\n", i, (v)->size_offset);\
            return ret; \
        }   \
    }while(0)

#define CVECTOR_CHECK_IDX(v, i) CVECTOR_CHECK_IDX_FULL(v, i, )
#define CVECTOR_CHECK_IDX_RETURN_NULL(v, i) CVECTOR_CHECK_IDX_FULL(v, i, NULL)

#define CVECTOR_OBJ(v, i) ((v)->objs[i])

/*********************************************************************
 *                          Vector Iterator                          *
 *********************************************************************/
void cvector_iter_init(cvector_iter *iter, cvector *v, int i)
{
    iter->v = v;
    iter->i = i;
}

void cvector_iter_set_null(cvector_iter *iter)
{
    iter->v = NULL; iter->i = -1;
}

bool cvector_iter_is_end(const cvector_iter *iter)
{
    return iter->i >= cvector_size(iter->v);
}

bool cvector_iter_is_rend(const cvector_iter *iter)
{
    return iter->i < 0;
}

void* cvector_iter_pobj(cvector_iter *iter)
{
    if(iter->i >= 0 && iter->i < cvector_size(iter->v)) {
        return CVECTOR_OBJ(iter->v, iter->i);
    } else {
        return NULL;
    }
}

cvector_iter cvector_iter_next(const cvector_iter *iter)
{
    cvector_iter iter_next;

    iter_next.v = iter->v;
    iter_next.i = iter->i + 1;

    return iter_next;
}

void cvector_iter_to_next(cvector_iter *iter)
{
    ++(iter->i);
}

cvector_iter cvector_iter_prev(const cvector_iter *iter)
{
    cvector_iter iter_next;

    iter_next.v = iter->v;
    iter_next.i = iter->i - 1;

    return iter_next;
}

void cvector_iter_to_prev(cvector_iter *iter)
{
    --(iter->i);
}

cvector* cvector_new(void)
{
    cvector *v = (cvector*)malloc(sizeof(cvector));

    v->size_offset = 0;
    v->size_alloc = 16;
    v->objs = (void**)malloc(sizeof(void*) * v->size_alloc);

    memset(v->objs, 0, sizeof(void*) * v->size_alloc);

    return v;
}

static void cvector_resize(cvector *v)
{
    v->size_alloc *= 2;
    v->objs = (void**)realloc(v->objs, sizeof(void*) * v->size_alloc);
}

void cvector_free(cvector *v)
{
    cvector_clear(v);
    free(v->objs);
    free(v);
}

void cvector_clear(cvector *v)
{
    int i = 0;
    for(i = 0; i < v->size_offset; ++i) {
        cobj_destory(CVECTOR_OBJ(v, i));
    }
    v->size_offset = 0;
}

bool cvector_is_empty(const cvector *v)
{
    return v->size_offset <= 0;
}

int  cvector_length(const cvector *v)
{
    return v->size_offset;
}

int  cvector_size(const cvector *v)
{
    return v->size_offset;
}

void cvector_print(const cvector *v)
{
    int i = 0;
    printf("[");
    for(i = 0; i < v->size_offset; ++i) {
        cobj_print(CVECTOR_OBJ(v, i));
        if(i != v->size_offset - 1) {
            printf(", ");
        }
    }
    printf("]");
}

void cvector_remove(cvector *v, cvector_iter *iter)
{
    return cvector_remove_at(v, iter->i);
}

static void cvector_detach_at(cvector *v, int i)
{
    int idx = 0;

    for(idx = i; idx < v->size_offset - 1; ++idx) {
        v->objs[idx] = v->objs[idx + 1];
    }
    --(v->size_offset);
}

void cvector_remove_at(cvector *v, int i)
{
    void *obj = NULL;

    CVECTOR_CHECK_IDX(v, i);

    obj = CVECTOR_OBJ(v, i);

    cvector_detach_at(v, i);

    cobj_destory(obj);
}

void cvector_replace(cvector *v, int i, void *obj)
{
    void *obj_old = NULL;

    CVECTOR_CHECK_IDX(v, i);

    obj_old = CVECTOR_OBJ(v, i);
    v->objs[i] = obj;
    cobj_destory(obj_old);
}

void cvector_insert(cvector *v, int i, void *obj)
{
    int idx = 0;
    int pos = i;

    if(pos < 0) { pos += cvector_length(v); }

    if(pos < 0) { pos = 0; }
    else if(pos > cvector_length(v)) { pos = cvector_length(v); }

    /* check is need adjust size */
    if(v->size_offset + 1 >= v->size_alloc) {
        cvector_resize(v);
    }

    for(idx = v->size_offset - 1; idx >= i; --idx) {
        v->objs[idx + 1] = v->objs[idx];
    }
    v->objs[i] = obj;
    ++(v->size_offset);
}

void cvector_insert_with_iter(cvector *v, cvector_iter *iter, void *obj)
{
    cvector_insert(v, iter->i, obj);
}

void cvector_append(cvector *v, void *obj)
{
    /* check is need adjust size */
    if(v->size_offset + 1 >= v->size_alloc) {
        cvector_resize(v);
    }

    v->objs[v->size_offset++] = obj;
}

void cvector_prepend(cvector *v, void *obj)
{
    int idx = 0;

    /* check is need adjust size */
    if(v->size_offset + 1 >= v->size_alloc) {
        cvector_resize(v);
    }

    for(idx = v->size_offset - 1; idx >= 0; --idx) {
        v->objs[idx + 1] = v->objs[idx];
    }
    v->objs[0] = obj;
    ++(v->size_offset);
}

void* cvector_pop_at(cvector *v, int i)
{
    void *obj = CVECTOR_OBJ(v, i);

    cvector_detach_at(v, i);

    return obj;
}

void* cvector_pop(cvector *v, cvector_iter *iter)
{
    return cvector_pop_at(v, iter->i);
}

void* cvector_pop_front(cvector *v)
{
    return cvector_pop_at(v, 0);
}

void* cvector_pop_back(cvector *v)
{
    return cvector_pop_at(v, v->size_offset - 1);
}

void* cvector_at(cvector *v, int i)
{
    CVECTOR_CHECK_IDX_RETURN_NULL(v, i);

    return CVECTOR_OBJ(v, i);
}

void*  cvector_at_first(cvector *v)
{
    return CVECTOR_OBJ(v, 0);
}

void*  cvector_at_last(cvector *v)
{
    return CVECTOR_OBJ(v, v->size_offset - 1);
}
