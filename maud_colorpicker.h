#ifndef _MAUD_COLORPICKER_H
#define _MAUD_COLORPICKER_H
#include "maud_player.h"

char* maud_colorpicker_getslidername(int slider_type);
void maud_colorpicker_setslider_trackprop(maud_colorpicker_t* color_picker, int slider_type,
    TTF_Font* track_font, const char* track_name, int track_fontsize, SDL_Color track_namecolor,
    int track_namespacing, int track_segmentwidth, int track_height);
void maud_colorpicker_setslider_handleprop(maud_colorpicker_t* color_picker, int slider_type,
    int handle_width, int handle_height, SDL_Color handle_color, SDL_Color handle_bordercolor);
void maud_colorpicker_setslider_handlepos(maud_colorpicker_t* color_picker, int slider_type,
    int handle_pos);
void maud_colorpicker_setslider_trackposition(maud_colorpicker_t* color_picker, int slider_type, int track_x, int track_y);
void maud_colorpicker_setsliders_trackposition(maud_colorpicker_t* color_picker, int track_x, int track_y,
    int track_spacing);
void maud_colorpicker_setsliders_props(maud_colorpicker_t* color_picker,
    color_tracknameattrib_t track_nameattribs[4], int track_segmentwidth,
    int track_height, int handle_width, int handle_height,
    SDL_Color handle_color, SDL_Color handle_bordercolor);
void maud_colorpicker_setpreview_position(maud_colorpicker_t* color_picker, int x, int y);
void maud_colorpicker_setpreview_props(maud_colorpicker_t* color_picker, TTF_Font* font,
    int font_size, int width, int height);
void maud_colorpicker_setcolor(maud_colorpicker_t* color_picker, int r, int g, int b, int a);
void maud_colorpicker_setcolorfromhandle(maud_colorpicker_t* color_picker);
void maud_colorpicker_handleslider_inputbox_event(maud_t* mplayer,
    maud_colorpicker_t* color_picker, int slider_type);
bool maud_colorpicker_handlehex_inputpaste(maud_colorpicker_t* color_picker);
void maud_colorpicker_handlehex_inputbox_event(maud_t* mplayer,
    maud_colorpicker_t* color_picker);
void maud_colorpicker_handleinputbox_events(maud_t* mplayer,
    maud_colorpicker_t* color_picker);
bool maud_colorpicker_inputboxclicked(maud_t* mplayer, maud_colorpicker_t* color_picker);
SDL_Color maud_colorpicker_getslidersegment_color(int slider_type, int color_value);
void maud_colorpicker_renderslider(maud_t* mplayer, maud_colorpicker_t* color_picker, int slider_type);
void maud_colorpicker_rendersliders(maud_t* mplayer, maud_colorpicker_t* color_picker);
void maud_colorpicker_display(maud_t* mplayer, maud_colorpicker_t* color_picker);
void maud_colorpicker_destroy(maud_colorpicker_t* mplayer);
#endif