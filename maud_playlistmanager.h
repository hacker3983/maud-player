#ifndef _MUSIC_PLAYLISTMANAGER_H
#define _MUSIC_PLAYLISTMANAGER_H
#include "music_player.h"
#include "music_selectionmenu.h"
#include "stringlist.h"
#include "music_queue.h"
#include "music_filemanager.h"
#include "music_tooltips.h"

enum playlist_layoutmode {
    PLAYLIST_LISTVIEW,
    PLAYLIST_GRIDVIEW,
    PLAYLIST_LAYOUTCOUNT
};

void mplayer_playlistmanager_initialize_playlistinput(mplayer_t* mplayer);
void mplayer_playlistmanager_initialize_renameinput(mplayer_t* mplayer, const char* input_data);
SDL_Texture* mplayer_playlistmanager_getlayouticon(mplayer_t* mplayer, int type);
SDL_Texture* mplayer_playlistmanager_getcurrentlayouticon(mplayer_t* mplayer);
static char* mplayer_playlistmanager_getlayoutname(int type);
static char* mplayer_playlistmanager_getcurrentlayoutname(mplayer_t* mplayer);
void mplayer_playlistmanager_write_data_tofile(mplayer_t* mplayer);
void mplayer_playlistmanager_write_playlist_tofile(mplayer_t* mplayer, FILE* f, mplayer_playlist_t playlist);
void mplayer_playlistmanager_write_escapedstring_tofile(FILE* f, const char* string);
char* mplayer_playlistmanager_getmusicnamefrom_index(mplayer_t* mplayer, size_t music_listindex,
    size_t music_id);
void mplayer_playlistmanager_getmusicindex_byname(mplayer_t* mplayer, char* music_name,
    size_t* music_listindex, size_t* music_id);
void mplayer_playlistmanager_createplaylist_fromparsed_data(mplayer_t* mplayer, stringlist_t* string_list);
char* mplayer_playlistmanager_parsemusic_name(char* music_name, size_t* list_index);
char* mplayer_playlistmanager_getstring(FILE* playlist_datafile);
bool mplayer_playlistmanager_read_datafile(mplayer_t* mplayer);
bool mplayer_playlistmanager_createplaylist(mplayer_t* mplayer, const char* playlist_name);
void mplayer_playlistmanager_addmusic_toplaylist(mplayer_t* mplayer, const char* playlist_name,
    size_t music_listindex, size_t music_id);

void mplayer_playlistmanager_addmusicselection_toplaylist(mplayer_t* mplayer, const char* playlist_name);
bool mplayer_playlistmanager_removeplaylist(mplayer_t* mplayer, const char* playlist_name);
void mplayer_playlistmanager_renderplaylist_listlayout(mplayer_t* mplayer, size_t playlist_index,
    SDL_Rect* playlist_background);
void mplayer_playlistmanager_renderplaylist_gridlayout(mplayer_t* mplayer, int start_x,
    size_t playlist_index, SDL_Rect* playlist_background, bool* show_tooltip);

void mplayer_playlistmanager_renderplaylist_card(mplayer_t* mplayer, SDL_Rect* playlist_card);

// Find the maximum height from a giving list of rectangles
int mplayer_playlistmanager_findmaxheight(SDL_Rect* rects, size_t rect_count);

// Functions used for initializing button dimensions and positions on screen
void mplayer_playlistmanager_initialize_newplaylistbutton(mplayer_t* mplayer);
void mplayer_playlistmanager_initialize_playlistlayout_toggleswitch(mplayer_t* mplayer);
void mplayer_playlistmanager_initialize_buttonbar(mplayer_t* mplayer);
void mplayer_playlistmanager_setplaylist_cardposition(mplayer_t* mplayer, SDL_Rect playlistmenu_canvas,
    SDL_Rect* playlist_card);


// Display a toggle switch for changing the layout of how playlists are displayed on the screen
void mplayer_playlistmanager_display_playlistlayout_toggleswitch(mplayer_t* mplayer);
// Display the new playlist button
void mplayer_playlistmanager_display_newplaylistbutton(mplayer_t* mplayer);
// Diplay input for renaming playlists
void mplayer_playlistmanager_displayrename_input(mplayer_t* mplayer);
// Display the validation message on screen whenever playlist input field is empty
void mplayer_playlistmanager_display_playlistname_validation(mplayer_t* mplayer);
// Display all the button elements such as the new playlist button in the playlist tab
void mplayer_playlistmanager_display_buttonbar(mplayer_t* mplayer);
// Display playlists on screen
void mplayer_playlistmanager_displayplaylists(mplayer_t* mplayer);

// Display the playlist menu after we have clicked on a playlist
void mplayer_playlistmanager_display_playlistmenu(mplayer_t* mplayer, SDL_Rect* playlistmenu_canvasref);

// Display the contents of a playlist
void mplayer_playlistmanager_display_playlistcontent(mplayer_t* mplayer);

// Display all the elements in the playlist tab
void mplayer_playlistmanager_display(mplayer_t* mplayer);

void mplayer_playlistmanager_display_layoutdrop_downmenu(mplayer_t* mplayer);

void mplayer_playlistmanager_handle_playlistlayout_toggleswitch(mplayer_t* mplayer);

// Handles events for playlists that are rendered on the screen
void mplayer_playlistmanager_handleplaylist_event(mplayer_t* mplayer, size_t playlist_index, SDL_Rect playlist_canvas);

// Destroys the memory used by a playlist
void mplayer_playlistmanager_destroyplaylist(mplayer_playlist_t* playlist);

// Destroys / deallocate all the memory used by the playlist manager
void mplayer_playlistmanager_destroy(mplayer_t* mplayer);
#endif