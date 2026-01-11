#include "maud_queue.h"
#include "maud_checkbox.h"
#include "maud_songsmanager.h"

void maud_queue_renderer_init_queueprops_scrolly_playlistmenu(maud_t* maud, maud_queueprops_t* queue_props) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistmenu_t* playlist_menu = &playlist_manager->playlist_menu;
    int scrollstart_y = playlist_menu->canvas.y + playlist_menu->canvas.h;
    if(active_tab == PLAYLISTS_TAB) {
        queue_props->scrollstart_y = scrollstart_y;
        queue_props->scroll_y = scrollstart_y;
        queue_props->check_scrollarea_hover = false;
    }
}

void maud_queue_renderer_init_queueprops(maud_t* maud, maud_queue_t* queue) {
    maud_playlistmanager_t* playlist_manager = &maud->playlist_manager;
    maud_playlistmenu_t* playlist_menu = &playlist_manager->playlist_menu;
    maud_queueprops_t* queue_props = &queue->queue_props;
    if(queue_props->init) {
        return;
    }
    int scrollarea_startx = songs_box.x + 2,
        scrollarea_starty = songs_box.y + 1,
        scrollarea_width = maud->win_width,
        scrollstart_y = songs_box.y + 1;
    maud_queueprops_t new_props = {
        .scrollarea_startx = scrollarea_startx,
        .scrollarea_starty = scrollarea_starty,
        .scrollstart_y = scrollstart_y,
        .scroll_y = scrollstart_y,
        .clip_area = {
            .x = scrollarea_startx, .y = scrollarea_starty,
            .w = scrollarea_width, .h = songs_box.h-1
        },
        .scroll_area = {
            .x = scrollarea_startx, .y = scrollarea_starty,
            .w = scrollarea_width, .h = songs_box.h-1
        },
        .start_renderpos = &queue->start_renderpos,
        .check_scrollarea_hover = true
    };
    maud_queue_renderer_init_queueprops_scrolly_playlistmenu(maud, &new_props);
    new_props.init = true;
    queue->queue_props = new_props;
}

void maud_queue_renderer_init_item(maud_t* maud, maud_queue_t* queue,
    size_t item_index, int start_x, int start_y) {
    maud_queueitem_t* item = &queue->items[item_index];
    size_t music_listindex = item->music_listindex,
           music_id = item->music_id;
    maud_queueitem_t new_item = {
        .canvas = {
            .x = start_x, .y = start_y,
            .w = maud->win_width - scrollbar.w - 5,
            .h = 20
        },
        .color = {0, 42, 50, 0xFF},
        .music_id = item->music_id,
        .music_listindex = item->music_listindex,
        .checkbox_ticked = item->checkbox_ticked,
        .fill = item->fill,
        .checkbox_color = white,
        .checkbox_fillcolor = {0},
        .checkbox_tickcolor = {0x00, 0xff, 0x00, 0xff},
        .music_name = {
            .font_size = 18,
            .utext = maud->music_lists[music_listindex]
                [music_id].music_name,
            .text_canvas = {
                .x = 50
            },
            .text_color = white
        }
    };
    text_info_t *music_name = &new_item.music_name;
    SDL_Rect *canvas = &new_item.canvas,
             *text_canvas = &music_name->text_canvas;
    maud_textmanager_sizetext(maud->font, music_name);
    canvas->h += text_canvas->h;
    text_canvas->x += canvas->x;
    text_canvas->y = canvas->y + (canvas->h -
        text_canvas->h) / 2;
    *item = new_item;
}

void maud_queue_renderer_handleremovebtn(maud_t* maud, maud_queue_t* queue) {
    if(!maud->remove_btnclicked) {
        return;
    }
    bool playid_ticked = false;
    // find start checkbox tick position
    size_t start_checkindex = 0;
    for(size_t i=0;i<queue->item_count;i++) {
        if(queue->items[i].checkbox_ticked) {
            start_checkindex = i;
        }
    }
    if(Mix_PlayingMusic() && queue->items[queue->playid].checkbox_ticked) {
        playid_ticked = true;
    } else if(Mix_PlayingMusic() && !playid_ticked) {
        // shift the playid to the start checkbox tick position
        for(size_t i=queue->playid;i>start_checkindex;i--) {
            maud_queueitem_t current_item = queue->items[i],
                             previous_item = queue->items[i-1];
            queue->items[i] = previous_item,
            queue->items[i-1] = current_item;
        }
        queue->playid = start_checkindex;
    }
    // shift the ticked items to the end of the list
    for(size_t i=0;i<maud->tick_count;i++) {
        for(size_t j=0;j<queue->item_count-1;j++) {
            maud_queueitem_t current_item = queue->items[j],
                             next_item = queue->items[j+1];
            if(current_item.checkbox_ticked) {
                queue->items[j] = queue->items[j+1];
                queue->items[j+1] = current_item;
            }
        }
    }
    // reallocate memory to remove the ticked items
    size_t new_count = queue->item_count - maud->tick_count;
    if(!new_count) {
        maud_queue_destroy(queue);
    } else {
        queue->items = realloc(queue->items, new_count * sizeof(maud_queueitem_t));
        queue->item_count = new_count;
    }
    // stop the music that is currently playing if it was ticked
    if(playid_ticked && Mix_PlayingMusic()) {
        Mix_PauseMusic();
        Mix_HaltMusic();
        if(queue->playid < queue->item_count) {
            queue->playid %= queue->item_count;
            size_t music_listindex = queue->items[queue->playid].music_listindex,
            music_id = queue->items[queue->playid].music_id;
            Mix_PlayMusic(maud->music_lists[music_listindex][music_id].music, 1);
        }
    }
    maud_selectionmenu_clearmusic_selection(maud, &maud->selection_menu);
    maud->remove_btnclicked = false;
}

