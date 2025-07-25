#include "maud_player.h"
#include "maud_selectionmenu.h"

void maud_selectionmenu_create(maud_t* maud) {
    // Set the x position for the music selection menu
    maud->music_selectionmenu.x = 0;
    // Set the y position calculation for selection menu will be down below the search bar
    maud->music_selectionmenu.y = maud->music_searchbar.y + maud->music_searchbar.h;
    // set the width and the height for the selection menu
    maud->music_selectionmenu.w = maud->win_width;
    maud->music_selectionmenu.h = maud->music_searchbar.h;
    // whenever we haven't selected anything then we will not render the selection menu onto the screen
    if(!maud->tick_count) {
        maud->item_selected = false;
        return;
    }
    if(!maud->item_selected && maud->tick_count == 1) {
        maud->item_selected = true;
    }
    text_info_t songs_selectioninfo = {0};
    SDL_Rect playbtn_background = {0}, playnext_btnbackground = {0},
             remove_btn = {0}, move_up = {0}, move_down = {0},
             prev_canvas = {0};
    maud_selectionmenu_display_checkallbtn(maud);
    maud_selectionmenu_display_songselectioninfo(maud, &songs_selectioninfo);
    maud_selectionmenu_handle_checkallbtn_toggleoption(maud, songs_selectioninfo);
    if(active_tab == QUEUES_TAB && maud->tick_count > 1) {
        prev_canvas = songs_selectioninfo.text_canvas;
    } else {
        maud_selectionmenu_display_playbtn(maud, songs_selectioninfo, &playbtn_background);
        maud_selectionmenu_handle_playbtn(maud, playbtn_background);
        prev_canvas = playbtn_background;
    }

    maud_selectionmenu_display_playnextbtn(maud, prev_canvas, &playnext_btnbackground);
    maud_selectionmenu_handle_playnextbtn(maud, playnext_btnbackground);
    prev_canvas = playnext_btnbackground;
    maud_selectionmenu_display_addbtn(maud, prev_canvas);
    if(active_tab != SONGS_TAB) {
        maud_selectionmenu_display_removebtn(maud, &remove_btn);
        maud_selectionmenu_handle_removebtn(maud, remove_btn);
        maud_selectionmenu_display_movebtn(maud, 0, remove_btn, &move_up);
        maud_selectionmenu_handle_moveup(maud, move_up);
        maud_selectionmenu_display_movebtn(maud, 1, move_up, &move_down);
        maud_selectionmenu_handle_movedown(maud, move_down);
    }
    if(maud_rect_hover(maud, maud->music_selectionmenu) && maud->mouse_clicked) {
        if(maud->music_selectionmenu_addtobtn_clicked) {
            maud->music_selectionmenu_addtobtn_clicked = false;
        }
        maud->mouse_clicked = false;
    }
}

void maud_selectionmenu_display_movebtn(maud_t* maud, int type, SDL_Rect prevbtn,
    SDL_Rect* move_btnref) {
    if(maud->tick_count > 1) {
        return;
    }
    const char* arrow_path = (type == 0) ? "images/up-arrow.png" : "images/down-arrow.png";
    char* move_text = (type == 0) ? "Move up" : "Move down";
    SDL_Rect arrow_canvas = {
        .x = 0,
        .y = 0,
        .w = 30,
        .h = 30
    };

    text_info_t move_btntext = {
        .font_size = 16,
        .text = move_text,
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    maud_textmanager_sizetext(maud->font, &move_btntext);
    int max_h = (arrow_canvas.h > move_btntext.text_canvas.h) ? arrow_canvas.h :
        move_btntext.text_canvas.h;
    SDL_Rect full_canvas = {
        .x = prevbtn.x + prevbtn.w + 10,
        .y = 0,
        .w = arrow_canvas.w + move_btntext.text_canvas.w + 15,
        .h = max_h + 10
    };
    full_canvas.y = maud->music_selectionmenu.y + (maud->music_selectionmenu.h - full_canvas.h) / 2;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(black));
    SDL_RenderDrawRect(maud->renderer, &full_canvas);
    SDL_RenderFillRect(maud->renderer, &full_canvas);
    arrow_canvas.x = full_canvas.x + 5;
    arrow_canvas.y = full_canvas.y + (full_canvas.h - arrow_canvas.h) / 2;

    SDL_Texture* arrow_texture = IMG_LoadTexture(maud->renderer, arrow_path);
    SDL_RenderCopy(maud->renderer, arrow_texture, NULL, &arrow_canvas);
    SDL_DestroyTexture(arrow_texture);

    move_btntext.text_canvas.x = arrow_canvas.x + arrow_canvas.w + 5;
    move_btntext.text_canvas.y = full_canvas.y + (full_canvas.h - move_btntext.text_canvas.h) / 2;
    SDL_Texture* movebtn_text_texture = maud_textmanager_rendertext(maud, maud->font,
        &move_btntext);
    SDL_RenderCopy(maud->renderer, movebtn_text_texture, NULL, &move_btntext.text_canvas);
    SDL_DestroyTexture(movebtn_text_texture);
    *move_btnref = full_canvas;
}

