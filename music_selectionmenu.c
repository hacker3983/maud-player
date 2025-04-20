#include "music_player.h"
#include "music_selectionmenu.h"

void mplayer_selectionmenu_create(mplayer_t* mplayer) {
    // Set the x position for the music selection menu
    mplayer->music_selectionmenu.x = 0;
    // Set the y position calculation for selection menu will be down below the search bar
    mplayer->music_selectionmenu.y = mplayer->music_searchbar.y + mplayer->music_searchbar.h;
    // set the width and the height for the selection menu
    mplayer->music_selectionmenu.w = mplayer->win_width;
    mplayer->music_selectionmenu.h = mplayer->music_searchbar.h;
    // whenever we haven't selected anything then we will not render the selection menu onto the screen
    if(!mplayer->tick_count) {
        mplayer->item_selected = false;
        return;
    }
    if(!mplayer->item_selected && mplayer->tick_count == 1) {
        mplayer->item_selected = true;
    }
    text_info_t songs_selectioninfo = {0};
    SDL_Rect playbtn_background = {0}, playnext_btnbackground = {0},
             remove_btn = {0}, move_up = {0}, move_down = {0},
             prev_canvas = {0};
    mplayer_selectionmenu_display_checkallbtn(mplayer);
    mplayer_selectionmenu_display_songselectioninfo(mplayer, &songs_selectioninfo);
    mplayer_selectionmenu_handle_checkallbtn_toggleoption(mplayer, songs_selectioninfo);
    if(active_tab == QUEUES_TAB && mplayer->tick_count > 1) {
        prev_canvas = songs_selectioninfo.text_canvas;
    } else {
        mplayer_selectionmenu_display_playbtn(mplayer, songs_selectioninfo, &playbtn_background);
        mplayer_selectionmenu_handle_playbtn(mplayer, playbtn_background);
        prev_canvas = playbtn_background;
    }

    mplayer_selectionmenu_display_playnextbtn(mplayer, prev_canvas, &playnext_btnbackground);
    mplayer_selectionmenu_handle_playnextbtn(mplayer, playnext_btnbackground);
    prev_canvas = playnext_btnbackground;
    mplayer_selectionmenu_display_addbtn(mplayer, prev_canvas);
    if(active_tab != SONGS_TAB) {
        mplayer_selectionmenu_display_removebtn(mplayer, &remove_btn);
        mplayer_selectionmenu_handle_removebtn(mplayer, remove_btn);
        mplayer_selectionmenu_display_movebtn(mplayer, 0, remove_btn, &move_up);
        mplayer_selectionmenu_handle_moveup(mplayer, move_up);
        mplayer_selectionmenu_display_movebtn(mplayer, 1, move_up, &move_down);
        mplayer_selectionmenu_handle_movedown(mplayer, move_down);
    }
    if(mplayer_rect_hover(mplayer, mplayer->music_selectionmenu) && mplayer->mouse_clicked) {
        if(mplayer->music_selectionmenu_addtobtn_clicked) {
            mplayer->music_selectionmenu_addtobtn_clicked = false;
        }
        mplayer->mouse_clicked = false;
    }
}

void mplayer_selectionmenu_display_movebtn(mplayer_t* mplayer, int type, SDL_Rect prevbtn,
    SDL_Rect* move_btnref) {
    if(mplayer->tick_count > 1) {
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
    mplayer_textmanager_sizetext(mplayer->font, &move_btntext);
    int max_h = (arrow_canvas.h > move_btntext.text_canvas.h) ? arrow_canvas.h :
        move_btntext.text_canvas.h;
    SDL_Rect full_canvas = {
        .x = prevbtn.x + prevbtn.w + 10,
        .y = 0,
        .w = arrow_canvas.w + move_btntext.text_canvas.w + 15,
        .h = max_h + 10
    };
    full_canvas.y = mplayer->music_selectionmenu.y + (mplayer->music_selectionmenu.h - full_canvas.h) / 2;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(black));
    SDL_RenderDrawRect(mplayer->renderer, &full_canvas);
    SDL_RenderFillRect(mplayer->renderer, &full_canvas);
    arrow_canvas.x = full_canvas.x + 5;
    arrow_canvas.y = full_canvas.y + (full_canvas.h - arrow_canvas.h) / 2;

    SDL_Texture* arrow_texture = IMG_LoadTexture(mplayer->renderer, arrow_path);
    SDL_RenderCopy(mplayer->renderer, arrow_texture, NULL, &arrow_canvas);
    SDL_DestroyTexture(arrow_texture);

    move_btntext.text_canvas.x = arrow_canvas.x + arrow_canvas.w + 5;
    move_btntext.text_canvas.y = full_canvas.y + (full_canvas.h - move_btntext.text_canvas.h) / 2;
    SDL_Texture* movebtn_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        &move_btntext);
    SDL_RenderCopy(mplayer->renderer, movebtn_text_texture, NULL, &move_btntext.text_canvas);
    SDL_DestroyTexture(movebtn_text_texture);
    *move_btnref = full_canvas;
}

