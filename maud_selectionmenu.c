#include "maud_modal.h"
#include "maud_selectionmenu.h"
#include "maud_dropdown_menu.h"
#include "maud_songsmanager.h"

void maud_selectionmenu_create(maud_t* maud) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    if(!maud->tick_count) {
        return;
    }
    maud_selectionmenu_renderer_display(maud);
    maud_selectionmenu_handle_selectallbtn_toggleoption(maud, selection_menu);
    maud_selectionmenu_handle_playbtn(maud, selection_menu);
    maud_selectionmenu_handle_playnextbtn(maud, selection_menu);
    maud_selectionmenu_handle_addtobtn(maud, selection_menu);
    maud_selectionmenu_handle_removebtn(maud, selection_menu);
    maud_selectionmenu_handle_moveup(maud, selection_menu);
    maud_selectionmenu_handle_movedown(maud, selection_menu);
    maud_selectionmenu_init_addto_dropdown(maud);
}

void maud_selectionmenu_handle_selectallbtn_toggleoption(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_selectallbtn_t* select_allbtn = &selection_menu->select_allbtn;
    maud_selectionmenubtn_t* addto_btn = &selection_menu->addtobtn;
    bool *fill = &select_allbtn->checkbox.fill,
         *tick = &select_allbtn->checkbox.tick;
    if(addto_btn->clicked) {
        return;
    }
    size_t target_count = (active_tab == SONGS_TAB) ? maud->music_count
                        : (active_tab == QUEUES_TAB) ? maud->play_queue.item_count : 0;
    if(maud_rect_hover(maud, select_allbtn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        *fill = true;
        if(maud->mouse_clicked) {
            if(*tick) {
                *fill = false, *tick = false;
            } else {
                *tick = true;
            }
            select_allbtn->clicked = true;
            maud->checkall_btntoggled = true;
            maud->mouse_clicked = false;
        }
    } else if(!select_allbtn->clicked && maud->tick_count == target_count) {
        *fill = true, *tick = true;
    } else {
        *fill = false, *tick = false;
        select_allbtn->clicked = false;
    }
}

bool maud_selectionmenu_togglesong_checkbox(maud_t* maud, music_t* music_list, size_t music_checkbox_index) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenu_selectallbtn_t* select_all = &selection_menu->select_allbtn;
    size_t i = music_checkbox_index;
    if(select_all->checkbox.tick && !music_list[i].checkbox_ticked) {
        music_list[i].fill = true;
        music_list[i].checkbox_ticked = true;
        maud_queue_addmusic(maud, &maud->selection_queue, 0, 0, i);
        maud->music_selected = true;
        if(maud->tick_count < maud->music_count) {
            maud->tick_count++;
        }
        return true;
    } else if(select_all->clicked &&
        !select_all->checkbox.tick && music_list[i].checkbox_ticked) {
        music_list[i].fill = false;
        music_list[i].checkbox_ticked = false;
        maud_queue_removemusicby_musiclistidx_id(&maud->selection_queue, 0, i);
        maud->music_selected = true;
        maud->tick_count--;
        if(!maud->tick_count) {
            select_all->clicked = false;
        }
        return true;
    }
    return false;
}

