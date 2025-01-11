#include "music_playlistmanager.h"
#include "music_playerinfo_extern.h"
#include "music_string.h"


void mplayer_playlistmanager_initialize_playlistinput(mplayer_t* mplayer) {
    if(mplayer->playlist_manager.playlist_inputboxinited) {
        return;
    }
    SDL_Rect input_canvas = {.x = 0, .y = 0, .w = 350, .h = 60};
    SDL_Color input_canvascolor = {0x00, 0x00, 0xff, 0xff};
    char* placeholder_text = "New playlist name here...";
    SDL_Color placeholder_color = {0xff, 0xff, 0xff, 0xff};
    SDL_Color cursor_color = {0x00, 0xff, 0x00, 0xff};
    mplayer->playlist_manager.playlist_inputbox = mplayer_inputbox_create(mplayer->music_font, mplayer->font, 
    input_canvas, placeholder_text, placeholder_color, input_canvascolor, placeholder_color, cursor_color);
    mplayer->playlist_manager.playlist_inputboxinited = true;
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
    mplayer_playlistmanager_t playlist_manager = mplayer->playlist_manager;
    button_bar_t button_bar = playlist_manager.button_bar;
    newplaylistbtn_t new_playlistbtn = button_bar.new_playlistbtn;
    if(!new_playlistbtn.clicked) {
        return;
    }
    SDL_Rect box_container = {
        .x = 0, .y = 0,
        .w = 10, .h = 10
    };
    SDL_Color box_containercolor = {0xff, 0x00, 0x00, 0xff};
    mplayer_inputbox_t *playlist_inputbox = &playlist_manager.playlist_inputbox;
    playlist_inputbox->inputbox_canvas.x = new_playlistbtn.canvas.x - (playlist_inputbox->inputbox_canvas.w -
        new_playlistbtn.canvas.w)/2,
    playlist_inputbox->inputbox_canvas.y = new_playlistbtn.canvas.y + new_playlistbtn.canvas.h;
    SDL_Rect createbtn = {
        .x = 0, .y = playlist_inputbox->inputbox_canvas.y,
        .w = 10, .h = 10
    };
    SDL_Color createbtn_color = {0xF4, 0x60, 0x36, 0xFF};
    text_info_t createbtn_text = {
        .font_size = 20,
        .text = "Create playlist",
        .text_canvas = {0},
        .text_color = {0xff, 0xff, 0xff, 0xff},
        .utext = NULL
    };
    mplayer_textmanager_sizetext(mplayer->font, &createbtn_text);
    createbtn.w += createbtn_text.text_canvas.w,
    createbtn.h += createbtn_text.text_canvas.h;
    if(playlist_inputbox->inputbox_canvas.w > createbtn.w) {
        box_container.w += playlist_inputbox->inputbox_canvas.w;
    } else {
        box_container.w += createbtn.w;
    }
    box_container.h += playlist_inputbox->inputbox_canvas.h + createbtn.h;
    box_container.x = new_playlistbtn.canvas.x;
    box_container.y = new_playlistbtn.canvas.y + new_playlistbtn.canvas.h;

    playlist_inputbox->inputbox_canvas.x = box_container.x + (box_container.w -
        playlist_inputbox->inputbox_canvas.w) / 2;
    playlist_inputbox->inputbox_canvas.y = box_container.y + 5;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(box_containercolor));
    SDL_RenderDrawRect(mplayer->renderer, &box_container);
    SDL_RenderFillRect(mplayer->renderer, &box_container);
    mplayer_inputbox_display(mplayer, playlist_inputbox);
    if(mplayer->mouse_clicked && !mplayer_rect_hover(mplayer, box_container)) {
        new_playlistbtn.clicked = false;
        mplayer->mouse_clicked = false;
        mplayer_inputbox_clear(playlist_inputbox);
    }
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
    SDL_Rect playlist_background = {
        .x = 0, .y = button_bar.canvas.y + button_bar.canvas.h + 5,
    };
    int layout_type = mplayer->playlist_manager.layout_type;
    bool show_tooltip = false;
    for(size_t i=0;i<mplayer->playlist_manager.playlist_count;i++) {
        if(layout_type == PLAYLIST_LISTVIEW) {
            mplayer_playlistmanager_renderplaylist_listlayout(mplayer, i, &playlist_background);
        } else if(layout_type == PLAYLIST_GRIDVIEW) {
            mplayer_playlistmanager_renderplaylist_gridlayout(mplayer, grid_startx, i, &playlist_background,
                &show_tooltip);
        }
    }
    if(show_tooltip) {
        mplayer_tooltip_render(mplayer, &mplayer->playlist_manager.playlist_tooltip);
    }
}

