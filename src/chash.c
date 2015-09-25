/* {{{
 * =============================================================================
 *      Filename    :   chash.c
 *      Description :
 *      Created     :   2014-09-20 11:09:05
 *      Author      :    Wu Hong
 * =============================================================================
 }}} */
#include <memory.h>
#include "cobj_int.h"
#include "cobj_str.h"
#include "chash.h"
#include "clist.h"
#include "murmurhash.h"

#ifdef CHASH_ENABLE_SEM
#include "csem.h"
#endif

typedef struct chash_item
{
    COBJ_HEAD_VARS;

    uint32_t hash_val;

    void *key;
    void *val;
}chash_item;

typedef struct chash_bkt
{
    chash *hash;
    clist *items;
} chash_bkt;

struct chash
{
#ifdef CHASH_ENABLE_SEM
    cmutex  *mutex;
#endif

    uint32_t    bkts_num;
    uint32_t    bkts_num_log2;
    chash_bkt *bkts;

    uint32_t cnt_items;
};

struct chash_iter
{
    const chash *hash;
    uint32_t bkt_idx;
    clist_iter item_iter;
};

static inline chash_item* cobj_to_item(const void *obj)
{
    return (chash_item*)obj;
}

static int cobj_item_fprint(const void *obj, FILE *pfile)
{
    chash_item *item = cobj_to_item(obj);

    fprintf(pfile, "\"");
    cobj_fprint(item->key, pfile);
    fprintf(pfile, "\": \"");
    cobj_fprint(item->val, pfile);
    fprintf(pfile, "\"");

    return 0;
}

chash_item* cobj_hash_item_new(uint32_t hashval, void *key, void *val);
static void *cobj_item_dup(const void *obj)
{
    chash_item *item = cobj_to_item(obj);
    void* key = cobj_dup(item->key);
    void* val = cobj_dup(item->val);

    return cobj_hash_item_new(item->hash_val, key, val);
}

static void cobj_item_free(void *obj)
{
    chash_item *item = cobj_to_item(obj);

    cobj_free(item->key);
    cobj_free(item->val);
}

static cobj_ops_t cobj_ops_hashitem = {
    .name = "Hash Item",
    .obj_size = sizeof(chash_item),
    .cb_print = cobj_item_fprint,
    .cb_dup = cobj_item_dup,
    .cb_destructor = cobj_item_free,
};

chash_item* cobj_hash_item_new(uint32_t hashval, void *key, void *val)
{
    chash_item *item = (chash_item*)malloc(sizeof(chash_item));

    cobj_set_ops(item, &cobj_ops_hashitem);
    item->hash_val = hashval;
    item->key = key;
    item->val = val;

    return item;
}

uint32_t cobj_hash_item_hashval(const chash_item *item)
{
    return item->hash_val;
}

void* cobj_hash_item_key(chash_item *item)
{
    return item->key;
}

void* cobj_hash_item_value(chash_item *item)
{
    return item->val;
}

void cobj_hash_item_set(chash_item *item, void *key, void *val)
{
    chash_item *hash_item = (chash_item*)item;

    cobj_free(hash_item->key);
    cobj_free(hash_item->val);

    hash_item->key = key;
    hash_item->val = val;
}


bool chash_iter_is_end(chash_iter *itor)
{
    return itor->bkt_idx == (itor->hash->bkts_num);
}

chash_iter* chash_iter_next(chash_iter *itor)
{
    uint32_t bkt_idx  = 0;
    chash_bkt *bkt  = NULL;
    const chash       *hash = NULL;
    bool is_exsit_next = false;

    hash = itor->hash;
    clist_iter_to_next(&(itor->item_iter));

    if(!clist_iter_is_end(&(itor->item_iter))) {
        is_exsit_next = true;
    } else {
        for (bkt_idx = itor->bkt_idx + 1; bkt_idx < hash->bkts_num; bkt_idx++) {
            bkt = &(hash->bkts[bkt_idx]);

            if(clist_size(bkt->items) > 0) {
                itor->bkt_idx   = bkt_idx;
                itor->item_iter = clist_begin(bkt->items);
                is_exsit_next   = true;
                break;
            }
        }
    }

    if(!is_exsit_next) {
        itor->bkt_idx = hash->bkts_num;
    }

    return itor;
}

