#include "maud_selectionmenu.h"
#include "maud_dropdown_menu.h"

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
    if(maud_rect_hover(maud, removebtn->canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            printf("You clicked the remove button\n");
            if(active_tab == QUEUES_TAB) {
                printf("play queue");
                maud_queue_print(maud, *play_queue);
                for(size_t i=0;i<selection_queue->item_count;i++) {
                    printf("Trying to remove music %s with uid %zu\n",
                        selection_queue->items[i].music_item->music_name,
                        selection_queue->items[i].uid
                    );
                    maud_queue_removemusicby_music(play_queue,
                        selection_queue->items[i].uid);
                }
            }
            maud_selectionmenu_clearmusic_selection(maud, selection_menu);
            // TODO: Implement this for playlists
            maud->mouse_clicked = false;
        }
    }
}

void maud_selectionmenu_handle_moveup(maud_t* maud, maud_selectionmenu_t* selection_menu) {
    maud_selectionmenubtn_t* movebtn = &selection_menu->moveup_btn;
    maud_queue_t* play_queue = &maud->play_queue,
    *selection_queue = &maud->selection_queue;
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
    maud_selectionmenubtn_t* movebtn = &selection_menu->moveup_btn;
    maud_queue_t *play_queue = &maud->play_queue,
                 *selection_queue = &maud->selection_queue;
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
    maud_dropdown_menu_t* dropdown = &maud->dropdown;
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_selectionmenubtn_t* addtobtn = &selection_menu->addtobtn;
    if(!addtobtn->clicked) {
        return;
    }
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
    maud_dropdown_menu_t* dropdown = &maud->dropdown;
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
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_dropdown_menu_t* dropdown = &maud->dropdown;
    if(!dropdown->items) {
        return;
    }
    maud_dropdown_item_t* item = &dropdown->items[1];
    if(!item->clicked) {
        return;
    }
    SDL_Rect modal = {
        .x = 0, .y = 0,
        .w = 400, .h = 450
    };
    modal.x = (maud->win_width - modal.w)/2, modal.y = (maud->win_height - modal.h)/2;
    SDL_Color modal_color = {0x21, 0x01, 0x24, 0xFF};
    
    // Render the Add new playlist modal box and its contents / elements
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(modal_color));
    SDL_RenderDrawRect(maud->renderer, &modal);
    SDL_RenderFillRect(maud->renderer, &modal);

    // Render new playlist text on modal box
    text_info_t newplaylist_text = {
        .font_size = 20,
        .text = "Add to new playlist",
        .text_canvas = {
            .x = modal.x + 20, .y = modal.y + 20,
            .w = 0, .h = 0
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    SDL_Texture *newplaylist_text_texture = maud_textmanager_rendertext(maud, maud->font, &newplaylist_text);
    SDL_Rect newplaylist_textbgcanvas = {
        .x = modal.x, .y = modal.y,
        .w = modal.w, .h = newplaylist_text.text_canvas.h + 40
    };
    // Draw background for "Add to new playlist" text
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(dark_purple));
    SDL_RenderDrawRect(maud->renderer, &newplaylist_textbgcanvas);
    SDL_RenderFillRect(maud->renderer, &newplaylist_textbgcanvas);
    SDL_RenderCopy(maud->renderer, newplaylist_text_texture, NULL, &newplaylist_text.text_canvas);
    SDL_DestroyTexture(newplaylist_text_texture); newplaylist_text_texture = NULL;

    // Draw a background canvas for the image and render the the new playlist image on the modal
    SDL_Rect newplaylist_imagecanvas = {
        .x = 0, .y = newplaylist_text.text_canvas.y + newplaylist_text.text_canvas.h + 50,
        .w = 140, .h = 140
    };
    SDL_Color newplaylist_imagecanvas_color = {0x00, 0x00, 0x00, 0x00};
    newplaylist_imagecanvas.x = (maud->win_width - newplaylist_imagecanvas.w) / 2;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(newplaylist_imagecanvas_color));
    SDL_RenderDrawRect(maud->renderer, &newplaylist_imagecanvas);
    SDL_RenderFillRect(maud->renderer, &newplaylist_imagecanvas);
    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE]
                    [music_addplaylistbtn.texture_idx], NULL, &newplaylist_imagecanvas);

    //maud_inputbox_getsize(maud, &maud->playlist_inputbox);
    maud->playlist_inputbox.canvas.x = modal.x + (modal.w -
        maud->playlist_inputbox.canvas.w) / 2,
    maud->playlist_inputbox.canvas.y = newplaylist_imagecanvas.y + newplaylist_imagecanvas.h + 30;

    SDL_Rect *placeholder_canvas = &maud->playlist_inputbox.placeholder_canvas;
    placeholder_canvas->x = maud->playlist_inputbox.canvas.x + 10;
    placeholder_canvas->y = maud->playlist_inputbox.canvas.y + (maud->
        playlist_inputbox.canvas.h - placeholder_canvas->h) / 2;
    maud_inputbox_display(maud, &maud->playlist_inputbox);

    // Add the create button to the new playlist modal box
    text_info_t createbtn = {
        .font_size = 20,
        .text = "Create",
        .text_canvas = {
            .x = modal.x + 20, .y = modal.y + modal.h - 50,
            .w = 0, .h = 0
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    SDL_Color createbtn_canvascolor = {0x00, 0x00, 0xff, 0xff}/*{0xf4, 60, 36, 0xff}*/;
    SDL_Texture* createbtn_texture = maud_textmanager_rendertext(maud, maud->font, &createbtn);
    createbtn.text_canvas.y -= createbtn.text_canvas.h;
    SDL_Rect createbtn_canvas = {
        .x = createbtn.text_canvas.x - 10, .y = createbtn.text_canvas.y - 10,
        .w = createbtn.text_canvas.w + 20, .h = createbtn.text_canvas.h + 20
    };

    // Add the cancel button to the new playlist modal box
    text_info_t cancelbtn = {
        .font_size = 20,
        .text = "Cancel",
        .text_canvas = {
            .x = modal.x + modal.w - 20, .y = modal.y + modal.h - 50,
            .w = 0, .h = 0
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    SDL_Texture* cancelbtn_texture = maud_textmanager_rendertext(maud, maud->font, &cancelbtn);
    cancelbtn.text_canvas.x -= cancelbtn.text_canvas.w, cancelbtn.text_canvas.y -= cancelbtn.text_canvas.h;
    SDL_Rect cancelbtn_canvas = {
        .x = cancelbtn.text_canvas.x - 10, .y = cancelbtn.text_canvas.y - 10,
        .w = cancelbtn.text_canvas.w + 20, .h = cancelbtn.text_canvas.h + 20  
    };
    SDL_Color cancelbtn_canvascolor = {0xf4, 60, 36, 0xff}/*{0x00, 0x00, 0xff, 0xff}*/;

    SDL_Rect button_container = {
        .x = modal.x, .y = createbtn_canvas.y - 10,
        .w = modal.w, .h = (modal.y + modal.h) - (createbtn_canvas.y + createbtn_canvas.h) + createbtn_canvas.h + 20
    };

    // Render a container to hold the create and cancel button within the modal
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(dark_purple));
    SDL_RenderDrawRect(maud->renderer, &button_container);
    SDL_RenderFillRect(maud->renderer, &button_container);

    // Render the create button onto the modal
    createbtn_canvas.y = button_container.y + (button_container.h - createbtn_canvas.h) / 2;
    createbtn.text_canvas.y = createbtn_canvas.y + (createbtn_canvas.h - createbtn.text_canvas.h) / 2;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(createbtn_canvascolor));
    SDL_RenderDrawRect(maud->renderer, &createbtn_canvas);
    SDL_RenderFillRect(maud->renderer, &createbtn_canvas);
    SDL_RenderCopy(maud->renderer, createbtn_texture, NULL, &createbtn.text_canvas);
    SDL_DestroyTexture(createbtn_texture); createbtn_texture = NULL;

    // Render the cancel button onto the modal
    cancelbtn_canvas.y = button_container.y + (button_container.h - cancelbtn_canvas.h) / 2;
    cancelbtn.text_canvas.y = cancelbtn_canvas.y + (cancelbtn_canvas.h - cancelbtn.text_canvas.h) / 2;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(cancelbtn_canvascolor));
    SDL_RenderDrawRect(maud->renderer, &cancelbtn_canvas);
    SDL_RenderFillRect(maud->renderer, &cancelbtn_canvas);
    SDL_RenderCopy(maud->renderer, cancelbtn_texture, NULL, &cancelbtn.text_canvas);
    SDL_DestroyTexture(cancelbtn_texture); cancelbtn_texture = NULL;

    maud->addtoplaylist_modalcanvases[ADDTOPLAYLIST_MODALCANVAS] = modal,
    maud->addtoplaylist_modalcanvases[ADDTOPLAYLIST_MODALCANVAS_CREATEBTN] = createbtn_canvas,
    maud->addtoplaylist_modalcanvases[ADDTOPLAYLIST_MODALCANVAS_CANCELBTN] = cancelbtn_canvas;
}

