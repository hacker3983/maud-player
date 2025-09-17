#include "maud_colorpicker.h"
#include "maud_string.h"
#include "maud_textmanager.h"
#include "maud_inputboxes.h"

char* maud_colorpicker_getslidername(int slider_type) {
    if(slider_type < 0 || slider_type > 3) {
        return NULL;
    }
    char *slider_name = calloc(2, sizeof(char));
    slider_name[0] = "RGBA"[slider_type];
    return slider_name;
}

void maud_colorpicker_setslider_trackprop(maud_colorpicker_t* color_picker, int slider_type) {
    color_slider_t* slider = &color_picker->sliders[slider_type];
    color_sliderprops_t* slider_props = &color_picker->props.slider_props;
    text_info_t* track_nameinfo = &slider->track_nameinfo;
    int* max_trackname_width = &slider_props->max_trackname_width;
    slider->track_font = slider_props->track_font;
    slider->track_name = slider_props->track_names[slider_type];
    track_nameinfo->font_size = slider_props->track_fontsize;
    track_nameinfo->text = slider->track_name;
    track_nameinfo->text_color = slider_props->track_namecolor;
    maud_textmanager_sizetext(slider_props->track_font, track_nameinfo);
    if(track_nameinfo->text_canvas.w > (*max_trackname_width)) {
        *max_trackname_width = track_nameinfo->text_canvas.w;
    }
    slider->track.w = 256 * slider_props->track_segmentwidth;
    slider->track.h = slider_props->track_height;
    slider->inputbox = maud_inputbox_create(
        slider_props->track_font,
        slider_props->track_fontsize,
        (SDL_Color){0x12, 0x12, 0x12, 0xff},
        NULL,
        (SDL_Color){0},
        (SDL_Color){0xff, 0xff, 0xff, 0xff},
        (SDL_Color){0x00, 0xff, 0x00, 0xff},
        0,
        0,
        150,
        50,
        2,
        25
    );
    slider_props->inputbox_width = slider->inputbox.canvas.w,
    slider_props->inputbox_height = slider->inputbox.canvas.h;
    slider->inputbox.show_cursor = false;
}

void maud_colorpicker_setslider_handleprop(maud_colorpicker_t* color_picker, int slider_type) {
    color_slider_t* slider = &color_picker->sliders[slider_type];
    color_sliderprops_t* slider_props = &color_picker->props.slider_props;
    slider->handle.w = slider_props->handle_width;
    slider->handle.h = slider_props->handle_height;
    slider->handle_pos = slider_props->handle_pos;
    slider->handle_color = slider_props->handle_color;
    slider->handle_bordercolor = slider_props->handle_bordercolor;
}

void maud_colorpicker_setsliders_trackprop(maud_colorpicker_t* color_picker) {
    for(size_t i=0;i<4;i++) {
        maud_colorpicker_setslider_trackprop(color_picker, i);
        maud_colorpicker_setslider_handleprop(color_picker, i);
    }
    maud_colorpicker_setcolorfromhandle(color_picker);
}

void maud_colorpicker_setpreview_props(maud_colorpicker_t* color_picker) {
    color_pickerprops_t* picker_props = &color_picker->props;
    color_picker->preview_canvas.w = picker_props->preview_width,
    color_picker->preview_canvas.h = picker_props->preview_height;

    color_picker->hex_inputbox = maud_inputbox_create(
        picker_props->preview_font,
        picker_props->preview_fontsize,
        (SDL_Color){0x12, 0x12, 0x12, 0xff},
        NULL,
        (SDL_Color){0},
        white,
        (SDL_Color){0x00, 0xff, 0x00, 0xff},
        0, 0,
        200,
        50,
        2,
        25
    );
}

void maud_colorpicker_setpreview_position(maud_colorpicker_t* color_picker) {
    color_pickerprops_t* picker_props = &color_picker->props;
    SDL_Rect* canvas = &color_picker->canvas, *preview_canvas = &color_picker->preview_canvas;
    preview_canvas->x = canvas->x,
    preview_canvas->y = canvas->y;

    maud_inputbox_t* hex_inputbox = &color_picker->hex_inputbox;
    hex_inputbox->canvas.x = preview_canvas->x + preview_canvas->w + picker_props->preview_spacing;
    hex_inputbox->canvas.y = preview_canvas->y + (preview_canvas->h - hex_inputbox->canvas.h) / 2;
}

