#ifndef _MPLAYER_H
#define _MPLAYER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>
#include <stdbool.h>
#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>
#include <commdlg.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

extern const char* WINDOW_TITLE, *SETTING_TITLE, *FONT_FILE, *MUSIC_PATHINFO_FILE, *FILE_EXTENSIONS[];
extern int WIDTH, HEIGHT, FONT_SIZE, TAB_INIT, active_tab, prev_tab, TAB_SPACING, SBOXDISTANCE_X,  SETTING_LINESPACING,
UNDERLINE_THICKNESS, checkbox_init;
extern SDL_Rect scrollbar, music_status, songs_box, checkbox_size;
extern const size_t text_info_size, tab_info_size, setting_textinfo_size, MUSICBTN_COUNT, MTOTALBTN_COUNT, SETTINGSBTN_COUNT;
extern const SDL_Color window_color, text_color, text2_color, underline_color, white, black, music_statusbar_color,
songs_boxcolor, setting_wincolor, setting_textcolor, back_btnbg_color, setting_underlinecolor;

#define color_toparam(color) \
    color.r, color.g, color.b, color.a
#define MENU_COUNT 2
#define TEXTURE_TYPECOUNT 3
#define CURSOR_COUNT 2

// Music time
typedef struct music_time {
    int hrs, mins, secs;
} mtime_t;

// structure representing music
typedef struct music {
    char* music_name, *music_path;
    Mix_Music* music;
    mtime_t music_position;
    mtime_t music_duration;
    SDL_Rect canvas, checkbox_size;
    int hover, clicked, checkbox_ticked, fill;
} music_t;

typedef struct music_location {
    char* path;
} musloc_t;

typedef struct music_info {
    musloc_t* locations, *files;
    size_t location_count, file_count;
} musinfo_t;

typedef struct text_info {
    int font_size;
    char* text;
    SDL_Color text_color;
    SDL_Rect text_canvas;
} text_info_t;

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
    int hover;
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
    MUSIC_SKIPBTN,
    MUSIC_PREVBTN,
    MUSIC_PAUSEBTN,
};

enum setting_buttons {
    SETTING_BUTTON,
    BACK_BUTTON
};

enum TAB_ID {
        SONGS_TAB,
        ALBUMS_TAB,
        QUEUES_TAB
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

typedef struct mplayer_menu {
    text_info_t* texts;
    SDL_Texture** textures[TEXTURE_TYPECOUNT];
    SDL_Rect* texture_canvases[TEXTURE_TYPECOUNT];
    SDL_Rect* canvases;
    size_t texture_sizes[TEXTURE_TYPECOUNT], text_count, canvas_count;
} mplayer_menu_t;

enum cursor_types {
    MPLAYER_CURSOR_POINTER,
    MPLAYER_CURSOR_DEFAULT
};

// Structure representing music player's graphical utilities, music information
typedef struct mplayer {
    // Music Player Graphical Utilities
    SDL_Window* window; // window
    SDL_Renderer* renderer; // renderer
    TTF_Font* font; // font
    SDL_Event e; // event
    int menu_opt, quit;
    mplayer_menu_t menus[MENU_COUNT], *menu;
    SDL_Cursor* cursors[2];

    // Music Informations such music name, path, duration, etc
    musinfo_t musinfo;
    music_t* music_list;
    int music_hover;
    size_t music_id, prevmusic_id, music_count;
    int mouse_x, mouse_y, tick_count;
} mplayer_t;

void mplayer_init();

SDL_Window* mplayer_createwindow(const char* title, int width, int height);
SDL_Renderer* mplayer_createrenderer(SDL_Window* window);
TTF_Font* mplayer_openfont(const char* file, int size);
void mplayer_createsongs_box(mplayer_t* mplayer);
void mplayer_createmusicbar(mplayer_t* mplayer);
void mplayer_createapp(mplayer_t* mplayer);
void mplayer_getmusic_locations(mplayer_t* mplayer);
void mplayer_getmusic_filepaths(mplayer_t* mplayer);
void mplayer_getmusicpath_info(mplayer_t* mplayer);
char* mplayer_getmusic_namefrompath(const char* path);
void mplayer_loadmusics(mplayer_t* mplayer);
void mplayer_addmusic_location(mplayer_t* mplayer, char* location);
mtime_t mplayer_music_gettime(double seconds);
bool mplayer_musiclocation_exists(mplayer_t* mplayer, char* location);
void mplayer_freemusic_info(mplayer_t* mplayer);
void mplayer_run(mplayer_t* mplayer);
void mplayer_defaultmenu(mplayer_t* mplayer);
void mplayer_settingmenu(mplayer_t* mplayer);
void mplayer_setcursor(mplayer_t* mplayer, int cursor_type);
void mplayer_set_window_color(SDL_Renderer* renderer, SDL_Color bg_color);
void mplayer_set_window_title(mplayer_t* mplayer, const char* title);
bool mplayer_tab_hover(mplayer_t* mplayer, tabinfo_t tab);
bool mplayer_ibutton_hover(mplayer_t* mplayer, ibtn_t button);
bool mplayer_tbutton_hover(mplayer_t* mplayer, tbtn_t button);
bool mplayer_tabs_hover(mplayer_t* mplayer, tabinfo_t* tabs, int* tab_id, size_t tab_count);
bool mplayer_ibuttons_hover(mplayer_t* mplayer, ibtn_t* buttons, int* btn_id, size_t button_count);
bool mplayer_tbuttons_hover(mplayer_t* mplayer, tbtn_t* buttons, int* btn_id, size_t button_count);
bool mplayer_music_hover(mplayer_t* mplayer);
bool mplayer_checkbox_clicked(mplayer_t* mplayer);
void mplayer_drawcheckbox(mplayer_t* mplayer, mcheckbox_t* checkbox_info);
void mplayer_drawmusic_checkbox(mplayer_t* mplayer, SDL_Color box_color,
    SDL_Color fill_color, bool fill, SDL_Color tick_color, bool check);
SDL_Texture* mplayer_rendertext(mplayer_t* mplayer, text_info_t* text_info);
SDL_Texture* mplayer_rendertab(mplayer_t* mplayer, tabinfo_t* tab_info);
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
extern ibtn_t music_btns[], setting_btns[], setting_iconbtn;
#endif