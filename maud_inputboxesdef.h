#ifndef _MUSIC_INPUTBOXESDEF_H
#define _MUSIC_INPUTBOXESDEF_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum input_selectiontype {
    INPUT_SELECTIONLEFT,
    INPUT_SELECTIONRIGHT
};

typedef struct input_character {
    char* utf8_char;
    size_t utf8_charlen;
    SDL_Rect canvas;
} inputchar_t;

typedef struct input_data {
    inputchar_t* characters;
    char* data;
    int selection_direction;
    size_t selection_start, selection_end, selection_count;
    size_t cursor_pos, character_count, size;
} inputdata_t;

typedef struct inputbox {
    TTF_Font* font;
    int font_size;
    size_t render_pos;

    SDL_Rect canvas;
    SDL_Color canvas_color;

    inputdata_t input;
    SDL_Color text_color;

    char* placeholder;
    SDL_Rect placeholder_canvas;
    SDL_Color placeholder_color;
    bool placeholder_show;

    SDL_Rect cursor_canvas;
    SDL_Color cursor_color;
    int cursor_width, cursor_height;
    uint64_t cursor_timeout;
    double blink_timeoutsecs;
    bool show_cursor, cursor_blink,
         cursor_blinking;

    int mouse_startx, mouse_endx;
    bool mouse_drag, start_xinited, end_xinited;
    bool entered, fill;
    bool hover, clicked;
} maud_inputbox_t;
#endif