void mplayer_selectionmenu_handle_moveup(mplayer_t* mplayer, SDL_Rect movebtn) {
    music_queue_t* play_queue = &mplayer->play_queue, *selection_queue = &mplayer->selection_queue;
    if(!mplayer_rect_hover(mplayer, movebtn)) {
        return;
    }
    mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
    if(mplayer->mouse_clicked) {
        size_t item_index = selection_queue->items[0].uid;
        if(item_index) {
            if(play_queue->playid == item_index) {
                play_queue->playid = item_index-1;
            } else if(play_queue->playid == item_index-1) {
                play_queue->playid = item_index;
            }
            mplayer_queue_swapitem(&play_queue->items[item_index], &play_queue->items[item_index-1]);
            mplayer_selectionmenu_clearmusic_selection(mplayer);
        }
        mplayer->mouse_clicked = false;
    }
}

void mplayer_selectionmenu_handle_movedown(mplayer_t* mplayer, SDL_Rect movebtn) {
    music_queue_t* play_queue = &mplayer->play_queue, *selection_queue = &mplayer->selection_queue;
    if(!mplayer_rect_hover(mplayer, movebtn)) {
        return;
    }
    mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
    if(mplayer->mouse_clicked) {
        size_t item_index = selection_queue->items[0].uid;
        if(item_index < play_queue->item_count-1) {
            if(play_queue->playid == item_index) {
                play_queue->playid = item_index+1;
            } else if(play_queue->playid == item_index+1) {
                play_queue->playid = item_index;
            }
            mplayer_queue_swapitem(&play_queue->items[item_index], &play_queue->items[item_index+1]);
            mplayer_selectionmenu_clearmusic_selection(mplayer);
        }
        mplayer->mouse_clicked = false;
    }
}

void mplayer_selectionmenu_display_removebtn(mplayer_t* mplayer, SDL_Rect* removebtn_ref) {
    SDL_Rect addtobtn_canvas = mplayer->music_selectionmenu_addtocanvas;
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
    mplayer_textmanager_sizetext(mplayer->font, &removebtn_textinfo);
    int max_h = (removebtn_icon.h > removebtn_textinfo.text_canvas.h) ? removebtn_icon.h :
                    removebtn_textinfo.text_canvas.h;
    SDL_Rect full_canvas = {
        .x = addtobtn_canvas.x + addtobtn_canvas.w + 10,
        .y = 0,
        .w = removebtn_icon.w + removebtn_textinfo.text_canvas.w + 20,
        .h = max_h + 20
    };
    full_canvas.y = mplayer->music_selectionmenu.y + (mplayer->music_selectionmenu.h - full_canvas.h) / 2;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(black));
    SDL_RenderDrawRect(mplayer->renderer, &full_canvas);
    SDL_RenderFillRect(mplayer->renderer, &full_canvas);

    removebtn_icon.x = full_canvas.x + 5;
    removebtn_icon.y = full_canvas.y + (full_canvas.h - removebtn_icon.h)/2;
    SDL_Texture* removebtn_icontexture = IMG_LoadTexture(mplayer->renderer, "images/remove 2.png");
    SDL_RenderCopy(mplayer->renderer, removebtn_icontexture, NULL, &removebtn_icon);
    SDL_DestroyTexture(removebtn_icontexture);

    removebtn_textinfo.text_canvas.x = removebtn_icon.x + removebtn_icon.w + 5;
    removebtn_textinfo.text_canvas.y = full_canvas.y + (full_canvas.h -
        removebtn_textinfo.text_canvas.h)/2;
    SDL_Texture* removebtn_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        &removebtn_textinfo);
    SDL_RenderCopy(mplayer->renderer, removebtn_text_texture, NULL, &removebtn_textinfo.text_canvas);
    SDL_DestroyTexture(removebtn_text_texture);
    *removebtn_ref = full_canvas;
}

void mplayer_selectionmenu_handle_removebtn(mplayer_t* mplayer, SDL_Rect removebtn) {
    if(active_tab == SONGS_TAB) {
        return;
    }
    if(mplayer_rect_hover(mplayer, removebtn)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            mplayer->remove_btnclicked = true;
            mplayer->mouse_clicked = false;
        }
    }
}

