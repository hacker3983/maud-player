#include "maud_playlistmanager.h"

void maud_playlistmanager_menurenderer_init_playlistprops(maud_t* maud,
    maud_playlistprops_t* playlist_props) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistmenu_t* playlist_menu = &playlist_manager->playlist_menu;
    maud_buttonbar_t* button_bar = &playlist_manager->button_bar;
    size_t playlist_selectionindex = playlist_props->selection_index;
    maud_playlist_t *playlists = playlist_manager->playlists,
                    *playlist = &playlists[playlist_selectionindex];
    if(playlist_props->init) {
        return;
    }
    int start_x = 0,
        start_y = playlist_menu->canvas.y + playlist_menu->canvas.h;
    maud_playlistprops_t new_props = {
        .start_x = start_x, .start_y = start_y,
        .scroll_y = start_y, .scroll_area = {
            .x = start_x, .y = start_y,
            .w = maud->win_width, .h = music_status.y - start_y
        },
        .start_renderpos = &playlist_manager->playlist_renderpos
    };
    *new_props.start_renderpos = 0;
    new_props.init = true;
    *playlist_props = new_props;
}

void maud_playlistmanager_menurenderer_init_textsizes(maud_t* maud,
    maud_playlistmenu_t* playlist_menu) {
    text_info_t *textinfo_list[] = {
        &playlist_menu->item_count,
        &playlist_menu->playlist_name,
        &playlist_menu->addtobtn.text,
        &playlist_menu->renamebtn.text,
        &playlist_menu->deletebtn.text,
    };
    size_t textinfo_count = sizeof(textinfo_list) / sizeof(text_info_t*);
    for(size_t i=0;i<textinfo_count;i++) {
        maud_textmanager_sizetext(maud->font, textinfo_list[i]);
    }
}

void maud_playlistmanager_menurenderer_init_btns(maud_t* maud,
    maud_playlistmenu_t* playlist_menu, int start_x, int start_y) {
    maud_playlistmenubtn_t* buttons[] = {
        &playlist_menu->playallbtn,
        &playlist_menu->addtobtn,
        &playlist_menu->renamebtn,
        &playlist_menu->deletebtn
    };
    size_t button_count = sizeof(buttons) / sizeof(maud_playlistmenubtn_t);
    for(size_t i=0;i<button_count;i++) {
        SDL_Rect *canvas = &buttons[i]->canvas,
                 *icon_canvas = &buttons[i]->icon_canvas,
                 *text_canvas = &buttons[i]->text.text_canvas;
        canvas->x = start_x;
        canvas->y = start_y;
        canvas->w += icon_canvas->w + text_canvas->w;
        canvas->h += maud_playlistmanager_findmaxheight(
            (SDL_Rect[]){
                *icon_canvas,
                *text_canvas
            },
            2
        );
        
        icon_canvas->x = canvas->x + 5,
        icon_canvas->y = canvas->y + (canvas->h -
            icon_canvas->h) / 2;
        
        text_canvas->x = icon_canvas->x + icon_canvas->w + 9;
        text_canvas->y = canvas->y + (canvas->h -
            text_canvas->h) / 2;
        start_x += canvas->w + 10;
    }
}

