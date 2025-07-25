#ifndef _MUSIC_PLAYLIST_H
#define _MUSIC_PLAYLIST_H
#include <stdlib.h>
#include <stdbool.h>
#include "maud_queuedef.h"

// A structure representing the actual playlist reference
typedef struct maud_playlist {
    char* name; // The name of the playlist
    maud_queue_t queue; // A queue of song indices from either the temporary or main music list
} maud_playlist_t;

// Create a new playlist as long as it doesn't exist
bool maud_playlist_create(maud_playlist_t** playlists, size_t* playlist_count, const char* playlist_name);
// Determine whether a playlist exists or not
bool maud_playlist_exists(maud_playlist_t* playlists, size_t playlist_count, const char* playlist_name);
// Rename a playlist
bool maud_playlist_rename(maud_playlist_t** playlists, size_t playlist_count, const char* playlist_name, const char* change_name);
// Remove a playlist from the list of playlists
bool maud_playlist_remove(maud_playlist_t** playlists, size_t* playlist_count, const char* playlist_name);
// Add a music from queue to the playlist
void maud_playlist_addmusicfromqueue(maud_playlist_t** playlists, size_t playlist_count, const char* playlist_name);
// Free the memory allocated used by a playlist
void maud_playlist_destroy(maud_playlist_t* playlist);
// Free all the memory used by the playlists
void maud_playlists_destroy(maud_playlist_t** playlists, size_t* playlist_count);
#endif