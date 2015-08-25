#ifndef CSEM_H_201409042109
#define CSEM_H_201409042109
#ifdef __cplusplus
extern "C" {
#endif

/* {{{
 * =============================================================================
 *      Filename    :   csem.h
 *      Description :
 *      Created     :   2014-09-04 21:09:38
 *      Author      :    Wu Hong
 * =============================================================================
 }}} */
#include <stdbool.h>

typedef struct csem_s csem;
#define cmutex csem

csem* csem_new(int value);
csem* cmutex_new(void);
void  csem_free(csem *sem);

int  csem_lock(csem *sem);
int  csem_acquire(csem *sem, int n);
void csem_unlock(csem *sem);
int  csem_lock_timed(csem *sem, int msec);
bool csem_try_lock(csem *sem);
bool csem_try_acquire(csem *sem, int n);
void csem_release(csem *sem, int n);

#define csem_down       csem_lock
#define csem_up         csem_unlock
#define cmutex_free     csem_free
#define cmutex_lock     csem_lock
#define cmutex_unlock   csem_unlock

/**
 * @Brief  Returns the number of resources currently available to the semaphore.
 *         This number can never be negative.
 *
 * @Param sem
 *
 * @Returns
 */
int csem_getvalue(csem *sem);

#ifdef __cplusplus
}
#endif
#endif  /* CSEM_H_201409042109 */
