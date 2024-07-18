//
// Created by SLYAPA on 18.07.2024.
//

#ifndef UNTITLED3_HASHTABLE_H
#define UNTITLED3_HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define NONE 0
#define OCCUPIED 1
#define DEL 2

#define ht_size(h) ((h).size)
#define ht_max_size(h) ((h).max_size)
#define ht_capacity(h) ((h).capacity)
#define ht_key(h, i) ((h).keys[(i)])
#define ht_val(h, i) ((h).vals[(i)])

#define ht_valid(h, i) ((h).flags && (h).flags[(i)] == OCCUPIED)

/*округление до степени двойки, как работает хер поймет*/
/** Round up a 8-bit integer variable `x` to a nearest power of 2  */
#define roundup8(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, ++(x))

/** Round up a 16-bit integer variable `x` to a nearest power of 2  */
#define roundup16(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, ++(x))

/** Round up a 32-bit integer variable `x` to a nearest power of 2  */
#define roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

/** Round up a 64-bit integer variable `x` to a nearest power of 2  */
#define roundup64(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, (x)|=(x)>>32, ++(x))

#if SIZE_MAX == UINT64_MAX
/** Round up a size_t variable `x` to a nearest power of 2  */
#define roundupsize(x) roundup64(x)
#elif SIZE_MAX == UINT32_MAX
/** Round up a size_t variable `x` to a nearest power of 2  */
#define roundupsize(x) roundup32(x)
#elif SIZE_MAX == UINT16_MAX
/** Round up a size_t variable `x` to a nearest power of 2  */
#define roundupsize(x) roundup16(x)
#elif SIZE_MAX == UINT8_MAX
/** Round up a size_t variable `x` to a nearest power of 2  */
#define roundupsize(x) roundup8(x)
#endif
/* *** */

#define HT(key_t, val_t) struct { \
    size_t size,                  \
            max_size,             \
            capacity; \
    char *flags;                  \
    key_t *keys;                  \
    val_t *vals;\
}
/*size - текущее колво елементов, max_size - предел заполнения в 75%, capacity - физический ее размер */

#define ht_init(h) do { \
    (h).size = (h).max_size = (h).capacity = 0; \
    (h).keys = NULL;         \
    (h).vals = NULL;         \
    (h).flags = NULL;\
} while(0)

#define ht_destroy(h) do { \
    free((h).vals);         \
    free((h).flags);        \
    free((h).keys);\
} while(0)

#define ht_clear(h) do { \
    (h).size = 0;         \
    if ((h).flags) {     \
        memset((h).flags, 0, (h).capacity);\
    }\
} while(0)

#define ht_get(h, key, result, hash_f, eq_f) do { \
    if ((h).size == 0){                  \
        (result) = 0;                    \
        break;\
    }                                    \
    size_t ht_mask = (h).capacity - 1;   \
    (result) = hash_f(key) & ht_mask;    \
    size_t ht_step = 0;                  \
    while((h).flags[(result)] == DEL || ((h).flags[(result)] == OCCUPIED && !eq_f((h).keys[(result)], (key)))) { \
        (result) = ((result) + ++ht_step) & ht_mask;  /*экивалентно MOD capacity */                                 \
    }                                    \
    if ((h).flags[(result)] == NONE) {   \
        (result) = 0;                    \
    }\
} while(0)
/*capacity всегда степень 2, согласно "x % 2^n == x & (2^n - 1)" ht_mask нам надо чтобы ходить по кругу таблицы без деления по модулю*/

#define ht_append(h, key_t, val_t, key, index, absent, hash_f, eq_f) do { \
    _Bool ht_success;                                                     \
    size_t ht_new_size = (h).size ? (h).size + 1 : 2;                     \
    ht_reserve((h), key_t, val_t, ht_new_size, ht_success, hash_f);       \
    if (!ht_success) {                                                    \
        (absent) = -1;                                                    \
        break;\
    }                                                                     \
    size_t ht_mask = (h).capacity - 1;                                    \
    (index) = hash_f(key) & ht_mask;                                      \
    size_t ht_step = 0;                                                   \
    while (((h).flags[(index)] == OCCUPIED && !eq_f((h).keys[(index)], (key)))) { \
        (index) = ((index) + ++ht_step) & ht_mask;                                                                      \
    }                                                                     \
    if ((h).flags[(index)] == OCCUPIED) {                                        \
        (absent) = 0;                                                                      \
    } else {                                                              \
        (h).flags[(index)] = OCCUPIED;                                    \
        (h).keys[(index)] = (key);                                        \
        (h).size++;                                                       \
        (absent) = 1;\
    }\
} while(0)


