#ifndef _MAUD_PLAYLISTMANAGERDEF_H
#define _MAUD_PLAYLISTMANAGERDEF_H
#include "maud_queuedef.h"
#include "maud_playlists.h"
#include "maud_tooltipsdef.h"
#include "maud_textinfodef.h"
#include "maud_inputboxesdef.h"

enum playlist_layoutmode {
    PLAYLIST_LISTVIEW,
    PLAYLIST_GRIDVIEW,
    PLAYLIST_LAYOUTCOUNT
};

typedef struct maud_newplaylist_button {
    SDL_Rect canvas;
    SDL_Color color;
    SDL_Rect icon_canvas;
    text_info_t text;
    int content_spacing;
    bool clicked;
} maud_newplaylistbtn_t;

typedef struct maud_playlistlayout_button {
    SDL_Rect canvas;
    SDL_Color color;
    SDL_Rect icon_canvas;
    text_info_t text;
} maud_playlistlayoutbtn_t;

typedef struct maud_playlistlayout_switchdropdown {
    SDL_Rect canvas;
    SDL_Color color;
    int horizontal_spacing;
    maud_playlistlayoutbtn_t layout_btns[PLAYLIST_LAYOUTCOUNT];
} maud_playlistlayout_switchdropdown_t;

typedef struct maud_playlistlayout_switch {
    SDL_Rect canvas;
    text_info_t view_text;
    SDL_Rect icon_canvas, dropdown_iconcanvas;
    maud_playlistlayout_switchdropdown_t dropdown_menu;
    int content_spacing;
    bool clicked;
} maud_playlistlayout_switch_t;

typedef struct maud_buttonbar {
    SDL_Rect canvas;
    maud_newplaylistbtn_t new_playlistbtn;
    text_info_t playlistlayout_text;
    maud_playlistlayout_switch_t layout_switch;
    bool init;
} maud_buttonbar_t;

typedef struct maud_createplaylist_btn {
    SDL_Rect canvas;
    SDL_Color color;
    text_info_t text;
    bool clicked;
} maud_createplaylistbtn_t;

typedef struct maud_newplaylist_input {
    SDL_Rect canvas;
    SDL_Color color;
    maud_inputbox_t inputbox;
    bool playlistname_exists, playlistname_empty;

    maud_createplaylistbtn_t create_playlistbtn;
    bool init;
} maud_newplaylist_input_t;

typedef struct maud_rename_input {
    maud_inputbox_t inputbox;
} maud_renameplaylist_input_t;

typedef struct maud_playlistprops {
    int start_x, start_y,
        scroll_y;
    SDL_Rect scroll_area;
    size_t selection_index;
    bool selected;
    size_t *start_renderpos,
            prev_renderpos,
            next_renderpos,
            end_renderpos;
    maud_tooltip_t tooltip;
    bool init, show_tooltip;
} maud_playlistprops_t;

typedef struct maud_playlistmanager {
    maud_buttonbar_t button_bar;
    maud_playlist_t* playlists;
    size_t playlist_renderpos, playlist_count;
    maud_playlistprops_t playlist_props;
    maud_newplaylist_input_t new_playlistinput;
    maud_renameplaylist_input_t renameplaylist_input;
    int layout_type;
} maud_playlistmanager_t;
#endif