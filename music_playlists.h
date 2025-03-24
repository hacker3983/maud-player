#ifndef _MUSIC_PLAYLIST_H
#define _MUSIC_PLAYLIST_H
#include <stdlib.h>
#include <stdbool.h>
#include "music_queuedef.h"

// A structure representing the actual playlist reference
typedef struct music_playlist {
    char* name; // The name of the playlist
    music_queue_t queue; // A queue of song indices from either the temporary or main music list
} mplayer_playlist_t;

// Create a new playlist as long as it doesn't exist
bool mplayer_playlist_create(mplayer_playlist_t** playlists, size_t* playlist_count, const char* playlist_name);
// Determine whether a playlist exists or not
bool mplayer_playlist_exists(mplayer_playlist_t* playlists, size_t playlist_count, const char* playlist_name);
// Rename a playlist
bool mplayer_playlist_rename(mplayer_playlist_t** playlists, size_t playlist_count, const char* playlist_name, const char* change_name);
// Remove a playlist from the list of playlists
bool mplayer_playlist_remove(mplayer_playlist_t** playlists, size_t* playlist_count, const char* playlist_name);
// Add a music from queue to the playlist
void mplayer_playlist_addmusicfromqueue(mplayer_playlist_t** playlists, size_t playlist_count, const char* playlist_name);
// Free the memory allocated used by a playlist
void mplayer_playlist_destroy(mplayer_playlist_t* playlist);
// Free all the memory used by the playlists
void mplayer_playlists_destroy(mplayer_playlist_t** playlists, size_t* playlist_count);
#endif