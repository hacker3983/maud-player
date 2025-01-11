#include "music_player.h"
#include "music_checkboxes.h"
#include "music_inputboxes.h"
#include "music_textmanager.h"
#include "music_filemanager.h"
#include "music_menumanager.h"
#include "music_texturemanager.h"
#include "music_selectionmenu.h"
#include "music_settingsmenu.h"
#include "music_playerinfo.h"
#include "music_queue.h"
#include "music_songsmanager.h"
#include "music_playlistmanager.h"
#include "music_playerbutton_manager.h"

SDL_Rect *playbtn_canvas = NULL, *playbtn_listcanvas = NULL,
        *prevbtn_canvas = NULL, *skipbtn_canvas = NULL,
        *pausebtn_canvas = NULL, *repeatoffbtn_canvas = NULL,
        *repeatallbtn_canvas = NULL, *repeatonebtn_canvas = NULL,
        *shufflebtn_canvas = NULL;

void mplayer_init() {
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

SDL_Window* mplayer_createwindow(const char* title, int width, int height) {
    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    width, height, SDL_WINDOW_RESIZABLE);
    return window;
}

SDL_Renderer* mplayer_createrenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer;
}

TTF_Font* mplayer_openfont(const char* file, int size) {
    TTF_Font* font = TTF_OpenFont(file, size);
    return font;
}

void mplayer_createapp(mplayer_t* mplayer) {
    // create music player's graphical utilities
    mplayer->menu_opt = MPLAYER_DEFAULT_MENU;
    mplayer->window = mplayer_createwindow(WINDOW_TITLE, WIDTH, HEIGHT);
    mplayer->renderer = mplayer_createrenderer(mplayer->window);
    mplayer->font = mplayer_openfont(FONT_FILE, FONT_SIZE);
    mplayer->music_font = mplayer_openfont(MUSIC_FONTFILE, MUSIC_FONTSIZE);
    mplayer->menu = &mplayer->menus[mplayer->menu_opt];
    mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
    mplayer->cursors[MPLAYER_CURSOR_DEFAULT] = SDL_GetDefaultCursor();
    mplayer->cursors[MPLAYER_CURSOR_POINTER] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    mplayer->cursors[MPLAYER_CURSOR_TYPEABLE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    mplayer->repeat_id = MUSIC_REPEATALLBTN,
    mplayer->music_selectionmenu_addto_dropdown_color = window_color;
    mplayer->playlist_manager.layout_type = PLAYLIST_GRIDVIEW;

    const char* placeholder_text = "New playlist name here...";
    SDL_Rect inputbox_canvas = {
        .x = 0, .y = 0,
        .w = 300, .h = 50
    };
    SDL_Color placeholder_color = {0xff, 0xff, 0xff, 0xff},
              input_datacolor = {0xff, 0xff, 0xff, 0xff},
              input_boxcolor = {0x00, 0x00, 0x00, 0x00},
              cursor_color = {0x00, 0xff, 0x00, 0xff};
    mplayer->playlist_inputbox = mplayer_inputbox_create(mplayer->music_font, mplayer->font,
        inputbox_canvas, placeholder_text,
        placeholder_color, input_boxcolor, input_datacolor, cursor_color);

    // Initialize the play queue and music selection queue
    mplayer_queue_init(&mplayer->play_queue);
    mplayer_queue_init(&mplayer->selection_queue);

    // create music information
    mplayer_filemanager_getmusicpath_info(mplayer);
    if(mplayer->locations == NULL) {
        #ifdef _WIN32
        wchar_t* location = NULL;
        char* username = getenv("USERNAME"), *location_str = NULL, root_path[4] = {0};
        location_str = calloc(16+strlen(username), sizeof(char));
        mplayer_filemanager_getroot_path(root_path);
        strcpy(location_str, root_path);
        strcat(location_str, "Users\\");
        strcat(location_str, username);
        strcat(location_str, "\\Music");
        location = mplayer_stringtowide(location_str);
        mplayer_filemanager_addmusic_location(mplayer, location);
        free(location_str); location_str = NULL;
        free(location); location = NULL;
        #else
        char* home = getenv("HOME"), *location = NULL;
        location = calloc(strlen(home) + 7, sizeof(char));
        strcat(location, home);
        strcat(location, "/Music");
        mplayer_addmusic_location(mplayer, location);
        free(location); location = NULL;
        #endif
    }
}

void mplayer_set_window_color(SDL_Renderer* renderer, SDL_Color bg_color) {
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderClear(renderer);
}

void mplayer_set_window_title(mplayer_t* mplayer, const char* title) {
    SDL_SetWindowTitle(mplayer->window, title);
}

void mplayer_setcursor(mplayer_t* mplayer, int cursor_type) {
    if(!mplayer->cursor_active && cursor_type != MPLAYER_CURSOR_DEFAULT) {
        mplayer->cursor = cursor_type;
        mplayer->cursor_active = true;
        return;
    }
    mplayer->cursor = cursor_type;
    mplayer->cursor_active = false;
}

void mplayer_activatecursor(mplayer_t* mplayer) {
    int cursor = mplayer->cursor;
    if(cursor < 0 || cursor >= CURSOR_COUNT) {
        return;
    }
    SDL_SetCursor(mplayer->cursors[cursor]);
    mplayer->cursor_active = false;
}

bool mplayer_tab_hover(mplayer_t* mplayer, tabinfo_t tab) {
    int x = tab.text_canvas.x, y = tab.text_canvas.y,
        w = tab.text_canvas.w, h = tab.text_canvas.h;
    if((mplayer->e.motion.x <= x + w && mplayer->e.motion.x >= x) &&
        (mplayer->e.motion.y <= y + h && mplayer->e.motion.y >= y)) {
        return true;
    }
    return false;
}

bool mplayer_rect_hover(mplayer_t* mplayer, SDL_Rect rect) {
    if((mplayer->mouse_x <= rect.x + rect.w && mplayer->mouse_x >= rect.x) &&
        (mplayer->mouse_y <= rect.y + rect.h && mplayer->mouse_y >= rect.y)) {
        return true;
    }
    return false;
}

bool mplayer_tabs_hover(mplayer_t* mplayer, tabinfo_t* tabs, int* tab_id, size_t tab_count) {
    for(size_t i=0;i<tab_count;i++) {
        if(mplayer_tab_hover(mplayer, tabs[i])) {
            if(tab_id != NULL) {
                *tab_id = tabs[i].id;
            }
            tabs[i].hover = true; return true;
        }
        tabs[i].hover = false;
    }
    return false;
}

bool mplayer_music_hover(mplayer_t* mplayer, size_t index) {
    SDL_Rect canvas = mplayer->music_list[index].outer_canvas;
    if((mplayer->mouse_x <= canvas.x + canvas.w && mplayer->mouse_x >= canvas.x) &&
        (mplayer->mouse_y <= canvas.y + canvas.h && mplayer->mouse_y >= canvas.y)) {
            return true;
    }
    return false;
}

bool mplayer_songsbox_hover(mplayer_t* mplayer) {
    if((mplayer->mouse_x <= songs_box.x + songs_box.w && mplayer->mouse_x >= songs_box.x) &&
        (mplayer->mouse_y <= songs_box.y + songs_box.h && mplayer->mouse_y >= songs_box.y)) {
            return true;
    }
    return false;
}

bool mplayer_musiclist_playbutton_hover(mplayer_t* mplayer) {
    int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y;
    SDL_Rect canvas = music_listplaybtn.btn_canvas;
    if((mouse_x <= canvas.x + canvas.w && mouse_x >= canvas.x) &&
        (mouse_y <= canvas.y + canvas.h && mouse_y >= canvas.y)) {
        return true;
    }
    return false;
}

bool mplayer_progressbar_hover(mplayer_t* mplayer) {
    int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y;
    if((mouse_x <= mplayer->progress_bar.x + mplayer->progress_bar.w && mouse_x >= mplayer->progress_bar.x) &&
        (mouse_y <= mplayer->progress_bar.y + mplayer->progress_bar.h && mouse_y >= mplayer->progress_bar.y)) {
            //printf("Progress bar hovering status is True\n");
            return true;
    }
    return false;
}

bool mplayer_searchbar_hover(mplayer_t* mplayer) {
    int mouse_x = mplayer->mouse_x, mouse_y = mplayer->mouse_y;
    if((mouse_x <= mplayer->music_searchbar.x + mplayer->music_searchbar.w && mouse_x >= mplayer->music_searchbar.x) &&
        (mouse_y <= mplayer->music_searchbar.y + mplayer->music_searchbar.h && mouse_y >= mplayer->music_searchbar.y)) {
            return true;
    }
    return false;
}

bool mplayer_music_searchsubstr(mplayer_t* mplayer, size_t search_index) {
    /* Create a copy of the music search data and the music_name so that we do not modify the contents
       of the original and convert them to lowercase / uppercase so we can make our match incase sensitive
    */
    bool match = false;
    size_t music_namelen = strlen(mplayer->music_list[search_index].music_name),
           music_querylen = mplayer->current_musicsearch_querylen;
    char *music_name = mplayer_dupstr(mplayer->music_list[search_index].music_name, music_namelen),
         *music_query = mplayer_dupstr(mplayer->current_musicsearch_query, music_querylen);
    char *match_string = (char*)mplayer_strcasestr(music_name, music_query);
    if(match_string) {
        match = true;
    }
    free(music_query); free(music_name); music_query = NULL; music_name = NULL;
    free(match_string); match_string = NULL;
    return match;
}


SDL_Texture* mplayer_rendertab(mplayer_t* mplayer, tabinfo_t* tab_info) {
    SDL_Rect text_canvas = tab_info->text_canvas;
    TTF_SetFontSize(mplayer->font, tab_info->font_size);
    TTF_SizeText(mplayer->font, tab_info->text, &text_canvas.w, &text_canvas.h);
    SDL_Surface* surface = TTF_RenderText_Blended(mplayer->font, tab_info->text, tab_info->text_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mplayer->renderer, surface);
    SDL_FreeSurface(surface);
    tab_info->text_canvas = text_canvas;
    return texture;
}

void mplayer_renderactive_tab(mplayer_t* mplayer, tabinfo_t* tab_info) {
    if(!tab_info->active) return;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(tab_info->underline_color));
    for(int i=0;i<UNDERLINE_THICKNESS;i++) {
        SDL_RenderDrawLine(mplayer->renderer, tab_info->text_canvas.x,
        tab_info->text_canvas.y+i + tab_info->text_canvas.h, tab_info->text_canvas.x+tab_info->text_canvas.w,
        tab_info->text_canvas.y+i + tab_info->text_canvas.h);
    }
}

