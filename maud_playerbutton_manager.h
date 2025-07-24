#ifndef _MUSIC_PLAYERBUTTON_MANAGER_H
#define _MUSIC_PLAYERBUTTON_MANAGER_H
#include "music_player.h"

bool mplayer_buttonmanager_ibutton_hover(mplayer_t* mplayer, ibtn_t button);
bool mplayer_buttonmanager_tbutton_hover(mplayer_t* mplayer, tbtn_t button);
bool mplayer_buttonmanager_ibuttons_hover(mplayer_t* mplayer, ibtn_t* buttons, int* btn_id, size_t button_count);
bool mplayer_buttonmanager_tbuttons_hover(mplayer_t* mplayer, tbtn_t* buttons, int* btn_id, size_t button_count);
#endif