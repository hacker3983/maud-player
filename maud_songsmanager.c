#include "maud_songsmanager.h"
#include "maud_notification.h"

void maud_songsmanager_songstab_rendersongs(maud_t* maud) {
    music_t* music_list = maud->music_list;
    size_t music_count = maud->music_count;
    if(!music_count || !music_list) {
        return;
    }
    SDL_Rect outer_canvas = {.x = songs_box.x + 2, .y = songs_box.y + 1},
             end_canvas = outer_canvas;
    text_info_t music_name = {0};
    size_t end_renderpos = 0;
    for(size_t i=maud->music_renderpos;i<music_count;i++) {
        end_canvas.w = maud->win_width - scrollbar.w - 5;
        end_canvas.h = music_list[i].text_info.text_canvas.h + 22;
        end_renderpos = i+1;
        if(end_canvas.y + end_canvas.h >= songs_box.y + songs_box.h) {
            break;
        }
        end_canvas.y += end_canvas.h + 3;
    }
    bool decrease_y = false;
    if(maud->scroll && maud_rect_hover(maud, songs_box)) {
        switch(maud->scroll_type) {
            case MAUDSCROLL_DOWN:
                if(end_renderpos < music_count) {
                    maud->music_renderpos++;
                }
                break;
            case MAUDSCROLL_UP:
                if(maud->music_renderpos) {
                    maud->music_renderpos--;
                }
                break;
        }
        maud->scroll = false;
    }
    if(end_renderpos == music_count &&
        end_canvas.y + end_canvas.h > songs_box.y + songs_box.h) {
                decrease_y = true;    
    }
    SDL_Rect clip_area = songs_box;
    clip_area.h--;
    SDL_RenderSetClipRect(maud->renderer, &clip_area);
    for(size_t i=maud->music_renderpos;i<music_count;i++) {
        outer_canvas.w = maud->win_width - scrollbar.w - 5;
        outer_canvas.h = music_list[i].text_info.text_canvas.h + 22;
        if(maud_selectionmenu_togglesong_checkbox(maud, music_list, i)) {
            continue;
        }
        if(decrease_y) {
            outer_canvas.y -= (end_canvas.y + end_canvas.h) - (songs_box.y + songs_box.h - 1);
            decrease_y = false;
        }
        music_list[i].outer_canvas = outer_canvas;
        music_list[i].text_info.text_canvas.x = outer_canvas.x + 50;
        music_list[i].text_info.text_canvas.y = outer_canvas.y + (outer_canvas.h -
            music_list[i].text_info.text_canvas.h) / 2;
        music_name = music_list[i].text_info;
        maud_songsmanager_rendersong_canvas(maud, music_list, i);
        SDL_Color box_color = {0xff, 0xff, 0xff, 0xff}, tick_color = {0x00, 0xff, 0x00, 0xff},
                  fill_color = {0};
        maud_checkbox_t* checkbox = &maud->music_checkbox;
        checkbox->canvas.x = outer_canvas.x + 5;
        checkbox->canvas.w = checkbox_size.w;
        checkbox->canvas.h = outer_canvas.h - 10;
        checkbox->canvas.y = outer_canvas.y + (outer_canvas.h -
            checkbox->canvas.h) / 2;
        if(!maud->tick_count) {
            music_listplaybtn.btn_canvas.w = 30, music_listplaybtn.btn_canvas.h = outer_canvas.h - 10;
            music_listplaybtn.btn_canvas.x = (checkbox->canvas.x + checkbox->canvas.w) + 20;
            music_listplaybtn.btn_canvas.y = checkbox->canvas.y;
        }
        maud_songsmanager_handlesong_playbutton_hover(maud, outer_canvas);
        maud_songsmanager_handleprevbutton(maud);
        maud_songsmanager_handleskipbutton(maud);
        maud_songsmanager_handlesongselection(maud, music_list, i, &music_list[i].text_info);
        if(maud->tick_count) {
            // whenever any checkbox is ticked and the current music we are current music we are rendering is not ticked
            // we ensure that we set the checkbox fill state to false before drawing its checkbox
            if(!music_list[i].checkbox_ticked && !maud_music_hover(maud, i)) {
                music_list[i].fill = false;
            }
            maud_checkbox_drawmusic_checkbox(maud, box_color, fill_color, music_list[i].fill, tick_color,
                music_list[i].checkbox_ticked);
        } else if(maud_music_hover(maud, i)) {
            // whenever no checkbox is ticked and we hover over the music we display the check box
            // so that the user can click it or select it
            if(maud_checkbox_hovered(maud)) {
                maud_checkbox_drawmusic_checkbox(maud, box_color, fill_color, music_list[i].fill, tick_color,
                    music_list[i].checkbox_ticked);
            } else if(!maud_checkbox_hovered(maud)) {
                maud_checkbox_drawmusic_checkbox(maud, box_color, fill_color, false, tick_color, false);
            }
        }
        SDL_RenderCopy(maud->renderer, music_list[i].text_texture, NULL,
            &music_list[i].text_info.text_canvas);
        if(outer_canvas.y + outer_canvas.h >= songs_box.y + songs_box.h) {
            break;
        }
        outer_canvas.y += outer_canvas.h + 3;
    }
    SDL_RenderSetClipRect(maud->renderer, NULL);
    // scroll bar related information
    maud_scrollbar_t songsbox_scrollbar = {
        .rect = scrollbar,
        .displacement = 0.0,
        .orientation = 0,
        .start_pos = maud->music_renderpos,
        .final_pos = music_count,
        .padding_x = -2,
        .padding_y = -(2 + (int)((double)scrollbar.h / 2.0)),
        .scroll_area = songs_box
    };
    maud->scroll = false;
    maud_renderscroll_bar(maud, &songsbox_scrollbar, end_renderpos - maud->music_renderpos);
}

