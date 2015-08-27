/* {{{
 * =============================================================================
 *      Filename    :   test_ex_file.c
 *      Description :
 *      Created     :   2015-08-23 18:08:38
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <stdlib.h>
#include <CUnit/Console.h>
#include "clist.h"
#include "cobj_int.h"

void test_clist(void)
{
    /* CU_ASSERT(NULL == ex_path_normalize(NULL)); */
    int i = 0;
    int test_cnt = 10000;
    clist *list = clist_new();

    CU_ASSERT(0 == clist_len(list));
    for(i = 0; i < test_cnt; ++i) {
        clist_append(list, cobj_int_new(i));
    }

    CU_ASSERT(test_cnt == clist_len(list));
    CU_ASSERT(false == clist_is_empty(list));
    CU_ASSERT(0 == cobj_int_val(clist_at_obj(list, 0)));
    CU_ASSERT(1 == cobj_int_val(clist_at_obj(list, 1)));
    CU_ASSERT(test_cnt - 1 == cobj_int_val(clist_at_obj(list, test_cnt - 1)));

    for(i = 0; i < test_cnt; ++i) {
        void *obj = clist_pop_front(list);
        CU_ASSERT(i == cobj_int_val(obj));
        cobj_free(obj);
    }
    CU_ASSERT(0 == clist_len(list));
    CU_ASSERT(true == clist_is_empty(list));

    for(i = 0; i < test_cnt; ++i) {
        clist_append(list, cobj_int_new(i));
    }
    clist_clear(list);
    CU_ASSERT(0 == clist_len(list));
    for(i = 0; i < test_cnt; ++i) {
        clist_append(list, cobj_int_new(i));
    }

    clist_node *node = NULL;
    void *obj = NULL;
    i = 0;
    clist_foreach_val(list, node, obj) {
        CU_ASSERT(i == cobj_int_val(obj));
        ++i;
    }

    clist_iter iter = clist_begin(list);
    i = 0;
    clist_iter_foreach(&iter) {
        obj = clist_iter_obj(&iter);
        CU_ASSERT(i == cobj_int_val(obj));
        ++i;
    }

    iter = clist_rbegin(list);
    i = 0;
    clist_iter_foreach_obj(&iter, obj) {
        /* printf("%d - %d",test_cnt - i - 1, cobj_int_val(obj)); */
        CU_ASSERT(test_cnt - i - 1 == cobj_int_val(obj));
        ++i;
    }

    clist_free(list);
}

void add_test_clist(void)
{
    CU_pSuite suite = NULL;

    suite = CU_add_suite("clist", NULL, NULL);
    CU_add_test(suite, "test_clist", test_clist);
}