void mplayer_createmusicbar(mplayer_t* mplayer/*, SDL_Texture* musicbtn_textures[]*/) {
    playbtn_canvas = &music_btns[MUSIC_PLAYBTN].btn_canvas,
    skipbtn_canvas = &music_btns[MUSIC_SKIPBTN].btn_canvas,
    prevbtn_canvas = &music_btns[MUSIC_PREVBTN].btn_canvas;
    shufflebtn_canvas = &music_btns[MUSIC_SHUFFLEBTN].btn_canvas;
    pausebtn_canvas = &music_btns[MUSIC_PAUSEBTN].btn_canvas;
    repeatoffbtn_canvas = &music_btns[MUSIC_REPEATOFFBTN].btn_canvas;
    repeatallbtn_canvas = &music_btns[MUSIC_REPEATALLBTN].btn_canvas;
    repeatonebtn_canvas = &music_btns[MUSIC_REPEATONEBTN].btn_canvas;
    
    // create music bar
    music_status.y = HEIGHT - music_status.h, music_status.w = WIDTH;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(music_statusbar_color));
    SDL_RenderFillRect(mplayer->renderer, &music_status);
    SDL_RenderDrawRect(mplayer->renderer, &music_status);

    // place the play, previous and skip button in the middle of the music status
    playbtn_canvas->x = (WIDTH - playbtn_canvas->w)/2, playbtn_canvas->y = HEIGHT -
        (playbtn_canvas->h + (playbtn_canvas->h/2));
    
    pausebtn_canvas->x = playbtn_canvas->x, pausebtn_canvas->y = playbtn_canvas->y;

    /*prevbtn_canvas->x = ((WIDTH - prevbtn_canvas->w)/4), prevbtn_canvas->y = HEIGHT - (prevbtn_canvas->h +
       (prevbtn_canvas->h/2));*/
    shufflebtn_canvas->x = ((WIDTH - shufflebtn_canvas->w)/2) - shufflebtn_canvas->w - 60,
    shufflebtn_canvas->y = HEIGHT - (shufflebtn_canvas->h + (shufflebtn_canvas->h/2));
    prevbtn_canvas->x = ((WIDTH - prevbtn_canvas->w)/2) - prevbtn_canvas->w - 10, prevbtn_canvas->y = HEIGHT - (prevbtn_canvas->h +
       (prevbtn_canvas->h/2));

    /*skipbtn_canvas->x = (WIDTH - (skipbtn_canvas->w))/2 + (WIDTH - skipbtn_canvas->w)/4, skipbtn_canvas->y = HEIGHT -
        (skipbtn_canvas->h + (skipbtn_canvas->h/2));*/
    skipbtn_canvas->x = ((WIDTH - (skipbtn_canvas->w))/2) + skipbtn_canvas->w + 10, skipbtn_canvas->y = HEIGHT -
        (skipbtn_canvas->h + (skipbtn_canvas->h/2));

    repeatallbtn_canvas->x = skipbtn_canvas->x + skipbtn_canvas->w + 10,
    repeatallbtn_canvas->y = skipbtn_canvas->y;

    repeatonebtn_canvas->x = repeatallbtn_canvas->x,
    repeatonebtn_canvas->y = repeatallbtn_canvas->y;

    repeatoffbtn_canvas->x = repeatonebtn_canvas->x,
    repeatoffbtn_canvas->y = repeatonebtn_canvas->y;
    SDL_Texture* target_texture = NULL;
    // render all buttons on music status bar
    for(int i=0;i<MUSICBTN_COUNT;i++) {
        mplayer_tooltip_t musicbtn_tooltip = {
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
            .font = mplayer->music_font,
            .font_size = 18,
            .wrap_length = WIDTH - 5,
            .wrap_spacing = 10
        };
        bool filtered_btn = (i == MUSIC_PLAYBTN || i == MUSIC_PAUSEBTN || i == MUSIC_PREVBTN
            || i == MUSIC_SKIPBTN || i == MUSIC_SHUFFLEBTN);
        if(filtered_btn && mplayer_rect_hover(mplayer,
            music_btns[i].btn_canvas) && Mix_PlayingMusic()) {
            mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        } else if(!filtered_btn && mplayer_rect_hover(mplayer, music_btns[i].btn_canvas)) {
            mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
        }
        mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][i] = music_btns[i].btn_canvas;
        if(i == MUSIC_PLAYBTN || i == MUSIC_PAUSEBTN) {
            target_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][MUSIC_PLAYBTN];
            if(Mix_PlayingMusic() && !Mix_PausedMusic()) {
                target_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][MUSIC_PAUSEBTN];
                mplayer_tooltip_getsize(&musicbtn_tooltip);
                musicbtn_tooltip.y = music_btns[i].btn_canvas.y - musicbtn_tooltip.h;
                musicbtn_tooltip.x = music_btns[i].btn_canvas.x - ((musicbtn_tooltip.w -
                    music_btns[i].btn_canvas.w) / 2);
                mplayer_tooltip_render(mplayer, &musicbtn_tooltip);
            } else if(Mix_PlayingMusic() && i == MUSIC_PLAYBTN) {
               mplayer_tooltip_getsize(&musicbtn_tooltip);
                musicbtn_tooltip.y = music_btns[i].btn_canvas.y - musicbtn_tooltip.h;
                musicbtn_tooltip.x = music_btns[i].btn_canvas.x - ((musicbtn_tooltip.w -
                    music_btns[i].btn_canvas.w) / 2); 
                mplayer_tooltip_render(mplayer, &musicbtn_tooltip);
            }
            if(music_btns[i].clicked) {
                if(Mix_PlayingMusic() && Mix_PausedMusic() && music_btns[MUSIC_PLAYBTN].clicked) {
                    Mix_ResumeMusic();
                } else if(Mix_PlayingMusic() && !Mix_PausedMusic() && music_btns[MUSIC_PLAYBTN].clicked) {
                    Mix_PauseMusic();
                }
                music_btns[i].clicked = false;
            }
            SDL_RenderCopy(mplayer->renderer, target_texture, NULL, &music_btns[i].btn_canvas);
            continue;
        } else if(i == MUSIC_REPEATALLBTN || i == MUSIC_REPEATONEBTN || i == MUSIC_REPEATOFFBTN) {
            target_texture = mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][mplayer->repeat_id];
            if(i == mplayer->repeat_id && !mplayer->music_selectionmenu_addtobtn_clicked
                && !music_addplaylistbtn.clicked) {
                mplayer_tooltip_getsize(&musicbtn_tooltip);
                musicbtn_tooltip.y = music_btns[i].btn_canvas.y - musicbtn_tooltip.h;
                musicbtn_tooltip.x = music_btns[i].btn_canvas.x - ((musicbtn_tooltip.w -
                    music_btns[i].btn_canvas.w) / 2);
                mplayer_tooltip_render(mplayer, &musicbtn_tooltip);
            }
            SDL_RenderCopy(mplayer->renderer, target_texture, NULL, &music_btns[mplayer->repeat_id].btn_canvas);
            continue;
        }
        if(Mix_PlayingMusic()) {
            mplayer_tooltip_getsize(&musicbtn_tooltip);
            musicbtn_tooltip.y = music_btns[i].btn_canvas.y - musicbtn_tooltip.h;
            musicbtn_tooltip.x = music_btns[i].btn_canvas.x - ((musicbtn_tooltip.w -
            music_btns[i].btn_canvas.w) / 2);         
            mplayer_tooltip_render(mplayer, &musicbtn_tooltip);
        }
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][i],
            NULL, &music_btns[i].btn_canvas);
    }
}

