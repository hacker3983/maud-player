#include "music_playlistmanager.h"
#include "music_songsmanager.h"
#include "music_playerinfo_extern.h"
#include "music_string.h"
#include "music_scrollcontainer.h"
#include "music_notification.h"

void mplayer_playlistmanager_initialize_playlistinput(mplayer_t* mplayer) {
    if(mplayer->playlist_manager.playlist_inputboxinited) {
        return;
    }
    SDL_Rect input_canvas = {.x = 0, .y = 0, .w = 350, .h = 60};
    SDL_Color input_canvascolor = {0}/*{0x12, 0x12, 0x12, 0xff}{0x2F, 0x06, 0x01, 0xff}*/;
    char* placeholder_text = "New playlist name here...";
    SDL_Color placeholder_color = {0xff, 0xff, 0xff, 0xff};
    SDL_Color cursor_color = {0x00, 0xff, 0x00, 0xff};
    mplayer->playlist_manager.playlist_inputbox = mplayer_inputbox_create(mplayer->music_font, mplayer->font, 
    input_canvas, placeholder_text, placeholder_color, input_canvascolor, placeholder_color, cursor_color);
    mplayer->playlist_manager.playlist_inputboxinited = true;
}

void mplayer_playlistmanager_initialize_renameinput(mplayer_t* mplayer, const char* input_data) {
    SDL_Rect input_canvas = {.x = 0, .y = 0, .w = 350, .h = 60};
    SDL_Color input_canvascolor = {0};
    SDL_Color cursor_color = {0x00, 0xff, 0x00, 0xff};
    SDL_Color content_color = {0xff, 0xff, 0xff, 0xff};
    mplayer->playlist_manager.rename_inputbox = mplayer_inputbox_create(mplayer->music_font, mplayer->font,
        input_canvas, NULL, (SDL_Color){0}, input_canvascolor, content_color, cursor_color);
    mplayer_inputbox_t *rename_inputbox = &mplayer->playlist_manager.rename_inputbox;
    mplayer_inputbox_addinput_data(rename_inputbox, input_data);
}

char* mplayer_playlistmanager_getmusicnamefrom_index(mplayer_t* mplayer, size_t music_listindex,
    size_t music_id) {
    return mplayer->music_lists[music_listindex][music_id].music_name;
}

void mplayer_playlistmanager_getmusicindex_byname(mplayer_t* mplayer, char* music_name,
    size_t* music_listindex, size_t* music_id) {
    size_t music_namelen = strlen(music_name), list_index = 0;
    char* parsed_name = mplayer_playlistmanager_parsemusic_name(music_name, &list_index);
    music_t* music_list = mplayer->music_lists[list_index];
    size_t music_count = mplayer->music_counts[list_index];
    for(size_t i=0;i<music_count;i++) {
        if(strcmp(music_list[i].music_name, music_name) == 0) {
            *music_listindex = list_index, *music_id = i;
            break;
        }
    }
    if(list_index == 1) {
        free(parsed_name);
    }
}

bool mplayer_playlistmanager_playlistexists(mplayer_t* mplayer, const char* playlist_name) {
    return mplayer_playlist_exists(mplayer->playlist_manager.playlists,
        mplayer->playlist_manager.playlist_count, playlist_name);
}

bool mplayer_playlistmanager_createplaylist(mplayer_t* mplayer, const char* playlist_name) {
    bool success = mplayer_playlist_create(&mplayer->playlist_manager.playlists,
        &mplayer->playlist_manager.playlist_count, playlist_name);
    if(success) {
        size_t msg_len = 20 + strlen(playlist_name);
        char msg_buff[msg_len + 1];
        sprintf(msg_buff, "Playlist '%s' created.", playlist_name);
        msg_buff[msg_len] = '\0';
        mplayer_notification_push(&mplayer->notification, mplayer->font, 20,
            (SDL_Color){0x12, 0x12, 0x12, 0xff},
            msg_buff,
            white,
            2,
            20,
            20
        );
        printf("Successfully created playlist: %s\n", playlist_name);
        mplayer_playlistmanager_write_data_tofile(mplayer);
    }
    return success;
}

void mplayer_playlistmanager_addmusicselection_toplaylist(mplayer_t* mplayer, const char* playlist_name) {
    for(size_t i=0;i<mplayer->playlist_manager.playlist_count;i++) {
        mplayer_playlist_t* playlist = &mplayer->playlist_manager.playlists[i];
        if(strcmp(playlist->name, playlist_name) != 0) {
            continue;
        }
        if(mplayer_queue_addmusicfrom_queue(&playlist->queue, &mplayer->selection_queue)) {
            mplayer_playlistmanager_write_data_tofile(mplayer);
        }
        break;
    }
}

void mplayer_playlistmanager_addmusic_toplaylist(mplayer_t* mplayer, const char* playlist_name,
    size_t music_listindex, size_t music_id) {
    for(size_t i=0;i<mplayer->playlist_manager.playlist_count;i++) {
        mplayer_playlist_t* playlist = &mplayer->playlist_manager.playlists[i];
        if(strcmp(playlist->name, playlist_name) != 0) {
            continue;
        }
        if(mplayer_queue_addmusic(&playlist->queue, music_listindex, music_id)) {
            mplayer_playlistmanager_write_data_tofile(mplayer);
        }
    }
}

bool mplayer_playlistmanager_removeplaylist(mplayer_t* mplayer, const char* playlist_name) {
    bool success = mplayer_playlist_remove(&mplayer->playlist_manager.playlists,
        &mplayer->playlist_manager.playlist_count, playlist_name);
    if(success) {
        mplayer_playlistmanager_write_data_tofile(mplayer);
    }
    return success;
}

void mplayer_playlistmanager_destroyplaylist(mplayer_playlist_t* playlist) {
    mplayer_playlist_destroy(playlist);
}

void mplayer_playlistmanager_initialize_newplaylistbutton(mplayer_t* mplayer) {
    newplaylistbtn_t* new_playlistbtn = &mplayer->playlist_manager.button_bar.new_playlistbtn;
    int maximum_elementheight = 0;
    new_playlistbtn->content_spacing = 5;

    text_info_t new_playlistbtn_text = {
        .font_size = 18,
        .text = "New playlist",
        .text_canvas = {0},
        .text_color = {0},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &new_playlistbtn_text);
    new_playlistbtn->text = new_playlistbtn_text;

    SDL_Rect new_playlistbtn_icon = {
        .x = 0, .y = 0,
        .w = 20, .h = 20
    };
    new_playlistbtn->icon = new_playlistbtn_icon;

    SDL_Rect new_playlistbtn_canvas = {
        .x = 5, .y = 0,
        .w = 0, .h = 0
    };
    new_playlistbtn->canvas = new_playlistbtn_canvas;

    SDL_Color new_playlistbtn_color = {0xF4, 0x60, 0x36, 0xFF};
    new_playlistbtn->canvas_color = new_playlistbtn_color;

    // Determine the maximum height between the elements that need to be rendered
    maximum_elementheight = (new_playlistbtn_icon.h  > new_playlistbtn_text.text_canvas.h) ?
            new_playlistbtn_icon.h : new_playlistbtn_text.text_canvas.h;

    // Initialize the dimensions of the new playlist button canvas
    new_playlistbtn_canvas.w = new_playlistbtn_icon.w + new_playlistbtn_text.text_canvas.w +
        new_playlistbtn->content_spacing + 10;
    new_playlistbtn_canvas.h = maximum_elementheight + 20;
    new_playlistbtn->canvas = new_playlistbtn_canvas;
}

void mplayer_playlistmanager_initialize_buttonbar(mplayer_t* mplayer) {
    button_bar_t* button_bar = &mplayer->playlist_manager.button_bar;
    mplayer_playlistmanager_initialize_newplaylistbutton(mplayer);
    mplayer_playlistmanager_initialize_playlistlayout_toggleswitch(mplayer);
    SDL_Rect* canvases[] = {
        &button_bar->new_playlistbtn.canvas,
        &button_bar->playlistlayout_text.text_canvas,
        &button_bar->layout_switch.canvas
    };
    size_t canvas_count = sizeof(canvases) / sizeof(SDL_Rect*);
    int maximum_height = canvases[0]->h;
    for(size_t i=1;i<canvas_count;i++) {
        if(canvases[i]->h > maximum_height) {
            maximum_height = canvases[i]->h;
        }
    }
    button_bar->canvas.x = 0, button_bar->canvas.y = tab_info[0].text_canvas.y +
        tab_info[0].text_canvas.h + UNDERLINE_THICKNESS + 5;
    button_bar->canvas.w = WIDTH, button_bar->canvas.h = maximum_height + 10;
    for(size_t i=0;i<canvas_count;i++) {
        canvases[i]->y = button_bar->canvas.y + (button_bar->canvas.h - canvases[i]->h) / 2;
    }
}

