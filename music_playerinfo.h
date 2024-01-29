#ifndef _MUSIC_PLAYERINFO
#define _MUSIC_PLAYERINFO
#include "music_player.h"
const char* WINDOW_TITLE = "Maud Player (Music Audio Player)", *SETTING_TITLE = "Maud Player Settings",
        *FONT_FILE = "SF-Pro-Display-Black.otf", *MUSIC_PATHINFO_FILE = "MUSICPATHS.info",
        *FILE_EXTENSIONS[] = {"mp3", "flac", "ogg", "opus", "wav", "m4a", NULL};

const SDL_Color window_color = {0, 0, 0, 0}/*{0x2c, 0x36, 0x5e, 0xff}/*{0x3C, 0x16, 0x42, 0xff}*/,
text_color = {0xFF, 0xFF, 0x00, 0xFF}/*0xC4, 0x90, 0xD1, 0xff}/*{0xff, 0x8a, 0x5b, 0xff}{0xAF, 0xFC, 0x41, 0xff}*/,
text2_color = {0xd8, 0x1e, 0x5b, 0xff}/*{0x1D, 0xD3, 0xB0, 0xff}*/,
underline_color = {0x00, 0xff, 0x00, 0xff}, setting_underlinecolor = {0xA5, 0x2A, 0x2A, 0xFF},
white = {0xff, 0xff, 0xff, 0xff},
black = {0x00, 0x00, 0x00, 0x00},
green = {0x00, 0xFF, 0x00, 0xFF},
music_statusbar_color = {0x7A, 0x65, 0x63, 0xFF},
songs_boxcolor = green, back_btnbg_color = {0x46, 0xB1, 0xC9, 0xFF},
/* Reference: https://coolors.co/023c40-c3979f-0ad3ff-78ffd6-e1faf9 */
setting_wincolor = black/*{0x02, 0x3C, 0x40, 0xFF}*/,
setting_textcolor = {0xC3, 0x97, 0x9F, 0xFF};
int WIDTH = 900, HEIGHT = 600, TAB_INIT = 0, FONT_SIZE = 20, TAB_SPACING = 20,
SBOXDISTANCE_X = 50, SETTING_LINESPACING = 10, UNDERLINE_THICKNESS = 10, checkbox_init = 0;

text_info_t text_info[] = {
        {36, "Music", text_color, {10}},
        {36, "You're in the Songs tab.", text_color, {0}},
        {36, "You're in the Albums tab.", text_color, {0}}
},
setting_textinfo[] = {
        {45, "Go Back To Home", text2_color, {10, 0}},
};

tabinfo_t tab_info[] = {
        {28, "Songs", text2_color, underline_color, {0, 9}, SONGS_TAB, true, false},
        {28, "Albums", text2_color, {0}, {0, 9}, ALBUMS_TAB, false, false},
        {28, "Queues", text2_color, {0}, {0, 9}, QUEUES_TAB, false, false}
};
int active_tab = SONGS_TAB, prev_tab = -1;

ibtn_t music_btns[] = {
        {"images/play.png", MUSIC_PLAYBTN, {0, 0, 35, 35}, false, false},
        {"images/shuffle-arrows (1).png", MUSIC_SHUFFLEBTN, {0, 0, 35, 35}, false, false},
        {"images/skip-track.png", MUSIC_SKIPBTN, {0, 0, 35, 35}, false, false},
        {"images/previous.png", MUSIC_PREVBTN, {0, 0, 35, 35}, false, false},
        {"images/pause-button.png", MUSIC_PAUSEBTN, {0, 0, 35, 35}, false, false},
        {"images/repeat-all (1).png", MUSIC_REPEATALLBTN, {0, 0, 35, 35}, false, false},
        {"images/repeat-one (1).png", MUSIC_REPEATONEBTN, {0, 0, 35, 35}, false, false},
        {"images/repeat-off (1).png", MUSIC_REPEATOFFBTN, {0, 0, 35, 35}, false, false}
},
music_listplaybtn = {"images/play-button (1).png", MUSIC_LISTPLAYBTN, {0, 0, 30, 50}, false, false},
setting_btns[] = {
        {"images/back-button.png", BACK_BUTTON, {0, 5, 45, 45}, false}
},
setting_iconbtn = {"images/settings (5).png", SETTING_BUTTON, {0, 9, 45, 45}, false, false};
SDL_Rect img_canvas = {20, 0, 50, 50}, img_bgcanvas = {10, 0, 80, 70},
        img_border = {10, 0, 80, 70};
SDL_Rect scrollbar = {0, 0, 10, 50}, music_status = {0, 0, 0, 150},
        songs_box = {0, 0, 0, 0};
SDL_Rect checkbox_size = {0, 0, 30, 50};

const size_t text_info_size = sizeof(text_info)/sizeof(text_info_t), tab_info_size = sizeof(tab_info)/sizeof(tabinfo_t),
setting_textinfo_size = sizeof(setting_textinfo)/sizeof(text_info_t), MUSICBTN_COUNT = sizeof(music_btns)/sizeof(ibtn_t),
MTOTALBTN_COUNT = (sizeof(music_btns)/sizeof(ibtn_t)) + 2, SETTINGSBTN_COUNT = sizeof(setting_btns) / sizeof(ibtn_t);
#endif