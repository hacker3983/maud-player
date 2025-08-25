#include "maud_player.h"
#include "maud_checkboxes.h"
#include "maud_inputboxes.h"
#include "maud_textmanager.h"
#include "maud_filemanager.h"
#include "maud_menumanager.h"
#include "maud_texturemanager.h"
#include "maud_selectionmenu.h"
#include "maud_settingsmenu.h"
#include "maud_playerinfo.h"
#include "maud_queue.h"
#include "maud_colorpicker.h"
#include "maud_notification.h"
#include "maud_songsmanager.h"
#include "maud_playlistmanager.h"
#include "maud_playerbutton_manager.h"

SDL_Rect *playbtn_canvas = NULL, *playbtn_listcanvas = NULL,
        *prevbtn_canvas = NULL, *skipbtn_canvas = NULL,
        *pausebtn_canvas = NULL, *repeatoffbtn_canvas = NULL,
        *repeatallbtn_canvas = NULL, *repeatonebtn_canvas = NULL,
        *shufflebtn_canvas = NULL;

void maud_init() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL_Init(): Failed because %s\n", SDL_GetError());
        return;
    }
    printf("Successfully initialized SDL\n");
    if(TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init(): Failed to initialize SDL TTF because %s\n", TTF_GetError());
        return;
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "IMG_Init(): Failed to initialize SDL_Image because %s\n", IMG_GetError());
        return;
    }
    int audioinit_flags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_OPUS;
    if(Mix_Init(audioinit_flags) == audioinit_flags) {
        printf("successfully initialized SDL Mixer\n");
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 8192) < 0) {
        fprintf(stderr, "Mix_OpenAudio Failed\n");
    }
    setlocale(LC_ALL, "it_IT.UTF-8");
}

SDL_Window* maud_createwindow(const char* title, int width, int height) {
    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    width, height, SDL_WINDOW_RESIZABLE);
    return window;
}

SDL_Renderer* maud_createrenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer;
}

TTF_Font* maud_openfont(const char* file, int size) {
    TTF_Font* font = TTF_OpenFont(file, size);
    return font;
}