#define ht_reserve(h, key_t, val_t, new_capacity, success, hash_f) do { \
    if (new_capacity <= (h).max_size) {                                 \
        (success) = 1;                                                  \
        break;\
    }                                                                   \
    size_t ht_new_capacity = (new_capacity);                            \
    roundupsize(ht_new_capacity);                                       \
    if (ht_new_capacity <= (h).capacity) {                              \
        ht_new_capacity <<= 1;                                          \
    }                                                                   \
    char *ht_new_flags = malloc(ht_new_capacity);                       \
    if (!ht_new_flags) {                                                \
        (success) = 0;                                                  \
        break;\
    }                                                                   \
    key_t *ht_new_keys = malloc(ht_new_capacity * sizeof(key_t));       \
    if (!ht_new_keys) {                                                 \
        free(ht_new_flags);                                             \
        (success) = 0;                                                                \
        break;\
    }                                                                   \
    val_t *ht_new_vals = malloc(ht_new_capacity * sizeof(val_t));       \
    if (!ht_new_vals) {                                                 \
        free(ht_new_keys);                                              \
        free(ht_new_flags);                                             \
        (success) = 0;                                                  \
        break;\
    }                                                                   \
    memset(ht_new_flags, 0, ht_new_capacity);                           \
    size_t ht_mask = ht_new_capacity - 1;                                \
    for (size_t ht_i = 0; ht_i < (h).capacity; ht_i++) {                \
        if ((h).flags[ht_i] != OCCUPIED)                                \
            continue;                                                   \
        size_t ht_j = hash_f((h).keys[ht_i]) & ht_mask;                 \
        size_t ht_step = 0;                                             \
        while (ht_new_flags[ht_j]) {                                    \
            ht_j = (ht_j + ++ht_step) & ht_mask;                        \
        }                                                               \
        ht_new_flags[ht_j] = OCCUPIED;                                         \
        ht_new_keys[ht_j] = (h).keys[ht_i];                             \
        ht_new_vals[ht_j] = (h).vals[ht_i];                             \
    }                                                                   \
    free((h).vals);                                                     \
    free((h).keys);                                                     \
    free((h).flags);                                                    \
    (h).flags = ht_new_flags;                                           \
    (h).keys = ht_new_keys;                                             \
    (h).vals = ht_new_vals;                                             \
    (h).capacity = ht_new_capacity;                                     \
    (h).max_size = (ht_new_capacity >> 1) + (ht_new_capacity >> 2);     \
    (success) = 1;\
} while (0)

#define ht_remove(h, i) do { \
    (h).flags[(i)] = DEL;\
    (h).size--;\
} while (0)

#define ht_foreach(h, ht_i) \
for (                    \
        size_t ht_i = ht_next_valid_index((h).flags, (h).capacity, 0); \
        ht_i != ht_capacity((h)) && ht_valid((h), ht_i);               \
        ht_i++, ht_i = ht_next_valid_index((h).flags, (h).capacity, ht_i)\
)


static size_t ht_next_valid_index(const char *flags, size_t capacity, size_t index) {
    while ( index < capacity && flags[index] != OCCUPIED)
        index++;
    return index;
}




/* ************************************************* */


static inline size_t hash_f(const char *s) {
    size_t h = (size_t) *s;
    if (h) {
        for(++s; *s; ++s) {
            h = (h << 5) - h + (size_t) *s;
        }
    }
    return h;
}


int str_eq(char *a, char *b) { return  strcmp((a), (b)) == 0;}

#endif //UNTITLED3_HASHTABLE_H
