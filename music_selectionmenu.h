#ifndef _MPLAYER_SELECTIONMENU_H
#define _MPLAYER_SELECTIONMENU_H
#include "music_player.h"
#include "music_checkboxes.h"
#include "music_inputboxes.h"
#include "music_textmanager.h"
#include "music_queue.h"
#include "music_playlistmanager.h"

enum addtoplaylist_modalcanvases {
    ADDTOPLAYLIST_MODALCANVAS,
    ADDTOPLAYLIST_MODALCANVAS_CREATEBTN,
    ADDTOPLAYLIST_MODALCANVAS_CANCELBTN
};

void mplayer_selectionmenu_create(mplayer_t* mplayer);
void mplayer_selectionmenu_clearmusic_selection(mplayer_t* mplayer);
void mplayer_selectionmenu_display_addbtn(mplayer_t* mplayer, SDL_Rect playnext_btnbackground);
void mplayer_selectionmenu_display_addtoplaylist_modal(mplayer_t* mplayer);
void mplayer_selectionmenu_handle_addtoplaylist_modalevents(mplayer_t* mplayer);
void mplayer_selectionmenu_handle_addtobtn(mplayer_t* mplayer);
void mplayer_selectionmenu_display_playbtn(mplayer_t* mplayer, text_info_t songs_selectioninfo,
    SDL_Rect* playbtn_backgroundref);
void mplayer_selectionmenu_handle_playbtn(mplayer_t* mplayer, SDL_Rect playbtn_background);
void mplayer_selectionmenu_display_playnextbtn(mplayer_t* mplayer, SDL_Rect playbtn_background,
    SDL_Rect* playnext_backgroundref);
void mplayer_selectionmenu_handle_playnextbtn(mplayer_t* mplayer, SDL_Rect playnext_btncanvas);
void mplayer_selectionmenu_display_songselectioninfo(mplayer_t* mplayer, text_info_t* songs_selectioninfo);
void mplayer_selectionmenu_display_checkallbtn(mplayer_t* mplayer);
void mplayer_selectionmenu_handle_checkallbtn_toggleoption(mplayer_t* mplayer, text_info_t songs_selectioninfo);
bool mplayer_selectionmenu_toggleitem_checkboxmusic_queue(mplayer_t* mplayer, music_queue_t* queue,
    size_t item_index);
bool mplayer_selectionmenu_togglesong_checkbox(mplayer_t* mplayer, music_t* music_list, size_t music_checkbox_index);

bool mplayer_selectionmenu_togglecheckbox(mplayer_t* mplayer, music_queue_t* queue, size_t queue_itemindex,
    music_t* music_list, size_t music_checkbox_index);
#endif