void maud_createapp(maud_t* maud) {
    // create music player's graphical utilities
    maud->menu_opt = MAUD_DEFAULT_MENU;
    maud->window = maud_createwindow(WINDOW_TITLE, WIDTH, HEIGHT);
    maud->renderer = maud_createrenderer(maud->window);
    maud->font = maud_openfont(FONT_FILE, FONT_SIZE);
    maud->music_font = maud_openfont(MUSIC_FONTFILE, MUSIC_FONTSIZE);
    maud->menu = &maud->menus[maud->menu_opt];
    maud_setcursor(maud, MAUD_CURSOR_DEFAULT);
    maud->cursors[MAUD_CURSOR_DEFAULT] = SDL_GetDefaultCursor();
    maud->cursors[MAUD_CURSOR_POINTER] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    maud->cursors[MAUD_CURSOR_TYPEABLE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    maud->repeat_id = MAUD_REPEATALLBTN,
    maud->music_selectionmenu_addto_dropdown_color = window_color;
    maud->playlist_manager.layout_type = PLAYLIST_GRIDVIEW;

    const char* placeholder_text = "New playlist name here...";
    SDL_Rect inputbox_canvas = {
        .x = 0, .y = 0,
        .w = 300, .h = 50
    };
    SDL_Color placeholder_color = {0xff, 0xff, 0xff, 0xff},
              input_datacolor = {0xff, 0xff, 0xff, 0xff},
              input_boxcolor = {0x00, 0x00, 0x00, 0x00},
              cursor_color = {0x00, 0xff, 0x00, 0xff};
    maud->playlist_inputbox = maud_inputbox_create(maud->music_font, 20, input_boxcolor,
        placeholder_text, placeholder_color,
        cursor_color, input_datacolor, inputbox_canvas.x, inputbox_canvas.y,
        inputbox_canvas.w, inputbox_canvas.h, 2, 50/2);

    text_info_t placeholder = text_info[1];
    maud->search_inputbox = maud_inputbox_create(maud->music_font, 20,
        music_searchbar_color, placeholder.text, placeholder.text_color, green, white,
        0, 0, 0, 0, 2, 50/2);
    maud->search_inputbox.fill = false;

    // Initialize the play queue and music selection queue
    maud_queue_init(&maud->play_queue);
    maud_queue_init(&maud->selection_queue);

    // Initialize color picker system
    color_tracknameattrib_t color_attribs[4] = {
        {
            .track_font = maud->font,
            .track_fontsize = 20,
            .track_name = "Red",
            .track_namecolor = white,
            .track_namespacing = 10
        },
        {
            .track_font = maud->font,
            .track_fontsize = 20,
            .track_name = "Green:",
            .track_namecolor = white,
            .track_namespacing = 10
        },
        {
            .track_font = maud->font,
            .track_fontsize = 20,
            .track_name = "Blue:",
            .track_namecolor = white,
            .track_namespacing = 10
        },
        {
            .track_font = maud->font,
            .track_fontsize = 20,
            .track_name = "Alpha:",
            .track_namecolor = white,
            .track_namespacing = 10
        }
    };
    maud_colorpicker_t* color_picker = &maud->setting_navbar.customize_tab.color_picker;
    maud_colorpicker_setpreview_props(color_picker, maud->font, 20,
        400, 150);
    maud_colorpicker_setsliders_props(color_picker,
        color_attribs,
        2, 50, 20, 50,
        white,
        (SDL_Color){0x00, 0x00, 0x00, 0xff}
    );

    // create music information
    #if _WIN32 && MAUD_RELEASE
    if(!PathIsDirectory(MAUD_PROGRAMDATA)) {
        CreateDirectory(MAUD_PROGRAMDATA, NULL);
    }
    #endif
    maud_filemanager_getmusicpath_info(maud);
    if(maud->locations == NULL) {
        #ifdef _WIN32
        wchar_t* location = NULL;
        char* username = getenv("USERNAME"), *location_str = NULL, root_path[4] = {0};
        location_str = calloc(16+strlen(username), sizeof(char));
        maud_filemanager_getroot_path(root_path);
        strcpy(location_str, root_path);
        strcat(location_str, "Users\\");
        strcat(location_str, username);
        strcat(location_str, "\\Music");
        location = maud_stringtowide(location_str);
        maud_filemanager_addmusic_location(maud, location);
        free(location_str); location_str = NULL;
        free(location); location = NULL;
        #else
        char* home = getenv("HOME"), *location = NULL;
        location = calloc(strlen(home) + 7, sizeof(char));
        strcat(location, home);
        strcat(location, "/Music");
        maud_filemanager_addmusic_location(maud, location);
        free(location); location = NULL;
        #endif
    }
}

void maud_set_window_color(SDL_Renderer* renderer, SDL_Color bg_color) {
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderClear(renderer);
}

void maud_getwindow_size(maud_t* maud) {
    SDL_GetWindowSize(maud->window, &maud->win_width, &maud->win_height);
}

void maud_set_window_title(maud_t* maud, const char* title) {
    SDL_SetWindowTitle(maud->window, title);
}

void maud_setcursor(maud_t* maud, int cursor_type) {
    if(!maud->cursor_active && cursor_type != MAUD_CURSOR_DEFAULT) {
        maud->cursor = cursor_type;
        maud->cursor_active = true;
    } else if(!maud->cursor_active && cursor_type == MAUD_CURSOR_DEFAULT) {
        maud->cursor = cursor_type;
    } else if(maud->cursor_active && cursor_type == MAUD_CURSOR_DEFAULT) {
        maud->cursor_active = false;
    }
    SDL_SetCursor(maud->cursors[maud->cursor]);
}

bool maud_tab_hover(maud_t* maud, tabinfo_t tab) {
    return maud_rect_hover(maud, tab.text_canvas);
}

bool maud_rect_hover(maud_t* maud, SDL_Rect rect) {
    return ((maud->mouse_x <= rect.x + rect.w && maud->mouse_x >= rect.x) &&
        (maud->mouse_y <= rect.y + rect.h && maud->mouse_y >= rect.y));
}

bool maud_tabs_hover(maud_t* maud, tabinfo_t* tabs, int* tab_id, size_t tab_count) {
    for(size_t i=0;i<tab_count;i++) {
        if(maud_tab_hover(maud, tabs[i])) {
            if(tab_id != NULL) {
                *tab_id = tabs[i].id;
            }
            tabs[i].hover = true; return true;
        }
        tabs[i].hover = false;
    }
    return false;
}

bool maud_music_hover(maud_t* maud, size_t index) {
    return maud_rect_hover(maud, maud->music_list[index].outer_canvas);
}

bool maud_songsbox_hover(maud_t* maud) {
    return maud_rect_hover(maud, songs_box);
}

bool maud_musiclist_playbutton_hover(maud_t* maud) {
    return maud_rect_hover(maud, music_listplaybtn.btn_canvas);
}

bool maud_progressbar_hover(maud_t* maud) {
    return maud_rect_hover(maud, maud->progress_bar);
}

bool maud_music_searchsubstr(maud_t* maud, size_t search_index) {
    /* Create a copy of the music search data and the music_name so that we do not modify the contents
       of the original and convert them to lowercase / uppercase so we can make our match incase sensitive
    */
    bool match = false;
    size_t music_namelen = strlen(maud->music_list[search_index].music_name),
           music_querylen = maud->current_musicsearch_querylen;
    char *music_name = maud_dupstr(maud->music_list[search_index].music_name, music_namelen),
         *music_query = maud_dupstr(maud->current_musicsearch_query, music_querylen);
    char *match_string = (char*)maud_strcasestr(music_name, music_query);
    if(match_string) {
        match = true;
    }
    free(music_query); free(music_name); music_query = NULL; music_name = NULL;
    free(match_string); match_string = NULL;
    return match;
}

void maud_clearcurrent_searchquery_data(maud_t* maud) {
    maud_queue_t* searchresults_queue = &maud->searchresults_queue;    
    maud_queue_destroy(searchresults_queue);
    maud_scrollcontainer_destroy(&maud->queue_scrollcontainer);
    free(maud->current_musicsearch_query);
    maud->current_musicsearch_query = NULL;
    maud->current_musicsearch_querylen = 0;
    maud->search_index = 0;
    maud->searchresults_ready = false;   
}

void maud_setcurrent_searchquery_data(maud_t* maud) {
    maud_queue_t* searchresults_queue = &maud->searchresults_queue;
    char* search_query = maud->search_inputbox.input.data;
    size_t search_querylen = maud->search_inputbox.input.size;
    if(!search_query) {
        maud_clearcurrent_searchquery_data(maud);
        return;
    }
    if(!maud->current_musicsearch_query) {
        maud_clearcurrent_searchquery_data(maud);
        maud_setcurrent_searchquery(maud);
    } else if(strcmp(search_query, maud->current_musicsearch_query) != 0) {
        maud_clearcurrent_searchquery_data(maud);
        maud_setcurrent_searchquery(maud);
    }
}

void maud_setcurrent_searchquery(maud_t* maud) {
    char* search_query = maud->search_inputbox.input.data;
    size_t search_querylen = maud->search_inputbox.input.size;
    if(!search_query) {
        maud->current_musicsearch_query = NULL;
        maud->current_musicsearch_querylen = 0;
        return;
    }
    maud->current_musicsearch_query = maud_dupstr(search_query, search_querylen);
    maud->current_musicsearch_querylen = search_querylen;
}

void maud_search_music(maud_t* maud) {
    char* search_query = maud->search_inputbox.input.data;
    if(!search_query) {
        maud_clearcurrent_searchquery_data(maud);
        return;
    }
    maud_setcurrent_searchquery_data(maud);
    size_t *search_index = &maud->search_index;
    if((*search_index) < maud->music_count) {
        if(maud_music_searchsubstr(maud, *search_index)) {
            maud_queue_addmusic(&maud->searchresults_queue, 0, 0, *search_index); 
        }
        (*search_index)++;
    } else if(!maud->searchresults_ready) {
        maud_queue_print(maud, maud->searchresults_queue);
        maud->searchresults_ready = true;
    }
}

SDL_Texture* maud_rendertab(maud_t* maud, tabinfo_t* tab_info) {
    SDL_Rect text_canvas = tab_info->text_canvas;
    TTF_SetFontSize(maud->font, tab_info->font_size);
    TTF_SizeText(maud->font, tab_info->text, &text_canvas.w, &text_canvas.h);
    SDL_Surface* surface = TTF_RenderText_Blended(maud->font, tab_info->text, tab_info->text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(maud->renderer, surface);
    SDL_FreeSurface(surface);
    tab_info->text_canvas = text_canvas;
    return texture;
}

void maud_renderactive_tab(maud_t* maud, tabinfo_t* tab_info) {
    if(!tab_info->active) return;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(tab_info->underline_color));
    for(int i=0;i<UNDERLINE_THICKNESS;i++) {
        SDL_RenderDrawLine(maud->renderer, tab_info->text_canvas.x,
        tab_info->text_canvas.y+i + tab_info->text_canvas.h, tab_info->text_canvas.x+tab_info->text_canvas.w,
        tab_info->text_canvas.y+i + tab_info->text_canvas.h);
    }
}

void maud_createmusicbar(maud_t* maud/*, SDL_Texture* musicbtn_textures[]*/) {
    text_info_t current_music = {
        .font_size = 18,
        .text = NULL,
        .text_canvas = {0},
        .text_color = white,
        .utext = NULL
    };
    /*
    music name
    progress bar
    controls
    */
    maud_getcurrent_musicplaying_sizetext(maud, &current_music);
    maud_setprogressbar_size(maud);
    // create music bar
    music_status.w = maud->win_width,
    music_status.h = 50 + current_music.text_canvas.h +
        maud->progress_bar.h + music_btns[MAUD_PLAYBTN].btn_canvas.h;
    if(current_music.utext) {
        music_status.h += 10;
    }
    music_status.y = maud->win_height - music_status.h, music_status.w = maud->win_width;
    if(current_music.utext) {
        current_music.text_canvas.y = music_status.y + 10;
        maud->progress_bar.y = current_music.text_canvas.y + current_music.text_canvas.h + 20;
    };
    maud_setcontrolbtns_position(maud);
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(music_statusbar_color));
    SDL_RenderFillRect(maud->renderer, &music_status);
    SDL_RenderDrawRect(maud->renderer, &music_status);
    maud_displayprogression_control(maud);
    maud_rendercontrol_btns(maud);
}

