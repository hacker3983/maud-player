#ifndef _MUSIC_COLORPICKER_DEF_H
#define _MUSIC_COLORPICKER_DEF_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "music_textinfodef.h"
#include "music_inputboxesdef.h"

enum color_slider_type {
    COLOR_SLIDER_R,
    COLOR_SLIDER_G,
    COLOR_SLIDER_B,
    COLOR_SLIDER_A
};

typedef struct color_tracknames_attribute {
    TTF_Font* track_font;
    const char* track_name;
    int track_namespacing;
    SDL_Color track_namecolor;
    int track_fontsize;
} color_tracknameattrib_t;

typedef struct color_slider {
    TTF_Font* track_font;
    char* track_name;
    int track_namespacing;
    text_info_t track_nameinfo;
    int track_segmentwidth;
    mplayer_inputbox_t inputbox;
    SDL_Rect track; // The static background for the slider
    SDL_Rect handle; // The handle that the user drags over the slider
    SDL_Color handle_color;
    SDL_Color handle_bordercolor;
    int handle_pos;
} color_slider_t;

typedef struct color_picker {
    color_slider_t sliders[4]; // 4 sliders for rgba respectively
    int max_trackname_width;
    SDL_Rect preview_canvas; // A canvas that display the current color
    mplayer_inputbox_t hex_inputbox;
    SDL_Color color; // The current color
} mplayer_colorpicker_t;
#endif