void mplayer_playlistmanager_handleplaylist_event(mplayer_t* mplayer, size_t playlist_index, SDL_Rect playlist_canvas) {
    button_bar_t* button_bar = &mplayer->playlist_manager.button_bar;
    if(mplayer_rect_hover(mplayer, playlist_canvas) && !button_bar->layout_switch.clicked) {
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
    playlist_background->h = playlist_card.h + playlist_name.text_canvas.h + 10;
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

    // Draw the playlist card with its icon
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(dark_purple));
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

void mplayer_playlistmanager_display_playlistmenu(mplayer_t* mplayer) {
    mplayer_playlistmanager_t* playlist_manager = &mplayer->playlist_manager;
    mplayer_playlist_t* playlists = playlist_manager->playlists;
    size_t playlist_selectionindex = playlist_manager->playlist_selectionindex;
    SDL_Rect playlistmenu_canvas = {
        .x = 0, .y = tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + UNDERLINE_THICKNESS + 5,
        .w = WIDTH - 30,
        .h = 210
    };
    SDL_Color playlistmenu_canvascolor = {0xff, 0x00, 0x00, 0xff};
    playlistmenu_canvas.x = (WIDTH - playlistmenu_canvas.w) / 2;

    SDL_Rect playlist_card = {
        .x = 0, .y = 0,
        .w = 130, .h = 130
    };
    playlistmenu_canvas.h = playlist_card.h + 10;
    mplayer_playlistmanager_setplaylist_cardposition(mplayer, playlistmenu_canvas, &playlist_card);

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
}

void mplayer_playlistmanager_display(mplayer_t* mplayer) {
    mplayer_playlistmanager_t *playlist_manager = &mplayer->playlist_manager;
    mplayer_playlist_t* playlists = playlist_manager->playlists;
    size_t playlist_count = playlist_manager->playlist_count,
           playlist_selectionindex = playlist_manager->playlist_selectionindex;
    if(playlist_manager->playlist_selected) {
        mplayer_playlistmanager_display_playlistmenu(mplayer);
        /*music_queue_t playlist_queue = playlists[playlist_selectionindex].queue;
        text_info_t music_name = {
            .font_size = 20,
            .text = NULL,
            .text_canvas = {
                .x = 10,
                .y = tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + UNDERLINE_THICKNESS + 10,
                .w = 0, .h = 0
            },
            .text_color = {0xff, 0xff, 0xff, 0xff},
            .utext = NULL
        };
        for(size_t i=0;i<playlist_queue.item_count;i++) {
            music_queueitem_t item = playlist_queue.items[i];
            size_t music_listindex = item.music_listindex;
            for(size_t j=0;j<item.music_count;j++) {
                size_t music_id = item.music_ids[j];
                music_name.utext = mplayer->music_lists[music_listindex][music_id].music_name;
                SDL_Texture* music_nametexture = mplayer_textmanager_renderunicode(mplayer, mplayer->music_font,
                    &music_name);
                SDL_RenderCopy(mplayer->renderer, music_nametexture, NULL, &music_name.text_canvas);
                SDL_DestroyTexture(music_nametexture); music_nametexture = NULL;
                music_name.text_canvas.y += music_name.text_canvas.h + 5;
            }
        }*/
        return;
    }
    mplayer_playlistmanager_display_buttonbar(mplayer);
    mplayer_playlistmanager_displayplaylists(mplayer);
    mplayer_playlistmanager_display_layoutdrop_downmenu(mplayer);
    mplayer_playlistmanager_display_newplaylistbutton_input(mplayer);
}

void mplayer_playlistmanager_destroy(mplayer_t* mplayer) {
    mplayer_inputbox_destroy(&mplayer->playlist_manager.playlist_inputbox);
    mplayer_playlists_destroy(&mplayer->playlist_manager.playlists,
        &mplayer->playlist_manager.playlist_count);
}