SDL_TimerCallback draw_cursorblink(void* m) {
    mplayer_t* mplayer = (mplayer_t*)m;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(white));
    SDL_RenderDrawRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
    SDL_RenderFillRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
}

void mplayer_createsearch_bar(mplayer_t* mplayer) {
    if(mplayer_searchbar_hover(mplayer)) {
        mplayer_setcursor(mplayer, MPLAYER_CURSOR_TYPEABLE);
    } 
    // Calculate the music search bar positions and render the search bar to the screen
    mplayer->music_searchbar.w = WIDTH - 100, mplayer->music_searchbar.h = 50;
    mplayer->music_searchbar.x = (int)roundf((float)(WIDTH - mplayer->music_searchbar.w) / (float)2);
    mplayer->music_searchbar.y = tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + (UNDERLINE_THICKNESS + 5);
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(music_searchbar_color));
    SDL_RenderDrawRect(mplayer->renderer, &mplayer->music_searchbar);

    /* Check whether the music search data is empty and the user hasn't clicked the search bar.
       if that condition is true then we render the placeholder within the music search bar
       otherwise 
    */
    text_info_t *placeholder = &text_info[1], *searchbar_data = &mplayer->musicsearchbar_datainfo;
    SDL_Texture* texture = NULL;
    if(!mplayer->musicsearchbar_data && !mplayer->musicsearchbar_clicked) {
        texture = mplayer_textmanager_rendertext(mplayer, mplayer->music_font, placeholder);
        placeholder->text_canvas.y = mplayer->music_searchbar.y + ((mplayer->music_searchbar.h -
            placeholder->text_canvas.h) / 2),
        placeholder->text_canvas.x = mplayer->music_searchbar.x + ((mplayer->music_searchbar.w -
            placeholder->text_canvas.w) / 2);
        SDL_RenderCopy(mplayer->renderer, texture, NULL, &placeholder->text_canvas);
        SDL_DestroyTexture(texture);
        return;
    }
    
    if(mplayer->musicsearchbar_data) {
        int decrease_count = 0;
        if(searchbar_data->text_canvas.w > mplayer->music_searchbar.w) {
            decrease_count = searchbar_data->text_canvas.w - mplayer->music_searchbar.w;
        }
        //printf("Decrease_count: %d\n", decrease_count);
        size_t text_size = mplayer->musicsearchbar_datalen - mplayer->musicsearchbar_datarenderpos;
        size_t j = mplayer->musicsearchbar_datarenderpos;
        //printf("text_size: %zu\n", text_size);
        char* text = calloc(text_size+1, sizeof(char));
        for(size_t i=0;i<text_size;i++) {
            if(j < mplayer->musicsearchbar_datalen) {
                text[i] = mplayer->musicsearchbar_data[j++];
            }
        }
        searchbar_data->text = text;
        searchbar_data->font_size = 18;
        searchbar_data->text_color = white;
        searchbar_data->text_canvas.x = mplayer->music_searchbar.x + 1,
        searchbar_data->utext = NULL;
        texture = mplayer_textmanager_rendertext(mplayer, mplayer->music_font, searchbar_data);
        searchbar_data->text_canvas.y = mplayer->music_searchbar.y + ((mplayer->music_searchbar.h -
            searchbar_data->text_canvas.h) / 2);
        SDL_RenderCopy(mplayer->renderer, texture, NULL, &searchbar_data->text_canvas);
        SDL_DestroyTexture(texture);
    }
    if(mplayer->musicsearchbar_clicked) {
        int char_w = 0, relpos = mplayer->musicsearchcursor_relpos;
        if(relpos > 0) {
            char* str = calloc(relpos+1, sizeof(char));
            strncpy(str, searchbar_data->text, relpos);
            TTF_SizeText(mplayer->music_font, str, &char_w, NULL);
            free(str);
        }
        mplayer->music_searchbar_cursor.w = 1, mplayer->music_searchbar_cursor.h = mplayer->music_searchbar.h / 2;
        mplayer->music_searchbar_cursor.x = mplayer->music_searchbar.x + 2 + char_w,
        mplayer->music_searchbar_cursor.y = mplayer->music_searchbar.y + ((mplayer->music_searchbar.h - mplayer->music_searchbar_cursor.h)/2);
        if(mplayer->musicsearchcursor_blink) {
            if(!mplayer->blink_timeout) {
                mplayer->blink_timeout = SDL_GetTicks() + 500;
            }
            if(SDL_TICKS_PASSED(SDL_GetTicks(), mplayer->blink_timeout)) {
                mplayer->musicsearchcursor_blink = false;
                mplayer->blink_timeout = 0;
            }
        } else {
            if(!mplayer->blink_timeout) {
                mplayer->blink_timeout = SDL_GetTicks() + 500;
            }
            SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(underline_color));
            SDL_RenderDrawRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
            SDL_RenderFillRect(mplayer->renderer, &mplayer->music_searchbar_cursor);
            if(SDL_TICKS_PASSED(SDL_GetTicks(), mplayer->blink_timeout)) {
                mplayer->blink_timeout = 0;
                mplayer->musicsearchcursor_blink = true;
            }
        }
    }
}