void maud_setcontrolbtns_position(maud_t* maud) {
    playbtn_canvas = &music_btns[MAUD_PLAYBTN].btn_canvas,
    skipbtn_canvas = &music_btns[MAUD_SKIPBTN].btn_canvas,
    prevbtn_canvas = &music_btns[MAUD_PREVBTN].btn_canvas;
    shufflebtn_canvas = &music_btns[MAUD_SHUFFLEBTN].btn_canvas;
    pausebtn_canvas = &music_btns[MAUD_PAUSEBTN].btn_canvas;
    repeatoffbtn_canvas = &music_btns[MAUD_REPEATOFFBTN].btn_canvas;
    repeatallbtn_canvas = &music_btns[MAUD_REPEATALLBTN].btn_canvas;
    repeatonebtn_canvas = &music_btns[MAUD_REPEATONEBTN].btn_canvas;
    SDL_Rect container = {
        .x = 0,
        .y = maud->progress_bar.y + maud->progress_bar.h + 10,
        .w = playbtn_canvas->w * 5 + 40,
        .h = playbtn_canvas->h
    };
    container.x = (music_status.w - container.w) / 2;
    shufflebtn_canvas->x = container.x,
    shufflebtn_canvas->y = container.y;

    prevbtn_canvas->x = shufflebtn_canvas->x + shufflebtn_canvas->w + 10;
    prevbtn_canvas->y = container.y;

    playbtn_canvas->x = prevbtn_canvas->x + prevbtn_canvas->w + 10;
    playbtn_canvas->y = container.y;
    pausebtn_canvas->x = playbtn_canvas->x,
    pausebtn_canvas->y = playbtn_canvas->y;

    skipbtn_canvas->x = playbtn_canvas->x + playbtn_canvas->w + 10;
    skipbtn_canvas->y = container.y;

    repeatallbtn_canvas->x = skipbtn_canvas->x + skipbtn_canvas->w + 10,
    repeatallbtn_canvas->y = container.y;

    repeatonebtn_canvas->x = repeatallbtn_canvas->x,
    repeatonebtn_canvas->y = container.y;

    repeatoffbtn_canvas->x = repeatonebtn_canvas->x,
    repeatoffbtn_canvas->y = container.y;
}

void maud_rendercontrol_btns(maud_t* maud) {
    SDL_Texture* target_texture = NULL;
    // render all buttons on music status bar
    for(int i=0;i<MUSICBTN_COUNT;i++) {
        maud_tooltip_t musicbtn_tooltip = {
            .background_color = {0x22, 0x18, 0x1C, 0xFF},
            .text_color = {0xC5, 0xFF, 0xFD, 0xFF},
            .text = music_btns[i].tooltip_text,
            .margin_x = 10,
            .margin_y = 10,
            .x = 0,
            .y = 0,
            .element_canvas = music_btns[i].btn_canvas,
            .delay_secs = 0,
            .duration_secs = 0,
            .font = maud->music_font,
            .font_size = 18,
            .wrap_length = maud->win_width - 5,
            .wrap_spacing = 10
        };
        bool filtered_btn = (i == MAUD_PLAYBTN || i == MAUD_PAUSEBTN || i == MAUD_PREVBTN
            || i == MAUD_SKIPBTN || i == MAUD_SHUFFLEBTN);
        if(filtered_btn && maud_rect_hover(maud,
            music_btns[i].btn_canvas) && Mix_PlayingMusic()) {
            maud_setcursor(maud, MAUD_CURSOR_POINTER);
        } else if(!filtered_btn && maud_rect_hover(maud, music_btns[i].btn_canvas)) {
            maud_setcursor(maud, MAUD_CURSOR_POINTER);
        }
        maud->menu->texture_canvases[MAUD_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
        if(i == MAUD_PLAYBTN || i == MAUD_PAUSEBTN) {
            target_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE][MAUD_PLAYBTN];
            if(Mix_PlayingMusic() && !Mix_PausedMusic()) {
                target_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE][MAUD_PAUSEBTN];
                maud_tooltip_getsize(&musicbtn_tooltip);
                musicbtn_tooltip.y = music_btns[i].btn_canvas.y - musicbtn_tooltip.h;
                musicbtn_tooltip.x = music_btns[i].btn_canvas.x - ((musicbtn_tooltip.w -
                    music_btns[i].btn_canvas.w) / 2);
                maud_tooltip_renderhover(maud, &musicbtn_tooltip);
            } else if(Mix_PlayingMusic() && i == MAUD_PLAYBTN) {
               maud_tooltip_getsize(&musicbtn_tooltip);
                musicbtn_tooltip.y = music_btns[i].btn_canvas.y - musicbtn_tooltip.h;
                musicbtn_tooltip.x = music_btns[i].btn_canvas.x - ((musicbtn_tooltip.w -
                    music_btns[i].btn_canvas.w) / 2); 
                maud_tooltip_renderhover(maud, &musicbtn_tooltip);
            }
            if(music_btns[i].clicked) {
                if(Mix_PlayingMusic() && Mix_PausedMusic() && music_btns[MAUD_PLAYBTN].clicked) {
                    Mix_ResumeMusic();
                } else if(Mix_PlayingMusic() && !Mix_PausedMusic() && music_btns[MAUD_PLAYBTN].clicked) {
                    Mix_PauseMusic();
                }
                music_btns[i].clicked = false;
            }
            SDL_RenderCopy(maud->renderer, target_texture, NULL, &music_btns[i].btn_canvas);
            continue;
        } else if(i == MAUD_REPEATALLBTN || i == MAUD_REPEATONEBTN || i == MAUD_REPEATOFFBTN) {
            target_texture = maud->menu->textures[MAUD_BUTTON_TEXTURE][maud->repeat_id];
            if(i == maud->repeat_id && !maud->music_selectionmenu_addtobtn_clicked
                && !music_addplaylistbtn.clicked) {
                maud_tooltip_getsize(&musicbtn_tooltip);
                musicbtn_tooltip.y = music_btns[i].btn_canvas.y - musicbtn_tooltip.h;
                musicbtn_tooltip.x = music_btns[i].btn_canvas.x - ((musicbtn_tooltip.w -
                    music_btns[i].btn_canvas.w) / 2);
                maud_tooltip_renderhover(maud, &musicbtn_tooltip);
            }
            SDL_RenderCopy(maud->renderer, target_texture, NULL, &music_btns[maud->repeat_id].btn_canvas);
            continue;
        }
        if(Mix_PlayingMusic()) {
            maud_tooltip_getsize(&musicbtn_tooltip);
            musicbtn_tooltip.y = music_btns[i].btn_canvas.y - musicbtn_tooltip.h;
            musicbtn_tooltip.x = music_btns[i].btn_canvas.x - ((musicbtn_tooltip.w -
            music_btns[i].btn_canvas.w) / 2);         
            maud_tooltip_renderhover(maud, &musicbtn_tooltip);
        }
        SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE][i],
            NULL, &music_btns[i].btn_canvas);
    }
}

void maud_createsearch_bar(maud_t* maud) {
    maud_inputbox_t* search_inputbox = &maud->search_inputbox;
    // Calculate the music search bar positions and render the search bar to the screen
    maud->music_searchbar.w = maud->win_width - 100, maud->music_searchbar.h = 50;
    maud->music_searchbar.x = (int)roundf((float)(maud->win_width - maud->music_searchbar.w) / (float)2);
    maud->music_searchbar.y = tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + (UNDERLINE_THICKNESS + 5);
    search_inputbox->canvas = maud->music_searchbar;
    SDL_Rect* placeholder_canvas = &search_inputbox->placeholder_canvas;
    placeholder_canvas->x = search_inputbox->canvas.x + (search_inputbox->canvas.w -
        placeholder_canvas->w) / 2;
    placeholder_canvas->y = search_inputbox->canvas.y + (search_inputbox->canvas.h -
        placeholder_canvas->h) / 2;
    maud_inputbox_hover(maud, search_inputbox);
    maud_inputbox_clicked(maud, search_inputbox);
    search_inputbox->show_cursor = search_inputbox->clicked;
    /* Check whether the music search data is empty and the user hasn't clicked the search bar.
       if that condition is true then we render the placeholder within the music search bar
       otherwise
    */
    search_inputbox->placeholder_show = !search_inputbox->input.characters
        && !maud->search_inputbox.clicked;
    maud_inputbox_display(maud, search_inputbox);
}