void maud_selectionmenu_handle_addtoplaylist_modalevents(maud_t* maud) {
    maud_selectionmenu_t* selection_menu = &maud->selection_menu;
    maud_dropdown_menu_t* dropdown = &maud->dropdown;
    if(!dropdown->items) {
        return;
    }
    maud_dropdown_item_t* item = &dropdown->items[1];
    if(!item->clicked) {
        return;
    }
    //printf("maud_selectionmenu_handle_addtoplaylist_modalevents(): %ld\n", __LINE__);
    SDL_Rect* modal_canvases = maud->addtoplaylist_modalcanvases,
        modal = modal_canvases[ADDTOPLAYLIST_MODALCANVAS],
        createbtn = modal_canvases[ADDTOPLAYLIST_MODALCANVAS_CREATEBTN],
        cancelbtn = modal_canvases[ADDTOPLAYLIST_MODALCANVAS_CANCELBTN];
    // Whenever we click out of the new playlist modal / dialog we will set the new playist button
    // clicked state to true and destroy / clear out the data tha is present within it
    if(!maud_rect_hover(maud, modal) && maud->mouse_clicked) {
        maud_inputbox_clear(&maud->playlist_inputbox);
        item->clicked = false;
        dropdown->item_clicked = false;
        return;
    }
    // Handle click on create button
    if(maud_rect_hover(maud, createbtn)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked && maud->playlist_inputbox.input.data) {
            maud_playlistmanager_createplaylist(maud, maud->playlist_inputbox.input.data);
            maud_playlistmanager_addmusicselection_toplaylist(maud, maud->playlist_inputbox.input.data);
            maud_inputbox_clear(&maud->playlist_inputbox);
            maud_selectionmenu_clearmusic_selection(maud, selection_menu);
            maud->mouse_clicked = false;
            item->clicked = false;
            dropdown->item_clicked = false;
        }
    }
    // Handle click on cancel button
    if(maud_rect_hover(maud, cancelbtn)) {
        if(maud->mouse_clicked) {
            maud_inputbox_clear(&maud->playlist_inputbox);
            maud_selectionmenu_clearmusic_selection(maud, selection_menu);
            maud->mouse_clicked = false;
            item->clicked = false;
            dropdown->item_clicked = false;
        }
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
    }
}