void mplayer_playlistmanager_display_newplaylistbutton(mplayer_t* mplayer) {
    button_bar_t *button_bar = &mplayer->playlist_manager.button_bar;
    newplaylistbtn_t *new_playlistbtn = &button_bar->new_playlistbtn;
    mplayer_playlistmanager_t* playlist_manager = &mplayer->playlist_manager;

    // Set the draw color for the new playlist button canvas and draw the canvas in that color
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(new_playlistbtn->canvas_color));
    SDL_RenderDrawRect(mplayer->renderer, &new_playlistbtn->canvas);
    SDL_RenderFillRect(mplayer->renderer, &new_playlistbtn->canvas);

    // Set the position of the new playlist button icon on its canvas
    new_playlistbtn->icon.x = new_playlistbtn->canvas.x + new_playlistbtn->content_spacing;
    new_playlistbtn->icon.y = new_playlistbtn->canvas.y + (new_playlistbtn->canvas.h -
        new_playlistbtn->icon.h) / 2;
        
    // Add the new playlist button icon to the canvas
    SDL_Texture* new_playlistbtn_icon_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
        [music_addplaylistbtn.texture_idx];
    SDL_RenderCopy(mplayer->renderer, new_playlistbtn_icon_texture, NULL,
        &new_playlistbtn->icon);

    // Set the position of the new playlist button text on its canvas
    new_playlistbtn->text.text_canvas.x = new_playlistbtn->icon.x + new_playlistbtn->icon.w +
        new_playlistbtn->content_spacing;
    new_playlistbtn->text.text_canvas.y = new_playlistbtn->canvas.y + (new_playlistbtn->canvas.h -
        new_playlistbtn->text.text_canvas.h) / 2;

    if(mplayer_rect_hover(mplayer, new_playlistbtn->canvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            new_playlistbtn->clicked = !new_playlistbtn->clicked;
            printf("New_playlistbtn->clicked = %d\n", new_playlistbtn->clicked);
            if(!new_playlistbtn->clicked) {
                printf("Here because clicked is now false\n");
                mplayer_inputbox_clear(&playlist_manager->playlist_inputbox);
            }
            mplayer->mouse_clicked = false;
        }
    }
    // Add the new playlist button text to the canvas
    SDL_Texture* new_playlistbtn_text_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        &new_playlistbtn->text);
    SDL_RenderCopy(mplayer->renderer, new_playlistbtn_text_texture, NULL,
        &new_playlistbtn->text.text_canvas);
    SDL_DestroyTexture(new_playlistbtn_text_texture); new_playlistbtn_text_texture = NULL;
}

void mplayer_playlistmanager_display_newplaylistbutton_input(mplayer_t* mplayer) {
    mplayer_playlistmanager_initialize_playlistinput(mplayer);
    mplayer_playlistmanager_t* playlist_manager = &mplayer->playlist_manager;
    button_bar_t* button_bar = &playlist_manager->button_bar;
    newplaylistbtn_t* new_playlistbtn = &button_bar->new_playlistbtn;
    if(!new_playlistbtn->clicked) {
        return;
    }
    SDL_Rect box_container = {
        .x = 0, .y = 0,
        .w = 100, .h = 110
    };
    SDL_Color box_containercolor = dark_purple;
    mplayer_inputbox_t *playlist_inputbox = &playlist_manager->playlist_inputbox;
    playlist_inputbox->inputbox_canvas.x = new_playlistbtn->canvas.x - (playlist_inputbox->inputbox_canvas.w -
        new_playlistbtn->canvas.w)/2,
    playlist_inputbox->inputbox_canvas.y = new_playlistbtn->canvas.y + new_playlistbtn->canvas.h + 5;
    SDL_Rect createbtn = {
        .x = 0, .y = playlist_inputbox->inputbox_canvas.y,
        .w = 20, .h = 20
    };
    SDL_Color createbtn_color = {0x04, 0x8B, 0xA8, 0xff};
    text_info_t createbtn_text = {
        .font_size = 20,
        .text = "Create playlist",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &createbtn_text);
    createbtn.w += createbtn_text.text_canvas.w + 10,
    createbtn.h += createbtn_text.text_canvas.h + 10;
    if(playlist_inputbox->inputbox_canvas.w > createbtn.w) {
        box_container.w += playlist_inputbox->inputbox_canvas.w;
    } else {
        box_container.w += createbtn.w;
    }
    box_container.h += playlist_inputbox->inputbox_canvas.h + createbtn.h;
    box_container.x = new_playlistbtn->canvas.x;
    box_container.y = new_playlistbtn->canvas.y + new_playlistbtn->canvas.h;

    playlist_inputbox->inputbox_canvas.x = box_container.x + (box_container.w -
        playlist_inputbox->inputbox_canvas.w) / 2;
    playlist_inputbox->inputbox_canvas.y = box_container.y + 35;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(box_containercolor));
    SDL_RenderDrawRect(mplayer->renderer, &box_container);
    SDL_RenderFillRect(mplayer->renderer, &box_container);
    mplayer_inputbox_display(mplayer, playlist_inputbox);

    createbtn.x = playlist_inputbox->inputbox_canvas.x + (playlist_inputbox->inputbox_canvas.w -
        createbtn.w)/2;
    createbtn.y = playlist_inputbox->inputbox_canvas.y + playlist_inputbox->inputbox_canvas.h + 35;
    createbtn_text.text_canvas.x = createbtn.x + (createbtn.w - createbtn_text.text_canvas.w) / 2;
    createbtn_text.text_canvas.y = createbtn.y + (createbtn.h - createbtn_text.text_canvas.h) / 2;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(createbtn_color));
    SDL_RenderDrawRect(mplayer->renderer, &createbtn);
    SDL_RenderFillRect(mplayer->renderer, &createbtn);

    SDL_Texture* createtext_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &createbtn_text);
    SDL_RenderCopy(mplayer->renderer, createtext_texture, NULL, &createbtn_text.text_canvas);
    SDL_DestroyTexture(createtext_texture); createtext_texture = NULL;
    if(mplayer->mouse_clicked && !mplayer_rect_hover(mplayer, box_container)) {
        new_playlistbtn->clicked = false;
        mplayer->mouse_clicked = false;
        mplayer_inputbox_clear(playlist_inputbox);
    }
    if(playlist_inputbox->input.data) {
        playlist_manager->playlistname_empty = false;
    }
    if(mplayer_rect_hover(mplayer, createbtn)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            if(playlist_inputbox->input.data) {
                mplayer_playlistmanager_createplaylist(mplayer, playlist_inputbox->input.data);
                mplayer_inputbox_clear(playlist_inputbox);
                new_playlistbtn->clicked = false;
            } else {
                playlist_manager->playlistname_empty = true;
            }
            playlist_manager->createplaylistbtn_clicked = true;
            mplayer->mouse_clicked = false;
        }
    }
}

void mplayer_playlistmanager_display_playlistname_validation(mplayer_t* mplayer) {
    mplayer_playlistmanager_t* playlist_manager = &mplayer->playlist_manager;
    mplayer_inputbox_t* playlist_inputbox = &playlist_manager->playlist_inputbox;
    if(!playlist_manager->button_bar.new_playlistbtn.clicked) {
        return;
    }
    // Whenever the playlist btn status is set equal to false this function never performs the rest of
    // the operations
    if(!playlist_manager->createplaylistbtn_clicked) {
        return;
    }
    mplayer_tooltip_t popup_tooltip = {
        .background_color = {0x12, 0x12, 0x12, 0xff},
        .delay_secs = 0.5,
        .duration_secs = 0,
        .element_canvas = playlist_inputbox->inputbox_canvas,
        .font = mplayer->music_font,
        .margin_x = 10,
        .margin_y = 10,
        .text = "Please enter a valid playlist name!",
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .font_size = 18,
        .wrap_length = WIDTH-10,
        .wrap_spacing = 10,
        .x = 0,
        .y = 0
    };
    // Whenever the playlist name was not empty we reset createbtn clicked to false
    // otherwise its the clicked state remains true and we render the validation message to the screen
    if(!playlist_manager->playlistname_empty) {
        playlist_manager->createplaylistbtn_clicked = false;
    }
    // Get the size of the tooltip width and height and let it go down below the input box
    mplayer_tooltip_getsize(&popup_tooltip);
    popup_tooltip.x = playlist_inputbox->inputbox_canvas.x + (playlist_inputbox->inputbox_canvas.w
        - popup_tooltip.w) / 2;
    popup_tooltip.y = playlist_inputbox->inputbox_canvas.y + playlist_inputbox->inputbox_canvas.h;
    mplayer_tooltip_render(mplayer, &popup_tooltip);
}

