#ifndef _MUSIC_PLAYERINFO_EXTERN_H
#define _MUSIC_PLAYERINFO_EXTERN_H
extern const char* WINDOW_TITLE, *SETTING_TITLE, *FONT_FILE, *MUSIC_FONTFILE, *MUSIC_PATHINFO_FILE;
extern int WIDTH, HEIGHT, FONT_SIZE, MUSIC_FONTSIZE, TAB_INIT, active_tab, prev_tab, TAB_SPACING, SBOXDISTANCE_X,  SETTING_LINESPACING,
UNDERLINE_THICKNESS, checkbox_init;
extern SDL_Rect scrollbar, music_status, songs_box, checkbox_size, music_selectionmenu_checkbox_size;
extern const size_t text_info_size, tab_info_size, setting_textinfo_size, MUSICBTN_COUNT, MTOTALBTN_COUNT, SETTINGSBTN_COUNT;
extern const SDL_Color window_color, text_color, text2_color, underline_color, blue, white, black, dark_purple, music_statusbar_color,
music_searchbar_color, songs_boxcolor, setting_wincolor, setting_textcolor, back_btnbg_color, setting_underlinecolor;

#ifdef _WIN32
extern const wchar_t* FILE_EXTENSIONS[];
#else
extern const char* FILE_EXTENSIONS[];
#endif

extern text_info_t text_info[], setting_textinfo[];
extern tabinfo_t tab_info[];
extern ibtn_t music_btns[], setting_btns[], setting_iconbtn, music_listplaybtn, music_addfolderbtn, music_addplaylistbtn,
music_addtobtn, music_removebtn;
#endif