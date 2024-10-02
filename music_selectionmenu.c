#include "music_player.h"
#include "music_selectionmenu.h"

void mplayer_selectionmenu_create(mplayer_t* mplayer) {
    // Set the x position for the music selection menu
    mplayer->music_selectionmenu.x = 0;
    // Set the y position calculation for selection menu will be down below the search bar
    mplayer->music_selectionmenu.y = mplayer->music_searchbar.y + mplayer->music_searchbar.h;
    // set the width and the height for the selection menu
    mplayer->music_selectionmenu.w = WIDTH;
    mplayer->music_selectionmenu.h = mplayer->music_searchbar.h;
    // whenever we haven't selected anything then we will not render the selection menu onto the screen
    if(!mplayer->tick_count) {
        return;
    }
    text_info_t songs_selectioninfo = {0};
    SDL_Rect playbtn_background = {0};
    mplayer_selectionmenu_display_checkallbtn(mplayer);
    mplayer_selectionmenu_display_songselectioninfo(mplayer, &songs_selectioninfo);
    mplayer_selectionmenu_handle_checkallbtn_toggleoption(mplayer, songs_selectioninfo);
    mplayer_selectionmenu_display_playbtn(mplayer, songs_selectioninfo, &playbtn_background);
    mplayer_selectionmenu_display_addbtn(mplayer, playbtn_background);
    if(mplayer_rect_hover(mplayer, mplayer->music_selectionmenu) && mplayer->mouse_clicked) {
        if(mplayer->music_selectionmenu_addtobtn_clicked) {
            mplayer->music_selectionmenu_addtobtn_clicked = false;
        }
        mplayer->mouse_clicked = false;
    }
}

void mplayer_selectionmenu_display_addbtn(mplayer_t* mplayer, SDL_Rect playbtn_background) {
    // Create add to button used for adding selected musics to current play queue or creating a new playlist of them    
    music_addtobtn.btn_canvas.x = playbtn_background.x + playbtn_background.w + 20;
    music_addtobtn.btn_canvas.y = mplayer->music_selectionmenu.y +
                                ((mplayer->music_selectionmenu.h - music_addtobtn.btn_canvas.h) / 2);
 
    SDL_Texture* addtobtn_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][music_addtobtn.texture_idx];
    text_info_t addtobtn_text = {
        .font_size = 20,
        .text = "Add to",
        .text_canvas = {
            .x = music_addtobtn.btn_canvas.x + music_addtobtn.btn_canvas.w + 5,
            .y = music_listplaybtn.btn_canvas.y,
            .h = music_addtobtn.btn_canvas.h
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };

    SDL_Texture* addtobtn_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &addtobtn_text);
    SDL_RenderCopy(mplayer->renderer, addtobtn_texture, NULL, &music_addtobtn.btn_canvas);
    SDL_RenderCopy(mplayer->renderer, addtobtn_text_texture, NULL, &addtobtn_text.text_canvas);
    SDL_DestroyTexture(addtobtn_text_texture); addtobtn_text_texture = NULL;
    SDL_Rect addtobtn_fullcanvas = {
        .x = music_addtobtn.btn_canvas.x,
        .y = music_addtobtn.btn_canvas.y,
        .w = music_addtobtn.btn_canvas.w + addtobtn_text.text_canvas.w + 5,
        .h = music_addtobtn.btn_canvas.h
    };
    mplayer->music_selectionmenu_addtocanvas = addtobtn_fullcanvas;
    bool *addtobtn_clicked = &mplayer->music_selectionmenu_addtobtn_clicked;
    if(mplayer_rect_hover(mplayer, addtobtn_fullcanvas)) {
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

    SDL_Rect playqueue_btncanvas = {0};
    text_info_t playqueue_btntext = {
        .font_size = 20,
        .text = "Play queue",
        .text_canvas = {
            .x = drop_downmenu.x + 10, .y = drop_downmenu.y + 10,
            .w = 0, .h = 0
        },
        .text_color = (const SDL_Color)white,
        .utext = NULL
    };
    SDL_Texture* playqueue_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &playqueue_btntext);
    SDL_RenderCopy(mplayer->renderer, playqueue_text_texture, NULL, &playqueue_btntext.text_canvas);
    SDL_DestroyTexture(playqueue_text_texture);

    SDL_Rect new_playlistbtn_canvas = {
        .x = playqueue_btntext.text_canvas.x, .y = playqueue_btntext.text_canvas.y + playqueue_btntext.text_canvas.h + 10,
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

    SDL_Rect new_playlistbtn_combo = {
        .x = new_playlistbtn_canvas.x,
        .y = new_playlistbtn_canvas.y,
        .w = new_playlistbtn_canvas.w + new_playlistbtn_text.text_canvas.w +
             (new_playlistbtn_text.text_canvas.x + new_playlistbtn_text.text_canvas.w)
                - (new_playlistbtn_canvas.x + new_playlistbtn_canvas.w),
        .h = new_playlistbtn_canvas.h
    };
    if(mplayer_rect_hover(mplayer, playqueue_btntext.text_canvas) && mplayer->mouse_clicked) {
        printf("You clicked the play queue button within the drop down menu\n");
        mplayer->music_selectionmenu_addtobtn_clicked = false; mplayer->mouse_clicked = false;
    } else if(mplayer_rect_hover(mplayer, new_playlistbtn_combo) && mplayer->mouse_clicked) {
        music_addplaylistbtn.clicked = true;
        printf("You clicked the new playlist button within the drop down menu\n");
        mplayer->music_selectionmenu_addtobtn_clicked = false; mplayer->mouse_clicked = false;
    } else if(mplayer->mouse_clicked && mplayer->music_selectionmenu_addtobtn_clicked) {
        mplayer->music_selectionmenu_addtobtn_clicked = false;
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
    modal.x = (WIDTH - modal.w)/2, modal.y = (HEIGHT - modal.h)/2;
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
    // Draw background for "Add to new playlis" text
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
    newplaylist_imagecanvas.x = (WIDTH - newplaylist_imagecanvas.w) / 2;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(newplaylist_imagecanvas_color));
    SDL_RenderDrawRect(mplayer->renderer, &newplaylist_imagecanvas);
    SDL_RenderFillRect(mplayer->renderer, &newplaylist_imagecanvas);
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
                    [music_addplaylistbtn.texture_idx], NULL, &newplaylist_imagecanvas);

    // TODO: Implement input box for typing the playlist name that should be created
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
    if(!mplayer_rect_hover(mplayer, modal) && mplayer->mouse_clicked) {
        music_addplaylistbtn.clicked = false;
    }
}