int mplayer_getsize_t_length(size_t number) {
    int num_length = 1;
    while(number >= 10) {
        number /= 10;
        num_length++;
    }
    return num_length;
}

char* mplayer_size_t_tostring(size_t number, int* digit_count) {
    int num_length = mplayer_getsize_t_length(number);
    char* number_string = malloc((num_length + 1) * sizeof(char));
    if(!number_string) {
        *digit_count = 0;
        return NULL;
    }
    snprintf(number_string, num_length+1, "%zu", number);
    *digit_count = num_length;
    return number_string;
}

void mplayer_createsongs_box(mplayer_t* mplayer) {
    songs_box.y = mplayer->music_searchbar.y + mplayer->music_searchbar.h + 5/*tab_info[0].text_canvas.y + tab_info[0].text_canvas.h + (UNDERLINE_THICKNESS + 5)*/;
    if(mplayer->tick_count) {
        songs_box.y = mplayer->music_selectionmenu.y + mplayer->music_selectionmenu.h;
    }
    songs_box.w = WIDTH, songs_box.h = HEIGHT - music_status.h - songs_box.y;
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(songs_boxcolor));
    SDL_RenderDrawRect(mplayer->renderer, &songs_box);
}

void mplayer_displaymusic_status(mplayer_t* mplayer, mtime_t curr_duration, mtime_t full_duration) {
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
    for(int i=0;i<2;i++) {
        textures[i] = mplayer_textmanager_rendertext(mplayer, mplayer->music_font, &duration_texts[i]);
    }
    
    mplayer->progress_bar.w = WIDTH - (duration_texts[0].text_canvas.w * 2) - 20,
    mplayer->progress_bar.x = duration_texts[0].text_canvas.x + duration_texts[0].text_canvas.w + 6;
    mplayer->progress_bar.y = music_status.y+10, mplayer->progress_bar.h = duration_texts[0].text_canvas.h;
    duration_texts[1].text_canvas.x = mplayer->progress_bar.x + mplayer->progress_bar.w + 6;
    for(int i=0;i<2;i++) {
        SDL_RenderCopy(mplayer->renderer, textures[i], NULL, &duration_texts[i].text_canvas);
        SDL_DestroyTexture(textures[i]);
    }
    music_queue_t* play_queue = &mplayer->play_queue;
    if(!play_queue->items) {
        return;
    }
    size_t play_itemindex = play_queue->play_itemindex, playid = play_queue->playid,
           playmusic_count = play_queue->items[play_queue->play_itemindex].music_count,
           music_listindex = play_queue->items[play_queue->play_itemindex].music_listindex,
           music_id = play_queue->items[play_itemindex].music_ids[playid],
           nextmusic_queueindex = play_itemindex,
           next_playid = playid+1,
           nextmusic_id = 0,
           nextmusic_listindex = 0;

    if(next_playid == playmusic_count) {
        next_playid = 0;
        nextmusic_queueindex++;
        nextmusic_queueindex %= play_queue->item_count;
        nextmusic_listindex = play_queue->items[nextmusic_queueindex].music_listindex;
    }
    nextmusic_id = play_queue->items[play_itemindex].music_ids[next_playid];
    music_t **music_lists = mplayer->music_lists,
            *music_list = music_lists[music_listindex];

    if(Mix_PlayingMusic()) {
        int repeatbtn_endpos = music_btns[mplayer->repeat_id].btn_canvas.x +
                               music_btns[mplayer->repeat_id].btn_canvas.w;
        text_info_t music_name = {18, NULL, NULL, white, {10, 0, 0, 0}},
                    nextmusic_name = {18, NULL, NULL, white, {WIDTH - 20, 0, 0, 0}};
        music_name.text_canvas.y = mplayer->progress_bar.y + mplayer->progress_bar.h + 20;
        music_name.utext = music_list[music_id].music_name;
        SDL_Texture* music_nametexture = mplayer_textmanager_renderunicode(mplayer, mplayer->music_font,
            &music_name);
        // As long as the texture returned is not NULL then we copy the texture to the renderer
        if(music_nametexture) {
            SDL_RenderCopy(mplayer->renderer, music_nametexture, NULL, &music_name.text_canvas);
            SDL_DestroyTexture(music_nametexture); music_nametexture = NULL;
        }
        nextmusic_name.utext = music_lists[nextmusic_listindex][nextmusic_id].music_name;
        SDL_Texture* nextmusic_nametexture = mplayer_textmanager_renderunicode(mplayer, mplayer->music_font,
            &nextmusic_name);
        nextmusic_name.text_canvas.x -= nextmusic_name.text_canvas.w;
        nextmusic_name.text_canvas.y = mplayer->progress_bar.y + mplayer->progress_bar.h + 20;
        if(nextmusic_nametexture) {
            SDL_RenderCopy(mplayer->renderer, nextmusic_nametexture, NULL, &nextmusic_name.text_canvas);
            SDL_DestroyTexture(nextmusic_nametexture); nextmusic_nametexture = NULL;
        }
    }
}

void mplayer_renderprogress_bar(mplayer_t* mplayer, SDL_Color bar_color, SDL_Color line_color, 
    double curr_durationsecs, double full_durationsecs) {
    double percentage = round((double)(curr_durationsecs) / (double)(full_durationsecs) * (double)100);
    int percentageof = (int)round(((double)percentage / (double)100 * (double)mplayer->progress_bar.w));

    mplayer->progress_count.x = mplayer->progress_bar.x;
    mplayer->progress_count.y = mplayer->progress_bar.y;
    
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(bar_color));
    SDL_RenderDrawRect(mplayer->renderer, &mplayer->progress_bar);
    SDL_RenderFillRect(mplayer->renderer, &mplayer->progress_bar);

    if(Mix_PlayingMusic()) {
        mplayer->progress_count.w = percentageof;
        mplayer->progress_count.h = mplayer->progress_bar.h;
        if(mplayer->progressbar_clicked || mplayer->progressbar_dragged) {
            printf("mplayer->progressbar_clicked = %d, mpalyer->progressbar_dragged = %d\n",
                mplayer->progressbar_clicked, mplayer->progressbar_dragged);
            // Determine what percentage of the widtth of the progress bar was clicked
            int seek_position = mplayer->mouse_x - mplayer->progress_bar.x;
            percentage = (double)seek_position / (double)mplayer->progress_bar.w * (double)100;
            mplayer->progress_count.w = seek_position;
            /* Calculate the new duration based on the portion of the progress bar that was clicked
               and then set the new music position
            */
            curr_durationsecs = round(((double)percentage / (double)100) * (double)full_durationsecs);
            Mix_SetMusicPosition(curr_durationsecs);
            mplayer->progressbar_clicked = false; mplayer->progressbar_dragged = false;
        }
        SDL_Color green = {0x00, 0xff, 0x00, 0xff};
        SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(green));
        SDL_RenderDrawRect(mplayer->renderer, &mplayer->progress_count);
        SDL_RenderFillRect(mplayer->renderer, &mplayer->progress_count);
    }
}

