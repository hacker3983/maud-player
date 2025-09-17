#include "maud_playlistmanager.h"
#include "maud_songsmanager.h"
#include "maud_playerinfo_extern.h"
#include "maud_string.h"
#include "maud_scrollcontainer.h"
#include "maud_notification.h"

int maud_playlistmanager_findmaxheight(SDL_Rect* rects, size_t rect_count) {
    if(!rects) {
        return 0;
    }
    int maximum_height = rects[0].h;
    for(size_t i=1;i<rect_count;i++) {
        if(rects[i].h > maximum_height) {
            maximum_height = rects[i].h;
        }
    }
    return maximum_height;
}

char* maud_playlistmanager_getmusicnamefrom_index(maud_t* maud, size_t music_listindex,
    size_t music_id) {
    return maud->music_lists[music_listindex][music_id].music_name;
}

void maud_playlistmanager_getmusicindex_byname(maud_t* maud, char* music_name,
    size_t* music_listindex, size_t* music_id) {
    size_t music_namelen = strlen(music_name), list_index = 0;
    char* parsed_name = maud_playlistmanager_parsemusic_name(music_name, &list_index);
    music_t* music_list = maud->music_lists[list_index];
    size_t music_count = maud->music_counts[list_index];
    for(size_t i=0;i<music_count;i++) {
        if(strcmp(music_list[i].music_name, music_name) == 0) {
            *music_listindex = list_index, *music_id = i;
            break;
        }
    }
    if(list_index == 1) {
        free(parsed_name);
    }
}

bool maud_playlistmanager_playlistexists(maud_t* maud, const char* playlist_name) {
    return maud_playlist_exists(maud->playlist_manager.playlists,
        maud->playlist_manager.playlist_count, playlist_name);
}

bool maud_playlistmanager_createplaylist(maud_t* maud, const char* playlist_name) {
    bool success = maud_playlist_create(&maud->playlist_manager.playlists,
        &maud->playlist_manager.playlist_count, playlist_name);
    if(success) {
        size_t msg_len = 20 + strlen(playlist_name);
        char msg_buff[msg_len + 1];
        sprintf(msg_buff, "Playlist '%s' created.", playlist_name);
        msg_buff[msg_len] = '\0';
        maud_notification_push(&maud->notification, maud->font, 20,
            (SDL_Color){0x12, 0x12, 0x12, 0xff},
            msg_buff,
            (SDL_Color){0x00, 0xff, 0x00, 0xff},
            0.9,
            20,
            20,
            10
        );
        printf("Successfully created playlist: %s\n", playlist_name);
        maud_playlistmanager_write_data_tofile(maud);
    }
    return success;
}


void maud_playlistmanager_addmusicselection_toplaylist(maud_t* maud, const char* playlist_name) {
    for(size_t i=0;i<maud->playlist_manager.playlist_count;i++) {
        maud_playlist_t* playlist = &maud->playlist_manager.playlists[i];
        if(strcmp(playlist->name, playlist_name) != 0) {
            continue;
        }
        if(maud_queue_addmusicfrom_queue(&playlist->queue, &maud->selection_queue)) {
            maud_playlistmanager_write_data_tofile(maud);
        }
        break;
    }
}


void maud_playlistmanager_addmusic_toplaylist(maud_t* maud, const char* playlist_name,
    size_t music_listindex, size_t music_id) {
    for(size_t i=0;i<maud->playlist_manager.playlist_count;i++) {
        maud_playlist_t* playlist = &maud->playlist_manager.playlists[i];
        if(strcmp(playlist->name, playlist_name) != 0) {
            continue;
        }
        if(maud_queue_addmusic(&playlist->queue, 0, music_listindex, music_id)) {
            maud_playlistmanager_write_data_tofile(maud);
        }
    }
}

bool maud_playlistmanager_removeplaylist(maud_t* maud, const char* playlist_name) {
    bool success = maud_playlist_remove(&maud->playlist_manager.playlists,
        &maud->playlist_manager.playlist_count, playlist_name);
    if(success) {
        maud_playlistmanager_write_data_tofile(maud);
    }
    return success;
}

void maud_playlistmanager_destroyplaylist(maud_playlist_t* playlist) {
    maud_playlist_destroy(playlist);
}

void maud_playlistmanager_display(maud_t* maud) {
    maud_playlistmanager_buttonbar_display(maud);
    maud_playlistmanager_displayplaylists(maud);
    maud_playlistmanager_layoutdropdown_menu_display(maud);
    maud_playlistmanager_newplaylist_input_displayall(maud);
}

void maud_playlistmanager_displayplaylists(maud_t* maud) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    switch(playlist_manager->layout_type) {
        case PLAYLIST_GRIDVIEW: maud_playlistmanager_gridrenderer_displayplaylists(maud); break;
        case PLAYLIST_LISTVIEW: fprintf(stderr, "List view has not been implemented yet!\n"); break;
    }
    if(playlist_props->show_tooltip) {
        maud_tooltip_renderhover(maud, &playlist_props->tooltip);
    }
    playlist_props->show_tooltip = false;
}

void maud_playlistmanager_destroy(maud_t* maud) {
    maud_inputbox_destroy(&maud->playlist_manager.new_playlistinput.inputbox);
    maud_inputbox_destroy(&maud->playlist_manager.renameplaylist_input.inputbox);
    maud_playlists_destroy(&maud->playlist_manager.playlists,
        &maud->playlist_manager.playlist_count);
}