int maud_getsize_t_length(size_t number) {
    int num_length = 1;
    while(number >= 10) {
        number /= 10;
        num_length++;
    }
    return num_length;
}

char* maud_size_t_tostring(size_t number, int* digit_count) {
    int num_length = maud_getsize_t_length(number);
    char* number_string = malloc((num_length + 1) * sizeof(char));
    if(!number_string) {
        *digit_count = 0;
        return NULL;
    }
    snprintf(number_string, num_length+1, "%zu", number);
    *digit_count = num_length;
    return number_string;
}

void maud_createsongs_box(maud_t* maud) {
    songs_box.y = maud->music_searchbar.y + maud->music_searchbar.h + 5/*tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + (UNDERLINE_THICKNESS + 5)*/;
    if(maud->tick_count) {
        songs_box.y = maud->music_selectionmenu.y + maud->music_selectionmenu.h;
    }
    songs_box.w = maud->win_width, songs_box.h = maud->win_height - music_status.h - songs_box.y;
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(songs_boxcolor));
    SDL_RenderDrawRect(maud->renderer, &songs_box);
}


void maud_getcurrent_musicplaying_sizetext(maud_t* maud, text_info_t* current_music) {
    maud_queue_t* play_queue = &maud->play_queue;
    if(!play_queue->items) {
        return;
    }
    size_t playid = play_queue->playid,
           music_listindex = play_queue->items[playid].music_listindex,
           music_id = play_queue->items[playid].music_id;
    music_t **music_lists = maud->music_lists,
            *music_list = music_lists[music_listindex];
    if(Mix_PlayingMusic()) {
        current_music->utext = music_lists[music_listindex][music_id].music_name;
        maud_textmanager_sizetext(maud->music_font, current_music);
    }
}

void maud_setprogressbar_size(maud_t* maud) {
    mtime_t curr_duration = {0}, full_duration = {0};
    maud_getduration_progression(maud, &curr_duration, &full_duration);
    text_info_t duration_texts[2] = {
        // The current duration of the music being played
        {
            .font_size = 14,
            // cast the current duration to a character array since pointers on the stack cannot be modified
            .text = (char[]){"00:00:00"},
            .text_canvas = {
                .x = music_status.x + 1,
                .y = music_status.y + 10
            },
            .text_color = white
        },
        // The final duration of music being played 
        {
            .font_size = 14,
            // cast the final duration to a character array since pointers to string literals on the stack
            // cannot be modified because they are constant
            .text = (char[]){"00:00:00"},
            .text_canvas = {
                .x = music_status.x + music_status.w,
                .y = music_status.y + 10
            },
            .text_color = white
        }
    };
    SDL_Texture* textures[2] = {0};
    sprintf(duration_texts[0].text, "%02d:%02d:%02d", curr_duration.hrs, curr_duration.mins, curr_duration.secs);
    sprintf(duration_texts[1].text, "%02d:%02d:%02d", full_duration.hrs, full_duration.mins, full_duration.secs);
    maud_textmanager_sizetext(maud->music_font, &duration_texts[0]);
    maud->progress_bar.w = maud->win_width - (duration_texts[0].text_canvas.w * 2) - 20,
    maud->progress_bar.h = duration_texts[0].text_canvas.h;
}

void maud_displaymusic_status(maud_t* maud, mtime_t curr_duration, mtime_t full_duration) {
    text_info_t duration_texts[2] = {
        // The current duration of the music being played
        {
            .font_size = 14,
            // cast the current duration to a character array since pointers on the stack cannot be modified
            .text = (char[]){"00:00:00"},
            .text_canvas = {
                .x = music_status.x + 1,
                .y = music_status.y + 20
            },
            .text_color = white
        },
        // The final duration of music being played 
        {
            .font_size = 14,
            // cast the final duration to a character array since pointers to string literals on the stack
            // cannot be modified because they are constant
            .text = (char[]){"00:00:00"},
            .text_canvas = {
                .x = music_status.x + music_status.w,
                .y = music_status.y + 20
            },
            .text_color = white
        }
    };
    SDL_Texture* textures[2] = {0};
    sprintf(duration_texts[0].text, "%02d:%02d:%02d", curr_duration.hrs, curr_duration.mins, curr_duration.secs);
    sprintf(duration_texts[1].text, "%02d:%02d:%02d", full_duration.hrs, full_duration.mins, full_duration.secs);
    for(int i=0;i<2;i++) {
        textures[i] = maud_textmanager_rendertext(maud, maud->music_font, &duration_texts[i]);
    }
    maud->progress_bar.x = duration_texts[0].text_canvas.x + duration_texts[0].text_canvas.w + 6;
    maud->progress_bar.y = music_status.y+20;
    duration_texts[1].text_canvas.x = maud->progress_bar.x + maud->progress_bar.w + 6;
    if(Mix_PlayingMusic()) {
        maud_queue_t* play_queue = &maud->play_queue;
        size_t playid = play_queue->playid,
           music_listindex = play_queue->items[playid].music_listindex,
           music_id = play_queue->items[playid].music_id;
        music_t **music_lists = maud->music_lists;
        text_info_t music_name = {18, NULL, NULL, white, {0, 0, 0, 0}};
        music_name.utext = music_lists[music_listindex][music_id].music_name;
        maud_textmanager_sizetext(maud->music_font, &music_name);
        char* truncated_name = NULL;
        if(music_name.text_canvas.x + music_name.text_canvas.w > maud->win_width) {
            truncated_name = maud_textmanager_truncatetext(maud->music_font, &music_name,
                maud->win_width);
        }
        music_name.text_canvas.y = music_status.y + 10;
        if(truncated_name) {
            char* original_name = music_name.utext;
            music_name.utext = truncated_name;
            maud_textmanager_sizetext(maud->music_font, &music_name);
            maud_tooltip_t music_nametooltip = {
                .background_color = {0},
                .delay_secs = 0.5,
                .duration_secs = 0,
                .element_canvas = music_name.text_canvas,
                .font = maud->music_font,
                .font_size = 16,
                .margin_x = 5,
                .margin_y = 10,
                .text = original_name,
                .text_color = green,
                .wrap_length = maud->win_width - 10,
                .wrap_spacing = 10,
                .x = 0,
                .y = 0,
            };
            maud_tooltip_getsize(&music_nametooltip);
            music_nametooltip.x = music_name.text_canvas.x;
            music_nametooltip.y = music_name.text_canvas.y - music_nametooltip.h;
            if(maud_rect_hover(maud, music_name.text_canvas)) {
                maud_setcursor(maud, MAUD_CURSOR_POINTER);
            }
            maud->music_tooltip = music_nametooltip;
            maud->display_musictooltip = true;
        } else {
            maud->display_musictooltip = false;
        }
        maud->progress_bar.y = music_name.text_canvas.y + music_name.text_canvas.h + 10;
        duration_texts[0].text_canvas.y = maud->progress_bar.y,
        duration_texts[1].text_canvas.y = maud->progress_bar.y;
        SDL_Texture* texture = maud_textmanager_renderunicode(maud, maud->music_font, &music_name);
        SDL_RenderCopy(maud->renderer, texture, NULL, &music_name.text_canvas);
        SDL_DestroyTexture(texture); texture = NULL;
        free(truncated_name);
        truncated_name = NULL;
   }
   for(int i=0;i<2;i++) {
        SDL_RenderCopy(maud->renderer, textures[i], NULL, &duration_texts[i].text_canvas);
        SDL_DestroyTexture(textures[i]);
   }
}

