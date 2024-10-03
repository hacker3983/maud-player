#ifndef _MPLAYER_H
#define _MPLAYER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_syswm.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <errno.h>
#include <wctype.h>
#include <pthread.h>
#include "music_inputboxesdef.h"
#include "music_playerbutton_types.h"
#include "music_playertexture_types.h"
#include "music_scrollcontainers.h"
#include "music_playerscroll_types.h"
#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>
#include <commdlg.h>
#include <shlobj.h>
#include <io.h>
#include <fcntl.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#define color_toparam(color) \
    color.r, color.g, color.b, color.a
#define MENU_COUNT 2
#define TEXTURE_TYPECOUNT 3
#define CURSOR_COUNT 2

// Music time
typedef struct music_time {
    int hrs, mins, secs;
} mtime_t;

typedef struct text_info {
    int font_size;
    char *text, *utext;
    SDL_Color text_color;
    SDL_Rect text_canvas;
} text_info_t;

typedef struct mplayer_menu {
    text_info_t* texts;
    SDL_Texture** textures[TEXTURE_TYPECOUNT];
    SDL_Rect* texture_canvases[TEXTURE_TYPECOUNT];
    SDL_Rect* canvases;
    size_t texture_sizes[TEXTURE_TYPECOUNT],
           text_count, canvas_count;
} mplayer_menu_t;

// structure representing music
typedef struct music {
    char* music_name;
    #ifdef _WIN32
    char *music_alternatepath;
    wchar_t *music_path;
    #else
    char *music_path;
    #endif
    text_info_t text_info;
    SDL_Texture* text_texture;
    size_t searchmusic_id;
    Mix_Music* music;
    mtime_t music_position;
    mtime_t music_duration;
    double music_durationsecs;
    SDL_Rect outer_canvas, checkbox_size;
    bool checkbox_ticked, fill, music_playing, render, search_match, search_render, fit,
        remove;
    size_t location_index;
} music_t;

typedef struct mplayer_playlist {
    char* name;
    music_t* music_list;
    size_t music_count;
} mplayer_playlist_t;

typedef struct music_location {
    #ifdef _WIN32
    wchar_t* path;
    char* altpath;
    #else
    char* path;
    #endif
    size_t location_index;
    size_t file_count;
    SDL_Rect canvas;
    bool render, init;
    struct music_location* files;
} musloc_t;

typedef struct tab_info {
    int font_size;
    char* text;
    SDL_Color text_color, underline_color;
    SDL_Rect text_canvas;
    int id;
    bool active, hover;
} tabinfo_t;

enum TAB_ID {
        SONGS_TAB,
        ALBUMS_TAB,
        QUEUES_TAB,
        PLAYLISTS_TAB,
};

typedef struct mplayer_scrollbar {
    SDL_Rect rect;        // A rectangle representing the dimension and position x, y of the scrollbar
    int orientation;      // Orientation of the scrollbar (vertical or horizontal)
    size_t start_pos;     // The starting position of the first visible element in the scroll area
    size_t final_pos;     // The final position of the last visible element in the scroll area
    double displacement;  // The distance between the initial starting position of the scrollbar and initial
                          // starting position of the songs box
    int padding_x;        // The spacing between the left/right edge of the width of the scroll area
    int padding_y;        // the spacing between the top/bottom edge of the height of the scroll area
    SDL_Rect scroll_area;
} mplayer_scrollbar_t;

enum cursor_types {
    MPLAYER_CURSOR_TYPEABLE,
    MPLAYER_CURSOR_POINTER,
    MPLAYER_CURSOR_DEFAULT
};

