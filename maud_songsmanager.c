#include "maud_songsmanager.h"
#include "maud_notification.h"

void maud_songsmanager_songstab_rendersongs(maud_t* maud) {
    text_info_t utext = {14, NULL, NULL, white, {songs_box.x + 2, songs_box.y + 1}};
    int default_w = 0, default_h = 0;
    SDL_Rect outer_canvas = utext.text_canvas;
    music_t* music_list = maud->music_list;
    size_t music_count = maud->music_count, max_renderpos = maud->music_maxrenderpos,
           *music_renderpos = &maud->music_renderpos;
    /*if(maud->musicsearchbar_data && maud->music_searchresult &&
        maud->music_searchresult_count < maud->music_searchresult_indicescount) {
        music_list = maud->music_searchresult;
        music_count = maud->music_searchresult_count;
        music_renderpos = &maud->music_searchrenderpos;
        max_renderpos = maud->match_maxrenderpos;
    }*/
    // if the music_count is zero then we exit the render songs function
    if(!music_count) {
        return;
    }    
    if(!music_list) {
        return;
    }
    /*  get the size of a character so we can determine the maximum amount of textures
        we can render with in the songs box
    */
    TTF_SetFontSize(maud->music_font, utext.font_size);
    TTF_SizeUTF8(maud->music_font, "A", &utext.text_canvas.w, &utext.text_canvas.h);
    default_w = utext.text_canvas.w, default_h = utext.text_canvas.h + 22;
    // calculation for the scrollability
    int def_outerheight = 22;
    if(songs_box.h < def_outerheight) {
        def_outerheight = songs_box.h;
    }
    SDL_Rect prev_canvas = {0};
    size_t max_textures = 0, music_rendercount = 0, *music_renderlist = NULL, max_rendercount = 0;
    
    float max_textures_precision = roundf((float)songs_box.h /
        ((float)music_list[0].text_info.text_canvas.h + (float)def_outerheight));
    // Ensure that the max_textures_precision is not negative to prevent crashing
    if(max_textures_precision > 0) {
        max_textures = (size_t)max_textures_precision;
        max_rendercount = max_textures;
        music_renderlist = calloc(max_rendercount+1, sizeof(size_t));
        prev_canvas = music_list[0].outer_canvas;
        prev_canvas.x = songs_box.x + 2, prev_canvas.y = songs_box.y + 1;
        music_list[0].outer_canvas.y = songs_box.y + 1;
    }
    /* Ensure that the renderlist is not NULL. this happens whenever the max textures or max_textures precision
       is less than equal to zero
    */
    if(!music_renderlist) {
        return;
    }
    for(size_t i=0;i<music_count;i++) {
        utext = music_list[i].text_info;
        outer_canvas = music_list[i].outer_canvas;
        default_h = utext.text_canvas.h, default_w = utext.text_canvas.w;
        if(music_list[i].search_match && !maud->search_inputbox.input.data) {
            music_list[i].search_match = false;
            music_list[i].search_render = false;
            if(songs_box.h < utext.text_canvas.h + def_outerheight) {
                music_list[i].outer_canvas.h = songs_box.h;
            } else {
                music_list[i].outer_canvas.h = utext.text_canvas.h + def_outerheight;
            }
        }
        SDL_Rect temp_canvas = music_list[i].outer_canvas;
        if(maud_selectionmenu_togglesong_checkbox(maud, music_list, i)) {
            continue;
        }
        if(def_outerheight < 22 && music_list[i].text_info.text_canvas.h + def_outerheight < temp_canvas.h) {
            music_list[i].outer_canvas.h = music_list[i].text_info.text_canvas.h + def_outerheight;
        }
        /*if(maud->musicsearchbar_data && !music_list[i].search_render && maud->music_searchresult) { continue; }
        else*/
        if(!music_list[i].render /*&& !maud->musicsearchbar_data*/) { continue; }
        // set the calculated x, y position and other related info for the music text and outer canvas
        music_list[i].outer_canvas.x = prev_canvas.x,
        music_list[i].outer_canvas.y = prev_canvas.y;
        music_list[i].outer_canvas.w = maud->win_width - scrollbar.w - 5;
        music_list[i].text_info.text_canvas.y = outer_canvas.y +
            ((music_list[i].outer_canvas.h -
            utext.text_canvas.h) / 2);

        outer_canvas = music_list[i].outer_canvas;
        utext = music_list[i].text_info;
        if(music_rendercount <= max_rendercount) {
            music_renderlist[music_rendercount] = i;
            // Handling scroll events
            if(!music_addplaylistbtn.clicked && !maud->music_selectionmenu_addtobtn_clicked && maud->scroll
                && maud_rect_hover(maud, songs_box) /*&& (maud->music_searchresult ||
                    !maud->musicsearchbar_data)*/) {
                size_t index = music_renderlist[music_rendercount];
                bool *music_renderstatus = /*(maud->music_searchresult) ? &music_list[i].search_render :*/
                                        &music_list[i].render;
                switch(maud->scroll_type) {
                    case MAUDSCROLL_DOWN:
                        int musicendpos_y = (music_list[max_renderpos].outer_canvas.y +
                        music_list[max_renderpos].text_info.text_canvas.h + def_outerheight);
                        if(max_renderpos - index >= max_textures && index < max_renderpos) {
                            music_list[index].outer_canvas.h -= 8;
                            if(music_list[index].outer_canvas.h <= (def_outerheight/2)) {
                                music_list[index].outer_canvas.h = 0;
                                *music_renderstatus = false;
                                (*music_renderpos)++;
                            }
                        } else if(max_renderpos - index >= max_textures && music_list[index].render
                            && music_list[index].outer_canvas.h <= (def_outerheight/2) && index < max_renderpos) {
                            // if the scroll up event occured before and the height of the outercanvas is zero
                            // then we set that particular music render status to be false
                            *music_renderstatus = false;
                            (*music_renderpos)++;
                        } else if(max_renderpos - index < max_textures && music_count >= max_textures
                            && musicendpos_y >= songs_box.y + songs_box.h && index < max_renderpos) {
                            // whenever we are close to the last texture in which the max_renderpos - index is less
                            // than the maximum amount of textures then we decrement the height of the texture at the
                            // top until it is less than or equal to zero
                            //printf("Decrementation 2\n");
                            music_list[index].outer_canvas.h -= 8;
                            if(music_list[index].outer_canvas.h <= (def_outerheight/2)) {
                                music_list[index].outer_canvas.h = 0;
                                *music_renderstatus = false;
                                (*music_renderpos)++;
                            }
                        }
                        break;
                    case MAUDSCROLL_UP:
                        if(index >= 0) {
                            music_list[index].outer_canvas.h += 8;
                            if(music_list[index].outer_canvas.h >= def_outerheight) {
                                music_list[index].outer_canvas.h = default_h + def_outerheight;
                                if(index > 0) {
                                    index--; (*music_renderpos)--;
                                    music_renderstatus = /*(maud->music_searchresult) ? &music_list[index].search_render :*/
                                        &music_list[index].render;
                                }
                            }
                            *music_renderstatus = true;
                        }
                        break;
                }
                maud->scroll = false;
            }
            size_t next_index = (i < max_renderpos - 1) ? i+1 : i;
            SDL_Rect next_outercanvas = music_list[next_index].outer_canvas;
            SDL_Rect next_textcanvas = music_list[next_index].text_info.text_canvas;
            int musicendpos_y = (outer_canvas.y + utext.text_canvas.h + def_outerheight),
                nextendpos_y = next_outercanvas.y + next_textcanvas.h + def_outerheight;
            // calculate the amount we should decrease the original height of the outercanvas by to let it fit within
            // the songs box extra_h
            int extra_h = musicendpos_y - songs_box.y - songs_box.h + 5;
            // Whenever the current music position i is less than the maximum render position and the last music position
            // at the end is greater than the height of the songs box then we adjust the height to let it fit directly
            // within the songs box
            if(i <= max_renderpos && musicendpos_y > songs_box.y + songs_box.h) {
                if(music_rendercount == max_rendercount) {
                    extra_h -= 4;
                }
                if(i > 0) {
                    music_list[i].outer_canvas.h = utext.text_canvas.h + def_outerheight - extra_h;
                }
                music_list[i].fit = false;
                //music_list[i].fit = false;
            } else if(!music_list[i].fit && musicendpos_y < songs_box.y + songs_box.h &&
                def_outerheight == 22) {
                if(music_list[i].outer_canvas.y + utext.text_canvas.h + 22 < songs_box.y + songs_box.h) {
                        music_list[i].outer_canvas.h = utext.text_canvas.h + def_outerheight;
                }
                music_list[i].fit = true;
            }
            maud_songsmanager_rendersong_canvas(maud, music_list, music_renderlist[music_rendercount]);
            SDL_Color box_color = {0xff, 0xff, 0xff, 0xff}, tick_color = {0x00, 0xff, 0x00, 0xff},
            fill_color = {0}/*{0xFF, 0xA5, 0x00, 0xff}*/;
            checkbox_size.x = outer_canvas.x+5;
            checkbox_size.h = outer_canvas.h-10;
            checkbox_size.y = outer_canvas.y + ((outer_canvas.h - checkbox_size.h)/2);
            // Do not modify the x, y, width, and height of the music list play btn canvas whenever we selected music
            if(!maud->tick_count) {
                music_listplaybtn.btn_canvas.w = 30, music_listplaybtn.btn_canvas.h = outer_canvas.h - 10;
                music_listplaybtn.btn_canvas.x = (checkbox_size.x + checkbox_size.w) + 20,
                music_listplaybtn.btn_canvas.y = checkbox_size.y;
            }
            int mouse_x = maud->mouse_x, mouse_y = maud->mouse_y;
            maud_songsmanager_handlesong_playbutton_hover(maud, outer_canvas);
            maud_songsmanager_handleprevbutton(maud);
            maud_songsmanager_handleskipbutton(maud);
            maud_songsmanager_handlesongselection(maud, music_list, i, &utext);
            if(maud->tick_count) {
                // whenever any checkbox is ticked and the current music we are current music we are rendering is not ticked
                // we ensure that we set the checkbox fill state to false before drawing its checkbox
                if(!music_list[i].checkbox_ticked && !maud_music_hover(maud, i)) {
                    music_list[i].fill = false;
                }
                maud_drawmusic_checkbox(maud, box_color, fill_color, music_list[i].fill, tick_color,
                    music_list[i].checkbox_ticked);
            } else if(maud_music_hover(maud, i)) {
                // whenever no checkbox is ticked and we hover over the music we display the check box
                // so that the user can click it or select it
                if(maud_checkbox_hovered(maud)) {
                    maud_drawmusic_checkbox(maud, box_color, fill_color, music_list[i].fill, tick_color,
                        music_list[i].checkbox_ticked);
                } else if(!maud_checkbox_hovered(maud)) {
                    maud_drawmusic_checkbox(maud, box_color, fill_color, false, tick_color, false);
                }
            }
            SDL_RenderCopy(maud->renderer, music_list[music_renderlist[music_rendercount]].text_texture, NULL,
                &utext.text_canvas);
            music_rendercount++;
        } else {
            break;
        }
        // calculate the y position for the next musics outercanvas
        prev_canvas.y += music_list[i].outer_canvas.h + 3;
    }
    /* Determine the index, and Calculate the end y position of the last texture that was rendered to the screen */
    size_t index = (music_rendercount < 1) ? 0 : music_renderlist[music_rendercount-1];
    int endpos_y = music_list[index].outer_canvas.y + music_list[index].text_info.text_canvas.h + def_outerheight;
    /* Calculate the end y position for the songs_box and Initialize songs_renderheight_total to store the total height
       of the textures that are currently being rendered to the screen */
    int songsbox_endpos_y = songs_box.y + songs_box.h, songs_renderheight_total = 0;

    /* The data and calculations above are used whenever the window is resized and textures cannot fit on the screen.
       These conditions are evaluated:
       1. the index of the last music that was rendered to the screen is equal to the maximum render position
       2. The end y position of the songs box - the end y position for the last music that is being rendered is greater
       than the height of the text for the last texture
       As long as the conditions above are true then we adjust the height of the previous textures that are not being
       rendered to the screen so that they fit entirely within the songs_box
    */
    if(maud->window_resized && index == max_renderpos && songsbox_endpos_y - endpos_y > music_list[index].text_info.text_canvas.h) {
        /* Determine the total height of each texture that is currently being rendered to the screen */
        for(size_t i=0;i<music_rendercount;i++) {
            songs_renderheight_total += music_list[music_renderlist[i]].outer_canvas.h;
        }
        /* Using the total height of each texture being rendered. We determine the amount of textures that are
           not being rendered to the screen by finding the difference in the height of the songs box and the
           total height of the textures being rendered to the screen
        */
        int amount_notbeing_rendered = (int)roundf((float)(songs_box.h - songs_renderheight_total) /
        (float)(music_list[0].text_info.text_canvas.h + def_outerheight));
        int amount_being_rendered = (max_textures-1) - amount_notbeing_rendered;
        
        /* Whenever the first render position is greater than the amount of textures not being rendered then
           we adjust the height of previous render positions and the first_render position
           so that it fits within the screen.
        */
        size_t first_renderpos = music_renderlist[0];
        if(first_renderpos > amount_notbeing_rendered) {
            for(size_t i=first_renderpos-amount_notbeing_rendered;i<=first_renderpos;i++) {
                bool* render_status = /*(maud->music_searchresult) ? &music_list[i].search_render :*/
                                        &music_list[i].render;
                music_list[i].outer_canvas.h = music_list[i].text_info.text_canvas.h + def_outerheight;
                //music_list[i].render = true;
                maud->music_renderpos--;
                *render_status = true;
            }
        }
    }
    // scroll bar related information
    maud_scrollbar_t songsbox_scrollbar = {
        .rect = scrollbar,
        .displacement = 0.0,
        .orientation = 0,
        .start_pos = *music_renderpos,
        .final_pos = music_count,
        .padding_x = -2,
        .padding_y = -(2 + (int)((double)scrollbar.h / 2.0)),
        .scroll_area = songs_box
    };
    maud->scroll = false;
    maud_renderscroll_bar(maud, &songsbox_scrollbar, max_textures);
    maud->window_resized = false;
    free(music_renderlist); music_renderlist = NULL;
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