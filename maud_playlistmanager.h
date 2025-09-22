#ifndef _MUSIC_PLAYLISTMANAGER_H
#define _MUSIC_PLAYLISTMANAGER_H
#include "maud_player.h"
#include "maud_selectionmenu.h"
#include "stringlist.h"
#include "maud_queue.h"
#include "maud_filemanager.h"
#include "maud_tooltips.h"

bool maud_playlistmanager_playlistexists(maud_t* maud, const char* playlist_name);
void maud_playlistmanager_newplaylist_input_init(maud_t* maud,
    maud_newplaylist_input_t* newplaylist_input);
void maud_playlistmanager_renameplaylist_input_init(maud_t* maud,
    maud_renameplaylist_input_t* rename_input, const char* input_data);
SDL_Texture* maud_playlistmanager_layout_geticon(maud_t* maud, int type);
SDL_Texture* maud_playlistmanager_layout_getcurrenticon(maud_t* maud);
char* maud_playlistmanager_layout_getname(int type);
char* maud_playlistmanager_layout_getcurrentname(maud_t* maud);
char* maud_playlistmanager_layout_getname_view(int type);
char* maud_playlistmanager_layout_getcurrentname_view(maud_t* maud);
void maud_playlistmanager_layoutdropdown_menu_init(maud_t* maud,
    maud_playlistlayout_switch_t* layout_switch);
void maud_playlistmanager_layoutdropdown_menu_init_names(maud_t* maud,
    maud_playlistlayout_switchdropdown_t* dropdown);
void maud_playlistmanager_layoutdropdown_menu_display(maud_t* maud);
void maud_playlistmanager_write_data_tofile(maud_t* maud);
void maud_playlistmanager_write_playlist_tofile(maud_t* maud, FILE* f, maud_playlist_t playlist);
void maud_playlistmanager_write_escapedstring_tofile(FILE* f, const char* string);
char* maud_playlistmanager_getmusicnamefrom_index(maud_t* maud, size_t music_listindex,
    size_t music_id);
void maud_playlistmanager_getmusicindex_byname(maud_t* maud, char* music_name,
    size_t* music_listindex, size_t* music_id);
void maud_playlistmanager_createplaylist_fromparsed_data(maud_t* maud, stringlist_t* string_list);
char* maud_playlistmanager_parsemusic_name(char* music_name, size_t* list_index);
char* maud_playlistmanager_getstring(FILE* playlist_datafile);
bool maud_playlistmanager_read_datafile(maud_t* maud);
bool maud_playlistmanager_createplaylist(maud_t* maud, const char* playlist_name);
void maud_playlistmanager_addmusic_toplaylist(maud_t* maud, const char* playlist_name,
    size_t music_listindex, size_t music_id);

void maud_playlistmanager_addmusicselection_toplaylist(maud_t* maud, const char* playlist_name);
bool maud_playlistmanager_removeplaylist(maud_t* maud, const char* playlist_name);
void maud_playlistmanager_renderplaylist_listlayout(maud_t* maud, size_t playlist_index,
    SDL_Rect* playlist_background);
void maud_playlistmanager_listrenderer_init(maud_t* maud);
void maud_playlistmanager_listrenderer_init_playlistprops(maud_t* maud,
    maud_playlistprops_t* playlist_props);
void maud_playlistmanager_listrenderer_init_list(maud_t* maud,
    maud_playlist_t* playlists, size_t index, int start_x, int start_y);
void maud_playlistmanager_listrenderer_init_lists(maud_t* maud, maud_playlist_t* playlists,
    size_t playlist_count);
void maud_playlistmanager_listrenderer_fillplaylistgap(maud_t* maud,
    maud_playlistprops_t* playlist_props);
void maud_playlistmanager_listrenderer_renderplaylist(maud_t* maud, maud_playlist_t* playlists, size_t index);
void maud_playlistmanager_listrenderer_handleplaylist_event(maud_t* maud,
    maud_playlist_t* playlists, size_t playlist_index);
void maud_playlistmanager_listrenderer_handleplaylist_scrollevent(maud_t* maud,
    maud_playlistprops_t* playlist_props);
void maud_playlistmanager_listrenderer_displayplaylists(maud_t* maud);
void maud_playlistmanager_gridrenderer_init(maud_t* maud);
void maud_playlistmanager_gridrenderer_init_playlistprops(maud_t* maud,
    maud_playlistprops_t* playlist_props);
void maud_playlistmanager_gridrenderer_init_grid(maud_t* maud,
    maud_playlist_t* playlists, size_t index, int start_x, int start_y);
void maud_playlistmanager_gridrenderer_init_grids(maud_t* maud, maud_playlist_t* playlists,
    size_t playlist_count);