void maud_renderprogress_bar(maud_t* maud, SDL_Color bar_color, SDL_Color line_color, 
    double curr_durationsecs, double full_durationsecs) {
    double percentage = round((double)(curr_durationsecs) / (double)(full_durationsecs) * (double)100);
    int percentageof = (int)round(((double)percentage / (double)100 * (double)maud->progress_bar.w));

    maud->progress_count.x = maud->progress_bar.x;
    maud->progress_count.y = maud->progress_bar.y;
    
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(bar_color));
    SDL_RenderDrawRect(maud->renderer, &maud->progress_bar);
    SDL_RenderFillRect(maud->renderer, &maud->progress_bar);

    if(Mix_PlayingMusic()) {
        maud->progress_count.w = percentageof;
        maud->progress_count.h = maud->progress_bar.h;
        if(maud->progressbar_clicked || maud->progressbar_dragged) {
            printf("maud->progressbar_clicked = %d, mpalyer->progressbar_dragged = %d\n",
                maud->progressbar_clicked, maud->progressbar_dragged);
            // Determine what percentage of the widtth of the progress bar was clicked
            int seek_position = maud->mouse_x - maud->progress_bar.x;
            percentage = (double)seek_position / (double)maud->progress_bar.w * (double)100;
            maud->progress_count.w = seek_position;
            /* Calculate the new duration based on the portion of the progress bar that was clicked
               and then set the new music position
            */
            curr_durationsecs = round(((double)percentage / (double)100) * (double)full_durationsecs);
            Mix_SetMusicPosition(curr_durationsecs);
            maud->progressbar_clicked = false; maud->progressbar_dragged = false;
        }
        SDL_Color green = {0x00, 0xff, 0x00, 0xff};
        SDL_SetRenderDrawColor(maud->renderer, color_toparam(green));
        SDL_RenderDrawRect(maud->renderer, &maud->progress_count);
        SDL_RenderFillRect(maud->renderer, &maud->progress_count);
    }
}

void maud_controlmusic_progression(maud_t* maud) {
    mtime_t curr_duration = {0}, full_duration = {0};
    double curr_durationsecs = {0}, full_durationsecs = {0};
    int progress = 0;
    maud_queue_t* play_queue = &maud->play_queue;
    if(!play_queue->items) {
        return;
    }
    size_t *playid = &play_queue->playid,
           music_listindex = play_queue->items[*playid].music_listindex,
           music_id = play_queue->items[*playid].music_id;
    music_t **music_lists = maud->music_lists,
            *music_list = music_lists[music_listindex];
    maud_getcurrentmusic_progression(maud);
    if(Mix_PlayingMusic() && !Mix_PausedMusic()) {
        curr_duration = music_list[music_id].music_position,
        full_duration = music_list[music_id].music_duration,
        curr_durationsecs = music_list[music_id].music_positionsecs,
        full_durationsecs = music_list[music_id].music_durationsecs;
        if(Mix_PlayingMusic() && curr_durationsecs > 0.0) {
            progress = (int)(curr_durationsecs / full_durationsecs * 100.0);
        }
        printf("Playing %s %02d:%02d:%02ds of %02d:%02d:%02ds %d%% completed\n",
            music_list[music_id].music_name,
            curr_duration.hrs, curr_duration.mins, curr_duration.secs,
            full_duration.hrs, full_duration.mins, full_duration.secs, progress);
    }
    switch(maud->repeat_id) {
        case MAUD_REPEATALLBTN:
            if(play_queue->playid < play_queue->item_count && !Mix_PlayingMusic()) {
                // play the next music whwnever the current on is complete
                (*playid)++;
                (*playid) %= play_queue->item_count;
                music_listindex = play_queue->items[*playid].music_listindex,
                music_id = play_queue->items[*playid].music_id;
                music_list = music_lists[music_listindex];
                if(!Mix_PlayMusic(music_list[music_id].music, 1)) {
                    printf("Playing music %s\n", music_list[music_id].music_name);
                } else {
                    maud_songsmanager_addplayback_error(maud, music_list[music_id].music_name);
                }
            }
            break;
        case MAUD_REPEATONEBTN:
            if(!Mix_PlayingMusic()) {
                if(!Mix_PlayMusic(music_lists[music_listindex][music_id].music, 1)) {
                    printf("Playing music %s\n", music_list[music_id].music_name);
                } else {
                    // Whenever the current music object is equal to NULL then we skip to the next song that has
                    // a valid music object this can happen whenever we failed to load a music
                    printf("Failed to play music %s\n", music_list[music_id].music_name);
                    maud_songsmanager_addplayback_error(maud, music_list[music_id].music_name);
                    (*playid)++;
                    (*playid) %= play_queue->item_count;
                }
            }
            break;
        case MAUD_REPEATOFFBTN:
            if(play_queue->playid < play_queue->item_count && !Mix_PlayingMusic()) {
                (*playid)++;
                (*playid) %= play_queue->item_count;
                music_listindex = play_queue->items[*playid].music_listindex,
                music_id = play_queue->items[*playid].music_id;
                music_list = music_lists[music_listindex];
                if(!Mix_PlayMusic(music_list[music_id].music, 1)) {
                    printf("Playing music %s\n", music_list[music_id].music_name);
                } else {
                    maud_songsmanager_addplayback_error(maud, music_list[music_id].music_name);
                    printf("Failed to play music %s\n", music_list[music_id].music_name);
                }
                // Whenever we the play id gets reset to zero we Pause the music so that it doesn't repeat
                // the songs in the music list all over again
                if((*playid) == 0) {
                    Mix_PauseMusic();
                }
            }
            break;
    }
}

void maud_getcurrentmusic_progression(maud_t* maud) {
    maud_queue_t* play_queue = &maud->play_queue;
    if(!play_queue->items) {
        return;
    }
    size_t *playid = &play_queue->playid,
           music_listindex = play_queue->items[*playid].music_listindex,
           music_id = play_queue->items[*playid].music_id;
    music_t **music_lists = maud->music_lists,
            *music_list = music_lists[music_listindex];
    if(Mix_PlayingMusic()) {
        music_list[music_id].music_positionsecs = Mix_GetMusicPosition(music_list[music_id].music);
        music_list[music_id].music_position = maud_filemanager_music_gettime(music_list[music_id].
            music_positionsecs);
    }
}

void maud_getduration_progression(maud_t* maud, mtime_t* curr_duration,
    mtime_t* full_duration) {
    maud_queue_t* play_queue = &maud->play_queue;
    size_t *playid = &play_queue->playid,
            music_listindex = 0, music_id = 0;
    music_t **music_lists = maud->music_lists, *music_list = NULL;
    if(play_queue->items) {
        music_listindex = play_queue->items[*playid].music_listindex,
        music_id = play_queue->items[*playid].music_id,
        music_list = music_lists[music_listindex];
        *curr_duration = music_list[music_id].music_position,
        *full_duration = music_list[music_id].music_duration;
    }
}

void maud_displayprogression_control(maud_t* maud) {
    maud_queue_t* play_queue = &maud->play_queue;
    mtime_t curr_duration = {0}, full_duration = {0};
    double full_durationsecs = 0, curr_durationsecs = 0;
    int progress = 0;

    size_t *playid = &play_queue->playid, music_listindex = 0, music_id = 0;
    music_t **music_lists = maud->music_lists, *music_list = NULL;
    if(play_queue->items) {
        music_listindex = play_queue->items[*playid].music_listindex,
        music_id = play_queue->items[*playid].music_id,
        music_list = music_lists[music_listindex];
    
        curr_duration = music_list[music_id].music_position,
        full_duration = music_list[music_id].music_duration,
        curr_durationsecs = music_list[music_id].music_positionsecs,
        full_durationsecs = music_list[music_id].music_durationsecs;
    }
    if(maud->menu_opt == MAUD_DEFAULT_MENU) {
        maud_displaymusic_status(maud, curr_duration, full_duration);
        SDL_Color progressbar_color = {0x00, 0x00, 0x00, 0x00}, progress_linecolor = {0xFF, 0xFF, 0x00, 0xFF};
        maud_renderprogress_bar(maud, progressbar_color, progress_linecolor,
            curr_durationsecs, full_durationsecs);
    }
}