void mplayer_selectionmenu_display_addbtn(mplayer_t* mplayer, SDL_Rect playnext_btnbackground) {
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
    mplayer_textmanager_sizetext(mplayer->font, &addtobtn_text);
    int max_h = (addbtn_icon.h > addtobtn_text.text_canvas.h) ? addbtn_icon.h : addtobtn_text.text_canvas.h;
    SDL_Rect full_canvas = {
        .x = playnext_btnbackground.x + playnext_btnbackground.w + 10,
        .y = 0,
        .w = addbtn_icon.w + addtobtn_text.text_canvas.w + 15,
        .h = max_h + 10
    };
    full_canvas.y = mplayer->music_selectionmenu.y + (mplayer->music_selectionmenu.h -
        full_canvas.h) / 2;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(black));
    SDL_RenderDrawRect(mplayer->renderer, &full_canvas);
    SDL_RenderFillRect(mplayer->renderer, &full_canvas);

    addbtn_icon.x = full_canvas.x + 5;
    addbtn_icon.y = full_canvas.y + (full_canvas.h - addbtn_icon.h) / 2;
    SDL_Texture* addtobtn_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][music_addtobtn.texture_idx];
    SDL_RenderCopy(mplayer->renderer, addtobtn_texture, NULL, &addbtn_icon);
    
    addtobtn_text.text_canvas.x = addbtn_icon.x + addbtn_icon.w + 5;
    addtobtn_text.text_canvas.y = full_canvas.y + (full_canvas.h - addtobtn_text.text_canvas.h) / 2;
    SDL_Texture* addtobtn_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &addtobtn_text);
    SDL_RenderCopy(mplayer->renderer, addtobtn_text_texture, NULL, &addtobtn_text.text_canvas);
    SDL_DestroyTexture(addtobtn_text_texture); addtobtn_text_texture = NULL;
    mplayer->music_selectionmenu_addtocanvas = full_canvas;
    bool *addtobtn_clicked = &mplayer->music_selectionmenu_addtobtn_clicked;
    if(mplayer_rect_hover(mplayer, full_canvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            *addtobtn_clicked = !(*addtobtn_clicked);
            mplayer->mouse_clicked = false;
        }
    }
}

void mplayer_selectionmenu_handle_addtobtn(mplayer_t* mplayer) {
    if(!mplayer->music_selectionmenu_addtobtn_clicked) {
        return;
    }
    SDL_Rect addtobtn = mplayer->music_selectionmenu_addtocanvas;
    SDL_Color drop_downcolor = mplayer->music_selectionmenu_addto_dropdown_color;
    SDL_Rect drop_downmenu = {
        .x = addtobtn.x - 5,
        .y = addtobtn.y + addtobtn.h + 5,
        .w = 200,
        .h = 100
    };
    mplayer->music_selectionmenu_addto_dropdown = drop_downmenu;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(drop_downcolor));
    SDL_RenderDrawRect(mplayer->renderer, &drop_downmenu);
    SDL_RenderFillRect(mplayer->renderer, &drop_downmenu);

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

    SDL_Texture* playqueue_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &playqueue_btntext);
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
                    [music_playqueuebtn.texture_idx], NULL, &playqueue_btncanvas);
    SDL_RenderCopy(mplayer->renderer, playqueue_text_texture, NULL, &playqueue_btntext.text_canvas);
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
    SDL_Texture* new_playlistbtn_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        &new_playlistbtn_text);
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
                                        [music_addplaylistbtn.texture_idx], NULL, &new_playlistbtn_canvas);
    SDL_RenderCopy(mplayer->renderer, new_playlistbtn_text_texture, NULL, &new_playlistbtn_text.text_canvas);
    SDL_DestroyTexture(new_playlistbtn_text_texture); new_playlistbtn_text_texture = NULL;

    int new_playlistbtn_maxheight = (new_playlistbtn_canvas.h > new_playlistbtn_text.text_canvas.h) ?
                                    new_playlistbtn_canvas.h : new_playlistbtn_text.text_canvas.h;

    SDL_Rect new_playlistbtn_combo = {
        .x = drop_downmenu.x,
        .y = new_playlistbtn_canvas.y,
        .w = drop_downmenu.w,
        .h = new_playlistbtn_maxheight
    };
    if(mplayer_rect_hover(mplayer, playqueue_btncombo)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            printf("You clicked the play queue button within the drop down menu\n");
            music_playqueuebtn.clicked = true;
            mplayer->music_selectionmenu_addtobtn_clicked = false; mplayer->mouse_clicked = false;
        }
    } else if(mplayer_rect_hover(mplayer, new_playlistbtn_combo)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            music_addplaylistbtn.clicked = true;
            printf("You clicked the new playlist button within the drop down menu\n");
            mplayer->music_selectionmenu_addtobtn_clicked = false; mplayer->mouse_clicked = false;
        }
    } else if(mplayer->mouse_clicked && mplayer->music_selectionmenu_addtobtn_clicked) {
        mplayer->music_selectionmenu_addtobtn_clicked = false;
        mplayer->mouse_clicked = false;
    }
}

