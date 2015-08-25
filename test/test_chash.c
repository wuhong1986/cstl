/* {{{
 * =============================================================================
 *      Filename    :   test_chash.c
 *      Description :
 *      Created     :   2015-08-21 13:08:45
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */

#include "CUnit/Console.h"
#include "chash.h"
#include "cobj_str.h"
#include "cobj_int.h"

void test_chash_int()
{
    int i = 0;
    /* int test_cnt = 1000; */
    int test_cnt = 10;
    char str[32];
    chash *hash = chash_new();

    for(i = 0; i < test_cnt; ++i) {
        sprintf(str, "str%d", i);
        chash_int_set(hash, i, cobj_str_new(str));
    }

    /* chash_printf_test(stdout, hash); */

    for(i = 0; i < test_cnt; ++i) {
        sprintf(str, "str%d", i);
        cobj_str *obj = (cobj_str*)chash_int_get(hash, i);
        /* printf("i:%d val:%s\n", i, obj ? cobj_str_val(obj) : "NULL"); */
	    CU_ASSERT(obj != NULL && strcmp(cobj_str_val(obj), str) == 0);
    }

    chash_free(hash);
}

void test_chash_str()
{
    int i = 0;
    /* int test_cnt = 1000; */
    int test_cnt = 10;
    char str[32];
    chash *hash = chash_new();

    for(i = 0; i < test_cnt; ++i) {
        sprintf(str, "str%d", i);
        chash_str_set(hash, str, cobj_int_new(i));
    }

    for(i = 0; i < test_cnt; ++i) {
        sprintf(str, "str%d", i);
        cobj_int *obj = (cobj_int*)chash_str_get(hash, str);
	    CU_ASSERT(obj != NULL && cobj_int_val((cobj_int*)obj) == i);
    }

    chash_free(hash);
}

void add_test_chash(void)
{
    CU_pSuite pSuite = NULL;

	pSuite = CU_add_suite("test_chash", NULL, NULL);

    CU_add_test(pSuite, "test_chash_int", test_chash_int);
    CU_add_test(pSuite, "test_chash_str", test_chash_str);
}