void maud_playlistmanager_gridrenderer_fillplaylistgap(maud_t* maud,
    maud_playlistprops_t* playlist_props);
void maud_playlistmanager_gridrenderer_renderplaylist(maud_t* maud, maud_playlist_t* playlists, size_t index);
void maud_playlistmanager_gridrenderer_handleplaylist_event(maud_t* maud,
    maud_playlist_t* playlists, size_t playlist_index);
void maud_playlistmanager_gridrenderer_getprevrender_pos(maud_t* maud,
    maud_playlistprops_t* playlist_props);
void maud_playlistmanager_gridrenderer_getnextrender_pos(maud_t* maud,
    maud_playlistprops_t* playlist_props);
void maud_playlistmanager_gridrenderer_handleplaylist_scrollevent(maud_t* maud,
    maud_playlistprops_t* playlist_props);
void maud_playlistmanager_gridrenderer_displayplaylists(maud_t* maud);
void maud_playlistmanager_renderplaylist_card(maud_t* maud, SDL_Rect* playlist_card);

// Find the maximum height from a giving list of rectangles
int maud_playlistmanager_findmaxheight(SDL_Rect* rects, size_t rect_count);

// Functions used for initializing button dimensions and positions on screen
void maud_playlistmanager_buttonbar_initialize(maud_t* maud);
void maud_playlistmanager_buttonbar_initialize_newplaylistbutton(maud_t* maud,
    maud_buttonbar_t* button_bar);
void maud_playlistmanager_buttonbar_intialize_playlistlayout_text(maud_t* maud,
    maud_buttonbar_t* button_bar);
void maud_playlistmanager_buttonbar_initialize_playlistlayout_toggleswitch(maud_t* maud,
    maud_buttonbar_t* button_bar);
void maud_playlistmanager_setplaylist_cardposition(maud_t* maud, SDL_Rect playlistmenu_canvas,
    SDL_Rect* playlist_card);

// Display playlist layout text
void maud_playlistmanager_buttonbar_display_playlistlayout_text(maud_t* maud,
    maud_buttonbar_t* button_bar);
// Display a toggle switch for changing the layout of how playlists are displayed on the screen
void maud_playlistmanager_buttonbar_display_playlistlayout_toggleswitch(maud_t* maud,
    maud_buttonbar_t* button_bar);
// Display the new playlist button
void maud_playlistmanager_buttonbar_display_newplaylistbutton(maud_t* maud,
    maud_buttonbar_t* button_bar);
void maud_playlistmanager_buttonbar_handle_newplaylistbutton(maud_t* maud,
    maud_buttonbar_t* button_bar);
void maud_playlistmanager_newplaylist_input_handlebtns(maud_t* maud,
    maud_newplaylist_input_t* new_playlistinput);
void maud_playlistmanager_newplaylist_input_displayall(maud_t* maud);
void maud_playlistmanager_newplaylist_input_display(maud_t* maud,
    maud_newplaylist_input_t* new_playlistinput);
// Diplay input for renaming playlists
void maud_playlistmanager_renameplaylist_input_display(maud_t* maud,
    maud_renameplaylist_input_t* rename_input);
// Display the validation message on screen whenever playlist input field is empty
void maud_playlistmanager_newplaylist_input_display_validation(maud_t* maud,
    maud_newplaylist_input_t* new_playlistinput);
// Display all the button elements such as the new playlist button in the playlist tab
void maud_playlistmanager_buttonbar_display(maud_t* maud);
// Display playlists on screen
void maud_playlistmanager_displayplaylists(maud_t* maud);

// Display the playlist menu after we have clicked on a playlist
void maud_playlistmanager_display_playlistmenu(maud_t* maud, SDL_Rect* playlistmenu_canvasref);

// Display the contents of a playlist
void maud_playlistmanager_display_playlistcontent(maud_t* maud);

// Display all the elements in the playlist tab
void maud_playlistmanager_display(maud_t* maud);
void maud_playlistmanager_layoutdropdown_menu_display(maud_t* maud);
void maud_playlistmanager_buttonbar_handle_playlistlayout_toggleswitch(maud_t* maud,
    maud_buttonbar_t* button_bar);

// Handles events for playlists that are rendered on the screen
void maud_playlistmanager_handleplaylist_event(maud_t* maud, size_t playlist_index, SDL_Rect playlist_canvas);

// Destroys the memory used by a playlist
void maud_playlistmanager_destroyplaylist(maud_playlist_t* playlist);

// Destroys / deallocate all the memory used by the playlist manager
void maud_playlistmanager_destroy(maud_t* maud);
#endif