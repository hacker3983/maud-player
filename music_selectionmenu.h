#ifndef _MPLAYER_SELECTIONMENU_H
#define _MPLAYER_SELECTIONMENU_H
#include "music_player.h"
#include "music_checkboxes.h"
#include "music_textmanager.h"

void mplayer_selectionmenu_create(mplayer_t* mplayer);
void mplayer_selectionmenu_display_addbtn(mplayer_t* mplayer, SDL_Rect playbtn_background);
void mplayer_selectionmenu_display_playbtn(mplayer_t* mplayer, text_info_t songs_selectioninfo,
    SDL_Rect* playbtn_backgroundref);
void mplayer_selectionmenu_display_songselectioninfo(mplayer_t* mplayer, text_info_t* songs_selectioninfo);
void mplayer_selectionmenu_display_checkallbtn(mplayer_t* mplayer);
void mplayer_selectionmenu_handle_checkallbtn_toggleoption(mplayer_t* mplayer, text_info_t songs_selectioninfo);
bool mplayer_selectionmenu_togglesong_checkbox(mplayer_t* mplayer, music_t* music_list, size_t music_checkbox_index);
#endif