#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <memory.h>
#include "cstring.h"
#include "cobj.h"

struct cstring_s{
    char *d;
    size_t n; /* allocd size */
    size_t i; /* index of first unused byte */
};

static size_t cstring_base_size = 128;
#ifdef CSTRING_DEBUG
static int cstring_obj_cnt = 0;
int cstr_get_obj_cnt(void)
{
    return cstring_obj_cnt;
}
#endif

void cstr_set_base_size(size_t size)
{
    cstring_base_size = size;
}

/**
 * @Brief  malloc memory for string 's'
 *
 * @Param s     string need malloc
 * @Param size  new string's size
 */
static cstr* cstr_alloc_more(cstr *s, size_t size)
{
    if((s->n - s->i) < size) {
        s->d = (char*)realloc(s->d, s->n + size);
        /* if (s->d == NULL) oom(); */
        s->n += size;
    }

    return s;
}

static void cstr_init(cstr *str)
{
    memset(str, 0, sizeof(cstr));
    cstr_alloc_more(str, cstring_base_size);
    str->d[0] = '\0';
}

cstr* cstr_new(void)
{
   cstr *str = (cstr*)calloc(1, sizeof(cstr));
   cstr_init(str);

#ifdef CSTRING_DEBUG
   ++cstring_obj_cnt;
#endif

   return str;
}

cstr* cstr_new_with_format(const char *fmt, ...)
{
    cstr *str = cstr_new();

    va_list ap;
    va_start(ap, fmt);
    cstr_printf_va(str, fmt, ap);
    va_end(ap);

    return str;
}

void cstr_free(cstr *str)
{
    if(str->d != NULL) {
        free(str->d);
        str->d = NULL;
    }
    str->i = 0;
    str->n = 0;

    free(str);

#ifdef CSTRING_DEBUG
   --cstring_obj_cnt;
#endif
}

void cstr_clear(cstr *str)
{
    str->i = 0;
    str->d[0] = '\0';
}

void cstr_add(cstr *dst, const cstr *src)
{
    cstr_alloc_more(dst, src->i + 1);

    if(src->i) memcpy(&(dst->d[dst->i]), src->d, src->i);

    dst->i += src->i;
    dst->d[dst->i]='\0';
}

cstr* cstr_add_const(const cstr *str, const cstr *str_add)
{
    cstr *str_cpy = cstr_copy_const(str);

    cstr_add(str_cpy, str_add);

    return str_cpy;
}

void cstr_add_obj(cstr *str, const void *obj)
{
    cstr *str_obj = cobj_to_cstr(obj);

    cstr_add(str, str_obj);
    cstr_free(str_obj);
}

cstr* cstr_add_obj_const(const cstr *str, const void *obj)
{
    cstr *str_cpy = cstr_copy_const(str);

    cstr_add_obj(str_cpy, obj);

    return str_cpy;
}

void cstr_copy(cstr *dst, const cstr *src)
{
    cstr_alloc_more(dst, src->i + 1);

    if(src->i) memcpy(dst->d, src->d, src->i);

    dst->i = src->i;
    dst->d[dst->i]='\0';
}

cstr* cstr_copy_const(const cstr *s)
{
    cstr *str = cstr_new();

    cstr_copy(str, s);

    return str;
}


size_t cstr_len(const cstr *str)
{
    return str->i;
}

const char* cstr_body(const cstr *str)
{
    return str->d;
}

int cstr_printf_va(cstr *s, const char *fmt, va_list ap)
{
   int n = 0;
   va_list cp;

   while (1) {
#ifdef _WIN32
      cp = ap;
#else
      va_copy(cp, ap);
#endif
      n = vsnprintf (&s->d[s->i], s->n-s->i, fmt, cp);
      va_end(cp);

      if ((n > -1) && (n < (int)(s->n-s->i))) {
        s->i += n;
        return n;
      }

      /* Else try again with more space. */
      if (n > -1) cstr_alloc_more(s,n + 1); /* exact */
      else cstr_alloc_more(s,(s->n)*2);   /* 2x */
   }

   return n;
}

#if 0
void cstr_printf(cstr *str, const char *fmt, ...)
{
   va_list ap;
   va_start(ap,fmt);
   cstr_printf_va(str, fmt, ap);
   va_end(ap);
}
#endif

