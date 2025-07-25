#include "maud_queue.h"
#include "maud_scrollcontainer.h"
#include "maud_checkboxes.h"
#include "maud_songsmanager.h"

void maud_queue_init(maud_queue_t* queue) {
    queue->items = NULL;
    queue->playid = 0;
    queue->item_count = 0;
}

maud_queueitem_t* maud_queue_realloclist_by(maud_queue_t* queue, size_t amount) {
    maud_queueitem_t* new_items = NULL;
    size_t new_itemcount = 0;
    if(!amount) {
        return queue->items;
    }
    if(!queue->items) {
        new_items = malloc(amount * sizeof(maud_queueitem_t));
        new_itemcount = amount;
        queue->item_count = 0;
    } else {
        new_items = realloc(queue->items, (queue->item_count + amount) * sizeof(maud_queueitem_t));
        new_itemcount = queue->item_count + amount;
    }
    if(!new_items) {
        return NULL;
    }
    memset(new_items + queue->item_count, 0, (new_itemcount - queue->item_count) * sizeof(maud_queueitem_t));
    queue->items = new_items;
    queue->item_count = new_itemcount;
    return new_items;
}

bool maud_queue_addmusic(maud_queue_t* queue, size_t uid, size_t music_listindex, size_t music_id) {
    if(!maud_queue_realloclist_by(queue, 1)) {
        return false;
    }
    queue->items[queue->item_count-1].uid = uid;
    queue->items[queue->item_count-1].music_listindex = music_listindex,
    queue->items[queue->item_count-1].music_id = music_id;
    return true;
}

bool maud_queue_addmusicfrom_queue(maud_queue_t* destination_queue, maud_queue_t* source_queue) {
    if(!source_queue->items) {
        return false;
    }
    size_t old_itemcount = destination_queue->item_count;
    if(!maud_queue_realloclist_by(destination_queue, source_queue->item_count)) {
        return false;
    }
    for(size_t i=old_itemcount,j=0;j<source_queue->item_count;i++, j++) {
        destination_queue->items[i] = source_queue->items[j];
    }
    return true;
}

void maud_queue_removemusicby_playid(maud_queue_t* queue, size_t playid) {
    if((!queue->items) || (playid >= queue->item_count)) {
        return;
    }
    for(size_t i=playid;i<queue->item_count-1;i++) {
        maud_queue_swapitem(&queue->items[i], &queue->items[i+1]);
    }
    queue->item_count--;
    queue->items = realloc(queue->items, queue->item_count * sizeof(maud_queueitem_t));
}

void maud_queue_removemusicby_musiclistidx_id(maud_queue_t* queue, size_t music_listindex, size_t musicid) {
    if(!queue->items) {
        return;
    }
    bool found = false;
    size_t target_index = 0;
    for(size_t i=0;i<queue->item_count;i++) {
        if(queue->items[i].music_listindex == music_listindex &&
            queue->items[i].music_id == musicid && !found) {
            target_index = i;
            found = true;
        }
        if(found && i < queue->item_count-1) {
            maud_queue_swapitem(&queue->items[i], &queue->items[i+1]);
        }
    }
    if(!found) {
        return;
    }
    queue->item_count--;
    queue->items = realloc(queue->items, queue->item_count * sizeof(maud_queueitem_t));
}

void maud_queue_removemusicby_uid(maud_queue_t* queue, size_t uid) {
    if(!queue->items) {
        return;
    }
    bool found = false;
    for(size_t i=0;i<queue->item_count;i++) {
        if(queue->items[i].uid == uid && !found) {
            found = true;
        }
        if(found && i < queue->item_count-1) {
            maud_queue_swapitem(&queue->items[i], &queue->items[i+1]);
        }
    }
    if(!found) {
        return;
    }
    queue->item_count--;
    queue->items = realloc(queue->items, queue->item_count * sizeof(maud_queueitem_t));
}

void maud_queue_swap(size_t* a, size_t* b) {
    size_t temp = *a;
    *a = *b, *b = temp;
}