void mplayer_playlistmanager_initialize_playlistlayout_toggleswitch(mplayer_t* mplayer) {
    button_bar_t* button_bar = &mplayer->playlist_manager.button_bar;
    playlist_layoutswitch_t *layout_switch = &button_bar->layout_switch;
    layout_switch->content_spacing = 5;
    text_info_t playlistlayout_text = {
        .font_size = 20,
        .text = "Playlist Layout:",
        .text_canvas = {
            .x = 0, .y = 0,
            .w = 0, .h = 0
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &playlistlayout_text);
    button_bar->playlistlayout_text = playlistlayout_text;

    SDL_Rect layout_iconcanvas = {
        .x = 0, .y = 0,
        .w = 30, .h = 30
    };
    layout_switch->icon_canvas = layout_iconcanvas;
    int* layout_type = &mplayer->playlist_manager.layout_type;
    char* layout_name = mplayer_playlistmanager_getcurrentlayoutname(mplayer),
        *layout_view = calloc(10, sizeof(char));
    sprintf(layout_view, "%s View", layout_name);
    text_info_t layout_viewtext = {
        .font_size = 20,
        .text = layout_view,
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &layout_viewtext);
    layout_switch->view_text = layout_viewtext;

    SDL_Rect dropdown_iconcanvas = {
        .x = 0, .y = 0,
        .w = 30, .h = 30
    };
    layout_switch->dropdown_iconcanvas = dropdown_iconcanvas;

    SDL_Rect canvases[] = {
        playlistlayout_text.text_canvas,
        layout_iconcanvas,
        layout_viewtext.text_canvas,
        dropdown_iconcanvas
    };
    size_t canvas_count = sizeof(canvases)/sizeof(SDL_Rect);
    int maximum_height = mplayer_playlistmanager_findmaxheight(canvases, canvas_count);
    layout_switch->canvas.w = 0;
    for(size_t i=0;i<canvas_count;i++) {
               layout_switch->canvas.w += canvases[i].w;
        if(i != canvas_count-1) {
            layout_switch->canvas.w += layout_switch->content_spacing;
        }
    }
    layout_switch->canvas.h = maximum_height;
}

void mplayer_playlistmanager_display_playlistlayout_toggleswitch(mplayer_t* mplayer) {
    button_bar_t *button_bar = &mplayer->playlist_manager.button_bar;
    text_info_t *playlistlayout_text = &button_bar->playlistlayout_text;
    playlist_layoutswitch_t *layout_switch = &button_bar->layout_switch;

    // Render the text "Playlist Layout:"
    SDL_Texture* playlistlayout_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        playlistlayout_text);
    playlistlayout_text->text_canvas.x = WIDTH - layout_switch->canvas.w - 10,
    playlistlayout_text->text_canvas.y = button_bar->canvas.y + (button_bar->canvas.h -
        playlistlayout_text->text_canvas.h) / 2;
    SDL_RenderCopy(mplayer->renderer, playlistlayout_texture, NULL, &playlistlayout_text->text_canvas);
    SDL_DestroyTexture(playlistlayout_texture); playlistlayout_texture = NULL;

    // Set the layout_switch canvas x position
    layout_switch->canvas.x = playlistlayout_text->text_canvas.x + playlistlayout_text->text_canvas.w +
        layout_switch->content_spacing;

    // Render the grid / list icon depending on the layout
    SDL_Texture* layout_icon = mplayer_playlistmanager_getcurrentlayouticon(mplayer);
    layout_switch->icon_canvas.x = layout_switch->canvas.x;
    layout_switch->icon_canvas.y = layout_switch->canvas.y + (layout_switch->canvas.h
        - layout_switch->icon_canvas.h) / 2;
    SDL_RenderCopy(mplayer->renderer, layout_icon, NULL, &layout_switch->icon_canvas);
    SDL_DestroyTexture(layout_icon); layout_icon = NULL;
 
    // Render the name of the view either "Grid View" or "List VIew"
    SDL_Texture* layout_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        &layout_switch->view_text);
    layout_switch->view_text.text_canvas.x = layout_switch->icon_canvas.x + layout_switch->icon_canvas.w
        + 5;
    layout_switch->view_text.text_canvas.y = layout_switch->canvas.y + (layout_switch->canvas.h -
        layout_switch->view_text.text_canvas.h) / 2;
    SDL_RenderCopy(mplayer->renderer, layout_texture, NULL, &layout_switch->view_text.text_canvas);
    SDL_DestroyTexture(layout_texture); layout_texture = NULL;
    free(layout_switch->view_text.text); layout_switch->view_text.text = NULL;

    // Render the drop down / arrow down icon
    SDL_Texture* drop_downtexture = IMG_LoadTexture(mplayer->renderer, "images/arrow-down.png");
    layout_switch->dropdown_iconcanvas.x = layout_switch->view_text.text_canvas.x +
        layout_switch->view_text.text_canvas.w + layout_switch->content_spacing;
    layout_switch->dropdown_iconcanvas.y = layout_switch->canvas.y + (layout_switch->canvas.h -
        layout_switch->dropdown_iconcanvas.h) / 2;
    SDL_RenderCopy(mplayer->renderer, drop_downtexture, NULL, &layout_switch->dropdown_iconcanvas);
    SDL_DestroyTexture(drop_downtexture); drop_downtexture = NULL;
}

void mplayer_playlistmanager_handle_playlistlayout_toggleswitch(mplayer_t* mplayer) {
    button_bar_t *button_bar = &mplayer->playlist_manager.button_bar;
    if(mplayer_rect_hover(mplayer, button_bar->layout_switch.canvas)) {
        printf("You are hovering over the layout switch\n");
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            printf("you clicked the layout switch\n");
            button_bar->layout_switch.clicked = !(button_bar->layout_switch.clicked);
            mplayer->mouse_clicked = false;
        }
    }
}

