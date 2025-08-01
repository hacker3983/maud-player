#include "maud_playlistmanager.h"

void maud_playlistmanager_write_data_tofile(maud_t* maud) {
    FILE* f = fopen(MUSIC_PLAYLISTSINFO_FILE, "w");
    maud_playlist_t* playlists = maud->playlist_manager.playlists;
    size_t playlist_count = maud->playlist_manager.playlist_count;
    for(size_t i=0;i<playlist_count;i++) {
        maud_playlistmanager_write_playlist_tofile(maud, f, playlists[i]);
        if(i != playlist_count-1) {
            fputc('\n', f);
        }
    }
    fclose(f);
}

void maud_playlistmanager_write_playlist_tofile(maud_t* maud, FILE* f, maud_playlist_t playlist) {
    // Write the playlist name to the file
    maud_playlistmanager_write_escapedstring_tofile(f, playlist.name);
    fputs(": ", f);
    // Write the contents of the playlist 
    maud_queue_t queue = playlist.queue;
    for(size_t i=0;i<queue.item_count;i++) {
        size_t music_listindex = queue.items[i].music_listindex, music_id = queue.items[i].music_id;
        char* music_name = maud_playlistmanager_getmusicnamefrom_index(maud, music_listindex,
            music_id);
        maud_playlistmanager_write_escapedstring_tofile(f, music_name);
        if(i != queue.item_count-1) {
            fputs(", ", f);
        }
    }
}

void maud_playlistmanager_write_escapedstring_tofile(FILE* f, const char* string) {
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
            case '\\':
                fputs("\\\\", f);
                break;
            default:
                fputc(string[i], f);
        }
    }
    fputc('"', f);
}