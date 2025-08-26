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

void maud_settingmenu(maud_t* maud);
void maud_settingmenu_initnavbar_tab(maud_t* maud);
void maud_settingmenu_createnavbar(maud_t* maud, SDL_Color color, SDL_Color customize_tabcolor,
    SDL_Color musiclibrary_tabcolor);
void maud_settingmenu_rendernavbar(maud_t* maud);
void maud_settingmenu_rendertab_underline(maud_t* maud, SDL_Color underline_color);
void maud_settingmenu_handlebackbutton(maud_t* maud);
void maud_settingmenu_handlecustomize_tab(maud_t* maud);
void maud_settingmenu_handlemusiclibrary_tab(maud_t* maud);
void maud_settingmenu_handleabout_tab(maud_t* maud);
void maud_settingmenu_rendercustomizetab_headertext(maud_t* maud,
    maud_customizetab_t* customize_tab);
void maud_settingmenu_rendercustomizetab_colorpicker(maud_t* maud,
    maud_customizetab_t* customize_tab);
void maud_settingmenu_rendermusiclibrary_headertext(maud_t* maud,
    maud_musiclibrarytab_t* musiclibrary_tab);
void maud_settingmenu_rendermusiclibrary_locationsection(maud_t* maud,
    maud_musiclibrarytab_t* musiclibrary_tab);
void maud_settingmenu_rendermusiclibrary_addlocationbtn(maud_t* maud,
    maud_musiclibrarytab_t* musiclibrary_tab);
void maud_settingmenu_rendermusiclibrary_locations(maud_t* maud,
    maud_musiclibrarytab_t* musiclibrary_tab);
void maud_settingmenu_displaycustomize_tab(maud_t* maud);
void maud_settingmenu_displaymusiclibrary_tab(maud_t* maud);
void maud_settingmenu_displayabout_tab(maud_t* maud);
void maud_settingmenu_displaycurrent_tab(maud_t* maud);
void maud_settingmenu_destroy(maud_t* maud);
#endif