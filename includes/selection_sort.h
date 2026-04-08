
#ifndef SELECTION_SORT_H
#define SELECTION_SORT_H

#include <stdlib.h>

void selection_sort(void *arr, size_t len, size_t size, int (*compar)(const void *, const void *));

int compareInt(const void *a, const void *b);
int compareFloat(const void *a, const void *b);
int compareChar(const void *a, const void *b);
void swap(void *a, void *b, size_t size);

int testEmpty();
int testInt();
int testFloat();
int testChar();

#endif //SELECTION_SORT_H
