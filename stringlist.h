#ifndef _STRINGLIST_H
#define _STRINGLIST_H
#include "maud_string.h"

typedef struct stringlist {
    char** strings;
    size_t count;
} stringlist_t;

void stringlist_init(stringlist_t* string_list);
bool stringlist_append(stringlist_t* string_list, char* string);
void stringlist_destroy(stringlist_t* string_list);
#endif