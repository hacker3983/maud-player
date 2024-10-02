#ifndef _MUSIC_PLAYLISTMANAGER_H
#define _MUSIC_PLAYLISTMANAGER_H
#include "music_player.h"
#include "music_filemanager.h"

mplayer_playlist_t mplayer_playlist_create(const char* playlist_name);
bool mplayer_playlist_addmusic(mplayer_playlist_t* playlist, music_t* music_ref);
bool mplayer_playlist_removemusic(mplayer_playlist_t* playlist, music_t* music_ref);
void mplayer_playlist_destroy(mplayer_playlist_t* playlist);
#endif