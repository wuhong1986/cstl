#ifndef COBJ_INT_H_201504160904
#define COBJ_INT_H_201504160904
#ifdef __cplusplus
extern "C" {
#endif

/* {{{
 * =============================================================================
 *      Filename    :   cobj_int.h
 *      Description :
 *      Created     :   2015-04-16 09:04:56
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */

#include "cobj.h"

typedef struct cobj_int
{
    COBJ_HEAD_VARS;

    int val;
}cobj_int;

void  cobj_int_init(cobj_int *obj, int val);
cobj_int* cobj_int_new(int val);

int cobj_int_val(cobj_int *obj);

#ifdef __cplusplus
}
#endif
#endif  /* COBJ_INT_H_201504160904 */