void maud_songsmanager_rendersong_canvas(maud_t* maud, music_t* music_list, size_t music_id) {
    SDL_SetRenderDrawColor(maud->renderer, 0, 42, 50, 0xFF /*0x3B, 0x35, 0x61, 0xFF*/);
    SDL_RenderDrawRect(maud->renderer, &music_list[music_id].outer_canvas);
    SDL_RenderFillRect(maud->renderer, &music_list[music_id].outer_canvas);
}

void maud_songsmanager_handlesong_playbutton_hover(maud_t* maud, SDL_Rect outer_canvas) {
    SDL_Rect hoverbg_canvas = {0};
    // check if we hover over the play button and we haven't ticked any checkbox
    if(maud_musiclist_playbutton_hover(maud) && !maud->music_selectionmenu_addtobtn_clicked
        && !maud->tick_count) {
        hoverbg_canvas = music_listplaybtn.btn_canvas;
        hoverbg_canvas.x -= 5, hoverbg_canvas.w += 5;
        hoverbg_canvas.h = outer_canvas.h, hoverbg_canvas.y = outer_canvas.y;
        SDL_SetRenderDrawColor(maud->renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderDrawRect(maud->renderer, &hoverbg_canvas);
        SDL_RenderFillRect(maud->renderer, &hoverbg_canvas);
    }
}

void maud_songsmanager_addplayback_error(maud_t* maud, const char* music_name) {
    size_t msg_len = 112 + strlen(music_name);
    char msg_buff[msg_len+1];
    sprintf(msg_buff,
        "The selected music %s cannot be played because it is either a corrupted, "
        "unsupported file format or file extension.",
        music_name
    );
    msg_buff[msg_len] = '\0';
    maud_notification_push(&maud->notification, maud->font, 20,
        (SDL_Color){0x12, 0x12, 012, 0x12},
        msg_buff,
        white,
        2,
        20,
        20,
        10
    );
}

void maud_songsmanager_handleplaybutton(maud_t* maud, music_t* music_list, size_t music_id) {
    maud_queue_t* play_queue = &maud->play_queue;
    size_t prev_playid = play_queue->playid;
    if(maud_musiclist_playbutton_hover(maud) && !music_addplaylistbtn.clicked &&
        !maud->music_selectionmenu_addtobtn_clicked
        && !maud->tick_count) {
        if(maud->mouse_clicked) {
            // Whenever we click the play button for a particular music we play the current play queue and
            // add the new music ids

            maud_queue_destroy(play_queue);
            play_queue->playid = music_id;
            for(size_t i=0;i<maud->music_count;i++) {
                    maud_queue_addmusic(play_queue, 0, 0, i);
            }
            /* whenever we hover over the playbutton on the music
               we determine if we should restart the current playing music or play a new music
            */
            if(Mix_PlayingMusic() && prev_playid == music_id) {
                Mix_SetMusicPosition(0);
                if(Mix_PausedMusic()) {
                    Mix_ResumeMusic();
                }
            } else {
                if(Mix_PlayingMusic()) {
                    Mix_HaltMusic();
                }
                if(Mix_PlayMusic(maud->music_list[music_id].music, 1) == -1) {
                    printf("Failed to play music\n");
                    maud_songsmanager_addplayback_error(maud, maud->music_list[music_id].music_name);
                }
            }
            maud->mouse_clicked = false;
        }
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
    }
}


void maud_songsmanager_handlesongselection(maud_t* maud, music_t* music_list, size_t music_id, text_info_t* music_textinfo) {
    if(!music_addplaylistbtn.clicked && !maud->music_selectionmenu_addtobtn_clicked
        && maud_music_hover(maud, music_id)
            /*&& (maud->music_searchresult || !maud->musicsearchbar_data)*/) {
        maud_songsmanager_handlecheckbox_musicselection(maud, music_list, music_id);
        maud_songsmanager_handleplaybutton(maud, music_list, music_id);
        maud->menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_listplaybtn.texture_idx] =
            music_listplaybtn.btn_canvas;
        if(!maud->tick_count) {
            // if we haven't ticked any checkbox then we render the music list play button to the screen
            music_textinfo->text_canvas.x += music_listplaybtn.btn_canvas.w + 20;
            SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE]
                [music_listplaybtn.texture_idx], NULL, &music_listplaybtn.btn_canvas);
        }
    }
}

