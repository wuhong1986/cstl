#ifndef CONTAINER_H_201508272308
#define CONTAINER_H_201508272308
#ifdef __cplusplus
extern "C" {
#endif

/* {{{
 * =============================================================================
 *      Filename    :   container.h
 *      Description :
 *      Created     :   2015-08-27 23:08:33
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */

typedef struct iterator_info_s
{
    const char *name;
} iterator_info_t;

typedef enum iterator_dir_e
{
    ITERATOR_DIR_FORWARD = 0,
    ITERATOR_DIR_BACKWARD
} iterator_dir_t;

#define CONTAINER_HEAD_VARS const container_info_t *__container;
#define ITERATOR_HEAD_VARS  const iterator_info_t *__iterator; iterator_dir_t __iterator_dir;

void iterator_to_next(void *iter);
void iterator_to_prev(void *iter);
void iterator_free(void *iter);

void* container_begin(void *contanier);
void* container_rbegin(void *contanier);

#ifdef __cplusplus
}
#endif
#endif  /* CONTAINER_H_201508272308 */