void mplayer_selectionmenu_handle_playbtn(mplayer_t* mplayer, SDL_Rect playbtn_background) {
    music_queue_t *play_queue = &mplayer->play_queue,
                  *selection_queue = &mplayer->selection_queue;
    if(!mplayer_rect_hover(mplayer, playbtn_background)) {
        return;
    }
    size_t music_listindex = 0, music_id = 0, *playid = &play_queue->playid;
    mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
    if(mplayer->mouse_clicked) {
        printf("You clicked the play button now we are gonna modify the play queue\n");
        if(active_tab == QUEUES_TAB) {
            *playid = selection_queue->items[0].uid;
        } else {
            mplayer_queue_destroy(play_queue);
            if(!mplayer_queue_addmusicfrom_queue(play_queue, selection_queue)) {
                printf("Failed to add the selection queue to the play queue\n");
                Mix_HaltMusic();
                return;
            }
            printf("Successfully added the selection queue to the play queue and play queue now looks like:");
            printf("mplayer->play_queue->item_count = %zu\n", play_queue->item_count);
            mplayer_queue_print(mplayer, *play_queue);
        }
        if(Mix_PlayingMusic() ) {
            Mix_PauseMusic();
            Mix_HaltMusic();
        }
        music_listindex = play_queue->items[*playid].music_listindex,
        music_id = play_queue->items[*playid].music_id;
        if(play_queue->items && !Mix_PlayMusic(mplayer->music_lists[music_listindex]
            [music_id].music, 1)) {
            printf("Playing music %s\n",
                mplayer->music_lists[music_listindex][music_id].music_name
            );
        }
        mplayer_selectionmenu_clearmusic_selection(mplayer);
        mplayer->mouse_clicked = false;
    }
}