void cstr_format(cstr *str, const char *fmt, ...)
{
   va_list ap;

   cstr_clear(str);

   va_start(ap,fmt);
   cstr_printf_va(str, fmt, ap);
   va_end(ap);
}

void cstr_append(cstr *str, const char *fmt, ...)
{
   va_list ap;
   va_start(ap,fmt);
   cstr_printf_va(str, fmt, ap);
   va_end(ap);
}

void cstr_prepend(cstr *str, const char *fmt, ...)
{
   va_list ap;
   cstr *str_cpy = cstr_copy_const(str);

   cstr_clear(str);

   va_start(ap,fmt);
   cstr_printf_va(str, fmt, ap);
   va_end(ap);

   cstr_add(str, str_cpy);

   cstr_free(str_cpy);
}

void cstr_lower(cstr *s)
{
    int len = cstr_len(s);
    int j = 0;

    for (j = 0; j < len; j++) {
        s->d[j] = tolower(s->d[j]);
    }
}

void cstr_upper(cstr *s)
{
    int len = cstr_len(s);
    int j = 0;

    for (j = 0; j < len; j++) {
        s->d[j] = toupper(s->d[j]);
    }
}

int  cstr_cmp(const cstr *s1, const cstr *s2)
{
    size_t l1 = 0, l2 = 0, minlen = 0;
    int cmp = 0;

    if(s1 == NULL && s2 == NULL){
        return 0;
    } else if(s1 != NULL && s2 != NULL){
        l1 = cstr_len(s1);
        l2 = cstr_len(s2);
        minlen = (l1 < l2) ? l1 : l2;
        cmp = memcmp(s1->d, s2->d, minlen);
        if (cmp == 0) return l1 - l2;
        else return cmp;
    } else if(s1 == NULL) {
        return -1;
    } else {/* s2 == NULL */
        return 1;
    }
}

bool cstr_is_equal(const cstr *s1, const cstr *s2)
{
    return cstr_cmp(s1, s2) == 0;
}

static void cstr_trim_common(cstr *str, const char *cset, bool left, bool right)
{
    char *start = NULL, *end = NULL, *sp = NULL, *ep = NULL;
    char *s   = str->d;
    size_t len;

    if(str == NULL || cstr_len(str) == 0) return;

    sp = start = s;
    ep = end   = s + cstr_len(str) - 1;

    while(left  && sp <= end && strchr(cset, *sp)) sp++;
    while(right && ep > start && strchr(cset, *ep)) ep--;

    len = (sp > ep) ? 0 : ((ep-sp)+1);
    if (str->d != sp) memmove(str->d, sp, len);
    str->d[len] = '\0';
    str->i = len;
}

void cstr_trim_with_set(cstr *s, const char *cset)
{
    cstr_trim_common(s, cset, true, true);
}

void cstr_triml_with_set(cstr *s, const char *cset)
{
    cstr_trim_common(s, cset, true, false);
}

void cstr_trimr_with_set(cstr *s, const char *cset)
{
    cstr_trim_common(s, cset, false, true);
}

cstr* cstr_trim_with_set_const(const cstr *s, const char *cset)
{
    cstr *str_cpy = cstr_copy_const(s);

    cstr_trim_common(str_cpy, cset, true, true);

    return str_cpy;
}

cstr* cstr_triml_with_set_const(const cstr *s, const char *cset)
{
    cstr *str_cpy = cstr_copy_const(s);

    cstr_trim_common(str_cpy, cset, true, false);

    return str_cpy;
}

cstr* cstr_trimr_with_set_const(const cstr *s, const char *cset)
{
    cstr *str_cpy = cstr_copy_const(s);

    cstr_trim_common(str_cpy, cset, false, true);

    return str_cpy;
}


inline static const char* cstr_get_trim_blank_str(void)
{
    return "\n\f\r\t\v ";
}

void cstr_trim(cstr *s)
{
    cstr_trim_with_set(s, cstr_get_trim_blank_str());
}

void cstr_triml(cstr *s)
{
    cstr_triml_with_set(s, cstr_get_trim_blank_str());
}

void cstr_trimr(cstr *s)
{
    cstr_trimr_with_set(s, cstr_get_trim_blank_str());
}

cstr* cstr_trim_const(const cstr *s)
{
    return cstr_trim_with_set_const(s, cstr_get_trim_blank_str());
}

cstr* cstr_triml_const(const cstr *s)
{
    return cstr_triml_with_set_const(s, cstr_get_trim_blank_str());
}

