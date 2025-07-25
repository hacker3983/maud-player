#include "maud_playlists.h"
#include "maud_queue.h"

bool maud_playlist_create(maud_playlist_t** playlists, size_t* playlist_count, const char* playlist_name) {
    maud_playlist_t* new_playlists = NULL;
    size_t new_playlistcount = 0;
    if(maud_playlist_exists(*playlists, *playlist_count, playlist_name)) {
        return true;
    }
    // Duplicate the playlist name that was passed in a parameter
    char* playlistname_duplicate = maud_dupstr(playlist_name, strlen(playlist_name));
    // If a heap memory allocation failure occurs then we return false from the function
    if(!playlistname_duplicate) {
        return false;
    }
    // Allocate memory for the new playlist structure
    if(!(*playlists)) {
        new_playlists = (maud_playlist_t*)malloc(sizeof(maud_playlist_t));
    } else {
        new_playlistcount = *playlist_count;
        new_playlists = (maud_playlist_t*)realloc((*playlists), (new_playlistcount + 1)
            * sizeof(maud_playlist_t));
    }
    // If we failed to allocate memory for the new playlists structure then we return false
    if(!new_playlists) {
        return false;
    }
    // If all there was no memory allocation failure then we initialize all the properties of the newly
    // created playlist
    memset(new_playlists+new_playlistcount, 0, sizeof(maud_playlist_t));
    new_playlists[new_playlistcount++].name = playlistname_duplicate;
    *playlists = new_playlists, *playlist_count = new_playlistcount;
    return true;
}

bool maud_playlist_rename(maud_playlist_t** playlists, size_t playlist_count, const char* playlist_name, const char* new_name) {
    if(!maud_playlist_exists(*playlists, playlist_count, playlist_name)) {
        return false;
    }
    for(size_t i=0;i<playlist_count;i++) {
        if(strcmp((*playlists)[i].name, playlist_name) == 0) {
            free((*playlists)[i].name);
            size_t new_namelen = strlen(new_name);
            (*playlists)[i].name = calloc(new_namelen + 1, sizeof(char));
            strcpy((*playlists)[i].name, new_name);
            return true;
        }
    }
    return false;
}

bool maud_playlist_remove(maud_playlist_t** playlists, size_t* playlist_count, const char* playlist_name) {
    if(!maud_playlist_exists(*playlists, *playlist_count, playlist_name)) {
        return false;
    }
    size_t modified_count = (*playlist_count) ? (*playlist_count)-1 : 0, new_index = 0;
    maud_playlist_t* modified_playlists = (modified_count) ? calloc(modified_count,
        sizeof(maud_playlist_t)) : NULL;
    for(size_t i=0;i<(*playlist_count);i++) {
        if(strcmp((*playlists)[i].name, playlist_name) == 0) {
            maud_playlist_destroy(&(*playlists)[i]);
            continue;
        }
        modified_playlists[new_index++] = (*playlists)[i];
    }
    free(*playlists);
    *playlists = modified_playlists, *playlist_count = modified_count;
    return true;
}

bool maud_playlist_exists(maud_playlist_t* playlists, size_t playlist_count, const char* playlist_name) {
    for(size_t i=0;i<playlist_count;i++) {
        if(strcmp(playlists[i].name, playlist_name) == 0) {
            return true;
        }
    }
    return false;
}

void maud_playlist_destroy(maud_playlist_t* playlist) {
    free(playlist->name); playlist->name = NULL;
    maud_queue_destroy(&playlist->queue); 
}

void maud_playlists_destroy(maud_playlist_t** playlists, size_t* playlist_count) {
    for(size_t i=0;i<(*playlist_count);i++) {
        maud_playlist_destroy(&(*playlists)[i]);
    }
    *playlists = NULL; *playlist_count = 0;
}