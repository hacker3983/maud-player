#ifndef _MUSIC_SETTINGSMENU_H
#define _MUSIC_SETTINGSMENU_H
#include "music_player.h"
#include "music_tooltips.h"
#include "music_checkboxes.h"
#include "music_textmanager.h"
#include "music_filemanager.h"
#include "music_menumanager.h"
#include "music_texturemanager.h"
#include "music_scrollcontainer.h"
#include "music_playerbutton_manager.h"

typedef struct music_settingmenu_navbar {
    // Information related to the actual navigation bar
    SDL_Rect canvas;
    SDL_Color color;

    // Information related to the back button
    SDL_Rect backbtn_canvas;
    mplayer_tooltip_t backbtn_tooltip;
    text_info_t backbtn_textinfo;
} mplayer_settingmenu_navbar_t;

typedef struct music_settingmenu_librarycategory {
    text_info_t text_info;

    // Information related to the add folder button
    SDL_Rect music_addfolderbtn_canvas;
    text_info_t addfolderbtn_textinfo;
    mplayer_tooltip_t addfolderbtn_tooltip;
} mplayer_settingmenu_librarycategory_t;

typedef struct music_settingmenu_personalizationcategory {
    text_info_t text_info;
} mplayer_settingmenu_personalizationcategory_t;

typedef struct music_settingmenu_aboutcategory {
    text_info_t text_info;
} mplayer_settingmenu_aboutcategory_t;

void mplayer_settingmenu(mplayer_t* mplayer);
void mplayer_settingmenu_additem(mplayer_t* mplayer, SDL_Rect canvas);
void mplayer_settingmenu_handleupdate(mplayer_t* mplayer);
void mplayer_settingmenu_create_navigationbar(mplayer_t* mplayer, mplayer_settingmenu_navbar_t* navbar_ref);
void mplayer_settingmenu_render_navigationbar(mplayer_t* mplayer, mplayer_settingmenu_navbar_t* navbar_ref);
bool mplayer_settingmenu_handle_backbtn(mplayer_t* mplayer, mplayer_settingmenu_navbar_t* navbar_ref);
void mplayer_settingmenu_render_musiclibrary(mplayer_t* mplayer, mplayer_settingmenu_navbar_t navbar,
    mplayer_settingmenu_librarycategory_t* location_category);
void mplayer_settingmenu_render_musiclocations(mplayer_t* mplayer, SDL_Rect previous_canvas);
void mplayer_settingmenu_render_personalization(mplayer_t* mplayer, SDL_Rect previous_canvas,
    mplayer_settingmenu_personalizationcategory_t* personalization_category);
void mplayer_settingmenu_render_about(mplayer_t* mplayer,
    mplayer_settingmenu_personalizationcategory_t personalization_category);
void mplayer_settingmenu_scrollcontainer_tryappend_scrollitem(mplayer_t* mplayer, SDL_Rect item);
void mplayer_settingmenu_scrollcontainer_update(mplayer_t* mplayer);
#endif