void maud_queue_renderer_init_items(maud_t* maud, maud_queue_t* queue) {
    maud_queueprops_t* queue_props = &queue->queue_props;
    SDL_Rect* scroll_area = &queue_props->scroll_area;
    int start_x = 0, start_y = queue_props->scroll_y;
    size_t start_renderpos = *queue_props->start_renderpos;
    maud_queue_renderer_handleremovebtn(maud, queue);
    for(size_t i=start_renderpos;i<queue->item_count;i++) {
        SDL_Rect *item_canvas = &queue->items[i].canvas;
        maud_queue_renderer_init_item(maud, queue, i,
            start_x, start_y);
        queue_props->end_renderpos = i;
        if(item_canvas->y + item_canvas->h >= scroll_area->y + scroll_area->h) {
            break;
        }
        start_y += item_canvas->h + 3;
    }
}

void maud_queue_renderer_init(maud_t* maud, maud_queue_t* queue) {
    maud_queue_renderer_init_queueprops(maud, queue);
    maud_queue_renderer_init_items(maud, queue);
    maud_queue_renderer_handlequeue_scrollevent(maud, queue);
}

void maud_queue_renderer_renderitem_checkbox(maud_t* maud, maud_queue_t* queue, size_t item_index) {
    maud_queueitem_t *item = &queue->items[item_index];
    
    bool render_checkbox = false, fill_status = false, tick_status = false;
    if(maud->tick_count) {
        // whenever any checkbox is ticked and the current music we are current music we are rendering is not ticked
        // we ensure that we set the checkbox fill state to false before drawing its checkbox
        if(!item->checkbox_ticked && !maud_rect_hover(maud, item->canvas)) {
            item->fill = false;
        }
        fill_status = item->fill, tick_status = item->checkbox_ticked;
        render_checkbox = true;
    } else if(maud_rect_hover(maud, item->canvas)) {
        // whenever no checkbox is ticked and we hover over the music we display the check box
        // so that the user can click it or select it
        if(maud_checkbox_hovered(maud)) {
            fill_status = item->fill, tick_status = item->checkbox_ticked;
        } else if(!maud_checkbox_hovered(maud)) {
            fill_status = false, tick_status = false;
        }
        render_checkbox = true;
    }
    if(render_checkbox) {
        maud_checkbox_drawmusic_checkbox(maud, item->checkbox_color,
            item->checkbox_fillcolor, fill_status, item->checkbox_tickcolor, tick_status);
    }
}

void maud_queue_renderer_handleitem_event(maud_t* maud, maud_queue_t* queue, size_t item_index) {
    maud_queueitem_t *item = &queue->items[item_index];
    if(maud_rect_hover(maud, item->canvas)) {
        maud_songsmanager_handlesong_playbutton_hover(maud, item->canvas);
        maud_queue_handleitem_selection(maud, queue, item_index, item->canvas, &item->music_name);
        if(maud_rect_hover(maud, music_listplaybtn.btn_canvas)) {
            maud_setcursor(maud, MAUD_CURSOR_POINTER);
        } else if(maud_checkbox_hovered(maud)) {
            maud_setcursor(maud, MAUD_CURSOR_POINTER);
        }
    }
}

void maud_queue_renderer_init_itemcheckbox(maud_t* maud, maud_queue_t* queue, size_t item_index) {
    SDL_Rect *canvas = &queue->items[item_index].canvas;
    maud_checkbox_t* checkbox = &maud->music_checkbox;
    checkbox->canvas.x = canvas->x + 5;
    checkbox->canvas.w = checkbox_size.w;
    checkbox->canvas.h = canvas->h - 10;
    checkbox->canvas.y = canvas->y + (canvas->h -
        checkbox->canvas.h) / 2;
}

void maud_queue_renderer_init_itemplaybtn(maud_t* maud, maud_queue_t* queue, size_t item_index) {
    maud_queueitem_t* item = &queue->items[item_index];
    SDL_Rect* canvas = &item->canvas;
    maud_checkbox_t* checkbox = &maud->music_checkbox;
    if(!maud->tick_count) {
        music_listplaybtn.btn_canvas.w = 30, music_listplaybtn.btn_canvas.h = canvas->h - 10;
        music_listplaybtn.btn_canvas.x = (checkbox->canvas.x + checkbox->canvas.w) + 20,
        music_listplaybtn.btn_canvas.y = checkbox->canvas.y;
    }
}

