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

enum file_extensions {
        MPLAYER_MP3EXT,
        MPLAYER_FLACEXT,
        MPLAYER_OGGEXT,
        MPLAYER_OPUSEXT,
        MPLAYER_WAVEXT,
        MPLAYER_M4AEXT
};

typedef struct text_info {
    int font_size;
    char *text, *utext;
    SDL_Color text_color;
    SDL_Rect text_canvas;
} text_info_t;

// Music time
typedef struct music_time {
    int hrs, mins, secs;
} mtime_t;

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

typedef struct image_buttons {
    char* imgbtn_path;
    char* tooltip_text;
    int id;
    SDL_Rect btn_canvas;
    bool hover, clicked;
    int texture_idx;
} ibtn_t;

typedef struct text_buttons {
    int font_size;
    char* text;
    SDL_Color text_color;
    SDL_Rect btn_canvas;
    int id, hover;
} tbtn_t;

enum musical_buttons {
    MUSIC_PLAYBTN,
    MUSIC_SHUFFLEBTN,
    MUSIC_SKIPBTN,
    MUSIC_PREVBTN,
    MUSIC_PAUSEBTN,
    MUSIC_REPEATALLBTN,
    MUSIC_REPEATONEBTN,
    MUSIC_REPEATOFFBTN,
    MUSIC_LISTPLAYBTN,
    MUSIC_ADDFOLDERBTN,
    MUSIC_ADDPLAYLISTBTN,
    MUSIC_ADDTOBTN,
    MUSIC_REMOVEBTN
};

enum setting_buttons {
    SETTING_BUTTON,
    BACK_BUTTON
};

enum TAB_ID {
        SONGS_TAB,
        ALBUMS_TAB,
        QUEUES_TAB,
        PLAYLISTS_TAB,
};

enum menu_options {
    MPLAYER_DEFAULT_MENU,
    MPLAYER_SETTINGS_MENU
};
enum texture_types {
    MPLAYER_TEXT_TEXTURE,
    MPLAYER_TAB_TEXTURE,
    MPLAYER_BUTTON_TEXTURE
};

typedef struct music_checkbox_info {
    SDL_Rect checkbox_canvas;
    SDL_Color fill_color; // checkbox COlor
    SDL_Color box_color; // Checkbox Color
    SDL_Color tk_color;
    bool tick, fill;
} mcheckbox_t;

typedef struct mplayer_tooltip {
    char* text;                 // The text that will be displayed in the tooltip
    int x, y;                   // The horizontal (x) and vertical (y) position of the tooltip when the element is hovered
                                // by the user (Note the x and y positions will be always relative to the position of
                                // the element when hovered by the user)
    int margin_x, margin_y;     // margin_x represents the amount we should center the text within the tooltip canvas
                                // horizontally while margin_y represents the amount we should center the text within
                                // the tooltip canvas vertically
    SDL_Rect element_canvas;    // The area of the button or icon that the tooltip will be displayed for
    SDL_Color background_color; // background color for the tooltip
    SDL_Color text_color;       // text color for the tooltip
    float delay_secs;           // A time in seconds that it will take to start displaying the tooltip on screen 
    float duration_secs;        // A duration or how long the tooltip should be on screen for (If a time of zero is
                                // specified then the tooltip will display for ever)
    int font_size;              // The font size of the text that should be displayed within the tooltip
    TTF_Font* font;             // The font that should be used to render text within the tooltip
} mplayer_tooltip_t;

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

typedef struct mplayer_menu {
    text_info_t* texts;
    SDL_Texture** textures[TEXTURE_TYPECOUNT];
    SDL_Rect* texture_canvases[TEXTURE_TYPECOUNT];
    SDL_Rect* canvases;
    size_t texture_sizes[TEXTURE_TYPECOUNT], text_count, canvas_count;
} mplayer_menu_t;

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
    mplayer_scrollcontainer_t* settingmenu_scrollcontainers;
    size_t settingmenu_scrollcontainer_index, settingmenu_scrollcontainer_count;
    bool mouse_clicked, musicsearchbar_clicked, musicsearchcursor_blink, music_hover, music_newsearch, music_playing,
         scroll, progressbar_clicked, progressbar_dragged, mouse_buttondown, music_locationremoved, music_locationadded, music_searchresult_ready,
         start_search, searchthread_created, update_searchresults,
         music_selectionmenu_checkbox_fillall,
         music_selectionmenu_checkbox_tickall,
         music_selectionmenu_checkbox_clicked,
         hover_object,
         settingmenu_scrollcontainer_init;
    SDL_Rect progress_bar, progress_count, music_searchbar,
        music_selectionmenu, music_searchbar_cursor;
} mplayer_t;