chash_iter* chash_iter_new(const chash *hash)
{
    uint32_t bkt_idx = 0;
    chash_bkt *bkt = NULL;

    chash_iter *itor = (chash_iter*)calloc(1, sizeof(chash_iter));

    itor->hash    = hash;
    itor->bkt_idx = hash->bkts_num;

    for (bkt_idx = 0; bkt_idx < hash->bkts_num; bkt_idx++) {
        bkt = &(hash->bkts[bkt_idx]);

        if(clist_size(bkt->items) > 0) {
            itor->bkt_idx   = bkt_idx;
            itor->item_iter = clist_begin(bkt->items);
            break;
        }
    }

    return itor;
}

void chash_iter_free(chash_iter *itor)
{
    free(itor);
}

void* chash_iter_key(chash_iter *itor)
{
    if(!clist_iter_is_end(&(itor->item_iter))) {
        return clist_iter_obj(&(itor->item_iter));
    }

    return NULL;
}

void* chash_iter_value(chash_iter *itor)
{
    if(!clist_iter_is_end(&(itor->item_iter))) {
        return cobj_hash_item_value((chash_item*)clist_iter_obj(&(itor->item_iter)));
    }

    return NULL;
}

inline static uint32_t hash_val_to_bkt(uint32_t hash_val, uint32_t bkts_num)
{
    return hash_val % bkts_num;
}

inline static uint32_t chash_get_bkts_num(const chash *hash)
{
    return hash->bkts_num;// tables_bkts_num[hash->bkts_num_idx];
}

static chash_bkt* chash_get_bkt(const chash *hash, uint32_t hash_val)
{
    uint32_t bkt_idx = hash_val_to_bkt(hash_val, chash_get_bkts_num(hash));
    return &(hash->bkts[bkt_idx]);
}

static uint32_t chash_bkt_get_item_num(const chash_bkt *bkt)
{
    return clist_size(bkt->items);
}

static chash_bkt* chash_bkts_new(chash *hash)
{
    chash_bkt *bkts = NULL;
    chash_bkt *bkt  = NULL;
    uint32_t i = 0;

    bkts = (chash_bkt*)calloc(hash->bkts_num, sizeof(chash_bkt));
    for (i = 0; i < hash->bkts_num; i++) {
        bkt = &(bkts[i]);
        bkt->hash  = hash;
        bkt->items = clist_new();
    }

    return bkts;
}

chash *chash_new(void)
{
    chash *hash = (chash*)calloc(1, sizeof(chash));

    hash->bkts_num = 32;
    hash->bkts_num_log2 = 5;
#ifdef CHASH_ENABLE_SEM
    hash->mutex = cmutex_new();
#endif

    hash->bkts = chash_bkts_new(hash);

    return hash;
}

uint32_t chash_count(const chash *hash)
{
    return hash->cnt_items;
}

void chash_free(chash *hash)
{
    uint32_t  bkts_num = 0;
    uint32_t  i        = 0;
    chash_bkt *bkt     = NULL;

    if(hash){
        bkts_num = hash->bkts_num;
#ifdef CHASH_ENABLE_SEM
        cmutex_free(hash->mutex);
#endif

        for (i = 0; i < bkts_num; i++) {
            bkt = &(hash->bkts[i]);
            clist_free(bkt->items);
        }

        free(hash->bkts);
        free(hash);
    }
}

void chash_clear(chash *hash)
{
    uint32_t bkts_num = hash->bkts_num;
    uint32_t i = 0;
    chash_bkt *bkt     = NULL;

    for (i = 0; i < bkts_num; i++) {
        bkt = &(hash->bkts[i]);
        clist_clear(bkt->items);
    }
}

#ifdef CHASH_ENABLE_SEM
void chash_lock(chash *hash)
{
    cmutex_lock(hash->mutex);
}

void chash_unlock(chash *hash)
{
    cmutex_unlock(hash->mutex);
}
#endif

static clist_iter bkt_find_item_node(chash_bkt* bkt, uint32_t hash_val, const void *key)
{
    chash_item *item = NULL;
    clist_iter iter = clist_begin(bkt->items);

    clist_iter_foreach_obj(&iter, item) {
        /* hash 值不一致肯定不是 */
        if(cobj_hash_item_hashval(item) != hash_val) continue;
        if(cobj_equal(cobj_hash_item_key(item), key)) {
            break;
        }
    }

    return iter;
}

static chash_item* bkt_find_item(chash_bkt* bkt, uint32_t hash_val, const void *key)
{
    clist_iter iter = bkt_find_item_node(bkt, hash_val, key);

    return (chash_item*)clist_iter_obj(&iter);
}

bool chash_haskey(const chash *hash, const void *key)
{
    uint32_t    hash_val = 0;
    chash_bkt *bkt     = 0;

    hash_val = cobj_hash(key);
    bkt = chash_get_bkt(hash, hash_val);

    return bkt_find_item(bkt, hash_val, key) != NULL;
}