void maud_queue_swapitem(maud_queueitem_t* a, maud_queueitem_t* b) {
    maud_queueitem_t temp = *a;
    *a = *b, *b = temp;
}

bool maud_queue_addmusicqueue_toplaynext(maud_t* maud, maud_queue_t* destination_queue, maud_queue_t* source_queue) {
    if(!source_queue->items) {
        return false;
    }
    if(!destination_queue->items) {
        if(!maud_queue_addmusicfrom_queue(destination_queue, source_queue)) {
            return false;
        }
        return true;
    }
    maud_queue_t new_queue = {0};
    new_queue.playid = destination_queue->playid;
    size_t new_itemcount = destination_queue->item_count + source_queue->item_count;
    if(!maud_queue_realloclist_by(&new_queue, new_itemcount)) {
        return false;
    }
    for(size_t i=0;i<destination_queue->playid+1;i++) {
        new_queue.items[i] = destination_queue->items[i];
    }
    for(size_t i=destination_queue->playid+1,j=0;j<source_queue->item_count;i++, j++) {
        new_queue.items[i] = source_queue->items[j];
    }
    size_t item_afterindex = destination_queue->playid + 1 + source_queue->item_count;
    for(size_t i=item_afterindex,j=destination_queue->playid+1;j<destination_queue->item_count;i++,j++) {
        new_queue.items[i] = destination_queue->items[j];
    }
    maud_queue_destroy(destination_queue);
    destination_queue->items = new_queue.items;
    destination_queue->playid = new_queue.playid;
    destination_queue->item_count = new_queue.item_count;
    return true;
}

size_t maud_queue_getmusic_count(maud_queue_t queue) {
    return queue.item_count;
}

void maud_queue_handleitem_selection(maud_t* maud, maud_queue_t* queue, size_t item_index,
    SDL_Rect outer_canvas, text_info_t* item_textinfo) {
    if(!music_addplaylistbtn.clicked && !maud->music_selectionmenu_addtobtn_clicked
        && maud_rect_hover(maud, outer_canvas)) {
        maud_queue_handlecheckbox_itemselection(maud, queue, item_index);
        maud_queue_handleplaybutton(maud, queue, item_index);
        maud->menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_listplaybtn.texture_idx] =
            music_listplaybtn.btn_canvas;
        if(!maud->tick_count) {
            // if we haven't ticked any checkbox then we render the music list play button to the screen
            item_textinfo->text_canvas.x += music_listplaybtn.btn_canvas.w + 20;
            SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE]
                [music_listplaybtn.texture_idx], NULL, &music_listplaybtn.btn_canvas);
        }
    }
}

