#ifndef _MUSIC_MENUMANAGER_H
#define _MUSIC_MENUMANAGER_H
#include "music_player.h"
#include "music_texturemanager.h"

enum menu_options {
    MPLAYER_DEFAULT_MENU,
    MPLAYER_SETTINGS_MENU
};

void mplayer_menumanager_setup_menu(mplayer_t* mplayer);
void mplayer_menumanager_createmenu_texture(mplayer_t* mplayer, int type, size_t amount);
void mplayer_menumanager_addmenu_texture(mplayer_t* mplayer, int type);
void mplayer_menumanager_menuplace_texture(mplayer_t* mplayer, int type, SDL_Texture* texture, SDL_Rect canvas);
void mplayer_menumanager_menuadd_canvas(mplayer_t* mplayer, SDL_Rect canvas);
void mplayer_menumanager_appendtext(mplayer_t* mplayer, text_info_t text);
void mplayer_menumanager_menu_freetext(mplayer_t* mplayer, int menu_option);
#endif