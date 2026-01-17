#include "maud_selectionmenu.h"

void maud_selectionmenu_renderer_init_menu(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    int start_x = 0, start_y = maud->music_searchbar.y + maud->music_searchbar.h;
    maud_selectionmenu_t new_menu = {
        .canvas = {
            .x = start_x, .y = start_y,
            .w = maud->win_width, .h = 10
        },
        .color = {0x0E, 0x34, 0xA0, 0xFF},
        .select_allbtn = {
            .canvas = {
                .x = start_x + 10,
                .w = 10, .h = 10
            },
            .checkbox = {
                .canvas = {
                    .w = checkbox_size.w,
                    .h = 30
                },
                .color = white,
                .tick_color = {0x00, 0xff, 0x00, 0xff},
                .fill_color = {0x00, 0x00, 0x00, 0xff},
                .fill = selection_menu->select_allbtn.checkbox.fill,
                .tick = selection_menu->select_allbtn.checkbox.tick
            },
            .selection_info = {
                .font_size = 16,
                .text_color = {0},
                .text_canvas = {
                    .x = 10
                }
            },
            .clicked = selection_menu->select_allbtn.clicked
        },
        .playbtn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .icon_canvas = {
                .x = 10,
                .w = 24, .h = 30
            },
            .img_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE]
                [music_listplaybtn.texture_idx],
            .text = {
                .font_size = 16,
                .text = "Play",
                .text_color = white
            },
            .clicked = selection_menu->playbtn.clicked
        },
        .playnextbtn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .icon_canvas = {
                .w = 24, .h = 30
            },
            .img_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE]
                [music_listplaybtn.texture_idx],
            .text = {
                .font_size = 16,
                .text = "Play next",
                .text_color = white
            },
            .clicked = selection_menu->playnextbtn.clicked
        },
        .addtobtn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .img_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE]
                [music_addtobtn.texture_idx],
            .text = {
                .font_size = 16,
                .text = "Add to",
                .text_color = white
            },
            .clicked = selection_menu->addtobtn.clicked
        },
        .removebtn = {
            .canvas = {
                .w = 20, .h = 20
            },
            .color = black,
            .icon_canvas = {
                .w = 24, .h = 24
            },
            .img_path = "images/remove 2.png",
            .text = {
                .font_size = 18,
                .text = "Remove",
                .text_color = white
            },
            .clicked = selection_menu->removebtn.clicked
        },
        .moveup_btn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .img_path = "images/up-arrow.png",
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .text = {
                .font_size = 16,
                .text = "Move up",
                .text_color = white
            },
            .clicked = selection_menu->moveup_btn.clicked
        },
        .movedown_btn = {
            .canvas = {
                .w = 15, .h = 10
            },
            .color = black,
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .img_path = "images/down-arrow.png",
            .text = {
                .font_size = 16,
                .text = "Move down",
                .text_color = white
            },
            .clicked = selection_menu->movedown_btn.clicked
        }
    };
    maud_selectionmenu_renderer_init_btns(maud, &new_menu);
    new_menu.canvas.h += maud_playlistmanager_findmaxheight(
        (SDL_Rect[]) {
            new_menu.select_allbtn.canvas,
            new_menu.playbtn.canvas,
            new_menu.playnextbtn.canvas,
            new_menu.removebtn.canvas,
            new_menu.moveup_btn.canvas,
            new_menu.movedown_btn.canvas
        },
        6
    );
    maud_selectionmenu_renderer_init_btns_position(maud, &new_menu);
    *selection_menu = new_menu;
}

void maud_selectionmenu_renderer_init_selectioninfo(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_checkbox_t* checkbox = &selection_menu->select_allbtn.checkbox;
    text_info_t *selection_info = &selection_menu->select_allbtn.selection_info;
    int number_len = 0;
    char* number_str = maud_size_t_tostring(maud->tick_count, &number_len);
    if(!number_str) {
        fprintf(stderr, "Failed to get the selection count length cause memory allocation failure\n");
        return;
    }
    char* selection_text = calloc(number_len + 16, sizeof(char));
    sprintf(selection_text, "%s song", number_str);
    (maud->tick_count > 1) ? strcat(selection_text, "s") : 0;
    strcat(selection_text, " selected");
    selection_info->text = selection_text;
    maud_textmanager_sizetext(maud->font, selection_info);
    free(number_str);
}

void maud_selectionmenu_renderer_init_selectallbtn(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_selectallbtn_t *select_all = &selection_menu->select_allbtn;
    maud_checkbox_t *checkbox = &select_all->checkbox;
    text_info_t* selection_info = &select_all->selection_info;
    maud_selectionmenu_renderer_init_selectioninfo(maud, selection_menu);
    select_all->canvas.w += checkbox->canvas.w + selection_info->text_canvas.w;
    select_all->canvas.h += maud_playlistmanager_findmaxheight(
        (SDL_Rect[]) {
            checkbox->canvas,
            selection_info->text_canvas
        },
        2
    );
}

void maud_selectionmenu_renderer_init_btns(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_renderer_init_selectallbtn(maud, selection_menu);
    maud_selectionmenubtn_t *buttons[] = {
        &selection_menu->playbtn,
        &selection_menu->playnextbtn,
        &selection_menu->addtobtn,
        &selection_menu->removebtn,
        &selection_menu->moveup_btn,
        &selection_menu->movedown_btn
    };
    size_t button_count = sizeof(buttons) / sizeof(maud_selectionmenubtn_t*);
    for(size_t i=0;i<button_count;i++) {
        SDL_Rect *canvas = &buttons[i]->canvas,
                 *icon_canvas = &buttons[i]->icon_canvas,
                 *text_canvas = &buttons[i]->text.text_canvas;
        maud_textmanager_sizetext(maud->font, &buttons[i]->text);
        canvas->w += icon_canvas->w + text_canvas->w;
        canvas->h += maud_playlistmanager_findmaxheight(
            (SDL_Rect[]) {
                *icon_canvas,
                *text_canvas
            },
            2
        );
    }
}

