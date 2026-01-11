#ifndef _MAUD_SELECTIONMENUDEF_H
#define _MAUD_SELECTIONMENUDEF_H
#include <SDL2/SDL.h>
#include "maud_textinfodef.h"
#include "maud_checkboxdef.h"

typedef struct maud_selectionmenu_selectallbtn {
    SDL_Rect canvas;
    maud_checkbox_t checkbox;
    text_info_t selection_info;
    bool clicked, toggled;
} maud_selectionmenu_selectallbtn_t;

typedef struct maud_selectionmenu_button {
    SDL_Rect canvas;
    SDL_Color color;
    SDL_Rect icon_canvas;
    const char* img_path;
    SDL_Texture* img_texture;
    text_info_t text;
    bool clicked;
} maud_selectionmenubtn_t;

typedef struct maud_selectionmenu {
    SDL_Rect canvas;
    SDL_Color color;
    maud_selectionmenu_selectallbtn_t select_allbtn;
    maud_selectionmenubtn_t playbtn, playnextbtn,
        addtobtn, removebtn, moveup_btn, movedown_btn;
    bool init;
} maud_selectionmenu_t;
#endif