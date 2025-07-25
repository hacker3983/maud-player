#ifndef _MUSIC_PLAYERBUTTON_MANAGER_H
#define _MUSIC_PLAYERBUTTON_MANAGER_H
#include "maud_player.h"

bool maud_buttonmanager_ibutton_hover(maud_t* maud, ibtn_t button);
bool maud_buttonmanager_tbutton_hover(maud_t* maud, tbtn_t button);
bool maud_buttonmanager_ibuttons_hover(maud_t* maud, ibtn_t* buttons, int* btn_id, size_t button_count);
bool maud_buttonmanager_tbuttons_hover(maud_t* maud, tbtn_t* buttons, int* btn_id, size_t button_count);
#endif