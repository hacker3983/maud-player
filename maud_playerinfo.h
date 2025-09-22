#ifndef _MAUD_PLAYERINFO_H
#define _MAUD_PLAYERINFO_H
#include "maud_player.h"
#define MAUD_RELEASE 0
#define MAUD_PROGRAMDATA "C:\\ProgramData\\Maud Player\\"

#if MAUD_RELEASE
#define MAUD_PATHINFO_FILE MAUD_PROGRAMDATA "MUSICPATHS.info"
#define MAUD_PLAYLISTSINFO_FILE MAUD_PROGRAMDATA "MUSICPLAYLISTS.info"
#define MAUD_TEMPLISTINFO_FILE MAUD_PROGRAMDATA "C:\\ProgramData\\Maud Player\\MUSICTEMPLIST.info"
#else
#define MAUD_PATHINFO_FILE "MUSICPATHS.info"
#define MAUD_PLAYLISTSINFO_FILE "MUSICPLAYLISTS.info"
#define MAUD_TEMPLISTINFO_FILE "MUSICTEMPLIST.info"
#endif

const char *WINDOW_TITLE = "Maud Player (Music Audio Player)", *SETTING_TITLE = "Maud Player Settings",
           *FONT_FILE = "SF-Pro-Display-Black.otf", *MUSIC_FONTFILE = "Symbola.ttf",
           *MUSIC_PATHINFO_FILE = MAUD_PATHINFO_FILE,
           *MUSIC_PLAYLISTSINFO_FILE = MAUD_PLAYLISTSINFO_FILE,
           *MUSIC_TEMPLISTINFO_FILE = MAUD_TEMPLISTINFO_FILE;
#ifdef _WIN32
const wchar_t *FILE_EXTENSIONS[] = {L"mp3", L"flac", L"ogg", L"opus", L"wav", L"m4a", NULL};
#else
const char *FILE_EXTENSIONS[] = {"mp3", "flac", "ogg", "opus", "wav", "m4a", NULL};
#endif

const SDL_Color window_color = {0, 0, 0, 0} /*{0x2c, 0x36, 0x5e, 0xff}/*{0x3C, 0x16, 0x42, 0xff}*/,
                text_color = {0xFF, 0xFF, 0x00, 0xFF} /*0xC4, 0x90, 0xD1, 0xff}/*{0xff, 0x8a, 0x5b, 0xff}{0xAF, 0xFC, 0x41, 0xff}*/,
                text2_color = {0xd8, 0x1e, 0x5b, 0xff} /*{0x1D, 0xD3, 0xB0, 0xff}*/,
                underline_color = {0x00, 0xff, 0x00, 0xff}, setting_underlinecolor = {0xA5, 0x2A, 0x2A, 0xFF},
                blue =  {0x00, 0x00, 0xff, 0xff},
                white = {0xff, 0xff, 0xff, 0xff},
                black = {0x00, 0x00, 0x00, 0x00},
                green = {0x00, 0xFF, 0x00, 0xFF},
                dark_purple = {0x16, 0x00, 0x1E, 0xFF},
                music_searchbar_color = {0xFF, 0xFF, 0xFF, 0xFF},
                music_statusbar_color = {0x7A, 0x65, 0x63, 0xFF},
                songs_boxcolor = green, back_btnbg_color = {0x46, 0xB1, 0xC9, 0xFF},
    /* Reference: https://coolors.co/023c40-c3979f-0ad3ff-78ffd6-e1faf9, https://coolors.co/010001-2b0504-874000-bc5f04-f4442e */
    setting_wincolor = {0x00, 0x00, 0x00, 0xFF}/*{0x25, 0x16, 0x05, 0xFF}{0x21, 0x01, 0x24, 0xFF} {0x0B, 0x31, 0x42, 0xFF}{0x54, 0x08, 0x04, 0xFF}{0x1B, 0x06, 0x5E, 0xFF}black{0x20, 0x81, 0xC3, 0xFF}{0x02, 0x3C, 0x40, 0xFF}*/,
                setting_textcolor = {0xC3, 0x97, 0x9F, 0xFF};
int WIDTH = 900, HEIGHT = 600, TAB_INIT = 0, FONT_SIZE = 20, MUSIC_FONTSIZE = 14, TAB_SPACING = 20,
    SBOXDISTANCE_X = 50, SETTING_LINESPACING = 10, UNDERLINE_THICKNESS = 10, checkbox_init = 0;