void mplayer_playlistmanager_display_layoutdrop_downmenu(mplayer_t* mplayer) {
    button_bar_t* button_bar = &mplayer->playlist_manager.button_bar;
    if(!button_bar->layout_switch.clicked) {
        return;
    }
    int content_spacing = 5, current_playlistlayout = mplayer->playlist_manager.layout_type;
    SDL_Rect layout_iconcanvases[PLAYLIST_LAYOUTCOUNT] = {
        [PLAYLIST_LISTVIEW] = {
            .x = 0, .y = 0,
            .w = 20, .h = 20
        },
        [PLAYLIST_GRIDVIEW] = {
            .x = 0, .y = 0,
            .w = 20, .h = 20
        }
    }, menu_canvas = {
        .x = 0,
        .y = button_bar->layout_switch.canvas.y + button_bar->layout_switch.canvas.h,
        .w = content_spacing * PLAYLIST_LAYOUTCOUNT,
        .h = layout_iconcanvases[0].h + (content_spacing * PLAYLIST_LAYOUTCOUNT)
    },
    option_canvases[PLAYLIST_LAYOUTCOUNT] = {0};
    SDL_Color menu_color = {0x00, 0x00, 0xff, 0xff}/*{0xF4, 0x60, 0x36, 0xFF}*/;
    SDL_Texture *layout_icontextures[PLAYLIST_LAYOUTCOUNT] = {
        [PLAYLIST_LISTVIEW] = mplayer_playlistmanager_getlayouticon(mplayer, PLAYLIST_LISTVIEW),
        [PLAYLIST_GRIDVIEW] = mplayer_playlistmanager_getlayouticon(mplayer, PLAYLIST_GRIDVIEW)
    }, *layout_text_textures[PLAYLIST_LAYOUTCOUNT] = {0};

    char *list_text = mplayer_playlistmanager_getlayoutname(PLAYLIST_LISTVIEW),
         *list_textdup = mplayer_dupstr(list_text, strlen(list_text)),
         *grid_text = mplayer_playlistmanager_getlayoutname(PLAYLIST_GRIDVIEW),
         *grid_textdup = mplayer_dupstr(list_text, strlen(grid_text));
    text_info_t layout_textlist[PLAYLIST_LAYOUTCOUNT] = {
        {
            .font_size = 20,
            .text = list_textdup,
            .text_canvas = {0},
            .text_color = {0xff, 0xff, 0xff, 0xff},
            .utext = NULL
        },
        {
            .font_size = 20,
            .text = grid_textdup,
            .text_canvas = {0},
            .text_color = {0xff, 0xff, 0xff, 0xff},
            .utext = NULL
        }
    };
    
    int maximum_textheight = 0, total_textwidth = 0;
    for(size_t i=0;i<PLAYLIST_LAYOUTCOUNT;i++) {
        layout_text_textures[i] = mplayer_textmanager_rendertext(mplayer, mplayer->font,
            &layout_textlist[i]);
        if(!maximum_textheight) {
            maximum_textheight = layout_textlist[i].text_canvas.h;
        } else if(layout_textlist[i].text_canvas.w > maximum_textheight) {
            maximum_textheight = layout_textlist[i].text_canvas.h;
        }
        total_textwidth += layout_textlist[i].text_canvas.w;
    }

    // Set the dimensions for the "List" option on the drop down menu
    option_canvases[0].w = layout_textlist[0].text_canvas.w + 5,
    option_canvases[0].h = layout_iconcanvases[0].h + layout_textlist[0].text_canvas.h +
        (content_spacing * 3);

    // Set the dimensions for the "Grid" option the drop down menu
    option_canvases[1].w = layout_textlist[1].text_canvas.w + 5;
    option_canvases[1].h = layout_iconcanvases[1].h + layout_textlist[1].text_canvas.h + content_spacing;

    menu_canvas.x = WIDTH - option_canvases[0].w - option_canvases[1].w
        - (content_spacing * 5) - 20;
    menu_canvas.w = option_canvases[0].w + option_canvases[0].h + (content_spacing * 2) + 10,
    menu_canvas.h = option_canvases[0].h + 5;

    // Set the position of the List option
    option_canvases[0].x = menu_canvas.x + (menu_canvas.w - option_canvases[0].w - option_canvases[1].w)
        / 2;
    option_canvases[0].y = menu_canvas.y + (menu_canvas.h - option_canvases[0].h) / 2;

    layout_iconcanvases[0].x = option_canvases[0].x + (option_canvases[0].w -
        layout_iconcanvases[0].w) / 2;
    layout_iconcanvases[0].y = option_canvases[0].y + 5;
    layout_textlist[0].text_canvas.x = option_canvases[0].x + (option_canvases[0].w -
        layout_textlist[0].text_canvas.w) / 2;
    layout_textlist[0].text_canvas.y = layout_iconcanvases[0].y + layout_iconcanvases[0].h + 5;

    // Set the position of the Grid option
    option_canvases[1].x = option_canvases[0].x + option_canvases[1].w + 5;
    option_canvases[1].y = menu_canvas.y + (menu_canvas.h - option_canvases[1].h) / 2;
    
    layout_iconcanvases[1].x = option_canvases[1].x + (option_canvases[1].w -
    layout_iconcanvases[1].w) / 2;
    layout_textlist[1].text_canvas.x = option_canvases[0].x + (option_canvases[1].w -
        layout_textlist[1].text_canvas.w) / 2;

    layout_iconcanvases[1].y = layout_iconcanvases[0].y;
    layout_textlist[1].text_canvas.x = option_canvases[1].x + (option_canvases[1].w -
        layout_textlist[1].text_canvas.w) / 2;
    layout_textlist[1].text_canvas.y = layout_iconcanvases[1].y + layout_iconcanvases[1].h + 5;

    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(menu_color));
    SDL_RenderDrawRect(mplayer->renderer, &menu_canvas);
    SDL_RenderFillRect(mplayer->renderer, &menu_canvas);

    for(size_t i=0;i<PLAYLIST_LAYOUTCOUNT;i++) {
        SDL_Color active_color = {0xF4, 0x60, 0x36, 0xFF};
        if(i == mplayer->playlist_manager.layout_type) {
            SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(active_color));
            SDL_RenderDrawRect(mplayer->renderer, &option_canvases[i]);
            SDL_RenderFillRect(mplayer->renderer, &option_canvases[i]);
        } else if(mplayer_rect_hover(mplayer, option_canvases[i])) {
            mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            if(mplayer->mouse_clicked) {
                mplayer->playlist_manager.layout_type = i;
                mplayer->mouse_clicked = false;
            }
            SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(active_color));
            SDL_RenderDrawRect(mplayer->renderer, &option_canvases[i]);
            SDL_RenderFillRect(mplayer->renderer, &option_canvases[i]);
        }
        SDL_RenderCopy(mplayer->renderer, layout_icontextures[i], NULL, &layout_iconcanvases[i]);
        SDL_DestroyTexture(layout_icontextures[i]); layout_icontextures[i] = NULL;

        SDL_RenderCopy(mplayer->renderer, layout_text_textures[i], NULL, &layout_textlist[i].text_canvas);
        SDL_DestroyTexture(layout_text_textures[i]); layout_text_textures[i] = NULL;
        free(layout_textlist[i].text); layout_textlist[i].text = NULL;
    }
    if(mplayer->mouse_clicked && button_bar->layout_switch.clicked) {
        mplayer->mouse_clicked = false;
        button_bar->layout_switch.clicked = false;
    }
}

SDL_Texture* mplayer_playlistmanager_getlayouticon(mplayer_t* mplayer, int type) {
    SDL_Texture *layout_icon = NULL;
    switch(type) {
        case PLAYLIST_LISTVIEW: layout_icon = IMG_LoadTexture(mplayer->renderer, "images/list.png"); break;
        case PLAYLIST_GRIDVIEW: layout_icon = IMG_LoadTexture(mplayer->renderer, "images/grid.png"); break;
    }
    return layout_icon;
}


SDL_Texture* mplayer_playlistmanager_getcurrentlayouticon(mplayer_t* mplayer) {
    return mplayer_playlistmanager_getlayouticon(mplayer, mplayer->playlist_manager.layout_type);
}

static char* mplayer_playlistmanager_getlayoutname(int type) {
    static char layout_name[5] = {0};
    switch(type) {
        case PLAYLIST_LISTVIEW: strcpy(layout_name, "List"); break;
        case PLAYLIST_GRIDVIEW: strcpy(layout_name, "Grid"); break;
    }
    return layout_name;
}

static char* mplayer_playlistmanager_getcurrentlayoutname(mplayer_t* mplayer) {
    return mplayer_playlistmanager_getlayoutname(mplayer->playlist_manager.layout_type);
}

int mplayer_playlistmanager_findmaxheight(SDL_Rect* rects, size_t rect_count) {
    if(!rects) {
        return 0;
    }
    int maximum_height = rects[0].h;
    for(size_t i=1;i<rect_count;i++) {
        if(rects[i].h > maximum_height) {
            maximum_height = rects[i].h;
        }
    }
    return maximum_height;
}

