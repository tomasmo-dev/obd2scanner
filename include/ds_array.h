#ifndef DS_ARRAY_H
#define DS_ARRAY_H

#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 5
#define EXPAND_BY 1.7

#define HEADER_PTR(arr) ((header *)(arr) - 1)
#define CALCULATE_SIZE(h, cnt) (sizeof(header) + ((cnt) * (h)->elemSize))

#define DS_COUNT(arr) ((arr) ? HEADER_PTR((arr))->count : 0)
#define DS_CAPACITY(arr) ((arr) ? HEADER_PTR((arr))->capacity : 0)

typedef struct {
  int count;
  int capacity;

  size_t elemSize;
} header;

#if defined(__cplusplus)
  #define __typeof__(x) decltype(x)
#endif

#define ds_push(array, value)                                                  \
  do {                                                                         \
    if ((array) == NULL) {                                                     \
      size_t eSize = sizeof((array)[0]);                                       \
      header *h_init = (header *)malloc(sizeof(header) + (eSize * INITIAL_CAPACITY));    \
      memset(h_init, 0, sizeof(header) + (eSize * INITIAL_CAPACITY));          \
      h_init->capacity = INITIAL_CAPACITY;                                     \
      h_init->elemSize = eSize;                                                \
      h_init->count = 0;                                                       \
      (array) = (__typeof__(array))(h_init + 1);                               \
    }                                                                          \
                                                                               \
    header *h = HEADER_PTR((array));                                           \
    if (h->count + 1 >= h->capacity) {                                         \
      int old_cap = h->capacity;                                               \
      int new_cap = (int)(old_cap * EXPAND_BY) + 1;                            \
      size_t new_total_size = sizeof(header) + (new_cap * h->elemSize);        \
      header *new_h = (header *)realloc(h, new_total_size);                    \
      if (new_h) {                                                             \
        h = new_h;                                                             \
        h->capacity = new_cap;                                                 \
        (array) = (__typeof__(array))(h + 1);                                  \
        /* Zero out ONLY the new tail portion */                               \
        size_t bytes_to_zero = (new_cap - old_cap) * h->elemSize;              \
        memset((char *)(array) + (old_cap * h->elemSize), 0, bytes_to_zero);   \
      }                                                                        \
    }                                                                          \
    (array)[h->count] = (value);                                               \
    HEADER_PTR(array)->count++;                                                \
  } while (0)

#define ds_free(arr)                                                           \
  do {                                                                         \
    void *ptr = HEADER_PTR((arr));                                             \
    free(ptr);                                                                 \
    arr = NULL;                                                                \
  } while (0)

#define ds_push_many(array, elements, count)                                   \
  do {                                                                         \
    for (size_t i = 0; i < (count); i++) {                                     \
      ds_push((array), (elements)[i]);                                         \
    }                                                                          \
  } while (0)

#define ds_push_many_nt(array, elements)                                       \
  do {                                                                         \
    for (size_t i = 0;; i++) {                                                 \
      if ((elements)[i] == 0)                                                  \
        break;                                                                 \
      ds_push((array), (elements)[i]);                                         \
    }                                                                          \
  } while (0)

#define ds_shrink(array)                                                       \
  do {                                                                         \
    header *h = HEADER_PTR((array));                                           \
    int cap = h->capacity;                                                     \
    int cnt = h->count;                                                        \
                                                                               \
    if (cap > cnt) {                                                           \
      header *temp_h = realloc(h, CALCULATE_SIZE((h), (cnt)));                 \
      if (temp_h) {                                                            \
        h = temp_h;                                                            \
        h->capacity = cnt;                                                     \
        (array) = (__typeof__(array))(h + 1);                                  \
      }                                                                        \
    }                                                                          \
                                                                               \
  } while (0)

#define ds_reserve(array, cap)                                                 \
  do {                                                                         \
    header *h = HEADER_PTR((array));                                           \
    size_t old_cap = h->capacity;                                              \
                                                                               \
    if ((cap) > h->capacity) {                                                 \
      header *tmp_h = realloc(h, CALCULATE_SIZE((h), (cap)));                  \
      if (tmp_h) {                                                             \
        h = tmp_h;                                                             \
        h->capacity = (cap);                                                   \
        (array) = (__typeof__(array))(h + 1);                                  \
        h = HEADER_PTR((array));                                               \
        size_t uninitialized_b = ((cap) - old_cap);                            \
        char *ini_offset = (char *)(array) + (old_cap * h->elemSize);          \
        memset(ini_offset, 0, uninitialized_b * h->elemSize);                  \
      }                                                                        \
    }                                                                          \
  } while (0)

#define ds_zero_fill(array)                                                    \
  do {                                                                         \
    header *h = HEADER_PTR((array));                                           \
    memset((char *)(array), 0, (size_t)(h->capacity * h->elemSize));           \
  } while (0)

#define ds_pop(array)                                                          \
  ({                                                                           \
    header *__h = HEADER_PTR((array));                                         \
    __h->count--;                                                              \
    (array)[__h->count];                                                       \
  })

#define ds_remove_last(array)                                                  \
  do {                                                                         \
    HEADER_PTR(array)->count--;                                                \
  } while (0)

#endif // DS_ARRAY_H