void maud_playlistmanager_menurenderer_init_menu(maud_t* maud,
    maud_playlistmenu_t* playlist_menu) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_buttonbar_t* button_bar = &playlist_manager->button_bar;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    size_t playlist_selectionindex = playlist_props->selection_index;
    maud_playlist_t *playlists = playlist_manager->playlists,
                    *playlist = &playlists[playlist_selectionindex];
    size_t itemcount_textlen = maud_getsize_t_length(playlist->queue.item_count) + 7;
    char* itemcount_text = calloc(itemcount_textlen, sizeof(char));
    sprintf(itemcount_text, "%zu item", playlist->queue.item_count);
    if(playlist->queue.item_count > 1) {
        strcat(itemcount_text, "s");
    }
    maud_playlistmenu_t new_menu = {
        .canvas = {
            .y = button_bar->canvas.y,
            .w = maud->win_width - 30
        },
        .color = {0x12, 0x12, 0x12, 0xff},
        .playlist_card = {
            .canvas = {
                .w = 180, .h = 180
            },
            .color = dark_purple,
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .collapse_x = playlist_menu->playlist_card.collapse_x,
            .collapse_y = playlist_menu->playlist_card.collapse_y
        },
        .playlist_name = {
            .font_size = 20,
            .text_color = white,
            .utext = playlist->name
        },
        .item_count = {
            .font_size = 20,
            .text_color = white,
            .text = itemcount_text
        },
        .playallbtn = {
            .canvas = {
                .w = 20, .h = 20
            },
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .color = {0xF4, 0x60, 0x36, 0xFF},
            .text = {
                .font_size = 18,
                .text = "Play all",
                .text_color = white
            },
            .icon_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE]
                [music_listplaybtn.texture_idx]
        },
        .addtobtn = {
            .canvas = {
                .w = 20, .h = 20
            },
            .color = {0x00, 0xff, 0x00, 0xff},
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .text = {
                .font_size = 18,
                .text = "Add to",
                .text_color = white
            },
            .icon_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE]
                            [music_addtobtn.texture_idx]
        },
        .renamebtn = {
            .canvas = {
                .w = 20, .h = 20
            },
            .color = {0x00, 0xff, 0x00, 0xff},
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .text = {
                .font_size = 18,
                .text = "Rename",
                .text_color = white
            },
            .icon_path = "images/rename.png"
        },
        .deletebtn = {
            .canvas = {
                .w = 20, .h = 20
            },
            .color = {0x00, 0xff, 0x00, 0xff},
            .icon_canvas = {
                .w = 30, .h = 30
            },
            .text = {
                .font_size = 18,
                .text = "Delete",
                .text_color = white
            },
            .icon_path = "images/delete.png"
        },
        .collapse = playlist_menu->collapse
    };
    maud_playlistmanager_menurenderer_init_textsizes(maud, &new_menu);
    SDL_Rect *canvas = &new_menu.canvas,
             *card_canvas = &new_menu.playlist_card.canvas,
             *card_iconcanvas = &new_menu.playlist_card.icon_canvas,
             *playlist_namecanvas = &new_menu.playlist_name.text_canvas,
             *itemcount_textcanvas = &new_menu.item_count.text_canvas;
    canvas->h = card_canvas->h + 20;
    canvas->x = (maud->win_width - canvas->w) / 2;
    card_canvas->x = canvas->x + 10;
    card_canvas->y = canvas->y + (canvas->h -
        card_canvas->h) / 2;
    
    card_iconcanvas->x = card_canvas->x + (card_canvas->w
        - card_iconcanvas->w) / 2;
    card_iconcanvas->y = card_canvas->y + (card_canvas->h
        - card_iconcanvas->h) / 2;

    playlist_namecanvas->x = card_canvas->x + card_canvas->w + 15;
    playlist_namecanvas->y = card_canvas->y + 5;
    SDL_Rect prev_canvas = *playlist_namecanvas;
    if(!new_menu.collapse) {
        itemcount_textcanvas->x = playlist_namecanvas->x,
        itemcount_textcanvas->y = playlist_namecanvas->y + playlist_namecanvas->h + 10;
        prev_canvas = *itemcount_textcanvas;
    }
    maud_playlistmanager_menurenderer_init_btns(maud, &new_menu, prev_canvas.x,
        prev_canvas.y + prev_canvas.h + 5);
    *playlist_menu = new_menu;
}

void maud_playlistmanager_menurenderer_init(maud_t* maud) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    maud_playlistmenu_t* playlist_menu = &playlist_manager->playlist_menu;
    maud_playlistmanager_menurenderer_init_menu(maud, playlist_menu);
    maud_playlistmanager_menurenderer_init_playlistprops(maud, playlist_props);
}

void maud_playlistmanager_menurenderer_renderbtns(maud_t* maud, maud_playlistmenu_t* playlist_menu) {
    maud_playlistmenubtn_t* buttons[] = {
        &playlist_menu->playallbtn,
        &playlist_menu->addtobtn,
        &playlist_menu->renamebtn,
        &playlist_menu->deletebtn
    };
    size_t button_count = sizeof(buttons) / sizeof(maud_playlistmenubtn_t);
    for(size_t i=0;i<button_count;i++) {
        SDL_SetRenderDrawColor(maud->renderer, color_toparam(buttons[i]->color));
        SDL_RenderDrawRect(maud->renderer, &buttons[i]->canvas);
        if(i == 0) {
            SDL_RenderFillRect(maud->renderer, &buttons[i]->canvas);
        }
        if(buttons[i]->icon_path) {
            buttons[i]->icon_texture = IMG_LoadTexture(maud->renderer, buttons[i]->icon_path);
        }
        SDL_RenderCopy(maud->renderer, buttons[i]->icon_texture, NULL, &buttons[i]->icon_canvas);
        if(buttons[i]->icon_path) {
            SDL_DestroyTexture(buttons[i]->icon_texture);
        }

        SDL_Texture *text_texture = maud_textmanager_rendertext(maud, maud->font, &buttons[i]->text);
        SDL_RenderCopy(maud->renderer, text_texture, NULL, &buttons[i]->text.text_canvas);
        SDL_DestroyTexture(text_texture);
    }
}

void maud_playlistmanager_menurenderer_renderplaylistmenu(maud_t* maud,
    maud_playlistmenu_t* playlist_menu) {
    SDL_Rect *canvas = &playlist_menu->canvas;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(playlist_menu->color));
    SDL_RenderDrawRect(maud->renderer, canvas);
    SDL_RenderFillRect(maud->renderer, canvas);

    SDL_Rect *card = &playlist_menu->playlist_card;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(playlist_menu->color));
    SDL_RenderDrawRect(maud->renderer, card);
    SDL_RenderFillRect(maud->renderer, card);
    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE]
        [music_playlistbtn.texture_idx], NULL, card);

    SDL_Texture* playlist_nametexture = maud_textmanager_renderunicode(maud, maud->font,
        &playlist_menu->playlist_name);
    SDL_RenderCopy(maud->renderer, playlist_nametexture, NULL,
        &playlist_menu->playlist_name.text_canvas);
    SDL_DestroyTexture(playlist_nametexture);
    maud_playlistmanager_menurenderer_renderbtns(maud, playlist_menu);
}

void maud_playlistmanager_menurenderer_display(maud_t* maud) {
    maud_playlistmanager_menurenderer_init(maud);
}