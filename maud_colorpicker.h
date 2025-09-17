#ifndef _MAUD_COLORPICKER_H
#define _MAUD_COLORPICKER_H
#include "maud_player.h"

char* maud_colorpicker_getslidername(int slider_type);
void maud_colorpicker_setslider_trackprop(maud_colorpicker_t* color_picker, int slider_type);
void maud_colorpicker_setslider_handleprop(maud_colorpicker_t* color_picker, int slider_type);
void maud_colorpicker_setsliders_trackprop(maud_colorpicker_t* color_picker);
void maud_colorpicker_setpreview_props(maud_colorpicker_t* color_picker);
void maud_colorpicker_setpreview_position(maud_colorpicker_t* color_picker);
void maud_colorpicker_setslider_trackposition(maud_colorpicker_t* color_picker, int slider_type,
    int track_x, int track_y);
void maud_colorpicker_setsliders_trackposition(maud_colorpicker_t* color_picker);
void maud_colorpicker_setslider_props(maud_colorpicker_t* color_picker, color_sliderprops_t* props);
void maud_colorpicker_setprops(maud_colorpicker_t* color_picker, color_pickerprops_t* props);
void maud_colorpicker_getsize(maud_colorpicker_t* color_picker);
void maud_colorpicker_setposition(maud_colorpicker_t* color_picker, int x, int y);
void maud_colorpicker_setrgba_values(maud_colorpicker_t* color_picker);
void maud_colorpicker_sethex_values(maud_colorpicker_t* color_picker);
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