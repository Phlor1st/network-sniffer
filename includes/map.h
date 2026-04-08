
#ifndef MAP_H
#define MAP_H

#include <stdint.h>

typedef struct mapitem {
    char *key; // указатель на строку, представляющую ключ
    uint64_t val; // накопленное число байт для ключа
} MI;

typedef struct map {
    int size;   // текущий размер карты (количество элементов)
    MI *items; // указатель на массив элементов
} M;


struct map* mapNew();
void mapAdd(char* key, uint64_t val, struct map* map);
struct mapitem* mapGet(char* key, struct map* map);
void mapPrint(struct map* map);
void mapClose(struct map* map);

#endif //MAP_H