void mplayer_playlistmanager_displayplaylists(mplayer_t* mplayer) {
    int grid_startx = 5;
    button_bar_t button_bar = mplayer->playlist_manager.button_bar;
    SDL_Rect scroll_area = {
        .x = 0, .y = button_bar.canvas.y + button_bar.canvas.h + 5,
        .w = WIDTH, .h = music_status.y - (button_bar.canvas.y + button_bar.canvas.h + 5)
    };
    int layout_type = mplayer->playlist_manager.layout_type;
    bool show_tooltip = false;
    SDL_Rect playlist_background = {
        .x = 0, .y = scroll_area.y
    };
    SDL_RenderSetClipRect(mplayer->renderer, &scroll_area);
    if(layout_type == PLAYLIST_LISTVIEW) {
        mplayer_scrollcontainer_setprops(&mplayer->playlist_manager.playlist_container, scroll_area, 5, 0,
        mplayer->playlist_manager.playlist_count);
        playlist_background.y = mplayer->playlist_manager.playlist_container.scroll_y;
    }

    for(size_t i=0;i<mplayer->playlist_manager.playlist_count;i++) {
        if(layout_type == PLAYLIST_LISTVIEW) {
            if(i < mplayer->playlist_manager.playlist_container.content_renderpos) {
                continue;
            }
            if(playlist_background.y < scroll_area.y + scroll_area.h) {
                mplayer_playlistmanager_renderplaylist_listlayout(mplayer, i, &playlist_background);
                mplayer_scrollcontainer_additem(&mplayer->playlist_manager.playlist_container, playlist_background);
                mplayer_scrollcontainer_setnext_itemcanvas(&mplayer->playlist_manager.playlist_container, playlist_background);
            } else {
                break;
            }
        } else if(layout_type == PLAYLIST_GRIDVIEW) {
            mplayer_playlistmanager_renderplaylist_gridlayout(mplayer, grid_startx, i, &playlist_background,
                &show_tooltip);
        }
    }
    SDL_RenderSetClipRect(mplayer->renderer, NULL);
    if(layout_type == PLAYLIST_LISTVIEW) {
        if(mplayer->scroll) {
            printf("Perform scrolling\n");
            mplayer_scrollcontainer_performscroll(mplayer, &mplayer->playlist_manager.playlist_container);
            mplayer->scroll = false;
        }
        if(mplayer->playlist_manager.playlist_container.item_container.items) {
            mplayer_scrollcontainer_init(&mplayer->playlist_manager.playlist_container);
        }
        mplayer_scrollcontainer_resetitem_index(&mplayer->playlist_manager.playlist_container);
    }
    if(show_tooltip) {
        mplayer_tooltip_renderhover(mplayer, &mplayer->playlist_manager.playlist_tooltip);
    }
}

void mplayer_playlistmanager_handleplaylist_event(mplayer_t* mplayer, size_t playlist_index, SDL_Rect playlist_canvas) {
    button_bar_t* button_bar = &mplayer->playlist_manager.button_bar;
    if(mplayer_rect_hover(mplayer, playlist_canvas) && !button_bar->layout_switch.clicked
        && !mplayer->playlist_manager.button_bar.new_playlistbtn.clicked) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            mplayer->playlist_manager.playlist_selectionindex = playlist_index;
            mplayer->playlist_manager.playlist_selected = true;
            printf("You selected the playlist %s\n", mplayer->playlist_manager.playlists
                [playlist_index].name);
            mplayer->mouse_clicked = false;
        }
    }
}

void mplayer_playlistmanager_renderplaylist_listlayout(mplayer_t* mplayer, size_t playlist_index,
    SDL_Rect* playlist_background) {
    text_info_t playlist_name = {
        .font_size = 20,
        .text = NULL,
        .text_canvas = {
            .x = 0, .y = 0
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = mplayer->playlist_manager.playlists[playlist_index].name
    };
    SDL_Texture* playlist_nametexture = mplayer_textmanager_renderunicode(mplayer, mplayer->font, &playlist_name);
    int maximum_height = (playlist_name.text_canvas.h > music_playlistbtn.btn_canvas.h) ?
        playlist_name.text_canvas.h : music_playlistbtn.btn_canvas.h;
    playlist_background->w = WIDTH, playlist_background->h = maximum_height + 10;
    mplayer_playlistmanager_handleplaylist_event(mplayer, playlist_index, *playlist_background);

    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(dark_purple));
    SDL_RenderDrawRect(mplayer->renderer, playlist_background);
    SDL_RenderFillRect(mplayer->renderer, playlist_background);

    music_playlistbtn.btn_canvas.x = playlist_background->x + 5;
    music_playlistbtn.btn_canvas.y = playlist_background->y + (playlist_background->h -
        music_playlistbtn.btn_canvas.h) / 2;

    playlist_name.text_canvas.x = music_playlistbtn.btn_canvas.x + music_playlistbtn.btn_canvas.w + 5;
    playlist_name.text_canvas.y = playlist_background->y + (playlist_background->h -
        playlist_name.text_canvas.h) / 2;

    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
        [music_playlistbtn.texture_idx], NULL, &music_playlistbtn.btn_canvas);
    SDL_RenderCopy(mplayer->renderer, playlist_nametexture, NULL, &playlist_name.text_canvas);
    SDL_DestroyTexture(playlist_nametexture); playlist_nametexture = NULL;
    playlist_background->y += playlist_background->h + 5;
}

void mplayer_playlistmanager_renderplaylist_gridlayout(mplayer_t* mplayer, int start_x,
    size_t playlist_index, SDL_Rect* playlist_background, bool* show_tooltip) {
    mplayer_playlistmanager_t* playlist_manager = &mplayer->playlist_manager;
    text_info_t playlist_name = {
        .font_size = 16,
        .text = playlist_manager->playlists[playlist_index].name,
        .text_canvas = {
            .x = 0, .y = 0,
            .w = 0, .h = 0
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    SDL_Rect playlist_card = {
        .x = 0, .y = 0,
        .w = 130, .h = 120
    };

    // Set the playlist background width
    playlist_background->w = 150;
    // Render the playlist name
    char* truncated_text = mplayer_textmanager_truncatetext(mplayer->font, &playlist_name,
        playlist_background->w - start_x);
    if(truncated_text) {
        playlist_name.text = truncated_text;
    }
    SDL_Texture* playlist_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font,
        &playlist_name);

    // Set the playlist background height and the playlist card positions
    playlist_background->h = playlist_card.h + playlist_name.text_canvas.h + 30;
    playlist_card.x = playlist_background->x + (playlist_background->w - playlist_card.w) / 2;
    playlist_card.y = playlist_background->y + (playlist_background->h - playlist_card.h) / 2;

    // set the playlist name position
    playlist_name.text_canvas.x = playlist_card.x;
    playlist_name.text_canvas.y = playlist_card.y + playlist_card.h + 5;

    mplayer_tooltip_t playlist_tooltip = {
        .background_color = {0xFF, 0x3C, 0x38, 0xFF},
        .delay_secs = .5,
        .duration_secs = 0,
        .font = mplayer->music_font,
        .font_size = 16,
        .margin_x = 10,
        .margin_y = 10,
        .text = playlist_manager->playlists[playlist_index].name,
        .element_canvas = playlist_name.text_canvas,
        .text_color = {0xFF, 0xFF, 0xFF, 0xFF},
        .x = playlist_background->x,
        .y = playlist_name.text_canvas.y,
        .w = 0, .h = 0,
        .wrap_length = playlist_background->w,
        .wrap_spacing = 5,
        .contents = NULL,
        .content_count = 0
    };
    mplayer_tooltip_getsize(&playlist_tooltip);
    playlist_tooltip.y -= playlist_tooltip.h - 2;
    if(truncated_text && mplayer_rect_hover(mplayer, playlist_name.text_canvas)) {
        playlist_manager->playlist_tooltip = playlist_tooltip;
        *show_tooltip = true;
    }
    free(truncated_text);

    // set the playlist icon position
    music_playlistbtn.btn_canvas.x = playlist_card.x + (playlist_card.w -
        music_playlistbtn.btn_canvas.w) / 2;
    music_playlistbtn.btn_canvas.y = playlist_card.y + (playlist_card.h -
        music_playlistbtn.btn_canvas.h) / 2;

    // Draw playlist background
    SDL_Color bg_color = {0x00, 0xff, 0x00, 0xff};
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bg_color));
    SDL_RenderDrawRect(mplayer->renderer, playlist_background);

    // Draw the playlist card with its icon
    SDL_Color playlist_cardcolor = {0x12, 0x12, 0x12, 0xff};
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(playlist_cardcolor));
    SDL_RenderDrawRect(mplayer->renderer, &playlist_card);
    SDL_RenderFillRect(mplayer->renderer, &playlist_card);

    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
            [music_playlistbtn.texture_idx], NULL, &music_playlistbtn.btn_canvas);
  
    // Finally display the name of the playlist
    SDL_RenderCopy(mplayer->renderer, playlist_texture, NULL, &playlist_name.text_canvas);
    SDL_DestroyTexture(playlist_texture); playlist_texture = NULL;
    mplayer_playlistmanager_handleplaylist_event(mplayer, playlist_index, *playlist_background);

    playlist_background->x += playlist_background->w + start_x;
    if(playlist_background->x + playlist_background->w >= WIDTH) {
        playlist_background->x = start_x;
        playlist_background->y = playlist_name.text_canvas.y + playlist_name.text_canvas.h + 5;
    }
}