void maud_songsmanager_handlecheckbox_musicselection(maud_t* maud, music_t* music_list, size_t music_id) {
    // check if the mouse is hovered over the music
    if(maud_checkbox_hovered(maud)) {
        if(maud->mouse_clicked) {
            // if we are in the position of the checkbox and we clicked it
            switch(music_list[music_id].checkbox_ticked) {
                case false:
                    /* whenever the checkbox isn't already ticked then we set checkbox as ticked
                       and the fill color for it as true
                    */
                    music_list[music_id].fill = true;
                    music_list[music_id].checkbox_ticked = true;
                    maud_queue_addmusic(&maud->selection_queue, 0, 0, music_id);
                    maud->tick_count++;
                    break;
                case true:
                    /* whenever the checkbox is already ticked then we set the checkbox as not ticked
                       and the fill equal to false
                    */
                    music_list[music_id].fill = false;
                    music_list[music_id].checkbox_ticked = false;
                    maud_queue_removemusicby_musiclistidx_id(&maud->selection_queue, 0, music_id);
                    maud->tick_count--;
                    if(!maud->tick_count && maud->music_selectionmenu_checkbox_tickall) {
                        maud->music_selectionmenu_checkbox_tickall = false;
                    }
                    break;
            }
            maud->music_selected = true;
                maud->mouse_clicked = false;
        } else {
            /* if we just hover over the checkbox without clicking it then we set that checkbox
                fill equal to true
            */
            music_list[music_id].fill = true;
        }
        maud_setcursor(maud, MAUD_CURSOR_POINTER);
    } else if(maud->tick_count && maud->mouse_clicked && !music_addplaylistbtn.clicked &&
        !maud->music_selectionmenu_addtobtn_clicked) {
        // whenever we click the music without clicking any of its elements
        // we set clicked equal to false to prevent it from performing any action that we do not want
        if(music_list[music_id].checkbox_ticked) {
            maud->music_selected = true;
            maud->tick_count--;
            if(!maud->tick_count && maud->music_selectionmenu_checkbox_tickall) {
                maud->music_selectionmenu_checkbox_tickall = false;
            }
            maud_queue_removemusicby_musiclistidx_id(&maud->selection_queue, 0, music_id);
            music_list[music_id].fill = false;
            music_list[music_id].checkbox_ticked = false;
        } else if(maud->tick_count) {
            maud->music_selected = true;
            maud_queue_addmusic(&maud->selection_queue, 0, 0, music_id);
            maud->tick_count++;
            music_list[music_id].fill = true;
            music_list[music_id].checkbox_ticked = true;
        }
        maud->mouse_clicked = false;
    }
}