cstr* cstr_trimr_const(const cstr *s)
{
    return cstr_trimr_with_set_const(s, cstr_get_trim_blank_str());
}

void cstr_reverse(cstr *s)
{
    size_t i = 0;
    size_t i_swap = 0;
    char temp = '\0';

    for(i = 0; i < cstr_len(s) / 2; ++i) {
        i_swap       = s->i - i - 1;
        temp         = s->d[i];
        s->d[i]      = s->d[i_swap];
        s->d[i_swap] = temp;
    }
}

cstr* cstr_reverse_const(const cstr *s)
{
    cstr *s_cpy = cstr_copy_const(s);

    cstr_reverse(s_cpy);

    return s_cpy;
}

/*******************************************************************************
 * begin substring search functions                                            *
 ******************************************************************************/
/* Build KMP table from left to right. */
static void _cstr_BuildTable(const char *P_Needle, ssize_t P_NeedleLen,
                             long *P_KMP_Table)
{
    long i, j;

    i = 0;
    j = i - 1;
    P_KMP_Table[i] = j;
    while (i < P_NeedleLen) {
        while ( (j > -1) && (P_Needle[i] != P_Needle[j]) ) {
           j = P_KMP_Table[j];
        }
        i++;
        j++;
        if (i < P_NeedleLen) {
            if (P_Needle[i] == P_Needle[j]) {
                P_KMP_Table[i] = P_KMP_Table[j];
            } else {
                P_KMP_Table[i] = j;
            }
        } else {
            P_KMP_Table[i] = j;
        }
    }

    return;
}


/* Build KMP table from right to left. */
static void _cstr_BuildTableR(const char *P_Needle, ssize_t P_NeedleLen,
                              long *P_KMP_Table)
{
    long i, j;

    i = P_NeedleLen - 1;
    j = i + 1;
    P_KMP_Table[i + 1] = j;
    while (i >= 0) {
        while ( (j < P_NeedleLen) && (P_Needle[i] != P_Needle[j]) ) {
           j = P_KMP_Table[j + 1];
        }
        i--;
        j--;
        if (i >= 0) {
            if (P_Needle[i] == P_Needle[j]) {
                P_KMP_Table[i + 1] = P_KMP_Table[j + 1];
            } else {
                P_KMP_Table[i + 1] = j;
            }
        } else {
            P_KMP_Table[i + 1] = j;
        }
    }

    return;
}


/* Search data from left to right. ( Multiple search mode. ) */
static long _cstr_find(const char *P_Haystack, size_t P_HaystackLen,
                       const char *P_Needle,   size_t P_NeedleLen,
                       long *P_KMP_Table)
{
    long i, j;
    long V_FindPosition = -1;

    /* Search from left to right. */
    i = j = 0;
    while((j < (int)P_HaystackLen) && (((P_HaystackLen - j) + i) >= P_NeedleLen)) {
        while((i > -1) && (P_Needle[i] != P_Haystack[j])) {
            i = P_KMP_Table[i];
        }
        i++;
        j++;
        if (i >= (int)P_NeedleLen) {
            /* Found. */
            V_FindPosition = j - i;
            break;
        }
    }

    return V_FindPosition;
}


/* Search data from right to left. ( Multiple search mode. ) */
static long _cstr_findR(const char *P_Haystack, size_t P_HaystackLen,
                        const char *P_Needle,   size_t P_NeedleLen,
                        long *P_KMP_Table)
{
    long i, j;
    long V_FindPosition = -1;

    /* Search from right to left. */
    j = (P_HaystackLen - 1);
    i = (P_NeedleLen - 1);
    while ( (j >= 0) && (j >= i) ) {
        while ( (i < (int)P_NeedleLen) && (P_Needle[i] != P_Haystack[j]) ) {
            i = P_KMP_Table[i + 1];
        }
        i--;
        j--;
        if (i < 0) {
            /* Found. */
            V_FindPosition = j + 1;
            break;
        }
    }

    return V_FindPosition;
}


/**
 * @Brief  Search data from left to right. ( One time search mode. )
 *
 * @Param s
 * @Param P_StartPosition Start from 0. -1 means last position.
 * @Param P_Needle
 * @Param P_NeedleLen
 *
 * @Returns
 */
