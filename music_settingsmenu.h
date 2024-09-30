#ifndef _MUSIC_SETTINGSMENU_H
#define _MUSIC_SETTINGSMENU_H
#include "music_player.h"
#include "music_tooltips.h"
#include "music_checkboxes.h"
#include "music_textmanager.h"
#include "music_filemanager.h"
#include "music_menumanager.h"
#include "music_texturemanager.h"
#include "music_scrollcontainers.h"
#include "music_playerbutton_manager.h"

void mplayer_settingmenu(mplayer_t* mplayer);
void mplayer_settingmenu_render_settingbtns(mplayer_t* mplayer);
void mplayer_settingmenu_render_navigationbar(mplayer_t* mplayer, SDL_Rect* navbar_canvas, SDL_Color navbar_canvascolor);
void mplayer_settingmenu_render_musiclocations(mplayer_t* mplayer, SDL_Rect* previous_canvas, int disappear_y);
void mplayer_settingmenu_tryappend_scrollcontainer(mplayer_t* mplayer, mplayer_scrollcontainer_t container);
void mplayer_settingmenu_scrollcontainer_update(mplayer_t* mplayer);
#endif