#include "maud_playlistmanager.h"


void maud_playlistmanager_buttonbar_initialize(maud_t* maud) {
    maud_buttonbar_t* button_bar = &maud->playlist_manager.button_bar;
    if(button_bar->init) {
        return;
    }
    maud_playlistmanager_buttonbar_initialize_newplaylistbutton(maud, button_bar);
    maud_playlistmanager_buttonbar_intialize_playlistlayout_text(maud, button_bar);
    maud_playlistmanager_buttonbar_initialize_playlistlayout_toggleswitch(maud, button_bar);
    SDL_Rect *canvas_ptrlist[] = {
        &button_bar->new_playlistbtn.canvas,
        &button_bar->layout_switch.canvas,
        &button_bar->playlistlayout_text.text_canvas 
    },
    canvas_list[] = {
        button_bar->new_playlistbtn.canvas,
        button_bar->layout_switch.canvas,
        button_bar->playlistlayout_text.text_canvas
    };
    size_t canvas_count = sizeof(canvas_list) / sizeof(SDL_Rect);
    int max_height = maud_playlistmanager_findmaxheight(canvas_list, canvas_count);
    button_bar->canvas.y = tab_info[0].text_canvas.y +
        tab_info[0].text_canvas.h + UNDERLINE_THICKNESS + 5;
    button_bar->canvas.w = maud->win_width, button_bar->canvas.h = max_height + 10;
    for(size_t i=0;i<canvas_count;i++) {
        canvas_ptrlist[i]->y = button_bar->canvas.y + (button_bar->canvas.h - canvas_ptrlist[i]->h) / 2;
    }
}

void maud_playlistmanager_buttonbar_initialize_newplaylistbutton(maud_t* maud,
    maud_buttonbar_t* button_bar) {
    maud_newplaylistbtn_t new_playlistbtn = {
        .canvas = {
            .x = 5
        },
        .color = {0xF4, 0x60, 0x36, 0xFF},
        .text = {
            .font_size = 18,
            .text = "New Playlist",
            .text_color = {0}
        },
        .icon_canvas = {
            .w = 20, .h = 20
        },
        .content_spacing = 5
    };
    SDL_Rect *icon_canvas = &new_playlistbtn.icon_canvas,
             *text_canvas = &new_playlistbtn.text.text_canvas,
             *canvas = &new_playlistbtn.canvas;
    maud_textmanager_sizetext(maud->font, &new_playlistbtn.text);
    int max_height = (icon_canvas->h > text_canvas->h) ? icon_canvas->h :
        text_canvas->h;
    canvas->w = icon_canvas->w + text_canvas->w + new_playlistbtn.content_spacing + 10;
    canvas->h = max_height + 20;
    button_bar->new_playlistbtn = new_playlistbtn;
    button_bar->init = true;
}

void maud_playlistmanager_buttonbar_intialize_playlistlayout_text(maud_t* maud,
    maud_buttonbar_t* button_bar) {
    text_info_t playlistlayout_text = {
        .font_size = 20,
        .text = "Playlist Layout:",
        .text_color = white,
    };
    maud_textmanager_sizetext(maud->font, &playlistlayout_text);
    button_bar->playlistlayout_text = playlistlayout_text;
}

void maud_playlistmanager_buttonbar_initialize_playlistlayout_toggleswitch(maud_t* maud,
    maud_buttonbar_t* button_bar) {
    maud_playlistlayout_switch_t layout_switch = {
        .view_text = {
            .font_size = 20,
            .text = maud_playlistmanager_layout_getcurrentname_view(maud),
            .text_color = white
        },
        .icon_canvas = {
            .w = 30, .h = 30,
        },
        .dropdown_iconcanvas = {
            .w = 30, .h = 30,
        },
        .content_spacing = 5
    };
    maud_textmanager_sizetext(maud->font, &layout_switch.view_text);
    SDL_Rect canvases[] = {
        button_bar->playlistlayout_text.text_canvas,
        layout_switch.view_text.text_canvas,
        layout_switch.icon_canvas,
        layout_switch.dropdown_iconcanvas
    };
    size_t canvas_count = sizeof(canvases) / sizeof(SDL_Rect);
    int max_height = 0, width = 0;
    for(size_t i=0;i<canvas_count;i++) {
        width += canvases[i].w;
        if(i != canvas_count-1) {
            width += layout_switch.content_spacing;
        }
        if(canvases[i].h > max_height) {
            max_height = canvases[i].h;
        }
    }
    layout_switch.canvas.w = width;
    layout_switch.canvas.h = max_height;
    button_bar->layout_switch = layout_switch;
}

void maud_playlistmanager_buttonbar_display(maud_t* maud) {
    maud_buttonbar_t* button_bar = &maud->playlist_manager.button_bar;
    maud_playlistmanager_buttonbar_initialize(maud);
    maud_playlistmanager_buttonbar_display_newplaylistbutton(maud, button_bar);
    maud_playlistmanager_buttonbar_display_playlistlayout_text(maud, button_bar);
    maud_playlistmanager_buttonbar_display_playlistlayout_toggleswitch(maud, button_bar);
}