void maud_selectionmenu_handle_moveup(maud_t* maud, SDL_Rect movebtn) {
    maud_queue_t* play_queue = &maud->play_queue, *selection_queue = &maud->selection_queue;
    if(!maud_rect_hover(maud, movebtn)) {
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
            maud_selectionmenu_clearmusic_selection(maud);
        }
        maud->mouse_clicked = false;
    }
}

void maud_selectionmenu_handle_movedown(maud_t* maud, SDL_Rect movebtn) {
    maud_queue_t* play_queue = &maud->play_queue, *selection_queue = &maud->selection_queue;
    if(!maud_rect_hover(maud, movebtn)) {
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
            maud_selectionmenu_clearmusic_selection(maud);
        }
        maud->mouse_clicked = false;
    }
}

void maud_selectionmenu_display_removebtn(maud_t* maud, SDL_Rect* removebtn_ref) {
    SDL_Rect addtobtn_canvas = maud->music_selectionmenu_addtocanvas;
    SDL_Rect removebtn_icon = {
        .x = 0,
        .y = 0,
        .w = 24,
        .h = 24
    };
    text_info_t removebtn_textinfo = {
        .font_size = 18,
        .text = "Remove",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    maud_textmanager_sizetext(maud->font, &removebtn_textinfo);
    int max_h = (removebtn_icon.h > removebtn_textinfo.text_canvas.h) ? removebtn_icon.h :
                    removebtn_textinfo.text_canvas.h;
    SDL_Rect full_canvas = {
        .x = addtobtn_canvas.x + addtobtn_canvas.w + 10,
        .y = 0,
        .w = removebtn_icon.w + removebtn_textinfo.text_canvas.w + 20,
        .h = max_h + 20
    };
    full_canvas.y = maud->music_selectionmenu.y + (maud->music_selectionmenu.h - full_canvas.h) / 2;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(black));
    SDL_RenderDrawRect(maud->renderer, &full_canvas);
    SDL_RenderFillRect(maud->renderer, &full_canvas);

    removebtn_icon.x = full_canvas.x + 5;
    removebtn_icon.y = full_canvas.y + (full_canvas.h - removebtn_icon.h)/2;
    SDL_Texture* removebtn_icontexture = IMG_LoadTexture(maud->renderer, "images/remove 2.png");
    SDL_RenderCopy(maud->renderer, removebtn_icontexture, NULL, &removebtn_icon);
    SDL_DestroyTexture(removebtn_icontexture);

    removebtn_textinfo.text_canvas.x = removebtn_icon.x + removebtn_icon.w + 5;
    removebtn_textinfo.text_canvas.y = full_canvas.y + (full_canvas.h -
        removebtn_textinfo.text_canvas.h)/2;
    SDL_Texture* removebtn_text_texture = maud_textmanager_rendertext(maud, maud->font,
        &removebtn_textinfo);
    SDL_RenderCopy(maud->renderer, removebtn_text_texture, NULL, &removebtn_textinfo.text_canvas);
    SDL_DestroyTexture(removebtn_text_texture);
    *removebtn_ref = full_canvas;
}

