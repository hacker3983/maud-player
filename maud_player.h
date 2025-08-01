#ifndef _MAUD_H
#define _MAUD_H
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
#include "maud_inputboxesdef.h"
#include "maud_queuedef.h"
#include "maud_notificationdef.h"
#include "maud_tooltipsdef.h"
#include "maud_playlistmanagerdef.h"
#include "maud_scrollcontainerdef.h"
#include "maud_colorpickerdef.h"
#include "maud_playerbutton_types.h"
#include "maud_playertexture_types.h"
#include "maud_playerscroll_types.h"
#include "maud_textinfodef.h"
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
#define CURSOR_COUNT 3

// Music time
typedef struct music_time {
    int hrs, mins, secs;
} mtime_t;

typedef struct maud_menu {
    text_info_t* texts;
    SDL_Texture** textures[TEXTURE_TYPECOUNT];
    SDL_Rect* texture_canvases[TEXTURE_TYPECOUNT];
    SDL_Rect* canvases;
    size_t texture_sizes[TEXTURE_TYPECOUNT],
           text_count, canvas_count;
} maud_menu_t;

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
    mtime_t music_position, music_duration;
    double music_positionsecs, music_durationsecs;
    SDL_Rect outer_canvas, checkbox_size;
    bool checkbox_ticked, fill, render, search_match, search_render, fit,
        remove;
    size_t location_index;
} music_t;

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
        QUEUES_TAB,
        PLAYLISTS_TAB,
        COLORPICKER_TAB
};

typedef struct maud_scrollbar {
    SDL_Rect rect;        // A rectangle representing the dimension and position x, y of the scrollbar
    int orientation;      // Orientation of the scrollbar (vertical or horizontal)
    size_t start_pos;     // The starting position of the first visible element in the scroll area
    size_t final_pos;     // The final position of the last visible element in the scroll area
    double displacement;  // The distance between the initial starting position of the scrollbar and initial
                          // starting position of the songs box
    int padding_x;        // The spacing between the left/right edge of the width of the scroll area
    int padding_y;        // the spacing between the top/bottom edge of the height of the scroll area
    SDL_Rect scroll_area;
} maud_scrollbar_t;

enum cursor_types {
    MAUD_CURSOR_TYPEABLE,
    MAUD_CURSOR_POINTER,
    MAUD_CURSOR_DEFAULT
};

#define EVAL_CURSOR(cursor) \
    (cursor == MAUD_CURSOR_DEFAULT) ? "DEFAULT CURSOR" : \
    (cursor == MAUD_CURSOR_POINTER) ? "POINTER CURSOR" : \
    (cursor == MAUD_CURSOR_TYPEABLE) ? "TYPEABLE CURSOR" : \
    "UNDEFINED CURSOR"

// Structure representing music player's graphical utilities, music information
typedef struct mplayer {
    // Music Player Graphical Utilities
    SDL_Window* window; // window
    int win_width, win_height;
    SDL_Renderer* renderer; // renderer
    TTF_Font* font, *music_font; // fonts
    SDL_Event e; // event
    int menu_opt, quit, cursor;
    maud_menu_t menus[MENU_COUNT], *menu;
    SDL_Cursor* cursors[3];
    bool window_resized, cursor_active;
    Uint32 blink_timeout;

    // Music Informations such music name, path, duration, etc
    musloc_t* locations;
    size_t location_count, total_filecount;

    // Notification system
    maud_notification_t notification;

    // Color picker system
    maud_colorpicker_t color_picker;

    music_t *music_list, *music_templist, *music_lists[2];
    size_t music_counts[2], music_count, music_renderpos, tick_count;
    maud_queue_t play_queue, searchresults_queue, selection_queue;
    size_t search_index;
    bool searchresults_ready;
    maud_playlistmanager_t playlist_manager;
    maud_scrollcontainer_t queue_scrollcontainer;
    bool item_selected, remove_btnclicked;

    maud_tooltip_t music_tooltip;
    bool display_musictooltip;

    int repeat_id, mouse_x, mouse_y, scroll_type, scroll_y;
    char *current_musicsearch_query;
    size_t music_failcount, music_maxrenderpos, music_renderinit,
        musicpending_removalcount, musiclocation_count, current_musicsearch_querylen;
    maud_inputbox_t playlist_inputbox, search_inputbox;
    maud_itemcontainer_t settingmenu_itemcontainer;
    maud_scrollcontainer_t settingmenu_scrollcontainer;
    size_t settingmenu_contentcount;
    bool mouse_clicked, music_hover, music_newsearch,
         scroll, progressbar_clicked, progressbar_dragged, mouse_buttondown, music_locationremoved,
         music_locationadded, music_selectionmenu_checkbox_fillall, music_selectionmenu_checkbox_tickall,
         music_selectionmenu_checkbox_clicked, music_selectionmenu_addtobtn_clicked,
         music_selectionmenu_addtobtn_dropdown_clicked, hover_object,
         music_selected, songsbox_resized, checkall_btntoggled;
    
    SDL_Color music_selectionmenu_addto_dropdown_color;
    SDL_Rect progress_bar, progress_count, music_searchbar,
        music_selectionmenu, music_selectionmenu_addtocanvas,
        music_selectionmenu_addto_dropdown, addtoplaylist_modalcanvases[3];
} maud_t;