long cstr_find(const cstr *s, long P_StartPosition,
               const char *P_Needle, ssize_t P_NeedleLen)
{
    long V_StartPosition;
    long V_HaystackLen;
    long *V_KMP_Table;
    long V_FindPosition = -1;

    if (P_StartPosition < 0) {
        V_StartPosition = s->i + P_StartPosition;
    } else {
        V_StartPosition = P_StartPosition;
    }
    V_HaystackLen = s->i - V_StartPosition;
    if ( (V_HaystackLen >= P_NeedleLen) && (P_NeedleLen > 0) ) {
        V_KMP_Table = (long *)malloc(sizeof(long) * (P_NeedleLen + 1));
        if (V_KMP_Table != NULL) {
            _cstr_BuildTable(P_Needle, P_NeedleLen, V_KMP_Table);

            V_FindPosition = _cstr_find(s->d + V_StartPosition,
                                            V_HaystackLen,
                                            P_Needle,
                                            P_NeedleLen,
                                            V_KMP_Table);
            if (V_FindPosition >= 0) {
                V_FindPosition += V_StartPosition;
            }

            free(V_KMP_Table);
        }
    }

    return V_FindPosition;
}


/*
 * @Brief   Search data from right to left. ( One time search mode. )
 * @Param s
 * @Param P_StartPosition   Start from 0. -1 means last position.
 * @Param P_Needle
 * @Param P_NeedleLen
 *
 * @Returns
 */
long cstr_findR(const cstr *s, long P_StartPosition,
                const char *P_Needle, ssize_t P_NeedleLen)
{
    long V_StartPosition = 0;
    long V_HaystackLen   = 0;
    long *V_KMP_Table    = NULL;
    long V_FindPosition  = -1;

    if (P_StartPosition < 0) {
        V_StartPosition = s->i + P_StartPosition;
    } else {
        V_StartPosition = P_StartPosition;
    }
    V_HaystackLen = V_StartPosition + 1;
    if ( (V_HaystackLen >= P_NeedleLen) && (P_NeedleLen > 0) ) {
        V_KMP_Table = (long *)malloc(sizeof(long) * (P_NeedleLen + 1));
        if (V_KMP_Table != NULL) {
            _cstr_BuildTableR(P_Needle, P_NeedleLen, V_KMP_Table);

            V_FindPosition = _cstr_findR(s->d,
                                             V_HaystackLen,
                                             P_Needle,
                                             P_NeedleLen,
                                             V_KMP_Table);

            free(V_KMP_Table);
        }
    }

    return V_FindPosition;
}
/*******************************************************************************
 * end substring search functions                                              *
 ******************************************************************************/

static bool cstr_is_xxx(const cstr *s, int (*callback)(int))
{
    bool ret = true;
    size_t i = 0;

    for(i = 0; i < cstr_len(s); ++i) {
        if(!callback(s->d[i])) {
            ret = false;
            break;
        }
    }

    return ret;
}

bool cstr_is_alnum(const cstr *s)
{
    return cstr_is_xxx(s, isalnum);
}

bool cstr_is_num(const cstr *s)
{
    return cstr_is_xxx(s, isdigit);
}

bool cstr_is_alpha(const cstr *s)
{
    return cstr_is_xxx(s, isalpha);
}

//bool cstr_is_decimal(const cstr *s)
//{
//    printf("%s error!\n", __FUNCTION__);
//    return false;
//}

bool cstr_is_digit(const cstr *s)
{
    return cstr_is_xxx(s, isdigit);
}

bool cstr_is_xdigit(const cstr *s)
{
    return cstr_is_xxx(s, isxdigit);
}

bool cstr_is_lower(const cstr *s)
{
    return cstr_is_xxx(s, islower);
}

bool cstr_is_upper(const cstr *s)
{
    return cstr_is_xxx(s, isupper);
}

/* bool cstr_is_space(const cstr *s) */
/* { */
/*     return cstr_is_xxx(s, isblank); */
/* } */

bool cstr_is_startwith(const cstr *s, const char *sub);
bool cstr_is_endswith(const cstr *s, const char *sub);

int cstr_fprint(const cstr *s, FILE *stream)
{
    return fprintf(stream, "%s", cstr_body(s));
}

int cstr_print(const cstr *s)
{
    return cstr_fprint(s, stdout);
}

int cstr_fprintln(const cstr *s, FILE *stream)
{
    return fprintf(stream, "%s\n", cstr_body(s));
}

int cstr_println(const cstr *s)
{
    return cstr_fprintln(s, stdout);
}
