#include "music_playlistmanager.h"

bool mplayer_playlistmanager_read_datafile(mplayer_t* mplayer) {
    stringlist_t string_list = {0};
    FILE* playlist_datafile = fopen(MUSIC_PLAYLISTSINFO_FILE, "r");
    if(!playlist_datafile) {
        return false;
    }
    char c = '\0';
    while((c = fgetc(playlist_datafile)) != EOF) {
        if(c == '"' || c == '[') {
            char* string = mplayer_playlistmanager_getstring(playlist_datafile);
            stringlist_append(&string_list, string);
        } else if(c == '\n') {
            mplayer_playlistmanager_createplaylist_fromparsed_data(mplayer, &string_list);
            stringlist_destroy(&string_list);
        }
    }
    if(string_list.strings) {
        mplayer_playlistmanager_createplaylist_fromparsed_data(mplayer, &string_list);
        stringlist_destroy(&string_list);
    }
    return true;
}

char* mplayer_playlistmanager_getstring(FILE* playlist_datafile) {
    char* playlist_name = NULL, c = '\0';
    bool escape = false;
    while((c = fgetc(playlist_datafile)) != EOF) {
        if(c == '"' || c == ']' || c == '\n') {
            break;
        } else if(c == '\\') {
            c = fgetc(playlist_datafile);
        } else if(c == ',') {
            continue;
        }
        mplayer_concatstr(&playlist_name, (char[2]){c, '\0'});
    }
    return playlist_name;
}

char* mplayer_playlistmanager_parsemusic_name(char* music_name, size_t* list_index) {
    size_t music_namelen = strlen(music_name), parsed_namelen = 0;
    char* parsed_name = NULL;
    if(music_name[0] == '[') {
        *list_index = 1;
        parsed_namelen = music_namelen - 2;
        parsed_name = (char*)malloc(music_namelen - 1);
    } else {
        return music_name;
    }
    if(parsed_name) {
        parsed_name[parsed_namelen] = '\0';
        strncpy(parsed_name, music_name+1, parsed_namelen);
    }
    return parsed_name;
}

void mplayer_playlistmanager_createplaylist_fromparsed_data(mplayer_t* mplayer, stringlist_t* string_list) {
    if(!string_list->strings) {
        return;
    }
    char* playlist_name = string_list->strings[0];
    mplayer_playlistmanager_createplaylist(mplayer, playlist_name);
    for(size_t i=1;i<string_list->count;i++) {
        size_t music_listindex = 0, music_id = 0;
        mplayer_playlistmanager_getmusicindex_byname(mplayer, string_list->strings[i], &music_listindex,
            &music_id);
        mplayer_playlistmanager_addmusic_toplaylist(mplayer, playlist_name, music_listindex, music_id);
    }
}