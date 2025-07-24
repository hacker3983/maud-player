#ifndef _MUSIC_COLORPICKER_H
#define _MUSIC_COLORPICKER_H
#include "music_player.h"

char* mplayer_colorpicker_getslidername(int slider_type);
void mplayer_colorpicker_setslider_trackprop(mplayer_colorpicker_t* color_picker, int slider_type,
    TTF_Font* track_font, const char* track_name, int track_fontsize, SDL_Color track_namecolor,
    int track_namespacing, int track_segmentwidth, int track_height);
void mplayer_colorpicker_setslider_handleprop(mplayer_colorpicker_t* color_picker, int slider_type,
    int handle_width, int handle_height, SDL_Color handle_color, SDL_Color handle_bordercolor);
void mplayer_colorpicker_setslider_handlepos(mplayer_colorpicker_t* color_picker, int slider_type,
    int handle_pos);
void mplayer_colorpicker_setslider_trackposition(mplayer_colorpicker_t* color_picker, int slider_type, int track_x, int track_y);
void mplayer_colorpicker_setsliders_trackposition(mplayer_colorpicker_t* color_picker, int track_x, int track_y,
    int track_spacing);
void mplayer_colorpicker_setsliders_props(mplayer_colorpicker_t* color_picker,
    color_tracknameattrib_t track_nameattribs[4], int track_segmentwidth,
    int track_height, int handle_width, int handle_height,
    SDL_Color handle_color, SDL_Color handle_bordercolor);
void mplayer_colorpicker_setpreview_position(mplayer_colorpicker_t* color_picker, int x, int y);
void mplayer_colorpicker_setpreview_props(mplayer_colorpicker_t* color_picker, TTF_Font* font,
    int font_size, int width, int height);
void mplayer_colorpicker_setcolor(mplayer_colorpicker_t* color_picker, int r, int g, int b, int a);
void mplayer_colorpicker_setcolorfromhandle(mplayer_colorpicker_t* color_picker);
void mplayer_colorpicker_handleslider_inputbox_event(mplayer_t* mplayer,
    mplayer_colorpicker_t* color_picker, int slider_type);
bool mplayer_colorpicker_handlehex_inputpaste(mplayer_colorpicker_t* color_picker);
void mplayer_colorpicker_handlehex_inputbox_event(mplayer_t* mplayer,
    mplayer_colorpicker_t* color_picker);
void mplayer_colorpicker_handleinputbox_events(mplayer_t* mplayer,
    mplayer_colorpicker_t* color_picker);
bool mplayer_colorpicker_inputboxclicked(mplayer_t* mplayer, mplayer_colorpicker_t* color_picker);
SDL_Color mplayer_colorpicker_getslidersegment_color(int slider_type, int color_value);
void mplayer_colorpicker_renderslider(mplayer_t* mplayer, mplayer_colorpicker_t* color_picker, int slider_type);
void mplayer_colorpicker_rendersliders(mplayer_t* mplayer, mplayer_colorpicker_t* color_picker);
void mplayer_colorpicker_display(mplayer_t* mplayer, mplayer_colorpicker_t* color_picker);
void mplayer_colorpicker_destroy(mplayer_colorpicker_t* mplayer);
#endif