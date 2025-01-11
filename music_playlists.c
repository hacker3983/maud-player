#include "music_playlists.h"
#include "music_queue.h"

bool mplayer_playlist_create(mplayer_playlist_t** playlists, size_t* playlist_count, const char* playlist_name) {
    mplayer_playlist_t* new_playlists = NULL;
    size_t new_playlistcount = 0;
    if(mplayer_playlist_exists(*playlists, *playlist_count, playlist_name)) {
        return true;
    }
    // Duplicate the playlist name that was passed in a parameter
    char* playlistname_duplicate = mplayer_dupstr(playlist_name, strlen(playlist_name));
    // If a heap memory allocation failure occurs then we return false from the function
    if(!playlistname_duplicate) {
        return false;
    }
    // Allocate memory for the new playlist structure
    if(!(*playlists)) {
        new_playlists = (mplayer_playlist_t*)malloc(sizeof(mplayer_playlist_t));
    } else {
        new_playlistcount = *playlist_count;
        new_playlists = (mplayer_playlist_t*)realloc((*playlists), (new_playlistcount + 1)
            * sizeof(mplayer_playlist_t));
    }
    // If we failed to allocate memory for the new playlists structure then we return false
    if(!new_playlists) {
        return false;
    }
    // If all there was no memory allocation failure then we initialize all the properties of the newly
    // created playlist
    memset(new_playlists+new_playlistcount, 0, sizeof(mplayer_playlist_t));
    new_playlists[new_playlistcount++].name = playlistname_duplicate;
    *playlists = new_playlists, *playlist_count = new_playlistcount;
    return true;
}

bool mplayer_playlist_remove(mplayer_playlist_t** playlists, size_t* playlist_count, const char* playlist_name) {
    if(!mplayer_playlist_exists(*playlists, *playlist_count, playlist_name)) {
        return false;
    }
    size_t modified_count = (*playlist_count) ? (*playlist_count)-1 : 0, new_index = 0;
    mplayer_playlist_t* modified_playlists = (modified_count) ? calloc(modified_count,
        sizeof(mplayer_playlist_t)) : NULL;
    if(!modified_playlists) {
        return false;
    }
    for(size_t i=0;i<(*playlist_count);i++) {
        if(strcmp((*playlists)[i].name, playlist_name) == 0) {
            mplayer_playlist_destroy(&(*playlists)[i]);
            continue;
        }
        modified_playlists[new_index++] = (*playlists)[i];
    }
    free(*playlists);
    *playlists = modified_playlists, *playlist_count = modified_count;
    return true;
}

bool mplayer_playlist_exists(mplayer_playlist_t* playlists, size_t playlist_count, const char* playlist_name) {
    for(size_t i=0;i<playlist_count;i++) {
        if(strcmp(playlists[i].name, playlist_name) == 0) {
            return true;
        }
    }
    return false;
}

void mplayer_playlist_destroy(mplayer_playlist_t* playlist) {
    free(playlist->name); playlist->name = NULL;
    mplayer_queue_destroy(&playlist->queue); 
}

void mplayer_playlists_destroy(mplayer_playlist_t** playlists, size_t* playlist_count) {
    for(size_t i=0;i<(*playlist_count);i++) {
        mplayer_playlist_destroy(&(*playlists)[i]);
    }
    *playlists = NULL; *playlist_count = 0;
}