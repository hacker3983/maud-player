#ifndef _MUSIC_SCROLLCONTAINER_H
#define _MUSIC_SCROLLCONTAINER_H
#include "music_player.h"

// Initialize the scroll container
void mplayer_scrollcontainer_init(music_scrollcontainer_t* container);

// set the scroll container content count
void mplayer_scrollcontainer_setcontent_count(music_scrollcontainer_t* container, size_t content_count);

// set the scroll container scroll area / viewport
void mplayer_scrollcontainer_setscroll_area(music_scrollcontainer_t* container, SDL_Rect scroll_area);

// set the scroll container scroll y position
void mplayer_scrollcontainer_setscroll_y(music_scrollcontainer_t* container, int scroll_y);

// set the scroll container scroll y from the scroll area / viewport
void mplayer_scrollcontainer_setscroll_yfromscroll_area(music_scrollcontainer_t* container);

// set the scroll container scroll speed
void mplayer_scrollcontainer_setscroll_speed(music_scrollcontainer_t* container, int scroll_speed);

// set the scroll container properties
void mplayer_scrollcontainer_setprops(music_scrollcontainer_t* container, SDL_Rect scroll_area,
    int scroll_speed, size_t content_count);

// Adds a new scroll item to the container
void mplayer_scrollcontainer_additem(music_scrollcontainer_t* container, SDL_Rect scroll_item);

// Set the next item canvas
void mplayer_scrollcontainer_setnext_itemcanvas(music_scrollcontainer_t* container, SDL_Rect canvas);

// Reset the item canvas index
void mplayer_scrollcontainer_resetitem_index(music_scrollcontainer_t* container);

// Detects whether we hover over the scroll area where we should detect scroll
bool mplayer_scrollcontainer_hover_scrollarea(mplayer_t* mplayer, music_scrollcontainer_t container);

// Perform scroll specifically over the screen area but let them disappear whenever item is scrolled below the scroll area
// y position
void mplayer_scrollcontainer_performscroll_overscrollarea(mplayer_t* mplayer, music_scrollcontainer_t* container);

// Perform scrolling operation on a scroll container when we detect scroll within the scroll area
void mplayer_scrollcontainer_performscroll(mplayer_t* mplayer, music_scrollcontainer_t* container, int scroll_y);

// Destroy the memory allocated for the scroll container
void mplayer_scrollcontainer_destroy(music_scrollcontainer_t* container);
#endif