void maud_colorpicker_setslider_trackposition(maud_colorpicker_t* color_picker, int slider_type,
    int track_x, int track_y) {
    color_slider_t* slider = &color_picker->sliders[slider_type];
    color_sliderprops_t* slider_props = &color_picker->props.slider_props;
    int track_height = slider->track.h,
        track_namespacing = slider_props->track_namespacing;
    text_info_t* track_nameinfo = &slider->track_nameinfo;
    track_nameinfo->text_canvas.x = track_x;
    track_nameinfo->text_canvas.y = track_y + (track_height - track_nameinfo->text_canvas.h) / 2;
    slider->inputbox.canvas.x = track_x + track_namespacing
        + slider_props->max_trackname_width;
    slider->inputbox.canvas.y = track_y;

    slider->track.x = slider->inputbox.canvas.x +
            slider->inputbox.canvas.w + track_namespacing,
    slider->track.y = track_y;
}

void maud_colorpicker_setsliders_trackposition(maud_colorpicker_t* color_picker) {
    color_pickerprops_t* picker_props = &color_picker->props;
    color_sliderprops_t* slider_props = &picker_props->slider_props;
    SDL_Rect *canvas = &color_picker->canvas,
             *preview_canvas = &color_picker->preview_canvas;

    int track_x = canvas->x,
        track_y = preview_canvas->y + preview_canvas->h + 10,
        track_verticalspacing = slider_props->track_verticalspacing;
    for(int i=0;i<4;i++) {
        maud_colorpicker_setslider_trackposition(color_picker, i, track_x, track_y);
        track_y += color_picker->sliders[i].track.h + track_verticalspacing;
    }
}

void maud_colorpicker_setslider_props(maud_colorpicker_t* color_picker, color_sliderprops_t* props) {
    color_sliderprops_t* slider_props = &color_picker->props.slider_props;
    slider_props->track_font = props->track_font;
    slider_props->track_fontsize = props->track_fontsize;
    slider_props->track_segmentwidth = props->track_segmentwidth;
    slider_props->track_height = props->track_height;
    slider_props->track_namecolor = props->track_namecolor;
    slider_props->track_verticalspacing = props->track_verticalspacing;
    slider_props->track_namespacing = props->track_namespacing;
    slider_props->handle_pos = props->handle_pos;
    slider_props->handle_color = props->handle_color;
    slider_props->handle_bordercolor = props->handle_bordercolor;
    slider_props->handle_width = props->handle_width;
    slider_props->handle_height = props->handle_height;
    for(size_t i=0;i<4;i++) {
        char* track_name = props->track_names[i];
        if(track_name) {
            slider_props->track_names[i] = maud_dupstr(track_name, strlen(track_name));
        } else {
            slider_props->track_names[i] = maud_colorpicker_getslidername(i);
        }
    }
    maud_colorpicker_setsliders_trackprop(color_picker);
}

void maud_colorpicker_setprops(maud_colorpicker_t* color_picker, color_pickerprops_t* props) {
    color_pickerprops_t* picker_props = &color_picker->props;
    picker_props->preview_font = props->preview_font;
    picker_props->preview_fontsize = props->preview_fontsize;
    picker_props->preview_width = props->preview_width;
    picker_props->preview_height = props->preview_height;
    picker_props->preview_spacing = props->preview_spacing;
    maud_colorpicker_setpreview_props(color_picker);
    maud_colorpicker_setslider_props(color_picker, &props->slider_props);
    maud_colorpicker_getsize(color_picker);
}

void maud_colorpicker_getsize(maud_colorpicker_t* color_picker) {
    color_slider_t* sliders = color_picker->sliders;
    color_sliderprops_t* slider_props = &color_picker->props.slider_props;
    SDL_Rect* canvas = &color_picker->canvas;
    int track_w = 256 * slider_props->track_segmentwidth;
    SDL_Rect slider_canvas = {
        .w =
        (
            slider_props->max_trackname_width + track_w
            + (slider_props->track_namespacing * 2)
        ),
        .h = slider_props->track_height
    }, canvas_list[] = {
        color_picker->preview_canvas,
        slider_canvas,
        slider_canvas,
        slider_canvas,
        slider_canvas
    };
    size_t canvas_count = sizeof(canvas_list) / sizeof(SDL_Rect);
    int max_width = 0, total_height = slider_props->track_verticalspacing * 3;
    for(size_t i=0;i<canvas_count;i++) {
        SDL_Rect canvas = canvas_list[i];
        if(canvas.w > max_width) {
            max_width = canvas.w;
        }
        total_height += canvas.h;
    }
}

