#ifndef _MAUD_MENUMANAGER_H
#define _MAUD_MENUMANAGER_H
#include "maud_player.h"
#include "maud_texturemanager.h"

enum menu_options {
    MAUD_DEFAULT_MENU,
    MAUD_SETTINGS_MENU
};

void maud_menumanager_setup_menu(maud_t* maud);
void maud_menumanager_createmenu_texture(maud_t* maud, int type, size_t amount);
void maud_menumanager_addmenu_texture(maud_t* maud, int type);
void maud_menumanager_menuplace_texture(maud_t* maud, int type, SDL_Texture* texture, SDL_Rect canvas);
void maud_menumanager_menuadd_canvas(maud_t* maud, SDL_Rect canvas);
void maud_menumanager_appendtext(maud_t* maud, text_info_t text);
void maud_menumanager_menu_freetext(maud_t* maud, int menu_option);
#endif