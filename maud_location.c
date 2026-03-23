#include "maud_string.h"
#include "maud_location.h"

bool maud_location_init(maud_location_t* location, const char* path) {
    char* path_dupstr = NULL;
    if(path) {
        path_dupstr = maud_dupstr(path, strlen(path));
        if(!path_dupstr) {
            return false;
        }
    }
    location->path = path_dupstr;
    maud_file_list_load_files(&location->files, path);
    return true;
}

void maud_location_destroy(maud_location_t* location) {
    free(location->path);
    location->path = NULL;
    maud_file_list_destroy(&location->files);
}

void maud_location_list_init(maud_locationlist_t* list) {
    list->locations = NULL;
    list->count = 0;
}

bool maud_location_list_locationexists(maud_locationlist_t* list, const char* path) {
    if(!list->locations) {
        return false;
    }
    for(size_t i=0;i<list->count;i++) {
        if(strcmp(list->locations[i].path, path) == 0) {
            return true;
        }
    }
    return false;
}

void maud_location_list_addlocation(maud_locationlist_t* list, const char* path) {
    if(!path) {
        return;
    }
    if(maud_location_list_locationexists(list, path)) {
        return;
    }
    maud_location_t new_location = {0};
    if(!maud_location_init(&new_location, path)) {
        return;
    }
    size_t new_count = list->count + 1;
    maud_location_t* new_locations = realloc(list->locations, new_count *
        sizeof(maud_location_t));
    if(!new_locations) {
        maud_location_destroy(&new_location);
        return;
    }
    new_locations[list->count] = new_location;
    list->locations = new_locations;
    list->count = new_count;
}

void maud_location_list_removelocation(maud_locationlist_t* list, size_t location_index) {
    if(!list->locations) {
        return;
    }
    if(location_index >= list->count) {
        return;
    }
    for(size_t i=location_index;i<list->count-1;i++) {
        maud_location_t current = list->locations[i];
        list->locations[i] = list->locations[i+1],
        list->locations[i+1] = current;
    }
    list->count--;
    if(!list->count) {
        maud_location_list_destroy(list);
        return;
    }
    maud_location_destroy(&list->locations[list->count]);
    list->locations = realloc(list->locations, list->count * sizeof(maud_location_t));
}


void maud_location_list_load_data(maud_locationlist_t* list) {
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "r");
    if(!f) {
        fprintf(stderr, "Error: %s file doesn't exist: Creating %s file...\n",
            MUSIC_PATHINFO_FILE, MUSIC_PATHINFO_FILE);
        return;
    }
    char c = '\0';
    char* location_path = NULL;
    size_t location_len = 0;
    while((c = fgetc(f)) != EOF) {
        if(c == '\n' && location_len > 0) {
            maud_location_list_addlocation(list, location_path);
            free(location_path);
            location_path = NULL;
            location_len = 0;
            continue;
        } else if(c == '\n' && location_len == 0) {
            continue;
        }
        maud_concatstr(&location_path, (const char[]){c, '\0'});
        location_len++;
    }
    if(location_path) {
        maud_location_list_addlocation(list, location_path);
        free(location_path);
        location_len = 0;
    }
    fclose(f);
}

void maud_location_list_write_data(maud_locationlist_t* list) {
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "w");
    for(size_t i=0;i<list->count;i++) {
        fprintf(f, "%s", list->locations[i].path);
        if(i != list->count-1) {
            fputc('\n', f);
        }
    }
    fclose(f);
}

void maud_location_list_destroy(maud_locationlist_t* list) {
    for(size_t i=0;i<list->count;i++) {
        maud_location_destroy(&list->locations[i]);
    }
    free(list->locations);
    maud_location_list_init(list);
}