void maud_queue_renderer_renderitem(maud_t* maud, maud_queue_t* queue, size_t item_index) {
    maud_queueitem_t *item = &queue->items[item_index];
    SDL_Rect* canvas = &item->canvas;
    //maud_selectionmenu_toggleitem_checkboxmusic_queue(maud, queue, item_index);
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(item->color));
    SDL_RenderDrawRect(maud->renderer, canvas);
    SDL_RenderFillRect(maud->renderer, canvas);

    maud_queue_renderer_init_itemcheckbox(maud, queue, item_index);
    maud_queue_renderer_init_itemplaybtn(maud, queue, item_index);
    maud_queue_renderer_handleitem_event(maud, queue, item_index);
    maud_queue_renderer_renderitem_checkbox(maud, queue, item_index);

    SDL_Texture* music_nametexture = maud_textmanager_renderunicode(maud, maud->music_font, &item->music_name);
    SDL_RenderCopy(maud->renderer, music_nametexture, NULL, &item->music_name.text_canvas);
    SDL_DestroyTexture(music_nametexture);
}

void maud_queue_renderer_updateareas(maud_t* maud, maud_queue_t* queue) {
    maud_queueprops_t* queue_props = &queue->queue_props;
    SDL_Rect *scroll_area = &queue_props->scroll_area,
             *clip_area = &queue_props->clip_area;
    if(maud->tick_count) {
        clip_area->y = songs_box.y + 1,
        clip_area->h = songs_box.h - 1,
        scroll_area->y = songs_box.y + 1,
        scroll_area->h = songs_box.h - 1;
    } else {
        clip_area->y = songs_box.y + 1,
        clip_area->h = songs_box.h - 1,
        scroll_area->y = songs_box.y + 1,
        scroll_area->h = songs_box.h - 1;
    }
}

void maud_queue_renderer_display(maud_t* maud, maud_queue_t* queue) {
    maud_queueprops_t* queue_props = &queue->queue_props;
    SDL_Rect *scroll_area = &queue_props->scroll_area,
             *clip_area = &queue_props->clip_area;
    if(!queue->items) {
        return;
    }
    maud_queue_renderer_init(maud, queue);
    maud_queue_renderer_updateareas(maud, queue);
    SDL_RenderSetClipRect(maud->renderer, clip_area);
    size_t start_renderpos = *queue_props->start_renderpos,
           end_renderpos = queue_props->end_renderpos+1;
    for(size_t i=start_renderpos;i<end_renderpos;i++) {
        maud_queue_renderer_renderitem(maud, queue, i);
    }
    SDL_RenderSetClipRect(maud->renderer, NULL);
    // scroll bar related information
    maud_scrollbar_t queue_scrollbar = {
        .rect = scrollbar,
        .displacement = 0.0,
        .orientation = 0,
        .start_pos = start_renderpos + 1,
        .final_pos = queue->item_count,
        .padding_x = -2,
        .padding_y = -(2 + (int)((double)scrollbar.h / 2.0)),
        .scroll_area = *scroll_area
    };
    maud_renderscroll_bar(maud, &queue_scrollbar, 8);
}

void maud_queue_renderer_handlequeue_scrollevent(maud_t* maud, maud_queue_t* queue) {
    maud_queueprops_t* queue_props = &queue->queue_props;
    SDL_Rect* scroll_area = &queue_props->scroll_area;
    bool check_scrollarea_hover = queue_props->check_scrollarea_hover;
    size_t start_renderpos = *queue_props->start_renderpos,
           end_renderpos = queue_props->end_renderpos;
    maud_queueitem_t *first_item = &queue->items[start_renderpos],
                     *last_item = &queue->items[end_renderpos];
    int scrollstart_y = queue_props->scrollstart_y;
    if(!maud->scroll) {
        return;
    }
    if(check_scrollarea_hover && !maud_rect_hover(maud, *scroll_area)) {
        return;
    }
    printf("handling queue scroll event\n");
    if(maud->scroll_type == MAUDSCROLL_DOWN
        && last_item->canvas.y + last_item->canvas.h > scroll_area->y + scroll_area->h) {
        queue_props->scroll_y -= 10;
        if(queue_props->scroll_y + first_item->canvas.h < scrollstart_y) {
            printf("here\n");
            (*queue_props->start_renderpos)++;
            queue_props->scroll_y = scrollstart_y;
        }
    } else if(maud->scroll_type == MAUDSCROLL_UP) {
        if(start_renderpos == 0 && first_item->canvas.y < scroll_area->y) {
            queue_props->scroll_y += 10;
            if(queue_props->scroll_y >= scroll_area->y) {
                queue_props->scroll_y = scroll_area->y;
            }
        } else if(start_renderpos) {
            if(first_item->canvas.y <= scroll_area->y) {
                queue_props->scroll_y += 10;
                if(queue_props->scroll_y >= scroll_area->y) {
                    (*queue_props->start_renderpos)--;
                    maud_queueitem_t* prev_item = &queue->items[(*queue_props->start_renderpos)];
                    queue_props->scroll_y = scroll_area->y - (prev_item->canvas.h - 10);
                }
            }
        }
    }
    maud->scroll = false;
}