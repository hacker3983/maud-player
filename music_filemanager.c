#include "music_player.h"

void mplayer_getroot_path(char* root_path) {
        size_t directory_length = GetCurrentDirectoryA(0, NULL);
        char* directory = calloc(directory_length, sizeof(char));
        GetCurrentDirectoryA(directory_length, directory);
        SetCurrentDirectoryA("/");
        GetCurrentDirectoryA(4, root_path);
        SetCurrentDirectoryA(directory);
        free(directory); directory = NULL;
}

void mplayer_getmusic_locations(mplayer_t* mplayer) {
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "r");
    if(f == NULL) {
        mplayer->location_list = NULL;
        mplayer->location_count = 0;
        return;
    }
    musloc_t* music_loclist = calloc(1, sizeof(musloc_t));
    char* music_loc = calloc(2, sizeof(char)), c = '\0';
    size_t mloc_len = 0, muslist_count = 0;
    while((c = fgetc(f)) != EOF) {
        if(c == '\n') {
            music_loclist[muslist_count++].location = music_loc;
            music_loclist = realloc(music_loclist, (muslist_count + 1) * sizeof(musloc_t));
            music_loclist[muslist_count].location = NULL;
            music_loc = calloc(2, sizeof(char));
            mloc_len = 0;
            continue;
        } 
        music_loc[mloc_len++] = c;
        char* temp = music_loc;
        music_loc = calloc(mloc_len + 1, sizeof(char));
        strncpy(music_loc, temp, mloc_len);
        free(temp);
    }
    (mloc_len == 0) ? free(music_loc) : (music_loclist[muslist_count++].location = music_loc);
    music_loc = NULL;
    fclose(f);

    // in case the file is empty we free the pointers and set them to NULL
    if(mloc_len == 0) {
        free(music_loclist);
        music_loclist = NULL;
    }

    mplayer->location_list = music_loclist;
    mplayer->location_count = muslist_count;
}

bool mplayer_musiclocation_exists(mplayer_t* mplayer, char* location) {
    if(mplayer->location_count == 0) {
        mplayer_getmusic_locations(mplayer);
    }
    for(size_t i=0;i<mplayer->location_count;i++) {
        if(strcmp(mplayer->location_list[i].location, location) == 0) {
            return true;
        }
    }
    return false;
}

void mplayer_addmusic_location(mplayer_t* mplayer, char* location) {
    FILE* f = fopen(MUSIC_PATHINFO_FILE, "a+");
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);
    if(size > 0) {
        if(mplayer_musiclocation_exists(mplayer, location)) {
            fclose(f);
            return;
        }
        fputs("\n", f);
    }
    fwrite(location, sizeof(char), strlen(location), f);
    fclose(f);
    mplayer_freemusic_locations(mplayer);
    mplayer_getmusic_locations(mplayer);
}

music_t* mplayer_loadmusics(mplayer_t* mplayer) {
}

void mplayer_freemusic_locations(mplayer_t* mplayer) {
    for(size_t i=0;i<mplayer->location_count;i++) {
        free(mplayer->location_list[i].location);
    }
    free(mplayer->location_list);
}