void mplayer_init();
void mplayer_setup_menu(mplayer_t* mplayer);
void mplayer_createsearch_bar(mplayer_t* mplayer);
void mplayer_createsongs_box(mplayer_t* mplayer);
void mplayer_createmusicbar(mplayer_t* mplayer);
void mplayer_createapp(mplayer_t* mplayer);
void mplayer_getmusic_locations(mplayer_t* mplayer);
void mplayer_getmusic_filepaths(mplayer_t* mplayer);
void mplayer_getmusicpath_info(mplayer_t* mplayer);
int mplayer_getsize_t_length(size_t number);
char* mplayer_size_t_tostring(size_t number, int* digit_count);
char* mplayer_getmusic_namefrompath(Mix_Music* music, void* path);
void mplayer_copymusicinfo_fromsearchindex(mplayer_t* mplayer, size_t index, music_t* music_info);
void mplayer_loadmusics(mplayer_t* mplayer);
void mplayer_browsefolder(mplayer_t* mplayer);
void mplayer_addmusic_location(mplayer_t* mplayer, void* locationv);
void mplayer_setmusic_removestatus(mplayer_t* mplayer, size_t index);
void mplayer_removemusics_pendingremoval(mplayer_t* mplayer);
void mplayer_delmusic_locationindex(mplayer_t* mplayer, size_t loc_index);
SDL_Window* mplayer_createwindow(const char* title, int width, int height);
SDL_Renderer* mplayer_createrenderer(SDL_Window* window);
TTF_Font* mplayer_openfont(const char* file, int size);
mtime_t mplayer_music_gettime(double seconds);
bool mplayer_musiclocation_exists(mplayer_t* mplayer, void* locationv);
void mplayer_freemusic(music_t* music_ref);
void mplayer_freemusic_searchresult(music_t** music_results, size_t* music_resultcount);
void mplayer_freemusic_list(mplayer_t* mplayer);
void mplayer_freemusicpath_info(mplayer_t* mplayer);
void mplayer_run(mplayer_t* mplayer);
void mplayer_defaultmenu(mplayer_t* mplayer);
void mplayer_setcursor(mplayer_t* mplayer, int cursor_type);
void mplayer_set_window_color(SDL_Renderer* renderer, SDL_Color bg_color);
void mplayer_set_window_title(mplayer_t* mplayer, const char* title);
bool mplayer_tab_hover(mplayer_t* mplayer, tabinfo_t tab);
bool mplayer_rect_hover(mplayer_t* mplayer, SDL_Rect rect);
bool mplayer_ibutton_hover(mplayer_t* mplayer, ibtn_t button);
bool mplayer_tbutton_hover(mplayer_t* mplayer, tbtn_t button);
bool mplayer_tabs_hover(mplayer_t* mplayer, tabinfo_t* tabs, int* tab_id, size_t tab_count);
bool mplayer_ibuttons_hover(mplayer_t* mplayer, ibtn_t* buttons, int* btn_id, size_t button_count);
bool mplayer_tbuttons_hover(mplayer_t* mplayer, tbtn_t* buttons, int* btn_id, size_t button_count);
bool mplayer_music_hover(mplayer_t* mplayer, size_t i);
bool mplayer_songsbox_hover(mplayer_t* mplayer);
bool mplayer_progressbar_hover(mplayer_t* mplayer);
bool mplayer_musiclist_playbutton_hover(mplayer_t* mplayer);
bool mplayer_checkbox_hovered(mplayer_t* mplayer);
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
char* mplayer_stringtolower(char** string, size_t len);
wchar_t* mplayer_widetolower(wchar_t** wstring, size_t wlen);
char* mplayer_dupstr(char* string, size_t len);
char* mplayer_strcasestr(char* haystack, char* needle);
void mplayer_drawcheckbox(mplayer_t* mplayer, mcheckbox_t* checkbox_info);
void mplayer_drawmusic_checkbox(mplayer_t* mplayer, SDL_Color box_color,
    SDL_Color fill_color, bool fill, SDL_Color tick_color, bool check);
void mplayer_rendertooltip(mplayer_t* mplayer, mplayer_tooltip_t* tooltip);
void mplayer_renderscroll_bar(mplayer_t* mplayer, mplayer_scrollbar_t* scrollbar, size_t max_textures);
void mplayer_rendersongs(mplayer_t* mplayer);
void mplayer_renderprogress_bar(mplayer_t* mplayer, SDL_Color bar_color, SDL_Color line_color,
    double curr_durationsecs, double full_durationsecs);
int mplayer_get_textsize(TTF_Font* font, text_info_t* utext_info);
SDL_Texture* mplayer_rendertext(mplayer_t* mplayer, TTF_Font* font, text_info_t* text_info);
SDL_Texture* mplayer_renderunicode_text(mplayer_t* mplayer, TTF_Font* font, text_info_t* utext_info);
SDL_Texture* mplayer_rendertab(mplayer_t* mplayer, tabinfo_t* tab_info);
void mplayer_controlmusic_progression(mplayer_t* mplayer);
void mplayer_displaymusic_status(mplayer_t* mplayer, mtime_t curr_duration, mtime_t full_duration);
void mplayer_displayprogression_control(mplayer_t* mplayer);
void mplayer_renderactive_tab(mplayer_t* mplayer, tabinfo_t* tab_info);
void mplayer_centertext(mplayer_t* mplayer, text_info_t* text_info);
void mplayer_centerx(mplayer_t* mplayer, text_info_t* text_info);
void mplayer_centery(mplayer_t* mplayer, text_info_t* text_info);
void mplayer_menu_appendtext(mplayer_t* mplayer, text_info_t text);
SDL_Texture** mplayer_createtexture_list(size_t amount);
void mplayer_realloctexture(SDL_Texture*** texture_objs, size_t* amount);
void mplayer_createmenu_texture(mplayer_t* mplayer, int type, size_t amount);
void mplayer_addmenu_texture(mplayer_t* mplayer, int type);
void mplayer_menuplace_texture(mplayer_t* mplayer, int type, SDL_Texture* texture, SDL_Rect canvas);
void mplayer_menuadd_canvas(mplayer_t* mplayer, SDL_Rect canvas);
void mplayer_menu_freetext(mplayer_t* mplayer, int menu_option);
void mplayer_destroytextures(SDL_Texture** textures, size_t n);
void mplayer_destroyapp(mplayer_t* mplayer);

#ifdef _WIN32
void mplayer_getroot_path(char* root_path);
#endif
#include "music_playerinfo_extern.h"
#endif