void mplayer_playlistmanager_display_buttonbar(mplayer_t* mplayer) {
    mplayer_playlistmanager_initialize_buttonbar(mplayer);
    mplayer_playlistmanager_display_newplaylistbutton(mplayer);
    mplayer_playlistmanager_display_playlistlayout_toggleswitch(mplayer);
    mplayer_playlistmanager_handle_playlistlayout_toggleswitch(mplayer);
}

void mplayer_playlistmanager_setplaylist_cardposition(mplayer_t* mplayer, SDL_Rect playlistmenu_canvas,
    SDL_Rect* playlist_card) {
    playlist_card->x = playlistmenu_canvas.x + 10,
    playlist_card->y = playlistmenu_canvas.y + (playlistmenu_canvas.h - playlist_card->h) / 2;

    music_playlistbtn.btn_canvas.x = playlist_card->x + (playlist_card->w -
    music_playlistbtn.btn_canvas.w) / 2;
    music_playlistbtn.btn_canvas.y = playlist_card->y + (playlist_card->h -
        music_playlistbtn.btn_canvas.h) / 2;
}

void mplayer_playlistmanager_renderplaylist_card(mplayer_t* mplayer, SDL_Rect* playlist_card) {
    // Draw the playlist card
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(dark_purple));
    SDL_RenderDrawRect(mplayer->renderer, playlist_card);
    SDL_RenderFillRect(mplayer->renderer, playlist_card);

    // Copy the icon texture to it
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE]
            [music_playlistbtn.texture_idx], NULL, &music_playlistbtn.btn_canvas);
}

void mplayer_playlistmanager_display_playlistmenu(mplayer_t* mplayer, SDL_Rect* playlistmenu_canvasref) {
    mplayer_playlistmanager_t* playlist_manager = &mplayer->playlist_manager;
    mplayer_playlist_t* playlists = playlist_manager->playlists;
    size_t playlist_selectionindex = playlist_manager->playlist_selectionindex;
    SDL_Rect playlistmenu_canvas = {
        .x = 0, .y = tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + UNDERLINE_THICKNESS + 5,
        .w = WIDTH - 30,
        .h = 210
    };
    SDL_Color playlistmenu_canvascolor = {0x12, 0x12, 0x12, 0xff}/*{0xff, 0x00, 0x00, 0xff}*/;
    playlistmenu_canvas.x = (WIDTH - playlistmenu_canvas.w) / 2;

    SDL_Rect playlist_card = {
        .x = 0, .y = 0,
        .w = 130, .h = 130
    };
    playlistmenu_canvas.h = playlist_card.h + 10;
    mplayer_playlistmanager_setplaylist_cardposition(mplayer, playlistmenu_canvas, &playlist_card);
    *playlistmenu_canvasref = playlistmenu_canvas;


    // Render the playlist menu canvas
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(playlistmenu_canvascolor));
    SDL_RenderDrawRect(mplayer->renderer, &playlistmenu_canvas);
    SDL_RenderFillRect(mplayer->renderer, &playlistmenu_canvas);

    // Render the playlist card
    mplayer_playlistmanager_renderplaylist_card(mplayer, &playlist_card);

    // Render the playlist name
    text_info_t playlist_name = {
        .font_size = 20,
        .text = NULL,
        .text_canvas = {
            .x = playlist_card.x + playlist_card.w + 5, .y = playlist_card.y + 5,
            .w = 0, .h = 0
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = playlists[playlist_selectionindex].name
    };
    SDL_Texture* playlist_nametexture = mplayer_textmanager_renderunicode(mplayer, mplayer->font,
        &playlist_name);
    SDL_RenderCopy(mplayer->renderer, playlist_nametexture, NULL, &playlist_name.text_canvas);
    SDL_DestroyTexture(playlist_nametexture); playlist_nametexture = NULL;

    // Render the number of items within the playlist
    size_t item_count = mplayer_queue_getmusic_count(playlists[playlist_selectionindex].queue);
    int number_len = mplayer_getsize_t_length(item_count);
    char* item_countinfo = calloc(number_len + 7, sizeof(char));
    sprintf(item_countinfo, "%zu ", item_count);
    strcat(item_countinfo, "item");
    if(item_count > 1) {
        strcat(item_countinfo, "s");
    }
    text_info_t item_info = {
        .font_size = 20,
        .text = item_countinfo,
        .text_canvas = {
            .x = playlist_name.text_canvas.x,
            .y = playlist_name.text_canvas.y + playlist_name.text_canvas.h + 10,
            .w = 0, .h = 0
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    SDL_Texture *item_infotexture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &item_info);
    free(item_countinfo); item_countinfo = NULL;
    SDL_RenderCopy(mplayer->renderer, item_infotexture, NULL, &item_info.text_canvas);

    text_info_t playall_text = {
        .font_size = 18,
        .text = "Play all",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    TTF_SizeText(mplayer->font, playall_text.text, &playall_text.text_canvas.w, &playall_text.text_canvas.h);

    int max_h = (music_listplaybtn.btn_canvas.h > playall_text.text_canvas.h) ?
                 music_listplaybtn.btn_canvas.h : playall_text.text_canvas.h;
    SDL_Rect playall_btn = {
        .x = item_info.text_canvas.x,
        .y = item_info.text_canvas.y + item_info.text_canvas.h + 5,
        .w = music_listplaybtn.btn_canvas.w + 20 + playall_text.text_canvas.w,
        .h = max_h + 30
    };

    music_playlistbtn.btn_canvas.x = playall_btn.x + 5,
    music_playlistbtn.btn_canvas.y = playall_btn.y + (playall_btn.h - music_playlistbtn.btn_canvas.h) / 2;

    playall_text.text_canvas.x = music_playlistbtn.btn_canvas.x + music_playlistbtn.btn_canvas.w + 9;
    playall_text.text_canvas.y = playall_btn.y + (playall_btn.h - playall_text.text_canvas.h) / 2;

    SDL_Color playall_bg = {0xF4, 0x60, 0x36, 0xFF};
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(playall_bg));
    SDL_RenderDrawRect(mplayer->renderer, &playall_btn);
    SDL_RenderFillRect(mplayer->renderer, &playall_btn);

    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][music_listplaybtn.texture_idx],
        NULL, &music_playlistbtn.btn_canvas);

    SDL_Texture* texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &playall_text);
    SDL_RenderCopy(mplayer->renderer, texture, NULL, &playall_text.text_canvas);
    SDL_DestroyTexture(texture);

    SDL_Rect add_icon = {
        .x = 0,
        .y = 0,
        .w = 30,
        .h = 30
    };
    text_info_t add_text = {
        .font_size = 18,
        .text = "Add to",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &add_text);
    max_h = (add_text.text_canvas.h > add_icon.h) ? add_text.text_canvas.h : add_icon.h;
    SDL_Rect add = {
        .x = playall_btn.x + playall_btn.w + 10,
        .y = playall_btn.y,
        .w = add_text.text_canvas.w + add_icon.w + 20,
        .h = max_h + 20
    };
    SDL_Color add_color = {0x00, 0xff, 0x00, 0xff};

    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(add_color));
    SDL_RenderDrawRect(mplayer->renderer, &add);

    add_icon.x = add.x + 5,
    add_icon.y = add.y + (add.h - add_icon.w) / 2;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][music_addtobtn.texture_idx],
        NULL, &add_icon);
    
    add_text.text_canvas.x = add_icon.x + add_icon.w + 5;
    add_text.text_canvas.y = add.y + (add.h - add_text.text_canvas.h) / 2;
    SDL_Texture* add_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &add_text);
    SDL_RenderCopy(mplayer->renderer, add_texture, NULL, &add_text.text_canvas);
    SDL_DestroyTexture(add_texture);

    SDL_Rect rename_icon = {
        .x = 0,
        .y = 0,
        .w = 30,
        .h = 30
    };
    text_info_t rename_text = {
        .font_size = 18,
        .text = "Rename",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &rename_text);
    max_h = (rename_text.text_canvas.h > rename_icon.h) ? rename_text.text_canvas.h : rename_icon.h;
    SDL_Rect rename = {
        .x = add.x + add.w + 10,
        .y = add.y,
        .w = rename_text.text_canvas.w + rename_icon.w + 20,
        .h = max_h + 20
    };
    SDL_Color rename_color = {0x00, 0xff, 0x00, 0xff};
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(rename_color));
    SDL_RenderDrawRect(mplayer->renderer, &rename);

    rename_icon.x = rename.x + 5;
    rename_icon.y = rename.y + (rename.h - rename_icon.h) / 2;
    SDL_Texture* rename_icontexture = IMG_LoadTexture(mplayer->renderer, "images/rename.png");
    SDL_RenderCopy(mplayer->renderer, rename_icontexture, NULL, &rename_icon);
    SDL_DestroyTexture(rename_icontexture);

    rename_text.text_canvas.x = rename_icon.x + rename_icon.w + 5;
    rename_text.text_canvas.y = rename.y + (rename.h - rename_text.text_canvas.h) / 2;
    SDL_Texture* rename_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &rename_text);
    SDL_RenderCopy(mplayer->renderer, rename_texture, NULL, &rename_text.text_canvas);
    SDL_DestroyTexture(rename_texture);

    SDL_Rect delete_icon = {
        .x = 0,
        .y = 0,
        .w = 30,
        .h = 30
    };
    text_info_t delete_text = {
        .font_size = 18,
        .text = "Delete",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &delete_text);
    max_h = (delete_text.text_canvas.h > delete_icon.h) ? delete_text.text_canvas.h: delete_icon.h;
    SDL_Rect delete = {
        .x = rename.x + rename.w + 10,
        .y = add.y,
        .w = delete_text.text_canvas.w + delete_icon.w + 20,
        .h = max_h + 20
    };
    SDL_Color delete_color = {0x00, 0xff, 0x00, 0xff};
    
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(delete_color));
    SDL_RenderDrawRect(mplayer->renderer, &delete);
    
    delete_icon.x = delete.x + 5;
    delete_icon.y = delete.y + (delete.h - delete_icon.h) / 2;
    SDL_Texture* delete_icontexture = IMG_LoadTexture(mplayer->renderer, "images/delete.png");
    SDL_RenderCopy(mplayer->renderer, delete_icontexture, NULL, &delete_icon);
    SDL_DestroyTexture(delete_icontexture);

    delete_text.text_canvas.x = delete_icon.x + delete_icon.w + 5;
    delete_text.text_canvas.y = delete.y + (delete.h - delete_text.text_canvas.h) / 2;
    SDL_Texture* delete_infotexture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &delete_text);
    SDL_RenderCopy(mplayer->renderer, delete_infotexture, NULL, &delete_text.text_canvas);
    SDL_DestroyTexture(delete_infotexture);
    if(playlist_manager->rename_clicked) {
        return;
        mplayer_playlistmanager_displayrename_input(mplayer);
    } else if(mplayer_rect_hover(mplayer, playall_btn)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            music_queue_t* play_queue = &mplayer->play_queue;
            Mix_PauseMusic();
            Mix_HaltMusic();
            mplayer_queue_destroy(play_queue);
            mplayer_queue_addmusicfrom_queue(play_queue, &playlists[playlist_selectionindex].queue);
            if(!playlists[playlist_selectionindex].queue.items) {
                mplayer_notification_push(&mplayer->notification, mplayer->font, 20,
                    (SDL_Color){0x12, 0x12, 0x12, 0xff},
                    "There are no items in the selected playlist.",
                    white,
                    2,
                    20,
                    20
                );
            }
            mplayer_songsmanager_playmusic(mplayer);
            printf("You clicked the play all button\n");
            mplayer->mouse_clicked = false;
        }
    } else if(mplayer_rect_hover(mplayer, add)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            printf("You clicked the add button\n");
            mplayer->mouse_clicked = false;
        }
    } else if(mplayer_rect_hover(mplayer, rename)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            mplayer_playlistmanager_initialize_renameinput(mplayer, playlists[playlist_selectionindex].name);
            playlist_manager->rename_clicked = true;
            printf("You clicked the rename button\n");
            mplayer->mouse_clicked = false;
        }
    } else if(mplayer_rect_hover(mplayer, delete)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            size_t msg_len = 20 + strlen(playlists[playlist_selectionindex].name);
            char msg_buff[msg_len+1];
            sprintf(msg_buff, "Playlist '%s' deleted.", playlists[playlist_selectionindex].name);
            msg_buff[msg_len] = '\0';
            mplayer_playlistmanager_removeplaylist(mplayer, playlists[playlist_selectionindex].name);
            mplayer_notification_push(&mplayer->notification, mplayer->font, 20,
                (SDL_Color){0x12, 0x12, 0x12, 0xff},
                msg_buff,
                white,
                2,
                20,
                20  
            );
            playlist_manager->playlist_selected = false;
            mplayer->mouse_clicked = false;
        }
    }
}

