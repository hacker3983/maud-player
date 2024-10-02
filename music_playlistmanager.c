#include "music_playlistmanager.h"

mplayer_playlist_t mplayer_playlist_create(const char* playlist_name) {
    mplayer_playlist_t new_playlist = {0};
    size_t playlist_namelen = strlen(playlist_name);
    new_playlist.name = mplayer_dupstr(playlist_name, playlist_namelen);
    new_playlist.music_list = NULL;
    new_playlist.music_count = NULL;
    return new_playlist;   
}

bool mplayer_playlist_addmusic(mplayer_playlist_t* playlist, music_t* music_ref) {
    music_t* new_musiclist = NULL;
    if(!playlist->music_list) {
        new_musiclist = calloc(1, sizeof(music_t));
    } else {
        new_musiclist = realloc(playlist->music_list, (playlist->music_count+1) * sizeof(music_t));
        memset(new_musiclist+playlist->music_count, 0, playlist->music_count+1);
    }
    if(!new_musiclist) {
        return false;
    }
    new_musiclist[playlist->music_count].music = music_ref->music;
    new_musiclist[playlist->music_count].music_name = music_ref->music_name;
    new_musiclist[playlist->music_count].music_path = music_ref->music_path;
    new_musiclist[playlist->music_count].text_info = music_ref->text_info;
    #ifdef _WIN32
    new_musiclist[playlist->music_count].music_alternatepath = music_ref->music_alternatepath;
    #endif
    new_musiclist[playlist->music_count].music_durationsecs = music_ref->music_durationsecs;
    new_musiclist[playlist->music_count].music_duration = music_ref->music_duration;
    new_musiclist[playlist->music_count].checkbox_size = music_ref->checkbox_size;
    new_musiclist[playlist->music_count].location_index = music_ref->location_index;
    playlist->music_list = new_musiclist;
    playlist->music_count++;
    return true;
}

void mplayer_playlist_destroy(mplayer_playlist_t* playlist) {
    free(playlist->name); playlist->name = NULL;
    for(size_t i=0;i<playlist->music_count;i++) {
        mplayer_filemanager_freemusic(&playlist->music_list[i]);
    }
    playlist->music_list = NULL;
    playlist->music_count = 0;
}