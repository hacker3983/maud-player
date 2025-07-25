#ifndef _MUSIC_SETTINGSMENU_H
#define _MUSIC_SETTINGSMENU_H
#include "maud_player.h"
#include "maud_tooltips.h"
#include "maud_checkboxes.h"
#include "maud_textmanager.h"
#include "maud_filemanager.h"
#include "maud_menumanager.h"
#include "maud_texturemanager.h"
#include "maud_scrollcontainer.h"
#include "maud_playerbutton_manager.h"

typedef struct music_settingmenu_navbar {
    // Information related to the actual navigation bar
    SDL_Rect canvas;
    SDL_Color color;

    // Information related to the back button
    SDL_Rect backbtn_canvas;
    maud_tooltip_t backbtn_tooltip;
    text_info_t backbtn_textinfo;
} maud_settingmenu_navbar_t;

typedef struct music_settingmenu_librarycategory {
    text_info_t text_info;

    // Information related to the add folder button
    SDL_Rect music_addfolderbtn_canvas;
    text_info_t addfolderbtn_textinfo;
    maud_tooltip_t addfolderbtn_tooltip;
} maud_settingmenu_librarycategory_t;

typedef struct music_settingmenu_personalizationcategory {
    text_info_t text_info;
} maud_settingmenu_personalizationcategory_t;

typedef struct music_settingmenu_aboutcategory {
    text_info_t text_info;
} maud_settingmenu_aboutcategory_t;

void maud_settingmenu(maud_t* maud);
void maud_settingmenu_additem(maud_t* maud, SDL_Rect canvas);
void maud_settingmenu_handleupdate(maud_t* maud);
void maud_settingmenu_create_navigationbar(maud_t* maud, maud_settingmenu_navbar_t* navbar_ref);
void maud_settingmenu_render_navigationbar(maud_t* maud, maud_settingmenu_navbar_t* navbar_ref);
bool maud_settingmenu_handle_backbtn(maud_t* maud, maud_settingmenu_navbar_t* navbar_ref);
void maud_settingmenu_render_musiclibrary(maud_t* maud, maud_settingmenu_navbar_t navbar,
    maud_settingmenu_librarycategory_t* location_category);
void maud_settingmenu_render_musiclocations(maud_t* maud, SDL_Rect previous_canvas);
void maud_settingmenu_render_personalization(maud_t* maud, SDL_Rect previous_canvas,
    maud_settingmenu_personalizationcategory_t* personalization_category);
void maud_settingmenu_render_about(maud_t* maud,
    maud_settingmenu_personalizationcategory_t personalization_category);
void maud_settingmenu_scrollcontainer_tryappend_scrollitem(maud_t* maud, SDL_Rect item);
void maud_settingmenu_scrollcontainer_update(maud_t* maud);
#endif