void maud_colorpicker_setposition(maud_colorpicker_t* color_picker, int x, int y) {
    color_picker->canvas.x = x, color_picker->canvas.y = y;
    maud_colorpicker_setpreview_position(color_picker);
    maud_colorpicker_setsliders_trackposition(color_picker);
}

void maud_colorpicker_setslider_handlepos(maud_colorpicker_t* color_picker, int slider_type,
    int handle_pos) {
    color_slider_t* slider = &color_picker->sliders[slider_type];
    SDL_Color color = color_picker->color;
    switch(slider_type) {
        case COLOR_SLIDER_R:
            slider->handle_pos = handle_pos;
            break;
        case COLOR_SLIDER_G:
            slider->handle_pos = handle_pos;
            break;
        case COLOR_SLIDER_B:
            slider->handle_pos = handle_pos;
            break;
        case COLOR_SLIDER_A:
            slider->handle_pos = handle_pos;
            break;
    }
}
void maud_colorpicker_setcolor(maud_colorpicker_t* color_picker, int r, int g, int b, int a) {
    color_picker->color.r = r, color_picker->color.g = g,
    color_picker->color.b = b, color_picker->color.a = a;
    maud_colorpicker_setslider_handlepos(color_picker, COLOR_SLIDER_R, r);
    maud_colorpicker_setslider_handlepos(color_picker, COLOR_SLIDER_G, g);
    maud_colorpicker_setslider_handlepos(color_picker, COLOR_SLIDER_B, b);
    maud_colorpicker_setslider_handlepos(color_picker, COLOR_SLIDER_A, a);
}

SDL_Color maud_colorpicker_getslidersegment_color(int slider_type, int color_value) {
    SDL_Color color_segmentcolor = {0x00, 0x00, 0x00, 0xff};
    switch(slider_type) {
        case COLOR_SLIDER_R:
            color_segmentcolor.r = color_value;
            break;
        case COLOR_SLIDER_G:
            color_segmentcolor.g = color_value;
            break;
        case COLOR_SLIDER_B:
            color_segmentcolor.b = color_value;
            break;
        case COLOR_SLIDER_A:
            color_segmentcolor.r = 255,
            color_segmentcolor.g = 255,
            color_segmentcolor.b = 255,
            color_segmentcolor.a = color_value;
            break;
    }
    return color_segmentcolor;
}

void maud_colorpicker_setcolorfromhandle(maud_colorpicker_t* color_picker) {
    color_slider_t* sliders = color_picker->sliders;
    maud_inputbox_t* hex_inputbox = &color_picker->hex_inputbox;
    inputdata_t* hex_input = &hex_inputbox->input;
    int *r = &sliders[COLOR_SLIDER_R].handle_pos,
        *g = &sliders[COLOR_SLIDER_G].handle_pos,
        *b = &sliders[COLOR_SLIDER_B].handle_pos,
        *a = &sliders[COLOR_SLIDER_A].handle_pos;
    maud_colorpicker_setcolor(color_picker, *r, *g, *b, *a);
    maud_colorpicker_sethex_values(color_picker);
    maud_colorpicker_setrgba_values(color_picker);
}

void maud_colorpicker_setrgba_values(maud_colorpicker_t* color_picker) {
    color_slider_t* sliders = color_picker->sliders;
    char color_value[4] = {0};
    for(size_t i=0;i<4;i++) {
        sprintf(color_value, "%d", sliders[i].handle_pos);
        maud_inputbox_clear(&sliders[i].inputbox);
        maud_inputbox_addinputdata(&sliders[i].inputbox, color_value);
        memset(color_value, 0, 3);
    }
}