void maud_selectionmenu_handle_removebtn(maud_t* maud, SDL_Rect removebtn) {
    if(active_tab == SONGS_TAB) {
        return;
    }
    if(maud_rect_hover(maud, removebtn)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            maud->remove_btnclicked = true;
            maud->mouse_clicked = false;
        }
    }
}

void maud_selectionmenu_display_addbtn(maud_t* maud, SDL_Rect playnext_btnbackground) {
    // Create add to button used for adding selected musics to current play queue or creating a new playlist of them    
    SDL_Rect addbtn_icon = {
        .x = 0, .y = 0,
        .w = 30, .h = 30
    };
 
    text_info_t addtobtn_text = {
        .font_size = 16,
        .text = "Add to",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    maud_textmanager_sizetext(maud->font, &addtobtn_text);
    int max_h = (addbtn_icon.h > addtobtn_text.text_canvas.h) ? addbtn_icon.h : addtobtn_text.text_canvas.h;
    SDL_Rect full_canvas = {
        .x = playnext_btnbackground.x + playnext_btnbackground.w + 10,
        .y = 0,
        .w = addbtn_icon.w + addtobtn_text.text_canvas.w + 15,
        .h = max_h + 10
    };
    full_canvas.y = maud->music_selectionmenu.y + (maud->music_selectionmenu.h -
        full_canvas.h) / 2;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(black));
    SDL_RenderDrawRect(maud->renderer, &full_canvas);
    SDL_RenderFillRect(maud->renderer, &full_canvas);

    addbtn_icon.x = full_canvas.x + 5;
    addbtn_icon.y = full_canvas.y + (full_canvas.h - addbtn_icon.h) / 2;
    SDL_Texture* addtobtn_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE][music_addtobtn.texture_idx];
    SDL_RenderCopy(maud->renderer, addtobtn_texture, NULL, &addbtn_icon);
    
    addtobtn_text.text_canvas.x = addbtn_icon.x + addbtn_icon.w + 5;
    addtobtn_text.text_canvas.y = full_canvas.y + (full_canvas.h - addtobtn_text.text_canvas.h) / 2;
    SDL_Texture* addtobtn_text_texture = maud_textmanager_rendertext(maud, maud->font, &addtobtn_text);
    SDL_RenderCopy(maud->renderer, addtobtn_text_texture, NULL, &addtobtn_text.text_canvas);
    SDL_DestroyTexture(addtobtn_text_texture); addtobtn_text_texture = NULL;
    maud->music_selectionmenu_addtocanvas = full_canvas;
    bool *addtobtn_clicked = &maud->music_selectionmenu_addtobtn_clicked;
    if(maud_rect_hover(maud, full_canvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            *addtobtn_clicked = !(*addtobtn_clicked);
            maud->mouse_clicked = false;
        }
    }
}

