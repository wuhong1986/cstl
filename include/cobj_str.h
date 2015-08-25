#ifndef COBJ_STR_H_201504161404
#define COBJ_STR_H_201504161404
#ifdef __cplusplus
extern "C" {
#endif

/* {{{
 * =============================================================================
 *      Filename    :   cobj_str.h
 *      Description :
 *      Created     :   2015-04-16 14:04:56
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */

#include "cobj.h"

typedef struct cobj_str
{
    COBJ_HEAD_VARS;
    char *val;
}cobj_str;

cobj_str *cobj_str_new(const char *str);
void cobj_str_init(cobj_str *obj, const char *str);
void cobj_str_release(cobj_str *obj);
const char* cobj_str_val(cobj_str *obj);

#ifdef __cplusplus
}
#endif
#endif  /* COBJ_STR_H_201504161404 */

