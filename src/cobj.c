/* {{{
 * =============================================================================
 *      Filename    :   void.c
 *      Description :
 *      Created     :   2015-04-16 09:04:32
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <stdlib.h>
#include <memory.h>
#include "cobj.h"
#include "cstring.h"
#include "murmurhash.h"

#ifdef DEBUG_COBJ
static int cnt_objs = 0;
#endif

typedef struct cobj
{
    COBJ_HEAD_VARS;
} cobj;

#define COBJ(obj) ((cobj*)(obj))

void cobj_set_ops(void *obj, const cobj_ops_t *ops)
{
    COBJ(obj)->ops = ops;
}

int cobj_fprint(const void *obj, FILE *pfile)
{
    if(obj == NULL) {
        return fprintf(pfile, "<NULL>");
    } else if(COBJ(obj)->ops->cb_print) {
        return COBJ(obj)->ops->cb_print(obj, pfile);
    } else {
        return fprintf(pfile, "< void at 0x%lX, Type \"%s\" >",
                       (unsigned long)obj, COBJ(obj)->ops->name);
    }
}

int cobj_print(const void *obj)
{
    return cobj_fprint(obj, stdout);
}

struct cstring_s *cobj_to_cstr(const void *obj)
{
    cstr *str = NULL;

    if(COBJ(obj)->ops->cb_cstr) {
        return COBJ(obj)->ops->cb_cstr(obj);
    } else {
        str = cstr_new_with_format("< void at 0x%lX, Type \"%s\" >",
                                   (unsigned long)obj, COBJ(obj)->ops->name);
        return str;
    }
}

void *cobj_dup_callback_default(const void *obj)
{
    void *val_dup = NULL;
    size_t obj_size = 0;

    obj_size = COBJ(obj)->ops->obj_size;

    if(0 == obj_size) {
        printf("[void][DUP] Type:%s duplicate size == 0",
               COBJ(obj)->ops->name);
    }

    val_dup = malloc(obj_size);
    memcpy(val_dup, obj, obj_size);

    return (void*)val_dup;
}

void *cobj_dup(const void *obj)
{
    if(COBJ(obj)->ops->cb_dup) {
        return COBJ(obj)->ops->cb_dup(obj);
    } else {
        return cobj_dup_callback_default(obj);
    }
}

void cobj_destory(void *obj)
{
#ifdef DEBUG_COBJ
    --cnt_objs;
    printf("[void][FREE] Type:%s ", COBJ(obj)->ops->name);
    cobj_print(obj);
    printf(", object counts:%d\n", cnt_objs);
#endif

    if(COBJ(obj)->ops->cb_destructor) {
        COBJ(obj)->ops->cb_destructor(obj);
    }
}

void cobj_free(void *obj)
{
    cobj_destory(obj);
    free(obj);
}

uint32_t cobj_hash(const void *obj)
{
    if(COBJ(obj)->ops->cb_hash) {
        return COBJ(obj)->ops->cb_hash(obj);
    } else {
#if 0
        printf("\n");
        cobj_print(obj);
        int i = 0;
        for(i = 0; i < cobj_size(obj); ++i) {
            printf(" %02X ", ((const unsigned char*)obj)[i]);
        }
        printf("\n");
#endif
        return murmurhash((const char*)obj, cobj_size(obj));
    }
}

int  cobj_cmp(const void *obj1, const void *obj2)
{
    if(COBJ(obj1)->ops->cb_cmp && COBJ(obj2)->ops->cb_cmp
    && COBJ(obj1)->ops->cb_cmp == COBJ(obj2)->ops->cb_cmp) {
        return COBJ(obj1)->ops->cb_cmp(obj1, obj2);
    } else {
        return (unsigned long)obj1 - (unsigned long)obj2;
    }
}

bool cobj_equal(const void *obj1, const void *obj2)
{
    return cobj_cmp(obj1, obj2) == 0;
}

int cobj_size(const void *obj)
{
    return COBJ(obj)->ops->obj_size;
}
