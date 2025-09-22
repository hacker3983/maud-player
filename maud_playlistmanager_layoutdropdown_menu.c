#include "maud_playlistmanager.h"

void maud_playlistmanager_layoutdropdown_menu_init_names(maud_t* maud,
    maud_playlistlayout_switchdropdown_t* dropdown) {
    maud_playlistlayoutbtn_t* layout_btns = dropdown->layout_btns;
    int horizontal_spacing = dropdown->horizontal_spacing,
        max_height = 0, total_width = horizontal_spacing * (PLAYLIST_LAYOUTCOUNT-1);
    for(size_t i=0;i<PLAYLIST_LAYOUTCOUNT;i++) {
        maud_playlistlayoutbtn_t* layout_btn = &layout_btns[i];
        char* name_ptr = maud_playlistmanager_layout_getname(i);
        layout_btn->text.text = maud_dupstr(name_ptr, strlen(name_ptr));
        maud_textmanager_sizetext(maud->font, &layout_btn->text);
        int current_width = layout_btn->text.text_canvas.w,
            current_height = layout_btn->text.text_canvas.h;
        layout_btn->canvas.w = current_width + 15;
        layout_btn->canvas.h = current_height + layout_btn->icon_canvas.h + 15;
        total_width += layout_btn->canvas.w;
        if(layout_btn->canvas.h > max_height) {
            max_height = layout_btn->canvas.h;
        }
    }
    dropdown->canvas.w = total_width + 20;
    dropdown->canvas.h = max_height + 20;
    dropdown->canvas.x = maud->win_width - dropdown->canvas.w - 20;
    int center_x = dropdown->canvas.x + (dropdown->canvas.w - total_width) / 2,
        center_y = dropdown->canvas.y + (dropdown->canvas.h - max_height) / 2;
    for(size_t i=0;i<PLAYLIST_LAYOUTCOUNT;i++) {
        maud_playlistlayoutbtn_t* layout_btn = &layout_btns[i];
        SDL_Rect* icon_canvas = &layout_btn->icon_canvas;
        SDL_Rect* text_canvas = &layout_btn->text.text_canvas;
        int total_height = icon_canvas->h + text_canvas->h + 5;
        layout_btn->canvas.x = center_x;
        layout_btn->canvas.y = center_y;

        icon_canvas->x = layout_btn->canvas.x + (layout_btn->canvas.w - icon_canvas->w) / 2;
        icon_canvas->y = layout_btn->canvas.y + (layout_btn->canvas.h - total_height) / 2;

        text_canvas->x = layout_btn->canvas.x + (layout_btn->canvas.w - text_canvas->w) / 2;
        text_canvas->y = icon_canvas->y + icon_canvas->h + 5;
        center_x += layout_btn->canvas.w + horizontal_spacing;
    }
}

void maud_playlistmanager_layoutdropdown_menu_init(maud_t* maud, maud_playlistlayout_switch_t* layout_switch) {
    maud_playlistlayout_switchdropdown_t dropdown = {
        .canvas = {
            .y = layout_switch->canvas.y + layout_switch->canvas.h
        },
        .horizontal_spacing = 10,
        .color = {0x00, 0x00, 0xff, 0xff},
        .layout_btns = {
            {
                .color = {0xF4, 0x60, 0x36, 0xFF},
                .icon_canvas = {
                    .w = 20, .h = 20
                },
                .text = {
                    .font_size = 20,
                    .text_color = white,
                }
            },
            {
                .color = {0xF4, 0x60, 0x36, 0xFF},
                .icon_canvas = {
                    .w = 20, .h = 20
                },
                .text = {
                    .font_size = 20,
                    .text_color = white,
                }
            }
        }
    };
    maud_playlistmanager_layoutdropdown_menu_init_names(maud, &dropdown);
    layout_switch->dropdown_menu = dropdown;
}

void maud_playlistmanager_layoutdropdown_menu_display(maud_t* maud) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_buttonbar_t* button_bar = &playlist_manager->button_bar;
    maud_playlistlayout_switch_t* layout_switch = &button_bar->layout_switch;
    maud_playlistlayout_switchdropdown_t* dropdown = &layout_switch->dropdown_menu;
    maud_playlistlayoutbtn_t* layout_btns = dropdown->layout_btns;
    if(!layout_switch->clicked) {
        return;
    }
    int content_spacing = layout_switch->content_spacing,
        layout_type = playlist_manager->layout_type;
    maud_playlistmanager_layoutdropdown_menu_init(maud, layout_switch);

    SDL_SetRenderDrawColor(maud->renderer, color_toparam(dropdown->color));
    SDL_RenderDrawRect(maud->renderer, &dropdown->canvas);
    SDL_RenderFillRect(maud->renderer, &dropdown->canvas);

    for(size_t i=0;i<PLAYLIST_LAYOUTCOUNT;i++) {
        maud_playlistlayoutbtn_t* layout_btn = &layout_btns[i];
        bool hovering = maud_rect_hover(maud, layout_btn->canvas);
        if(hovering) {
            maud_setcursor(maud, MAUD_CURSOR_POINTER);
        }
        if(i == playlist_manager->layout_type) {
            SDL_SetRenderDrawColor(maud->renderer, color_toparam(layout_btn->color));
            SDL_RenderDrawRect(maud->renderer, &layout_btn->canvas);
            SDL_RenderFillRect(maud->renderer, &layout_btn->canvas);
        } else if(hovering) {
            if(maud->mouse_clicked) {
                playlist_manager->playlist_props.init = false;
                playlist_manager->layout_type = i;
                maud->mouse_clicked = false;
            }
            SDL_SetRenderDrawColor(maud->renderer, color_toparam(layout_btn->color));
            SDL_RenderDrawRect(maud->renderer, &layout_btn->canvas);
            SDL_RenderFillRect(maud->renderer, &layout_btn->canvas);
        }
        SDL_Texture* icon_texture = maud_playlistmanager_layout_geticon(maud, i);
        SDL_RenderCopy(maud->renderer, icon_texture, NULL, &layout_btn->icon_canvas);
        SDL_DestroyTexture(icon_texture);

        SDL_Texture* text_texture = maud_textmanager_rendertext(maud, maud->font, &layout_btn->text);
        SDL_RenderCopy(maud->renderer, text_texture, NULL, &layout_btn->text.text_canvas);
        SDL_DestroyTexture(text_texture);
        free(layout_btn->text.text);
    }
}