void mplayer_controlmusic_progression(mplayer_t* mplayer) {
    mtime_t curr_duration = {0}, full_duration = {0};
    double curr_durationsecs = {0}, full_durationsecs = {0};
    int progress = 0;
    music_queue_t* play_queue = &mplayer->play_queue;
    if(!play_queue->items) {
        return;
    }
    size_t *play_itemindex = &play_queue->play_itemindex, *playid = &play_queue->playid,
           music_listindex = play_queue->items[*play_itemindex].music_listindex,
           music_id = play_queue->items[*play_itemindex].music_ids[*playid],
           playmusic_count = play_queue->items[*play_itemindex].music_count;
    music_t **music_lists = mplayer->music_lists,
            *music_list = music_lists[music_listindex];
    mplayer_getcurrentmusic_progression(mplayer);
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
    switch(mplayer->repeat_id) {
        case MUSIC_REPEATALLBTN:
            if(play_queue->playid < playmusic_count && !Mix_PlayingMusic()) {
                // play the next music whwnever the current on is complete
                (*playid)++;
                if(*playid == playmusic_count) {
                    (*play_itemindex)++;
                    (*play_itemindex) %= play_queue->item_count;
                    (*playid) %= playmusic_count;
                }
                music_listindex = play_queue->items[*play_itemindex].music_listindex,
                music_id = play_queue->items[*play_itemindex].music_ids[*playid];
                music_list = music_lists[music_listindex];
                if(!Mix_PlayMusic(music_list[music_id].music, 1)) {
                    printf("Playing music %s\n", music_list[music_id].music_name);
                } else {
                    printf("Failed to play music %s\n", music_list[music_id].music_name);
                }
            }
            break;
        case MUSIC_REPEATONEBTN:
            if(!Mix_PlayingMusic()) {
                if(!Mix_PlayMusic(music_lists[music_listindex][music_id].music, 1)) {
                    printf("Playing music %s\n", mplayer->music_list[music_id].music_name);
                } else {
                    // Whenever the current music object is equal to NULL then we skip to the next song that has
                    // a valid music object this can happen whenever we failed to load a music
                    printf("Failed to play music %s\n", mplayer->music_list[music_id].music_name);
                    (*playid)++;
                    if(*playid == playmusic_count) {
                        (*play_itemindex)++;
                        (*play_itemindex) %= play_queue->item_count;
                        (*playid) %= playmusic_count;
                    }
                }
            }
            break;
        case MUSIC_REPEATOFFBTN:
            if(play_queue->playid < playmusic_count && !Mix_PlayingMusic()) {
                (*playid)++;
                if(*playid == playmusic_count) {
                    (*play_itemindex)++;
                    (*play_itemindex) %= play_queue->item_count;
                    (*playid) %= playmusic_count;
                }
                music_listindex = play_queue->items[*play_itemindex].music_listindex,
                music_id = play_queue->items[*play_itemindex].music_ids[*playid];
                music_list = music_lists[music_listindex];
                if(!Mix_PlayMusic(music_list[music_id].music, 1)) {
                    printf("Playing music %s\n", mplayer->music_list[music_id].music_name);
                } else {
                    printf("Failed to play music %s\n", mplayer->music_list[music_id].music_name);
                }
                // Whenever we the play id gets reset to zero we Pause the music so that it doesn't repeat
                // the songs in the music list all over again
                if((*playid) == 0 && (*play_itemindex) == 0) {
                    Mix_PauseMusic();
                }
            }
            break;
    }
}

void mplayer_getcurrentmusic_progression(mplayer_t* mplayer) {
    music_queue_t* play_queue = &mplayer->play_queue;
    if(!play_queue->items) {
        return;
    }
    size_t *play_itemindex = &play_queue->play_itemindex, *playid = &play_queue->playid,
           music_listindex = play_queue->items[*play_itemindex].music_listindex,
           music_id = play_queue->items[*play_itemindex].music_ids[*playid],
           playmusic_count = play_queue->items[*play_itemindex].music_count;
    music_t **music_lists = mplayer->music_lists,
            *music_list = music_lists[music_listindex];
    if(Mix_PlayingMusic()) {
        music_list[music_id].music_positionsecs = Mix_GetMusicPosition(music_list[music_id].music);
        music_list[music_id].music_position = mplayer_filemanager_music_gettime(music_list[music_id].
            music_positionsecs);
    }
}

void mplayer_displayprogression_control(mplayer_t* mplayer) {
    music_queue_t* play_queue = &mplayer->play_queue;
    mtime_t curr_duration = {0}, full_duration = {0};
    double full_durationsecs = 0, curr_durationsecs = 0;
    int progress = 0;

    size_t *play_itemindex = &play_queue->play_itemindex, *playid = &play_queue->playid,
            music_listindex = 0, music_id = 0;
    music_t **music_lists = mplayer->music_lists, *music_list = NULL;
    if(play_queue->items) {
        music_listindex = play_queue->items[*play_itemindex].music_listindex,
        music_id = play_queue->items[*play_itemindex].music_ids[*playid],
        music_list = music_lists[music_listindex];
    
        curr_duration = music_list[music_id].music_position,
        full_duration = music_list[music_id].music_duration,
        curr_durationsecs = music_list[music_id].music_positionsecs,
        full_durationsecs = music_list[music_id].music_durationsecs;
    }
    if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU) {
        mplayer_displaymusic_status(mplayer, curr_duration, full_duration);
        SDL_Color progressbar_color = {0x00, 0x00, 0x00, 0x00}, progress_linecolor = {0xFF, 0xFF, 0x00, 0xFF};
        mplayer_renderprogress_bar(mplayer, progressbar_color, progress_linecolor,
            curr_durationsecs, full_durationsecs);
    }
}

void mplayer_renderscroll_bar(mplayer_t* mplayer, mplayer_scrollbar_t* scrollbar, size_t max_textures) {
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
    if(mplayer->mouse_x <= scrollbar->rect.x + scrollbar->rect.w &&
        mplayer->mouse_x >= scrollbar->rect.x && mplayer_rect_hover(mplayer, scroll_area)) {
        if(mplayer->mouse_buttondown) {
            if(mplayer->mouse_y <= scrollbar->rect.y) {
                mplayer->scroll_type = MPLAYERSCROLL_UP;
            } else if(mplayer->mouse_y >= scrollbar->rect.y + scrollbar->rect.h) {
                mplayer->scroll_type = MPLAYERSCROLL_DOWN;
            }
            mplayer->scroll = true;
        }
    }
    // Draw the scrollbar to the screen
    SDL_SetRenderDrawColor(mplayer->renderer, color_toparam(white));
    SDL_RenderFillRect(mplayer->renderer, scrollbar_rect);
    SDL_RenderDrawRect(mplayer->renderer, scrollbar_rect);
}

