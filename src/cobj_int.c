/* {{{
 * =============================================================================
 *      Filename    :   cobj_int.c
 *      Description :
 *      Created     :   2015-04-16 09:04:58
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <memory.h>
#include "cobj_int.h"
#include "murmurhash.h"

static inline int cobj_to_int(const void *obj)
{
    return ((cobj_int*)obj)->val;
}

int cobj_int_val(cobj_int *obj)
{
    return obj->val;
}

int cobj_int_fprint(const void *obj, FILE *pfile)
{
    return fprintf(pfile, "%d", cobj_to_int(obj));
}

int cobj_int_cmp(const void *obj1, const void *obj2)
{
    return cobj_to_int(obj1) - cobj_to_int(obj2);
}

void *cobj_int_dup(const void *obj)
{
    return (void*)cobj_int_new(cobj_to_int(obj));
}

uint32_t cobj_int_hash(const void *obj)
{
    return murmurhash((const char*)(&((cobj_int*)obj)->val), sizeof(int));
}

cobj_ops_t cobj_ops_int = {
    .name = "int",
    .obj_size = sizeof(cobj_int),
    .cb_print = cobj_int_fprint,
    .cb_dup = cobj_int_dup,
    .cb_cmp = cobj_int_cmp,
    .cb_hash = cobj_int_hash,
};

cobj_int *cobj_int_new(int val)
{
    cobj_int *obj = (cobj_int*)malloc(sizeof(cobj_int));

    cobj_int_init(obj, val);

    return obj;
}

void  cobj_int_init(cobj_int *obj, int val)
{
    /* memset(obj, 0, sizeof(cobj_int)); */
    cobj_set_ops(obj, &cobj_ops_int);
    obj->val = val;
}
