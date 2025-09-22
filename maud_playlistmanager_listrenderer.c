#include "maud_playlistmanager.h"

void maud_playlistmanager_listrenderer_init_playlistprops(maud_t* maud,
    maud_playlistprops_t* playlist_props) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_buttonbar_t* button_bar = &playlist_manager->button_bar;
    if(playlist_props->init) {
        return;
    }
    int start_x = 0,
        start_y = button_bar->canvas.y + button_bar->canvas.h + 5;
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

void maud_playlistmanager_listrenderer_init_list(maud_t* maud,
    maud_playlist_t* playlists, size_t index, int start_x, int start_y) {
    char *playlist_name = playlists[index].name;
    maud_playlistprops_t* playlist_props = &maud->playlist_manager.playlist_props;
    maud_tooltip_t* tooltip = &playlist_props->tooltip;
    maud_playlistlist_t list = {
        .canvas = {
            .x = start_x, .y = start_y,
            .w = maud->win_width, .h = 10
        },
        .color = dark_purple,
        .icon_canvas = {
            .w = 30, .h = 30
        },
        .name = {
            .font_size = 20,
            .text_color = white,
            .utext = playlist_name
        }
    };
    SDL_Rect *canvas = &list.canvas,
             *icon_canvas = &list.icon_canvas,
             *text_canvas = &list.name.text_canvas;
    maud_textmanager_sizetext(maud->font, &list.name);
    canvas->h += maud_playlistmanager_findmaxheight(
        (SDL_Rect[]) {
            *icon_canvas,
            *text_canvas
        },
        2
    );
    icon_canvas->x = canvas->x + 5;
    icon_canvas->y = canvas->y + (canvas->h -
        icon_canvas->h) / 2;
    
    text_canvas->x = icon_canvas->x + icon_canvas->w + 5;
    text_canvas->y = canvas->y + (canvas->h -
        text_canvas->h) / 2;
    playlists[index].list = list;
}

void maud_playlistmanager_listrenderer_init_lists(maud_t* maud, maud_playlist_t* playlists,
    size_t playlist_count) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    SDL_Rect* scroll_area = &playlist_props->scroll_area;
    scroll_area->w = maud->win_width, scroll_area->h = music_status.y - playlist_props->start_y;
    int start_x = playlist_props->start_x,
        start_y = playlist_props->scroll_y;
    size_t start_renderpos = *playlist_props->start_renderpos;
    for(size_t i=start_renderpos;i<playlist_count;i++) {
        SDL_Rect* list_canvas = &playlists[i].list.canvas;
        maud_playlistmanager_listrenderer_init_list(maud, playlists, i,
            start_x, start_y);
        playlist_props->end_renderpos = i;
        if(list_canvas->y + list_canvas->h >= scroll_area->y +
            scroll_area->h) {
            break;
        }
        start_y += list_canvas->h + 5;
    }
    maud_playlistmanager_listrenderer_fillplaylistgap(maud, playlist_props);
}

void maud_playlistmanager_listrenderer_init(maud_t* maud) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    maud_playlist_t* playlists = playlist_manager->playlists;
    size_t playlist_count = playlist_manager->playlist_count;
    maud_playlistmanager_listrenderer_init_playlistprops(maud, playlist_props);
    maud_playlistmanager_listrenderer_init_lists(maud, playlists, playlist_count);
    maud_playlistmanager_listrenderer_handleplaylist_scrollevent(maud, playlist_props);
}

void maud_playlistmanager_listrenderer_renderplaylist(maud_t* maud, maud_playlist_t* playlists, size_t index) {
    maud_playlistlist_t* list = &playlists[index].list;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(list->color));
    SDL_RenderDrawRect(maud->renderer, &list->canvas);
    SDL_RenderFillRect(maud->renderer, &list->canvas);

    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE]
        [music_playlistbtn.texture_idx], NULL, &list->icon_canvas);

    SDL_Texture* playlist_nametexture = maud_textmanager_renderunicode(maud, maud->font, &list->name);
    SDL_RenderCopy(maud->renderer, playlist_nametexture, NULL, &list->name.text_canvas);
    SDL_DestroyTexture(playlist_nametexture);
    maud_playlistmanager_listrenderer_handleplaylist_event(maud, playlists, index);
}