void maud_selectionmenu_renderer_init_selectallbtn_position(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_selectallbtn_t *select_allbtn = &selection_menu->select_allbtn;
    maud_checkbox_t* checkbox = &select_allbtn->checkbox;
    SDL_Rect *canvas = &select_allbtn->canvas,
             *text_canvas = &select_allbtn->selection_info.text_canvas;

    canvas->y = selection_menu->canvas.y + (selection_menu->canvas.h
        - canvas->h) / 2;
    checkbox->canvas.x = canvas->x,
    checkbox->canvas.y = canvas->y + (canvas->h -
        checkbox->canvas.h) / 2;

    text_canvas->x = checkbox->canvas.x + checkbox->canvas.w + 10;
    text_canvas->y = canvas->y + (canvas->h -
        text_canvas->h) / 2;
}


void maud_selectionmenu_renderer_init_btns_position(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_selectallbtn_t *select_allbtn = &selection_menu->select_allbtn;
    maud_checkbox_t* checkbox = &select_allbtn->checkbox;
    maud_selectionmenubtn_t *buttons[] = {
        &selection_menu->playbtn,
        &selection_menu->playnextbtn,
        &selection_menu->addtobtn,
        &selection_menu->removebtn,
        &selection_menu->moveup_btn,
        &selection_menu->movedown_btn
    };
    size_t button_count = sizeof(buttons) / sizeof(maud_selectionmenubtn_t*);
    SDL_Rect *canvas = &selection_menu->canvas;
    maud_selectionmenu_renderer_init_selectallbtn_position(maud, selection_menu);
    int start_x = select_allbtn->canvas.x + select_allbtn->canvas.w + 5;
    for(size_t i=0;i<button_count;i++) {
        SDL_Rect *button_canvas = &buttons[i]->canvas,
                 *icon_canvas = &buttons[i]->icon_canvas,
                 *text_canvas = &buttons[i]->text.text_canvas;
        button_canvas->x = start_x;
        button_canvas->y = canvas->y + (canvas->h -
            button_canvas->h) / 2;
        icon_canvas->x = button_canvas->x + 5;
        icon_canvas->y = button_canvas->y + (button_canvas->h -
            icon_canvas->h) / 2;
        text_canvas->x = icon_canvas->x + icon_canvas->w + 5;
        text_canvas->y = button_canvas->y + (button_canvas->h -
            text_canvas->h) / 2;
        start_x += button_canvas->w + 10;
    }
}

void maud_selectionmenu_renderer_init(maud_t* maud) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenu_renderer_init_menu(maud, selection_menu);
}

void maud_selectionmenu_renderer_renderselectallbtn(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_selectallbtn_t* select_allbtn = &selection_menu->select_allbtn;
    maud_checkbox_draw(maud, &select_allbtn->checkbox);
    SDL_Texture* selection_infotexture = maud_textmanager_rendertext(maud, maud->font,
        &select_allbtn->selection_info);
    SDL_RenderCopy(maud->renderer, selection_infotexture, NULL, &select_allbtn->selection_info.text_canvas);
    SDL_DestroyTexture(selection_infotexture);
    free(select_allbtn->selection_info.text);
}

void maud_selectionmenu_renderer_renderbtns(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_selectallbtn_t* select_allbtn = &selection_menu->select_allbtn;
    maud_selectionmenubtn_t *buttons[] = {
        &selection_menu->playbtn,
        &selection_menu->playnextbtn,
        &selection_menu->addtobtn,
        &selection_menu->removebtn,
        &selection_menu->moveup_btn,
        &selection_menu->movedown_btn
    };
    size_t button_count = sizeof(buttons) / sizeof(maud_selectionmenubtn_t*);
    maud_selectionmenu_renderer_renderselectallbtn(maud, selection_menu);
    for(size_t i=0;i<button_count;i++) {
        SDL_Rect *btn_canvas = &buttons[i]->canvas,
                 *text_canvas = &buttons[i]->text.text_canvas,
                 *icon_canvas = &buttons[i]->icon_canvas;
        SDL_SetRenderDrawColor(maud->renderer, color_toparam(buttons[i]->color));
        SDL_RenderDrawRect(maud->renderer, btn_canvas);
        SDL_RenderFillRect(maud->renderer, btn_canvas);

        bool destroy_icontexture = false;
        SDL_Texture* icon_texture = buttons[i]->img_texture;
        if(!icon_texture && buttons[i]->img_path) {
            icon_texture = IMG_LoadTexture(maud->renderer, buttons[i]->img_path);
        }

        SDL_RenderCopy(maud->renderer, icon_texture, NULL, icon_canvas);
        if(!buttons[i]->img_texture && icon_texture) {
            SDL_DestroyTexture(icon_texture);
        }
        SDL_Texture* text_texture = maud_textmanager_rendertext(maud, maud->font, &buttons[i]->text);
        SDL_RenderCopy(maud->renderer, text_texture, NULL, text_canvas);
        SDL_DestroyTexture(text_texture);
    }
}

void maud_selectionmenu_renderer_rendermenu(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(selection_menu->color));
    SDL_RenderDrawRect(maud->renderer, &selection_menu->canvas);
    SDL_RenderFillRect(maud->renderer, &selection_menu->canvas);

    maud_selectionmenu_renderer_renderbtns(maud, selection_menu);
}

void maud_selectionmenu_renderer_display(maud_t* maud) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenu_renderer_init(maud);
    maud_selectionmenu_renderer_rendermenu(maud, selection_menu);
}