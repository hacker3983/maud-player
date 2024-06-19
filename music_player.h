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

extern const char* WINDOW_TITLE, *SETTING_TITLE, *FONT_FILE, *MUSIC_FONTFILE, *MUSIC_PATHINFO_FILE;
extern int WIDTH, HEIGHT, FONT_SIZE, MUSIC_FONTSIZE, TAB_INIT, active_tab, prev_tab, TAB_SPACING, SBOXDISTANCE_X,  SETTING_LINESPACING,
UNDERLINE_THICKNESS, checkbox_init;
extern SDL_Rect scrollbar, music_status, songs_box, checkbox_size;
extern const size_t text_info_size, tab_info_size, setting_textinfo_size, MUSICBTN_COUNT, MTOTALBTN_COUNT, SETTINGSBTN_COUNT;
extern const SDL_Color window_color, text_color, text2_color, underline_color, white, black, music_statusbar_color,
music_searchbar_color, songs_boxcolor, setting_wincolor, setting_textcolor, back_btnbg_color, setting_underlinecolor;

#ifdef _WIN32
extern const wchar_t* FILE_EXTENSIONS[];
#else
extern const char* FILE_EXTENSIONS[];
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
    char* text;
    #ifdef _WIN32
    wchar_t* utext;
    #else
    char* utext;
    #endif
    SDL_Color text_color;
    SDL_Rect text_canvas;
} text_info_t;

// Music time
typedef struct music_time {
    int hrs, mins, secs;
} mtime_t;

// structure representing music
typedef struct music {
    #ifdef _WIN32
    char *music_alternatepath;
    #endif
    #ifdef _WIN32
    wchar_t* music_name, *music_path;
    #else
    char* music_name, *music_path;
    #endif
    text_info_t text_info;
    SDL_Texture* text_texture;
    size_t searchmusic_id;
    Mix_Music* music;
    mtime_t music_position;
    mtime_t music_duration;
    double music_durationsecs;
    SDL_Rect outer_canvas, checkbox_size;
    int scroll_y;
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

enum scroll_types {
    MPLAYERSCROLL_UP,
    MPLAYERSCROLL_DOWN,
    MPLAYERSCROLL_RIGHT,
    MPLAYERSCROLL_LEFT
};

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

    // Music Informations such music name, path, duration, etc
    musloc_t* locations;
    size_t location_count, total_filecount;
    music_t* music_list, *music_searchresult;
    music_t* current_music, *prev_music;
    size_t music_id, prevmusic_id, playid, searchid, music_count, music_searchresult_count, music_renderpos,
           music_searchrenderpos;
    int repeat_id, mouse_x, mouse_y, tick_count, scroll_type;
    char* musicsearchbar_data;
    text_info_t musicsearchbar_datainfo;
    int musicsearchcursor_relpos;
    size_t musicsearchbar_datalen, musicsearchbar_datarenderpos,
        music_failcount, music_maxrenderpos, match_maxrenderpos, music_renderinit,
        musicpending_removalcount, musiclocation_count;
    bool mouse_clicked, musicsearchbar_clicked, musicsearchcursor_blink, music_hover, music_newsearch, music_playing,
         scroll, progressbar_clicked, music_locationremoved, music_locationadded;
    SDL_Rect progress_bar, progress_count, music_searchbar, music_searchbar_cursor;
} mplayer_t;

void mplayer_init();
SDL_Window* mplayer_createwindow(const char* title, int width, int height);
SDL_Renderer* mplayer_createrenderer(SDL_Window* window);
TTF_Font* mplayer_openfont(const char* file, int size);
void mplayer_createsearch_bar(mplayer_t* mplayer);
void mplayer_createsongs_box(mplayer_t* mplayer);
void mplayer_createmusicbar(mplayer_t* mplayer);
void mplayer_createapp(mplayer_t* mplayer);
void mplayer_getmusic_locations(mplayer_t* mplayer);
void mplayer_getmusic_filepaths(mplayer_t* mplayer);
void mplayer_getmusicpath_info(mplayer_t* mplayer);
void* mplayer_getmusic_namefrompath(Mix_Music* music, void* path);
void mplayer_copymusicinfo_fromsearchindex(mplayer_t* mplayer, size_t index, music_t* music_info);
void mplayer_loadmusics(mplayer_t* mplayer);
void mplayer_browsefolder(mplayer_t* mplayer);
void mplayer_addmusic_location(mplayer_t* mplayer, void* locationv);
void mplayer_setmusic_removestatus(mplayer_t* mplayer, size_t index);
void mplayer_removemusics_pendingremoval(mplayer_t* mplayer);
void mplayer_delmusic_locationindex(mplayer_t* mplayer, size_t loc_index);
mtime_t mplayer_music_gettime(double seconds);
bool mplayer_musiclocation_exists(mplayer_t* mplayer, void* locationv);
void mplayer_freemusic(music_t* music_ref);
void mplayer_freemusic_searchresult(mplayer_t* mplayer);
void mplayer_freemusic_list(mplayer_t* mplayer);
void mplayer_freemusicpath_info(mplayer_t* mplayer);
void mplayer_run(mplayer_t* mplayer);
void mplayer_defaultmenu(mplayer_t* mplayer);
void mplayer_settingmenu(mplayer_t* mplayer);
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
#ifdef _WIN32
wchar_t* mplayer_stringtowide(const char* string);
char* mplayer_widetostring(wchar_t* wstring);
Uint16* mplayer_widetouint16(wchar_t* wstring);
#else
Uint16* mplayer_stringtouint16(char* string);
#endif
char* mplayer_stringtolower(char** string, size_t len);
wchar_t* mplayer_widetolower(wchar_t** wstring, size_t wlen);
void mplayer_drawcheckbox(mplayer_t* mplayer, mcheckbox_t* checkbox_info);
void mplayer_drawmusic_checkbox(mplayer_t* mplayer, SDL_Color box_color,
    SDL_Color fill_color, bool fill, SDL_Color tick_color, bool check);
void mplayer_renderscroll_bar(mplayer_t* mplayer, mplayer_scrollbar_t* scrollbar, size_t max_textures);
void mplayer_rendersongs(mplayer_t* mplayer);
void mplayer_renderprogress_bar(mplayer_t* mplayer, SDL_Color bar_color, SDL_Color line_color,
    double curr_durationsecs, double full_durationsecs);
int mplayer_get_textsize(TTF_Font* font, int font_size, text_info_t* utext_info);
SDL_Texture* mplayer_rendertext(mplayer_t* mplayer, TTF_Font* font, text_info_t* text_info);
SDL_Texture* mplayer_renderunicode_text(mplayer_t* mplayer, TTF_Font* font, text_info_t* utext_info);
SDL_Texture* mplayer_rendertab(mplayer_t* mplayer, tabinfo_t* tab_info);
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
extern text_info_t text_info[], setting_textinfo[];
extern tabinfo_t tab_info[];
extern ibtn_t music_btns[], setting_btns[], setting_iconbtn, music_listplaybtn, music_addfolderbtn, music_addplaylistbtn,
music_removebtn;
#endif