#ifndef _MUSIC_TOOLTIPSDEF_H
#define _MUSIC_TOOLTIPSDEF_H
#include "music_textinfodef.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct maud_tooltip {
    char* text;                 // The text that will be displayed in the tooltip
    int x, y;                   // The horizontal (x) and vertical (y) position of the tooltip when the element is hovered
                                // by the user (Note the x and y positions will be always relative to the position of
                                // the element when hovered by the user)
    int margin_x, margin_y;     // margin_x represents the amount we should center the text within the tooltip canvas
                                // horizontally while margin_y represents the amount we should center the text within
                                // the tooltip canvas vertically
    SDL_Rect element_canvas;    // The area of the button or icon that the tooltip will be displayed for
    int wrap_length;            // The wrap length determines how much text should be separated by on new line
    int wrap_spacing;           // The distance between text whenever they are wrapped on new lines
    SDL_Color background_color; // background color for the tooltip
    SDL_Color text_color;       // text color for the tooltip
    float delay_secs;           // A time in seconds that it will take to start displaying the tooltip on screen 
    float duration_secs;        // A duration or how long the tooltip should be on screen for (If a time of zero is
                                // specified then the tooltip will display for ever)
    int font_size;              // The font size of the text that should be displayed within the tooltip
    TTF_Font* font;             // The font that should be used to render text within the tooltip
    int w, h;                   // the width and the height of the tooltip returned by maud_tooltip_getsize()
    text_info_t* contents;      // The content list is returned whenever maud_tooltip_render() is called
    size_t content_count;       // The content count
} maud_tooltip_t;
#endif