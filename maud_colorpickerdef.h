#ifndef _MAUD_COLORPICKER_DEF_H
#define _MAUD_COLORPICKER_DEF_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "maud_textinfodef.h"
#include "maud_inputboxesdef.h"

enum color_slider_type {
    COLOR_SLIDER_R,
    COLOR_SLIDER_G,
    COLOR_SLIDER_B,
    COLOR_SLIDER_A
};

typedef struct color_slider {
    TTF_Font* track_font;
    char* track_name;
    text_info_t track_nameinfo;
    maud_inputbox_t inputbox;
    SDL_Rect track; // The static background for the slider
    SDL_Rect handle; // The handle that the user drags over the slider
    SDL_Color handle_color;
    SDL_Color handle_bordercolor;
    int handle_pos;
} color_slider_t;

typedef struct color_slider_props {
    TTF_Font* track_font;
    int track_fontsize;
    char* track_names[4];

    SDL_Color track_namecolor;
    int track_segmentwidth,
        track_namespacing, track_height,
        track_verticalspacing,
        max_trackname_width;

    SDL_Color handle_color;
    SDL_Color handle_bordercolor;
    int handle_width, handle_height,
        handle_pos;
    int inputbox_width, inputbox_height;
} color_sliderprops_t;

typedef struct color_picker_props {
    TTF_Font* preview_font;
    int preview_fontsize, preview_width,
        preview_height, preview_spacing;
    color_sliderprops_t slider_props;
} color_pickerprops_t;

typedef struct color_picker {
    SDL_Rect canvas;
    color_slider_t sliders[4]; // 4 sliders for rgba respectively
    color_pickerprops_t props;
    SDL_Rect preview_canvas; // A canvas that displays the current color
    maud_inputbox_t hex_inputbox;
    SDL_Color color; // The current color
} maud_colorpicker_t;
#endif