void maud_songsmanager_handleskipbutton(maud_t* maud) {
    maud_queue_t* play_queue = &maud->play_queue;
    if(!(music_btns[MAUD_SKIPBTN].clicked && play_queue->items)) {
        return;
    }
    size_t *playid = &play_queue->playid,
           music_listindex = play_queue->items[*playid].music_listindex,
           music_id = play_queue->items[*playid].music_id;
    music_t **music_lists = maud->music_lists, *music_list = music_lists[music_listindex];
    (*playid)++;
    (*playid) %= play_queue->item_count;
    maud_songsmanager_togglemusic_playback(maud);
    music_btns[MAUD_SKIPBTN].clicked = false;
}

void maud_songsmanager_handleprevbutton(maud_t* maud) {
    maud_queue_t* play_queue = &maud->play_queue;
    if(!(music_btns[MAUD_PREVBTN].clicked && play_queue->items)) {
        return;
    }
    // Select the play id
    size_t *playid = &play_queue->playid,
           music_listindex = play_queue->items[*playid].music_listindex,
           music_id = play_queue->items[*playid].music_id;
    music_t **music_lists = maud->music_lists,
            *music_list = music_lists[music_listindex];

    if(*playid) {
        (*playid)--;
    }
    // Halt any current music playing and play the actual music when we press the play button
    // Depending on whether it is paused or not
    maud_songsmanager_togglemusic_playback(maud);
    music_btns[MAUD_PREVBTN].clicked = false;
}

int maud_songsmanager_playmusic(maud_t* maud) {
    maud_queue_t* play_queue = &maud->play_queue;
    if(!play_queue->items) {
        return -1;
    }
    size_t *playid = &play_queue->playid,
           music_listindex = play_queue->items[*playid].music_listindex,
           music_id = play_queue->items[*playid].music_id;
    music_t **music_lists = maud->music_lists,
            *music_list = music_lists[music_listindex];
    if(!music_list[music_id].music) {
        size_t msg_len = 120 + strlen(music_list[music_id].music_name);
        char msg_buff[msg_len+1];
        sprintf(msg_buff,
            "The selected music %s cannot be played this is because"
            "it is either corrupted, an unsupported file format or file extension.",
            music_list[music_id].music_name
        );
        msg_buff[msg_len] = '\0';
        maud_notification_push(&maud->notification, maud->font, 20,
            (SDL_Color){0x12, 0x12, 012, 0x12},
            msg_buff,
            white,
            2,
            20,
            20,
            10
        );
    }
    return Mix_PlayMusic(music_list[music_id].music, 1);
}

void maud_songsmanager_playmusic_pause(maud_t* maud) {
    if(!maud_songsmanager_playmusic(maud)) {
        Mix_PauseMusic();
    }
}

void maud_songsmanager_togglemusic_playback(maud_t* maud) {
    maud_queue_t* play_queue = &maud->play_queue;
    if(!play_queue->items) {
        return;
    }
    size_t *playid = &play_queue->playid,
           music_listindex = play_queue->items[*playid].music_listindex,
           music_id = play_queue->items[*playid].music_id;
    music_t **music_lists = maud->music_lists,
            *music_list = music_lists[music_listindex];
    Mix_HaltMusic();
    if(!Mix_PausedMusic()) {
        if(!maud_songsmanager_playmusic(maud)) {
            printf("Playing music %s\n", music_list[music_id].music_name);
        }
        return;
    }
    maud_songsmanager_playmusic_pause(maud);
}