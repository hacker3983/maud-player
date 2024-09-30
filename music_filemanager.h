#ifndef _MUSIC_FILEMANAGER_H
#define _MUSIC_FILEMANAGER_H
#include "music_player.h"
#include "music_extensions.h"
#include "music_textmanager.h"

#ifdef _WIN32
void mplayer_filemanager_getroot_path(char* root_path);
#endif
mtime_t mplayer_filemanager_music_gettime(double seconds);
void mplayer_filemanager_addmusic_location(mplayer_t* mplayer, void* locationv);
void mplayer_filemanager_removemusics_pendingremoval(mplayer_t* mplayer);
void mplayer_filemanager_delmusic_locationindex(mplayer_t* mplayer, size_t loc_index);
void mplayer_filemanager_browsefolder(mplayer_t* mplayer);
bool mplayer_filemanager_musiclocation_exists(mplayer_t* mplayer, void* locationv);
void mplayer_filemanager_getmusic_locations(mplayer_t* mplayer);
void mplayer_filemanager_getmusic_filepaths(mplayer_t* mplayer);
void mplayer_filemanager_getmusicpath_info(mplayer_t* mplayer);
char* mplayer_filemanager_getmusic_namefrompath(Mix_Music* music, void* path);
void mplayer_filemanager_copymusicinfo_fromsearchindex(mplayer_t* mplayer, size_t index, music_t* music_info);
void mplayer_filemanager_loadmusics(mplayer_t* mplayer);
void mplayer_filemanager_freemusic(music_t* music_ref);
void mplayer_filemanager_freemusic_searchresult(music_t** music_results, size_t* music_resultcount);
void mplayer_filemanager_freemusic_list(mplayer_t* mplayer);
void mplayer_filemanager_freemusicpath_info(mplayer_t* mplayer);
#endif