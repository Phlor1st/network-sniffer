#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../includes/map.h"
#include "../includes/selection_sort.h"


M *mapNew() {
    M *map;

    map = malloc(sizeof(M));
    map->size = 0;
    map->items = NULL;

    return map;
}

void mapAdd(char *key, uint64_t val, M *map) { // добавление нового элемента
    MI *findElement;

    findElement = mapGet(key, map);

    // ключ уже существует, увеличиваем значение на val
    if (findElement) {
        findElement->val += val;
    } else {
        char *newkey;

        newkey = malloc(strlen(key) + 1);
        strcpy(newkey, key); // Копируем ключ в выделенную память

        if (map->size == 0) {
            map->items = malloc(sizeof(MI));
        } else {
            map->items = realloc(map->items, sizeof(MI) * (map->size + 1));
        }

        (map->items + map->size)->key = newkey;
        (map->items + map->size)->val = val;
        map->size++;
    }
}


MI *mapGet(char *key, M *map) {
    int i;

    for (i = 0; i < map->size; i++) {
        if (strcmp((map->items + i)->key, key) == 0) {
            return (map->items + i); // Возвращаем указатель на найденный элемент
        }
    }

    return NULL;
}

void mapPrint(M *map) {
    int i;
    char *istr;


    printf("%-20s%-20s%-20s%-10s\n", "Source Ip", "Destination Ip", "Protocol", "Bytes");

    //Сортировка
    for (int idx = 0; idx < map->size; idx++) {
        for (int minIdx = idx + 1; minIdx < map->size; minIdx++) {
            if ((map->items + idx)->val < (map->items + minIdx)->val)
                swap(map->items + idx, map->items + minIdx, sizeof(MI));
        }
    }

    for (i = 0; i < map->size; i++) {
        istr = strtok((map->items + i)->key, "|");
        while (istr != NULL) {
            printf("%-20s", istr);
            istr = strtok(NULL, "|");
        }
        printf("%-10llu\n", (unsigned long long)(map->items + i)->val);
    }
}

void mapClose(M *map) {
    int i = 0;

    for (; i < map->size; i++) {
        free((map->items + i)->key);
    }

    free(map->items);
    free(map);
}