void mplayer_run(mplayer_t* mplayer) {
    mplayer_menumanager_setup_menu(mplayer);
    while(!mplayer->quit) {
        if(mplayer->menu_opt == MPLAYER_DEFAULT_MENU) {
            mplayer_defaultmenu(mplayer);
        } else if(mplayer->menu_opt == MPLAYER_SETTINGS_MENU) {
            mplayer_settingmenu(mplayer);
        }
        mplayer_controlmusic_progression(mplayer);
    }
}

void mplayer_defaultmenu(mplayer_t* mplayer) {
    int tab_hoverid = 0;
    int music_id = 0;
    //SDL_GetTicks();
    mplayer_set_window_title(mplayer, WINDOW_TITLE);
    while(SDL_PollEvent(&mplayer->e)) {
        if(mplayer->e.type == SDL_QUIT) {
            mplayer->quit = 1; break;
        } else if(mplayer->e.type == SDL_WINDOWEVENT && mplayer->e.window.event == SDL_WINDOWEVENT_RESIZED) {
            mplayer->window_resized = true;
        }  else if(mplayer->e.type == SDL_TEXTINPUT) {
            if(music_addplaylistbtn.clicked) {
                mplayer_inputbox_handleinputs(mplayer, &mplayer->playlist_inputbox);
                break;
            } else if(mplayer->playlist_manager.button_bar.new_playlistbtn.clicked) {
                mplayer_inputbox_handleinputs(mplayer, &mplayer->playlist_manager.playlist_inputbox);
                break;
            }
            if(mplayer->musicsearchbar_clicked) {
                char* temp_text = NULL, *temp2_text = NULL;
                size_t text_len = strlen(mplayer->e.text.text), text2_len = 0;
                size_t music_searchbuffer_len = mplayer->musicsearchbar_datalen + text_len;
                // use malloc instead of calloc because it is faster
                char* music_searchbuffer = calloc(music_searchbuffer_len+1, sizeof(char));
                if(mplayer->musicsearchbar_data) {
                    // Get the text before the cursor position
                    if(mplayer->musicsearchcursor_relpos > 0 ||
                        mplayer->musicsearchcursor_relpos < mplayer->musicsearchbar_datalen) {
                        temp_text = malloc(mplayer->musicsearchcursor_relpos * sizeof(char));
                    }
                    // when text is found before the cursor position we store it in temp_text
                    if(temp_text) {
                        strncpy(temp_text, mplayer->musicsearchbar_data, mplayer->musicsearchcursor_relpos);
                        mplayer->musicsearchbar_data += mplayer->musicsearchcursor_relpos;
                        /* Obtain the length of the data after the cursor position if it is greater than zero
                           then we store the text that is found after the cursor position
                        */
                        text2_len = strlen(mplayer->musicsearchbar_data);
                        if(text2_len > 0) {
                            temp2_text = malloc(text2_len * sizeof(char));
                            strncpy(temp2_text, mplayer->musicsearchbar_data, text2_len);
                            //printf("%s\n", temp2_text);
                        }
                        mplayer->musicsearchbar_data -= mplayer->musicsearchcursor_relpos;
                    }
                }
                // whenever we find text before the cursor position we store it into the search data variable
                if(temp_text) {
                    strncpy(music_searchbuffer, temp_text, mplayer->musicsearchcursor_relpos);
                }
                // Insert the particular character that the user types in at a particular position
                strncat(music_searchbuffer, mplayer->e.text.text, text_len);
                // then copy the text that was after the cursor position
                if(temp2_text) {
                    strncat(music_searchbuffer, temp2_text, text2_len);
                }
                mplayer->musicsearchcursor_relpos++;
                mplayer->musicsearchbar_datalen = music_searchbuffer_len;
                mplayer->musicsearchbar_data = music_searchbuffer;
                mplayer->music_newsearch = true;
                free(temp_text); free(temp2_text);
                temp_text = NULL; temp2_text = NULL;
            }
        } else if(mplayer->e.type == SDL_KEYDOWN) {
            if(music_addplaylistbtn.clicked) {
                mplayer_inputbox_handleinputs(mplayer, &mplayer->playlist_inputbox);
                break;
            } else if(mplayer->playlist_manager.button_bar.new_playlistbtn.clicked) {
                mplayer_inputbox_handleinputs(mplayer, &mplayer->playlist_manager.playlist_inputbox);
                break;
            }
            switch(mplayer->e.key.keysym.scancode) {
                case SDL_SCANCODE_BACKSPACE:
                    if(mplayer->musicsearchbar_clicked && mplayer->musicsearchbar_datalen > 0) {
                        if(mplayer->musicsearchcursor_relpos > 0) {
                            mplayer->musicsearchbar_datalen--;
                            mplayer->musicsearchcursor_relpos--;
                            if(mplayer->musicsearchbar_datarenderpos > 0) {
                                mplayer->musicsearchbar_datarenderpos--;
                            }
                            mplayer->musicsearchbar_data[mplayer->musicsearchcursor_relpos] = 0;
                            for(size_t i=mplayer->musicsearchcursor_relpos;i<mplayer->musicsearchbar_datalen;i++) {
                                mplayer->musicsearchbar_data[i] = mplayer->musicsearchbar_data[i+1];
                            }
                            mplayer->musicsearchbar_data[mplayer->musicsearchbar_datalen] = 0;
                        }
                        if(!mplayer->musicsearchbar_datalen) {
                            free(mplayer->musicsearchbar_data); mplayer->musicsearchbar_data = NULL;
                            mplayer->music_newsearch = false;
                        } else {
                            char* newsearchbar_data = calloc(mplayer->musicsearchbar_datalen+1, sizeof(char));
                            newsearchbar_data[mplayer->musicsearchbar_datalen] = 0;
                            strcpy(newsearchbar_data, mplayer->musicsearchbar_data);
                            free(mplayer->musicsearchbar_data); mplayer->musicsearchbar_data = newsearchbar_data;
                            mplayer->music_newsearch = true;
                        }
                    }
                    break;
                case SDL_SCANCODE_RIGHT:
                    if(mplayer->musicsearchcursor_relpos < (int)mplayer->musicsearchbar_datalen) {
                        mplayer->musicsearchcursor_relpos++;
                        int w = 0, h = 0;
                        TTF_SizeText(mplayer->music_font, mplayer->musicsearchbar_data, &w, NULL);
                        if(w > mplayer->music_searchbar.w) {
                            mplayer->musicsearchbar_datarenderpos++;
                        }
                    }
                    break;
                case SDL_SCANCODE_LEFT:
                    if(mplayer->musicsearchcursor_relpos >= 1) {
                        mplayer->musicsearchcursor_relpos--;
                        int w = 0, h = 0;
                        TTF_SizeText(mplayer->music_font, mplayer->musicsearchbar_data, &w, &h);
                        if(mplayer->musicsearchbar_datarenderpos > 0) {
                            mplayer->musicsearchbar_datarenderpos--;
                        }
                    }
                    break;
            }
        } else if(mplayer->e.type == SDL_MOUSEMOTION) {
            mplayer->mouse_x = mplayer->e.motion.x, mplayer->mouse_y = mplayer->e.motion.y;
            if(mplayer_buttonmanager_ibutton_hover(mplayer, setting_iconbtn)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                setting_iconbtn.hover = true;
            } else if(mplayer_buttonmanager_ibutton_hover(mplayer, music_addfolderbtn)) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                music_addfolderbtn.hover = true;
            } else if(mplayer_tabs_hover(mplayer, tab_info, &tab_hoverid, tab_info_size) && TAB_INIT) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else if(mplayer_progressbar_hover(mplayer) && Mix_PlayingMusic()) {
                if(mplayer->mouse_buttondown) {
                    mplayer->progressbar_dragged = true;
                }
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
            } else {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_DEFAULT);
                setting_iconbtn.hover = false;
            }
        } else if(mplayer->e.type == SDL_MOUSEWHEEL) {
            mplayer_scrolltype_getmousewheel_scrolltype(mplayer->e, &mplayer->scroll_type);
            mplayer->scroll = true;
        } else if(Mix_PlayingMusic() && mplayer->e.type == SDL_KEYUP) {
            if(mplayer->e.key.keysym.sym == SDLK_SPACE && !music_addplaylistbtn.clicked && !mplayer->musicsearchbar_clicked)
                if(Mix_PausedMusic()) {
                    Mix_ResumeMusic();
                } else {
                    Mix_PauseMusic();
                }
        } else if(mplayer->e.type == SDL_MOUSEBUTTONDOWN) {
            mplayer->mouse_x = mplayer->e.button.x, mplayer->mouse_y = mplayer->e.button.y;
            mplayer->mouse_buttondown = true;
        } else if(mplayer->e.type == SDL_MOUSEBUTTONUP) {
            mplayer->mouse_buttondown = false;
            mplayer->mouse_x = mplayer->e.button.x, mplayer->mouse_y = mplayer->e.button.y;
            if(mplayer->music_selectionmenu_addtobtn_clicked || music_addplaylistbtn.clicked) {
                mplayer->mouse_clicked = true;
                break;
            }
            if(mplayer_tabs_hover(mplayer, tab_info, &tab_hoverid, tab_info_size) && TAB_INIT) {
                tab_info[prev_tab].active = false;
                tab_info[tab_hoverid].underline_color = underline_color;
                tab_info[tab_hoverid].active = true;
                active_tab = tab_hoverid;
            } else if(mplayer_progressbar_hover(mplayer) && Mix_PlayingMusic()) {
                mplayer_setcursor(mplayer, MPLAYER_CURSOR_POINTER);
                mplayer->progressbar_clicked = true;
            } else if(mplayer_buttonmanager_ibutton_hover(mplayer, setting_iconbtn)) {
                mplayer->menu_opt = MPLAYER_SETTINGS_MENU;
                mplayer_menumanager_setup_menu(mplayer);
                mplayer->mouse_clicked = false;
                return;
            } else if(mplayer_buttonmanager_ibutton_hover(mplayer, music_addfolderbtn)) {
                mplayer_filemanager_browsefolder(mplayer);
                mplayer->mouse_clicked = false;
                return;
            } else if(mplayer_buttonmanager_ibutton_hover(mplayer, music_btns[MUSIC_PLAYBTN])) {
                // since the pause and play button will be in the same position we
                // can just check if we clicked in the position for the play button
                switch(music_btns[MUSIC_PLAYBTN].clicked) {
                    case true:
                        // if we clicked in the position for the play button already
                        // that means we clicked on the pause btn
                        music_btns[MUSIC_PLAYBTN].clicked = false;
                        music_btns[MUSIC_PAUSEBTN].clicked = true;
                        break;
                    case false:
                        // if play button wasn't already clicked then that means we clicked on the play button
                        music_btns[MUSIC_PAUSEBTN].clicked = false;
                        music_btns[MUSIC_PLAYBTN].clicked = true;
                        break;

                }
            } else if(mplayer_buttonmanager_ibutton_hover(mplayer, music_btns[MUSIC_PREVBTN])) {
                music_btns[MUSIC_PREVBTN].clicked = true;
            } else if(mplayer_buttonmanager_ibutton_hover(mplayer, music_btns[MUSIC_SKIPBTN])) {
                music_btns[MUSIC_SKIPBTN].clicked = true;
            } else if(mplayer_buttonmanager_ibutton_hover(mplayer, music_btns[MUSIC_REPEATALLBTN])) {
                if(mplayer->repeat_id == MUSIC_REPEATOFFBTN) {
                    mplayer->repeat_id = MUSIC_REPEATALLBTN;
                    break;
                }
                music_btns[mplayer->repeat_id++].clicked = true;
            }
            /* Check if we have clicked in the music search bar. if the condition is true we set clicked equal to false
               Otherwise if the we haven't clicked the music search bar but another region of the screen then we lose
               focus in the search bar
            */
            if(mplayer_searchbar_hover(mplayer)) {
                mplayer->musicsearchbar_clicked = true;
            } else {
                mplayer->musicsearchbar_clicked = false;
            }
            mplayer->mouse_clicked = true;
        }
    }
    SDL_GetWindowSize(mplayer->window, &WIDTH, &HEIGHT);
    mplayer_set_window_color(mplayer->renderer, window_color);
    if(!mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][0]) {
        mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][0] = mplayer_textmanager_rendertext(mplayer, mplayer->font, &text_info[0]);
    }
    mplayer->menu->texture_canvases[MPLAYER_TEXT_TEXTURE][0] = text_info[0].text_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TEXT_TEXTURE][0], NULL,
            &text_info[0].text_canvas);
    SDL_Rect tab_canvas = tab_info[0].text_canvas, text_canvas = text_info[0].text_canvas;
    tab_info[0].text_canvas.x = text_canvas.x + text_canvas.w + TAB_SPACING;
    if(!TAB_INIT) { TAB_INIT = 1; }
    for(int i=0;i<tab_info_size;i++) {
        if(!mplayer->menu->textures[MPLAYER_TAB_TEXTURE][i]) {
            mplayer->menu->textures[MPLAYER_TAB_TEXTURE][i] = mplayer_rendertab(mplayer, &tab_info[i]);
        }
        if(i > 0) {
            int tab_x = tab_info[i-1].text_canvas.x + tab_info[i-1].text_canvas.w + TAB_SPACING;
            tab_info[i].text_canvas.x = tab_x;
        }
        (tab_info[i].active) ?  mplayer_renderactive_tab(mplayer, &tab_info[i]) : 0;
        SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_TAB_TEXTURE][i], NULL,
            &tab_info[i].text_canvas);
        mplayer->menu->texture_canvases[MPLAYER_TAB_TEXTURE][i] = tab_info[i].text_canvas;
    }

    // Create add folder button on screen
    music_addfolderbtn.btn_canvas.x = WIDTH - (setting_iconbtn.btn_canvas.w * 2) - 4;
    mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][music_addfolderbtn.texture_idx] =
        music_addfolderbtn.btn_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][music_addfolderbtn.texture_idx],
        NULL, &music_addfolderbtn.btn_canvas);
    mplayer_tooltip_t add_folder_tooltip = {
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
        .font = mplayer->music_font,
        .font_size = 18,
        .wrap_length = WIDTH-5,
        .wrap_spacing = 10,
        .w = 0,
        .h = 0
    };
    if(!mplayer->music_selectionmenu_addtobtn_clicked || !music_addplaylistbtn.clicked) {
        mplayer_tooltip_getsize(&add_folder_tooltip);
        add_folder_tooltip.x = music_addfolderbtn.btn_canvas.x - add_folder_tooltip.w;
        mplayer_tooltip_render(mplayer, &add_folder_tooltip);
    }
    // Create settings button on screen
    setting_iconbtn.btn_canvas.x = WIDTH - setting_iconbtn.btn_canvas.w - 2;
    mplayer->menu->texture_canvases[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx] =
        setting_iconbtn.btn_canvas;
    SDL_RenderCopy(mplayer->renderer, mplayer->menu->textures[MPLAYER_BUTTON_TEXTURE][setting_iconbtn.texture_idx],
        NULL, &setting_iconbtn.btn_canvas);
    mplayer_tooltip_t settings_button_tooltip = {
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
        .font = mplayer->music_font,
        .font_size = 18,
        .wrap_length = WIDTH - 5
    };
    if(!mplayer->music_selectionmenu_addtobtn_clicked || !music_addplaylistbtn.clicked) {
        mplayer_tooltip_getsize(&settings_button_tooltip);
        settings_button_tooltip.x = setting_iconbtn.btn_canvas.x - settings_button_tooltip.w;
        mplayer_tooltip_render(mplayer, &settings_button_tooltip);
    }
    if(!mplayer->music_list) {
        mplayer_filemanager_loadmusics(mplayer);
        printf("Successfully loaded all musics\n");
        mplayer_playlistmanager_read_datafile(mplayer);
        if(mplayer->musicsearchbar_data) {
            mplayer->music_newsearch = true;
        }
    }
    /* Create music bar */
    mplayer_createmusicbar(mplayer);
    mplayer_displayprogression_control(mplayer);
    if(active_tab != PLAYLISTS_TAB && mplayer->playlist_manager.playlist_selected) {
        mplayer->playlist_manager.playlist_selected = false;
    }
    if(active_tab == SONGS_TAB) {
        /* Create the search bar for searching for music */
        mplayer_createsearch_bar(mplayer);
        /* Create a selection menu bar that allows the user to create playlists, select a music to play next etc */
        mplayer_selectionmenu_create(mplayer);
        /* Create songs box */
        mplayer_createsongs_box(mplayer);
        // render songs so we can set the new music position based on the percentage of the music we are in
        // whether in the search result or the regular music list
        mplayer_songsmanager_songstab_rendersongs(mplayer);
        mplayer_selectionmenu_display_addtoplaylist_modal(mplayer);
        mplayer_selectionmenu_handle_addtoplaylist_modalevents(mplayer);
        mplayer_selectionmenu_handle_addtobtn(mplayer);
        if(mplayer->selection_queue.items && mplayer->music_selected) {
            printf("The music selection queue looks like this:\n");
            mplayer_queue_print(mplayer, mplayer->selection_queue);
            mplayer->music_selected = false;
        }
        if(music_playqueuebtn.clicked) {
            mplayer_queue_addmusicfrom_queue(&mplayer->play_queue, &mplayer->selection_queue);
            mplayer_selectionmenu_clearmusic_selection(mplayer);
            music_playqueuebtn.clicked = false;
        }
        mplayer->mouse_clicked = false;
    } else if(active_tab == QUEUES_TAB) {
        mplayer_createsearch_bar(mplayer);
        mplayer_createsongs_box(mplayer);
        mplayer_songsmanager_handleprevbutton(mplayer);
        mplayer_songsmanager_handleskipbutton(mplayer);
        mplayer_queue_display(mplayer, mplayer->play_queue);
        mplayer_queue_print(mplayer, mplayer->play_queue);
        mplayer->mouse_clicked = false;
    } else if(active_tab == PLAYLISTS_TAB) {
        mplayer_playlistmanager_display(mplayer);
        mplayer->mouse_clicked = false;
    }
    prev_tab = active_tab;
    mplayer_activatecursor(mplayer);
    // Present all rendered objects on the screen
    SDL_RenderPresent(mplayer->renderer);
}