void mplayer_playlistmanager_displayrename_input(mplayer_t* mplayer) {
    mplayer_playlistmanager_t *playlist_manager = &mplayer->playlist_manager;
    mplayer_inputbox_t *rename_inputbox = &playlist_manager->rename_inputbox;
    text_info_t rename_text = {
        .font_size = 18,
        .text = "Rename playlist",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    SDL_Rect playlist_card = {
        .x = 0,
        .y = 0,
        .w = 100,
        .h = 150
    };
    SDL_Color playlist_cardcolor = {0x12, 0x12, 0x12, 0xff};
    SDL_Rect playlist_icon = {
        .x = 0,
        .y = 0,
        .w = 40,
        .h = 40
    };
    mplayer_textmanager_sizetext(mplayer->font, &rename_text);
    int max_w = (rename_inputbox->inputbox_canvas.w > rename_text.text_canvas.w) ?
                    rename_inputbox->inputbox_canvas.w : rename_text.text_canvas.w;
    text_info_t rename_btntext = {
        .font_size = 18,
        .text = "Rename",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &rename_btntext);
    SDL_Rect rename_btncanvas = {
        .x = 0,
        .y = 0,
        .w = rename_btntext.text_canvas.w + 20,
        .h = rename_btntext.text_canvas.h + 20,
    };
    SDL_Color rename_btncolor = {0x00, 0x00, 0xff, 0xff};
    text_info_t cancel_btntext = {
        .font_size = 18,
        .text = "Cancel",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &cancel_btntext);
    SDL_Rect cancel_btncanvas = {
        .x = 0,
        .y = 0,
        .w = cancel_btntext.text_canvas.w + 20,
        .h = cancel_btntext.text_canvas.h + 20
    };
    SDL_Color cancel_btncolor = {0xff, 0x00, 0x00, 0xff};

    SDL_Rect box_container = {
        .x = 0, .y = 0,
        .w = max_w + 60,
        .h = rename_text.text_canvas.h + playlist_card.h + rename_inputbox->inputbox_canvas.h + rename_btncanvas.h + 80
    };
    SDL_Color box_color = {0xF6, 0xAE, 0x2D, 0xff};
    box_container.x = (WIDTH - box_container.w) / 2;
    box_container.y = (HEIGHT - box_container.h) / 2;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(box_color));
    SDL_RenderDrawRect(mplayer->renderer, &box_container);
    SDL_RenderFillRect(mplayer->renderer, &box_container);

    rename_text.text_canvas.x = box_container.x + 10;
    rename_text.text_canvas.y = box_container.y + 5;
    SDL_Texture* rename_texture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &rename_text);
    SDL_RenderCopy(mplayer->renderer, rename_texture, NULL, &rename_text.text_canvas);
    SDL_DestroyTexture(rename_texture);

    playlist_card.x = box_container.x + (box_container.w - playlist_card.w) / 2;
    playlist_card.y = rename_text.text_canvas.y + rename_text.text_canvas.h + 15;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(playlist_cardcolor));
    SDL_RenderDrawRect(mplayer->renderer, &playlist_card);
    SDL_RenderFillRect(mplayer->renderer, &playlist_card);

    playlist_icon.x = playlist_card.x + (playlist_card.w - playlist_icon.w) / 2;
    playlist_icon.y = playlist_card.y + (playlist_card.h - playlist_icon.h) / 2;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][music_playlistbtn.texture_idx],
        NULL, &playlist_icon);

    rename_inputbox->inputbox_canvas.x = box_container.x + (box_container.w - rename_inputbox->inputbox_canvas.w) / 2,
    rename_inputbox->inputbox_canvas.y = playlist_card.y + playlist_card.h + 15;
    mplayer_inputbox_display(mplayer, rename_inputbox);

    rename_btncanvas.x = box_container.x + 10;
    rename_btncanvas.y = box_container.y + box_container.h - rename_btncanvas.h - 20;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(rename_btncolor));
    SDL_RenderDrawRect(mplayer->renderer, &rename_btncanvas);
    SDL_RenderFillRect(mplayer->renderer, &rename_btncanvas);

    rename_btntext.text_canvas.x = rename_btncanvas.x + (rename_btncanvas.w - rename_btntext.text_canvas.w) / 2;
    rename_btntext.text_canvas.y = rename_btncanvas.y + (rename_btncanvas.h - rename_btntext.text_canvas.h) / 2;
    SDL_Texture* rename_btntexture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &rename_btntext);
    SDL_RenderCopy(mplayer->renderer, rename_btntexture, NULL, &rename_btntext.text_canvas);

    cancel_btncanvas.x = box_container.x + box_container.w - cancel_btncanvas.w - 10;
    cancel_btncanvas.y = rename_btncanvas.y;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(cancel_btncolor));
    SDL_RenderDrawRect(mplayer->renderer, &cancel_btncanvas);
    SDL_RenderFillRect(mplayer->renderer, &cancel_btncanvas);

    cancel_btntext.text_canvas.x = cancel_btncanvas.x + (cancel_btncanvas.w - cancel_btntext.text_canvas.w) / 2;
    cancel_btntext.text_canvas.y = cancel_btncanvas.y + (cancel_btncanvas.h - cancel_btntext.text_canvas.h) / 2;
    SDL_Texture* cancel_btntexture = mplayer_textmanager_rendertext(mplayer, mplayer->font, &cancel_btntext);
    SDL_RenderCopy(mplayer->renderer, cancel_btntexture, NULL, &cancel_btntext.text_canvas);
    SDL_DestroyTexture(cancel_btntexture);

    if(mplayer->mouse_clicked && !mplayer_rect_hover(mplayer, box_container)) {
        mplayer_inputbox_clear(&playlist_manager->rename_inputbox);
        playlist_manager->rename_clicked = false;
        mplayer->mouse_clicked = false;
    } else if(mplayer_rect_hover(mplayer, rename_btncanvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            size_t new_namelen = 0;
            char* new_name = rename_inputbox->input.data;
            char* original_name = playlist_manager->playlists[playlist_manager->playlist_selectionindex].name;
            size_t msg_len = 35 + strlen(original_name) + strlen(new_name);
            char msg_buff[msg_len+1];
            sprintf(msg_buff, "Playlist '%s' has been renamed to '%s'.", original_name, new_name);
            msg_buff[msg_len] = '\0';
            mplayer_playlist_rename(&playlist_manager->playlists, playlist_manager->playlist_count, original_name,
                new_name);
            mplayer_playlistmanager_write_data_tofile(mplayer);
            mplayer_inputbox_clear(&playlist_manager->rename_inputbox);
            mplayer_notification_push(&mplayer->notification, mplayer->font, 20,
                (SDL_Color){0x12, 0x12, 0x12, 0xff},
                msg_buff,
                white,
                2,
                20,
                20
            );
            playlist_manager->rename_clicked = false;
            mplayer->mouse_clicked = false;
        }
    } else if(mplayer_rect_hover(mplayer, cancel_btncanvas)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        if(mplayer->mouse_clicked) {
            mplayer_inputbox_clear(&playlist_manager->rename_inputbox);
            playlist_manager->rename_clicked = false;
            mplayer->mouse_clicked = false;
        }
    } else if(mplayer->mouse_clicked) {
        mplayer->mouse_clicked = false;
    }
}