static chash_item *bkt_add(chash_bkt *bkt, uint32_t hashval, void *key, void *val)
{/*{{{*/
    chash_item *item = cobj_hash_item_new(hashval, key, val);

    clist_append(bkt->items, item);

#ifdef DEBUG_CHASH
    printf("[CHASH][NEW] hash:0x%08X ", hashval);
    cobj_print(item);
    printf("\n");
#endif

    return item;
}/*}}}*/

static void bkt_del(chash *hash, chash_bkt *bkt,
                    uint32_t hash_val, const void *key)
{
    clist_iter iter = bkt_find_item_node(bkt, hash_val, key);
    if(!clist_iter_is_end(&iter)) {
        clist_remove(&iter);
        --(hash->cnt_items);
    }
}

static void chash_adjust(chash *hash)
{
    chash_bkt *bkts_old = hash->bkts;
    uint32_t bkts_num_old = hash->bkts_num;
    uint32_t bkts_num_new = bkts_num_old * 2;
    uint32_t bkt_idx_old = 0;
    uint32_t bkt_idx_new = 0;
    clist_iter iter_pop;
    clist_iter iter;
    chash_bkt *bkt_old = NULL;
    chash_bkt *bkt_new = NULL;
    chash_item *item = NULL;

    /* printf("adjust hash, bukts num:%d\n", bkts_num_new); */

    hash->bkts_num = bkts_num_new;
    hash->bkts = chash_bkts_new(hash);
    for (bkt_idx_old = 0; bkt_idx_old < bkts_num_old; bkt_idx_old++) {
        bkt_old = &(bkts_old[bkt_idx_old]);

        iter = clist_begin(bkt_old->items);
        while(!clist_iter_is_end(&iter)) {
            iter_pop = iter;
            clist_iter_to_next(&iter);

            /* 从原先bkt 弹出该node */
            item = (chash_item*)clist_pop(&iter_pop);

            bkt_idx_new = hash_val_to_bkt(cobj_hash_item_hashval(item), bkts_num_new);
            bkt_new = &(hash->bkts[bkt_idx_new]);
            clist_append(bkt_new->items, item);
        }
        clist_free(bkt_old->items);
    }

    hash->bkts_num_log2 += 1;
    free(bkts_old);
}

void chash_set(chash *hash, void  *key,  void *val)
{
    uint32_t hash_val = 0;
    chash_bkt  *bkt = NULL;
    chash_item *item = NULL;

    hash_val = cobj_hash(key);
    bkt      = chash_get_bkt(hash, hash_val);

    item = bkt_find_item(bkt, hash_val, key);
    if(NULL == item) {
        item = bkt_add(bkt, hash_val, key, val);
        ++hash->cnt_items;

        /* 某个bkt 的数据过多，重新调整hash表 */
        if(chash_bkt_get_item_num(bkt) > hash->bkts_num_log2) {
            /* chash_printf_test(stdout, hash); */
            chash_adjust(hash);
            /* chash_printf_test(stdout, hash); */
        }

    } else {
        cobj_hash_item_set(item, key, val);
    }
}

void* chash_get_value(chash *hash, const void *key)
{
    uint32_t  hash_val = 0;
    chash_bkt  *bkt  = NULL;
    chash_item *item = NULL;

    hash_val = cobj_hash(key);
    bkt      = chash_get_bkt(hash, hash_val);

    /* cobj_print(key); */
    /* printf(" hash_val:%08X\n", hash_val); */

    item = bkt_find_item(bkt, hash_val, key);

    return item ? cobj_hash_item_value(item) : NULL;
}

void chash_del(chash *hash, const void *key)
{
    uint32_t  hash_val = 0;
    chash_bkt  *bkt = NULL;

    hash_val = cobj_hash(key);
    bkt      = chash_get_bkt(hash, hash_val);

    bkt_del(hash, bkt, hash_val, key);
}