void mplayer_selectionmenu_display_playbtn(mplayer_t* mplayer, text_info_t songs_selectioninfo,
    SDL_Rect* playbtn_backgroundref) {
    // Render the play button icon within the selection menu
    // This can be use to play musics that were selected using the check boxes
    SDL_Texture* music_listplaybtn_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
        [music_listplaybtn.texture_idx];
    music_listplaybtn.btn_canvas.x = songs_selectioninfo.text_canvas.x + songs_selectioninfo.text_canvas.w + 20,
    // Center the music list play button within the selection menu canvas on the y axis / vertically
    music_listplaybtn.btn_canvas.y = mplayer->music_selectionmenu.y +
        ((mplayer->music_selectionmenu.h - music_listplaybtn.btn_canvas.h) / 2);
    mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_listplaybtn.texture_idx] =
        music_listplaybtn.btn_canvas;

    // Add play text beside the play button
    text_info_t play_text = {
        .font_size = 20,
        .text = "Play",
        .text_canvas = {
            .x = music_listplaybtn.btn_canvas.x + music_listplaybtn.btn_canvas.w + 5,
            .y = music_listplaybtn.btn_canvas.y,
            .h = music_listplaybtn.btn_canvas.h,
        },
        .text_color = {0xFF, 0xFF, 0xFF, 0xFF},
        .utext = NULL
    };
    SDL_Texture* playtext_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &play_text);
    int playbtn_background_height = music_listplaybtn.btn_canvas.h + 10;
    SDL_Rect playbtn_background = {
        .x = music_listplaybtn.btn_canvas.x - 5,
        .y = mplayer->music_selectionmenu.y + ((mplayer->music_selectionmenu.h - playbtn_background_height)/2),
        .w = play_text.text_canvas.w + music_listplaybtn.btn_canvas.w + 15,
        .h = playbtn_background_height,
    };
    // Render the full play button background or canvas to the screen
    SDL_SetRenderDrawColor(mplayer->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawRect(mplayer->renderer, &playbtn_background);
    SDL_RenderFillRect(mplayer->renderer, &playbtn_background);

    // Copy the play button onto the renderer
    if(SDL_RenderCopy(mplayer->renderer, music_listplaybtn_texture, NULL,
        &music_listplaybtn.btn_canvas) < 0) {
        printf("SDL_RenderCopyFailed(): because %s\n", SDL_GetError());
    }
    // Copy the texture for the play button text onto the renderer
    if(SDL_RenderCopy(mplayer->renderer, playtext_texture, NULL, &play_text.text_canvas)) {
        printf("SDL_RenderCopyFailed(): because %s\n", SDL_GetError());
    }
    SDL_DestroyTexture(playtext_texture); playtext_texture = NULL;
    *playbtn_backgroundref = playbtn_background;
}

