#include "music_playlistmanager.h"

void mplayer_playlistmanager_write_data_tofile(mplayer_t* mplayer) {
    FILE* f = fopen(MUSIC_PLAYLISTSINFO_FILE, "w");
    mplayer_playlist_t* playlists = mplayer->playlist_manager.playlists;
    size_t playlist_count = mplayer->playlist_manager.playlist_count;
    for(size_t i=0;i<playlist_count;i++) {
        mplayer_playlistmanager_write_playlist_tofile(mplayer, f, playlists[i]);
        if(i != playlist_count-1) {
            fputc('\n', f);
        }
    }
    fclose(f);
}

void mplayer_playlistmanager_write_playlist_tofile(mplayer_t* mplayer, FILE* f, mplayer_playlist_t playlist) {
    // Write the playlist name to the file
    mplayer_playlistmanager_write_escapedstring_tofile(f, playlist.name);
    fputs(": ", f);
    // Write the contents of the playlist 
    music_queue_t queue = playlist.queue;
    for(size_t i=0;i<queue.item_count;i++) {
        size_t music_listindex = queue.items[i].music_listindex, music_id = 0;
        for(size_t j=0;j<queue.items[i].music_count;j++) {
            music_id = queue.items[i].music_ids[j];
            char* music_name = mplayer_playlistmanager_getmusicnamefrom_index(mplayer, music_listindex,
                music_id);
            mplayer_playlistmanager_write_escapedstring_tofile(f, music_name);
            if(j != queue.items[i].music_count-1) {
                fputs(", ", f);
            }               
        }
    }
}

void mplayer_playlistmanager_write_escapedstring_tofile(FILE* f, const char* string) {
    size_t string_length = strlen(string);
    fputc('"', f);
    for(size_t i=0;i<string_length;i++) {
        switch(string[i]) {
            case '"':
                fputs("\\\"", f);
                break;
            case ':':
                fputs("\\:", f);
                break;
            case ',':
                fputs("\\,", f);
                break;
            case '[':
                fputs("\\[", f);
                break;
            case ']':
                fputs("\\]", f);
                break;
            default:
                fputc(string[i], f);
        }
    }
    fputc('"', f);
}