void maud_queue_handlecheckbox_itemselection(maud_t* maud, maud_queue_t* queue, size_t item_index) {
    size_t music_listindex = queue->items[item_index].music_listindex,
           music_id = queue->items[item_index].music_id;
    // check if the mouse is hovered over the music
    if(maud_checkbox_hovered(maud)) {
        if(maud->mouse_clicked) {
            // if we are in the position of the checkbox and we clicked it
            switch(queue->items[item_index].checkbox_ticked) {
                case false:
                    /* whenever the checkbox isn't already ticked then we set checkbox as ticked
                       and the fill color for it as true
                    */
                    queue->items[item_index].fill = true;
                    queue->items[item_index].checkbox_ticked = true;
                    maud_queue_addmusic(&maud->selection_queue, item_index, music_listindex,
                        music_id);
                    if(!maud->tick_count) {
                        maud->songsbox_resized = true;
                    }
                    maud->tick_count++;
                    break;
                case true:
                    /* whenever the checkbox is already ticked then we set the checkbox as not ticked
                       and the fill equal to false
                    */
                    queue->items[item_index].fill = false;
                    queue->items[item_index].checkbox_ticked = false;
                    maud_queue_removemusicby_uid(&maud->selection_queue, item_index);
                    maud->tick_count--;
                    if(!maud->tick_count && maud->music_selectionmenu_checkbox_tickall) {
                        maud->music_selectionmenu_checkbox_tickall = false;
                    }
                    if(!maud->tick_count) {
                        maud->songsbox_resized = true;
                    }
                    break;
            }
            maud->music_selected = true;
            maud->mouse_clicked = false;
        } else {
            /* if we just hover over the checkbox without clicking it then we set that checkbox
                fill equal to true
            */
            queue->items[item_index].fill = true;
        }
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
    } else if(maud->tick_count && maud->mouse_clicked && !music_addplaylistbtn.clicked &&
        !maud->music_selectionmenu_addtobtn_clicked) {
        // whenever we click the music without clicking any of its elements
        // we set clicked equal to false to prevent it from performing any action that we do not want
        if(queue->items[item_index].checkbox_ticked) {
            maud->music_selected = true;
            maud->tick_count--;
            if(!maud->tick_count) {
                maud->songsbox_resized = true;
            }
            if(!maud->tick_count && maud->music_selectionmenu_checkbox_tickall) {
                maud->music_selectionmenu_checkbox_tickall = false;
            }
            maud_queue_removemusicby_uid(&maud->selection_queue, item_index);
            queue->items[item_index].fill = false;
            queue->items[item_index].checkbox_ticked = false;
        } else if(maud->tick_count) {
            maud->music_selected = true;
            maud_queue_addmusic(&maud->selection_queue, item_index, music_listindex, music_id);
            maud->tick_count++;
            queue->items[item_index].fill = true;
            queue->items[item_index].checkbox_ticked = true;
        }
        maud->mouse_clicked = false;
    }
}

void maud_queue_handleplaybutton(maud_t* maud, maud_queue_t* queue, size_t item_index) {
    maud_queue_t* play_queue = &maud->play_queue;
    size_t music_listindex = queue->items[item_index].music_listindex,
           music_id = queue->items[item_index].music_id;
    //puts("maud_queue_handleplaybutton(): has not been implemented as yet\n");
    if(maud_musiclist_playbutton_hover(maud) && !music_addplaylistbtn.clicked &&
        !maud->music_selectionmenu_addtobtn_clicked
        && !maud->tick_count) {
        if(maud->mouse_clicked) {
            // Whenever we click the play button for a particular music we play the current play queue
            if(active_tab == SONGS_TAB) {
                maud_queue_destroy(play_queue);
                Mix_HaltMusic();
                play_queue->playid = item_index;
                for(size_t i=0;i<queue->item_count;i++) {
                    int curr_musiclistindex = queue->items[i].music_listindex,
                        curr_musicid = queue->items[i].music_id;
                    maud_queue_addmusic(play_queue, 0, curr_musiclistindex, curr_musicid);
                }
            }

            /* whenever we hover over the play button on the music
               we determine if we should restart the current playing music or play a new music
            */
            if(Mix_PlayingMusic() && play_queue->playid == item_index) {
                Mix_SetMusicPosition(0);
                if(Mix_PausedMusic()) {
                    Mix_ResumeMusic();
                }
            } else {
                if(Mix_PlayingMusic()) {
                    Mix_HaltMusic();
                }
                Mix_SetMusicPosition(0);
                play_queue->playid = item_index;
                if(Mix_PlayMusic(maud->music_lists[music_listindex][music_id].music, 1) == -1) {
                    printf("Failed to play music\n");
                    maud_songsmanager_addplayback_error(maud, maud->music_list[music_id].music_name);
                }
            }
            maud->mouse_clicked = false;
        }
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
    }
}

