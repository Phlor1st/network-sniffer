#include <stdlib.h>
#include <string.h>

#include "../includes/map.h"

int compareInt(const void *a, const void *b) {
    int *tmpA = (int *)a;
    int *tmpB = (int *)b;
    return *tmpA < *tmpB;
}

int compareFloat(const void *a, const void *b) {
    float *tmpA = (float *)a;
    float *tmpB = (float *)b;
    return *tmpA < *tmpB;
}

int compareChar(const void *a, const void *b) {
    char *tmpA = (char *)a;
    char *tmpB = (char *)b;
    return *tmpA < *tmpB;
}


void swap(void *a, void *b, size_t size) {
    char *tmpB = malloc(size);
    memcpy(tmpB, b, size);
    memcpy(b, a, size);
    memcpy(a, tmpB, size);
    free(tmpB);
}

void selection_sort(void *arr, size_t len, size_t size, int (*compar)(const void *, const void *)) {
    for (int idx = 0; idx < len; idx++) {
        for (int minIdx = idx + 1; minIdx < len; minIdx++) {
            if (compar((char *)arr + idx * size, (char *)arr + minIdx * size) == 0) {
                swap((char *)arr + idx * size, (char *)arr + minIdx * size, size);
            }
        }

    }
}
