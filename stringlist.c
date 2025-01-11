#include "stringlist.h"

void stringlist_init(stringlist_t* string_list) {
    string_list->strings = NULL;
    string_list->count = 0;
}

bool stringlist_append(stringlist_t* string_list, char* string) {
    char** new_listptr = NULL;
    if(!string_list->strings) {
        new_listptr = (char**)malloc(sizeof(char*));
    } else {
        new_listptr = (char**)realloc(string_list->strings, (string_list->count+1) * sizeof(char*));  
    }
    if(!new_listptr) {
        return false;
    }
    new_listptr[string_list->count++] = string;
    string_list->strings = new_listptr;
    return true;
}

void stringlist_destroy(stringlist_t* string_list) {
    for(size_t i=0;i<string_list->count;i++) {
        free(string_list->strings[i]); string_list->strings[i] = NULL;
    }
    free(string_list->strings);
    stringlist_init(string_list);
}