void mplayer_selectionmenu_display_addtoplaylist_modal(mplayer_t* mplayer) {
    if(!music_addplaylistbtn.clicked) {
        return;
    }
    SDL_Rect modal = {
        .x = 0, .y = 0,
        .w = 400, .h = 450
    };
    modal.x = (mplayer->win_width - modal.w)/2, modal.y = (mplayer->win_height - modal.h)/2;
    SDL_Color modal_color = {0x21, 0x01, 0x24, 0xFF};
    
    // Render the Add new playlist modal box and its contents / elements
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(modal_color));
    SDL_RenderDrawRect(mplayer->renderer, &modal);
    SDL_RenderFillRect(mplayer->renderer, &modal);

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
    SDL_Texture *newplaylist_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &newplaylist_text);
    SDL_Rect newplaylist_textbgcanvas = {
        .x = modal.x, .y = modal.y,
        .w = modal.w, .h = newplaylist_text.text_canvas.h + 40
    };
    // Draw background for "Add to new playlist" text
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(dark_purple));
    SDL_RenderDrawRect(mplayer->renderer, &newplaylist_textbgcanvas);
    SDL_RenderFillRect(mplayer->renderer, &newplaylist_textbgcanvas);
    SDL_RenderCopy(mplayer->renderer, newplaylist_text_texture, NULL, &newplaylist_text.text_canvas);
    SDL_DestroyTexture(newplaylist_text_texture); newplaylist_text_texture = NULL;

    // Draw a background canvas for the image and render the the new playlist image on the modal
    SDL_Rect newplaylist_imagecanvas = {
        .x = 0, .y = newplaylist_text.text_canvas.y + newplaylist_text.text_canvas.h + 50,
        .w = 140, .h = 140
    };
    SDL_Color newplaylist_imagecanvas_color = {0x00, 0x00, 0x00, 0x00};
    newplaylist_imagecanvas.x = (mplayer->win_width - newplaylist_imagecanvas.w) / 2;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(newplaylist_imagecanvas_color));
    SDL_RenderDrawRect(mplayer->renderer, &newplaylist_imagecanvas);
    SDL_RenderFillRect(mplayer->renderer, &newplaylist_imagecanvas);
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
                    [music_addplaylistbtn.texture_idx], NULL, &newplaylist_imagecanvas);

    mplayer_inputbox_getsize(mplayer, &mplayer->playlist_inputbox);
    mplayer->playlist_inputbox.inputbox_canvas.x = modal.x + (modal.w -
        mplayer->playlist_inputbox.inputbox_canvas.w) / 2,
    mplayer->playlist_inputbox.inputbox_canvas.y = newplaylist_imagecanvas.y + newplaylist_imagecanvas.h + 30;

    text_info_t* placeholder_textinfo = &mplayer->playlist_inputbox.placeholder_info;
    placeholder_textinfo->text_canvas.x = mplayer->playlist_inputbox.inputbox_canvas.x + 10;
    placeholder_textinfo->text_canvas.y = mplayer->playlist_inputbox.inputbox_canvas.y + (mplayer->
        playlist_inputbox.inputbox_canvas.h - placeholder_textinfo->text_canvas.h) / 2;
    mplayer_inputbox_display(mplayer, &mplayer->playlist_inputbox);

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
    SDL_Texture* createbtn_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &createbtn);
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
    SDL_Texture* cancelbtn_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &cancelbtn);
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
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(dark_purple));
    SDL_RenderDrawRect(mplayer->renderer, &button_container);
    SDL_RenderFillRect(mplayer->renderer, &button_container);

    // Render the create button onto the modal
    createbtn_canvas.y = button_container.y + (button_container.h - createbtn_canvas.h) / 2;
    createbtn.text_canvas.y = createbtn_canvas.y + (createbtn_canvas.h - createbtn.text_canvas.h) / 2;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(createbtn_canvascolor));
    SDL_RenderDrawRect(mplayer->renderer, &createbtn_canvas);
    SDL_RenderFillRect(mplayer->renderer, &createbtn_canvas);
    SDL_RenderCopy(mplayer->renderer, createbtn_texture, NULL, &createbtn.text_canvas);
    SDL_DestroyTexture(createbtn_texture); createbtn_texture = NULL;

    // Render the cancel button onto the modal
    cancelbtn_canvas.y = button_container.y + (button_container.h - cancelbtn_canvas.h) / 2;
    cancelbtn.text_canvas.y = cancelbtn_canvas.y + (cancelbtn_canvas.h - cancelbtn.text_canvas.h) / 2;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(cancelbtn_canvascolor));
    SDL_RenderDrawRect(mplayer->renderer, &cancelbtn_canvas);
    SDL_RenderFillRect(mplayer->renderer, &cancelbtn_canvas);
    SDL_RenderCopy(mplayer->renderer, cancelbtn_texture, NULL, &cancelbtn.text_canvas);
    SDL_DestroyTexture(cancelbtn_texture); cancelbtn_texture = NULL;

    mplayer->addtoplaylist_modalcanvases[ADDTOPLAYLIST_MODALCANVAS] = modal,
    mplayer->addtoplaylist_modalcanvases[ADDTOPLAYLIST_MODALCANVAS_CREATEBTN] = createbtn_canvas,
    mplayer->addtoplaylist_modalcanvases[ADDTOPLAYLIST_MODALCANVAS_CANCELBTN] = cancelbtn_canvas;
}
void mplayer_selectionmenu_handle_addtoplaylist_modalevents(mplayer_t* mplayer) {
    if(!music_addplaylistbtn.clicked) {
        return;
    }
    SDL_Rect* modal_canvases = mplayer->addtoplaylist_modalcanvases,
        modal = modal_canvases[ADDTOPLAYLIST_MODALCANVAS],
        createbtn = modal_canvases[ADDTOPLAYLIST_MODALCANVAS_CREATEBTN],
        cancelbtn = modal_canvases[ADDTOPLAYLIST_MODALCANVAS_CANCELBTN];
    // Whenever we click out of the new playlist modal / dialog we will set the new playist button
    // clicked state to true and destroy / clear out the data tha is present within it
    if(!mplayer_rect_hover(mplayer, modal) && mplayer->mouse_clicked) {
        mplayer_inputbox_clear(&mplayer->playlist_inputbox);
        music_addplaylistbtn.clicked = false;
        return;
    }
    // Handle click on create button
    if(mplayer_rect_hover(mplayer, createbtn)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked && mplayer->playlist_inputbox.input.data) {
            char* playlist_name = mplayer->playlist_inputbox.input.data;
            mplayer_playlistmanager_createplaylist(mplayer, playlist_name);
            mplayer_playlistmanager_addmusicselection_toplaylist(mplayer, playlist_name);
            mplayer_inputbox_clear(&mplayer->playlist_inputbox);
            mplayer_selectionmenu_clearmusic_selection(mplayer);
            mplayer->mouse_clicked = false;
            music_addplaylistbtn.clicked = false;
        }
    }
    // Handle click on cancel button
    if(mplayer_rect_hover(mplayer, cancelbtn)) {
        if(mplayer->mouse_clicked) {
            mplayer_inputbox_clear(&mplayer->playlist_inputbox);
            mplayer_selectionmenu_clearmusic_selection(mplayer);
            mplayer->mouse_clicked = false;
            music_addplaylistbtn.clicked = false;
        }
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
    }
}

