#include "maud_playlistmanager.h"

SDL_Texture* maud_playlistmanager_layout_geticon(maud_t* maud, int type) {
    SDL_Texture *layout_icon = NULL;
    switch(type) {
        case PLAYLIST_LISTVIEW: layout_icon = IMG_LoadTexture(maud->renderer, "images/list.png"); break;
        case PLAYLIST_GRIDVIEW: layout_icon = IMG_LoadTexture(maud->renderer, "images/grid.png"); break;
    }
    return layout_icon;
}


SDL_Texture* maud_playlistmanager_layout_getcurrenticon(maud_t* maud) {
    return maud_playlistmanager_layout_geticon(maud, maud->playlist_manager.layout_type);
}

char* maud_playlistmanager_layout_getname(int type) {
    static char layout_name[5] = {0};
    switch(type) {
        case PLAYLIST_LISTVIEW: strcpy(layout_name, "List"); break;
        case PLAYLIST_GRIDVIEW: strcpy(layout_name, "Grid"); break;
    }
    return layout_name;
}


char* maud_playlistmanager_layout_getcurrentname(maud_t* maud) {
    return maud_playlistmanager_layout_getname(maud->playlist_manager.layout_type);
}

char* maud_playlistmanager_layout_getname_view(int type) {
    static char layout_name[10]  = {0};
    sprintf(
        layout_name,
        "%s View",
        maud_playlistmanager_layout_getname(type)
    );
    return layout_name;
}

char* maud_playlistmanager_layout_getcurrentname_view(maud_t* maud) {
    return maud_playlistmanager_layout_getname_view(maud->playlist_manager.layout_type);
}