text_info_t text_info[] = {
    {36, "Music", NULL, text_color, {10}},
    {18, "Type here to search for music", NULL, white, {0, 0}}
},
setting_textinfo[] = {
    {30, "Settings", NULL, {0xFF, 0xFF, 0xFF, 0xFF}/*{0x01, 0x00, 0x01, 0xFF}{0xE3, 0xE3, 0x6A, 0xFF}{0xEA, 0xF2, 0x7C, 0xFF}{0xE5, 0x58, 0x12, 0xFF}{0x95, 0xC6, 0x23, 0xFF}{0xF9, 0x62, 0x7D, 0xFF}{0x0C, 0xF5, 0x74, 0xFF}*/,
        {10, 0}},
    {30, "About", NULL, {0xFF, 0xFF, 0xFF, 0xFF}, {10, 0}},
    {28, "Add folder", NULL, {0xF0, 0xE1, 0x79, 0xFF}, {10, 0}}
};

tabinfo_t tab_info[] = {
    {28, "Songs", text2_color, underline_color, {0, 9}, SONGS_TAB, true, false},
    {28, "Play Queue", text2_color, {0}, {0, 9}, QUEUES_TAB, false, false},
    {28, "Playlists", text2_color, {0}, {0, 9}, PLAYLISTS_TAB, false, false}
};
int active_tab = SONGS_TAB, prev_tab = -1;

ibtn_t music_btns[] = {
    {"images/play.png", "Play", MAUD_PLAYBTN, {0, 0, 35, 35}, false, false, 0},
    {"images/shuffle-arrows (1).png", "Shuffle", MAUD_SHUFFLEBTN, {0, 0, 35, 35}, false, false, 0},
    {"images/skip-track.png", "Skip / Next", MAUD_SKIPBTN, {0, 0, 35, 35}, false, false, 0},
    {"images/previous.png", "Previous", MAUD_PREVBTN, {0, 0, 35, 35}, false, false, 0},
    {"images/pause-button.png", "Pause", MAUD_PAUSEBTN, {0, 0, 35, 35}, false, false, 0},
    {
        "images/repeat-all (1).png",
        "Repeat all",
        MAUD_REPEATALLBTN,
        {0, 0, 35, 35},
        false,
        false,
    },
    {"images/repeat-one (1).png", "Repeat one", MAUD_REPEATONEBTN, {0, 0, 35, 35}, false, false, 0},
    {"images/repeat-off (1).png", "Repeat Off", MAUD_REPEATOFFBTN, {0, 0, 35, 35}, false, false, 0}
},
music_listplaybtn = {"images/play-button (1).png", "Play", MAUD_LISTPLAYBTN, {0, 0, 30, 50}, false, false, 0},
music_addfolderbtn = {"images/add-folder.png", "Add a music location (Directory or Folder)", MAUD_ADDFOLDERBTN, {0, 5, 45, 45}, false, false, 0},
music_addplaylistbtn = {"images/add-to-playlist.png", "Add Playlist", MAUD_ADDPLAYLISTBTN, {0, 0, 30, 40}, false, false, 0},
music_addtobtn = {"images/add.png", "Add to", MAUD_ADDTOBTN, {0, 0, 30, 40}, false, false, 0},
music_addtocirclebtn = {"images/add 2.png", "Add to", MAUD_ADDTOCIRCLEBTN, {0, 0, 30, 40}, false, false, 0},
music_playqueuebtn = {"images/play-queue.png", "Play queue", MAUD_PLAYQUEUEBTN, {0, 0, 30, 40}, false, false, 0},
music_playlistbtn = {"images/playlist.png", "Playlist", MAUD_PLAYLISTBTN, {0, 0, 30, 30}, false, false, 0},
music_removebtn = {"images/removelocation.png", "Remove", MAUD_REMOVEBTN, {0, 0, 24, 24}, false, false, 0},
setting_btns[] = {
    {"images/back-button.png", "Home", BACK_BUTTON, {5, 0, 45, 45}, false, false, 0},
    {"images/add-folder.png", "Add a music location (Directory or Folder)", MAUD_ADDFOLDERBTN, {0, 5, 45, 45}, false, false, 0}
},
setting_iconbtn = {"images/settings (5).png", "Settings", SETTING_BUTTON, {0, 9, 45, 45}, false, false, 0};
SDL_Rect img_canvas = {20, 0, 50, 50}, img_bgcanvas = {10, 0, 80, 70},
         img_border = {10, 0, 80, 70};
SDL_Rect scrollbar = {0, 0, 20, 50}, music_status = {0, 0, 0, 0},
         songs_box = {0, 0, 0, 0};
SDL_Rect checkbox_size = {0, 0, 30, 50},
         music_selectionmenu_checkbox_size = {0, 0, 30, 50};

const size_t text_info_size = sizeof(text_info) / sizeof(text_info_t), tab_info_size = sizeof(tab_info) / sizeof(tabinfo_t),
             setting_textinfo_size = sizeof(setting_textinfo) / sizeof(text_info_t), MUSICBTN_COUNT = sizeof(music_btns) / sizeof(ibtn_t),
             MTOTALBTN_COUNT = (sizeof(music_btns) / sizeof(ibtn_t)) + 2, SETTINGSBTN_COUNT = sizeof(setting_btns) / sizeof(ibtn_t);
#endif