void mplayer_selectionmenu_display_playbtn(mplayer_t* mplayer, text_info_t songs_selectioninfo,
    SDL_Rect* playbtn_backgroundref) {
    // Render the play button icon within the selection menu
    // This can be use to play musics that were selected using the check boxes
    SDL_Texture* music_listplaybtn_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
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
    mplayer_textmanager_sizetext(mplayer->font, &play_text);
    SDL_Texture* playtext_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &play_text);
    int playbtn_background_height = playbtn_icon.h + 10;
    SDL_Rect playbtn_background = {
        .x = playbtn_icon.x - 5,
        .y = mplayer->music_selectionmenu.y + (mplayer->music_selectionmenu.h - playbtn_background_height)/2,
        .w = play_text.text_canvas.w + playbtn_icon.w + 15,
        .h = playbtn_background_height,
    };
    // Render the full play button background or canvas to the screen
    SDL_SetRenderDrawColor(mplayer->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawRect(mplayer->renderer, &playbtn_background);
    SDL_RenderFillRect(mplayer->renderer, &playbtn_background);

    // Copy the play button onto the renderer
    playbtn_icon.y = playbtn_background.y + (playbtn_background.h - playbtn_icon.h) / 2;
    if(SDL_RenderCopy(mplayer->renderer, music_listplaybtn_texture, NULL,
        &playbtn_icon) < 0) {
        printf("SDL_RenderCopyFailed(): because %s\n", SDL_GetError());
    }
    // Copy the texture for the play button text onto the renderer
    play_text.text_canvas.y = playbtn_background.y + (playbtn_background.h - play_text.text_canvas.h) / 2;
    if(SDL_RenderCopy(mplayer->renderer, playtext_texture, NULL, &play_text.text_canvas)) {
        printf("SDL_RenderCopyFailed(): because %s\n", SDL_GetError());
    }
    SDL_DestroyTexture(playtext_texture); playtext_texture = NULL;
    *playbtn_backgroundref = playbtn_background;
}

void mplayer_selectionmenu_display_playnextbtn(mplayer_t* mplayer, SDL_Rect playbtn_background,
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
    mplayer_textmanager_sizetext(mplayer->font, &playnextbtn_textinfo);
    int playbtn_backgroundheight = playbtn_icon.h + 10;
    SDL_Rect playnextbtn_background = {
        .x = playbtn_background.x + playbtn_background.w + 10,
        .y = mplayer->music_selectionmenu.y + (mplayer->music_selectionmenu.h - playbtn_backgroundheight) / 2,
        .w = playnextbtn_textinfo.text_canvas.w + playbtn_icon.w + 15,
        .h = playbtn_backgroundheight
    };

    SDL_Texture* playnextbtntext_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        &playnextbtn_textinfo);

    // Set the background color for the play button icon
    SDL_SetRenderDrawColor(mplayer->renderer, black.r, black.g, black.b, 34);
    SDL_RenderDrawRect(mplayer->renderer, &playnextbtn_background);
    SDL_RenderFillRect(mplayer->renderer, &playnextbtn_background);

    // Copy the play button texture to its canvas
    playbtn_icon.x = playnextbtn_background.x + 5;
    playbtn_icon.y = playnextbtn_background.y + (playnextbtn_background.h - playbtn_icon.h) / 2;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
        [music_listplaybtn.texture_idx], NULL, &playbtn_icon);

    // Copy the rendered play next button text to its canvas
    playnextbtn_textinfo.text_canvas.x = playbtn_icon.x + playbtn_icon.w + 5;
    playnextbtn_textinfo.text_canvas.y = playnextbtn_background.y + (playnextbtn_background.h
        - playnextbtn_textinfo.text_canvas.h) / 2;
    SDL_RenderCopy(mplayer->renderer, playnextbtntext_texture, NULL, &playnextbtn_textinfo.text_canvas);
    SDL_DestroyTexture(playnextbtntext_texture); playnextbtntext_texture = NULL;
    *playnext_backgroundref = playnextbtn_background;
}

