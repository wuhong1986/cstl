/* {{{
 * =============================================================================
 *      Filename    :   test_main.c
 *      Description :
 *      Created     :   2015-08-22 21:08:25
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */

#include "CUnit/Console.h"

extern void add_test_clist(void);
extern void add_test_chash(void);
extern void add_test_cvector(void);

int main(int argc, char *argv[])
{
    CU_initialize_registry();

    add_test_clist();
    add_test_chash();
    add_test_cvector();

    CU_console_run_tests();

    CU_cleanup_registry();

    return 0;
}