void maud_selectionmenu_handle_playbtn(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenubtn_t* playbtn = &selection_menu->playbtn;
    maud_queue_t *play_queue = &maud->play_queue,
                 *selection_queue = &maud->selection_queue;
    if(!maud_rect_hover(maud, playbtn->canvas)) {
        return;
    }
    size_t music_listindex = 0, music_id = 0, *playid = &play_queue->playid;
    maud_setcursor(maud, MAUD_CURSOR_POINTER);
    if(maud->mouse_clicked) {
        play_queue->playing = true;
        printf("You clicked the play button now we are gonna modify the play queue\n");
        if(active_tab == QUEUES_TAB) {
            *playid = selection_queue->items[0].uid;
        } else {
            maud_queue_destroy(play_queue);
            if(!maud_queue_addmusicfrom_queue(play_queue, selection_queue)) {
                printf("Failed to add the selection queue to the play queue\n");
                Mix_HaltMusic();
                return;
            }
            printf("Successfully added the selection queue to the play queue and play queue now looks like:");
            printf("maud->play_queue->item_count = %zu\n", play_queue->item_count);
            maud_queue_print(maud, *play_queue);
        }
        if(Mix_PlayingMusic() ) {
            Mix_PauseMusic();
            Mix_HaltMusic();
        }
        music_listindex = play_queue->items[*playid].music_listindex,
        music_id = play_queue->items[*playid].music_id;
        if(play_queue->items && !Mix_PlayMusic(maud->music_lists[music_listindex]
            [music_id].music, 1)) {
            printf("Playing music %s\n",
                maud->music_lists[music_listindex][music_id].music_name
            );
        }
        maud_selectionmenu_clearmusic_selection(maud, selection_menu);
        maud->mouse_clicked = false;
    }
}

void maud_selectionmenu_handle_playnextbtn(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenubtn_t* playnextbtn = &selection_menu->playnextbtn;
    maud_selectionmenu_selectallbtn_t* select_allbtn = &selection_menu->select_allbtn;
    maud_queue_t* play_queue = &maud->play_queue;
    if(maud_rect_hover(maud, playnextbtn->canvas)) {
        if(maud->mouse_clicked) {
            play_queue->playing = true;
            bool playqueue_wasempty = !maud->play_queue.items;
            maud_queue_addmusicqueue_toplaynext(maud, &maud->play_queue, &maud->selection_queue);
            if(!Mix_PlayingMusic() && playqueue_wasempty) {
                maud->play_queue.playid = 0;
                size_t playid = 0, music_listindex = maud->play_queue.items[playid].music_listindex,
                       music_id = maud->play_queue.items[playid].music_id;
                Mix_PlayMusic(maud->music_lists[music_listindex][music_id].music, 1);
            }
            maud_selectionmenu_clearmusic_selection(maud, selection_menu);
            select_allbtn->toggled = true;
        }
    }
}

void maud_selectionmenu_handle_addtobtn(maud_t* maud,
    maud_selectionmenu_t* selection_menu) {
    maud_selectionmenubtn_t* addtobtn = &selection_menu->addtobtn;
    if(maud_rect_hover(maud, addtobtn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            addtobtn->clicked = !addtobtn->clicked;
            maud->mouse_clicked = false;
        }
    }
}

void maud_selectionmenu_handle_removebtn(maud_t* maud,
    maud_selectionmenu_t* selection_menu) {
    maud_selectionmenubtn_t* removebtn = &selection_menu->removebtn;
    maud_queue_t *play_queue = &maud->play_queue,
                 *selection_queue = &maud->selection_queue;
    size_t found_index = 0;
    bool resume = false, paused = false;
    if(maud_rect_hover(maud, removebtn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            while(maud_queue_findfirst_selection(play_queue, &found_index)) {
                if(Mix_PlayingMusic()) {
                    if(play_queue->playid == found_index) {
                        if(!Mix_PausedMusic()) {
                            Mix_PauseMusic();
                            resume = true;
                        } else {
                            paused = true;
                        }
                    }
                }
                maud_queue_removemusicby_playid(play_queue, found_index);
            }
            if(play_queue->playid >= play_queue->item_count) {
                play_queue->playid = 0;
            }
            if(!play_queue->items) {
                Mix_HaltMusic();
            } else if(resume) {
                maud_songsmanager_playmusic(maud);
            } else if(paused) {
                maud_songsmanager_playmusic_pause(maud);
            }
            maud_selectionmenu_clearmusic_selection(maud, selection_menu);
            printf("Sucessfully removed selected music!\n");
            // TODO: Implement this for playlists
            maud->mouse_clicked = false;
        }
    }
}

