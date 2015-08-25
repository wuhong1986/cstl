/* {{{
 * =============================================================================
 *      Filename    :   cobj_str.c
 *      Description :
 *      Created     :   2015-04-16 14:04:39
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <string.h>
#include "cobj_str.h"
#include "murmurhash.h"

const char* obj_to_str(const void *obj)
{
    return obj ? (const char*)((cobj_str*)(obj))->val : "<null>";
}

static int cobj_str_fprint(const void *obj, FILE *pfile)
{
    return fprintf(pfile, "%s", obj_to_str(obj));
}

static int cobj_str_cmp(const void *obj1, const void *obj2)
{
    if(!obj1 && !obj2) return 0;
    if(!obj1) return -1;
    if(!obj2) return 1;

    return strcmp(obj_to_str(obj1), obj_to_str(obj2));
}

static void *cobj_str_dup(const void *obj)
{
    return (void*)cobj_str_new((const char *)obj);
}

static void cobj_str_free(void *obj)
{
    cobj_str *str = (cobj_str*)obj;
    if(str && str->val){
        free(str->val);
    }
}

uint32_t cobj_str_hash(const void *obj)
{
    return murmurhash(((cobj_str*)obj)->val, strlen(((cobj_str*)obj)->val));
}

cobj_ops_t cobj_ops_str = {
    .name = "str",
    .obj_size = sizeof(const char*),
    .cb_print = cobj_str_fprint,
    .cb_dup = cobj_str_dup,
    .cb_destructor = cobj_str_free,
    .cb_cmp = cobj_str_cmp,
    .cb_hash = cobj_str_hash,
};

cobj_str *cobj_str_new(const char *str)
{
    cobj_str *obj_str = (cobj_str*)malloc(sizeof(cobj_str));

    cobj_str_init(obj_str, str);

    return obj_str;
}

const char* cobj_str_val(cobj_str *obj)
{
    return ((const char*)obj->val);
}

void cobj_str_init(cobj_str *obj, const char *str)
{
    cobj_set_ops(obj, &cobj_ops_str);

    obj->val = strdup(str);
}

void cobj_str_release(cobj_str *obj)
{
    if(obj->val) {
        free(obj->val);
        obj->val = NULL;
    }
}

