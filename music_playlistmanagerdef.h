#ifndef _MUSIC_PLAYLISTMANAGERDEF_H
#define _MUSIC_PLAYLISTMANAGERDEF_H
#include "music_queuedef.h"
#include "music_playlists.h"
#include "music_tooltipsdef.h"
#include "music_textinfodef.h"
#include "music_inputboxesdef.h"
#include "music_scrollcontainerdef.h"

typedef struct newplaylist_button {
    text_info_t text;
    SDL_Rect icon, canvas;
    int content_spacing;
    SDL_Color canvas_color;
    bool clicked;
} newplaylistbtn_t;

typedef struct playlist_layoutswitch {
    SDL_Rect canvas;
    text_info_t view_text;
    SDL_Rect icon_canvas, dropdown_iconcanvas;
    int content_spacing;
    bool clicked;
} playlist_layoutswitch_t;

typedef struct button_bar {
    SDL_Rect canvas;
    newplaylistbtn_t new_playlistbtn;
    text_info_t playlistlayout_text;
    playlist_layoutswitch_t layout_switch;
} button_bar_t;

// A structure of the playlist manager
typedef struct mplayer_playlistmanager {
    mplayer_playlist_t* playlists; // An array of playlists with their songs queue
    int playlistmenu_collapsex,
        playlistmenu_collapsey;
    bool playlistmenu_collapse, playlistmenu_scrolled;

    button_bar_t button_bar;
    mplayer_tooltip_t playlist_tooltip;
    mplayer_inputbox_t playlist_inputbox;
    bool createplaylistbtn_clicked, playlistname_empty,
         playlistname_exists;
    mplayer_inputbox_t rename_inputbox;
    music_scrollcontainer_t playlist_itemcontainer,
        playlist_container;
    bool rename_clicked, rename_inputinited;
    SDL_Rect layoutbtn;
    int layout_type;
    size_t playlist_count;  // The number of playlists that were created / parsed
    size_t playlist_selectionindex;
    bool playlist_selected, playlist_inputboxinited;
} mplayer_playlistmanager_t;
#endif