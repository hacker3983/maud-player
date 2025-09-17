#include "maud_playlistmanager.h"

void maud_playlistmanager_gridrenderer_init_playlistprops(maud_t* maud, maud_playlistprops_t* playlist_props) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_buttonbar_t* button_bar = &playlist_manager->button_bar;
    if(playlist_props->init) {
        return;
    }
    int start_x = 5,
        start_y = button_bar->canvas.y + button_bar->canvas.h + 5;
    maud_playlistprops_t new_props = {
        .start_x = start_x, .start_y = start_y,
        .scroll_y = start_y, .scroll_area = {
            .x = start_x, .y = start_y,
            .w = maud->win_width, .h = music_status.y - start_y
        },
        .start_renderpos = &playlist_manager->playlist_renderpos,
        .tooltip = {
            .background_color = {0xFF, 0x3C, 0x38, 0xFF},
            .delay_secs = .5,
            .duration_secs = 0,
            .font = maud->music_font,
            .font_size = 16,
            .margin_x = 10,
            .margin_y = 10,
            .text_color = white,
            .wrap_spacing = 5,
            .contents = NULL,
            .content_count = 0
        }
    };
    new_props.init = true;
    *playlist_props = new_props;
}

void maud_playlistmanager_gridrenderer_init_grid(maud_t* maud,
    maud_playlist_t* playlists, size_t index, int start_x, int start_y) {
    char *playlist_name = playlists[index].name;
    maud_playlistprops_t* playlist_props = &maud->playlist_manager.playlist_props;
    maud_tooltip_t* tooltip = &playlist_props->tooltip;
    maud_playlistgrid_t grid = {
        .canvas = {
            .x = start_x, .y = start_y,
            .w = 40, .h = 40
        },
        .color = {0x00, 0xff, 0x00, 0xff},
        .card = {
            .canvas = {
                .w = 130, .h = 120
            },
            .color = {0x12, 0x12, 0x12, 0xff},
            .icon_canvas = {
                .w = 30, .h = 50
            }
        },
        .name = {
            .font_size = 16,
            .text = playlist_name,
            .text_color = white
        }
    };
    SDL_Rect *canvas = &grid.canvas,
             *card_canvas = &grid.card.canvas,
             *card_iconcanvas = &grid.card.icon_canvas,
             *playlist_namecanvas = &grid.name.text_canvas;
    canvas->w += card_canvas->w;
    char* truncated_text = maud_textmanager_truncatetext(maud->font,
        &grid.name, canvas->w);
    if(truncated_text) {
        grid.name.text = truncated_text;
        grid.truncated = true;
    }
    maud_textmanager_sizetext(maud->font, &grid.name);
    canvas->h += card_canvas->h + playlist_namecanvas->h;

    card_canvas->x = canvas->x + (canvas->w -
        card_canvas->w) / 2;
    card_canvas->y = canvas->y + (canvas->h -
        card_canvas->h) / 2;

    card_iconcanvas->x = card_canvas->x + (card_canvas->w -
        card_iconcanvas->w) / 2;
    card_iconcanvas->y = card_canvas->y + (card_canvas->h -
        card_iconcanvas->h) / 2;

    playlist_namecanvas->x = canvas->x + (canvas->w -
        playlist_namecanvas->w) / 2;
    playlist_namecanvas->y = card_canvas->y + card_canvas->h + 5;
    if(truncated_text && maud_rect_hover(maud, *playlist_namecanvas)) {
        tooltip->x = canvas->x, tooltip->y = playlist_namecanvas->y;
        tooltip->wrap_length = playlist_namecanvas->w;
        tooltip->text = playlist_name;
        tooltip->element_canvas = *playlist_namecanvas;
        maud_tooltip_getsize(tooltip);
        tooltip->y -= tooltip->h - 2;
        playlist_props->show_tooltip = true;
    }
    playlists[index].grid = grid;
}

void maud_playlistmanager_gridrenderer_init_grids(maud_t* maud, maud_playlist_t* playlists,
    size_t playlist_count) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    SDL_Rect* scroll_area = &playlist_props->scroll_area;
    scroll_area->w = maud->win_width, scroll_area->h = music_status.y - playlist_props->start_y;
    int start_x = playlist_props->start_x,
        start_y = playlist_props->scroll_y;
    size_t start_renderpos = *playlist_props->start_renderpos;
    for(size_t i=start_renderpos;i<playlist_count;i++) {
        SDL_Rect* grid_canvas = &playlists[i].grid.canvas;
        maud_playlistmanager_gridrenderer_init_grid(maud, playlists, i,
            start_x, start_y);
        playlist_props->end_renderpos = i;
        start_x += grid_canvas->w + 5;
        if(start_x + grid_canvas->w >= maud->win_width && grid_canvas->y +
            grid_canvas->h >= playlist_props->scroll_area.y +
            playlist_props->scroll_area.h) {
            break;
        }
        if(start_x + grid_canvas->w >= maud->win_width) {
            start_x = playlist_props->start_x;
            start_y += grid_canvas->h + 5;
        }
    }
}

void maud_playlistmanager_gridrenderer_init(maud_t* maud) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    maud_playlist_t* playlists = playlist_manager->playlists;
    size_t playlist_count = playlist_manager->playlist_count;
    maud_playlistmanager_gridrenderer_init_playlistprops(maud, playlist_props);
    maud_playlistmanager_gridrenderer_init_grids(maud, playlists, playlist_count);
    maud_playlistmanager_gridrenderer_handleplaylist_scrollevent(maud, playlist_props);
}

