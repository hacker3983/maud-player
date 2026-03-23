#ifndef _MAUD_MUSIC_H
#define _MAUD_MUSIC_H
#include "maud_player.h"
#include "maud_extensions.h"

void maud_music_init(music_t* music);
const char* maud_music_getextension_name(Mix_Music* music);
char* maud_music_getname_frompath(Mix_Music* music, const char* path);
bool maud_music_load(maud_t* maud, music_t* music, char* location_path,
    char* music_path, char* music_altpath);
mtime_t maud_music_gettime(double seconds);
void maud_music_destroy(music_t* music);

bool maud_music_list_add(music_t** music_list, size_t* music_count, music_t* music);
bool maud_music_list_poplast(music_t** music_list, size_t* music_count);
void maud_music_list_shiftitem_toend(music_t** music_list, size_t* music_count,
    size_t item_index);
bool maud_music_list_findmusic_byid(music_t* music_list, size_t music_count,
    size_t music_id, size_t* music_index);
bool maud_music_list_findmusic_bypath(music_t* music_list, size_t music_count,
    const char* path, size_t* found_index);
bool maud_music_list_removemusics_bypath(music_t** music_list, size_t* music_count,
    const char* path);
void maud_music_lists_sync(maud_t* maud);
void maud_music_lists_sync_musicitems_id(maud_t* maud);
void maud_music_list_destroy(music_t** music_list, size_t* music_count);
#endif