void mplayer_playlistmanager_display_playlistcontent(mplayer_t* mplayer) {
    mplayer_playlistmanager_t *playlist_manager = &mplayer->playlist_manager;
    mplayer_playlist_t* playlists = playlist_manager->playlists;
    music_scrollcontainer_t *playlist_itemcontainer = &playlist_manager->playlist_itemcontainer;
    size_t playlist_count = playlist_manager->playlist_count,
           playlist_selectionindex = playlist_manager->playlist_selectionindex;
    SDL_Rect playlistmenu_canvas = {0};
    mplayer_playlistmanager_display_playlistmenu(mplayer, &playlistmenu_canvas);
    music_queue_t playlist_queue = playlists[playlist_selectionindex].queue;
    SDL_Rect scroll_area = {
        .x = 0,
        .y = playlistmenu_canvas.y + playlistmenu_canvas.h + 5,
        .w = WIDTH,
        .h = music_status.y - (playlistmenu_canvas.y + playlistmenu_canvas.h + 5)
    };
    SDL_RenderSetClipRect(mplayer->renderer, &scroll_area);
    if(playlist_itemcontainer->init && playlist_itemcontainer->content_count != playlist_queue.item_count) {
        playlist_itemcontainer->content_count = playlist_queue.item_count;
    }
    mplayer_scrollcontainer_setprops(playlist_itemcontainer, scroll_area, 20, 0, playlist_queue.item_count);
    text_info_t music_name = {
        .font_size = 20,
        .text = NULL,
        .text_canvas = {
            .x = 10,
            .y = 0,
            .w = 0, .h = 0
        },
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    SDL_Rect outer_canvas = {
        .x = 0,
        .y = scroll_area.y,
        .w = WIDTH,
        .h = 0
    };
    if(playlist_itemcontainer->init) {
        outer_canvas.y = playlist_itemcontainer->scroll_y;
    }
    size_t content_renderpos = 0;
    for(size_t i=playlist_itemcontainer->content_renderpos;i<playlist_queue.item_count;i++) {
        music_queueitem_t item = playlist_queue.items[i];
        size_t music_listindex = item.music_listindex;
        size_t music_id = item.music_id;
        music_name.utext = mplayer->music_lists[music_listindex][music_id].music_name;
        mplayer_textmanager_sizetext(mplayer->music_font, &music_name);
        outer_canvas.h = music_name.text_canvas.h + 15;
        music_name.text_canvas.y = outer_canvas.y + (outer_canvas.h - music_name.text_canvas.h) / 2;
        if(outer_canvas.y < scroll_area.y + scroll_area.h) {
            mplayer_scrollcontainer_additem(playlist_itemcontainer, outer_canvas);
            mplayer_scrollcontainer_setnext_itemcanvas(playlist_itemcontainer, outer_canvas);
        } else {
            break;
        }
        SDL_SetRenderDrawColor(mplayer->renderer, 0, 42, 50, 0xFF);
        SDL_RenderDrawRect(mplayer->renderer, &outer_canvas);
        SDL_RenderFillRect(mplayer->renderer, &outer_canvas);
        SDL_Texture* music_nametexture = mplayer_textmanager_renderunicode(mplayer, mplayer->music_font,
            &music_name);
        SDL_RenderCopy(mplayer->renderer, music_nametexture, NULL, &music_name.text_canvas);
        SDL_DestroyTexture(music_nametexture); music_nametexture = NULL;
        outer_canvas.y += outer_canvas.h + 5;
    }
    if(mplayer->scroll) {
        mplayer_scrollcontainer_performscroll(mplayer, playlist_itemcontainer);
        mplayer->scroll = false;
    }
    if(playlist_itemcontainer->item_container.items) {
        mplayer_scrollcontainer_init(playlist_itemcontainer);
    }
    mplayer_scrollcontainer_resetitem_index(playlist_itemcontainer);
    SDL_RenderSetClipRect(mplayer->renderer, NULL);
    if(mplayer->playlist_manager.rename_clicked) {
        mplayer_playlistmanager_displayrename_input(mplayer);
    }
}

void mplayer_playlistmanager_display(mplayer_t* mplayer) {
    mplayer_playlistmanager_t *playlist_manager = &mplayer->playlist_manager;
    if(playlist_manager->playlist_selected) {
        mplayer_playlistmanager_display_playlistcontent(mplayer);
        return;
    }
    mplayer_playlistmanager_display_buttonbar(mplayer);
    mplayer_playlistmanager_displayplaylists(mplayer);
    mplayer_playlistmanager_display_layoutdrop_downmenu(mplayer);
    mplayer_playlistmanager_display_newplaylistbutton_input(mplayer);
    mplayer_playlistmanager_display_playlistname_validation(mplayer);
}

void mplayer_playlistmanager_destroy(mplayer_t* mplayer) {
    mplayer_inputbox_destroy(&mplayer->playlist_manager.playlist_inputbox);
    mplayer_playlists_destroy(&mplayer->playlist_manager.playlists,
        &mplayer->playlist_manager.playlist_count);
}