void maud_colorpicker_sethex_values(maud_colorpicker_t* color_picker) {
    color_slider_t* sliders = color_picker->sliders;
    char hex_colorvalues[10] = {0};
    maud_inputbox_t* hex_inputbox = &color_picker->hex_inputbox;
    int *r = &sliders[COLOR_SLIDER_R].handle_pos,
        *g = &sliders[COLOR_SLIDER_G].handle_pos,
        *b = &sliders[COLOR_SLIDER_B].handle_pos,
        *a = &sliders[COLOR_SLIDER_A].handle_pos;
    sprintf(hex_colorvalues, "#%02X%02X%02X%02X", *r, *g, *b, *a);
    maud_inputbox_clear(hex_inputbox);
    maud_inputbox_addinputdata(hex_inputbox, hex_colorvalues);
}

void maud_colorpicker_renderslider(maud_t* mplayer, maud_colorpicker_t* color_picker,
    int slider_type) {
    color_slider_t* slider = &color_picker->sliders[slider_type];
    color_sliderprops_t* slider_props = &color_picker->props.slider_props;
    SDL_Color color_segmentcolor = {0};
    SDL_Rect color_segment = {
        .x = slider->track.x,
        .y = slider->track.y,
        .w = slider_props->track_segmentwidth,
        .h = slider->track.h
    };
    SDL_Texture* slidername_texture = maud_textmanager_rendertext(mplayer,
        slider->track_font, &slider->track_nameinfo);
    SDL_RenderCopy(mplayer->renderer, slidername_texture, NULL,
        &slider->track_nameinfo.text_canvas);
    SDL_DestroyTexture(slidername_texture);
    slider->inputbox.show_cursor = slider->inputbox.clicked;
    maud_inputbox_display(mplayer, &slider->inputbox);
    for(int i=0;i<256;i++) {
        color_segmentcolor = maud_colorpicker_getslidersegment_color(slider_type, i);
        SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(color_segmentcolor));
        SDL_RenderDrawRect(mplayer->renderer, &color_segment);
        SDL_RenderFillRect(mplayer->renderer, &color_segment);
        if(mplayer->mouse_buttondown && maud_rect_hover(mplayer, color_segment)) {
            slider->handle_pos = i;
            maud_inputbox_clear(&color_picker->hex_inputbox);
            maud_colorpicker_setcolorfromhandle(color_picker);
        }
        color_segment.x += color_segment.w;
    }

    slider->handle.x = slider->track.x + (slider->handle_pos *
        slider_props->track_segmentwidth);
    slider->handle.y = slider->track.y;
    int handle_insidewidth = slider->handle.w - 4,
        handle_insideheight = slider->handle.h - 4;
    SDL_Rect handle_inside = {
        .x = slider->handle.x + (slider->handle.w - handle_insidewidth) / 2,
        .y = slider->handle.y + (slider->handle.h - handle_insideheight) / 2,
        .w = handle_insidewidth,
        .h = handle_insideheight
    };
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(slider->handle_bordercolor));
    SDL_RenderDrawRect(mplayer->renderer, &slider->handle);
    SDL_RenderFillRect(mplayer->renderer, &slider->handle);

    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(slider->handle_color));
    SDL_RenderDrawRect(mplayer->renderer, &handle_inside);
    SDL_RenderFillRect(mplayer->renderer, &handle_inside);
}

void maud_colorpicker_rendersliders(maud_t* mplayer, maud_colorpicker_t* color_picker) {
    for(size_t i=0;i<4;i++) {
        maud_colorpicker_renderslider(mplayer, color_picker, i);
    }
}

bool maud_colorpicker_inputboxclicked(maud_t* mplayer, maud_colorpicker_t* color_picker) {
    if(!mplayer->mouse_clicked) {
        return false;
    }
    color_slider_t* sliders = color_picker->sliders;
    maud_inputbox_t *inputbox_list[] = {
        &color_picker->hex_inputbox,
        &sliders[COLOR_SLIDER_R].inputbox,
        &sliders[COLOR_SLIDER_G].inputbox,
        &sliders[COLOR_SLIDER_B].inputbox,
        &sliders[COLOR_SLIDER_A].inputbox
    };
    size_t inputbox_count = sizeof(inputbox_list) / sizeof(maud_inputbox_t*);
    bool clicked_status = false;
    for(int i=0;i<inputbox_count;i++) {
        if(maud_inputbox_hover(mplayer, inputbox_list[i])) {
            inputbox_list[i]->clicked = true;
            clicked_status = true;
            mplayer->mouse_clicked = false;
            continue;
        }
        inputbox_list[i]->clicked = false;
    }
    return clicked_status;
}