void maud_queue_display(maud_t* maud, maud_queue_t* queue) {
    maud_scrollcontainer_t* queue_scrollcontainer = &maud->queue_scrollcontainer;
    text_info_t music_name = {
        .font_size = 18,
        .text = NULL,
        .text_canvas = {
            .x = 0, .y = 0,
            .w = 0, .h = 0
        },
        .text_color = white,
        .utext = NULL
    };
    SDL_Rect scroll_area = {
        .x = songs_box.x + 2, .y = songs_box.y + 1,
        .w = maud->win_width, .h = songs_box.h
    };
    SDL_RenderSetClipRect(maud->renderer, &scroll_area);
    maud_scrollcontainer_setprops(queue_scrollcontainer, scroll_area, 20, queue->item_count);
    maud_scrollcontainer_setscroll_area(queue_scrollcontainer, scroll_area);
    maud_scrollcontainer_setcontent_count(queue_scrollcontainer, queue->item_count);
    bool update_itempositions = false;
    if(maud->songsbox_resized) {
        update_itempositions = true;
        printf("Update item positions\n");
        maud_scrollcontainer_setscroll_yfromscroll_area(queue_scrollcontainer);
        maud->songsbox_resized = false;
    }
    SDL_Rect outer_canvas = {
        .x = songs_box.x + 2, .y = queue_scrollcontainer->scroll_y,
        .w = maud->win_width - scrollbar.w - 5, .h = 0
    };
    SDL_Color box_color = {0xff, 0xff, 0xff, 0xff}, tick_color = {0x00, 0xff, 0x00, 0xff},
        fill_color = {0}/*{0xFF, 0xA5, 0x00, 0xff}*/;
    if(maud->remove_btnclicked) {
        for(size_t i=0;i<maud->tick_count;i++) {
            bool reset_playid = false;
            for(size_t j=0;j<queue->item_count;j++) {
                if(queue->items[j].checkbox_ticked) {
                    maud_queue_removemusicby_playid(queue, j);
                    if(queue->playid == j) {
                        if(Mix_PlayingMusic()) {
                            Mix_PauseMusic();
                            Mix_HaltMusic();
                            if(queue->item_count) {
                                queue->playid %= queue->item_count;
                            }
                            if(queue->playid < queue->item_count) {
                                size_t music_listindex = queue->items[queue->playid].music_listindex,
                                music_id = queue->items[queue->playid].music_id;
                                Mix_PlayMusic(maud->music_lists[music_listindex][music_id].music, 1);
                            }
                        }
                    } else if(queue->playid > j) {
                        queue->playid--;
                    }
                    break;
                }
            }
        }
        maud_selectionmenu_clearmusic_selection(maud);
        maud_scrollcontainer_destroy(queue_scrollcontainer);
        maud->remove_btnclicked = false;
        return;
    }
    for(size_t i=0;i<queue->item_count;i++) {
        maud_selectionmenu_toggleitem_checkboxmusic_queue(maud, queue, i);
        size_t music_listindex = queue->items[i].music_listindex;
        music_t* music_list = maud->music_lists[music_listindex];
        size_t music_id = queue->items[i].music_id;
        music_name.utext = maud->music_lists[music_listindex][music_id].music_name;
        maud_textmanager_sizetext(maud->font, &music_name);
        if(i < queue_scrollcontainer->content_renderpos) {
            continue;
        }
        outer_canvas.h = music_name.text_canvas.h + 22;
        music_name.text_canvas.x = outer_canvas.x + 50;
        music_name.text_canvas.y = outer_canvas.y + (outer_canvas.h - music_name.text_canvas.h)/2;
        checkbox_size.x = outer_canvas.x+5;
        checkbox_size.h = outer_canvas.h-10;
        checkbox_size.y = outer_canvas.y + ((outer_canvas.h - checkbox_size.h)/2);
        if(update_itempositions || outer_canvas.y < scroll_area.y + scroll_area.h) {
            maud_scrollcontainer_additem(queue_scrollcontainer, outer_canvas);
            maud_scrollcontainer_setnext_itemcanvas(queue_scrollcontainer, outer_canvas);
            if(!maud->tick_count) {
                music_listplaybtn.btn_canvas.w = 30, music_listplaybtn.btn_canvas.h = outer_canvas.h - 10;
                music_listplaybtn.btn_canvas.x = (checkbox_size.x + checkbox_size.w) + 20,
                music_listplaybtn.btn_canvas.y = checkbox_size.y;
            }
            SDL_Texture* music_nametexture = maud_textmanager_renderunicode(maud,
            maud->music_font, &music_name);
            SDL_SetRenderDrawColor(maud->renderer, 0, 42, 50, 0xFF /*0x3B, 0x35, 0x61, 0xFF*/);
            SDL_RenderDrawRect(maud->renderer, &outer_canvas);
            SDL_RenderFillRect(maud->renderer, &outer_canvas);
            if(maud_rect_hover(maud, outer_canvas)) {
                maud_songsmanager_handlesong_playbutton_hover(maud, outer_canvas);
                maud_queue_handleitem_selection(maud, queue, i, outer_canvas, &music_name);
                if(maud_rect_hover(maud, music_listplaybtn.btn_canvas)) {
                    maud_setcursor(maud, MAUD_CURSOR_POINTER);
                }
                if(maud_checkbox_hovered(maud)) {
                    maud_setcursor(maud, MAUD_CURSOR_POINTER);
                }
            }
            if(maud->tick_count) {
                // whenever any checkbox is ticked and the current music we are current music we are rendering is not ticked
                // we ensure that we set the checkbox fill state to false before drawing its checkbox
                if(!queue->items[i].checkbox_ticked && !maud_rect_hover(maud, outer_canvas)) {
                    queue->items[i].fill = false;
                }
                maud_drawmusic_checkbox(maud, box_color, fill_color, queue->items[i].fill, tick_color,
                    queue->items[i].checkbox_ticked);
            } else if(maud_rect_hover(maud, outer_canvas)) {
                // whenever no checkbox is ticked and we hover over the music we display the check box
                // so that the user can click it or select it
                if(maud_checkbox_hovered(maud)) {
                    maud_drawmusic_checkbox(maud, box_color, fill_color, queue->items[i].fill, tick_color,
                        queue->items[i].checkbox_ticked);
                } else if(!maud_checkbox_hovered(maud)) {
                    maud_drawmusic_checkbox(maud, box_color, fill_color, false, tick_color, false);
                }
            }
            SDL_RenderCopy(maud->renderer, music_nametexture, NULL, &music_name.text_canvas);
            SDL_DestroyTexture(music_nametexture); music_nametexture = NULL;
        }
        
        outer_canvas.y += outer_canvas.h + 3;
    }
    // scroll bar related information
    maud_scrollbar_t songsbox_scrollbar = {
        .rect = scrollbar,
        .displacement = 0.0,
        .orientation = 0,
        .start_pos = queue_scrollcontainer->content_renderpos + 1,
        .final_pos = maud_queue_getmusic_count(*queue),
        .padding_x = -2,
        .padding_y = -(2 + (int)((double)scrollbar.h / 2.0)),
        .scroll_area = scroll_area
    };
    SDL_RenderSetClipRect(maud->renderer, NULL);
    maud_renderscroll_bar(maud, &songsbox_scrollbar, 8);
    if(maud->scroll) {
        maud_scrollcontainer_performscroll_overscrollarea(maud, queue_scrollcontainer);
        maud->scroll = false;
    }
    if(queue_scrollcontainer->item_container.items) {
        maud_scrollcontainer_init(queue_scrollcontainer);
    }
    maud_scrollcontainer_resetitem_index(queue_scrollcontainer);
}

void maud_queue_print(maud_t* maud, maud_queue_t queue) {
    printf("[\n");
    size_t item_count = queue.item_count;
    for(size_t i=0;i<item_count;i++) {
        size_t music_listindex = queue.items[i].music_listindex, music_id = queue.items[i].music_id;
        char* music_name = maud->music_lists[music_listindex][music_id].music_name;
        printf("\t%zu:{music_listindex: %zu, music id:%zu, music_name: %s}", i+1, music_listindex,
            music_id, music_name);
        if(i != item_count-1) {
            printf(",\n");
        }
    }
    printf("\n]\n");
}

void maud_queue_destroy(maud_queue_t* queue) {
    free(queue->items);
    maud_queue_init(queue);
}