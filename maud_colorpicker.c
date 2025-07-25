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

void maud_colorpicker_setslider_trackprop(maud_colorpicker_t* color_picker, int slider_type,
    TTF_Font* track_font, const char* track_name, int track_fontsize, SDL_Color track_namecolor,
    int track_namespacing, int track_segmentwidth, int track_height) {
    color_picker->sliders[slider_type].track_font = track_font;
    text_info_t* track_nameinfo = &color_picker->sliders[slider_type].track_nameinfo;
    if(track_name) {
        color_picker->sliders[slider_type].track_name = maud_dupstr(track_name, strlen(track_name));
    } else {
        color_picker->sliders[slider_type].track_name = maud_colorpicker_getslidername(slider_type);
    }
    color_picker->sliders[slider_type].track_namespacing = track_namespacing;
    track_nameinfo->text_color = track_namecolor;
    track_nameinfo->font_size = track_fontsize;
    track_nameinfo->text = color_picker->sliders[slider_type].track_name;
    maud_textmanager_sizetext(track_font, track_nameinfo);
    if(track_nameinfo->text_canvas.w > color_picker->max_trackname_width) {
        color_picker->max_trackname_width = track_nameinfo->text_canvas.w;
    }
    color_picker->sliders[slider_type].track.w = 256 * track_segmentwidth,
    color_picker->sliders[slider_type].track.h = track_height;
    color_picker->sliders[slider_type].track_segmentwidth = track_segmentwidth;
    color_picker->sliders[slider_type].inputbox = maud_inputbox_create(
        track_font,
        track_fontsize,
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
    color_picker->sliders[slider_type].inputbox.show_cursor = false;
}

void maud_colorpicker_setslider_handleprop(maud_colorpicker_t* color_picker, int slider_type,
    int handle_width, int handle_height, SDL_Color handle_color, SDL_Color handle_bordercolor) {
    color_picker->sliders[slider_type].handle.w = handle_width,
    color_picker->sliders[slider_type].handle.h = handle_height,
    color_picker->sliders[slider_type].handle_pos = 255;
    color_picker->sliders[slider_type].handle_color = handle_color;
    color_picker->sliders[slider_type].handle_bordercolor = handle_bordercolor;
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

void maud_colorpicker_setsliders_props(maud_colorpicker_t* color_picker,
    color_tracknameattrib_t track_nameattribs[4], int track_segmentwidth,
    int track_height, int handle_width, int handle_height,
    SDL_Color handle_color, SDL_Color handle_bordercolor) {
    for(size_t i=0;i<4;i++) {
        maud_colorpicker_setslider_trackprop(color_picker, i, track_nameattribs[i].track_font,
            track_nameattribs[i].track_name, track_nameattribs[i].track_fontsize,
            track_nameattribs[i].track_namecolor, track_nameattribs[i].track_namespacing,
            track_segmentwidth, track_height);
        maud_colorpicker_setslider_handleprop(color_picker, i, handle_width, handle_height, handle_color, handle_bordercolor);
    }
    maud_colorpicker_setcolorfromhandle(color_picker);
}

void maud_colorpicker_setslider_trackposition(maud_colorpicker_t* color_picker, int slider_type, int track_x, int track_y) {
    int track_height = color_picker->sliders[slider_type].track.h,
        track_namespacing = color_picker->sliders[slider_type].track_namespacing;
    text_info_t* track_nameinfo = &color_picker->sliders[slider_type].track_nameinfo;
    track_nameinfo->text_canvas.x = track_x;
    track_nameinfo->text_canvas.y = track_y + (track_height - track_nameinfo->text_canvas.h) / 2;
    color_picker->sliders[slider_type].inputbox.canvas.x = track_x + track_namespacing + color_picker->max_trackname_width;
    color_picker->sliders[slider_type].inputbox.canvas.y = track_y;

    color_picker->sliders[slider_type].track.x = color_picker->sliders[slider_type].inputbox.canvas.x +
        color_picker->sliders[slider_type].inputbox.canvas.w + track_namespacing,
    color_picker->sliders[slider_type].track.y = track_y;
}

void maud_colorpicker_setsliders_trackposition(maud_colorpicker_t* color_picker, int track_x, int track_y,
    int track_spacing) {
    for(int i=0;i<4;i++) {
        maud_colorpicker_setslider_trackposition(color_picker, i, track_x, track_y);
        track_y += color_picker->sliders[i].track.h + track_spacing;
    }   
}

void maud_colorpicker_setpreview_position(maud_colorpicker_t* color_picker, int x, int y) {
    color_picker->preview_canvas.x = x,
    color_picker->preview_canvas.y = y;

    color_picker->hex_inputbox.canvas.x = x + color_picker->preview_canvas.w + 10;
    color_picker->hex_inputbox.canvas.y = y + (color_picker->preview_canvas.h -
        color_picker->hex_inputbox.canvas.h) / 2;
}

void maud_colorpicker_setpreview_props(maud_colorpicker_t* color_picker, TTF_Font* font,
    int font_size, int width, int height) {
    color_picker->preview_canvas.w = width,
    color_picker->preview_canvas.h = height;

    color_picker->hex_inputbox = maud_inputbox_create(font, font_size,
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
    maud_colorpicker_setcolor(
        color_picker,
        sliders[COLOR_SLIDER_R].handle_pos,
        sliders[COLOR_SLIDER_G].handle_pos,
        sliders[COLOR_SLIDER_B].handle_pos,
        sliders[COLOR_SLIDER_A].handle_pos
    );
    char color_value[4] = {0}, hex_colorvalue[10] = {0};
    bool init_hexinput = false;
    if(!hex_input->data) {
        maud_inputbox_addinputdata(hex_inputbox, "#");
        init_hexinput = true;
    }
    for(int i=0;i<4;i++) {
        if(init_hexinput) {
            sprintf(hex_colorvalue, "%02X", sliders[i].handle_pos);
            maud_inputbox_addinputdata(hex_inputbox, hex_colorvalue);
        }
        sprintf(color_value, "%d", sliders[i].handle_pos);
        maud_inputbox_clear(&sliders[i].inputbox);
        maud_inputbox_addinputdata(&sliders[i].inputbox, color_value);
        memset(color_value, 0, 4);
    }
}

void maud_colorpicker_renderslider(maud_t* mplayer, maud_colorpicker_t* color_picker,
    int slider_type) {
    color_slider_t* slider = &color_picker->sliders[slider_type];
    SDL_Color color_segmentcolor = {0};
    SDL_Rect color_segment = {
        .x = slider->track.x,
        .y = slider->track.y,
        .w = slider->track_segmentwidth,
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
        slider->track_segmentwidth);
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
    maud_inputbox_t* inputbox = &color_picker->sliders[slider_type].inputbox;
    inputdata_t* input = &inputbox->input;
    int color_value = 0;
    bool update_color = false;
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
        maud_colorpicker_setcolorfromhandle(color_picker);
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
        maud_colorpicker_setcolorfromhandle(color_picker);
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
    maud_inputbox_destroy(&color_picker->hex_inputbox);
    for(size_t i=0;i<4;i++) {
        free(color_picker->sliders[i].track_name); color_picker->sliders[i].track_name = NULL;
        maud_inputbox_destroy(&color_picker->sliders[i].inputbox);
    }
}