void maud_renderscroll_bar(maud_t* maud, maud_scrollbar_t* scrollbar, size_t max_textures) {
    int padding_x = scrollbar->padding_x, padding_y = scrollbar->padding_y;
    SDL_Rect* scrollbar_rect = &scrollbar->rect, scroll_area = scrollbar->scroll_area;
    double scroll_startpos = (double)scrollbar->start_pos, scroll_finalpos = (double)scrollbar->final_pos;

    // Calculate the initial position of the scrollbar
    scrollbar_rect->x = scroll_area.w - scrollbar_rect->w + padding_x,
    scrollbar_rect->y = scroll_area.y + -padding_y;
    scrollbar->displacement = (double)scrollbar_rect->y - (double)scroll_area.y;
    /* Calculate the percentage of the music we are at based on the music render position
       the music render position is incremented by 1 when scrolling down and decremented by 1when scrolling up
    */

    double percentage = (scroll_startpos / scroll_finalpos);
    if(scroll_finalpos - scroll_startpos == max_textures) {
        percentage = 1.0;
    }
    // find x percentage of the songs box height - scrollbar height - distance - 2
    double percentage_of = ((double)scroll_area.h - (double)scrollbar_rect->h + (double)padding_y -
                            scrollbar->displacement) * percentage;
    int additional_y = (int)percentage_of;
    if(additional_y > 0) {
        // increment the scrollbar y position to let it move whenever a scroll event happens
        scrollbar_rect->y += additional_y;
    }
    // Handle scroll logic whenever the user interacts with scroll bar
    if(maud->mouse_x <= scrollbar->rect.x + scrollbar->rect.w &&
        maud->mouse_x >= scrollbar->rect.x && maud_rect_hover(maud, scroll_area)) {
        if(maud->mouse_buttondown) {
            if(maud->mouse_y <= scrollbar->rect.y) {
                maud->scroll_type = MAUDSCROLL_UP;
            } else if(maud->mouse_y >= scrollbar->rect.y + scrollbar->rect.h) {
                maud->scroll_type = MAUDSCROLL_DOWN;
            }
            maud->scroll = true;
        }
    }
    // Draw the scrollbar to the screen
    SDL_SetRenderDrawColor(maud->renderer, color_toparam(white));
    SDL_RenderFillRect(maud->renderer, scrollbar_rect);
    SDL_RenderDrawRect(maud->renderer, scrollbar_rect);
}

void maud_run(maud_t* maud) {
    maud_menumanager_setup_menu(maud);
    while(!maud->quit) {
        if(maud->menu_opt == MAUD_DEFAULT_MENU) {
            maud_menumanager_setup_menu(maud);
            maud_defaultmenu(maud);
        } else if(maud->menu_opt == MAUD_SETTINGS_MENU) {
            maud_settingmenu(maud);
        }
        maud_controlmusic_progression(maud);
    }
}