void mplayer_selectionmenu_handle_playnextbtn(mplayer_t* mplayer, SDL_Rect playnext_btncanvas) {
    if(mplayer_rect_hover(mplayer, playnext_btncanvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            bool playqueue_wasempty = !mplayer->play_queue.items;
            mplayer_queue_addmusicqueue_toplaynext(mplayer, &mplayer->play_queue, &mplayer->selection_queue);
            if(!Mix_PlayingMusic() && playqueue_wasempty) {
                mplayer->play_queue.playid = 0;
                size_t playid = 0, music_listindex = mplayer->play_queue.items[playid].music_listindex,
                       music_id = mplayer->play_queue.items[playid].music_id;
                Mix_PlayMusic(mplayer->music_lists[music_listindex][music_id].music, 1);
            }
            mplayer_selectionmenu_clearmusic_selection(mplayer);
            mplayer->checkall_btntoggled = true;
        }
    }
}

void mplayer_selectionmenu_display_songselectioninfo(mplayer_t* mplayer, text_info_t* songs_selectioninfo_ref) {
    // The amount of digits that makes up the number of selected songs
    int songs_selectioncount_length = 0;
    size_t music_selection_tickcount = mplayer->tick_count;
    if(mplayer->music_selectionmenu_checkbox_tickall) {
        if(active_tab == SONGS_TAB) {
            music_selection_tickcount = mplayer->music_count;
        } else if(active_tab) {
            music_selection_tickcount = mplayer->play_queue.item_count;
        }
    }
    // Convert the number of selected songs into a string
    char* songs_selectioncount_text = mplayer_size_t_tostring(music_selection_tickcount,
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
    (mplayer->tick_count > 1) ? strcat(songs_selectioninfo_text, "s"): 0;
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
    mplayer_textmanager_sizetext(mplayer->font, &songs_selectioninfo);
    songs_selectioninfo.text_canvas.y = mplayer->music_selectionmenu.y + (mplayer->music_selectionmenu.h -
        songs_selectioninfo.text_canvas.h) / 2;

    SDL_Texture* songs_selectioninfo_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &songs_selectioninfo);
    SDL_RenderCopy(mplayer->renderer, songs_selectioninfo_texture, NULL, &songs_selectioninfo.text_canvas);
    SDL_DestroyTexture(songs_selectioninfo_texture); songs_selectioninfo_texture = NULL;
    *songs_selectioninfo_ref = songs_selectioninfo;
    free(songs_selectioncount_text); songs_selectioncount_text = NULL;
    free(songs_selectioninfo_text); songs_selectioninfo_text = NULL;
}

bool mplayer_selectionmenu_toggleitem_checkboxmusic_queue(mplayer_t* mplayer, music_queue_t* queue,
    size_t item_index) {
    size_t i = item_index, music_listindex = queue->items[i].music_listindex,
        music_id = queue->items[i].music_id;
    if(mplayer->music_selectionmenu_checkbox_tickall && !queue->items[i].checkbox_ticked) {
        queue->items[i].fill = true;
        queue->items[i].checkbox_ticked = true;
        mplayer_queue_addmusic(&mplayer->selection_queue, item_index, music_listindex, music_id);
        mplayer->music_selected = true;
        if(mplayer->tick_count < queue->item_count) {
            mplayer->tick_count++;
        }
        return true;
    } else if(mplayer->music_selectionmenu_checkbox_clicked &&
        !mplayer->music_selectionmenu_checkbox_tickall && queue->items[i].checkbox_ticked) {
        queue->items[i].fill = false;
        queue->items[i].checkbox_ticked = false;
        mplayer_queue_removemusicby_uid(&mplayer->selection_queue, item_index);
        mplayer->music_selected = true;
        mplayer->tick_count--;
        if(!mplayer->tick_count) {
            mplayer->music_selectionmenu_checkbox_clicked = false;
        }
        return true;
    }
    return false;

}

bool mplayer_selectionmenu_togglesong_checkbox(mplayer_t* mplayer, music_t* music_list, size_t music_checkbox_index) {
    size_t i = music_checkbox_index;
    if(mplayer->music_selectionmenu_checkbox_tickall && !music_list[i].checkbox_ticked) {
        music_list[i].fill = true;
        music_list[i].checkbox_ticked = true;
        mplayer_queue_addmusic(&mplayer->selection_queue, 0, 0, i);
        mplayer->music_selected = true;
        if(mplayer->tick_count < mplayer->music_count) {
            mplayer->tick_count++;
        }
        return true;
    } else if(mplayer->music_selectionmenu_checkbox_clicked &&
        !mplayer->music_selectionmenu_checkbox_tickall && music_list[i].checkbox_ticked) {
        music_list[i].fill = false;
        music_list[i].checkbox_ticked = false;
        mplayer_queue_removemusicby_musiclistidx_id(&mplayer->selection_queue, 0, i);
        mplayer->music_selected = true;
        mplayer->tick_count--;
        if(!mplayer->tick_count) {
            mplayer->music_selectionmenu_checkbox_clicked = false;
        }
        return true;
    }
    return false;
}