void mplayer_selectionmenu_display_songselectioninfo(mplayer_t* mplayer, text_info_t* songs_selectioninfo_ref) {
    // The amount of digits that makes up the number of selected songs
    int songs_selectioncount_length = 0;
    size_t music_selection_tickcount = mplayer->tick_count;
    if(mplayer->music_selectionmenu_checkbox_tickall) {
        music_selection_tickcount = mplayer->music_count;
    }
    // Convert the number of selected songs into a string
    char* songs_selectioncount_text = mplayer_size_t_tostring(music_selection_tickcount,
            &songs_selectioncount_length);

    // Allocate memory to store number of selected songs information on screen
    // it will look like this "# of songs"
    char* songs_selectioninfo_text = calloc(songs_selectioncount_length + 15, sizeof(char));
    strcpy(songs_selectioninfo_text, songs_selectioncount_text);
    strcat(songs_selectioninfo_text, " song");
    // If there are more than one songs selected we add an s to pluralize the word song
    (mplayer->tick_count > 1) ? strcat(songs_selectioninfo_text, "s"): 0;
    strcat(songs_selectioninfo_text, " selected");

    text_info_t songs_selectioninfo = {
        .font_size = 20,
        .text = songs_selectioninfo_text,
        .text_canvas = {
            .x = music_selectionmenu_checkbox_size.x + music_selectionmenu_checkbox_size.w + 10,
            .y = music_selectionmenu_checkbox_size.y,
        },
        .text_color = {},
        .utext = NULL
    };

    SDL_Texture* songs_selectioninfo_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &songs_selectioninfo);
    SDL_RenderCopy(mplayer->renderer, songs_selectioninfo_texture, NULL, &songs_selectioninfo.text_canvas);
    SDL_DestroyTexture(songs_selectioninfo_texture); songs_selectioninfo_texture = NULL;
    *songs_selectioninfo_ref = songs_selectioninfo;
    free(songs_selectioncount_text); songs_selectioncount_text = NULL;
    free(songs_selectioninfo_text); songs_selectioninfo_text = NULL;
}

bool mplayer_selectionmenu_togglesong_checkbox(mplayer_t* mplayer, music_t* music_list, size_t music_checkbox_index) {
    size_t i = music_checkbox_index;
    if(mplayer->music_selectionmenu_checkbox_tickall && !music_list[i].checkbox_ticked) {
        music_list[i].fill = true;
        music_list[i].checkbox_ticked = true;
        if(mplayer->tick_count < mplayer->music_count) {
            mplayer->tick_count++;
        }
        return true;
    } else if(mplayer->music_selectionmenu_checkbox_clicked &&
        !mplayer->music_selectionmenu_checkbox_tickall && music_list[i].checkbox_ticked) {
        music_list[i].fill = false;
        music_list[i].checkbox_ticked = false;
        mplayer->tick_count--;
        if(!mplayer->tick_count) {
            mplayer->music_selectionmenu_checkbox_clicked = false;
        }
        return true;
    }
    return false;
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
    
    if(mplayer_rect_hover(mplayer, music_selectionmenu_togglecanvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        *fill = true;
        if(mplayer->mouse_clicked) {
            if(*tick) {
                *fill = false, *tick = false;
            } else {
                *tick = true;
            }
            mplayer->music_selectionmenu_checkbox_clicked = true;
        }
    } else if(!mplayer->music_selectionmenu_checkbox_clicked && mplayer->tick_count == mplayer->music_count) {
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