void maud_defaultmenu(maud_t* maud) {
    int tab_hoverid = 0;
    int music_id = 0;
    maud_set_window_title(maud, WINDOW_TITLE);
    while(SDL_PollEvent(&maud->e)) {
        if(maud->e.type == SDL_QUIT) {
            maud->quit = 1; break;
        } else if(maud->e.type == SDL_WINDOWEVENT && maud->e.window.event == SDL_WINDOWEVENT_RESIZED) {
            maud->window_resized = true;
        }  else if(maud->e.type == SDL_TEXTINPUT) {
            if(music_addplaylistbtn.clicked) {
                maud_inputbox_handle_events(maud, &maud->playlist_inputbox);
                continue;
            } else if(maud->playlist_manager.button_bar.new_playlistbtn.clicked) {
                maud_inputbox_handle_events(maud, &maud->playlist_manager.playlist_inputbox);
            } else if(maud->playlist_manager.rename_clicked) {
                maud_inputbox_handle_events(maud, &maud->playlist_manager.rename_inputbox);
            } else if(maud->search_inputbox.clicked) {
                maud_inputbox_handle_events(maud, &maud->search_inputbox);
            }
        } else if(maud->e.type == SDL_KEYDOWN) {
            if(music_addplaylistbtn.clicked) {
                maud_inputbox_handle_events(maud, &maud->playlist_inputbox);
            } else if(maud->playlist_manager.button_bar.new_playlistbtn.clicked) {
                maud_inputbox_handle_events(maud, &maud->playlist_manager.playlist_inputbox);
            } else if(maud->playlist_manager.rename_clicked) {
                maud_inputbox_handle_events(maud, &maud->playlist_manager.rename_inputbox);
            } else if(maud->search_inputbox.clicked) {
                maud_inputbox_handle_events(maud, &maud->search_inputbox);
            }
        } else if(maud->e.type == SDL_MOUSEMOTION) {
            maud->mouse_x = maud->e.motion.x, maud->mouse_y = maud->e.motion.y;
            if(maud_buttonmanager_ibutton_hover(maud, setting_iconbtn)) {
                maud_setcursor(maud, MAUD_CURSOR_POINTER);
                setting_iconbtn.hover = true;
            } else if(maud_buttonmanager_ibutton_hover(maud, music_addfolderbtn)) {
                maud_setcursor(maud, MAUD_CURSOR_POINTER);
                music_addfolderbtn.hover = true;
            } else if(maud_tabs_hover(maud, tab_info, &tab_hoverid, tab_info_size) && TAB_INIT) {
                maud_setcursor(maud, MAUD_CURSOR_POINTER);
            } else if(maud_progressbar_hover(maud) && Mix_PlayingMusic()) {
                if(maud->mouse_buttondown) {
                    maud->progressbar_dragged = true;
                }
                maud_setcursor(maud, MAUD_CURSOR_POINTER);
            } else {
                maud_setcursor(maud, MAUD_CURSOR_DEFAULT);
                setting_iconbtn.hover = false;
            }
        } else if(maud->e.type == SDL_MOUSEWHEEL) {
            maud_scrolltype_getmousewheel_scrolltype(maud->e, &maud->scroll_type);
            maud->scroll = true;
        } else if(Mix_PlayingMusic() && maud->e.type == SDL_KEYUP) {
            if(maud->e.key.keysym.sym == SDLK_SPACE && !maud->playlist_manager.button_bar.new_playlistbtn.clicked
                && !music_addplaylistbtn.clicked && !maud->search_inputbox.clicked && !maud->playlist_manager.rename_clicked) {
                if(Mix_PausedMusic()) {
                    Mix_ResumeMusic();
                } else {
                    Mix_PauseMusic();
                }
            }
        } else if(maud->e.type == SDL_MOUSEBUTTONDOWN) {
            maud->mouse_x = maud->e.button.x, maud->mouse_y = maud->e.button.y;
            maud->mouse_buttondown = true;
        } else if(maud->e.type == SDL_MOUSEBUTTONUP) {
            maud->mouse_buttondown = false;
            maud->mouse_x = maud->e.button.x, maud->mouse_y = maud->e.button.y;
            if(maud->music_selectionmenu_addtobtn_clicked || music_addplaylistbtn.clicked) {
                maud->mouse_clicked = true;
                break;
            }
            if(maud_tabs_hover(maud, tab_info, &tab_hoverid, tab_info_size) && TAB_INIT) {
                tab_info[prev_tab].active = false;
                maud_selectionmenu_clearmusic_selection(maud);
                maud_scrollcontainer_destroy(&maud->queue_scrollcontainer);
                maud_scrollcontainer_destroy(&maud->playlist_manager.playlist_itemcontainer);
                maud_scrollcontainer_destroy(&maud->playlist_manager.playlist_container);
                maud->playlist_manager.playlistmenu_collapsex = 0;
                maud->playlist_manager.playlistmenu_collapsey = 0;
                maud->playlist_manager.playlistmenu_collapse = false;
                maud->playlist_manager.playlistmenu_scrolled = false;
                maud->playlist_manager.button_bar.new_playlistbtn.clicked = false;
                maud->playlist_manager.rename_clicked = false;
                maud_inputbox_clear(&maud->search_inputbox);
                maud_inputbox_clear(&maud->playlist_inputbox);
                maud_inputbox_clear(&maud->playlist_manager.playlist_inputbox);
                maud->search_inputbox.clicked = false;
                tab_info[tab_hoverid].underline_color = underline_color;
                tab_info[tab_hoverid].active = true;
                active_tab = tab_hoverid;
            } else if(maud_progressbar_hover(maud) && Mix_PlayingMusic()) {
                maud->progressbar_clicked = true;
            } else if(maud_buttonmanager_ibutton_hover(maud, setting_iconbtn)) {
                maud->menu_opt = MAUD_SETTINGS_MENU;
                maud_menumanager_setup_menu(maud);
                maud->mouse_clicked = false;
                return;
            } else if(maud_buttonmanager_ibutton_hover(maud, music_addfolderbtn)) {
                maud_filemanager_browsefolder(maud);
                maud->mouse_clicked = false;
                return;
            } else if(maud_buttonmanager_ibutton_hover(maud, music_btns[MAUD_PLAYBTN])) {
                // since the pause and play button will be in the same position we
                // can just check if we clicked in the position for the play button
                switch(music_btns[MAUD_PLAYBTN].clicked) {
                    case true:
                        // if we clicked in the position for the play button already
                        // that means we clicked on the pause btn
                        music_btns[MAUD_PLAYBTN].clicked = false;
                        music_btns[MAUD_PAUSEBTN].clicked = true;
                        break;
                    case false:
                        // if play button wasn't already clicked then that means we clicked on the play button
                        music_btns[MAUD_PAUSEBTN].clicked = false;
                        music_btns[MAUD_PLAYBTN].clicked = true;
                        break;

                }
            } else if(maud_buttonmanager_ibutton_hover(maud, music_btns[MAUD_PREVBTN])) {
                music_btns[MAUD_PREVBTN].clicked = true;
            } else if(maud_buttonmanager_ibutton_hover(maud, music_btns[MAUD_SKIPBTN])) {
                music_btns[MAUD_SKIPBTN].clicked = true;
            } else if(maud_buttonmanager_ibutton_hover(maud, music_btns[MAUD_REPEATALLBTN])) {
                if(maud->repeat_id == MAUD_REPEATOFFBTN) {
                    maud->repeat_id = MAUD_REPEATALLBTN;
                    break;
                }
                music_btns[maud->repeat_id++].clicked = true;
            };
            maud->mouse_clicked = true;
        }
    }

    SDL_GetWindowSize(maud->window, &maud->win_width, &maud->win_height);
    maud_set_window_color(maud->renderer, window_color);
    if(!maud->menu->textures[MAUD_TEXT_TEXTURE][0]) {
        maud->menu->textures[MAUD_TEXT_TEXTURE][0] = maud_textmanager_rendertext(maud, maud->font, &text_info[0]);
    }
    maud->menu->texture_canvases[MAUD_TEXT_TEXTURE][0] = text_info[0].text_canvas;
    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_TEXT_TEXTURE][0], NULL,
            &text_info[0].text_canvas);
    SDL_Rect tab_canvas = tab_info[0].text_canvas, text_canvas = text_info[0].text_canvas;
    tab_info[0].text_canvas.x = text_canvas.x + text_canvas.w + TAB_SPACING;
    if(!TAB_INIT) { TAB_INIT = 1; }

    for(int i=0;i<tab_info_size;i++) {
        if(!maud->menu->textures[MAUD_TAB_TEXTURE][i]) {
            maud->menu->textures[MAUD_TAB_TEXTURE][i] = maud_rendertab(maud, &tab_info[i]);
        }
        if(i > 0) {
            int tab_x = tab_info[i-1].text_canvas.x + tab_info[i-1].text_canvas.w + TAB_SPACING;
            tab_info[i].text_canvas.x = tab_x;
        }
        (tab_info[i].active) ?  maud_renderactive_tab(maud, &tab_info[i]) : 0;
        SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_TAB_TEXTURE][i], NULL,
            &tab_info[i].text_canvas);
        maud->menu->texture_canvases[MAUD_TAB_TEXTURE][i] = tab_info[i].text_canvas;
    }

    // Create add folder button on screen
    music_addfolderbtn.btn_canvas.x = maud->win_width - (setting_iconbtn.btn_canvas.w * 2) - 4;
    maud->menu->texture_canvases[MAUD_BUTTON_TEXTURE][music_addfolderbtn.texture_idx] =
        music_addfolderbtn.btn_canvas;
    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE][music_addfolderbtn.texture_idx],
        NULL, &music_addfolderbtn.btn_canvas);
    maud_tooltip_t add_folder_tooltip = {
        .background_color = {0x22, 0x18, 0x1C, 0xFF},
        .text_color = {0xC5, 0xFF, 0xFD, 0xFF},
        .text = music_addfolderbtn.tooltip_text,
        .margin_x = 10,
        .margin_y = 10,
        .x = 0,
        .y = music_addfolderbtn.btn_canvas.y,
        .element_canvas = music_addfolderbtn.btn_canvas,
        .delay_secs = 0,
        .duration_secs = 0,
        .font = maud->music_font,
        .font_size = 18,
        .wrap_length = maud->win_width-5,
        .wrap_spacing = 10,
        .w = 0,
        .h = 0
    };
    if(!maud->music_selectionmenu_addtobtn_clicked || !music_addplaylistbtn.clicked) {
        maud_tooltip_getsize(&add_folder_tooltip);
        add_folder_tooltip.x = music_addfolderbtn.btn_canvas.x - add_folder_tooltip.w;
        maud_tooltip_renderhover(maud, &add_folder_tooltip);
    }

    // Create settings button on screen
    setting_iconbtn.btn_canvas.x = maud->win_width - setting_iconbtn.btn_canvas.w - 2;
    maud->menu->texture_canvases[MAUD_BUTTON_TEXTURE][setting_iconbtn.texture_idx] =
        setting_iconbtn.btn_canvas;
    SDL_RenderCopy(maud->renderer, maud->menu->textures[MAUD_BUTTON_TEXTURE][setting_iconbtn.texture_idx],
        NULL, &setting_iconbtn.btn_canvas);
    maud_tooltip_t settings_button_tooltip = {
        .background_color = {0x22, 0x18, 0x1C, 0xFF},
        .text_color = {0xC5, 0xFF, 0xFD, 0xFF},
        .text = setting_iconbtn.tooltip_text,
        .margin_x = 10,
        .margin_y = 10,
        .x = 0,
        .y = setting_iconbtn.btn_canvas.y,
        .element_canvas = setting_iconbtn.btn_canvas,
        .delay_secs = 0,
        .duration_secs = 0,
        .font = maud->music_font,
        .font_size = 18,
        .wrap_length = maud->win_width - 5
    };
    if(!maud->music_selectionmenu_addtobtn_clicked || !music_addplaylistbtn.clicked) {
        maud_tooltip_getsize(&settings_button_tooltip);
        settings_button_tooltip.x = setting_iconbtn.btn_canvas.x - settings_button_tooltip.w;
        maud_tooltip_renderhover(maud, &settings_button_tooltip);
    }
    if(!maud->music_list) {
        maud_filemanager_loadmusics(maud);
        printf("Successfully loaded all musics\n");
        if(!maud->playlist_manager.playlists) {
            maud_playlistmanager_read_datafile(maud);
        }
    }
    /* Create music bar */
    maud_createmusicbar(maud);
    if(active_tab != PLAYLISTS_TAB && maud->playlist_manager.playlist_selected) {
        maud->playlist_manager.playlist_selected = false;
        maud->playlist_manager.rename_clicked = false;
    }

    if(active_tab == SONGS_TAB) {
        /* Create the search bar for searching for music */
        maud_createsearch_bar(maud);
        maud_search_music(maud);
        /* Create a selection menu bar that allows the user to create playlists, select a music to play next etc */
        maud_selectionmenu_create(maud);
        /* Create songs box */
        maud_createsongs_box(maud);
        // render songs so we can set the new music position based on the percentage of the music we are in
        // whether in the search result or the regular music list
        if(maud->search_inputbox.input.data) {
            maud_songsmanager_handleprevbutton(maud);
            maud_songsmanager_handleskipbutton(maud);
            maud_queue_display(maud, &maud->searchresults_queue);
        } else {
            maud_songsmanager_songstab_rendersongs(maud);
        }
        maud_selectionmenu_display_addtoplaylist_modal(maud);
        maud_selectionmenu_handle_addtoplaylist_modalevents(maud);
        maud_selectionmenu_handle_addtobtn(maud);
        if(maud->selection_queue.items && maud->music_selected) {
            //printf("The music selection queue looks like this:\n");
            //maud_queue_print(maud, maud->selection_queue);
            maud->music_selected = false;
        }
        if(music_playqueuebtn.clicked) {
            maud_queue_addmusicfrom_queue(&maud->play_queue, &maud->selection_queue);
            maud_selectionmenu_clearmusic_selection(maud);
            music_playqueuebtn.clicked = false;
        }
        maud->mouse_clicked = false;
    } else if(active_tab == QUEUES_TAB) {
        maud_createsearch_bar(maud); 
        if(maud->checkall_btntoggled) {
            maud->songsbox_resized = true;
            maud->checkall_btntoggled = false;
        }
        maud_selectionmenu_create(maud);
        maud_createsongs_box(maud);
        maud_songsmanager_handleprevbutton(maud);
        maud_songsmanager_handleskipbutton(maud);
        maud_queue_display(maud, &maud->play_queue);
        maud_selectionmenu_display_addtoplaylist_modal(maud);
        maud_selectionmenu_handle_addtoplaylist_modalevents(maud);
        maud_selectionmenu_handle_addtobtn(maud);
        if(maud->selection_queue.items && maud->music_selected) {
            //printf("The music selection queue looks like this:\n");
            //maud_queue_print(maud, maud->selection_queue);
            maud->music_selected = false;
        }
        if(music_playqueuebtn.clicked) {
            maud_queue_addmusicfrom_queue(&maud->play_queue, &maud->selection_queue);
            maud_selectionmenu_clearmusic_selection(maud);
            music_playqueuebtn.clicked = false;
        }
        //maud_queue_print(maud, maud->play_queue);
        maud->mouse_clicked = false;
    } else if(active_tab == PLAYLISTS_TAB) {
        maud_songsmanager_handleprevbutton(maud);
        maud_songsmanager_handleskipbutton(maud);
        maud_playlistmanager_display(maud);
        maud->mouse_clicked = false;
    }
    maud_notification_display(maud, &maud->notification);
    if(maud->display_musictooltip) {
        maud_tooltip_renderhover(maud, &maud->music_tooltip);
    }
    prev_tab = active_tab;
    maud_setcursor(maud, MAUD_CURSOR_DEFAULT);
    // Present all rendered objects on the screen
    SDL_RenderPresent(maud->renderer);
}