void maud_playlistmanager_gridrenderer_renderplaylist(maud_t* maud, maud_playlist_t* playlists, size_t index) {
    maud_playlistgrid_t* grid = &playlists[index].grid;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(grid->color));
    SDL_RenderDrawRect(maud->renderer, &grid->canvas);

    SDL_SetRenderDrawColor(maud->renderer, color_toparam(grid->card.color));
    SDL_RenderDrawRect(maud->renderer, &grid->card.canvas);
    SDL_RenderFillRect(maud->renderer, &grid->card.canvas);

    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE]
        [music_playlistbtn.texture_idx], NULL, &grid->card.icon_canvas);

    SDL_Texture* playlist_texture = maud_textmanager_rendertext(maud, maud->font,
        &grid->name);
    SDL_RenderCopy(maud->renderer, playlist_texture, NULL,
        &grid->name.text_canvas);
    SDL_DestroyTexture(playlist_texture);
    if(grid->truncated) {
        free(grid->name.text);
    }
    maud_playlistmanager_gridrenderer_handleplaylist_event(maud, playlists, index);
}

void maud_playlistmanager_gridrenderer_displayplaylists(maud_t* maud) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlist_t* playlists = playlist_manager->playlists;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    maud_playlistmanager_gridrenderer_init(maud);
    SDL_RenderSetClipRect(maud->renderer, &playlist_props->scroll_area);
    size_t start_renderpos = playlist_manager->playlist_renderpos,
           end_renderpos = playlist_props->end_renderpos + 1;
    for(size_t i=start_renderpos;i<end_renderpos;i++) {
        maud_playlistmanager_gridrenderer_renderplaylist(maud, playlists, i);
    }
    SDL_RenderSetClipRect(maud->renderer, NULL);
}

void maud_playlistmanager_gridrenderer_handleplaylist_scrollevent(maud_t* maud,
    maud_playlistprops_t* playlist_props) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlist_t* playlists = playlist_manager->playlists;
    size_t playlist_count = playlist_manager->playlist_count;
    SDL_Rect* scroll_area = &playlist_props->scroll_area;
    size_t start_renderpos = *playlist_props->start_renderpos,
           *prev_renderpos = &playlist_props->prev_renderpos,
           *next_renderpos = &playlist_props->next_renderpos,
           end_renderpos = playlist_props->end_renderpos;
    maud_playlistgrid_t *first_grid = &playlists[start_renderpos].grid,
                        *last_grid = &playlists[end_renderpos].grid;
    if(!maud->scroll) {
        return;
    }
    if(!maud_rect_hover(maud, *scroll_area)) {
        return;
    }
    if(maud->scroll_type == MAUDSCROLL_DOWN &&
        last_grid->canvas.y + last_grid->canvas.h > scroll_area->y + scroll_area->h) {
        playlist_props->scroll_y -= 10;
        if(playlist_props->scroll_y + first_grid->canvas.h < scroll_area->y) {
            maud_playlistmanager_gridrenderer_getnextrender_pos(maud, playlist_props);
            (*playlist_props->start_renderpos) = *next_renderpos;
            playlist_props->scroll_y = scroll_area->y;
        }
    } else if(maud->scroll_type == MAUDSCROLL_UP) {
        if(start_renderpos == 0 && first_grid->canvas.y < scroll_area->y) {
                playlist_props->scroll_y += 10;
                if(playlist_props->scroll_y > scroll_area->y) {
                    playlist_props->scroll_y = scroll_area->y;
                }
        } else if(start_renderpos) {
            if(first_grid->canvas.y < scroll_area->y) {
                playlist_props->scroll_y += 10;
                if(playlist_props->scroll_y >= scroll_area->y) {
                    maud_playlistmanager_gridrenderer_getprevrender_pos(maud, playlist_props);
                    (*playlist_props->start_renderpos) = *prev_renderpos;
                    maud_playlistgrid_t* prev_grid = &playlists[(*prev_renderpos)].grid;
                    playlist_props->scroll_y = scroll_area->y - (prev_grid->canvas.h - 10);
                }
            }
        }
    }
    maud->scroll = false;
}

void maud_playlistmanager_gridrenderer_handleplaylist_event(maud_t* maud,
    maud_playlist_t* playlists, size_t playlist_index) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistgrid_t* grid = &playlists[playlist_index].grid;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    maud_buttonbar_t* button_bar = &maud->playlist_manager.button_bar;
    if(maud_rect_hover(maud, grid->canvas) &&
        !button_bar->layout_switch.clicked &&
        !button_bar->new_playlistbtn.clicked) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            playlist_props->selection_index = playlist_index;
            playlist_props->selected = true;
            maud->mouse_clicked = false;
        }
    }
}

void maud_playlistmanager_gridrenderer_getprevrender_pos(maud_t* maud,
    maud_playlistprops_t* playlist_props) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlist_t* playlists = playlist_manager->playlists;
    size_t start_renderpos = *playlist_props->start_renderpos;
    for(size_t i=start_renderpos;i>0;) {
        if(i) {
            i--;
        }
        if(playlists[i].grid.canvas.x == playlist_props->start_x) {
            playlist_props->prev_renderpos = i;
            break;
        }
        if(!i) {
            break;
        }
    }
}

void maud_playlistmanager_gridrenderer_getnextrender_pos(maud_t* maud,
    maud_playlistprops_t* playlist_props) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlist_t* playlists = playlist_manager->playlists;
    size_t start_renderpos = *playlist_props->start_renderpos,
           end_renderpos = playlist_props->end_renderpos;
    int start_x = playlist_props->start_x;
    for(size_t i=start_renderpos;i<end_renderpos;i++) {
        start_x += playlists[i].grid.canvas.w + 5;
        if(start_x + playlists[i].grid.canvas.w >=
            maud->win_width) {
            printf("next_renderpos is %zu\n", i + 1);
            playlist_props->next_renderpos = i + 1;
            break;
        }
    }
}
