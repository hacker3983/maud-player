#ifndef _MUSIC_SETTINGSMENU_H
#define _MUSIC_SETTINGSMENU_H
#include "music_player.h"

void mplayer_settingmenu(mplayer_t* mplayer);
void mplayer_settingmenu_render_settingbtns(mplayer_t* mplayer);
void mplayer_settingmenu_render_musiclocations(mplayer_t* mplayer, SDL_Rect* previous_canvas, int disappear_y);
void mplayer_settingmenu_tryappend_scrollcontainer(mplayer_t* mplayer, mplayer_scrollcontainer_t container);
void mplayer_settingmenu_scrollcontainer_update(mplayer_t* mplayer);
#endif