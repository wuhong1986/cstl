#ifndef CHASH_H_201409201109
#define CHASH_H_201409201109
#ifdef __cplusplus
extern "C" {
#endif

/* {{{
 * =============================================================================
 *      Filename    :   chash.h
 *      Description :
 *      Created     :   2014-09-20 11:09:36
 *      Author      :    Wu Hong
 * =============================================================================
 }}} */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define CHASH_ENABLE_SEM

typedef struct chash chash;
typedef struct chash_iter chash_iter;

uint32_t hash_val(const uint8_t *key, uint32_t keylen);

chash *chash_new(void);
void chash_free(chash *hash);
uint32_t chash_count(const chash *hash);
bool chash_haskey(const chash *hash, const void *key);
void chash_del(chash *hash, const void *key);
void* chash_get_value(chash *hash, const void *key);
void chash_set(chash *hash, void *key, void *val);

#ifdef CHASH_ENABLE_SEM
void chash_lock(chash *hash);
void chash_unlock(chash *hash);
#endif

chash_iter* chash_iter_new(const chash *hash);
void chash_iter_free(chash_iter *itor);
bool chash_iter_is_end(chash_iter *itor);
chash_iter* chash_iter_next(chash_iter *itor);
void* chash_iter_key(chash_iter *itor);
void* chash_iter_value(chash_iter *itor);

void chash_printf(const chash *hash, FILE *file);
void chash_printf_test(const chash *hash, FILE *file);

bool chash_int_haskey(const chash *hash, int key);
void chash_int_set(chash *hash, int key, void *val);
void* chash_int_get(chash *hash, int key);
void chash_int_del(chash *hash, int key);

bool chash_str_haskey(const chash *hash, const char *key);
void chash_str_set(chash *hash, const char* key, void *val);
void* chash_str_get(chash *hash, const char* key);
void chash_str_del(chash *hash, const char* key);
const char* chash_str_iter_key(chash_iter *itor);

bool chash_str_str_haskey(const chash *hash, const char *key);
const char* chash_str_str_get(chash *hash, const char *key);
void chash_str_str_set(chash *hash, const char *key, const char *val);
void chash_str_str_del(chash *hash, const char* key);
const char* chash_str_str_iter_key(chash_iter *itor);

#ifdef __cplusplus
}
#endif
#endif  /* CHASH_H_201409201109 */
