#include "maud_music.h"
#include "maud_textmanager.h"

void maud_music_init(music_t* music) {
    music->music = NULL;
    music->music_name = NULL;
    music->file_path = NULL;
    music->location_path = NULL;
    music->text_texture = NULL;
}

const char* maud_music_getextension_name(Mix_Music* music) {
    int music_type = Mix_GetMusicType(music);
    const char* pextension_name = NULL;
    static char extension_name[5] = {0};
    switch(music_type) {
        case MUS_MP3:  pextension_name = FILE_EXTENSIONS[MAUD_MP3EXT];  break;
        case MUS_FLAC: pextension_name = FILE_EXTENSIONS[MAUD_FLACEXT]; break;
        case MUS_OGG:  pextension_name = FILE_EXTENSIONS[MAUD_OGGEXT];  break;
        case MUS_OPUS: pextension_name = FILE_EXTENSIONS[MAUD_OPUSEXT]; break;
        case MUS_WAV:  pextension_name = FILE_EXTENSIONS[MAUD_WAVEXT];  break;
        default:       pextension_name = FILE_EXTENSIONS[MAUD_M4AEXT];
    }
    strcpy(extension_name, pextension_name);
    return extension_name;
}

char* maud_music_getname_frompath(Mix_Music* music, const char* path) {
    if(path == NULL) {
        return NULL;
    }
    const char* extension_name = maud_music_getextension_name(music);
    char *music_name = NULL, *filename = NULL;
    #ifdef _WIN32
    //filename = maud_widetoutf8(wcsrchr((wchar_t*)path, L'\\'));
    filename = strrchr(path, '\\');
    #else
    filename = strrchr(path, '/');
    #endif
    // Skip forward slash or backslash
    filename++;
    // Calculate music name length by subtracting extension length
    size_t extension_len = strlen(extension_name) + 1,
           filename_len = strlen(filename),
           music_namelen = filename_len - extension_len;
    if(!music_namelen) {
        music_namelen = filename_len;
    }
    // Allocate memory and copy the music name into the allocated pointer
    music_name = calloc(music_namelen + 1, sizeof(char*));
    strncpy(music_name, filename, music_namelen);
    return music_name;
}

mtime_t maud_music_gettime(double seconds) {
    mtime_t music_time = {0};

    // Calculate hours
    if(((int)seconds/3600) > 0) {
        music_time.hrs = seconds/3600;
        seconds -= music_time.hrs * 3600;
    }

    // Calculate minutes and seconds
    if(((int)seconds/60) > 0 && ((int)seconds/60) < 60) {
        music_time.mins = seconds/60;
        seconds -= music_time.mins * 60;
    }
    music_time.secs = seconds;
    return music_time;
}

bool maud_music_load(maud_t* maud, music_t* music, char* location_path,
    char* music_path, char* music_altpath) {
    maud_iteminfo_t* item_info = &maud->item_info;
    maud_tileinfo_t* tile_info = &item_info->tile_info;
    text_info_t utext = {18, NULL, NULL, tile_info->foreground_color, {songs_box.x + 2, songs_box.y + 1}};
    SDL_Rect outer_canvas = utext.text_canvas;
    bool loaded = true;
    if(music->music) {
        return loaded;
    }
    music->music = Mix_LoadMUS(music_path);
    #ifdef _WIN32
    if(!music->music) {
        music->music = Mix_LoadMUS(music_altpath);
    }
    #endif
    if(!music->music) {
        fprintf(stderr,
            "Error: Failed to load music %s\n", music_path);
        loaded = false;
    }
    music->file_path = music_path;
    music->location_path = location_path;
    if(!music->music_name) {
        music->music_name = maud_music_getname_frompath(music->music, music_path);
    }
    utext.utext = music->music_name;
    if(!music->text_texture) {
        music->text_texture = maud_textmanager_renderunicode(maud, maud->music_font, &utext);
    }
    outer_canvas.h = utext.text_canvas.h + 22, outer_canvas.w = WIDTH - scrollbar.w;
    utext.text_canvas.x = outer_canvas.x + 50,
    utext.text_canvas.y = outer_canvas.y + ((outer_canvas.h - utext.text_canvas.h) / 2);
    music->text_info = utext;
    music->outer_canvas = outer_canvas;
    music->fit = true;
    utext.text_canvas.y += utext.text_canvas.h + 22;
    outer_canvas.y += outer_canvas.h + 3;
    if(maud->music_selectionmenu_checkbox_tickall) {
        music->fill = true;
        music->checkbox_ticked = true;
        maud->tick_count++;
    }

    music->music_position = (mtime_t){0};
    music->music_durationsecs = Mix_MusicDuration(music->music);
    music->music_duration = maud_music_gettime(music->music_durationsecs);
    return loaded;
}

