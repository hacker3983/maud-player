#ifndef _MAUD_PLAYERBUTTON_TYPES_H
#define _MAUD_PLAYERBUTTON_TYPES_H
#include <SDL2/SDL.h>
typedef struct image_buttons {
    char* imgbtn_path;
    char* tooltip_text;
    int id;
    SDL_Rect btn_canvas;
    bool hover, clicked;
    int texture_idx;
} ibtn_t;

typedef struct text_buttons {
    int font_size;
    char* text;
    SDL_Color text_color;
    SDL_Rect btn_canvas;
    int id, hover;
} tbtn_t;

enum musical_buttons {
    MAUD_PLAYBTN,
    MAUD_SHUFFLEBTN,
    MAUD_SKIPBTN,
    MAUD_PREVBTN,
    MAUD_PAUSEBTN,
    MAUD_REPEATALLBTN,
    MAUD_REPEATONEBTN,
    MAUD_REPEATOFFBTN,
    MAUD_LISTPLAYBTN,
    MAUD_ADDFOLDERBTN,
    MAUD_ADDPLAYLISTBTN,
    MAUD_ADDTOBTN,
    MAUD_ADDTOCIRCLEBTN,
    MAUD_PLAYQUEUEBTN,
    MAUD_PLAYLISTBTN,
    MAUD_REMOVEBTN
};

enum setting_buttons {
    SETTING_BUTTON,
    BACK_BUTTON
};
#endif