void maud_selectionmenu_handle_moveup(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenubtn_t* movebtn = &selection_menu->moveup_btn;
    maud_queue_t* play_queue = &maud->play_queue,
    *selection_queue = &maud->selection_queue;
    if(!movebtn->render) {
        return;
    }
    if(!maud_rect_hover(maud, movebtn->canvas)) {
        return;
    }
    maud_setcursor(maud, MAUD_CURSOR_POINTER);
    if(maud->mouse_clicked) {
        size_t item_index = selection_queue->items[0].uid;
        if(item_index) {
            if(play_queue->playid == item_index) {
                play_queue->playid = item_index-1;
            } else if(play_queue->playid == item_index-1) {
                play_queue->playid = item_index;
            }
            maud_queue_swapitem(&play_queue->items[item_index], &play_queue->items[item_index-1]);
            maud_selectionmenu_clearmusic_selection(maud, selection_menu);
        }
        maud->mouse_clicked = false;
    }
}

void maud_selectionmenu_handle_movedown(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenubtn_t* movebtn = &selection_menu->movedown_btn;
    maud_queue_t *play_queue = &maud->play_queue,
                 *selection_queue = &maud->selection_queue;
    if(!movebtn->render) {
        return;
    }
    if(!maud_rect_hover(maud, movebtn->canvas)) {
        return;
    }
    maud_setcursor(maud, MAUD_CURSOR_POINTER);
    if(maud->mouse_clicked) {
        size_t item_index = selection_queue->items[0].uid;
        if(item_index < play_queue->item_count-1) {
            if(play_queue->playid == item_index) {
                play_queue->playid = item_index+1;
            } else if(play_queue->playid == item_index+1) {
                play_queue->playid = item_index;
            }
            maud_queue_swapitem(&play_queue->items[item_index], &play_queue->items[item_index+1]);
            maud_selectionmenu_clearmusic_selection(maud, selection_menu);
        }
        maud->mouse_clicked = false;
    }
}


void maud_selectionmenu_init_addto_dropdown(maud_t* maud) {
    maud_dropdown_menu_t* dropdown = &maud->dropdown_menus[MAUD_DROPDOWN_SELECTIONMENU];
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenubtn_t* addtobtn = &selection_menu->addtobtn;
    if(!addtobtn->clicked) {
        return;
    }
    maud->dropdown_menuindex = MAUD_DROPDOWN_SELECTIONMENU;
    maud_dropdown_menu_init(dropdown,
        addtobtn->canvas.x - 5,
        addtobtn->canvas.y + addtobtn->canvas.h + 5,
        200,
        100,
        maud->music_selectionmenu_addto_dropdown_color,
        10,
        10,
        5,
        30,
        30
    );
    if(!dropdown->items) {
        maud_dropdown_menu_add(dropdown,
            maud->font,
            "Play Queue",
            20,
            NULL,
            maud->menu->textures[MAUD_BUTTON_TEXTURE]
                    [music_playqueuebtn.texture_idx],
            (SDL_Color){0xff, 0xff, 0xff, 0xff}
        );
        
        maud_dropdown_menu_add(dropdown,
            maud->font,
            "New playlist",
            20,
            NULL,
            maud->menu->textures[MAUD_BUTTON_TEXTURE]
                [music_addplaylistbtn.texture_idx],
            (SDL_Color){0xff, 0xff, 0xff, 0xff}
        );
    }
    maud_dropdown_menu_setitem_positions(maud, dropdown);
    if(dropdown->item_clicked) {
        addtobtn->clicked = false;
        dropdown->item_clicked = false;
    } else if(maud->mouse_clicked && addtobtn->clicked) {
        addtobtn->clicked = false;
        maud->mouse_clicked = false;
    }
}

