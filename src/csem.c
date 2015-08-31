/* {{{
 * =============================================================================
 *      Filename    :   csem_unix.c
 *      Description :
 *      Created     :   2014-09-04 21:09:48
 *      Author      :    Wu Hong
 * =============================================================================
 }}} */
#ifdef WIN32

#include <windows.h>
#include <stdio.h>
#include "csem.h"
struct csem_s
{
    HANDLE sem;
};

#else
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include "csem.h"

struct csem_s
{
    sem_t sem;
};
#endif

csem* csem_new(int value)
{
    csem *sem = (csem*)malloc(sizeof(struct csem_s));
    if(sem) {
#ifdef WIN32
        sem->sem = CreateSemaphore(NULL, /* default security attributes */
                                   value,/* 初始化的信号量 */
                                   0xFFFFFFF,/* 是允许信号量增加到最大值 */
                                   NULL);    /* unnamed semaphore */
#else
        sem_init(&(sem->sem), 0, value);
#endif
    }

    return sem;
}

csem* cmutex_new(void)
{
    return csem_new(1);
}

void  csem_free(csem *sem)
{
#ifdef WIN32
    CloseHandle(sem->sem);
#else
    sem_destroy(&(sem->sem));
#endif
    free(sem);
}

int  csem_lock(csem *sem)
{
#ifdef WIN32
    return csem_lock_timed(sem, INFINITE);
#else
    int ret = 0;

    do {
        ret = sem_wait(&(sem->sem));
        if(ret < 0 && errno != EINTR){
            printf("semop down failed:%s", strerror(errno));
            assert(0);
        }
    } while (ret != 0);

    return ret;
#endif
}

/* void csem_acquire(csem *sem, int n); */
void csem_unlock(csem *sem)
{
#ifdef WIN32
    ReleaseSemaphore(sem->sem, 1, NULL);
#else
    sem_post(&(sem->sem));
#endif
}

int  csem_lock_timed(csem *sem, int msec)
{
#ifdef WIN32
    unsigned int result = WaitForSingleObject(sem->sem, msec);

    if(WAIT_OBJECT_0 == result){
        return 0;
    } else if(WAIT_TIMEOUT == result) {
        return 1;
    } else {
        return -1;
    }
#else
    struct timespec time;
    int ret = 0;

    if(clock_gettime(CLOCK_REALTIME, &time) == -1) {
        printf("clock_gettime failed");
        return -1;
    }

    time.tv_sec  += msec / 1000;
    time.tv_nsec += msec % 1000 * 1000000;
    if(time.tv_nsec >= 1000000000) {
        time.tv_sec += 1;
        time.tv_nsec -= 1000000000;
    }

    /* printf("sec: %d, nsec: %d\n", time.tv_sec, time.tv_nsec); */
    while ((ret = sem_timedwait(&(sem->sem), &time)) == -1 && errno == EINTR)
        continue;       /* Restart if interrupted by handler */

    if(ret == -1){
        if(ETIMEDOUT == errno){
            /* printf("sem down timeout, wait time:%d\n", msec); */
            return 1;
        } else {
            printf("semop down failed:%s\n", strerror(errno));
            assert(0);
            return -1;
        }
    }

    return 0;
#endif
}

bool csem_try_lock(csem *sem)
{
#ifdef WIN32
    unsigned int dwWaitResult = WaitForSingleObject(sem->sem, 0L);

    switch (dwWaitResult)
    {
        // The semaphore object was signaled.
        case WAIT_OBJECT_0:
            return true;
        default:
            break;

        // The semaphore was nonsignaled, so a time-out occurred.
        /* case WAIT_TIMEOUT:  */
        /*     printf("Thread %d: wait timed out\n", GetCurrentThreadId()); */
        /*     break;  */
    }

    return false;
#else
    if(0 == sem_trywait(&(sem->sem))) {
        return true;
    } else {
        return false;
    }
#endif
}

/* bool csem_try_acquire(csem *sem, int n); */
/* void csem_release(csem *sem, int n); */

/**
 * @Brief  Returns the number of resources currently available to the semaphore.
 *         This number can never be negative.
 *
 * @Param sem
 *
 * @Returns
 */
int csem_getvalue(csem *sem)
{
#ifdef WIN32
    long value = 0;

    //QuerySemaphore(sem->sem, &value);

    return value;
#else
    int retval = 0;
    int val    = 0;

    retval = sem_getvalue(&(sem->sem), &val);
    if(retval < 0) return retval;

    return val;
#endif
}
