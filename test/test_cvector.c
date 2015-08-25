/* {{{
 * =============================================================================
 *      Filename    :   test_cvector.c
 *      Description :
 *      Created     :   2015-05-15 14:05:24
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include "CUnit/Console.h"
#include "cobj_int.h"
#include "cobj_str.h"
#include "cvector.h"

void test_cvector(void)
{
    cvector *v = cvector_new();
    int test_cnt = 10000;
    int i = 0;

    for(i = 0; i < test_cnt; ++i) {
        cvector_append(v, cobj_int_new(i));
    }

    for(i = 0; i < test_cnt; ++i) {
        CU_ASSERT(i == cobj_int_val(cvector_at(v, i)));
    }

    cvector_clear(v);
    for(i = 0; i < test_cnt; ++i) {
        cvector_prepend(v, cobj_int_new(i));
    }

    for(i = 0; i < test_cnt; ++i) {
        CU_ASSERT(test_cnt - i - 1 == cobj_int_val(cvector_at(v, i)));
    }

    cvector_free(v);
}

void add_test_cvector(void)
{
    CU_pSuite pSuite = NULL;

	pSuite = CU_add_suite("test_cvector", NULL, NULL);

    CU_add_test(pSuite, "test_cvector", test_cvector);
}