void maud_selectionmenu_display_addto_dropdown(maud_t* maud) {
    maud_dropdown_menu_t* dropdown = &maud->dropdown_menus[maud->dropdown_menuindex];
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenubtn_t* addtobtn = &selection_menu->addtobtn;
    if(!addtobtn->clicked) {
        return;
    }
    maud_dropdown_menu_render(maud, dropdown);
}

void maud_selectionmenu_clearmusic_selection(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenu_selectallbtn_t* select_allbtn = &selection_menu->select_allbtn;
    for(size_t i=0;i<maud->play_queue.item_count;i++) {
        maud->play_queue.items[i].fill = false;
        maud->play_queue.items[i].checkbox_ticked = false;
    }
    if(active_tab == QUEUES_TAB) {
        maud->songsbox_resized = true;
    }
    for(size_t i=0;i<maud->selection_queue.item_count;i++) {
        size_t music_listindex = maud->selection_queue.items[i].music_listindex,
        music_id = maud->selection_queue.items[i].music_id;
        maud->music_lists[music_listindex][music_id].fill = false;
        maud->music_lists[music_listindex][music_id].checkbox_ticked = false;
    }

    maud->tick_count = 0;
    select_allbtn->checkbox.tick = false;
    select_allbtn->checkbox.fill = false;
    select_allbtn->clicked = false;
    free(maud->selection_queue.items); maud->selection_queue.items = NULL;
    maud->selection_queue.item_count = 0;
}

void maud_selectionmenu_display(maud_t* maud) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenu_renderer_display(maud);
}

void maud_selectionmenu_display_addtoplaylist_modal(maud_t* maud) {
    maud_dropdown_menu_t* dropdown = &maud->dropdown_menus[maud->dropdown_menuindex];
    if(!dropdown->items) {
        return;
    }
    maud_dropdown_item_t* item = &dropdown->items[1];
    if(!item->clicked) {
        return;
    }
    maud_modal_t* addtoplaylist_modal = &maud->addtoplaylist_modal;
    addtoplaylist_modal->image.texture = maud->menu->textures[MAUD_BUTTON_TEXTURE]
            [music_addplaylistbtn.texture_idx];
    maud_modal_display(maud, addtoplaylist_modal);
}

void maud_selectionmenu_handle_addtoplaylist_modalevents(maud_t* maud) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_dropdown_menu_t* dropdown = &maud->dropdown_menus[maud->dropdown_menuindex];
    if(!dropdown->items) {
        return;
    }
    maud_dropdown_item_t* item = &dropdown->items[1];
    if(!item->clicked) {
        return;
    }
    maud_modal_t* addtoplaylist_modal = &maud->addtoplaylist_modal;
    maud_modal_input_t* modal_input = &addtoplaylist_modal->input;
    maud_inputbox_t* inputbox = &modal_input->inputbox;
    maud_modal_handle_events(maud, addtoplaylist_modal);
    if(addtoplaylist_modal->leave) {
        item->clicked = false;
        dropdown->item_clicked = false;
        addtoplaylist_modal->leave = false;
        return;
    }

    maud_modal_button_t* create_btn = &addtoplaylist_modal->confirm_button;
    // Handle click on createbtn
    if(create_btn->clicked && inputbox->input.data) {
        maud_playlistmanager_createplaylist(maud, inputbox->input.data);
        maud_playlistmanager_addmusicselection_toplaylist(maud, inputbox->input.data);
        maud_inputbox_clear(inputbox);
        maud_selectionmenu_clearmusic_selection(maud, selection_menu);
        item->clicked = false;
        dropdown->item_clicked = false;
        create_btn->clicked = false;
    }

    // Handle click on cancel button
    maud_modal_button_t* cancel_btn = &addtoplaylist_modal->cancel_button;
    if(cancel_btn->clicked) {
        maud_selectionmenu_clearmusic_selection(maud, selection_menu);
        item->clicked = false;
        dropdown->item_clicked = false;
        cancel_btn->clicked = false;
    }
}