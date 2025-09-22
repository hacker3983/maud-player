#ifndef _MAUD_SELECTIONMENU_H
#define _MAUD_SELECTIONMENU_H
#include "maud_player.h"
#include "maud_checkbox.h"
#include "maud_inputboxes.h"
#include "maud_textmanager.h"
#include "maud_queue.h"
#include "maud_playlistmanager.h"

enum addtoplaylist_modalcanvases {
    ADDTOPLAYLIST_MODALCANVAS,
    ADDTOPLAYLIST_MODALCANVAS_CREATEBTN,
    ADDTOPLAYLIST_MODALCANVAS_CANCELBTN
};

void maud_selectionmenu_create(maud_t* maud);
void maud_selectionmenu_display(maud_t* maud);
void maud_selectionmenu_clearmusic_selection(maud_t* maud);
void maud_selectionmenu_display_addbtn(maud_t* maud, SDL_Rect playnext_btnbackground);
void maud_selectionmenu_display_addtoplaylist_modal(maud_t* maud);
void maud_selectionmenu_handle_addtoplaylist_modalevents(maud_t* maud);
void maud_selectionmenu_handle_addtobtn(maud_t* maud);
void maud_selectionmenu_display_playbtn(maud_t* maud, text_info_t songs_selectioninfo,
    SDL_Rect* playbtn_backgroundref);
void maud_selectionmenu_handle_playbtn(maud_t* maud, SDL_Rect playbtn_background);
void maud_selectionmenu_display_playnextbtn(maud_t* maud, SDL_Rect playbtn_background,
    SDL_Rect* playnext_backgroundref);
void maud_selectionmenu_handle_playnextbtn(maud_t* maud, SDL_Rect playnext_btncanvas);
void maud_selectionmenu_display_removebtn(maud_t* maud, SDL_Rect* removebtn_ref);
void maud_selectionmenu_handle_removebtn(maud_t* maud, SDL_Rect removebtn);
void maud_selectionmenu_display_movebtn(maud_t* maud, int type, SDL_Rect prevbtn,
    SDL_Rect* move_btnref);
void maud_selectionmenu_handle_moveup(maud_t* maud, SDL_Rect movebtn);
void maud_selectionmenu_handle_movedown(maud_t* maud, SDL_Rect movebtn);
void maud_selectionmenu_display_songselectioninfo(maud_t* maud, text_info_t* songs_selectioninfo);
void maud_selectionmenu_display_checkallbtn(maud_t* maud);
void maud_selectionmenu_handle_checkallbtn_toggleoption(maud_t* maud, text_info_t songs_selectioninfo);
bool maud_selectionmenu_toggleitem_checkboxmusic_queue(maud_t* maud, maud_queue_t* queue,
    size_t item_index);
bool maud_selectionmenu_togglesong_checkbox(maud_t* maud, music_t* music_list, size_t music_checkbox_index);

bool maud_selectionmenu_togglecheckbox(maud_t* maud, maud_queue_t* queue, size_t queue_itemindex,
    music_t* music_list, size_t music_checkbox_index);
#endif