void maud_music_destroy(music_t* music) {
    free(music->music_name);
    Mix_FreeMusic(music->music);
    SDL_DestroyTexture(music->text_texture);
    maud_music_init(music);
}

bool maud_music_list_add(music_t** music_list, size_t* music_count, music_t* music) {
    size_t new_count = (*music_count) + 1;
    music_t* new_musics = realloc((*music_list), new_count * sizeof(music_t));
    if(!new_musics) {
        return false;
    }
    music->music_id = (*music_count);
    new_musics[new_count-1] = *music;
    *music_list = new_musics;
    *music_count = new_count;
    return true;
}

bool maud_music_list_findmusic_byid(music_t* music_list, size_t music_count,
    size_t music_id, size_t* music_index) {
    bool found = false;
    for(size_t i=0;i<music_count;i++) {
        if(music_list[i].music_id == music_id) {
            *music_index = i;
            found = true;
            break;
        }
    }
    return found;
}

void maud_music_list_shiftitem_toend(music_t** music_list, size_t* music_count,
    size_t item_index) {
    if(item_index >= (*music_count)) {
        return;
    }
    for(size_t i=item_index;i<(*music_count)-1;i++) {
        music_t curr_item = (*music_list)[i],
                next_item = (*music_list)[i+1];
        (*music_list)[i] = next_item,
        (*music_list)[i+1] = curr_item;
    }
}

bool maud_music_list_poplast(music_t** music_list, size_t* music_count) {
    if(!(*music_list)) {
        return false;
    }
    size_t new_count = (*music_count) - 1;
    if(!new_count) {
        maud_music_list_destroy(music_list, music_count);
        return true;
    }
    *music_count = new_count;
    maud_music_destroy(&(*music_list)[new_count]);
    (*music_list) = realloc((*music_list), new_count * sizeof(music_t));
    return true;
}

bool maud_music_list_findmusic_bypath(music_t* music_list, size_t music_count,
    const char* path, size_t* found_index) {
    if(!music_list) {
        return false;
    }
    bool found = false;
    for(size_t i=0;i<music_count;i++) {
        music_t* music = &music_list[i];
        bool is_location = strcmp(music->location_path, path) == 0,
             is_filepath = strcmp(music->file_path, path) == 0;
        if(is_location || is_filepath) {
            *found_index = i;
            found = true;
            break;
        }
    }
    return found;
}

bool maud_music_list_removemusics_bypath(music_t** music_list, size_t* music_count,
    const char* path) {
    bool found = false;
    size_t found_index = 0;
    while((found = maud_music_list_findmusic_bypath(*music_list, *music_count,
        path, &found_index))) {
        const char* music_name = (*music_list)[found_index].music_name;
        printf("Removing music %s from music list by path %s\n", music_name, path);
        maud_music_list_shiftitem_toend(music_list, music_count, found_index);
        maud_music_list_poplast(music_list, music_count);
    }
}

void maud_music_lists_sync(maud_t* maud) {
    maud->music_lists[0] = maud->music_list;
    maud->music_counts[0] = maud->music_count;
}

void maud_music_lists_sync_musicitems_id(maud_t* maud) {
    music_t** music_lists = maud->music_lists;
    size_t* music_counts = maud->music_counts;
    for(size_t i=0;i<2;i++) {
        music_t* music_list = music_lists[i];
        size_t music_count = music_counts[i];
        for(size_t j=0;j<music_count;j++) {
            if(music_list[j].music_id != j) {
                music_list[j].music_id = j;
            }
        }
    }
}

void maud_music_list_destroy(music_t** music_list, size_t* music_count) {
    for(size_t i=0;i<(*music_count);i++) {
        music_t* music = &(*music_list)[i];
        maud_music_destroy(music);
    }
    free(*music_list);
    *music_list = NULL;
    *music_count = 0;
}