void maud_colorpicker_display(maud_t* mplayer, maud_colorpicker_t* color_picker) {
    SDL_SetRenderDrawBlendMode(mplayer->renderer, SDL_BLENDMODE_BLEND);
    maud_colorpicker_setpreview_position(color_picker);
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(color_picker->color));
    SDL_RenderDrawRect(mplayer->renderer, &color_picker->preview_canvas);
    SDL_RenderFillRect(mplayer->renderer, &color_picker->preview_canvas);
    maud_colorpicker_inputboxclicked(mplayer, color_picker);
    color_picker->hex_inputbox.show_cursor = color_picker->hex_inputbox.clicked;
    maud_inputbox_display(mplayer, &color_picker->hex_inputbox);
    maud_colorpicker_rendersliders(mplayer, color_picker);
    SDL_SetRenderDrawBlendMode(mplayer->renderer, SDL_BLENDMODE_NONE);
}

void maud_colorpicker_handleslider_inputbox_event(maud_t* mplayer,
    maud_colorpicker_t* color_picker, int slider_type) {
    color_slider_t* sliders = color_picker->sliders;
    maud_inputbox_t* inputbox = &sliders[slider_type].inputbox;
    inputdata_t* input = &inputbox->input;
    int color_value = 0;
    bool update_color = false;
    int *r = &sliders[COLOR_SLIDER_R].handle_pos,
        *g = &sliders[COLOR_SLIDER_G].handle_pos,
        *b = &sliders[COLOR_SLIDER_B].handle_pos,
        *a = &sliders[COLOR_SLIDER_A].handle_pos;
    switch(mplayer->e.type) {
        case SDL_KEYDOWN:
            int key = mplayer->e.key.keysym.sym;
            if(SDL_GetModState() & KMOD_CTRL && (key == SDLK_a
                || key == SDLK_c || key == SDLK_v)) {
                maud_inputbox_handle_events(mplayer, inputbox);
                if(key == SDLK_v) {
                    color_value = atoi(input->data);
                    color_value = (color_value > 255) ? 255 : color_value;
                    maud_colorpicker_setslider_handlepos(color_picker, slider_type,
                        color_value);
                    update_color = true;
                }
                break;
            }
            if(key <= SDLK_9 && key >= SDLK_0 && input->character_count < 3) {
                maud_inputbox_addinputdata(inputbox, (const char[2]){key});
                if(input->data) {
                    color_value = atoi(input->data);
                    color_value = (color_value > 255) ? 255 : color_value;
                    maud_colorpicker_setslider_handlepos(color_picker, slider_type,
                    color_value);
                    update_color = true;
                }
            } else if(key == SDLK_LEFT || key == SDLK_RIGHT) {
                maud_inputbox_handle_events(mplayer, inputbox);
            } else if(key == SDLK_BACKSPACE) {
                maud_inputbox_handle_events(mplayer, inputbox);
                if(input->data) {
                    color_value = atoi(input->data);
                    color_value = (color_value > 255) ? 255 : color_value;
                    maud_colorpicker_setslider_handlepos(color_picker, slider_type,
                    color_value);
                    update_color = true;
                }
            }
            break;
    }
    if(update_color) {
        maud_colorpicker_setcolor(color_picker, *r, *g, *b, *a);
        maud_colorpicker_sethex_values(color_picker);
    }
}

bool maud_colorpicker_handlehex_inputpaste(maud_colorpicker_t* color_picker) {
    maud_inputbox_t* hex_inputbox = &color_picker->hex_inputbox;
    inputdata_t* hex_input = &hex_inputbox->input;
    char* clipboard_data = SDL_GetClipboardText();
    if(!clipboard_data) {
        return false;
    }
    maud_inputbox_deleteselection(hex_inputbox);
    char hex_buff[10] = {'#'};
    size_t hex_bufflen = 1, clipboard_datalen = strlen(clipboard_data);
    for(size_t i=0;i<clipboard_datalen;i++) {
        if(i == 0 && clipboard_data[0] == '#') {
            continue;
        }
        char hex_char = tolower(clipboard_data[i]);
        if(!((hex_char <= SDLK_9 && hex_char >= SDLK_0 ||
            hex_char <= SDLK_f && hex_char >= SDLK_a))) {
            break;
        }
        strcat(hex_buff+1, (const char[2]){toupper(clipboard_data[i])});
        hex_bufflen++;
        if(hex_bufflen == 9) {
            break;
        }
    }
    if(hex_bufflen == 1) {
        strcat(hex_buff, "FFFFFFFF");
        hex_bufflen += 8;
    }
    if(!hex_input->data) {
        maud_inputbox_addinputdata(hex_inputbox, hex_buff);
        return true;
    }
    size_t hex_nullindex = 10 - hex_input->character_count;
    hex_buff[hex_nullindex] = '\0';
    if(hex_nullindex == 1) {
        return false;
    }
    maud_inputbox_addinputdata(hex_inputbox, hex_buff+1);
    free(clipboard_data);
    return true;
}

