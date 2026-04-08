
#ifndef MAP_H
#define MAP_H


typedef struct mapitem {
    char *key; // указатель на строку, представляющую ключ
    void *val; // указатель на значение (любого типа)
    int type; // тип значения
} MI;

typedef struct map {
    int size;   // текущий размер карты (количество элементов)
    MI *items; // указатель на массив элементов
} M;


struct map* mapNew();
void mapAdd(char* key, void* val, struct map* map);
struct mapitem* mapGet(char* key, struct map* map);
void mapPrint(struct map* map);
void mapClose(struct map* map);

#endif //MAP_H
