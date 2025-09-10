#ifndef _MAUD_SETTINGSMENUDEF_H
#define _MAUD_SETTINGSMENUDEF_H
#include "maud_textinfodef.h"
#include "maud_tooltipsdef.h"
#include "maud_colorpickerdef.h"

typedef struct maud_customizetab_button {
    SDL_Rect canvas;
    SDL_Color color;
    text_info_t text;
} maud_customizetab_btn_t;

typedef struct maud_musictile_modifier {
    text_info_t backgroundcolor_content;
    maud_customizetab_btn_t bgcolor_button;
    
    text_info_t textcolor_content;
    maud_customizetab_btn_t textcolor_button;
} maud_musictile_modifier_t;

typedef struct maud_customizetab {
    SDL_Rect canvas;
    SDL_Color canvas_color;
    SDL_Rect icon_canvas;
    text_info_t header_text;
    maud_musictile_modifier_t musictile_modifier;
    maud_colorpicker_t color_picker;
} maud_customizetab_t;

typedef struct maud_addlocationbtn {
    SDL_Color color;
    SDL_Rect canvas;
    SDL_Rect icon_canvas;
    text_info_t text;
} maud_addlocationbtn_t;

typedef struct maud_musiclibrary_tab {
    SDL_Rect canvas;
    SDL_Color canvas_color;
    SDL_Rect icon_canvas;
    text_info_t header_text;
    text_info_t location_text;
    maud_addlocationbtn_t addlocation_btn;
} maud_musiclibrarytab_t;

enum maud_settingmenu_navtabs {
    MAUD_ABOUT_TAB,
    MAUD_CUSTOMIZE_TAB,
    MAUD_MUSICLIBRARY_TAB
};

typedef struct maud_settingmenu_navbar {
    // Information related to the actual navigation bar
    SDL_Rect canvas;
    SDL_Color color;

    // Information related to the back button
    SDL_Rect backbtn_canvas;
    maud_tooltip_t backbtn_tooltip;
    text_info_t backbtn_textinfo;

    // Customize tab
    maud_customizetab_t customize_tab;

    // Music Library tab
    maud_musiclibrarytab_t musiclibrary_tab;

    // About tab
    SDL_Rect information_canvas;

    int current_tab;
    bool tab_init;
} maud_settingmenu_navbar_t;
#endif