void maud_destroyapp(maud_t* maud) {
    // Write playlist data to data file
    maud_playlistmanager_write_data_tofile(maud);

    // Close the Fonts used by the program to render text
    TTF_CloseFont(maud->font);
    TTF_CloseFont(maud->music_font);

    // Release the memory back to the system that was used to create the cursors
    for(int i=0;i<sizeof(maud->cursors)/sizeof(SDL_Cursor*);i++) {
        SDL_FreeCursor(maud->cursors[i]); maud->cursors[i] = NULL;
    }

    // Free music resources used by program
    maud_filemanager_freemusic_list(maud);
    maud_filemanager_freemusicpath_info(maud);

    // Destroy the notification system
    maud_notification_destroy(&maud->notification);

    // Destroy the play queue and the selection queue resources
    maud_queue_destroy(&maud->play_queue);
    maud_queue_destroy(&maud->selection_queue);
    maud_queue_destroy(&maud->searchresults_queue);

    // Destroy inputbox
    maud_inputbox_destroy(&maud->playlist_inputbox);

    // Destroy the playlist manager an its resources
    maud_playlistmanager_destroy(maud);

    // Destroy setting menu systems
    maud_settingmenu_destroy(maud);

    // free the text informations
    maud_menumanager_menu_freetext(maud, MAUD_DEFAULT_MENU);
    maud_menumanager_menu_freetext(maud, MAUD_SETTINGS_MENU);

    // free texture objects for default menu
    maud_texturemanager_destroytextures(maud->menus[MAUD_DEFAULT_MENU].textures[MAUD_TEXT_TEXTURE],
        maud->menus[MAUD_DEFAULT_MENU].texture_sizes[MAUD_TEXT_TEXTURE]);
    maud_texturemanager_destroytextures(maud->menus[MAUD_DEFAULT_MENU].textures[MAUD_BUTTON_TEXTURE],
        maud->menus[MAUD_DEFAULT_MENU].texture_sizes[MAUD_BUTTON_TEXTURE]);

    // free texture objects for setting menu
    maud_texturemanager_destroytextures(maud->menus[MAUD_SETTINGS_MENU].textures[MAUD_TEXT_TEXTURE],
        maud->menus[MAUD_SETTINGS_MENU].texture_sizes[MAUD_TEXT_TEXTURE]);

    maud_texturemanager_destroytextures(maud->menus[MAUD_SETTINGS_MENU].textures[MAUD_BUTTON_TEXTURE],
        maud->menus[MAUD_SETTINGS_MENU].texture_sizes[MAUD_BUTTON_TEXTURE]);

    for(size_t i=0;i<MENU_COUNT;i++) {
        free(maud->menus[i].texture_canvases[MAUD_TEXT_TEXTURE]);
        free(maud->menus[i].texture_canvases[MAUD_BUTTON_TEXTURE]);
        free(maud->menus[i].texture_canvases[MAUD_TAB_TEXTURE]);

        free(maud->menus[i].textures[MAUD_TEXT_TEXTURE]);
        free(maud->menus[i].textures[MAUD_BUTTON_TEXTURE]);
        free(maud->menus[i].textures[MAUD_TAB_TEXTURE]);

        maud->menus[i].textures[MAUD_TEXT_TEXTURE] = NULL;
        maud->menus[i].textures[MAUD_BUTTON_TEXTURE] = NULL;
        maud->menus[i].textures[MAUD_TAB_TEXTURE] = NULL;

        maud->menus[i].texture_canvases[MAUD_TEXT_TEXTURE] = NULL;
        maud->menus[i].texture_canvases[MAUD_BUTTON_TEXTURE] = NULL;
        maud->menus[i].texture_canvases[MAUD_TAB_TEXTURE] = NULL;
    }

    // Destroys music player graphical utilities
    SDL_DestroyRenderer(maud->renderer);
    SDL_DestroyWindow(maud->window);

    // uninitialize libraries
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}