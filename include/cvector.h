#ifndef CVECTOR_H_201505151105
#define CVECTOR_H_201505151105
#ifdef __cplusplus
extern "C" {
#endif

/* {{{
 * =============================================================================
 *      Filename    :   cvector.h
 *      Description :
 *      Created     :   2015-05-15 11:05:10
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */

#include "cobj.h"

struct cvector_s;
struct cvector_iter_s;
struct cvector_node_s;

typedef struct cvector_s
{
    unsigned int size_alloc;
    unsigned int size_offset;
    void **objs;
}cvector;

typedef struct cvector_iter_s
{
    struct cvector_s *v;
    int i;
}cvector_iter;

void cvector_iter_init(cvector_iter *iter, cvector *v, int i);
void cvector_iter_set_null(cvector_iter *iter);
bool cvector_iter_is_end(const cvector_iter *iter);
bool cvector_iter_is_rend(const cvector_iter *iter);
void* cvector_iter_pobj(cvector_iter *iter);
cvector_iter cvector_iter_next(const cvector_iter *iter);
void cvector_iter_to_next(cvector_iter *iter);
cvector_iter cvector_iter_prev(const cvector_iter *iter);
void cvector_iter_to_prev(cvector_iter *iter);

cvector* cvector_new(void);
void cvector_free(cvector *v);
void cvector_clear(cvector *v);
bool cvector_is_empty(const cvector *v);
int  cvector_length(const cvector *v);
int  cvector_size(const cvector *v);
void cvector_print(const cvector *v);

void cvector_insert(cvector *v, int i, void *obj);
void cvector_insert_with_iter(cvector *v, cvector_iter *iter, void *obj);
void cvector_append(cvector *v, void *obj);
void cvector_prepend(cvector *v, void *obj);
void* cvector_pop(cvector *v, cvector_iter *iter);
void* cvector_pop_at(cvector *v, int i);
void* cvector_pop_front(cvector *v);
void* cvector_pop_back(cvector *v);
void* cvector_at(cvector *v, int i);
void* cvector_at_first(cvector *v);
void* cvector_at_last(cvector *v);
cvector_iter cvector_erase(cvector_iter *iter);
cvector_iter cvector_begin(cvector *v);
cvector_iter cvector_end(cvector *v);
cvector_iter cvector_rbegin(cvector *v);
cvector_iter cvector_rend(cvector *v);
void* cvector_first(cvector *v);
void* cvector_front(cvector *v);
void* cvector_last(cvector *v);
int   cvector_indexof(const cvector *v, const void *obj);

void cvector_remove(cvector *v, cvector_iter *iter);
void cvector_remove_at(cvector *v, int i);
void cvector_replace(cvector *v, int i, void *obj);

#ifdef __cplusplus
}
#endif
#endif  /* CVECTOR_H_201505151105 */

