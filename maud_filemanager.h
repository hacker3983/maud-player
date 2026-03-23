#ifndef _MAUD_FILEMANAGER_H
#define _MAUD_FILEMANAGER_H
#include "maud_player.h"
#include "maud_location.h"
#include "maud_music.h"
#include "maud_queue.h"

#ifdef _WIN32
void maud_filemanager_getroot_path(char* root_path);
#endif
mtime_t maud_filemanager_music_gettime(double seconds);
void maud_filemanager_addmusic_location(maud_t* maud, const char* path);
void maud_filemanager_delmusic_locationindex(maud_t* maud, size_t loc_index);
void maud_filemanager_browsefolder(maud_t* maud);
void maud_filemanager_getmusic_locations(maud_t* maud);
const char* maud_filemanager_getextension_name(Mix_Music* music);
char* maud_filemanager_getmusic_namefrompath(Mix_Music* music, const char* path);
void maud_filemanager_copymusicinfo_fromsearchindex(maud_t* maud, size_t index, music_t* music_info);
void maud_filemanager_addmusic(maud_t* maud, music_t** music_list,
    size_t* music_count, music_t* music);
void maud_filemanager_loadmusic(maud_t* maud, music_t* music, char* location_path,
    maud_file_t* file);
void maud_filemanager_loadmusics(maud_t* maud);
void maud_filemanager_freemusic(music_t* music_ref);
void maud_filemanager_freemusic_searchresult(music_t** music_results, size_t* music_resultcount);
void maud_filemanager_freemusic_list(maud_t* maud);
void maud_filemanager_freemusicpath_info(maud_t* maud);
#endif