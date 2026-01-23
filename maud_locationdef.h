#ifndef _MAUD_LOCATIONLISTDEF_H
#define _MAUD_LOCATIONLISTDEF_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <stddef.h>

typedef struct music_file {
    char* altpath, *path;
} maud_file_t;

typedef struct maud_filelist {
    maud_file_t* files;
    size_t count;
} maud_filelist_t;

typedef struct music_location {
    char* path;
    maud_filelist_t files;
} maud_location_t;

typedef struct maud_location_list {
    maud_location_t* locations;
    size_t count;
} maud_locationlist_t;
#endif