void mplayer_destroyapp(mplayer_t* mplayer) {
    // Close the Fonts used by the program to render text
    TTF_CloseFont(mplayer->font);
    TTF_CloseFont(mplayer->music_font);

    // Release the memory back to the system that was used to create the cursors
    for(int i=0;i<sizeof(mplayer->cursors)/sizeof(SDL_Cursor*);i++) {
        SDL_FreeCursor(mplayer->cursors[i]); mplayer->cursors[i] = NULL;
    }

    // free whatever data the user types into the searchbar
    free(mplayer->musicsearchbar_data); mplayer->musicsearchbar_data = NULL;
    mplayer->musicsearchbar_datalen = 0;

    // Free music resources used by program
    mplayer_filemanager_freemusic_list(mplayer);
    mplayer_filemanager_freemusicpath_info(mplayer);

    // Destroy the play queue and the selection queue resources
    mplayer_queue_destroy(&mplayer->play_queue);
    mplayer_queue_destroy(&mplayer->selection_queue);

    // Destroy inputbox
    mplayer_inputbox_destroy(&mplayer->playlist_inputbox);

    // Destroy the playlist manager an its resources
    mplayer_playlistmanager_destroy(mplayer);

    // free the text informations
    mplayer_menumanager_menu_freetext(mplayer, MPLAYER_DEFAULT_MENU);
    mplayer_menumanager_menu_freetext(mplayer, MPLAYER_SETTINGS_MENU);

    // free texture objects for default menu
    mplayer_texturemanager_destroytextures(mplayer->menus[MPLAYER_DEFAULT_MENU].textures[MPLAYER_TEXT_TEXTURE],
        mplayer->menus[MPLAYER_DEFAULT_MENU].texture_sizes[MPLAYER_TEXT_TEXTURE]);
    mplayer_texturemanager_destroytextures(mplayer->menus[MPLAYER_DEFAULT_MENU].textures[MPLAYER_BUTTON_TEXTURE],
        mplayer->menus[MPLAYER_DEFAULT_MENU].texture_sizes[MPLAYER_BUTTON_TEXTURE]);

    // free texture objects for setting menu
    mplayer_texturemanager_destroytextures(mplayer->menus[MPLAYER_SETTINGS_MENU].textures[MPLAYER_TEXT_TEXTURE],
        mplayer->menus[MPLAYER_SETTINGS_MENU].texture_sizes[MPLAYER_TEXT_TEXTURE]);

    mplayer_texturemanager_destroytextures(mplayer->menus[MPLAYER_SETTINGS_MENU].textures[MPLAYER_BUTTON_TEXTURE],
        mplayer->menus[MPLAYER_SETTINGS_MENU].texture_sizes[MPLAYER_BUTTON_TEXTURE]);

    for(size_t i=0;i<MENU_COUNT;i++) {
        free(mplayer->menus[i].texture_canvases[MPLAYER_TEXT_TEXTURE]);
        free(mplayer->menus[i].texture_canvases[MPLAYER_BUTTON_TEXTURE]);
        free(mplayer->menus[i].texture_canvases[MPLAYER_TAB_TEXTURE]);

        free(mplayer->menus[i].textures[MPLAYER_TEXT_TEXTURE]);
        free(mplayer->menus[i].textures[MPLAYER_BUTTON_TEXTURE]);
        free(mplayer->menus[i].textures[MPLAYER_TAB_TEXTURE]);

        mplayer->menus[i].textures[MPLAYER_TEXT_TEXTURE] = NULL;
        mplayer->menus[i].textures[MPLAYER_BUTTON_TEXTURE] = NULL;
        mplayer->menus[i].textures[MPLAYER_TAB_TEXTURE] = NULL;

        mplayer->menus[i].texture_canvases[MPLAYER_TEXT_TEXTURE] = NULL;
        mplayer->menus[i].texture_canvases[MPLAYER_BUTTON_TEXTURE] = NULL;
        mplayer->menus[i].texture_canvases[MPLAYER_TAB_TEXTURE] = NULL;
    }

    // Destroys music player graphical utilities
    SDL_DestroyRenderer(mplayer->renderer);
    SDL_DestroyWindow(mplayer->window);

    // uninitialize libraries
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}