void maud_playlistmanager_buttonbar_display_newplaylistbutton(maud_t* maud,
    maud_buttonbar_t* button_bar) {
    maud_newplaylistbtn_t* new_playlistbtn = &button_bar->new_playlistbtn;
    SDL_Rect* canvas = &new_playlistbtn->canvas;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(new_playlistbtn->color));
    SDL_RenderDrawRect(maud->renderer, canvas);
    SDL_RenderFillRect(maud->renderer, canvas);

    SDL_Rect* icon_canvas = &new_playlistbtn->icon_canvas;
    icon_canvas->x = new_playlistbtn->canvas.x + new_playlistbtn->content_spacing;
    icon_canvas->y = new_playlistbtn->canvas.y + (canvas->h - icon_canvas->h) / 2;
    SDL_Texture* icon_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE]
        [music_addplaylistbtn.texture_idx];
    SDL_RenderCopy(maud->renderer, icon_texture, NULL, icon_canvas);

    SDL_Rect* text_canvas = &new_playlistbtn->text.text_canvas;
    text_canvas->x = icon_canvas->x + icon_canvas->w
        + new_playlistbtn->content_spacing;
    text_canvas->y = canvas->y + (canvas->h - text_canvas->h) / 2;
    maud_playlistmanager_buttonbar_handle_newplaylistbutton(maud, button_bar);
    SDL_Texture* text_texture = maud_textmanager_rendertext(maud, maud->font,
        &new_playlistbtn->text);
    SDL_RenderCopy(maud->renderer, text_texture, NULL, text_canvas);
    SDL_DestroyTexture(text_texture);
}
void maud_playlistmanager_buttonbar_display_playlistlayout_text(maud_t* maud,
    maud_buttonbar_t* button_bar) {
    maud_playlistlayout_switch_t* layout_switch = &button_bar->layout_switch;
    text_info_t* text = &button_bar->playlistlayout_text;
    SDL_Rect* text_canvas = &text->text_canvas;
    text_canvas->x = maud->win_width - layout_switch->canvas.w - 10;
    text_canvas->y = button_bar->canvas.y + (button_bar->canvas.h -
        text_canvas->h) / 2;
    SDL_Texture* text_texture = maud_textmanager_rendertext(maud, maud->font, text);
    SDL_RenderCopy(maud->renderer, text_texture, NULL, text_canvas);
    SDL_DestroyTexture(text_texture);
}

void maud_playlistmanager_buttonbar_display_playlistlayout_toggleswitch(maud_t* maud,
    maud_buttonbar_t* button_bar) {
    maud_playlistlayout_switch_t* layout_switch = &button_bar->layout_switch;
    int content_spacing = layout_switch->content_spacing;
    text_info_t* playlistlayout_text = &button_bar->playlistlayout_text;
    SDL_Rect* canvas = &layout_switch->canvas,
        *icon_canvas = &layout_switch->icon_canvas;

    canvas->x = playlistlayout_text->text_canvas.x +
                playlistlayout_text->text_canvas.w + content_spacing;
    maud_playlistmanager_buttonbar_handle_playlistlayout_toggleswitch(maud, button_bar);
    
    // Render current layout icon
    SDL_Texture* icon_texture = maud_playlistmanager_layout_getcurrenticon(maud);
    icon_canvas->x = canvas->x;
    icon_canvas->y = canvas->y + (canvas->h - icon_canvas->h) / 2;
    SDL_RenderCopy(maud->renderer, icon_texture, NULL, icon_canvas);
    SDL_DestroyTexture(icon_texture);

    // Render view text
    text_info_t* view_text = &layout_switch->view_text;
    SDL_Rect* view_textcanvas = &view_text->text_canvas;
    view_textcanvas->x = icon_canvas->x + icon_canvas->w +
                         content_spacing;
    view_textcanvas->y = canvas->y + (canvas->h -
        view_textcanvas->h) / 2;
    SDL_Texture* text_texture = maud_textmanager_rendertext(maud, maud->font,
        view_text);
    SDL_RenderCopy(maud->renderer, text_texture, NULL, view_textcanvas);
    SDL_DestroyTexture(text_texture);

    // Render drop down icon
    SDL_Rect* dropdown_iconcanvas = &layout_switch->dropdown_iconcanvas;
    dropdown_iconcanvas->x = view_textcanvas->x + view_textcanvas->w + content_spacing;
    dropdown_iconcanvas->y = canvas->y + (canvas->h - dropdown_iconcanvas->h) / 2;
    SDL_Texture* dropdown_texture = IMG_LoadTexture(maud->renderer, "images/arrow-down.png");
    SDL_RenderCopy(maud->renderer, dropdown_texture, NULL, dropdown_iconcanvas);
    SDL_DestroyTexture(dropdown_texture);
}

void maud_playlistmanager_buttonbar_handle_newplaylistbutton(maud_t* maud,
    maud_buttonbar_t* button_bar) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_newplaylistbtn_t* new_playlistbtn = &button_bar->new_playlistbtn;
    maud_newplaylist_input_t* new_playlistinput = &playlist_manager->new_playlistinput;
    maud_inputbox_t* inputbox = &new_playlistinput->inputbox;
    if(maud_rect_hover(maud, new_playlistbtn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            new_playlistbtn->clicked = !new_playlistbtn->clicked;
            printf("New_playlistbtn->clicked = %d\n", new_playlistbtn->clicked);
            if(!new_playlistbtn->clicked) {
                printf("Here because clicked is now false\n");
                maud_inputbox_clear(inputbox);
            }
            maud->mouse_clicked = false;
        }
    }
}
void maud_playlistmanager_buttonbar_handle_playlistlayout_toggleswitch(maud_t* maud,
    maud_buttonbar_t* button_bar) {
    if(maud_rect_hover(maud, button_bar->layout_switch.canvas)) {
        printf("You are hovering over the layout switch\n");
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            printf("you clicked the layout switch\n");
            button_bar->layout_switch.clicked = !(button_bar->layout_switch.clicked);
            maud->mouse_clicked = false;
        }
    }
}