void maud_selectionmenu_handle_addtobtn(maud_t* maud) {
    if(!maud->music_selectionmenu_addtobtn_clicked) {
        return;
    }
    SDL_Rect addtobtn = maud->music_selectionmenu_addtocanvas;
    SDL_Color drop_downcolor = maud->music_selectionmenu_addto_dropdown_color;
    SDL_Rect drop_downmenu = {
        .x = addtobtn.x - 5,
        .y = addtobtn.y + addtobtn.h + 5,
        .w = 200,
        .h = 100
    };
    maud->music_selectionmenu_addto_dropdown = drop_downmenu;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(drop_downcolor));
    SDL_RenderDrawRect(maud->renderer, &drop_downmenu);
    SDL_RenderFillRect(maud->renderer, &drop_downmenu);

    SDL_Rect playqueue_btncanvas = {
        .x = drop_downmenu.x + 10, .y = drop_downmenu.y + 10,
        .w = 30, .h = 30
    };
    text_info_t playqueue_btntext = {
        .font_size = 20,
        .text = "Play queue",
        .text_canvas = {
            .x = playqueue_btncanvas.x + playqueue_btncanvas.w + 5, .y = playqueue_btncanvas.y,
            .w = 0, .h = 0
        },
        .text_color = (const SDL_Color)white,
        .utext = NULL
    };

    SDL_Texture* playqueue_text_texture = maud_textmanager_rendertext(maud, maud->font, &playqueue_btntext);
    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE]
                    [music_playqueuebtn.texture_idx], NULL, &playqueue_btncanvas);
    SDL_RenderCopy(maud->renderer, playqueue_text_texture, NULL, &playqueue_btntext.text_canvas);
    SDL_DestroyTexture(playqueue_text_texture);
    int playqueue_btn_maxheight = (playqueue_btncanvas.h > playqueue_btntext.text_canvas.h) ?
                                    playqueue_btncanvas.h : playqueue_btntext.text_canvas.h;
    SDL_Rect playqueue_btncombo = {
        .x = drop_downmenu.x, .y = playqueue_btncanvas.y,
        .w = drop_downmenu.w, .h = playqueue_btn_maxheight
    };

    SDL_Rect new_playlistbtn_canvas = {
        .x = playqueue_btncanvas.x, .y = playqueue_btncanvas.y + playqueue_btncanvas.h + 10,
        .w = 30, .h = 30
    };

    text_info_t new_playlistbtn_text = {
        .font_size = 20,
        .text = "New playlist",
        .text_canvas = {
            .x = new_playlistbtn_canvas.x + new_playlistbtn_canvas.w + 5,
            .y = new_playlistbtn_canvas.y
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL,
    };
    SDL_Texture* new_playlistbtn_text_texture = maud_textmanager_rendertext(maud, maud->font,
        &new_playlistbtn_text);
    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE]
                                        [music_addplaylistbtn.texture_idx], NULL, &new_playlistbtn_canvas);
    SDL_RenderCopy(maud->renderer, new_playlistbtn_text_texture, NULL, &new_playlistbtn_text.text_canvas);
    SDL_DestroyTexture(new_playlistbtn_text_texture); new_playlistbtn_text_texture = NULL;

    int new_playlistbtn_maxheight = (new_playlistbtn_canvas.h > new_playlistbtn_text.text_canvas.h) ?
                                    new_playlistbtn_canvas.h : new_playlistbtn_text.text_canvas.h;

    SDL_Rect new_playlistbtn_combo = {
        .x = drop_downmenu.x,
        .y = new_playlistbtn_canvas.y,
        .w = drop_downmenu.w,
        .h = new_playlistbtn_maxheight
    };
    if(maud_rect_hover(maud, playqueue_btncombo)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            printf("You clicked the play queue button within the drop down menu\n");
            music_playqueuebtn.clicked = true;
            maud->music_selectionmenu_addtobtn_clicked = false; maud->mouse_clicked = false;
        }
    } else if(maud_rect_hover(maud, new_playlistbtn_combo)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            music_addplaylistbtn.clicked = true;
            printf("You clicked the new playlist button within the drop down menu\n");
            maud->music_selectionmenu_addtobtn_clicked = false; maud->mouse_clicked = false;
        }
    } else if(maud->mouse_clicked && maud->music_selectionmenu_addtobtn_clicked) {
        maud->music_selectionmenu_addtobtn_clicked = false;
        maud->mouse_clicked = false;
    }
}

void maud_selectionmenu_handle_playbtn(maud_t* maud, SDL_Rect playbtn_background) {
    maud_queue_t *play_queue = &maud->play_queue,
                  *selection_queue = &maud->selection_queue;
    if(!maud_rect_hover(maud, playbtn_background)) {
        return;
    }
    size_t music_listindex = 0, music_id = 0, *playid = &play_queue->playid;
    maud_setcursor(maud, MAUD_CURSOR_POINTER);
    if(maud->mouse_clicked) {
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
        maud_selectionmenu_clearmusic_selection(maud);
        maud->mouse_clicked = false;
    }
}

