#ifndef _MUSIC_PLAYERBUTTON_TYPES_H
#define _MUSIC_PLAYERBUTTON_TYPES_H
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
    MUSIC_PLAYBTN,
    MUSIC_SHUFFLEBTN,
    MUSIC_SKIPBTN,
    MUSIC_PREVBTN,
    MUSIC_PAUSEBTN,
    MUSIC_REPEATALLBTN,
    MUSIC_REPEATONEBTN,
    MUSIC_REPEATOFFBTN,
    MUSIC_LISTPLAYBTN,
    MUSIC_ADDFOLDERBTN,
    MUSIC_ADDPLAYLISTBTN,
    MUSIC_ADDTOBTN,
    MUSIC_REMOVEBTN
};

enum setting_buttons {
    SETTING_BUTTON,
    BACK_BUTTON
};
#endif