// Structure representing music player's graphical utilities, music information
typedef struct mplayer {
    // Music Player Graphical Utilities
    SDL_Window* window; // window
    SDL_Renderer* renderer; // renderer
    TTF_Font* font, *music_font; // fonts
    SDL_Event e; // event
    int menu_opt, quit;
    mplayer_menu_t menus[MENU_COUNT], *menu;
    SDL_Cursor* cursors[3];
    bool window_resized;
    pthread_mutex_t mutex;
    Uint32 blink_timeout, searchthread_creationtimer;
    pthread_t search_thread;

    // Music Informations such music name, path, duration, etc
    musloc_t* locations;
    size_t location_count, total_filecount;
    music_t* music_list, *music_searchresult, *music_populatedsearch_result;
    music_t* current_music, *prev_music;
    size_t* music_searchresult_indices;
    size_t music_id, prevmusic_id, playid, searchid, music_count, music_searchresult_count, music_renderpos,
           music_searchrenderpos, music_searchresult_indicescount, music_populatedresult_count, tick_count;
    int repeat_id, mouse_x, mouse_y, scroll_type, scroll_y;
    char* musicsearchbar_data, *current_musicsearch_query, *additional_musicsearch_query;
    text_info_t musicsearchbar_datainfo;
    int musicsearchcursor_relpos;
    size_t musicsearchbar_datalen, musicsearchbar_datarenderpos, additional_musicsearch_querylen,
        music_failcount, music_maxrenderpos, match_maxrenderpos, music_renderinit,
        musicpending_removalcount, musiclocation_count, current_musicsearch_querylen,
        populate_index;
    mplayer_inputbox_t playlist_inputbox;
    mplayer_scrollcontainer_t* settingmenu_scrollcontainers;
    size_t settingmenu_scrollcontainer_index, settingmenu_scrollcontainer_count;
    bool mouse_clicked, musicsearchbar_clicked, musicsearchcursor_blink, music_hover, music_newsearch, music_playing,
         scroll, progressbar_clicked, progressbar_dragged, mouse_buttondown, music_locationremoved, music_locationadded, music_searchresult_ready,
         start_search, searchthread_created, update_searchresults,
         music_selectionmenu_checkbox_fillall,
         music_selectionmenu_checkbox_tickall,
         music_selectionmenu_checkbox_clicked,
         music_selectionmenu_addtobtn_clicked,
         music_selectionmenu_addtobtn_dropdown_clicked,
         hover_object,
         settingmenu_scrollcontainer_init;
    SDL_Color music_selectionmenu_addto_dropdown_color;
    SDL_Rect progress_bar, progress_count, music_searchbar,
        music_selectionmenu, music_selectionmenu_addtocanvas,
        music_selectionmenu_addto_dropdown,
        music_searchbar_cursor;
} mplayer_t;

void mplayer_init();
void mplayer_createsearch_bar(mplayer_t* mplayer);
void mplayer_createsongs_box(mplayer_t* mplayer);
void mplayer_createmusicbar(mplayer_t* mplayer);
void mplayer_createapp(mplayer_t* mplayer);
int mplayer_getsize_t_length(size_t number);
char* mplayer_size_t_tostring(size_t number, int* digit_count);
SDL_Window* mplayer_createwindow(const char* title, int width, int height);
SDL_Renderer* mplayer_createrenderer(SDL_Window* window);
TTF_Font* mplayer_openfont(const char* file, int size);
void mplayer_run(mplayer_t* mplayer);
void mplayer_defaultmenu(mplayer_t* mplayer);
void mplayer_setcursor(mplayer_t* mplayer, int cursor_type);
void mplayer_set_window_color(SDL_Renderer* renderer, SDL_Color bg_color);
void mplayer_set_window_title(mplayer_t* mplayer, const char* title);
bool mplayer_tab_hover(mplayer_t* mplayer, tabinfo_t tab);
bool mplayer_rect_hover(mplayer_t* mplayer, SDL_Rect rect);
bool mplayer_tabs_hover(mplayer_t* mplayer, tabinfo_t* tabs, int* tab_id, size_t tab_count);
bool mplayer_music_hover(mplayer_t* mplayer, size_t i);
bool mplayer_songsbox_hover(mplayer_t* mplayer);
bool mplayer_progressbar_hover(mplayer_t* mplayer);
bool mplayer_musiclist_playbutton_hover(mplayer_t* mplayer);
bool mplayer_music_searchsubstr(mplayer_t* mplayer, size_t search_index);
void mplayer_search_music(mplayer_t* mplayer);
void mplayer_populate_searchresults(mplayer_t* mplayer);
void* mplayer_searchthread(void* arg);
#ifdef _WIN32
wchar_t* mplayer_stringtowide(const char* string);
char* mplayer_widetoutf8(wchar_t* wstring);
#else
Uint16* mplayer_stringtouint16(char* string);
#endif
void mplayer_renderscroll_bar(mplayer_t* mplayer, mplayer_scrollbar_t* scrollbar, size_t max_textures);
void mplayer_rendersongs(mplayer_t* mplayer);
void mplayer_renderprogress_bar(mplayer_t* mplayer, SDL_Color bar_color, SDL_Color line_color,
    double curr_durationsecs, double full_durationsecs);
SDL_Texture* mplayer_rendertab(mplayer_t* mplayer, tabinfo_t* tab_info);
void mplayer_controlmusic_progression(mplayer_t* mplayer);
void mplayer_displaymusic_status(mplayer_t* mplayer, mtime_t curr_duration, mtime_t full_duration);
void mplayer_displayprogression_control(mplayer_t* mplayer);
void mplayer_renderactive_tab(mplayer_t* mplayer, tabinfo_t* tab_info);
void mplayer_centertext(mplayer_t* mplayer, text_info_t* text_info);
void mplayer_centerx(mplayer_t* mplayer, text_info_t* text_info);
void mplayer_centery(mplayer_t* mplayer, text_info_t* text_info);
void mplayer_destroyapp(mplayer_t* mplayer);
#include "music_playerinfo_extern.h"
#endif