void maud_selectionmenu_display_addtoplaylist_modal(maud_t* maud) {
    if(!music_addplaylistbtn.clicked) {
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
    if(!music_addplaylistbtn.clicked) {
        return;
    }
    SDL_Rect* modal_canvases = maud->addtoplaylist_modalcanvases,
        modal = modal_canvases[ADDTOPLAYLIST_MODALCANVAS],
        createbtn = modal_canvases[ADDTOPLAYLIST_MODALCANVAS_CREATEBTN],
        cancelbtn = modal_canvases[ADDTOPLAYLIST_MODALCANVAS_CANCELBTN];
    // Whenever we click out of the new playlist modal / dialog we will set the new playist button
    // clicked state to true and destroy / clear out the data tha is present within it
    if(!maud_rect_hover(maud, modal) && maud->mouse_clicked) {
        maud_inputbox_clear(&maud->playlist_inputbox);
        music_addplaylistbtn.clicked = false;
        return;
    }
    // Handle click on create button
    if(maud_rect_hover(maud, createbtn)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked && maud->playlist_inputbox.input.data) {
            maud_playlistmanager_createplaylist(maud, maud->playlist_inputbox.input.data);
            maud_playlistmanager_addmusicselection_toplaylist(maud, maud->playlist_inputbox.input.data);
            maud_inputbox_clear(&maud->playlist_inputbox);
            maud_selectionmenu_clearmusic_selection(maud);
            maud->mouse_clicked = false;
            music_addplaylistbtn.clicked = false;
        }
    }
    // Handle click on cancel button
    if(maud_rect_hover(maud, cancelbtn)) {
        if(maud->mouse_clicked) {
            maud_inputbox_clear(&maud->playlist_inputbox);
            maud_selectionmenu_clearmusic_selection(maud);
            maud->mouse_clicked = false;
            music_addplaylistbtn.clicked = false;
        }
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
    }
}

void maud_selectionmenu_display_playbtn(maud_t* maud, text_info_t songs_selectioninfo,
    SDL_Rect* playbtn_backgroundref) {
    // Render the play button icon within the selection menu
    // This can be use to play musics that were selected using the check boxes
    SDL_Texture* music_listplaybtn_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE]
        [music_listplaybtn.texture_idx];

    SDL_Rect playbtn_icon = {
        .x = songs_selectioninfo.text_canvas.x + songs_selectioninfo.text_canvas.w + 10,
        .y = 0,
        .w = 24,
        .h = 30,
    };

    // Add play text beside the play button
    text_info_t play_text = {
        .font_size = 16,
        .text = "Play",
        .text_canvas = {
            .x = playbtn_icon.x + playbtn_icon.w + 5,
            .y = 0,
            .w = 0,
            .h = 0,
        },
        .text_color = {0xFF, 0xFF, 0xFF, 0xFF},
        .utext = NULL
    };
    maud_textmanager_sizetext(maud->font, &play_text);
    SDL_Texture* playtext_texture = maud_textmanager_rendertext(maud, maud->font, &play_text);
    int playbtn_background_height = playbtn_icon.h + 10;
    SDL_Rect playbtn_background = {
        .x = playbtn_icon.x - 5,
        .y = maud->music_selectionmenu.y + (maud->music_selectionmenu.h - playbtn_background_height)/2,
        .w = play_text.text_canvas.w + playbtn_icon.w + 15,
        .h = playbtn_background_height,
    };
    // Render the full play button background or canvas to the screen
    SDL_SetRenderDrawColor(maud->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawRect(maud->renderer, &playbtn_background);
    SDL_RenderFillRect(maud->renderer, &playbtn_background);

    // Copy the play button onto the renderer
    playbtn_icon.y = playbtn_background.y + (playbtn_background.h - playbtn_icon.h) / 2;
    if(SDL_RenderCopy(maud->renderer, music_listplaybtn_texture, NULL,
        &playbtn_icon) < 0) {
        printf("SDL_RenderCopyFailed(): because %s\n", SDL_GetError());
    }
    // Copy the texture for the play button text onto the renderer
    play_text.text_canvas.y = playbtn_background.y + (playbtn_background.h - play_text.text_canvas.h) / 2;
    if(SDL_RenderCopy(maud->renderer, playtext_texture, NULL, &play_text.text_canvas)) {
        printf("SDL_RenderCopyFailed(): because %s\n", SDL_GetError());
    }
    SDL_DestroyTexture(playtext_texture); playtext_texture = NULL;
    *playbtn_backgroundref = playbtn_background;
}

