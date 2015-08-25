#ifndef COBJ_H_201504160804
#define COBJ_H_201504160804
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "cstring.h"

typedef void*   (*cobj_cb_dup)(const void *obj);
typedef void    (*cobj_cb_destructor)(void *obj);
typedef int     (*cobj_cb_cmp)(const void *obj1, const void *obj2);
typedef int     (*cobj_cb_fprint)(const void *obj, FILE *pfile);
typedef cstr*   (*cobj_cb_cstr)(const void *obj);
typedef int     (*cobj_cb_memsize)(const void *obj);
typedef uint32_t (*cobj_cb_hash)(const void *obj);

typedef struct cobj_ops_s
{
    const char *name;
    int  obj_size;
    cobj_cb_fprint      cb_print;
    cobj_cb_dup         cb_dup;
    cobj_cb_destructor  cb_destructor;
    cobj_cb_cmp         cb_cmp;
    cobj_cb_cstr        cb_cstr;
    cobj_cb_memsize     cb_memsize;
    cobj_cb_hash        cb_hash;
}cobj_ops_t;

#define COBJ_HEAD_VARS const cobj_ops_t *ops

void  cobj_set_ops(void *obj, const cobj_ops_t *ops);
void* cobj_dup(const void *obj);
cstr* cobj_to_cstr(const void *obj);
int cobj_print(const void *obj);
int cobj_size(const void *obj);
int cobj_fprint(const void *obj, FILE *pfile);
void cobj_destory(void *obj);
void cobj_free(void *obj);
uint32_t cobj_hash(const void *obj);
int  cobj_cmp(const void *obj1, const void *obj2);
bool cobj_equal(const void *obj1, const void *obj2);

#ifdef __cplusplus
}
#endif
#endif  /* COBJ_H_201504160804 */