void maud_init();
void maud_createsearch_bar(maud_t* mplayer);
bool maud_music_searchsubstr(maud_t* mplayer, size_t search_index);
void maud_setcurrent_searchquery(maud_t* mplayer);
void maud_setcurrent_searchquery_data(maud_t* mplayer);
void maud_clearcurrent_searchquery_data(maud_t* mplayer);
void maud_search_music(maud_t* mplayer);
void maud_createsongs_box(maud_t* mplayer);
void maud_setcontrolbtns_position(maud_t* mplayer);
void maud_setprogressbar_size(maud_t* mplayer);
void maud_rendercontrol_btns(maud_t* mplayer);
void maud_getduration_progression(maud_t* mplayer, mtime_t* curr_duration,
    mtime_t* full_duration);
void maud_getcurrent_musicplaying_sizetext(maud_t* mplayer, text_info_t* current_music);
void maud_rendercontrol_btns(maud_t* mplayer);
void maud_createmusicbar(maud_t* mplayer);
void maud_createapp(maud_t* mplayer);
int maud_getsize_t_length(size_t number);
char* maud_size_t_tostring(size_t number, int* digit_count);
SDL_Window* maud_createwindow(const char* title, int width, int height);
SDL_Renderer* maud_createrenderer(SDL_Window* window);
TTF_Font* maud_openfont(const char* file, int size);
void maud_run(maud_t* mplayer);
void maud_defaultmenu(maud_t* mplayer);
void maud_setcursor(maud_t* mplayer, int cursor_type);
void maud_set_window_color(SDL_Renderer* renderer, SDL_Color bg_color);
void maud_set_window_title(maud_t* mplayer, const char* title);
bool maud_tab_hover(maud_t* mplayer, tabinfo_t tab);
bool maud_rect_hover(maud_t* mplayer, SDL_Rect rect);
bool maud_tabs_hover(maud_t* mplayer, tabinfo_t* tabs, int* tab_id, size_t tab_count);
bool maud_music_hover(maud_t* mplayer, size_t i);
bool maud_songsbox_hover(maud_t* mplayer);
bool maud_progressbar_hover(maud_t* mplayer);
bool maud_musiclist_playbutton_hover(maud_t* mplayer);
#ifdef _WIN32
wchar_t* maud_stringtowide(const char* string);
char* maud_widetoutf8(wchar_t* wstring);
#else
Uint16* maud_stringtouint16(char* string);
#endif
void maud_renderscroll_bar(maud_t* mplayer, maud_scrollbar_t* scrollbar, size_t max_textures);
void maud_renderprogress_bar(maud_t* mplayer, SDL_Color bar_color, SDL_Color line_color,
    double curr_durationsecs, double full_durationsecs);
SDL_Texture* maud_rendertab(maud_t* mplayer, tabinfo_t* tab_info);
void maud_getcurrentmusic_progression(maud_t* mplayer);
void maud_controlmusic_progression(maud_t* mplayer);
void maud_setmusicname_position(maud_t* mplayer, text_info_t* current_music,
    text_info_t* next_musicname);
void maud_displaymusic_status(maud_t* mplayer, mtime_t curr_duration, mtime_t full_duration);
void maud_displayprogression_control(maud_t* mplayer);
void maud_renderactive_tab(maud_t* mplayer, tabinfo_t* tab_info);
void maud_centertext(maud_t* mplayer, text_info_t* text_info);
void maud_centerx(maud_t* mplayer, text_info_t* text_info);
void maud_centery(maud_t* mplayer, text_info_t* text_info);
void maud_destroyapp(maud_t* mplayer);
#include "maud_playerinfo_extern.h"
#endif