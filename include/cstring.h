#ifndef CSTRING_H_201408242108
#define CSTRING_H_201408242108
#ifdef __cplusplus
extern "C" {
#endif

/* {{{
 * =============================================================================
 *      Filename    :   cstring.h
 *      Description :   提供C语言对动态字符串的常用操作
 *          使用此方法可以不需要预先分配制定的大小，无需关心字符串所占空间大小
 *
 *          参考开源代码包括：
 *              utstring
 *              sds
 *      Created     :   2014-08-24 21:08:09
 *      Author      :    Wu Hong
 * =============================================================================
 }}} */

#include <sys/types.h>
#include <stdarg.h>
#include <stdbool.h>

#define CSTRING_DEBUG

#ifdef __GNUC__
#define CSTRING_FMT_EXTRA_1_2 __attribute__ (( format( printf, 1, 2) ))
#define CSTRING_FMT_EXTRA_2_3 __attribute__ (( format( printf, 2, 3) ))
#else
#define CSTRING_FMT_EXTRA_1_2
#define CSTRING_FMT_EXTRA_2_3
#endif


struct cstring_s;
typedef struct cstring_s cstr;

void cstr_set_base_size(size_t size);

size_t cstr_len(const cstr *str);
const char* cstr_body(const cstr *str);

cstr* cstr_new(void);
cstr* cstr_new_with_format(const char *fmt, ...) CSTRING_FMT_EXTRA_1_2;
void cstr_free(cstr *str);
void cstr_clear(cstr *str);


void cstr_copy(cstr *dst, const cstr *src);
cstr* cstr_copy_const(const cstr *s);

void cstr_add(cstr *str, const cstr *str_add);
cstr* cstr_add_const(const cstr *str, const cstr *str_add);
void cstr_add_obj(cstr *str, const void *obj);
cstr* cstr_add_obj_const(const cstr *str, const void *obj);

/* Remove the part of the string from left and from right composed just of
 * contiguous characters found in 'cset', that is a null terminted C string.
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = ("AA...AA.a.aa.aHelloWorld     :::");
 * s = cstr_trim2(s,"Aa. :");
 * printf("%s\n", s);
 *
 * Output will be just "Hello World".
 */
void cstr_trim_with_set(cstr *s, const char *cset);
void cstr_triml_with_set(cstr *s, const char *cset);
void cstr_trimr_with_set(cstr *s, const char *cset);
void cstr_trim(cstr *s);
void cstr_triml(cstr *s);
void cstr_trimr(cstr *s);
cstr* cstr_trim_with_set_const(const cstr *s, const char *cset);
cstr* cstr_triml_with_set_const(const cstr *s, const char *cset);
cstr* cstr_trimr_with_set_const(const cstr *s, const char *cset);
cstr* cstr_trim_const(const cstr *s);
cstr* cstr_triml_const(const cstr *s);
cstr* cstr_trimr_const(const cstr *s);

void cstr_reverse(cstr *s);
cstr* cstr_reverse_const(const cstr *s);

long cstr_find(const cstr *s, long pos_start,
               const char *P_Needle, ssize_t P_NeedleLen);
long cstr_findr(const cstr *s,        long P_StartPosition,
                const char *P_Needle, ssize_t P_NeedleLen);

void cstr_substr(cstr *s, int pos_start, int pos_end);
void cstr_substr_to_tail(cstr *s, int pos_start);
void cstr_substr_from_head(cstr *s, int pos_end);
cstr* cstr_substr_const(const cstr *s, int pos_start, int pos_end);

void cstr_replace(cstr *s, const char *old_s, const char *new_s);
cstr* cstr_replace_const(const cstr *s, const char *old_s, const char *new_s);

void cstr_lower(cstr *s);
void cstr_upper(cstr *s);
int  cstr_cmp(const cstr *s1, const cstr *s2);
bool cstr_is_equal(const cstr *s1, const cstr *s2);

int cstr_printf_va(cstr *s, const char *fmt, va_list ap);
void cstr_format(cstr *str, const char *fmt, ...) CSTRING_FMT_EXTRA_2_3;
void cstr_append(cstr *str, const char *fmt, ...) CSTRING_FMT_EXTRA_2_3;
void cstr_prepend(cstr *str, const char *fmt, ...) CSTRING_FMT_EXTRA_2_3;
cstr* cstr_append_const(const cstr *str, const char *fmt, ...) CSTRING_FMT_EXTRA_2_3;
cstr* cstr_prepend_const(const cstr *str, const char *fmt, ...) CSTRING_FMT_EXTRA_2_3;

bool cstr_is_alnum(const cstr *s);
bool cstr_is_num(const cstr *s);
bool cstr_is_alpha(const cstr *s);
bool cstr_is_decimal(const cstr *s);
bool cstr_is_digit(const cstr *s);
bool cstr_is_xdigit(const cstr *s);
bool cstr_is_lower(const cstr *s);
bool cstr_is_upper(const cstr *s);
bool cstr_is_space(const cstr *s);
bool cstr_is_startwith(const cstr *s, const char *sub);
bool cstr_is_endswith(const cstr *s, const char *sub);

int cstr_fprint(const cstr *s, FILE *stream);
int cstr_print(const cstr *s);
int cstr_fprintln(const cstr *s, FILE *stream);
int cstr_println(const cstr *s);

#ifdef CSTRING_DEBUG
int cstr_get_obj_cnt(void);
#endif

#ifdef __cplusplus
}
#endif
#endif  /* CSTRING_H_201408242108 */