void maud_colorpicker_handlehex_inputbox_event(maud_t* mplayer,
    maud_colorpicker_t* color_picker) {
    color_slider_t* sliders = color_picker->sliders;
    maud_inputbox_t* hex_inputbox = &color_picker->hex_inputbox;
    inputdata_t* hex_input = &hex_inputbox->input;
    int *r = &sliders[COLOR_SLIDER_R].handle_pos,
        *g = &sliders[COLOR_SLIDER_G].handle_pos,
        *b = &sliders[COLOR_SLIDER_B].handle_pos,
        *a = &sliders[COLOR_SLIDER_A].handle_pos;
    bool update_color = false;
    switch(mplayer->e.type) {
        case SDL_KEYDOWN:
            int key = mplayer->e.key.keysym.sym;
            if(SDL_GetModState() & KMOD_CTRL && (key == SDLK_a
                || key == SDLK_c || key == SDLK_v)) {
                if(key == SDLK_v) {
                    if(maud_colorpicker_handlehex_inputpaste(color_picker)) {
                        sscanf(hex_input->data, "#%2x%2x%2x%2x", r, g, b, a);
                    }
                    update_color = true;
                    break;
                }
                maud_inputbox_handle_events(mplayer, hex_inputbox);
                break;
            }
            if((key <= SDLK_9 && key >= SDLK_0 ||
                key <= SDLK_f && key >= SDLK_a) && hex_input->character_count < 9) {
                maud_inputbox_addinputdata(hex_inputbox, (const char[2]){toupper(key)});
                if(hex_input->character_count > 1) {
                    sscanf(hex_input->data, "#%2x%2x%2x%2x", r, g, b, a);
                    update_color = true;
                }
            } else if(key == SDLK_LEFT && hex_input->cursor_pos > 1) {
                if(hex_input->selection_count && !hex_input->selection_start) {
                    hex_input->selection_start = 1;
                }
                maud_inputbox_handle_events(mplayer, hex_inputbox);
            } else if(key == SDLK_RIGHT) {
                maud_inputbox_handle_events(mplayer, hex_inputbox);
            } else if(key == SDLK_BACKSPACE && (hex_input->cursor_pos > 1
                || hex_input->selection_count)) {
                maud_inputbox_handle_events(mplayer, hex_inputbox);
                sscanf(hex_input->data, "#%2x%2x%2x%2x", r, g, b, a);
                update_color = true;
            }
            break;
    }
    if(update_color) {
        maud_colorpicker_setcolor(color_picker, *r, *g, *b, *a);
        maud_colorpicker_setrgba_values(color_picker);
    }
}

void maud_colorpicker_handleinputbox_events(maud_t* mplayer,
    maud_colorpicker_t* color_picker) {
    color_slider_t* sliders = color_picker->sliders;
    if(color_picker->hex_inputbox.clicked) {
        maud_colorpicker_handlehex_inputbox_event(mplayer, color_picker);
    }
    for(size_t i=0;i<4;i++) {
        if(sliders[i].inputbox.clicked) {
            maud_colorpicker_handleslider_inputbox_event(mplayer, color_picker, i);
            break;
        }
    }
}

void maud_colorpicker_destroy(maud_colorpicker_t* color_picker) {
    color_sliderprops_t* slider_props = &color_picker->props.slider_props;
    maud_inputbox_destroy(&color_picker->hex_inputbox);
    for(size_t i=0;i<4;i++) {
        free(slider_props->track_names[i]);
        maud_inputbox_destroy(&color_picker->sliders[i].inputbox);
    }
}