void chash_printf_test(const chash *hash, FILE *file)
{
    uint32_t i = 0;
    uint32_t idx_item = 0;
    const clist_node *node;
    const chash_bkt  *bkt  = NULL;
    const chash_item *item = NULL;

    fprintf(file, "--------------------------------------------------\n");
    fprintf(file, "\t\tHash Print\n");
    fprintf(file, "--------------------------------------------------\n");

    fprintf(file, "bkt num:%d item cnt:%d\n",
                  chash_get_bkts_num(hash), hash->cnt_items);

    for (i = 0; i < chash_get_bkts_num(hash); i++) {
        bkt = &(hash->bkts[i]);
        fprintf(file, "  |-bkt idx:%d, item cnt:%d\n", i, clist_size(bkt->items));

        idx_item = 0;
        clist_foreach_val(bkt->items, node, item) {
            fprintf(file, "    |-item:%d ", idx_item);
            fprintf(file, " hash:0x%08X", cobj_hash_item_hashval(item));
            fprintf(file, " key:");
            /* fprintf(file, " key:%X", (long)item->key); */
            cobj_print(item);
            fprintf(file, "\n");
            ++idx_item;
        }
    }
}

void chash_printf(const chash *hash, FILE *file)
{
    uint32_t idx_item = 0;
    chash_iter *itor = NULL;

    fprintf(file, "{");

    itor = chash_iter_new(hash);
    while(!chash_iter_is_end(itor)){
        cobj_fprint(clist_iter_obj(&(itor->item_iter)), file);

        ++idx_item;
        if(idx_item < hash->cnt_items) {
            fprintf(file, ", ");
        }

        chash_iter_next(itor);
    }
    fprintf(file, "}");

    chash_iter_free(itor);
}

void chash_to_cstr(const chash *hash, cstr *str)
{
    uint32_t idx_item = 0;
    chash_iter *itor = NULL;
    cstr *str_item = NULL;

    cstr_append(str, "{");

    itor = chash_iter_new(hash);
    while(!chash_iter_is_end(itor)){
        str_item = cobj_to_cstr(clist_iter_obj(&(itor->item_iter)));
        cstr_add(str, str_item);
        cstr_free(str_item);

        ++idx_item;
        if(idx_item < hash->cnt_items) {
            cstr_append(str, ", ");
        }

        chash_iter_next(itor);
    }
    cstr_append(str, "}");

    chash_iter_free(itor);
}

/* special chash */
bool chash_int_haskey(const chash *hash, int key)
{
    bool is_exist = false;
    cobj_int key_obj;

    cobj_int_init(&key_obj, key);

    is_exist = chash_haskey(hash, &key_obj);

    return is_exist;
}

void* chash_int_get(chash *hash, int key)
{
    void *val_obj = NULL;
    cobj_int key_obj;

    cobj_int_init(&key_obj, key);

    val_obj = chash_get_value(hash, &key_obj);

    return val_obj;
}

void chash_int_set(chash *hash, int key, void *val)
{
    cobj_int *key_obj = cobj_int_new(key);

    chash_set(hash, key_obj, val);
}

void chash_int_del(chash *hash, int key)
{
    cobj_int key_obj;

    cobj_int_init(&key_obj, key);

    chash_del(hash, &key_obj);
}

bool chash_str_haskey(const chash *hash, const char *key)
{
    bool is_exist = false;
    cobj_str key_obj;

    cobj_str_init(&key_obj, key);

    is_exist = chash_haskey(hash, &key_obj);

    cobj_str_release(&key_obj);

    return is_exist;
}

void* chash_str_get(chash *hash, const char* key)
{
    void *val_obj = NULL;
    cobj_str key_obj;

    cobj_str_init(&key_obj, key);

    val_obj = chash_get_value(hash, &key_obj);
    cobj_str_release(&key_obj);

    return val_obj;
}

void chash_str_set(chash *hash, const char* key, void *val)
{
    cobj_str *key_obj = cobj_str_new(key);

    chash_set(hash, key_obj, val);
}

void chash_str_del(chash *hash, const char* key)
{
    cobj_str key_obj;

    cobj_str_init(&key_obj, key);
    chash_del(hash, &key_obj);
    cobj_str_release(&key_obj);
}

const char* chash_str_iter_key(chash_iter *itor)
{
    return cobj_str_val((cobj_str*)chash_iter_key(itor));
}

const char* chash_str_str_get(chash *hash, const char *key)
{
    cobj_str *val = (cobj_str*)chash_str_get(hash, key);

    return val ? cobj_str_val(val) : NULL;
}

void chash_str_str_set(chash *hash, const char *key, const char *val)
{
    cobj_str *val_obj = cobj_str_new(val);

    chash_str_set(hash, key, val_obj);
}

bool chash_str_str_haskey(const chash *hash, const char *key)
{
    return chash_str_haskey(hash, key);
}

void chash_str_str_del(chash *hash, const char* key)
{
    chash_str_del(hash, key);
}

const char* chash_str_str_iter_key(chash_iter *itor)
{
    return chash_str_iter_key(itor);
}