void maud_playlistmanager_listrenderer_displayplaylists(maud_t* maud) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlist_t* playlists = playlist_manager->playlists;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    if(!playlists) {
        return;
    }
    maud_playlistmanager_listrenderer_init(maud);
    SDL_RenderSetClipRect(maud->renderer, &playlist_props->scroll_area);
    size_t start_renderpos = playlist_manager->playlist_renderpos,
           end_renderpos = playlist_props->end_renderpos + 1;
    for(size_t i=start_renderpos;i<end_renderpos;i++) {
        maud_playlistmanager_listrenderer_renderplaylist(maud, playlists, i);
    }
    SDL_RenderSetClipRect(maud->renderer, NULL);
}

void maud_playlistmanager_listrenderer_handleplaylist_event(maud_t* maud,
    maud_playlist_t* playlists, size_t playlist_index) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistlist_t* list = &playlists[playlist_index].list;
    maud_playlistprops_t* playlist_props = &playlist_manager->playlist_props;
    maud_buttonbar_t* button_bar = &maud->playlist_manager.button_bar;
    if(maud_rect_hover(maud, list->canvas) &&
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

void maud_playlistmanager_listrenderer_handleplaylist_scrollevent(maud_t* maud,
    maud_playlistprops_t* playlist_props) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlist_t* playlists = playlist_manager->playlists;
    size_t playlist_count = playlist_manager->playlist_count;
    SDL_Rect* scroll_area = &playlist_props->scroll_area;
    size_t start_renderpos = *playlist_props->start_renderpos,
           end_renderpos = playlist_props->end_renderpos;
    maud_playlistlist_t *first_list = &playlists[start_renderpos].list,
                        *last_list = &playlists[end_renderpos].list;
    if(!maud->scroll) {
        return;
    }
    if(!maud_rect_hover(maud, *scroll_area)) {
        return;
    }
    if(maud->scroll_type == MAUDSCROLL_DOWN
        && last_list->canvas.y + last_list->canvas.h > scroll_area->y + scroll_area->h) {
        playlist_props->scroll_y -= 10;
        if(playlist_props->scroll_y + first_list->canvas.h < scroll_area->y) {
            (*playlist_props->start_renderpos)++;
            playlist_props->scroll_y = scroll_area->y;
        }
    } else if(maud->scroll_type == MAUDSCROLL_UP) {
        if(start_renderpos == 0 && first_list->canvas.y < scroll_area->y) {
            playlist_props->scroll_y += 10;
            if(playlist_props->scroll_y >= scroll_area->y) {
                playlist_props->scroll_y = scroll_area->y;
            }
        } else if(start_renderpos) {
            if(first_list->canvas.y <= scroll_area->y) {
                playlist_props->scroll_y += 10;
                if(playlist_props->scroll_y >= scroll_area->y) {
                    (*playlist_props->start_renderpos)--;
                    maud_playlistlist_t* prev_list = &playlists[(*playlist_props->start_renderpos)].list;
                    playlist_props->scroll_y = scroll_area->y - (prev_list->canvas.h - 10);
                }
            }
        }
    }
    maud->scroll = false;
}

void maud_playlistmanager_listrenderer_fillplaylistgap(maud_t* maud,
    maud_playlistprops_t* playlist_props) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlist_t* playlists = playlist_manager->playlists;
    SDL_Rect* scroll_area = &playlist_props->scroll_area;
    size_t start_renderpos = *playlist_props->start_renderpos,
           end_renderpos = playlist_props->end_renderpos,
           playlist_count = playlist_manager->playlist_count;
    maud_playlistlist_t *last_list = &playlists[end_renderpos].list;
    int last_y = last_list->canvas.y + last_list->canvas.h + 5,
        end_rendery = scroll_area->y + scroll_area->h - 5;
    if(end_renderpos == playlist_count-1 && last_y < end_rendery) {
        (*playlist_props->start_renderpos)--;
        playlist_props->scroll_y = scroll_area->y;
    }
}