bool mplayer_selectionmenu_togglecheckbox(mplayer_t* mplayer, music_queue_t* queue, size_t queue_itemindex,
    music_t* music_list, size_t music_checkbox_index) {
    bool ret_val = false;
    if(active_tab == SONGS_TAB) {
        ret_val = mplayer_selectionmenu_togglesong_checkbox(mplayer, music_list, music_checkbox_index);
    } else if(active_tab == QUEUES_TAB) {
        ret_val = mplayer_selectionmenu_toggleitem_checkboxmusic_queue(mplayer, queue, queue_itemindex);
    }
    return ret_val;
}

void mplayer_selectionmenu_clearmusic_selection(mplayer_t* mplayer) {
    for(size_t i=0;i<mplayer->play_queue.item_count;i++) {
        mplayer->play_queue.items[i].fill = false;
        mplayer->play_queue.items[i].checkbox_ticked = false;
    }
    if(active_tab == QUEUES_TAB) {
        mplayer->songsbox_resized = true;
    }
    for(size_t i=0;i<mplayer->selection_queue.item_count;i++) {
        size_t music_listindex = mplayer->selection_queue.items[i].music_listindex,
        music_id = mplayer->selection_queue.items[i].music_id;
        mplayer->music_lists[music_listindex][music_id].fill = false;
        mplayer->music_lists[music_listindex][music_id].checkbox_ticked = false;
    }

    mplayer->tick_count = 0;
    mplayer->music_selectionmenu_checkbox_fillall = false;
    mplayer->music_selectionmenu_checkbox_tickall = false;
    mplayer->music_selectionmenu_checkbox_clicked = false;
    free(mplayer->selection_queue.items); mplayer->selection_queue.items = NULL;
    mplayer->selection_queue.item_count = 0;
}

void mplayer_selectionmenu_handle_checkallbtn_toggleoption(mplayer_t* mplayer, text_info_t songs_selectioninfo) {
    if(mplayer->music_selectionmenu_addtobtn_clicked) {
        return;
    }
    bool *fill = &mplayer->music_selectionmenu_checkbox_fillall,
         *tick = &mplayer->music_selectionmenu_checkbox_tickall;
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

    size_t target_count = (active_tab == SONGS_TAB) ? mplayer->music_count
                        : (active_tab == QUEUES_TAB) ? mplayer->play_queue.item_count : 0;
    
    if(mplayer_rect_hover(mplayer, music_selectionmenu_togglecanvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        *fill = true;
        if(mplayer->mouse_clicked) {
            if(*tick) {
                *fill = false, *tick = false;
            } else {
                *tick = true;
            }
            mplayer->checkall_btntoggled = true;
            mplayer->music_selectionmenu_checkbox_clicked = true;
        }
    } else if(!mplayer->music_selectionmenu_checkbox_clicked && mplayer->tick_count == target_count) {
        *fill = true, *tick = true;
    } else {
        *fill = false, *tick = false,
        mplayer->music_selectionmenu_checkbox_clicked = false;
    }
}

void mplayer_selectionmenu_display_checkallbtn(mplayer_t* mplayer) {
    bool *fill = &mplayer->music_selectionmenu_checkbox_fillall,
         *tick = &mplayer->music_selectionmenu_checkbox_tickall;
    SDL_Color box_color = {0xff, 0xff, 0xff, 0xff}, tick_color = {0x00, 0xff, 0x00, 0xff},
    fill_color = {0xFF, 0xA5, 0x00, 0xff};

    SDL_SetRenderDrawColor(mplayer->renderer, 0x0E, 0x34, 0xA0, 0xFF);
    SDL_RenderDrawRect(mplayer->renderer, &mplayer->music_selectionmenu);
    SDL_RenderFillRect(mplayer->renderer, &mplayer->music_selectionmenu);

    // Calculate the positions and the height of the checkbox
    music_selectionmenu_checkbox_size.x = mplayer->music_selectionmenu.x + 10;
    music_selectionmenu_checkbox_size.h = music_listplaybtn.btn_canvas.h;
    music_selectionmenu_checkbox_size.y = mplayer->music_selectionmenu.y +
                                        ((mplayer->music_selectionmenu.h - music_selectionmenu_checkbox_size.h)/2);

    mcheckbox_t checkbox_info = {0};
    checkbox_info.checkbox_canvas = music_selectionmenu_checkbox_size;
    checkbox_info.box_color = box_color;
    checkbox_info.tk_color = tick_color;
    checkbox_info.tick = *tick;
    checkbox_info.fill = *fill;
    mplayer_drawcheckbox(mplayer, &checkbox_info);
}