void maud_selectionmenu_display_playnextbtn(maud_t* maud, SDL_Rect playbtn_background,
    SDL_Rect* playnext_backgroundref) {
    SDL_Rect playbtn_icon = {
        .x = 0, .y = 0,
        .w = 24, .h = 30
    };    
    text_info_t playnextbtn_textinfo = {
        .font_size = 16,
        .text = "Play next",
        .text_canvas = {0},
        .text_color = {0xFF, 0xFF, 0xFF, 0xFF},
        .utext = NULL
    };
    maud_textmanager_sizetext(maud->font, &playnextbtn_textinfo);
    int playbtn_backgroundheight = playbtn_icon.h + 10;
    SDL_Rect playnextbtn_background = {
        .x = playbtn_background.x + playbtn_background.w + 10,
        .y = maud->music_selectionmenu.y + (maud->music_selectionmenu.h - playbtn_backgroundheight) / 2,
        .w = playnextbtn_textinfo.text_canvas.w + playbtn_icon.w + 15,
        .h = playbtn_backgroundheight
    };

    SDL_Texture* playnextbtntext_texture = maud_textmanager_rendertext(maud, maud->font,
        &playnextbtn_textinfo);

    // Set the background color for the play button icon
    SDL_SetRenderDrawColor(maud->renderer, black.r, black.g, black.b, 34);
    SDL_RenderDrawRect(maud->renderer, &playnextbtn_background);
    SDL_RenderFillRect(maud->renderer, &playnextbtn_background);

    // Copy the play button texture to its canvas
    playbtn_icon.x = playnextbtn_background.x + 5;
    playbtn_icon.y = playnextbtn_background.y + (playnextbtn_background.h - playbtn_icon.h) / 2;
    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE]
        [music_listplaybtn.texture_idx], NULL, &playbtn_icon);

    // Copy the rendered play next button text to its canvas
    playnextbtn_textinfo.text_canvas.x = playbtn_icon.x + playbtn_icon.w + 5;
    playnextbtn_textinfo.text_canvas.y = playnextbtn_background.y + (playnextbtn_background.h
        - playnextbtn_textinfo.text_canvas.h) / 2;
    SDL_RenderCopy(maud->renderer, playnextbtntext_texture, NULL, &playnextbtn_textinfo.text_canvas);
    SDL_DestroyTexture(playnextbtntext_texture); playnextbtntext_texture = NULL;
    *playnext_backgroundref = playnextbtn_background;
}

void maud_selectionmenu_handle_playnextbtn(maud_t* maud, SDL_Rect playnext_btncanvas) {
    if(maud_rect_hover(maud, playnext_btncanvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        if(maud->mouse_clicked) {
            bool playqueue_wasempty = !maud->play_queue.items;
            maud_queue_addmusicqueue_toplaynext(maud, &maud->play_queue, &maud->selection_queue);
            if(!Mix_PlayingMusic() && playqueue_wasempty) {
                maud->play_queue.playid = 0;
                size_t playid = 0, music_listindex = maud->play_queue.items[playid].music_listindex,
                       music_id = maud->play_queue.items[playid].music_id;
                Mix_PlayMusic(maud->music_lists[music_listindex][music_id].music, 1);
            }
            maud_selectionmenu_clearmusic_selection(maud);
            maud->checkall_btntoggled = true;
        }
    }
}

void maud_selectionmenu_display_songselectioninfo(maud_t* maud, text_info_t* songs_selectioninfo_ref) {
    // The amount of digits that makes up the number of selected songs
    int songs_selectioncount_length = 0;
    size_t music_selection_tickcount = maud->tick_count;
    if(maud->music_selectionmenu_checkbox_tickall) {
        if(active_tab == SONGS_TAB) {
            music_selection_tickcount = maud->music_count;
        } else if(active_tab) {
            music_selection_tickcount = maud->play_queue.item_count;
        }
    }
    // Convert the number of selected songs into a string
    char* songs_selectioncount_text = maud_size_t_tostring(music_selection_tickcount,
            &songs_selectioncount_length);
    //printf("songs_selectioncount_text: %s, Size_t length is %zu\n", songs_selectioncount_text,
    //    songs_selectioncount_length);

    if(!songs_selectioncount_text) {
        printf("Failed to get the selection count length cause memory allocation failure\n");
        return;
    }

    // Allocate memory to store number of selected songs information on screen
    // it will look like this "# of songs"
    char* songs_selectioninfo_text = calloc(songs_selectioncount_length + 16, sizeof(char));
    strcpy(songs_selectioninfo_text, songs_selectioncount_text);
    strcat(songs_selectioninfo_text, " song");
    // If there are more than one songs selected we add an s to pluralize the word song
    (maud->tick_count > 1) ? strcat(songs_selectioninfo_text, "s"): 0;
    strcat(songs_selectioninfo_text, " selected");

    text_info_t songs_selectioninfo = {
        .font_size = 16,
        .text = songs_selectioninfo_text,
        .text_canvas = {
            .x = music_selectionmenu_checkbox_size.x + music_selectionmenu_checkbox_size.w + 10,
            .y = 0,
        },
        .text_color = {},
        .utext = NULL
    };
    maud_textmanager_sizetext(maud->font, &songs_selectioninfo);
    songs_selectioninfo.text_canvas.y = maud->music_selectionmenu.y + (maud->music_selectionmenu.h -
        songs_selectioninfo.text_canvas.h) / 2;

    SDL_Texture* songs_selectioninfo_texture = maud_textmanager_rendertext(maud, maud->font, &songs_selectioninfo);
    SDL_RenderCopy(maud->renderer, songs_selectioninfo_texture, NULL, &songs_selectioninfo.text_canvas);
    SDL_DestroyTexture(songs_selectioninfo_texture); songs_selectioninfo_texture = NULL;
    *songs_selectioninfo_ref = songs_selectioninfo;
    free(songs_selectioncount_text); songs_selectioncount_text = NULL;
    free(songs_selectioninfo_text); songs_selectioninfo_text = NULL;
}

bool maud_selectionmenu_toggleitem_checkboxmusic_queue(maud_t* maud, maud_queue_t* queue,
    size_t item_index) {
    size_t i = item_index, music_listindex = queue->items[i].music_listindex,
        music_id = queue->items[i].music_id;
    if(maud->music_selectionmenu_checkbox_tickall && !queue->items[i].checkbox_ticked) {
        queue->items[i].fill = true;
        queue->items[i].checkbox_ticked = true;
        maud_queue_addmusic(&maud->selection_queue, item_index, music_listindex, music_id);
        maud->music_selected = true;
        if(maud->tick_count < queue->item_count) {
            maud->tick_count++;
        }
        return true;
    } else if(maud->music_selectionmenu_checkbox_clicked &&
        !maud->music_selectionmenu_checkbox_tickall && queue->items[i].checkbox_ticked) {
        queue->items[i].fill = false;
        queue->items[i].checkbox_ticked = false;
        maud_queue_removemusicby_uid(&maud->selection_queue, item_index);
        maud->music_selected = true;
        maud->tick_count--;
        if(!maud->tick_count) {
            maud->music_selectionmenu_checkbox_clicked = false;
        }
        return true;
    }
    return false;

}

bool maud_selectionmenu_togglesong_checkbox(maud_t* maud, music_t* music_list, size_t music_checkbox_index) {
    size_t i = music_checkbox_index;
    if(maud->music_selectionmenu_checkbox_tickall && !music_list[i].checkbox_ticked) {
        music_list[i].fill = true;
        music_list[i].checkbox_ticked = true;
        maud_queue_addmusic(&maud->selection_queue, 0, 0, i);
        maud->music_selected = true;
        if(maud->tick_count < maud->music_count) {
            maud->tick_count++;
        }
        return true;
    } else if(maud->music_selectionmenu_checkbox_clicked &&
        !maud->music_selectionmenu_checkbox_tickall && music_list[i].checkbox_ticked) {
        music_list[i].fill = false;
        music_list[i].checkbox_ticked = false;
        maud_queue_removemusicby_musiclistidx_id(&maud->selection_queue, 0, i);
        maud->music_selected = true;
        maud->tick_count--;
        if(!maud->tick_count) {
            maud->music_selectionmenu_checkbox_clicked = false;
        }
        return true;
    }
    return false;
}

bool maud_selectionmenu_togglecheckbox(maud_t* maud, maud_queue_t* queue, size_t queue_itemindex,
    music_t* music_list, size_t music_checkbox_index) {
    bool ret_val = false;
    if(active_tab == SONGS_TAB) {
        ret_val = maud_selectionmenu_togglesong_checkbox(maud, music_list, music_checkbox_index);
    } else if(active_tab == QUEUES_TAB) {
        ret_val = maud_selectionmenu_toggleitem_checkboxmusic_queue(maud, queue, queue_itemindex);
    }
    return ret_val;
}

void maud_selectionmenu_clearmusic_selection(maud_t* maud) {
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
    maud->music_selectionmenu_checkbox_fillall = false;
    maud->music_selectionmenu_checkbox_tickall = false;
    maud->music_selectionmenu_checkbox_clicked = false;
    free(maud->selection_queue.items); maud->selection_queue.items = NULL;
    maud->selection_queue.item_count = 0;
}

void maud_selectionmenu_handle_checkallbtn_toggleoption(maud_t* maud, text_info_t songs_selectioninfo) {
    if(maud->music_selectionmenu_addtobtn_clicked) {
        return;
    }
    bool *fill = &maud->music_selectionmenu_checkbox_fillall,
         *tick = &maud->music_selectionmenu_checkbox_tickall;
    SDL_Color box_color = {0xff, 0xff, 0xff, 0xff}, tick_color = {0x00, 0xff, 0x00, 0xff},
    fill_color = {0xFF, 0xA5, 0x00, 0xff};
    int width_between = songs_selectioninfo.text_canvas.x - (music_selectionmenu_checkbox_size.x
        + music_selectionmenu_checkbox_size.w);
    SDL_Rect music_selectionmenu_togglecanvas = {
        .x = music_selectionmenu_checkbox_size.x,
        .y = music_selectionmenu_checkbox_size.y,
        .w = music_selectionmenu_checkbox_size.w + width_between + songs_selectioninfo.text_canvas.w,
        .h = music_selectionmenu_checkbox_size.h
    };

    size_t target_count = (active_tab == SONGS_TAB) ? maud->music_count
                        : (active_tab == QUEUES_TAB) ? maud->play_queue.item_count : 0;
    
    if(maud_rect_hover(maud, music_selectionmenu_togglecanvas)) {
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
        *fill = true;
        if(maud->mouse_clicked) {
            if(*tick) {
                *fill = false, *tick = false;
            } else {
                *tick = true;
            }
            maud->checkall_btntoggled = true;
            maud->music_selectionmenu_checkbox_clicked = true;
        }
    } else if(!maud->music_selectionmenu_checkbox_clicked && maud->tick_count == target_count) {
        *fill = true, *tick = true;
    } else {
        *fill = false, *tick = false,
        maud->music_selectionmenu_checkbox_clicked = false;
    }
}

void maud_selectionmenu_display_checkallbtn(maud_t* maud) {
    bool *fill = &maud->music_selectionmenu_checkbox_fillall,
         *tick = &maud->music_selectionmenu_checkbox_tickall;
    SDL_Color box_color = {0xff, 0xff, 0xff, 0xff}, tick_color = {0x00, 0xff, 0x00, 0xff},
    fill_color = {0xFF, 0xA5, 0x00, 0xff};

    SDL_SetRenderDrawColor(maud->renderer, 0x0E, 0x34, 0xA0, 0xFF);
    SDL_RenderDrawRect(maud->renderer, &maud->music_selectionmenu);
    SDL_RenderFillRect(maud->renderer, &maud->music_selectionmenu);

    // Calculate the positions and the height of the checkbox
    music_selectionmenu_checkbox_size.x = maud->music_selectionmenu.x + 10;
    music_selectionmenu_checkbox_size.h = music_listplaybtn.btn_canvas.h;
    music_selectionmenu_checkbox_size.y = maud->music_selectionmenu.y +
                                        ((maud->music_selectionmenu.h - music_selectionmenu_checkbox_size.h)/2);

    mcheckbox_t checkbox_info = {0};
    checkbox_info.checkbox_canvas = music_selectionmenu_checkbox_size;
    checkbox_info.box_color = box_color;
    checkbox_info.tk_color = tick_color;
    checkbox_info.tick = *tick;
    checkbox_info.fill = *fill;
    maud_drawcheckbox(maud, &checkbox_info);
}